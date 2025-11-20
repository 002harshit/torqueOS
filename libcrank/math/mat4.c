#include "../math.h"

mat4_t mat4_zero()
{
  return (mat4_t) {0};
}

mat4_t mat4_identity()
{
  return (mat4_t) {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1,
  };
}

mat4_t mat4_mul(mat4_t a, mat4_t b)
{
  return (mat4_t) {
    (a.m00 * b.m00) + (a.m01 * b.m10) + (a.m02 * b.m20) + (a.m03 * b.m30),
    (a.m00 * b.m01) + (a.m01 * b.m11) + (a.m02 * b.m21) + (a.m03 * b.m31),
    (a.m00 * b.m02) + (a.m01 * b.m12) + (a.m02 * b.m22) + (a.m03 * b.m32),
    (a.m00 * b.m03) + (a.m01 * b.m13) + (a.m02 * b.m23) + (a.m03 * b.m33),

    (a.m10 * b.m00) + (a.m11 * b.m10) + (a.m12 * b.m20) + (a.m13 * b.m30),
    (a.m10 * b.m01) + (a.m11 * b.m11) + (a.m12 * b.m21) + (a.m13 * b.m31),
    (a.m10 * b.m02) + (a.m11 * b.m12) + (a.m12 * b.m22) + (a.m13 * b.m32),
    (a.m10 * b.m03) + (a.m11 * b.m13) + (a.m12 * b.m23) + (a.m13 * b.m33),

    (a.m20 * b.m00) + (a.m21 * b.m10) + (a.m22 * b.m20) + (a.m23 * b.m30),
    (a.m20 * b.m01) + (a.m21 * b.m11) + (a.m22 * b.m21) + (a.m23 * b.m31),
    (a.m20 * b.m02) + (a.m21 * b.m12) + (a.m22 * b.m22) + (a.m23 * b.m32),
    (a.m20 * b.m03) + (a.m21 * b.m13) + (a.m22 * b.m23) + (a.m23 * b.m33),

    (a.m30 * b.m00) + (a.m31 * b.m10) + (a.m32 * b.m20) + (a.m33 * b.m30),
    (a.m30 * b.m01) + (a.m31 * b.m11) + (a.m32 * b.m21) + (a.m33 * b.m31),
    (a.m30 * b.m02) + (a.m31 * b.m12) + (a.m32 * b.m22) + (a.m33 * b.m32),
    (a.m30 * b.m03) + (a.m31 * b.m13) + (a.m32 * b.m23) + (a.m33 * b.m33),
  };
}

mat4_t mat4_transpose(mat4_t m)
{
  return (mat4_t) {
    m.m00, m.m10, m.m20, m.m30,
    m.m01, m.m11, m.m21, m.m31,
    m.m02, m.m12, m.m22, m.m32,
    m.m03, m.m13, m.m23, m.m33,
  };
}

// Ref: https://journalppw.com/index.php/jpsp/article/download/1313/668/1516

mat4_t mat4_translate(vec3_t t)
{
  return (mat4_t) {
    1,   0,   0, 0,
    0,   1,   0, 0,
    0,   0,   1, 0,
    t.x, t.y, t.z, 1,
  };
}

mat4_t mat4_rotate_x(float angle_rad)
{
  float c = cosf(angle_rad);
  float s = sinf(angle_rad);
  return (mat4_t) {
    1,  0,  0,  0,
    0,  c, -s,  0,
    0,  s,  c,  0,
    0,  0,  0,  1
  };
}

mat4_t mat4_rotate_y(float angle_rad)
{
  float c = cosf(angle_rad);
  float s = sinf(angle_rad);
  return (mat4_t) {
    c,  0,  s,  0,
    0,  1,  0,  0,
    -s,  0,  c,  0,
    0,  0,  0,  1,
  };
}

mat4_t mat4_rotate_z(float angle_rad)
{
  float c = cosf(angle_rad);
  float s = sinf(angle_rad);
  return (mat4_t) {
    c, -s,  0,  0,
    s,  c,  0,  0,
    0,  0,  1,  0,
    0,  0,  0,  1,
  };
}

mat4_t mat4_scale(vec3_t s)
{
  return (mat4_t) {
    s.x,   0,   0,   0,
    0, s.y,   0,   0,
    0,   0, s.z,   0,
    0,   0,   0,   1,
  };
}

// Wikipedia specifies axis aligned rotation for column major rotation
// As we are using row major we will use transpose of it
// Ref: https://en.wikipedia.org/wiki/Rotation_matrix#General_3D_rotations

mat4_t mat4_rotate_axis(vec3_t axis, float angle_rad)
{
  float c = cosf(angle_rad);
  float s = sinf(angle_rad);
  vec3_t u = vec3_normalize(axis);
  return (mat4_t) {
    fsq(u.x) * (1 - c) + c, u.x * u.y * (1 - c) + u.z * s, u.x * u.z * (1 - c) - u.y * s, 0,
    u.x * u.y * (1 - c) - u.z * s, fsq(u.y) * (1 - c) + c, u.y * u.z * (1 - c) + u.x * s, 0,
    u.x * u.z * (1 - c) + u.y * s,  u.y * u.z * (1 - c) - u.x * s, fsq(u.z) * (1 - c) + c, 0,
    0, 0, 0, 1,
  };
}

// Ref: https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml
// Ref: https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatrh
// Ref: https://stackoverflow.com/questions/349050/calculating-a-lookat-matrix

