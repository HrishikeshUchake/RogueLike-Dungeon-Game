// object_template.h
#ifndef OBJECT_TEMPLATE_H
#define OBJECT_TEMPLATE_H

#include <string>
#include <vector>
#include "monster_template.h" // For Dice structure
#include "object_instance.h"   // For ObjectInstance

enum class ObjectType {
    WEAPON, OFFHAND, RANGED, ARMOR, HELMET, CLOAK, GLOVES, BOOTS,
    RING, AMULET, LIGHT, SCROLL, BOOK, FLASK, GOLD, AMMUNITION,
    FOOD, WAND, CONTAINER, STACK, INVALID
};

struct ObjectTemplate {
    std::string name;
    std::string description;
    ObjectType type;
    char symbol;
    std::vector<std::string> colors;
    Dice hit, dodge, defense, weight, speed, attribute, value, damage;
    bool artifact;
    int rarity;

    // Factory method
    ObjectInstance generate_instance() const;
};

std::vector<ObjectTemplate> parse_objects(const std::string& filepath);
void print_objects(const std::vector<ObjectTemplate>& objs);

#endif // OBJECT_TEMPLATE_H
