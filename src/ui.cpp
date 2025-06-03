#include "ui.h"
#include "global.h"
#include "character.h"
#include "dungeon.h"
#include <ncurses.h>
#include <string>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <cmath>


bool inBounds(int x, int y) {
    return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}

// Constant for the light radius 
static const int LIGHT_RADIUS = 3;

// Update the fog map for cells within the PC's light radius.
void update_fog_map() {
    for (int y = pc_y - LIGHT_RADIUS; y <= pc_y + LIGHT_RADIUS; y++) {
        for (int x = pc_x - LIGHT_RADIUS; x <= pc_x + LIGHT_RADIUS; x++) {
            if (inBounds(x, y)) {
                fog_map[y][x] = dungeon[y][x];
            }
        }
    }
}

int get_color_pair(const std::vector<std::string>& colors) {
    // Use the first color in the list
    if (colors.empty()) return 7; // default white

    std::string c = colors[0];
    if (c == "RED")     return 1;
    if (c == "GREEN")   return 2;
    if (c == "BLUE")    return 3;
    if (c == "CYAN")    return 4;
    if (c == "MAGENTA") return 5;
    if (c == "YELLOW")  return 6;
    if (c == "WHITE")   return 7;
    if (c == "BLACK")   return 8;
    return 7; // fallback
}


void display_dungeon() {
    if (!fog_toggle) {
        update_fog_map();
    }
    for (int r = 0; r < HEIGHT; r++) {
        move(r + 1, 0);  // offset for message line
        for (int c = 0; c < WIDTH; c++) {
            char ch;

            bool visible = fog_toggle || (std::abs(r - pc_y) <= LIGHT_RADIUS && std::abs(c - pc_x) <= LIGHT_RADIUS);

            if (fog_toggle) {
                ch = dungeon[r][c];
            } else {
                if (visible) {
                    ch = dungeon[r][c];
                } else {
                    ch = fog_map[r][c];
                }
            }

            // Special case: Player Character
            if (ch == '@') {
                attron(COLOR_PAIR(9));  // PC: white on blue
                addch(ch);
                attroff(COLOR_PAIR(9));
                continue;
            }

            bool rendered = false;

            if (visible) {
                // Check if a monster occupies this tile
                for (const auto& mon : characters) {
                    if (mon.alive && mon.x == c && mon.y == r && mon.type == CharType::Monster) {
                        int pair = get_color_pair(mon.color);
                        attron(COLOR_PAIR(pair));
                        addch(mon.symbol);
                        attroff(COLOR_PAIR(pair));
                        rendered = true;
                        break;
                    }
                }

                // If no monster, check for object
                if (!rendered) {
                    for (const auto& obj : object_instances) {
                        if (obj.x == c && obj.y == r) {
                            int pair = get_color_pair(obj.color);
                            attron(COLOR_PAIR(pair));
                            addch(obj.symbol);
                            attroff(COLOR_PAIR(pair));
                            rendered = true;
                            break;
                        }
                    }
                }
            }

            // If neither monster nor object, render terrain
            if (!rendered) {
                addch(ch);
            }
        }
    }

    refresh();
}




void init_curses() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();

    init_pair(1, COLOR_RED,     COLOR_BLACK);
    init_pair(2, COLOR_GREEN,   COLOR_BLACK);
    init_pair(3, COLOR_BLUE,    COLOR_BLACK);
    init_pair(4, COLOR_CYAN,    COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_YELLOW,  COLOR_BLACK);
    init_pair(7, COLOR_WHITE,   COLOR_BLACK);
    init_pair(8, COLOR_BLACK,   COLOR_BLACK);
    init_pair(9, COLOR_WHITE, COLOR_BLUE); // PC color

}


void end_curses() {
    endwin();
}

void display_message(const std::string &msg) {
    move(0, 0);
    clrtoeol();
    mvprintw(0, 0, "%s", msg.c_str());
    refresh();
}