mat4_t mat4_lookat_lh(vec3_t eye, vec3_t center, vec3_t up)
{
  vec3_t f = vec3_normalize(vec3_sub(center, eye)); // vz
  vec3_t s = vec3_normalize(vec3_cross(up, f)); // vx
  vec3_t u = vec3_cross(f, s); // vy

  return (mat4_t) {
    s.x, u.x, f.x, 0,
    s.y, u.y, f.y, 0,
    s.z, u.z, f.z, 0,
    -vec3_dot(s, eye), -vec3_dot(u, eye), -vec3_dot(f, eye), 1,
  };
}

mat4_t mat4_lookat_rh(vec3_t eye, vec3_t center, vec3_t up)
{
  vec3_t f = vec3_normalize(vec3_sub(eye, center)); // vz
  vec3_t s = vec3_normalize(vec3_cross(up, f)); // vx
  vec3_t u = vec3_cross(f, s); // vy

  return (mat4_t) {
    s.x, u.x, f.x, 0,
    s.y, u.y, f.y, 0,
    s.z, u.z, f.z, 0,
    -vec3_dot(s, eye), -vec3_dot(u, eye), -vec3_dot(f, eye), 1,
  };
}


// Reference uses column major
// Ref: https://docs.gl/gl3/glFrustum

// mat4_t mat4_frustum(float left, float right, float bottom, float top, float near, float far)
// {
// 
// }

// Reference uses column major
// Ref: https://docs.gl/gl3/glOrtho

mat4_t mat4_ortho(float left, float right, float bottom, float top, float near, float far)
{
  vec3_t t = (vec3_t) {
    -(right + left) / (right - left),
    -(top + bottom) / (top - bottom),
    -(far + near)   / (far - near),
  };
  return (mat4_t) {
    2 / (right - left), 0, 0, 0,
    0, 2 / (top - bottom),0, 0,
    0, 0, -2 / (far - near), 0,
    t.x, t.y, t.z, 1,
  };	
}

mat4_t mat4_perspective(float fov_rad, float aspect, float n, float f)
{
  // Ref: https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/opengl-perspective-projection-matrix.html
  float scale = tanf(fov_rad * 0.5) * n; 
  float t = scale;
  float b = -t;
  float r = t * aspect;
  float l  = -r;
  return (mat4_t) {
    2 * n / (r - l), 0, 0, 0,
    0,   2 * n / (t - b), 0,  0,
    (r + l) / (r - l), (t + b) / (t - b), -(f + n)/ (f - n), -1,
    0, 0, - 2 * f * n / (f - n),  0
  };
}

// TODO: confirm if the math is right or not
mat4_t mat4_inverse(mat4_t m)
{
  // Matrix result = { 0 };

  // Cache the matrix values (speed optimization)
  float a00 = m.m00, a01 = m.m01, a02 = m.m02, a03 = m.m03;
  float a10 = m.m10, a11 = m.m11, a12 = m.m12, a13 = m.m13;
  float a20 = m.m20, a21 = m.m21, a22 = m.m22, a23 = m.m23;
  float a30 = m.m30, a31 = m.m31, a32 = m.m32, a33 = m.m33;

  float b00 = a00*a11 - a01*a10;
  float b01 = a00*a12 - a02*a10;
  float b02 = a00*a13 - a03*a10;
  float b03 = a01*a12 - a02*a11;
  float b04 = a01*a13 - a03*a11;
  float b05 = a02*a13 - a03*a12;
  float b06 = a20*a31 - a21*a30;
  float b07 = a20*a32 - a22*a30;
  float b08 = a20*a33 - a23*a30;
  float b09 = a21*a32 - a22*a31;
  float b10 = a21*a33 - a23*a31;
  float b11 = a22*a33 - a23*a32;

  // Calculate the invert determinant (inlined to avoid double-caching)
  float invDet = 1.0f/(b00*b11 - b01*b10 + b02*b09 + b03*b08 - b04*b07 + b05*b06);

  mat4_t result = {0};
  result.m00 = (a11*b11 - a12*b10 + a13*b09)*invDet;
  result.m01 = (-a01*b11 + a02*b10 - a03*b09)*invDet;
  result.m02 = (a31*b05 - a32*b04 + a33*b03)*invDet;
  result.m03 = (-a21*b05 + a22*b04 - a23*b03)*invDet;
  result.m10 = (-a10*b11 + a12*b08 - a13*b07)*invDet;
  result.m11 = (a00*b11 - a02*b08 + a03*b07)*invDet;
  result.m12 = (-a30*b05 + a32*b02 - a33*b01)*invDet;
  result.m13 = (a20*b05 - a22*b02 + a23*b01)*invDet;
  result.m20 = (a10*b10 - a11*b08 + a13*b06)*invDet;
  result.m21 = (-a00*b10 + a01*b08 - a03*b06)*invDet;
  result.m22 = (a30*b04 - a31*b02 + a33*b00)*invDet;
  result.m23 = (-a20*b04 + a21*b02 - a23*b00)*invDet;
  result.m30 = (-a10*b09 + a11*b07 - a12*b06)*invDet;
  result.m31 = (a00*b09 - a01*b07 + a02*b06)*invDet;
  result.m32 = (-a30*b03 + a31*b01 - a32*b00)*invDet;
  result.m33 = (a20*b03 - a21*b01 + a22*b00)*invDet;
  return result;
}
