//
// Written by Andrea Iob <andrea_iob@hotmail.com>
//

#include "cell.hpp"
#include "interface.hpp"
#include "utils.hpp"

#include<iostream>

/*!
	\ingroup Common
	@{
*/

/*!
	\class Cell

	\brief The Cell class defines the cells.

	Cell is class that defines the cells.
*/

/*!
	Default constructor.
*/
Cell::Cell()
	: Element()
{

}

/*!
	Creates a new cell.
*/
Cell::Cell(const long &id, ElementInfo::Type type)
	: Element(id, type)
{

}

/*!
	Copy-constructor
*/
Cell::Cell(const Cell &other)
{
	*this = other;
}

/*!
	Copy assignament operator
*/
Cell& Cell::operator=(const Cell &other)
{
	Element::operator=(other);

	m_interior = other.m_interior;

	m_interfaces = other.m_interfaces;
}

/*!
	Initializes the data structures of the cell.

	\param type is the type of the element
	\param nInterfacesPerFace is the number of interfaces per face that
	will be used to initialize the interface's data structure. If this
	parameter is less or equal to zero, the interface's data structure
	will not be initialized.
*/
void Cell::initialize(ElementInfo::Type type, int nInterfacesPerFace)
{
	Element::initialize(type);

	if (type != ElementInfo::UNDEFINED && nInterfacesPerFace >= 1) {
		const ElementInfo &elementInfo = get_info();
		std::vector<int> interfaceCount(elementInfo.nFaces, nInterfacesPerFace);
		initialize_empty_interfaces(interfaceCount);
	}
}

/*!
	Sets if the cells belongs to the the interior domain.

	\param interior defines if the cells belongs to the the interior domain
*/
void Cell::set_interior(bool interior)
{
	m_interior = interior;
}

/*!
	Gets if the cells belongs to the the interior domain.

	\result Returns true if the cells belongs to the the interior domain,
	otherwise it returns false.
*/
bool Cell::is_interior() const
{
	return m_interior;
}

/*!
	Initialize all the interfaces of the cell.

	\param interfaces the list of all interfaces associated to the cell
*/
void Cell::initialize_interfaces(std::vector<std::vector<long>> &interfaces)
{
	m_interfaces = CollapsedVector2D<long>(interfaces);
}

/*!
	Initialize the data structure that holds the information about the
	interfaces.

	\param nInterfaces An array with the number of interfaces for each face
*/
void Cell::initialize_empty_interfaces(const std::vector<int> interfaceCount)
{
	m_interfaces = CollapsedVector2D<long>(interfaceCount, NULL_ELEMENT_ID);
}

/*!
	Sets the i-th interface associated the the given face of the cell.

	\param face the face of the cell
	\param index the index of the interface
	\param interface is the index of the interface 
*/
void Cell::set_interface(const int &face, const int &index, const long &interface)
{
	m_interfaces.set(face, index, interface);
}

/*!
	Add an interface to the given face of the cell.

	\param face is the face of the cell
	\param interface is the index of the interface that will be added
*/
void Cell::push_interface(const int &face, const long &interface)
{
	m_interfaces.push_back_in_sub_array(face, interface);
}

/*!
	Deletes the specified interface from the interfaces associate to the
	given face of the cell.

	\param face the face of the cell
	\param i is the index of the interface to delete
*/
void Cell::delete_interface(const int &face, const int &i)
{
	m_interfaces.erase(face, i);
}

/*!
	Unsets the interfaces associated to the cell.
*/
void Cell::unset_interfaces()
{
	m_interfaces.clear();
}

/*!
	Gets the total number of interfaces of the cell.

	\result The total number of interfaces of the cell.
*/
int Cell::get_interface_count() const
{
	return m_interfaces.sub_arrays_total_size();
}

/*!
	Gets the number of interfaces of the specified face of the cell.

	\param face the face of the cell
	\result The number of interfaces of the specified face of the cell.
*/
int Cell::get_interface_count(const int &face) const
{
	return m_interfaces.sub_array_size(face);
}

