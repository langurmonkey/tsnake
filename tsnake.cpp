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

/* defines */
#define EMPTY  	    ' '
#define SNAKE       '#'
#define FOOD        'O'
#define WALL        '/'

#define UP          0
#define DOWN        1
#define RIGHT       2
#define LEFT        3

#define START_LEN   4

#define ALIGN_RIGHT 0
#define ALIGN_LEFT  1

/* structs */
struct point {
    int x, y;
};

struct game_state {
    // current position of the head of the snake
    point pos;
    // current direction
    int dir;
    // current score
    int score;
    // snake speed in seconds/cell
    float speed;
    // are we running?
    bool running;
    // snake queue
    std::deque<point> snake;
    // current food position
    point food;
    // flag that goes up when we eat
    bool f_eat;
    // timers
    clock_t curr, last;
    // game window
    WINDOW* gamew;
    // game window size
    int gw_w, gw_h;
};

/* function definitions */
void update(game_state* state, int newy, int newx);
void do_chdir(game_state* state, int newy, int newx, int newdir, int opposite_dir);
int out_of_boudns(game_state* state, int y, int x);
int is_move_hit(game_state* state, int y, int x);
void draw_map(game_state* state);
void print_bottom(char* text);
void create_food(game_state* state);
void print_status(std::string status, int align, int col);
int ask_end();
bool speed_scl(game_state* state, float scale);
bool speed_up(game_state* state);
bool speed_down(game_state* state);
int start_game();

#define C_DEFAULT 1
#define C_FOOD    2
#define C_SNAKE   3
#define C_STATUS  4
#define C_BORDER  5
#define C_GREEN   6

int main(void)
{
    /* init random */
    srand(time(NULL));

    /* initialize curses */
    initscr();
    start_color();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    curs_set(0);
    init_pair(C_STATUS, COLOR_YELLOW, COLOR_BLACK);
    init_pair(C_FOOD, COLOR_BLACK, COLOR_BLUE);
    init_pair(C_SNAKE, COLOR_RED, COLOR_GREEN);
    init_pair(C_DEFAULT, COLOR_WHITE, COLOR_BLACK);
    init_pair(C_BORDER, COLOR_BLUE, COLOR_BLACK);
    init_pair(C_GREEN, COLOR_GREEN, COLOR_BLACK);
    
    clear();
    
    /* start the game */
    while(start_game()){}

    /* clean up */
    endwin();
    exit(0);
}

