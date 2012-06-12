/*
 * enemygen.cpp
 *
 *  Created on: Mar 20, 2012
 *      Author: 100397561
 */

#include "../util/math_util.h"

#include "../world/objects/EnemyInst.h"

#include "../world/GameState.h"
#include "../world/GameTiles.h"

#include "enemygen.h"

void post_generate_enemy(GameState* gs, enemy_id etype, int amount) {
	MTwist& mt = gs->rng();
	GameTiles& tiles = gs->tile_grid();

	for (int i = 0; i < amount; i++) {
		Pos epos;
		int tries = 0;
		do {
			int rand_x = mt.rand(tiles.tile_width());
			int rand_y = mt.rand(tiles.tile_height());
			epos = centered_multiple(Pos(rand_x, rand_y), TILE_SIZE);
		} while (gs->solid_test(NULL, epos.x, epos.y, 15));
		gs->add_instance(new EnemyInst(etype, epos.x, epos.y));
	}
}

int generate_enemy(const EnemyGenChance& ec, MTwist& mt, GeneratedLevel& level,
		GameState* gs, int amount) {

	enemy_id etype = ec.enemytype;

	GameTiles& tiles = gs->tile_grid();
	int start_x = (tiles.tile_width() - level.width()) / 2;
	int start_y = (tiles.tile_height() - level.height()) / 2;

	int room = mt.rand(level.rooms().size());
	Region r = level.rooms()[room].room_region;

	for (int i = 0; i < amount; i++) {
		Pos epos;
		int tries = 0;
		do {
			if (tries++ < 20) {
				epos = generate_location_in_region(mt, level, r);
			} else
				epos = generate_location(mt, level);
		} while (level.at(epos).near_entrance);
		level.at(epos).has_instance = true;

		epos.x += start_x, epos.y += start_y;

		Pos world_pos = centered_multiple(epos, TILE_SIZE);
		gs->add_instance(new EnemyInst(etype, world_pos.x, world_pos.y));
	}
	return amount;
}

static int get_total_chance(const EnemyGenSettings& rs) {
	int total_chance = 0;
	for (int i = 0; i < rs.enemy_chances.size(); i++) {
		total_chance += rs.enemy_chances[i].genchance;
	}
	return total_chance;
}

//Generates enemy monsters
void generate_enemies(const EnemyGenSettings& rs, MTwist& mt,
		GeneratedLevel& level, GameState* gs) {

	int nmons = mt.rand(rs.num_monsters);
	int total_chance = get_total_chance(rs);

	for (int i = 0; i < rs.enemy_chances.size(); i++) {
		const EnemyGenChance& ec = rs.enemy_chances[i];
		if (ec.guaranteed > 0) {
			generate_enemy(ec, mt, level, gs, ec.guaranteed);
		}
	}

	if (total_chance == 0)
		return;
	for (int i = 0; i < nmons;) {
		int monster_roll = mt.rand(total_chance);
		int monstn;
		for (monstn = 0; monstn < rs.enemy_chances.size(); monstn++) {
			monster_roll -= rs.enemy_chances[monstn].genchance;
			if (monster_roll < 0)
				break;
		}

		const EnemyGenChance& ec = rs.enemy_chances[monstn];

		int amnt = 1;
		if (mt.rand(100) < ec.groupchance) {
			amnt = mt.rand(ec.groupsize);
		}

		i += generate_enemy(ec, mt, level, gs, amnt);
	}
}
