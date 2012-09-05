/*
 * items.h:
 *  Define item state. These are defined in terms of a base item, and applied properties.
 */

#ifndef ITEMS_H_
#define ITEMS_H_

#include "../stat_modifiers.h"

class ItemEntry;
class EquipmentEntry;
class ProjectileEntry;
class WeaponEntry;

class SerializeBuffer;

enum item_flags {
	NOFLAGS = 0, CURSED = 1, GOLDEN = 2
};

struct ItemProperties {
	//// For projectiles/weapons:
	DamageStats damage;

	//// For equipment items:
	// Stat modifiers while wearing this equipment
	StatModifiers stat_modifiers;
	CooldownModifiers cooldown_modifiers;
	// Status effects from wearing this armour
	StatusEffectModifiers effect_modifiers;

	item_flags flags;

	// Depending on your skill, using an item will lower this property until it is known
	// XXX: Have a different stat for different players ? (Might be important for PvP)
	int unknownness;

	bool is_identified() {
		return unknownness <= 0;
	}
	void identify() {
		unknownness = 0;
	}

	ItemProperties() :
			flags(NOFLAGS), unknownness(1) {
	}

	bool operator==(const ItemProperties& properties) const;

	void serialize(SerializeBuffer& serializer);
	void deserialize(SerializeBuffer& serializer);
};

struct Item {
	item_id id;
	ItemProperties properties;
	int amount;

	ItemEntry& item_entry() const;
	EquipmentEntry& equipment_entry() const;
	ProjectileEntry& projectile_entry() const;
	WeaponEntry& weapon_entry() const;

	bool is_normalItem() const;
	bool is_equipment() const;
	bool is_projectile() const;
	bool is_weapon() const;
	bool empty() const {
		return amount == 0 || id == NO_ITEM;
	}

	void add_copies(int amount) {
		this->amount += amount;
	}

	void remove_copies(int amount) {
		this->amount -= amount;
		LANARTS_ASSERT(this->amount >= 0);
		if (this->amount == 0){
			clear();
		}
	}

	void clear() {
		id = NO_ITEM;
		properties = ItemProperties();
	}

	explicit Item(item_id id = NO_ITEM, int amount = 0, ItemProperties properties = ItemProperties()) :
			id(id), properties(properties), amount(amount) {
	}

	bool is_same_item(const Item& item) const;

	void serialize(SerializeBuffer& serializer);
	void deserialize(SerializeBuffer& serializer);
};

#endif /* ITEMS_H_ */