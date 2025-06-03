#include "dungeon.h"
#include "global.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#ifdef __APPLE__
  #include <libkern/OSByteOrder.h>
  #define be32toh(x) OSSwapBigToHostInt32(x)
  #define htobe32(x) OSSwapHostToBigInt32(x)
  #define be16toh(x) OSSwapBigToHostInt16(x)
  #define htobe16(x) OSSwapHostToBigInt16(x)
#else
  #include <endian.h>
#endif



// Internal helper to fill a room.
static void fillRoom(int w, int h, int x, int y) {
    for (int row = y; row < y + h; row++) {
        for (int col = x; col < x + w; col++) {
            dungeon[row][col] = '.';
            hardness[row][col] = 0;
        }
    }
}

// Internal helper to check room validity.
static bool isValidRoom(int rw, int rh, int rx, int ry) {
    if (rw < 1 || rh < 1 || (rw + rx >= WIDTH - 1) || (rh + ry >= HEIGHT - 1))
        return false;
    for (int row = ry; row < ry + rh; row++) {
        for (int col = rx; col < rx + rw; col++) {
            if (dungeon[row][col] != ' ')
                return false;
        }
    }
    return true;
}

void initializeDungeon() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == 0 || x == WIDTH - 1 || y == 0 || y == HEIGHT - 1) {
                dungeon[y][x] = ' ';
                hardness[y][x] = 255;
            } else {
                dungeon[y][x] = ' ';
                hardness[y][x] = (rand() % 254) + 1;
            }
        }
    }
}

void generateRooms() {
    int attempts = 2000;
    int c = 0;
    while (attempts > 0 && c < 6) {
        int rw = (rand() % 6) + 4;
        int rh = (rand() % 4) + 3;
        int rx = (rand() % (WIDTH - rw - 2)) + 1;
        int ry = (rand() % (HEIGHT - rh - 2)) + 1;
        if (isValidRoom(rw, rh, rx, ry)) {
            fillRoom(rw, rh, rx, ry);
            room_x[c] = rx; room_y[c] = ry;
            room_w[c] = rw; room_h[c] = rh;
            c++;
        }
        attempts--;
    }
    room_count = c;
}

void connectRoomsViaCorridor() {
    if (room_count < 2) return;
    for (int i = 1; i < room_count; i++) {
        int x1 = room_x[i - 1] + room_w[i - 1] / 2;
        int y1 = room_y[i - 1] + room_h[i - 1] / 2;
        int x2 = room_x[i] + room_w[i] / 2;
        int y2 = room_y[i] + room_h[i] / 2;
        while (x1 != x2) {
            if (x1 >= 0 && x1 < WIDTH && y1 >= 0 && y1 < HEIGHT) {
                if (dungeon[y1][x1] != '.') {
                    dungeon[y1][x1] = '#';
                    hardness[y1][x1] = 0;
                }
            }
            x1 += (x2 > x1) ? 1 : -1;
        }
        while (y1 != y2) {
            if (x1 >= 0 && x1 < WIDTH && y1 >= 0 && y1 < HEIGHT) {
                if (dungeon[y1][x1] != '.') {
                    dungeon[y1][x1] = '#';
                    hardness[y1][x1] = 0;
                }
            }
            y1 += (y2 > y1) ? 1 : -1;
        }
    }
}

void placeStairs() {
    bool flag = true, upFlag = false, downFlag = false;
    while (flag) {
        int up_x = rand() % WIDTH, up_y = rand() % HEIGHT;
        int down_x = rand() % WIDTH, down_y = rand() % HEIGHT;
        if ((dungeon[up_y][up_x] == '.' || dungeon[up_y][up_x] == '#') && !upFlag) {
            dungeon[up_y][up_x] = '<';
            up_xCoord = up_x; up_yCoord = up_y;
            upCount = 1; upFlag = true;
        }
        if ((dungeon[down_y][down_x] == '.' || dungeon[down_y][down_x] == '#') &&
            !(up_x == down_x && up_y == down_y) && !downFlag) {
            dungeon[down_y][down_x] = '>';
            down_xCoord = down_x; down_yCoord = down_y;
            downCount = 1; downFlag = true;
        }
        if (upFlag && downFlag)
            flag = false;
    }
}

void placePC(int x, int y) {
    dungeon[y][x] = '@';
}

