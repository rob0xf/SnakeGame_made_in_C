#include "Snake.h"

#include <ncurses.h>

#include <stdio.h>    //for FILE
#include <stdlib.h>   //for exit()
#include <stdbool.h>  //for true and false
#include <string.h>   //for strlen()
#include <time.h>     //for srand() and the struct of timespec


Snake snake;
Coordinates coordinates;
Food food;
StateGame stateGame;
User user;


void setup(WINDOW *win)
{ 
    nodelay(win, true);
    keypad(win, true);
    getmaxyx(win, stateGame.lines, stateGame.cols);
    box(win, 0,0);
        
    snake.length = 1;
    snake.tail = 1;
    initializeArray(&snake, 2, stateGame.cols * stateGame.lines);

    coordinates.y = 1;
    coordinates.x = 0;
    
    if (stateGame.difficulty == Hard) {
        coordinates.current_y = 1;
        coordinates.current_x = 1;
    } 
    else {
        coordinates.current_y = stateGame.lines / 2;
        coordinates.current_x = stateGame.cols / 2;
    }

    srand(time(NULL));
    food.status = false;
    food.y = 1 + rand() % (stateGame.lines - 2);
    food.x = 1 + rand() % (stateGame.cols - 2);

    stateGame.score = 0;
    stateGame.quit = false;
    stateGame.game_over = false;
}


void delay(const unsigned long miliseconds)
{
    struct timespec req = {0};
    req.tv_sec = miliseconds / 1000;
    req.tv_nsec = (miliseconds % 1000) * 1000000;
    nanosleep(&req, NULL);
}


void handleSnake(WINDOW *win)
{
    int ch = wgetch(win);

    if (ch == 'q' || ch == 'Q') 
        stateGame.quit = true;
    
    if (ch == KEY_UP || ch == 'w' || ch == 'W'){
        if (!(coordinates.y == 1 && coordinates.x == 0)) {
            coordinates.y = -1;
            coordinates.x = 0;
        }
    }

    if (ch == KEY_DOWN || ch == 's' || ch == 'S'){
        if (!(coordinates.y == -1 && coordinates.x == 0)) {
            coordinates.y = 1;
            coordinates.x = 0;
        }
    }             

    if (ch == KEY_RIGHT || ch == 'd' || ch == 'D'){
        if (!(coordinates.y == 0 && coordinates.x == -1)) {
            coordinates.y = 0;
            coordinates.x = 1;
        }
    }

    if (ch == KEY_LEFT || ch == 'a' || ch == 'A'){
        if (!(coordinates.y == 0 && coordinates.x == 1)) {
            coordinates.y = 0;
            coordinates.x = -1;
        }
    }
}


void eraseSnakeTraces(WINDOW *win)
{
    for (int i = 0; i < snake.tail; i++)
        mvwaddch(win, snake.position[i][1], snake.position[i][0], ' ');    
}


void calculateCoordinates(void)
{
    for (int i = snake.length - 1; i > 0; i--) {
        snake.position[i][1] = snake.position[i-1][1];
        snake.position[i][0] = snake.position[i-1][0];
    }
            
    coordinates.current_y += coordinates.y;
    coordinates.current_x += coordinates.x;
 
    snake.position[0][1] = coordinates.current_y;
    snake.position[0][0] = coordinates.current_x;
}


void printSnake(WINDOW *win)
{
    for (int i = 0; i < snake.length; i++)
        mvwaddch(win, snake.position[i][1], snake.position[i][0], '#');
}


void handleCollisionWithFood(void)
{
    for (int i = 0; i < snake.length; i++)
        if (snake.position[i][1] == food.y && snake.position[i][0] == food.x) {
            snake.length++;
            snake.tail++;
            stateGame.score++;
            food.status = false;
        }
}


void handleCollisionWithBody(void)
{
    for (int i = 1; i < snake.length; i++) 
        if (snake.position[0][1] == snake.position[i][1] && snake.position[0][0] == snake.position[i][0])
            stateGame.game_over = true;
}


void handleCollisionWithBorderModeEasy(void)
{
    if (snake.position[0][1] == 0) 
        coordinates.current_y = stateGame.lines - 1;

    if (snake.position[0][1] == stateGame.lines - 1)
        coordinates.current_y = 1;

    if (snake.position[0][0] == 0)
        coordinates.current_x = stateGame.cols - 1;

    if (snake.position[0][0] == stateGame.cols - 1)
        coordinates.current_x = 1;
}


bool thisSnakeAtBorder(int pos_x, int pos_y, int cols, int lines)
{
    return (pos_x == 0 || pos_x == cols - 1 || pos_y == 0 || pos_y == lines - 1);
}


