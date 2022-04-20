/*
 * AI implementation of a computer player based on Monte-Carlo tree search
 */

#include "bits.hh"
#include "mcts_player.hh"
#include "moves.hh"
#include "random_player.hh"

#include <algorithm>
#include <cstdlib>
#include <mutex>
#include <thread>
#include <vector>

namespace Othello {

////////////////////////////////////////////////////////////////////////////////
// Checks the envrionment variable NTHREAD to set how many threads to use.
// If it's not defined, just uses all available hardware threads.
MCTSPlayer::MCTSPlayer(Color color, stop_ptr_t stop)
: Player(color),
  stop_(stop),
  nthread_(std::thread::hardware_concurrency())
{
  if (auto thread_str = getenv("NTHREAD")) {
    nthread_ = atoi(thread_str);
  }
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
      nodes.push_back(std::make_pair(pos, MCTSNode(newb, opponent_of(color_))));
    }
  }
  return nodes;
}

////////////////////////////////////////////////////////////////////////////////
// Given a list of nodes (moves and win records), pick the move that had the
// most wins relative to games played from that node.
bits_t
MCTSPlayer::highest_win_odds(const nodes_t& nodes) const
{
  assert(nodes.size() > 0);

  const auto best = std::max_element(nodes.cbegin(), nodes.cend(),
      [&](const auto& n1, const auto& n2) {
        return n1.second.win_odds(color_) < n2.second.win_odds(color_);
      });

  assert(best->second.win_odds(color_) >= 0);
  return best->first;
}

////////////////////////////////////////////////////////////////////////////////
void
MCTSPlayer::simulate_games(nodes_t& nodes) const
{
  std::vector<std::mutex> mutexes(nodes.size());

  player_ptr_t myp(std::shared_ptr<Player>(new RandomPlayer(color_)));
  player_ptr_t opp(std::shared_ptr<Player>(new RandomPlayer(opponent_of(color_))));

  StopCondition& stop = *stop_;
  unsigned idx = rand();  // Round-robin index into nodes

  while (!stop()) {
    idx = (idx + 1) % nodes.size();
    auto& node = nodes[idx].second;

    // After making the move in node, we flip players, then play the whole game:
    const auto tile_diff = play_game(node.board(), opp, myp);

    if (tile_diff) {
      std::scoped_lock guard(mutexes[idx]);
      node.mark_win(tile_diff > 0? Color::BLACK : Color::WHITE);
    }
  }
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
  auto nodes = compute_nodes(board, moves);
  assert(bits_set(moves) == nodes.size());
  assert(nodes.size() > 0);


  std::vector<std::thread> threads;

  stop_->reset();
  for (unsigned t = 0; t < nthread_; t++) {
    threads.push_back(std::thread([&](){ simulate_games(nodes); }));
  }

  for (unsigned t = 0; t < nthread_; t++) {
    threads[t].join();
  }

  return highest_win_odds(nodes);
}

} // namespace
