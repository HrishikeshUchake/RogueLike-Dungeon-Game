#include "object_generator.h"
#include "object_template.h"
#include "object_instance.h"
#include "global.h"
#include <cstdlib>
#include <vector> // Include vector for characters
#include "character.h" // Include the header where characters are defined


bool cell_is_occupied(int x, int y) {
    if (dungeon[y][x] != '.') return true;

    for (const auto& ch : characters) {
        if (ch.alive && ch.x == x && ch.y == y)
            return true;
    }

    for (const auto& obj : object_instances) {
        if (obj.x == x && obj.y == y)
            return true;
    }

    return false;
}

void generate_objects(int count) {
    object_instances.clear();

    for (int i = 0; i < count; ) {
        const ObjectTemplate* chosen = nullptr;

        for (int attempt = 0; attempt < 1000; ++attempt) {
            const auto& candidate = object_templates[rand() % object_templates.size()];
            if (candidate.artifact && seen_artifacts.count(candidate.name)) continue;
            if ((rand() % 100) < candidate.rarity) {
                chosen = &candidate;
                break;
            }
        }

        if (!chosen) continue; // No valid object found

        ObjectInstance obj = chosen->generate_instance();

        // Find a safe, unoccupied floor tile
        int rx, ry, attempts = 0;
        do {
            rx = rand() % WIDTH;
            ry = rand() % HEIGHT;
            attempts++;
        } while (cell_is_occupied(rx, ry) && attempts < 1000);

        if (attempts >= 1000) continue;

        obj.x = rx;
        obj.y = ry;

        if (obj.is_artifact)
            seen_artifacts.insert(obj.name);

        object_instances.push_back(obj);
        std::cout << "Placed " << object_instances.size() << " objects in the dungeon." << std::endl;
        ++i;
    }
}

