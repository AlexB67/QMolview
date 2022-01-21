#include "molecule.h"
#include "molview.h"
#include "CPK_colours.h"
#include <QFile>
#include <QMessageBox>
#include <QtWidgets/QGridLayout>
#include <cmath>
#include <memory>
#include <iostream>

using Qt3DCustom::atom_colours;

MolView::MolView(Qt3DCore::QEntity *rootEntity,
                 Qt3DRender::QCamera *cameraEntity)
    : m_rootEntity(rootEntity),
      m_camera(cameraEntity)
{
}

MolView::~MolView()
{
    stop_normal_mode();
    cleanup();
}

void MolView::init_renderFunc(const QString& file,  const QString& suffix)
{
    std::unique_ptr<Mol::Molecule> mol =
    std::make_unique<Mol::Molecule>(connectivity_from_last_frame, sort_axes, align_axes);

    Mol::fileTypes filetype; // = (suffix == "xyz")
    if (suffix == "xyz") filetype = Mol::fileTypes::FILE_TYPE_XYZ;
    else if (suffix == "cml") filetype = Mol::fileTypes::FILE_TYPE_CML;
    else filetype = Mol::fileTypes::FILE_TYPE_MOLDEN;

    const auto result = mol->get_geom(file.toStdString(), filetype);

    if (!result.first)
    {
        QMessageBox box;
        box.setWindowTitle(tr("File information message."));
        box.setText(tr("Failed to parse input file."));
        box.setDetailedText(tr("Ensure that the file is structured correctly.\n\n"
        "NB: If an xyz file contains multiple frames,\n"
        "each frame must contain the same molecule.\n\n"
        "Normal mode files must be in (Psi4 compatible) Molden format\n"
        "https://www3.cmbi.umcn.nl/molden/molden_format.html\n"
        "CML files must comply with the appropriate XML specification.\n\n"
        "Parse error on line:\n") + result.second.c_str());
        box.setIcon(QMessageBox::Warning);
        box.setStandardButtons(QMessageBox::Close);
        box.setDefaultButton(QMessageBox::Close);

#ifdef Q_OS_LINUX
        QSpacerItem* horizontalSpacer = new QSpacerItem(450, 100, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout *layout = static_cast<QGridLayout *>(box.layout());
        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
#endif
        box.exec();
        return;
    }

    scale = 1.0; // not really used anymore since it's 1, but it is in the code
    num_frames = mol->get_num_frames();
    num_atoms =  mol->get_num_atoms();
    coords = mol->get_coords();
    zvals = mol->get_zvalues();
    axes_idx = mol->get_sort_axes();
    largest_distance = mol->get_max_extent();

    // For now we assume connectivity is unaltered
    // TODO for all frames Molden and cml have 1 frame
    // Options: Use the last or first frame for connectivity
    int num = (connectivity_from_last_frame == true)
            ? num_frames - 1 : 0;

    connectivity = mol->get_connectivity(num);
    bond_order = mol->get_bond_orders();

    if (filetype == Mol::fileTypes::FILE_TYPE_MOLDEN)
    {
        frequencies = mol->get_frequencies();
        intensities = mol->get_ir_intensities();
        normal_modes = mol->get_normal_modes();
        sendFrequencies();
    }

    int at = 0; // draw atoms
    for(const auto& c : std::as_const(coords[0]))
    {
        const float radius = static_cast<float>(periodic_table::covalent_radii[zvals[at]]);
        atoms.push_back(new AtomMesh(c, radius, scale, atom_colours[zvals[at]], m_rootEntity));
        ++at;
    }

    // draw bonds from connectivity info
    for (int i = 0; i < zvals.size(); ++i)
        for (int j = 0; j < i; ++j)
            if (connectivity[i][j])
                bonds.push_back(new BondMesh(i, j, bond_order[i][j], scale, coords[0][i], scale * coords[0][j], m_rootEntity));
    // Axes
    axes.push_back(new AxisMesh(largest_distance / 3.5f, QColor(190, 10, 10, 255), QVector3D(1, 0, 0), m_rootEntity));
    axes.push_back(new AxisMesh(largest_distance / 3.5f, QColor(10, 190, 10, 255), QVector3D(0, 1, 0), m_rootEntity));
    axes.push_back(new AxisMesh(largest_distance / 3.5f, QColor(50, 50, 190, 255), QVector3D(0, 0, 1), m_rootEntity));

    // Normal mode vectors
    if (filetype == Mol::fileTypes::FILE_TYPE_MOLDEN)
    {
        at = 0;
        for(const auto& c : std::as_const(coords[0]))
        {
            mode_vectors1.push_back(new AxisMesh(QColor(190, 10, 10), c, c - normal_modes[0][at], m_rootEntity));
            mode_vectors2.push_back(new AxisMesh(QColor(10, 190, 10), c, c + normal_modes[0][at], m_rootEntity));
            ++at;
        }
    }

    if (filetype == Mol::fileTypes::FILE_TYPE_XYZ || filetype == Mol::fileTypes::FILE_TYPE_CML)
        sendFileInfo(QString("Frames: ") + QString::number(num_frames) +
                   QString(" | Atoms: ") + QString::number(mol->get_zvalues().size()) +
                   QString(" | Bonds: ") + QString::number(bonds.size()) +
                   QString(" | RMM: ") + QString::number(mol->get_mass(), 'f', 4) +
                   QString(" | Formula: ") + mol->get_formula_string().c_str());
    else
        sendFileInfo(QString("Normal Modes: ") + QString::number(mol->get_normal_modes().size()) +
                   QString(" | Atoms: ") + QString::number(mol->get_zvalues().size()) +
                   QString(" | Bonds: ") + QString::number(bonds.size()) +
                   QString(" | RMM: ") + QString::number(mol->get_mass(), 'f', 4) +
                   QString(" | Formula: ") + mol->get_formula_string().c_str());

    reset();
}

void MolView::cleanup()
{
    for (auto& a : atoms)
        delete a;

    atoms.clear();

    for (auto& b : bonds)
        delete b;

    bonds.clear();

    for (auto& x : axes)
        delete x;

    axes.clear();

    for (auto& v : mode_vectors1)
        delete v;

    mode_vectors1.clear();

    for (auto& v : mode_vectors2)
        delete v;

    mode_vectors2.clear();

    coords.clear();
    normal_modes.clear();
    frequencies.clear();
    intensities.clear();
    zvals.clear();
    connectivity.clear();
}

void MolView::set_atom_coords(int frame)
{
    for (int i = 0; i < num_atoms; ++i)
        atoms[i]->getTransform()->setTranslation(coords[frame][i]);
}

void MolView::set_bond_coords(int frame)
{
    for (int bond = 0; bond < bonds.size(); ++bond)
    {
        int i = bonds[bond]->atom_one_idx();
        int j = bonds[bond]->atom_two_idx();

        const QVector3D& start_pos = coords[frame][i];
        const QVector3D& end_pos = coords[frame][j];
        bonds[bond]->setBondCoords(start_pos, end_pos);
    }
}

