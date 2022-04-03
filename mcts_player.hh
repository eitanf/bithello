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
  MCTSPlayer(Color color,
      StopCondition& stop,
      uint64_t seed = 0);

  virtual ~MCTSPlayer();

  virtual void display_board(Board) const {};

  // Fetch the move with the best odds of winning based on MCTS:
  virtual bits_t get_move(Board b, bits_t moves) const;

  virtual void notify_move(Board, bits_t) const {};

  virtual void game_over(Board) const {};

 private:
  Player* myp_;
  Player* opp_;
  StopCondition& stop_;
  mutable std::default_random_engine generator_;
  mutable std::uniform_int_distribution<idx_t> dist_;

  // A pair with a new move and the resulting board (in node)
  using nodes_t = std::vector<std::pair<bits_t, MCTSNode>>;

  // Compute all nodes for a given starting board and legal moves:
  nodes_t all_nodes(Board board, bits_t moves) const;

  // Return the move that is associated with the best odds of winning among nodes
  bits_t highest_win_odds(const nodes_t& nodes) const;
};

} // namespace
