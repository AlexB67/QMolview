#include "mol_utils.h"
#include <iostream>
#include <sstream>
#include <algorithm>

// Build a chemical formulaaar in html markup format
// From a vector of elements
std::string MolUtils::build_formula_string(const std::vector<std::string>& formula_vector)
{
    std::vector<std::string> tmp;

    for (size_t i = 0; i < formula_vector.size(); ++i)
    {
        auto cnt = std::count(formula_vector.begin(), formula_vector.end(), formula_vector.at(i));

        if (i == 0)
        {
            (cnt > 1) ? tmp.push_back(formula_vector.at(i) + std::to_string(cnt))
                      : tmp.push_back(formula_vector.at(i));
        }
        else
        {
            bool found{false};
            for (const auto &f : tmp)
            {
                if (cnt > 1)
                {
                    if (f == formula_vector.at(i) + std::to_string(cnt)) found = true;
                }
                else
                {
                    if (f == formula_vector.at(i)) found = true;
                }
            }

            if (!found)
            {
                (cnt > 1) ? tmp.push_back(formula_vector.at(i) + std::to_string(cnt))
                          : tmp.push_back(formula_vector.at(i));
            }
        }
    }

    std::string formula_string{""};
    for (const auto &f : tmp) // Build a markup string
    {
        if (const auto pos = f.find_first_of("0123456789") != std::string::npos)
            formula_string += f.substr(0, pos) + "<big><sub>" + f.substr(pos, f.length()) + "</sub></big>";
        else
            formula_string += f;
    }

    return formula_string;
}


float MolUtils::get_max_extent(QVector<QVector<QVector3D>>& coords, int natoms, int frame_num)
{
    float xdist_max = std::numeric_limits<float>::min();
    float ydist_max = std::numeric_limits<float>::min();
    float zdist_max = std::numeric_limits<float>::min();

    for (int i = 0; i < natoms; ++i)
        if (std::fabs(coords[frame_num][i][0]) > xdist_max)
            xdist_max = std::fabs(coords[frame_num][i][0]);

    for (int i = 0; i < natoms; ++i)
        if (std::fabs(coords[frame_num][i][1]) > ydist_max)
            ydist_max = std::fabs(coords[frame_num][i][1]);

    for (int i = 0; i < natoms; ++i)
        if (std::fabs(coords[frame_num][i][2]) > zdist_max)
            zdist_max = std::fabs(coords[frame_num][i][2]);

    return 2.0f * std::sqrt(xdist_max * xdist_max +
                            ydist_max * ydist_max +
                            zdist_max * zdist_max);
}

