#ifndef DEDUPE_AUX_H
#define DEDUPE_AUX_H
#include <stdint.h>
void dedupe_image_to_feat(uint8_t *image_lab, double *hist_out, const int height, const int width);
#endif
