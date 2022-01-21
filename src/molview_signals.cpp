#include "molview.h"
#include "molecule.h"
#include <Qt3DRender/qpointlight.h>
#include <Qt3DCore/qtransform.h>
#include <QEvent>
#include <QFile>
#include <QMouseEvent>
#include <QEventLoop>
#include <QTimer>
#include <cmath>
#include <iostream>


// When  button is held
void MolView::RotateX(QPushButton *button, QComboBox *viewbox, float direction, bool snap)
{
    viewbox->setCurrentIndex(-1);
    if (snap) m_camera->setViewCenter(QVector3D(0, 0, 0));

    while(button->isDown())
    {
        m_camera->panAboutViewCenter(direction * 0.4f, QVector3D(1, 0, 0));
        QEventLoop eventloop;
        QTimer::singleShot(5, &eventloop, &QEventLoop::quit);
        eventloop.exec();
    }
}

// When a button initiates a timer
void MolView::RotateX(float direction)
{
    m_camera->panAboutViewCenter(direction * 0.5f, QVector3D(1, 0, 0));
}

void MolView::RotateY(QPushButton *button, QComboBox *viewbox, float direction, bool snap)
{
    viewbox->setCurrentIndex(-1);
     if (snap) m_camera->setViewCenter(QVector3D(0, 0, 0));

    while(button->isDown())
    {
        m_camera->panAboutViewCenter(direction * 0.4f, QVector3D(0, 1, 0));
        QEventLoop eventloop;
        QTimer::singleShot(5, &eventloop, &QEventLoop::quit);
        eventloop.exec();
    }
}

void MolView::RotateY(float direction)
{
    m_camera->panAboutViewCenter(direction * 0.5f, QVector3D(0, 1, 0));
}

void MolView::RotateZ(QPushButton *button, QComboBox *viewbox, float direction, bool snap)
{
    viewbox->setCurrentIndex(-1);
    if (snap) m_camera->setViewCenter(QVector3D(0, 0, 0));

    while(button->isDown())
    {
        m_camera->panAboutViewCenter(direction * 0.4f, QVector3D(0, 0, 1));
        QEventLoop eventloop;
        QTimer::singleShot(5, &eventloop, &QEventLoop::quit);
        eventloop.exec();
    }
}

void MolView::RotateZ(float direction)
{
    m_camera->panAboutViewCenter(direction * 0.5f, QVector3D(0, 0, 1));
}

void MolView::scale_atoms(QDial *dial)
{
    const float value = static_cast<float>(dial->value()) / 100.0f;

    for (int i = 0; i < num_atoms; ++i)
    {
        const float radius = static_cast<float>(periodic_table::covalent_radii[zvals[i]]);
        atoms[i]->getTransform()->setScale(scale * radius * value);
    }

    // single bonds
    float bond_scale_s = 0.5f * scale * (value - 0.2f) + 0.075f;
    if (bond_scale_s < 0.1f) bond_scale_s = 0.1f;

    // double bonds
    float bond_scale_d = 0.5f * scale * (value * 1.5f - 0.2f) + 0.075f;
    if (bond_scale_d < 0.1f) bond_scale_d = 0.1f;

    // triple bonds or higher
    float bond_scale_t = 0.5f * scale * (value * 2.0f - 0.2f) + 0.075f;
    if (bond_scale_t < 0.1f) bond_scale_t = 0.1f;

    for (const auto& b : std::as_const(bonds))
    {
        float bond_scale = 1.0f;
        if (b->getBondOrder() == 1) bond_scale = bond_scale_s;
        else if (b->getBondOrder() == 2) bond_scale = bond_scale_d;
        else bond_scale = bond_scale_t;

        b->getBond()->setRadius(bond_scale);
    }
}

void MolView::showFrame(QSpinBox *spinner)
{
    int frame_num = spinner->value();
    set_atom_coords(frame_num - 1);
    set_bond_coords(frame_num - 1);
}

void MolView::play_xyz_animation(QSpinBox *spinner, QPushButton *)
{   
    spinner->blockSignals(true);

    (spinner->value() == spinner->maximum())
     ? spinner->setValue(1)
     : spinner->setValue(spinner->value() + 1);

    int frame_num  = spinner->value();

    set_atom_coords(frame_num - 1);
    set_bond_coords(frame_num - 1);
}

