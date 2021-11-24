#ifndef MOLVIEW3D_H
#define MOLVIEW3D_H

#include "molview.h"
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <Qt3DExtras/qforwardrenderer.h>
#include <Qt3DExtras/QOrbitCameraController>
#include <QtWidgets/QComboBox>
#include <QMouseEvent>
#include <QEvent>
#include <QWidget>

class MolView3D : public Qt3DExtras::Qt3DWindow
{
public:
    MolView3D(QWidget *parent = nullptr, QComboBox *m_view_angles = nullptr);
    ~MolView3D() override { delete m_modifier;}
    MolView     *createScene();
    QWidget     *get_container() const { return container; }

private:
    QWidget                 *m_parent;
    QComboBox               *m_view_angles;
    QWidget                 *container;
    Qt3DCore::QEntity       *rootEntity;
    Qt3DRender::QCamera     *cameraEntity;
    Qt3DExtras::QOrbitCameraController *camController;
    Qt3DCore::QTransform    *lightTransform;
    MolView                 *m_modifier;
    bool start_mouse_move{false};

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
};

#endif // MOLVIEW3D_H
