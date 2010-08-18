extern "C" {
  int compute_surf_descriptors(char *data, int height, int width, int max_points, float *points);
  int compute_surf_points(char *data, int height, int width, int max_points, float *points, int *x, int *y, int *scale, float *orientation, bool *sign, float *cornerness);
}
