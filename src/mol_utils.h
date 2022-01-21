#ifndef MOL_UTILS_H
#define MOL_UTILS_H

#include "molecule.h"
#include "math_utils.h"
#include <QVector>
#include <QVector3D>
#include <string>

// Would be perfect for C++20 modules but we are in 17 mode
namespace MolUtils
{
// ********************************************************************
// All functions below assume the molecule to be at the centre of mass.
// ********************************************************************

// Realgn a molecule along the principal axes w.r.t frame out of num_frames.
void align_axes(QVector<QVector<QVector3D>>& coords, const QVector<size_t>& zvals, int num_frames, int frame);

// Realgn a molecule along the principal axes as well as normal modes
void align_axes(QVector<QVector<QVector3D>>& coords, QVector<QVector<QVector3D>>& normal_modes, const QVector<size_t>& zvals);

// sort geometry and normal modes
// Convention is to sort axes such that DIM X > DIM Y > DIM Z
QVector<int> sort_axes(QVector<QVector<QVector3D>>& coords,
                       QVector<QVector<QVector3D>>& normal_modes,
                       int natoms, int num_frames, int frame);

// sort geometry
// Convention is to sort axes such that DIM X > DIM Y > DIM Z
QVector<int> sort_axes(QVector<QVector<QVector3D>>& coords, int natoms, int num_frames, int frame);


// Build a chemical formulaaar in html markup format
// From a vector of elements
std::string build_formula_string(const std::vector<std::string>& formula_vector);

// Get a rough estimate of the molecule largests dimension for the given frame number
// used to draw axes
float get_max_extent(QVector<QVector<QVector3D>>& coords, int natoms, int frame_num);

bool check_is_float(const std::string val, const std::string line);

}
#endif

