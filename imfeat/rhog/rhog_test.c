#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <stdio.h>
#include "rhog.h"

double * alloc_block_bins(int celly, int cellx, int block_diameter, int orientation_bins) {
  return malloc((sizeof(double)) * (celly - block_diameter + 1) * (cellx - block_diameter + 1) * block_diameter * block_diameter * orientation_bins);
}

void compute_cells(int width, int height, int cell_diameter, int *cellx, int *celly) {
  *cellx = (width - 2) / cell_diameter;
  *celly = (height - 2) / cell_diameter;
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

void image_test(char *image_path) {
  IplImage *im = cvLoadImage(image_path, CV_LOAD_IMAGE_GRAYSCALE);
  unsigned char *image = compact_gray_ipl(im);
  int cell_diameter = 6, block_diameter = 3, orientation_bins = 9, cellx, celly;
  double *block_bins;
  compute_cells(im->width, im->height, cell_diameter, &cellx, &celly);
  block_bins = alloc_block_bins(celly, cellx, block_diameter, orientation_bins);
  print_ipl(im);
  compute_hog(image, im->height, im->width, block_bins, cell_diameter, block_diameter, orientation_bins);
  cvReleaseImage(&im);
  free(image);
}

int main() {
  image_test("horz_grad.png");
  return 0;
}
