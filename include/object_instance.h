#ifndef OBJECT_INSTANCE_H
#define OBJECT_INSTANCE_H

#include <string>
#include <vector>
#include "monster_template.h"  // For Dice

struct ObjectInstance {
    std::string name;
    char symbol;
    std::vector<std::string> color;
    int hit, dodge, defense, weight, speed, attribute, value;
    Dice damage;
    bool is_artifact;
    std::string description;
    int x, y;  // Position in the dungeon
};

#endif // OBJECT_INSTANCE_H
