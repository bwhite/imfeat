#ifndef MOMENTS_H
#define MOMENTS_H
#include <vector>

class Moments {
public:
  /* Data is assumed to have the channels for each pixel in consecutive order (for RGB) RGBRGBRGB and will return the moments in that order Mean R, Variance R, Skew R..., Mean G...; however, the actual order of the channels input doesn't matter (so BGR works just as fine) */
  Moments(int channels, int num_pixels, int num_moments);
  std::vector<double> compute(unsigned char * data);
  std::vector<double> compute(char * data);
private:
  const int channels;
  const int num_pixels;
  const int num_moments;
  /* Computes integral powers >=0, will eventually become slower than the true power for higher powers */
  double fast_pow(double val, int power);
  double safe_root(double val, int rpower);
};
extern "C" {
void compute(int channels, int num_pixels, int num_moments, char *data, double *moments);
}
#endif
