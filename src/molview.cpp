/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "molview.h"
#include <QtCore/QDebug>
#include <Qt3DRender/qpointlight.h>
#include <Qt3DCore/qtransform.h>
#include <math.h>
#include <memory>
#include <iostream>

MolView::MolView(Qt3DCore::QEntity *rootEntity,
                 Qt3DRender::QCamera *cameraEntity,
                 Qt3DCore::QTransform *lightTransform)
    : m_rootEntity(rootEntity),
      m_camera(cameraEntity),
      m_lightTransform(lightTransform)
{
    QObject::connect(m_camera, &Qt3DRender::QCamera::positionChanged,
                     this, &MolView::camera_Changed);
}

MolView::~MolView()
{
    stop_normal_mode();
}

void MolView::addBond(Qt3DCore::QEntity *parentEntity, const QVector3D &srcPos,
                      const QVector3D &targPos, const int atom_idx1, const int atom_idx2)
{
    auto edgeEntity = new Qt3DCore::QEntity{parentEntity};
    auto cylinder = new Qt3DExtras::QCylinderMesh{edgeEntity};

    bond_lengths.push_back(cylinder);

    auto len = srcPos.distanceToPoint(targPos);
    cylinder->setLength(len);
    cylinder->setRadius(scale * 0.15f); // 0.15

    auto transPoint = targPos - srcPos;

    float xAngle = static_cast<float>(std::atan(sqrt(std::pow(transPoint.z(), 2) + std::pow(transPoint.x(), 2))
                  / transPoint.y()) / pi * 180);

    float yAngle = (transPoint.x() == 0 && transPoint.z() == 0)
                 ? 0
                 : static_cast<float>(std::atan(transPoint.x() / transPoint.z()) / pi * 180);

    auto transform = new Qt3DCore::QTransform{edgeEntity};

    if(srcPos[2] > targPos[2]) xAngle *= -1;
    transform->setRotationX(xAngle);
    transform->setRotationY(yAngle);
    transform->setTranslation((srcPos + targPos) / 2);

    bonds_transforms.push_back(transform); // store for animations
    bonds_indices.push_back(QVector<int>{atom_idx1, atom_idx2}); // store for animations

    auto material = new Qt3DExtras::QPhongMaterial{edgeEntity};
    material->setDiffuse(QColor(199, 199, 199));
    material->setShininess(500.0f);
    material->setAmbient(QColor(20, 20, 20, 125));

    edgeEntity->addComponent(cylinder);
    edgeEntity->addComponent(transform);
    edgeEntity->addComponent(material);
    edgeEntity->setObjectName("bond");
}

void MolView::addAtom(Qt3DCore::QEntity *parentEntity, const QVector3D &pos, const float covradii, const QColor colour)
{
    auto edgeEntity = new Qt3DCore::QEntity{parentEntity};
    auto sphere = new Qt3DExtras::QSphereMesh{edgeEntity};
    auto transform = new Qt3DCore::QTransform{edgeEntity};

    sphere->setSlices(32);

    transform->setScale(scale * covradii * 0.35f); // 0.35
    transform->setTranslation(pos);

    atoms_transforms.push_back(transform); // store for animations

    auto material = new Qt3DExtras::QPhongMaterial{edgeEntity};
    material->setDiffuse(colour);
    material->setShininess(500.0f);
    material->setAmbient(QColor(20, 20, 20, 125));

    edgeEntity->addComponent(sphere);
    edgeEntity->addComponent(transform);
    edgeEntity->addComponent(material);
    edgeEntity->setObjectName("atom");
}

