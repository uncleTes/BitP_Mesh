// =================================================================================== //
// INCLUDES                                                                            //
// =================================================================================== //
#include "ClassParaTree.hpp"
#include "ClassArray.hpp"
#include <sstream>
#include <iomanip>
#include <fstream>

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
/*! Default Constructor of Para_Tree.
 * It builds one octant with node 0 in the Origin (0,0,0) and side of length 1.
 * \param[in] dim The space dimension of the m_octree. 2D is the default value.
 * \param[in] maxlevel Maximum allowed level of refinement for the octree. The default value is 20.
 * \param[in] logfile The file name for the log of this object. PABLO.log is the default value.
 * \param[in] comm The MPI communicator used by the parallel octree. MPI_COMM_WORLD is the default value.
 */
#if ENABLE_MPI
ClassParaTree::ClassParaTree(uint8_t dim, int8_t maxlevel, string logfile, MPI_Comm comm) : m_dim(uint8_t(min(max(2,int(dim)),3))),m_log(logfile,comm),m_comm(comm),m_trans(maxlevel,dim),m_octree(maxlevel,dim){
#else
ClassParaTree::ClassParaTree(uint8_t dim, int8_t maxlevel, string logfile ) : m_dim(uint8_t(min(max(2,int(dim)),3))),m_log(logfile),m_trans(maxlevel, dim),m_octree(maxlevel,dim){
#endif
	m_global.setGlobal(maxlevel, m_dim);
	m_serial = true;
	m_errorFlag = 0;
	m_maxDepth = 0;
	m_globalNumOctants = m_octree.getNumOctants();
#if ENABLE_MPI
	m_errorFlag = MPI_Comm_size(m_comm,&m_nproc);
	m_errorFlag = MPI_Comm_rank(m_comm,&m_rank);
#else
	rank = 0;
	m_nproc = 1;
#endif
	m_partitionFirstDesc = new uint64_t[m_nproc];
	m_partitionLastDesc = new uint64_t[m_nproc];
	m_partitionRangeGlobalIdx = new uint64_t[m_nproc];
	uint64_t lastDescMorton = m_octree.getLastDesc().computeMorton();
	uint64_t firstDescMorton = m_octree.getFirstDesc().computeMorton();
	for(int p = 0; p < m_nproc; ++p){
		m_partitionRangeGlobalIdx[p] = 0;
		m_partitionLastDesc[p] = lastDescMorton;
		m_partitionLastDesc[p] = firstDescMorton;
	}
	// Write info log
	m_log.writeLog("---------------------------------------------");
	m_log.writeLog("- PABLO PArallel Balanced Linear Octree -");
	m_log.writeLog("---------------------------------------------");
	m_log.writeLog(" ");
	m_log.writeLog("---------------------------------------------");
	m_log.writeLog(" Number of proc		:	" + to_string(static_cast<unsigned long long>(m_nproc)));
	m_log.writeLog(" Dimension		:	" + to_string(static_cast<unsigned long long>(m_dim)));
	m_log.writeLog(" Max allowed level	:	" + to_string(static_cast<unsigned long long>(m_global.m_maxLevel)));
	m_log.writeLog("---------------------------------------------");
	m_log.writeLog(" ");
#if ENABLE_MPI
	MPI_Barrier(m_comm);
#endif
};

// =============================================================================== //

/*! Constructor of Para_Tree with input parameters.
 * It builds one octant with :
 * \param[in] X Coordinate X of node 0,
 * \param[in] Y Coordinate Y of node 0,
 * \param[in] Z Coordinate Z of node 0,
 * \param[in] L Side length of the octant.
 * \param[in] dim The space dimension of the m_octree. 2D is the default value.
 * \param[in] maxlevel Maximum allowed level of refinement for the octree. The default value is 20.
 * \param[in] logfile The file name for the log of this object. PABLO.log is the default value.
 * \param[in] comm The MPI communicator used by the parallel octree. MPI_COMM_WORLD is the default value.
 */
#if ENABLE_MPI
ClassParaTree::ClassParaTree(double X, double Y, double Z, double L, uint8_t dim, int8_t maxlevel, string logfile, MPI_Comm comm):m_dim(uint8_t(min(max(2,int(dim)),3))),m_trans(X,Y,Z,L,maxlevel,dim),m_log(logfile,comm),m_comm(comm),m_octree(maxlevel,dim){
#else
ClassParaTree::ClassParaTree(double X, double Y, double Z, double L, uint8_t dim, int8_t maxlevel, string logfile):m_dim(uint8_t(min(max(2,int(dim)),3))),m_trans(X,Y,Z,L,maxlevel,dim),m_log(logfile),m_octree(maxlevel,dim){
#endif
	m_global.setGlobal(maxlevel, m_dim);
	m_serial = true;
	m_errorFlag = 0;
	m_maxDepth = 0;
	m_globalNumOctants = m_octree.getNumOctants();
#if ENABLE_MPI
	m_errorFlag = MPI_Comm_size(m_comm,&m_nproc);
	m_errorFlag = MPI_Comm_rank(m_comm,&m_rank);
#else
	rank = 0;
	m_nproc = 1;
#endif
	m_partitionFirstDesc = new uint64_t[m_nproc];
	m_partitionLastDesc = new uint64_t[m_nproc];
	m_partitionRangeGlobalIdx = new uint64_t[m_nproc];
	uint64_t lastDescMorton = m_octree.getLastDesc().computeMorton();
	uint64_t firstDescMorton = m_octree.getFirstDesc().computeMorton();
	for(int p = 0; p < m_nproc; ++p){
		m_partitionRangeGlobalIdx[p] = 0;
		m_partitionLastDesc[p] = lastDescMorton;
		m_partitionLastDesc[p] = firstDescMorton;
	}
	// Write info log
	m_log.writeLog("---------------------------------------------");
	m_log.writeLog("- PABLO PArallel Balanced Linear Octree -");
	m_log.writeLog("---------------------------------------------");
	m_log.writeLog(" ");
	m_log.writeLog("---------------------------------------------");
	m_log.writeLog(" Number of proc		:	" + to_string(static_cast<unsigned long long>(m_nproc)));
	m_log.writeLog(" Dimension		:	" + to_string(static_cast<unsigned long long>(m_dim)));
	m_log.writeLog(" Max allowed level	:	" + to_string(static_cast<unsigned long long>(m_global.m_maxLevel)));
	m_log.writeLog(" Domain Origin		:	" + to_string(static_cast<unsigned long long>(X)));
	m_log.writeLog("				" + to_string(static_cast<unsigned long long>(Y)));
	m_log.writeLog("				" + to_string(static_cast<unsigned long long>(Z)));
	m_log.writeLog(" Domain Size		:	" + to_string(static_cast<unsigned long long>(L)));
	m_log.writeLog("---------------------------------------------");
	m_log.writeLog(" ");
#if ENABLE_MPI
	MPI_Barrier(m_comm);
#endif
};

// =============================================================================== //

/*! Constructor of Para_Tree for restart a simulation with input parameters.
 * For each process it builds a vector of octants. The input parameters are :
 * \param[in] X Physical Coordinate X of node 0,
 * \param[in] Y Physical Coordinate Y of node 0,
 * \param[in] Z Physical Coordinate Z of node 0,
 * \param[in] L Physical Side length of the domain,
 * \param[in] XYZ Coordinates of octants (node 0) in logical domain,
 * \param[in] levels Level of each octant.
 * \param[in] dim The space dimension of the m_octree. 2D is the default value.
 * \param[in] maxlevel Maximum allowed level of refinement for the octree. The default value is 20.
 * \param[in] logfile The file name for the log of this object. PABLO.log is the default value.
 * \param[in] comm The MPI communicator used by the parallel octree. MPI_COMM_WORLD is the default value.
 */
#if ENABLE_MPI
ClassParaTree::ClassParaTree(double X, double Y, double Z, double L, u32vector2D & XYZ, u8vector & levels, uint8_t dim, int8_t maxlevel, string logfile, MPI_Comm comm):m_dim(uint8_t(min(max(2,int(dim)),3))),m_trans(X,Y,Z,L,maxlevel,dim),m_log(logfile,comm),m_comm(comm),m_octree(maxlevel,dim){
#else
ClassParaTree::ClassParaTree(double X, double Y, double Z, double L, u32vector2D & XYZ, u8vector & levels, uint8_t dim, int8_t maxlevel, string logfile):m_dim(uint8_t(min(max(2,int(dim)),3))),m_trans(X,Y,Z,L,maxlevel,dim),m_log(logfile),m_octree(maxlevel,dim){
#endif
	uint8_t lev, iface;
	uint32_t x0, y0, z0;
	uint32_t NumOctants = XYZ.size();
	m_dim = dim;
	m_global.setGlobal(maxlevel, m_dim);
	m_octree.m_octants.resize(NumOctants);
	for (uint32_t i=0; i<NumOctants; i++){
		lev = uint8_t(levels[i]);
		x0 = uint32_t(XYZ[i][0]);
        y0 = uint32_t(XYZ[i][1]);
        z0 = uint32_t(XYZ[i][2]);
		ClassOctant oct(false, m_dim, lev, x0, y0, z0);
		oct.setBalance(false);
		if (x0 == 0){
			iface = 0;
			oct.setBound(iface);
		}
		else if (x0 == m_global.m_maxLength - oct.getSize(m_global.m_maxLevel)){
			iface = 1;
			oct.setBound(iface);
		}
        if (y0 == 0){
            iface = 2;
            oct.setBound(iface);
        }
        else if (y0 == m_global.m_maxLength - oct.getSize(m_global.m_maxLevel)){
            iface = 3;
            oct.setBound(iface);
        }
        if (z0 == 0){
            iface = 4;
            oct.setBound(iface);
        }
        else if (z0 == m_global.m_maxLength - oct.getSize(m_global.m_maxLevel)){
            iface = 5;
            oct.setBound(iface);
        }
		m_octree.m_octants[i] = oct;
	}

	//ATTENTO if nompi deve aver l'else
#if ENABLE_MPI
	m_errorFlag = MPI_Comm_size(m_comm,&m_nproc);
	m_errorFlag = MPI_Comm_rank(m_comm,&m_rank);
	m_serial = true;
	if (m_nproc > 1 ) m_serial = false;
#else
	m_serial = true;
	m_nproc = 1;
	rank = 0;
#endif
	m_partitionFirstDesc = new uint64_t[m_nproc];
	m_partitionLastDesc = new uint64_t[m_nproc];
	m_partitionRangeGlobalIdx = new uint64_t[m_nproc];

	setFirstDesc();
	setLastDesc();
	m_octree.updateLocalMaxDepth();
	updateAdapt();
#if ENABLE_MPI
	setPboundGhosts();
#endif
	// Write info log
	m_log.writeLog("---------------------------------------------");
	m_log.writeLog("- PABLO PArallel Balanced Linear Octree -");
	m_log.writeLog("---------------------------------------------");
	m_log.writeLog(" ");
	m_log.writeLog("---------------------------------------------");
	m_log.writeLog("- PABLO restart -");
	m_log.writeLog("---------------------------------------------");
	m_log.writeLog(" Number of proc		:	" + to_string(static_cast<unsigned long long>(m_nproc)));
	m_log.writeLog(" Dimension		:	" + to_string(static_cast<unsigned long long>(m_dim)));
	m_log.writeLog(" Max allowed level	:	" + to_string(static_cast<unsigned long long>(m_global.m_maxLevel)));
	m_log.writeLog(" Domain Origin		:	" + to_string(static_cast<unsigned long long>(X)));
	m_log.writeLog("				" + to_string(static_cast<unsigned long long>(Y)));
	m_log.writeLog("				" + to_string(static_cast<unsigned long long>(Z)));
	m_log.writeLog(" Domain Size		:	" + to_string(static_cast<unsigned long long>(L)));
	m_log.writeLog(" Number of octants	:	" + to_string(static_cast<unsigned long long>(m_globalNumOctants)));
	m_log.writeLog("---------------------------------------------");
	m_log.writeLog(" ");
#if ENABLE_MPI
	MPI_Barrier(m_comm);
#endif
};

// =============================================================================== //

ClassParaTree::~ClassParaTree(){
	m_log.writeLog("---------------------------------------------");
	m_log.writeLog("--------------- R.I.P. PABLO ----------------");
	m_log.writeLog("---------------------------------------------");
	m_log.writeLog("---------------------------------------------");
};

// =================================================================================== //
// METHODS
// =================================================================================== //

// =================================================================================== //
// BASIC GET/SET METHODS
// =================================================================================== //

/*! Get the rank of local process.
 * \return Rank of local process.
 */
int
ClassParaTree::getRank(){
	return m_rank;
};

/*! Get the total number of processes.
 * \return Number of processes.
 */
int
ClassParaTree::getNproc(){
	return m_nproc;
};

/*! Get thecommunicator used by octree between processes.
 * \return MPI Communicator.
 */
MPI_Comm
ClassParaTree::getComm(){
	return m_comm;
};

/*! Get the partition information of the octree over the processes
 * by using the global index of the octants.
 * \return Pointer to m_partitionRangeGlobalIdx (global array containing global
 * index of the last existing octant in each processor).
 */
uint64_t*
ClassParaTree::getPartitionRangeGlobalIdx(){
	return m_partitionRangeGlobalIdx;
};

/*! Get the coordinates of the origin of the octree.
 * \return Coordinates of the origin.
 */
darray3
ClassParaTree::getOrigin(){
	return m_trans.m_origin;
};

/*! Get the coordinate X of the origin of the octree.
 * \return Coordinate X of the origin.
 */
double
ClassParaTree::getX0(){
	return m_trans.m_origin[0];
};

/*! Get the coordinate Y of the origin of the octree.
 * \return Coordinate Y of the origin.
 */
double
ClassParaTree::getY0(){
	return m_trans.m_origin[1];
};

/*! Get the coordinate Z of the origin of the octree.
 * \return Coordinate Z of the origin.
 */
double
ClassParaTree::getZ0(){
	return m_trans.m_origin[2];
};

/*! Get the length of the domain.
 * \return Length of the octree.
 */
double
ClassParaTree::getL(){
	return m_trans.m_L;
};

/*! Get the maximum level of refinement allowed for this octree.
 * \return Maximum refinement level for the octree.
 */
int
ClassParaTree::getMaxLevel(){
	return m_global.m_maxLevel;
};

/*! Get the length of the domain in logical domain.
 * \return Length of the octree in logical domain.
 */
uint32_t
ClassParaTree::getMaxLength(){
	return m_global.m_maxLength;
}

/*! Get the number of nodes for each octant (4 for 2D case, 8 for 3D case)
 * \return Number of nodes for octant.
 */
uint8_t
ClassParaTree::getNnodes(){
	return m_global.m_nnodes;
}

/*! Get the number of faces for each octant (4 for 2D case, 6 for 3D case)
 * \return Number of faces for octant.
 */
uint8_t
ClassParaTree::getNfaces(){
	return m_global.m_nfaces;
}

/*! Get the number of edges for each octant (0 for 2D case, 12 for 3D case)
 * \return Number of edges for octant.
 */
uint8_t
ClassParaTree::getNedges(){
	return m_global.m_nedges;
}

/*! Get the number of possible children for each octant (4 for 2D case, 8 for 3D case)
 * \return Number of children for octant.
 */
uint8_t
ClassParaTree::getNchildren(){
	return m_global.m_nchildren;
}

/*! Get the number of nodes for each face of an octant (2 for 2D case, 4 for 3D case)
 * \return Number of nodes for face for an octant.
 */
uint8_t
ClassParaTree::getNnodesperface(){
	return m_global.m_nnodesPerFace;
}

/*! Get the components (in logical domain) of the 6 normals to the faces of an octant (for the 2D case consider only the first 4)
 * \param[out] normals Normals array[6][3] to the faces of an octant.
 */
void
ClassParaTree::getNormals(int8_t normals[6][3]){
	for (int i=0; i<6; i++){
		for (int j=0; j<3; j++){
			normals[i][j] = m_global.m_normals[i][j];
		}
	}
}

/*! Get the indices of the faces of virtual octants opposed to the 6 faces of an octant
 * (for the 2D case consider only the first 4 terms).
 * \param[out] oppface Opposed faces array[6] to the faces of an octant (oppface[i] = Index of the
 * face of an octant neighbour through the i-th face of the current octant).
 */
void
ClassParaTree::getOppface(uint8_t oppface[6]){
	for (int j=0; j<6; j++){
		oppface[j] = m_global.m_oppFace[j];
	}
}

/*! Get the face-node connectivity for 6 faces (in 2D case consider only the first 4 terms).
 * \param[out] facenode Connectivity face-node. facenode[i][0:1] = local indices of nodes
 * of the i-th face of an octant.
 */
void
ClassParaTree::getFacenode(uint8_t facenode[6][4]){
	for (int i=0; i<6; i++){
		for (int j=0; j<4; j++){
			facenode[i][j] = m_global.m_faceNode[i][j];
		}
	}
}

/*! Get the node-face connectivity for 8 nodes (in 2D case consider only the first 4 terms).
 * \param[out] nodeface Connectivity node-face. nodeface[i][0:1] = local indices of faces
 * sharing the i-th node of an octant.
 */
void
ClassParaTree::getNodeface(uint8_t nodeface[8][3]){
	for (int i=0; i<8; i++){
		for (int j=0; j<3; j++){
			nodeface[i][j] = m_global.m_nodeFace[i][j];
		}
	}
}

/*! Get the edge-face connectivity for 12 edge (in 2D case not to be considered at all).
 * \param[out] edgeface Connectivity edge-face. edgeface[i][0:1] = local indices of
 * faces sharing the i-th edge of an octant.
 */
void
ClassParaTree::getEdgeface(uint8_t edgeface[12][2]){
	for (int i=0; i<12; i++){
		for (int j=0; j<2; j++){
			edgeface[i][j] = m_global.m_edgeFace[i][j];
		}
	}
}

/*!Get the normals of the nodes (in 2D case consider only the first 4).
 * \param[out] nodecoeffs Components (x,y,z) of the "normals" of the nodes.
 */
void
ClassParaTree::getNodecoeffs(int8_t nodecoeffs[8][3]){
	for (int i=0; i<8; i++){
		nodecoeffs[i][3] = 0;
		for (int j=0; j<m_dim; j++){
			nodecoeffs[i][j] = m_global.m_nodeCoeffs[i][j];
		}
	}
}

/*!Get the normals per edge (in 2D case not to be considered at all).
 * \param[out] edgecoeffs Components (x,y,z) of the "normals" per edge.
 */
void
ClassParaTree::getEdgecoeffs(int8_t edgecoeffs[12][3]){
	for (int i=0; i<12; i++){
		for (int j=0; j<3; j++){
		edgecoeffs[i][j] = m_global.m_edgeCoeffs[i][j];
		}
	}
}

/*! Get the components (in logical domain) of the 6 normals to the faces of an octant (for the 2D case consider only the first 4)
 * \return Pointer to normals array[6][3] to the faces of an octant.
 */
int8_t
(*ClassParaTree::getNormals())[3]{
	return m_global.m_normals;
}

/*! Get the indices of the faces of virtual octants opposed to the 6 faces of an octant
 * (for the 2D case consider only the first 4 terms).
 * \return Pointer to opposed faces array[6] to the faces of an octant (oppface[i] = Index of the
 * face of an octant neighbour through the i-th face of the current octant).
 */
uint8_t
(*ClassParaTree::getOppface()){
	return m_global.m_oppFace;
}

/*! Get the face-node connectivity for 6 faces (in 2D case consider only the first 4 terms).
 * \return Pointer to connectivity face-node. facenode[i][0:1] = local indices of nodes
 * of the i-th face of an octant.
 */
uint8_t
(*ClassParaTree::getFacenode())[4]{
	return m_global.m_faceNode;
}

/*! Get the node-face connectivity for 8 nodes (in 2D case consider only the first 4 terms).
 * \return Pointer to connectivity node-face. nodeface[i][0:1] = local indices of faces
 * sharing the i-th node of an octant.
 */
uint8_t
(*ClassParaTree::getNodeface())[3]{
	return m_global.m_nodeFace;
}

/*! Get the edge-face connectivity for 12 edge (in 2D case not to be considered at all).
 * \return Pointer to connectivity edge-face. edgeface[i][0:1] = local indices of
 * faces sharing the i-th edge of an octant.
 */
uint8_t
(*ClassParaTree::getEdgeface())[2]{
	return m_global.m_edgeFace;
}

/*!Get the normals of the nodes (in 2D case consider only the first 4).
 * \return Pointer to components (x,y,z) of the "normals" of the nodes.
 */
int8_t
(*ClassParaTree::getNodecoeffs())[3]{
	return m_global.m_nodeCoeffs;
}

/*!Get the normals per edge (in 2D case not to be considered at all).
 * \return Pointer to components (x,y,z) of the "normals" per edge.
 */
int8_t
(*ClassParaTree::getEdgecoeffs())[3]{
	return m_global.m_edgeCoeffs;
}

/*|Set the maximum refinement level allowed for the octree.
 * \param[in] maxlevel Maximum refinement level.
 */
void
ClassParaTree::setMaxLevel(int8_t maxlevel){
	m_global.m_maxLevel = maxlevel;
};

// =================================================================================== //
// INDEX BASED METHODS
// =================================================================================== //

/*! Get the coordinates of an octant, i.e. the coordinates of its node 0.
 * \param[in] idx Local index of target octant.
 * \return Coordinates X,Y,Z of node 0.
 */
darray3
ClassParaTree::getCoordinates(uint32_t idx) {
	return m_trans.mapCoordinates(m_octree.m_octants[idx].getCoordinates());
}

/*! Get the coordinates of an octant, i.e. the coordinates of its node 0.
 * \param[in] idx Local index of target octant.
 * \return Coordinate X of node 0.
 */
double
ClassParaTree::getX(uint32_t idx) {
	return m_trans.mapX(m_octree.m_octants[idx].getX());
}

/*! Get the coordinates of an octant, i.e. the coordinates of its node 0.
 * \param[in] idx Local index of target octant.
 * \return Coordinate Y of node 0.
 */
double
ClassParaTree::getY(uint32_t idx) {
	return m_trans.mapY(m_octree.m_octants[idx].getY());
}

/*! Get the coordinates of an octant, i.e. the coordinates of its node 0.
 * \param[in] idx Local index of target octant.
 * \return Coordinate Z of node 0.
 */
double
ClassParaTree::getZ(uint32_t idx) {
	return m_trans.mapZ(m_octree.m_octants[idx].getZ());
}

/*! Get the size of an octant, i.e. the side length.
 * \param[in] idx Local index of target octant.
 * \return Area of octant.
 */
double
ClassParaTree::getSize(uint32_t idx) {
	return m_trans.mapSize(m_octree.m_octants[idx].getSize(m_global.m_maxLevel));
}

/*! Get the area of an octant (for 2D case the same value of getSize).
 * \param[in] idx Local index of target octant.
 * \return Area of octant.
 */
double
ClassParaTree::getArea(uint32_t idx) {
	return m_trans.mapArea(m_octree.m_octants[idx].getArea(m_global.m_maxLevel));
}

/*! Get the volume of an octant.
 * \param[in] idx Local index of target octant.
 * \return Volume of octant.
 */
double
ClassParaTree::getVolume(uint32_t idx) {
	return m_trans.mapVolume(m_octree.m_octants[idx].getVolume(m_global.m_maxLevel));
}

/*! Get the coordinates of the center of an octant.
 * \param[in] idx Local index of target octant.
 * \param[out] center Coordinates of the center of octant.
 */
void
ClassParaTree::getCenter(uint32_t idx, darray3& center) {
	darray3 center_ = m_octree.m_octants[idx].getCenter(m_global.m_maxLevel);
	m_trans.mapCenter(center_, center);
}

/*! Get the coordinates of the center of an octant.
 * \param[in] idx Local index of target octant.
 * \return center Coordinates of the center of octant.
 */
darray3
ClassParaTree::getCenter(uint32_t idx) {
	darray3 center;
	darray3 center_ = m_octree.m_octants[idx].getCenter(m_global.m_maxLevel);
	m_trans.mapCenter(center_, center);
	return center;
}

/*! Get the coordinates of the center of a face of an octant.
 * \param[in] idx Local index of target octant.
 * \param[in] iface Index of the target face.
 * \return center Coordinates of the center of the iface-th face af octant.
 */
darray3
ClassParaTree::getFaceCenter(uint32_t idx, uint8_t iface) {
	darray3 center;
	darray3 center_ = m_octree.m_octants[idx].getFaceCenter(iface, m_global.m_maxLevel);
	m_trans.mapCenter(center_, center);
	return center;
}

/*! Get the coordinates of the center of a face of an octant.
 * \param[in] idx Local index of target octant.
 * \param[in] iface Index of the target face.
 * \param[out] center Coordinates of the center of the iface-th face af octant.
 */
void
ClassParaTree::getFaceCenter(uint32_t idx, uint8_t iface, darray3& center) {
	darray3 center_ = m_octree.m_octants[idx].getFaceCenter(iface, m_global.m_maxLevel);
	m_trans.mapCenter(center_, center);
}

/*! Get the coordinates of single node of an octant.
 * \param[in] idx Local index of target octant.
 * \param[in] inode Index of the target node.
 * \return center Coordinates of the center of the iface-th face af octant.
 */
darray3
ClassParaTree::getNode(uint32_t idx, uint8_t inode) {
	darray3 node;
	u32array3 node_ = m_octree.m_octants[idx].getNode(inode, m_global.m_maxLevel);
	m_trans.mapNode(node_, node);
	return node;
}

/*! Get the coordinates of the center of a face of an octant.
 * \param[in] idx Local index of target octant.
 * \param[in] iface Index of the target face.
 * \param[out] center Coordinates of the center of the iface-th face af octant.
 */
void
ClassParaTree::getNode(uint32_t idx, uint8_t inode, darray3& node) {
	u32array3 node_ = m_octree.m_octants[idx].getNode(inode, m_global.m_maxLevel);
	m_trans.mapNode(node_, node);
}

/*! Get the coordinates of the nodes of an octant.
 * \param[in] idx Local index of target octant.
 * \param[out] nodes Coordinates of the nodes of octant.
 */
void
ClassParaTree::getNodes(uint32_t idx, darr3vector & nodes) {
	u32arr3vector nodes_;
	m_octree.m_octants[idx].getNodes(nodes_, m_global.m_maxLevel);
	m_trans.mapNodes(nodes_, nodes);
}

/*! Get the coordinates of the nodes of an octant.
 * \param[in] idx Local index of target octant.
 * \return nodes Coordinates of the nodes of octant.
 */
darr3vector
ClassParaTree::getNodes(uint32_t idx){
	darr3vector nodes;
	u32arr3vector nodes_;
	m_octree.m_octants[idx].getNodes(nodes_, m_global.m_maxLevel);
	m_trans.mapNodes(nodes_, nodes);
	return nodes;
}

/*! Get the normal of a face of an octant.
 * \param[in] Local index of target octant.
 * \param[in] iface Index of the face for normal computing.
 * \param[out] normal Coordinates of the normal of face.
 */
void
ClassParaTree::getNormal(uint32_t idx, uint8_t & iface, darray3 & normal) {
	i8array3 normal_;
	m_octree.m_octants[idx].getNormal(iface, normal_, m_global.m_normals, m_global.m_maxLevel);
	m_trans.mapNormals(normal_, normal);
}

/*! Get the normal of a face of an octant.
 * \param[in] idx Local index of target octant.
 * \param[in] iface Index of the face for normal computing.
 * \return normal Coordinates of the normal of face.
 */
darray3
ClassParaTree::getNormal(uint32_t idx, uint8_t & iface){
	darray3 normal;
	i8array3 normal_;
	m_octree.m_octants[idx].getNormal(iface, normal_, m_global.m_normals, m_global.m_maxLevel);
	m_trans.mapNormals(normal_, normal);
	return normal;
}

/*! Get the refinement marker of an octant.
 * \param[in] idx Local index of target octant.
 * \return Marker of octant.
 */
int8_t
ClassParaTree::getMarker(uint32_t idx){
	return m_octree.getMarker(idx);
};

/*! Get the level of an octant.
 * \param[in] idx Local index of target octant.
 * \return Level of octant.
 */
uint8_t
ClassParaTree::getLevel(uint32_t idx){
	return m_octree.getLevel(idx);
};

/** Compute the Morton index of an octant (without level).
 * \param[in] idx Local index of target octant.
 * \return morton Morton index of the octant.
 */
uint64_t
ClassParaTree::computeMorton(uint32_t idx){
	return m_octree.computeMorton(idx);
};

/*! Get the balancing condition of an octant.
 * \param[in] idx Local index of target octant.
 * \return Has octant to be balanced?
 */
bool
ClassParaTree::getBalance(uint32_t idx){
	return !m_octree.getBalance(idx);
};

/*! Get the bound condition of the face of the octant
 * \param[in] idx Local index of the target octant
 * \param[in] iface Index of the face
 * \return Is the face a boundary face?
 */
bool
ClassParaTree::getBound(uint32_t idx, uint8_t iface){
	return m_octree.m_octants[idx].getBound(iface);
}

/*! Get the bound condition of the face of the octant
 * \param[in] idx Local index of the target octant
 * \return Is the octant a boundary octant?
 */
bool
ClassParaTree::getBound(uint32_t idx){
	return m_octree.m_octants[idx].getBound();
}

/*! Get the partition bound condition of the face of the octant
 * \param[in] idx Local index of the target octant
 * \param[in] iface Index of the face
 * \return Is the face a partition boundary face?
 */
bool
ClassParaTree::getPbound(uint32_t idx, uint8_t iface){
	return m_octree.m_octants[idx].getPbound(iface);
}

/*! Get the partition bound condition of the face of the octant
 * \param[in] idx Local index of the target octant
 * \return Is the octant a partition boundary octant?
 */
bool
ClassParaTree::getPbound(uint32_t idx){
	return m_octree.m_octants[idx].getPbound();
}

#if ENABLE_MPI
/*! Get the nature of an octant.
 * \param[in] idx Local index of target octant.
 * \return Is octant ghost?
 */
bool
ClassParaTree::getIsGhost(uint32_t idx){
	return (findOwner(m_octree.m_octants[idx].computeMorton()) != m_rank);
};
#endif

/*! Get if the octant is new after refinement.
 * \param[in] idx Local index of target octant.
 * \return Is octant new?
 */
bool
ClassParaTree::getIsNewR(uint32_t idx){
	return m_octree.m_octants[idx].getIsNewR();
};

/*! Get if the octant is new after coarsening.
 * \param[in] idx Local index of target octant.
 * \return Is octant new?
 */
bool
ClassParaTree::getIsNewC(uint32_t idx){
	return m_octree.m_octants[idx].getIsNewC();
};

/*! Get the global index of an octant.
 * \param[in] idx Local index of target octant.
 * \return Global index of octant.
 */
uint64_t
ClassParaTree::getGlobalIdx(uint32_t idx){
	if (m_rank){
		return m_partitionRangeGlobalIdx[m_rank-1] + uint64_t(idx + 1);
	}
	else{
		return uint64_t(idx);
	};
	return m_globalNumOctants;
};

/*! Get the global index of a ghost octant.
 * \param[in] idx Local index of target ghost octant.
 * \return Global index of ghost octant.
 */
uint64_t
ClassParaTree::getGhostGlobalIdx(uint32_t idx){
	if (idx<m_octree.m_sizeGhosts){
		return m_octree.m_globalIdxGhosts[idx];
	};
	return uint64_t(m_octree.m_sizeGhosts);
};


/*! Set the refinement marker of an octant.
 * \param[in] idx Local index of target octant.
 * \param[in] marker Refinement marker of octant (n=n refinement in adapt, -n=n coarsening in adapt, default=0).
 */
void
ClassParaTree::setMarker(uint32_t idx, int8_t marker){
	m_octree.setMarker(idx, marker);
};

/*! Set the balancing condition of an octant.
 * \param[in] idx Local index of target octant.
 * \param[in] balance Has octant to be 2:1 balanced in adapting procedure?
 */
void
ClassParaTree::setBalance(uint32_t idx, bool balance){
	m_octree.setBalance(idx, !balance);
};

// =================================================================================== //
// POINTER BASED METHODS
// =================================================================================== //

/*! Get the coordinates of an octant, i.e. the coordinates of its node 0.
 * \param[in] oct Pointer to the target octant
 * \return Coordinates of node 0.
 */
darray3
ClassParaTree::getCoordinates(ClassOctant* oct) {
	return m_trans.mapCoordinates(oct->getCoordinates());
}

/*! Get the coordinates of an octant, i.e. the coordinates of its node 0.
 * \param[in] oct Pointer to the target octant
 * \return Coordinate X of node 0.
 */
double
ClassParaTree::getX(ClassOctant* oct) {
	return m_trans.mapX(oct->getX());
}

/*! Get the coordinates of an octant, i.e. the coordinates of its node 0.
 * \param[in] oct Pointer to the target octant
 * \return Coordinate Y of node 0.
 */
double
ClassParaTree::getY(ClassOctant* oct) {
	return m_trans.mapY(oct->getY());
}

/*! Get the coordinates of an octant, i.e. the coordinates of its node 0.
 * \param[in] oct Pointer to the target octant
 * \return Coordinate Z of node 0.
 */
double
ClassParaTree::getZ(ClassOctant* oct) {
	return m_trans.mapZ(oct->getZ());
}

/*! Get the size of an octant, i.e. the side length.
 * \param[in] oct Pointer to the target octant
 * \return Area of octant.
 */
double
ClassParaTree::getSize(ClassOctant* oct) {
	return m_trans.mapSize(oct->getSize(m_global.m_maxLevel));
}

/*! Get the area of an octant (for 2D case the same value of getSize).
 * \param[in] oct Pointer to the target octant
 * \return Area of octant.
 */
double
ClassParaTree::getArea(ClassOctant* oct) {
	return m_trans.mapArea(oct->getArea(m_global.m_maxLevel));
}

/*! Get the volume of an octant.
 * \param[in] oct Pointer to the target octant
 * \return Volume of octant.
 */
double
ClassParaTree::getVolume(ClassOctant* oct) {
	return m_trans.mapVolume(oct->getVolume(m_global.m_maxLevel));
}

/*! Get the coordinates of the center of an octant.
 * \param[in] oct Pointer to the target octant
 * \param[out] center Coordinates of the center of octant.
 */
void
ClassParaTree::getCenter(ClassOctant* oct, darray3& center) {
	darray3 center_ = oct->getCenter(m_global.m_maxLevel);
	m_trans.mapCenter(center_, center);
}

/*! Get the coordinates of the center of an octant.
 * \param[in] oct Pointer to the target octant
 * \return center Coordinates of the center of octant.
 */
darray3
ClassParaTree::getCenter(ClassOctant* oct) {
	darray3 center;
	darray3 center_ = oct->getCenter(m_global.m_maxLevel);
	m_trans.mapCenter(center_, center);
	return center;
}

/*! Get the coordinates of the center of a face of an octant.
 * \param[in] oct Pointer to the target octant
 * \param[in] iface Index of the target face.
 * \return center Coordinates of the center of the iface-th face af octant.
 */
darray3
ClassParaTree::getFaceCenter(ClassOctant* oct, uint8_t iface) {
	darray3 center;
	darray3 center_ = oct->getFaceCenter(iface, m_global.m_maxLevel);
	m_trans.mapCenter(center_, center);
	return center;
}

/*! Get the coordinates of the center of a face of an octant.
 * \param[in] oct Pointer to the target octant
 * \param[in] iface Index of the target face.
 * \param[out] center Coordinates of the center of the iface-th face af octant.
 */
void
ClassParaTree::getFaceCenter(ClassOctant* oct, uint8_t iface, darray3& center) {
	darray3 center_ = oct->getFaceCenter(iface, m_global.m_maxLevel);
	m_trans.mapCenter(center_, center);
}

/*! Get the coordinates of single node of an octant.
 * \param[in] oct Pointer to the target octant
 * \param[in] inode Index of the target node.
 * \return center Coordinates of the center of the iface-th face af octant.
 */
darray3
ClassParaTree::getNode(ClassOctant* oct, uint8_t inode) {
	darray3 node;
	u32array3 node_ = oct->getNode(inode, m_global.m_maxLevel);
	m_trans.mapNode(node_, node);
	return node;
}

/*! Get the coordinates of the center of a face of an octant.
 * \param[in] oct Pointer to the target octant
 * \param[in] iface Index of the target face.
 * \param[out] center Coordinates of the center of the iface-th face af octant.
 */
void
ClassParaTree::getNode(ClassOctant* oct, uint8_t inode, darray3& node) {
	u32array3 node_ = oct->getNode(inode, m_global.m_maxLevel);
	m_trans.mapNode(node_, node);
}

/*! Get the coordinates of the nodes of an octant.
 * \param[in] oct Pointer to the target octant
 * \param[out] nodes Coordinates of the nodes of octant.
 */
void
ClassParaTree::getNodes(ClassOctant* oct, darr3vector & nodes) {
	u32arr3vector nodes_;
	oct->getNodes(nodes_, m_global.m_maxLevel);
	m_trans.mapNodes(nodes_, nodes);
}

/*! Get the coordinates of the nodes of an octant.
 * \param[in] oct Pointer to the target octant
 * \return nodes Coordinates of the nodes of octant.
 */
darr3vector
ClassParaTree::getNodes(ClassOctant* oct){
	darr3vector nodes;
	u32arr3vector nodes_;
	oct->getNodes(nodes_, m_global.m_maxLevel);
	m_trans.mapNodes(nodes_, nodes);
	return nodes;
}

/*! Get the normal of a face of an octant.
 * \param[in] oct Pointer to the target octant
 * \param[in] iface Index of the face for normal computing.
 * \param[out] normal Coordinates of the normal of face.
 */
void
ClassParaTree::getNormal(ClassOctant* oct, uint8_t & iface, darray3 & normal) {
	i8array3 normal_;
	oct->getNormal(iface, normal_, m_global.m_normals, m_global.m_maxLevel);
	m_trans.mapNormals(normal_, normal);
}

/*! Get the normal of a face of an octant.
 * \param[in] oct Pointer to the target octant
 * \param[in] iface Index of the face for normal computing.
 * \return normal Coordinates of the normal of face.
 */
darray3
ClassParaTree::getNormal(ClassOctant* oct, uint8_t & iface){
	darray3 normal;
	i8array3 normal_;
	oct->getNormal(iface, normal_, m_global.m_normals, m_global.m_maxLevel);
	m_trans.mapNormals(normal_, normal);
	return normal;
}

/*! Get the refinement marker of an octant.
 * \param[in] oct Pointer to the target octant
 * \return Marker of octant.
 */
int8_t
ClassParaTree::getMarker(ClassOctant* oct){
	return oct->getMarker();
};

/*! Get the level of an octant.
 * \param[in] oct Pointer to the target octant
 * \return Level of octant.
 */
uint8_t
ClassParaTree::getLevel(ClassOctant* oct){
	return oct->getLevel();
};

/** Compute the Morton index of an octant (without level).
 * \param[in] oct Pointer to the target octant
 * \return morton Morton index of the octant.
 */
uint64_t
ClassParaTree::computeMorton(ClassOctant* oct){
	return oct->computeMorton();
};

/*! Get the balancing condition of an octant.
 * \param[in] oct Pointer to the target octant
 * \return Has octant to be balanced?
 */
bool
ClassParaTree::getBalance(ClassOctant* oct){
	return !oct->getBalance();
};

/*! Get the bound condition of the face of the octant
 * \param[in] oct Pointer to the target octant
 * \param[in] iface Index of the face
 * \return Is the face a boundary face?
 */
bool
ClassParaTree::getBound(ClassOctant* oct, uint8_t iface){
	return oct->getBound(iface);
}

/*! Get the bound condition of the octant
 * \param[in] oct Pointer to the target octant
 * \return Is the octant a boundary octant?
 */
bool
ClassParaTree::getBound(ClassOctant* oct){
	return oct->getBound();
}

/*! Get the partition bound condition of the face of the octant
 * \param[in] oct Pointer to the target octant
 * \param[in] iface Index of the face
 * \return Is the face a partition boundary face?
 */
bool
ClassParaTree::getPbound(ClassOctant* oct, uint8_t iface){
	return oct->getPbound(iface);
}

/*! Get the partition bound condition of the face of the octant
 * \param[in] oct Pointer to the target octant
 * \return Is the octant a partition boundary octant?
 */
bool
ClassParaTree::getPbound(ClassOctant* oct){
	return oct->getPbound();
}

/*! Get if the octant is new after refinement.
 * \param[in] oct Pointer to the target octant
 * \return Is octant new?
 */
bool
ClassParaTree::getIsNewR(ClassOctant* oct){
	return oct->getIsNewR();
};

/*! Get if the octant is new after coarsening.
 * \param[in] oct Pointer to the target octant
 * \return Is octant new?
 */
bool
ClassParaTree::getIsNewC(ClassOctant* oct){
	return oct->getIsNewC();
};

/*! Set the refinement marker of an octant.
 * \param[in] oct Pointer to the target octant
 * \param[in] marker Refinement marker of octant (n=n refinement in adapt, -n=n coarsening in adapt, default=0).
 */
void
ClassParaTree::setMarker(ClassOctant* oct, int8_t marker){
	oct->setMarker(marker);
};

/*! Set the balancing condition of an octant.
 * \param[in] oct Pointer to the target octant
 * \param[in] balance Has octant to be 2:1 balanced in adapting procedure?
 */
void
ClassParaTree::setBalance(ClassOctant* oct, bool balance){
	oct->setBalance(!balance);
};

// =================================================================================== //
// LOCAL TREE GET/SET METHODS
// =================================================================================== //

/*! Get the status label of the octree.
 * 	\return Status label of the octree.
 */
uint64_t
ClassParaTree::getStatus(){
	return m_status;
}

/*! Get the local number of octants.
 * \return Local number of octants.
 */
uint32_t
ClassParaTree::getNumOctants() const{
	return m_octree.getNumOctants();
};

/*! Get the local number of ghost octants.
 * \return Local number of ghost octants.
 */
uint32_t
ClassParaTree::getNumGhosts() const{
	return m_octree.getSizeGhost();
};

/** Get the local number of nodes.
 * \return Local total number of nodes.
 */
uint32_t
ClassParaTree::getNumNodes() const{
	return m_octree.m_nodes.size();
}

/*! Get the local depth of the octree.
 * \return Local depth of the octree.
 */
uint8_t
ClassParaTree::getLocalMaxDepth() const{
	return m_octree.getLocalMaxDepth();
};

/*! Get the codimension for 2:1 balancing
 * \return Maximum codimension of the entity through which the 2:1 balance is performed.
 */
uint8_t
ClassParaTree::getBalanceCodimension() const{
	return m_octree.getBalanceCodim();
};

/*! Get the coordinates of the extreme points of a bounding box containing the local tree
 *  \param[out] P0 Array with coordinates of the first point (lowest coordinates);
 *  \param[out] P1 Array with coordinates of the last point (highest coordinates).
 */
void
ClassParaTree::getBoundingBox(darray3 & P0, darray3 & P1){
	darray3		cnode, cnode0, cnode1;
	uint32_t 	nocts = getNumOctants();
	uint32_t	id = 0;
	cnode0 = getNode(id, 0);
	id = nocts-1;
	cnode1 = getNode(id, m_global.m_nnodes-1);
	for (uint8_t i=0; i<3; i++){
		P0[i] = cnode0[i];
		P1[i] = cnode1[i];
	}
	for (uint32_t idx=0; idx<m_global.m_nnodes; idx++){
		for (uint8_t inode=0; inode<nocts; inode++){
			cnode = getNode(idx, inode);
			for (uint8_t i=0; i<3; i++){
				P0[i] = min(P0[i], cnode[i]);
				P1[i] = max(P1[i], cnode[i]);
			}
		}
	}
};

/*!Get the first possible descendant with maximum refinement level of the local tree.
 * \return Constant reference to the first finest descendant of the local tree.
 */
const
ClassOctant & ClassParaTree::getFirstDesc() const{
	return m_octree.getFirstDesc();
};

/*!Get the last possible descendant with maximum refinement level of the local tree.
 * \return Constant reference to the last finest descendant of the local tree.
 */
const
ClassOctant & ClassParaTree::getLastDesc() const{
	return m_octree.getLastDesc();
};

/*!Get the morton index of the last possible descendant with maximum refinement level of a target octant.
 * \param[in] idx Local index of the target octant.
 * \return Morton index of the last finest descendant of the target octant.
 */
uint64_t
ClassParaTree::getLastDescMorton(uint32_t idx) {
	return m_octree.m_octants[idx].buildLastDesc(m_global.m_maxLevel).computeMorton();
};

/*! Set the codimension for 2:1 balancing
 * \param[in] Maximum codimension of the entity through which the 2:1 balance is performed (1 = 2:1 balance through edges (default); 2 = 2:1 balance through nodes and edges).
 */
void
ClassParaTree::setBalanceCodimension(uint8_t b21codim){
	m_octree.setBalanceCodim(b21codim);
};

// =================================================================================== //
// INTERSECTION GET/SET METHODS
// =================================================================================== //

/*! Get the local number of intersections.
 * \return Local number of intersections.
 */
uint32_t
ClassParaTree::getNumIntersections() {
	return m_octree.m_intersections.size();
}

/*! Get a pointer to target intersection.
 * \param[in] idx Local index of intersection.
 * \return Pointer to target intersection.
 */
ClassIntersection*
ClassParaTree::getIntersection(uint32_t idx) {
	if (idx < m_octree.m_intersections.size()){
		return &m_octree.m_intersections[idx];
	}
	return NULL;
}

/*! Get the level of an intersection.
 * \param[in] inter Pointer to target intersection.
 * \return Level of intersection.
 */
uint8_t
ClassParaTree::getLevel(ClassIntersection* inter) {
	if(inter->m_finer && inter->m_isghost)
		return m_octree.extractGhostOctant(inter->m_owners[inter->m_finer]).getLevel();
	else
		return m_octree.extractOctant(inter->m_owners[inter->m_finer]).getLevel();
}

/*! Get the finer owner octant of an intersection.
 * \param[in] inter Pointer to target intersection.
 * \return The finer octant of the owners of intersection (false/true = 0/1).
 */
bool
ClassParaTree::getFiner(ClassIntersection* inter) {
	return inter->m_finer;
}

/*! Get if an intersection is a boundary domain intersection.
 * \param[in] inter Pointer to target intersection.
 * \return Boundary or not boundary?.
 */
bool
ClassParaTree::getBound(ClassIntersection* inter) {
	return inter->getBound();
}

/*! Get if an intersection is an intersection between an internal and a ghost element.
 * \param[in] inter Pointer to target intersection.
 * \return Ghost or not ghost?.
 */
bool
ClassParaTree::getIsGhost(ClassIntersection* inter) {
	return inter->getIsGhost();
}

/*! Get if an intersection is a boundary intersection for a process.
 * \param[in] inter Pointer to target intersection.
 * \return Process boundary or not boundary?.
 */
bool
ClassParaTree::getPbound(ClassIntersection* inter) {
	return inter->getPbound();
}

/*! Get the face index of an intersection.
 * \param[in] inter Pointer to target intersection.
 * \return Face index of the first octant owner of intersection (owners[0]).
 */
uint8_t
ClassParaTree::getFace(ClassIntersection* inter) {
	return inter->m_iface;
}

/*! Get the owner octants of an intersection.
 * \param[in] inter Pointer to target intersection.
 * \return A couple of octants owners of intersection.
 */
u32vector
ClassParaTree::getOwners(ClassIntersection* inter) {
	u32vector owners(2);
	owners[0] = inter->m_owners[0];
	owners[1] = inter->m_owners[1];
	return owners;
}

/*! Get the owner octant of an intersection with inner normal.
 * \param[in] inter Pointer to target intersection.
 * \return Index of the octant owner with inner normal.
 */
uint32_t
ClassParaTree::getIn(ClassIntersection* inter) {
	return inter->getIn();
}

/*! Get the owner octant of an intersection with outer normal.
 * \param[in] inter Pointer to target intersection.
 * \return Index of the octant owner with outer normal.
 */
uint32_t
ClassParaTree::getOut(ClassIntersection* inter) {
	return inter->getOut();
}

/*! Get the size of an intersection.
 * \param[in] inter Pointer to target intersection.
 * \return Size of intersection.
 */
double
ClassParaTree::getSize(ClassIntersection* inter) {
	uint32_t Size;
	if(inter->m_finer && inter->m_isghost)
		Size = m_octree.extractGhostOctant(inter->m_owners[inter->m_finer]).getSize(m_global.m_maxLevel);
	else
		Size = m_octree.extractOctant(inter->m_owners[inter->m_finer]).getSize(m_global.m_maxLevel);
	return m_trans.mapSize(Size);
}

/*! Get the area of an intersection (for 2D case the same value of getSize).
 * \param[in] inter Pointer to target intersection.
 * \return Area of intersection.
 */
double
ClassParaTree::getArea(ClassIntersection* inter) {
	uint32_t Area;
	if(inter->m_finer && inter->m_isghost)
		Area = m_octree.extractGhostOctant(inter->m_owners[1]).getArea(m_global.m_maxLevel);
	else
		Area = m_octree.extractOctant(inter->m_owners[inter->m_finer]).getArea(m_global.m_maxLevel);
	return m_trans.mapSize(Area);
}

/*! Get the coordinates of the center of an intersection.
 * \param[in] inter Pointer to target intersection.
 * \param[out] center Coordinates of the center of intersection.
 */
darray3
ClassParaTree::getCenter(ClassIntersection* inter){
	darray3 center;
	ClassOctant oct;
	if(inter->m_finer && inter->m_isghost)
		oct = m_octree.extractGhostOctant(inter->m_owners[inter->m_finer]);
	else
		oct = m_octree.extractOctant(inter->m_owners[inter->m_finer]);
	darray3  center_ = oct.getCenter(m_global.m_maxLevel);
	int sign = ( int(2*((inter->m_iface)%2)) - 1);
	double deplace = double (sign * int(oct.getSize(m_global.m_maxLevel))) / 2;
	center_[inter->m_iface/2] = uint32_t(int(center_[inter->m_iface/2]) + deplace);
	m_trans.mapCenter(center_, center);
	return center;
}

/*! Get the coordinates of the nodes of an intersection.
 * \param[in] oct Pointer to target intersection.
 * \return nodes Coordinates of the nodes of intersection.
 */
darr3vector
ClassParaTree::getNodes(ClassIntersection* inter){
	darr3vector nodes;
	ClassOctant oct;
	if(inter->m_finer && inter->m_isghost)
		oct = m_octree.extractGhostOctant(inter->m_owners[inter->m_finer]);
	else
		oct = m_octree.extractOctant(inter->m_owners[inter->m_finer]);
	uint8_t iface = inter->m_iface;
	u32arr3vector nodes_all;
	oct.getNodes(nodes_all, m_global.m_maxLevel);
	u32arr3vector nodes_(m_global.m_nnodesPerFace);
	for (int i=0; i<m_global.m_nnodesPerFace; i++){
		for (int j=0; j<3; j++){
			nodes_[i][j] = nodes_all[m_global.m_faceNode[iface][i]][j];
		}
	}
	m_trans.mapNodesIntersection(nodes_, nodes);
	return nodes;
}

/*! Get the normal of an intersection.
 * \param[in] oct Pointer to target intersection.
 * \param[out] normal Coordinates of the normal of intersection.
 */
darray3
ClassParaTree::getNormal(ClassIntersection* inter){
	darray3 normal;
	ClassOctant oct;
	if(inter->m_finer && inter->m_isghost)
		oct = m_octree.extractGhostOctant(inter->m_owners[inter->m_finer]);
	else
		oct = m_octree.extractOctant(inter->m_owners[inter->m_finer]);
	uint8_t iface = inter->m_iface;
	i8array3 normal_;
	oct.getNormal(iface, normal_, m_global.m_normals, m_global.m_maxLevel);
	m_trans.mapNormals(normal_, normal);
	return normal;
}

// =================================================================================== //
// OTHER GET/SET METHODS
// =================================================================================== //

/** Get an octant as pointer to the target octant.
 * \param[in] idx Local index of target octant.
 * \return Pointer to target octant.
 */
ClassOctant*
ClassParaTree::getOctant(uint32_t idx) {
	if (idx < m_octree.getNumOctants()){
		return &m_octree.m_octants[idx] ;
	}
	return NULL;
};

/** Get a ghost octant as pointer to the target octant.
 * \param[in] idx Local index (in ghosts structure) of target ghost octant.
 * \return Pointer to target ghost octant.
 */
ClassOctant*
ClassParaTree::getGhostOctant(uint32_t idx) {
	if (idx < m_octree.getSizeGhost()){
		return &m_octree.m_ghosts[idx] ;
	}
	return NULL;
};

/*! Get the global index of an octant.
 * \param[in] idx Local index of target octant.
 * \return Global index of octant.
 */
uint64_t
ClassParaTree::getGlobalIdx(ClassOctant* oct){
#if ENABLE_MPI
	if (getIsGhost(oct)){
		uint32_t idx = m_octree.findGhostMorton(oct->computeMorton());
		return m_octree.m_globalIdxGhosts[idx];
	}
#endif
	uint32_t idx = m_octree.findMorton(oct->computeMorton());
	if (m_rank){
		return m_partitionRangeGlobalIdx[m_rank-1] + uint64_t(idx + 1);
	}
	return uint64_t(idx);
};

/*! Get the local index of an octant.
 * \param[in] oct Pointer to target octant.
 * \return Local index of octant.
 */
uint32_t
ClassParaTree::getIdx(ClassOctant* oct){
#if ENABLE_MPI
	if (getIsGhost(oct)){
		return m_octree.findGhostMorton(oct->computeMorton());
	}
#endif
	return m_octree.findMorton(oct->computeMorton());
};

/*! Get the local index of an octant.
 * \param[in] oct Target octant.
 * \return Local index of octant.
 */
uint32_t
ClassParaTree::getIdx(ClassOctant oct){
#if ENABLE_MPI
	if (getIsGhost(oct)){
		return m_octree.findGhostMorton(oct.computeMorton());
	}
	else{
#endif
		return m_octree.findMorton(oct.computeMorton());
#if ENABLE_MPI
	};
#endif
	return m_octree.getNumOctants();
};

#if ENABLE_MPI
/*! Get the nature of an octant.
 * \param[in] oct Pointer to target octant.
 * \return Is octant ghost?
 */
bool
ClassParaTree::getIsGhost(ClassOctant* oct){
	if (m_serial)
		return false;
	return (findOwner(oct->computeMorton()) != m_rank);
};

/*! Get the nature of an octant.
 * \param[in] oct Target octant.
 * \return Is octant ghost?
 */
bool
ClassParaTree::getIsGhost(ClassOctant oct){
	if (m_serial)
		return false;
	return (findOwner(oct.computeMorton()) != m_rank);
};
#endif

// =================================================================================== //
// PRIVATE GET/SET METHODS
// =================================================================================== //

/*! Set the first finer descendant of the local tree.
 */
void
ClassParaTree::setFirstDesc(){
	m_octree.setFirstDesc();
};

/*! Set the last finer descendant of the local tree.
 */
void
ClassParaTree::setLastDesc(){
	m_octree.setLastDesc();
};

// =================================================================================== //
// OTHER METHODS												    			   //
// =================================================================================== //

// =================================================================================== //
// OTHER OCTANT BASED METHODS												    			   //
// =================================================================================== //

/** Finds neighbours of octant through iface in vector octants.
 * Returns a vector (empty if iface is a bound face) with the index of neighbours
 * in their structure (octants or ghosts) and sets isghost[i] = true if the
 * i-th neighbour is ghost in the local tree.
 * \param[in] idx Index of current octant
 * \param[in] iface Index of face/edge/node passed through for neighbours finding
 * \param[in] codim Codimension of the iface-th entity 1=edge, 2=node
 * \param[out] neighbours Vector of neighbours indices in octants/ghosts structure
 * \param[out] isghost Vector with boolean flag; true if the respective octant in neighbours is a ghost octant. Can be ignored in serial runs. */
void
ClassParaTree::findNeighbours(uint32_t idx, uint8_t iface, uint8_t codim, u32vector & neighbours, vector<bool> & isghost){

	bool	Fedge = ((codim>1) && (m_dim==3));
	bool	Fnode = (codim == m_dim);

	if (codim == 1){
		m_octree.findNeighbours(idx, iface, neighbours, isghost);
	}
	else if (Fedge){
		m_octree.findEdgeNeighbours(idx, iface, neighbours, isghost);
	}
	else if (Fnode){
		m_octree.findNodeNeighbours(idx, iface, neighbours, isghost);
	}
	else {
		neighbours.clear();
		isghost.clear();
	}
};

/** Finds neighbours of octant through iface in vector octants.
 * Returns a vector (empty if iface is a bound face) with the index of neighbours
 * in their structure (octants or ghosts) and sets isghost[i] = true if the
 * i-th neighbour is ghost in the local tree.
 * \param[in] oct Pointer to current octant
 * \param[in] iface Index of face/edge/node passed through for neighbours finding
 * \param[in] codim Codimension of the iface-th entity 1=edge, 2=node
 * \param[out] neighbours Vector of neighbours indices in octants/ghosts structure
 * \param[out] isghost Vector with boolean flag; true if the respective octant in neighbours is a ghost octant. Can be ignored in serial runs. */
void
ClassParaTree::findNeighbours(ClassOctant* oct, uint8_t iface, uint8_t codim, u32vector & neighbours, vector<bool> & isghost){

	bool	Fedge = ((codim>1) && (m_dim==3));
	bool	Fnode = (codim == m_dim);

	if (codim == 1){
		m_octree.findNeighbours(oct, iface, neighbours, isghost);
	}
	else if (Fedge){
		m_octree.findEdgeNeighbours(oct, iface, neighbours, isghost);
	}
	else if (Fnode){
		m_octree.findNodeNeighbours(oct, iface, neighbours, isghost);
	}
	else {
		neighbours.clear();
		isghost.clear();
	}

};

/** Finds neighbours of ghost octant through iface in vector octants.
 * Returns a vector (empty if iface is a bound face) with the index of neighbours
 * in their structure ( only local octants ).
 * \param[in] idx Index of current octant
 * \param[in] iface Index of face/edge/node passed through for neighbours finding
 * \param[in] codim Codimension of the iface-th entity 1=edge, 2=node
 * \param[out] neighbours Vector of neighbours indices in octants/ghosts structure
 */
void
ClassParaTree::findGhostNeighbours(uint32_t idx, uint8_t iface, uint8_t codim, u32vector & neighbours){

	bool	Fedge = ((codim>1) && (m_dim==3));
	bool	Fnode = (codim == m_dim);

	if (codim == 1){
		m_octree.findGhostNeighbours(idx, iface, neighbours);
	}
	else if (Fedge){
		m_octree.findGhostEdgeNeighbours(idx, iface, neighbours);
	}
	else if (Fnode){
		m_octree.findGhostNodeNeighbours(idx, iface, neighbours);
	}
	else {
		neighbours.clear();
	}
};

/** Get the octant owner of an input point.
 * \param[in] point Coordinates of target point.
 * \return Pointer to octant owner of target point (=NULL if point is outside of the domain).
 */
ClassOctant*
ClassParaTree::getPointOwner(dvector & point){
	uint32_t noctants = m_octree.m_octants.size();
	uint32_t idxtry = noctants/2;
	uint32_t x, y, z;
	uint64_t morton, mortontry;
	int powner = 0;

	x = m_trans.mapX(point[0]);
	y = m_trans.mapX(point[1]);
	z = m_trans.mapX(point[2]);
	if ((x > m_global.m_maxLength) || (y > m_global.m_maxLength) || (z > m_global.m_maxLength))
		return NULL;

	if (x == m_global.m_maxLength) x = x - 1;
	if (y == m_global.m_maxLength) y = y - 1;
	if (z == m_global.m_maxLength) z = z - 1;
	morton = mortonEncode_magicbits(x,y,z);

	powner = 0;
	if (!m_serial) powner = findOwner(morton);

	if ((powner!=m_rank) && (!m_serial))
		return NULL;

	int32_t jump = idxtry;
	while(abs(jump) > 0){
		mortontry = m_octree.m_octants[idxtry].computeMorton();
		jump = ((mortontry<morton)-(mortontry>morton))*abs(jump)/2;
		idxtry += jump;
		if (idxtry > noctants-1){
			if (jump > 0){
				idxtry = noctants - 1;
				jump = 0;
			}
			else if (jump < 0){
				idxtry = 0;
				jump = 0;
			}
		}
	}
	if(m_octree.m_octants[idxtry].computeMorton() == morton){
		return &m_octree.m_octants[idxtry];
	}
	else{
		// Step until the mortontry lower than morton (one idx of distance)
		{
			while(m_octree.m_octants[idxtry].computeMorton() < morton){
				idxtry++;
				if(idxtry > noctants-1){
					idxtry = noctants-1;
					break;
				}
			}
			while(m_octree.m_octants[idxtry].computeMorton() > morton){
				idxtry--;
				if(idxtry > noctants-1){
					idxtry = 0;
					break;
				}
			}
		}
		return &m_octree.m_octants[idxtry];
	}

};

/** Get the octant owner of an input point.
 * \param[in] point Coordinates of target point.
 * \return Index of octant owner of target point (max uint32_t representable if point outside of the domain).
 */
uint32_t
ClassParaTree::getPointOwnerIdx(dvector & point){
	uint32_t noctants = m_octree.m_octants.size();
	uint32_t idxtry = noctants/2;
	uint32_t x, y, z;
	uint64_t morton, mortontry;
	int powner = 0;

	x = m_trans.mapX(point[0]);
	y = m_trans.mapY(point[1]);
	z = m_trans.mapZ(point[2]);

	if ((x > m_global.m_maxLength) || (y > m_global.m_maxLength) || (z > m_global.m_maxLength)
			|| (point[0] < m_trans.m_origin[0]) || (point[1] < m_trans.m_origin[1]) || (point[2] < m_trans.m_origin[2])){
		return -1;
	}

	if (x == m_global.m_maxLength) x = x - 1;
	if (y == m_global.m_maxLength) y = y - 1;
	if (z == m_global.m_maxLength) z = z - 1;
	morton = mortonEncode_magicbits(x,y,z);


	powner = 0;
	if(!m_serial) powner = findOwner(morton);

	if ((powner!=m_rank) && (!m_serial))
		return -1;

	int32_t jump = idxtry;
	while(abs(jump) > 0){

		mortontry = m_octree.m_octants[idxtry].computeMorton();
		jump = ((mortontry<morton)-(mortontry>morton))*abs(jump)/2;
		idxtry += jump;
		if (idxtry > noctants-1){
			if (jump > 0){
				idxtry = noctants - 1;
				jump = 0;
			}
			else if (jump < 0){
				idxtry = 0;
				jump = 0;
			}
		}
	}
	if(m_octree.m_octants[idxtry].computeMorton() == morton){
		return idxtry;
	}
	else{
		// Step until the mortontry lower than morton (one idx of distance)
		{
			while(m_octree.m_octants[idxtry].computeMorton() < morton){
				idxtry++;
				if(idxtry > noctants-1){
					idxtry = noctants-1;
					break;
				}
			}
			while(m_octree.m_octants[idxtry].computeMorton() > morton){
				idxtry--;
				if(idxtry > noctants-1){
					idxtry = 0;
					break;
				}
			}
		}
		return idxtry;
	}
};

/** Get mapping info of an octant after an adapting with tracking changes.
 * \param[in] idx Index of new octant.
 * \param[out] mapper Mapper from new octants to old octants. I.e. mapper[i] = j -> the i-th octant after adapt was in the j-th position before adapt;
 * if the i-th octant is new after refinement the j-th old octant was the father of the new octant;
 * if the i-th octant is new after coarsening the j-th old octant was a child of the new octant (mapper size = 4).
 * \param[out] isghost Info on ghostness of old octants.
 * I.e. isghost[i] = true/false -> the mapper[i] = j-th old octant was a local/ghost octant.
 */
void
ClassParaTree::getMapping(uint32_t & idx, u32vector & mapper, vector<bool> & isghost){

	uint32_t	i, nocts = getNumOctants();
	uint32_t	nghbro = m_octree.m_lastGhostBros.size();;

	mapper.clear();
	isghost.clear();

	mapper.push_back(m_mapIdx[idx]);
	isghost.push_back(false);
	if (getIsNewC(idx)){
		if (idx < nocts-1 || !nghbro){
			for (i=1; i<m_global.m_nchildren; i++){
				mapper.push_back(m_mapIdx[idx]+i);
				isghost.push_back(false);
			}
		}
		else if (idx == nocts-1 && nghbro){
			for (i=1; i<m_global.m_nchildren-nghbro; i++){
				mapper.push_back(m_mapIdx[idx]+i);
				isghost.push_back(false);
			}
			for (i=0; i<nghbro; i++){
				mapper.push_back(m_octree.m_lastGhostBros[i]);
				isghost.push_back(true);
			}
		}
	}

};

// =================================================================================== //
// OTHER PARATREE BASED METHODS												    			   //
// =================================================================================== //

/** Adapt the octree mesh with user setup for markers and 2:1 balancing conditions.
 * \param[in] mapper_flag True/False if you want/don't want to track the changes in structure octant by a mapper.
 * \return Boolean if adapt has done something.
 */
bool
ClassParaTree::adapt(bool mapper_flag){

	bool done = false;

		done = private_adapt_mapidx(mapper_flag);
		m_status += done;
		return done;

};

/** Adapt the octree mesh refining all the octants by one level.
 * Optionally track the changes in structure octant by a mapper.
 * \param[in] mapper_flag True/false for tracking/not tracking the changes in structure octant .
 */
bool
ClassParaTree::adaptGlobalRefine(bool mapper_flag) {
	//TODO recoding for adapting with abs(marker) > 1
	bool globalDone = false, localDone = false;
	uint32_t nocts = m_octree.getNumOctants();
	vector<ClassOctant>::iterator iter, iterend = m_octree.m_octants.end();

	for (iter = m_octree.m_octants.begin(); iter != iterend; iter++){
		iter->m_info[12] = false;
		iter->m_info[13] = false;
		iter->m_info[15] = false;
	}

	m_mapIdx.clear();
	if (mapper_flag){
		// m_mapIdx init
		m_mapIdx.resize(nocts);
		u32vector(m_mapIdx).swap(m_mapIdx);

		for (uint32_t i=0; i<nocts; i++){
			m_mapIdx[i] = i;
		}
	}
#if ENABLE_MPI
	if(m_serial){
#endif
		m_log.writeLog("---------------------------------------------");
		m_log.writeLog(" ADAPT (Global Refine)");
		m_log.writeLog(" ");

		m_log.writeLog(" ");
		m_log.writeLog(" Initial Number of octants	:	" + to_string(static_cast<unsigned long long>(m_octree.getNumOctants())));

		// Refine
		if (mapper_flag){
			while(m_octree.globalRefine(m_mapIdx));
		}
		else{
			while(m_octree.globalRefine(m_mapIdx));
		}

		if (m_octree.getNumOctants() > nocts)
			localDone = true;
		m_log.writeLog(" Number of octants after Refine	:	" + to_string(static_cast<unsigned long long>(m_octree.getNumOctants())));
		nocts = m_octree.getNumOctants();
		updateAdapt();

#if ENABLE_MPI
		MPI_Barrier(m_comm);
		m_errorFlag = MPI_Allreduce(&localDone,&globalDone,1,MPI::BOOL,MPI_LOR,m_comm);
#endif
		m_log.writeLog(" ");
		m_log.writeLog("---------------------------------------------");
#if ENABLE_MPI
	}
	else{
		m_log.writeLog("---------------------------------------------");
		m_log.writeLog(" ADAPT (Global Refine)");
		m_log.writeLog(" ");

		m_log.writeLog(" ");
		m_log.writeLog(" Initial Number of octants	:	" + to_string(static_cast<unsigned long long>(m_globalNumOctants)));

		// Refine
		if (mapper_flag){
			while(m_octree.globalRefine(m_mapIdx));
		}
		else{
			while(m_octree.globalRefine(m_mapIdx));
		}

		if (m_octree.getNumOctants() > nocts)
			localDone = true;
		updateAdapt();
		setPboundGhosts();
		m_log.writeLog(" Number of octants after Refine	:	" + to_string(static_cast<unsigned long long>(m_globalNumOctants)));
		nocts = m_octree.getNumOctants();

		MPI_Barrier(m_comm);
		m_errorFlag = MPI_Allreduce(&localDone,&globalDone,1,MPI::BOOL,MPI_LOR,m_comm);
		m_log.writeLog(" ");
		m_log.writeLog("---------------------------------------------");
	}
	return globalDone;
#else
	return localDone;
#endif
}

/** Adapt the octree mesh coarsening all the octants by one level.
 * Optionally track the changes in structure octant by a mapper.
 * \param[in] mapper_flag True/false for tracking/not tracking the changes in structure octant .
 */
bool
ClassParaTree::adaptGlobalCoarse(bool mapper_flag) {
	//TODO recoding for adapting with abs(marker) > 1
	bool globalDone = false, localDone = false;
	uint32_t nocts = m_octree.getNumOctants();
	vector<ClassOctant>::iterator iter, iterend = m_octree.m_octants.end();

	for (iter = m_octree.m_octants.begin(); iter != iterend; iter++){
		iter->m_info[12] = false;
		iter->m_info[13] = false;
		iter->m_info[15] = false;
	}

	m_mapIdx.clear();
	if (mapper_flag){
		// m_mapIdx init
		m_mapIdx.resize(nocts);
		u32vector(m_mapIdx).swap(m_mapIdx);

		for (uint32_t i=0; i<nocts; i++){
			m_mapIdx[i] = i;
		}
	}
#if ENABLE_MPI
	if(m_serial){
#endif
		m_log.writeLog("---------------------------------------------");
		m_log.writeLog(" ADAPT (Global Coarse)");
		m_log.writeLog(" ");

		// 2:1 Balance
		balance21(true);

		m_log.writeLog(" ");
		m_log.writeLog(" Initial Number of octants	:	" + to_string(static_cast<unsigned long long>(m_octree.getNumOctants())));

		// Coarse
		if (mapper_flag){
			while(m_octree.globalCoarse(m_mapIdx));
			updateAfterCoarse(m_mapIdx);
			balance21(false);
			while(m_octree.refine(m_mapIdx));
			updateAdapt();
		}
		else{
			while(m_octree.globalCoarse(m_mapIdx));
			updateAfterCoarse();
			balance21(false);
			while(m_octree.refine(m_mapIdx));
			updateAdapt();
		}

		if (m_octree.getNumOctants() < nocts){
			localDone = true;
		}
		nocts = m_octree.getNumOctants();

		m_log.writeLog(" Number of octants after Coarse	:	" + to_string(static_cast<unsigned long long>(nocts)));
#if ENABLE_MPI
		MPI_Barrier(m_comm);
		m_errorFlag = MPI_Allreduce(&localDone,&globalDone,1,MPI::BOOL,MPI_LOR,m_comm);
#endif
		m_log.writeLog(" ");
		m_log.writeLog("---------------------------------------------");
#if ENABLE_MPI
	}
	else{
		m_log.writeLog("---------------------------------------------");
		m_log.writeLog(" ADAPT (Global Coarse)");
		m_log.writeLog(" ");

		// 2:1 Balance
		balance21(true);

		m_log.writeLog(" ");
		m_log.writeLog(" Initial Number of octants	:	" + to_string(static_cast<unsigned long long>(m_globalNumOctants)));

		// Coarse
		if (mapper_flag){
			while(m_octree.globalCoarse(m_mapIdx));
			updateAfterCoarse(m_mapIdx);
			setPboundGhosts();
			balance21(false);
			while(m_octree.refine(m_mapIdx));
			updateAdapt();
		}
		else{
			while(m_octree.globalCoarse(m_mapIdx));
			updateAfterCoarse();
			setPboundGhosts();
			balance21(false);
			while(m_octree.refine(m_mapIdx));
			updateAdapt();
		}
		setPboundGhosts();
		if (m_octree.getNumOctants() < nocts){
			localDone = true;
		}
		nocts = m_octree.getNumOctants();

		MPI_Barrier(m_comm);
		m_errorFlag = MPI_Allreduce(&localDone,&globalDone,1,MPI::BOOL,MPI_LOR,m_comm);
		m_log.writeLog(" Number of octants after Coarse	:	" + to_string(static_cast<unsigned long long>(m_globalNumOctants)));
		m_log.writeLog(" ");
		m_log.writeLog("---------------------------------------------");
	}
	return globalDone;
#else
	return localDone;
#endif
}

/** It finds the process owning the element definded by the Morton number passed as argument
 * The Morton number can be computed using the method computeMorton() of ClassOctant.
 * \param[in] morton Morton number of the element you want find the owner of
 * \return Rank of the process owning the element
 */
int
ClassParaTree::findOwner(const uint64_t & morton) {
	int p = -1;
	int length = m_nproc;
	int beg = 0;
	int end = m_nproc -1;
	int seed = m_nproc/2;
	while(beg != end){
		if(morton <= m_partitionLastDesc[seed]){
			end = seed;
			if(morton > m_partitionLastDesc[seed-1])
				beg = seed;
		}
		else{
			beg = seed;
			if(morton <= m_partitionLastDesc[seed+1])
				beg = seed + 1;
		}
		length = end - beg;
		seed = beg + length/2;
	}
	p = beg;
	return p;
}

/** Compute the connectivity of octants and store the coordinates of nodes.
 */
void
ClassParaTree::computeConnectivity() {
	m_octree.computeConnectivity();
}

/** Clear the connectivity of octants.
 */
void
ClassParaTree::clearConnectivity() {
	m_octree.clearConnectivity();
}

/** Update the connectivity of octants.
 */
void
ClassParaTree::updateConnectivity() {
	m_octree.updateConnectivity();
}

/** Get the connectivity of the octants
 * \return Constant reference to the connectivity matrix of noctants*nnodes with
 * the connectivity of each octant (4/8 indices of nodes for 2D/3D case).
 */
const u32vector2D &
ClassParaTree::getConnectivity(){
	return m_octree.m_connectivity;
}

/** Get the local connectivity of an octant
 * \param[in] idx Local index of octant
 * \return Constant reference to the connectivity of the octant
 * (4/8 indices of nodes for 2D/3D case).
 */
const u32vector &
ClassParaTree::getConnectivity(uint32_t idx){
	return m_octree.m_connectivity[idx];
}

/** Get the local connectivity of an octant
 * \param[in] oct Pointer to an octant
 * \return Constant reference to the connectivity of the octant (4/8 indices of nodes for 2D/3D case).
 */
const u32vector &
ClassParaTree::getConnectivity(ClassOctant* oct){
	return m_octree.m_connectivity[getIdx(oct)];
}

/** Get the logical coordinates of the nodes
 * \return Constant reference to the nodes matrix [nnodes*3] with the coordinates of the nodes.
 */
const u32arr3vector &
ClassParaTree::getNodes(){
	return m_octree.m_nodes;
}

/** Get the logical coordinates of a node
 * \param[in] inode Local index of node
 * \return Constant reference to a vector containing the coordinates of the node.
 */
const u32array3 &
ClassParaTree::getNodeLogicalCoordinates(uint32_t inode){
	return m_octree.m_nodes[inode];
}

/** Get the physical coordinates of a node
 * \param[in] inode Local index of node
 * \return Vector with the coordinates of the node.
 */
darray3
ClassParaTree::getNodeCoordinates(uint32_t inode){
	return m_trans.mapCoordinates(m_octree.m_nodes[inode]);
}

/** Compute the connectivity of ghost octants and store the coordinates of nodes.
 */
void
ClassParaTree::computeGhostsConnectivity() {
	m_octree.computeGhostsConnectivity();
}

/** Clear the connectivity of ghost octants.
 */
void
ClassParaTree::clearGhostsConnectivity() {
	m_octree.clearGhostsConnectivity();
}

/** Update the connectivity of ghost octants.
 */
void
ClassParaTree::updateGhostsConnectivity() {
	m_octree.updateGhostsConnectivity();
}

/** Get the connectivity of the ghost octants
 * \return Constant reference to connectivity matrix [nghostoctants*nnodes] with
 * the connectivity of each octant (4/8 indices of nodes for 2D/3D case).
 */
const u32vector2D &
ClassParaTree::getGhostConnectivity(){
	return m_octree.m_ghostsConnectivity;
}

/** Get the local connectivity of a ghost octant
 * \param[in] idx Local index of ghost octant
 * \return Constant reference to the connectivity of the ghost octant
 * (4/8 indices of nodes for 2D/3D case).
 */
const u32vector &
ClassParaTree::getGhostConnectivity(uint32_t idx){
	return m_octree.m_ghostsConnectivity[idx];
}

/** Get the local connectivity of a ghost octant
 * \param[in] oct Pointer to a ghost octant
 * \return Constant reference to the connectivity of the ghost octant
 * (4/8 indices of nodes for 2D/3D case).
 */
const u32vector &
ClassParaTree::getGhostConnectivity(ClassOctant* oct){
	return m_octree.m_ghostsConnectivity[getIdx(oct)];
}

/** Get the logical coordinates of the ghost nodes
 * \return Constant reference to nodes matrix [nghostnodes*3] with the coordinates
 * of the nodes.
 */
const u32arr3vector &
ClassParaTree::getGhostNodes(){
	return m_octree.m_ghostsNodes;
}

/** Get the logical coordinates of a ghost node
 * \param[in] inode Local index of node
 * \return Constant reference to a vector with the coordinates of the node.
 */
const u32array3 &
ClassParaTree::getGhostNodeLogicalCoordinates(uint32_t inode){
	return m_octree.m_ghostsNodes[inode];
}

/** Get the physical coordinates of a ghost node
 * \param[in] inode Local index of node
 * \return Vector with the coordinates of the node.
 */
darray3
ClassParaTree::getGhostNodeCoordinates(uint32_t inode){
	return m_trans.mapCoordinates(m_octree.m_ghostsNodes[inode]);
}

#if ENABLE_MPI
/** Distribute Load-Balancing the octants of the whole tree over
 * the processes of the job following the Morton order.
 * Until loadBalance is not called for the first time the mesh is serial.
 */
void
ClassParaTree::loadBalance(){

	//Write info on log
	m_log.writeLog("---------------------------------------------");
	m_log.writeLog(" LOAD BALANCE ");

	uint32_t* partition = new uint32_t [m_nproc];
	computePartition(partition);
	if(m_serial)
	{
		m_log.writeLog(" ");
		m_log.writeLog(" Initial Serial distribution : ");
		for(int ii=0; ii<m_nproc; ii++){
			m_log.writeLog(" Octants for proc	"+ to_string(static_cast<unsigned long long>(ii))+"	:	" + to_string(static_cast<unsigned long long>(m_partitionRangeGlobalIdx[ii]+1)));
		}

		uint32_t stride = 0;
		for(int i = 0; i < m_rank; ++i)
			stride += partition[i];
		ClassLocalTree::octvector octantsCopy = m_octree.m_octants;
		ClassLocalTree::octvector::const_iterator first = octantsCopy.begin() + stride;
		ClassLocalTree::octvector::const_iterator last = first + partition[m_rank];
		m_octree.m_octants.assign(first, last);

		octvector(m_octree.m_octants).swap(m_octree.m_octants);

		first = octantsCopy.end();
		last = octantsCopy.end();

		//Update and ghosts here
		updateLoadBalance();
		setPboundGhosts();

	}
	else
	{
		m_log.writeLog(" ");
		m_log.writeLog(" Initial Parallel partition : ");
		m_log.writeLog(" Octants for proc	"+ to_string(static_cast<unsigned long long>(0))+"	:	" + to_string(static_cast<unsigned long long>(m_partitionRangeGlobalIdx[0]+1)));
		for(int ii=1; ii<m_nproc; ii++){
			m_log.writeLog(" Octants for proc	"+ to_string(static_cast<unsigned long long>(ii))+"	:	" + to_string(static_cast<unsigned long long>(m_partitionRangeGlobalIdx[ii]-m_partitionRangeGlobalIdx[ii-1])));
		}

		//empty ghosts
		m_octree.m_ghosts.clear();
		m_octree.m_sizeGhosts = 0;
		//compute new partition range globalidx
		uint64_t* newPartitionRangeGlobalidx = new uint64_t[m_nproc];
		for(int p = 0; p < m_nproc; ++p){
			newPartitionRangeGlobalidx[p] = 0;
			for(int pp = 0; pp <= p; ++pp)
				newPartitionRangeGlobalidx[p] += (uint64_t)partition[pp];
			--newPartitionRangeGlobalidx[p];
		}

		//find resident octants local offset lastHead(lh) and firstTail(ft)
		int32_t lh,ft;
		if(m_rank == 0)
			lh = -1;
		else{
			lh = (int32_t)(newPartitionRangeGlobalidx[m_rank-1] + 1 - m_partitionRangeGlobalIdx[m_rank-1] - 1 - 1);
		}
		if(lh < 0)
			lh = - 1;
		else if(lh > (int32_t)(m_octree.m_octants.size() - 1))
			lh = m_octree.m_octants.size() - 1;

		if(m_rank == m_nproc - 1)
			ft = m_octree.m_octants.size();
		else if(m_rank == 0)
			ft = (int32_t)(newPartitionRangeGlobalidx[m_rank] + 1);
		else{
			ft = (int32_t)(newPartitionRangeGlobalidx[m_rank] - m_partitionRangeGlobalIdx[m_rank -1]);
		}
		if(ft > (int32_t)(m_octree.m_octants.size() - 1))
			ft = m_octree.m_octants.size();
		else if(ft < 0)
			ft = 0;

		//compute size Head and size Tail
		uint32_t headSize = (uint32_t)(lh + 1);
		uint32_t tailSize = (uint32_t)(m_octree.m_octants.size() - ft);
		uint32_t headOffset = headSize;
		uint32_t tailOffset = tailSize;

		//build send buffers
		map<int,ClassCommBuffer> sendBuffers;

		//Compute first predecessor and first successor to send buffers to
		int64_t firstOctantGlobalIdx = 0;// offset to compute global index of each octant in every process
		int64_t globalLastHead = (int64_t) lh;
		int64_t globalFirstTail = (int64_t) ft; //lastHead and firstTail in global ordering
		int firstPredecessor = -1;
		int firstSuccessor = m_nproc;
		if(m_rank != 0){
			firstOctantGlobalIdx = (int64_t)(m_partitionRangeGlobalIdx[m_rank-1] + 1);
			globalLastHead = firstOctantGlobalIdx + (int64_t)lh;
			globalFirstTail = firstOctantGlobalIdx + (int64_t)ft;
			for(int pre = m_rank - 1; pre >=0; --pre){
				if((uint64_t)globalLastHead <= newPartitionRangeGlobalidx[pre])
					firstPredecessor = pre;
			}
			for(int post = m_rank + 1; post < m_nproc; ++post){
				if((uint64_t)globalFirstTail <= newPartitionRangeGlobalidx[post] && (uint64_t)globalFirstTail > newPartitionRangeGlobalidx[post-1])
					firstSuccessor = post;
			}
		}
		else if(m_rank == 0){
			firstSuccessor = 1;
		}
		MPI_Barrier(m_comm); //da spostare prima della prima comunicazione

		uint32_t x,y,z;
		uint8_t l;
		int8_t m;
		bool info[17];
		int intBuffer = 0;
		int contatore = 0;
		//build send buffers from Head
		uint32_t nofElementsFromSuccessiveToPrevious = 0;
		if(headSize != 0){
			for(int p = firstPredecessor; p >= 0; --p){
				if(headSize < partition[p]){

					intBuffer = (newPartitionRangeGlobalidx[p] - partition[p] );
					intBuffer = abs(intBuffer);
					nofElementsFromSuccessiveToPrevious = globalLastHead - intBuffer;
					if(nofElementsFromSuccessiveToPrevious > headSize || contatore == 1)
						nofElementsFromSuccessiveToPrevious  = headSize;

					//						int buffSize = nofElementsFromSuccessiveToPrevious * (int)ceil((double)m_global.m_octantBytes / (double)(CHAR_BIT/8));
					int buffSize = nofElementsFromSuccessiveToPrevious * (int)ceil((double)m_global.m_octantBytes / (double)(CHAR_BIT/8));
					sendBuffers[p] = ClassCommBuffer(buffSize,'a',m_comm);
					int pos = 0;
					//for(uint32_t i = 0; i <= (uint32_t)(lh - nofElementsFromSuccessiveToPrevious + 1); ++i){
					for(uint32_t i = (uint32_t)(lh - nofElementsFromSuccessiveToPrevious + 1); i <= (uint32_t)lh; ++i){
						//PACK octants from 0 to lh in sendBuffer[p]
						const ClassOctant & octant = m_octree.m_octants[i];
						x = octant.getX();
						y = octant.getY();
						z = octant.getZ();
						l = octant.getLevel();
						m = octant.getMarker();
						for(int ii = 0; ii < 17; ++ii)
							info[ii] = octant.m_info[ii];
						m_errorFlag = MPI_Pack(&x,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&y,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&z,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&l,1,MPI_UINT8_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&m,1,MPI_INT8_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						for(int j = 0; j < 17; ++j){
							MPI_Pack(&info[j],1,MPI::BOOL,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						}
					}

					if(nofElementsFromSuccessiveToPrevious == headSize)
						break;

					lh -= nofElementsFromSuccessiveToPrevious;
					globalLastHead -= nofElementsFromSuccessiveToPrevious;
					headSize = lh + 1;
					++contatore;

				}
				else{
					nofElementsFromSuccessiveToPrevious = globalLastHead - (newPartitionRangeGlobalidx[p] - partition[p]);
					int buffSize = nofElementsFromSuccessiveToPrevious * (int)ceil((double)m_global.m_octantBytes / (double)(CHAR_BIT/8));
					sendBuffers[p] = ClassCommBuffer(buffSize,'a',m_comm);
					int pos = 0;
					for(uint32_t i = (uint32_t)(lh - nofElementsFromSuccessiveToPrevious + 1); i <= (uint32_t)lh; ++i){
						//pack octants from lh - partition[p] to lh
						const ClassOctant & octant = m_octree.m_octants[i];
						x = octant.getX();
						y = octant.getY();
						z = octant.getZ();
						l = octant.getLevel();
						m = octant.getMarker();
						for(int i = 0; i < 17; ++i)
							info[i] = octant.m_info[i];
						m_errorFlag = MPI_Pack(&x,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&y,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&z,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&l,1,MPI_UINT8_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&m,1,MPI_INT8_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						for(int j = 0; j < 17; ++j){
							MPI_Pack(&info[j],1,MPI::BOOL,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						}
					}
					lh -= nofElementsFromSuccessiveToPrevious;
					globalLastHead -= nofElementsFromSuccessiveToPrevious;
					headSize = lh + 1;
					if(headSize == 0)
						break;
				}
			}

		}
		uint32_t nofElementsFromPreviousToSuccessive = 0;
		contatore = 0;
		//build send buffers from Tail
		if(tailSize != 0){
			for(int p = firstSuccessor; p < m_nproc; ++p){
				if(tailSize < partition[p]){

					nofElementsFromPreviousToSuccessive = newPartitionRangeGlobalidx[p] - globalFirstTail + 1;
					if(nofElementsFromPreviousToSuccessive > tailSize || contatore == 1)
						nofElementsFromPreviousToSuccessive = tailSize;

					int buffSize = nofElementsFromPreviousToSuccessive * (int)ceil((double)m_global.m_octantBytes / (double)(CHAR_BIT/8));
					sendBuffers[p] = ClassCommBuffer(buffSize,'a',m_comm);
					int pos = 0;
					uint32_t octantsSize = (uint32_t)m_octree.m_octants.size();
					for(uint32_t i = ft; i < ft + nofElementsFromPreviousToSuccessive; ++i){
						//PACK octants from ft to octantsSize-1
						const ClassOctant & octant = m_octree.m_octants[i];
						x = octant.getX();
						y = octant.getY();
						z = octant.getZ();
						l = octant.getLevel();
						m = octant.getMarker();
						for(int ii = 0; ii < 17; ++ii)
							info[ii] = octant.m_info[ii];
						m_errorFlag = MPI_Pack(&x,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&y,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&z,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&l,1,MPI_UINT8_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&m,1,MPI_INT8_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						for(int j = 0; j < 17; ++j){
							MPI_Pack(&info[j],1,MPI::BOOL,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						}
					}

					if(nofElementsFromPreviousToSuccessive == tailSize)
						break;
					ft += nofElementsFromPreviousToSuccessive;
					globalFirstTail += nofElementsFromPreviousToSuccessive;
					tailSize -= nofElementsFromPreviousToSuccessive;
					++contatore;
				}
				else{
					nofElementsFromPreviousToSuccessive = newPartitionRangeGlobalidx[p] - globalFirstTail + 1;
					//						int buffSize = nofElementsFromPreviousToSuccessive * (int)ceil((double)m_global.m_octantBytes / (double)(CHAR_BIT/8));
					int buffSize = nofElementsFromPreviousToSuccessive * (int)ceil((double)m_global.m_octantBytes / (double)(CHAR_BIT/8));
					//int buffSize = partition[p] * (int)ceil((double)m_global.m_octantBytes / (double)(CHAR_BIT/8));
					sendBuffers[p] = ClassCommBuffer(buffSize,'a',m_comm);
					uint32_t endOctants = ft + nofElementsFromPreviousToSuccessive  - 1;
					int pos = 0;
					for(uint32_t i = ft; i <= endOctants; ++i ){
						//PACK octants from ft to ft + partition[p] -1
						const ClassOctant & octant = m_octree.m_octants[i];
						x = octant.getX();
						y = octant.getY();
						z = octant.getZ();
						l = octant.getLevel();
						m = octant.getMarker();
						for(int i = 0; i < 17; ++i)
							info[i] = octant.m_info[i];
						m_errorFlag = MPI_Pack(&x,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&y,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&z,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&l,1,MPI_UINT8_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&m,1,MPI_INT8_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						for(int j = 0; j < 17; ++j){
							MPI_Pack(&info[j],1,MPI::BOOL,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						}
					}
					//ft += partition[p];
					//tailSize -= partition[p];
					ft += nofElementsFromPreviousToSuccessive;
					globalFirstTail += nofElementsFromPreviousToSuccessive;
					tailSize -= nofElementsFromPreviousToSuccessive;
					if(tailSize == 0)
						break;
				}
			}
		}

		//Build receiver sources
		vector<ClassArray> recvs(m_nproc);
		recvs[m_rank] = ClassArray((uint32_t)sendBuffers.size()+1,-1);
		recvs[m_rank].m_array[0] = m_rank;
		int counter = 1;
		map<int,ClassCommBuffer>::iterator sitend = sendBuffers.end();
		for(map<int,ClassCommBuffer>::iterator sit = sendBuffers.begin(); sit != sitend; ++sit){
			recvs[m_rank].m_array[counter] = sit->first;
			++counter;
		}
		int* nofRecvsPerProc = new int[m_nproc];
		m_errorFlag = MPI_Allgather(&recvs[m_rank].m_arraySize,1,MPI_INT,nofRecvsPerProc,1,MPI_INT,m_comm);
		int globalRecvsBuffSize = 0;
		int* displays = new int[m_nproc];
		for(int pp = 0; pp < m_nproc; ++pp){
			displays[pp] = 0;
			globalRecvsBuffSize += nofRecvsPerProc[pp];
			for(int ppp = 0; ppp < pp; ++ppp){
				displays[pp] += nofRecvsPerProc[ppp];
			}
		}
		int* globalRecvsBuff = new int[globalRecvsBuffSize];
		m_errorFlag = MPI_Allgatherv(recvs[m_rank].m_array,recvs[m_rank].m_arraySize,MPI_INT,globalRecvsBuff,nofRecvsPerProc,displays,MPI_INT,m_comm);

		vector<set<int> > sendersPerProc(m_nproc);
		for(int pin = 0; pin < m_nproc; ++pin){
			for(int k = displays[pin]+1; k < displays[pin] + nofRecvsPerProc[pin]; ++k){
				sendersPerProc[globalRecvsBuff[k]].insert(globalRecvsBuff[displays[pin]]);
			}
		}

		//Communicate Octants (size)
		MPI_Request* req = new MPI_Request[sendBuffers.size()+sendersPerProc[m_rank].size()];
		MPI_Status* stats = new MPI_Status[sendBuffers.size()+sendersPerProc[m_rank].size()];
		int nReq = 0;
		map<int,int> recvBufferSizePerProc;
		set<int>::iterator senditend = sendersPerProc[m_rank].end();
		for(set<int>::iterator sendit = sendersPerProc[m_rank].begin(); sendit != senditend; ++sendit){
			recvBufferSizePerProc[*sendit] = 0;
			m_errorFlag = MPI_Irecv(&recvBufferSizePerProc[*sendit],1,MPI_UINT32_T,*sendit,m_rank,m_comm,&req[nReq]);
			++nReq;
		}
		map<int,ClassCommBuffer>::reverse_iterator rsitend = sendBuffers.rend();
		for(map<int,ClassCommBuffer>::reverse_iterator rsit = sendBuffers.rbegin(); rsit != rsitend; ++rsit){
			m_errorFlag =  MPI_Isend(&rsit->second.m_commBufferSize,1,MPI_UINT32_T,rsit->first,rsit->first,m_comm,&req[nReq]);
			++nReq;
		}
		MPI_Waitall(nReq,req,stats);

		//COMMUNICATE THE BUFFERS TO THE RECEIVERS
		//recvBuffers structure is declared and each buffer is initialized to the right size
		//then, sendBuffers are communicated by senders and stored in recvBuffers in the receivers
		uint32_t nofNewHead = 0;
		uint32_t nofNewTail = 0;
		map<int,ClassCommBuffer> recvBuffers;
		map<int,int>::iterator ritend = recvBufferSizePerProc.end();
		for(map<int,int>::iterator rit = recvBufferSizePerProc.begin(); rit != ritend; ++rit){
			recvBuffers[rit->first] = ClassCommBuffer(rit->second,'a',m_comm);
			uint32_t nofNewPerProc = (uint32_t)(rit->second / (uint32_t)ceil((double)m_global.m_octantBytes / (double)(CHAR_BIT/8)));
			if(rit->first < m_rank)
				nofNewHead += nofNewPerProc;
			else if(rit->first > m_rank)
				nofNewTail += nofNewPerProc;
		}
		nReq = 0;
		for(set<int>::iterator sendit = sendersPerProc[m_rank].begin(); sendit != senditend; ++sendit){
			//nofBytesOverProc += recvBuffers[sit->first].m_commBufferSize;
			m_errorFlag = MPI_Irecv(recvBuffers[*sendit].m_commBuffer,recvBuffers[*sendit].m_commBufferSize,MPI_PACKED,*sendit,m_rank,m_comm,&req[nReq]);
			++nReq;
		}
		for(map<int,ClassCommBuffer>::reverse_iterator rsit = sendBuffers.rbegin(); rsit != rsitend; ++rsit){
			m_errorFlag =  MPI_Isend(rsit->second.m_commBuffer,rsit->second.m_commBufferSize,MPI_PACKED,rsit->first,rsit->first,m_comm,&req[nReq]);
			++nReq;
		}
		MPI_Waitall(nReq,req,stats);

		//MOVE RESIDENT TO BEGIN IN OCTANTS
		uint32_t resEnd = m_octree.getNumOctants() - tailOffset;
		uint32_t nofResidents = resEnd - headOffset;
		int octCounter = 0;
		for(uint32_t i = headOffset; i < resEnd; ++i){
			m_octree.m_octants[octCounter] = m_octree.m_octants[i];
			++octCounter;
		}
		uint32_t newCounter = nofNewHead + nofNewTail + nofResidents;
		m_octree.m_octants.resize(newCounter);
		//MOVE RESIDENTS IN RIGHT POSITION
		uint32_t resCounter = nofNewHead + nofResidents - 1;
		for(uint32_t k = 0; k < nofResidents ; ++k){
			m_octree.m_octants[resCounter - k] = m_octree.m_octants[nofResidents - k - 1];
		}

		//UNPACK BUFFERS AND BUILD NEW OCTANTS
		newCounter = 0;
		bool jumpResident = false;
		map<int,ClassCommBuffer>::iterator rbitend = recvBuffers.end();
		for(map<int,ClassCommBuffer>::iterator rbit = recvBuffers.begin(); rbit != rbitend; ++rbit){
			uint32_t nofNewPerProc = (uint32_t)(rbit->second.m_commBufferSize / (uint32_t)ceil((double)m_global.m_octantBytes / (double)(CHAR_BIT/8)));
			int pos = 0;
			if(rbit->first > m_rank && !jumpResident){
				newCounter += nofResidents ;
				jumpResident = true;
			}
			for(int i = nofNewPerProc - 1; i >= 0; --i){
				m_errorFlag = MPI_Unpack(rbit->second.m_commBuffer,rbit->second.m_commBufferSize,&pos,&x,1,MPI_UINT32_T,m_comm);
				m_errorFlag = MPI_Unpack(rbit->second.m_commBuffer,rbit->second.m_commBufferSize,&pos,&y,1,MPI_UINT32_T,m_comm);
				m_errorFlag = MPI_Unpack(rbit->second.m_commBuffer,rbit->second.m_commBufferSize,&pos,&z,1,MPI_UINT32_T,m_comm);
				m_errorFlag = MPI_Unpack(rbit->second.m_commBuffer,rbit->second.m_commBufferSize,&pos,&l,1,MPI_UINT8_T,m_comm);
				m_octree.m_octants[newCounter] = ClassOctant(m_dim,l,x,y,z);
				m_errorFlag = MPI_Unpack(rbit->second.m_commBuffer,rbit->second.m_commBufferSize,&pos,&m,1,MPI_INT8_T,m_comm);
				m_octree.m_octants[newCounter].setMarker(m);
				for(int j = 0; j < 17; ++j){
					m_errorFlag = MPI_Unpack(rbit->second.m_commBuffer,rbit->second.m_commBufferSize,&pos,&info[j],1,MPI::BOOL,m_comm);
					m_octree.m_octants[newCounter].m_info[j] = info[j];
				}
				++newCounter;
			}
		}
		octvector(m_octree.m_octants).swap(m_octree.m_octants);

		delete [] newPartitionRangeGlobalidx; newPartitionRangeGlobalidx = NULL;
		delete [] nofRecvsPerProc; nofRecvsPerProc = NULL;
		delete [] displays; displays = NULL;
		delete [] req; req = NULL;
		delete [] stats; stats = NULL;
		delete [] globalRecvsBuff; globalRecvsBuff = NULL;
		//Update and ghosts here
		updateLoadBalance();
		setPboundGhosts();

	}
	delete [] partition;
	partition = NULL;

	//Write info of final partition on log
	m_log.writeLog(" ");
	m_log.writeLog(" Final Parallel partition : ");
	m_log.writeLog(" Octants for proc	"+ to_string(static_cast<unsigned long long>(0))+"	:	" + to_string(static_cast<unsigned long long>(m_partitionRangeGlobalIdx[0]+1)));
	for(int ii=1; ii<m_nproc; ii++){
		m_log.writeLog(" Octants for proc	"+ to_string(static_cast<unsigned long long>(ii))+"	:	" + to_string(static_cast<unsigned long long>(m_partitionRangeGlobalIdx[ii]-m_partitionRangeGlobalIdx[ii-1])));
	}
	m_log.writeLog(" ");
	m_log.writeLog("---------------------------------------------");

}

/** Distribute Load-Balanced the octants of the whole tree over
 * the processes of the job. Until loadBalance is not called for the first time the mesh is serial.
 * The families of octants of a desired level are retained compact on the same process.
 * \param[in] level Number of level over the max depth reached in the tree at which families of octants are fixed compact on the same process (level=0 is classic LoadBalance).
 */
void
ClassParaTree::loadBalance(uint8_t & level){

	//Write info on log
	m_log.writeLog("---------------------------------------------");
	m_log.writeLog(" LOAD BALANCE ");

	uint32_t* partition = new uint32_t [m_nproc];
	computePartition(partition, level);
	if(m_serial)
	{
		m_log.writeLog(" ");
		m_log.writeLog(" Initial Serial distribution : ");
		for(int ii=0; ii<m_nproc; ii++){
			m_log.writeLog(" Octants for proc	"+ to_string(static_cast<unsigned long long>(ii))+"	:	" + to_string(static_cast<unsigned long long>(m_partitionRangeGlobalIdx[ii]+1)));
		}

		uint32_t stride = 0;
		for(int i = 0; i < m_rank; ++i)
			stride += partition[i];
		ClassLocalTree::octvector octantsCopy = m_octree.m_octants;
		ClassLocalTree::octvector::const_iterator first = octantsCopy.begin() + stride;
		ClassLocalTree::octvector::const_iterator last = first + partition[m_rank];
		m_octree.m_octants.assign(first, last);
		octvector(m_octree.m_octants).swap(m_octree.m_octants);

		first = octantsCopy.end();
		last = octantsCopy.end();

		//Update and ghosts here
		updateLoadBalance();
		setPboundGhosts();

	}
	else
	{
		m_log.writeLog(" ");
		m_log.writeLog(" Initial Parallel partition : ");
		m_log.writeLog(" Octants for proc	"+ to_string(static_cast<unsigned long long>(0))+"	:	" + to_string(static_cast<unsigned long long>(m_partitionRangeGlobalIdx[0]+1)));
		for(int ii=1; ii<m_nproc; ii++){
			m_log.writeLog(" Octants for proc	"+ to_string(static_cast<unsigned long long>(ii))+"	:	" + to_string(static_cast<unsigned long long>(m_partitionRangeGlobalIdx[ii]-m_partitionRangeGlobalIdx[ii-1])));
		}

		//empty ghosts
		m_octree.m_ghosts.clear();
		m_octree.m_sizeGhosts = 0;
		//compute new partition range globalidx
		uint64_t* newPartitionRangeGlobalidx = new uint64_t[m_nproc];
		for(int p = 0; p < m_nproc; ++p){
			newPartitionRangeGlobalidx[p] = 0;
			for(int pp = 0; pp <= p; ++pp)
				newPartitionRangeGlobalidx[p] += (uint64_t)partition[pp];
			--newPartitionRangeGlobalidx[p];
		}

		//find resident octants local offset lastHead(lh) and firstTail(ft)
		int32_t lh,ft;
		if(m_rank == 0)
			lh = -1;
		else{
			lh = (int32_t)(newPartitionRangeGlobalidx[m_rank-1] + 1 - m_partitionRangeGlobalIdx[m_rank-1] - 1 - 1);
		}
		if(lh < 0)
			lh = - 1;
		else if(lh > (int32_t)(m_octree.m_octants.size() - 1))
			lh = m_octree.m_octants.size() - 1;

		if(m_rank == m_nproc - 1)
			ft = m_octree.m_octants.size();
		else if(m_rank == 0)
			ft = (int32_t)(newPartitionRangeGlobalidx[m_rank] + 1);
		else{
			ft = (int32_t)(newPartitionRangeGlobalidx[m_rank] - m_partitionRangeGlobalIdx[m_rank -1]);
		}
		if(ft > (int32_t)(m_octree.m_octants.size() - 1))
			ft = m_octree.m_octants.size();
		else if(ft < 0)
			ft = 0;

		//compute size Head and size Tail
		uint32_t headSize = (uint32_t)(lh + 1);
		uint32_t tailSize = (uint32_t)(m_octree.m_octants.size() - ft);
		uint32_t headOffset = headSize;
		uint32_t tailOffset = tailSize;

		//build send buffers
		map<int,ClassCommBuffer> sendBuffers;

		//Compute first predecessor and first successor to send buffers to
		int64_t firstOctantGlobalIdx = 0;// offset to compute global index of each octant in every process
		int64_t globalLastHead = (int64_t) lh;
		int64_t globalFirstTail = (int64_t) ft; //lastHead and firstTail in global ordering
		int firstPredecessor = -1;
		int firstSuccessor = m_nproc;
		if(m_rank != 0){
			firstOctantGlobalIdx = (int64_t)(m_partitionRangeGlobalIdx[m_rank-1] + 1);
			globalLastHead = firstOctantGlobalIdx + (int64_t)lh;
			globalFirstTail = firstOctantGlobalIdx + (int64_t)ft;
			for(int pre = m_rank - 1; pre >=0; --pre){
				if((uint64_t)globalLastHead <= newPartitionRangeGlobalidx[pre])
					firstPredecessor = pre;
			}
			for(int post = m_rank + 1; post < m_nproc; ++post){
				if((uint64_t)globalFirstTail <= newPartitionRangeGlobalidx[post] && (uint64_t)globalFirstTail > newPartitionRangeGlobalidx[post-1])
					firstSuccessor = post;
			}
		}
		else if(m_rank == 0){
			firstSuccessor = 1;
		}
		MPI_Barrier(m_comm); //da spostare prima della prima comunicazione

		uint32_t x,y,z;
		uint8_t l;
		int8_t m;
		bool info[17];
		int intBuffer = 0;
		int contatore = 0;
		//build send buffers from Head
		uint32_t nofElementsFromSuccessiveToPrevious = 0;
		if(headSize != 0){
			for(int p = firstPredecessor; p >= 0; --p){
				if(headSize < partition[p]){
					intBuffer = (newPartitionRangeGlobalidx[p] - partition[p] );
					intBuffer = abs(intBuffer);
					nofElementsFromSuccessiveToPrevious = globalLastHead - intBuffer;
					if(nofElementsFromSuccessiveToPrevious > headSize || contatore == 1)
						nofElementsFromSuccessiveToPrevious  = headSize;

					int buffSize = nofElementsFromSuccessiveToPrevious * (int)ceil((double)m_global.m_octantBytes / (double)(CHAR_BIT/8));
					sendBuffers[p] = ClassCommBuffer(buffSize,'a',m_comm);
					int pos = 0;
					for(uint32_t i = (uint32_t)(lh - nofElementsFromSuccessiveToPrevious + 1); i <= (uint32_t)lh; ++i){
						//PACK octants from 0 to lh in sendBuffer[p]
						const ClassOctant & octant = m_octree.m_octants[i];
						x = octant.getX();
						y = octant.getY();
						z = octant.getZ();
						l = octant.getLevel();
						m = octant.getMarker();
						for(int ii = 0; ii < 17; ++ii)
							info[ii] = octant.m_info[ii];
						m_errorFlag = MPI_Pack(&x,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&y,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&z,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&l,1,MPI_UINT8_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&m,1,MPI_INT8_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						for(int j = 0; j < 17; ++j){
							MPI_Pack(&info[j],1,MPI::BOOL,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						}
					}
					if(nofElementsFromSuccessiveToPrevious == headSize)
						break;

					lh -= nofElementsFromSuccessiveToPrevious;
					globalLastHead -= nofElementsFromSuccessiveToPrevious;
					headSize = lh + 1;
					++contatore;
				}
				else{
					nofElementsFromSuccessiveToPrevious = globalLastHead - (newPartitionRangeGlobalidx[p] - partition[p]);
					int buffSize = nofElementsFromSuccessiveToPrevious * (int)ceil((double)m_global.m_octantBytes / (double)(CHAR_BIT/8));
					sendBuffers[p] = ClassCommBuffer(buffSize,'a',m_comm);
					int pos = 0;
					for(uint32_t i = (uint32_t)(lh - nofElementsFromSuccessiveToPrevious + 1); i <= (uint32_t)lh; ++i){
						//pack octants from lh - partition[p] to lh
						const ClassOctant & octant = m_octree.m_octants[i];
						x = octant.getX();
						y = octant.getY();
						z = octant.getZ();
						l = octant.getLevel();
						m = octant.getMarker();
						for(int i = 0; i < 17; ++i)
							info[i] = octant.m_info[i];
						m_errorFlag = MPI_Pack(&x,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&y,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&z,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&l,1,MPI_UINT8_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&m,1,MPI_INT8_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						for(int j = 0; j < 17; ++j){
							MPI_Pack(&info[j],1,MPI::BOOL,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						}
					}
					lh -= nofElementsFromSuccessiveToPrevious;
					globalLastHead -= nofElementsFromSuccessiveToPrevious;
					headSize = lh + 1;
					if(headSize == 0)
						break;
				}
			}

		}
		uint32_t nofElementsFromPreviousToSuccessive = 0;
		contatore = 0;
		//build send buffers from Tail
		if(tailSize != 0){
			for(int p = firstSuccessor; p < m_nproc; ++p){
				if(tailSize < partition[p]){
					nofElementsFromPreviousToSuccessive = newPartitionRangeGlobalidx[p] - globalFirstTail + 1;
					if(nofElementsFromPreviousToSuccessive > tailSize || contatore == 1)
						nofElementsFromPreviousToSuccessive = tailSize;

					int buffSize = nofElementsFromPreviousToSuccessive * (int)ceil((double)m_global.m_octantBytes / (double)(CHAR_BIT/8));
					sendBuffers[p] = ClassCommBuffer(buffSize,'a',m_comm);
					int pos = 0;
					uint32_t octantsSize = (uint32_t)m_octree.m_octants.size();
					for(uint32_t i = ft; i < ft + nofElementsFromPreviousToSuccessive; ++i){
						//PACK octants from ft to octantsSize-1
						const ClassOctant & octant = m_octree.m_octants[i];
						x = octant.getX();
						y = octant.getY();
						z = octant.getZ();
						l = octant.getLevel();
						m = octant.getMarker();
						for(int ii = 0; ii < 17; ++ii)
							info[ii] = octant.m_info[ii];
						m_errorFlag = MPI_Pack(&x,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&y,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&z,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&l,1,MPI_UINT8_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&m,1,MPI_INT8_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						for(int j = 0; j < 17; ++j){
							MPI_Pack(&info[j],1,MPI::BOOL,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						}
					}
					if(nofElementsFromPreviousToSuccessive == tailSize)
						break;
					ft += nofElementsFromPreviousToSuccessive;
					globalFirstTail += nofElementsFromPreviousToSuccessive;
					tailSize -= nofElementsFromPreviousToSuccessive;
					++contatore;
				}
				else{
					nofElementsFromPreviousToSuccessive = newPartitionRangeGlobalidx[p] - globalFirstTail + 1;
					int buffSize = nofElementsFromPreviousToSuccessive * (int)ceil((double)m_global.m_octantBytes / (double)(CHAR_BIT/8));
					sendBuffers[p] = ClassCommBuffer(buffSize,'a',m_comm);
					uint32_t endOctants = ft + nofElementsFromPreviousToSuccessive - 1;
					int pos = 0;
					for(uint32_t i = ft; i <= endOctants; ++i ){
						//PACK octants from ft to ft + partition[p] -1
						const ClassOctant & octant = m_octree.m_octants[i];
						x = octant.getX();
						y = octant.getY();
						z = octant.getZ();
						l = octant.getLevel();
						m = octant.getMarker();
						for(int ii = 0; ii < 17; ++ii)
							info[ii] = octant.m_info[ii];
						m_errorFlag = MPI_Pack(&x,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&y,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&z,1,MPI_UINT32_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&l,1,MPI_UINT8_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						m_errorFlag = MPI_Pack(&m,1,MPI_INT8_T,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						for(int j = 0; j < 17; ++j){
							MPI_Pack(&info[j],1,MPI::BOOL,sendBuffers[p].m_commBuffer,buffSize,&pos,m_comm);
						}
					}
					ft += nofElementsFromPreviousToSuccessive;
					globalFirstTail += nofElementsFromPreviousToSuccessive;
					tailSize -= nofElementsFromPreviousToSuccessive;
					if(tailSize == 0)
						break;
				}
			}
		}

		//Build receiver sources
		vector<ClassArray> recvs(m_nproc);
		recvs[m_rank] = ClassArray((uint32_t)sendBuffers.size()+1,-1);
		recvs[m_rank].m_array[0] = m_rank;
		int counter = 1;
		map<int,ClassCommBuffer>::iterator sitend = sendBuffers.end();
		for(map<int,ClassCommBuffer>::iterator sit = sendBuffers.begin(); sit != sitend; ++sit){
			recvs[m_rank].m_array[counter] = sit->first;
			++counter;
		}
		int* nofRecvsPerProc = new int[m_nproc];
		m_errorFlag = MPI_Allgather(&recvs[m_rank].m_arraySize,1,MPI_INT,nofRecvsPerProc,1,MPI_INT,m_comm);
		int globalRecvsBuffSize = 0;
		int* displays = new int[m_nproc];
		for(int pp = 0; pp < m_nproc; ++pp){
			displays[pp] = 0;
			globalRecvsBuffSize += nofRecvsPerProc[pp];
			for(int ppp = 0; ppp < pp; ++ppp){
				displays[pp] += nofRecvsPerProc[ppp];
			}
		}
		int* globalRecvsBuff = new int[globalRecvsBuffSize];
		m_errorFlag = MPI_Allgatherv(recvs[m_rank].m_array,recvs[m_rank].m_arraySize,MPI_INT,globalRecvsBuff,nofRecvsPerProc,displays,MPI_INT,m_comm);

		vector<set<int> > sendersPerProc(m_nproc);
		for(int pin = 0; pin < m_nproc; ++pin){
			for(int k = displays[pin]+1; k < displays[pin] + nofRecvsPerProc[pin]; ++k){
				sendersPerProc[globalRecvsBuff[k]].insert(globalRecvsBuff[displays[pin]]);
			}
		}

		//Communicate Octants (size)
		MPI_Request* req = new MPI_Request[sendBuffers.size()+sendersPerProc[m_rank].size()];
		MPI_Status* stats = new MPI_Status[sendBuffers.size()+sendersPerProc[m_rank].size()];
		int nReq = 0;
		map<int,int> recvBufferSizePerProc;
		set<int>::iterator senditend = sendersPerProc[m_rank].end();
		for(set<int>::iterator sendit = sendersPerProc[m_rank].begin(); sendit != senditend; ++sendit){
			recvBufferSizePerProc[*sendit] = 0;
			m_errorFlag = MPI_Irecv(&recvBufferSizePerProc[*sendit],1,MPI_UINT32_T,*sendit,m_rank,m_comm,&req[nReq]);
			++nReq;
		}
		map<int,ClassCommBuffer>::reverse_iterator rsitend = sendBuffers.rend();
		for(map<int,ClassCommBuffer>::reverse_iterator rsit = sendBuffers.rbegin(); rsit != rsitend; ++rsit){
			m_errorFlag =  MPI_Isend(&rsit->second.m_commBufferSize,1,MPI_UINT32_T,rsit->first,rsit->first,m_comm,&req[nReq]);
			++nReq;
		}
		MPI_Waitall(nReq,req,stats);

		//COMMUNICATE THE BUFFERS TO THE RECEIVERS
		//recvBuffers structure is declared and each buffer is initialized to the right size
		//then, sendBuffers are communicated by senders and stored in recvBuffers in the receivers
		uint32_t nofNewHead = 0;
		uint32_t nofNewTail = 0;
		map<int,ClassCommBuffer> recvBuffers;
		map<int,int>::iterator ritend = recvBufferSizePerProc.end();
		for(map<int,int>::iterator rit = recvBufferSizePerProc.begin(); rit != ritend; ++rit){
			recvBuffers[rit->first] = ClassCommBuffer(rit->second,'a',m_comm);
			uint32_t nofNewPerProc = (uint32_t)(rit->second / (uint32_t)ceil((double)m_global.m_octantBytes / (double)(CHAR_BIT/8)));
			if(rit->first < m_rank)
				nofNewHead += nofNewPerProc;
			else if(rit->first > m_rank)
				nofNewTail += nofNewPerProc;
		}
		nReq = 0;
		for(set<int>::iterator sendit = sendersPerProc[m_rank].begin(); sendit != senditend; ++sendit){
			//nofBytesOverProc += recvBuffers[sit->first].m_commBufferSize;
			m_errorFlag = MPI_Irecv(recvBuffers[*sendit].m_commBuffer,recvBuffers[*sendit].m_commBufferSize,MPI_PACKED,*sendit,m_rank,m_comm,&req[nReq]);
			++nReq;
		}
		for(map<int,ClassCommBuffer>::reverse_iterator rsit = sendBuffers.rbegin(); rsit != rsitend; ++rsit){
			m_errorFlag =  MPI_Isend(rsit->second.m_commBuffer,rsit->second.m_commBufferSize,MPI_PACKED,rsit->first,rsit->first,m_comm,&req[nReq]);
			++nReq;
		}
		MPI_Waitall(nReq,req,stats);

		//MOVE RESIDENT TO BEGIN IN OCTANTS
		uint32_t resEnd = m_octree.getNumOctants() - tailOffset;
		uint32_t nofResidents = resEnd - headOffset;
		int octCounter = 0;
		for(uint32_t i = headOffset; i < resEnd; ++i){
			m_octree.m_octants[octCounter] = m_octree.m_octants[i];
			++octCounter;
		}
		uint32_t newCounter = nofNewHead + nofNewTail + nofResidents;
		m_octree.m_octants.resize(newCounter);
		//MOVE RESIDENTS IN RIGHT POSITION
		uint32_t resCounter = nofNewHead + nofResidents - 1;
		for(uint32_t k = 0; k < nofResidents ; ++k){
			m_octree.m_octants[resCounter - k] = m_octree.m_octants[nofResidents - k - 1];
		}

		//UNPACK BUFFERS AND BUILD NEW OCTANTS
		newCounter = 0;
		bool jumpResident = false;
		map<int,ClassCommBuffer>::iterator rbitend = recvBuffers.end();
		for(map<int,ClassCommBuffer>::iterator rbit = recvBuffers.begin(); rbit != rbitend; ++rbit){
			uint32_t nofNewPerProc = (uint32_t)(rbit->second.m_commBufferSize / (uint32_t)ceil((double)m_global.m_octantBytes / (double)(CHAR_BIT/8)));
			int pos = 0;
			if(rbit->first > m_rank && !jumpResident){
				newCounter += nofResidents ;
				jumpResident = true;
			}
			for(int i = nofNewPerProc - 1; i >= 0; --i){
				m_errorFlag = MPI_Unpack(rbit->second.m_commBuffer,rbit->second.m_commBufferSize,&pos,&x,1,MPI_UINT32_T,m_comm);
				m_errorFlag = MPI_Unpack(rbit->second.m_commBuffer,rbit->second.m_commBufferSize,&pos,&y,1,MPI_UINT32_T,m_comm);
				m_errorFlag = MPI_Unpack(rbit->second.m_commBuffer,rbit->second.m_commBufferSize,&pos,&z,1,MPI_UINT32_T,m_comm);
				m_errorFlag = MPI_Unpack(rbit->second.m_commBuffer,rbit->second.m_commBufferSize,&pos,&l,1,MPI_UINT8_T,m_comm);
				m_octree.m_octants[newCounter] = ClassOctant(m_dim,l,x,y,z);
				m_errorFlag = MPI_Unpack(rbit->second.m_commBuffer,rbit->second.m_commBufferSize,&pos,&m,1,MPI_INT8_T,m_comm);
				m_octree.m_octants[newCounter].setMarker(m);
				for(int j = 0; j < 17; ++j){
					m_errorFlag = MPI_Unpack(rbit->second.m_commBuffer,rbit->second.m_commBufferSize,&pos,&info[j],1,MPI::BOOL,m_comm);
					m_octree.m_octants[newCounter].m_info[j] = info[j];
				}
				++newCounter;
			}
		}
		octvector(m_octree.m_octants).swap(m_octree.m_octants);

		delete [] newPartitionRangeGlobalidx; newPartitionRangeGlobalidx = NULL;
		delete [] nofRecvsPerProc; nofRecvsPerProc = NULL;
		delete [] displays; displays = NULL;
		delete [] req; req = NULL;
		delete [] stats; stats = NULL;
		delete [] globalRecvsBuff; globalRecvsBuff = NULL;
		//Update and ghosts here
		updateLoadBalance();
		setPboundGhosts();

	}
	delete [] partition;
	partition = NULL;

	//Write info of final partition on log
	m_log.writeLog(" ");
	m_log.writeLog(" Final Parallel partition : ");
	m_log.writeLog(" Octants for proc	"+ to_string(static_cast<unsigned long long>(0))+"	:	" + to_string(static_cast<unsigned long long>(m_partitionRangeGlobalIdx[0]+1)));
	for(int ii=1; ii<m_nproc; ii++){
		m_log.writeLog(" Octants for proc	"+ to_string(static_cast<unsigned long long>(ii))+"	:	" + to_string(static_cast<unsigned long long>(m_partitionRangeGlobalIdx[ii]-m_partitionRangeGlobalIdx[ii-1])));
	}
	m_log.writeLog(" ");
	m_log.writeLog("---------------------------------------------");

}
#endif

// =================================================================================== //
// OTHER INTERSECTION BASED METHODS												    			   //
// =================================================================================== //

/** Compute the intersection between octants (local, ghost, boundary).
 */
void
ClassParaTree::computeIntersections(){
	m_octree.computeIntersections();
}

// =================================================================================== //
// OTHER PRIVATE METHODS												    			   //
// =================================================================================== //

/*! Extract an octant from the local tree.
 * \param[in] idx Local index of target octant.
 * \return Reference to target octant.
 */
ClassOctant&
ClassParaTree::extractOctant(uint32_t idx) {
	return m_octree.extractOctant(idx) ;
};

/*! Adapt the octree mesh with user setup for markers and 2:1 balancing conditions.
 * \param[in] mapflag True to track the changes in structure octant by a mapper.
 */
bool
ClassParaTree::private_adapt_mapidx(bool mapflag) {
	//TODO recoding for adapting with abs(marker) > 1

	bool globalDone = false, localDone = false;
	bool refine = true, coarse = true, globalCoarse = true;
	uint32_t nocts = m_octree.getNumOctants();
	u32vector m_mapIdx_temp, m_mapIdx_temp2;
	vector<ClassOctant >::iterator iter, iterend = m_octree.m_octants.end();

	for (iter = m_octree.m_octants.begin(); iter != iterend; iter++){
		iter->m_info[12] = false;
		iter->m_info[13] = false;
		iter->m_info[15] = false;
	}

	// m_mapIdx init
	u32vector().swap(m_mapIdx);
	u32vector().swap(m_mapIdx_temp);
	u32vector().swap(m_mapIdx_temp2);
	if (mapflag) {
		m_mapIdx.resize(nocts);
		m_mapIdx_temp.resize(nocts);
		for (uint32_t i=0; i<nocts; i++){
			m_mapIdx[i] = i;
			m_mapIdx_temp[i] = i;
		}
	}

#if ENABLE_MPI
	if(m_serial){
#endif
		m_log.writeLog("---------------------------------------------");
		m_log.writeLog(" ADAPT (Refine/Coarse)");
		m_log.writeLog(" ");

		// 2:1 Balance
		balance21(true);

		m_log.writeLog(" ");
		m_log.writeLog(" Initial Number of octants	:	" + to_string(static_cast<unsigned long long>(m_octree.getNumOctants())));

		// Refine
		//		while(m_octree.refine(m_mapIdx));
		while (refine) {
			refine = m_octree.refine(m_mapIdx_temp);
			m_mapIdx_temp2.resize(m_octree.getNumOctants());
			for (uint32_t i=0; i<m_octree.getNumOctants(); i++){
				m_mapIdx_temp2[m_mapIdx_temp[i]] = m_mapIdx[m_mapIdx_temp[i]];
			}
			m_mapIdx.clear();
			m_mapIdx = m_mapIdx_temp2;
		}
		if (m_octree.getNumOctants() > nocts)
			localDone = true;
		m_log.writeLog(" Number of octants after Refine	:	" + to_string(static_cast<unsigned long long>(m_octree.getNumOctants())));
		nocts = m_octree.getNumOctants();
		updateAdapt();

		// Coarse
		//while(m_octree.coarse(m_mapIdx));
		while (coarse) {
			coarse = m_octree.coarse(m_mapIdx_temp);
			if (mapflag){
				m_mapIdx_temp2.resize(m_octree.getNumOctants());
				for (uint32_t i=0; i<m_octree.getNumOctants(); i++){
					m_mapIdx_temp2[m_mapIdx_temp[i]] = m_mapIdx[m_mapIdx_temp[i]];
				}
				m_mapIdx.clear();
				m_mapIdx = m_mapIdx_temp2;
			}
			updateAfterCoarse(m_mapIdx);
		}
		//updateAfterCoarse(m_mapIdx);
		//			balance21(false);
		//			while(m_octree.refine(m_mapIdx));
		//			updateAdapt();
		if (m_octree.getNumOctants() < nocts){
			localDone = true;
		}
		nocts = m_octree.getNumOctants();

		m_log.writeLog(" Number of octants after Coarse	:	" + to_string(static_cast<unsigned long long>(nocts)));
#if ENABLE_MPI
		MPI_Barrier(m_comm);
		m_errorFlag = MPI_Allreduce(&localDone,&globalDone,1,MPI::BOOL,MPI_LOR,m_comm);
#endif
		m_log.writeLog(" ");
		m_log.writeLog("---------------------------------------------");
#if ENABLE_MPI
	}
	else{
		m_log.writeLog("---------------------------------------------");
		m_log.writeLog(" ADAPT (Refine/Coarse)");
		m_log.writeLog(" ");

		// 2:1 Balance
		balance21(true);

		m_log.writeLog(" ");
		m_log.writeLog(" Initial Number of octants	:	" + to_string(static_cast<unsigned long long>(m_globalNumOctants)));

		// Refine
//		while(m_octree.refine(m_mapIdx));
		while (refine) {
			refine = m_octree.refine(m_mapIdx_temp);
			if (mapflag){
				m_mapIdx_temp2.resize(m_octree.getNumOctants());
				for (uint32_t i=0; i<m_octree.getNumOctants(); i++){
					m_mapIdx_temp2[m_mapIdx_temp[i]] = m_mapIdx[m_mapIdx_temp[i]];
				}
				m_mapIdx.clear();
				m_mapIdx = m_mapIdx_temp2;
			}
		}
		if (m_octree.getNumOctants() > nocts)
			localDone = true;
		updateAdapt();
		setPboundGhosts();
		m_log.writeLog(" Number of octants after Refine	:	" + to_string(static_cast<unsigned long long>(m_globalNumOctants)));
		nocts = m_octree.getNumOctants();


		// Coarse
//		while(m_octree.coarse(m_mapIdx));
		while (globalCoarse) {
			coarse = m_octree.coarse(m_mapIdx_temp);
			if (mapflag){
				m_mapIdx_temp2.resize(m_octree.getNumOctants());
				for (uint32_t i=0; i<m_octree.getNumOctants(); i++){
					m_mapIdx_temp2[m_mapIdx_temp[i]] = m_mapIdx[m_mapIdx_temp[i]];
				}
				m_mapIdx.clear();
				m_mapIdx = m_mapIdx_temp2;
			}
			updateAfterCoarse(m_mapIdx);
			setPboundGhosts();
			globalCoarse = false;
			MPI_Barrier(m_comm);
			m_errorFlag = MPI_Allreduce(&coarse,&globalCoarse,1,MPI::BOOL,MPI_LOR,m_comm);
		}
//		updateAfterCoarse(m_mapIdx);
//		setPboundGhosts();
		//			balance21(false);
		//			while(m_octree.refine(m_mapIdx));
		//			updateAdapt();
		//			setPboundGhosts();
		if (m_octree.getNumOctants() < nocts){
			localDone = true;
		}
		nocts = m_octree.getNumOctants();

		MPI_Barrier(m_comm);
		m_errorFlag = MPI_Allreduce(&localDone,&globalDone,1,MPI::BOOL,MPI_LOR,m_comm);
		m_log.writeLog(" Number of octants after Coarse	:	" + to_string(static_cast<unsigned long long>(m_globalNumOctants)));
		m_log.writeLog(" ");
		m_log.writeLog("---------------------------------------------");
	}
	return globalDone;
#else
	return localDone;
#endif
}

/*!Update the local tree after an adapt.
 */
void
ClassParaTree::updateAdapt(){
#if ENABLE_MPI
	if(m_serial)
	{
#endif
		m_maxDepth = m_octree.m_localMaxDepth;
		m_globalNumOctants = m_octree.getNumOctants();
		for(int p = 0; p < m_nproc; ++p){
			m_partitionRangeGlobalIdx[p] = m_globalNumOctants - 1;
		}
#if ENABLE_MPI
	}
	else
	{
		//update m_maxDepth
		m_errorFlag = MPI_Allreduce(&m_octree.m_localMaxDepth,&m_maxDepth,1,MPI_UINT8_T,MPI_MAX,m_comm);
		//update m_globalNumOctants
		uint64_t local_num_octants = m_octree.getNumOctants();
		m_errorFlag = MPI_Allreduce(&local_num_octants,&m_globalNumOctants,1,MPI_UINT64_T,MPI_SUM,m_comm);
		//update m_partitionRangeGlobalIdx
		uint64_t* rbuff = new uint64_t[m_nproc];
		m_errorFlag = MPI_Allgather(&local_num_octants,1,MPI_UINT64_T,rbuff,1,MPI_UINT64_T,m_comm);
		for(int p = 0; p < m_nproc; ++p){
			m_partitionRangeGlobalIdx[p] = 0;
			for(int pp = 0; pp <=p; ++pp)
				m_partitionRangeGlobalIdx[p] += rbuff[pp];
			--m_partitionRangeGlobalIdx[p];
		}
		//update partition_range_position
		uint64_t lastDescMorton = m_octree.getLastDesc().computeMorton();
		m_errorFlag = MPI_Allgather(&lastDescMorton,1,MPI_UINT64_T,m_partitionLastDesc,1,MPI_UINT64_T,m_comm);
		uint64_t firstDescMorton = m_octree.getFirstDesc().computeMorton();
		m_errorFlag = MPI_Allgather(&firstDescMorton,1,MPI_UINT64_T,m_partitionFirstDesc,1,MPI_UINT64_T,m_comm);
		delete [] rbuff; rbuff = NULL;
	}
#endif
}

#if ENABLE_MPI
/*! Compute the partition of the octree over the processes (only compute the information about
 * how distribute the mesh). This is an uniform distribution method.
 * \param[out] partition Pointer to partition information array. partition[i] = number of octants
 * to be stored on the i-th process (i-th rank).
 */
void
ClassParaTree::computePartition(uint32_t* partition){

	uint32_t division_result = 0;
	uint32_t remind = 0;

	division_result = uint32_t(m_globalNumOctants/(uint64_t)m_nproc);
	remind = (uint32_t)(m_globalNumOctants%(uint64_t)m_nproc);

	for(uint32_t i = 0; i < (uint32_t)m_nproc; ++i)
		if(i<remind)
			partition[i] = division_result + 1;
		else
			partition[i] = division_result;

}

/*! Compute the partition of the octree over the processes (only compute the information about
 * how distribute the mesh). This is an weighted distribution method: each process will have the same weight.
 * \param[out] partition Pointer to partition information array. partition[i] = number of octants
 * to be stored on the i-th process (i-th rank).
 * \param[in] weight Pointer to weight array. weight[i] = weight of i-th local octant.
 */
void
ClassParaTree::computePartition(uint32_t* partition, dvector* weight){
	if(m_serial){

		double division_result = 0;
		double global_weight = 0.0;
		for (int i=0; i<weight->size(); i++){
			global_weight += (*weight)[i];
		}
		division_result = global_weight/(double)m_nproc;

		//Estimate resulting weight distribution starting from proc 0 (sending tail)
		//Estimate sending weight by each proc in initial conf (sending tail)
		uint32_t i = 0, tot = 0;
		int iproc = 0;
		while (iproc < m_nproc-1){
			double partial_weight = 0.0;
			partition[iproc] = 0;
			while(partial_weight < division_result){
				partial_weight += (*weight)[i];
				tot++;
				partition[iproc]++;
				i++;
			}
			iproc++;
		}
		partition[m_nproc-1] = weight->size() - tot;
	}
	else{

		double division_result = 0;
		double remind = 0;
		dvector local_weight(m_nproc,0.0);
		dvector temp_local_weight(m_nproc,0.0);
		dvector2D sending_weight(m_nproc, dvector(m_nproc,0.0));
		double* rbuff = new double[m_nproc];
		double global_weight = 0.0;
		for (int i=0; i<weight->size(); i++){
			local_weight[m_rank] += (*weight)[i];
		}
		m_errorFlag = MPI_Allgather(&local_weight[m_rank],1,MPI_DOUBLE,rbuff,1,MPI_DOUBLE,m_comm);
		for (int i=0; i<m_nproc; i++){
			local_weight[i] = rbuff[i];
			global_weight += rbuff[i];
		}
		delete [] rbuff; rbuff = NULL;
		division_result = global_weight/(double)m_nproc;

		//Estimate resulting weight distribution starting from proc 0 (sending tail)

		temp_local_weight = local_weight;
		//Estimate sending weight by each proc in initial conf (sending tail)

		for (int iter = 0; iter < 1; iter++){

			vector<double> delta(m_nproc);
			for (int i=0; i<m_nproc; i++){
				delta[i] = temp_local_weight[i] - division_result;
			}

			for (int i=0; i<m_nproc-1; i++){

				double post_weight = 0.0;
				for (int j=i+1; j<m_nproc; j++){
					post_weight += temp_local_weight[j];
				}
				if (temp_local_weight[i] > division_result){

					delta[i] = temp_local_weight[i] - division_result;
					if (post_weight < division_result*(m_nproc-i-1)){

						double post_delta =  division_result*(m_nproc-i-1) - post_weight;
						double delta_sending = min(local_weight[i], min(delta[i], post_delta));
						int jproc = i+1;
						double sending = 0;
						while (delta_sending > 0 && jproc<m_nproc){
							sending = min(division_result, delta_sending);
							sending = min(sending, (division_result-temp_local_weight[jproc]));
							sending = max(sending, 0.0);
							sending_weight[i][jproc] += sending;
							temp_local_weight[jproc] += sending;
							temp_local_weight[i] -= sending;
							delta_sending -= sending;
							delta[i] -= delta_sending;
							jproc++;
						}
					} //post
				}//weight>
			}//iproc

			for (int i = m_nproc-1; i>0; i--){

				double pre_weight = 0.0;
				for (int j=i-1; j>=0; j--){
					pre_weight += temp_local_weight[j];
				}
				if (temp_local_weight[i] > division_result){

					delta[i] = temp_local_weight[i] - division_result;
					if (pre_weight < division_result*(i)){

						double pre_delta =  division_result*(i) - pre_weight;
						double delta_sending = min(local_weight[i], min(temp_local_weight[i], min(delta[i], pre_delta)));
						int jproc = i-1;
						double sending = 0;
						while (delta_sending > 0 && jproc >=0){
							sending = min(division_result, delta_sending);
							sending = min(sending, (division_result-temp_local_weight[jproc]));
							sending = max(sending, 0.0);
							sending_weight[i][jproc] += sending;
							temp_local_weight[jproc] += sending;
							temp_local_weight[i] -= sending;
							delta_sending -= sending;
							delta[i] -= delta_sending;
							jproc--;
						}
					}//pre
				}//weight>
			}//iproc
		}//iter

		//Update partition locally
		//to send
		u32vector sending_cell(m_nproc,0);
		int i = getNumOctants();;
		for (int jproc=m_nproc-1; jproc>m_rank; jproc--){
			double pack_weight = 0.0;
			while(pack_weight < sending_weight[m_rank][jproc] && i > 0){
				i--;
				pack_weight += (*weight)[i];
				sending_cell[jproc]++;
			}
		}
		partition[m_rank] = i;
		i = 0;
		for (int jproc=0; jproc<m_rank; jproc++){
			double pack_weight = 0.0;
			while(pack_weight < sending_weight[m_rank][jproc] && i <  getNumOctants()-1){
				i++;
				pack_weight += (*weight)[i];
				sending_cell[jproc]++;
			}
		}
		partition[m_rank] -= i;

		//to receive
		u32vector rec_cell(m_nproc,0);
		MPI_Request* req = new MPI_Request[m_nproc*10];
		MPI_Status* stats = new MPI_Status[m_nproc*10];
		int nReq = 0;
		for (int iproc=0; iproc<m_nproc; iproc++){
			m_errorFlag = MPI_Irecv(&rec_cell[iproc],1,MPI_UINT32_T,iproc,m_rank,m_comm,&req[nReq]);
			++nReq;
		}
		for (int iproc=0; iproc<m_nproc; iproc++){
			m_errorFlag =  MPI_Isend(&sending_cell[iproc],1,MPI_UINT32_T,iproc,iproc,m_comm,&req[nReq]);
			++nReq;
		}
		MPI_Waitall(nReq,req,stats);

		delete [] req; req = NULL;
		delete [] stats; stats = NULL;

		i = 0;
		for (int jproc=0; jproc<m_nproc; jproc++){
			i+= rec_cell[jproc];
		}
		partition[m_rank] += i;
		uint32_t part = partition[m_rank];
		m_errorFlag = MPI_Allgather(&part,1,MPI_UINT32_T,partition,1,MPI_UINT32_T,m_comm);
	}
};

/*! Compute the partition of the octree over the processes (only compute the information about
 * how distribute the mesh). This is a "compact families" method: the families of octants
 * of a desired level are retained compact on the same process.
 * \param[out] partition Pointer to partition information array. partition[i] = number of octants
 * to be stored on the i-th process (i-th rank).
 * \param[in] level Number of level over the max depth reached in the tree at
 * which families of octants are fixed compact on the same process
 * (level=0 is uniform partition).
 */
void
ClassParaTree::computePartition(uint32_t* partition, uint8_t & level_) {

	uint8_t level = uint8_t(min(int(max(int(m_maxDepth) - int(level_), int(1))) , int(m_global.m_maxLevel)));
	uint32_t* partition_temp = new uint32_t[m_nproc];
	uint8_t* boundary_proc = new uint8_t[m_nproc-1];
	uint8_t dimcomm, indcomm;
	uint8_t* glbdimcomm = new uint8_t[m_nproc];
	uint8_t* glbindcomm = new uint8_t[m_nproc];

	uint32_t division_result = 0;
	uint32_t remind = 0;
	uint32_t Dh = uint32_t(pow(double(2),double(m_global.m_maxLevel-level)));
	uint32_t istart, nocts, rest, forw, backw;
	uint32_t i = 0, iproc, j;
	uint64_t sum;
	int32_t* pointercomm;
	int32_t* deplace = new int32_t[m_nproc-1];
	division_result = uint32_t(m_globalNumOctants/(uint64_t)m_nproc);
	remind = (uint32_t)(m_globalNumOctants%(uint64_t)m_nproc);
	for(uint32_t i = 0; i < (uint32_t)m_nproc; ++i)
		if(i<remind)
			partition_temp[i] = division_result + 1;
		else
			partition_temp[i] = division_result;

	j = 0;
	sum = 0;
	for (iproc=0; iproc<(uint32_t)(m_nproc-1); iproc++){
		sum += partition_temp[iproc];
		while(sum > m_partitionRangeGlobalIdx[j]){
			j++;
		}
		boundary_proc[iproc] = j;
	}
	nocts = m_octree.m_octants.size();
	sum = 0;
	dimcomm = 0;
	indcomm = 0;
	for (iproc=0; iproc<(uint32_t)(m_nproc-1); iproc++){
		deplace[iproc] = 1;
		sum += partition_temp[iproc];
		if (boundary_proc[iproc] == m_rank){
			if (dimcomm == 0){
				indcomm = iproc;
			}
			dimcomm++;
			if (m_rank!=0)
				istart = sum - m_partitionRangeGlobalIdx[m_rank-1] - 1;
			else
				istart = sum;

			i = istart;
			rest = m_octree.m_octants[i].getX()%Dh + m_octree.m_octants[i].getY()%Dh;
			while(rest!=0){
				if (i==nocts){
					i = istart + nocts;
					break;
				}
				i++;
				rest = m_octree.m_octants[i].getX()%Dh + m_octree.m_octants[i].getY()%Dh;
			}
			forw = i - istart;
			i = istart;
			rest = m_octree.m_octants[i].getX()%Dh + m_octree.m_octants[i].getY()%Dh;
			while(rest!=0){
				if (i==0){
					i = istart - nocts;
					break;
				}
				i--;
				rest = m_octree.m_octants[i].getX()%Dh + m_octree.m_octants[i].getY()%Dh;
			}
			backw = istart - i;
			if (forw<backw)
				deplace[iproc] = forw;
			else
				deplace[iproc] = -(int32_t)backw;
		}
	}

	m_errorFlag = MPI_Allgather(&dimcomm,1,MPI_UINT8_T,glbdimcomm,1,MPI_UINT8_T,m_comm);
	m_errorFlag = MPI_Allgather(&indcomm,1,MPI_UINT8_T,glbindcomm,1,MPI_UINT8_T,m_comm);
	for (iproc=0; iproc<(uint32_t)(m_nproc); iproc++){
		pointercomm = &deplace[glbindcomm[iproc]];
		m_errorFlag = MPI_Bcast(pointercomm, glbdimcomm[iproc], MPI_INT32_T, iproc, m_comm);
	}

	for (iproc=0; iproc<(uint32_t)(m_nproc); iproc++){
		if (iproc < (uint32_t)(m_nproc-1))
			partition[iproc] = partition_temp[iproc] + deplace[iproc];
		else
			partition[iproc] = partition_temp[iproc];
		if (iproc !=0)
			partition[iproc] = partition[iproc] - deplace[iproc-1];
	}

	delete [] partition_temp; partition_temp = NULL;
	delete [] boundary_proc; boundary_proc = NULL;
	delete [] glbdimcomm; glbdimcomm = NULL;
	delete [] glbindcomm; glbindcomm = NULL;
	delete [] deplace; deplace = NULL;
}

/*! Update the distributed octree after a LoadBalance over the processes.
 */
void
ClassParaTree::updateLoadBalance() {
	m_octree.updateLocalMaxDepth();
	uint64_t* rbuff = new uint64_t[m_nproc];
	uint64_t local_num_octants = m_octree.getNumOctants();
	m_errorFlag = MPI_Allgather(&local_num_octants,1,MPI_UINT64_T,rbuff,1,MPI_UINT64_T,m_comm);
	for(int p = 0; p < m_nproc; ++p){
		m_partitionRangeGlobalIdx[p] = 0;
		for(int pp = 0; pp <=p; ++pp)
			m_partitionRangeGlobalIdx[p] += rbuff[pp];
		--m_partitionRangeGlobalIdx[p];
	}
	//update first last descendant
	m_octree.setFirstDesc();
	m_octree.setLastDesc();
	//update partition_range_position
	uint64_t lastDescMorton = m_octree.getLastDesc().computeMorton();
	m_errorFlag = MPI_Allgather(&lastDescMorton,1,MPI_UINT64_T,m_partitionLastDesc,1,MPI_UINT64_T,m_comm);
	uint64_t firstDescMorton = m_octree.getFirstDesc().computeMorton();
	m_errorFlag = MPI_Allgather(&firstDescMorton,1,MPI_UINT64_T,m_partitionFirstDesc,1,MPI_UINT64_T,m_comm);
	m_serial = false;
	delete [] rbuff; rbuff = NULL;
}

/*! Build the structure with the information about ghost octants, partition boundary octants
 *  and parameters for communicate between porcesses.
 */
void
ClassParaTree::setPboundGhosts() {
	//BUILD BORDER OCTANT INDECES VECTOR (map value) TO BE SENT TO THE RIGHT PROCESS (map key)
	//find local octants to be sent as ghost to the right processes
	//it visits the local octants building virtual neighbors on each octant face
	//find the owner of these virtual neighbor and build a map (process,border octants)
	//this map contains the local octants as ghosts for neighbor processes

	// NO PBORDERS !
	ClassLocalTree::octvector::iterator end = m_octree.m_octants.end();
	ClassLocalTree::octvector::iterator begin = m_octree.m_octants.begin();
	m_bordersPerProc.clear();
	int count = 0;
	for(ClassLocalTree::octvector::iterator it = begin; it != end; ++it){
		set<int> procs;
		//if (m_rank!=0) cout << m_rank << " border face " << endl;
        //if (m_rank!=0 && it->getZ()==0) cout << "oct " << count << " info " << it->m_info << endl;
        //if (m_rank!=0 && it->getZ()==0) cout << it->getX() << " " << it->getY() << "  " << it->getZ() << endl;
		//Virtual Face Neighbors
		for(uint8_t i = 0; i < m_global.m_nfaces; ++i){
			if(it->getBound(i) == false){
				//if (m_rank!=0 && it->getZ()==0) cout << "oct " << count << " face " << int(i) << " bound " << it->getBound(i) << endl;
				uint32_t virtualNeighborsSize = 0;
				vector<uint64_t> virtualNeighbors = it->computeVirtualMorton(i,m_maxDepth,virtualNeighborsSize,m_global.m_maxLevel);
				uint32_t maxDelta = virtualNeighborsSize/2;
				for(uint32_t j = 0; j <= maxDelta; ++j){
					//if (m_rank!=0 && it->getZ()==0) cout << "oct " << count << " j " << j << " face " << int(i) << " searching for pBegin " << virtualNeighbors[j] << endl;
					int pBegin = findOwner(virtualNeighbors[j]);
					//if (m_rank!=0) cout << j << " pBegin " << pBegin << endl;
					//if (m_rank!=0) cout << j << " searching for pEnd " << virtualNeighbors[virtualNeighborsSize - 1 - j] << endl;
					int pEnd = findOwner(virtualNeighbors[virtualNeighborsSize - 1 - j]);
					//if (m_rank!=0) cout << j << " pEnd " << pEnd << endl;
					procs.insert(pBegin);
					procs.insert(pEnd);
					if(pBegin != m_rank || pEnd != m_rank){
						it->setPbound(i,true);
					}
					else{
						it->setPbound(i,false);
					}
				}
			}
		}
		// if (m_rank!=0) cout << m_rank << " border edge " << endl;
		//Virtual Edge Neighbors
		for(uint8_t e = 0; e < m_global.m_nedges; ++e){
			uint32_t virtualEdgeNeighborSize = 0;
			vector<uint64_t> virtualEdgeNeighbors = it->computeEdgeVirtualMorton(e,m_maxDepth,virtualEdgeNeighborSize,m_octree.m_balanceCodim, m_global.m_maxLevel, m_global.m_edgeFace);
			uint32_t maxDelta = virtualEdgeNeighborSize/2;
			if(virtualEdgeNeighborSize){
				for(uint32_t ee = 0; ee <= maxDelta; ++ee){
					int pBegin = findOwner(virtualEdgeNeighbors[ee]);
					int pEnd = findOwner(virtualEdgeNeighbors[virtualEdgeNeighborSize - 1- ee]);
					procs.insert(pBegin);
					procs.insert(pEnd);
				}
			}
		}
		//if (m_rank!=0) cout << m_rank << " border corner " << endl;
		//Virtual Corner Neighbors
		for(uint8_t c = 0; c < m_global.m_nnodes; ++c){
			if(!it->getBound(m_global.m_nodeFace[c][0]) && !it->getBound(m_global.m_nodeFace[c][1])){
				uint32_t virtualCornerNeighborSize = 0;
				uint64_t virtualCornerNeighbor = it ->computeNodeVirtualMorton(c,m_maxDepth,virtualCornerNeighborSize, m_global.m_maxLevel, m_global.m_nodeFace);
				if(virtualCornerNeighborSize){
					int proc = findOwner(virtualCornerNeighbor);
					procs.insert(proc);
				}
			}
		}

		set<int>::iterator pitend = procs.end();
		for(set<int>::iterator pit = procs.begin(); pit != pitend; ++pit){
			int p = *pit;
			if(p != m_rank){
				//TODO better reserve to avoid if
				m_bordersPerProc[p].push_back(distance(begin,it));
				vector<uint32_t> & bordersSingleProc = m_bordersPerProc[p];
				if(bordersSingleProc.capacity() - bordersSingleProc.size() < 2)
					bordersSingleProc.reserve(2*bordersSingleProc.size());
			}
		}
		count++;
	}

	//cout << m_rank << " border end " << endl;

	MPI_Barrier(m_comm);

	//	cout << m_rank << " pack " << endl;
	//PACK (mpi) BORDER OCTANTS IN CHAR BUFFERS WITH SIZE (map value) TO BE SENT TO THE RIGHT PROCESS (map key)
	//it visits every element in m_bordersPerProc (one for every neighbor proc)
	//for every element it visits the border octants it contains and pack them in a new structure, sendBuffers
	//this map has an entry ClassCommBuffer for every proc containing the size in bytes of the buffer and the octants
	//to be sent to that proc packed in a char* buffer
	uint64_t global_index;
	uint32_t x,y,z;
	uint8_t l;
	int8_t m;
	bool info[17];
	map<int,ClassCommBuffer> sendBuffers;
	map<int,vector<uint32_t> >::iterator bitend = m_bordersPerProc.end();
	uint32_t pbordersOversize = 0;
	for(map<int,vector<uint32_t> >::iterator bit = m_bordersPerProc.begin(); bit != bitend; ++bit){
		pbordersOversize += bit->second.size();
		//int buffSize = bit->second.size() * (int)ceil((double)(m_global.m_octantBytes + m_global.m_globalIndexBytes) / (double)(CHAR_BIT/8));
		int buffSize = bit->second.size() * (int)ceil((double)(m_global.m_octantBytes + m_global.m_globalIndexBytes) / (double)(CHAR_BIT/8));
		int key = bit->first;
		const vector<uint32_t> & value = bit->second;
		sendBuffers[key] = ClassCommBuffer(buffSize,'a',m_comm);
		int pos = 0;
		int nofBorders = value.size();
		for(int i = 0; i < nofBorders; ++i){
			//the use of auxiliary variable can be avoided passing to MPI_Pack the members of octant but octant in that case cannot be const
			const ClassOctant & octant = m_octree.m_octants[value[i]];
			x = octant.getX();
			y = octant.getY();
			z = octant.getZ();
			l = octant.getLevel();
			m = octant.getMarker();
			global_index = getGlobalIdx(value[i]);
			for(int i = 0; i < 17; ++i)
				info[i] = octant.m_info[i];
			m_errorFlag = MPI_Pack(&x,1,MPI_UINT32_T,sendBuffers[key].m_commBuffer,buffSize,&pos,m_comm);
			m_errorFlag = MPI_Pack(&y,1,MPI_UINT32_T,sendBuffers[key].m_commBuffer,buffSize,&pos,m_comm);
			m_errorFlag = MPI_Pack(&z,1,MPI_UINT32_T,sendBuffers[key].m_commBuffer,buffSize,&pos,m_comm);
			m_errorFlag = MPI_Pack(&l,1,MPI_UINT8_T,sendBuffers[key].m_commBuffer,buffSize,&pos,m_comm);
			m_errorFlag = MPI_Pack(&m,1,MPI_INT8_T,sendBuffers[key].m_commBuffer,buffSize,&pos,m_comm);
			for(int j = 0; j < 17; ++j){
				MPI_Pack(&info[j],1,MPI::BOOL,sendBuffers[key].m_commBuffer,buffSize,&pos,m_comm);
			}
			m_errorFlag = MPI_Pack(&global_index,1,MPI_UINT64_T,sendBuffers[key].m_commBuffer,buffSize,&pos,m_comm);
		}
	}

	//COMMUNICATE THE SIZE OF BUFFER TO THE RECEIVERS
	//the size of every borders buffer is communicated to the right process in order to build the receive buffer
	//and stored in the recvBufferSizePerProc structure
	MPI_Request* req = new MPI_Request[sendBuffers.size()*20];
	MPI_Status* stats = new MPI_Status[sendBuffers.size()*20];
	int nReq = 0;
	map<int,int> recvBufferSizePerProc;
	map<int,ClassCommBuffer>::iterator sitend = sendBuffers.end();
	for(map<int,ClassCommBuffer>::iterator sit = sendBuffers.begin(); sit != sitend; ++sit){
		recvBufferSizePerProc[sit->first] = 0;
		m_errorFlag = MPI_Irecv(&recvBufferSizePerProc[sit->first],1,MPI_UINT32_T,sit->first,m_rank,m_comm,&req[nReq]);
		++nReq;
	}
	map<int,ClassCommBuffer>::reverse_iterator rsitend = sendBuffers.rend();
	for(map<int,ClassCommBuffer>::reverse_iterator rsit = sendBuffers.rbegin(); rsit != rsitend; ++rsit){
		m_errorFlag =  MPI_Isend(&rsit->second.m_commBufferSize,1,MPI_UINT32_T,rsit->first,rsit->first,m_comm,&req[nReq]);
		++nReq;
	}
	MPI_Waitall(nReq,req,stats);

	//COMMUNICATE THE BUFFERS TO THE RECEIVERS
	//recvBuffers structure is declared and each buffer is initialized to the right size
	//then, sendBuffers are communicated by senders and stored in recvBuffers in the receivers
	//at the same time every process compute the size in bytes of all the ghost octants
	uint32_t nofBytesOverProc = 0;
	map<int,ClassCommBuffer> recvBuffers;
	map<int,int>::iterator ritend = recvBufferSizePerProc.end();
	for(map<int,int>::iterator rit = recvBufferSizePerProc.begin(); rit != ritend; ++rit){
		recvBuffers[rit->first] = ClassCommBuffer(rit->second,'a',m_comm);
	}
	nReq = 0;
	for(map<int,ClassCommBuffer>::iterator sit = sendBuffers.begin(); sit != sitend; ++sit){
		nofBytesOverProc += recvBuffers[sit->first].m_commBufferSize;
		m_errorFlag = MPI_Irecv(recvBuffers[sit->first].m_commBuffer,recvBuffers[sit->first].m_commBufferSize,MPI_PACKED,sit->first,m_rank,m_comm,&req[nReq]);
		++nReq;
	}
	for(map<int,ClassCommBuffer>::reverse_iterator rsit = sendBuffers.rbegin(); rsit != rsitend; ++rsit){
		m_errorFlag =  MPI_Isend(rsit->second.m_commBuffer,rsit->second.m_commBufferSize,MPI_PACKED,rsit->first,rsit->first,m_comm,&req[nReq]);
		++nReq;
	}
	MPI_Waitall(nReq,req,stats);

	//COMPUTE GHOSTS SIZE IN BYTES
	//number of ghosts in every process is obtained through the size in bytes of the single octant
	//and ghost vector in local tree is resized
	//uint32_t nofGhosts = nofBytesOverProc / (uint32_t)(m_global.m_octantBytes + m_global.m_globalIndexBytes);
	uint32_t nofGhosts = nofBytesOverProc / (uint32_t)(m_global.m_octantBytes + m_global.m_globalIndexBytes);
	m_octree.m_sizeGhosts = nofGhosts;
	m_octree.m_ghosts.clear();
	m_octree.m_ghosts.resize(nofGhosts);
	m_octree.m_globalIdxGhosts.resize(nofGhosts);

	//UNPACK BUFFERS AND BUILD GHOSTS CONTAINER OF CLASS_LOCAL_TREE
	//every entry in recvBuffers is visited, each buffers from neighbor processes is unpacked octant by octant.
	//every ghost octant is built and put in the ghost vector
	uint32_t ghostCounter = 0;
	map<int,ClassCommBuffer>::iterator rritend = recvBuffers.end();
	for(map<int,ClassCommBuffer>::iterator rrit = recvBuffers.begin(); rrit != rritend; ++rrit){
		int pos = 0;
		//			int nofGhostsPerProc = int(rrit->second.m_commBufferSize / (uint32_t) (m_global.m_octantBytes + m_global.m_globalIndexBytes));
		int nofGhostsPerProc = int(rrit->second.m_commBufferSize / (uint32_t) (m_global.m_octantBytes + m_global.m_globalIndexBytes));
		for(int i = 0; i < nofGhostsPerProc; ++i){
			m_errorFlag = MPI_Unpack(rrit->second.m_commBuffer,rrit->second.m_commBufferSize,&pos,&x,1,MPI_UINT32_T,m_comm);
			m_errorFlag = MPI_Unpack(rrit->second.m_commBuffer,rrit->second.m_commBufferSize,&pos,&y,1,MPI_UINT32_T,m_comm);
			m_errorFlag = MPI_Unpack(rrit->second.m_commBuffer,rrit->second.m_commBufferSize,&pos,&z,1,MPI_UINT32_T,m_comm);
			m_errorFlag = MPI_Unpack(rrit->second.m_commBuffer,rrit->second.m_commBufferSize,&pos,&l,1,MPI_UINT8_T,m_comm);
			m_octree.m_ghosts[ghostCounter] = ClassOctant(m_dim,l,x,y,z);
			m_errorFlag = MPI_Unpack(rrit->second.m_commBuffer,rrit->second.m_commBufferSize,&pos,&m,1,MPI_INT8_T,m_comm);
			m_octree.m_ghosts[ghostCounter].setMarker(m);
			for(int j = 0; j < 17; ++j){
				m_errorFlag = MPI_Unpack(rrit->second.m_commBuffer,rrit->second.m_commBufferSize,&pos,&info[j],1,MPI::BOOL,m_comm);
				m_octree.m_ghosts[ghostCounter].m_info[j] = info[j];
			}
			m_errorFlag = MPI_Unpack(rrit->second.m_commBuffer,rrit->second.m_commBufferSize,&pos,&global_index,1,MPI_UINT64_T,m_comm);
			m_octree.m_globalIdxGhosts[ghostCounter] = global_index;
			++ghostCounter;
		}
	}
	recvBuffers.clear();
	sendBuffers.clear();
	recvBufferSizePerProc.clear();

	delete [] req; req = NULL;
	delete [] stats; stats = NULL;

}

/*! Communicate the marker of the octants and the auxiliary info[15].
 */
void
ClassParaTree::commMarker() {
	//PACK (mpi) LEVEL AND MARKER OF BORDER OCTANTS IN CHAR BUFFERS WITH SIZE (map value) TO BE SENT TO THE RIGHT PROCESS (map key)
	//it visits every element in m_bordersPerProc (one for every neighbor proc)
	//for every element it visits the border octants it contains and pack its marker in a new structure, sendBuffers
	//this map has an entry ClassCommBuffer for every proc containing the size in bytes of the buffer and the octants marker
	//to be sent to that proc packed in a char* buffer
	int8_t marker;
	bool mod;
	map<int,ClassCommBuffer> sendBuffers;
	map<int,vector<uint32_t> >::iterator bitend = m_bordersPerProc.end();
	uint32_t pbordersOversize = 0;
	for(map<int,vector<uint32_t> >::iterator bit = m_bordersPerProc.begin(); bit != bitend; ++bit){
		pbordersOversize += bit->second.size();
		int buffSize = bit->second.size() * (int)ceil((double)(m_global.m_markerBytes + m_global.m_boolBytes) / (double)(CHAR_BIT/8));
		int key = bit->first;
		const vector<uint32_t> & value = bit->second;
		sendBuffers[key] = ClassCommBuffer(buffSize,'a',m_comm);
		int pos = 0;
		int nofBorders = value.size();
		for(int i = 0; i < nofBorders; ++i){
			//the use of auxiliary variable can be avoided passing to MPI_Pack the members of octant but octant in that case cannot be const
			const ClassOctant & octant = m_octree.m_octants[value[i]];
			marker = octant.getMarker();
			mod	= octant.m_info[15];
			m_errorFlag = MPI_Pack(&marker,1,MPI_INT8_T,sendBuffers[key].m_commBuffer,buffSize,&pos,m_comm);
			m_errorFlag = MPI_Pack(&mod,1,MPI::BOOL,sendBuffers[key].m_commBuffer,buffSize,&pos,m_comm);
		}
	}

	//COMMUNICATE THE SIZE OF BUFFER TO THE RECEIVERS
	//the size of every borders buffer is communicated to the right process in order to build the receive buffer
	//and stored in the recvBufferSizePerProc structure
	MPI_Request* req = new MPI_Request[sendBuffers.size()*2];
	MPI_Status* stats = new MPI_Status[sendBuffers.size()*2];
	int nReq = 0;
	map<int,int> recvBufferSizePerProc;
	map<int,ClassCommBuffer>::iterator sitend = sendBuffers.end();
	for(map<int,ClassCommBuffer>::iterator sit = sendBuffers.begin(); sit != sitend; ++sit){
		recvBufferSizePerProc[sit->first] = 0;
		m_errorFlag = MPI_Irecv(&recvBufferSizePerProc[sit->first],1,MPI_UINT32_T,sit->first,m_rank,m_comm,&req[nReq]);
		++nReq;
	}
	map<int,ClassCommBuffer>::reverse_iterator rsitend = sendBuffers.rend();
	for(map<int,ClassCommBuffer>::reverse_iterator rsit = sendBuffers.rbegin(); rsit != rsitend; ++rsit){
		m_errorFlag =  MPI_Isend(&rsit->second.m_commBufferSize,1,MPI_UINT32_T,rsit->first,rsit->first,m_comm,&req[nReq]);
		++nReq;
	}
	MPI_Waitall(nReq,req,stats);

	//COMMUNICATE THE BUFFERS TO THE RECEIVERS
	//recvBuffers structure is declared and each buffer is initialized to the right size
	//then, sendBuffers are communicated by senders and stored in recvBuffers in the receivers
	//at the same time every process compute the size in bytes of all the level and marker of ghost octants
	uint32_t nofBytesOverProc = 0;
	map<int,ClassCommBuffer> recvBuffers;
	map<int,int>::iterator ritend = recvBufferSizePerProc.end();
	for(map<int,int>::iterator rit = recvBufferSizePerProc.begin(); rit != ritend; ++rit){
		recvBuffers[rit->first] = ClassCommBuffer(rit->second,'a',m_comm);
	}
	nReq = 0;
	for(map<int,ClassCommBuffer>::iterator sit = sendBuffers.begin(); sit != sitend; ++sit){
		nofBytesOverProc += recvBuffers[sit->first].m_commBufferSize;
		m_errorFlag = MPI_Irecv(recvBuffers[sit->first].m_commBuffer,recvBuffers[sit->first].m_commBufferSize,MPI_PACKED,sit->first,m_rank,m_comm,&req[nReq]);
		++nReq;
	}
	for(map<int,ClassCommBuffer>::reverse_iterator rsit = sendBuffers.rbegin(); rsit != rsitend; ++rsit){
		m_errorFlag =  MPI_Isend(rsit->second.m_commBuffer,rsit->second.m_commBufferSize,MPI_PACKED,rsit->first,rsit->first,m_comm,&req[nReq]);
		++nReq;
	}
	MPI_Waitall(nReq,req,stats);

	//UNPACK BUFFERS AND BUILD GHOSTS CONTAINER OF CLASS_LOCAL_TREE
	//every entry in recvBuffers is visited, each buffers from neighbor processes is unpacked octant by octant.
	//every ghost octant is built and put in the ghost vector
	uint32_t ghostCounter = 0;
	map<int,ClassCommBuffer>::iterator rritend = recvBuffers.end();
	for(map<int,ClassCommBuffer>::iterator rrit = recvBuffers.begin(); rrit != rritend; ++rrit){
		int pos = 0;
		int nofGhostsPerProc = int(rrit->second.m_commBufferSize / ((uint32_t) (m_global.m_markerBytes + m_global.m_boolBytes)));
		for(int i = 0; i < nofGhostsPerProc; ++i){
			m_errorFlag = MPI_Unpack(rrit->second.m_commBuffer,rrit->second.m_commBufferSize,&pos,&marker,1,MPI_INT8_T,m_comm);
			m_octree.m_ghosts[ghostCounter].setMarker(marker);
			m_errorFlag = MPI_Unpack(rrit->second.m_commBuffer,rrit->second.m_commBufferSize,&pos,&mod,1,MPI::BOOL,m_comm);
			m_octree.m_ghosts[ghostCounter].m_info[15] = mod;
			++ghostCounter;
		}
	}
	recvBuffers.clear();
	sendBuffers.clear();
	recvBufferSizePerProc.clear();
	delete [] req; req = NULL;
	delete [] stats; stats = NULL;

}
#endif

/*! Update the distributed octree over the processes after a coarsening procedure.
 */
void
ClassParaTree::updateAfterCoarse(){
	m_mapIdx.clear();
#if ENABLE_MPI
	if(m_serial){
#endif
		updateAdapt();
#if ENABLE_MPI
	}
	else{
		//Only if parallel
		updateAdapt();
		uint64_t lastDescMortonPre, firstDescMortonPost;
		lastDescMortonPre = (m_rank!=0) * m_partitionLastDesc[m_rank-1];
		firstDescMortonPost = (m_rank<m_nproc-1)*m_partitionFirstDesc[m_rank+1] + (m_rank==m_nproc-1)*m_partitionLastDesc[m_rank];
		m_octree.checkCoarse(lastDescMortonPre, firstDescMortonPost, m_mapIdx);
		updateAdapt();
	}
#endif
}

/*! Update the distributed octree over the processes after a coarsening procedure
 * and track the change in a mapper.
 * \param[out] mapidx Mapper from new octants to old octants.
 * I.e. mapper[i] = j -> the i-th octant after adapt was in the j-th position before adapt.
 */
void
ClassParaTree::updateAfterCoarse(u32vector & mapidx){
#if ENABLE_MPI
	if(m_serial){
#endif
		updateAdapt();
#if ENABLE_MPI
	}
	else{
		//Only if parallel
		updateAdapt();
		uint64_t lastDescMortonPre, firstDescMortonPost;
		lastDescMortonPre = (m_rank!=0) * m_partitionLastDesc[m_rank-1];
		firstDescMortonPost = (m_rank<m_nproc-1)*m_partitionFirstDesc[m_rank+1] + (m_rank==m_nproc-1)*m_partitionLastDesc[m_rank];
		m_octree.checkCoarse(lastDescMortonPre, firstDescMortonPost, mapidx);
		updateAdapt();
	}

#endif
}

/*!Balance 2:1 the octree.
 * \param[in] first Is the first call of the 2:1 balance method?
 */
void
ClassParaTree::balance21(bool const first){
#if ENABLE_MPI
	bool globalDone = true, localDone = false;
	int  iteration  = 0;

	commMarker();
	m_octree.preBalance21(true);

	if (first){
		m_log.writeLog("---------------------------------------------");
		m_log.writeLog(" 2:1 BALANCE (balancing Marker before Adapt)");
		m_log.writeLog(" ");
		m_log.writeLog(" Iterative procedure	");
		m_log.writeLog(" ");
		m_log.writeLog(" Iteration	:	" + to_string(static_cast<unsigned long long>(iteration)));

		commMarker();

		localDone = m_octree.localBalance(true);
		commMarker();
		m_octree.preBalance21(false);
		MPI_Barrier(m_comm);
		m_errorFlag = MPI_Allreduce(&localDone,&globalDone,1,MPI::BOOL,MPI_LOR,m_comm);

		while(globalDone){
			iteration++;
			m_log.writeLog(" Iteration	:	" + to_string(static_cast<unsigned long long>(iteration)));
			commMarker();
			localDone = m_octree.localBalance(false);
			commMarker();
			m_octree.preBalance21(false);
			m_errorFlag = MPI_Allreduce(&localDone,&globalDone,1,MPI::BOOL,MPI_LOR,m_comm);
		}

		commMarker();
		m_log.writeLog(" Iteration	:	Finalizing ");
		m_log.writeLog(" ");
		//localDone = m_octree.localBalance(false);
		//commMarker();
		//m_octree.preBalance21(true);
		//commMarker();

		m_log.writeLog(" 2:1 Balancing reached ");
		m_log.writeLog(" ");
		m_log.writeLog("---------------------------------------------");

	}
	else{

		commMarker();
		MPI_Barrier(m_comm);
		localDone = m_octree.localBalanceAll(true);
		commMarker();
		m_octree.preBalance21(false);
		MPI_Barrier(m_comm);
		m_errorFlag = MPI_Allreduce(&localDone,&globalDone,1,MPI::BOOL,MPI_LOR,m_comm);

		while(globalDone){
			iteration++;
			commMarker();
			localDone = m_octree.localBalanceAll(false);
			commMarker();
			m_octree.preBalance21(false);
			m_errorFlag = MPI_Allreduce(&localDone,&globalDone,1,MPI::BOOL,MPI_LOR,m_comm);
		}

		commMarker();
		//			localDone = m_octree.localBalance(false);
		//			commMarker();
		//			m_octree.preBalance21(false);
		//			commMarker();

	}
#else
	bool localDone = false;
	int  iteration  = 0;

	m_octree.preBalance21(true);

	if (first){
		m_log.writeLog("---------------------------------------------");
		m_log.writeLog(" 2:1 BALANCE (balancing Marker before Adapt)");
		m_log.writeLog(" ");
		m_log.writeLog(" Iterative procedure	");
		m_log.writeLog(" ");
		m_log.writeLog(" Iteration	:	" + to_string(static_cast<unsigned long long>(iteration)));

		localDone = m_octree.localBalance(true);
		m_octree.preBalance21(false);

		while(localDone){
			iteration++;
			m_log.writeLog(" Iteration	:	" + to_string(static_cast<unsigned long long>(iteration)));
			localDone = m_octree.localBalance(false);
			m_octree.preBalance21(false);
		}

		m_log.writeLog(" Iteration	:	Finalizing ");
		m_log.writeLog(" ");
		//			localDone = m_octree.localBalance(false);
		//			m_octree.preBalance21(false);

		m_log.writeLog(" 2:1 Balancing reached ");
		m_log.writeLog(" ");
		m_log.writeLog("---------------------------------------------");

	}
	else{

		localDone = m_octree.localBalanceAll(true);
		m_octree.preBalance21(false);

		while(localDone){
			iteration++;
			localDone = m_octree.localBalanceAll(false);
			m_octree.preBalance21(false);
		}

		//			localDone = m_octree.localBalance(false);
		//			m_octree.preBalance21(false);

	}

#endif /* NOMPI */
}

// =================================================================================== //
// TESTING OUTPUT METHODS												    			   //
// =================================================================================== //

/** Write the physical octree mesh in .vtu format in a user-defined file.
 * If the connectivity is not stored, the method temporary computes it.
 * If the connectivity of ghost octants is already computed, the method writes the ghosts on file.
 * \param[in] filename Seriously?....
 */
void
ClassParaTree::write(string filename) {

	bool clear = false;
	if (m_octree.m_connectivity.size() == 0) {
		m_octree.computeConnectivity();
		clear = true;
	}

	stringstream name;
	name << "s" << std::setfill('0') << std::setw(4) << m_nproc << "-p" << std::setfill('0') << std::setw(4) << m_rank << "-" << filename << ".vtu";

	ofstream out(name.str().c_str());
	if(!out.is_open()){
		stringstream ss;
		ss << filename << "*.vtu cannot be opened and it won't be written.";
		m_log.writeLog(ss.str());
		return;
	}
	int nofNodes = m_octree.m_nodes.size();
	int nofGhostNodes = m_octree.m_ghostsNodes.size();
	int nofOctants = m_octree.m_connectivity.size();
	int nofGhosts = m_octree.m_ghostsConnectivity.size();
	int nofAll = nofGhosts + nofOctants;
	out << "<?xml version=\"1.0\"?>" << endl
			<< "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"BigEndian\">" << endl
			<< "  <UnstructuredGrid>" << endl
			<< "    <Piece NumberOfCells=\"" << m_octree.m_connectivity.size() + m_octree.m_ghostsConnectivity.size() << "\" NumberOfPoints=\"" << m_octree.m_nodes.size() + m_octree.m_ghostsNodes.size() << "\">" << endl;
	out << "      <Points>" << endl
			<< "        <DataArray type=\"Float64\" Name=\"Coordinates\" NumberOfComponents=\""<< 3 <<"\" format=\"ascii\">" << endl
			<< "          " << std::fixed;
	for(int i = 0; i < nofNodes; i++)
	{
		for(int j = 0; j < 3; ++j){
			if (j==0) out << std::setprecision(6) << m_trans.mapX(m_octree.m_nodes[i][j]) << " ";
			if (j==1) out << std::setprecision(6) << m_trans.mapY(m_octree.m_nodes[i][j]) << " ";
			if (j==2) out << std::setprecision(6) << m_trans.mapZ(m_octree.m_nodes[i][j]) << " ";
		}
		if((i+1)%4==0 && i!=nofNodes-1)
			out << endl << "          ";
	}
	for(int i = 0; i < nofGhostNodes; i++)
	{
		for(int j = 0; j < 3; ++j){
			if (j==0) out << std::setprecision(6) << m_trans.mapX(m_octree.m_ghostsNodes[i][j]) << " ";
			if (j==1) out << std::setprecision(6) << m_trans.mapY(m_octree.m_ghostsNodes[i][j]) << " ";
			if (j==2) out << std::setprecision(6) << m_trans.mapZ(m_octree.m_ghostsNodes[i][j]) << " ";
		}
		if((i+1)%4==0 && i!=nofNodes-1)
			out << endl << "          ";
	}
	out << endl << "        </DataArray>" << endl
			<< "      </Points>" << endl
			<< "      <Cells>" << endl
			<< "        <DataArray type=\"UInt64\" Name=\"connectivity\" NumberOfComponents=\"1\" format=\"ascii\">" << endl
			<< "          ";
	for(int i = 0; i < nofOctants; i++)
	{
		for(int j = 0; j < m_global.m_nnodes; j++)
		{
			int jj = j;
			if (m_dim==2){
				if (j<2){
					jj = j;
				}
				else if(j==2){
					jj = 3;
				}
				else if(j==3){
					jj = 2;
				}
			}
			out << m_octree.m_connectivity[i][jj] << " ";
		}
		if((i+1)%3==0 && i!=nofOctants-1)
			out << endl << "          ";
	}
	for(int i = 0; i < nofGhosts; i++)
	{
		for(int j = 0; j < m_global.m_nnodes; j++)
		{
			int jj = j;
			if (m_dim==2){
				if (j<2){
					jj = j;
				}
				else if(j==2){
					jj = 3;
				}
				else if(j==3){
					jj = 2;
				}
			}
			out << m_octree.m_ghostsConnectivity[i][jj] + nofNodes << " ";
		}
		if((i+1)%3==0 && i!=nofGhosts-1)
			out << endl << "          ";
	}
	out << endl << "        </DataArray>" << endl
			<< "        <DataArray type=\"UInt64\" Name=\"offsets\" NumberOfComponents=\"1\" format=\"ascii\">" << endl
			<< "          ";
	for(int i = 0; i < nofAll; i++)
	{
		out << (i+1)*m_global.m_nnodes << " ";
		if((i+1)%12==0 && i!=nofAll-1)
			out << endl << "          ";
	}
	out << endl << "        </DataArray>" << endl
			<< "        <DataArray type=\"UInt8\" Name=\"types\" NumberOfComponents=\"1\" format=\"ascii\">" << endl
			<< "          ";
	for(int i = 0; i < nofAll; i++)
	{
		int type;
		type = 5 + (m_dim*2);
		out << type << " ";
		if((i+1)%12==0 && i!=nofAll-1)
			out << endl << "          ";
	}
	out << endl << "        </DataArray>" << endl
			<< "      </Cells>" << endl
			<< "    </Piece>" << endl
			<< "  </UnstructuredGrid>" << endl
			<< "</VTKFile>" << endl;


	if(m_rank == 0){
		name.str("");
		name << "s" << std::setfill('0') << std::setw(4) << m_nproc << "-" << filename << ".pvtu";
		ofstream pout(name.str().c_str());
		if(!pout.is_open()){
			stringstream ss;
			ss << filename << "*.pvtu cannot be opened and it won't be written.";
			m_log.writeLog(ss.str());
			return;
		}

		pout << "<?xml version=\"1.0\"?>" << endl
				<< "<VTKFile type=\"PUnstructuredGrid\" version=\"0.1\" byte_order=\"BigEndian\">" << endl
				<< "  <PUnstructuredGrid GhostLevel=\"0\">" << endl
				<< "    <PPointData>" << endl
				<< "    </PPointData>" << endl
				<< "    <PCellData Scalars=\"\">" << endl;
		pout << "    </PCellData>" << endl
				<< "    <PPoints>" << endl
				<< "      <PDataArray type=\"Float64\" Name=\"Coordinates\" NumberOfComponents=\"3\"/>" << endl
				<< "    </PPoints>" << endl;
		for(int i = 0; i < m_nproc; i++)
			pout << "    <Piece Source=\"s" << std::setw(4) << std::setfill('0') << m_nproc << "-p" << std::setw(4) << std::setfill('0') << i << "-" << filename << ".vtu\"/>" << endl;
		pout << "  </PUnstructuredGrid>" << endl
				<< "</VTKFile>";

		pout.close();

	}
#if ENABLE_MPI
	MPI_Barrier(m_comm);
#endif

}

/** Write the physical octree mesh in .vtu format with data for test in a user-defined file.
 * If the connectivity is not stored, the method temporary computes it.
 * The method doesn't write the ghosts on file.
 * \param[in] filename Seriously?....
 */
void
ClassParaTree::writeTest(string filename, vector<double> data) {

	bool clear = false;
	if (m_octree.m_connectivity.size() == 0) {
		m_octree.computeConnectivity();
		clear = true;
	}

	stringstream name;
	name << "s" << std::setfill('0') << std::setw(4) << m_nproc << "-p" << std::setfill('0') << std::setw(4) << m_rank << "-" << filename << ".vtu";

	ofstream out(name.str().c_str());
	if(!out.is_open()){
		stringstream ss;
		ss << filename << "*.vtu cannot be opened and it won't be written.";
		m_log.writeLog(ss.str());
		return;
	}
	int nofNodes = m_octree.m_nodes.size();
	int nofOctants = m_octree.m_connectivity.size();
	int nofAll = nofOctants;
	out << "<?xml version=\"1.0\"?>" << endl
			<< "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"BigEndian\">" << endl
			<< "  <UnstructuredGrid>" << endl
			<< "    <Piece NumberOfCells=\"" << m_octree.m_connectivity.size() << "\" NumberOfPoints=\"" << m_octree.m_nodes.size() << "\">" << endl;
	out << "      <CellData Scalars=\"Data\">" << endl;
	out << "      <DataArray type=\"Float64\" Name=\"Data\" NumberOfComponents=\"1\" format=\"ascii\">" << endl
			<< "          " << std::fixed;
	int ndata = m_octree.m_connectivity.size();
	for(int i = 0; i < ndata; i++)
	{
		out << std::setprecision(6) << data[i] << " ";
		if((i+1)%4==0 && i!=ndata-1)
			out << endl << "          ";
	}
	out << endl << "        </DataArray>" << endl
			<< "      </CellData>" << endl
			<< "      <Points>" << endl
			<< "        <DataArray type=\"Float64\" Name=\"Coordinates\" NumberOfComponents=\""<< 3 <<"\" format=\"ascii\">" << endl
			<< "          " << std::fixed;
	for(int i = 0; i < nofNodes; i++)
	{
		for(int j = 0; j < 3; ++j){
			if (j==0) out << std::setprecision(6) << m_trans.mapX(m_octree.m_nodes[i][j]) << " ";
			if (j==1) out << std::setprecision(6) << m_trans.mapY(m_octree.m_nodes[i][j]) << " ";
			if (j==2) out << std::setprecision(6) << m_trans.mapZ(m_octree.m_nodes[i][j]) << " ";
		}
		if((i+1)%4==0 && i!=nofNodes-1)
			out << endl << "          ";
	}
	out << endl << "        </DataArray>" << endl
			<< "      </Points>" << endl
			<< "      <Cells>" << endl
			<< "        <DataArray type=\"UInt64\" Name=\"connectivity\" NumberOfComponents=\"1\" format=\"ascii\">" << endl
			<< "          ";
	for(int i = 0; i < nofOctants; i++)
	{
		for(int j = 0; j < m_global.m_nnodes; j++)
		{
			int jj = j;
			if (m_dim==2){
				if (j<2){
					jj = j;
				}
				else if(j==2){
					jj = 3;
				}
				else if(j==3){
					jj = 2;
				}
			}
			out << m_octree.m_connectivity[i][jj] << " ";
		}
		if((i+1)%3==0 && i!=nofOctants-1)
			out << endl << "          ";
	}
	out << endl << "        </DataArray>" << endl
			<< "        <DataArray type=\"UInt64\" Name=\"offsets\" NumberOfComponents=\"1\" format=\"ascii\">" << endl
			<< "          ";
	for(int i = 0; i < nofAll; i++)
	{
		out << (i+1)*m_global.m_nnodes << " ";
		if((i+1)%12==0 && i!=nofAll-1)
			out << endl << "          ";
	}
	out << endl << "        </DataArray>" << endl
			<< "        <DataArray type=\"UInt8\" Name=\"types\" NumberOfComponents=\"1\" format=\"ascii\">" << endl
			<< "          ";
	for(int i = 0; i < nofAll; i++)
	{
		int type;
		type = 5 + (m_dim*2);
		out << type << " ";
		if((i+1)%12==0 && i!=nofAll-1)
			out << endl << "          ";
	}
	out << endl << "        </DataArray>" << endl
			<< "      </Cells>" << endl
			<< "    </Piece>" << endl
			<< "  </UnstructuredGrid>" << endl
			<< "</VTKFile>" << endl;


	if(m_rank == 0){
		name.str("");
		name << "s" << std::setfill('0') << std::setw(4) << m_nproc << "-" << filename << ".pvtu";
		ofstream pout(name.str().c_str());
		if(!pout.is_open()){
			stringstream ss;
			ss << filename << "*.pvtu cannot be opened and it won't be written.";
			m_log.writeLog(ss.str());
			return;
		}

		pout << "<?xml version=\"1.0\"?>" << endl
				<< "<VTKFile type=\"PUnstructuredGrid\" version=\"0.1\" byte_order=\"BigEndian\">" << endl
				<< "  <PUnstructuredGrid GhostLevel=\"0\">" << endl
				<< "    <PPointData>" << endl
				<< "    </PPointData>" << endl
				<< "    <PCellData Scalars=\"Data\">" << endl
				<< "      <PDataArray type=\"Float64\" Name=\"Data\" NumberOfComponents=\"1\"/>" << endl
				<< "    </PCellData>" << endl
				<< "    <PPoints>" << endl
				<< "      <PDataArray type=\"Float64\" Name=\"Coordinates\" NumberOfComponents=\"3\"/>" << endl
				<< "    </PPoints>" << endl;
		for(int i = 0; i < m_nproc; i++)
			pout << "    <Piece Source=\"s" << std::setw(4) << std::setfill('0') << m_nproc << "-p" << std::setw(4) << std::setfill('0') << i << "-" << filename << ".vtu\"/>" << endl;
		pout << "  </PUnstructuredGrid>" << endl
				<< "</VTKFile>";

		pout.close();

	}
#if ENABLE_MPI
	MPI_Barrier(m_comm);
#endif

}

// =============================================================================== //