int start_game()
{
    /* new game state */
    game_state state;
    
    /* auxiliary variables */
    float secs;
    clock_t start;
    int ch;
    std::stringstream stream;

    if(LINES < 10 || COLS < 35){
        endwin();
        std::cout << "Terminal too small! (min [35,10], current [" << COLS << "," << LINES << "]" << std::endl;
        exit(0);
    }

    /* create window */
    state.gw_w = COLS;
    state.gw_h = LINES - 1;
    state.gamew = newwin(state.gw_h, state.gw_w, 0, 0);

    /* initialize the map if any */
    draw_map(&state);

    /* start player at [START_LEN,5] going right */
    state.pos.x = START_LEN + 1;
    state.pos.y = 2;
    state.dir = RIGHT;
    state.score = 0;
    start = clock();

    /* init snake */
    wattron(state.gamew, COLOR_PAIR(C_SNAKE));
    state.snake.clear();
    for(int i = 0; i < START_LEN; i++) {
        point p = {state.pos.x + 1 - START_LEN + i, state.pos.y};
        state.snake.push_front(p);
        mvwaddch(state.gamew, p.y, p.x, SNAKE);
    }
    wattroff(state.gamew, COLOR_PAIR(C_SNAKE));

    /* init food */
    create_food(&state);

    refresh();
    wrefresh(state.gamew);

    /* frame rate is 1 second (speed) */
    state.speed = 1.0;

    /* clocks */
    state.last = 0;

    /* async char read */
    nodelay(stdscr, TRUE);
    
    /* run */
    state.running = true;

    do {
        /* update current time */
        state.curr = clock();

        /* print status */
        mvhline(LINES - 1, 0, EMPTY, COLS);
        
        secs = ((float)(state.curr - start) / CLOCKS_PER_SEC);
        stream << std::fixed << std::setprecision(2) << secs;
        std::string st = "  Score: " + std::to_string(state.score) + "      Elapsed: " + std::to_string((int) secs) + " seconds      Speed: x" + std::to_string((int)(1.0F / state.speed));

        print_status(st, ALIGN_LEFT, C_STATUS);
        print_status("(q) end game", ALIGN_RIGHT, C_DEFAULT);

        /* get char async, see nodelay() */
        ch = getch();

        /* test inputted key and determine direction */
        if(ch != ERR){
            switch (ch) {
                case KEY_UP:
                case 'w':
                case 'k':
                    if(state.dir != DOWN)
                        do_chdir(&state, state.pos.y - 1, state.pos.x, UP, DOWN);
                    break;
                case KEY_DOWN:
                case 's':
                case 'j':
                    if(state.dir != UP)
                        do_chdir(&state, state.pos.y + 1, state.pos.x, DOWN, UP);
                    break;
                case KEY_LEFT:
                case 'a':
                case 'h':
                    if(state.dir != RIGHT)
                        do_chdir(&state, state.pos.y, state.pos.x - 1, LEFT, RIGHT);
                    break;
                case KEY_RIGHT:
                case 'd':
                case 'l':
                    if(state.dir != LEFT)
                        do_chdir(&state, state.pos.y, state.pos.x + 1, RIGHT, LEFT);
                    break;
                case 'q':
                    // quit
                    state.running = false;
                    state.curr = state.last = clock();
                    break;
                case '+':
                    if(speed_up(&state))
                        state.curr = state.last = clock();
                    break;
                case '-':
                    if(speed_down(&state))
                        state.curr = state.last = clock();
                    break;
            }
        }
        
        float dt = ((float)(state.curr - state.last) / CLOCKS_PER_SEC);
        if (dt > state.speed) {
            state.last = state.curr;
            /* auto-move */
            switch (state.dir) {
                case UP:
                    state.pos.y--;
                    break;
                case DOWN:
                    state.pos.y++;
                    break;
                case RIGHT:
                    state.pos.x++;
                    break;
                case LEFT:
                    state.pos.x--;
                    break;
            }
            if(is_move_hit(&state, state.pos.y, state.pos.x)) {
                /* end */
                state.running = false;
                break;
            } else {
                /* update */
                update(&state, state.pos.y, state.pos.x);
            }
        }
        
        /* chech food */
        if(state.food.x == state.pos.x && state.food.y == state.pos.y){
            create_food(&state);
            state.f_eat = true;
            state.score++;
            if (state.score % 20 == 0)
                speed_up(&state);
        }

        
        /* refresh */
        wattron(state.gamew, COLOR_PAIR(C_BORDER));
        box(state.gamew, 0, 0);
        wattroff(state.gamew, COLOR_PAIR(C_BORDER));


        refresh();
        wrefresh(state.gamew);
    }
    while (state.running);

    /* done */
    std::string msg3 = "YOUR SCORE: " + std::to_string(state.score);
    std::string msg0 = "The game has finished, quit?";
    std::string msg1 = "(y|q)      Quit";
    std::string msg2 = "(n|r)      Restart game";
    int minl = msg0.size() + 2;
    int ew_w = std::clamp(COLS / 2, minl, COLS);
    int ew_h = std::clamp(LINES / 2, 4, LINES);
    WINDOW* endw = newwin(ew_h, ew_w, (LINES - ew_h) / 2, (COLS - ew_w) / 2);
    nodelay(stdscr, FALSE);

    /* score in green */
    wattron(endw, COLOR_PAIR(C_GREEN));
    mvwaddstr(endw, ew_h / 2 - 2, ew_w / 2 - msg3.size() / 2, msg3.c_str());
    wattroff(endw, COLOR_PAIR(C_GREEN));

    mvwaddstr(endw, ew_h / 2, ew_w / 2 - minl / 2 + 1, msg0.c_str());
    mvwaddstr(endw, ew_h / 2 + 2, ew_w / 2 - minl / 2 + 5, msg1.c_str());
    mvwaddstr(endw, ew_h / 2 + 3, ew_w / 2 - minl / 2 + 5, msg2.c_str());
    box(endw, 0, 0);
    
    wrefresh(endw);
   
    return ask_end();
}

