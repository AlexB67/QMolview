#ifndef MOLVIEW3D_AXES_H
#define MOLVIEW3D_AXES_H

#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QDiffuseSpecularMaterial>

namespace Qt3DCustom
{

class AxisMesh
{
public:
    AxisMesh(float length,
             const QColor& colour,
             const QVector3D& target,
             Qt3DCore::QEntity *parentEntity = nullptr);

    AxisMesh(const QColor& colour,
             const QVector3D& source,
             const QVector3D& target,
             Qt3DCore::QEntity *parentEntity = nullptr);

    AxisMesh(const AxisMesh &) = delete;
    AxisMesh &operator=(const AxisMesh &other) = delete;
    AxisMesh(const AxisMesh &&) = delete;
    AxisMesh &&operator=(const AxisMesh &&other) = delete;
    ~AxisMesh();

    void showAxis(bool show);
    void setCoords(const QVector3D &start_pos, const QVector3D &end_pos);
    
private:
    Qt3DCore::QEntity         *m_childEntity_cylinder{nullptr};
    Qt3DCore::QEntity         *m_childEntity_cone{nullptr};
    Qt3DExtras::QCylinderMesh *m_cylinder{nullptr};
    Qt3DExtras::QConeMesh     *m_cone{nullptr};
    Qt3DCore::QTransform      *m_transform_cylinder{nullptr};
    Qt3DCore::QTransform      *m_transform_cone{nullptr};
    Qt3DExtras::QDiffuseSpecularMaterial *m_cylinder_material{nullptr};
    Qt3DExtras::QDiffuseSpecularMaterial *m_cone_material{nullptr};
    bool has_cone{true};
};
}
#endif
