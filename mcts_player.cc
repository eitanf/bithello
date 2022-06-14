/*
 * AI implementation of a computer player based on Monte-Carlo tree search
 */

#include "bits.hh"
#include "mcts_player.hh"
#include "moves.hh"
#include "random_player.hh"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <vector>

namespace Othello {

////////////////////////////////////////////////////////////////////////////////
// Checks the envrionment variable NTHREAD to set how many threads to use.
// If it's not defined, just uses all available hardware threads.
MCTSPlayer::MCTSPlayer(Color color, stop_ptr_t stop)
: Player(color),
  stop_(stop),
  nthread_(std::thread::hardware_concurrency()),
  pool_(nthread_)
{
  if (auto thread_str = getenv("NTHREAD")) {
    nthread_ = atoi(thread_str);
  }
}

////////////////////////////////////////////////////////////////////////////////
// If we measured MCTS performance, output it upon destruction
MCTSPlayer::~MCTSPlayer()
{
#ifdef BENCHMARK
  std::cout << total_moves_ << std::endl;
  std::clog.imbue(std::locale(""));
  std::clog << "Player " << (color_ == Color::DARK? "dark" : "light") <<
    " evaluated a total of " << total_plays_ << " games and " <<
    total_moves_ << " moves" << std::endl;
#endif
}

////////////////////////////////////////////////////////////////////////////////
// Create a vector of pairs of valid game moves and the board they'd cause if
// that move is taken (an MCTSNode).
MCTSPlayer::nodes_t
MCTSPlayer::compute_nodes(Board board, bits_t moves) const
{
  nodes_t nodes;

  for (bits_t pos = 1; pos; pos <<= 1) {
    if (moves & pos) {
      const auto newb = effect_move(board, color_, pos);
      nodes.push_back(MCTSNode(newb, opponent_of(color_), pos));
    }
  }
  return nodes;
}

////////////////////////////////////////////////////////////////////////////////
// Given a subrange of nodes (moves and win records), pick the move that had the
// most wins relative to games played from that node.
bits_t
MCTSPlayer::highest_win_odds(nodes_iter_t begin, nodes_iter_t end) const
{
  assert(nodes.size() > 0);
  assert(std::distance(begin, end) > 0);

  const auto best = std::max_element(begin, end,
      [&](const auto& n1, const auto& n2) {
        return n1.win_odds(color_) < n2.win_odds(color_);
      });

  assert(best->win_odds(color_) >= 0);
  return best->original_move();
}

////////////////////////////////////////////////////////////////////////////////
// simulate games runs a loop until the external stop condition is triggered.
// Througout the loop, it pits two random players against each other, starting
// from a given board that is selected from a round-robin scan of all legal
// moves from the current board (starting at a random choice of move).
// After each simulated game, it records the game stats in local variables.
// When the loop is done, all stats are added to member variables, under a lock.
void
MCTSPlayer::simulate_games(nodes_t& nodes) const
{
  std::mutex mut;
  const int nmoves = nodes.size();
  assert(nmoves);
  std::vector<uint64_t> d_wins(nmoves, 0);
  std::vector<uint64_t> l_wins(nmoves, 0);

#ifdef BENCHMARK
  int64_t plays = 0;
#endif

  auto myp = RandomPlayer(color_);
  auto opp = RandomPlayer(opponent_of(color_));
  StopCondition& stop = *stop_;
  int idx = rand() % nmoves;  // Round-robin index into nodes

  for (; !stop(); idx = (idx + 1) % nmoves) {
    const auto tile_diff = play_game(nodes[idx].board(), &opp, &myp); // flip players
    if (tile_diff > 0) {  // Record winner, if any:
      d_wins[idx]++;
    } else if (tile_diff < 0) {
      l_wins[idx]++;
    }
#ifdef BENCHMARK
    plays++;
#endif
  }

  // Update final list of wins (thread-safe)
  std::unique_lock guard(mut);
  for (idx = 0; idx < nmoves; ++idx) {
    nodes[idx].count_wins(d_wins[idx], l_wins[idx]);
  }
#ifdef BENCHMARK
  total_plays_ += plays;
  total_moves_ += plays * nodes[0].board().moves_left();
#endif
}

////////////////////////////////////////////////////////////////////////////////
// get_move: compute the move with the highest aggregate probability of winning.
// The main loop runs until the (external) stop condition is met. During this
// time, it picks a random move from the available legal moves, and recursively
// "makes" the move and continues with random moves of alternating players until
// the game is over, recording who won. After the loop, the move that produced
// the most wins is selected.
// This version is multithreaded: it requires that StopCondition be thread-safe.
bits_t
MCTSPlayer::get_move(Board board, bits_t moves) const
{
  stop_->reset();

  auto nodes = compute_nodes(board, moves);
  assert(nodes.size() >= bits_set(moves));

  for (unsigned t = 0; t < nthread_; t++) {
    pool_.push_task([&](){ simulate_games(nodes); });
  }

  pool_.wait_for_tasks();
  return highest_win_odds(nodes.cbegin(), nodes.cend());
}

} // namespace
