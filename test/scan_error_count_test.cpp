// Copyright 2026 TechMagic K.K.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "gtest/gtest.h"
#include "urg_node2/urg_node2.hpp"

using urg_node2::update_scan_error_count;

// 1 回 2 秒かかる失敗（応答なしタイムアウト）が続いても、error_limit を超えた回で再接続になる。
// 判定は経過時間に依存しないため、失敗の間隔はテストでは表現しない
TEST(ScanErrorCount, slow_consecutive_failures_reach_reconnect)
{
  const int error_limit = 4;
  int error_count = 0;
  for (int i = 1; i <= error_limit; ++i) {
    EXPECT_FALSE(update_scan_error_count(false, error_limit, error_count)) << "failure " << i;
    EXPECT_EQ(error_count, i);
  }
  EXPECT_TRUE(update_scan_error_count(false, error_limit, error_count));
  EXPECT_EQ(error_count, error_limit + 1);
}

// 失敗の合間に 1 回でも取得に成功すれば回数は 0 に戻り、再接続しない
TEST(ScanErrorCount, success_resets_count)
{
  const int error_limit = 4;
  int error_count = 0;
  for (int i = 0; i < error_limit; ++i) {
    EXPECT_FALSE(update_scan_error_count(false, error_limit, error_count));
  }
  EXPECT_FALSE(update_scan_error_count(true, error_limit, error_count));
  EXPECT_EQ(error_count, 0);
  for (int i = 0; i < error_limit; ++i) {
    EXPECT_FALSE(update_scan_error_count(false, error_limit, error_count));
  }
  EXPECT_EQ(error_count, error_limit);
}

// 成功が続く間は回数が増えない
TEST(ScanErrorCount, successes_keep_count_zero)
{
  int error_count = 0;
  for (int i = 0; i < 10; ++i) {
    EXPECT_FALSE(update_scan_error_count(true, 4, error_count));
  }
  EXPECT_EQ(error_count, 0);
}
