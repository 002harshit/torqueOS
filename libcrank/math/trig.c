#include "../math.h"
#include "../std.h"

/*
 * TODO: Rewrite this file in my style when i get good at math
 * bcuz I did not write most of the code in this file
 * Most of the code are taken from these sources:

 * for sinf, cosf, tanf implementation
 * https://forums.developer.nvidia.com/t/accuracy-optimized-implementation-of-tanf-without-performance-impact/219720/2

 * for asinf implementation
 * https://git.musl-libc.org/cgit/musl/tree/src/math/asinf.c
 */

static const float pio2 = 1.570796326794896558e+00;

static const float 
pS0 = 1.6666586697e-01,
pS1 = -4.2743422091e-02,
pS2 = -8.6563630030e-03,
qS1 = -7.0662963390e-01;

static float R(float z) {
  float p, q;
  p = z * (pS0 + z * (pS1 + z * pS2));
  q = 1.0f + z * qS1;
  return p / q;
}

float asinf(float x) {
  double s;
  float z;
  unsigned int hx, ix;
  memcpy(&hx, &x, sizeof(hx));
  ix = hx & 0x7fffffff;
  if (ix >= 0x3f800000) {          /* |x| >= 1 */
    if (ix == 0x3f800000)          /* |x| == 1 */
      return x * pio2 + 0x1p-120f; /* asin(+-1) = +-pi/2 with inexact */
    return 0 / (x - x);            /* asin(|x|>1) is NaN */
  }
  if (ix < 0x3f000000) { /* |x| < 0.5 */
    /* if 0x1p-126 <= |x| < 0x1p-12, avoid raising underflow */
    if (ix < 0x39800000 && ix >= 0x00800000)
      return x;
    return x + x * R(x * x);
  }
  /* 1 > |x| >= 0.5 */
  z = (1 - fabsf(x)) * 0.5f;
  s = sqrtf(z);
  x = pio2 - 2 * (s + s * R(z));
  if (hx >> 31)
    return -x;
  return x;
}

static inline float tanf_poly(float s) {
  float r;
  r = 4.38117981e-3f;
  r = (r * s) + 8.94600598e-5f;
  r = (r * s) + 1.08341556e-2f;
  r = (r * s) + 2.12811474e-2f;
  r = (r * s) + 5.40602170e-2f;
  r = (r * s) + 1.33326918e-1f;
  r = (r * s) + 3.33333433e-1f;
  r = r * s;
  return r;
}

/* tangent core computation after reduction */
static inline float tanf_core(float a, int i)
{
  float s = a * a;
  float t = tanf_poly(s);
  float r = (t * a) + a;

  if (i & 1) {
    float lo = a - r;
    lo = (t * a) + lo;
    float inv = -1.0f / r;
    r = (r * inv) + 1.0f;
    r = (lo * inv) + r;
    r = (r * inv) + inv;
  }
  return r;
}

/* 190 bits of 2/π (Payne–Hanek constants) */
static const unsigned int TWO_OVER_PI[] = {0x28be60dbu, 0x9391054au,
                                           0x7f09d5f4u, 0x7d4d3770u,
                                           0x36d8a566u, 0x4f10e410u};

/* Payne–Hanek style reduction using only float/int */
static inline float trig_red_slowpath_f(float a, int *quadrant) {
  const float SCALE = 1.5707963f / 1073741824.0f;
  unsigned int bits;
  memcpy(&bits, &a, sizeof(bits));

  unsigned int mant = ((bits & 0x7fffffu) << 8) | 0x80000000u;
  int e = ((bits >> 23) & 0xff) - 126;

  unsigned int i = (unsigned int)e >> 5;
  e &= 31;

  unsigned int hi = i ? TWO_OVER_PI[i - 1] : 0u;
  unsigned int mid = TWO_OVER_PI[i];
  unsigned int lo = TWO_OVER_PI[i + 1];
  unsigned int tmp = TWO_OVER_PI[i + 2];

  if (e) {
    hi = (hi << e) | (mid >> (32 - e));
    mid = (mid << e) | (lo >> (32 - e));
    lo = (lo << e) | (tmp >> (32 - e));
  }

  /* emulate 64-bit multiply using 32-bit parts */
  unsigned int lo_lo = (unsigned int)((mant & 0xffffu) * (lo & 0xffffu));
  unsigned int lo_hi = (unsigned int)((mant & 0xffffu) * (lo >> 16));
  unsigned int hi_lo = (unsigned int)((mant >> 16) * (lo & 0xffffu));
  unsigned int hi_hi = (unsigned int)((mant >> 16) * (lo >> 16));

  /* combine partial products into 32-bit scaled sum */
  unsigned int carry =
      ((lo_lo >> 16) + (lo_hi & 0xffffu) + (hi_lo & 0xffffu)) >> 16;
  unsigned int low32 = hi_hi + (lo_hi >> 16) + (hi_lo >> 16) + carry;

  /* mix in middle term for higher precision */
  unsigned int mix = (mant >> 8) * (mid >> 8);
  unsigned int p_hi = low32 + (mix >> 2);
  unsigned int p_lo = mix << 30; /* emulate << 32 via scaling */

  /* extract quadrant from top bits */
  int q = (p_hi >> 30) & 3;
  unsigned int frac_part = p_hi & 0x3fffffffU;

  /* convert fractional part to [−0.5, 0.5) range */
  float frac = (float)frac_part * (1.0f / 1073741824.0f);
  if (frac >= 0.5f) {
    frac -= 1.0f;
    q++;
  }

  float r = frac * SCALE; /* scale by π/2 range */
  if (a < 0.0f) {
    r = -r;
    q = -q;
  }
  *quadrant = q;
  return r;
}

