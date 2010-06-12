#include "Histogram.hpp"
Histogram::Histogram(int channels, int num_pixels, int num_bins) : channels(channels), num_pixels(num_pixels), num_bins(num_bins) {
}

std::vector<int> Histogram::compute(char *data) {
  return compute((unsigned char *)data);
}

std::vector<int> Histogram::compute(unsigned char *data) {
  std::vector<int> bins(num_bins * channels, 0);
  int bin_width = 256 / num_bins;
  int cur_channel = 0;
  int total_pixels = num_pixels * channels;
  for (int i = 0; i < total_pixels; i++) {
    ++bins[cur_channel * num_bins + data[i] / bin_width];
    if (++cur_channel >= channels)
      cur_channel = 0;
  }
  return bins;
}
