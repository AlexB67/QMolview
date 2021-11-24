#include "geom.h"
#include <fstream>
#include <limits>
#include <iostream>
#include <istream>
#include <sstream>
#include <qglobal.h>

// std::filesystem bugged in min-gw so disable
// The test isd redundant anyway as it is
// checked in the file dialog
#ifdef Q_OS_LINUX
#include <filesystem>
#endif

void Mol::Molecule::get_geom(const std::string& filename, fileTypes filetype)
{

#ifdef Q_OS_LINUX
    if (!std::filesystem::exists(filename))
         throw std::logic_error("File not found.");
#endif

    file_type = filetype;

    if (filetype == Mol::fileTypes::FILE_TYPE_MOLDEN)
    {
        get_normal_modes(filename);
        return;
    }

    auto detect_type = [](const std::string s)
    {
            return !s.empty() && s.find_first_not_of("0123456789") != std::string::npos;
    };

    std::ifstream inputfile(filename, std::ifstream::in);

    for (size_t k = 0; ; ++k)
    {
        std::string line;
        std::getline(inputfile, line);
        std::istringstream data(line);
        data >> natoms; // read number of atoms
        std::getline(inputfile, line);
        std::string comment = line;

        num_sctructures = k;

        if (inputfile.eof())
        {
            inputfile.close();
            return;
        }

        geom.resize(geom.size() + 1);
        geom[k].resize(natoms);
        if (!k) zval.resize(natoms);

        std::string atom_zval_or_symbol;

        for(size_t i = 0; i < zval.size(); ++i) // read file line by line
        {
            std::getline(inputfile, line);
            if (line.empty()) continue;

            std::istringstream coord_data(line);

            coord_data >> atom_zval_or_symbol;

            if (k == 0)
            {
                if (detect_type(atom_zval_or_symbol))
                    zval[i] = periodic_table::atomic_number(atom_zval_or_symbol);
                else
                    zval[i] = std::stoul(atom_zval_or_symbol);
            }

            molecular_mass += periodic_table::masses[zval[i]];
            geom[k][i] = Matrix1D<double>(3);
            double tmp;
            coord_data >> tmp;
            geom[k][i][0] = tmp;
            coord_data >> tmp;
            geom[k][i][1] = tmp;
            coord_data >> tmp;
            geom[k][i][2] = tmp;

            geom[k][i][0] *= periodic_table::angstrom_to_bohr;
            geom[k][i][1] *= periodic_table::angstrom_to_bohr;
            geom[k][i][2] *= periodic_table::angstrom_to_bohr;
        }

        // Move to center of mass
        double x_com = 0.0;
        double y_com = 0.0;
        double z_com = 0.0;
        double m_i;

        for (size_t i = 0; i < zval.size(); ++i)
        {
            m_i = periodic_table::masses[zval[i]];
            x_com += m_i * geom[k][i][0];
            y_com += m_i * geom[k][i][1];
            z_com += m_i * geom[k][i][2];
        }

        x_com /= molecular_mass;
        y_com /= molecular_mass;
        z_com /= molecular_mass;

        for (size_t i = 0; i < zval.size(); ++i)
        {
            geom[k][i][0] += x_com;
            geom[k][i][1] += y_com;
            geom[k][i][2] += z_com;
        }
    }

    return;
}

