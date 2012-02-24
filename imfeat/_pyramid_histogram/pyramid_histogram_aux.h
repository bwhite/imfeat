#ifndef HISTOGRAM_AUX
#define HISTOGRAM_AUX
#include <stdint.h>
void image_to_bin_map(float *data, int height, int width, float *min_vals, float *bin_width, int32_t *num_bins, int32_t *bin_map);
#endif
