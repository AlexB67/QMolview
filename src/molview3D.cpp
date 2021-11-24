#include "molview3D.h"
#include <QtGui/QScreen>
#include <Qt3DRender/qcamera.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qcameralens.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qsceneloader.h>
#include <Qt3DRender/qpointlight.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/qaspectengine.h>
#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DExtras/qt3dwindow.h>
#include <QCursor>

MolView3D::MolView3D(QWidget *parent, QComboBox *view_angles)
    : m_parent(parent),
      m_view_angles(view_angles)
{
    container = QWidget::createWindowContainer(this);
    QSize screenSize = screen()->size();
    container->setMinimumSize(QSize(600, 600));
    container->setMaximumSize(screenSize);

    // Root
    rootEntity = new Qt3DCore::QEntity();

    // Camera
    cameraEntity = camera();
    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(20.0f, 15.0f, 20.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));

    // Lights
    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(lightEntity);

    light->setColor("white");
    light->setIntensity(1.0);
    lightEntity->addComponent(light);

    lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightTransform->setTranslation(cameraEntity->position());
    lightEntity->addComponent(lightTransform);

    // Camera controller
    camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    camController->setCamera(cameraEntity);

    // set root
    setRootEntity(rootEntity);
}

MolView *MolView3D::createScene()
{
    m_modifier = new MolView(rootEntity, cameraEntity, lightTransform);
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
    if (event->button() == Qt::RightButton)
    {
        if (m_view_angles) m_view_angles->setCurrentText(tr("Custom"));
        m_modifier->setFocus(true);
        unsetCursor();
    }
    else if (event->button() == Qt::LeftButton)
        unsetCursor();
    else
        m_modifier->setFocus(false);

    Qt3DExtras::Qt3DWindow::mouseReleaseEvent(event);
}


void MolView3D::keyReleaseEvent(QKeyEvent *event)
{
     if (event->key() == Qt::Key_Alt || event->key() == Qt::Key_Up ||
         event->key() == Qt::Key_Down || event->key() == Qt::Key_Left ||
         event->key() == Qt::Key_Right)
     {
         if (m_view_angles) m_view_angles->setCurrentText(tr("Custom"));
         m_modifier->setFocus(true);
     }
     else
         m_modifier->setFocus(false);

     Qt3DExtras::Qt3DWindow::keyReleaseEvent(event);
}