void handleCollisionWhitBorderModeHard(void)
{
    int pos_x = snake.position[0][0];
    int pos_y = snake.position[0][1];
    
    if (thisSnakeAtBorder(pos_x, pos_y, stateGame.cols, stateGame.lines))
        stateGame.game_over = true;
}


void drawObstacle(WINDOW* win, int y, int x, int size_y, int size_x)
{
    for (int i = 0; i < size_y; i++)
        for (int j = 0; j < size_x; j++) 
            mvwaddch(win, y + i, x + j, '@');
}


bool thisSnakeOrFoodInObstacle(int start_x, int end_x, int start_y, int end_y, int pos_x, int pos_y)
{
    return (pos_x >= start_x && pos_x <= end_x - 1) && (pos_y >= start_y && pos_y <= end_y - 1);
}


void handleCollisionWithObstacle(WINDOW* win)
{
    const int porcentage_y = 24;
    const int porcentage_x = 36;

    const int size_y = stateGame.lines * porcentage_y / 100;
    const int size_x = stateGame.cols * porcentage_x / 100;
    const int start_y = (stateGame.lines - size_y) / 2;
    const int start_x = (stateGame.cols - size_x) / 2;

    drawObstacle(win, start_y, start_x, size_y, size_x);
    wrefresh(win);
    
    int pos_x = snake.position[0][0];
    int pos_y = snake.position[0][1];

    if (thisSnakeOrFoodInObstacle(start_x,
                                  start_x + size_x,
                                  start_y,
                                  start_y + size_y,
                                  pos_x,
                                  pos_y))
        stateGame.game_over = true;

    
    if (thisSnakeOrFoodInObstacle(start_x,
                                     start_x + size_x,
                                     start_y,
                                     start_y + size_x,
                                     food.x,
                                     food.y)) {
        if (food.status)
            food.status = false;
    }

            
}


void printFood(WINDOW *win)
{
    food.status = true;
    food.y = 1 + rand() % (stateGame.lines - 2);
    food.x = 1 + rand() % (stateGame.cols - 2);
    mvwaddch(win, food.y, food.x, '&');
}



void gameOver(WINDOW *win)
{
    const char * const message = "!!!GAME OVER¡¡¡";
     
    int y = stateGame.lines / 2;
    int x = (stateGame.cols / 2) - (strlen(message) / 2 - 1);
    mvwprintw(win, y, x, " %s ", message);
    wrefresh(win);

    delay(1400);

    werase(win);
    releaseArray(&snake, stateGame.lines * stateGame.cols);
}


void mainLoop(WINDOW *win)
{   
    while (!stateGame.game_over && !stateGame.quit)
    {
        handleSnake(win);
        eraseSnakeTraces(win);
        calculateCoordinates();

        printSnake(win);
        
        handleCollisionWithFood();

        handleCollisionWithBody();

        if (stateGame.difficulty == Easy) {
            handleCollisionWithBorderModeEasy();
            box(win, 0,0);
        }

        if (stateGame.difficulty == Normal || stateGame.difficulty == Hard)
            handleCollisionWhitBorderModeHard();        
        
        if (stateGame.difficulty == Hard)
            handleCollisionWithObstacle(win);
        
        if (!food.status) 
            printFood(win);

        delay(110);
    }
}


void snakeGame(void)
{
    WINDOW* board = createWindow(80, 80, 0);
    stateGame.quit = false;
    
    while (!stateGame.quit) {
        setup(board);
        mainLoop(board);
        gameOver(board);
    }
}


void drawBanner(void) 
{
    const int banner_length = 5;

    const char * const banner[] =
        {      
            ".%%%%...%%..%%...%%%%...%%..%%..%%%%%%",
            "%%......%%%.%%..%%..%%..%%.%%...%%....",
            ".%%%%...%%.%%%..%%%%%%..%%%%....%%%%..",
            "....%%..%%..%%..%%..%%..%%.%%...%%....",
            ".%%%%...%%..%%..%%..%%..%%..%%..%%%%%%",
        };
    
    //en y calculamos el area de lines = 0 hasta donde comienza el tablero del menu principal
    const int porcentage_borad = 52; 
    const int lines_board = LINES * porcentage_borad / 100;
    const int area = (LINES - lines_board) / 2;

    int y = area / 2 - (banner_length) / 2;

    //en x calclulamos las coordenadas para mostrar el banner en el centro
    int x = (COLS / 2) - strlen(banner[0]) / 2;
    

    for (int i = 0; i <  banner_length; i++) 
        mvwaddstr(stdscr, y + i, x, banner[i]);
    
    wrefresh(stdscr);
}

/*
void createUser(User* user)
{
        
}*/


