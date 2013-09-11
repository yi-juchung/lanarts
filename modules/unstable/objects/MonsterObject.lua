local ObjectUtils = import "lanarts.objects.ObjectUtils"
local AttackableObject = import ".AttackableObject"
local MonsterType = import "@MonsterType"

local MonsterObject = ObjectUtils.type_create(AttackableObject) 
MonsterObject.MONSTER_TRAIT = "MONSTER_TRAIT"

function MonsterObject.create(args)
    assert(args.sprite and args.monster_type)

    if type(args.monster_type) == "string" then
        args.monster_type = MonsterType.lookup(args.monster_type)
    end

    args.base_stats = table.deep_clone(args.monster_type.stats)
    args.unarmed_attack = args.unarmed_attack or args.monster_type.unarmed_attack

    -- Set up type signature
    args.type = args.type or MonsterObject
    args.traits = args.traits or {}
    table.insert(args.traits, MonsterObject.MONSTER_TRAIT)

    -- AttackableObject configuration
    args.can_attack = true

    return MonsterObject._base_create(MonsterObject, args)
end

function MonsterObject.is_attackable(obj)
    return table.contains(obj.traits, MonsterObject.MONSTER_TRAIT)
end

return MonsterObject