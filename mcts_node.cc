/*
 * Implementation for MTCS_node class
 */

#include "mcts_node.hh"

#include <cassert>

namespace Othello {

////////////////////////////////////////////////////////////////////////////////
MCTSNode::MCTSNode(Board board, Color turn, const MCTSNode* parent)
  : board_(board),
    b_wins_(0), w_wins_(0),
    parent_(reinterpret_cast<size_t>(parent) | int(turn))
{
  assert(!(reinterpret_cast<size_t>(parent) & 1));
}

////////////////////////////////////////////////////////////////////////////////
void
MCTSNode::mark_win(Color whom)
{
  if (whom == Color::BLACK) {
    count_wins(1, 0);
  } else {
    count_wins(0, 1);
  }
}

////////////////////////////////////////////////////////////////////////////////
void
MCTSNode::count_wins(uint32_t b_wins, uint32_t w_wins)
{
  b_wins_ += b_wins;
  w_wins_ += w_wins;

  const auto parent = reinterpret_cast<MCTSNode*>(parent_ & ~1);
  if (parent) {
    parent->count_wins(b_wins, w_wins);
  }
}

////////////////////////////////////////////////////////////////////////////////
double
MCTSNode::win_odds(Color whom) const
{
  const double wins = (whom == Color::BLACK)? b_wins_ : w_wins_;
  const double losses = (whom == Color::BLACK)? w_wins_ : b_wins_;
  return wins / (losses + 1);
}

////////////////////////////////////////////////////////////////////////////////
std::ostream&
MCTSNode::operator<<(std::ostream& os)
{
  os << "Node has board: " << board_ << "\n";
  os << "turn: " << ((turn() == Color::WHITE)? "white" : "black") << "\t";
  os << "black wins: " << b_wins_ << "\t";
  os << "white wins: " << w_wins_ << "\n";
  return os;
}

} // namespace

