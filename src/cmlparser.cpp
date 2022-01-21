#include "molecule.h"
#include "mol_utils.h"
#include <QFile>
#include <tuple>
#include <QStringList>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QXmlStreamAttribute>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

struct connections
{
    int idx1;
    int idx2;
    int bond_order;
};

Q_DECLARE_TYPEINFO(connections, Q_PRIMITIVE_TYPE);

std::pair<bool, std::string> Mol::Molecule::get_cml_file(const std::string& filename)
{
    QFile inputFile(filename.c_str());

    if (!QFile::exists(filename.c_str()))
        return std::make_pair(false, "File doesn't exist.");
    else if (!inputFile.open(QIODevice::ReadOnly))
        return std::make_pair(false, "Open file read only failed");

    QXmlStreamReader reader(&inputFile);
    QVector<connections> id_to_idx;
    coords = QVector<QVector<QVector3D>>(1);

    num_frames = 1;
    zval.clear();
    id_to_idx.clear();
    molecular_mass = 0;
    const float conv = static_cast<float>(periodic_table::angstrom_to_bohr);

    while (!reader.atEnd())
    {
        reader.readNext();

        if (reader.error())
        {
            inputFile.close();
            return std::make_pair(false, reader.errorString().toStdString());
        }
        else
        {
            if (reader.name() == QStringLiteral("atom"))
            {
                for (int i = 0; i < reader.attributes().length(); ++i)
                {
                    if (reader.attributes().at(i).name() == QStringLiteral("elementType"))
                    {
                        size_t z = periodic_table::atomic_number(reader.attributes().at(i).value().toString().toStdString());
                        zval.push_back(z);
                        formula_vector.emplace_back(reader.attributes().at(i).value().toString().toStdString());
                        molecular_mass += static_cast<float>(periodic_table::masses[z]);
                        coords[0].push_back(QVector3D(0, 0, 0));
                        ++natoms;
                    }
                    else if (reader.attributes().at(i).name() == QStringLiteral("x3"))
                    {
                        if (MolUtils::check_is_float(reader.attributes().at(i).value().toString().toStdString(), "x3 invalid "))
                            coords[0][natoms - 1][0] = reader.attributes().at(i).value().toFloat() * conv;
                        else
                        {
                            inputFile.close();
                            return std::make_pair(false, "x3: Invalid float: Line " + std::to_string(reader.lineNumber()));
                        }
                    }
                    else if (reader.attributes().at(i).name() == QStringLiteral("y3"))
                    {
                        if (MolUtils::check_is_float(reader.attributes().at(i).value().toString().toStdString(), "y3 invalid "))
                            coords[0][natoms - 1][1] = reader.attributes().at(i).value().toFloat() * conv;
                        else
                        {
                            inputFile.close();
                            return std::make_pair(false, reader.text().toString().toStdString());
                        }

                    }
                    else if (reader.attributes().at(i).name() == QStringLiteral("z3"))
                    {
                        if (MolUtils::check_is_float(reader.attributes().at(i).value().toString().toStdString(), "z3 invalid "))
                            coords[0][natoms - 1][2] = reader.attributes().at(i).value().toFloat() * conv;
                        else
                        {
                            inputFile.close();
                            return std::make_pair(false, reader.text().toString().toStdString());
                        }
                    }
                }
            }
            else if (reader.name() == QStringLiteral("bond"))
            {
                for (int i = 0; i < reader.attributes().length(); ++i)
                {
                    if (reader.attributes().at(i).name() == QStringLiteral("atomRefs2"))
                    {
                        auto qstr = reader.attributes().at(i).value().toString();
                        auto qstr1 = qstr.section(" ", 0, 0).remove(QRegularExpression("[a-z]"));
                        auto qstr2 = qstr.section(" ", -1).remove(QRegularExpression("[a-z]"));

                        connections c;
                        c.idx1 = qstr1.toInt() - 1;
                        c.idx2 = qstr2.toInt() - 1;
                        id_to_idx.push_back(c);
                    }
                    else if (reader.attributes().at(i).name() == QStringLiteral("order"))
                        id_to_idx[id_to_idx.size() - 1].bond_order = reader.attributes().at(i).value().toInt();
                }
            }
        }
    }

    inputFile.close();

    formula_string = MolUtils::build_formula_string(formula_vector);

    float x_com = 0;
    float y_com = 0;
    float z_com = 0;
    float m_i = 0;

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

    connectivity = QVector<QVector<bool>>(natoms);
    bond_order = QVector<QVector<int>>(natoms);

    for (auto& i : connectivity)
        i = QVector<bool>(natoms, false);

    for (auto& i : bond_order)
        i = QVector<int>(natoms, 0);

    for (const auto& c : std::as_const(id_to_idx))
    {
        connectivity[c.idx1][c.idx2] = true;
        connectivity[c.idx2][c.idx1] = true;
        bond_order[c.idx1][c.idx2] = c.bond_order;
        bond_order[c.idx2][c.idx1] = c.bond_order;
    }

    if (align_axes)
        MolUtils::align_axes(coords, zval, num_frames, 0);

    if (sort_axes)
        axes_idx = MolUtils::sort_axes(coords, natoms, num_frames, 0);

    largest_distance = MolUtils::get_max_extent(coords, natoms, 0);

    return std::make_pair(true, "success");
}
