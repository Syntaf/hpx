//  Copyright (c) 2015 Agustin Berge
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This demonstrates the issue as reported by #1523: Remote async with deferred
// launch policy never executes

#include <hpx/hpx_main.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/async.hpp>
#include <hpx/util/lightweight_test.hpp>

bool null_action_executed = false;

void null_thread()
{
    null_action_executed = true;
}
HPX_PLAIN_ACTION(null_thread, null_action);

int main()
{
    hpx::async<null_action>(hpx::launch::deferred, hpx::find_here()).get();

    HPX_TEST(null_action_executed);
    return 0;
}
