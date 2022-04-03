/*
 * Unit tests for bits functionality
 */

#define CATCH_CONFIG_MAIN

#include "bits.hh"
#include "catch.hh"

using namespace Othello;

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "pos2bit stays in range", "[bits]" ) {
  REQUIRE(pos2bit(0, 0) == 0);
  REQUIRE(pos2bit(N-1, N-1) == N2 - 1);
  REQUIRE(pos2bit(N, 0) == N2);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "pos2bit always sets exactly one bit", "[bits]" ) {
  for (idx_t row = 0; row < N; ++row) {
    for (idx_t col = 0; col < N; ++col) {
      REQUIRE(bits_set(set(0, pos2bit(row, col))) == 1);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "DB_hi shifts right correctly", "[bits]" ) {
  DB_hi db(0x8000000000000000, 0);
  REQUIRE((db & 0xFFFFFFFFFFFFFFFFULL) != 0);
  REQUIRE((0xFFFFFFFFFFFFFFFFULL & db) != 0);
  REQUIRE(((db >> 1) & 0xFFFFFFFFFFFFFFFFULL) != 0);
  REQUIRE(((db >> (N2-1)) & 0xFFFFFFFFFFFFFFFFULL) != 0);
  REQUIRE((((db >> (N2-1)) << (N2-1)) & 0xFFFFFFFFFFFFFFFFULL) != 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "DB_hi shifts left correctly", "[bits]" ) {
  DB_hi db(0, 1ULL);
  REQUIRE((db & 0xFFFFFFFFFFFFFFFFULL) == 0);
  REQUIRE((0xFFFFFFFFFFFFFFFFULL & db) == 0);
  REQUIRE(((db << 1) & 0xFFFFFFFFFFFFFFFFULL) == 0);
  REQUIRE(((db << (N2-1)) & 0xFFFFFFFFFFFFFFFFULL) == 0);
  REQUIRE((((db << (N2-1)) >> (N2-1)) & 0xFFFFFFFFFFFFFFFFULL) == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "DB_lo shifts right correctly", "[bits]" ) {
  DB_lo db(0x8000000000000001ULL, 0);
  REQUIRE((db & 0xFFFFFFFFFFFFFFFFULL) == 0);
  REQUIRE((0xFFFFFFFFFFFFFFFFULL & db) == 0);
  REQUIRE(((db >> 1) & 0xFFFFFFFFFFFFFFFFULL) != 0);
  REQUIRE(((db >> (N2-1)) & 0xFFFFFFFFFFFFFFFFULL) != 0);
  REQUIRE((((db << (N2-1)) >> (N2-1)) & 0xFFFFFFFFFFFFFFFFULL) == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "DB_lo shifts left correctly", "[bits]" ) {
  DB_lo db(0, 1ULL);
  REQUIRE((db & 0xFFFFFFFFFFFFFFFFULL) != 0);
  REQUIRE((0xFFFFFFFFFFFFFFFFULL & db) != 0);
  REQUIRE(((db << 1) & 0xFFFFFFFFFFFFFFFFULL) != 0);
  REQUIRE(((db << (N2-1)) & 0xFFFFFFFFFFFFFFFFULL) != 0);
  REQUIRE((((db >> (N2-1)) << (N2-1)) & 0xFFFFFFFFFFFFFFFFULL) == 0);
}
