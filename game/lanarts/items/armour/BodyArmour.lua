Data.equipment_create {
    name = "Leather Armour",
    description = "A durable, flexible armour.",
    type = "armour",

    shop_cost = {15,25},

    spr_item = "leather armour",

    spell_cooldown_multiplier = 1.1,

    reduction = {base = 1},
    resistance = {base = 2}
}

Data.equipment_create {
    name = "Chainmail",
    description = "A strong armour with limited flexibility.",
    type = "armour",

    shop_cost = {45,60},

    spr_item = "chainmail",

    spell_cooldown_multiplier = 1.15,

    ranged_cooldown_multiplier = 1.15,

    reduction = {base = 2},
    resistance = {base = 3}
}

Data.equipment_create {
    name = "Platemail",
    description = "A strong, heavy armour.",
    type = "armour",

    shop_cost = {95,105},

    spr_item = "platemail",

    spell_cooldown_multiplier = 1.35,

    ranged_cooldown_multiplier = 1.35,

    reduction = {base = 3},
    resistance = {base = 4}
}

Data.equipment_create {
    name = "Robe",
    description = "A lightly protective, uninhibited robe.",
    type = "armour",

    shop_cost = {10,20},

    spr_item = "robe",

    reduction = {base = 1},
    magic_reduction = {base = 1},

    resistance = {base = 1},
    magic_resistance = {base = 1}
}

Data.equipment_create {
    name = "Magician's Robe",
    description = "An enchanted robe of protection and magical power.",
    type = "armour",

    shop_cost = {45,60},

    spr_item = "enchanted robe",

    reduction = {base = 1},
    magic_reduction = {base = 2},

    resistance = {base = 1},
    magic_resistance = {base = 1},

    stat_bonuses = {magic = 1}
}

Data.equipment_create {
    name = "Runed Robe",
    description = "A heavily enchanted robe of protection and magical power.",
    type = "armour",

    shop_cost = {95,105},

    spr_item = "runed robe",

    reduction = {base = 1},
    magic_reduction = {base = 2},

    resistance = {base = 2},
    magic_resistance = {base = 2},

    stat_bonuses = {defence = 1, magic = 1}
}

Data.equipment_create {
    name = "Robe of Mana",
    description = "A robe of great magical capacity.",
    type = "armour",

    shop_cost = {95,105},

    spr_item = "robe of mana",

    stat_bonuses = {mp = 35}
}
