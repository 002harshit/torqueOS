#include "../math.h"

vec3_t vec3_zero()
{
	return (vec3_t){0, 0, 0};
}

vec3_t vec3(float x, float y, float z)
{
	return (vec3_t){x, y, z};
}

vec3_t vec3_add(vec3_t a, vec3_t b)
{
	return (vec3_t){a.x + b.x, a.y + b.y, a.z + b.z};
}

vec3_t vec3_sub(vec3_t a, vec3_t b)
{
	return (vec3_t){a.x - b.x, a.y - b.y, a.z - b.z};
}

vec3_t vec3_mul(vec3_t a, vec3_t b)
{
	return (vec3_t){a.x * b.x, a.y * b.y, a.z * b.z};
}

vec3_t vec3_div(vec3_t a, vec3_t b)
{
	return (vec3_t){a.x / b.x, a.y / b.y, a.z / b.z};
}

vec3_t vec3_addv(vec3_t a, float b)
{
	return (vec3_t){a.x + b, a.y + b, a.z + b};
}

vec3_t vec3_subv(vec3_t a, float b)
{
	return (vec3_t){a.x - b, a.y - b, a.z - b};
}

vec3_t vec3_mulv(vec3_t a, float b)
{
	return (vec3_t){a.x * b, a.y * b, a.z * b};
}

vec3_t vec3_divv(vec3_t a, float b)
{
	return (vec3_t){a.x / b, a.y / b, a.z / b};
}

float vec3_lensq(vec3_t a)
{
	return fsq(a.x) + fsq(a.y) + +fsq(a.z);
}

float vec3_len(vec3_t a)
{
	return sqrtf(vec3_lensq(a));
}

float vec3_distsq(vec3_t a, vec3_t b)
{
	return fsq(a.x - b.x) + fsq(a.y - b.y) + fsq(a.z - b.z);
}

float vec3_dist(vec3_t a, vec3_t b)
{
	return sqrtf(vec3_distsq(a, b));
}

int vec3_is_equal(vec3_t a, vec3_t b)
{
	return fequal(a.x, b.x) && fequal(a.y, b.y) && fequal(a.z, b.z);
}

vec3_t vec3_lerp(vec3_t a, vec3_t b, float t)
{
	return (vec3_t){flerp(a.x, b.x, t), flerp(a.y, b.y, t), flerp(a.z, b.z, t)};
}

vec3_t vec3_min(vec3_t a, vec3_t b)
{
	return (vec3_t){fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.x, b.x)};
}

vec3_t vec3_max(vec3_t a, vec3_t b)
{
	return (vec3_t){fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.x, b.x)};
}

vec3_t vec3_normalize(vec3_t a)
{
	float l = vec3_len(a);
	if (l > 0)
		return (vec3_t){a.x / l, a.y / l, a.z / l};
	return vec3_zero();
}

vec3_t vec3_invert(vec3_t a)
{
	return (vec3_t){1.0 / a.x, 1.0 / a.y, 1.0 / a.z};
}

float vec3_dot(vec3_t a, vec3_t b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3_t vec3_cross(vec3_t a, vec3_t b)
{
	return (vec3_t){
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x,
	};
}

vec3_t vec3_transform(vec3_t v, mat4_t m)
{
	return (vec3_t){
		v.x * m.m00 + v.y * m.m10 + v.z * m.m20 + 1.0 * m.m30,
		v.x * m.m01 + v.y * m.m11 + v.z * m.m21 + 1.0 * m.m31,
		v.x * m.m02 + v.y * m.m12 + v.z * m.m22 + 1.0 * m.m32,
	};
}

float vec3_angle(vec3_t a, vec3_t b)
{

	return atan2f(vec3_len(vec3_cross(a, b)), vec3_dot(a, b));
}

vec3_t vec3_rotate_axis(vec3_t v, vec3_t axis, float angle_rad)
{
	// Ref:
	// https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
	axis = vec3_normalize(axis);
	float c = cosf(angle_rad);
	float s = sinf(angle_rad);
	vec3_t p0 = vec3_mulv(v, c);
	vec3_t p1 = vec3_mulv(vec3_cross(axis, v), s);
	vec3_t p2 = vec3_mulv(axis, vec3_dot(axis, v) * (1 - c));
	return vec3_add(vec3_add(p0, p1), p2);
}

vec3_t vec3_barycentric(vec3_t p, vec3_t a, vec3_t b, vec3_t c)
{
	// Ref:
	// https://realtimecollisiondetection.net/books/rtcd/
	vec3_t v0 = vec3_sub(b, a);
	vec3_t v1 = vec3_sub(c, a);
	vec3_t v2 = vec3_sub(p, a);
	float d00 = vec3_dot(v0, v0);
	float d01 = vec3_dot(v0, v1);
	float d11 = vec3_dot(v1, v1);
	float d20 = vec3_dot(v2, v0);
	float d21 = vec3_dot(v2, v1);
	float denom = d00 * d11 - d01 * d01;
	vec3_t result = vec3_zero();
	result.y = (d11 * d20 - d01 * d21) / denom; // v
	result.z = (d00 * d21 - d01 * d20) / denom; // w
	result.x = 1.0 - result.y - result.z;		// u
	return result;
}

// Reference: https://docs.gl/sl4/reflect
// TODO: confirm if math is right or not
vec3_t vec3_reflect(vec3_t I, vec3_t N)
{
	// I - 2.0 * dot(N, I) * N
	return vec3_sub(I, vec3_mulv(N, vec3_dot(N, I) * 2.0));
}

