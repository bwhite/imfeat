/* From the paper "Image Indexing Using Color Correlograms"
 */

#include <cstring>
#include <cassert>
#include "Correlogram.hpp"

void convert_colors_rg16(unsigned char *data, int size, unsigned char *out) {
  unsigned char r, g;
  int val;
  for (int i = 0; i < size; i++) {
    val = 0;
    // Color is in BGR
    r = data[i*3+2];
    g = data[i*3+1];
    
    if (r <= 127)
      if (r <= 63)
	val = 12;
      else
	val = 8;
    else
      if (r <= 191)
	val = 4;

    if (g <= 127)
      if (g <= 63)
	val += 3;
      else
	val += 2;
    else
      if (g <= 191)
	val += 1;
    out[i] = val;
  }
}

void convert_colors_rg64(unsigned char *data, int size, unsigned char *out) {
  unsigned char r, g, b;
  for (int i = 0; i < size; i++) {
    r = data[i*3+2] & 0xC0;
    g = data[i*3+1] & 0xC0;
    b = data[i*3] & 0xC0;
    
    out[i] = (r >> 2) + (g >> 4) + (b >> 6);
  }
}

Autocorrelogram::Autocorrelogram(int height, int width, int unique_colors, std::vector<int> distance_set): height(height), width(width), unique_colors(unique_colors), distance_set(distance_set), max_dist(distance_set.back()), max_height(height - distance_set.back()), max_width(width - distance_set.back()) {
  lambda_x = new unsigned int[height * width];
  lambda_y = new unsigned int[height * width];
  hist = new unsigned int[unique_colors];
}

Autocorrelogram::~Autocorrelogram() {
  delete [] lambda_x;
  delete [] lambda_y;
  delete [] hist;
}

void Autocorrelogram::build_hist(const unsigned char *data) {
  memset(hist, 0, unique_colors * sizeof(unsigned int));
  for (int i = max_dist; i < max_height; ++i)
    for (int j = max_dist; j < max_width; ++j)
      ++hist[data[i * width + j]];
}

void Autocorrelogram::clear() {
  int clear_bytes = height * width * sizeof(unsigned int);
  memset(lambda_x, 0, clear_bytes);
  memset(lambda_y, 0, clear_bytes);
}

void Autocorrelogram::update_lambda_x(const unsigned char *data, int k, unsigned char color) {
  for (int i = max_dist; i < max_height; ++i)
    for (int j = max_dist; j < max_width; ++j) {
      int off = i * width + j;
      assert(0 <= i && i < height);
      assert(0 <= j + k && j + k < width);
      if (data[off + k] == color)
	++lambda_x[off];
    }
}

void Autocorrelogram::update_lambda_y(const unsigned char *data, int k, unsigned char color) {
  for (int i = max_dist; i < max_height; ++i)
    for (int j = max_dist; j < max_width; ++j) {
      int off = i * width + j;
      assert(0 <= i + k && i + k < height);
      assert(0 <= j && j < width);
      if (data[off + k * width] == color)
	++lambda_y[off];
    }
}

bool Autocorrelogram::is_dist_in_set(int k) {
  int dist_set_size = distance_set.size();
  for (int i = 0; i < dist_set_size; ++i)
    if (distance_set[i] == k)
      return true;
  return false;
}

double Autocorrelogram::make_correlogram_scalar(const unsigned char *data, int k, unsigned char color) {
  double scalar = 0.0;
  int off0 = -k + k * width;
  int off1 = -k - k * width;
  int off2 = -k + (-k+1) * width;
  int off3 = k + (-k+1) * width;
  for (int i = max_dist; i < max_height; ++i)
    for (int j = max_dist; j < max_width; ++j) {
      int off = i * width + j;
      if (data[off] == color)
	scalar += lambda_x[off + off0] + lambda_x[off + off1] + lambda_y[off + off2] + lambda_y[off + off3];
    }
  return scalar / (hist[color] * 8 * k);
}

std::vector<double> Autocorrelogram::compute(const unsigned char *data) {
  std::vector<double> ac(unique_colors * distance_set.size(), 0.0);
  int ac_ind = 0;
  int twice_max_dist = 2 * max_dist;
  // Build the histogram (used throughout the process)
  build_hist(data);
  // For each color find, iteratively build up the lambdas, each step computing the autocorrelation if necessary
  for (int color = 0; color < unique_colors; ++color) {
    clear();
    for (int dist = 0; dist <= twice_max_dist; ++dist) {
      // Update lambdas lambda y lags by 2 which allows us to avoid overlap between them
      update_lambda_x(data, dist, color);
      if (dist >= 2)
	update_lambda_y(data, dist-2, color);
      // Check if we should compute an autocorrelation, if so then do it
      if (dist >= 2 && dist % 2 == 0 && is_dist_in_set(dist / 2))
	ac[ac_ind++] = make_correlogram_scalar(data, dist / 2, color);
    }
  }
  return ac;
}

void autocorrelogram(unsigned char *data, int height, int width, int unique_colors,
                     int *distance_set, int distance_set_size, double *ac_out, int ac_out_size) {
    std::vector<int> distance_set_vec(distance_set_size);
    for (int i = 0; i < distance_set_size; ++i)
        distance_set_vec[i] = distance_set[i];
    Autocorrelogram ac(height, width, unique_colors, distance_set_vec);
    std::vector<double> ac_vec = ac.compute(data);
    assert((unsigned int)ac_out_size == ac_vec.size());
    for (int i = 0; i < ac_out_size; ++i)
        ac_out[i] = ac_vec[i];
}
