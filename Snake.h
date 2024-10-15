#ifndef SNAKE_H
#define SNAKE_H

#include <ncurses.h>

 
typedef struct {
    int length;
    int tail;
    int **position;
} Snake;

typedef struct {
    int y;
    int x;
    int current_y;
    int current_x;
} Coordinates;

typedef struct {
    int status;
    int y;
    int x;
} Food;

enum Difficulty {
    Easy,
    Normal,
    Hard
};

typedef struct {
    int cols;
    int lines;

    int score;
    int quit;
    int game_over;

    enum Difficulty difficulty;
} StateGame;

typedef struct {
    char user_name[15];
    int best_score;
} User;


void setup(WINDOW *);
void delay(const unsigned long);
void handleSnake(WINDOW *);
void eraseSnakeTraces(WINDOW *);
void calculateCoordinates(void);
void printSnake(WINDOW *);
void handleCollisionWithFood(void);
void handleColiisionWithBody(void);
void handleCollisionWithBorderModeEasy(void);
bool thisSnakeAtBorder(int, int, int, int);
void handleCollisionWithBorderModeHard(void);
void drawObstacle(WINDOW *, int, int, int, int);
bool thisSnakeOrFoodInObstacle(int, int, int, int, int, int);
void handleCollisionWithObstacle(WINDOW *);
void printFood(WINDOW *);
void gameOver(WINDOW *);

void mainLoop(WINDOW *);
void snakGame(void);

void drawBanner(void);

void createUser(User* user);
//void loadUser(User* user);

void drawMenu(WINDOW *, int, const char * const *, const int);
void handleMenuNavigation(WINDOW *, int *, bool *, const int);
void mainMenu(void);
void gameDifficultyMenu(void);
void userMenu(void);

WINDOW* createWindow(const int, const int, int);

void initializeArray(Snake* snake, const int, const int);
void releaseArray(Snake* snake, const int);

#endif

