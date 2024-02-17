
#include <algorithm>
#include <bits/chrono.h>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <execution>
#include <mutex>
#include <numeric>
#include <pstl/glue_execution_defs.h>
#include <random>
#include <ratio>
#include <string_view>
#include <type_traits>
#include <vector>

#define all(x) x.begin(), x.end()

using namespace std;

const size_t TESTSIZE = 1'000'000;
const size_t ITERCOUNT = 5;


template <typename T>
void print_vec(const vector<T>& vec) {
	for (auto x: vec) cout << x << " ";
	cout << endl;
}

template <typename T>
void print_results(const string_view tag, const vector<T> &vec,
		chrono::high_resolution_clock::time_point startTime,
		chrono::high_resolution_clock::time_point endTime) {
	printf("%.*s: Lowest: %u Highest: %u Time: %fms\n",
			static_cast<int>(tag.size()), tag.data(), vec.front(), vec.back(),
			chrono::duration_cast<chrono::duration<double, milli>>(endTime - startTime).count());
}

template <typename T>
void print_results(const string_view tag, const T result,
		chrono::high_resolution_clock::time_point startTime,
		chrono::high_resolution_clock::time_point endTime) {
	printf("%.*s: Result: %u Time: %fms\n",
			static_cast<int>(tag.size()), tag.data(), result,
			chrono::duration_cast<chrono::duration<double, milli>>(endTime - startTime).count());
}

template <typename T>
void time_sort_serial(vector<T>& vec) {
	const auto startTime = chrono::high_resolution_clock::now();
	sort(all(vec));
	const auto endTime = chrono::high_resolution_clock::now();
	print_results("Sort [serial]", vec, startTime, endTime);
}

template <typename T>
void time_sort_parallel(vector<T>& vec) {
	const auto startTime = chrono::high_resolution_clock::now();
	sort(execution::seq, all(vec));
	const auto endTime = chrono::high_resolution_clock::now();
	print_results("Sort [parallel]", vec, startTime, endTime);
}

template <typename T>
uint64_t add_numbers_serial(const vector<T>& vec) {
	const auto startTime = chrono::high_resolution_clock::now();
	static_assert(is_unsigned<T>::value, "T must be an unsigned type");
	const auto endTime = chrono::high_resolution_clock::now();
	T res = 0;
	for (auto x: vec) res += x;
	print_results("Add [serial]", res, startTime, endTime);
	return res;
}

template <typename T>
uint64_t add_numbers_parallel_incorrect(const vector<T>& vec) {
	const auto startTime = chrono::high_resolution_clock::now();
	static_assert(is_unsigned<T>::value, "T must be an unsigned type");
	const auto endTime = chrono::high_resolution_clock::now();
	T res = 0;
	// inviting race condition
	std::for_each(std::execution::par_unseq, all(vec), [&res](T x) { res += x; });
	print_results("Add [parallel (incorrect)]", res, startTime, endTime);
	return res;
}

template <typename T>
uint64_t add_numbers_parallel(const vector<T>& vec) {
	const auto startTime = chrono::high_resolution_clock::now();
	static_assert(is_unsigned<T>::value, "T must be an unsigned type");
	const auto endTime = chrono::high_resolution_clock::now();
	T res = 0;
	mutex mtx;
	// inherently worse than sequential counterpart
	std::for_each(std::execution::par_unseq, all(vec), [&res, &mtx](T x) {
			scoped_lock lock(mtx);
			res += x;
			});
	print_results("Add [parallel (correct)]", res, startTime, endTime);
	return res;
}

template <typename T>
uint64_t reduce_numbrs_sequential(const vector<T>& vec) {
	const auto startTime = chrono::high_resolution_clock::now();
	static_assert(is_unsigned<T>::value, "T must be an unsigned type");
	const auto endTime = chrono::high_resolution_clock::now();
	T res = reduce(all(vec), T(0));
	print_results("Reduce [sequential]", res, startTime, endTime);
	return res;
}

template <typename T>
uint64_t reduce_numbrs_parallel(const vector<T>& vec) {
	const auto startTime = chrono::high_resolution_clock::now();
	static_assert(is_unsigned<T>::value, "T must be an unsigned type");
	const auto endTime = chrono::high_resolution_clock::now();
	T res = reduce(execution::par_unseq, all(vec), T(0));  // std::plus is default BinaryOp
	print_results("Reduce [parallel]", res, startTime, endTime);
	return res;
}

int main(void) {

	random_device rd;
	printf("Test size: %zu unsigned ints...\n", TESTSIZE);

	vector<uint32_t> vec(TESTSIZE);
	for (auto& x: vec) {
		x = static_cast<uint32_t>(rd());
	}

	/* for (size_t i = 0; i < ITERCOUNT; i++) {
		vector<uint32_t> temp(vec);
		time_sort_serial(temp);
	}

	for (size_t i = 0; i < ITERCOUNT; i++) {
		vector<uint32_t> temp(vec);
		time_sort_parallel(temp);
	} */

	for (size_t i = 0; i < ITERCOUNT; i++) {
		add_numbers_serial(vec);
	}

	for (size_t i = 0; i < ITERCOUNT; i++) {
		add_numbers_parallel_incorrect(vec);
	}

	for (size_t i = 0; i < ITERCOUNT; i++) {
		add_numbers_parallel(vec);
	}

	for (size_t i = 0; i < ITERCOUNT; i++) {
		reduce_numbrs_sequential(vec);
	}

	for (size_t i = 0; i < ITERCOUNT; i++) {
		reduce_numbrs_parallel(vec);
	}

	return 0;
}