void display_monster_list() {
    struct moninfo_t {
        char symbol;
        int rel_x, rel_y;
    };
    std::vector<moninfo_t> list;
    for (auto &ch : characters) {
        if (!ch.alive)
            continue;
        if (ch.type == CharType::PC)
            continue;
        // Only show monster if illuminated.
        if (!fog_toggle && (std::abs(ch.y - pc_y) > LIGHT_RADIUS || std::abs(ch.x - pc_x) > LIGHT_RADIUS))
            continue;
        moninfo_t info;
        info.symbol = ch.symbol;
        info.rel_x = ch.x - pc_x;
        info.rel_y = ch.y - pc_y;
        list.push_back(info);
    }
    
    clear();
    refresh();
    
    int offset = 0;
    const int lines_avail = 20;
    bool done = false;
    while (!done) {
        clear();
        mvprintw(0, 0, "--- Monster List (press ESC to exit, up/down to scroll) ---");
        int line = 1;
        for (int i = 0; i < lines_avail; i++) {
            int idx = offset + i;
            if (idx >= (int)list.size())
                break;
            int dx = list[idx].rel_x;
            int dy = list[idx].rel_y;
            const char* vert = nullptr, *horiz = nullptr;
            int ady = (dy < 0) ? -dy : dy;
            int adx = (dx < 0) ? -dx : dx;
            if (dy < 0)
                vert = "north";
            else if (dy > 0)
                vert = "south";
            if (dx < 0)
                horiz = "west";
            else if (dx > 0)
                horiz = "east";
            char desc[80];
            if (vert && horiz) {
                snprintf(desc, sizeof(desc), "%c, %d %s and %d %s", list[idx].symbol, ady, vert, adx, horiz);
            } else if (vert) {
                snprintf(desc, sizeof(desc), "%c, %d %s", list[idx].symbol, ady, vert);
            } else if (horiz) {
                snprintf(desc, sizeof(desc), "%c, %d %s", list[idx].symbol, adx, horiz);
            } else {
                snprintf(desc, sizeof(desc), "%c, same cell??", list[idx].symbol);
            }
            mvprintw(line++, 0, "%s", desc);
        }
        refresh();
        int ch = getch();
        switch (ch) {
            case 27:
                done = true;
                break;
            case KEY_UP:
                if (offset > 0)
                    offset--;
                break;
            case KEY_DOWN:
                if (offset + lines_avail < (int)list.size())
                    offset++;
                break;
            default:
                break;
        }
    }
    clear();
    display_dungeon();
    display_message("Exited monster list.");
}

bool pc_can_walk_on(char cell) {
    return (cell == '.' || cell == '#' || cell == '<' || cell == '>');
}

// new function: Teleport mode.
void handle_teleport_mode(character_t &pc) {
    // Start the targeting pointer at the PC's current location.
    int target_x = pc.x;
    int target_y = pc.y;
    
    // Temporarily disable fog so the full dungeon is visible during targeting.
    bool old_fog_toggle = fog_toggle;
    fog_toggle = true;
    
    display_message("Teleport mode: use movement keys to target; press 'g' to confirm, 'r' for random, ESC to cancel.");
    
    bool done = false;
    while (!done) {
        // Redraw the full dungeon (fog off).
        display_dungeon();
        // Draw the targeting pointer (an asterisk) at the target location.
        mvaddch(target_y + 1, target_x, '*');  // dungeon is drawn starting at row 1
        refresh();
        
        int ch = getch();
        switch (ch) {
            case 27: // ESC cancels teleport mode.
                display_message("Teleport cancelled.");
                done = true;
                break;
            case '7': case 'y': {
                if (inBounds(target_x - 1, target_y - 1) && hardness[target_y - 1][target_x - 1] != 255) {
                    target_x--; target_y--;
                }
                break;
            }
            case '8': case 'k': {
                if (inBounds(target_x, target_y - 1) && hardness[target_y - 1][target_x] != 255)
                    target_y--;
                break;
            }
            case '9': case 'u': {
                if (inBounds(target_x + 1, target_y - 1) && hardness[target_y - 1][target_x + 1] != 255) {
                    target_x++; target_y--;
                }
                break;
            }
            case '6': case 'l': {
                if (inBounds(target_x + 1, target_y) && hardness[target_y][target_x + 1] != 255)
                    target_x++;
                break;
            }
            case '3': case 'n': {
                if (inBounds(target_x + 1, target_y + 1) && hardness[target_y + 1][target_x + 1] != 255) {
                    target_x++; target_y++;
                }
                break;
            }
            case '2': case 'j': {
                if (inBounds(target_x, target_y + 1) && hardness[target_y + 1][target_x] != 255)
                    target_y++;
                break;
            }
            case '1': case 'b': {
                if (inBounds(target_x - 1, target_y + 1) && hardness[target_y + 1][target_x - 1] != 255) {
                    target_x--; target_y++;
                }
                break;
            }
            case '4': case 'h': {
                if (inBounds(target_x - 1, target_y) && hardness[target_y][target_x - 1] != 255)
                    target_x--;
                break;
            }
            case 'r': { // Random teleport target.
                int rx, ry;
                do {
                    rx = rand() % WIDTH;
                    ry = rand() % HEIGHT;
                } while (hardness[ry][rx] == 255);  // Immutable rock is not allowed.
                target_x = rx;
                target_y = ry;
                break;
            }
            
                        
            case 'g': { // Confirm teleport.
                if (hardness[target_y][target_x] == 255) {
                    display_message("Cannot teleport into immutable rock!");
                } else {
                    // Teleport: update PC's position.
                    dungeon[pc.y][pc.x] = base_map[pc.y][pc.x];
                    pc.x = target_x;
                    pc.y = target_y;
                    dungeon[pc.y][pc.x] = '@';
                    // Update the base map to reflect the new position.
                    for (int y = 0; y < HEIGHT; y++) {
                        fog_map[y].fill(' ');
                    }
                    update_fog_map();
                    
                    display_message("Teleported.");
                    done = true;
                }
                break;
            }
            default:
                break;
        }
    }
    // Restore previous fog setting.
    fog_toggle = old_fog_toggle;
    display_dungeon();
}

