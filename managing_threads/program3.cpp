#include <cstdio>
#include <iostream>
#include <thread>

void f(int i, std::string const& s) {
  std::cout << s << " gave me " << i << '\n';
}

void oops(char const* param) {
  char buffer[1024];
  sprintf(buffer, "%s", param);

  // explicit casting of pointer to std::string to avoid dangling pointer
  std::thread t(f, 3, std::string(buffer));
  t.detach();
}

int main(void) {
  oops("Who");

  return 0;
}

// But what if f() demands for non-const reference, will this method work ?
