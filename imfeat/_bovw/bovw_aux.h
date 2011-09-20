#ifndef BOVW_AUX
#define BOVW_AUX
#include <stdint.h>
void bovw_fast_hist(int32_t *neighbor_map, int32_t *bovw, const int height, const int width, const int bins, const int level);
void bovw_fast_sum(int32_t *bovw_fine, int32_t *bovw_coarse, const int height_fine, const int width_fine, const int bins);
#endif
