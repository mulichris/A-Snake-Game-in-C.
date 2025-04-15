/**
 * Simple Snake Game in C
 * 
 * This program implements a classic snake game with console/terminal graphics.
 * The player controls a snake that moves around the screen, eating food to grow longer.
 * The game features "tunneling" behavior - when the snake hits a wall, it wraps around
 * to the opposite side of the screen. The game ends if the snake hits itself.
 * 
 * Controls:
 *   W/↑: Move Up
 *   S/↓: Move Down
 *   A/←: Move Left
 *   D/→: Move Right
 *   P: Pause Game
 *   Q: Quit Game
 * 
 * Compile with: gcc -o snake snake.c -lcurses
 * Or use the provided Makefile: make
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <curses.h>

/* Game Constants */
#define WIDTH 30      // Width of the game board
#define HEIGHT 20     // Height of the game board
#define INITIAL_SIZE 3  // Initial size of the snake

/* Direction Constants */
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

/* Color Pair IDs */
#define COLOR_PAIR_BORDER 1  // Border color pair
#define COLOR_PAIR_SNAKE  2  // Snake color pair
#define COLOR_PAIR_HEAD   3  // Snake head color pair
#define COLOR_PAIR_FOOD   4  // Food color pair
#define COLOR_PAIR_TEXT   5  // Text color pair

/* Game Characters */
#define SNAKE_BODY 'o'
#define SNAKE_HEAD '@'
#define FOOD '*'
#define EMPTY ' '
#define BORDER '#'

/* Structure to represent a point on the game board */
typedef struct {
    int x;
    int y;
} Point;

/* Structure to represent the snake */
typedef struct {
    Point body[WIDTH * HEIGHT]; // Maximum possible size
    int size;                   // Current size
    int direction;              // Current direction
} Snake;

/* Function prototypes */
void initializeGame(Snake *snake, Point *food);
void drawGame(Snake snake, Point food, bool paused);
void moveSnake(Snake *snake);
bool checkCollision(Snake snake);
void placeFood(Snake snake, Point *food);
bool eatFood(Snake *snake, Point *food);
void handleInput(Snake *snake, bool *gameOver, bool *gamePaused);
void endGame(int score);

/* Main function - entry point of the program */
int main() {
    /* Seed the random number generator */
    srand(time(NULL));
    
    /* Game variables */
    Snake snake;
    Point food;
    bool gameOver = false;
    bool gamePaused = false;
    
    /* Initialize ncurses library for terminal control */
    initscr();            // Initialize screen
    cbreak();             // Disable line buffering
    noecho();             // Don't echo input characters
    keypad(stdscr, TRUE); // Enable function keys (like arrow keys)
    curs_set(0);          // Hide cursor
    timeout(100);         // Set non-blocking input with 100ms timeout
    
    /* Initialize color if terminal supports it */
    if (has_colors()) {
        start_color();
        /* Define color pairs */
        init_pair(COLOR_PAIR_BORDER, COLOR_BLUE, COLOR_BLACK);
        init_pair(COLOR_PAIR_SNAKE, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOR_PAIR_HEAD, COLOR_CYAN, COLOR_BLACK);
        init_pair(COLOR_PAIR_FOOD, COLOR_RED, COLOR_BLACK);
        init_pair(COLOR_PAIR_TEXT, COLOR_WHITE, COLOR_BLACK);
    }
    
    /* Initialize the game state */
    initializeGame(&snake, &food);
    
    /* Main game loop */
    while (!gameOver) {
        /* Draw the current game state */
        drawGame(snake, food, gamePaused);
        
        /* Handle user input */
        handleInput(&snake, &gameOver, &gamePaused);
        
        /* Skip updates if game is paused */
        if (gamePaused) {
            napms(100); // Sleep to reduce CPU usage while paused
            continue;
        }
        
        /* Move the snake if the game is still active */
        if (!gameOver) {
            moveSnake(&snake);
            
            /* Check if the snake ate food */
            if (eatFood(&snake, &food)) {
                /* If food was eaten, place new food */
                placeFood(snake, &food);
            }
            
            /* Check for collisions with self */
            if (checkCollision(snake)) {
                gameOver = true;
            }
        }
    }
    
    /* End game and clean up */
    endGame(snake.size - INITIAL_SIZE);
    
    return 0;
}

