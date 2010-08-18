#ifndef SURFPOINT
#define SURFPOINT
#include <cstdio>
#include <list>

class SurfPoint
{
public:
	SurfPoint(int x, int y, int scale, bool sign, float cornerness);
	~SurfPoint();
	bool operator < (const SurfPoint& rhs);
	int x;
	int y;
	int scale;
	float orientation;
	bool sign;
	float cornerness;
	float *features64;
	float *features128;

	SurfPoint(const void *data);
	void write(void *data);

#ifdef POINT_STATS
	int rank;
#endif

	static void read(std::list<SurfPoint*> *points, const void *data);
	static size_t write(std::list<SurfPoint*> *points, void **data);

private:
};
#endif
