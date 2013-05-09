#include <iostream>
#include <vector>
#include <stdexcept>
#include <ctime>

#include <lcommon/fatal_error.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <lua.hpp>

#include <lcommon/directory.h>

#include <net-lib/lanarts_net.h>
#include <lsound/lsound.h>

#include <ldraw/display.h>
#include <ldraw/lua_ldraw.h>

#include <luawrap/luawrap.h>
#include <luawrap/calls.h>

#include "data/game_data.h"

#include "draw/draw_sprite.h"
#include "draw/fonts.h"

#include "gamestate/GameState.h"

#include "lua_api/lua_api.h"
#include "lua_api/lua_yaml.h"
#include "lua_api/lua_newapi.h"

using namespace std;

static GameState* init_gamestate() {

	lua_State* L = lua_api::create_luastate();
	lua_api::add_search_path(L, "res/?.lua");

	GameSettings settings; // Initialized with defaults
	// Load the manual settings
	if (!load_settings_data(settings, "settings.yaml")) {
		fatal_error("Fatal error: settings.yaml not found, the game is probably being loaded from the wrong place.\n");
	}

	bool can_create_saves = ensure_directory("saves");
	if (!can_create_saves) {
		printf("Problem creating save directory, will not be able to create save files!\n");
	}
	load_settings_data(settings, "saves/saved_settings.yaml"); // Override with remembered settings

	if (SDL_Init(0) < 0) {
		exit(0);
	}

	Size window_size(settings.view_width, settings.view_height);
	ldraw::display_initialize("Lanarts", window_size, settings.fullscreen);

	res::font_primary().initialize(settings.font, 10);
	res::font_menu().initialize(settings.menu_font, 20);

	lanarts_net_init(true);
	lua_api::preinit_state(L);
	lsound::init();

	//GameState claims ownership of the passed lua_State*
	const int HUD_WIDTH = 160;
	int windoww = settings.view_width, windowh = settings.view_height;
	int vieww = windoww - HUD_WIDTH, viewh = windowh;

	GameState* gs = new GameState(settings, L);
	lua_api::register_api(gs, L);

	return gs;
}

/* Must take (int, char**) to play nice with SDL */
int main(int argc, char** argv) {
	label_StartOver:

	GameState* gs = init_gamestate();
	lua_State* L = gs->luastate();

	lua_api::require(L, "main"); // loads engine hooks

	LuaValue engine = luawrap::globals(L)["Engine"];

	engine["menu_start"].push();
	bool did_exit = !luawrap::call<bool>(L);
	save_settings_data(gs->game_settings(), "saves/saved_settings.yaml"); // Save settings from menu
	if (did_exit) {
		/* User has quit! */
		goto label_Quit;
	}

	gs->start_connection();

	engine["resources_load"].push();
	luawrap::call<void>(L);

	try {
		init_game_data(gs->game_settings(), L);
	} catch (const std::exception& err) {
		fprintf(stderr, "%s\n", err.what());
		fflush(stderr);
		goto label_Quit;
	}

	if (gs->game_settings().conntype == GameSettings::SERVER) {
		engine["pregame_menu_start"].push();
		bool did_exit = !luawrap::call<bool>(L);

		if (did_exit) { /* User has quit! */
			goto label_Quit;
		}
	}

	if (gs->start_game()) {
		engine["game_start"].push();
		luawrap::call<void>(L);

		if (!gs->io_controller().user_has_exit()) {
			if (gs->game_settings().conntype != GameSettings::CLIENT) {
				save_settings_data(gs->game_settings(),
						"saves/saved_settings.yaml"); // Save settings from in-game
			}
			delete gs;
			goto label_StartOver;
		}
	}

	if (gs->game_settings().conntype != GameSettings::CLIENT) {
		save_settings_data(gs->game_settings(), "saves/saved_settings.yaml"); // Save settings from in-game
	}

	label_Quit:

	lanarts_quit();

	delete gs;

	printf("Lanarts shut down cleanly\n");

//	lua_close(L); // TODO: To exit cleanly we must clear all resource vectors explicitly

	return 0;
}
