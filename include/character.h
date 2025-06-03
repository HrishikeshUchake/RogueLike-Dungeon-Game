// character.h
#ifndef CHARACTER_H
#define CHARACTER_H

#include "global.h"
#include "object_instance.h"
#include <optional>
#include <array>

enum class CharType { PC, Monster };

// Equipment slots (one per type, two rings)
enum EquipSlot {
  EQ_WEAPON, EQ_OFFHAND, EQ_RANGED, EQ_ARMOR, EQ_HELMET,
  EQ_CLOAK, EQ_GLOVES, EQ_BOOTS, EQ_AMULET, EQ_LIGHT,
  EQ_RING1, EQ_RING2,
  NUM_EQUIP_SLOTS
};

struct character_t {
  CharType type;
  bool alive;
  int x, y;
  int hp;
  Dice base_damage;
  static constexpr int MAX_CARRY = 10;
  std::array<std::optional<ObjectInstance>, MAX_CARRY> inventory;
  std::array<std::optional<ObjectInstance>, NUM_EQUIP_SLOTS> equipment;

  // speed now represents current speed (base + equipment modifiers)
  int speed;

  // monster‑only fields unchanged
  int turn;
  int monster_btype;
  char symbol;
  std::vector<std::string> color;
  int mana; 
  int max_mana;
};

struct event_t {
  int time;
  character_t* c;
};

// Character management functions
void create_pc();
void create_monster();
void do_monster_movement(character_t &m);
// character.h
void try_pickup_item(character_t &pc);
void new_level(int nummon);
int calculate_total_damage(const character_t &attacker);
void perform_attack(character_t &attacker, character_t &defender);
extern std::vector<character_t> characters;
extern bool pc_is_alive;

#endif // CHARACTER_H
