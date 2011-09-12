#include "lbp_aux.h"

void make_lbp_mask_fast(uint8_t *data, const int height, const int width, uint8_t *out) {
    int i, j;
    uint8_t *r0 = data, *r1 = data + width, *r2 = data + 2 * width;
    const int height_m1 = height - 1, width_m1 = width - 1;
    if (height < 3 || width < 3)
        return;
    for (i = 1; i < height_m1; ++i, r0 += width, r1 += width, r2 += width)
        for (j = 1; j < width_m1; ++j) {
            const uint8_t v = r1[j];
            // Clockwise starting at top left corner
            out[i * width + j] = ((v > r0[j - 1]) << 0 | (v > r0[j + 0]) << 1 | (v > r0[j + 1]) << 2 |
                                  (v > r1[j + 1]) << 3 | (v > r2[j + 1]) << 4 | (v > r2[j + 0]) << 5 |
                                  (v > r2[j - 1]) << 6 | (v > r1[j - 1]) << 7);
        }
    // Pad the borders (without corners)
    for (j = 1; j < width_m1; ++j)  // Top
        out[j] = out[width + j];
    for (j = 1; j < width_m1; ++j)  // Bottom
        out[width * height_m1 + j] = out[(width * (height_m1 - 1)) + j];
    for (i = 1; i < height_m1; ++i)  // Left
        out[width * i] = out[width * i + 1];
    for (i = 1; i < height_m1; ++i)  // Right
        out[width * i + width_m1] = out[width * i + width_m1 - 1];
    out[0] = out[1];  // TL corner
    out[width_m1] = out[width_m1 - 1]; // TR corner
    out[height_m1 * width] = out[height_m1 * width + 1];  // BL corner
    out[height * width - 1] = out[height * width - 2];  // BR corner
}