/* Initialize the game state */
void initializeGame(Snake *snake, Point *food) {
    /* Set initial snake position in the middle of the board */
    int startX = WIDTH / 2;
    int startY = HEIGHT / 2;
    
    /* Set initial snake properties */
    snake->size = INITIAL_SIZE;
    snake->direction = RIGHT;
    
    /* Create initial snake body segments */
    for (int i = 0; i < snake->size; i++) {
        snake->body[i].x = startX - i;
        snake->body[i].y = startY;
    }
    
    /* Place the first food item */
    placeFood(*snake, food);
}

/* Draw the current game state on the screen */
void drawGame(Snake snake, Point food, bool paused) {
    /* Clear the screen before redrawing */
    clear();
    
    /* Create a 2D representation of the game board */
    char board[HEIGHT][WIDTH];
    
    /* Initialize board with empty spaces */
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (y == 0 || y == HEIGHT - 1 || x == 0 || x == WIDTH - 1) {
                board[y][x] = BORDER; // Draw borders
            } else {
                board[y][x] = EMPTY;  // Empty space
            }
        }
    }
    
    /* Place the snake on the board */
    for (int i = 0; i < snake.size; i++) {
        int x = snake.body[i].x;
        int y = snake.body[i].y;
        
        /* Ensure positions are within bounds */
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            if (i == 0) {
                board[y][x] = SNAKE_HEAD;  // Snake head
            } else {
                board[y][x] = SNAKE_BODY;  // Snake body
            }
        }
    }
    
    /* Place the food on the board */
    board[food.y][food.x] = FOOD;
    
    /* Draw the board on the screen with colors */
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            /* Apply appropriate color based on cell content */
            if (has_colors()) {
                switch (board[y][x]) {
                    case BORDER:
                        attron(COLOR_PAIR(COLOR_PAIR_BORDER));
                        break;
                    case SNAKE_HEAD:
                        attron(COLOR_PAIR(COLOR_PAIR_HEAD));
                        break;
                    case SNAKE_BODY:
                        attron(COLOR_PAIR(COLOR_PAIR_SNAKE));
                        break;
                    case FOOD:
                        attron(COLOR_PAIR(COLOR_PAIR_FOOD));
                        break;
                    default:
                        attron(COLOR_PAIR(COLOR_PAIR_TEXT));
                        break;
                }
            }
            
            /* Draw the character */
            mvaddch(y, x, board[y][x]);
            
            /* Reset attributes */
            if (has_colors()) {
                attroff(COLOR_PAIR(COLOR_PAIR_BORDER));
                attroff(COLOR_PAIR(COLOR_PAIR_HEAD));
                attroff(COLOR_PAIR(COLOR_PAIR_SNAKE));
                attroff(COLOR_PAIR(COLOR_PAIR_FOOD));
                attroff(COLOR_PAIR(COLOR_PAIR_TEXT));
            }
        }
    }
    
    /* Apply text color */
    if (has_colors()) {
        attron(COLOR_PAIR(COLOR_PAIR_TEXT));
    }
    
    /* Display score information */
    mvprintw(HEIGHT + 1, 0, "Score: %d   |   P: Pause   |   Q: Quit", snake.size - INITIAL_SIZE);
    
    /* Display pause message if game is paused */
    if (paused) {
        mvprintw(HEIGHT / 2, WIDTH / 2 - 5, "GAME PAUSED");
        mvprintw(HEIGHT / 2 + 1, WIDTH / 2 - 9, "Press P to continue");
    }
    
    /* Reset text color */
    if (has_colors()) {
        attroff(COLOR_PAIR(COLOR_PAIR_TEXT));
    }
    
    /* Refresh the screen to show the new state */
    refresh();
}

