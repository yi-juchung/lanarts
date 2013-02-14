require "InstanceBox"
require "InstanceLine"
require "TextLabel"
require "Sprite"
require "utils"

local cached_screen
local setup_callback

local function cached_setup(percentage, task)
	local ret = cached_screen
	setup_callback(percentage, task)

	if percentage == 100 then -- cleanup
		cached_screen = nil
		setup_callback = nil
	end

	return ret
end

local function loading_string(percentage)
	return percentage .. "% Loaded"
end

local function center_screen_setup()
	local center_box = InstanceBox.create( { size = {640, 480} } )
	local percentage_label = TextLabel.create( fonts.small, {origin=CENTER_TOP}, "")
	local task_label = TextLabel.create( fonts.small, {color=COL_MUTED_GREEN, origin=CENTER_TOP}, "")
	local loading_label = TextLabel.create( fonts.large, { color=COL_LIGHT_GRAY, origin=CENTER_TOP }, "" )

	center_box:add_instance( 
		loading_label,
		CENTER,
		-- Up 100 pixels
		{0, -100}
	)
	center_box:add_instance( 
		Sprite.image_create("res/enemies/bosses/sprites/reddragon.png"), -- dragon
		CENTER
	)
	center_box:add_instance( percentage_label, CENTER )
	center_box:add_instance( task_label, CENTER, --[[Down 50 pixels]] {0, 50} )

	function setup_callback(percentage, task) 
--		percentage_label.text = percentage .. "% Done: "
		task_label.text = task
		loading_label.text = loading_string(percentage)
	end

	return center_box
end

local function loading_screen_setup(...) 
	if cached_screen then
		return cached_setup(...)
	end

	local screen_box = InstanceBox.create( { size = display.window_size } )

	screen_box:add_instance( center_screen_setup(), CENTER )

	-- set up cache
	cached_screen = screen_box

	return cached_setup(...)
end

-- Loading callback
function system.loading_draw(...)
	perf.timing_begin("system.loading_draw")
    display.draw_start()

    local screen = loading_screen_setup(...)
    screen:draw( {0,0} )

    display.draw_finish()
	perf.timing_end("system.loading_draw")
end