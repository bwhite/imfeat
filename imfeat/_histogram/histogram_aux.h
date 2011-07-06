#ifndef HISTOGRAM_AUX
#define HISTOGRAM_AUX
#include <stdint.h>
void histogram_joint_fast(float *data, int height, int width, float *min_vals, float *bin_width, int32_t *num_bins, int32_t *hist);
void histogram_fast(float *data, int height, int width, float *min_vals, float *bin_width, int32_t *num_bins, int32_t *hist);
void histogram_gray(float *data, int height, int width, float min_val, float bin_width, int32_t num_bins, int32_t *hist);
#endif
