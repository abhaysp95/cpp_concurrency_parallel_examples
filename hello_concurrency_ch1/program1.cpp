#include <iostream>
#include <thread>

using namespace std;

void hello() {
  cout << "Concurrent Hello\n";
}

int main(void) {
  thread t(hello);
  t.join();

  return 0;
}
