/*
 * featuregen.cpp:
 *  Defines parameters for feature generation (eg tile generation, staircase placement),
 *  as well as the generate_features function
 */

#include "data/game_data.h"

#include "draw/SpriteEntry.h"
#include "draw/TileEntry.h"
#include "gamestate/GameMapState.h"

#include "gamestate/GameState.h"
#include "gamestate/GameTiles.h"

#include "gamestate/tileset_data.h"
#include "objects/store/StoreInst.h"
#include "objects/FeatureInst.h"

#include <lcommon/math_util.h>

#include "featuregen.h"
#include "itemgen.h"

/* Generate a random subtile for a tile */
static Tile rltile(MTwist& mt, int tile) {
	int subtiles = game_tile_data[tile].images.size();
	return Tile(tile, mt.rand(subtiles));
}

/* Generate a random tile from a tile range */
static Tile rltile(MTwist& mt, const Range& r) {
	return rltile(mt, mt.rand(r));
}

static tileset_id randtileset(MTwist& mt,
		const std::vector<tileset_id>& tilesets) {
	return tilesets[mt.rand(tilesets.size())];
}

static void create_door(GameState* gs, GeneratedRoom& room, const Pos& xy) {
	GameTiles& tiles = gs->tiles();
	int tw = tiles.tile_width(), th = tiles.tile_height();
	int lw = room.width(), lh = room.height();

	int start_x = (tw - lw) / 2;
	int start_y = (th - lh) / 2;

	if (!room.within(xy)) {
		return;
	}

	Sqr& s = room.at(xy);
	if (!s.has_instance && s.passable) {
		s.has_instance = true;
		Pos fpos = xy + Pos(start_x, start_y);
		fpos = centered_multiple(fpos, TILE_SIZE);
		gs->add_instance(new FeatureInst(fpos, FeatureInst::DOOR_CLOSED));
	}
}
static void remove_wall(GameState* gs, GeneratedRoom& l, int x, int y) {
	GameTiles& tiles = gs->tiles();
	int tw = tiles.tile_width(), th = tiles.tile_height();
	int lw = l.width(), lh = l.height();

	int start_x = (tw - lw) / 2;
	int start_y = (th - lh) / 2;

	Sqr& s = l.at(x, y);
	if (!s.passable) {
		s.passable = true;
	}
}
static void create_doors_all_around(GameState* gs, GeneratedRoom& room,
		const BBox& region) {
	for (int x = region.x1; x < region.x2; x++) {
		create_door(gs, room, Pos(x, region.y1));
		create_door(gs, room, Pos(x, region.y2 - 1));
	}
	for (int y = region.y1; y < region.y2; y++) {
		create_door(gs, room, Pos(region.x1, y));
		create_door(gs, room, Pos(region.x2 - 1, y));
	}
}
static void remove_all_around(GameState* gs, GeneratedRoom& room,
		const BBox& region) {
	for (int x = region.x1 + 1; x < region.x2 - 1; x++) {
		remove_wall(gs, room, x, region.y1);
		remove_wall(gs, room, x, region.y2 - 1);
	}
	for (int y = region.y1 + 1; y < region.y2 - 1; y++) {
		remove_wall(gs, room, region.x1, y);
		remove_wall(gs, room, region.x2 - 1, y);
	}
}
static StoreInventory generate_shop_inventory(MTwist& mt, int itemn) {
	StoreInventory inv;
	itemgenlist_id itemgenlist = get_itemgenlist_by_name("Store Items");
	for (int i = 0; i < itemn; /*below*/) {
		const ItemGenChance& igc = generate_item_choice(mt, itemgenlist);
		int quantity = mt.rand(igc.quantity);
		Item item = Item(igc.itemtype, quantity);
		int cost = mt.rand(item.item_entry().shop_cost.multiply(quantity));
		if (cost > 0) {
			inv.add(item, cost);
			i++;
		}
	}
	return inv;
}
static void generate_shop(GameState* gs, GeneratedRoom& room, MTwist& mt,
		const Pos& p) {
	Pos worldpos = room.get_world_coordinate(gs, p);
	room.at(p).has_instance = true;

	int itemn = mt.rand(Range(2, 14));
	gs->add_instance(
			new StoreInst(worldpos, false, res::sprite_id("store"),
					generate_shop_inventory(mt, itemn)));

}
static void generate_statue(GameState* gs, GeneratedRoom& room, MTwist& mt,
		const Pos& p) {
	Pos worldpos = room.get_world_coordinate(gs, p);
	room.at(p).has_instance = true;

	sprite_id spriteid = res::sprite_id("statue");
	SpriteEntry& statue_sprite = game_sprite_data.at(spriteid);
	ldraw::Drawable& sprite = res::sprite(spriteid);
	int nimages = (int)sprite.animation_duration();
	if (nimages <= 0) nimages = 1;
	int imgid = mt.rand(nimages);
	gs->add_instance(
			new FeatureInst(worldpos, FeatureInst::OTHER, true, spriteid,
					FeatureInst::DEPTH, imgid));
}

static bool is_near_wall(GeneratedRoom& room, const Pos& p) {
	for (int dy = -1; dy <= +1; dy++) {
		for (int dx = -1; dx <= +1; dx++) {
			if (!room.within(Pos(p.x + dx, p.y + dy))) {
				continue;
			}
			if (!room.at(Pos(p.x + dx, p.y + dy)).passable) {
				return true;
			}
		}
	}
	return false;
}

