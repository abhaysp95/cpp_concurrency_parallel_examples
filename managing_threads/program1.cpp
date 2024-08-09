#include <iostream>
#include <thread>
using namespace std;

class BackgroundTask {
public:
  void operator()() const {
    cout << "Who are you ?\n";
  }
};

int main(void) {
  thread mt((BackgroundTask()));

  mt.join();
  return 0;
}
