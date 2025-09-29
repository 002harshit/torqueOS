#include "../math.h"

vec2_t vec2(float x, float y)
{
	return (vec2_t){x, y};
}

vec2_t vec2_zero()
{
	return (vec2_t){0, 0};
}

vec2_t vec2_add(vec2_t a, vec2_t b)
{
	return (vec2_t){a.x + b.x, a.y + b.y};
}

vec2_t vec2_sub(vec2_t a, vec2_t b)
{
	return (vec2_t){a.x - b.x, a.y - b.y};
}

vec2_t vec2_mul(vec2_t a, vec2_t b)
{
	return (vec2_t){a.x * b.x, a.y * b.y};
}

vec2_t vec2_addv(vec2_t a, float v)
{
	return (vec2_t){a.x + v, a.y + v};
}

vec2_t vec2_subv(vec2_t a, float v)
{
	return (vec2_t){a.x - v, a.y - v};
}

vec2_t vec2_mulv(vec2_t a, float v)
{
	return (vec2_t){a.x * v, a.y * v};
}

vec2_t vec2_divv(vec2_t a, float v)
{
	return (vec2_t){a.x / v, a.y / v};
}

float vec2_lensq(vec2_t a)
{
	return fsq(a.x) + fsq(a.y);
}

float vec2_len(vec2_t a)
{
	return sqrtf(fsq(a.x)) + (fsq(a.y));
}

float vec2_distsq(vec2_t a, vec2_t b)
{
	return fsq(a.x - b.x) + fsq(a.y - b.y);
}

float vec2_dist(vec2_t a, vec2_t b)
{
	return sqrtf(fsq(a.x - b.x) + fsq(a.y - b.y));
}

vec2_t vec2_wrap(vec2_t value, vec2_t min, vec2_t max)
{
	return (vec2_t){fwrap(value.x, min.x, max.x), fwrap(value.y, min.y, max.y)};
}

vec2_t vec2_wrap2(vec2_t value, float min, float max)
{
	return (vec2_t){fwrap(value.x, min, max), fwrap(value.y, min, max)};
}

int vec2_is_almost_eq(vec2_t a, vec2_t b)
{
	return fequal(a.x, b.x) && fequal(a.y, b.y);
}

vec2_t vec2_clamp(vec2_t value, vec2_t min, vec2_t max)
{
	return (vec2_t){fclamp(value.x, min.x, max.x), fclamp(value.y, min.y, max.y)};
}

vec2_t vec2_clamp2(vec2_t value, float min, float max)
{
	return (vec2_t){fclamp(value.x, min, max), fwrap(value.y, min, max)};
}

vec2_t vec2_lerp(vec2_t a, vec2_t b, float t)
{
	return (vec2_t){flerp(a.x, b.x, t), flerp(a.y, b.y, t)};
}

vec2_t vec2_normalize(vec2_t a)
{
	float l = vec2_len(a);
	if (l > 0)
		return (vec2_t){a.x / l, a.y / l};
	return vec2_zero();
}

vec2_t vec2_invert(vec2_t a)
{
	return (vec2_t){1.0 / a.x, 1.0 / a.y};
}

float vec2_dot(vec2_t a, vec2_t b)
{
	return a.x * b.x + a.y * b.y;
}

float vec2_cross(vec2_t a, vec2_t b)
{
	return a.x * b.y - a.y * b.x;
}

float vec2_angle(vec2_t a, vec2_t b)
{
	return atan2f(vec2_cross(a, b), vec2_dot(a, b));
}

vec2_t vec2_rotate(vec2_t a, float angle_rad)
{
	float c = cosf(angle_rad);
	float s = sinf(angle_rad);
	return (vec2_t){a.x * c - a.y * s, a.x * s + a.y * c};
}

