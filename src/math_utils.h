#ifndef MATH_UTILS_H
#define MATH_UTILS_H
#include <cmath>
#include <array>
#include <algorithm>

namespace MathUtils
{
// diag a 3x3 symmetric matrix
// Compute eigenvectors and eigenvalues
// Adapted from Numerical recipes / libmsym
template <typename T>
void jacobi(std::array<T, 6>& m, std::array<T, 3>& e, std::array<std::array<T, 3>, 3>& ev, const T threshold)
{
    T err = 1;
    e[0] = m[0];
    e[1] = m[3];
    e[2] = m[5];

    ev[0][1] = 0; ev[0][2] = 0; ev[1][2] = 0;
    ev[1][0] = 0; ev[2][0] = 0; ev[2][1] = 0;
    ev[0][0] = 1; ev[1][1] = 1; ev[2][2] = 1;

    while(err > 0)
    {
        err = 0.0;
        for(int od = 0; od < 3; od++)
        {
            int i = 1 << od, row = od >> 1, col = 1 + (od >> (od >> 1));
            T ami = std::fabs(m[i]), eps = ami / threshold;

            if(std::fabs(e[row]) + eps == std::fabs(e[row]) && std::fabs(e[col]) + eps == std::fabs(e[col]))
            {
                m[i] = 0.0;
            }
            else if(ami > 0.0)
            {
                err = static_cast<T>(std::fmax(ami, err));
                T d = e[col] - e[row];
                T t = static_cast<T>(std::copysign(2, d) * m[i] / (std::fabs(d) + std::sqrt(d * d + 4 * m[i] * m[i])));
                T c = static_cast<T>(1.0 / std::sqrt(1.0 + t * t));
                T s = c * t;

                e[row] -= t * m[i];
                e[col] += t * m[i];

                m[i] = 0.0;

                /* rotate eigenvectors */
                for (int k = 0; k < 3; k++)
                {
                    T evr = ev[k][row], evc = ev[k][col];
                    ev[k][row] = c * evr - s * evc;
                    ev[k][col] = s * evr + c * evc;
                }

                /* rotate index */
                int ix = col ^ 3, iy = 4 >> row;
                T mix = m[ix], miy = m[iy];
                m[ix] = c * mix - s * miy;
                m[iy] = s * mix + c * miy;
            }
        }
    }
}

// Invert a real symmetric 3x3  matrix
// no checks for overflow - we assume it can be inverted
template<typename T>
void inverse(const std::array<std::array<T, 3>, 3>& in_mat, std::array<std::array<T, 3>, 3>& inverse_out)
{
    T det1 = in_mat[1][1] * in_mat[2][2] - in_mat[2][1] * in_mat[1][2];
    T det2 = in_mat[1][0] * in_mat[2][2] - in_mat[1][2] * in_mat[2][0];
    T det3 = in_mat[1][0] * in_mat[2][1] - in_mat[1][1] * in_mat[2][0];
    T det = (in_mat[0][0] * det1 - in_mat[0][1] * det2 + in_mat[0][2] * det3);

    inverse_out[0][0] = det1 / det;
    inverse_out[0][1] = (in_mat[0][2] * in_mat[2][1] - in_mat[0][1] * in_mat[2][2]) / det;
    inverse_out[0][2] = (in_mat[0][1] * in_mat[1][2] - in_mat[0][2] * in_mat[1][1]) / det;
    inverse_out[1][0] = -det2 / det;
    inverse_out[1][1] = (in_mat[0][0] * in_mat[2][2] - in_mat[0][2] * in_mat[2][0]) / det;
    inverse_out[1][2] = (in_mat[1][0] * in_mat[0][2] - in_mat[0][0] * in_mat[1][2]) / det;
    inverse_out[2][0] = det3 / det;
    inverse_out[2][1] = (in_mat[2][0] * in_mat[0][1] - in_mat[0][0] * in_mat[2][1]) / det;
    inverse_out[2][2] = (in_mat[0][0] * in_mat[1][1] - in_mat[1][0] * in_mat[0][1]) / det;
}

}
#endif