// Realgn a molecule along the principal axes
void MolUtils::align_axes(QVector<QVector<QVector3D>>& coords, const QVector<size_t>& zvals, int num_frames, int frame)
{
    std::array<float, 6> i_tensor;
    i_tensor.fill(0);

    for(int i = 0; i < zvals.size(); ++i)
    {
        float m_i = static_cast<float>(periodic_table::masses[zvals[i]]);
        i_tensor[0] += m_i * (coords[frame][i][1] * coords[frame][i][1] + coords[frame][i][2] * coords[frame][i][2]);
        i_tensor[3] += m_i * (coords[frame][i][0] * coords[frame][i][0] + coords[frame][i][2] * coords[frame][i][2]);
        i_tensor[5] += m_i * (coords[frame][i][0] * coords[frame][i][0] + coords[frame][i][1] * coords[frame][i][1]);
        i_tensor[1] -= m_i * coords[frame][i][0] * coords[frame][i][1];
        i_tensor[2] -= m_i * coords[frame][i][0] * coords[frame][i][2];
        i_tensor[4] -= m_i * coords[frame][i][1] * coords[frame][i][2];
    }

    std::array<std::array<float, 3>, 3> evecs;
    std::array<float, 3> evals;
    MathUtils::jacobi<float>(i_tensor, evals, evecs, 0.0001f);

    // Make sure we have the correct axes convention ordering
    size_t m = static_cast<size_t>(((((evals[0] < evals[1])  << 1) |
                                      (evals[1] < evals[2])) << 1) |
                                      (evals[2] < evals[0]));

    constexpr std::array<size_t, 7> min = {0, 2, 1, 1, 0, 2, 0};
    constexpr std::array<size_t, 7> mid = {1, 1, 0, 2, 2, 0, 1};
    constexpr std::array<size_t, 7> max = {2, 0, 2, 0, 1, 1, 2};
    std::array<size_t, 3> index = {min[m], mid[m], max[m]};
    std::array<float, 3> evals_new;
    std::array<std::array<float, 3>, 3> new_evecs;

    // update eigen vectors eigenvalues per new convention
    for(size_t i = 0; i < 3; ++i)
    {
        evals_new[i] = evals[index[i]];
        for(size_t j = 0; j < 3; ++j)
            new_evecs[i][j] = evecs[j][index[i]];
    }

    auto tmp_coords = coords;
    // Realign coordinates to principal axes
    for (int n = 0; n < num_frames; ++n)
        for (int l = 0; l < zvals.size(); ++l)
        {
            size_t j_ = 0;
            for (int j = 0; j < 3; ++j)
            {
                tmp_coords[n][l][j] = 0;
                size_t k_ = 0;
                for (int k = 0; k < 3; ++k)
                {   // NB: inverse_evecs = evecs.trranspose()
                    tmp_coords[n][l][j] += new_evecs[j_][k_] * coords[n][l][k];
                    ++k_;
                }

                ++j_;
            }
        }

    coords = tmp_coords;
}

void MolUtils::align_axes(QVector<QVector<QVector3D>>& coords, QVector<QVector<QVector3D>>& normal_modes, const QVector<size_t>& zvals)
{
    std::array<float, 6> i_tensor;
    i_tensor.fill(0);

    for(int i = 0; i < zvals.size(); ++i)
    {
        float m_i = static_cast<float>(periodic_table::masses[zvals[i]]);
        i_tensor[0] += m_i * (coords[0][i][1] * coords[0][i][1] + coords[0][i][2] * coords[0][i][2]);
        i_tensor[3] += m_i * (coords[0][i][0] * coords[0][i][0] + coords[0][i][2] * coords[0][i][2]);
        i_tensor[5] += m_i * (coords[0][i][0] * coords[0][i][0] + coords[0][i][1] * coords[0][i][1]);
        i_tensor[1] -= m_i * coords[0][i][0] * coords[0][i][1];
        i_tensor[2] -= m_i * coords[0][i][0] * coords[0][i][2];
        i_tensor[4] -= m_i * coords[0][i][1] * coords[0][i][2];
    }

    std::array<std::array<float, 3>, 3> evecs;
    std::array<float, 3> evals;
    MathUtils::jacobi<float>(i_tensor, evals, evecs, 0.0001f);

    // Make sure we have the correct axes convention ordering
    size_t m = static_cast<size_t>(((((evals[0] < evals[1])  << 1) |
                                      (evals[1] < evals[2])) << 1) |
                                      (evals[2] < evals[0]));

    constexpr std::array<size_t, 7> min = {0, 2, 1, 1, 0, 2, 0};
    constexpr std::array<size_t, 7> mid = {1, 1, 0, 2, 2, 0, 1};
    constexpr std::array<size_t, 7> max = {2, 0, 2, 0, 1, 1, 2};
    std::array<size_t, 3> index = {min[m], mid[m], max[m]};
    std::array<float, 3> evals_new;
    std::array<std::array<float, 3>, 3> new_evecs;

    // update eigen vectors eigenvalues per new convention
    for(size_t i = 0; i < 3; ++i)
    {
        evals_new[i] = evals[index[i]];
        for(size_t j = 0; j < 3; ++j)
            new_evecs[i][j] = evecs[j][index[i]];
    }

    auto tmp_coords = coords;
    // Realign coordinates to principal axes
    for (int l = 0; l < zvals.size(); ++l)
    {
        size_t j_ = 0;
        for (int j = 0; j < 3; ++j)
        {
            tmp_coords[0][l][j] = 0;
            size_t k_ = 0;
            for (int k = 0; k < 3; ++k)
            {   // NB: inverse_evecs = evecs.trranspose()
                tmp_coords[0][l][j] += new_evecs[j_][k_] * coords[0][l][k];
                ++k_;
            }

            ++j_;
        }
    }

    coords = tmp_coords;

    auto tmp_modes = normal_modes;

    for (int n = 0; n < normal_modes.size(); ++n)
        for (int l = 0; l < zvals.size(); ++l)
        {
            size_t j_ = 0;
            for (int j = 0; j < 3; ++j)
            {
                tmp_modes[n][l][j] = 0;
                size_t k_ = 0;
                for (int k = 0; k < 3; ++k)
                {   // NB: inverse_evecs = evecs.trranspose()
                    tmp_modes[n][l][j] += new_evecs[j_][k_] * normal_modes[n][l][k];
                    ++k_;
                }

                ++j_;
            }
        }

    normal_modes = tmp_modes;

}

