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

  assert (!(dark_ & light_) &&
      "Can't have locations that are both light and dark");

  assert (((dark_ | light_) & middle) >= middle &&
      "At least the four middle positions have to be occupied");
}

////////////////////////////////////////////////////////////////////////////////
std::ostream&
operator<<(std::ostream& out, Board board)
{
  for (idx_t r = 0; r < N; ++r) {
    for (idx_t c = 0; c < N; ++c) {
      if (test(board.light_, r, c)) {
        out << "o";
      } else if (test(board.dark_, r, c)) {
        out << "x";
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
