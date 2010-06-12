#ifndef HISTOGRAM_HPP
#define HISTOGRAM_HPP
#include <vector>
class Histogram {
public:
  /* Data is assumed to have the channels for each pixel in consecutive order (for RGB) RGBRGBRGB and will return the moments in that order Mean R, Variance R, Skew R..., Mean G...; however, the actual order of the channels input doesn't matter (so BGR works just as fine).  Note that the number of bins should evenly divide 256.  */
  Histogram(int channels, int num_pixels, int num_bins);
  std::vector<int> compute(unsigned char *data);
  std::vector<int> compute(char *data);
private:
  const int channels;
  const int num_pixels;
  const int num_bins;
  int *bins;
};
#endif
