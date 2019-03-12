# tsnake, a terminal snake game

`tsnake` is a terminal snake game written in C++ using the ncurses library with
support for maps and interactive resizing.

![tsnake](https://raw.githubusercontent.com/langurmonkey/tsnake/master/img/tsnake.gif)

Move the snake using the arrow keys `↑↓←→`, `wasd` or vi's `hjkl`.
The game starts with a speed of 2 m/s and every 20 points the speed
is increased by one until the maximum speed of 20 m/s is reached.
The speed can be increased with `+` and decreased with `-` during gameplay if
cheat mode (`--cheat`) is enabled.

Here are some controls:

-  (`p`) pause the game
-  (`r`) restart with next map
-  (`q`) show quit dialog

The game has a few maps which can be activated using the -m option.
Maps are cycled automatically when the game is restarted.

## Installation

If you are on **Arch Linux** or derivatives, use our [`tsnake`](https://aur.archlinux.org/packages/tsnake/) AUR package:

```
$  yay -S tsnake
```
You are done.

If you are on **any other distro**, first clone the repository.

```
$  git clone git@gitlab.com:langurmonkey/tsnake.git && cd tsnake
```

Then, build and install the software.

```
$  make && make install
```

You may need to run the `make install` as a superuser.

## Running

To run the software, just do:

```
$  tsnake
```

## Usage

Here is the info listed by the `--help` flag.

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

Also, you can check out the man page after installing if you wish.

```
$  man tsnake
```
