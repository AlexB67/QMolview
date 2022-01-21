#ifndef MOLVIEW3D_CAMCONTROLLER_H
#define MOLVIEW3D_CAMCONTROLLER_H

#include <Qt3DExtras/QAbstractCameraController>
#include <Qt3DCore/QEntity>
#include <QVector3D>

namespace Qt3DCustom
{ 
class OrbitCamController : public Qt3DExtras::QAbstractCameraController
{
public:
    OrbitCamController(Qt3DCore::QEntity *entity);
    ~OrbitCamController() {}

    void set_camera_mouse_speed(int);

private:
    Qt3DCore::QEntity *m_entity;
    float zoom_limit;
    float key_speed;
    float key_linear_speed;
    float mouse_speed;
    float mouse_linear_speed;
    QVector3D upVectorY;
    QVector3D upVectorZ;

    float clampInputs(float input1, float input2);
    float zoomDistance(QVector3D firstPoint, QVector3D secondPoint);
    void moveCamera(const QAbstractCameraController::InputState &state, float dt) override;
};
}

#endif
