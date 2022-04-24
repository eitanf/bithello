/*
 * Implementations for Stop classes
 */

#include "stop.hh"
#include <iostream>

using namespace std::chrono;

namespace Othello {

void
StopByDuration::reset()
{
  begin_ = steady_clock::now();
}

///////////////////////////////////////////////////////
bool
StopByDuration::operator()()
{
    const auto now = steady_clock::now();
    const auto done = duration_cast<milliseconds>(now - begin_).count() >= duration_;
#ifdef BENCHMARK
    evals_++;
#endif
  return done;
}

StopByDuration::~StopByDuration()
{
#ifdef BENCHMARK
  std::clog << "Evaluated a total of " << evals_ <<
    " partial games (color: " << int(color_) << ")\n";
#endif
}

} // namespace
