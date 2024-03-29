#ifndef __VEHICLE_H__
#define __VEHICLE_H__

#include <SDL2/SDL_render.h>
#include <stdlib.h>
#include "color.h"

enum __road_direction_t__ {
	ROAD_UP,
	ROAD_LEFT,
	ROAD_DOWN,
	ROAD_RIGHT,
};

struct __vehicle_t__ {
	double x;
	double y;
	double progress;
	double rotation;
	double speed;
	double width;
	double height;
	enum __colorscheme_t__ color;
	enum __road_direction_t__ from;
	enum __road_direction_t__ into;
	SDL_Texture *texture;
};

void spawn_vehicle();
void move_vehicle(struct __vehicle_t__*, uint, uint, int, int);
void render_vehicle(struct __vehicle_t__*, int, int);
void update_vehicle_position(struct __vehicle_t__*);

#endif // __VEHICLE_H__
