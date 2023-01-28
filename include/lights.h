#ifndef __LIGHTS_H__
#define __LIGHTS_H__

#include <stdlib.h>

enum __lights_t__ {
	TL_STOP,
	TL_SLOW,
	TL_GO,
};

void render_traffic_light(uint, int, int);

#endif // __LIGHTS_H__
