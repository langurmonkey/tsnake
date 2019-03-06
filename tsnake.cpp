/* tsnake.cpp */

#include <iostream>
#include <string>
#include <deque>
#include <algorithm>
#include <iomanip> // setprecision
#include <sstream> // stringstream
#include <curses.h>
#include <stdlib.h>
#include <time.h>

#define EMPTY  	    ' '
#define SNAKE       '#'
#define FOOD        'x'
#define WALL        '/'

#define UP          0
#define DOWN        1
#define RIGHT       2
#define LEFT        3

#define START_LEN   4

struct point {
    int x, y;
};

void update(int x, int y);
void paint_snake();
void do_chdir(int newy, int newx, int newdir, int opposite_dir);
int out_of_boudns(int y, int x);
int is_move_hit(int y, int x);
void draw_map(void);
void print_bottom(char* text);
void create_food();
void print_status(const char* status);

int y, x;
int dir;
int running;
int score;
std::deque<point> snake;
point food;
int nfood;
clock_t curr, last;
WINDOW* gamew;
int gw_w, gw_h;

int main(void)
{
    float speed, secs;
    clock_t start;
    int ch;
    std::stringstream stream;

    /* init random */
    srand(time(NULL));

    /* initialize curses */
    initscr();
    if(has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    } else {
        start_color();
    }
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    curs_set(0);
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLUE);
    init_pair(3, COLOR_RED, COLOR_GREEN);
    init_pair(4, COLOR_WHITE, COLOR_BLACK);
    init_pair(5, COLOR_BLUE, COLOR_BLACK);
    clear();

    if(LINES < 10 || COLS < 35){
        endwin();
        std::cout << "Terminal too small! (min [35,10], current [" << COLS << "," << LINES << "]" << std::endl;
        exit(0);
    }

    /* create window */
    gw_w = COLS;
    gw_h = LINES - 1;
    gamew = newwin(gw_h, gw_w, 0, 0);

    /* initialize the map if any */
    draw_map();

    /* start player at [START_LEN,5] going right */
    x = START_LEN + 1;
    y = 2;
    dir = RIGHT;
    score = 0;
    start = clock();

    /* init snake */
    wbkgd(gamew, COLOR_PAIR(3));
    for(int i = 0; i < START_LEN; i++) {
        point p = {x + 1 - START_LEN + i, y};
        snake.push_front(p);
        mvwaddch(gamew, p.y, p.x, SNAKE);
    }

    /* init food */
    create_food();

    refresh();
    wrefresh(gamew);

    /* frame rate is 1 second (speed) */
    speed = 1.0;


    /* clocks */
    curr = clock();
    last = 0;

    /* run */
    running = 1;

    do {
        /* update current time */
        curr = clock();

        /* print status */
        secs = ((float)(curr - start) / CLOCKS_PER_SEC);
        stream << std::fixed << std::setprecision(2) << secs;
        std::string st = "Score: " + std::to_string(score) + "      Elapsed: " + std::to_string((int) secs) + " seconds         Speed: x" + std::to_string((int)(1.0F/speed));
        print_status(st.c_str());


        /* get char async, see nodelay() */
        ch = getch();

        /* test inputted key and determine direction */
        if(ch != ERR){
            switch (ch) {
                case KEY_UP:
                case 'w':
                case 'k':
                    if(dir != DOWN)
                        do_chdir(y - 1, x, UP, DOWN);
                    break;
                case KEY_DOWN:
                case 's':
                case 'j':
                    if(dir != UP)
                        do_chdir(y + 1, x, DOWN, UP);
                    break;
                case KEY_LEFT:
                case 'a':
                case 'h':
                    if(dir != RIGHT)
                        do_chdir(y, x - 1, LEFT, RIGHT);
                    break;
                case KEY_RIGHT:
                case 'd':
                case 'l':
                    if(dir != LEFT)
                        do_chdir(y, x + 1, RIGHT, LEFT);
                    break;
                case 'q':
                    // quit
                    running = 0;
                    curr = last = clock();
                    break;
                case '+':
                    speed = std::clamp(speed / 2.0F, 0.03125F, 1.0F);
                    curr = last = clock();
                    break;
                case '-':
                    speed = std::clamp(speed * 2.0F, 0.03125F, 1.0F);
                    curr = last = clock();
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
                update(x, y);
            }
        }
        
        /* chech food */
        if(food.x == x && food.y == y){
            create_food();
            nfood = 1;
            score++;
            if (score % 20 == 0)
                speed = std::clamp(speed / 2.0F, 0.03125F, 1.0F);
        }

        
        /* refresh */
        wbkgd(gamew, COLOR_PAIR(5));
        box(gamew, 0, 0);
        wrefresh(gamew);
        refresh();
    }
    while (running);

    /* done */
    std::string msg = "Quit (q) or restart (r)?";
    int minl = msg.size() + 2;
    int ew_w = std::clamp(COLS / 2, minl, COLS);
    int ew_h = std::clamp(LINES / 2, 4, LINES);
    WINDOW* endw = newwin(ew_h, ew_w, (LINES - ew_h) / 2, (COLS - ew_w) / 2);
    nodelay(stdscr, FALSE);
    mvwaddstr(endw, ew_h / 2, ew_w / 2 - minl / 2 + 1, msg.c_str());
    box(endw, 0, 0);
    wrefresh(endw);
    
    getch();

    endwin();

    exit(0);
}

