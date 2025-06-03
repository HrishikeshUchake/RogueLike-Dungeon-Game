#include "character.h"
#include "global.h"
#include "dungeon.h"
#include "pathfinding.h"
#include "ui.h"
#include "monster_template.h"
#include "object_generator.h"
#include <cstdlib>
#include <climits>
#include <unordered_set>
#include <ncurses.h>

std::vector<character_t> characters;
bool pc_is_alive = true;


void create_pc() {
    character_t pc;
    pc.type           = CharType::PC;
    pc.alive          = true;
    pc.x = pc_x; pc.y = pc_y;

    // NEW: starting stats
    pc.hp             = 50;             // you choose
    pc.base_damage    = Dice{0, 1, 4};  // bare fists: 0 + 1d4
    pc.speed          = 10;             // base speed
    // initialize carry & equipment slots
    for (auto &slot : pc.inventory) slot.reset();
    for (auto &slot : pc.equipment) slot.reset();

    pc.turn           = 0;
    pc.symbol         = '@';

    pc.mana = 10;
    pc.max_mana = 10; 

    characters.push_back(pc);
    dungeon[pc_y][pc_x] = '@';
}


void create_monster() {
    if (monster_templates.empty()) return;

    // Try selecting a monster by rarity
    MonsterTemplate selected;
    bool found = false;
    for (int attempt = 0; attempt < 1000; ++attempt) {
        const auto& cand = monster_templates[rand() % monster_templates.size()];
        if ((rand() % 100) < cand.rarity) {
            selected = cand;
            found = true;
            break;
        }
    }
    if (!found) return;

    // Find spawn location
    int rx, ry;
    do {
        rx = rand() % WIDTH;
        ry = rand() % HEIGHT;
    } while (dungeon[ry][rx] != '.');

    character_t m;
    m.type = CharType::Monster;
    m.alive = true;
    m.x = rx;
    m.y = ry;
    m.speed = selected.speed.roll();
    m.hp = selected.hp.roll();
    m.turn = 0;
    m.symbol = selected.symbol;
    m.color = selected.colors;
    m.monster_btype = 0;  

    // Example: handle "SMART", "TELE", etc. abilities
    for (const std::string& ab : selected.abilities) {
        if (ab == "SMART") m.monster_btype |= 0x1;
        else if (ab == "TELE") m.monster_btype |= 0x2;
        else if (ab == "TUNNEL") m.monster_btype |= 0x4;
        else if (ab == "ERRATIC") m.monster_btype |= 0x8;
    }

    characters.push_back(m);
    dungeon[ry][rx] = m.symbol;
}


void do_monster_movement(character_t &m) {
    if (!m.alive)
        return;
    int oldx = m.x, oldy = m.y;
    bool intelligence = (m.monster_btype & 0x1);
    bool tunneling = (m.monster_btype & 0x4);
    bool erratic = (m.monster_btype & 0x8);
    
    bool do_random = false;
    if (erratic && (rand() % 2 == 0))
        do_random = true;
    
    int bestx = m.x, besty = m.y;
    if (do_random) {
        int rr = rand() % 9;
        int ddx[9] = {0, -1, 1, 0, 0, -1, -1, 1, 1};
        int ddy[9] = {0, 0, 0, -1, 1, -1, 1, -1, 1};
        bestx = m.x + ddx[rr];
        besty = m.y + ddy[rr];
    } else if (!intelligence) {
        int dx = (pc_x > m.x) ? 1 : ((pc_x < m.x) ? -1 : 0);
        int dy = (pc_y > m.y) ? 1 : ((pc_y < m.y) ? -1 : 0);
        bestx = m.x + dx;
        besty = m.y + dy;
    } else {
        int bestDist = INT_MAX;
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (i == 0 && j == 0)
                    continue;
                int nx = m.x + j, ny = m.y + i;
                if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) {
                    int d = tunneling ? disTunneling[ny][nx] : disNonTunneling[ny][nx];
                    if (d < bestDist) {
                        bestDist = d;
                        bestx = nx;
                        besty = ny;
                    }
                }
            }
        }
    }
    
    // If tunneling and encountering a wall.
    if (tunneling && hardness[besty][bestx] > 0 && hardness[besty][bestx] < 255) {
        hardness[besty][bestx] -= 85;
        if (hardness[besty][bestx] > 0)
            return;
        dungeon[besty][bestx] = '#';
        base_map[besty][bestx] = '#';
    }
    
    if (dungeon[besty][bestx] == '@') {
        character_t &pc = characters[0]; 
        perform_attack(m, pc);
        return;
    }
    
    
    dungeon[oldy][oldx] = base_map[oldy][oldx];
    m.x = bestx;
    m.y = besty;
    if (m.alive)
        dungeon[besty][bestx] = m.symbol;
}

void new_level(int nummon) {
    characters.clear();
    // Create new dungeon level.
    initializeDungeon();
    generateRooms();
    connectRoomsViaCorridor();
    placeStairs();
    
    if (room_count > 0) {
        pc_x = room_x[0];
        pc_y = room_y[0];
    } else {
        pc_x = 1;
        pc_y = 1;
    }
    base_map = dungeon;
    placePC(pc_x, pc_y);
    
    djikstraForNonTunnel(pc_x, pc_y);
    djikstraForTunnel(pc_x, pc_y);
    
    create_pc();
    generate_objects(10);  
    for (int i = 0; i < nummon; i++)
        create_monster();

}

int calculate_total_damage(const character_t &attacker) {
    int total = 0;

    // Base damage for PC
    if (attacker.type == CharType::PC) {
        total += attacker.base_damage.roll();

        // Add weapon damage from equipped items
        for (const auto &slot : attacker.equipment) {
            if (slot) {
                total += slot->damage.roll();
            }
        }
    } else {
        // NPCs use their own damage dice
        for (const auto &mt : monster_templates) {
            if (mt.symbol == attacker.symbol) {
                total += mt.damage.roll();
                break;
            }
        }
    }
    return total;
}

void perform_attack(character_t &attacker, character_t &defender) {
    int dmg = calculate_total_damage(attacker);
    defender.hp -= dmg;

    if (defender.type == CharType::PC) {
        if (defender.hp <= 0) {
            pc_is_alive = false;
            display_message("You were slain!");
        } else {
            display_message("You were hit for " + std::to_string(dmg) + " damage.");
        }
    } else {
        if (defender.hp <= 0) {
            defender.alive = false;
            dungeon[defender.y][defender.x] = base_map[defender.y][defender.x];
            display_message("You killed " + std::string(1, defender.symbol));
        } else {
            display_message("Hit enemy for " + std::to_string(dmg) + " damage.");
        }
    }
    if (!defender.alive && defender.symbol == 'B') {  //boss kill check, end game if monster "B" is defeated
        display_message("You defeated the boss! You win!");
        getch(); end_curses(); std::exit(0);
    }
    
    std::cout <<"[ATTACK] Damage: " << dmg << ", Target HP before: " << defender.hp << std::endl;
}



