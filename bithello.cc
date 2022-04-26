/*
 * Bithello: an efficient Othello game tree-search engine.
 * Author: Eitan Frachtenberg (c) 2022
 *
 * All checks for valid moves are performed via branch-free bit manipulations
 * that can execute on all bytes (rows) in parallel.
 */

#include <cassert>
#include <cstring>
#include <climits>
#include <iostream>
#include <string>
#include <memory>

#include "board.hh"
#include "moves.hh"
#include "mcts_player.hh"
#include "random_player.hh"
#include "text_player.hh"

using namespace std;

namespace Othello {

const string MCTS_STR("mcts");
const string RAND_STR("random");
const string TEXT_STR("text");
const uint64_t DEFAULT_MOVES = 1000;

////////////////////////////////////////////////////////////////////////////////
// help(): Display command-line options
void help(const char* pname)
{
  cerr << pname << ": Run two-player othello game. Required arguments:\n" <<
    "-b player_args\n" << "-w player_args\n" <<
    "Where -b denotes the first player (black), -w the white player,\n" <<
    "and player_args is one of the following player types/arguments:\n" <<
    "\t" << RAND_STR << ": An 'AI' player that picks legal moves at random\n" <<
    "\t" << TEXT_STR << ": A text-based UI for a human player\n" <<
    "\t" << MCTS_STR << ": A monte-carlo tree-search AI player\n" <<
    "\t\tOptions for MCTS:\n" <<
    "\t\t -m [number]: how many moves to evaluate for each turn (default: " <<
    DEFAULT_MOVES << ")\n" <<
    "\t\t -d [number]: how many milliseconds to evaluate in each turn\n" <<
    "All player types can be abbreviated to unique prefix.\n" <<
    "Example: start a game with first player human, second player easy MCTS:\n" <<
    "\t" << pname << " -b text -w mcts -m 100" <<
    endl;
  exit(-2);
}

////////////////////////////////////////////////////////////////////////////////
//  Parse a set of command line arguments to extract a type of player+arguments,
//  and allocate a new player of this type.
//  Returns nullptr for any parsing error.
//  Modifies argc/argv to "consume" all the used command-line arguments.
player_ptr_t
parse_player_options(Color color, int& argc, char**& argv)
{
  stop_ptr_t stopper;
  const string type(*argv++);
  --argc;

  if (RAND_STR.find(type) == 0) {
    return new RandomPlayer(color);
  }

  if (TEXT_STR.find(type) == 0) {
    return new TextPlayer(color);
  }

  if (MCTS_STR.find(type) == 0) {
  // If the player is MCTS, we need to parse more optional arguments:
    if (argc && !strcmp(*argv, "-m")) {
      ++argv; -- argc;
      uint64_t moves;
      if (!argc-- || (moves = atoll(*argv++)) < 1) {
        return nullptr;
      }
      stopper = shared_ptr<StopCondition>(new StopByMoves(moves));

    } else if (argc && !strcmp(*argv, "-d")) {
      ++argv; -- argc;
      uint64_t duration;
      if (!argc-- || (duration = atoll(*argv++)) < 1) {
        return nullptr;
      }
#ifdef BENCHMARK
      stopper = shared_ptr<StopCondition>(new StopByDuration(color, duration));
#else
      stopper = shared_ptr<StopCondition>(new StopByDuration(duration));
#endif

    } else {
      stopper = shared_ptr<StopCondition>(new StopByMoves(DEFAULT_MOVES));
    }

    return new MCTSPlayer(color, stopper);
  }

  return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Find out the the types and parameters for the black and white players from
// the command line, and return those as newly allocated Player pointers.
pair<player_ptr_t, player_ptr_t>
parse_command_line(int argc, char* argv[])
{
  player_ptr_t bp = nullptr;
  player_ptr_t wp = nullptr;
  const auto pname = argv[0];
  argv++; argc--;

  if (argc < 4) {
    help(pname);
  }

  while (argc > 1) {  // Loop till we have exhausted command-line arguments
    if (string(*argv) == "-b") {
      if (!(bp = parse_player_options(Color::BLACK, --argc, ++argv))) {
        cerr << "ERR bp\n";
        help(pname);
      }
    }
    else if (argc && string(*argv) == "-w") {
      if (!(wp = parse_player_options(Color::WHITE, --argc, ++argv))) {
        cerr << "ERR wp\n";
        help(pname);
      }
    }
    else help(pname);
  }

  if (!bp || !wp) {
    help(pname);
  }

  return { bp, wp };
}

} // namespace
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
  using namespace Othello;

  static_assert(N == 8, "This program not optimized for board sizes other than 8x8");
  static_assert(N2 == sizeof(bits_t) * CHAR_BIT, "Must have exactly 64 bits");

  auto [ black, white ] = parse_command_line(argc, argv);

  const Board board({ "", "", "", "...ox", "...xo" });
  const auto tile_diff = play_game(board, black, white);

  cout << "Winner is: ";
  if (tile_diff > 0) {
    cout << "Black\n";
  } else if (tile_diff < 0) {
    cout << "White\n";
  } else {
    cout << "tie!\n";
  }

  delete black;
  delete white;
  return tile_diff;
}

