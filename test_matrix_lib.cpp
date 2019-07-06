/*
 * test_matrix_lib.cpp
 *
 *  Created on: 25 июн. 2019 г.
 *      Author: sveta
 */

/*
 * test_ip_filter.cpp
 *
 *  Created on: 10 мая 2019 г.
 *      Author: sveta
 */
#define BOOST_TEST_MODULE ip_filter_test_module
#include <boost/test/included/unit_test.hpp>
#include "matrix_lib.h"

BOOST_AUTO_TEST_SUITE(matrix_test_suite)

BOOST_AUTO_TEST_CASE(matrix_test_case)
{
	Matrix<int, 0> mtrx;
	auto n = 10;

	for (auto i = 0; i < n; i++) {
		mtrx[i][i] = i;
	}
	BOOST_REQUIRE_EQUAL(mtrx.size(),n-1);
}

BOOST_AUTO_TEST_CASE(const_matrix_test_case)
{
	const Matrix<int, 5> mtrx;

	BOOST_REQUIRE_EQUAL(mtrx[5][2], 5);
}


BOOST_AUTO_TEST_SUITE_END()





