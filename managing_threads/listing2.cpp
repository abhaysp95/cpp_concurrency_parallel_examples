#include <chrono>
#include <exception>
#include <iostream>
#include <string_view>
#include <thread>

using namespace std;

const size_t SZ=10;

void do_something(int i) {
  cout << "Doing something\n";
  this_thread::sleep_for(chrono::milliseconds(i));
}

struct func {
  int &i;  // dangling reference is not a concern for this listing (refer listing1)
  func(int &_i): i(_i) {}

  void operator()() {
    for (size_t j = 0; j < SZ; j++)
      do_something(j);  

  }

};

class MyException: public std::exception {
  string _msg;
public:
  explicit MyException(string_view msg): _msg(msg) {};
  const char * what() const noexcept override {
    return this->_msg.c_str();
  }
};

// lets throw exception
void doing_something_in_main_thread() {
  this_thread::sleep_for(chrono::milliseconds(10));
  throw MyException("This looks bad. Main in trouble!!!\n");
}

int main(void) {
  int local_state = 0;
  func my_func(local_state);

  thread t(my_func);
  try {
    doing_something_in_main_thread();
  } catch (...) {
    cout << "Got exception. Joining thread\n";
    t.join();
    cout << "Thread joined. Joinable: " << (t.joinable() ? "True" : "False") << '\n';
    throw;
  }
  t.join();
}

// NOTE: Once concern is that unlike this simple program where there is one way
// to exit, with using try-catch you have to join threads on all the other ways
// in some complex program where there are many ways to exit.
