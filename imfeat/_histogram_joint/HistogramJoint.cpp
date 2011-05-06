#include "HistogramJoint.hpp"
HistogramJoint::HistogramJoint(int num_pixels, int num_bins) : num_pixels(num_pixels), num_bins(num_bins) {
}

std::vector<int> HistogramJoint::compute(char *data) {
  return compute((unsigned char *)data);
}

#define at(x,y,z,bins) ((z*bins+y)*bins+x)

std::vector<int> HistogramJoint::compute(unsigned char *data) {
  std::vector<int> bins(num_bins * num_bins * num_bins, 0);
  int bin_width = 256 / num_bins;
  int total_pixels = num_pixels * 3;
  for (int i = 0; i < total_pixels; i+= 3)
    ++bins[at(data[i]/bin_width,
	      data[i+1]/bin_width,
	      data[i+2]/bin_width,
	      num_bins)];
  return bins;
}

void HistogramJoint::compute(unsigned char *data, int *bins) {
  int bin_width = 256 / num_bins;
  int total_pixels = num_pixels * 3;
  for (int i = 0; i < total_pixels; i+= 3)
    ++bins[at(data[i]/bin_width,
	      data[i+1]/bin_width,
	      data[i+2]/bin_width,
	      num_bins)];
}

void compute(int num_pixels, int num_bins, char *data, int *bins) {
  HistogramJoint hist(num_pixels, num_bins);
  hist.compute((unsigned char *)data, bins);
}
