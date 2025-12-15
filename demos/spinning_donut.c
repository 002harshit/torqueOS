#include "torus.stl.c"
#include "libcrank/std.h"
#include "libcrank/math.h"
#include "kernel/allocator.h"
#include "driver/timer.h"
#include "driver/kb_ps2.h"
#include "driver/gfx.h"

#define NEAR 0.001f
#define FAR 100.0f

float angle = 0;
gfx_color_t bg_color = {18, 18, 24};
mat4_t model, view, proj, mvp, model_inv_transpose;

vec3_t light_color = {0.8, 0.8, 0.8};
vec3_t ambient = {0.01, 0.01, 0.01};
vec3_t obj_color = {0.2, 0.8, 0.9};
float specular_strength = 0.8;
vec3_t view_pos = {0, 0, 3};
vec3_t light_pos = {0, 2, 10};

float* depth_buffer;

int width, height;

void draw_triangle(const float triangle[4][3]);

static int should_exit;
static void update_key_callback(unsigned char keycode, char is_released)
{
  should_exit = SCANCODE_LCTRL == keycode;
}

void spinning_donut_demo()
{
  width = _gfx.width;
  height = _gfx.height;
  depth_buffer = allocator_malloc(sizeof(float) * width * height);
	view = mat4_lookat_lh(view_pos, vec3_zero(), (vec3_t){0, 1, 0});
	proj = mat4_perspective(deg_to_rad(70), (float) width / height, NEAR, FAR);

  kb_set_callback(update_key_callback);
  timer_stop();
  timer_start(60);
  should_exit = 0;

  unsigned long ticks_prev = timer_get_elapsed();
  
  printf("[INFO] Press Left-Control to get out of spinning donut demo\n");
  while (!should_exit) {
    unsigned long ticks_now = timer_get_elapsed();
    float delta_ms = (ticks_now - ticks_prev) / 1000.0;
    ticks_prev = ticks_now;

    angle += 2 * delta_ms;
    gfx_clear(bg_color);
    for (int k = 0; k < width * height; k++)
      depth_buffer[k] = 1000000;
    model = mat4_rotate_axis((vec3_t){1, 1, 1}, angle);
    mvp = mat4_mul(mat4_mul(model, view), proj);

    model_inv_transpose = mat4_transpose(mat4_inverse(model));

    for (size_t i = 0; i < (size_t)torus_triangles_count; i++) {
      draw_triangle(torus_data[i]);
    }
    gfx_flush();
  }
  allocator_free(depth_buffer);
  printf("[INFO] Exited out of spinning donut loop\n");
}

vec3_t viewport(vec3_t v)
{
  return (vec3_t){(v.x + 1) * width / 2, (v.y + 1) * height / 2, v.z};
}

static inline float cal_depth(vec3_t v)
{
  return ((1.0 / v.z) + 1.0) / 2.0; // from 0 to 1 maybe????
}

static inline void _bresenham_low(vec2_t p0, vec2_t p1, gfx_color_t color)
{
  vec2_t d = vec2_sub(p1, p0);
  float yi = 1.0;
  if (d.y < 0) {
    yi = -1.0;
    d.y = -d.y;
  }
  float D = (2 * d.y) - d.x;
  float y = p0.y;
  for (float x=p0.x;x< p1.x;x++) {
    gfx_setpixel((int)x, (int)y, color);
    if (D > 0) {
      y = y + yi;
      D = D + (2 * (d.y - d.x));
    } else {
      D = D + 2 * d.y;
    }
  }
}

static inline void _bresenham_high(vec2_t p0, vec2_t p1, gfx_color_t color)
{
  vec2_t d = vec2_sub(p1, p0);
  float xi = 1.0;
  if (d.x < 0) {
    xi = -1.0;
    d.x = -d.x;
  }
  float D = (2 * d.x) - d.y;
  float x = p0.x;
  for (float y=p0.y; y < p1.y; y++) {
    gfx_setpixel((int)x, (int)y, color);
    if (D > 0) {
      x = x + xi;
      D = D + (2 * (d.x - d.y));
    } else {
      D = D + 2 * d.x;
    }
  }
}

// Bresenham line algorithm
// Reference: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
static void draw_line(float ax, float ay, float bx, float by, gfx_color_t color)
{
  // compiler with hopefully optimize this shit
  vec2_t p0 = {ax, ay};
  vec2_t p1 = {bx, by};
  if (fabsf(p1.y - p0.y) < fabsf(p1.x - p0.x)) {
    if (p0.x  > p1.x) 
      _bresenham_low(p1, p0, color);
    else
      _bresenham_low(p0, p1, color); 
  }
  else {
    if (p0.y > p1.y) 
      _bresenham_high(p1, p0, color);
    else
      _bresenham_high(p0, p1, color);
  }
}


static float signed_triangle_area(float ax, float ay, float bx, float by, float cx, float cy)
{
  return .5*((by-ay)*(bx+ax) + (cy-by)*(cx+bx) + (ay-cy)*(ax+cx));
}

