#ifndef HISTOGRAM_HPP
#define HISTOGRAM_HPP
#include <vector>
class Histogram {
public:
  /* Data is assumed to have the channels for each pixel in consecutive order (for RGB) RGBRGBRGB and will return the moments in that order Mean R, Variance R, Skew R..., Mean G...; however, the actual order of the channels input doesn't matter (so BGR works just as fine).  Note that the number of bins should evenly divide 256.  */
  Histogram(int channels, int num_pixels, int num_bins);
  void compute(unsigned char *data, int *bins);
private:
  const int channels;
  const int num_pixels;
  const int num_bins;
  int *bins;
};
extern "C" {
  void compute(int channels, int num_pixels, int num_bins, char *data, int *bins);
}
#endif
