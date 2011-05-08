#ifndef AUTOCORRELOGRAM_H
#define AUTOCORRELOGRAM_H
#ifdef __cplusplus
extern "C" {
#endif
    void autocorrelogram(unsigned char *data, int height, int width, int unique_colors,
                         int *distance_set, int distance_set_size, double *ac_out, int ac_out_size);
    void convert_colors_rg16(unsigned char *data, int size, unsigned char *out);
    void convert_colors_rg64(unsigned char *data, int size, unsigned char *out);
#ifdef __cplusplus
}
#endif
#endif