void generate_features(const FeatureGenSettings& fs, MTwist& mt,
		GeneratedRoom& room, GameState* gs) {

	std::vector<RoomRegion>& rooms = room.rooms();
	const int nrooms = rooms.size();

	for (int i = 0; i < nrooms; i++) {
		Region& r = rooms[i].region;
		if (gs->rng().rand(100) == 0) {
			remove_all_around(gs, room,
					BBox(r.x - 1, r.y - 1, r.x + r.w + 1, r.y + r.h + 1));
		}
	}

	GameTiles& tiles = gs->tiles();
	TilesetEntry& tileset = game_tileset_data[randtileset(mt, fs.tilesets)];
	tiles.clear();

	int tw = tiles.tile_width(), th = tiles.tile_height();
	int lw = room.width(), lh = room.height();

	int start_x = (tw - lw) / 2;
	int start_y = (th - lh) / 2;

	int end_x = start_x + lw;
	int end_y = start_y + lh;

	for (int y = 0; y < th; y++) {
		for (int x = 0; x < tw; x++) {
			Pos xy(x, y);

			if (y < start_y || y >= end_y || x < start_x || x >= end_x) {
				tiles.set_solid(xy, true);
				tiles.set_seethrough(xy, false);
				tiles.get(xy) = rltile(mt, tileset.wall);
				continue;
			}

			Sqr& s = room.at(x - start_x, y - start_y);
			tiles.set_solid(xy, !s.passable);
			tiles.set_seethrough(xy, s.passable);
			if (s.passable) {
				tiles.get(xy) = rltile(mt, tileset.floor);
				if (s.roomID) {
//					if (s.marking)
// 					tiles[ind] = TILE_MESH_0+s.marking;
				} else if (s.feature == SMALL_CORRIDOR) {
					tiles.get(xy) = rltile(mt, tileset.corridor);
				}
			} else {
				tiles.get(xy) = rltile(mt, tileset.wall);
				if (s.feature == SMALL_CORRIDOR) {
					if (mt.rand(4) == 0) {
						tiles.get(xy) = rltile(mt, tileset.altwall);
					}
				}
			}
		}
	}

	for (int i = 0; i < room.rooms().size(); i++) {
		if (gs->rng().rand(3) != 0)
			continue;
		Region r = room.rooms()[i].region;
		/*

		 int rx = r.x + gs->rng().rand(1, r.w-1);
		 int ry = r.y + gs->rng().rand(1, r.h-1);
		 int rw = gs->rng().rand(r.w);
		 int rh = gs->rng().rand(r.h);

		 if (rx + rw >= r.x+r.w) rw = r.x+r.w - 1 - rx;
		 if (ry + rh >= r.y+r.h) rh = r.y+r.h - 1 - ry;*/
		int rx = r.x, rw = r.w;
		int ry = r.y, rh = r.h;

		for (int y = ry; y < ry + rh; y++) {
			for (int x = rx; x < rx + rw; x++) {
				Sqr& s = room.at(x, y);
				if (s.passable && s.roomID && s.feature != SMALL_CORRIDOR)
					tiles.get(Pos(x + start_x, y + start_y)) = rltile(mt,
							tileset.altfloor);
			}
		}
	}
	gs->get_level()->entrances.clear();
	for (int n = 0; n < fs.nstairs_down; n++) {
		Pos p = generate_location(mt, room);
		room.at(p).has_instance = true;

		p.x += start_x;
		p.y += start_y;

		tiles.get(p) = rltile(mt, res::tileid("stairs_down"));

		gs->get_level()->entrances.push_back(GameRoomPortal(p, Pos(0, 0)));
	}

	gs->get_level()->exits.clear();
	for (int n = 0; n < fs.nstairs_up; n++) {
		Pos p = generate_location(mt, room);
		room.at(p).has_instance = true;

		int x = p.x - 4, y = p.y - 4;
		int ex = p.x + 5, ey = p.y + 5;
		x = std::max(0, x), y = std::max(0, y);
		ex = std::min(ex, room.width()), ey = std::min(ey, room.height());
		for (int yy = y; yy < ey; yy++) {
			for (int xx = x; xx < ex; xx++) {
				room.at(xx, yy).near_entrance = true;
			}
		}

		p.x += start_x;
		p.y += start_y;

		tiles.get(p) = rltile(mt, res::tileid("stairs_up"));
		gs->get_level()->exits.push_back(GameRoomPortal(p, Pos(0, 0)));
	}

	for (int i = 0; i < nrooms; i++) {
		Region& r = rooms[i].region;
		if (mt.rand(100) == 0) {
			create_doors_all_around(gs, room,
					BBox(r.x - 1, r.y - 1, r.x + r.w + 1, r.y + r.h + 1));
		}
	}

	int amount_statues = mt.rand(fs.nstatues);
	for (int i = 0; i < amount_statues; i++) {
		int ind = mt.rand(rooms.size());
		RoomRegion& r1 = rooms[ind];
		Region inner = r1.region.remove_perimeter();
		if (inner.w < 2 || inner.h < 2) {
			continue;
		}
		Pos pos;
		int attempts = 0;
		const int MAX_STATUE_ATTEMPTS = 16;
		do {
			if (++attempts < MAX_STATUE_ATTEMPTS) {
				goto GiveUpOnStatueGeneration;
			}
			pos = generate_location_in_region(mt, room, inner);
			Sqr& sqr = room.at(pos);
			if (!sqr.passable || sqr.has_instance) {
				/* We may not have many candidate locations, just skip this one in case */
				goto GiveUpOnStatueGeneration;
			}
		} while (is_near_wall(room, pos));
		generate_statue(gs, room, mt, pos);
		GiveUpOnStatueGeneration:;
	}

	if (mt.rand(4) == 0) {
		for (int attempts = 0; attempts < 200; attempts++) {
			Pos pos = generate_location(mt, room);
			Sqr& sqr = room.at(pos);
			if (!sqr.passable || sqr.has_instance) {
				continue;
			}
			generate_shop(gs, room, mt, pos);
			break;
		}
	}
}
