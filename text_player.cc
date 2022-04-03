/*
 * Text-based UI for a human Othello player
 */

#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>

#include "text_player.hh"

namespace Othello {


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
  std::stringstream s;
  s << board;
  auto str = s.str();

  idx_t i = 0;
  for (idx_t r = 0; r < N; ++r) {
    for (idx_t c = 0; c < N; ++c) {
      if (test(valid, r, c)) {
        str[r * (N + 1) + c] = charset[i++];
      }
    }
  }
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

  std::cout << "Legal moves for " <<
    ((color_ == Color::BLACK)? "x" : "o") <<
    ":\n" <<
    display_moves(board, moves) << "\n";

  char c = 0;

  for (;;) {
    std::cout << "Enter move> ";
    std::cin >> c;
    c = toupper(c);
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
    std::cout << "White player chose this move:\n" <<
      display_moves(before, pos, "O");
  } else {
    std::cout << "Black player chose this move:\n" <<
      display_moves(before, pos, "X");
  }
}

////////////////////////////////////////////////////////////////////////////////
void
TextPlayer::game_over(Board board) const
{
  std::cout << "Final board:\n" << board << "\n";
  const auto b = bits_set(board.black_);
  const auto w = bits_set(board.white_);
  std::cout << "Count for x: " << b << "\tcount for o: " << w << std::endl;
}

} // namespace
