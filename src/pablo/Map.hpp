#ifndef MAP_HPP_
#define MAP_HPP_

// =================================================================================== //
// INCLUDES                                                                            //
// =================================================================================== //
#include <vector>
#include <iostream>
#include <array>

// =================================================================================== //
// TYPEDEFS
// =================================================================================== //
typedef std::vector<double>			dvector;
typedef std::vector<dvector>		dvector2D;
typedef std::vector<uint32_t>		u32vector;
typedef std::vector<u32vector>		u32vector2D;
typedef std::vector<uint64_t>		u64vector;
typedef std::vector<u64vector>		u64vector2D;
typedef std::array<double, 3>		darray3;
typedef std::array<int8_t, 3>		i8array3;
typedef std::array<uint32_t, 3>		u32array3;
typedef std::vector<u32array3>		u32arr3vector;
typedef std::vector<darray3>		darr3vector;

// =================================================================================== //
// CLASS DEFINITION                                                                    //
// =================================================================================== //
/*!
 *  \ingroup        PABLO
 *  @{
 *	\date			17/dec/2015
 *	\authors		Edoardo Lombardi
 *	\authors		Marco Cisternino
 *	\copyright		Copyright 2014 Optimad engineering srl. All rights reserved.
 *	\par			License:\n
 *	This version of PABLO is released under the LGPL License.
 *
 *	\brief Transformation Mapper
 *
 *	Definition of the transformation from the logical domain to the physical domain.
 *	It contains a default (temporary) implementation of a scaling and translation mapper
 *	of logical octree.
 *	Map has to be implemented and customized by the user for different applications.
 */
class Map{

	// =================================================================================== //
	// FRIENDSHIPS
	// =================================================================================== //

	friend class ParaTree;

	// =================================================================================== //
	// MEMBERS
	// =================================================================================== //
private:
	darray3 	m_origin;				/**<Coordinate X,Y,Z of the origin of the octree in the physical domain*/
	double 		m_L;					/**<Side length of octree in the physical domain*/
	uint8_t		m_dim;					/**<Space Dimension*/
	uint8_t		m_nnodes;				/**<Number of nodes*/
	uint8_t		m_nnodesPerFace;		/**<Number of nodes for each face*/
	uint32_t	m_maxLength;			/**< Length of the logical domain */
	double		m_maxLength_1;			/**< 1/Length of the logical domain */

	// =================================================================================== //
	// CONSTRUCTORS AND OPERATORS
	// =================================================================================== //
	Map(int8_t maxlevel, uint8_t dim_);
//	Map(double & X, double & Y, double & Z, double & LL, int8_t maxlevel, uint8_t dim_);

	// =================================================================================== //
	// METHODS
	// =================================================================================== //

	darray3 mapCoordinates(u32array3 const & X);
	double mapX(uint32_t const & X);
	double mapY(uint32_t const & Y);
	double mapZ(uint32_t const & Z);
	u32array3 mapCoordinates(darray3 const & X);
	uint32_t mapX(double const & X);
	uint32_t mapY(double const & Y);
	uint32_t mapZ(double const & Z);
	double mapSize(uint32_t const & size);
	double mapArea(uint64_t const & area);
	double mapVolume(uint64_t const & volume);
	void mapCenter(double* & center, darray3 & mapcenter);
	void mapCenter(darray3 & center, darray3 & mapcenter);
	void mapNodes(uint32_t (*nodes)[3], darr3vector & mapnodes);
	void mapNodes(u32arr3vector nodes, darr3vector & mapnodes);
	void mapNode(u32array3 & node, darray3 & mapnode);
	void mapNodesIntersection(uint32_t (*nodes)[3], darr3vector & mapnodes);
	void mapNodesIntersection(u32arr3vector nodes, darr3vector & mapnodes);
	void mapNormals(i8array3 normal, darray3 & mapnormal);

};

/* @} */

#endif /* MAP_HPP_ */