/*!
	Gets the i-th interface of the specified face of the cell.

	\param face the face of the cell
	\param index the index of the interface to retreive
	\result The requested interface.
*/
long Cell::get_interface(const int &face, const int &index) const
{
	return m_interfaces.get(face, index);
}

/*!
	Gets all the interfaces of the cell.

	\result The interfaces of the cell.
*/
const long * Cell::get_interfaces() const
{
	return m_interfaces.get(0);
}

/*!
	Gets the interfaces of the given face of the cell.

	\as get_interface(const int &face, const int &index) const

	\param face the face of the cell
	\result The requested interfaces
*/
const long * Cell::get_interfaces(const int &face) const
{
	return m_interfaces.get(face);
}

/*!
	Displays the cell information to an output stream

	\param[in] out is the output stream
	\param[in] indent is the number of trailing spaces to prepend when
	writing the information
*/
void Cell::display(std::ostream &out, unsigned short int indent)
{
	// ====================================================================== //
	// VARIABLES DECLARATION                                                  //
	// ====================================================================== //

	// Local variables
	std::string                 t_s(indent, ' ');

	// Counters
	int                         i, j;
	int                         nn;

	// ====================================================================== //
	// DISPLAY INFOS                                                          //
	// ====================================================================== //
	if (get_type() != ElementInfo::UNDEFINED) {
	    out << t_s << "cell type:    (unknown)" << std::endl;
	    return;
	}

	// Scope variables -------------------------------------------------- //
	int                         nv = get_vertex_count();
	int                         nf = get_face_count();

	// General info ----------------------------------------------------- //
	out << t_s << "cell type:    " << get_type() << std::endl;
	out << t_s << "ID:           " << get_id() << std::endl;
	out << t_s << "is ghost:     ";
	if (m_interior)     { out << "(false)"; }
	else                { out << "(true)"; }
	out << std::endl;

	// Connectivity infos --------------------------------------------------- //
	out << t_s << "connectivity: [ ";
	for (i = 0; i < nv-1; ++i) {
		out << get_vertex(i) << ", ";
	} //next i
	out << get_vertex(nv-1) << " ]" << std::endl;

	// neighbors infos ------------------------------------------------------ //
	out << t_s << "neighbors:   [ ";
	for (i = 0; i < nf-1; ++i) {
		nn = get_interface_count(i);
		out << "[ ";
		for (j = 0; j < nn-1; ++j) {
			out << get_interface(i, j) << ", ";
		} //next j
		out << get_interface(i, nn-1) << " ], ";
	} //next i
	nn = get_interface_count(nf-1);
	out << "[ ";
	for (j = 0; j < nn-1; ++j) {
		out << get_interface(nf-1, j) << ", ";
	} //next j
	out << get_interface(nf-1, nn-1) << " ]";
	out << " ]" << std::endl;
}

/*!
	Input stream operator for class Cell. Stream cell data from memory
	input stream to container.

	\param[in] buffer is the input stream from memory
	\param[in] cell is the cell object
	\result Returns the same input stream received in input.
*/
ibinarystream& operator>>(ibinarystream &buffer, Cell &cell)
{
	// Write connectivity data ---------------------------------------------- //
	Element &element_(cell);
	buffer >> element_;

	// Write interface data ------------------------------------------------- //
	buffer >> cell.m_interfaces;

	return buffer;
}

/*!
	Output stream operator for class Cell. Stream cell data from container
	to output stream.

	\param[in] buffer is the output stream from memory
	\param[in] cell is the cell object
	\result Returns the same output stream received in input.
*/
obinarystream& operator<<(obinarystream  &buffer, const Cell &cell)
{
	// Write connectivity data ---------------------------------------------- //
	const Element &element(cell);
	buffer << element;

	// Write interface data ------------------------------------------------- //
	buffer << cell.m_interfaces;

	return buffer;
}

/*!
	Get the size of the buffer required to communicate cell.

	\result Returns the buffer size (in bytes).
*/
unsigned int Cell::get_binary_size()
{
    return (Element::get_binary_size() + m_interfaces.get_binary_size());
}

/*!
	@}
*/
