#include "global.h"
#include "dungeon.h"
#include "pathfinding.h"
#include "character.h"
#include "ui.h"
#include "monster_template.h"
#include "object_generator.h"

#include <cstring>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <queue>
#include <curses.h>


#ifdef __APPLE__
  #include <libkern/OSByteOrder.h>
  #define be32toh(x) OSSwapBigToHostInt32(x)
  #define htobe32(x) OSSwapHostToBigInt32(x)
  #define be16toh(x) OSSwapBigToHostInt16(x)
  #define htobe16(x) OSSwapHostToBigInt16(x)
#else
  #include <endian.h>
#endif

// Comparator for the event queue (min-heap based on event time)
struct EventComparator {
    bool operator()(const event_t &a, const event_t &b) const {
        return a.time > b.time;
    }
};

int main(int argc, char* argv[]) {
    srand(time(nullptr));
    
    bool load = false, save = false, parse_mode = false;
    int local_num_mon = DEFAULT_NUMMON;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--load") == 0)
            load = true;
        else if (strcmp(argv[i], "--save") == 0)
            save = true;
        else if (strcmp(argv[i], "--nummon") == 0 && i + 1 < argc)
            local_num_mon = std::atoi(argv[++i]);
        else if (strcmp(argv[i], "--parse") == 0) {
                parse_mode = true;
        }
    }
    
    // setup dungeon: check directory, get file path, load or generate dungeon.
    checkDir();
    char path[1024];
    getPath(path, sizeof(path));

    const char* home = getenv("HOME");
    std::string mpath = std::string(home) + "/.rlg327/monster_desc.txt";
    monster_templates = parse_monsters(mpath);
    std::cout << "Loaded " << monster_templates.size() << " monster templates." << std::endl;
    if (monster_templates.empty()) {
        std::cerr << "Warning: No valid monster templates loaded.\n";
    }

    std::string opath = std::string(home) + "/.rlg327/object_desc.txt";
    object_templates = parse_objects(opath);
    std::cout << "Loaded " << object_templates.size() << " object templates.\n";
    if (object_templates.empty()) {
    std::cerr << "Warning: No valid object templates loaded.\n";
    }


    if (parse_mode) {
        const char* home = getenv("HOME");
        std::string mpath = std::string(home) + "/.rlg327/monster_desc.txt";
        auto monsters = parse_monsters(mpath);
        print_monsters(monsters);
        return 0;
    }
    
    
    if (load) {
        load_dungeon(path);
    } else {
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
    }
    base_map = dungeon;  // save a copy of the current dungeon.
    placePC(pc_x, pc_y);
    generate_objects(10);  // generate some objects in the dungeon.
    
    if (save) {
        save_dungeon(path);
    }
    
    // compute initial distances for monsters.
    djikstraForNonTunnel(pc_x, pc_y);
    djikstraForTunnel(pc_x, pc_y);
    
    // create the player character and monsters.
    characters.clear();
    create_pc();
    for (int i = 0; i < local_num_mon; i++) {
        create_monster();
    }
    
    // build a priority event queue for scheduling actions.
    std::priority_queue<event_t, std::vector<event_t>, EventComparator> eventQueue;
    for (auto &ch : characters) {
        event_t e;
        e.time = 0;
        e.c = &ch;
        eventQueue.push(e);
    }
    
    init_curses();
    int current_time = 0;
    int aliveMonsters = local_num_mon;
    
    // main game loop: process events until the PC dies or all monsters are dead.
    while (!eventQueue.empty() && pc_is_alive && aliveMonsters > 0) {
        event_t e = eventQueue.top();
        eventQueue.pop();
        current_time = e.time;
        character_t* c = e.c;
        
        if (!c->alive)
            continue;
        
        if (c->type == CharType::PC) {
            display_dungeon();
            handle_pc_input(*c);
            if (c->type == CharType::PC) {
                if (c->turn % 5 == 0 && c->mana < c->max_mana) { // every 5 turns
                    c->mana++;
                    display_message("You feel your mana slowly returning...");
                }
            }            
            if (pc_is_alive) {
                // recompute distances after PC moves.
                pc_x = c->x;
                pc_y = c->y;
                djikstraForNonTunnel(pc_x, pc_y);
                djikstraForTunnel(pc_x, pc_y);
            }
            c->turn++;
        } else {  // monster turn.
            do_monster_movement(*c);
            c->turn++;
            if (!pc_is_alive)
                break;
            if (!c->alive)
                aliveMonsters--;
        }
        
        if (c->alive) {
            int next_time = current_time + (1000 / c->speed);
            event_t ne;
            ne.time = next_time;
            ne.c = c;
            eventQueue.push(ne);
        }
    }
    
    if (!pc_is_alive) {
        display_dungeon();
        display_message("You lose! The PC has been killed.");
    } else if (aliveMonsters == 0) {
        display_dungeon();
        display_message("You win! All monsters have been slain.");
    } else {
        display_message("Simulation ended early (queue empty?).");
    }
    
    getch();
    end_curses();
    
    return 0;
}
