#include "../math.h"

float fsq(float f)
{
	return f * f;
}

float deg_to_rad(float angle_rad)
{
	return angle_rad * M_PI / 180.0;
}

float fwrap(float value, float min, float max)
{
	return value - (max - min) * floorf((value - min) / (max - min));
}

int fequal(float x, float y, float epsilon)
{
	return (fabsf(x - y)) <= (epsilon * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))));
}

float fclamp(float value, float min, float max)
{
	float res = (value < min) ? min : value;
	return (res > max) ? max : res;
}

float flerp(float a, float b, float t)
{
	return a + t * (b - a);
}

float fnormalize(float value, float min, float max)
{
	return (value - min) / (max - min);
}

float fremap(float value, float smin, float smax, float emin, float emax)
{
	return emin + (value - smin) * (emax - emin) / (smax - smin);
}