/* Move the snake one step in its current direction */
void moveSnake(Snake *snake) {
    /* Get the current head position */
    int headX = snake->body[0].x;
    int headY = snake->body[0].y;
    
    /* Calculate new head position based on direction */
    switch (snake->direction) {
        case UP:
            headY--;
            break;
        case RIGHT:
            headX++;
            break;
        case DOWN:
            headY++;
            break;
        case LEFT:
            headX--;
            break;
    }
    
    /* Implement tunneling/wrap-around behavior when snake hits walls */
    /* If snake goes off the left edge, appear on the right edge */
    if (headX <= 0) {
        headX = WIDTH - 2; // -2 to account for the border
    }
    /* If snake goes off the right edge, appear on the left edge */
    else if (headX >= WIDTH - 1) {
        headX = 1; // 1 to account for the border
    }
    /* If snake goes off the top edge, appear on the bottom edge */
    if (headY <= 0) {
        headY = HEIGHT - 2; // -2 to account for the border
    }
    /* If snake goes off the bottom edge, appear on the top edge */
    else if (headY >= HEIGHT - 1) {
        headY = 1; // 1 to account for the border
    }
    
    /* Shift all body segments forward */
    for (int i = snake->size - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }
    
    /* Update head position */
    snake->body[0].x = headX;
    snake->body[0].y = headY;
}

/* Check if the snake has collided with itself */
bool checkCollision(Snake snake) {
    /* Get head position */
    int x = snake.body[0].x;
    int y = snake.body[0].y;
    
    /* No longer check for wall collisions since snake now tunnels through walls */
    /* Wall collision code removed:
    if (x <= 0 || x >= WIDTH - 1 || y <= 0 || y >= HEIGHT - 1) {
        return true;
    }
    */
    
    /* Only check for collision with own body */
    for (int i = 1; i < snake.size; i++) {
        if (x == snake.body[i].x && y == snake.body[i].y) {
            return true;
        }
    }
    
    return false;
}

/* Place food at a random empty position on the game board */
void placeFood(Snake snake, Point *food) {
    /* Create arrays to track all empty positions */
    int emptyX[WIDTH * HEIGHT];
    int emptyY[WIDTH * HEIGHT];
    int emptyCount = 0;
    
    /* Find all empty cells on the board */
    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            bool isEmpty = true;
            
            /* Check if this cell contains a snake segment */
            for (int i = 0; i < snake.size; i++) {
                if (snake.body[i].x == x && snake.body[i].y == y) {
                    isEmpty = false;
                    break;
                }
            }
            
            if (isEmpty) {
                emptyX[emptyCount] = x;
                emptyY[emptyCount] = y;
                emptyCount++;
            }
        }
    }
    
    /* If there are empty cells, randomly choose one for the food */
    if (emptyCount > 0) {
        int randomIndex = rand() % emptyCount;
        food->x = emptyX[randomIndex];
        food->y = emptyY[randomIndex];
    }
}

/* Check if the snake has eaten the food */
bool eatFood(Snake *snake, Point *food) {
    /* Get head position */
    int headX = snake->body[0].x;
    int headY = snake->body[0].y;
    
    /* Check if head is at the food position */
    if (headX == food->x && headY == food->y) {
        /* Increase snake size by duplicating the last segment */
        snake->body[snake->size] = snake->body[snake->size - 1];
        snake->size++;
        return true;
    }
    
    return false;
}

/* Handle user keyboard input */
void handleInput(Snake *snake, bool *gameOver, bool *gamePaused) {
    int key = getch();
    
    if (key != ERR) {  /* ERR is returned if no key is pressed */
        switch (key) {
            case 'p': 
            case 'P':
                /* Toggle pause state */
                *gamePaused = !(*gamePaused);
                break;
            
            /* Only process movement keys when game is not paused */
            case 'w': 
            case KEY_UP:
                if (!(*gamePaused) && snake->direction != DOWN)
                    snake->direction = UP;
                break;
            case 'd': 
            case KEY_RIGHT:
                if (!(*gamePaused) && snake->direction != LEFT)
                    snake->direction = RIGHT;
                break;
            case 's': 
            case KEY_DOWN:
                if (!(*gamePaused) && snake->direction != UP)
                    snake->direction = DOWN;
                break;
            case 'a': 
            case KEY_LEFT:
                if (!(*gamePaused) && snake->direction != RIGHT)
                    snake->direction = LEFT;
                break;
            case 'q': 
            case 'Q':
                *gameOver = true;
                break;
        }
    }
}

/* End the game and display the final score */
void endGame(int score) {
    /* Turn off ncurses attributes */
    curs_set(1);
    endwin();
    
    /* Print final score */
    printf("\nGame Over!\n");
    printf("Your final score: %d\n", score);
    printf("Thanks for playing!\n");
} 