void MolView::play_normal_mode(QTableWidget *mode_table, float amplitude)
{
    int row = mode_table->currentRow();
    if (row < 0 || row >= mode_table->rowCount()) return;

    if (step > 1.0) step = 0.0;
    float sin_arg = amplitude * static_cast<float>(std::sin(2.0 * pi * step));

    for (int i = 0; i < num_atoms; ++i)
    {
        QVector3D pos = normal_modes[row][i] * sin_arg;
        atoms[i]->getTransform()->setTranslation(coords[0][i] + pos);
    }

    for (int bond = 0; bond < bonds.size(); ++bond)
    {
        int i = bonds[bond]->atom_one_idx();
        int j = bonds[bond]->atom_two_idx();
        const QVector3D& atom1_vec = coords[0][i] + normal_modes[row][i] * sin_arg;
        const QVector3D& atom2_vec = coords[0][j] + normal_modes[row][j] * sin_arg;
        bonds[bond]->setBondCoords(atom1_vec, atom2_vec);
    }

    step += 0.025f;
}

void  MolView::set_norm_vectors(QTableWidget *mode_table, QCheckBox *box, float amplitude)
{
    if (!mode_vectors1.size()) return;

    if(!box->isChecked())
    {
        for (int i = 0; i < mode_vectors1.size(); ++i)
        {
            mode_vectors1[i]->showAxis(false);
            mode_vectors2[i]->showAxis(false);
        }

        for (auto& b : bonds)
            b->setAlpha(false);

        for (auto& a : atoms)
            a->setAlpha(false);

        return;
    }

    int row = mode_table->currentRow();
    if (row < 0 || row >= mode_table->rowCount()) return;

    for (int i = 0; i < mode_vectors1.size(); ++i)
    {
        if (coords[0][i].distanceToPoint(coords[0][i] + normal_modes[0][i]) > 0.001f)
        {
            mode_vectors1[i]->showAxis(true);
            mode_vectors2[i]->showAxis(true);
            mode_vectors1[i]->setCoords(coords[0][i], coords[0][i] - amplitude * normal_modes[row][i]);
            mode_vectors2[i]->setCoords(coords[0][i], coords[0][i] + amplitude * normal_modes[row][i]);
        }
        else
        {
             mode_vectors1[i]->showAxis(false);
             mode_vectors2[i]->showAxis(false);
        }
    }

    if (box->checkState() == Qt::Checked)
    {
        for (auto& b : bonds)
            b->setAlpha(true, 0.2f);

        for (auto& a : atoms)
            a->setAlpha(true, 0.7f);
    }
    else if (box->checkState() == Qt::PartiallyChecked)
    {
        for (auto& b : bonds)
            b->setAlpha(false);

        for (auto& a : atoms)
            a->setAlpha(false);
    }

}

void MolView::stop_normal_mode()
{
    // restore defalt coordinates
    step = 0;
    set_atom_coords(0);
    set_bond_coords(0);
}

void MolView::reset()
{
    m_camera->setViewCenter(QVector3D(0, 0, 0));
    m_camera->setPosition(QVector3D(-10.0f, 10.0f, 15.0f));
    m_camera->setUpVector(QVector3D(0, 1, 0));
    m_camera->viewAll();
}
void MolView::resetView(QComboBox *view_angles)
{
    view_angles->setCurrentText(tr("Perspective 1"));
}

