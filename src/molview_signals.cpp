#include "molview.h"
#include <Qt3DRender/qpointlight.h>
#include <Qt3DCore/qtransform.h>
#include <QEvent>
#include <QMouseEvent>
#include <QEventLoop>
#include <QTimer>
#include <math.h>
#include <iostream>

void MolView::camera_Changed()
{
    m_lightTransform->setTranslation(m_camera->position());
}

void MolView::RotateX(QPushButton *button, QComboBox *viewbox)
{
    m_camera->setViewCenter(QVector3D(0, 0, 0));
    viewbox->setCurrentText(tr("Custom"));

    while(button->isDown())
    {
        m_camera->panAboutViewCenter(0.5, QVector3D(1, 0, 0));
        QEventLoop eventloop;
        QTimer::singleShot(5, &eventloop, &QEventLoop::quit);
        eventloop.exec();
    }
}

void MolView::RotateY(QPushButton *button, QComboBox *viewbox)
{
    m_camera->setViewCenter(QVector3D(0, 0, 0));
    viewbox->setCurrentText(tr("Custom"));

    while(button->isDown())
    {
        m_camera->panAboutViewCenter(0.5, QVector3D(0, 1, 0));
        QEventLoop eventloop;
        QTimer::singleShot(5, &eventloop, &QEventLoop::quit);
        eventloop.exec();
    }
}

void MolView::RotateZ(QPushButton *button, QComboBox *viewbox)
{
    m_camera->setViewCenter(QVector3D(0, 0, 0));
    viewbox->setCurrentText(tr("Custom"));

    while(button->isDown())
    {
        m_camera->panAboutViewCenter(0.5, QVector3D(0, 0, 1));
        QEventLoop eventloop;
        QTimer::singleShot(5, &eventloop, &QEventLoop::quit);
        eventloop.exec();
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

void MolView::play_normal_mode(QTableWidget *mode_table)
{
    int row = mode_table->currentRow();
    if (row < 0 || row >= mode_table->rowCount()) return;

    if (step > 1.0f) step = 0;
    float sin_arg = static_cast<float>(std::sin(2.0f * pi * step));

    for (int i = 0; i < num_atoms; ++i)
    {
        QVector3D pos = normal_modes[row][i] * sin_arg;
        atoms_transforms[i]->setTranslation(coords[0][i] + pos);
    }

    for (int bond = 0; bond < bonds_indices.size(); ++bond)
    {
        int i = bonds_indices[bond][0];
        int j = bonds_indices[bond][1];

        QVector3D srcPos = coords[0][i];
        QVector3D targPos = coords[0][j];

        QVector3D src = normal_modes[row][i] * sin_arg;
        srcPos += src;

        QVector3D target = normal_modes[row][j] * sin_arg;
        targPos += target;

        auto len = srcPos.distanceToPoint(targPos);
        bond_lengths[bond]->setLength(len);

        auto transPoint = targPos - srcPos;
        float xAngle = static_cast<float>(std::atan(sqrt(std::pow(transPoint.z(), 2)
                     + std::pow(transPoint.x(), 2)) / transPoint.y()) / pi * 180);

        float yAngle = (transPoint.x() == 0 && transPoint.z() == 0)
                ? 0.0f
                : static_cast<float>(std::atan(transPoint.x() / transPoint.z()) / pi * 180);

        if(srcPos[2] > targPos[2]) xAngle *= -1;
        bonds_transforms[bond]->setRotationX(xAngle);
        bonds_transforms[bond]->setRotationY(yAngle);
        bonds_transforms[bond]->setTranslation((srcPos + targPos) / 2);
    }

    step += 0.025f;

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
    m_camera->setPosition(QVector3D(20.0f, 15.0f, 20.0f));
//     m_camera->setPosition(QVector3D(0.0f, 0.0f, 20.0f));
    m_camera->setUpVector(QVector3D(0, 1, 0));
    m_camera->setViewCenter(QVector3D(0, 0, 0));
    m_camera->viewAll();
}
void MolView::resetView(QComboBox *view_angles)
{
    view_angles->setCurrentText(tr("Default"));
}

void MolView::views(QComboBox *view_angles)
{
    if (view_angles->currentText() == tr("Custom"))
        return;

    if (view_angles->currentText() == tr("Default"))
    {
        m_camera->setPosition(QVector3D(20.0f, 15.0f, 20.0f));
        m_camera->setUpVector(QVector3D(0, 1, 0));
    }
    else if (view_angles->currentText() == tr("Front"))
    {
        m_camera->setPosition(QVector3D(0, 0.0, 20.0f));
        m_camera->setUpVector(QVector3D(0, 1, 0)); // 0 0 1
    }
    else if (view_angles->currentText() == tr("Back"))
    {
        m_camera->setPosition(QVector3D(0, 0.0, -20.0f));
        m_camera->setUpVector(QVector3D(0, 1, 0)); // 0 0 1
    }
    else if (view_angles->currentText() == tr("Left"))
    {
        m_camera->setPosition(QVector3D(20, 0.0, 0.0));
        m_camera->setUpVector(QVector3D(0, 1, 0)); // 1 0 0
    }

    else if (view_angles->currentText() == tr("Right"))
    {
        m_camera->setPosition(QVector3D(-20, 0.0, 0.0));
        m_camera->setUpVector(QVector3D(0, 1, 0)); // 1 0 0
    }

    m_camera->setViewCenter(QVector3D(0, 0, 0));
    m_camera->viewAll();
}

void MolView::openFile(const QString& file, const QString& suffix)
{
    if (file.length() && suffix.length())
    {
        cleanup();
        renderFunc(file, suffix);
    }
}

void MolView::sendFrames(const QString& num_frames)
{
    Q_EMIT num_frames_changed(num_frames);
}

void MolView::sendFrequencies()
{
    Q_EMIT frequencies_changed(frequencies, intensities);
}
