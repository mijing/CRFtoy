#ifndef CUBE
#define CUBE
#include<algorithm>
#include<assert.h>
#include<cstring>

template<typename T>
class Cube
{
private:
	size_t maxd1;
	size_t maxd2;
	size_t maxd3;
	size_t d1;
	size_t d2;
	size_t d3;
	T* array;
public:
	Cube(size_t v1, size_t v2, size_t v3)
	{
		maxd1 = v1;
		maxd2 = v2;
		maxd3 = v3;
		d1 = v1;
		d2 = v2;
		d3 = v3;
		assert(v1 > 0 && v2 > 0 && v3 > 0);
		array = new T[maxd1*maxd2*maxd3];
	}

	void set_dimensions(size_t v1, size_t v2, size_t v3)
	{
		assert(v1*v2*v2 <= d1*d2*d3);
		d1 = v1;
		d2 = v2;
		d3 = v3;
	}

	void clean()
	{
		memset(array, 0, maxd1*maxd2*maxd3);
	}

	T& element(size_t i, size_t j, size_t k)
	{
		assert(i < d1);
		assert(j < d2);
		assert(k < d3);
		return array[i*d2*d3 + j*d3 + k];
	}

	T& operator()(size_t i, size_t j, size_t k)
	{
		assert(i < d1);
		assert(j < d2);
		assert(k < d3);
		return array[i*d2*d3 + j*d3 + k];
	}

	~Cube()
	{
		delete[] array;
		array = nullptr;
	}
};

#endif
