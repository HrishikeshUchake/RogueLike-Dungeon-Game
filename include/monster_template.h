#ifndef MONSTER_TEMPLATE_H
#define MONSTER_TEMPLATE_H

#include <string>
#include <vector>
#include <random>

struct Dice {
    int base;
    int dice;
    int sides;

    int roll() const;
    static Dice parse(const std::string& s);
    std::string to_string() const;
};

struct MonsterTemplate {
    std::string name;
    std::string description;
    char symbol;
    std::vector<std::string> colors;
    Dice speed;
    std::vector<std::string> abilities;
    Dice hp;
    Dice damage;
    int rarity;
};

std::vector<MonsterTemplate> parse_monsters(const std::string& filepath);



void print_monsters(const std::vector<MonsterTemplate>& monsters);

#endif
