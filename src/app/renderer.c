#include <SDL2/SDL.h>
#include <stdlib.h>

#include "app.h"

void app_render() {
	if (!app.running) {
		printf(APP_FATAL_FN("app_render") "no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
	}

	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
	SDL_RenderClear(app.renderer);
	SDL_RenderPresent(app.renderer);
}
