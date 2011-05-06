#ifndef RHOG_H
#define RHOG_H
void compute_hog(unsigned char *image, int height, int width, double *block_bins, int cell_diameter, int block_diameter, int orientation_bins);
void compute_hog_rgb(unsigned char *imager, unsigned char *imageg, unsigned char *imageb, int height, int width, double *block_bins, int cell_diameter, int block_diameter, int orientation_bins);
#endif
