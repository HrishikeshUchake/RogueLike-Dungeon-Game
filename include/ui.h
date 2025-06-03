#ifndef UI_H
#define UI_H

#include "global.h"
#include "character.h"
#include <string>

void init_curses();
void end_curses();

void display_message(const std::string &msg);
void display_dungeon();
void display_monster_list();

void handle_magic_spell_mode(character_t &pc);
void handle_ranged_attack_mode(character_t &pc);

bool pc_can_walk_on(char cell);
void handle_pc_input(character_t &pc);

int get_color_pair(const std::vector<std::string>& colors);


#endif // UI_H
