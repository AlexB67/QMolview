#include "molecule.h"
#include "mol_utils.h"
#include <fstream>
#include <limits>
#include <iostream>
#include <istream>
#include <sstream>
#include <qglobal.h>
#include <algorithm>
#include <tuple>

// std::filesystem bugged in min-gw so disable
// The test is redundant anyway as it is
// checked in the file dialog
#ifdef Q_OS_LINUX
#include <filesystem>
#endif

Mol::Molecule::Molecule(bool last_frame, bool sort, bool align)
: use_last_frame(last_frame),
  sort_axes(sort),
  align_axes(align)
{
    axes_idx = QVector<int>{0, 1, 2}; // i.e. no sorting
}

Mol::Molecule::Molecule()
: use_last_frame(true),
  sort_axes(true),
  align_axes(true)
{
    axes_idx = QVector<int>{0, 1, 2}; // i.e. no sorting
}

std::pair<bool, std::string> Mol::Molecule::get_geom(const std::string& filename, fileTypes filetype)
{

#ifdef Q_OS_LINUX
    if (!std::filesystem::exists(filename))
         return std::make_pair(false, "File not found.");
#endif

    file_type = filetype;

    if (filetype == Mol::fileTypes::FILE_TYPE_MOLDEN)
    {
        const auto retcode = get_molden_file(filename);
        if (false == retcode.first) return retcode;

        if (align_axes)
            MolUtils::align_axes(coords, normal_modes, zval);

        if (sort_axes)
            axes_idx = MolUtils::sort_axes(coords, normal_modes, natoms, num_frames, 0);

        largest_distance = MolUtils::get_max_extent(coords, natoms, 0);

        return retcode;
    }
    else if (filetype == Mol::fileTypes::FILE_TYPE_XYZ)
    {
        const auto retcode = get_xyz_file(filename);
        if (false == retcode.first) return retcode;

        int frame_idx = (use_last_frame) ? num_frames - 1 : 0;

        if (align_axes)
            MolUtils::align_axes(coords, zval, num_frames, frame_idx);

        if (sort_axes)
            axes_idx = MolUtils::sort_axes(coords, natoms, num_frames, frame_idx);

        largest_distance = MolUtils::get_max_extent(coords, natoms, frame_idx);

        return retcode;
    }
    else if (filetype == Mol::fileTypes::FILE_TYPE_CML)
    {
        return get_cml_file(filename);
    }

    return std::make_pair(false, "Invalid file format.");
}

std::pair<bool, std::string> Mol::Molecule::get_xyz_file(const std::string& filename)
{
    auto detect_type = [](const std::string s)
    {
        return !s.empty() && s.find_first_not_of("0123456789") != std::string::npos;
    };

    std::ifstream inputfile(filename, std::ifstream::in);

    formula_vector.clear();

    constexpr float conv = static_cast<float>(periodic_table::angstrom_to_bohr);

    for (int k = 0; ; ++k)
    {
        std::string line;
        std::getline(inputfile, line);
        std::istringstream data(line);
        data >> natoms; // read number of atoms
        std::getline(inputfile, line);
        std::string comment = line;

        num_frames = k;

        if (inputfile.eof())
        {
            formula_string = MolUtils::build_formula_string(formula_vector);
            inputfile.close();
            return std::make_pair(true, "success");
        }

        coords.resize(coords.size() + 1);
        coords[k] = QVector<QVector3D>(natoms);
        if (!k) zval.resize(natoms);

        std::string atom_zval_or_symbol;

        for(int i = 0; i < zval.size(); ++i) // read file line by line
        {
            std::getline(inputfile, line);
            if (line.empty()) continue;

            std::istringstream coord_data(line);

            coord_data >> atom_zval_or_symbol;

            if (k == 0)
            {
                if (detect_type(atom_zval_or_symbol))
                {
                    zval[i] = periodic_table::atomic_number(atom_zval_or_symbol);
                    formula_vector.emplace_back(atom_zval_or_symbol);
                }
                else
                {
                    zval[i] = std::stoul(atom_zval_or_symbol);
                    formula_vector.emplace_back(periodic_table::symbols[std::stoul(atom_zval_or_symbol)]);
                }

                molecular_mass += static_cast<float>(periodic_table::masses[zval[i]]);
            }

            coords[k][i] = QVector3D();
            std::string x, y, z;

            coord_data >> x;
            if (MolUtils::check_is_float(x, line))
                coords[k][i].setX(std::stof(x) * conv);
            else
            {
                inputfile.close();
                return std::make_pair(false, line);
            }


            coord_data >> y;
            if (MolUtils::check_is_float(y, line))
                coords[k][i].setY(std::stof(y) * conv);
            else
            {
                inputfile.close();
                return std::make_pair(false, line);
            }

            coord_data >> z;
            if (MolUtils::check_is_float(z, line))
                coords[k][i].setZ(std::stof(z) * conv);
            else
            {
                inputfile.close();
                return std::make_pair(false, line);
            }
        }

        // Move to center of mass
        float x_com = 0.0;
        float y_com = 0.0;
        float z_com = 0.0;
        float m_i;

        for (int i = 0; i < zval.size(); ++i)
        {
            m_i = static_cast<float>(periodic_table::masses[zval[i]]);
            x_com -= m_i * coords[k][i][0];
            y_com -= m_i * coords[k][i][1];
            z_com -= m_i * coords[k][i][2];
        }

        x_com /= molecular_mass;
        y_com /= molecular_mass;
        z_com /= molecular_mass;

        for (int i = 0; i < zval.size(); ++i)
        {
            coords[k][i][0] += x_com;
            coords[k][i][1] += y_com;
            coords[k][i][2] += z_com;
        }
    }

    formula_string = MolUtils::build_formula_string(formula_vector);

    return std::make_pair(true, "success");
}

