#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <opencv/highgui.h>
#include <opencv/cv.h>

void compute_derivatives(unsigned char *image, int height, int width, double *dy,
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

inline double compute_orientation_bin(double dy, double dx, double bin_width) {
  printf("angle<%f,%f>\n", fabs(atan2(dy, dx)), atan2(dy, dx));
  return fabs(atan2(dy, dx)) / bin_width;
}

inline double compute_gradient_magnitude(double dy, double dx) {
  return sqrt(dy*dy + dx*dx);
}

inline void update_cell_bins(double *cell_bins, double bin, double mag, int orientation_bins) {
  int b0, b1;
  double m0, m1;
  int bin_floor = floor(bin);
  int bin_ceil = ceil(bin);
  double frac = bin - bin_floor;
  if (frac < .5) {
    b1 = bin_floor;
    m0 = .5 - frac;
    m1 = .5 + frac;
    if (bin_floor == 0) {
      b0 = orientation_bins - 1;
    } else {
      b0 = bin_floor - 1;
    }
  } else {
    b0 = bin_floor;
    m0 = 1.5 - frac;
    m1 = frac - .5;
    if (bin_ceil == orientation_bins) {
      b1 = 0;
    } else {
      b1 = bin_floor + 1;
    }
  }
  printf("b: %f b0: %d m0: %f b1: %d m1: %f\n", bin, b0, m0, b1, m1);
  cell_bins[b0] += m0 * mag;
  cell_bins[b1] += m1 * mag;
}

void compute_cells(double *dy, double *dx, int height, int width, double *cell_bins,
		   int celly, int cellx, int cell_diameter,
		   int orientation_bins) {
  int i, j, k, l;
  int cell_cnt = 0;
  double bin_width = 3.1415926535897931 / orientation_bins;
  for (i = 0; i < celly; ++i)
    for (j = 0; j < cellx; ++j, ++cell_cnt) {
      int pixel_origin = (i * width + j) * cell_diameter + width + 1;
      for (k = 0; k < cell_diameter; ++k)
	for (l = 0; l < cell_diameter; ++l) {
	  double bin = compute_orientation_bin(dy[pixel_origin + width * k + l],
					    dx[pixel_origin + width * k + l],
					    bin_width);
	  double mag = compute_gradient_magnitude(dy[pixel_origin + width * k + l],
						  dx[pixel_origin + width * k + l]);
	  update_cell_bins(cell_bins + (i * cellx + j) * orientation_bins, bin, mag, orientation_bins);
	}
    }
}

void compute_hog(unsigned char *image, int height, int width, double *desc) {
  // TODO Allow signed orientation angles
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
  printf("cellx: %d celly: %d blockx: %d blocky: %d num_bins: %d\n", cellx, celly, blockx, blocky, celly * cellx * orientation_bins);
  dy = malloc((sizeof *dy) * size);
  dx = malloc((sizeof *dx) * size);
  cell_bins = malloc((sizeof *cell_bins) * celly * cellx * orientation_bins);
  memset(cell_bins, 0, (sizeof *cell_bins) * celly * cellx * orientation_bins);
  compute_derivatives(image, height, width, dy, dx);
  compute_cells(dy, dx, height, width, cell_bins, celly, cellx, cell_diameter, orientation_bins);

  free(dy);
  free(dx);
  free(cell_bins);
}

void experiment(int height, int width) {
  unsigned char *image;
  image = malloc((sizeof *image) * height * width);
  memset(image, 0, (sizeof *image) * height * width);
  compute_hog(image, height, width, 0);
  free(image);
}

void print_ipl(IplImage *im) {
  printf("H:%d W:%d Ws:%d Chan:%d\n", im->height, im->width, im->widthStep, im->nChannels);
}

unsigned char* compact_gray_ipl(IplImage *im) {
  int i, j;
  unsigned char *image;
  image = malloc(sizeof *image * im->height * im->width);
  for (i = 0; i < im->height; ++i) {
    for (j = 0; j < im->width; ++j)
      image[i * im->width + j] = im->imageData[i * im->widthStep + j];
  }
  return image;
}

void image_test(char * image_path) {
  IplImage *im = cvLoadImage(image_path, CV_LOAD_IMAGE_GRAYSCALE);
  print_ipl(im);
  unsigned char *image = compact_gray_ipl(im);
  compute_hog(image, im->height, im->width, 0);
}

int main() {
  image_test("rand_grad.png");
  /*
  int i, j;
  for (i = 0; i < 25; i++)
    for (j = 0; j < 25; j++) {
      experiment(i, j);
    }
  */
  return 0;
}

/* Definitions
   Cell: Region over which a 1D hist of gradient orientations is collected (8x8)
   Block: A collection of neighboring cells that are used for contrast normalization (16x16)
   Diff Filter: [-1 0 1]
   Orientation Bins: 9 in 0 to pi

TODO
Perform block normalization
*/
