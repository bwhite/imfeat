#include "SurfPoint.h"
#include <cstdlib>
#include <cstring>

SurfPoint::SurfPoint(int x, int y, int scale, bool sign, float cornerness) : x(x),y(y),scale(scale),sign(sign), cornerness(cornerness)
{
	features64 = 0;
	features128 = 0;
#ifdef POINT_STATS
	rank = -1;
#endif
}

SurfPoint::~SurfPoint()
{
	if (features64) {
		delete [] features64;
		features64 = 0;
	}

	if (features128) {
		delete [] features128;
		features128 = 0;
	}
}

bool SurfPoint::operator < (const SurfPoint& rhs)
{
	return  cornerness < rhs.cornerness;
}

SurfPoint::SurfPoint(const void *data)
{
	memcpy(this, data, 32);
	data = (char*) data + 32;
	features64 = new float[64];
	features128 = 0;
	memcpy(features64, data, sizeof(float)*64);
}

void SurfPoint::write(void *data)
{
	memcpy(data, this, 32);
	data = (char*) data + 32;
	memcpy(data, features64, sizeof(float)*64);
}

size_t SurfPoint::write(std::list<SurfPoint*> *points, void **dataptr)
{
	char header[64] = "DVersion 0.01";
	size_t size = sizeof(char)*64 + sizeof(int) + (32 + sizeof(float)*64) * points->size();

	void *data = malloc(size);
	*dataptr = data;
	memcpy(data, header, sizeof(char)*64);
	data = (char *) data + 64;

	*(int *) data = (int) points->size();
	data = (int *) data + 1;

	for (std::list<SurfPoint*>::iterator it = points->begin(); it != points->end(); it++) {
		(*it)->write(data);
		data = (char *) data + 32;
		data = (float *) data + 64;
	}

	return size;
}

void SurfPoint::read(std::list<SurfPoint*> *points, const void *data)
{
	char header[64];
	memcpy(header, data, sizeof(char)*64);
	data = (char *) data + 64;
	float version;
	if (sscanf(header, " DVersion %f ", &version) != 1) {
		fprintf(stderr, "Couldn't parse header\n");
		exit(1);
	}
	if (version != (float)0.01) {
		fprintf(stderr, "Expected a different version found %f\n", version);
		exit(1);
	}

	int npoints;
	npoints = *(int *) data;
	data = (int *) data + 1;

	for (int i = 0; i < npoints; i++) {
		SurfPoint *point = new SurfPoint(data);
		data = (char *) data + 32;
		data = (float *) data + 64;
		points->push_back(point);
	}
}

