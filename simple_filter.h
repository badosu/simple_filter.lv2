#ifndef SIMPLE_FILTER
#define SIMPLE_FILTER

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#include "lv2/lv2plug.in/ns/ext/atom/atom.h"
#include "lv2/lv2plug.in/ns/ext/atom/util.h"
#include "lv2/lv2plug.in/ns/ext/midi/midi.h"
#include "lv2/lv2plug.in/ns/ext/urid/urid.h"

#define SIMPLE_FILTER_URI "http://bado.so/plugins/simple_filter"

typedef enum {
  PORT_IN_LEFT = 0,
  PORT_IN_RIGHT,
  PORT_OUT_LEFT,
  PORT_OUT_RIGHT,
  PORT_FREQUENCY,
  PORT_FILTER_TYPE
} PortIndex;

typedef enum {
  LOWPASS = 0,
  HIGHPASS
} FilterType;

#endif
