#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

const size_t SZ=1e5;

// let's find prefix sum for it
void do_something(int idx, int val) {
  cout << "Doing something\n";
  static vector<int> vec(SZ + 1, 0);
  if (!idx) vec[idx] = val;
  else vec[idx] = vec[idx - 1] + val;
}

struct func {
  int &i;
  func(int &_i): i(_i) {}

  void operator()() {
    for (size_t j = 0; j < SZ; j++)
      do_something(j, this->i);  // potential dangling reference in use

  }

};

int main(void) {
  int local_state = 0;
  func my_func(local_state);
  
  thread t(my_func);
  // adding sleep here just so that we can see the output from other thread in
  // action and when the detach happen can see the SEGFAULT
  this_thread::sleep_for(chrono::milliseconds(10));
  t.detach();

  // NOTE: reference to a local variable is passed and then thread is detached.
  // As this local variable gets out of scope, what do you think will happen ?
  // Booboo.., there will be no compile-time error like in Rust here :)
}
