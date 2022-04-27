/*
 * AI implementation of a computer player that searches the moves tree
 * using Monte-Carlo tree search.
 */

#pragma once

#include "mcts_node.hh"
#include "player.hh"
#include "stop.hh"

#include <random>
#include <vector>

namespace Othello {

class MCTSPlayer : public Player {
 public:

  // If seed is zero, some random value will be picked.
  MCTSPlayer(Color color, stop_ptr_t stop);

  virtual ~MCTSPlayer();

  virtual void display_board(Board) const {};

  // Fetch the move with the best odds of winning based on MCTS:
  virtual bits_t get_move(Board b, bits_t moves) const;

  virtual void notify_move(Board, bits_t) const {};

  virtual void game_over(Board) const {};

 private:
  // A pair with a new move and the resulting board (in node)
  using nodes_t = std::vector<std::pair<bits_t, MCTSNode>>;

  stop_ptr_t stop_;
  unsigned nthread_;

  // Compute all nodes for a given starting board and legal moves:
  nodes_t compute_nodes(Board board, bits_t moves) const;

  // Return the move that is associated with the best odds of winning among nodes
  bits_t highest_win_odds(const nodes_t& nodes) const;

  // Run a set of simulated games from current nodes and collect win statistics in nodes
  void simulate_games(nodes_t& nodes) const;

 private:
#ifdef BENCHMARK  // Benchmarking stat counters
  mutable int64_t total_plays_ = 0;
  mutable int64_t total_moves_ = 0;
#endif
};

} // namespace
