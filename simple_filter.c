#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define Q (2)
#define PI (3.14159265358979323846)
#define TWO_PI (2 * PI)

#include "simple_filter.h"

typedef struct {
  double sample_rate;

  const float* frequency;
  FilterType type;

  float del_in_left[2];
  float del_in_right[2];
  float del_out_left[2];
  float del_out_right[2];

  float* out_left;
  float* out_right;

  const float* in_left;
  const float* in_right;
} SimpleFilter;

static void
render_samples(uint32_t from, uint32_t to, SimpleFilter* self) {
  float* const out_left  = self->out_left;
  float* const out_right  = self->out_right;

  const float* in_left  = self->in_left;
  const float* in_right  = self->in_right;

  float lambda = 1.0 / tan(PI * ((*self->frequency) / self->sample_rate));

  float a_0 = 1.0 / (1.0 + 2 * lambda + lambda * lambda);
  float a_1 = 2 * a_0;
  float a_2 = a_0;
  float b_1 = 2 * a_0 * (1.0 - lambda * lambda);
  float b_2 = a_0 * (1.0 - 2 * lambda + lambda * lambda);

  for (uint32_t pos = from; pos < to; pos++) {
    out_left[pos]  = a_0 *  in_left[pos] + a_1 *  self->del_in_left[0] + a_2 *  self->del_in_left[1] - b_1 *  self->del_out_left[0] - b_2 *  self->del_out_left[1];
    out_right[pos] = a_0 * in_right[pos] + a_1 * self->del_in_right[0] + a_2 * self->del_in_right[1] - b_1 * self->del_out_right[0] - b_2 * self->del_out_right[1];

    self->del_in_left[1]  = self->del_in_left[0];
    self->del_in_right[1] = self->del_in_right[0];

    self->del_in_left[0]  = in_left[pos];
    self->del_in_right[0] = in_right[pos];

    self->del_out_left[1]  = self->del_out_left[0];
    self->del_out_right[1] = self->del_out_right[0];

    self->del_out_left[0]  = out_left[pos];
    self->del_out_right[0] = out_right[pos];
  }
}

/* -----------------
 * LV2 Audio functions
 * See: http://lv2plug.in/doc/html/group__lv2core.html#structLV2__Descriptor
 * -----------------
 */

static LV2_Handle
instantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               bundle_path,
            const LV2_Feature* const* features)
{
  SimpleFilter* self = (SimpleFilter*)malloc(sizeof(SimpleFilter));

  self->sample_rate = rate;

  self->type = LOWPASS;

  self->del_in_left[0] = 0;
  self->del_in_left[1] = 0;
  self->del_in_right[0] = 0;
  self->del_in_right[1] = 0;

  self->del_out_left[0] = 0;
  self->del_out_left[1] = 0;
  self->del_out_right[0] = 0;
  self->del_out_right[1] = 0;

  return (LV2_Handle)self;
}

static void
connect_port(LV2_Handle instance,
             uint32_t   port,
             void*      data)
{
  SimpleFilter* self = (SimpleFilter*)instance;

  switch ((PortIndex)port) {
  case PORT_IN_LEFT:
    self->in_left = (const float*)data;
    break;
  case PORT_IN_RIGHT:
    self->in_right = (const float*)data;
    break;
  case PORT_OUT_LEFT:
    self->out_left = (float*)data;
    break;
  case PORT_OUT_RIGHT:
    self->out_right = (float*)data;
    break;
  case PORT_FREQUENCY:
    self->frequency = (const float*)data;
    break;
  case PORT_FILTER_TYPE:
  {
    float* type = (float*)data;
    if (type != NULL) {
      self->type = (FilterType)(*type);
    }
    break;
  }
  }
}

/*
 * LV2 Audio function, should not allocate resources to be Real-Time
 * capable
 */
static void
activate(LV2_Handle instance)
{
}

static void
run(LV2_Handle instance, uint32_t n_samples)
{
  SimpleFilter* self = (SimpleFilter*)instance;

  render_samples(0, n_samples, self);
}

/*
 * Free resources allocated on activate()
 */
static void
deactivate(LV2_Handle instance)
{
}

/*
 * Free resources allocated on instantiate()
 */
static void
cleanup(LV2_Handle instance)
{
  SimpleFilter* self = (SimpleFilter*)instance;

  free(self);
}

const void*
extension_data(const char* uri)
{
  return NULL;
}

static const LV2_Descriptor descriptor = {
  SIMPLE_FILTER_URI,
  instantiate,
  connect_port,
  activate,
  run,
  deactivate,
  cleanup,
  extension_data
};

LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
  switch (index) {
  case 0:
    return &descriptor;
  default:
    return NULL;
  }
}