void MolView::coords_to_QVector(const std::vector<Matrix2D<double>>& coords_in, const int natoms)
{
     // Note: std vector uses size_t, but Qvector and co use an int
    coords = QVector<QVector<QVector3D>>(num_frames);

    for (int k = 0; k < num_frames; ++k)
        coords[k].resize(natoms);

    size_t k_ = 0;
    for (int k = 0; k < num_frames; ++k)
    {
        size_t i_ = 0;
        for (int i = 0; i < natoms; ++i)
        {
            coords[k][i] =
            QVector3D(static_cast<float>(coords_in[k_][i_][0U]),
                      static_cast<float>(coords_in[k_][i_][1U]),
                      static_cast<float>(coords_in[k_][i_][2U]));
            ++i_;
        }
        ++k_;
    }
}

void MolView::normal_modes_to_QVector(const std::vector<Matrix2D<double>>& modes_in,
                                      const std::vector<double>& freq_in,
                                      const std::vector<double>& intensities_in,
                                      const int natoms)
{
    // Note: std vector uses size_t, but Qvector and co use an int
    int num_modes = static_cast<int>(modes_in.size());
    normal_modes = QVector<QVector<QVector3D>>(num_modes);

    frequencies = QVector<double>(num_modes);

    if (intensities_in.size())
    {
        intensities = QVector<double>(num_modes);

        for (int k = 0; k < num_modes; ++k)
            intensities[k] = intensities_in[static_cast<size_t>(k)];
    }

    for (int k = 0; k < num_modes; ++k)
    {
        normal_modes[k].resize(natoms);
        frequencies[k] = freq_in[static_cast<size_t>(k)];
    }

    size_t k_ = 0;
    for (int k = 0; k < num_modes; ++k)
    {
        size_t i_ = 0;
        for (int i = 0; i < natoms; ++i)
        {
            normal_modes[k][i] = QVector3D(static_cast<float>(modes_in[k_][i_][0U]),
                                           static_cast<float>(modes_in[k_][i_][1U]),
                                           static_cast<float>(modes_in[k_][i_][2U]));
            ++i_;
        }
        ++k_;
   }
}

void MolView::connectivity_to_QVector(const Matrix2D<bool>& conn_in, const int natoms)
{
    connectivity = QVector<QVector<bool>>(natoms);

    size_t k_ = 0;
    for (int k = 0; k < natoms; ++k)
    {
        size_t i_ = 0;
        connectivity[k] = QVector<bool>(natoms);
        for (int i = 0; i < natoms; ++i)
        {
            connectivity[k][i] = conn_in[k_][i_];
            ++i_;
        }
        ++k_;
   }
}

void MolView::zvals_to_QVector(const std::vector<size_t>& zvals_in, const int natoms)
{
    zvals = QVector<size_t>(natoms);
    size_t i_ = 0;
    for (int i = 0; i < natoms; ++i)
    {
        zvals[i] = zvals_in[i_];
        ++i_;
    }
}

void MolView::renderFunc(const QString& file,  const QString& suffix)
{
    std::unique_ptr<Mol::Molecule> mol = std::make_unique<Mol::Molecule>();

    const Mol::fileTypes filetype = (suffix == "xyz")
    ? Mol::fileTypes::FILE_TYPE_XYZ : Mol::fileTypes::FILE_TYPE_MOLDEN;

    mol->get_geom(file.toStdString(), filetype);

    num_frames = static_cast<int>(mol->get_num_structures());

    if (filetype == Mol::fileTypes::FILE_TYPE_XYZ)
        sendFrames(QString("Number of Frames: ") + QString::number(num_frames));
    else
        sendFrames(QString("Number of Normal Modes: ") + QString::number(mol->get_normal_modes().size()));

    scale = 1.0; //1.5 / mol->get_largest_axis();
    num_atoms = static_cast<int>(mol->get_num_atoms());
    coords_to_QVector(mol->get_coords(), static_cast<int>(mol->get_num_atoms()));
    zvals_to_QVector(mol->get_zvalues(), static_cast<int>(mol->get_num_atoms()));

    // For now we assume connectivity is unaltered from the first frame onwards
    // TODO for all frames
    connectivity_to_QVector(mol->get_connectivity(0), static_cast<int>(mol->get_num_atoms()));

    if (filetype == Mol::fileTypes::FILE_TYPE_MOLDEN)
    {
        normal_modes_to_QVector(mol->get_normal_modes(),
                                mol->get_frequencies(),
                                mol->get_ir_intensities(),
                                static_cast<int>(mol->get_num_atoms()));
        sendFrequencies();
    }

    int at = 0; // draw atoms
    for(const auto& c : std::as_const(coords[0]))
    {
        const float radius = static_cast<float>(periodic_table::covalent_radii[zvals[at]]);
        addAtom(m_rootEntity, scale * c, radius, atom_colours[zvals[at]]);
        ++at;
    }

    // draw bonds from connectivity info
    for (int i = 0; i < zvals.size(); ++i)
        for (int j = 0; j < i; ++j)
            if (connectivity[i][j])
                addBond(m_rootEntity, scale * coords[0][i], scale * coords[0][j], i, j);

   reset();
}

