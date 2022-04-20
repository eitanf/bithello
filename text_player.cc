/*
 * Text-based UI for a human Othello player
 */

#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>

#include "text_player.hh"

namespace Othello {

using std::string;

#ifdef DISABLE_COLOR
#  define COLOR(str) ""
#else
#  define COLOR(str) str
#endif

#define BG_COLOR COLOR("\u001b[42m\u001b[34m");
#define NUM_COLOR COLOR("\u001b[42m\u001b[31m");
#define RESET_BG COLOR("\u001b[0m");

static const auto BLACK_STR = COLOR("\u001b[30m\u001b[42m") + string("●") + COLOR("\u001b[0m");
static const auto WHITE_STR = COLOR("\u001b[37m\u001b[42m") + string("●") + COLOR("\u001b[0m");
//static constexpr auto WHITE_STR = "○";

////////////////////////////////////////////////////////////////////////////////
void
TextPlayer::display_board(Board b) const
{
  std::cout << b << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
// Create a printable string representation of a board and add valid moves to it
std::string
TextPlayer::display_moves(Board board, bits_t valid, const char charset[]) const
{
  std::string str = "";
  idx_t symbol = 0;

  str += BG_COLOR;
  str += "┏";
  for (idx_t c = 0; c < N - 1; ++c)  str += "━┯";
  str += "━┓";
  str += RESET_BG;
  str +=  "\n";

  for (idx_t r = 0; r < N; ++r) {
      str += BG_COLOR;
      str += "┃";
    for (idx_t c = 0; c < N; ++c) {

      if (test(valid, r, c)) {
        str += NUM_COLOR;
        str += charset[symbol++];
        str += BG_COLOR;
      } else if (test(board.black_, r, c)) {
        str += BLACK_STR + BG_COLOR;
      } else if (test(board.white_, r, c)) {
        str += WHITE_STR + BG_COLOR;
      } else {
        str += " ";
      }
      str += (c < N - 1)? "│" : "┃";
    }
    str += RESET_BG;

    if (r < N - 1) {
      str += "\n";
      str += BG_COLOR;
      str += "┠";
      for (idx_t c = 0; c < N - 1; ++c)  str += "─┼";
      str += "─┨";
    }
    str += RESET_BG;
    str += "\n";
  }

  str += BG_COLOR;
  str += "┗";
  for (idx_t c = 0; c < N - 1; ++c)  str += "━┷";
  str += "━┛";
  str += RESET_BG;
  str += "\n";

  return str;
}

////////////////////////////////////////////////////////////////////////////////
// Ask for move presents valid moves to the user and prompts them to choose one
// by typing in the character/digit for that move, checking it's in range.
bits_t
TextPlayer::get_move(Board board, bits_t moves) const
{
  const auto nlegal = bits_set(moves);
  assert(nlegal && "Must have at least one legal move to ask for any");

  std::cout << "\n" + BLACK_STR << ": " << bits_set(board.black_) << " " <<
      WHITE_STR << ": " << bits_set(board.white_) << "    " <<
      "Legal moves for " <<
    ((color_ == Color::BLACK)? BLACK_STR : WHITE_STR) <<
    ":\n" <<
    display_moves(board, moves) << "\n";

  char c = 0;

  for (;;) {
    std::cout << "Enter move (U to undo)> ";
    std::cin >> c;
    c = toupper(c);
    if (c == 'U') {
      return 0;
    }
    if (strchr(SYMBOLS, c) && strchr(SYMBOLS, c) - SYMBOLS < nlegal) {
      break;
    }
    std::cout << "Invalid move, try again\n";
  }

  idx_t idx = 0;
  const idx_t choice = strchr(SYMBOLS, c) - SYMBOLS;
  bits_t mask(1);
  for (idx_t bit = 0; bit < N2; ++bit) {
    if (mask & moves) {
      if (idx++ == choice) {
        break;
      }
    }
    mask <<= 1;
  }

  assert(mask);
  assert(idx - 1 == choice);
  return mask;
}

////////////////////////////////////////////////////////////////////////////////
void
TextPlayer::notify_move(Board before, bits_t pos) const
{
  assert(bits_set(pos) == 1);

  if (color_ == Color::BLACK) {
    std::cout << "Player " << WHITE_STR << " chose this move:\n" <<
      display_moves(before, pos, "O");
  } else {
    std::cout << "Player " << BLACK_STR << " chose this move:\n" <<
      display_moves(before, pos, "X");
  }
}

////////////////////////////////////////////////////////////////////////////////
void
TextPlayer::game_over(Board board) const
{
  std::cout << "Final board:\n" << display_moves(board, 0, "");
  std::cout << "Count for " << BLACK_STR << ": " << bits_set(board.black_) <<
      "\tcount for " << WHITE_STR << ": " << bits_set(board.white_) << std::endl;

}

} // namespace
