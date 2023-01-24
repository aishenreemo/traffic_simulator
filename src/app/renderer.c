#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <stdlib.h>

#include "app.h"

void app_render() {
	if (!app.running) {
		printf(APP_FATAL_FN("app_render") "no 'app' instance running. aborting.");
		exit(EXIT_FAILURE);
	}

	int sw, sh;
	uint8_t color[3];
	SDL_Rect center_rect;
	SDL_Rect crossroad[4];
	SDL_Point dividers[4];

	// get screen width and height
	SDL_GetWindowSize(app.window, &sw, &sh);

	// set color to background
	hextocolor((char *) COLORSCHEME[BACKGROUND], color);
	SDL_SetRenderDrawColor(app.renderer, color[0], color[1], color[2], 255);
	SDL_RenderClear(app.renderer);

	// update crossroad/dividers
	for (int i = 0; i < 4; i++) {
		crossroad[i].x = (double) sw * (i % 2 ? 0 : 1.0 - 0.40);
		crossroad[i].y = (double) sh * (i < 2 ? 0 : 1.0 - 0.40);
		crossroad[i].w = (double) sw * 0.40;
		crossroad[i].h = (double) sh * 0.40;

		dividers[i].x = (double) sw * (i < 2 ? (i % 2 ? 0.0 : 1.0) : 0.5);
		dividers[i].y = (double) sh * (i < 2 ? 0.5 : (i % 2 ? 0.0 : 1.0));
	}

	center_rect.x = (double) sw * 0.40;
	center_rect.y = (double) sh * 0.40;
	center_rect.w = (double) sw * 0.20;
	center_rect.h = (double) sh * 0.20;

	// set color to green
	hextocolor((char *) COLORSCHEME[COLOR_02], color);
	SDL_SetRenderDrawColor(app.renderer, color[0], color[1], color[2], 255);

	// render rectangles that from the crossroad
	SDL_RenderFillRects(app.renderer, crossroad, 4);

	// set color to foreground
	hextocolor((char *) COLORSCHEME[FOREGROUND], color);
	SDL_SetRenderDrawColor(app.renderer, color[0], color[1], color[2], 255);

	// render dividers
	SDL_RenderDrawLines(app.renderer, dividers, 2);
	SDL_RenderDrawLines(app.renderer, dividers + 2, 2);

	// set color to background
	hextocolor((char *) COLORSCHEME[BACKGROUND], color);
	SDL_SetRenderDrawColor(app.renderer, color[0], color[1], color[2], 255);

	// render center_rect
	SDL_RenderFillRect(app.renderer, &center_rect);

	// set color to foreground
	hextocolor((char *) COLORSCHEME[FOREGROUND], color);
	SDL_SetRenderDrawColor(app.renderer, color[0], color[1], color[2], 255);
	for (uint i = 0; i < 9; i++) {
		// road
		for (uint j = 0; j < app.roads[i].length; j++) {
			vehicle_t *v = vector_get(&app.roads[i], j, NULL);
			render_vehicle(v, sw, sh);
		}
	}

	// present
	SDL_RenderPresent(app.renderer);
}
