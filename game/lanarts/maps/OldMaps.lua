local item_groups = import ".ItemGroups"
local Display = import "core.Display"
local item_utils = import ".ItemUtils"
local dungeons = import ".Dungeons"
local map_utils = import ".MapUtils"
local SourceMap = import "core.SourceMap"
local World = import "core.World"

local M = nilprotect {} -- Submodule

local easy_animals = {
  {enemy = "Giant Rat",         chance = 100,  group_chance = 33, group_size = 2          },
  {enemy = "Giant Bat",         chance = 100                                            }
}

M.medium_animals = {
  {enemy = "Giant Rat",         chance = 100, group_chance = 33, group_size = 3           },
  {enemy = "Giant Bat",         chance = 100, group_chance = 33, group_size = 3           },
  {enemy = "Giant Spider",      chance = 100,                                           },
  {enemy = "Hound",             chance = 100                                            }
}

local easy_enemies = {
  {enemy = "Giant Rat",         chance = 100, group_chance = 33, group_size = 3           },
  {enemy = "Skeleton",          chance = 100                                            },
  {enemy = "Chicken",           chance = 100                                            },
  {enemy = "Cloud Elemental",   chance = 100, group_chance = 33, group_size = 2           }
}

local medium_enemies = {
  {enemy = "Skeleton",          chance = 50                                              },
  {enemy = "Storm Elemental",   chance = 66,  group_chance = 33, group_size = 2           },
  {enemy = "Chicken",           chance = 50                                             },
  {enemy = "Unseen Horror",     chance = 5                                             },
  {enemy = "Dark Centaur",      chance = 10                                             }
}
  
local hard_enemies = {
 {enemy = "Storm Elemental",   chance = 20,    group_chance = 100, group_size = 3          },
 {enemy = "Hell Storm",        chance = 5,   group_chance = 33, group_size = 2           },
 {enemy = "Super Chicken",     chance = 100                                             },
 {enemy = "Skeleton",            chance = 75                                              },
 {enemy = "Ciribot",           chance = 75                                              },
 {enemy = "Golem",             guaranteed_spawns = 1                                    },
 {enemy = "Dark Centaur",      chance = 30                                              },
 {enemy = "Unseen Horror",     chance = 10,   group_chance = 33, group_size = 2           }
}

M.harder_enemies = {
  {enemy = "Super Chicken",     chance = 50,  group_chance = 33, group_size = {2,4}       },
  {enemy = "Ciribot",           chance = 100                                            },
  {enemy = "Storm Elemental",   chance = 5,   group_chance = 100, group_size = 4          },
  {enemy = "Hell Storm",        chance = 20,  group_chance = 33, group_size = 3           },
  {enemy = "Golem",             guaranteed_spawns = 1, group_chance = 100, group_size = 2 },
  {enemy = "Jester",            chance = 20                                             },
  {enemy = "Hydra",             chance = 20                                             },
  {enemy = "Unseen Horror",     chance = 10,  group_chance = 33, group_size = 2           }
}

local test_layout = {
        size = {70,70},
        rooms =    { padding = 1, amount = 1,  size = {60,60}},
        tunnels =  { padding = 1, width = {1,2}, per_room = 0}
}
local tiny_layout1 = {
        size = {{55,75},{55,76}}, -- {width} by {height},
        rooms =    { padding = 1, amount = 3,  size = {10,12}},
        tunnels =  { padding = 1, width = {1,4}, per_room = 2}
}        
local tiny_layout2 = {
        size = {{55,75},{45,76}}, -- {width} by {height},
        rooms =    { padding = 1, amount = 6,  size = {5,12} },
        tunnels =  { padding = 1, width = {1,4}, per_room = 2}
}

local tiny_layout3 = {
        size = {40,80},
        rooms =    { padding = 1, amount = {5,10}, size = {4,10} },
        tunnels =  { padding = 1, width = {1,4},  per_room = {2,3} }
}

local small_layout1 = {
        size = {40, 40},
        rooms =    { padding = 0, amount = 40, size = {4,4} },
        tunnels =  { padding = 1, width = {1,2},   per_room = 5 }
}

