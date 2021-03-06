/*
 * stat_formulas.cpp
 *  Represents formulas used in combat
 */

#include <cmath>

#include "gamestate/GameState.h"
#include "gamestate/GameLogger.h"

#include "items/EquipmentEntry.h"
#include "items/WeaponEntry.h"

#include "combat_stats.h"

#include "stat_formulas.h"
#include "stats.h"

/* What power, resistance difference causes damage to be raised by 100% */
const int POWER_MULTIPLE_INTERVAL = 50;

static float damage_multiplier(float power, float resistance) {
	float powdiff = power - resistance;
	float intervals = powdiff / POWER_MULTIPLE_INTERVAL;
	if (intervals < 0) {
		//100% / (1+intervals)
		return 1.0f / (1.0f - intervals);
	} else {
		//100% + 100% * intervals
		return 1.0f + intervals;
	}
}

static int basic_damage_formula(const EffectiveAttackStats& attacker,
		const DerivedStats& defender) {
	float mult = damage_multiplier(attacker.power, defender.resistance);
	float base = attacker.damage
			- defender.reduction * attacker.resist_modifier;
	event_log("basic_damage_formula: mult=%f, base=%f result=%f rounded=%d\n",
			mult, base, mult * base, int(round(mult * base)));
	if (base < 0)
		return 0;
	return round(mult * base);
}

static int physical_damage_formula(const EffectiveAttackStats& attacker,
		const EffectiveStats& defender) {
	return basic_damage_formula(attacker, defender.physical);
}

static int magic_damage_formula(const EffectiveAttackStats& attacker,
		const EffectiveStats& defender) {
	return basic_damage_formula(attacker, defender.magic);
}

int damage_formula(const EffectiveAttackStats& attacker,
		const EffectiveStats& defender) {
	float mdmg = magic_damage_formula(attacker, defender);
	float pdmg = physical_damage_formula(attacker, defender);

	event_log("damage_formula attacker damage=%d power=%d cooldown=%d magic_percentage=%f resist_modifier=%f\n",
			attacker.damage, attacker.power, attacker.cooldown,
			attacker.magic_percentage, attacker.resist_modifier);
	event_log("damage_formula defender physical.resistance=%f physical.reduction=%d magic.resistance=%f magic.reduction=%d \n",
			defender.physical.resistance, defender.physical.reduction,
			defender.magic.resistance, defender.magic.reduction);
	event_log("damage_formula: mdmg=%f, pdmg=%f\n", mdmg, pdmg);

	return mdmg * attacker.magic_percentage
			+ pdmg * attacker.physical_percentage();
}

static void factor_in_equipment_core_stats(MTwist& mt,
		EffectiveStats& effective, const Equipment& item) {
	CoreStats& core = effective.core;
	EquipmentEntry& entry = item.equipment_entry();

	core.apply_as_bonus(entry.core_stat_modifier());
}

static void factor_in_equipment_derived_stats(MTwist& mt,
		EffectiveStats& effective, const Equipment& item) {
	CoreStats& core = effective.core;
	EquipmentEntry& entry = item.equipment_entry();

	effective.cooldown_modifiers.apply(entry.cooldown_modifiers);

	effective.physical.resistance += entry.resistance().calculate(mt, core);
	effective.magic.resistance += entry.magic_resistance().calculate(mt, core);
	effective.physical.reduction += entry.damage_reduction().calculate(mt,
			core);
	effective.magic.reduction += entry.magic_reduction().calculate(mt, core);
}

static void factor_in_equipment_stats(MTwist& mt, EffectiveStats& effective,
		const EquipmentStats& equipment) {
	const Inventory& inventory = equipment.inventory;
	for (int i = 0; i < inventory.max_size(); i++) {
		const ItemSlot& itemslot = inventory.get(i);
		if (itemslot.is_equipped()) {
			factor_in_equipment_core_stats(mt, effective, itemslot.item);
		}
	}
	for (int i = 0; i < inventory.max_size(); i++) {
		const ItemSlot& itemslot = inventory.get(i);
		if (itemslot.is_equipped()) {
			factor_in_equipment_derived_stats(mt, effective, itemslot.item);
		}
	}
}
static void derive_secondary_stats(MTwist& mt, EffectiveStats& effective) {
	CoreStats& core = effective.core;
	effective.physical.resistance += core.defence / 2.5f;
	effective.magic.resistance += core.willpower / 2.5f;
	effective.physical.reduction += core.defence / 2.0f;
	effective.magic.reduction += core.willpower / 2.0f;
}

EffectiveStats effective_stats(GameState* gs, CombatGameInst* inst,
		const CombatStats& stats) {
	EffectiveStats ret;
	ret.core = stats.core;
	ret.movespeed = stats.movespeed;
	ret.allowed_actions = stats.effects.allowed_actions(gs);
	stats.effects.process(gs, inst, ret);
	factor_in_equipment_stats(gs->rng(), ret, stats.equipment);
	derive_secondary_stats(gs->rng(), ret);
	ret.cooldown_modifiers.apply(ret.cooldown_mult);
	event_log("effective_stats: hp=%d, mp=%d, hpregen=%f, mpregen=%f\n",
			ret.core.hp, ret.core.mp, ret.core.hpregen, ret.core.mpregen);
	return ret;
}

int experience_needed_formula(int xplevel) {
	float proportion = pow(xplevel, 2.7);
	return round(proportion) * 75 + 125;
}

