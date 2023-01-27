#include <libcollections/vector.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "app.h"

struct __app_t__ app;

void *app_event_thread(void *);

void app_init() {
	if (app.running) {
		printf(APP_WARN_FN("app_init") "cannot initialize 'app' twice. skipping\n");
		return;
	}

	srand((uint) time(NULL));

	// :)
	app.running = true;

	// initialize config
	app.config.spawn_cd = 0.75;
	app.config.spawn_rand = 0.5;
	app.config.duration = 60.0;
	app.config.max_vehicles = 100;
	app.config.debug = true;

	// initialize vectors
	vector_init(&app.sdl_event_queue, NULL);
	vector_init(&app.command_queue, NULL);

	// initialize spawn cooldown
	app.last_update = time_milliseconds();
	app.duration_cd = app.config.duration;
	app.spawn_cd = genrand_spawn_cd();

	// initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf(APP_FATAL "error initializing SDL... %s\n", SDL_GetError());
		app_quit(APP_EXIT_FAILURE);
	}

	// window
	app.window = SDL_CreateWindow(
		"Traffic Simulator",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                800, 600,
		SDL_WINDOW_RESIZABLE
	);

	// renderer
	app.renderer = SDL_CreateRenderer(
		app.window,
		-1,
		SDL_RENDERER_ACCELERATED
	);

	// get screen width and height
	int sw, sh;
	SDL_GetWindowSize(app.window, &sw, &sh);

	// initialize vehicles
	for (uint i = 0; i < 9; i++) vector_init(app.roads + i, NULL);
	vector_init(&app.pending_vehicles, NULL);

	// initialize traffic lights

	// initialize threads
	pthread_create(&app.event_thread, NULL, app_event_thread, NULL);
}

void app_quit(enum __app_exit_code_t__ exit_code) {
	// check if app is not running
	if (!app.running) {
		printf(APP_FATAL_FN("app_quit") "no 'app' instance running. aborting.\n");
		exit(APP_EXIT_FAILURE);
	}

	// :(
	app.running = false;

	// cancel threads
	pthread_cancel(app.event_thread);

	// deallocate vectors
	vector_drop(&app.sdl_event_queue);
	vector_drop(&app.command_queue);
	vector_drop(&app.pending_vehicles);

	for (uint i = 0; i < 5; i++) {
		for (uint j = 0; j < app.roads[i].length; j++) {
			vehicle_t *v = vector_get(app.roads + i, j, NULL);
			SDL_DestroyTexture(v->texture);
		}

		vector_drop(app.roads + i);
	}

	// deallocate SDL
	SDL_DestroyRenderer(app.renderer);
	SDL_DestroyWindow(app.window);

	// abort program or don't
	if ((int) exit_code > APP_EXIT_IGNORE) {
		printf("exiting....\n");
		exit((int) exit_code);
	}
}

void *app_event_thread(void *_) {
	// separate thread that accept events
	// check if app is not running
	if (!app.running) {
		printf(
			APP_FATAL_FN("app_event_thread")
			"no 'app' instance running. aborting."
		);
		exit(APP_EXIT_FAILURE);
	}

	while (true) {
		SDL_Event *event = malloc(sizeof(SDL_Event));
		SDL_WaitEvent(event); // wait for an event

		// push that event to a queue where `app_listen` reads it
		pthread_mutex_lock(&app.event_lock);
		vector_push(&app.sdl_event_queue, event, NULL);
		pthread_mutex_unlock(&app.event_lock);
	}

	return NULL;
}
