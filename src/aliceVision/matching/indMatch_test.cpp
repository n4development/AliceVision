// This file is part of the AliceVision project.
// Copyright (c) 2016 AliceVision contributors.
// Copyright (c) 2012 openMVG contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "aliceVision/matching/IndMatch.hpp"
#include "aliceVision/matching/io.hpp"

#define BOOST_TEST_MODULE IndMatch
#include <boost/test/included/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

using namespace aliceVision;
using namespace aliceVision::matching;
using namespace aliceVision::feature;

BOOST_AUTO_TEST_CASE(IndMatch_IO)
{
  {
    std::set<IndexT> viewsKeys;
    PairwiseMatches matches;

    // Test save + load of empty data
    BOOST_CHECK(Save(matches, ".", "test_txt", "txt", false));
    BOOST_CHECK(Load(matches, viewsKeys, {"."}, {},"test_txt"));
    BOOST_CHECK_EQUAL(0, matches.size());
  }
  {
    std::set<IndexT> viewsKeys;
    PairwiseMatches matches;

    // Test save + load of empty data
    BOOST_CHECK(Save(matches, ".", "test_txt_2", "txt", true));
    BOOST_CHECK(!Load(matches, viewsKeys, {"."}, {}, "test_text_2"));
    BOOST_CHECK_EQUAL(0, matches.size());
  }
  {
    std::set<IndexT> viewsKeys = {0, 1, 2};
    PairwiseMatches matches;
    // Test export with not empty data
    matches[std::make_pair(0,1)][EImageDescriberType::UNKNOWN] = {{0,0},{1,1}};
    matches[std::make_pair(1,2)][EImageDescriberType::UNKNOWN] = {{0,0},{1,1}, {2,2}};

    BOOST_CHECK(Save(matches, ".", "test_txt_3", "txt", false));
    matches.clear();
    BOOST_CHECK(Load(matches, viewsKeys, {"."}, {EImageDescriberType::UNKNOWN}, "test_txt_3"));
    BOOST_CHECK_EQUAL(2, matches.size());
    BOOST_CHECK_EQUAL(1, matches.count(std::make_pair(0,1)));
    BOOST_CHECK_EQUAL(1, matches.count(std::make_pair(1,2)));
    BOOST_CHECK_EQUAL(2, matches.at(std::make_pair(0,1)).at(EImageDescriberType::UNKNOWN).size());
    BOOST_CHECK_EQUAL(3, matches.at(std::make_pair(1,2)).at(EImageDescriberType::UNKNOWN).size());
  }
  {
    std::set<IndexT> viewsKeys = {0, 1, 2};
    PairwiseMatches matches;
    // Test export with not empty data
    matches[std::make_pair(0,1)][EImageDescriberType::UNKNOWN] = {{0,0},{1,1}};
    matches[std::make_pair(1,2)][EImageDescriberType::UNKNOWN] = {{0,0},{1,1}, {2,2}};

    BOOST_CHECK(Save(matches, ".", "test_txt_4", "txt", true));
    BOOST_CHECK(Load(matches, viewsKeys, {"."}, {EImageDescriberType::UNKNOWN}, "test_txt_4"));
    BOOST_CHECK_EQUAL(2, matches.size());
    BOOST_CHECK_EQUAL(1, matches.count(std::make_pair(0,1)));
    BOOST_CHECK_EQUAL(1, matches.count(std::make_pair(1,2)));
    BOOST_CHECK_EQUAL(2, matches.at(std::make_pair(0,1)).at(EImageDescriberType::UNKNOWN).size());
    BOOST_CHECK_EQUAL(3, matches.at(std::make_pair(1,2)).at(EImageDescriberType::UNKNOWN).size());
  }
}

BOOST_AUTO_TEST_CASE(IndMatch_DuplicateRemoval_NoRemoval)
{
  std::vector<IndMatch> vec_indMatch;

  vec_indMatch.push_back(IndMatch(2,3)); // 0
  vec_indMatch.push_back(IndMatch(0,1)); // 1

  // Check no removal
  BOOST_CHECK(!IndMatch::getDeduplicated(vec_indMatch));

  // Check lexigraphical sorting
  // Due to lexigraphical sorting (0,1) must appears first
  BOOST_CHECK_EQUAL(IndMatch(0,1), vec_indMatch[0]);
  BOOST_CHECK_EQUAL(IndMatch(2,3), vec_indMatch[1]);
}

BOOST_AUTO_TEST_CASE(IndMatch_DuplicateRemoval_Simple)
{
  std::vector<IndMatch> vec_indMatch;

  vec_indMatch.push_back(IndMatch(0,1)); // 0
  vec_indMatch.push_back(IndMatch(0,1)); // 1: error with addition 0

  vec_indMatch.push_back(IndMatch(1,2)); // 2
  vec_indMatch.push_back(IndMatch(1,2)); // 3: error with addition 2

  BOOST_CHECK(IndMatch::getDeduplicated(vec_indMatch));
  // Two matches must remain (line 0 and 2)
  BOOST_CHECK_EQUAL(2, vec_indMatch.size());
}

BOOST_AUTO_TEST_CASE(IndMatch_DuplicateRemoval)
{
  std::vector<IndMatch> vec_indMatch;

  vec_indMatch.push_back(IndMatch(0,1));
  vec_indMatch.push_back(IndMatch(0,1)); // Error defined before

  // Add some other matches
  vec_indMatch.push_back(IndMatch(0,2));
  vec_indMatch.push_back(IndMatch(1,1));
  vec_indMatch.push_back(IndMatch(2,3));
  vec_indMatch.push_back(IndMatch(3,3));

  BOOST_CHECK(IndMatch::getDeduplicated(vec_indMatch));
  // Five matches must remain (one (0,1) must disappear)
  BOOST_CHECK_EQUAL(5, vec_indMatch.size());

  BOOST_CHECK_EQUAL(IndMatch(0,1), vec_indMatch[0]);
  BOOST_CHECK_EQUAL(IndMatch(0,2), vec_indMatch[1]);
  BOOST_CHECK_EQUAL(IndMatch(1,1), vec_indMatch[2]);
  BOOST_CHECK_EQUAL(IndMatch(2,3), vec_indMatch[3]);
  BOOST_CHECK_EQUAL(IndMatch(3,3), vec_indMatch[4]);
}