void handle_ranged_attack_mode(character_t &pc) {
    int target_x = pc.x;
    int target_y = pc.y;

    bool old_fog_toggle = fog_toggle;
    fog_toggle = true;

    display_message("Ranged attack mode: move to target and press 'f' to fire, ESC to cancel.");

    bool done = false;
    while (!done) {
        display_dungeon();
        mvaddch(target_y + 1, target_x, '*'); // Draw targeting cursor
        refresh();

        int ch = getch();
        switch (ch) {
            case 27: // ESC
                display_message("Ranged attack canceled.");
                done = true;
                break;
            case '7': case 'y':
                if (inBounds(target_x - 1, target_y - 1))
                    target_x--, target_y--;
                break;
            case '8': case 'k':
                if (inBounds(target_x, target_y - 1))
                    target_y--;
                break;
            case '9': case 'u':
                if (inBounds(target_x + 1, target_y - 1))
                    target_x++, target_y--;
                break;
            case '6': case 'l':
                if (inBounds(target_x + 1, target_y))
                    target_x++;
                break;
            case '3': case 'n':
                if (inBounds(target_x + 1, target_y + 1))
                    target_x++, target_y++;
                break;
            case '2': case 'j':
                if (inBounds(target_x, target_y + 1))
                    target_y++;
                break;
            case '1': case 'b':
                if (inBounds(target_x - 1, target_y + 1))
                    target_x--, target_y++;
                break;
            case '4': case 'h':
                if (inBounds(target_x - 1, target_y))
                    target_x--;
                break;
                case 'f': {
                    if (pc.mana < 5) {
                        display_message("Not enough mana to cast Fireball!");
                        return;
                    }                    
                    // Animate projectile
                    int sx = pc.x;
                    int sy = pc.y;
                    int ex = target_x;
                    int ey = target_y;
                    int dx = (ex > sx) ? 1 : ((ex < sx) ? -1 : 0);
                    int dy = (ey > sy) ? 1 : ((ey < sy) ? -1 : 0);
                    int cx = sx;
                    int cy = sy;
                    while (cx != ex || cy != ey) {
                        if (cx != ex) cx += dx;
                        if (cy != ey) cy += dy;
                        display_dungeon();
                        move(cy + 1, cx);
                        attron(COLOR_PAIR(1)); // Red
                        addch('*');
                        attroff(COLOR_PAIR(1));
                        refresh();
                        napms(150);
                    }
                    int radius = 2;
                    clear();

                    // EXPLOSION ANIMATION START
                    for (int dy = -radius; dy <= radius; dy++) {
                        for (int dx = -radius; dx <= radius; dx++) {
                            int ex = target_x + dx;
                            int ey = target_y + dy;
                            if (inBounds(ex, ey) && dx*dx + dy*dy <= radius*radius) {
                                move(ey + 1, ex);
                                attron(COLOR_PAIR(6)); // Yellow
                                addch('*');
                                attroff(COLOR_PAIR(6));
                            }
                        }
                    }
                refresh();
                napms(300); // pause 300ms
                display_dungeon(); // redraw after flash

                pc.mana -= 5;

                
                    // Handle actual hit/miss
                    bool hit = false;
                    for (auto &ch : characters) {
                        if (ch.alive && ch.type == CharType::Monster && ch.x == target_x && ch.y == target_y) {
                            int damage = 5 + (rand() % 6);
                            ch.hp -= damage;
                            char buf[80];
                            snprintf(buf, sizeof(buf), "You hit %c for %d damage!", ch.symbol, damage);
                            display_message(buf);
                
                            if (ch.hp <= 0) {
                                ch.alive = false;
                                dungeon[ch.y][ch.x] = base_map[ch.y][ch.x];
                                display_message("Monster killed!");
                            }
                            hit = true;
                            break;
                        }
                    }
                    if (!hit) {
                        display_message("You missed. No monster there!");
                    }
                    done = true;
                    break;
                }
                
            default:
                break;
        }
    }

    fog_toggle = old_fog_toggle;
    display_dungeon();
}

