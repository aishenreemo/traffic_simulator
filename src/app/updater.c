#include "app.h"
#include "lights.h"
#include <libcollections/vector.h>

void app_update() {
	// check if app is not running
	if (!app.running) {
		printf(APP_FATAL_FN("app_update") "no 'app' instance running. aborting.");
		exit(APP_EXIT_FAILURE);
	}

	double new_update = time_milliseconds();
	double diff = (new_update - app.last_update) / 1000;

	app.spawn_cd -= diff;
	app.duration_cd -= diff;

	app.last_update = new_update;

	// count the total amount of vehicles present
	uint vehicles_length = 0;
	for (uint i = 0; i < 9; i++) vehicles_length += app.roads[i].length;

	// spawn a vehicle if spawn timer runs out
	if (app.spawn_cd < 0.0 && vehicles_length < app.config.max_vehicles) spawn_vehicle();

	// exit the program if duration_cd runs out
	if (app.duration_cd < 0.0) app_quit(APP_EXIT_SUCCESS);

	for (uint i = 0; i < 4; i++) {
		if (app.lights[i].variant != TL_SLOW) {
			app.lights[i].cooldown -= diff;
			continue;
		}
		app.lights[i].slow_cd -= diff;

		if (app.lights[i].slow_cd >= 0) continue;
		app.lights[i].slow_cd = 0;
		app.lights[i].cooldown = app.lights[i].stop ? 5.0 : 2.0;
		app.lights[i].variant = app.lights[i].stop ? TL_STOP : TL_GO;
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

		if (*cmd >= TOGGLE_LIGHT_UP && *cmd <= TOGGLE_LIGHT_RIGHT && !app.config.automatic) {
			uint i = *cmd - TOGGLE_LIGHT_UP;

			app.lights[i].variant = TL_SLOW;
			app.lights[i].stop = !app.lights[i].stop;
			app.lights[i].slow_cd = 0.5;
		}

		// remove the first item then process the next one
		vector_remove(&app.command_queue, 0, NULL);
	}

	int sw, sh;
	SDL_GetWindowSize(app.window, &sw, &sh);
	for (uint i = 0; i < 9; i++) {
		for (uint j = 0; j < app.roads[i].length; j++) {
			vehicle_t *v = vector_get(app.roads + i, j, NULL);
			move_vehicle(v, i, j, sw, sh);
			update_vehicle_position(v);
		}
	}

	while (app.pending_vehicles.length > 0) {
		uint *k = vector_get(&app.pending_vehicles, app.pending_vehicles.length - 1, NULL);
		uint i = k[0];
		uint j = k[1];

		vehicle_t *v = vector_get(app.roads + i, j, NULL);
		vehicle_t *v_copy = malloc(sizeof(vehicle_t));

		*v_copy = *v;
		vector_remove(app.roads + i, j, NULL);

		if (v_copy->progress < 1.0 && v_copy->progress >= 0.6) {
			vector_push(app.roads + v_copy->into + 5, v_copy, NULL);
		} else if (v->progress < 0.6 && v_copy->progress >= 0.4) {
			vector_push(app.roads + 4, v_copy, NULL);
		} else {
			SDL_DestroyTexture(v_copy->texture);
			free(v_copy);
		}

		vector_remove(&app.pending_vehicles, app.pending_vehicles.length - 1, NULL);
	}

	if (app.config.automatic) {
		uint highest = 5;
		double highest_cost = -1;
		for (uint i = 0; i < 4; i++) {
			if (app.roads[i].length == 0) continue;

			double vcost = app.roads[i].length;
			vehicle_t *v = vector_get(app.roads + i, 0, NULL);
			int diff = v->from - v->into;

			if (diff == 1 || diff == -3) vcost += 1;
			if (abs(diff) % 2 == 0) vcost += 2;
			if (diff == -1 || diff == 3) vcost += 3;

			double distances[4] = {0.4 - v->y, 0.4 - v->x, v->y - 0.6, v->x - 0.6};

			vcost -= 3 * distances[i];

			if (highest_cost > vcost) continue;
			highest_cost = vcost;
			highest = i;
		}

		for (uint i = 0; i < 4; i++) {
			if (app.lights[i].stop || i == highest) continue;
			if (app.lights[i].cooldown > 0) continue;
			app.lights[i].variant = TL_SLOW;
			app.lights[i].stop = true;
			app.lights[i].slow_cd = 0.5;
		}

		if (highest != 5 && app.lights[highest].variant == TL_STOP) {
			if (app.lights[highest].cooldown > 0) return;
			app.lights[highest].variant = TL_SLOW;
			app.lights[highest].stop = false;
			app.lights[highest].slow_cd = 0.5;
		}

	}
}