void Mol::Molecule::get_normal_modes(const std::string &filename)
{
    // Read a Molden normal mode file

    num_sctructures = 1;

    auto detect_type = [](const std::string s)
    {
            return !s.empty() && s.find_first_not_of("0123456789") != std::string::npos;
    };

    std::ifstream inputfile(filename, std::ifstream::in);
    std::string line;
    std::getline(inputfile, line);
    std::string header;

    // header
    if (line != "[Molden Format]")
    {
        inputfile.close();
        return;
    }

    // frequencies
    frequencies.clear();

    for(;;)
    {
        std::getline(inputfile, line); // blank line
        if (line.empty()) continue;
        else if (line == "[FREQ]") continue;
        else if (line == "[FR-COORD]") break;

        std::istringstream freq_data(line);
        double freq;
        freq_data >> freq;
        frequencies.push_back(freq);
    }

    geom.resize(1);
    size_t k = 0;

    // coordinates
    for(;;)
    {
        std::getline(inputfile, line);
        if (line.empty()) continue;
        else if (line == "[FR-NORM-COORD]") break;

        natoms = k + 1;
        std::istringstream geom_data(line);
        geom[0].resize(k + 1);
        geom[0][k].resize(3);

        std::string atom_zval_or_symbol;
        geom_data >> atom_zval_or_symbol;

        if (detect_type(atom_zval_or_symbol))
            zval.push_back(periodic_table::atomic_number(atom_zval_or_symbol));
        else
            zval.push_back(std::stoul(atom_zval_or_symbol));

        molecular_mass += periodic_table::masses[zval[k]];
        std::string x, y, z;
        geom_data >> x;
        geom[0][k][0] = std::stod(x);
        geom_data >> y;
        geom[0][k][1] = std::stod(y);
        geom_data >> z;
        geom[0][k][2] = std::stod(z);

        ++k;
    }

    // Move to center of mass
    // Molden files should alredy be, but just in case.
    double x_com = 0.0;
    double y_com = 0.0;
    double z_com = 0.0;
    double m_i;

    for (size_t i = 0; i < zval.size(); ++i)
    {
        m_i = periodic_table::masses[zval[i]];
        x_com += m_i * geom[0][i][0];
        y_com += m_i * geom[0][i][1];
        z_com += m_i * geom[0][i][2];
    }

    x_com /= molecular_mass;
    y_com /= molecular_mass;
    z_com /= molecular_mass;

    for (size_t i = 0; i < zval.size(); ++i)
    {
        geom[0][i][0] += x_com;
        geom[0][i][1] += y_com;
        geom[0][i][2] += z_com;
    }

    // normal coordinates
    k = 0;
    do
    {
        std::getline(inputfile, line); // title
        if (!line.length()) break;

        normal_modes.resize(normal_modes.size() + 1);
        normal_modes[k].resize(natoms);

        for (size_t i = 0; i < natoms; ++i)
        {
            std::getline(inputfile, line);
            std::istringstream norm_coord_data(line);
            std::string x, y, z;

            normal_modes[k][i] = Matrix1D<double>(3);

            norm_coord_data >> x;
            normal_modes[k][i][0] = std::stod(x);
            norm_coord_data >> y;
            normal_modes[k][i][1] = std::stod(y);
            norm_coord_data >> z;
            normal_modes[k][i][2] = std::stod(z);
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
            ir_intensities.push_back(std::stod(ir_data));
        }
        while (!inputfile.eof());

    inputfile.close();
}

double Mol::Molecule::get_largest_axis(std::size_t num) const
{
    double dist_min = std::numeric_limits<double>::max();
    double dist_max = std::numeric_limits<double>::min();

    for (size_t i = 0; i < zval.size(); ++i)
        for (size_t a = 0; a < 3; ++a)
        {
            if (geom[num][i][a] < dist_min) dist_min = geom[num][i][a];
            if (geom[num][i][a] > dist_max) dist_max = geom[num][i][a];
        }

    return dist_max - dist_min;
}

double Mol::Molecule::bond(std::size_t num, const size_t a, const size_t b) const
{
  return sqrt( (geom[num][a][0] - geom[num][b][0]) * (geom[num][a][0] - geom[num][b][0])
             + (geom[num][a][1] - geom[num][b][1]) * (geom[num][a][1] - geom[num][b][1])
             + (geom[num][a][2] - geom[num][b][2]) * (geom[num][a][2] - geom[num][b][2]));
}

Matrix2D<bool> Mol::Molecule::get_connectivity(std::size_t num)
{
    r.resize(natoms);

    for ( size_t i = 0; i < zval.size(); ++i)
        for ( size_t j = 0; j < zval.size(); ++j)
            r[i].emplace_back(bond(num, i, j));

    connctivity =  Matrix2D<bool>(natoms);

    for ( size_t i = 0; i < zval.size(); ++i)
    {
        connctivity[i] = Matrix1D<bool>(natoms, false);
        for ( size_t j = 0; j < i; ++j)
        {
            if (r[i][j] < 1.3 * (periodic_table::covalent_radii[zval[i]] +
                                 periodic_table::covalent_radii[zval[j]])) connctivity[i][j] = true;
            connctivity[j][i] = connctivity[i][j];
        }
    }

    return connctivity;
}
