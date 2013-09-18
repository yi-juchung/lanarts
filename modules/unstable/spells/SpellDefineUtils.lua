local Type, Traits = import "@SpellType", import ".SpellTraits"
local Apts = import "@stats.AptitudeTypes"
local ContentUtils = import "@stats.ContentUtils"
local StatusType = import "@StatusType"
local SpellType = import "@SpellType"
local Projectiles = import "lanarts.objects.Projectiles"

local M = nilprotect {} -- Submodule

function M.base_spell_on_create(spell, args)
    if spell.name then
        spell.name = (spell.name):interpolate(args)
    end
    if spell.description then
        spell.description = (spell.description):interpolate(args)
    end
    args.type = args.type or spell
    return setmetatable(args, {__index = spell})
end

function M.spell_define(args)
    args.on_create = args.on_create or M.base_spell_on_create

    return SpellType.define(args)
end

local function missile_spell_prereq(self, caster, xy)
    return self.attack:on_prerequisite(caster)
end

local function missile_spell_on_use(self, caster, xy)
    return self.attack:on_prerequisite(caster)
end

function M.missile_spell_define(args)
    if args.on_prerequisite then
        args.on_prerequisite = func_apply_and(args.on_prerequisite or missile_spell_prereq)
    end

    args.target_type = args.target_type or SpellType.TARGET_HOSTILE
    args.traits = args.traits or {Traits.FORCE_SPELL}

    args.types = args.types or {Apts.FORCE} -- For 'derive_attack'
    args.attack = ContentUtils.derive_attack(args)

    return M.spell_define(args)
end

return M