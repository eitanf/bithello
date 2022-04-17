/*
 * Class definitions for a node and a tree of nodes for Monte-Carlo Tree Search.
 * An MCTS starts from a vector of current available moves, for a given player.
 * One of these moves is picked at random, and a game is simulated to completion
 * from this node (move), eventually incrementing a counter for the winner,
 * two counters per node (one for each player). When the search is over, using
 * any desired termination criterion, the node (move) with the best winning
 * odds for the current player is picked.
 *
 * Nodes can be organized into layers (levels of the tree), starting from the
 * current possible move, and possibly elaborating a full game tree for a few
 * moves (levels). The tree search can then start from the nodes at the lowest
 * computed level instead of the top (current) level, to memoize the first few
 * moves.
 *
 * Each node represents a board state, and keeps track of the total wins for
 * black and white players that have been encountered in the random tree search.
 * Each node also holds a pointer to the parent node, the board from which this
 * move was derived (or nullptr if its the top level of the memoized tree).
 * This pointer is tagged to also hold the color for the player whose turn
 * it is at this node, so that the LSB has the same value as its color.
 */

#pragma once

#include <ostream>

#include "bits.hh"
#include "board.hh"
#include "moves.hh"

namespace Othello {

////////////////////////////////////////////////////////////////////////////////
// Tree node data
class MCTSNode {
 public:
  MCTSNode(Board board, Color turn, const MCTSNode* parent = nullptr);
  ~MCTSNode() = default;

  // Signal that a random game that started in this node was won by `who`
  // This operation needs to be thread-safe.
  void mark_win(Color whom);

  // Add specific win counts for both players:
  void count_wins(uint32_t b_wins, uint32_t w_wins);

  // Estimate the probabily for player `whom` to win starting from this node
  double win_odds(Color whom) const;

  const Board& board() const { return board_; }

  // Return the color of the current player
  Color turn() const { return Color(parent_ & 1); }

  std::ostream& operator<<(std::ostream&);

 private:
  Board       board_;
  uint32_t    b_wins_;
  uint32_t    w_wins_;
  size_t      parent_;
};

} // namespace
