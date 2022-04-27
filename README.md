# Bithello -- Bitboard Othello

## Building the code

Try the following instructions in a terminal (tested on Limux and MacOS):

```
git clone https://github.com/eitanf/bithello
make -j test
make -j
```
## Quick start

To run a quick game against an easy  AI, try the following in the terminal:

```
./bithello -d text -l mcts -m 200
```

You will see an initial board game that looks like this:

![initial board](initial.png)

You play the first (dark) player. You'll see digits or letter for your possible legal moves. For example, let's say you want to make the move denoted by `2`. Simply enter 2 on the prompt and press enter. The computer player (light) will now make its turn, and the board may look something like this (depending on the move the computer chose):

![next move](move2.png)

Continue playing till no more legal moves are available, at which point a winner is declared.

You can find instructions on how to play Othello [here](https://www.ultraboardgames.com/othello/game-rules.php) or [here](https://en.wikipedia.org/wiki/Reversi). You may also find this [book](https://web.archive.org/web/20061209182837/http://othellogateway.strategicviewpoints.com/rose/book.pdf) interesting if you want to dive deeper into the strategy of the game.

## Command-line options

You can set the different players to be either human (text); random, which picks a random move each turn and is very easy to beat (random), or Monte-Carlo tree-search AI (mcts). You can further configure the strength of the AI player. Try `./bithello -h` for a full list of current options.

You can play human against human on the same terminal (both players as `text`), human against computer, or computer against computer. The MCTS player can be configured to evaluate a fixed number of moves per turn, or a fixed amount of time in milliseconds.

## Performance

If you're curious about the performance of the MCTS algorithm, or you want to improve it, you can turn on the performance counters that measure how many total game plays and moves each MCTS player evaluated. To enable these counters, add `-DBENCHMARK` to the compilation flags (`CXXFLAGS`) in the Makefile, then run `make clean && make` and run a game of computer against computer.

By default, bithello will use as many threads as the hardware supports. You can control the actual number of threads with the NTHREAD environment variable. For example, to run a single-threaded MCTS player (50ms per move) against the random player, you can try:

```
NTHREAD=1 ./bithello -d mcts -t 50 -l random
```

For example, running on 32 threads, MCTS against itself (5 seconds per move) on an AMD 3970X processor (g++ v. 10) evaluates a total of about 4.4B games and 17.6B moves. Divided by 300 seconds total run time, this yields about 14.8M games/sec.

### License

Distributed under the [GPL v.3](https://www.gnu.org/licenses/gpl-3.0.en.html) license.