local small_layout2 = {
        size = {50, 50},
        rooms =    { padding = 1, amount = 20, size = {10,10}},
        tunnels =  { padding = 1, width = {1,2},  per_room = 2   }
}
local small_layout3 = {
        size = {64, 64},
        rooms =    { padding = 1, amount = 23, size = {5,7} },
        tunnels =  { width = {1,2}, per_room = {2, 5}      }
}
local small_layout4 = {
        size = {76, 76},
        rooms =    { padding = 1, amount = 20, size = {8,10}},
        tunnels =  { width = {1,2}, per_room = {2, 5}      }
} 
local small_layout5 = {
        size = {90, 60},
        rooms = {
          {padding = 1, amount = {3,4}, size = {15,15}},
          {padding = 1, amount = {8,10}, size = {8,10} }
        },
        tunnels =  { width = {1,2}, per_room = 2        }
}
local medium_layout1 = {
        size = {{60,80}, {60,80}},
        rooms = {
          {padding = 1, amount = {3,4}, size = {5,5}}
          , {padding = 4, amount = 20, size = {10,30}},
        },
        tunnels =  { width = {2,6}, per_room = {2, 3}      }
}         
local medium_layout2 = {
        size = {{60,80}, {60,80}},
        rooms = {
          {padding = 1, amount = {3,4}, size = {5,5}}
          , {padding = 4, amount = 20, size = {10,30}}
        },
        tunnels =  { width = {1,5}, per_room = {2, 20}      }
}
local large_layout1 = {
        size = {{70,90}, {70,90}},
        rooms = { padding = 3, amount = 15, size = {8,12} },
        tunnels = {
          width = {1, 3},
          per_room = 9
       }
}
local large_layout2 = {
        size = {{70,90}, {70,90}}, 
        rooms = { padding = 3, amount = 15, size = {8,12} }, 
        tunnels = { width = {1, 3}, per_room = 9 }
}

local large_layout3 = {
        size = {{70,90}, {70,90}},
        rooms = {{padding = 1, amount = {5,10}, size = {15,15}}, {padding = 1, amount = {15,20}, size = {8,15} }},
        tunnels = { width = {1, 3}, per_room = 9}
}

local tiny_layouts = {tiny_layout1, tiny_layout2, tiny_layout3}   
local small_layouts = {
  small_layout1, small_layout2, small_layout3, small_layout4, small_layout5
}

local medium_layouts = {
  medium_layout1, medium_layout2
}

local large_layouts = {
  large_layout1, large_layout2, large_layout3
}

