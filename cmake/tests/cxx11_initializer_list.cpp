////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2008 Beman Dawes
//  Copyright (c) 2015 Agustin Berge
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////

#include <initializer_list>

void test_aggregate_initializer_list() {
  int ail[] = {1, 2, 3};
}

struct ctor {
  ctor(std::initializer_list<int>) {}
};

void test_ctor_initializer_list() {
  ctor cil{1, 2, 3};
}

int main() {
  test_aggregate_initializer_list();
  test_ctor_initializer_list();
}