void drawMenu(WINDOW *win, int current_option, const char * const options[], const int option_length)
{
    int cols, lines;
    getmaxyx(win, lines, cols);

    werase(win);
    box(win, 0,0);

    for (int position = 0; position < option_length; position++) {
        if (current_option == position)
            wattron(win, A_STANDOUT);
        
        int x = cols / 2 - strlen(options[position]) / 2;
        int y = lines / 2 - option_length / 2 + position;

        mvwprintw(win, y, x, " %s ", options[position]);

        if (current_option == position)
            wattroff(win, A_STANDOUT);
    }

    wrefresh(win);
}           


void handleMenuNavigation(WINDOW *win, int *option, bool *enter_key, const int option_length)
{
    nodelay(win, true);
    keypad(win, true);
    int ch = wgetch(win);

    switch (ch) 
    {
        case KEY_UP: 
            *option = *option == 0 ? option_length -1 : *option - 1;
            break;
        case KEY_DOWN: 
            *option = *option == option_length - 1 ? 0 : *option + 1;
            break;
        case '\n':
            *enter_key = true;
            break;
    }
}


void mainMenu(void)
{
    start_color();
    drawBanner();

    WINDOW* menu = createWindow(52, 46, -5);

    const int menu_length = 4;
    const char * const menu_content[] = {
        "Play           ",
        "User and Score ",
        "Difficulty     ",
        "Exit           ",
    };
    
    int option = 0;
    bool enter_key = false;

    while(!enter_key)
    {
        handleMenuNavigation(menu, &option, &enter_key, menu_length);
        drawMenu(menu, option, menu_content, menu_length);
    }
    
    switch (option)
    {
        case 0:
            wclear(stdscr);
            wrefresh(stdscr);
            delwin(menu);
            snakeGame();
            break;
        
        case 1: 
            wclear(menu);
            wrefresh(menu);
            delwin(menu);
            userMenu();
            break;

        case 2:
            werase(menu);
            wrefresh(menu);
            delwin(menu);
            gameDifficultyMenu();
            break; 
        
        case 3:
            delwin(menu);
            break;
    }
}


void gameDifficultyMenu(void)
{
    WINDOW *difficulty_menu = createWindow(40, 43, 0);
    
    const int menu_length = 3;
    const char * const menu_content[] = {
        "Easy      ",
        "Normal    ",
        "Hard      ",
    };
    
    int option = 0;
    bool enter = false;

    while(!enter)
    {
        handleMenuNavigation(difficulty_menu, &option, &enter, menu_length);
        drawMenu(difficulty_menu, option, menu_content, menu_length);
    }

    
    switch (option) {
        case 0: 
            stateGame.difficulty = Easy;
            break;
        case 1: 
            stateGame.difficulty = Normal;
            break;
        case 2: 
            stateGame.difficulty = Hard;
            break;
    }
    
    werase(difficulty_menu);
    wrefresh(difficulty_menu);
    delwin(difficulty_menu);

    mainMenu();
}


void userMenu(void)
{
    WINDOW *user_menu = createWindow(48, 48, 0);

    const int menu_length = 3;
    const char * const menu_content[] = {
        "User: ________ ",
        "Best Score: ",
        "Main menu   ",
    };

    int option = 0;
    bool enter = false;

    while (!enter)
    {
        handleMenuNavigation(user_menu, &option, &enter, menu_length);
        drawMenu(user_menu, option, menu_content, menu_length);
    }
    
    switch (option) 
    {
        
        case 3: 
            wclear(user_menu);
            wrefresh(user_menu);
            delwin(user_menu);
            mainMenu();
            break;
    }

}


WINDOW* createWindow(const int cols_percentage, const int lines_percentage, const int move_line_positon)
{
    int cols, lines;
    getmaxyx(stdscr, lines, cols);

    const int num_cols = cols * cols_percentage / 100 + 1;
    const int num_lines = lines * lines_percentage / 100 + 1;
    const int porcentage_lines_move = lines * move_line_positon / 100;
    const int begin_x = (cols / 2) - (num_cols  / 2 );
    const int begin_y = (lines / 2) - (num_lines / 2 ) - porcentage_lines_move;

    WINDOW* win = newwin(num_lines, num_cols, begin_y, begin_x);

    return win;
}


void initializeArray(Snake* snake, const int cols_size, const int rows_size)
{   
    snake->position = (int **) malloc( rows_size * sizeof(int *));
    if (snake->position == NULL) 
        exit(EXIT_FAILURE);
    
    for (int col = 0; col < rows_size; col++) {
        snake->position[col] = (int *) malloc(cols_size * sizeof(int));

        if (snake->position[col] == NULL) {
            for (int i = 0; i < col; i++)
                free(snake->position[i]);
            
            free(snake->position);
            exit(EXIT_FAILURE);
        }
    }
}


void releaseArray(Snake* snake, const int rows_size)
{
    for (int i = 0; i < rows_size; i++)
        free(snake->position[i]);

    free(snake->position);
    snake->position = NULL;
}

