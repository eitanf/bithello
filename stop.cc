/*
 * Implementations for Stop classes
 */

#include "stop.hh"

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
    const auto done = now - begin_ >= duration_;
  return done;
}

StopByDuration::~StopByDuration()
{
}

} // namespace
