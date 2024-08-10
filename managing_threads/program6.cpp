
#include <algorithm>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
class BigObject {
  int* _data;
  int _size;
public:
  explicit BigObject(int x): _data(new int[x]), _size(x) {}
  void prepare_data(std::vector<int>&& data) {
    std::copy(data.begin(), data.begin()+this->_size, this->_data);
  }
  int get_size() const {
    return this->_size;
  }
};

void process_big_object(std::unique_ptr<BigObject> obj) {
  // going to do something with this object...
  std::cout << "Size of array: " << obj->get_size() << '\n';
}

int main(void) {
  std::unique_ptr<BigObject> p(new BigObject(3));
  p->prepare_data(std::vector<int>{1, 2, 3});

  // unique_ptr doesn't provides copy ctor and thus if not explicitly moved will
  // cause the problem
  std::thread t(process_big_object, std::move(p));
  t.join();

  return 0;
}
