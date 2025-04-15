# Snake Game in C

A simple terminal-based Snake game implemented in C using the ncurses library.

## Features

- Classic snake gameplay in your terminal
- Tunneling/wrap-around behavior when hitting walls
- Colorful graphics (if your terminal supports colors)
- Pause functionality
- Score tracking
- Clear, well-commented code for learning purposes
- Simple controls

## Prerequisites

To compile and run this game, you need:
- GCC or another C compiler
- ncurses library

On Debian-based systems (like Ubuntu/Kali Linux), you can install ncurses with:
```
sudo apt install libncurses5-dev libncursesw5-dev
```

On Red Hat-based systems:
```
sudo dnf install ncurses-devel
```

## How to Compile and Run

### Using the Makefile (recommended)

1. Compile the game:
   ```
   make
   ```
   
2. Run the game:
   ```
   make run
   ```
   
3. To clean up object files and executable:
   ```
   make clean
   ```

### Manual Compilation

If you don't want to use the Makefile, you can compile manually:
```
gcc -o snake snake.c -lncurses -Wall -Wextra -std=c99
```

Then run with:
```
./snake
```

## How to Play

- Use the WASD keys or arrow keys to control the snake:
  - W or ↑: Move Up
  - S or ↓: Move Down
  - A or ←: Move Left
  - D or →: Move Right
- Eat the food (*) to grow longer and increase your score
- When you hit a wall, the snake tunnels through to the opposite side
- The game ends if you hit your own body
- Press P to pause/resume the game
- Press Q to quit the game at any time

## Code Structure

The game code is heavily commented to explain how everything works:

- Game initialization and setup
- Drawing the game board
- Snake movement mechanics
- Collision detection
- Food placement algorithm
- User input handling

This makes it an excellent learning resource for beginning C programmers interested in game development or terminal-based applications.

## License

This code is provided as-is under the MIT License. Feel free to modify and distribute it as you wish. 