void MolView::views(QComboBox *view_angles)
{
    m_camera->setViewCenter(QVector3D(0, 0, 0));

    if (view_angles->currentIndex() == -1)
        return;
    if (view_angles->currentText() == tr("Perspective 1"))
    {
      reset();
    }
    else if (view_angles->currentText() == tr("Perspective 2"))
    {
        m_camera->setUpVector(QVector3D(0, 1, 0));
        m_camera->setPosition(QVector3D(20.0f, 15.0f, 20.0f));
    }
    else if (view_angles->currentText() == tr("Perspective 3"))
    {
        m_camera->setUpVector(QVector3D(0, 1, 0));
        m_camera->setPosition(QVector3D(5.0f, 5.0f, 10.0f));
        m_camera->panAboutViewCenter(90.0, QVector3D(1, 0, 0));
    }
    else if (view_angles->currentText() == tr("Front"))
    {
        m_camera->setUpVector(QVector3D(0, 1, 0));
        m_camera->setPosition(QVector3D(0, 0.0, 20.0f));
    }
    else if (view_angles->currentText() == tr("Back"))
    {
        m_camera->setUpVector(QVector3D(0, 1, 0));
        m_camera->setPosition(QVector3D(0, 0.0, -20.0f));
    }
    else if (view_angles->currentText() == tr("Left"))
    {
        m_camera->setUpVector(QVector3D(0, 1, 0));
        m_camera->setPosition(QVector3D(20, 0.0, 0.0));
    }
    else if (view_angles->currentText() == tr("Right"))
    {
        m_camera->setUpVector(QVector3D(0, 1, 0));
        m_camera->setPosition(QVector3D(-20, 0.0, 0.0));
    }
    else if (view_angles->currentText() == tr("Top"))
    {   // avoid exact position to not deadlock the mouse
        m_camera->setUpVector(QVector3D(0, 1, 0));
        m_camera->setPosition(QVector3D(-20.0, 0.0, 0.0));
        m_camera->panAboutViewCenter(90, QVector3D(0, 0, 1));
        m_camera->panAboutViewCenter(90, QVector3D(0, 1, 0));
    }
    else if (view_angles->currentText() == tr("Bottom"))
    {   // avoid exact position to not deadlock the mouse
        m_camera->setUpVector(QVector3D(0, 1, 0));
        m_camera->setPosition(QVector3D(-20.0, 0.0, 0.0));
        m_camera->panAboutViewCenter(90, QVector3D(0, 0, 1));
        m_camera->panAboutViewCenter(-90, QVector3D(0, 1, 0));
    }

//    m_camera->setViewCenter(QVector3D(0, 0, 0));
    m_camera->viewAll();
}

void MolView::openFile(const QString& file, const QString& suffix)
{
    if (file.length() && suffix.length())
    {
        cleanup();
        init_renderFunc(file, suffix);
    }
}

void MolView::sendFileInfo(const QString& file_info_as_str)
{
    Q_EMIT file_info_changed(file_info_as_str);
}

void MolView::sendFrequencies()
{
    Q_EMIT frequencies_changed(frequencies, intensities);
}

void MolView::reveal_axes(bool reveal, bool see_through)
{
    for (auto& x : axes)
        x->showAxis(reveal);

    for (auto& b : bonds)
    b->setAlpha(see_through, 0.2f);

    for (auto& a : atoms)
        a->setAlpha(see_through, 0.7f);
}

void MolView::write_xyz(const QString& file)
{
    auto file_out = QFile(file);

     if (!file_out.open(QIODevice::WriteOnly | QIODevice::Text))
             return;

     QTextStream out(&file_out);
     out.setRealNumberPrecision(6);
     out.setRealNumberNotation(QTextStream::FixedNotation);
       out.setFieldAlignment(QTextStream::AlignRight);
     out << zvals.size() << "\n";
     out << "QMolView scene to xyz";

     float conv = static_cast<float>(periodic_table::angstrom_to_bohr);

     int i = 0;
     for (const auto& a : std::as_const(atoms))
     {
         out.setFieldWidth(0);
         out << "\n";
         out.setFieldWidth(4);
         out << std::string(periodic_table::symbols[zvals[i]]).c_str();
         out.setFieldWidth(12);

         if (export_original_axes)
             out << a->getTransform()->translation()[axes_idx[0]] / conv
                 << a->getTransform()->translation()[axes_idx[1]] / conv
                 << a->getTransform()->translation()[axes_idx[2]] / conv;
         else
             out << a->getTransform()->translation()[0] / conv
                 << a->getTransform()->translation()[1] / conv
                 << a->getTransform()->translation()[2] / conv;
         ++i;
     }

     file_out.close();
}

