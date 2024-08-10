#include <cstdio>
#include <iostream>
#include <thread>

// now f() wants a non-const refernce to s
void f(int i, std::string & s) {
  // std::this_thread::sleep_for(std::chrono::milliseconds(1));
  std::cout << s << " gave me " << i << '\n';
}

void oops(char const* param) {
  char buffer[1024];
  sprintf(buffer, "%s", param);

  // If you still pass a it like this, it is going to get copied in internal
  // storage of thread like this and then will be passed as rvalue to f()
  // But now f() wants a non-const ref. and so you are going to get error
  // Well, the error says "std::thread arguments must be invocable after
  // conversion to rvalues" for such scenarios.
  // Shouldn't the error be like "can't pass rvalue to non-const ref." or
  // something...?
  // Anyways, you have to do the binding
  std::string buf(buffer);  // just for dangling pointer prevention
  std::thread t(f, 3, std::ref(buf));
  t.detach();
}

int main(void) {
  oops("Who");

  return 0;
}