local function tconcat(t1, t2)
    local t = {}
    for v in values(t1) do t[#t+1] = v end
    for v in values(t2) do t[#t+1] = v end
    return t
end

local map_layouts = {
  -- Level 1
  { layout = tiny_layouts,
    content = {
      items = { amount = 3,  group = item_groups.basic_items   },
      enemies = {
        wandering = false,
        amount = 7,
        generated = {
          {enemy = "Giant Rat",         chance = 100  },
          {enemy = "Giant Bat",         chance = 100 },
          {enemy = "Hound",         chance = 100 },
          {enemy = "Cloud Elemental",   guaranteed_spawns = 2 }
        }
      }
    }
  },
  -- Level 2
  { layout = tiny_layouts,
    content = {
      items = { amount = 4,  group = item_groups.basic_items },
      enemies = {
        wandering = false,
        amount = 12,
        generated = {
          {enemy = "Giant Rat",         chance = 100  },
          {enemy = "Giant Bat",         chance = 100 },
          {enemy = "Hound",         chance = 100 },
          {enemy = "Cloud Elemental",   chance = 50 },
          {enemy = "Giant Spider",   chance = 50 }
        }
      }  
    }
  },
  -- Level 3
  { layout = small_layouts,
    content = {
      items = { amount = 5,  group = tconcat(item_groups.basic_items, item_groups.enchanted_items)   },
      enemies = {
        amount = {10,14},
        generated = M.medium_animals 
      }
    }
  },
  -- Level 4
  { layout = small_layouts,
    content = {
      items = { amount = 8,  group = tconcat(item_groups.basic_items, item_groups.enchanted_items)   },
      enemies = {
        amount = {12,15},
        generated = medium_enemies
      }
    }
  },
  -- Level 5
  { templates = {path_resolve "dungeon1room1a.txt", path_resolve "dungeon1room1b.txt", path_resolve "dungeon1room1c.txt"},
    content = {
      items = { amount = 4,  group = tconcat(item_groups.basic_items, item_groups.enchanted_items)   },
      enemies = {
        amount = 6,
        generated = tconcat(medium_enemies, {{enemy = "Hell Warrior", guaranteed_spawns = 1}})
      }
    }
  },
  -- Level 6
  { layout = {small_layout1},
    content = {
      items = { amount = 8, group = tconcat(item_groups.basic_items, item_groups.enchanted_items)   },
      enemies = {
        amount = {14,16},
        generated = tconcat(medium_enemies, {{enemy = "Orc Warrior", guaranteed_spawns = {0,1}}})
      }
    }
  },
  -- Level 7
  { layout = medium_layouts,
    content = {
      items = { amount = 8, group = tconcat(item_groups.basic_items, item_groups.enchanted_items)   },
      enemies = {
        amount = {18,22},
        generated = tconcat(hard_enemies, {{enemy = "Orc Warrior", guaranteed_spawns = {0,1}}})
      }
    }
  },
  -- Level 8
  { layout = large_layouts,
    content = {
      items = { amount = 10, group = tconcat(item_groups.basic_items, item_groups.enchanted_items)   },
      enemies = {
        amount = 22,
        generated = hard_enemies
      }
    }
  
  },
  -- Level 9
  { layout = large_layouts,
    content = {
      items = { amount = 7,  group = item_groups.enchanted_items   },
      enemies = {
        amount = 25,
        generated = M.harder_enemies
      }
    }
  },
  -- Level 10
  { layout = large_layouts,
    content = {
      items = { amount = 7,  group = item_groups.enchanted_items   },
      enemies = {
        amount = 30,
        generated = tconcat(M.harder_enemies, {{enemy = "Zin", guaranteed_spawns = 1}})
      }
    }
  }
}

local function range_resolve(r)
    if type(r) == "table" then return random(r)
    else return r end
end

local function generate_items(map, items)
    for i=1,range_resolve(items.amount) do
        item_utils.item_object_generate(map, items.group)
    end
end

function M.enemy_generate(chances)
    local total_chance = 0
    for entry in values(chances) do
        total_chance = total_chance + (entry.chance or 0)
    end
    local rand = random(1, total_chance)
    for entry in values(chances) do
        rand = rand - (entry.chance or 0)
        if rand <= 0 then
            return entry.enemy
        end
    end
end

function M.generate_from_enemy_entries(map, chances, amount)
    local total_chance = 0
    for entry in values(chances) do
        total_chance = total_chance + (entry.chance or 0)
        local spawns = range_resolve(entry.guaranteed_spawns or 0)
        for i=1,spawns do 
            map_utils.random_enemy(map, entry.enemy)
        end
    end
    for i=1,amount do
        local rand = random(1, total_chance)
        for entry in values(chances) do
            rand = rand - (entry.chance or 0)
            if rand <= 0 then 
                map_utils.random_enemy(map, entry.enemy)
                break
            end
        end
    end
end

local function generate_enemies(map, enemies)
    local min, max
    if type(enemies.amount) == 'table' then 
        min, max = unpack(enemies.amount)
    else
        min, max = enemies.amount, enemies.amount
    end
    local amounts = {min * (0.8 + World.player_amount * 0.2), max * (0.8 + World.player_amount * 0.2)}
    local amount = math.round(randomf(amounts))
    M.generate_from_enemy_entries(map, enemies.generated, amount)
end

local function leaf_group_areas(map)
    local ret = {}
    for group in values(map.groups) do
        if #group.children == 0 then
            ret[#ret + 1] = group.area
        end
    end
    return ret
end

M.statue = Display.animation_create(Display.images_load "game/lanarts/features/sprites/statues/statue(0-17).png", 1.0)

local function generate_statues(map)
    local areas = leaf_group_areas(map)
    local amount = random(5,10)
    local i = 0
    local tries, MAX_TRIES = 1, 10
    while i < amount do
        local area = random_choice(areas)
        local sqr = map_utils.random_square(map, area)
        if not sqr then return end
        local query = SourceMap.rectangle_query {
            map = map,
            -- nearby 3x3 box
            area = bbox_create( {sqr[1]-1, sqr[2]-1}, {3, 3}),
            fill_selector = {matches_none = SourceMap.FLAG_SOLID}
        } 
        if query then
            map:square_apply(sqr, {add = SourceMap.FLAG_SOLID, remove = SourceMap.FLAG_SEETHROUGH})
            map_utils.spawn_decoration(map, M.statue, sqr, random(0,17))
            i = i + 1
            tries = 0
        end
        tries = tries + 1
        if tries >= MAX_TRIES then
            return
        end 
    end
end

local function generate_stores(map)
    if chance(0.25) then
        local items = {}
        for i=1,random(5,10) do
            table.insert(items, item_utils.item_generate(chance(.5) and item_groups.basic_items or item_groups.enchanted_items, true))
        end
        map_utils.random_store(map, items)
    end
end

local function generate_doors(map) 
    local areas = leaf_group_areas(map)
    for area in values(areas) do
        if chance(0.05) then
            local selector = {matches_none = SourceMap.FLAG_SOLID}
            local x1,y1,x2,y2 = unpack(area)
            x1, y1 = x1-1, y1-1
            for y=y1,y2 do
                for x=x1,x2 do
                    if x == x1 or x == x2 or y == y1 or y == y2 then
                        local sqr = {x,y}
                        if map:square_query(sqr, selector) then
                            map_utils.spawn_door(map, sqr)
                        end
                    end
                end
            end
        end
    end
end

local PADDING = 4
local function get_inner_area(map)
    local w,h = unpack(map.size)
    return bbox_create({PADDING, PADDING}, {w - PADDING*2, h - PADDING * 2})
end

local function generate_content(map, content, tileset)
    generate_items(map, content.items)
    generate_enemies(map, content.enemies)
    generate_statues(map)
    generate_doors(map)
    generate_stores(map)
end

local function generate_tunnels(map, tunnels, tileset)
    local per_room_range = type(tunnels.per_room) == "table" and tunnels.per_room or {tunnels.per_room, tunnels.per_room}
    dungeons.simple_tunnels(map, tunnels.width, per_room_range, tileset.wall, tileset.floor_tunnel or tileset.floor_alt, get_inner_area(map), tunnels.padding)
end

local function map_gen_apply(map, placements, wall, floor, size, padding)
    SourceMap.random_placement_apply { map = map, area = get_inner_area(map),
        child_operator = dungeons.room_carve_operator(wall, floor, padding or 1),
        size_range = size, amount_of_placements_range = placements,
        create_subgroup = false
    }
end

local function generate_rooms(map, rooms, tileset)
    if not rooms.amount then
        rooms = random_choice(rooms)
    end
    -- Ensure it is a range
    local amounts = range_resolve(rooms.amount)
    local alt_amount = math.random(math.floor(amounts*0.10), math.ceil(amounts*0.50))
    amounts = amounts - alt_amount
    -- Compensate for padding
    local size = {rooms.size[1] + rooms.padding*2, rooms.size[2] + rooms.padding*2}
    map_gen_apply(map, {alt_amount, alt_amount}, tileset.wall, tileset.floor_alt, size, rooms.padding)
    map_gen_apply(map, {amounts, amounts}, tileset.wall, tileset.floor , size, rooms.padding)
end

local function generate_layout(map, layout, tileset)
    generate_rooms(map, layout.rooms, tileset)
    generate_tunnels(map, layout.tunnels, tileset)
end

local function generate_from_template(label, template, tileset)
    return map_utils.area_template_to_map(label, template, --[[padding]] 4, { 
           ['x'] = { add = SourceMap.FLAG_SOLID, content = tileset.wall }, 
           ['.'] = { add = SourceMap.FLAG_SEETHROUGH, content = chance(.5) and tileset.floor_alt or tileset.floor }
    })
end

function M.create_map(label, floor, tileset)
    local entry = map_layouts[floor]
    -- Resolve room width & height ranges by applying range_resolve
    local map
    if entry.layout then
        local layout = random_choice(entry.layout)
        local size = map_call(range_resolve, layout.size)
        map = map_utils.map_create(label, size, tileset.wall)
        generate_layout(map, layout, tileset)
    else 
        local template = random_choice(entry.templates)
        map = generate_from_template(label, template, tileset)
    end
    generate_content(map, entry.content, tileset)
    return map
end

M.last_floor = #map_layouts

-- Submodule
return M