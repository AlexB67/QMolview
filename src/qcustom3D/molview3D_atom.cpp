#include "molview3D_atom.h"

Qt3DCustom::AtomMesh::AtomMesh(const QVector3D &pos,
                               const float covradius,
                               const float scale,
                               const QColor &colour,
                               Qt3DCore::QEntity *parentEntity)
    : Qt3DCore::QEntity(parentEntity),
      m_pos(pos),
      m_covradius(covradius),
      m_scale(scale),
      m_colour(colour)
{
    m_sphere = new Qt3DExtras::QSphereMesh{this};
    m_sphere->setSlices(32);

    m_transform = new Qt3DCore::QTransform(this);
    m_transform->setScale(m_scale * covradius * 0.35f);
    m_transform->setTranslation(pos);
    m_transform->setObjectName("transform");

    m_material = new Qt3DExtras::QDiffuseSpecularMaterial{this};

    m_material->setDiffuse(colour);
    m_material->setShininess(500.0f);
    m_material->setAmbient(QColor(20, 20, 20));

    addComponent(m_sphere);
    addComponent(m_transform);
    addComponent(m_material);
}

void Qt3DCustom::AtomMesh::setAlpha(const bool blend, float alpha)
{
    m_material->setAlphaBlendingEnabled(blend);
    // No need to reset since no blending is alpha = 1
    if (blend)
    {
        m_colour.setAlphaF(alpha);
        m_material->setDiffuse(m_colour);
    }
}

Qt3DCustom::AtomMesh::~AtomMesh()
{
    removeComponent(m_sphere);
    removeComponent(m_transform);
    removeComponent(m_material);
    
    delete m_sphere;
    delete m_material;
    delete m_transform;
}
