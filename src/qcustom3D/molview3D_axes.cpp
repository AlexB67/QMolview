#include "molview3D_axes.h"
#include <QtMath>
#include <iostream>

// For Axes with a cone arrow
// starting at (0, 0, 0)
Qt3DCustom::AxisMesh::AxisMesh(float length,
                               const QColor& colour,
                               const QVector3D& target,
                               Qt3DCore::QEntity *parentEntity)
{
    m_childEntity_cylinder = new Qt3DCore::QEntity{parentEntity};
    m_childEntity_cone = new Qt3DCore::QEntity{parentEntity};
    m_cylinder = new Qt3DExtras::QCylinderMesh{m_childEntity_cylinder};
    m_cone = new Qt3DExtras::QConeMesh(m_childEntity_cone);

    const QVector3D& start = QVector3D(0, 0, 0);
    const QVector3D& end = target * length;

    m_cylinder->setLength(start.distanceToPoint(end));
    m_cylinder->setRadius(length / 45.0f);
    m_cone->setLength(length / 10.0f);
    m_cone->setTopRadius(0.0f);
    m_cone->setBottomRadius(length / 25.0f);

    m_transform_cylinder = new Qt3DCore::QTransform{m_childEntity_cylinder};
    m_transform_cone = new Qt3DCore::QTransform{m_childEntity_cone};

    setCoords(start, end);

    m_cylinder_material = new Qt3DExtras::QDiffuseSpecularMaterial{m_childEntity_cylinder};
    m_cylinder_material->setDiffuse(colour);
    m_cylinder_material->setShininess(100.0f);
    m_cylinder_material->setAmbient(QColor(20, 20, 20, 125));

    m_cone_material = new Qt3DExtras::QDiffuseSpecularMaterial{m_childEntity_cone};
    m_cone_material->setDiffuse(colour);
    m_cone_material->setShininess(200.0f);
    m_cone_material->setAmbient(QColor(20, 20, 20, 125));

    m_childEntity_cylinder->addComponent(m_cylinder);
    m_childEntity_cylinder->addComponent(m_transform_cylinder);
    m_childEntity_cylinder->addComponent(m_cylinder_material);

    m_childEntity_cone->addComponent(m_cone);
    m_childEntity_cone->addComponent(m_transform_cone);
    m_childEntity_cone->addComponent(m_cone_material);

    m_cone->setEnabled(false);
    m_cylinder->setEnabled(false);
}

// For vectors, denoted by litle rods
Qt3DCustom::AxisMesh::AxisMesh(const QColor& colour,
                               const QVector3D& source,
                               const QVector3D& target,
                               Qt3DCore::QEntity *parentEntity)
{
    has_cone = false;
    m_childEntity_cylinder = new Qt3DCore::QEntity{parentEntity};
    m_cylinder = new Qt3DExtras::QCylinderMesh{m_childEntity_cylinder};

    const QVector3D& start = source;
    const QVector3D& end = target;

    float length = start.distanceToPoint(end);
    m_cylinder->setLength(length);
    m_cylinder->setRadius(0.03f);

    m_transform_cylinder = new Qt3DCore::QTransform{m_childEntity_cylinder};

    setCoords(start, end);

    m_cylinder_material = new Qt3DExtras::QDiffuseSpecularMaterial{m_childEntity_cylinder};
    m_cylinder_material->setDiffuse(colour);
    m_cylinder_material->setShininess(100.0f);
    m_cylinder_material->setAmbient(QColor(20, 20, 20, 125));

    m_childEntity_cylinder->addComponent(m_cylinder);
    m_childEntity_cylinder->addComponent(m_transform_cylinder);
    m_childEntity_cylinder->addComponent(m_cylinder_material);

    m_cylinder->setEnabled(false);
}

void Qt3DCustom::AxisMesh::setCoords(const QVector3D &start_pos, const QVector3D &end_pos)
{
    constexpr double DEG_TO_RAD{57.295779513082321};
    const auto& pos = end_pos - start_pos;
    m_cylinder->setLength(start_pos.distanceToPoint(end_pos));

    float theta = static_cast<float>(qAtan(qSqrt(pos.x() * pos.x() + pos.z() * pos.z())
                  / pos.y()) * DEG_TO_RAD);

    float phi = (qFuzzyCompare(pos.x(), 0.0f) && qFuzzyCompare(pos.z(), 0.0f))
              ? 0 : static_cast<float>(qAtan(pos.x() / pos.z()) * DEG_TO_RAD);

 //   if(start_pos[2] > end_pos[2]) theta *= -1;
    if(start_pos[2] > end_pos[2]) phi += 180;
    m_transform_cylinder->setRotationX(theta);
    m_transform_cylinder->setRotationY(phi);
    m_transform_cylinder->setTranslation((start_pos + end_pos) / 2);

    if (has_cone)
    {
        m_transform_cone->setRotationX(theta);
        m_transform_cone->setRotationY(phi);
        m_transform_cone->setTranslation(end_pos);
    }
}

void Qt3DCustom::AxisMesh::showAxis(bool show)
{
    m_cylinder->setEnabled(show);
    if (has_cone) m_cone->setEnabled(show);
}

Qt3DCustom::AxisMesh::~AxisMesh()
{
    m_childEntity_cylinder->removeComponent(m_cylinder);
    m_childEntity_cylinder->removeComponent(m_transform_cylinder);
    m_childEntity_cylinder->removeComponent(m_cylinder_material);

    delete m_cylinder;
    delete m_transform_cylinder;
    delete m_cylinder_material;
    delete m_childEntity_cylinder;

    if (has_cone)
    {
        m_childEntity_cone->removeComponent(m_cone);
        m_childEntity_cone->removeComponent(m_transform_cone);
        m_childEntity_cone->removeComponent(m_cone_material);

        delete m_cone;
        delete m_transform_cone;
        delete m_cone_material;
        delete m_childEntity_cone;
    }
}
