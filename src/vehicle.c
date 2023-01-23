#include "vehicle.h"
#include "app.h"
#include <math.h>
#include <stdint.h>

void spawn_vehicle() {
	// reset timer
	app.spawn_cd = genrand_spawn_cd();

	// create a new vehicle
	vehicle_t *newv = malloc(sizeof(vehicle_t));

	// where this vehicle came from? where this vehicle is going?
	newv->from = random_int(ROAD_UP, ROAD_RIGHT + 1);
	newv->into = random_int(ROAD_UP, ROAD_RIGHT + 1);

	// change destination if its the same with source
	if (newv->into == newv->from) newv->into = (newv->into + 1) % 4;

	newv->progress = 0;
	update_vehicle_position(newv);

	int sw, sh;
	SDL_Rect vrect;
	SDL_GetWindowSize(app.window, &sw, &sh);

	vrect.x = 0;
	vrect.y = 0;
	vrect.w = 0.07 * sw;
	vrect.h = 0.10 * sh;

	SDL_Surface *vsurface = SDL_CreateRGBSurface(0, 0.07 * sw, 0.10 * sh, 32, 0, 0, 0, 0);
	SDL_FillRect(vsurface, &vrect, strtol(COLORSCHEME[FOREGROUND], NULL, 16));

	newv->texture = SDL_CreateTextureFromSurface(app.renderer, vsurface);
	SDL_FreeSurface(vsurface);

	vector_push(app.roads + newv->from, newv, NULL);
}

void move_vehicle(vehicle_t *v, uint i, uint j) {
	double speed = 0.005;
	int diff = v->from - v->into;

	if (
		(diff == 1 || diff == -3) &&
		((v->progress < 0.45 && v->progress > 0.40) || (v->progress < 0.60 && v->progress > 0.55))
	) speed = 0.0025;

	if (
		(diff == -1 || diff == 3) &&
		(v->progress < 0.60 && v->progress > 0.40)
	) speed = 0.01;

	double new_progress = v->progress + speed;

	if (
		(v->progress < 0.4 && new_progress >= 0.4) ||
		(v->progress < 0.6 && new_progress >= 0.6) ||
		(v->progress < 1.0 && new_progress >= 1.0)
	) {
		uint *k = calloc(sizeof(uint), 2);
		k[0] = i;
		k[1] = j;
		vector_push(&app.pending_vehicles, k, NULL);
	}

	v->progress = new_progress;
}

void update_vpos_start(vehicle_t *v);
void update_vpos_middle(vehicle_t *v);
void update_vpos_end(vehicle_t *v);
void update_vehicle_position(vehicle_t *v) {
	if (v->progress < 0.4) {
		update_vpos_start(v);
	} else if (v->progress < 0.6) {
		update_vpos_middle(v);
	} else {
		update_vpos_end(v);
	}
}

void update_vpos_start(vehicle_t *v) {
	double q[4][3] = {
		{0.45, v->progress, 180.0},
		{v->progress, 0.55, 90.0},
		{0.55, 1 - v->progress, 0.0},
		{1 - v->progress, 0.45, 270.0},
	};

	v->x = q[v->from][0];
	v->y = q[v->from][1];
	v->rotation = q[v->from][2];
}


void update_vpos_middle(vehicle_t *v) {
	int diff = v->from - v->into;

	// if straight
	if (abs(diff) % 2 == 0) {
		double p = v->progress - 0.4;
		double q[4][2] = {
			{0.05, p},
			{p, 0.15},
			{0.15, 0.20 - p},
			{0.20 - p, 0.05}
		};

		v->x = q[v->from][0] + 0.40;
		v->y = q[v->from][1] + 0.40;
	// else if adjacent
	} else if (diff == -1 || diff == 3) {
		double p = (v->progress - 0.4) / 0.2;
		double q[4][3] = {
			{0,0.4,0.4},
			{270,0.4,0.6},
			{180,0.6,0.6},
			{90,0.6,0.4},
		};

		v->rotation = 90 * p + q[v->from][0];
		v->x = 0.05 * cos(M_PI * 2 * (v->rotation / 360)) + q[v->from][1];
		v->y = 0.05 * sin(M_PI * 2 * (v->rotation / 360)) + q[v->from][2];
	// else if opposite (other side)
	} else {
		if (v->progress < 0.45) {
			double p = (v->progress - 0.40);
			double q[4][2] = {
				{0.05, p * 2},
				{p * 2, 0.15},
				{0.15, 0.20 - (p * 2)},
				{0.20 - (p * 2), 0.05}
			};

			v->x = q[v->from][0] + 0.40;
			v->y = q[v->from][1] + 0.40;
		} else if (v->progress <= 0.55) {
			double p = (v->progress - 0.45) / 0.1;
			double q[4] = {180, 90, 0, 270};

			v->rotation = q[v->from] - 90 * p;
			v->x = 0.05 * cos(M_PI * 2 * (v->rotation / 360)) + 0.5;
			v->y = 0.05 * sin(M_PI * 2 * (v->rotation / 360)) + 0.5;
		} else {
			double p = v->progress - 0.55;
			double q[4][3] = {
				{0.55, 1 - (0.5 + p * 2), 0.0},
				{1 - (0.5 + p * 2), 0.45, 270.0},
				{0.45, 0.5 + p * 2, 180.0},
				{0.5 + p * 2, 0.55, 90.0},
			};

			v->x = q[v->into][0];
			v->y = q[v->into][1];
			v->rotation = q[v->into][2];
		}
	}
}

void update_vpos_end(vehicle_t *v) {
	double q[4][3] = {
		{0.55, 1 - v->progress, 0.0},
		{1 - v->progress, 0.45, 270.0},
		{0.45, v->progress, 180.0},
		{v->progress, 0.55, 90.0},
	};

	v->x = q[v->into][0];
	v->y = q[v->into][1];
	v->rotation = q[v->into][2];
}

void render_vehicle(vehicle_t *v, int sw, int sh) {
	double cx = v->x * sw;
	double cy = v->y * sh;
	double vw = 0.07 * sw;
	double vh = 0.10 * sh;

	SDL_Rect vrect;
	vrect.x = cx - (vw / 2.0);
	vrect.y = cy - (vh / 2.0);
	vrect.w = vw;
	vrect.h = vh;

	SDL_Point vcenter = {vw / 2, vh / 2};

	// set color to background
	uint8_t color[3];
	hextocolor((char *) COLORSCHEME[BACKGROUND], color);
	SDL_SetRenderDrawColor(app.renderer, color[0], color[1], color[2], 255);

	SDL_RenderCopyEx(
		app.renderer,
		v->texture,
		NULL,
		&vrect,
		v->rotation,
		&vcenter,
		SDL_FLIP_NONE
	);

	SDL_RenderDrawPoint(app.renderer, cx, cy);
}
