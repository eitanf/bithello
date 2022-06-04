/*
 * A board is represented as two bitmaps, one for dark player, one for light.
 * Each bitmap maps from every position in the board to whether that player
 * has a piece occupying that position. 8x8 board positions translate to 64 bits.
 * Board positions are interpreted where each byte represents a row (row zero is
 * the least-significant byte). Each column is represented by one bit in the
 * byte (least-significant bit is first column, so indexing is right-to-left).
 */

#pragma once

#include <cassert>
#include <string>
#include <vector>

#include "bits.hh"

namespace Othello {

////////////////////////////////////////////////////////////////////////////////
// A complete game board consists of two bitmaps, each for one color
class Board {
 public:
  ~Board() = default;
  Board(const Board&) = default;
  Board(Board&&) = default;
  constexpr Board(bits_t dark, bits_t light) : dark_(dark), light_(light) {}

  // Initialize a board from rows: strings of optional dark/light pieces
  // Each string in the vector represents one row, where a charachter that
  // matches bchar will be marked dark and wchar marked light.
  // You don't have to provide the full 8 rows or full 8 columns per row.
  Board(const std::vector<std::string>& rows, char bchar = 'x', char wchar = 'o')
  : dark_(mark_bits(rows, bchar)), light_(mark_bits(rows, wchar))
  {}

  constexpr bool operator==(const Board&) const = default;

  // Verify the board is a legal configuration under Othello rules:
  void assert_valid() const;

  // How many more turns are left on this board?
  constexpr unsigned moves_left() const
  {
    return N2 - bits_set(dark_) - bits_set(light_);
  }

  constexpr bits_t dark() const { return dark_; }
  constexpr bits_t light() const { return light_; }
 private:
  // Given a vector of row strings, return a bitmap to all positions in all rows
  // that have the given symbol in the string position.
  bits_t mark_bits(const std::vector<std::string>& rows, char symbol) const;
  const bits_t dark_;
  const bits_t light_;
};


////////////////////////////////////////////////////////////////////////////////
// Output a board pretty-printed to a stream
std::ostream& operator<<(std::ostream&, Board);

} // namespace
