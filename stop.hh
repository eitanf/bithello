/*
 * Abstract base class and several implementations for a function object that
 * determines when the search for moves should stop.
 * Only two operations are supported: resetting for a new move, and checking for
 * the stopping condition (after every move search).
 * All of these classes need to be thread-safe.
 */

#pragma once

#include <atomic>
#include <chrono>
#include <memory>

#ifdef BENCHMARK
#  include "player.hh"
#endif

namespace Othello {

struct StopCondition {
  virtual void reset() = 0;            // Start a brand-new search
  virtual bool operator()() = 0;       // Should MC search stop now?
  virtual ~StopCondition() = default;
};

/////////////////////////////////////
// This class stops search after a given set of moves
class StopByMoves : public StopCondition {
  std::atomic<uint64_t> count_ = 0;
  const uint64_t max_moves_;

 public:
  StopByMoves(uint64_t max_moves = 1000) : max_moves_(max_moves) {}
  virtual ~StopByMoves() = default;
  virtual void reset() { count_ = 0; }
  virtual bool operator()() { return ++count_ >= max_moves_; }
};


using stop_ptr_t = std::shared_ptr<StopCondition>;

/////////////////////////////////////
// This class stops search after a given duration in milliseconds
class StopByDuration : public StopCondition {
  std::chrono::steady_clock::time_point begin_;
  const long duration_;
#ifdef BENCHMARK
  Color color_;
  std::atomic<uint64_t> evals_;
#endif

 public:
  StopByDuration(uint64_t duration = 1000) : duration_(duration) {}
#ifdef BENCHMARK
  StopByDuration(Color color, uint64_t duration = 1000)
    : duration_(duration), color_(color), evals_(0) {}
#endif
  virtual ~StopByDuration();
  virtual void reset();
  virtual bool operator()();
};


} // namespace
