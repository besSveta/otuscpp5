#include "matrix_lib.h"
int main(int, char *[]) {

	Matrix<int, 0> mtrx;
	auto n = 10;

	for (auto i = 0; i < n; i++) {
	 mtrx[i][i] = i;
	 }

	 for (auto i = n - 1; i >= 0; i--) {
	 mtrx[i][n - i - 1] = i;
	 }
	 for (auto i = 1; i < 9; i++) {
	 for (auto j = 1; j < 9; j++) {
	 std::cout << mtrx[i][j] << " ";
	 }
	 std::cout << std::endl;
	 }
	 int x;
	 int y;
	 int v;

	 for (auto c : mtrx) {
	 std::tie(x, y, v) = c;
	 std::cout << "["<<x <<","<< y <<"]:"<< v << std::endl;
	 }

	 std::cout<<mtrx.size()<<std::endl;

	auto a = 125;
	auto b = 456;
	Matrix<int, 0> m;
	m[1][5] = 123;

	std::cout << "size = " << m.size() << std::endl;
	std::cout << m[1][5] << std::endl;
	std::cout << m[5][5] << std::endl;
	m[1][5] =a;
	std::cout << "size = " << m.size() << std::endl;
	std::cout << m[1][5] << std::endl;
	std::cout << m[5][5] << std::endl;
	m[5][5] = b;
	std::cout << "size = " << m.size() << std::endl;
	std::cout << m[1][5] << std::endl;
	std::cout << m[5][5] << std::endl;
	return 0;
}
