/*
 * lua_ldraw.h:
 *  Bindings for the ldraw library in lua.
 */

#ifndef LUA_LDRAW_LIB_H_
#define LUA_LDRAW_LIB_H_

#include "lua_image.h"
#include <SLB/Manager.hpp>
#include <SLB/Table.hpp>

#include <common/lua/lua_geometry.h>
#include <common/lua/lua_range.h>

#include "lua_colour.h"
#include "lua_drawoptions.h"
#include "lua_font.h"
#include "lua_drawable.h"

class LuaValue;
// Registers functions into table on top of stack
namespace ldraw {
void lua_register_ldraw(lua_State* L, const LuaValue& module);
}

#endif /* LUA_LDRAW_LIB_H_ */
