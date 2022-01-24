#ifndef MOLVIEW3D_H
#define MOLVIEW3D_H

#include "../molview.h"
#include "molview3D_cam_controller.h"
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <Qt3DExtras/qforwardrenderer.h>
#include <Qt3DRender/qpointlight.h>
//#include <QtGui/QOpenGLContext>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QMouseEvent>
#include <QEvent>
#include <QWidget>

class MolView3D : public Qt3DExtras::Qt3DWindow
{
public:
    MolView3D(QWidget   *parent = nullptr,
              QComboBox *view_angles = nullptr,
              QLabel    *view_angles_custom = nullptr,
              QLabel    *cam_info_message = nullptr);

    ~MolView3D() override {delete m_modifier;}

    MolView3D(const MolView3D&) = delete;
    MolView3D& operator=(const MolView3D& other) = delete;
    MolView3D(const MolView3D&&) = delete;
    MolView3D&& operator=(const MolView3D&& other) = delete;

    MolView     *createScene();
    Qt3DCustom::OrbitCamController *get_cam_controller() const { return camController; }
    void set_viewport_Intensity(int value);

private:
    QWidget                 *m_parent;
    QComboBox               *m_view_angles;
    QLabel                  *m_view_angles_custom;
    QLabel                  *m_cam_info_message;
    Qt3DCore::QEntity       *rootEntity;
    Qt3DRender::QCamera     *cameraEntity;
    Qt3DRender::QPointLight *light;
    Qt3DCustom::OrbitCamController  *camController;
    Qt3DCore::QTransform    *lightTransform;
    MolView                 *m_modifier;
    bool start_mouse_move{false};

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // MOLVIEW3D_H
