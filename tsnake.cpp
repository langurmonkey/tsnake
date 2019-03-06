/* snake.cpp */

#include <iostream>
#include <string>
#include <deque>
#include <curses.h>
#include <stdlib.h>
#include <time.h>

#define EMPTY  	    ' '
#define SNAKE       '*'
#define FOOD        '.'
#define WALL        '#'

#define UP          0
#define DOWN        1
#define RIGHT       2
#define LEFT        3

#define START_LEN   4

struct point {
    int x, y;
};

void update(int x, int y, std::deque<point>* snake);
void paint_snake(std::deque<point>* snake);
void do_chdir(int* y, int* x, int* dir, int* running, clock_t* curr, clock_t* last, std::deque<point>* snake, int newy, int newx, int newdir, int opposite_dir);
int out_of_boudns(int y, int x);
int is_move_hit(int y, int x);
int is_move_food(int y, int x);
void draw_map(void);
void print_bottom(char* text);


int main(void)
{
    float speed;
    int y, x;
    int dir;
    int running;
    std::deque<point> snake;

    clock_t curr, last;

    int ch;

    /* initialize curses */
    initscr();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    curs_set(0);

    clear();

    /* initialize the map if any */
    draw_map();

    /* start player at [START_LEN,5] going right */
    x = START_LEN;
    y = 5;
    dir = RIGHT;

    /* init snake */
    for(int i = 0; i < START_LEN; i++) {
        point p = {x + 1 - START_LEN + i, y};
        snake.push_front(p);
        mvaddch(p.y, p.x, SNAKE);
    }
    refresh();

    /* frame rate is 1 second (speed) */
    speed = 1.0;

    /* print cols and lines */
    move(LINES - 1, 0);
    printw("C: %d  L: %d", COLS, LINES);

    /* clocks */
    curr = clock();
    last = curr;

    /* run */
    running = 1;

    do {
        ch = getch();

        /* test inputted key and determine direction */
        if(ch != ERR){
            switch (ch) {
                case KEY_UP:
                case 'w':
                case 'k':
                    do_chdir(&y, &x, &dir, &running, &curr, &last, &snake, y - 1, x, UP, DOWN);
                    break;
                case KEY_DOWN:
                case 's':
                case 'j':
                    do_chdir(&y, &x, &dir, &running, &curr, &last, &snake, y + 1, x, DOWN, UP);
                    break;
                case KEY_LEFT:
                case 'a':
                case 'h':
                    do_chdir(&y, &x, &dir, &running, &curr, &last, &snake, y, x - 1, LEFT, RIGHT);
                    break;
                case KEY_RIGHT:
                case 'd':
                case 'l':
                    do_chdir(&y, &x, &dir, &running, &curr, &last, &snake, y, x + 1, RIGHT, LEFT);
                    break;
                case 'q':
                    // quit
                    running = 0;
                    curr = last = clock();
                    break;
                case '+':
                    speed /= 2.0F;
                    curr = last = clock();
                    break;
                case '-':
                    speed *= 2.0F;
                    break;
            }
        }
        
        float dt = ((float)(curr - last) / CLOCKS_PER_SEC);
        if (dt > speed) {
            last = curr;
            /* auto-move */
            switch (dir) {
                case UP:
                    y--;
                    break;
                case DOWN:
                    y++;
                    break;
                case RIGHT:
                    x++;
                    break;
                case LEFT:
                    x--;
                    break;
            }
            if(is_move_hit(y, x)) {
                /* end */
                running = 0;
                break;
            } else {
                /* update */
                update(x, y, &snake);
            }
        }

        /* update current time */
        curr = clock();
        
        /* refresh */
        refresh();
    }
    while (running);

    /* done */
    nodelay(stdscr, FALSE);
    std::string msg = "Press any key to quit";
    mvaddstr(LINES / 2, COLS / 2 - msg.size() / 2, msg.c_str());
    refresh();
    getch();

    endwin();

    exit(0);
}

void update(int x, int y, std::deque<point>* snake)
{
    point newpoint = {x, y};
    snake->push_front(newpoint);
    point erase = snake->back();
    snake->pop_back();

    mvaddch(erase.y, erase.x, EMPTY);
    move(y, x);
    paint_snake(snake);
}

void paint_snake(std::deque<point>* snake)
{
    std::deque<point>::iterator it = snake->begin();
    while (it != snake->end()){
        mvaddch(it->y, it->x, SNAKE);
        *it++;
    }
}

void do_chdir(int* y, int* x, int* dir, int* running, clock_t* curr, clock_t* last, std::deque<point>* snake, int newy, int newx, int newdir, int opposite_dir)
{
    if (!is_move_hit(newy, newx)) {
        update(newx, newy, snake);
        *x = newx;
        *y = newy;
        *dir = newdir;
        *curr = *last = clock();
    } else if (*dir != opposite_dir){
        *running = 0;
    }
}

void print_bottom(const char* text)
{
    move(LINES - 1, 0);
    printw(text);
}

int out_of_bounds(int y, int x)
{
    return y < 0 || x < 0 || y > LINES-1 || x > COLS-1;
}

int is_move_hit(int y, int x)
{
    int testch = mvinch(y, x);
    return testch == SNAKE || out_of_bounds(y, x);
}

int is_move_food(int y, int x)
{
    int testch = mvinch(y, x);
    return testch == FOOD;
}

void draw_map(void)
{
    int y;
    for (y = 0; y < LINES; y++) {
        mvhline(y, 0, EMPTY, COLS);
    }
}

