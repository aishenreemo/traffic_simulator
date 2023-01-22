#include <sys/time.h>
#include "app.h"

double time_milliseconds(void) {
	struct timeval tv;
	gettimeofday(&tv, NULL);

	return (double) ((tv.tv_sec) * 1000) + ((double) tv.tv_usec / 1000);
}

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
