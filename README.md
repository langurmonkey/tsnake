# tsnake, a terminal snake game

`tsnake` is a terminal snake game written in C++ using the ncurses library.
Move the snake with the arrow keys, with `wasd` or with vi keys (`hjkl`).
The game starts with a speed of 1 cells/sec and every 20 points the speed
is increased by one until the maximum speed of 15 cells/sec is reached.
The speed can be increased (+) and decreased (-) during gameplay if
cheat mode is enabled.
The game has a few maps which can be activated using the -m option.
Maps are cycled automatically when the game is restarted.

## Get it and build it

Do this:

```
$  git clone git@gitlab.com:langurmonkey/tsnake.git
$  cd tsnake
$  make
```

## Running

Just run:

```
$  tsnake
```

## Usage and options

```
Usage: tsnake [OPTION]...

Options:
 -h		shows usage information
 -v		prints version and exit
 -n [SIZE]	set inital size of snake, which defaults to 4
 -m [MAP_NUM]	number of the first map to use as an integer, which is cycled
		using (map % nmaps)
 -c		deactivate colors
 -x, --cheat	activate cheat mode where speed can be increased and
		decreased with '+' and '-'
```
