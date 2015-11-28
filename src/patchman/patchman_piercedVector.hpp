//
// Written by Andrea Iob <andrea_iob@hotmail.com>
//
#ifndef __PATCHMAN_PIERCED_VECTOR_HPP__
#define __PATCHMAN_PIERCED_VECTOR_HPP__

#include "cell.hpp"
#include "interface.hpp"
#include "node.hpp"

#include "piercedVector.tpp"

namespace pman {

extern template class PiercedVector<Cell>;
extern template class PiercedVector<Interface>;
extern template class PiercedVector<Node>;

}

#endif