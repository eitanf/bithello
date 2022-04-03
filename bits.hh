/*
 * Bit-manipulation types and functions.
 * The main bitmap type is a bits_t type (uint64_t), holding exactly enough bits
 * for each position in an 8x8 Othello board.
 * Indexing into a bitmap is done with the idx_t type.
 * In some cases we need a double-board size type (DB_base), with two subtypes,
 * once for comparisons with the high bitmap and one with the low.
 */

#pragma once

#include <cassert>
#include <cstdint>

namespace Othello {

using idx_t = unsigned;     // A row or column index into the board
constexpr idx_t N = 8;      // Board dimension: an Othello board is always 8x8
constexpr idx_t N2 = N * N; // Board size
constexpr idx_t NDIAG = 11; // No. of valid diagonals in a board.

// A contiguous bit vector for every piece on the board:
using bits_t = uint64_t;
constexpr bits_t ONE(1);

////////////////////////////////////////////////////////////////////////////////
// Utility functions to handle bitmaps/boards:

// Translate row/col position into bit index:
constexpr idx_t pos2bit(idx_t row, idx_t col) { return row * N + col; }

// set/test/clear:
constexpr bits_t set(bits_t bits, idx_t bit) { return bits | (ONE << bit); }
constexpr bits_t set(bits_t bits, idx_t row, idx_t col) { return set(bits, pos2bit(row, col)); }
constexpr bits_t setpos(idx_t row, idx_t col) { return set(0ull, row, col); }

constexpr bits_t test(bits_t bits, idx_t bit) { return (bits >> bit) & ONE; }
constexpr bits_t test(bits_t bits, idx_t row, idx_t col) { return test(bits, pos2bit(row, col)); }

constexpr bits_t clear(bits_t bits, idx_t bit) { return bits & ~(ONE << bit); }
constexpr bits_t clear(bits_t bits, idx_t row, idx_t col) { return clear(bits, pos2bit(row, col)); }

// Total count of set bits:
constexpr idx_t bits_set(bits_t bits) { return __builtin_popcountll(bits); }

////////////////////////////////////////////////////////////////////////////////
// Double-width bitmap classes for diagonal traversals.
// Holds two bits_t bitmaps, but only one (either hi_ or lo_) is used for &
// operators against regular-sized bitmaps. The other bitmap holds bits and
// can shift one way into the active bitmap or receive bits from it the other way.

template <bool USE_HI>
struct DB {
 public:
  bits_t hi_, lo_;
  constexpr DB(bits_t hi, bits_t lo) : hi_(hi), lo_(lo) {}
};

// Logical & operators depending on the class:
template <bool USE_HI>
constexpr bits_t
operator&(bits_t bits, DB<USE_HI> db)
{
  return bits & (USE_HI? db.hi_ : db.lo_);
}

template <bool USE_HI>
constexpr bits_t
operator&(DB<USE_HI> db, bits_t bits) { return bits & db; }

// Logical shift operators that work on both DB_hi and DB_lo:
template <bool USE_HI>
constexpr DB<USE_HI>
operator<<(DB<USE_HI> db, idx_t count)
{
  assert(count < N2 && "Large shifting is undefined behavior");
  return DB<USE_HI>((db.hi_ << count) | (db.lo_ >> (N2 - count)), db.lo_ << count);
}

template <bool USE_HI>
constexpr DB<USE_HI>
operator>>(DB<USE_HI> db, idx_t count)
{
  assert(count < N2 && "Large shifting is undefined behavior");
  return DB<USE_HI>((db.hi_ >> count),  (db.hi_ << (N2 - count)) | db.lo_ >> count);
}

////////////////
using DB_hi = DB<true>;
using DB_lo = DB<false>;

} // namespace
