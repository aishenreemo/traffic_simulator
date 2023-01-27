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

	// vehicle characteristics
	newv->color = random_int(COLOR_09, COLOR_15 + 1);
	newv->width = ((double) rand() / (double) RAND_MAX) * 0.02 + 0.060;
	newv->height = ((double) rand() / (double) RAND_MAX) * 0.04 + 0.080;
	newv->speed = ((double) rand() / (double) RAND_MAX) * 0.0020 + 0.0040;

	// change destination if its the same with source
	if (newv->into == newv->from) newv->into = (newv->into + 1) % 4;

	newv->progress = -0.25;
	update_vehicle_position(newv);

	int sw, sh;
	SDL_Rect vrect;
	SDL_GetWindowSize(app.window, &sw, &sh);

	vrect.x = 0;
	vrect.y = 0;
	vrect.w = newv->width * sw;
	vrect.h = newv->height * sh;

	SDL_Surface *vsurface = SDL_CreateRGBSurface(
		0,
		newv->width * sw,
		newv->height * sh,
		32, 0, 0, 0, 0
	);
	SDL_FillRect(vsurface, &vrect, strtol(COLORSCHEME[newv->color], NULL, 16));

	newv->texture = SDL_CreateTextureFromSurface(app.renderer, vsurface);
	SDL_FreeSurface(vsurface);

	vector_push(app.roads + newv->from, newv, NULL);
}

bool is_vehicle_colliding(vehicle_t *v1, vehicle_t *v2, uint i) {
	enum __road_direction_t__ dir = i > 4 ? (v1->into + 2) % 4 : v1->from;
	double moving_gap = 0.04;

	if (dir == ROAD_UP) {
		double v1y = v1->y + 0.05;
		double v2y = v2->y - 0.05;

		return (v2y - moving_gap) < v1y;
	} else if (dir == ROAD_DOWN) {
		double v1y = v1->y - 0.05;
		double v2y = v2->y + 0.05;

		return (v2y + moving_gap) > v1y;
	} else if (dir == ROAD_LEFT) {
		double v1x = v1->x + 0.05;
		double v2x = v2->x - 0.05;

		return (v2x - moving_gap) < v1x;
	} else {
		double v1x = v1->x - 0.05;
		double v2x = v2->x + 0.05;

		return (v2x + moving_gap) > v1x;
	}
};

