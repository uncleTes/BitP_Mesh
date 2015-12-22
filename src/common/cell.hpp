//
// Written by Andrea Iob <andrea_iob@hotmail.com>
//
#ifndef __BITP_MESH_CELL_HPP__
#define __BITP_MESH_CELL_HPP__

/*! \file */

#include "collapsedVector2D.hpp"
#include "element.hpp"

#include <memory>

/*!
	\ingroup Common
	@{
*/

class Cell;

ibinarystream& operator>>(ibinarystream &buf, Cell& cell);
obinarystream& operator<<(obinarystream &buf, const Cell& cell);

class Cell : public Element {

friend obinarystream& operator<<(obinarystream& buf, const Cell& cell);
friend ibinarystream& operator>>(ibinarystream& buf, Cell& cell);

public:
	Cell();
	Cell(const long &id, ElementInfo::Type type = ElementInfo::UNDEFINED);

	Cell(Cell&& other) = default;
	Cell& operator=(Cell&& other) = default;

	Cell(const Cell &other);
	Cell& operator = (const Cell &other);

	void initialize(ElementInfo::Type type, int nInterfacesPerFace = 0);

	void set_interior(bool interior);
	bool is_interior() const;
	
	void initialize_interfaces(std::vector<std::vector<long>> &interfaces);
	void initialize_empty_interfaces(const std::vector<int> interfaceCount);
	void set_interface(const int &face, const int &index, const long &interface);
	void push_interface(const int &face, const long &interface);
	void delete_interface(const int &face, const int &i);
	void unset_interfaces();
	int get_interface_count() const;
	int get_interface_count(const int &face) const;
	long get_interface(const int &face, const int &index = 0) const;
	const long * get_interfaces() const;
	const long * get_interfaces(const int &face) const;

	void display(std::ostream &out, unsigned short int indent);

	unsigned int get_binary_size( );

protected:

private:
	bool m_interior;

	CollapsedVector2D<long> m_interfaces;

};

/*!
	@}
*/

#endif
