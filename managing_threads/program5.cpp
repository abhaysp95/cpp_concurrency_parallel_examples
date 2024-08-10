#include <chrono>
#include <iostream>
#include <thread>
using namespace std;

class X {
public:
  void do_length_work(int x) {
    // NOTE: I noticed that it's giving output after it comes from sleep.
    // The reason was that I didn't had newline char at the end.
    // I learnt that stdout is line-buffered that it means it won't show output
    // until it encounters a newline char (or CRLF) or if buffer is flushed.
    std::cout << "Going on sleep for " << x << " seconds\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(x * 1000));
  }
};


int main(void) {
  X my_x;

  // Demonstrating on how the operation of std::thread ctor is same as of
  // std::bind
  std::thread t(&X::do_length_work, &my_x, 2);
  t.join();

  return 0;
}
