#include "histogram_aux.h"
void histogram_joint_fast(float *data, int height, int width, float *min_vals, float *bin_width, int32_t *num_bins, int32_t *hist) {
    int i, j, ind, total_ind;
    int size = height * width * 3;
    for (i = 0; i < size; i += 3, data += 3) {
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
        ++hist[total_ind];
    }
}

void histogram_fast(float *data, int height, int width, float *min_vals, float *bin_width, int32_t *num_bins, int32_t *hist) {
    int i, j, ind, total_ind;
    int size = height * width * 3;
    for (i = 0; i < size; i += 3, data += 3) {
        total_ind = 0;
        for (j = 0; j < 3; ++j) {
            ind = (data[j] - min_vals[j]) / bin_width[j];
            if (ind < 0)
                ind = 0;
            else if (ind >= num_bins[j])
                ind = num_bins[j] - 1;
            ++hist[total_ind + ind];
            total_ind += num_bins[j];
        }
    }
}

void histogram_gray(float *data, int height, int width, float min_val, float bin_width, int32_t num_bins, int32_t *hist) {
    int i, ind;
    int size = height * width;
    for (i = 0; i < size; i++) {
        ind = (data[i] - min_val) / bin_width;
        if (ind < 0)
            ind = 0;
        else if (ind >= num_bins)
            ind = num_bins - 1;
        ++hist[ind];
    }
}
