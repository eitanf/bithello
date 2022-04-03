#include <ostream>

#include "board.hh"

namespace Othello {

////////////////////////////////////////////////////////////////////////////////
bits_t
Board::mark_bits(const std::vector<std::string>& rows, char symbol) const
{
  bits_t ret = 0;

  assert(rows.size() <= N && "Can't have more than N rows");
  for (idx_t r = 0; r < rows.size(); ++r) {
    assert(rows[r].size() <= N && "Can't have more than N columns");
    for (idx_t c = 0; c < rows[r].size(); ++c) {
      if (rows[r][c] == symbol) {
        ret = set(ret, r, c);
      }
    }
  }
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
void
Board::assert_valid() const
{
  constexpr bits_t middle = 0x0000001818000000; // All four middle positions
  (void)middle;

  assert (!(black_ & white_) &&
      "Can't have locations that are both white and black");

  assert (((black_ | white_) & middle) >= middle &&
      "At least the four middle positions have to be occupied");
}

////////////////////////////////////////////////////////////////////////////////
std::ostream&
operator<<(std::ostream& out, Board board)
{
  for (idx_t r = 0; r < N; ++r) {
    for (idx_t c = 0; c < N; ++c) {
      if (test(board.white_, r, c)) {
        out << "o";
//        out << "○";
      } else if (test(board.black_, r, c)) {
        out << "x";
//        out << "●";
      } else {
        out << ".";
      }
    }
    out << "\n";
  }
  return out;
}

} // namespace

// namespace
