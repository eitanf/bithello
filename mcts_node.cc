/*
 * Implementation for MTCS_node class
 */

#include "mcts_node.hh"

#include <cassert>

namespace Othello {

////////////////////////////////////////////////////////////////////////////////
MCTSNode::MCTSNode(Board board, Color turn, const MCTSNode* parent)
  : board_(board),
    d_wins_(0), l_wins_(0),
    parent_(reinterpret_cast<size_t>(parent) | int(turn))
{
  assert(!(reinterpret_cast<size_t>(parent) & 1));
}

////////////////////////////////////////////////////////////////////////////////
void
MCTSNode::mark_win(Color whom)
{
  if (whom == Color::DARK) {
    count_wins(1, 0);
  } else {
    count_wins(0, 1);
  }
}

////////////////////////////////////////////////////////////////////////////////
void
MCTSNode::count_wins(uint32_t d_wins, uint32_t l_wins)
{
  d_wins_ += d_wins;
  l_wins_ += l_wins;

  const auto parent = reinterpret_cast<MCTSNode*>(parent_ & ~1);
  if (parent) {
    parent->count_wins(d_wins, l_wins);
  }
}

////////////////////////////////////////////////////////////////////////////////
double
MCTSNode::win_odds(Color whom) const
{
  const double wins = (whom == Color::DARK)? d_wins_ : l_wins_;
  const double losses = (whom == Color::DARK)? l_wins_ : d_wins_;
  return wins / (losses + 1);
}

////////////////////////////////////////////////////////////////////////////////
std::ostream&
MCTSNode::operator<<(std::ostream& os)
{
  os << "Node has board: " << board_ << "\n";
  os << "turn: " << ((turn() == Color::LIGHT)? "light" : "dark") << "\t";
  os << "dark wins: " << d_wins_ << "\t";
  os << "light wins: " << l_wins_ << "\n";
  return os;
}

} // namespace

