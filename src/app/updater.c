#include "app.h"

void spawn_vehicle();

void app_update() {
	// check if app is not running
	if (!app.running) {
		printf(APP_FATAL_FN("app_update") "no 'app' instance running. aborting.");
		exit(APP_EXIT_FAILURE);
	}

	// process every command in the command_queue vector (treating it also as a queue)
	while (app.command_queue.length > 0) {
		enum __app_command_t__ *cmd = vector_get(&app.command_queue, 0, NULL);

		if (COMMAND_QUIT == *cmd) app_quit(APP_EXIT_SUCCESS);
		if (COMMAND_DEBUG == *cmd) {
			int x, y, w, h;
			SDL_GetMouseState(&x, &y);
			SDL_GetWindowSize(app.window, &w, &h);

			int pw = ((double) x / (double) w) * 100;
			int ph = ((double) y / (double) h) * 100;

			printf("cursor: (%d, %d) | (%d%%w, %d%%h)\n", x, y, pw, ph);
		}

		// remove the first item then process the next one
		vector_remove(&app.command_queue, 0, NULL);
	}
}
