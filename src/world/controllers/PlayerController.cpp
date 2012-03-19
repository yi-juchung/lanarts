/*
 * PlayerController.cpp
 *
 *  Created on: Feb 20, 2012
 *      Author: 100397561
 */

#include "PlayerController.h"
#include "../GameTiles.h"
#include "../GameState.h"

PlayerController::PlayerController() {
}

PlayerController::~PlayerController() {
}

void PlayerController::pre_step(GameState* gs) {
	for (int i = 0; i < pids.size(); i++) {
		GameInst* player = gs->get_instance(pids[i]);
		int sx = player->last_x * VISION_SUBSQRS / TILE_SIZE;
		int sy = player->last_y * VISION_SUBSQRS / TILE_SIZE;
		fovs[i]->calculate(gs, sx, sy);
	}
}

void PlayerController::register_player(obj_id player) {
	local_player = player;
	pids.push_back(player);
	fovs.push_back(new fov(7, VISION_SUBSQRS));
}

void PlayerController::remove_player(obj_id player) {
	int i;
	for (i = 0; i < pids.size() && pids[i] != player; i++) {
		//find 'i' such that pids[i] == player
	}
	if (i == pids.size())
		return;
	pids.erase(pids.begin() + i);
	delete fovs[i];
	fovs.erase(fovs.begin() + i);
}

static int squish(int a, int b, int c) {
	return std::min(std::max(a, b), c - 1);
}

bool PlayerController::seen_by_player(GameState* gs, int pindex,
		GameInst* obj) {
	const int sub_sqrs = VISION_SUBSQRS;
	const int subsize = TILE_SIZE / sub_sqrs;

	int w = gs->width() / subsize, h = gs->height() / subsize;
	int x = obj->last_x, y = obj->last_y;
	int rad = obj->radius;
	int mingrid_x = (x - rad) / subsize, mingrid_y = (y - rad) / subsize;
	int maxgrid_x = (x + rad) / subsize, maxgrid_y = (y + rad) / subsize;
	int minx = squish(mingrid_x, 0, w), miny = squish(mingrid_y, 0, h);
	int maxx = squish(maxgrid_x, 0, w), maxy = squish(maxgrid_y, 0, h);
	fov& pfov = *fovs[pindex];
	//printf("minx=%d,miny=%d,maxx=%d,maxy=%d\n",minx,miny,maxx,maxy);

	for (int yy = miny; yy <= maxy; yy++) {
		for (int xx = minx; xx <= maxx; xx++) {
			if (pfov.within_fov(xx, yy))
				return true;
		}
	}
	return false;
}

void PlayerController::clear(){
	pids.clear();
	fovs.clear();
}