std::pair<bool, std::string> Mol::Molecule::get_molden_file(const std::string &filename)
{
    // Read a Molden normal mode file

    num_frames = 1;

    auto detect_type = [](const std::string s)
    {
        return !s.empty() && s.find_first_not_of("0123456789") != std::string::npos;
    };

    std::ifstream inputfile(filename, std::ifstream::in);
    std::string line;
    std::getline(inputfile, line);
    //std::string header;

    // header
    if (line != "[Molden Format]")
    {
        inputfile.close();
        return std::make_pair(false, "Incorrect header");
    }

    // frequencies
    frequencies.clear();
    formula_vector.clear();
    ir_intensities.clear();
    zval.clear();

    for(;;)
    {
        std::getline(inputfile, line); // blank line
        if (line.empty()) continue;
        else if (line == "[FREQ]") continue;
        else if (line == "[FR-COORD]") break;

        std::istringstream freq_data(line);
        std::string freq;
        freq_data >> freq;

        if (MolUtils::check_is_float(freq, line))
            frequencies.push_back(std::stod(freq));
        else
        {
            inputfile.close();
            return std::make_pair(false, line);
        }
    }

    coords = QVector<QVector<QVector3D>>(1);
    int k = 0;

    // coordinates
    for(;;)
    {
        std::getline(inputfile, line);
        if (line.empty()) continue;
        else if (line == "[FR-NORM-COORD]") break;

        natoms = k + 1;
        std::istringstream geom_data(line);
        coords[0].resize(natoms);
        coords[0][k] = QVector3D();

        std::string atom_zval_or_symbol;
        geom_data >> atom_zval_or_symbol;

        if (detect_type(atom_zval_or_symbol))
        {
            zval.push_back(periodic_table::atomic_number(atom_zval_or_symbol));
            formula_vector.emplace_back(atom_zval_or_symbol);
        }
        else
        {
            zval.push_back(std::stoul(atom_zval_or_symbol));
            formula_vector.emplace_back(periodic_table::symbols[std::stoul(atom_zval_or_symbol)]);
        }

        molecular_mass += static_cast<float>(periodic_table::masses[zval[k]]);
        std::string x, y, z;

        geom_data >> x;
        if (MolUtils::check_is_float(x, line))
            coords[0][k][0] = std::stof(x);
        else
        {
            inputfile.close();
            return std::make_pair(false, line);
        }

        geom_data >> y;
        if (MolUtils::check_is_float(y, line))
            coords[0][k][1] = std::stof(y);
        else
        {
            inputfile.close();
            return std::make_pair(false, line);
        }

        geom_data >> z;
        if (MolUtils::check_is_float(z, line))
            coords[0][k][2] = std::stof(z);
        else
        {
            inputfile.close();
            return std::make_pair(false, line);
        }

        ++k;
    }

    formula_string = MolUtils::build_formula_string(formula_vector);

    // Move to center of mass
    // Molden files should alredy be, but just in case.
    float x_com = 0.0;
    float y_com = 0.0;
    float z_com = 0.0;
    float m_i;

    for (int i = 0; i < zval.size(); ++i)
    {
        m_i = static_cast<float>(periodic_table::masses[zval[i]]);
        x_com -= m_i * coords[0][i][0];
        y_com -= m_i * coords[0][i][1];
        z_com -= m_i * coords[0][i][2];
    }

    x_com /= molecular_mass;
    y_com /= molecular_mass;
    z_com /= molecular_mass;

    for (int i = 0; i < zval.size(); ++i)
    {
        coords[0][i][0] += x_com;
        coords[0][i][1] += y_com;
        coords[0][i][2] += z_com;
    }

    // normal coordinates
    k = 0;
    do
    {
        std::getline(inputfile, line); // title
        if (!line.length()) break;

        normal_modes.resize(normal_modes.size() + 1);
        normal_modes[k].resize(natoms);

        for (int i = 0; i < natoms; ++i)
        {
            std::getline(inputfile, line);
            std::istringstream norm_coord_data(line);
            std::string x, y, z;

            normal_modes[k][i] = QVector3D();

            norm_coord_data >> x;
            if (MolUtils::check_is_float(x, line))
                normal_modes[k][i][0] = std::stof(x);
            else
            {
                inputfile.close();
                return std::make_pair(false, line);
            }

            norm_coord_data >> y;
            if (MolUtils::check_is_float(y, line))
                normal_modes[k][i][1] = std::stof(y);
            else
            {
                inputfile.close();
                return std::make_pair(false, line);
            }

            norm_coord_data >> z;
            if (MolUtils::check_is_float(z, line))
                normal_modes[k][i][2] = std::stof(z);
            else
            {
                inputfile.close();
                return std::make_pair(false, line);
            }
        }
        ++k;
    }
    while(true);

    if (!inputfile.eof())
        std::getline(inputfile, line); // [INT]


    if (line == "[INT]")
        do
        {

            std::getline(inputfile, line); // title
            if (!line.length()) break;

            std::istringstream ir_intensity_data(line);
            std::string ir_data;
            ir_intensity_data >> ir_data;

            if (MolUtils::check_is_float(ir_data, line))
                ir_intensities.push_back(std::stod(ir_data));
            else
            {
                inputfile.close();
                return std::make_pair(false, line);
            }
        }
        while (!inputfile.eof());

    inputfile.close();

    return std::make_pair(true, "success");
}

