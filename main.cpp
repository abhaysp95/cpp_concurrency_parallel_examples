
#include <algorithm>
#include <bits/chrono.h>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <iostream>
#include <execution>
#include <iterator>
#include <mutex>
#include <numeric>
#include <ostream>
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
	uint64_t res = 0;
	for (auto x: vec) res += x;
	print_results("Add [serial]", res, startTime, endTime);
	return res;
}

template <typename T>
uint64_t add_numbers_parallel_incorrect(const vector<T>& vec) {
	const auto startTime = chrono::high_resolution_clock::now();
	static_assert(is_unsigned<T>::value, "T must be an unsigned type");
	const auto endTime = chrono::high_resolution_clock::now();
	uint64_t res = 0;
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
	uint64_t res = 0;
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
	uint64_t res = reduce(all(vec), T(0));
	print_results("Reduce [sequential]", res, startTime, endTime);
	return res;
}

template <typename T>
uint64_t reduce_numbrs_parallel(const vector<T>& vec) {
	const auto startTime = chrono::high_resolution_clock::now();
	static_assert(is_unsigned<T>::value, "T must be an unsigned type");
	const auto endTime = chrono::high_resolution_clock::now();
	uint64_t res = reduce(execution::par_unseq, all(vec), T(0));  // std::plus is default BinaryOp
	print_results("Reduce [parallel]", res, startTime, endTime);
	return res;
}

/** test std::inclusive_scan and std::partial_sum */
void partial_sum_and_inclusive_scan() {
	vector<int> vec = {4, 5, 2, 3, 8, 7, 9};
	vector<int> temp(vec);

	// useful for prefix sum algos
	// you can provide op and init as extra params too
	partial_sum(all(temp), ostream_iterator<int>(cout, " "));
	cout << endl;

	// inclusive scan is unordered partial sum (use it for cummulative and
	// associative types only)
	// you can provide op and init as extra params too
	// inclusive_scan (applies op with unspecified grouping) will give
	// non-deterministic results for non-associtive ops, but is fine
	// non-commutative ops
	temp.clear();
	temp.insert(temp.begin(), all(vec));
	inclusive_scan(execution::par_unseq, all(temp), ostream_iterator<int>(cout, " "));

	// NOTE: there's also exclusive scan which will exclude ith element in op (sum) up to ith element
}

/** Fused algorithms:
 * transform_reduce
 * transform_inclusive_scan
 * transform_exclusive_scan
 *
 * the rules for cummulative & associative ops also apply to these fused algos
 * just like std::transform, there's binary version of std::transform_reduce also
 */

/** vector norm: sqrt. of the sum of the square of each element */
void calculate_vector_norm() {
	vector<int> vec = {4, 5, 2, 3, 8, 7, 9};

	double norm = sqrt(transform_reduce(
				execution::par_unseq, all(vec), double(0.0),
				[](double a, double b) { return a + b; },
				[](int x) { return x * x; }));
	cout << norm << endl;
}

void calculate_dot_product() {
	vector<int> vec1 = {4, 5, 2, 3, 8, 7, 9};
	vector<int> vec2(vec1);
	reverse(all(vec2));
	copy(all(vec1), ostream_iterator<int>(cout, " "));
	cout << '\n';
	copy(all(vec2), ostream_iterator<int>(cout, " "));
	cout << '\n';

	long long dprod = transform_reduce(execution::par_unseq, all(vec1), vec2.begin(),
			0L, plus<long>(), multiplies<int>());
	cout << dprod << endl;
}

int main(void) {

	random_device rd;
	printf("Test size: %zu unsigned ints...\n", TESTSIZE);

	/* vector<uint32_t> vec(TESTSIZE);
	for (auto& x: vec) {
		x = static_cast<uint32_t>(rd());
	} */

	/* for (size_t i = 0; i < ITERCOUNT; i++) {
		vector<uint32_t> temp(vec);
		time_sort_serial(temp);
	}

	for (size_t i = 0; i < ITERCOUNT; i++) {
		vector<uint32_t> temp(vec);
		time_sort_parallel(temp);
	} */

	/* for (size_t i = 0; i < ITERCOUNT; i++) {
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
	} */

	// partial_sum_and_inclusive_scan();

	calculate_vector_norm();

	calculate_dot_product();

	return 0;
}
