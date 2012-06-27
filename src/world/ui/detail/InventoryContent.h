/*
 * InventoryContent.h:
 *  Represents an interactive view of the inventory for the side bar
 */

#ifndef INVENTORYCONTENT_H_
#define INVENTORYCONTENT_H_

#include "SidebarContent.h"

class InventoryContent: public SidebarContent {
public:
	InventoryContent(const BBox& bbox) :
			SidebarContent(bbox) {
	}
	virtual void draw(GameState* gs) const;

	virtual int amount_of_pages(GameState* gs);
};

#endif /* INVENTORYCONTENT_H_ */