void handle_magic_spell_mode(character_t &pc) {
    int target_x = pc.x;
    int target_y = pc.y;

    bool old_fog_toggle = fog_toggle;
    fog_toggle = true;

    display_message("Poison ball mode: move to center, press 'f' to cast, ESC to cancel.");

    bool done = false;
    if (pc.mana < 3) {
        display_message("Not enough mana to cast Poison Ball!");
        return;
    }
    
    while (!done) {
        display_dungeon();
        mvaddch(target_y + 1, target_x, '*'); // Draw targeting cursor
        refresh();

        int ch = getch();
        switch (ch) {
            case 27: // ESC
                display_message("Spell casting canceled.");
                done = true;
                break;
            case '7': case 'y':
                if (inBounds(target_x - 1, target_y - 1))
                    target_x--, target_y--;
                break;
            case '8': case 'k':
                if (inBounds(target_x, target_y - 1))
                    target_y--;
                break;
            case '9': case 'u':
                if (inBounds(target_x + 1, target_y - 1))
                    target_x++, target_y--;
                break;
            case '6': case 'l':
                if (inBounds(target_x + 1, target_y))
                    target_x++;
                break;
            case '3': case 'n':
                if (inBounds(target_x + 1, target_y + 1))
                    target_x++, target_y++;
                break;
            case '2': case 'j':
                if (inBounds(target_x, target_y + 1))
                    target_y++;
                break;
            case '1': case 'b':
                if (inBounds(target_x - 1, target_y + 1))
                    target_x--, target_y++;
                break;
            case '4': case 'h':
                if (inBounds(target_x - 1, target_y))
                    target_x--;
                break;
            case 'f': { // 'f' to cast spell
                int radius = 2;
                clear();

                // EXPLOSION ANIMATION START
                for (int dy = -radius; dy <= radius; dy++) {
                    for (int dx = -radius; dx <= radius; dx++) {
                        int ex = target_x + dx;
                        int ey = target_y + dy;
                        if (inBounds(ex, ey) && dx*dx + dy*dy <= radius*radius) {
                            move(ey + 1, ex);
                            attron(COLOR_PAIR(2)); // GREEN
                            addch('*');
                            attroff(COLOR_PAIR(2));
                        }
                    }
                }
                refresh();
                napms(300); // pause 300ms
                display_dungeon(); // redraw after flash

                pc.mana -= 3;

            
                int monsters_hit = 0;
                for (auto &ch : characters) {
                    if (ch.alive && ch.type == CharType::Monster) {
                        int dx = ch.x - target_x;
                        int dy = ch.y - target_y;
                        if (dx*dx + dy*dy <= radius*radius) {
                            int damage = 3 + (rand() % 5);
                            ch.hp -= damage;
                            monsters_hit++;
                            if (ch.hp <= 0) {
                                ch.alive = false;
                                dungeon[ch.y][ch.x] = base_map[ch.y][ch.x];
                            }
                        }
                    }
                }
                if (monsters_hit > 0) {
                    display_message("Poison ball explodes! Monsters take damage.");
                } else {
                    display_message("Poison ball explodes harmlessly.");
                }
                done = true;
                break;
            }
            default:
                break;
        }
    }

    fog_toggle = old_fog_toggle;
    display_dungeon();
}

