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

#ifndef MOLVIEW_H
#define MOLVIEW_H

#include "geom.h"
#include <iostream>
#include <string>
#include <QtCore/QObject>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QDial>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QOrbitCameraController>
//#include <Qt3DExtras/qfirstpersoncameracontroller.h>
#include <Qt3DRender/QCamera>

// https://sciencenotes.org/molecule-atom-colors-cpk-colors/
// awked out of the web page, I wasn't gonna type all that :)
constexpr std::array<QColor, 96> atom_colours = {
    QColor(0, 0, 0), // X dummy
    QColor(255,255,255),
    QColor(217,255,255),
    QColor(204,128,255),
    QColor(194,255,0),
    QColor(255,181,181),
    QColor(144,144,144),
    QColor(48,80,248),
    QColor(255,13,13),
    QColor(144,224,80),
    QColor(179,227,245),
    QColor(171,92,242),
    QColor(138,255,0),
    QColor(191,166,166),
    QColor(240,200,160),
    QColor(255,128,0),
    QColor(255,255,48),
    QColor(31,240,31),
    QColor(128,209,227),
    QColor(143,64,212),
    QColor(61,255,0),
    QColor(230,230,230),
    QColor(191,194,199),
    QColor(166,166,171),
    QColor(138,153,199),
    QColor(156,122,199),
    QColor(224,102,51),
    QColor(240,144,160),
    QColor(80,208,80),
    QColor(200,128,51),
    QColor(125,128,176),
    QColor(194,143,143),
    QColor(102,143,143),
    QColor(189,128,227),
    QColor(255,161,0),
    QColor(166,41,41),
    QColor(92,184,209),
    QColor(112,46,176),
    QColor(0,255,0),
    QColor(148,255,255),
    QColor(148,224,224),
    QColor(115,194,201),
    QColor(84,181,181),
    QColor(59,158,158),
    QColor(36,143,143),
    QColor(10,125,140),
    QColor(0,105,133),
    QColor(192,192,192),
    QColor(255,217,143),
    QColor(166,117,115),
    QColor(102,128,128),
    QColor(158,99,181),
    QColor(212,122,0),
    QColor(148,0,148),
    QColor(66,158,176),
    QColor(87,23,143),
    QColor(0,201,0),
    QColor(112,212,255),
    QColor(255,255,199),
    QColor(217,255,199),
    QColor(199,255,199),
    QColor(163,255,199),
    QColor(143,255,199),
    QColor(97,255,199),
    QColor(69,255,199),
    QColor(48,255,199),
    QColor(31,255,199),
    QColor(0,255,156),
    QColor(0,230,117),
    QColor(0,212,82),
    QColor(0,191,56),
    QColor(0,171,36),
    QColor(77,194,255),
    QColor(77,166,255),
    QColor(33,148,214),
    QColor(38,125,171),
    QColor(38,102,150),
    QColor(23,84,135),
    QColor(208,208,224),
    QColor(255,209,35),
    QColor(184,184,208),
    QColor(166,84,77),
    QColor(87,89,97),
    QColor(158,79,181),
    QColor(171,92,0),
    QColor(117,79,69),
    QColor(66,130,150),
    QColor(66,0,102),
    QColor(0,125,0),
    QColor(112,171,250),
    QColor(0,186,255),
    QColor(0,161,255),
    QColor(0,143,255),
    QColor(0,128,255),
    QColor(0,107,255),
    QColor(84,92,242)
};

class MolView : public QObject
{
    Q_PROPERTY(bool focus MEMBER m_focus WRITE setFocus)
    Q_OBJECT

public:
    explicit MolView(Qt3DCore::QEntity *rootEntity,
                     Qt3DRender::QCamera *cameraEntity,
                     Qt3DCore::QTransform *lightTransform);

    ~MolView() override;

    void setFocus(bool focus) {m_focus = focus;}
    void openFile(const QString& file, const QString& suffix);
    void play_normal_mode(QTableWidget *);
    void stop_normal_mode();
    void resetView(QComboBox *);
    void RotateX(QPushButton *, QComboBox *);
    void RotateY(QPushButton *, QComboBox *);
    void RotateZ(QPushButton *, QComboBox *);
    void views(QComboBox *);
    void reset();
    void play_xyz_animation(QSpinBox *, QPushButton *);
    void scale_atoms(QDial *);

public
slots:
    void showFrame(QSpinBox *);
    void sendFrames(const QString&);
    void sendFrequencies();
    size_t get_frames_count() { return num_frames; }

signals:
    void num_frames_changed(const QString&);
    void frequencies_changed(const QVector<double>&, const QVector<double>&);

private:
    Qt3DCore::QEntity    *m_rootEntity;
    Qt3DRender::QCamera  *m_camera;
    Qt3DCore::QTransform *m_lightTransform;
    QVector<Qt3DExtras::QCylinderMesh *> bond_lengths;
    QVector<Qt3DCore::QTransform *> atoms_transforms;
    QVector<Qt3DCore::QTransform *> bonds_transforms;
    QVector<QVector<QVector3D>> coords;
    QVector<QVector<QVector3D>> normal_modes;
    QVector<double> frequencies;
    QVector<double> intensities;
    QVector<size_t> zvals;
    QVector<QVector<bool>> connectivity;
    QVector<QVector<int>>  bonds_indices;
    int num_frames{0};
    int num_atoms{0};
    float scale{0};
    float step{0};
    bool m_focus{false};

    void addBond(Qt3DCore::QEntity *parentEntity, const QVector3D &srcPos,
                 const QVector3D &targPos, const int atom_idx1, const int atom_idx2);

    void addAtom(Qt3DCore::QEntity *parentEntity, const QVector3D &Pos,
                 const float covradii, const QColor colour);

    void coords_to_QVector(const std::vector<Matrix2D<double>>& coords_in,
                           const int natoms);

    void normal_modes_to_QVector(const std::vector<Matrix2D<double>>& modes_in,
                                 const std::vector<double>& freq_in,
                                 const std::vector<double>& intensities_in,
                                 const int natoms);

    void connectivity_to_QVector(const Matrix2D<bool>& conn_in,
                                 const int natoms);

    void zvals_to_QVector(const std::vector<size_t>& zvals_in,
                          const int natoms);

    void set_atom_coords(int frame);
    void set_bond_coords(int frame);

    void renderFunc(const QString& file,  const QString& suffix);
    void cleanup();

private slots:
    void camera_Changed();
};

#endif // MolView_H

