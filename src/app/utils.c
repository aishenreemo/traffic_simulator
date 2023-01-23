#include <sys/time.h>
#include <stdint.h>

#include "app.h"

double time_milliseconds(void) {
	struct timeval tv;
	gettimeofday(&tv, NULL);

	return (double) ((tv.tv_sec) * 1000) + ((double) tv.tv_usec / 1000);
}

double genrand_spawn_cd() {
	double min = app.config.spawn_cd - (app.config.spawn_rand / 2);
	return ((double) rand() / (double) RAND_MAX) * app.config.spawn_rand + min;
}

void hextocolor(char hex[7], uint8_t rgb[3]) {
	int n = (int) strtol(hex, NULL, 16);

	rgb[0] = ((n >> 16) & 0xFF);
	rgb[1] = ((n >> 8) & 0xFF);
	rgb[2] = ((n) & 0xFF);
};

int random_int(int min_num, int max_num) {
	int low_num = 0;
	int hi_num = 0;

	if (min_num < max_num) {
		low_num = min_num;
		hi_num = max_num; // include max_num in output
	} else {
		low_num = max_num; // include max_num in output
		hi_num = min_num;
	}

	return (rand() % (hi_num - low_num)) + low_num;
}
