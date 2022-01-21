#ifndef MOLVIEW3D_BOND_H
#define MOLVIEW3D_BOND_H

#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QDiffuseSpecularMaterial>

namespace Qt3DCustom
{

inline constexpr double DEG_TO_RAD{57.295779513082321};
class BondMesh : public Qt3DCore::QEntity
{
public:
    BondMesh(const int atom_idx1, 
             const int atom_idx2,
             const int bond_order,
             const float scale,
             const QVector3D &start_pos, 
             const QVector3D &end_pos, 
             Qt3DCore::QEntity *parentEntity);

    BondMesh(const BondMesh &) = delete;
    BondMesh &operator=(const BondMesh &other) = delete;
    BondMesh(const BondMesh &&) = delete;
    BondMesh &&operator=(const BondMesh &&other) = delete;
    ~BondMesh();

    int atom_one_idx() const { return m_idx1; }
    int atom_two_idx() const { return m_idx2; }
    int getBondOrder() const { return m_bond_order; }
    const QColor& Colour() const { return m_colour; }
    QColor& Colour() { return m_colour; }
    void setAlpha(const bool blend, float alpha = 1.0f);
    void setBondCoords(const QVector3D &start_pos, const QVector3D &end_pos);
    Qt3DExtras::QCylinderMesh * getBond() const { return m_cylinder; }
    Qt3DCore::QTransform * getTransform() const { return m_transform; }
    
private:
    int   m_idx1;
    int   m_idx2;
    int   m_bond_order;
    float m_scale;
    QVector3D             m_start_pos;
    QVector3D             m_end_pos;
    QColor                m_colour;
    Qt3DExtras::QCylinderMesh *m_cylinder;
    Qt3DCore::QTransform      *m_transform;
    Qt3DExtras::QDiffuseSpecularMaterial *m_material;
};
}

#endif
