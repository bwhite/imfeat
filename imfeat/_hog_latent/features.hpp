#ifndef HOG_FEATURES_HPP
#define HOG_FEATURES_HPP
#ifdef __cplusplus
extern "C" {
#endif
    void process_feat_size(int height, int width, int sbin, int *out);
    void process(double *im_rowmajor, int height, int width, int sbin, double *feat, int feat_size);
#ifdef __cplusplus
}
#endif
#endif
