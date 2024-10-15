#include "Snake.h"

#include <stdlib.h>
#include <locale.h>

int main (void)
{
    setlocale(LC_ALL, "");
    setlocale(LC_CTYPE, "C-UTF-8");
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    mainMenu();

    curs_set(1);
    endwin();

    return EXIT_SUCCESS;
}
