local GameObject = import "core.GameObject"
local Map = import "core.Map"
local Display = import "core.Display"

local ObjectUtils = import ".ObjectUtils"

local M = nilprotect {} -- Submodule

M._door_closed, M._door_open = unpack(Display.images_load (path_resolve "plain_door.png%32x32"))
M.FEATURE_TRAIT = "feature"
M.FEATURE_DEPTH = 100

local DEACTIVATION_DISTANCE = 768

-- Base
M.FeatureBase = GameObject.type_create()
local Base = M.FeatureBase
function Base:init(args)
    Base.parent_init(self, args.xy, args.radius or 15, args.solid, args.depth or M.FEATURE_DEPTH)
    self.traits = self.traits or {}
    table.insert(self.traits, M.FEATURE_TRAIT)
end 
function Base:on_draw()
    if self.sprite and Display.object_within_view(self) then
        ObjectUtils.screen_draw(self.sprite, self.xy, self.alpha, self.frame)
    end
end

-- Decoration
M.Decoration = GameObject.type_create(Base)
local Decoration = M.Decoration
function Decoration:on_step()
    if self.sprite or Map.distance_to_player(self.map, self.xy) >= DEACTIVATION_DISTANCE then
        return -- Need to be able to scale to many deactivated instances
    end 
    if Map.object_visible(self) then
        self.sprite = self.real_sprite
    end
end
function Decoration:init(args)
    Decoration.parent_init(self, args)
    self.real_sprite = assert(args.sprite)
    self.depth = args.depth or M.FEATURE_DEPTH
    self.frame = args.frame or 0
end

-- Door
M.Door = GameObject.type_create(Base)
local Door = M.Door
local DOOR_OPEN_TIMEOUT = 128
function Door:on_step()
    if Map.distance_to_player(self.map, self.xy) >= DEACTIVATION_DISTANCE then
        return -- Need to be able to scale to many deactivated instances
    end 

    if self.open_timeout > 0 then
        self.open_timeout = self.open_timeout - 1
        return
    end

    local is_open = false
    local collisions = Map.rectangle_collision_check(self.map, self.area, self)
    for object in values(collisions) do
        if object.team then -- TODO proper combat object detection
            is_open = true
            break
        end
    end

    if is_open then
        self.open_timeout = DOOR_OPEN_TIMEOUT
    end

    if is_open ~= self.was_open then
        local tile_xy = ObjectUtils.tile_xy(self, true)
        Map.tile_set_solid(self.map, tile_xy, not is_open)
        Map.tile_set_seethrough(self.map, tile_xy, is_open)
    end

    local real_sprite = is_open and self.open_sprite or self.closed_sprite
    if self.sprite ~= real_sprite and Map.object_visible(self) then
        self.sprite = real_sprite
    end

    self.was_open = is_open
end
function Door:on_map_init()
    local tile_xy = ObjectUtils.tile_xy(self, true)
    self.was_open = false
    Map.tile_set_solid(self.map, ObjectUtils.tile_xy(self, true), true)
    Map.tile_set_seethrough(self.map, tile_xy, false)

    local whalf = self.open_sprite.width / 2 + self.padding
    local hhalf = self.open_sprite.height / 2 + self.padding

    self.area = {
        self.x - whalf, self.y - hhalf,
        self.x + whalf, self.y + hhalf
    }

    self.open_timeout = 0
end
function Door:init(args)
    Door.parent_init(self, args)
    self.open_sprite = args.open_sprite or M._door_open
    self.closed_sprite = args.closed_sprite or M._door_closed
    self.depth = args.depth or M.FEATURE_DEPTH
    self.padding = 4
end

return M
