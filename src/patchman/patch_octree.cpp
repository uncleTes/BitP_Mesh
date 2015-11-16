//
// Written by Andrea Iob <andrea_iob@hotmail.com>
//

#ifndef DISABLE_OCTREE
#include "patch_octree.hpp"

#include <math.h>

namespace pman {

/*!
	\class PatchOctree

	\brief The PatchOctree defines a Octree patch.

	PatchOctree defines a Octree patch.
*/

/*!
	Creates a new patch.
*/
PatchOctree::PatchOctree(const int &id, const int &dimension,
				 std::array<double, 3> origin, double length, double dh )
	: Patch(id, dimension)
{
	std::cout << ">> Initializing Octree mesh\n";

	// Info sull'octree
	int maxLevels;
	if (is_three_dimensional()) {
		maxLevels = 32;
	} else {
		maxLevels = 32;
	}

	m_tree_dh.resize(maxLevels);
	m_tree_area.reserve(maxLevels);
	m_tree_volume.reserve(maxLevels);
	for(int i = 0; i < maxLevels; i++) {
	    double levelLength = length / ((double) pow(2,i));

	    m_tree_dh.push_back(pow(levelLength, 1.));
	    m_tree_area.push_back(pow(levelLength, (double) (get_dimension() - 1)));
	    m_tree_volume.push_back(pow(levelLength, (double) (get_dimension())));
	};

	// Inizializzazione dell'octree
	double initial_level = ceil(log2(max(1., length / dh)));
	if (is_three_dimensional()) {
		m_tree_3D = Class_Para_Tree<3>(origin[0], origin[1], origin[2], length);
		m_tree_3D.setMarker((uint32_t) 0, initial_level);
	} else {
		m_tree_2D = Class_Para_Tree<2>(origin[0], origin[1], origin[2], length);
		m_tree_2D.setMarker((uint32_t) 0, initial_level);
	}

	// Info sulle interfacce
	for (int i = 0; i < dimension; i++) {
		for (int n = -1; n <= 1; n += 2) {
			std::array<double, 3> normal = {0.0, 0.0, 0.0};
			normal[i] = n;

			m_normals.push_back(normal);
		}
	}
}

/*!
	Destroys the patch.
*/
PatchOctree::~PatchOctree()
{

}

/*!
	Gets a pointer to the the opposite normal.

	\param normal is a pointer to the normal
	\result A pointer to the opposite normal.
 */
std::array<double, 3> & PatchOctree::_get_opposite_normal(std::array<double, 3> &normal)
{
	std::vector<std::array<double, 3> >::iterator itr_current = std::find(m_normals.begin(), m_normals.end(), normal);
	if (itr_current == m_normals.end()) {
		 throw std::out_of_range ("Input normal is not among the stored normals");
	}

	int dimension = get_dimension();

	int id_current  = std::distance(m_normals.begin(), itr_current);
	int id_opposite = (id_current + dimension) % (2 * dimension);

	return m_normals[id_opposite];
}

/*!
	Gets the octant of the cell with the specified id.

	\param id the id of the cell
	\result The octant of the specified cell
*/
long PatchOctree::get_cell_octant(const long &id) const
{
	return m_cell_to_octant.at(id);
}

/*!
	Gets the id of the specified octant.

	\param octant the index of the octant
	\result The id of the specified octant
*/
long PatchOctree::get_octant_id(const long &octant) const
{
	return m_octant_to_cell.at(octant);
}

/*!
	Gets the refinement level of the cell with the specified id.

	\param id is the id of the cell
	\result The refinement level of the specified cell.
*/
int PatchOctree::get_cell_level(const long &id)
{
	long octant = get_cell_octant(id);
	if (is_three_dimensional()) {
		return m_tree_3D.getLevel(octant);
	} else {
		return m_tree_2D.getLevel(octant);
	}
}

/*!
	Updates the patch.

	\result Returns true if the mesh was updated, false otherwise.
*/
bool PatchOctree::_update(vector<uint32_t> &cellMapping)
{
	if (!is_dirty()) {
		return false;
	}

	std::cout << ">> Updating the mesh\n";

	// Updating the tree
	bool updated;
	if (is_three_dimensional()) {
		updated = m_tree_3D.adapt(cellMapping);
	} else {
		updated = m_tree_2D.adapt(cellMapping);
	}

	if (!updated) {
		return updated;
	}

	// Evaluate tree conenctivity
	if (is_three_dimensional()) {
		m_tree_3D.computeConnectivity();
	} else {
		m_tree_2D.computeConnectivity();
	}

	// Definition of mesh
	update_vertices();
	update_cells();
	update_interfaces();

	// Delete tree conenctivity
	if (is_three_dimensional()) {
		m_tree_3D.clearConnectivity();
	} else {
		m_tree_2D.clearConnectivity();
	}

	return updated;
}

/*!
	Updates the vertices.
*/
void PatchOctree::update_vertices()
{
	if (!is_dirty()) {
		return;
	}

	if (m_vertices.empty()) {
		import_vertices();
	} else {
		reload_vertices();
	}
}

/*!
	Imports the vertices from the tree.
*/
void PatchOctree::import_vertices()
{
	std::cout << ">> Importing the vertices\n";

	// Reset
	reset_vertices();

	// Defintion of the vertices
	long nVertices;
	if (is_three_dimensional()) {
		nVertices = m_tree_3D.getNumNodes();
	} else {
		nVertices = m_tree_2D.getNumNodes();
	}

	m_vertices.reserve(nVertices);
	for (long n = 0; n < nVertices; n++) {
		m_vertices.emplace_back(n);
		Node &vertex = m_vertices.back();

		// Coordinate
		vector<double> nodeCoords;
		if (is_three_dimensional()) {
			nodeCoords = m_tree_3D.getNodeCoordinates(n);
		} else {
			nodeCoords = m_tree_2D.getNodeCoordinates(n);
		}

		std::array<double, 3> coords;
		coords[Node::COORD_X] = nodeCoords[Node::COORD_X];
		coords[Node::COORD_Y] = nodeCoords[Node::COORD_Y];
		coords[Node::COORD_Z] = nodeCoords[Node::COORD_Z];

		vertex.set_coords(coords);
	}
}

/*!
	Updates the vertices after a tree change.
*/
void PatchOctree::reload_vertices()
{
	std::cout << ">> Updating the vertices\n";

	reset_vertices();
	import_vertices();
}

/*!
	Updates the vertices.
*/
void PatchOctree::update_cells()
{
	if (!is_dirty()) {
		return;
	}

	if (m_cells.empty()) {
		import_cells();
	} else {
		reload_cells();
	}
}

/*!
	Imports the cells from the tree.
*/
void PatchOctree::import_cells()
{
	std::cout << ">> Importing the cells\n";

	// Reset
	reset_cells();

	// Info of the cells
	int nCellVertices;
	if (is_three_dimensional()) {
		nCellVertices = Element::get_vertex_count(Element::BRICK);
	} else {
		nCellVertices = Element::get_vertex_count(Element::RECTANGLE);
	}

	// Mapping della connettività
	vector<long> connectMap(nCellVertices);
	if (is_three_dimensional()) {
		connectMap[0] = 0;
		connectMap[1] = 1;
		connectMap[2] = 3;
		connectMap[3] = 2;
		connectMap[4] = 4;
		connectMap[5] = 5;
		connectMap[6] = 7;
		connectMap[7] = 6;
	} else {
		connectMap[0] = 0;
		connectMap[1] = 1;
		connectMap[2] = 3;
		connectMap[3] = 2;
	}

	// Importing the cells
	long m_nInternalCells;
	long m_nGhostsCells;
	if (is_three_dimensional()) {
		m_nInternalCells = m_tree_3D.getNumOctants();
		m_nGhostsCells   = m_tree_3D.getNumGhosts();
	} else {
		m_nInternalCells = m_tree_2D.getNumOctants();
		m_nGhostsCells   = m_tree_2D.getNumGhosts();
	}

	long nCells = m_nInternalCells + m_nGhostsCells;

	m_cell_to_octant.reserve(nCells);
	m_octant_to_cell.resize(nCells);
	m_cells.reserve(nCells);
	for (long n = 0; n < nCells; n++) {
		m_cells.emplace_back(n, this);
		Cell &cell = m_cells.back();

		// Distinguo tra celle interne e celle ghost
		bool isInternal = (n < m_nInternalCells);

		// Info associate al tree
		long octantId = n;
		long ghostId  = -1;
		if (!isInternal) {
			ghostId = octantId - m_nInternalCells;
		}

		m_cell_to_octant[n] = octantId;
		m_octant_to_cell[octantId] = n;

		int octantLevel;
		vector<double> octantCentroid;
		if (is_three_dimensional()) {
			Class_Octant<3> *octant;
			if (isInternal) {
				octant = m_tree_3D.getOctant(octantId) ;
			} else {
				octant = m_tree_3D.getGhostOctant(ghostId) ;
			}

			octantLevel    = m_tree_3D.getLevel(octant);
			octantCentroid = m_tree_3D.getCenter(octant);
		} else {
			Class_Octant<2> *octant;
			if (isInternal) {
				octant = m_tree_2D.getOctant(octantId) ;
			} else {
				octant = m_tree_2D.getGhostOctant(ghostId) ;
			}

			octantLevel    = m_tree_2D.getLevel(octant);
			octantCentroid = m_tree_2D.getCenter(octant);
		}

		// Tipo
		if (is_three_dimensional()) {
			cell.set_type(Element::BRICK);
		} else {
			cell.set_type(Element::RECTANGLE);
		}

		// Position
		if (isInternal) {
			cell.set_position_type(Cell::INTERNAL);
		} else {
			cell.set_position_type(Cell::GHOST);
		}

		// Volume
		cell.set_volume(&m_tree_volume[octantLevel]);

		// Centroide
		std::array<double, 3> centroid;
		for (unsigned int k = 0; k < centroid.size(); k++) {
			centroid[k] = octantCentroid[k];
		}

		cell.set_centroid(centroid);

		// Connettività
		vector<uint32_t> octantConnect;
		if (is_three_dimensional()) {
			octantConnect = m_tree_3D.getOctantConnectivity(n) ;
		} else {
			octantConnect = m_tree_2D.getOctantConnectivity(n) ;
		}

		std::unique_ptr<long[]> connect = std::unique_ptr<long[]>(new long[nCellVertices]);
		for (int k = 0; k < nCellVertices; k++) {
			connect[k] = octantConnect[connectMap[k]];
		}

		cell.set_connect(std::move(connect));
	}
}

/*!
	Updates the cells after a tree change.
*/
void PatchOctree::reload_cells()
{
	std::cout << ">> Updating the cells\n";

	reset_cells();
	import_cells();
}


/*!
	Updates the interfaces.
*/
void PatchOctree::update_interfaces()
{
	if (!is_dirty()) {
		return;
	}

	if (m_interfaces.empty()) {
		import_interfaces();
	} else {
		reload_interfaces();
	}
}

/*!
	Imports the interfaces from the tree.
*/
void PatchOctree::import_interfaces()
{
	std::cout << ">> Importing the interfaces\n";

	// Reset
	reset_interfaces();

	// Info of the cells
	Element::Type cellType;
	if (is_three_dimensional()) {
		cellType = Element::BRICK;
	} else {
		cellType = Element::RECTANGLE;
	}

	// Info on the interfaces
	int nInterfaceVertices;
	if (is_three_dimensional()) {
		nInterfaceVertices = Element::get_face_count(Element::RECTANGLE);
	} else {
		nInterfaceVertices = Element::get_face_count(Element::LINE);
	}

	// Initialize intersections
	if (is_three_dimensional()) {
		m_tree_3D.computeIntersections();
	} else {
		m_tree_2D.computeIntersections();
	}

	// Importing the interfaces
	long nInterfaces;
	if (is_three_dimensional()) {
		nInterfaces = m_tree_3D.getNumIntersections();
	} else {
		nInterfaces = m_tree_2D.getNumIntersections();
	}

	vector<vector<long>> cellToInterfaceMap(m_cells.size(), vector<long>(0));

	m_interfaces.reserve(nInterfaces);
	for (long n = 0; n < nInterfaces; n++) {
		m_interfaces.emplace_back(n, this);
		Interface &interface = m_interfaces.back();

		// Info associate al tree
		int level;
		bool isGhost;
		bool isBoundary;
		vector<uint32_t> cells;
		int ownerCell;
		int ownerFace;
		vector<double> faceCenter;
		if (is_three_dimensional()) {
			Class_Intersection<3> *treeInterface = m_tree_3D.getIntersection(n);

			level      = m_tree_3D.getLevel(treeInterface);
			isGhost    = m_tree_3D.getPbound(treeInterface);
			isBoundary = m_tree_3D.getBound(treeInterface);
			cells      = m_tree_3D.getOwners(treeInterface);
			ownerCell  = m_tree_3D.getFiner(treeInterface);
			ownerFace  = m_tree_3D.getFace(treeInterface);
			faceCenter = m_tree_3D.getCenter(treeInterface);
		} else {
			Class_Intersection<2> *treeInterface = m_tree_2D.getIntersection(n);

			level      = m_tree_2D.getLevel(treeInterface);
			isGhost    = m_tree_2D.getPbound(treeInterface);
			isBoundary = m_tree_2D.getBound(treeInterface);
			cells      = m_tree_2D.getOwners(treeInterface);
			ownerCell  = m_tree_2D.getFiner(treeInterface);
			ownerFace  = m_tree_2D.getFace(treeInterface);
			faceCenter = m_tree_2D.getCenter(treeInterface);
		}

		long ownerOctantId = cells[ownerCell];
		long ownerId = get_octant_id(ownerOctantId);

		// Tipo
		if (is_three_dimensional()) {
			interface.set_type(Element::RECTANGLE);
		} else {
			interface.set_type(Element::LINE);
		}

		// Area
		interface.set_area(&m_tree_area[level]);

		// Normal
		interface.set_normal(&m_normals[ownerFace]);

		// Centroid
		std::array<double, 3> centroid;
		for (unsigned int k = 0; k < centroid.size(); k++) {
			centroid[k] = faceCenter[k];
		}

		interface.set_centroid(centroid);

		// Position
		if (isGhost) {
			interface.set_position_type(Interface::GHOST);
		} else if (isBoundary) {
			interface.set_position_type(Interface::BOUNDARY);
		} else {
			interface.set_position_type(Interface::INTERNAL);
		}

		// Owner
		Cell &owner = m_cells[ownerId];
		cellToInterfaceMap[ownerId].push_back(interface.get_id());

		interface.set_owner(owner.get_id(), ownerFace);

		// Neighbour
		if (isBoundary) {
			interface.unset_neigh();
		} else {
			long neighOctantId = cells[ownerCell ? 0 : 1];
			if (isGhost) {
				neighOctantId += m_nInternalCells;
			}
			long neighId = get_octant_id(neighOctantId);

			Cell &neigh = m_cells[neighId];
			cellToInterfaceMap[neighId].push_back(interface.get_id());

			int neighFace = ownerFace + 1 - 2 * (ownerFace % 2);
			interface.set_neigh(neigh.get_id(), neighFace);
		}

		// Connectivity
		vector<uint32_t> octantConnect;
		if (is_three_dimensional()) {
			octantConnect = m_tree_3D.getOctantConnectivity(ownerId) ;
		} else {
			octantConnect = m_tree_2D.getOctantConnectivity(ownerId) ;
		}

		std::vector<int> localConnect = Element::get_face_local_connect(cellType, ownerFace);
		std::unique_ptr<long[]> connect = std::unique_ptr<long[]>(new long[nInterfaceVertices]);
		for (int i = 0; i < nInterfaceVertices; ++i) {
			connect[i] = octantConnect[localConnect[i]];
		}

		interface.set_connect(std::move(connect));
	}

	// Add interface information to the cells
	int nCellFaces;
	if (is_three_dimensional()) {
		nCellFaces = Element::get_face_count(Element::BRICK);
	} else {
		nCellFaces = Element::get_face_count(Element::RECTANGLE);
	}

	vector<vector<long>> interfaceList(nCellFaces, vector<long>(0));
	for (auto &cell : m_cells) {
		for (int k = 0; k < nCellFaces; k++) {
			interfaceList[k].clear();
		}

		int cellId = cell.get_id();
		for (unsigned long k = 0; k < cellToInterfaceMap[cellId].size(); k++) {
			long interfaceId = cellToInterfaceMap[cellId][k];
			Interface &interface = m_interfaces[interfaceId];

			int face;
			long ownerId = interface.get_owner();
			Cell &owner = m_cells[ownerId];
			if (owner.get_id() == cell.get_id()) {
				face = interface.get_owner_face();
			} else {
				face = interface.get_neigh_face();
			}

			interfaceList[face].push_back(interface.get_id());
		}

		cell.initialize_interfaces(interfaceList);

		cellToInterfaceMap[cellId].clear();
		vector<long>().swap(cellToInterfaceMap[cellId]);
	}

	// Clean intersections
	// if (is_three_dimensional()) {
	//	m_tree_3D.clearIntersections();
	//} else {
	//	m_tree_2D.clearIntersections();
	//}
}

/*!
	Updates the interfaces after a tree change.
*/
void PatchOctree::reload_interfaces()
{
	std::cout << ">> Updating the interfaces\n";

	reset_interfaces();
	import_interfaces();

}

/*!
	Marks a cell for refinement.

	\param id is the id of the cell that needs to be refined
*/
bool PatchOctree::_mark_cell_for_refinement(const long &id)
{
	if (is_three_dimensional()) {
		m_tree_3D.setMarker(id, 1);
	} else {
		m_tree_2D.setMarker(id, 1);
	}

	return true;
}

/*!
	Marks a cell for coarsening.

	\param id is the id of the cell that needs to be coarsened
*/
bool PatchOctree::_mark_cell_for_coarsening(const long &id)
{
	if (is_three_dimensional()) {
		m_tree_3D.setMarker(id, -1);
	} else {
		m_tree_2D.setMarker(id, -1);
	}

	return true;
}

/*!
	Enables cell balancing.

	\param id is the id of the cell
	\param enabled defines if enable the balancing for the specified cell
*/
bool PatchOctree::_enable_cell_balancing(const long &id, bool enabled)
{
	if (is_three_dimensional()) {
		m_tree_3D.setBalance(id, enabled);
	} else {
		m_tree_2D.setBalance(id, enabled);
	}

	return true;
}

}

#endif