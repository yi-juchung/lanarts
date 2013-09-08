-- Usage: 'MonsterType.define { ... attributes ... }', 'MonsterType.lookup(<name or ID>)'
local S = import "@Schemas"
local ResourceTypes = import "@ResourceTypes"

local MonsterType = ResourceTypes.type_create(
    S.enforce_function_create {
        name = S.STRING,
        traits = S.TABLE,
        description = S.STRING,
        xp_award = S.NUMBER,
        stats = S.TABLE,
        unarmed_attack = S.TABLE,

        -- Object controlling functions
        radius = S.defaulted(S.NUMBER, 14),
        init = S.FUNCTION_OR_NIL,
        on_step = S.FUNCTION_OR_NIL,
        on_draw = S.FUNCTION_OR_NIL,
        on_die = S.FUNCTION_OR_NIL,
        on_first_appear = S.FUNCTION_OR_NIL
    }
)

return MonsterType