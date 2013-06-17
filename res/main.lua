--[[
    This is the only Lua file explicitly loaded by the lanarts engine.

    This sets up all the engine hooks that control program flow, and pull
    in additional files.
]]

require_path_add("res/library_files/?.lua")

sound_volume = 0 -- Mute the game

function Engine.menu_start(...)
    require "menus.start_menu"

    return start_menu_show(...)
end

function Engine.pregame_menu_start(...)
    require "menus.pregame_menu"

    return pregame_menu_show(...)
end

function Engine.loading_screen_draw(...)
    require "menus.loading_screen"

    return loading_screen_draw(...)
end

function Engine.resources_load(...)
    --TODO: Find a better place for these helper functions
    function is_consumable(item)     return item.type == "consumable" end
    function is_weapon(item)         return item.type == "weapon" end
    function is_armour(item)         return item.type == "armour" end
    function is_projectile(item)     return item.type == "projectile" end

    require "sound"
    require "tiles.tilesets"

    dofile "res/enemies/enemies.lua"

    dofile "res/effects/effects.lua"

    dofile "res/spells/spell_effects.lua"
end

function Engine.game_start(...)
    require "game_loop"

    return game_loop(...)
end

function Engine.post_draw(...)
    require "game_loop"

    return game_post_draw(...)
end

function Engine.overlay_draw(...)
    require "game_loop"

    return game_overlay_draw(...)
end

function Engine.game_won(...)
    require "event_occurred"

    return player_has_won(...)
end

function Engine.event_occurred(...)
    require "event_occurred"

    return event_occurred(...)
end

function Engine.first_map_create(...)
	require "rooms.map_generation"

	return first_map_create(...)
end