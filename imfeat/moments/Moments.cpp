#include <cmath>
#include <cstdlib>
#include "Moments.hpp"

Moments::Moments(int channels, int num_pixels, int num_moments) : channels(channels), num_pixels(num_pixels), num_moments(num_moments){
}
std::vector<double> Moments::compute(char * data) {
  return compute((unsigned char*) data);
}

std::vector<double> Moments::compute(unsigned char * data) {
  std::vector<double> moments(channels * num_moments, 0.0);
  int num_pixels_t_channels = num_pixels * channels;
    
  // Compute Means
  int mean_bin = 0;
  for (int k = 0; k < channels; k++) {
    for (int i = k; i < num_pixels_t_channels; i+= channels)
      moments[mean_bin] += data[i];
    moments[mean_bin] /= num_pixels;
    mean_bin += num_moments;
  }

  // Compute higher order moments
  for (int j = 1; j < num_moments; j++) {
    int moment_bin = j;
    mean_bin = 0;
    for (int k = 0; k < channels; k++) {
      for (int i = 0; i < num_pixels_t_channels; i+= channels)
	moments[moment_bin] += fast_pow(data[i + k] - moments[mean_bin], j + 1);
      moments[moment_bin] /= num_pixels;
      moments[moment_bin] = safe_root(moments[moment_bin], j+1.0);
      mean_bin += num_moments;
      moment_bin += num_moments;
    }
  }
  return moments;
}

double Moments::fast_pow(double val, int power) {
  double temp = 1.0;
  for (int i = 0; i < power; i++)
    temp *= val;
  return temp;
}

double Moments::safe_root(double val, int rpower) {
  double p = 1.0 / rpower;
  if (val >= 0)
    return pow(val, p);
  else if (rpower % 2 == 0)
    return pow(abs(val), p);
  return -pow(-val, p);
}

void compute(int channels, int num_pixels, int num_moments, char *data, double *moments) {
  Moments m(channels, num_pixels, num_moments);
  std::vector<double> v = m.compute(data);
  for (int i = 0; i < v.size(); i++)
    moments[i] = v[i];
}
