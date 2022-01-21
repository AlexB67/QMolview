#include "molview3D_bond.h"
#include <QtMath>

Qt3DCustom::BondMesh::BondMesh(const int atom_idx1, 
                               const int atom_idx2,
                               const int bond_order,
                               const float scale,
                               const QVector3D &start_pos, 
                               const QVector3D &end_pos, 
                               Qt3DCore::QEntity *parentEntity)
    : Qt3DCore::QEntity(parentEntity),
      m_idx1(atom_idx1),
      m_idx2(atom_idx2),
      m_bond_order(bond_order),
      m_scale(scale),
      m_start_pos(start_pos),
      m_end_pos(end_pos)
{
    m_cylinder = new Qt3DExtras::QCylinderMesh{this};
    
    auto len = start_pos.distanceToPoint(end_pos);
    m_cylinder->setLength(len);
    // for now we draw multiple bonds as fatter cylinders
    // TODO draw multiple cylinders
    float radius = 0.15f;
    if (m_bond_order == 2)
        radius *= 1.5f;
    else if (m_bond_order >= 3)
        radius *= 2.0f;

    m_cylinder->setRadius(m_scale * radius);

    m_transform = new Qt3DCore::QTransform{this};
    m_transform->setObjectName("transform");

    setBondCoords(m_start_pos, m_end_pos);

    m_material = new Qt3DExtras::QDiffuseSpecularMaterial{this};
    m_colour = QColor(199, 199, 199);
    m_material->setDiffuse(m_colour);
    m_material->setShininess(500.0f);
    m_material->setAmbient(QColor(20, 20, 20));

    addComponent(m_cylinder);
    addComponent(m_transform);
    addComponent(m_material);
}

void Qt3DCustom::BondMesh::setBondCoords(const QVector3D &start_pos, const QVector3D &end_pos)
{
    const auto& pos = end_pos - start_pos;
    m_cylinder->setLength(start_pos.distanceToPoint(end_pos));

    float theta = static_cast<float>(qAtan(qSqrt(pos.x() * pos.x() + pos.z() * pos.z())
                  / pos.y()) * DEG_TO_RAD);

    float phi = (qFuzzyCompare(pos.x(), 0.0f) && qFuzzyCompare(pos.z(), 0.0f))
              ? 0 : static_cast<float>(qAtan(pos.x() / pos.z()) * DEG_TO_RAD);

  //  if(start_pos[2] > end_pos[2]) theta *= -1;
    if(start_pos[2] > end_pos[2]) phi += 180;
    m_transform->setRotationX(theta);
    m_transform->setRotationY(phi);
    m_transform->setTranslation((start_pos + end_pos) / 2);
}

void Qt3DCustom::BondMesh::setAlpha(const bool blend, float alpha)
{
    m_material->setAlphaBlendingEnabled(blend);
    // No need to reset since no blending is alpha = 1
    if (blend)
    {
        m_colour.setAlphaF(alpha);
        m_material->setDiffuse(m_colour);
    }
}

Qt3DCustom::BondMesh::~BondMesh()
{
    removeComponent(m_cylinder);
    removeComponent(m_transform);
    removeComponent(m_material);
    
    delete m_cylinder;
    delete m_material;
    delete m_transform;
}
