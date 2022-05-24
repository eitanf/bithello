/*
 * Parallel board scanning with bitmaps
 * ------------------------------------
 *
 * We need to be able to scan each row, column, and diagonal in both directions
 * to find valid moves, bordered by one of the player's pieces, an empty
 * position, and a run of opponent's pieces in between.
 *
 * We can scan all lines (rows, columns, or diagonals) in parallel using bitmaps.
 * For example, to scan all columns from top to botton, we start with a bitmask
 * where all the elements of the first row are set to 1, and check (using
 * parallel bit-wise operators which one is a candidate to start a sequence.
 * We then change our bit mask to "point" to all the elements of the second row
 * (only), and continue our parallel bitwise checks. Any legal positions are
 * identified as set bits in another bitmaps representing the complete board.
 * So to scan top-to-bottom (t2b), our initial mask is 0x000000FF, and we
 * change it 7 more times by shifting it 8 bits to the left, effectively moving
 * it to the next row. Scanning bottom-to-top is exactly the opposite.
 * Scanning left-to-right (l2r) starts with a mask where all the bits in the
 * leftmost position of each row are selected (0x808080808080), and shifts
 * right by one bit with every iteration of the scan (total of 8 masks).
 *
 * Scanning diagonally is more complicated. There are 15 diagonals an 8x8 board
 * (times four directions). But four of those diagonals are useless because they
 * are only one or two pieces long, so there can be no valid moves across those
 * diagonals. That leaves 11 diagonals to scan in parallel, ranging in length
 * from three pieces long to eight on the main diagonal.
 *
 * Consider for example the diagonals going up and to the right (bl2tr):
 * The first diagonal with at least three pieces goes from position <2,0>
 * (row 2, column 0), through <1,1> and <0,2>. The next one down starts
 * at <3,0> and goes through <2,1>, <1,2>, <0,3>, with the main diagonal going
 * from <7,0> to <0,7>. If we just started with a mask with start positions
 * <2,0> (bit position 16), <3, 0> (bit position 24), ... <7,0> (bit position 56),
 * and shifted right by 7 bits each iteration, we'd be able to effectively scan
 * the first six diagonals. Note that the first diagonal would "disappear" after
 * three shifts, because it's short, but that's ok, we'll just be testing in
 * parallel fewer set bits with every iteration after the third.
 *
 * But it gets complicated with the remaining 5 diagonals. For example, the last
 * diagonal start at position <7,5> and goes through <6,6> and <5,7>. After the
 * third iteration, when we continue shifting it right by 7 bits, it will go
 * to position <5,0>, which is obviously wrong, because it's not a continuation
 * of this diagonal, it's a different diagonal. So we need to somehow stop it
 * after three iterations. That would require complicated branching.
 *
 * Instead of stopping it early, we can start it later. We do so by extending
 * the board from 8x8 to a virtual 16x8 board. The first six diagonals still
 * ocuppy the same positions, starting at <2,0> through <7,0>. The next position
 * set will be <8,0> in the virtual board, which is outside of the 8x8 game
 * board so no bits would be set and compared against any game pieces on the
 * first iteration. But by the next iteration, we shift it right by seven bits
 * to position <7,1>, which is the starting position of the seventh diagonal.
 * It remains active for exactly seven iterations, which is its length.
 * The next diagonal starts at virtual position <9,0> and would take two
 * iterations to shift into the game board, and so forth.
 *
 * The implementation of the "virtual" board is accomplished with a new 
 * data type that has double the bits of a board (128), but can shift right or
 * left correctly. When compared to a board, only the hi or low 64 bits are used,
 * as required by the scan direction.
*/

#pragma once

#include "bits.hh"

namespace Othello {


////////////////////////////////////////////////////////////////////////////////
// Generic traversal (bit shift) operators by a compile-time constant count
template <idx_t COUNT>
struct bit_shr
{
  template <Bitwise T>
  constexpr T operator()(const T& bits) const { return bits >> COUNT; }
};

template <idx_t COUNT>
struct bit_shl
{
  template <Bitwise T>
  constexpr T operator()(const T& bits) const { return bits << COUNT; }
};


// Helpers to compute the starting points for diagonal scans into either
// a DB_lo struct (when scanning from bottom to top) or DB_hi (top to bottom).
constexpr DB_lo
diagonal_start_bottom(idx_t start_bit)
{
  DB_lo ret(0,0);
  for (unsigned i = 0; i < NDIAG; ++i) {
    ret = ret << N;
    ret.lo_ = set(ret.lo_, start_bit);
  }
  return ret;
}

constexpr DB_hi
diagonal_start_top(idx_t start_bit)
{
  DB_hi ret(0,0);
  for (unsigned i = 0; i < NDIAG; ++i) {
    ret = ret >> N;
    ret.hi_ = set(ret.hi_, start_bit);
  }
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
// Compile-time constants and operator to scan in all eight directions

// Starting positions for scans of board in a given direction.
// Remember that the LSB of a row is presented as the leftmost position.
inline constexpr bits_t L_START   = 0x0101010101010101;
inline constexpr bits_t R_START   = 0x8080808080808080;
inline constexpr bits_t T_START   = 0x00000000000000FF;
inline constexpr bits_t B_START   = 0xFF00000000000000;
inline constexpr DB_lo  BL_START  = diagonal_start_bottom(pos2bit(2, 0));
inline constexpr DB_lo  BR_START  = diagonal_start_bottom(pos2bit(2, 7));
inline constexpr DB_hi  TL_START  = diagonal_start_top(pos2bit(5, 0));
inline constexpr DB_hi  TR_START  = diagonal_start_top(pos2bit(5, 7));

// Operators to move to the next bits in the desired traversal order
inline constexpr auto L2R   = bit_shl<1>();    // horizontal left-to-right scan
inline constexpr auto R2L   = bit_shr<1>();    // horizontal right-to-left scan
inline constexpr auto T2B   = bit_shl<N>();    // vertical top-to-bottom scan
inline constexpr auto B2T   = bit_shr<N>();    // vertical bottom-to-top scan
inline constexpr auto BL2TR = bit_shr<N-1>();  // diagonal towards top-right
inline constexpr auto TR2BL = bit_shl<N-1>();  // diagonal towards bottom-left
inline constexpr auto BR2TL = bit_shr<N+1>();  // diagonal towards top-left
inline constexpr auto TL2BR = bit_shl<N+1>();  // diagonal towards bottom-right


} // namespace
