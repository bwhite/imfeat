#ifndef HISTOGRAMJOINT_HPP
#define HISTOGRAMJOINT_HPP
#include <vector>
class HistogramJoint {
public:
  /* Data is assumed to have the channels for each pixel in consecutive order (for RGB) RGBRGBRGB; however, the actual order of the channels input doesn't matter (so BGR works just as fine).  Note that the number of bins should evenly divide 256.  This currently only works for 3 dimensional color spaces */
  HistogramJoint(int num_pixels, int num_bins);
  std::vector<int> compute(unsigned char *data);
  std::vector<int> compute(char *data);
  void compute(unsigned char *data, int *bins);
private:
  const int num_pixels;
  const int num_bins;
};

extern "C" {
void compute(int num_pixels, int num_bins, char *data, int *bins);
}
#endif