void MolView::cleanup()
{
    foreach(auto *component, m_rootEntity->components())
    {
        m_rootEntity->removeComponent(component);
        delete(component);
        component = nullptr;
    }

    foreach (QObject *child, m_rootEntity->children())
    {
        Qt3DCore::QEntity *childEntity = qobject_cast<Qt3DCore::QEntity *>(child);
        if (childEntity->objectName() == "bond" || childEntity->objectName() == "atom") // bonds and atoms only
        {
            delete childEntity;
            childEntity = nullptr;
        }
    }

    atoms_transforms.clear();
    bonds_transforms.clear();
    bond_lengths.clear();
    bonds_indices.clear();
    coords.clear();
    normal_modes.clear();
    frequencies.clear();
    intensities.clear();
    zvals.clear();
    connectivity.clear();
}
void MolView::scale_atoms(QDial *dial)
{
    const float value = static_cast<float>(dial->value()) / 100.0f;

    for (int i = 0; i < num_atoms; ++i)
    {
        const float radius = static_cast<float>(periodic_table::covalent_radii[zvals[i]]);
        atoms_transforms[i]->setScale(scale * radius * value);
    }

   // float min_radius = scale * value * static_cast<float>(periodic_table::covalent_radii[1]);
    float bond_scale = 0.5f * scale * (value - 0.2f) + 0.075f;

    if (bond_scale < 0.1f) bond_scale = 0.1f;

    for (const auto& tr : std::as_const(bond_lengths))
        tr->setRadius(bond_scale);

    m_camera->viewAll();
}

void MolView::set_atom_coords(int frame)
{
    for (int i = 0; i < num_atoms; ++i)
        atoms_transforms[i]->setTranslation(coords[frame][i]);
}

void MolView::set_bond_coords(int frame)
{
    for (int bond = 0; bond < bonds_indices.size(); ++bond)
    {
        int i = bonds_indices[bond][0];
        int j = bonds_indices[bond][1];

        const QVector3D srcPos = coords[frame][i];
        const QVector3D targPos = coords[frame][j];

        auto len = srcPos.distanceToPoint(targPos);
        bond_lengths[bond]->setLength(len);

        auto transPoint = targPos - srcPos;
        float xAngle = static_cast<float>(std::atan(sqrt(std::pow(transPoint.z(), 2)
                     + std::pow(transPoint.x(), 2)) / transPoint.y()) / pi * 180);

        float yAngle = (transPoint.x() == 0 && transPoint.z() == 0)
                ? 0
                : static_cast<float>(std::atan(transPoint.x() / transPoint.z()) / pi * 180);

        if(srcPos[2] > targPos[2]) xAngle *= -1;
        bonds_transforms[bond]->setRotationX(xAngle);
        bonds_transforms[bond]->setRotationY(yAngle);
        bonds_transforms[bond]->setTranslation((srcPos + targPos) / 2);
    }
}

