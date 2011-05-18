#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "rhog_aux.h"

static void compute_derivatives(unsigned char *image, int height, int width,
			 double *dy, double *dx) {
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

static inline double compute_orientation_bin(double dy, double dx, double bin_width) {
  return fabs(atan2(dy, dx)) / bin_width;
}

static inline double compute_gradient_magnitude(double dy, double dx) {
  return sqrt(dy * dy + dx * dx);
}

static inline void update_cell_bins(double *cell_bins, double bin, double mag,
			     int orientation_bins) {
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
    if (bin_ceil + 1 >= orientation_bins) {
      b1 = 0;
    } else {
      b1 = bin_floor + 1;
    }
  }
  cell_bins[b0] += m0 * mag;
  cell_bins[b1] += m1 * mag;
}

static void compute_cells(double *dy, double *dx, int height, int width,
		   double *cell_bins, int celly, int cellx,
		   int cell_diameter, int orientation_bins) {
  int i, j, k, l;
  double bin_width = 3.1415926535897931 / orientation_bins;
  for (i = 0; i < celly; ++i)
    for (j = 0; j < cellx; ++j) {
      int pixel_origin = (i * width + j) * cell_diameter + width + 1;
      for (k = 0; k < cell_diameter; ++k)
	for (l = 0; l < cell_diameter; ++l) {
	  double bin = compute_orientation_bin(dy[pixel_origin + width * k + l],
					    dx[pixel_origin + width * k + l],
					    bin_width);
	  // Returned bin is [0, orient] and we make it [0, orient)
	  if (bin >= orientation_bins)
	    bin = 0;
	  double mag = compute_gradient_magnitude(dy[pixel_origin + width * k + l],
						  dx[pixel_origin + width * k + l]);
	  update_cell_bins(cell_bins + (i * cellx + j) * orientation_bins, bin, mag,
	  		   orientation_bins);
	}
    }
}

static void compute_blocks(double *cell_bins, int celly, int cellx,
		    double *block_bins, int block_diameter,
		    int orientation_bins) {
  int i, j, k, l, m;
  double eps = 1.;
  int celly_up = celly - block_diameter;
  int cellx_up = cellx - block_diameter;
  int block_start, next_block = 0;
  double sum_sqr, norm_val;
  for (i = 0; i <= celly_up; ++i) {
    for (j = 0; j <= cellx_up; ++j) {
      block_start = next_block;
      sum_sqr = eps;
      for (k = 0; k < block_diameter; ++k)
	for (l = 0; l < block_diameter; ++l) {
	  for (m = 0; m < orientation_bins; ++m) {
	    double cell_val = cell_bins[((i + k) * cellx + (j + l)) * orientation_bins + m];
	    sum_sqr += cell_val * cell_val;
	    block_bins[next_block++] = cell_val;
	  }
	}
      norm_val = sqrt(sum_sqr);
      for (k = block_start; k < next_block; ++k)
	block_bins[k] /= norm_val;
    }
  }
}


void compute_hog(unsigned char *image, int height, int width, double *block_bins, int cell_diameter, int block_diameter, int orientation_bins) {
  /*
  In the paper the pedestrian defaults are
  cell_diameter = 6
  block_diameter = 3
  orientation_bins = 9
 */
  // TODO Allow signed orientation angles
  // TODO Add gaussian smoothing of blocks
  int size = height * width;
  
  double *dx, *dy;
  double *cell_bins; // [y][x][bin]
  int cellx = (width - 2) / cell_diameter;
  int celly = (height - 2) / cell_diameter;
  dy = malloc((sizeof *dy) * size);
  dx = malloc((sizeof *dx) * size);
  cell_bins = calloc(celly * cellx * orientation_bins, sizeof *cell_bins);
  compute_derivatives(image, height, width, dy, dx);
  compute_cells(dy, dx, height, width, cell_bins, celly, cellx, cell_diameter, orientation_bins);
  compute_blocks(cell_bins, celly, cellx, block_bins, block_diameter, orientation_bins);
  free(cell_bins);
  free(dx);
  free(dy);
}

static void max_derivatives(double *dyr, double *dxr, double *dyg, double *dxg, double *dyb, double *dxb, int height, int width) {
  int i, j;
  int height_m1 = height - 1;
  int width_m1 = width - 1;
  double mag_r, mag_g, mag_b;
  for (i = 1; i < height_m1; ++i)
    for (j = 1; j < width_m1; ++j) {
      int ind = i * width + j;
      mag_r = dyr[ind] * dyr[ind] + dxr[ind] * dxr[ind];
      mag_g = dyg[ind] * dyg[ind] + dxg[ind] * dxg[ind];
      mag_b = dyb[ind] * dyb[ind] + dxb[ind] * dxb[ind];
      if (mag_g >= mag_b && mag_g > mag_r) {
	dyr[ind] = dyg[ind];
	dxr[ind] = dxg[ind];
      } else if (mag_b >= mag_g && mag_b > mag_r) {
	dyr[ind] = dyb[ind];
	dxr[ind] = dxb[ind];
      }
    }
}

void compute_hog_rgb(unsigned char *imager, unsigned char *imageg, unsigned char *imageb, int height, int width, double *block_bins, int cell_diameter, int block_diameter, int orientation_bins) {
  /*
  In the paper the pedestrian defaults are
  cell_diameter = 6
  block_diameter = 3
  orientation_bins = 9
 */
  // TODO Allow signed orientation angles
  // TODO Add gaussian smoothing of blocks
  int size = height * width;
  
  double *dxr, *dyr, *dxg, *dyg, *dxb, *dyb;
  double *cell_bins; // [y][x][bin]
  int cellx = (width - 2) / cell_diameter;
  int celly = (height - 2) / cell_diameter;
  dyr = malloc((sizeof *dyr) * size);
  dxr = malloc((sizeof *dxr) * size);
  dyg = malloc((sizeof *dyg) * size);
  dxg = malloc((sizeof *dxg) * size);
  dyb = malloc((sizeof *dyb) * size);
  dxb = malloc((sizeof *dxb) * size);
  cell_bins = calloc(celly * cellx * orientation_bins, sizeof *cell_bins);
  compute_derivatives(imager, height, width, dyr, dxr);
  compute_derivatives(imageg, height, width, dyg, dxg);
  compute_derivatives(imageb, height, width, dyb, dxb);
  max_derivatives(dyr, dxr, dyg, dxg, dyb, dxb, height, width);
  compute_cells(dyr, dxr, height, width, cell_bins, celly, cellx, cell_diameter, orientation_bins);
  compute_blocks(cell_bins, celly, cellx, block_bins, block_diameter, orientation_bins);
  free(cell_bins);
  free(dyr);
  free(dxr);
  free(dyg);
  free(dxg);
  free(dyb);
  free(dxb);
}



/* Definitions
   Cell: Region over which a 1D hist of gradient orientations is collected (8x8)
   Block: A collection of neighboring cells that are used for contrast normalization (16x16)
   Diff Filter: [-1 0 1]
   Orientation Bins: 9 in 0 to pi
*/