QVector<int> MolUtils::sort_axes(QVector<QVector<QVector3D>>& coords,
                                 QVector<QVector<QVector3D>>& normal_modes,
                                 int natoms, int num_frames, int frame)
{
    using p = std::pair<float, int>;

    QVector<float> dist_max = QVector<float>(3, std::numeric_limits<float>::min());
    QVector<float> dist_min = QVector<float>(3, std::numeric_limits<float>::max());

    // with respect to frame
    for (int i = 0; i < natoms; ++i)
        for (int j = 0; j < 3; ++j)
        {
            if (coords[frame][i][j] > dist_max[j]) dist_max[j] = coords[frame][i][j];
            if (coords[frame][i][j] < dist_min[j]) dist_min[j] = coords[frame][i][j];
        }

    QVector<std::pair<float, int>> dist = QVector<std::pair<float, int>>(3);

    for (int i = 0; i < 3; ++i)
        dist[i] = std::make_pair(dist_max[i] - dist_min[i], i);

    std::sort(dist.data(), dist.data() + dist.size(),
              [](const p& p1, const p& p2) -> bool
    {
        return p1.first >= p2.first;
    });

    QVector<int> idx = QVector<int>(3);
    for (int i = 0; i < 3; ++i) idx[i] = dist[i].second;
    // sort all frames
    auto sorted_geom = coords;
    for (int i = 0; i < num_frames; ++i)
        for (int j = 0; j < natoms; ++j)
            for (int k = 0; k < 3; ++k)
                sorted_geom[i][j][k] = coords[i][j][idx[k]];

    coords = sorted_geom;

    if (!normal_modes.size()) return idx;

    // sort all modes
    auto sorted_modes = normal_modes;
    for (int i = 0; i < normal_modes.size(); ++i)
        for (int j = 0; j < natoms; ++j)
            for (int k = 0; k < 3; ++k)
                sorted_modes[i][j][k] = normal_modes[i][j][idx[k]];

    normal_modes = sorted_modes;

    return idx;
}

QVector<int> MolUtils::sort_axes(QVector<QVector<QVector3D>>& coords, int natoms, int num_frames, int frame)
{
    QVector<QVector<QVector3D>> normal_modes = QVector<QVector<QVector3D>>(0);
    return sort_axes(coords, normal_modes, natoms, num_frames, frame);
}

bool MolUtils::check_is_float(const std::string val, const std::string line)
{
    if (val.empty())
    {
        std::clog << "\n\n  Error: empty number on line containing:\n"
                              << "  " << line << "\n\n";
        return false;
    }
    else if(val.find_first_not_of("+-0123456789.Ee") != std::string::npos)
    {
        std::clog << "\n\n  Error: Bad float format on line containing:\n"
                              << "  " << line << "\n\n";
        return false;
    }

    // we can still fall through when the above is not robust enough
    std::stringstream isbad(val);
    float isfloat;
    isbad >> isfloat;

    if(isbad.good())
    {
        std::clog << "\n\n  Error: Invalid number on line containing:\n"
                              << "  " << line << "\n\n";
        return false;
    }

    return true;
}
