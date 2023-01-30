#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdbool.h>
#include <stdlib.h>

struct __config_t__ {
	double spawn_cd;
	double spawn_rand;
	double duration;
	uint max_vehicles;
	bool debug;
	bool automatic;
};

#endif
