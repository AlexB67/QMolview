#include "molview3D_cam_controller.h"
#include <Qt3DRender/qcamera.h>

Qt3DCustom::OrbitCamController::OrbitCamController(Qt3DCore::QEntity *e)
: Qt3DExtras::QAbstractCameraController(e), m_entity(e)
{
    upVectorY = QVector3D(0.0f, 1.0f, 0.0f);
    upVectorZ = QVector3D(0.0f, 0.0f, 1.0f);
    key_linear_speed = linearSpeed() / 2.0f;
    key_speed = lookSpeed() / 2.0f;
    mouse_speed = lookSpeed();
    mouse_linear_speed = linearSpeed();
    zoom_limit = 2.0f;
}

void Qt3DCustom::OrbitCamController::set_camera_mouse_speed(int value)
{
    setLookSpeed(static_cast<float>(value) * mouse_speed / 100.0f);
    setLinearSpeed(static_cast<float>(value) * mouse_linear_speed / 100.0f);

    key_speed = lookSpeed() / 2.0f;
    key_linear_speed = linearSpeed() / 2.0f;
}

float Qt3DCustom::OrbitCamController::clampInputs(float input1, float input2)
{
    float axisValue = input1 + input2;
    return (axisValue < -1) ? -1 : (axisValue > 1) ? 1 : axisValue;
}
float Qt3DCustom::OrbitCamController::zoomDistance(QVector3D firstPoint, QVector3D secondPoint)
{
    return (secondPoint - firstPoint).lengthSquared();
}

void Qt3DCustom::OrbitCamController::moveCamera(const QAbstractCameraController::InputState &state, float dt)
{   // tweaked from the default camera code with pannning along Z as well as Y
    // Makes it much more usable .. IMO
    // TODO track object center, not viewcenter for rotations
    Qt3DRender::QCamera *theCamera = camera();

    if (theCamera == nullptr)
        return;

    // keyboard and mouse
    if (state.leftMouseButtonActive)
    {
        if (state.rightMouseButtonActive)
        {
            if (zoomDistance(camera()->position(), theCamera->viewCenter()) > zoom_limit * zoom_limit)
            {
                theCamera->translate(QVector3D(0, 0, state.ryAxisValue), theCamera->DontTranslateViewCenter);
            }
            else
            {
                theCamera->translate(QVector3D(0, 0, -0.5), theCamera->DontTranslateViewCenter);
            }
        }
        else
        {
            // Translate
            theCamera->translate(QVector3D(clampInputs(state.rxAxisValue, state.txAxisValue) * linearSpeed(),
                       clampInputs(state.ryAxisValue, state.tyAxisValue) * linearSpeed(), 0) * dt);
        }

        return;
    }
    else if (state.rightMouseButtonActive)
    {
        if (state.altKeyActive)
        {    // Orbit pan Z
            theCamera->panAboutViewCenter((- state.rxAxisValue * lookSpeed()) * dt, upVectorZ);
            theCamera->tiltAboutViewCenter((state.ryAxisValue * lookSpeed()) * dt);
            return;
        }
        else
        {    // Orbit pan Y
            theCamera->panAboutViewCenter((state.rxAxisValue * lookSpeed()) * dt, upVectorY);
            theCamera->tiltAboutViewCenter((state.ryAxisValue * lookSpeed()) * dt);
            return;
        }

        return;
    }

    // keyboard only
    if (state.altKeyActive)
    {   // Orbit
        theCamera->panAboutViewCenter((- state.txAxisValue * key_speed) * dt, upVectorZ);
        theCamera->tiltAboutViewCenter((state.tyAxisValue * key_speed) * dt);
    }
    else if (state.shiftKeyActive)
    {   // Orbit
        theCamera->panAboutViewCenter((state.txAxisValue * key_speed) * dt, upVectorY);
        theCamera->tiltAboutViewCenter((state.tyAxisValue * key_speed) * dt);
    }
    else
    {    // Translate
         theCamera->translate(QVector3D(clampInputs(state.leftMouseButtonActive
                                       ? state.rxAxisValue : 0, state.txAxisValue) * key_linear_speed,
                                       clampInputs(state.leftMouseButtonActive
                                       ? state.ryAxisValue : 0, state.tyAxisValue) * key_linear_speed,
                                       state.tzAxisValue * key_linear_speed) * dt);
    }
}
