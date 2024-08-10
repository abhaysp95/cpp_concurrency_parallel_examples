#include <iostream>
#include <thread>

void f(int i, std::string const& s) {
  std::cout << s << " gave me " << i << '\n';
}

int main(void) {
  std::thread t(f, 3, "hello");
  // NOTE: this is fine because the literal is copied into internal storage of
  // thread. And since it is literal it isn't bind to the scope of this function
  // and then it is converted to string_view from char const* when the callable
  // object or function is called from this newly created thread of execution.
  t.detach();

  return 0;
}
