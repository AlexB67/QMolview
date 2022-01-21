#include "molview3D.h"
#include <QtGui/QScreen>
#include <Qt3DRender/qcamera.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qcameralens.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qsceneloader.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/qaspectengine.h>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DExtras/qt3dwindow.h>
#include <QCursor>
#include <iostream>

MolView3D::MolView3D(QWidget *parent, QComboBox *view_angles, QLabel *cam_info_message)
    : m_parent(parent),
      m_view_angles(view_angles),
      m_cam_info_message(cam_info_message)
{
    // Root
    rootEntity = new Qt3DCore::QEntity();

    // Camera
    cameraEntity = camera();
    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));

    // Lights Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(cameraEntity);
    light = new Qt3DRender::QPointLight(lightEntity);

    light->setColor(QColor(Qt::white));
    light->setIntensity(1.0f);
    lightEntity->addComponent(light);

    lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightTransform->setTranslation(cameraEntity->position());
    lightEntity->addComponent(lightTransform);

    // Custom Camera controller
    camController = new Qt3DCustom::OrbitCamController(rootEntity);
    camController->setCamera(cameraEntity);

    // set root
    setRootEntity(rootEntity);
}

void MolView3D::set_viewport_Intensity(int value)
{
    light->setIntensity(static_cast<float>(value) / 100.0f);
}

MolView *MolView3D::createScene()
{
    m_modifier = new MolView(rootEntity, cameraEntity);
    return m_modifier;
}

void MolView3D::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton || event->button() == Qt::LeftButton)
    {
        setCursor(Qt::OpenHandCursor);
        start_mouse_move = true;
    }

    Qt3DExtras::Qt3DWindow::mousePressEvent(event);
}

void MolView3D::mouseMoveEvent(QMouseEvent *event)
{
    if (start_mouse_move)
    {
        start_mouse_move = false;
        setCursor(Qt::ClosedHandCursor);
    }

    Qt3DExtras::Qt3DWindow::mouseMoveEvent(event);
}



void MolView3D::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton || event->button() == Qt::LeftButton)
    {
        if (m_view_angles) m_view_angles->setCurrentIndex(-1);
        m_modifier->setFocus();
    }

    start_mouse_move = false;
    unsetCursor();
    Qt3DExtras::Qt3DWindow::mouseReleaseEvent(event);
}

void MolView3D::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Shift || event->key() == Qt::Key_Alt ||
        event->key() == Qt::Key_Up || event->key() == Qt::Key_Down ||
        event->key() == Qt::Key_Left || event->key() == Qt::Key_Right)
    {
        if (m_view_angles)
            m_view_angles->setCurrentIndex(-1);
        m_modifier->setFocus();

        if (event->key() == Qt::Key_Alt && m_cam_info_message)
            m_cam_info_message->setText(tr("Pan Mode: Y"));
    }
    else
        m_modifier->setFocus();

    unsetCursor();
    Qt3DExtras::Qt3DWindow::keyReleaseEvent(event);
}

void MolView3D::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Alt)
    {
        if (m_cam_info_message) m_cam_info_message->setText(tr("Pan Mode: Z"));
    }
    else if (event->key() == Qt::Key_Shift)
    {
        if (m_cam_info_message) m_cam_info_message->setText(tr("Pan Mode: Y"));
    }

    Qt3DExtras::Qt3DWindow::keyPressEvent(event);
}
