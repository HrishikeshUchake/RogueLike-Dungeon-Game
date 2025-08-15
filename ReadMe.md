# Roguelike Dungeon Generator (C++)

A terminal-based roguelike game built in modern **C++**, featuring **procedural dungeon generation**, **fog-of-war**, **turn-based combat**, and **teleportation mechanics**. Uses **ncurses** for a retro terminal UI and supports saving/loading dungeons via command-line options.

---

## Features

### Core Gameplay
- **Procedural dungeon generation**: Randomly generated rooms, corridors, and staircases
- **Fog-of-war system**: Visibility limited to nearby tiles for immersive exploration
- **Turn-based combat**: Strategic gameplay with an event queue system
- **Persistent save system**: Save and load dungeon state from disk `~/.rlg327`

### Customization
- **Custom monster definitions**: Create your own monsters! Saved and loaded from `~/.rlg327/monster_desc.txt`
- **Custom object definitions**: Design your own items! Saved and loaded from `~/.rlg327/object_desc.txt`
- **Equipment system**: Wear, remove, and manage various items and gear

### Special Abilities
- **Teleportation mechanics**: Strategic movement across the dungeon
- **Ranged attacks**: Combat from a distance
- **Magic spells**: Poison Ball and Fireball casting
- **Inventory management**: Complete item handling system

### Technical Features
- Built with **ncurses** for cross-platform terminal graphics
- **CHANGELOG** tracking for version releases
- Command-line argument support for various game modes

---

## Prerequisites

- **C++ Compiler** (GCC/Clang with C++11 support or higher)
- **ncurses library**
  ```bash
  # Ubuntu/Debian
  sudo apt-get install libncurses5-dev libncursesw5-dev
  
  # macOS
  brew install ncurses
  
  # CentOS/RHEL
  sudo yum install ncurses-devel
  ```
- **Make** build system

---

## Installation & Setup

### 1. Clone the Repository
```bash
git clone https://github.com/HrishikeshUchake/RogueLike-Dungeon-Game.git
cd RogueLike-Dungeon-Game
```

### 2. Compile
```bash
make clean  # Clean previous builds
make        # Compile the game
```

### 3. Setup Configuration Directory
The game automatically creates `~/.rlg327/` directory for save files and custom definitions.

---

## Usage

### Basic Gameplay
```bash
./dungeon           # Start new game
```

### Save & Load
```bash
./dungeon --save    # Save current dungeon state
./dungeon --load    # Load previously saved dungeon
```

### Custom Content
```bash
./dungeon --parse   # Parse custom monster and object descriptions
```

> **Note**: Ensure the following files exist with valid content:
> - `~/.rlg327/monster_desc.txt` - Custom monster definitions
> - `~/.rlg327/object_desc.txt` - Custom object definitions

---

## Controls

### Movement
| Key                     | Action                                  |
|-------------------------|-----------------------------------------|
| `7/8/9/4/6/1/2/3`       | Move (numeric pad or y/k/u/h/l/b/j/n)   |
| `5`, `.`, `Space`       | Rest / Skip turn                        |
| `<`, `>`                | Ascend/Descend stairs (on stair tiles)  |

### Combat & Magic
| Key | Action                               |
|-----|--------------------------------------|
| `a` | Enter ranged attack mode (`f` to fire) |
| `p` | Cast Poison Ball                     |
| `F` | Cast Fireball                        |

### Inventory & Equipment
| Key | Action                    |
|-----|---------------------------|
| `i` | Show inventory            |
| `e` | Show equipment            |
| `I` | Inspect inventory item    |
| `w` | Wear item                 |
| `t` | Take off equipment        |
| `d` | Drop item                 |

### Exploration & Interface
| Key | Action                          |
|-----|---------------------------------|
| `f` | Toggle fog of war               |
| `g` | Enter teleportation mode        |
| `r` | Random teleport target          |
| `m` | View visible monsters           |
| `L` | Look mode (inspect tiles)       |
| `Q` | Quit the game                   |

---

## Game Mechanics

### Dungeon Generation
- Algorithmic room placement with connecting corridors
- Randomized stair placement for level progression
- Balanced monster and item distribution

### Combat System
- Turn-based with priority queue for entity actions
- Line-of-sight calculations for ranged attacks
- Equipment affects combat statistics

### Fog of War
- Dynamic visibility system
- Explored areas remain visible but dimmed
- Toggle option for development/debugging

---

## File Structure

```
~/.rlg327/
├── dungeon          # Saved game state
├── monster_desc.txt # Custom monster definitions
└── object_desc.txt  # Custom object definitions
```

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

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/new-feature`)
3. Commit your changes (`git commit -am 'Add new feature'`)
4. Push to the branch (`git push origin feature/new-feature`)
5. Create a Pull Request

---

## Development

### Building for Debug
```bash
make debug    # Build with debug symbols
```

### Clean Build
```bash
make clean    # Remove compiled objects and executable
```

---

## License

This project is available under the MIT License. See the LICENSE file for more details.

---

## Acknowledgments

- Built using the **ncurses** library for terminal-based graphics
- Inspired by classic roguelike games like Rogue, NetHack, and Angband
- Developed as part of coursework/personal project at Iowa State University

---

**Created by [Hrishikesh Uchake](https://github.com/HrishikeshUchake)**

For questions, issues, or contributions, please open an issue on GitHub!



## License

MIT License — feel free to use, modify, or expand this project for personal or academic use.

---

## Author

Built by [Hrishikesh Uchake](https://github.com/HrishikeshUchake)




