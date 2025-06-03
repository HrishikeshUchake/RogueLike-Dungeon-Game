Roguelike Dungeon Generator
This is a simple C++ roguelike dungeon generator that creates random dungeons with rooms, corridors, stairs, fog-of-war, and teleporting. It uses ncurses for a cool terminal UI and lets you save/load your dungeon state.

Features
Random dungeon generation (rooms, corridors, stairs)

Fog-of-war: you only see what’s near your PC

Teleport mode: press g to target a new spot and teleport (or pick a random target with r)

Turn-based gameplay with an event queue

Save/load dungeon state via command-line options

Controls
Movement: 7/8/9/4/6/1/2/3 or y/k/u/h/l/b/j/n

Stairs: > to go down, < to go up (only works when you’re on stairs)

Rest: 5, . or space

Toggle fog: f

Teleport: g to enter teleport mode; use movement keys or r for a random target; press g again to confirm

Monster list: m

Quit: Q

Usage: 
Compile with:
make


Run with:
./dungeon


To save, run:
./dungeon --save


To load, run:
./dungeon --load


To parse monster desc:
./dungeon --parse
(make sure there's content in ~/.rlg327/monster_desc.txt in a correct format)


Ensure ~/.rlg327/object_desc.txt exists and contains valid Monster definitions.
Ensure ~/.rlg327/object_desc.txt exists and contains valid object definitions.


Key | Action
7/8/9/4/6/1/2/3 or y/k/u/h/l/b/j/n | Move in 8 directions
5, ., or Space | Rest (skip turn)
< / > | Go up/down stairs (when on stair tile)
f | Toggle fog of war
g | Enter teleport mode → move to target and press g again to confirm
r | Random teleport target (while in teleport mode)
m | Show monster list and their relative positions
Q | Quit the game
m | Show visible monsters 
L | Look mode to inspect monsters 
i | Show inventory 
e | Show equipment 
I | Inspect inventory item 
w | Wear item 
t | Take off equipment 
d | Drop item 
a | Enter ranged attack mode (target and fire with f)
p | Cast Poison Ball spell (area of effect magic attack)
F | Cast Fireball spell (powerful area magic attack)
