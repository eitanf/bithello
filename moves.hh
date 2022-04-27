/*
 * Efficient routines for finding and effecting (playing) legal moves in Othello
 * Searching for moves is done separately for each of the eight directions for
 * moves (iteratively). But in each direction, we scan 8 rows/columns in parallel
 * or 11 diagonals in parallel, using bitwise operations that implement a
 * finite-state machine (FSM) for finding valid positions. We run through the
 * FSM for the maximum length of a row/column/diagonal, which is 8 steps.
 * For each scan, we have a bitmap representing the starting pieces in each
 * row/column/diagonal, and an operator that shifts this bitmap onto the next
 * bits in the board (next in row/column/diagonal).
 *
 * Once a move is selected, as denoted by a bitmap with a single bit selected
 * for the position of the next move, we can effect the move, that is, flip the
 * pieces that are newly sandwiched by the player's pieces, by again scanning
 * across the board bitmap in eight directions, until we hit the starting bitmap.
 */

#pragma once

#include "board.hh"
#include "player.hh"
#include "scan.hh"

namespace Othello {

// Search in all directions and record the bits that were flipped, including
// the given move. Then return a new board with those bits (and the new move)
// set to the current player, and opponent's pieces flipped.
// If the move is invalid, the initial board is returned.
Board effect_move(const Board& board, Color curp, bits_t pos);


// Play a game till no more legal moves are available.
// Returns a positive number if `me' wins, negative if opponent, 0 for tie.
int play_game(Board board, player_ptr_t me, player_ptr_t opponent);

// Return a bitmap of all legal positions for a given player and a board.
// Scans board in all 8 directions for valid positions and adds them to bitmap.
bits_t all_legal_moves(Board board, Color curp);

/////////////////// Details:

// Given a mask representing starting position for a scan, an operator next that
// moves the mask to the next piece in the row/column/diagonal, and the two
// bitmaps for my and opponent's pieces, compute a bitmap of all board positions
// that would represent a valid move in the current scan direction.
template <class NEXT, typename MASK, idx_t ITERS = N>
constexpr inline bits_t legal_moves(MASK mask, NEXT next, bits_t mine, bits_t theirs);

// Search in one direction, starting from a single bit `start` and advancing
// each iteration using next, until we're either at the board's border or
// we're no longer seeing opponent pieces. In that case, if the current piece
// is mine, then the bits we collected are good, otherwise, nothing to flip.
template <class NEXT>
constexpr inline bits_t find_flipped(bits_t start, bits_t mine, bits_t theirs, NEXT next);


// Is the current position showing one of my pieces?
constexpr inline bits_t is_mine(bits_t mine) {
  return mine;
}

// Is the current position showing one of opponent's pieces, after one of mine
// or one of theirs (i.e., not after an unoccopied position):
constexpr inline bits_t is_theirs(bits_t saw_any, bits_t theirs)
{
  return saw_any & theirs;
}

// Is the current posistion a valid move for me, i.e., empty and after theirs?
constexpr inline bits_t is_valid_pos(bits_t saw_theirs, bits_t mine, bits_t theirs)
{
  return saw_theirs & ~mine & ~theirs;
}

// For every direction, there is a bitmap that represents bits that are not
// the last bits of that direction, i.e., not on the edge. This bitmap is
// exactly the inverse of the bitmap for the start positions in the opposite
// direction. This function is overloaded on an unused "next" variable in each direction.
constexpr inline bits_t in_scan(decltype(L2R), bits_t pos) { return pos & ~R_START; }
constexpr inline bits_t in_scan(decltype(R2L), bits_t pos) { return pos & ~L_START; }
constexpr inline bits_t in_scan(decltype(T2B), bits_t pos) { return pos & ~B_START; }
constexpr inline bits_t in_scan(decltype(B2T), bits_t pos) { return pos & ~T_START; }
constexpr inline bits_t in_scan(decltype(BL2TR), bits_t pos) { return pos & ~(TR_START.hi_); }
constexpr inline bits_t in_scan(decltype(BR2TL), bits_t pos) { return pos & ~(TL_START.hi_); }
constexpr inline bits_t in_scan(decltype(TL2BR), bits_t pos) { return pos & ~(BR_START.lo_); }
constexpr inline bits_t in_scan(decltype(TR2BL), bits_t pos) { return pos & ~(BL_START.lo_); }



/*
 * Template & inline function definitions
 */


////////////////////////////////////////////////////////////////////////////////
// Check N lines in parallel for an empty position that surrounds the opponent.
// See documentation in moves.cc for details on the algorithm.
template <class NEXT, typename MASK, idx_t ITERS>
constexpr inline bits_t
legal_moves(MASK mask, NEXT next, bits_t mine, bits_t theirs)
{
  bits_t saw_mine = 0;
  bits_t saw_theirs = 0;
  bits_t valid = 0;

  for (idx_t i = 0; i < ITERS; i++) {
    const bits_t m = mine & mask;  // Filter to current positions only
    const bits_t t = theirs & mask;

    valid |= is_valid_pos(saw_theirs, m, t);
    saw_theirs = next(is_theirs(saw_mine | saw_theirs, t));
    saw_mine = next(is_mine(m));
    mask = next(mask);
  }
  return valid;
}

////////////////////////////////////////////////////////////////////////////////
template <class NEXT>
constexpr inline bits_t
find_flipped(bits_t start, bits_t mine, bits_t theirs, NEXT next)
{
  bits_t mask = next(start), ret = 0;
  for (; in_scan(next, mask) & theirs; mask = next(mask)) {
    ret |= mask;
  }

  return (mask & mine)? ret : bits_t(0);
}



} // namespace