void load_dungeon(const char* path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        std::cerr << "Error opening file: " << path << std::endl;
        exit(1);
    }
    char marker[MARKER_LEN + 1];
    in.read(marker, MARKER_LEN);
    marker[MARKER_LEN] = '\0';
    if (strcmp(marker, FILE_MARKER) != 0) {
        std::cerr << "Invalid marker" << std::endl;
        in.close();
        exit(1);
    }
    uint32_t version;
    in.read(reinterpret_cast<char*>(&version), sizeof(version));
    version = be32toh(version);
    if (version != FILE_VERSION) {
        std::cerr << "Unsupported file version" << std::endl;
        in.close();
        exit(1);
    }
    uint32_t file_size;
    in.read(reinterpret_cast<char*>(&file_size), sizeof(file_size));
    file_size = be32toh(file_size);
    
    uint8_t pcx, pcy;
    in.read(reinterpret_cast<char*>(&pcx), 1);
    in.read(reinterpret_cast<char*>(&pcy), 1);
    pc_x = pcx; pc_y = pcy;
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            uint8_t h;
            in.read(reinterpret_cast<char*>(&h), 1);
            hardness[y][x] = h;
        }
    }
    uint16_t r;
    in.read(reinterpret_cast<char*>(&r), sizeof(r));
    r = be16toh(r);
    room_count = 0;
    for (int i = 0; i < r && i < MAX_ROOMS; i++) {
        uint8_t rx, ry, rw, rh;
        in.read(reinterpret_cast<char*>(&rx), 1);
        in.read(reinterpret_cast<char*>(&ry), 1);
        in.read(reinterpret_cast<char*>(&rw), 1);
        in.read(reinterpret_cast<char*>(&rh), 1);
        room_x[i] = rx; room_y[i] = ry;
        room_w[i] = rw; room_h[i] = rh;
        room_count++;
    }
    uint16_t u;
    in.read(reinterpret_cast<char*>(&u), sizeof(u));
    u = be16toh(u);
    upCount = 0;
    if (u > 0) {
        uint8_t sx, sy;
        in.read(reinterpret_cast<char*>(&sx), 1);
        in.read(reinterpret_cast<char*>(&sy), 1);
        upCount = 1; up_xCoord = sx; up_yCoord = sy;
    }
    uint16_t d;
    in.read(reinterpret_cast<char*>(&d), sizeof(d));
    d = be16toh(d);
    downCount = 0;
    if (d > 0) {
        uint8_t sx, sy;
        in.read(reinterpret_cast<char*>(&sx), 1);
        in.read(reinterpret_cast<char*>(&sy), 1);
        downCount = 1; down_xCoord = sx; down_yCoord = sy;
    }
    in.close();
    
    // Rebuild dungeon array from hardness and room data.
    for (int yy = 0; yy < HEIGHT; yy++) {
        for (int xx = 0; xx < WIDTH; xx++) {
            if (hardness[yy][xx] == 255)
                dungeon[yy][xx] = ' ';
            else if (hardness[yy][xx] > 0)
                dungeon[yy][xx] = ' ';
            else
                dungeon[yy][xx] = '#';
        }
    }
    for (int i = 0; i < room_count; i++) {
        for (int row = room_y[i]; row < room_y[i] + room_h[i]; row++) {
            for (int col = room_x[i]; col < room_x[i] + room_w[i]; col++) {
                dungeon[row][col] = '.';
            }
        }
    }
    if (upCount > 0)
        dungeon[up_yCoord][up_xCoord] = '<';
    if (downCount > 0)
        dungeon[down_yCoord][down_xCoord] = '>';
}

void save_dungeon(const char* path) {
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        std::cerr << "Error opening file for writing" << std::endl;
        return;
    }
    out.write(FILE_MARKER, MARKER_LEN);
    uint32_t version_be = htobe32(FILE_VERSION);
    out.write(reinterpret_cast<const char*>(&version_be), sizeof(version_be));
    
    uint16_t up_stairs_count = (upCount > 0) ? 1 : 0;
    uint16_t down_stairs_count = (downCount > 0) ? 1 : 0;
    uint32_t file_size = 1702 + (room_count * 4) + 2 + (up_stairs_count * 2) + 2 + (down_stairs_count * 2);
    uint32_t file_size_be = htobe32(file_size);
    out.write(reinterpret_cast<const char*>(&file_size_be), sizeof(file_size_be));
    
    uint8_t pcx = pc_x, pcy = pc_y;
    out.write(reinterpret_cast<const char*>(&pcx), 1);
    out.write(reinterpret_cast<const char*>(&pcy), 1);
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            uint8_t hh = hardness[y][x];
            out.write(reinterpret_cast<const char*>(&hh), 1);
        }
    }
    uint16_t r_be = htobe16(room_count);
    out.write(reinterpret_cast<const char*>(&r_be), sizeof(r_be));
    for (int i = 0; i < room_count; i++) {
        uint8_t rx = room_x[i], ry = room_y[i], rw = room_w[i], rh = room_h[i];
        out.write(reinterpret_cast<const char*>(&rx), 1);
        out.write(reinterpret_cast<const char*>(&ry), 1);
        out.write(reinterpret_cast<const char*>(&rw), 1);
        out.write(reinterpret_cast<const char*>(&rh), 1);
    }
    uint16_t up_be = htobe16(up_stairs_count);
    out.write(reinterpret_cast<const char*>(&up_be), sizeof(up_be));
    if (up_stairs_count == 1) {
        uint8_t sx = static_cast<uint8_t>(up_xCoord), sy = static_cast<uint8_t>(up_yCoord);
        out.write(reinterpret_cast<const char*>(&sx), 1);
        out.write(reinterpret_cast<const char*>(&sy), 1);
    }
    uint16_t down_be = htobe16(downCount > 0 ? 1 : 0);
    out.write(reinterpret_cast<const char*>(&down_be), sizeof(down_be));
    if (downCount > 0) {
        uint8_t sx = static_cast<uint8_t>(down_xCoord), sy = static_cast<uint8_t>(down_yCoord);
        out.write(reinterpret_cast<const char*>(&sx), 1);
        out.write(reinterpret_cast<const char*>(&sy), 1);
    }
    out.close();
}

void checkDir() {
    const char* home = getenv("HOME");
    if (!home) {
        std::cerr << "ERROR: No HOME env var." << std::endl;
        exit(1);
    }
    char path[1024];
    snprintf(path, sizeof(path), "%s%s", home, DUNGEON_DIR);
    if (mkdir(path, 0700) && errno != EEXIST) {
        std::cerr << "ERROR creating " << path << ": " << strerror(errno) << std::endl;
        exit(1);
    }
}

void getPath(char* buf, size_t size) {
    const char* home = getenv("HOME");
    snprintf(buf, size, "%s%s%s", home, DUNGEON_DIR, DUNGEON_FILE);
}
