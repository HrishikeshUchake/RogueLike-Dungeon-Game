#ifndef GLOBAL_H
#define GLOBAL_H

#include <array>
#include <climits>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <unordered_set>
#include "monster_template.h"
#include "object_template.h"
#include "object_instance.h"



// --- Constants ---
constexpr int WIDTH = 80;
constexpr int HEIGHT = 21;
constexpr int MAX_ROOMS = 10;
constexpr int DEFAULT_NUMMON = 10;

// Dungeon file constants
constexpr const char* DUNGEON_DIR = "/.rlg327/";
constexpr const char* DUNGEON_FILE = "dungeon";
constexpr const char* FILE_MARKER = "RLG327-S2025";
constexpr int MARKER_LEN = 12;
constexpr int FILE_VERSION = 0;

// --- Global Variables ---
// Main dungeon arrays.
extern std::array<std::array<char, WIDTH>, HEIGHT> dungeon;
extern std::array<std::array<int, WIDTH>, HEIGHT> hardness;
extern std::array<std::array<char, WIDTH>, HEIGHT> base_map;

// Fog of war array.
extern std::array<std::array<char, WIDTH>, HEIGHT> fog_map;
extern bool fog_toggle;

// PC coordinates.
extern int pc_x;
extern int pc_y;

// Room arrays.
extern std::array<int, MAX_ROOMS> room_x;
extern std::array<int, MAX_ROOMS> room_y;
extern std::array<int, MAX_ROOMS> room_w;
extern std::array<int, MAX_ROOMS> room_h;
extern int room_count;

// Stair information.
extern int upCount, downCount;
extern int up_xCoord, up_yCoord;
extern int down_xCoord, down_yCoord;

// Distance arrays for pathfinding.
extern std::array<std::array<int, WIDTH>, HEIGHT> disTunneling;
extern std::array<std::array<int, WIDTH>, HEIGHT> disNonTunneling;

// New flag: set to true when a new level has been generated.
extern bool level_changed;

extern std::vector<MonsterTemplate> monster_templates;

extern std::vector<ObjectInstance> object_instances;
extern std::vector<ObjectTemplate> object_templates;
extern std::unordered_set<std::string> seen_artifacts;


#endif // GLOBAL_H
