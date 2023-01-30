#ifndef __LIGHTS_H__
#define __LIGHTS_H__

#include <stdbool.h>
#include <stdlib.h>

enum __light_variant_t__ {
	TL_STOP,
	TL_SLOW,
	TL_GO,
};

struct __light_t__ {
	bool stop;
	double cooldown;
	enum __light_variant_t__ variant;
};

void render_traffic_light(uint, int, int);

#endif // __LIGHTS_H__
