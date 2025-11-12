#include "../math.h"

float floorf(float x)
{
  float t = (long)x;
  return t - (t > x);
}

float fabsf(float x)
{
  return x < 0 ? -x : x;
}

float fminf(float x, float y)
{
  return x < y ? x : y;
}

float fmaxf(float x, float y)
{
  return x > y ? x : y;
}

float sqrtf(float x)
{
  if (x != x || x < 0) return 0.0/0.0;
  if (x == 0.0f) return 0.0f;
  if (x > 3.4028235e38f) return x;

  // Newtons method
  // Reference: https://math.mit.edu/~stevenj/18.335/newton-sqrt.pdf
  float result = x / 2; // guess
  float xn;
  do {
    xn = result;
    result = (xn + x / xn) / 2.0;
  } while(fabsf(result - xn) > 1e-6f);
  return result;
}

float logf(float x)
{
	unsigned a = 0;
	float b = 0;
	if (x > 0) {
		unsigned d;
		float e = 0, c = 0, f = 0;
		for (c = ((x < 1.0) ? 1 / x : x); (c /= M_E) > 1; ++a) {

		}
		c = 1 / (c * M_E - 1), c = c + c + 1, f = c * c, b = 0;
		for (d = 1, c /= 2; e = b, b += 1 / (d * c), b - e;)
			d += 2, c *= f;
	} else {
		b = (x == 0) / 0.;
	}
	return x < 1 ? -(a + b) : a + b;
}

float expf(float x)
{
	float sum = 1.0;
	float cur = 1.0;
	unsigned int n = 1;

	while (fabsf(cur) > 1e-10f) {
		sum += (cur *= ((float)x) / n++);
	}

	return sum;
}

float powf(float base, float exp)
{
	float result;
	if (base == 0.0 && exp != 0.0) {
		result = 0.0;
	} else if (exp < 0) {
		result = powf(1 / base, -exp);
	} else if (base < 0.0 && exp != (long)exp) {
		result = -NAN;
	} else if (exp == 0.0) {
		result = 1.0;
	} else {
		float flag = 1.0;
		if (base < 0 && (long)exp % 2) {
			flag = -1.0;
		}
		if (base < 0)
			base = -base;
		long div = (long)exp;
		float integerPart = 1;
		for (int i = 0; i < div; i++)
			integerPart *= base;
		result =
			integerPart * expf((exp - (float)div) * logf(base)) * flag;
	}
	return result;
}
