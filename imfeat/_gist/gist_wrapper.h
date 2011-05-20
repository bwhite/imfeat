#ifndef GIST_WRAPPER_H
#define GIST_WRAPPER_H
#include <stdint.h>
void color_gist_scaletab_wrap(uint8_t *data, int height, int width, int nblocks, int n_scale, const int *orientations_per_scale, float *desc, int desc_size);
#endif
