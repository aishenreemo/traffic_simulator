#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdlib.h>

struct __config_t__ {
	double spawn_cd; // spawn cooldown
	double spawn_rand; // spawn interval randomness
	double duration; // duration till program terminate
	uint max_vehicles;
};

#endif
