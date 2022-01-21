#ifndef MOLVIEW3D_ATOM_H
#define MOLVIEW3D_ATOM_H

#include <Qt3DExtras/QSphereMesh>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QDiffuseSpecularMaterial>

namespace Qt3DCustom
{
class AtomMesh : public Qt3DCore::QEntity
{
public:
    AtomMesh(const QVector3D &pos, 
             const float covradius,
             const float scale,
             const QColor& colour,
             Qt3DCore::QEntity *parentEntity = nullptr);

    AtomMesh(const AtomMesh &) = delete;
    AtomMesh &operator=(const AtomMesh &other) = delete;
    AtomMesh(const AtomMesh &&) = delete;
    AtomMesh &&operator=(const AtomMesh &&other) = delete;
    ~AtomMesh();

    const QColor& Colour() const { return m_colour; }
    QColor& Colour() { return m_colour; }
    void setAlpha(const bool blend, float alpha = 1.0f);
    Qt3DCore::QTransform * getTransform() const { return m_transform; }
    
private:
    QVector3D             m_pos;
    float                 m_covradius;
    float                 m_scale;
    QColor                m_colour;
    Qt3DExtras::QSphereMesh *m_sphere;
    Qt3DCore::QTransform    *m_transform;
    Qt3DExtras::QDiffuseSpecularMaterial *m_material;
};
}

#endif
