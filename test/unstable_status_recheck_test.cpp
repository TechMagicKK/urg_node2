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

#include <chrono>

#include "gtest/gtest.h"
#include "urg_node2/urg_node2.hpp"

using urg_node2::should_recheck_unstable_status;
using urg_node2::kUnstableStatusRecheckInterval;
using namespace std::chrono_literals;

// 計測開始時に異常（初期化中）だったら、スキャンが届き続けても間隔経過後に読み直す
TEST(UnstableStatusRecheck, unstable_rechecks_after_interval)
{
  EXPECT_TRUE(should_recheck_unstable_status(true, false, 5s, 5s));
  EXPECT_TRUE(should_recheck_unstable_status(true, false, 1h, 5s));
}

// 間隔に満たない間は読み直さない（計測の止め直しを連発しない）
TEST(UnstableStatusRecheck, unstable_waits_for_interval)
{
  EXPECT_FALSE(should_recheck_unstable_status(true, false, 0s, 5s));
  EXPECT_FALSE(should_recheck_unstable_status(true, false, 4999ms, 5s));
}

// スキャン取得に失敗している間は止めない（止め直しで連続失敗回数が 0 に戻り、再接続に届かなくなるため）
TEST(UnstableStatusRecheck, scan_failure_never_rechecks)
{
  EXPECT_FALSE(should_recheck_unstable_status(false, false, 5s, 5s));
  EXPECT_FALSE(should_recheck_unstable_status(false, false, 1h, 5s));
}

// 正常なら経過時間によらず計測を止めない
TEST(UnstableStatusRecheck, stable_never_rechecks)
{
  EXPECT_FALSE(should_recheck_unstable_status(true, true, 0s, 5s));
  EXPECT_FALSE(should_recheck_unstable_status(true, true, 1h, 5s));
}

// 回復後は数秒で異常表示が消え、かつ計測の止め直しを連発しない間隔であること
TEST(UnstableStatusRecheck, interval_is_within_bounds)
{
  EXPECT_GE(kUnstableStatusRecheckInterval, 1s);
  EXPECT_LT(kUnstableStatusRecheckInterval, 10s);
}