/* hybrid reduction: Cody–Waite for small, Payne–Hanek for large */
static inline float trig_red_f(float a, float switch_over, int *q) {
  float j, r;

  if (a > switch_over || a < -switch_over) {
    r = trig_red_slowpath_f(a, q);
  } else {
    j = (a * 0.636619747f) + 1.2582912e+7f;
    int ji;
    memcpy(&ji, &j, sizeof(ji));
    *q = ji;
    j = j - 1.25829120e+7f;

    r = (j * -1.57079601f) + a;
    r = (j * -3.13916473e-7f) + r;
    r = (j * -5.39030253e-15f) + r;
  }
  return r;
}

float tanf(float a)
{
  int i;
  a = (a * 0.0f) + a;  // inf -> NaN, preserves -0.0f
  float r = trig_red_f(a, 252.898206f, &i);
  return tanf_core(r, i);
}

static inline float sinf_poly(float a, float s) {
  float r, t;
  r = 2.86567956e-6f;            /*  0x1.80a000p-19 */
  r = (r * s) + -1.98559923e-4f; /* -0x1.a0690cp-13 */
  r = (r * s) + 8.33338592e-3f;  /*  0x1.111182p-07 */
  r = (r * s) + -1.66666672e-1f; /* -0x1.555556p-03 */
  /* ensure -0.0f is propagated through the final multiply */
  t = (a * s) + 0.0f;
  r = (r * t) + a;
  return r;
}

/* cos polynomial on [-pi/4, +pi/4] */
static inline float cosf_poly(float s) {
  float r;
  r = 2.44677067e-5f;            /*  0x1.9a8000p-16 */
  r = (r * s) + -1.38877297e-3f; /* -0x1.6c0efap-10 */
  r = (r * s) + 4.16666567e-2f;  /*  0x1.555550p-05 */
  r = (r * s) + -5.00000000e-1f; /* -0x1.000000p-01 */
  r = (r * s) + 1.00000000e+0f;  /*  0x1.000000p+00 */
  return r;
}

/* Map sine or cosine value based on quadrant bits returned by trig_red_f */
static inline float sinf_cosf_core(float a, int i)
{
  float s = a * a;
  float r;

  /* if low bit set -> use cosine polynomial; else use sine polynomial */
  if (i & 1) {
    r = cosf_poly(s);
  } else {
    r = sinf_poly(a, s);
  }

  /* bit 1 of quadrant indicates sign flip */
  if (i & 2) {
    /* keep NaN behavior: 0.0f - NaN is NaN, sign not changed for NaN */
    r = 0.0f - r;
  }
  return r;
}

/* sinf and cosf using your trig_red_f (Cody–Waite + Payne–Hanek hybrid) */
float sinf(float a)
{
  int i;
  /* preserve +inf -> NaN behavior and keep -0.0f */
  a = (a * 0.0f) + a;
  float r = trig_red_f(a, 71476.0625f, &i);
  return sinf_cosf_core(r, i);
}

float cosf(float a)
{
  int i;
  a = (a * 0.0f) + a;
  float r = trig_red_f(a, 71476.0625f, &i);
  /* cos(x) == sin(x + pi/2) so pass i+1 to core */
  return sinf_cosf_core(r, i + 1);
}

float atanf(float x)
{
  return asinf(x / sqrtf(1.0 + x * x));
}

float acosf(float x) {
  return (M_PI / 2.0) - asinf(x);
}

float atan2f(float y, float x)
{
  float result = 0.0;
  if (x > 0.0) {
    result = atanf(y / x);
  } else if (x < 0.0 && y >= 0.0) {
    result = atanf(y / x) + M_PI;
  } else if (x < 0.0 && y < 0.0) {
    result = atanf(y / x) - M_PI;
  } else if (x == 0.0 && y > 0.0) {
    result = M_PI / 2.0;
  } else if (x == 0.0 && y < 0.0) {
    result = -M_PI / 2.0;
  } else {
    // Undefined condition (x == 0, y == 0)
    result = NAN;
  }
  return result;
}
