#include "bovw_aux.h"

void bovw_fast_hist(int32_t *neighbor_map, int32_t *bovw, const int height, const int width, const int bins, const int level) {
    int i, j;
    int bin_i, bin_j;
    const double height_inv = 1. / height;
    const double width_inv = 1. / width;
    const int level_bins = 1 << level;
    for (i = 0; i < height; ++i) {
        bin_i = i * height_inv * level_bins;
        for (j = 0; j < width; ++j) {
            bin_j = j * width_inv * level_bins;
            ++bovw[(bin_i * level_bins + bin_j) * bins + neighbor_map[i * width + j]];
        }
    }
}


void bovw_fast_sum(int32_t *bovw_fine, int32_t *bovw_coarse, const int height_fine, const int width_fine, const int bins) {
    int i, j, k;
    const int width_coarse = width_fine / 2;
    for (i = 0; i < height_fine; ++i)
        for (j = 0; j < width_fine; ++j)
            for (k = 0; k < bins; ++k)
                bovw_coarse[(i / 2 * width_coarse + j / 2) * bins + k] += bovw_fine[(i * width_fine + j) * bins + k];
}
