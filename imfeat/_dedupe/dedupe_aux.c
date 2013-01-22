#include "dedupe_aux.h"
#include "assert.h"

void normalize_hist(uint32_t *hist, double *hist_out, int offset, int bins) {
    uint32_t sum = 0;
    double sum_recip;
    int i;
    assert((offset + bins) <= 384);
    hist += offset;
    hist_out += offset;
    for (i = 0; i < bins; ++i)
        sum += hist[i];
    sum_recip = 1. / sum;
    for (i = 0; i < bins; ++i)
        hist_out[i] = hist[i] * sum_recip;
}

void dedupe_image_to_feat(uint8_t *image_lab, double *hist_out, const int height, const int width) {
    int i, j, y, x, y_4, y_2, x_4, x_2;
    uint8_t l, a , b;
    assert(height >= 4 && width >= 4);
    const int height_by_4 = height / 4;
    const int width_by_4 = width / 4;
    const int height_even_4_m1 = height_by_4 * 4 - 1;
    const int width_even_4_m1 = width_by_4 * 4 - 1;
    uint32_t hist[384] = {};
    for (i = 0; i < height; ++i) {
        for (j = 0; j < width; ++j, image_lab += 3) {
            // Convert color space to [0, 255)
            // http://opencv.willowgarage.com/documentation/cpp/miscellaneous_image_transformations.html#cv-cvtcolor
            l = image_lab[0];
            a = image_lab[1];
            b = image_lab[2];
            /* 128 bytes per level (one bin per byte, intensity has twice the resolution as O1 and O2)
               NOTE(brandyn): I am assuming they compute the histogram planar on each channel independently
               and concatenate as opposed to jointly.
            */
            y_4 = i <= height_even_4_m1 ? i / height_by_4 : 3;
            x_4 = j <= width_even_4_m1 ? j / width_by_4 : 3;
            y_2 = y_4 / 2;
            x_2 = x_4 / 2;
            // hist[channel][level][y][x][bin]
            // Channel L: 64 * 1 * 1 + 16 * 2 * 2 + 4 * 4 * 4 = 64 + 64 + 64 = 192 slots
            ++hist[0 + 0 + 0 + l / (256 / 64)];
            ++hist[0 + 64 + (y_2 * 2 + x_2) * 16 + l / (256 / 16)];
            ++hist[0 + 128 + (y_4 * 4 + x_4) * 4 + l / (256 / 4)];

            // Channel A: 32 * 1 * 1 + 8 * 2 * 2 + 2 * 4 * 4 = 32 + 32 + 32 = 96 slots
            ++hist[192 + 0 + 0 + a / (256 / 32)];
            ++hist[192 + 32 + (y_2 * 2 + x_2) * 8 + a / (256 / 8)];
            ++hist[192 + 64 + (y_4 * 4 + x_4) * 2 + a / (256 / 2)];

            // Channel B: 32 * 1 * 1 + 8 * 2 * 2 + 2 * 4 * 4 = 32 + 32 + 32 = 96 slots
            ++hist[288 + 0 + 0 + b / (256 / 32)];
            ++hist[288 + 32 + (y_2 * 2 + x_2) * 8 + b / (256 / 8)];
            ++hist[288 + 64 + (y_4 * 4 + x_4) * 2 + b / (256 / 2)];
        }
    }
    // L1 Normalize each of the 3 * (1 + 4 + 16) = 63 histograms
    normalize_hist(hist, hist_out, 0, 64); // L0
    normalize_hist(hist, hist_out, 192, 32); // A0
    normalize_hist(hist, hist_out, 288, 32); // B0
    for (y = 0; y < 2; ++y)
        for (x = 0; x < 2; ++x) {
            normalize_hist(hist, hist_out, 0 + 64 + (y * 2 + x) * 16, 16); // L1
            normalize_hist(hist, hist_out, 192 + 32 + (y * 2 + x) * 8, 8); // A1
            normalize_hist(hist, hist_out, 288 + 32 + (y * 2 + x) * 8, 8); // B1
        }
    for (y = 0; y < 4; ++y)
        for (x = 0; x < 4; ++x) {
            normalize_hist(hist, hist_out, 0 + 128 + (y * 4 + x) * 4, 4); // L2
            normalize_hist(hist, hist_out, 192 + 64 + (y * 4 + x) * 2, 2); // A2
            normalize_hist(hist, hist_out, 288 + 64 + (y * 4 + x) * 2, 2); // B2
        }
}
