#include <stdint.h>

#include "lights.h"
#include "app.h"

void render_traffic_light(uint i, int sw, int sh) {

	double rects[4][2] = {
		{0.30, 0.275},
		{0.30, 0.625},
		{0.625, 0.625},
		{0.625, 0.275},
	};

	SDL_Rect tl_rect;
	tl_rect.x = rects[i][0] * sw;
	tl_rect.y = rects[i][1] * sh;
	tl_rect.w = 0.07 * sw;
	tl_rect.h = 0.12 * sh;

	uint8_t color[3];
	hextocolor((char *) COLORSCHEME[COLOR_08], color);
	SDL_SetRenderDrawColor(app.renderer, color[0], color[1], color[2], 255);
	SDL_RenderFillRect(app.renderer, &tl_rect);

	SDL_Rect lights_rect[3];
	enum __colorscheme_t__ colors[3] = {COLOR_01, COLOR_03, COLOR_02};
	for (uint j = 0; j < 3; j++) {
		lights_rect[j].x = tl_rect.x + 0.025 * sw;
		lights_rect[j].y = tl_rect.y + ((0.03 * j) + 0.020) * sh;
		lights_rect[j].w = 0.025 * sw;
		lights_rect[j].h = 0.025 * sh;

		hextocolor((char *) COLORSCHEME[colors[j]], color);
		SDL_SetRenderDrawColor(app.renderer, color[0], color[1], color[2], 255);

		if (app.lights[i] == j) {
			SDL_RenderFillRect(app.renderer, lights_rect + j);
		} else {
			SDL_RenderDrawRect(app.renderer, lights_rect + j);
		}
	};
}