int ask_end()
{
    int opt = getch();

    switch(opt){
        case 'y':
        case 'q':
            return 0;
        case 'n':
        case 'r':
            // Restart
            return 1;
        default:
            return ask_end();
    }
}

void print_status(std::string status, int align, int col)
{
    attron(COLOR_PAIR(col));
    switch(align){
        case ALIGN_LEFT:
            move(LINES - 1, 0);
            printw(status.c_str());
            break;
        case ALIGN_RIGHT:
            move(LINES - 1, COLS - status.size() - 1);
            printw(status.c_str());
            break;
    }
    attroff(COLOR_PAIR(col));
}

point rd(game_state* state)
{
    point c;
    while(1){
        c.x = rand() % (state->gw_w - 2) + 1;
        c.y = rand() % (state->gw_h - 2) + 1;

        if((mvwinch(state->gamew, c.y, c.x) & A_CHARTEXT) == EMPTY)
            return c;
    }
}

void create_food(game_state* state)
{
    point newp = rd(state);
    state->food.x = newp.x;
    state->food.y = newp.y;
    wattron(state->gamew, COLOR_PAIR(C_FOOD));
    mvwaddch(state->gamew, state->food.y, state->food.x, FOOD);
    wattroff(state->gamew, COLOR_PAIR(C_FOOD));
}

void update(game_state* state, int newy, int newx)
{
    point newpoint = {newx, newy};
    state->snake.push_front(newpoint);

    wattron(state->gamew, COLOR_PAIR(C_SNAKE));
    mvwaddch(state->gamew, newy, newx, SNAKE);
    wattroff(state->gamew, COLOR_PAIR(C_SNAKE));
    if(!state->f_eat){
        point erase = state->snake.back();
        state->snake.pop_back();
        mvwaddch(state->gamew, erase.y, erase.x, EMPTY);
    }else{
        state->f_eat = false;
    }
    move(state->pos.y, state->pos.x);
}

void do_chdir(game_state* state, int newy, int newx, int newdir, int opposite_dir)
{
    if (!is_move_hit(state, newy, newx)) {
        update(state, newy, newx);
        state->pos.x = newx;
        state->pos.y = newy;
        state->dir = newdir;
        state->curr = state->last = clock();
    } else if (state->dir != opposite_dir){
        state->running = false;
    }
}

bool speed_up(game_state* state)
{
    return speed_scl(state, 0.5);
}

bool speed_down(game_state* state)
{
    return speed_scl(state, 2.0);
}

bool speed_scl(game_state* state, float scale)
{
    float cpy = state->speed;
    state->speed = std::clamp(state->speed * scale, 0.03125F, 1.0F);
    return cpy != state->speed;
}

void print_bottom(const char* text)
{
    move(LINES - 1, 0);
    printw(text);
}

int out_of_bounds(game_state* state, int y, int x)
{
    return y <= 0 || x <= 0 || y >= state->gw_h - 1 || x >= state->gw_w - 1;
}

int is_move_hit(game_state* state, int y, int x)
{
    int testch = mvwinch(state->gamew, y, x) & A_CHARTEXT;
    return testch == SNAKE || out_of_bounds(state, y, x);
}

void draw_map(game_state* state)
{
    int y;
    for (y = 0; y < LINES; y++) {
        mvwhline(state->gamew, y, 0, EMPTY, COLS);
    }
}

