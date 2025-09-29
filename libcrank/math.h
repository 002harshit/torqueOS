#ifndef CRANK_MATH_H
#define CRANK_MATH_H

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif // PI

#ifndef EPSILON
#define EPSILON 0.00001
#endif // EPSILON

typedef struct {
	float x, y;
} vec2_t;

typedef struct {
	float x, y, z;
} vec3_t;

// Row Major Matrix
typedef struct {
	float m00, m01, m02, m03;
	float m10, m11, m12, m13;
	float m20, m21, m22, m23;
	float m30, m31, m32, m33;
} mat4_t;

float floorf(float x);
float fabsf(float x);
float fminf(float x, float y);
float fmaxf(float x, float y);
float tanf(float x);
float sinf(float x);
float cosf(float x);
float atan2f(float y, float x);
float sqrtf(float x);


// FLOATS DECL
float fsq(float f);
float deg_to_rad(float angle_rad);
float fwrap(float value, float min, float max);
int fequal(float x, float y);
float fclamp(float value, float min, float max);
float flerp(float a, float b, float t);
float fnormalize(float value, float min, float max);
float fremap(float value, float smin, float smax, float emin, float emax);

// VEC2 DECL

vec2_t vec2(float x, float y);
vec2_t vec2_zero();
vec2_t vec2_add(vec2_t a, vec2_t b);
vec2_t vec2_sub(vec2_t a, vec2_t b);
vec2_t vec2_mul(vec2_t a, vec2_t b);
vec2_t vec2_addv(vec2_t a, float v);
vec2_t vec2_subv(vec2_t a, float v);
vec2_t vec2_mulv(vec2_t a, float v);
vec2_t vec2_divv(vec2_t a, float v);
float vec2_lensq(vec2_t a);
float vec2_len(vec2_t a);
float vec2_distsq(vec2_t a, vec2_t b);
float vec2_dist(vec2_t a, vec2_t b);
vec2_t vec2_wrap(vec2_t value, vec2_t min, vec2_t max);
vec2_t vec2_wrap2(vec2_t value, float min, float max);
int vec2_is_almost_eq(vec2_t a, vec2_t b);
vec2_t vec2_clamp(vec2_t value, vec2_t min, vec2_t max);
vec2_t vec2_clamp2(vec2_t value, float min, float max);
vec2_t vec2_lerp(vec2_t a, vec2_t b, float t);
vec2_t vec2_normalize(vec2_t a);
vec2_t vec2_invert(vec2_t a);
float vec2_dot(vec2_t a, vec2_t b);
float vec2_cross(vec2_t a, vec2_t b);
float vec2_angle(vec2_t a, vec2_t b);
vec2_t vec2_rotate(vec2_t a, float angle_rad);

// VEC3 DECL
vec3_t vec3_zero();
vec3_t vec3(float x, float y, float z);
vec3_t vec3_add(vec3_t a, vec3_t b);
vec3_t vec3_sub(vec3_t a, vec3_t b);
vec3_t vec3_mul(vec3_t a, vec3_t b);
vec3_t vec3_div(vec3_t a, vec3_t b);
vec3_t vec3_addv(vec3_t a, float b);
vec3_t vec3_subv(vec3_t a, float b);
vec3_t vec3_mulv(vec3_t a, float b);
vec3_t vec3_divv(vec3_t a, float b);
float vec3_lensq(vec3_t a);
float vec3_len(vec3_t a);
float vec3_distsq(vec3_t a, vec3_t b);
float vec3_dist(vec3_t a, vec3_t b);
int vec3_is_equal(vec3_t a, vec3_t b);
vec3_t vec3_lerp(vec3_t a, vec3_t b, float t);
vec3_t vec3_min(vec3_t a, vec3_t b);
vec3_t vec3_max(vec3_t a, vec3_t b);
vec3_t vec3_normalize(vec3_t a);
vec3_t vec3_invert(vec3_t a);
float vec3_dot(vec3_t a, vec3_t b);
vec3_t vec3_cross(vec3_t a, vec3_t b);
vec3_t vec3_transform(vec3_t v, mat4_t m);
float vec3_angle(vec3_t a, vec3_t b);
vec3_t vec3_rotate_axis(vec3_t v, vec3_t axis, float angle_rad);
vec3_t vec3_barycentric(vec3_t p, vec3_t a, vec3_t b, vec3_t c);
vec3_t vec3_reflect(vec3_t I, vec3_t N);

// MAT4 DECL
mat4_t mat4_zero();
mat4_t mat4_identity();
mat4_t mat4_mul(mat4_t a, mat4_t b);
mat4_t mat4_transpose(mat4_t m);
mat4_t mat4_translate(vec3_t t);
mat4_t mat4_rotate_x(float angle_rad);
mat4_t mat4_rotate_y(float angle_rad);
mat4_t mat4_rotate_z(float angle_rad);
mat4_t mat4_scale(vec3_t s);
mat4_t mat4_rotate_axis(vec3_t axis, float angle_rad);
mat4_t mat4_lookat_lh(vec3_t eye, vec3_t center, vec3_t up);
mat4_t mat4_lookat_rh(vec3_t eye, vec3_t center, vec3_t up);
mat4_t mat4_ortho(float left, float right, float bottom, float top, float near, float far);
mat4_t mat4_perspective(float fov_rad, float aspect, float n, float f);
mat4_t mat4_inverse(mat4_t m);

#endif // CRANK_MATH_H
