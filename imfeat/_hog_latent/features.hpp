#ifndef HOG_FEATURES_HPP
#define HOG_FEATURES_HPP
#ifdef __cplusplus
extern "C" {
#endif
    void process_feat_size(int height, int width, int sbin, int *out);
    void process(double *im_rowmajor, int height, int width, int sbin, double *feat, int feat_size);
    void convert_cm_to_rm(double *cm, double *rm, const int height, const int width);
    void convert_rm_to_cm(double *rm, double *cm, const int height, const int width);
    void resize_size(int height, int width, int scale, int *ddims);
    void resize(double *im_rowmajor, int height, int width, double scale, double *dst_rowmajor, int dst_rowmajor_size);
#ifdef __cplusplus
}
#endif
#endif