void handle_fireball_spell_mode(character_t &pc) {
    if (pc.mana < 5) {
        display_message("Not enough mana to cast Fireball!");
        return;
    }

    int target_x = pc.x;
    int target_y = pc.y;

    bool old_fog_toggle = fog_toggle;
    fog_toggle = true;

    display_message("Fireball mode: move to center, press 'f' to cast, ESC to cancel.");

    bool done = false;
    while (!done) {
        display_dungeon();
        mvaddch(target_y + 1, target_x, '*'); // Draw targeting cursor
        refresh();

        int ch = getch();
        switch (ch) {
            case 27: // ESC
                display_message("Spell casting canceled.");
                done = true;
                break;
            case '7': case 'y':
                if (inBounds(target_x - 1, target_y - 1))
                    target_x--, target_y--;
                break;
            case '8': case 'k':
                if (inBounds(target_x, target_y - 1))
                    target_y--;
                break;
            case '9': case 'u':
                if (inBounds(target_x + 1, target_y - 1))
                    target_x++, target_y--;
                break;
            case '6': case 'l':
                if (inBounds(target_x + 1, target_y))
                    target_x++;
                break;
            case '3': case 'n':
                if (inBounds(target_x + 1, target_y + 1))
                    target_x++, target_y++;
                break;
            case '2': case 'j':
                if (inBounds(target_x, target_y + 1))
                    target_y++;
                break;
            case '1': case 'b':
                if (inBounds(target_x - 1, target_y + 1))
                    target_x--, target_y++;
                break;
            case '4': case 'h':
                if (inBounds(target_x - 1, target_y))
                    target_x--;
                break;
            case 'f': { // 'f' to cast
                int radius = 1;

                // EXPLOSION FLASH
                for (int dy = -radius; dy <= radius; dy++) {
                    for (int dx = -radius; dx <= radius; dx++) {
                        int ex = target_x + dx;
                        int ey = target_y + dy;
                        if (inBounds(ex, ey) && dx*dx + dy*dy <= radius*radius) {
                            move(ey + 1, ex);
                            attron(COLOR_PAIR(1)); // RED Explosion
                            addch('*');
                            attroff(COLOR_PAIR(1));
                        }
                    }
                }
                refresh();
                napms(300);
                display_dungeon();

                pc.mana -= 5; // Spend mana

                int monsters_hit = 0;
                for (auto &ch : characters) {
                    if (ch.alive && ch.type == CharType::Monster) {
                        int dx = ch.x - target_x;
                        int dy = ch.y - target_y;
                        if (dx*dx + dy*dy <= radius*radius) {
                            int damage = 10 + (rand() % 6); // 10-15 massive fire damage
                            ch.hp -= damage;
                            monsters_hit++;
                            if (ch.hp <= 0) {
                                ch.alive = false;
                                dungeon[ch.y][ch.x] = base_map[ch.y][ch.x];
                            }
                        }
                    }
                }
                if (monsters_hit > 0) {
                    display_message("Fireball explodes! Massive damage!");
                } else {
                    display_message("Fireball explodes harmlessly.");
                }
                done = true;
                break;
            }
            default:
                break;
        }
    }

    fog_toggle = old_fog_toggle;
    display_dungeon();
}


void handle_monster_look_mode(character_t &pc) {
    int target_x = pc.x;
    int target_y = pc.y;
    bool old_fog = fog_toggle;
    fog_toggle = true;

    display_message("Look mode: Move with hjkl+yubn. Press 't' to inspect. ESC to exit.");

    while (true) {
        display_dungeon();

        // Draw cursor
        mvaddch(target_y + 1, target_x, '*');
        refresh();

        int ch = getch();
        if (ch == 27) {  // ESC
            display_message("Exited look mode.");
            break;
        }

        // Directional movement
        int dx = 0, dy = 0;
        if (ch == 'h' || ch == '4') dx = -1;
        else if (ch == 'l' || ch == '6') dx = 1;
        else if (ch == 'j' || ch == '2') dy = 1;
        else if (ch == 'k' || ch == '8') dy = -1;
        else if (ch == 'y' || ch == '7') { dx = -1; dy = -1; }
        else if (ch == 'u' || ch == '9') { dx = 1; dy = -1; }
        else if (ch == 'b' || ch == '1') { dx = -1; dy = 1; }
        else if (ch == 'n' || ch == '3') { dx = 1; dy = 1; }

        int nx = target_x + dx;
        int ny = target_y + dy;
        if (inBounds(nx, ny)) {
            target_x = nx;
            target_y = ny;
        }

        // Press 't' to inspect a monster at the cursor
        if (ch == 't') {
            for (const auto &mon : characters) {
                if (mon.alive && mon.type == CharType::Monster &&
                    mon.x == target_x && mon.y == target_y) {

                    clear();
                    mvprintw(0, 0, "=== Monster ===");
                    mvprintw(1, 0, "Symbol: %c", mon.symbol);
                    mvprintw(2, 0, "HP: %d", mon.hp);
                    mvprintw(3, 0, "Speed: %d", mon.speed);
                    mvprintw(4, 0, "Position: (%d, %d)", mon.x, mon.y);
                    mvprintw(5, 0, "Press any key...");
                    refresh();
                    getch();
                    break;
                }
            }
        }
    }

    fog_toggle = old_fog;
    display_dungeon();
}



