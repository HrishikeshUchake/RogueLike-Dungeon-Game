# Roguelike Dungeon Generator (C++)

A terminal-based roguelike game built in modern **C++**, featuring **procedural dungeon generation**, **fog-of-war**, **turn-based combat**, and **teleportation mechanics**. Uses **ncurses** for a retro terminal UI and supports saving/loading dungeons via command-line options.

---

## Features

- Random dungeon generation: rooms, corridors, staircases
- Fog-of-war: visibility limited to nearby tiles
- Turn-based gameplay with an event queue system
- Save and load dungeon state from disk `~/.rlg327`
- Create your own monster definitions! Saved and loaded from custom `~/.rlg327/monster_desc.txt` descriptors
- Create your own object definitions! Saved and loaded from custom `~/.rlg327/object_desc.txt` descriptors
- Special features: teleportation, ranged attacks, magic spells, and equipment management

---

## Controls

| Key                     | Action                                  |
|-------------------------|-----------------------------------------|
| `7/8/9/4/6/1/2/3`       | Move (numeric pad or y/k/u/h/l/b/j/n)   |
| `5`, `.`, `Space`       | Rest / Skip turn                        |
| `<`, `>`                | Ascend/Descend stairs (on stair tiles)  |
| `f`                     | Toggle fog of war                       |
| `g`                     | Enter teleport mode                     |
| `r`                     | Random teleport target (while in mode)  |
| `m`                     | View visible monsters                   |
| `L`                     | Look mode (inspect)                     |
| `i`                     | Show inventory                          |
| `e`                     | Show equipment                          |
| `I`                     | Inspect inventory item                  |
| `w`                     | Wear item                               |
| `t`                     | Take off equipment                      |
| `d`                     | Drop item                               |
| `a`                     | Enter ranged attack mode (`f` to fire) |
| `p`                     | Cast Poison Ball                        |
| `F`                     | Cast Fireball                           |
| `Q`                     | Quit the game                           |

---

## Usage

### Compile

```bash
make
````

### Run

```bash
./dungeon
```

### Save

```bash
./dungeon --save
```

### Load

```bash
./dungeon --load
```

### Parse Monster Descriptions

```bash
./dungeon --parse
```

> Ensure the following files exist with valid content:
>
> * `~/.rlg327/monster_desc.txt`
> * `~/.rlg327/object_desc.txt`

---

## Screenshots

<p align="center">
  <img src="https://github.com/user-attachments/assets/8177f825-c4d9-4917-9e53-ab0e4e7fcf66" width="600" alt="Dungeon View"/>
  <img src="https://github.com/user-attachments/assets/9980f501-5359-4214-8b03-690e21a85b54" width="450" alt="Monster List"/>
  <img src="https://github.com/user-attachments/assets/e6b2e63a-d97e-4b21-a8af-1d0975f4d53b" width="250" alt="Teleportation"/>
  <img src="https://github.com/user-attachments/assets/3f905cbe-5836-4602-a48f-346186ded782" width="800" alt="Gameplay"/>
  <img src="https://github.com/user-attachments/assets/41ebda2c-fea2-480c-8e13-16a7f8ed5a96" width="500" alt="Save Game"/>

https://github.com/user-attachments/assets/7689e3df-9ac0-4082-a926-9071bf435735


https://github.com/user-attachments/assets/cbf559d0-0570-47e2-8a9b-e859053c4b2a



</p>

---

## License

MIT License — feel free to use, modify, or expand this project for personal or academic use.

---

## Author

Built by [Hrishikesh Uchake](https://github.com/HrishikeshUchake)




