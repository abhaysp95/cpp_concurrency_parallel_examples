
#include <chrono>
#include <iostream>
#include <optional>
#include <thread>

void do_something(std::optional<int> i) {
  std::cout << std::this_thread::get_id() << " going on sleep for "
            << i.value_or(1) << " seconds\n";
  std::this_thread::sleep_for(std::chrono::seconds(i.value_or(1)));
  std::cout << "Out of sleep " << std::this_thread::get_id() << '\n';
}

std::thread f() {
  return std::thread(do_something, NULL);
}

std::thread g() {
  std::thread t(do_something, 3);
  std::cout << "Thread on g created\n";
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  return t;
}

int main(void) {
  f().join();
  std::thread gt = g();
  gt.join();

  return 0;
}