void move_vehicle(vehicle_t *v, uint i, uint j, int sw, int sh) {
	double speed = v->speed;
	int diff = v->from - v->into;

	if (
		(diff == 1 || diff == -3) &&
		((v->progress < 0.45 && v->progress > 0.40) || (v->progress < 0.60 && v->progress > 0.55))
	) speed /= 2;

	if (
		(diff == -1 || diff == 3) &&
		(v->progress < 0.60 && v->progress > 0.40)
	) speed *= 2;

	if (i != 4 && j != 0) {
		vehicle_t *nv = vector_get(app.roads + i, j - 1, NULL);
		if (is_vehicle_colliding(v, nv, i)) return;
	} else if (i == 4 || (i < 4 && j == 0)) {

		double vr = fmod(v->rotation + 270.0, 360.0);
		SDL_Point vp[5];
		vp[0].x = (0.085 * cos(M_PI * 2 * vr / 360) + v->x) * sw;
		vp[0].y = (0.085 * sin(M_PI * 2 * vr / 360) + v->y) * sh;
		vp[1].x = (0.08 * cos(M_PI * 2 * (vr + 10) / 360) + v->x) * sw;
		vp[1].y = (0.08 * sin(M_PI * 2 * (vr + 10) / 360) + v->y) * sh;
		vp[2].x = (0.08 * cos(M_PI * 2 * (vr - 10) / 360) + v->x) * sw;
		vp[2].y = (0.08 * sin(M_PI * 2 * (vr - 10) / 360) + v->y) * sh;
		vp[3].x = (0.07 * cos(M_PI * 2 * (vr + 15) / 360) + v->x) * sw;
		vp[3].y = (0.07 * sin(M_PI * 2 * (vr + 15) / 360) + v->y) * sh;
		vp[4].x = (0.07 * cos(M_PI * 2 * (vr - 15) / 360) + v->x) * sw;
		vp[4].y = (0.07 * sin(M_PI * 2 * (vr - 15) / 360) + v->y) * sh;

		for (uint k = 0; k < app.roads[4].length; k++) {
			if (k == j && i == 4) continue;
			vehicle_t *nv = vector_get(app.roads + 4, k, NULL);

			double cx = nv->x * sw;
			double cy = nv->y * sh;

			SDL_Rect vhitbox;
			if (fmod(nv->rotation, 180) < 45 || fmod(nv->rotation, 180) > 135) {
				vhitbox.x = cx - (nv->width * sw / 2.0);
				vhitbox.y = cy - (nv->height * sh / 2.0);
				vhitbox.w = nv->width * sw;
				vhitbox.h = nv->height * sh;
			} else {
				vhitbox.x = cx - (nv->height * sw / 2.0);
				vhitbox.y = cy - (nv->width * sh / 2.0);
				vhitbox.w = nv->height * sw;
				vhitbox.h = nv->width * sh;
			}

			if (
				SDL_PointInRect(vp + 0, &vhitbox) ||
				SDL_PointInRect(vp + 1, &vhitbox) ||
				SDL_PointInRect(vp + 2, &vhitbox) ||
				SDL_PointInRect(vp + 3, &vhitbox) ||
				SDL_PointInRect(vp + 4, &vhitbox)
			) return;
		}

		if (app.roads[v->into].length > 0) {
			uint k = app.roads[v->into].length - 1;
			vehicle_t *nv = vector_get(app.roads + v->into, k, NULL);

			double cx = nv->x * sw;
			double cy = nv->y * sh;

			SDL_Rect vhitbox;
			if (fmod(nv->rotation, 180) < 45 || fmod(nv->rotation, 180) > 135) {
				vhitbox.x = cx - (nv->width * sw / 2.0);
				vhitbox.y = cy - (nv->height * sh / 2.0);
				vhitbox.w = nv->width * sw;
				vhitbox.h = nv->height * sh;
			} else {
				vhitbox.x = cx - (nv->height * sw / 2.0);
				vhitbox.y = cy - (nv->width * sh / 2.0);
				vhitbox.w = nv->height * sw;
				vhitbox.h = nv->width * sh;
			}

			if (
				SDL_PointInRect(vp + 0, &vhitbox) ||
				SDL_PointInRect(vp + 1, &vhitbox) ||
				SDL_PointInRect(vp + 2, &vhitbox) ||
				SDL_PointInRect(vp + 3, &vhitbox) ||
				SDL_PointInRect(vp + 4, &vhitbox)
			) return;
		}
	}

	double new_progress = v->progress + speed;

	if (
		(v->progress < 0.4 && new_progress >= 0.4) ||
		(v->progress < 0.6 && new_progress >= 0.6) ||
		(v->progress < 1.25 && new_progress >= 1.25)
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

		double r = 90.0 * p + q[v->from][0];
		v->rotation = fmod(r + 180.0, 360.0);
		v->x = 0.05 * cos(M_PI * 2 * (r / 360.0)) + q[v->from][1];
		v->y = 0.05 * sin(M_PI * 2 * (r / 360.0)) + q[v->from][2];
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
	double vw = v->width * sw;
	double vh = v->height * sh;

	SDL_Rect vrect;
	vrect.x = cx - (vw / 2.0);
	vrect.y = cy - (vh / 2.0);
	vrect.w = vw;
	vrect.h = vh;

	SDL_Point vcenter = {vw / 2, vh / 2};

	// set color to red
	uint8_t color[3];
	hextocolor((char *) COLORSCHEME[COLOR_01], color);
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

	if (!app.config.debug) return;

	SDL_Rect vhitbox;
	if (fmod(v->rotation, 180) < 45 || fmod(v->rotation, 180) > 135) {
		vhitbox.x = cx - (v->width * sw / 2.0);
		vhitbox.y = cy - (v->height * sh / 2.0);
		vhitbox.w = v->width * sw;
		vhitbox.h = v->height * sh;
	} else {
		vhitbox.x = cx - (v->height * sw / 2.0);
		vhitbox.y = cy - (v->width * sh / 2.0);
		vhitbox.w = v->height * sw;
		vhitbox.h = v->width * sh;
	}

	double vr = fmod(v->rotation + 270.0, 360.0);
	SDL_Point vp[5];
	vp[0].x = (0.085 * cos(M_PI * 2 * vr / 360) + v->x) * sw;
	vp[0].y = (0.085 * sin(M_PI * 2 * vr / 360) + v->y) * sh;
	vp[1].x = (0.08 * cos(M_PI * 2 * (vr + 10) / 360) + v->x) * sw;
	vp[1].y = (0.08 * sin(M_PI * 2 * (vr + 10) / 360) + v->y) * sh;
	vp[2].x = (0.08 * cos(M_PI * 2 * (vr - 10) / 360) + v->x) * sw;
	vp[2].y = (0.08 * sin(M_PI * 2 * (vr - 10) / 360) + v->y) * sh;
	vp[3].x = (0.07 * cos(M_PI * 2 * (vr + 15) / 360) + v->x) * sw;
	vp[3].y = (0.07 * sin(M_PI * 2 * (vr + 15) / 360) + v->y) * sh;
	vp[4].x = (0.07 * cos(M_PI * 2 * (vr - 15) / 360) + v->x) * sw;
	vp[4].y = (0.07 * sin(M_PI * 2 * (vr - 15) / 360) + v->y) * sh;

	SDL_RenderDrawRect(app.renderer, &vhitbox);
	SDL_RenderDrawLines(app.renderer, vp, 5);
}
