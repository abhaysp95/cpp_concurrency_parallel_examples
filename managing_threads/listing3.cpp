#include <chrono>
#include <exception>
#include <iostream>
#include <string_view>
#include <thread>

using namespace std;

const size_t SZ=1e5;

void do_something() {
  cout << "Doing something\n";
}

class ThreadGuard {
  thread& _t;
public:
  explicit ThreadGuard(thread& t): _t(t) {
    cout << "ThreadGuard in use\n";
  }
  ~ThreadGuard() {
    if (_t.joinable()) {
      cout << "Joining thread!!!\n";
      _t.join();
    cout << "ThreadGuard Exiting\n";
    }
  }
  ThreadGuard(ThreadGuard const&) = delete;
  ThreadGuard& operator=(ThreadGuard const&) = delete;
};

struct func {
  int &i;  // dangling reference is not a concern for this listing (refer listing1)
  func(int &_i): i(_i) {}

  void operator()() {
    cout << "Thread started\n";
    for (size_t j = 0; j < SZ; j++)
      do_something();  
    cout << "Thread finished\n";
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
  this_thread::sleep_for(chrono::milliseconds(2));
  throw MyException("This looks bad. Main in trouble!!!\n");
}

void oops() {
  int local_state = 0;
  func my_func(local_state);

  thread t(my_func);
  ThreadGuard g(t);

  // this will throw exception. Will thread join ?
  doing_something_in_main_thread();
}

int main(void) {
  oops();
}

// NOTE: I'm not seeing the output from desctructor. Is behaviour somewhat
// different when exception is thrown vs. local variable getting out of scope
// and dtors getting called on stack unwinding then ?
