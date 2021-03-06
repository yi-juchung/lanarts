/*
 * collision_filters.h:
 *  Defines functions that are often used in conjunction with collision detection
 */

#ifndef COLLISION_FILTERS_H_
#define COLLISION_FILTERS_H_

class GameInst;

bool item_colfilter(GameInst* self, GameInst* other);
bool enemy_colfilter(GameInst* self, GameInst* other);
bool player_colfilter(GameInst* self, GameInst* other);

#endif /* COLLISION_FILTERS_H_ */
