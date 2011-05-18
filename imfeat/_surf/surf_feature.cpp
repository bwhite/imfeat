#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
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

int compute_surf_points(char *data, int height, int width, int max_points, float *points, int *x, int *y, int *scale, float *orientation, char *sign, float *cornerness) {
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
    *(x++) = (*iter)->x;
    *(y++) = (*iter)->y;
    *(scale++) = (*iter)->scale;
    *(orientation++) = (*iter)->orientation;
    *(sign++) = (*iter)->sign;
    *(cornerness++) = (*iter)->cornerness;
  }
  delete sdesc;
  SurfDetect::freePoints(&sp);
  return total_points;
}


int compute_surf_random(char *data, int height, int width, int max_points, float *points) {
  int total_points = 0;
  IntegralImage intim(data, height, width);
  std::list<SurfPoint*> *sp = SurfDetect::allocPoints();
  // Start Random
  //int prev = 0;
  double weight_normalize = 0.;
  srand(time(NULL));
  for (int scale = 1; scale < 10000; ++scale) {
    int bound = 17 * scale + 2;
    int x_min_bound = bound;
    int y_min_bound = bound;
    int x_max_bound = width - bound;
    int y_max_bound = height - bound;
    if (x_max_bound <= x_min_bound || y_max_bound <= y_min_bound)
      break;
    weight_normalize += (width - 2*bound) * (height - 2*bound) / (double)(scale * scale);
  }
  for (int scale = 1; scale < 10000; ++scale) {
    int bound = 17 * scale + 2;
    int x_min_bound = bound;
    int y_min_bound = bound;
    int x_max_bound = width - bound;
    int y_max_bound = height - bound;
    double cur_weight = (width - 2*bound) * (height - 2*bound) / (double)(scale * scale);
    cur_weight /= weight_normalize;
    int num_points = cur_weight * max_points;
    if (x_max_bound <= x_min_bound || y_max_bound <= y_min_bound || !max_points)
      break;
    double x_width = (x_max_bound - x_min_bound) / (double)RAND_MAX;
    double y_width = (y_max_bound - y_min_bound) / (double)RAND_MAX;
    for (int pt_num = 0; pt_num < num_points; ++pt_num) {
      int x = rand() * x_width + x_min_bound;
      int y = rand() * y_width + y_min_bound;
      //printf("%d [%d,%d)=%d [%d,%d)=%d\n", scale, x_min_bound, x_max_bound, x, y_min_bound, y_max_bound, y);
      sp->push_front(new SurfPoint(x, y, scale, true, 0));
    }
    //printf("%d %d %d\n", scale, sp->size(), sp->size() - prev);
    //prev = sp->size();
  }
  // End Random
  SurfDescribe *sdesc = new SurfDescribe();
  sdesc->compute(sp, &intim);
  int feature_bytes = sizeof(float) * 64;
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
