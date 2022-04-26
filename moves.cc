/*
 * Functions to compute legal moves and their effects using bitboard mechanics.
 */
#include <cassert>

#include "bits.hh"
#include "board.hh"
#include "moves.hh"
#include "player.hh"

#include <iostream>

namespace Othello {

////////////////////////////////////////////////////////////////////////////////
// Three-state FSM for finding legal positions in a specific traversal direction.
// The FSM processes pieces in consecutive order, assuming some transformation
// that goes from one piece to the next in the specified direction 
// State 0 is the default: haven't seen anything interesting yet.
// State 1 is triggered by seeing one of my pieces, and stays there until either
// an opponent's piece is seen (going to state 2) or neither (back to state 0).
// State 2 describes a sequence of 1+ opponent pieces and transitions back to
// state 1 if the next piece is mine or 0 if it's empty (emitting a valid position)
//
// Truth table for state transitions:
//
//     * inputs *        | * outputs *
// State | mine | theirs | State | valid
// ------+------+--------+-------+------
//   0   |   1  |   ?    |   1   |   0
//   0   |   0  |   ?    |   0   |   0
//   1   |   1  |   ?    |   1   |   0
//   1   |   ?  |   1    |   2   |   0
//   1   |   0  |   0    |   0   |   0
//   2   |   0  |   0    |   0   |   1
//   2   |   1  |   ?    |   1   |   0
//   2   |   ?  |   1    |   2   |   0
//
// In summary, given M=mine and T=theirs:
// S = 0 iff (S == 0 && M == 0) || (M == T == 0)
// S = 1 iff M == 1
// S = 2 iff S > 0 && T == 1
// V = 1 iff S == 2 && T == M == 0
//
// To use only binary bitmaps, we can break S down into two variables:
// - saw_mine is true iff last piece was mine (corresponds to S==1)
// - saw_theirs is true iff last piece was theirs, after seeing any piece
//   (corresponds to S==2).


////////////////////////////////////////////////////////////////////////////////
// Return a bitmap of all legal positions for a given player and a board.
// Scans board in all 8 directions for valid positions and adds them to bitmap.
bits_t
all_legal_moves(Board board, Color curp)
{
  const bits_t mine = (curp == Color::DARK)? board.dark_ : board.light_;
  const bits_t theirs = (curp == Color::DARK)? board.light_ : board.dark_;

  const bits_t ret =
      legal_moves(L_START, L2R, mine, theirs)
    | legal_moves(R_START, R2L, mine, theirs)
    | legal_moves(T_START, T2B, mine, theirs)
    | legal_moves(B_START, B2T, mine, theirs)
    | legal_moves(BL_START, BL2TR, mine, theirs)
    | legal_moves(BR_START, BR2TL, mine, theirs)
    | legal_moves(TR_START, TR2BL, mine, theirs)
    | legal_moves(TL_START, TL2BR, mine, theirs);

  return ret;
}

////////////////////////////////////////////////////////////////////////////////
// Effecting a move on a board: Given a board and a signle, legal move, we can
// bring the board to the new state after effecting the board by scanning in
// all 8 directions from the move position, checking for opponent pieces
// followed by one of mine, returning a bitmap of any flipped bits.

////////////////////////////////////////////////////////////////////////////////
// Search in all directions and record the bits that were flipped, including
// the given move. Then return a new board with those bits (and the new move)
// set to the current player, and opponent's pieces flipped.
// If the move is invalid, the initial board is returned.
// This function could be turned into constexpr if defined in a header.
Board
effect_move(const Board& board, Color curp, bits_t pos)
{
  const bits_t mine = (curp == Color::DARK)? board.dark_ : board.light_;
  const bits_t theirs = (curp == Color::DARK)? board.light_ : board.dark_;

  assert(pos && !(pos & (pos - 1)) && "Move position must be a single set bit");
  assert(!(pos & mine) && "Move position can't already be mine");
  assert(!(pos & theirs) && "Move position can't already be theirs");

  const bits_t bits_flipped =
      find_flipped(pos, mine, theirs, L2R)
    | find_flipped(pos, mine, theirs, R2L)
    | find_flipped(pos, mine, theirs, T2B)
    | find_flipped(pos, mine, theirs, B2T)
    | find_flipped(pos, mine, theirs, BL2TR)
    | find_flipped(pos, mine, theirs, BR2TL)
    | find_flipped(pos, mine, theirs, TL2BR)
    | find_flipped(pos, mine, theirs, TR2BL);

  if (!bits_flipped) {
    return board;
  }

  const bits_t newm = (mine ^ bits_flipped) | pos;
  const bits_t newt = theirs ^ bits_flipped;
  return (curp == Color::DARK)? Board(newm, newt) : Board(newt, newm);
}


////////////////////////////////////////////////////////////////////////////////
// Run an interactive two-player game from a given starting point
// Returns the difference between dark tiles and light tiles at the end.
int
play_game(Board board, player_ptr_t me, player_ptr_t opponent)
{
  board.assert_valid();
  constexpr int UNDO[2] = { -int(N2) - 1, N2 + 1 }; // Flags for an undo request

  bits_t pos = 0;
  auto legal = all_legal_moves(board, me->color_);

  // If this player has no moves, try the other player:
  if (!legal) {
    std::swap(me, opponent);
    legal = all_legal_moves(board, me->color_);
    if (!legal) {  // Nobody has moves, game over!
      me->game_over(board);
      opponent->game_over(board);
      return bits_set(board.dark_) - bits_set(board.light_);
    }
  }

  // Try to get a valid move (or UNDO for this player):
  while (!pos) {
    pos = me->get_move(board, legal);
    if (!pos) {  // Undo requested
      if (bits_set(board.dark_) + bits_set(board.light_) < 6) {
        std::cerr << "Can't undo yet!\n";
      } else {
        return UNDO[int(me->color_)];
      }
    }
  }

  // Regular move, notify it and continue playing:
  opponent->notify_move(board, pos);
  const auto diff = play_game(effect_move(board, me->color_, pos), opponent, me);
  // If we got a normal game outcome or an undo request from the opponent, pass it up:
  if (diff != UNDO[int(me->color_)]) {
    return diff;
  }
  // If we got an undo requested downstream from my player, replay this move:
  return play_game(board, me, opponent);
}


} // namespace
