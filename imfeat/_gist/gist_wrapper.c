#include <stdint.h>
#include "gist.h"
#include "standalone_image.h"
#include "string.h"
#include "stdlib.h"

void color_gist_scaletab_wrap(uint8_t *data, int height, int width, int nblocks, int n_scale, const int *orientations_per_scale, float *desc, int desc_size) {
    color_image_t *im=color_image_new(width, height);
    int i, size = height * width;
    for (i = 0; i < size; ++i) {
        im->c1[i] = *(data++);
        im->c2[i] = *(data++);
        im->c3[i] = *(data++);
    }

    float *desc_out = color_gist_scaletab(im, nblocks, n_scale, orientations_per_scale);

    memcpy(desc, desc_out, desc_size * sizeof(float));
    free(desc_out);
    color_image_delete(im);
}
