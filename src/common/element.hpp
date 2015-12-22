//
// Written by Andrea Iob <andrea_iob@hotmail.com>
//
#ifndef __BITP_MESH_ELEMENT_HPP__
#define __BITP_MESH_ELEMENT_HPP__

/*! \file */

#include <cstddef>
#include <memory>
#include <vector>

#include "BitP_Base_IO.hpp"

/*!
	\ingroup Common
	@{
*/

class ElementInfo {

public:
	enum Type {
		UNDEFINED  = -1,
		VERTEX     = 1,
		LINE       = 3,
		TRIANGLE   = 5,
		POLYGON    = 7,
		PIXEL      = 8,
		QUAD       = 9,
		TETRA      = 10,
		VOXEL      = 11,
		HEXAHEDRON = 12,
		WEDGE      = 13,
		PYRAMID    = 14,
		POLYHEDRON = 42
	};

	Type type;
	int dimension;

	int nVertices;
	int nEdges;
	int nFaces;

	static const ElementInfo undefinedInfo;
	static const ElementInfo vertexInfo;
	static const ElementInfo lineInfo;
	static const ElementInfo triangleInfo;
	static const ElementInfo pixelInfo;
	static const ElementInfo quadInfo;
	static const ElementInfo tetraInfo;
	static const ElementInfo voxelInfo;
	static const ElementInfo hexahedronInfo;
	static const ElementInfo pyramidInfo;
	static const ElementInfo wedgeInfo;

	std::vector<Type> face_type;
	std::vector<std::vector<int>> face_connect;

	std::vector<Type> edge_type;
	std::vector<std::vector<int>> edge_connect;

	ElementInfo();
	ElementInfo(ElementInfo::Type type);

	static const ElementInfo & get_element_info(ElementInfo::Type type);

private:
	void initializeUndefinedInfo();
	void initializeVertexInfo();
	void initializeLineInfo();
	void initializeTriangleInfo();
	void initializePixelInfo();
	void initializeQuadInfo();
	void initializeTetraInfo();
	void initializeVoxelInfo();
	void initializeHexahedronInfo();
	void initializePyramidInfo();
	void initializeWedgeInfo();

};

class Element;

ibinarystream& operator>>(ibinarystream &buf, Element& element);
obinarystream& operator<<(obinarystream &buf, const Element& element);

class Element {

friend obinarystream& operator<<(obinarystream& buf, const Element& element);
friend ibinarystream& operator>>(ibinarystream& buf, Element& element);

public:
	/*!
		Hasher for the ids.

		Since ids are unique, the hasher can be a function that
		takes an id and cast it to a size_t.

		The hasher is defined as a struct, because a struct can be
		passed as an object into metafunctions (meaning that the type
		deduction for the template paramenters can take place, and
		also meaning that inlining is easier for the compiler). A bare
		function would have to be passed as a function pointer.
		To transform a function template into a function pointer,
		the template would have to be manually instantiated (with a
		perhaps unknown type argument).

	*/
	struct IdHasher {
		/*!
			Function call operator that casts the specified
			value to a size_t.

			\tparam U type of the value
			\param value is the value to be casted
			\result Returns the value casted to a size_t.
		*/
		template<typename U>
		constexpr std::size_t operator()(U&& value) const noexcept
		{
			return static_cast<std::size_t>(std::forward<U>(value));
		}
	};

	Element();
	Element(const long &id, ElementInfo::Type type = ElementInfo::UNDEFINED);

	Element(Element&& other) = default;
	Element& operator=(Element&& other) = default;

	Element(const Element &other);
	Element& operator = (const Element &other);

	void initialize(ElementInfo::Type type);

	const ElementInfo & get_info() const;

	void set_id(const long &id);
	long get_id() const;
	
	void set_type(ElementInfo::Type type);
	ElementInfo::Type get_type() const;

	int get_dimension() const;
	bool is_three_dimensional() const;
	
	void set_connect(std::unique_ptr<long[]> connect);
	void unset_connect();
	const long * get_connect() const;
	long * get_connect();

	int get_face_count() const;
	ElementInfo::Type get_face_type(const int &face) const;
	std::vector<int> get_face_local_connect(const int &face) const;

	int get_edge_count() const;
	std::vector<int> get_edge_local_connect(const int &edge) const;

	void set_vertex(const int &index, const long &vertex);
	int get_vertex_count() const;
	int get_vertex(const int &vertex) const;

	static const long NULL_ELEMENT_ID;

	unsigned int get_binary_size();

private:
	long m_id;

	ElementInfo::Type m_type;

	std::unique_ptr<long[]> m_connect;
};

/*!
	@}
*/

#endif
