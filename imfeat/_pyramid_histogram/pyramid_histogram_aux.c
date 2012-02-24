#include "pyramid_histogram_aux.h"

void image_to_bin_map(float *data, int height, int width, float *min_vals, float *bin_width, int32_t *num_bins, int32_t *bin_map) {
    int i, j, ind, total_ind, bin_index;
    int size = height * width * 3;
    for (i = 0, bin_index = 0; i < size; i += 3, data += 3, ++bin_index) {
        total_ind = 0;
        for (j = 0; j < 3; ++j) {
            if (j > 0)
                total_ind *= num_bins[j - 1];
            ind = (data[j] - min_vals[j]) / bin_width[j];
            if (ind < 0)
                ind = 0;
            else if (ind >= num_bins[j])
                ind = num_bins[j] - 1;
            total_ind += ind;
        }
        bin_map[bin_index] = total_ind;
    }
}
