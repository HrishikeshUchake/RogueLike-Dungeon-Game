#ifndef DUNGEON_H
#define DUNGEON_H

#include "global.h"

// Dungeon generation functions
void initializeDungeon();
void generateRooms();
void connectRoomsViaCorridor();
void placeStairs();
void placePC(int x, int y);

// File I/O functions
void load_dungeon(const char* path);
void save_dungeon(const char* path);

// Helper functions for directory/path creation
void checkDir();
void getPath(char* buf, size_t size);

#endif // DUNGEON_H
