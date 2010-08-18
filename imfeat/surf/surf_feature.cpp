#include <cstring>
#include "SurfDetect.h"
#include "SurfDescribe.h"
#include "surf_feature.hpp"

bool cmp_surf_point(SurfPoint* s0, SurfPoint* s1) {
  if (s0->cornerness > s1->cornerness)
    return true;
  return false;
}

int compute_surf_descriptors(char *data, int height, int width, int max_points, float *points) {
  int total_points = 0;
  IntegralImage intim(data, height, width);
  std::list<SurfPoint*> *sp = SurfDetect::allocPoints();
  SurfDetect sdet(height, width);
  sdet.compute(sp, &intim);
  SurfDescribe *sdesc = new SurfDescribe();
  sdesc->compute(sp, &intim);
  int feature_bytes = sizeof(float) * 64;
  sp->sort(cmp_surf_point);
  for (std::list<SurfPoint*>::iterator iter = sp->begin(); iter != sp->end(); ++iter, ++total_points) {
    if (total_points >= max_points)
      break;
    memcpy(points, (*iter)->features64, feature_bytes);
    points += 64;
  }
  delete sdesc;
  SurfDetect::freePoints(&sp);
  return total_points;
}
