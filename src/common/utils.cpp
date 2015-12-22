//
// Written by Andrea Iob <andrea_iob@hotmail.com>
//

#define __BITP_MESH_UTILS_SRC__

#include "utils.hpp"
#include "utils.tpp"

namespace utils {

template bool add_to_ordered_vector<>(const long&, std::vector<long>&, std::less<long>);
template bool add_to_ordered_vector<>(const unsigned long&, std::vector<unsigned long>&, std::less<unsigned long>);

/*!
	Evaluates the cross product of two three-dimensional arrays.

	\param x is the first a three-dimensional array
	\param y is the second a three-dimensional array
	\param cross is the array used to store the cross product
*/
std::array<double, 3> cross_3D(std::array<double, 3> &x, std::array<double, 3> &y)
{
	std::array<double, 3> cross;

	cross[0] = x[1] * y[2] - x[2] * y[1];
	cross[1] = x[2] * y[0] - x[0] * y[2];
	cross[2] = x[0] * y[1] - x[1] * y[0];

	return cross;
}

/*!
	Normalizes a three-dimensional array.

	\param x is the three-dimensional array to be normalized
*/
void normalize_3D(std::array<double, 3> &x)
{
	double module = 0.;
	for (int k = 0; k < 3; ++k) {
		module += x[k] * x[k];
	}
	module = pow(module, 0.5);

	for (int k = 0; k < 3; ++k) {
		x[k] /= module;
	}
}

/*!
	Evaluates the transpose of a 3x3 matrix.

	\param A is the 3x3 matrix to be transposed
*/
void transpose_3D(std::array<std::array<double, 3>, 3> &A)
{
	for (int i = 0; i < 2; i++) {
		for (int j = i; j < 3; j++) {
			double tmp = A[i][j];
			A[i][j] = A[j][i];
			A[j][i] = tmp;
		}
	}
}

}
