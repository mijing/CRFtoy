#ifndef MATRIX
#define MATIX
#include<algorithm>
#include<assert.h>

template<typename T>
class Matrix
{
private:
	size_t maxd1;
	size_t maxd2;
	size_t d1;
	size_t d2;
	T* array;
public:
	Matrix(size_t rs, size_t cs)
	{
		maxd1 = rs;
		maxd2 = cs;
		d1 = rs;
		d2 = cs;
		assert(rs > 0 && cs > 0);
		size_t size = rs*cs;
		array = new T[rs*cs];
	}

	void set_d1_d2(size_t rs, size_t cs)
	{
		assert(rs*cs <= maxd2*maxd1);
		d1 = rs;
		d2 = cs;
	}

	void clean()
	{
		memset(array, 0, maxd2*maxd1);
	}

	T& element(size_t i, size_t j)
	{
		assert(i < d1);
		assert(j < d2);
		return array[i*d2 + j];
	}

	T& operator()(size_t i, size_t j)
	{
		assert(i < d1);
		assert(j < d2);
		return array[i*d2 + j];
	}

	~Matrix()
	{
		delete[] array;
		array = nullptr;
	}
};

#endif