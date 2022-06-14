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
 * dark and light players that have been encountered in the random tree search.
 * Each node also holds a pointer to the parent node, the board from which this
 * move was derived (or nullptr if its the top level of the memoized tree).
 */

#pragma once

#include <cassert>
#include <ostream>

#include "bits.hh"
#include "board.hh"
#include "moves.hh"

namespace Othello {

////////////////////////////////////////////////////////////////////////////////
// Tree node data
class MCTSNode {
 public:
  MCTSNode(Board board, Color turn, bits_t mv = 0, MCTSNode* parent = nullptr)
  : board_(board), player_(turn), move_(mv),
    d_wins_(0), l_wins_(0), parent_(parent)
  {}

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
  Color turn() const { return player_; }

  // Return the move that spawned this board/node
  bits_t original_move() const { assert(move_); return move_; }

  std::ostream& operator<<(std::ostream&);

 private:
  Board      board_;  // The current board
  Color      player_; // The current player
  bits_t     move_;   // The (previous) move that led to this board
  uint32_t   d_wins_; // How many times dark won from this board
  uint32_t   l_wins_; // How many times light won from this board
  MCTSNode*  parent_; // Parent node (if any)
};

} // namespace
