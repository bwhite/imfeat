#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <opencv/highgui.h>
#include <opencv/cv.h>

void compute_derivatives(char *image, int height, int width, double *dy,
			 double *dx) {
  int i, j;
  int height_m1 = height - 1;
  int width_m1 = width - 1;
  for (i = 1; i < height_m1; ++i)
    for (j = 1; j < width_m1; ++j) {
      int ind = i * width + j;
      dy[ind] = image[ind + width] - image[ind - width];
      dx[ind] = image[ind + 1] - image[ind - 1];
    }
}

inline int compute_orientation_bin(double dy, double dx, int orientation_bins) {
  const double offset = 1.5707963267948966;
  double bin_width = 3.1415926535897931 / orientation_bins;
  return (int)((atan(dy/dx) + offset) / bin_width);
}

inline double compute_gradient_magnitude(double dy, double dx) {
  return sqrt(dy*dy + dx*dx);
}

void compute_cells(double *dy, double *dx, int height, int width, double *cell_bins,
		   int celly, int cellx, int cell_diameter,
		   int orientation_bins) {
  int i, j, k, l;
  int cell_cnt = 0;
  for (i = 0; i < celly; ++i)
    for (j = 0; j < cellx; ++j, ++cell_cnt) {
      int pixel_origin = (i * width + j) * cell_diameter + width + 1;
      for (k = 0; k < cell_diameter; ++k)
	for (l = 0; l < cell_diameter; ++l) {
	  int bin = compute_orientation_bin(dy[pixel_origin + width * k + l],
					    dx[pixel_origin + width * k + l],
					    orientation_bins);
	  double mag = compute_gradient_magnitude(dy[pixel_origin + width * k + l],
						  dx[pixel_origin + width * k + l]);
	  if (i == 1 && j == 1)
	    printf("Mag:%f\n", mag);
	  cell_bins[(i * cellx + j) * orientation_bins + bin] += mag;
	}
    }
}

void compute_hog(char *image, int height, int width, double *desc) {
  int size = height * width;
  int cell_diameter = 6;
  int block_diameter = 3;
  int orientation_bins = 9;
  double *dx, *dy;
  double *cell_bins; // [y][x][bin]
  int cellx = (width - 2) / cell_diameter;
  int celly = (height - 2) / cell_diameter;
  int blockx = cellx / block_diameter;
  int blocky = celly / block_diameter;
  dy = malloc((sizeof *dy) * size);
  dx = malloc((sizeof *dx) * size);
  cell_bins = malloc((sizeof *cell_bins) * celly * cellx * orientation_bins);
  compute_derivatives(image, height, width, dy, dx);
  compute_cells(dy, dx, height, width, cell_bins, celly, cellx, cell_diameter, orientation_bins);

  free(dy);
  free(dx);
  free(cell_bins);
}

void experiment(int height, int width) {
  char *image;
  image = malloc((sizeof *image) * height * width);
  memset(image, 0, (sizeof *image) * height * width);
  compute_hog(image, height, width, 0);
  free(image);
}

int main() {
  int i, j;
  cvLoadImage("test.jpg", CV_LOAD_IMAGE_COLOR);
  for (i = 0; i < 50; i++)
    for (j = 0; j < 50; j++) {
      experiment(i, j);
    }
  return 0;
}

/* Definitions
   Cell: Region over which a 1D hist of gradient orientations is collected (8x8)
   Block: A collection of neighboring cells that are used for contrast normalization (16x16)
   Diff Filter: [-1 0 1]
   Orientation Bins: 9 in 0 to pi

TODO
Generate synthetic images
Load images and verify operation
Perform block normalization
*/
