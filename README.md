#Pong
---
My own implementation of the game ['Pong'](https://en.wikipedia.org/wiki/Pong) using the [Raylib](https://github.com/raysan5/raylib) library.

##Building
This project only depends on the Raylib library (>=v3.7.0)

This project was built on a Windows 10 machine using [Mingw-w64](https://www.mingw-w64.org). A Linux build will be coming soon.

To build this on Windows, first make sure Mingw-w64 is installed, then create `bin`, `bin\release`, `obj`, `include`, and `lib` directories in the project root directory. Put `raylib.h` header file in `include` and `libraylib.a` file in `lib`. And run `mingw32-make`. The executable will reside in `bin\release`

##Gameplay
Deflect the ball as many times as you can. Each block speeds up the ball. Game ends when the ball reaches the left or right side of the screen. You will score a point every time you block the ball.

*Note: I know this isn't the usual mechanics for Pong. It's just that I implemented a 1-player mode first and had trouble balancing the CPU paddle; it will either be too easy or block the ball every time. So I just changed the rules a bit and made it sort of an endurance game*

##Controls
Up and down arrow keys to move up and down, respectively. Esc to quit the game (this is the only way to get out of the game over screen as a restart feature is not implemented yet).

##Known Glitches
Sometimes, when the ball hits either paddle at a certain angle, it would just 'stick' to the side of the paddle and it will then launch itself once it leaves the lenght of the paddle.

##TODO
- Linux build
- 2-player mode
- alternate 1-player mode wherein the player controls both paddles and will be playing against the ball
- other improvements such as a restart key