void handle_pc_input(character_t &pc) {
    while (true) {
        int ch = getch();
        switch (ch) {
            case '7': case 'y': {
                int nx = pc.x - 1, ny = pc.y - 1;

                // Check for monster at target cell
                for (auto &target : characters) {
                    if (target.alive && target.x == nx && target.y == ny && target.type == CharType::Monster) {
                        perform_attack(pc, target);
                        return; 
                    }
                }
                if (inBounds(nx, ny) && pc_can_walk_on(dungeon[ny][nx])) {
                    dungeon[pc.y][pc.x] = base_map[pc.y][pc.x];
                    pc.x = nx; pc.y = ny;
                    dungeon[ny][nx] = '@';
                    try_pickup_item(pc);
                } else {
                    display_message("Blocked!");
                }
                return;
            }
            case '8': case 'k': {
                int nx = pc.x, ny = pc.y - 1;
                // Check for monster at target cell
                for (auto &target : characters) {
                    if (target.alive && target.x == nx && target.y == ny && target.type == CharType::Monster) {
                        perform_attack(pc, target);
                        return; // No movement if attack occurs
                    }
                }
                if (inBounds(nx, ny) && pc_can_walk_on(dungeon[ny][nx])) {
                    dungeon[pc.y][pc.x] = base_map[pc.y][pc.x];
                    pc.y = ny;
                    dungeon[ny][nx] = '@';
                    try_pickup_item(pc);
                } else {
                    display_message("Blocked!");
                }
                return;
            }
            case '9': case 'u': {
                int nx = pc.x + 1, ny = pc.y - 1;
                if (inBounds(nx, ny) && pc_can_walk_on(dungeon[ny][nx])) {
                    dungeon[pc.y][pc.x] = base_map[pc.y][pc.x];
                    pc.x = nx; pc.y = ny;
                    dungeon[ny][nx] = '@';
                    try_pickup_item(pc);
                } else {
                    display_message("Blocked!");
                }
                return;
            }
            case '6': case 'l': {
                int nx = pc.x + 1, ny = pc.y;
                for (auto &target : characters) {
                    if (target.alive && target.x == nx && target.y == ny && target.type == CharType::Monster) {
                        perform_attack(pc, target);
                        return; // No movement if attack occurs
                    }
                }
                if (inBounds(nx, ny) && pc_can_walk_on(dungeon[ny][nx])) {
                    dungeon[pc.y][pc.x] = base_map[pc.y][pc.x];
                    pc.x = nx;
                    dungeon[ny][nx] = '@';
                    try_pickup_item(pc);
                } else {
                    display_message("Blocked!");
                }
                return;
            }
            case '3': case 'n': {
                int nx = pc.x + 1, ny = pc.y + 1;
                for (auto &target : characters) {
                    if (target.alive && target.x == nx && target.y == ny && target.type == CharType::Monster) {
                        perform_attack(pc, target);
                        return; // No movement if attack occurs
                    }
                }
                if (inBounds(nx, ny) && pc_can_walk_on(dungeon[ny][nx])) {
                    dungeon[pc.y][pc.x] = base_map[pc.y][pc.x];
                    pc.x = nx; pc.y = ny;
                    dungeon[ny][nx] = '@';
                    try_pickup_item(pc);
                } else {
                    display_message("Blocked!");
                }
                return;
            }
            case '2': case 'j': {
                int nx = pc.x, ny = pc.y + 1;
                if (inBounds(nx, ny) && pc_can_walk_on(dungeon[ny][nx])) {
                    dungeon[pc.y][pc.x] = base_map[pc.y][pc.x];
                    pc.y = ny;
                    dungeon[ny][nx] = '@';
                    try_pickup_item(pc);
                } else {
                    display_message("Blocked!");
                }
                return;
            }
            case '1': case 'b': {
                int nx = pc.x - 1, ny = pc.y + 1;
                for (auto &target : characters) {
                    if (target.alive && target.x == nx && target.y == ny && target.type == CharType::Monster) {
                        perform_attack(pc, target);
                        return; // No movement if attack occurs
                    }
                }
                if (inBounds(nx, ny) && pc_can_walk_on(dungeon[ny][nx])) {
                    dungeon[pc.y][pc.x] = base_map[pc.y][pc.x];
                    pc.x = nx; pc.y = ny;
                    dungeon[ny][nx] = '@';
                    try_pickup_item(pc);
                } else {
                    display_message("Blocked!");
                }
                return;
            }
            case '4': case 'h': {
                int nx = pc.x - 1, ny = pc.y;
                for (auto &target : characters) {
                    if (target.alive && target.x == nx && target.y == ny && target.type == CharType::Monster) {
                        perform_attack(pc, target);
                        return; // No movement if attack occurs
                    }
                }
                if (inBounds(nx, ny) && pc_can_walk_on(dungeon[ny][nx])) {
                    dungeon[pc.y][pc.x] = base_map[pc.y][pc.x];
                    pc.x = nx;
                    dungeon[ny][nx] = '@';
                    try_pickup_item(pc);
                } else {
                    display_message("Blocked!");
                }
                return;
            }
            // g now activates teleport mode.
            case 'g': {
                handle_teleport_mode(pc);
                return;
            }
            case 'i': {
                clear();
                mvprintw(0, 0, "--- Inventory (0-9) ---");
                character_t &pc = characters[0];  // assuming PC is first
            
                int row = 1;
                for (int i = 0; i < character_t::MAX_CARRY; ++i) {
                    if (pc.inventory[i])
                        mvprintw(row++, 0, "%d: %s", i, pc.inventory[i]->name.c_str());
                    else
                        mvprintw(row++, 0, "%d: <empty>", i);
                }
            
                mvprintw(row + 1, 0, "Press any key to continue...");
                refresh();
                getch();
                display_dungeon();
                break;
            }
            case 'e': {
                clear();
                mvprintw(0, 0, "--- Equipment (a-l) ---");
                character_t &pc = characters[0];
            
                const char* slot_names[] = {
                    "a: WEAPON", "b: OFFHAND", "c: RANGED", "d: ARMOR", "e: HELMET", "f: CLOAK",
                    "g: GLOVES", "h: BOOTS", "i: AMULET", "j: LIGHT", "k: RING1", "l: RING2"
                };
            
                int row = 1;
                for (int i = 0; i < NUM_EQUIP_SLOTS; ++i) {
                    if (pc.equipment[i])
                        mvprintw(row++, 0, "%s - %s", slot_names[i], pc.equipment[i]->name.c_str());
                    else
                        mvprintw(row++, 0, "%s - <empty>", slot_names[i]);
                }
            
                mvprintw(row + 1, 0, "Press any key to continue...");
                refresh();
                getch();
                display_dungeon();
                break;
            }
            case 'I': {
                display_message("Inspect item (0-9), ESC to cancel");
                int ch = getch();
                if (ch >= '0' && ch <= '9') {
                    int idx = ch - '0';
                    character_t &pc = characters[0];
                    if (pc.inventory[idx]) {
                        clear();
                        mvprintw(0, 0, "=== %s ===", pc.inventory[idx]->name.c_str());
                        mvprintw(1, 0, "%s", object_templates[0].description.c_str()); // FIX: this line later
                        mvprintw(3, 0, "Press any key to return.");
                        refresh();
                        getch();
                    } else {
                        display_message("Empty slot.");
                    }
                } else {
                    display_message("Inspection cancelled.");
                }
                break;
            }
            case 'f': {
                fog_toggle = !fog_toggle;
                if (fog_toggle)
                    display_message("Fog disabled: full dungeon view.");
                else
                    display_message("Fog enabled: dungeon with fog of war.");
                return;
            }
            // Use stairs to go down.
            case '>': { 
            // Check underlying terrain from base_map.
            if (base_map[pc.y][pc.x] == '>') {
                new_level(DEFAULT_NUMMON);
                display_message("You go down the stairs...");
            } else {
                display_message("No downward staircase here!");
            }
                return;
            }
            case 'w': {
                display_message("Wear item from inventory (0-9), ESC to cancel");
                int ch = getch();
                if (ch >= '0' && ch <= '9') {
                    int idx = ch - '0';
                    character_t &pc = characters[0];
                    if (!pc.inventory[idx]) {
                        display_message("No item in that slot.");
                        break;
                    }
            
                    ObjectType type = object_templates[0].type; // Fix this later when templates are linked
            
                    int slot = -1;
                    switch (type) {
                        case ObjectType::WEAPON: slot = 0; break;
                        case ObjectType::OFFHAND: slot = 1; break;
                        case ObjectType::RANGED: slot = 2; break;
                        case ObjectType::ARMOR: slot = 3; break;
                        case ObjectType::HELMET: slot = 4; break;
                        case ObjectType::CLOAK: slot = 5; break;
                        case ObjectType::GLOVES: slot = 6; break;
                        case ObjectType::BOOTS: slot = 7; break;
                        case ObjectType::AMULET: slot = 8; break;
                        case ObjectType::LIGHT: slot = 9; break;
                        case ObjectType::RING:
                            slot = pc.equipment[10] ? 11 : 10; // Pick first free ring
                            break;
                        default:
                            display_message("Can't equip that type.");
                            return;
                    }
            
                    if (slot < 0 || slot >= NUM_EQUIP_SLOTS) {
                        display_message("Invalid slot.");
                        return;
                    }
            
                    if (pc.equipment[slot]) {
                        // Swap
                        std::swap(pc.inventory[idx], pc.equipment[slot]);
                        display_message("Swapped with equipped item.");
                    } else {
                        // Equip directly
                        pc.equipment[slot] = pc.inventory[idx];
                        pc.inventory[idx].reset();
                        display_message("Item equipped.");
                    }
                } else {
                    display_message("Wear cancelled.");
                }
                break;
            }
            case 't': {
                display_message("Take off equipment (a-l), ESC to cancel");
                int ch = getch();
                int slot = -1;
                if (ch >= 'a' && ch <= 'l')
                    slot = ch - 'a';
                else {
                    display_message("Cancelled.");
                    break;
                }
            
                character_t &pc = characters[0];
                if (!pc.equipment[slot]) {
                    display_message("Nothing in that slot.");
                    break;
                }
            
                // Find an open carry slot
                for (int i = 0; i < character_t::MAX_CARRY; ++i) {
                    if (!pc.inventory[i]) {
                        pc.inventory[i] = pc.equipment[slot];
                        pc.equipment[slot].reset();
                        display_message("Item taken off.");
                        return;
                    }
                }
            
                display_message("Inventory full. Cannot unequip.");
                break;
            }
            case 'd': {
                display_message("Drop item (0-9), ESC to cancel");
                int ch = getch();
                if (ch >= '0' && ch <= '9') {
                    int idx = ch - '0';
                    character_t &pc = characters[0];
                    if (!pc.inventory[idx]) {
                        display_message("Nothing in that slot.");
                        break;
                    }
            
                    ObjectInstance &item = *pc.inventory[idx];
                    item.x = pc.x;
                    item.y = pc.y;
                    object_instances.push_back(item);
                    pc.inventory[idx].reset();
                    display_message("Item dropped.");
                } else {
                    display_message("Drop cancelled.");
                }
                break;
            }
            case 'x': {
                display_message("Expunge item (0-9), ESC to cancel");
                int ch = getch();
                if (ch >= '0' && ch <= '9') {
                    int idx = ch - '0';
                    character_t &pc = characters[0];
                    if (!pc.inventory[idx]) {
                        display_message("No item to expunge.");
                    } else {
                        pc.inventory[idx].reset();
                        display_message("Item destroyed.");
                    }
                } else {
                    display_message("Expunge cancelled.");
                }
                break;
            }
            
            
                // Use stairs to go up.
            case '<': {
                if (base_map[pc.y][pc.x] == '<') {
                    new_level(DEFAULT_NUMMON);
                    display_message("You go up the stairs...");
                } else {
                display_message("No upward staircase here!");
                }
                return;
            }
            case 'L': {
                handle_monster_look_mode(pc);
                return;
            }            
            case 'm': {
                display_monster_list();
                break;
            }
            case 'Q': {
                return;
            }
            case 'a': {
                handle_ranged_attack_mode(pc);
                return;
            }
            case 'p': {
                handle_magic_spell_mode(pc);
                return;
            }
            case 'F': {
                handle_fireball_spell_mode(pc);
                return;
            }                        
            default:
                break;
        }
    }
}


void try_pickup_item(character_t &pc) {
    for (auto it = object_instances.begin(); it != object_instances.end(); ++it) {
        if (it->x == pc.x && it->y == pc.y) {
            // Find an empty inventory slot
            for (auto &slot : pc.inventory) {
                if (!slot.has_value()) {
                    slot = *it;
                    display_message("You picked up: " + it->name);
                    object_instances.erase(it);
                    return;
                }
            }
            display_message("Inventory full! Can't pick up " + it->name);
            return;
        }
    }
}

