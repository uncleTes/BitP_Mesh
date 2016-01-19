// =================================================================================== //
// INCm_dimUDES                                                                            //
// =================================================================================== //
#include "Map.hpp"
#include <math.h>

// =================================================================================== //
// NAME SPACES                                                                         //
// =================================================================================== //
using namespace std;

// =================================================================================== //
// CLASS IMPLEMENTATION                                                                    //
// =================================================================================== //

// =================================================================================== //
// CONSTRUCTORS AND OPERATORS
// =================================================================================== //

/*!Default constructor. Origin of octree in physical domain in (0,0,0)
 * and side length 1.
 */
Map::Map(int8_t maxlevel, uint8_t dim_){
	maxlevel = int8_t(max(0,min(int(maxlevel),21)));
	m_origin[0] = m_origin[1] = m_origin[2] = 0.0;
	m_L = 1.0;
	m_dim = dim_;
	m_nnodes = 1<<m_dim;
	m_nnodesPerFace = 1<<(m_dim-1);
	m_maxLength = uint32_t(1<<maxlevel);

};

/*!Customized constructor with origin of octree in physical
 * domain side length provided by the user.
 * \param[in] X Coordinate X of the origin.
 * \param[in] Y Coordinate Y of the origin.
 * \param[in] Z Coordinate Z of the origin.
 * \param[in] LL Side length of domain.
 */
Map::Map(double & X, double & Y, double & Z, double & LL, int8_t maxlevel, uint8_t dim_){
	maxlevel = int8_t(max(0,min(int(maxlevel),21)));
	m_origin[0] = X;
	m_origin[1] = Y;
	m_origin[2] = Z;
	m_L = LL;
	m_dim = dim_;
	m_nnodes = 1<<m_dim;
	m_nnodesPerFace = 1<<(m_dim-1);
	m_maxLength = uint32_t(1<<maxlevel);
};

// =================================================================================== //
// METHODS
// =================================================================================== //

/*! Transformation of coordinates X,Y,Z (logical->physical).
 * \param[in] X Coordinates from logical domain.
 * \return Coordinates in physical domain.
 */
darray3 Map::mapCoordinates(u32array3 const & X){
	darray3 coords;
	for (int i=0; i<3; ++i){
		coords[i] = (m_origin[i] + m_L/double(m_maxLength) * double(X[i]));
	}
	return coords;
};

/*! Transformation of coordinate X (logical->physical).
 * \param[in] X Coordinate X from logical domain.
 * \return Coordinate X in physical domain.
 */
double Map::mapX(uint32_t const & X){
	return (m_origin[0] + m_L/double(m_maxLength) * double(X));
};

/*! Transformation of coordinate Y (logical->physical).
 * \param[in] Y Coordinate Y from logical domain.
 * \return Coordinate Y in physical domain.
 */
double Map::mapY(uint32_t const & Y){
	return (m_origin[1] + m_L/double(m_maxLength) * double(Y));
};

/*! Transformation of coordinate Z (logical->physical).
 * \param[in] Z Coordinate Z from logical domain.
 * \return Coordinate Z in physical domain.
 */
double Map::mapZ(uint32_t const & Z){
	return (m_origin[2] + m_L/double(m_maxLength) * double(Z));
};

/*! Transformation of coordinates X,Y,Z (physical->logical).
 * \param[in] X Coordinates from physical domain.
 * \return Coordinates in logical domain.
 */
u32array3 Map::mapCoordinates(darray3 const & X){
	u32array3 coords;
	for (int i=0; i<3; ++i){
		coords[i] = (uint32_t)(double(m_maxLength)/m_L * (X[i] - m_origin[i]));
	}
	return coords;
};

/*! Transformation of coordinate X (physical->logical).
 * \param[in] X Coordinate X from physical domain.
 * \return Coordinate X in logical domain.
 */
uint32_t Map::mapX(double const & X){
	return (uint32_t)(double(m_maxLength)/m_L * (X - m_origin[0]));
};

/*! Transformation of coordinate Y (physical->logical).
 * \param[in] Y Coordinate Y from physical domain.
 * \return Coordinate Y in logical domain.
 */
uint32_t Map::mapY(double const & Y){
	return (uint32_t)(double(m_maxLength)/m_L * (Y - m_origin[1]));
};

/*! Transformation of coordinate Z (physical->logical).
 * \param[in] Z Coordinate Z from physical domain.
 * \return Coordinate Z in logical domain.
 */
uint32_t Map::mapZ(double const & Z){
	return (uint32_t)(double(m_maxLength)/m_L * (Z - m_origin[2]));
};

/*! Transformation of size of an octant (logical->physical).
 * \param[in] size Size of octant from logical domain.
 * \return Size of octant in physical domain.
 */
double Map::mapSize(uint32_t const & size){
	return ((m_L/double(m_maxLength))*double(size));
};

