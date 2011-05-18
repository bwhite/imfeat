#ifdef __cplusplus
extern "C" {
#endif
  int compute_surf_descriptors(char *data, int height, int width, int max_points, float *points);
  int compute_surf_points(char *data, int height, int width, int max_points, float *points, int *x, int *y, int *scale, float *orientation, char *sign, float *cornerness);
  int compute_surf_random(char *data, int height, int width, int max_points, float *points);
#ifdef __cplusplus
}
#endif