void print_status(const char* status)
{
    attron(COLOR_PAIR(1));
    mvhline(LINES - 1, 0, EMPTY, COLS);
    move(LINES - 1, 0);
    printw(status);
    attroff(COLOR_PAIR(1));
    refresh();
}

point rd()
{
    point c;
    while(1){
        c.x = rand() % (gw_w - 2) + 1;
        c.y = rand() % (gw_h - 2) + 1;

        int hit = 0;
        std::deque<point>::iterator it = snake.begin();
        while (it != snake.end() && !hit){
            hit = it->x == c.x && it->y == c.y;
            *it++;
        }
        if(!hit)
            return c;
    }
}

void create_food()
{
    point newp = rd();
    food.x = newp.x;
    food.y = newp.y;
    wbkgd(gamew, COLOR_PAIR(2));
    mvwaddch(gamew, food.y, food.x, FOOD);
    
}

void update(int x, int y)
{
    wbkgd(gamew, COLOR_PAIR(3));
    point newpoint = {x, y};
    snake.push_front(newpoint);
    if(!nfood){
        point erase = snake.back();
        snake.pop_back();
        mvwaddch(gamew, erase.y, erase.x, EMPTY);
    }else{
        nfood = 0;
    }
    move(y, x);
    paint_snake();
}

void paint_snake()
{
    std::deque<point>::iterator it = snake.begin();
    while (it != snake.end()){
        mvwaddch(gamew, it->y, it->x, SNAKE);
        *it++;
    }
}

void do_chdir(int newy, int newx, int newdir, int opposite_dir)
{
    if (!is_move_hit(newy, newx)) {
        update(newx, newy);
        x = newx;
        y = newy;
        dir = newdir;
        curr = last = clock();
    } else if (dir != opposite_dir){
        running = 0;
    }
}

void print_bottom(const char* text)
{
    move(LINES - 1, 0);
    printw(text);
}

int out_of_bounds(int y, int x)
{
    return y <= 0 || x <= 0 || y >= gw_h - 1 || x >= gw_w - 1;
}

int is_move_hit(int y, int x)
{
    int selfhit = 0;
    std::deque<point>::iterator it = snake.begin();
    while (it != snake.end()){
        selfhit = it->x == x && it->y == y;
        if(selfhit)
            break;
        *it++;
    }
    return selfhit || out_of_bounds(y, x);
}

void draw_map(void)
{
    int y;
    for (y = 0; y < LINES; y++) {
        mvwhline(gamew, y, 0, EMPTY, COLS);
    }
}