void blit_triangle(vec3_t a, vec3_t b, vec3_t c, vec3_t verts[3], vec3_t verts_color[3])
{
  vec3_t bound_min = vec3_min(vec3_min(a, b), c);
  vec3_t bound_max = vec3_max(vec3_max(a, b), c);

  float total_area = signed_triangle_area(a.x, a.y, b.x, b.y, c.x, c.y);

  // backface culling
  if (total_area < 1) {
    return;
  }

  for (int x = bound_min.x; x <= bound_max.x; x+=1.0) {
    for (int y = bound_min.y; y <= bound_max.y; y+=1.0) {
      // barycentric coords
      vec3_t point = {x, y, 0};
      vec3_t bary = vec3_barycentric(point, a, b, c);
      if (bary.x < 0 || bary.y < 0 || bary.z < 0)
        continue;
      // do fragment shader stuff
      float depth = bary.x * cal_depth(verts[0]) + bary.y * cal_depth(verts[1]) + bary.z * cal_depth(verts[2]);

      if (depth >= depth_buffer[y * width + x]) {
        continue;
      }

      vec3_t v_color = vec3_zero();
      v_color = vec3_add(v_color, vec3_mulv(verts_color[0], bary.x));
      v_color = vec3_add(v_color, vec3_mulv(verts_color[1], bary.y));
      v_color = vec3_add(v_color, vec3_mulv(verts_color[2], bary.z));
      v_color = vec3_mulv(v_color, 255);
      gfx_color_t color = {v_color.x, v_color.y, v_color.z};

      depth_buffer[y * width + x] = depth;
      gfx_setpixel(x, y, color);
    }
  }
}



void draw_triangle(const float triangle[4][3])
{
  vec3_t verts[3];
  vec3_t norm = vec3_normalize(vec3(triangle[0][0], triangle[0][1], triangle[0][2]));
  norm = vec3_normalize(vec3_transform(norm, mat4_transpose(mat4_inverse(model))));
  for (int i = 0; i < 3; i++) {
    verts[i] = (vec3_t){triangle[i+1][0], triangle[i+1][1], triangle[i+1][2]};
  }

  vec3_t final[3];
  vec3_t verts_color[3];
  // do vertex shader stuff

  for (int i = 0; i < 3; i++) {
    vec3_t frag_pos = vec3_transform(verts[i], model);
    vec3_t light_dir = vec3_normalize(vec3_sub(light_pos, frag_pos));

    float diff = fmaxf(vec3_dot(norm, light_dir), 0.0);
    vec3_t diffuse = vec3_mulv(light_color, diff);
    vec3_t view_dir = vec3_normalize(vec3_sub(view_pos,frag_pos));
    vec3_t reflect_dir = vec3_reflect(vec3_mulv(light_dir, -1), norm);
    float spec = powf(fmaxf(vec3_dot(view_dir, reflect_dir), 0.0), 4);
    vec3_t specular = vec3_mulv(light_color, specular_strength * spec);
    verts_color[i] =  vec3_mul(vec3_add(vec3_add(ambient, diffuse), specular), obj_color);
    verts_color[i] = vec3_clamp(verts_color[i], 0, 1);
    verts[i] = vec3_transform(verts[i], mvp);
    final[i] = viewport(vec3_mulv(verts[i], 1.0 / verts[i].z));

  }

  gfx_color_t white = {255, 255, 255};
  draw_line(final[0].x, final[0].y, final[1].x, final[1].y, white);
  draw_line(final[0].x, final[0].y, final[2].x, final[2].y, white);
  draw_line(final[1].x, final[1].y, final[2].x, final[2].y, white);
  return;

  // blit_triangle(final[0], final[1], final[2], verts, verts_color);


  // TODO: find bounding box
  vec3_t boundMin = vec3_max(vec3_min(vec3_min(final[0], final[1]), final[2]), vec3_zero());
  vec3_t boundMax = vec3_min(vec3_max(vec3_max(final[0], final[1]), final[2]), vec3(width, height, 0));

  for (unsigned int y = boundMin.y; y < boundMax.y; y++) {
    for (unsigned int x = boundMin.x; x < boundMax.x; x++) {
      vec3_t point = {x, y, 0};
      vec3_t bary = vec3_barycentric(point, final[0], final[1], final[2]);
      if (bary.x < 0 || bary.y < 0 || bary.z < 0)
        continue;
      // do fragment shader stuff
      float depth = bary.x * cal_depth(verts[0]) + bary.y * cal_depth(verts[1]) + bary.z * cal_depth(verts[2]);
      float curr_depth = depth_buffer[y * width + x];
      if (depth < curr_depth) {
        depth_buffer[y * width + x] = depth;
        vec3_t v_color = vec3_zero();
        v_color = vec3_add(v_color, vec3_mulv(verts_color[0], bary.x));
        v_color = vec3_add(v_color, vec3_mulv(verts_color[1], bary.y));
        v_color = vec3_add(v_color, vec3_mulv(verts_color[2], bary.z));
        v_color = vec3_mulv(v_color, 255);
        gfx_color_t color = {v_color.x, v_color.y, v_color.z};
        gfx_setpixel(x, y, color);
      }
    }
  }
}
