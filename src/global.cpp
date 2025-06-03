#include "global.h"
#include <unordered_set>

// dungeon data.
std::array<std::array<char, WIDTH>, HEIGHT> dungeon;
std::array<std::array<int, WIDTH>, HEIGHT> hardness;
std::array<std::array<char, WIDTH>, HEIGHT> base_map;

// Initialize fog_map with spaces so that unseen cells display as blank.
std::array<std::array<char, WIDTH>, HEIGHT> fog_map = [](){
    std::array<std::array<char, WIDTH>, HEIGHT> tmp;
    for (int i = 0; i < HEIGHT; i++) {
        tmp[i].fill(' ');
    }
    return tmp;
}();

bool fog_toggle = false;  // Fog is active by default

int pc_x = 0;
int pc_y = 0;

std::array<int, MAX_ROOMS> room_x;
std::array<int, MAX_ROOMS> room_y;
std::array<int, MAX_ROOMS> room_w;
std::array<int, MAX_ROOMS> room_h;
int room_count = 0;

int upCount = 0, downCount = 0;
int up_xCoord = 0, up_yCoord = 0;
int down_xCoord = 0, down_yCoord = 0;

std::array<std::array<int, WIDTH>, HEIGHT> disTunneling;
std::array<std::array<int, WIDTH>, HEIGHT> disNonTunneling;
std::vector<MonsterTemplate> monster_templates;

std::vector<ObjectInstance> object_instances;
std::vector<ObjectTemplate> object_templates;
std::unordered_set<std::string> seen_artifacts;