double Mol::Molecule::bond(int num, const int a, const int b) const
{
  return std::sqrt( (coords[num][a][0] - coords[num][b][0]) * (coords[num][a][0] - coords[num][b][0])
             + (coords[num][a][1] - coords[num][b][1]) * (coords[num][a][1] - coords[num][b][1])
             + (coords[num][a][2] - coords[num][b][2]) * (coords[num][a][2] - coords[num][b][2]));
}

QVector<QVector<bool>> Mol::Molecule::get_connectivity(int num)
{
    if (file_type == Mol::fileTypes::FILE_TYPE_CML) return connectivity;

    r = QVector<QVector<double>>(natoms);
    bond_order =  QVector<QVector<int>>(natoms);

    for (int i = 0; i < zval.size(); ++i)
    {
        for (int j = 0; j < zval.size(); ++j)
            r[i].push_back(bond(num, i, j));

        bond_order[i] = QVector<int>(natoms);
    }

    connectivity =  QVector<QVector<bool>>(natoms);

    for (int i = 0; i < zval.size(); ++i)
    {
        connectivity[i] = QVector<bool>(natoms, false);
        for (int j = 0; j < i; ++j)
        {
            if (r[i][j] < 1.3 * (periodic_table::covalent_radii[zval[i]] +
                                 periodic_table::covalent_radii[zval[j]])) {
                connectivity[i][j] = true;
                bond_order[i][j] = 1;  // TODO all single for now
            }
            else
            {
                connectivity[i][j] = false;
                bond_order[i][j] = 0;
            }

            connectivity[j][i] = connectivity[i][j];
            bond_order[j][i] = bond_order[i][j];
        }
    }

    return connectivity;
}