/*! Transformation of area of an octant (logical->physical).
 * \param[in] area Area of octant from logical domain.
 * \return Area of octant in physical domain.
 */
double Map::mapArea(uint64_t const & Area){
	return ((pow(m_L,m_dim-1)/pow(double(m_maxLength),m_dim-1))*double(Area));
};

/*! Transformation of volume of an octant (logical->physical).
 * \param[in] volume Volume of octant from logical domain.
 * \return Coordinate Volume of octant in physical domain.
 */
double Map::mapVolume(uint64_t const & Volume){
	return ((pow(m_L,m_dim)/pow(double(m_maxLength),m_dim))*double(Volume));
};

/*! Transformation of coordinates of center of an octant (logical->physical).
 * \param[in] center Pointer to coordinates of center from logical domain.
 * \param[out] mapcenter Coordinates of center in physical domain.
 */
void Map::mapCenter(double* & center, darray3 & mapcenter){
	for (int i=0; i<m_dim; i++){
		mapcenter[i] = m_origin[i] + m_L/double(m_maxLength) * center[i];
	}
};

/*! Transformation of coordinates of center of an octant (logical->physical).
 * \param[in] center Array of coordinates of center from logical domain.
 * \param[out] mapcenter Coordinates of center in physical domain.
 */
void Map::mapCenter(darray3 & center, darray3 & mapcenter){
	for (int i=0; i<m_dim; i++){
		mapcenter[i] = m_origin[i] + m_L/double(m_maxLength) * center[i];
	}
};

/*! Transformation of coordinates of nodes of an octant (logical->physical).
 * \param[in] nodes Pointer to coordinates of nodes from logical domain.
 * \param[out] mapnodes Coordinates of nodes in physical domain.
 */
void Map::mapNodes(uint32_t (*nodes)[3], darr3vector & mapnodes){
	mapnodes.resize(m_nnodes);
	for (int i=0; i<m_nnodes; i++){
		for (int j=0; j<3; j++){
			mapnodes[i][j] = m_origin[j] + m_L/double(m_maxLength) * double(nodes[i][j]);
		}
	}
};

/*! Transformation of coordinates of nodes of an octant (logical->physical).
 * \param[in] nodes Vector of coordinates of nodes from logical domain.
 * \param[out] mapnodes Coordinates of nodes in physical domain.
 */
void Map::mapNodes(u32arr3vector nodes, darr3vector & mapnodes){
	mapnodes.resize(m_nnodes);
	for (int i=0; i<m_nnodes; i++){
		for (int j=0; j<3; j++){
			mapnodes[i][j] = m_origin[j] + m_L/double(m_maxLength) * double(nodes[i][j]);
		}
	}
};

/*! Transformation of coordinates of a node of an octant (logical->physical).
 * \param[in] node Coordinates of  the node from logical domain.
 * \param[out] mapnodes Coordinates of the node in physical domain.
 */
void Map::mapNode(u32array3 & node, darray3 & mapnode){
	for (int j=0; j<3; j++){
		mapnode[j] = m_origin[j] + m_L/double(m_maxLength) * double(node[j]);
	}
};

/*! Transformation of coordinates of nodes of an intersection (logical->physical).
 * \param[in] nodes Pointer to coordinates of nodes from logical domain.
 * \param[out] mapnodes Coordinates of nodes in physical domain.
 */
void Map::mapNodesIntersection(uint32_t (*nodes)[3], darr3vector & mapnodes){
	mapnodes.resize(m_nnodesPerFace);
	for (int i=0; i<m_nnodesPerFace; i++){
		for (int j=0; j<3; j++){
			mapnodes[i][j] = m_origin[j] + m_L/double(m_maxLength) * double(nodes[i][j]);
		}
	}
};

/*! Transformation of coordinates of nodes of an intersection (logical->physical).
 * \param[in] nodes Pointer to coordinates of nodes from logical domain.
 * \param[out] mapnodes Coordinates of nodes in physical domain.
 */
void Map::mapNodesIntersection(u32arr3vector nodes, darr3vector & mapnodes){
	mapnodes.resize(m_nnodesPerFace);
	for (int i=0; i<m_nnodesPerFace; i++){
		for (int j=0; j<3; j++){
			mapnodes[i][j] = m_origin[j] + m_L/double(m_maxLength) * double(nodes[i][j]);
		}
	}
};

/*! Transformation of components of normal of an intersection (logical->physical).
 * \param[in] nodes Pointer to components of normal from logical domain.
 * \param[out] mapnodes components of normal in physical domain.
 */
void Map::mapNormals(i8array3 normal, darray3 & mapnormal){
	mapnormal[0] = double(normal[0]);
	mapnormal[1] = double(normal[1]);
	mapnormal[2] = double(normal[2]);
};