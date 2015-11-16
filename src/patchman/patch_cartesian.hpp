//
// Written by Andrea Iob <andrea_iob@hotmail.com>
//
#ifndef __PATCHMAN_PATCH_CARTESIAN_HPP__
#define __PATCHMAN_PATCH_CARTESIAN_HPP__

/*! \file */

#include "patch.hpp"

#include <cstddef>
#include <memory>
#include <vector>

namespace pman {

class PatchCartesian : public Patch {

public:
	PatchCartesian(const int &id, const int &dimension, std::array<double, 3> origin,
			   double length, double dh);

	~PatchCartesian();

protected:
	std::array<double, 3> & _get_opposite_normal(std::array<double, 3> &normal);
	bool _update(std::vector<uint32_t> &cellMapping);
	bool _mark_cell_for_refinement(const long &id);
	bool _mark_cell_for_coarsening(const long &id);
	bool _enable_cell_balancing(const long &id, bool enabled);

private:
	static const int SPACE_MAX_DIM;

	std::vector<double> m_cellSize;
	std::vector<double> m_minCoord;

	std::vector<double> m_x;
	std::vector<double> m_y;
	std::vector<double> m_z;

	std::vector<int> m_nCells1D;
	std::vector<int> m_nVertices1D;
	std::vector<int> m_x_nInterfaces1D;
	std::vector<int> m_y_nInterfaces1D;
	std::vector<int> m_z_nInterfaces1D;

	double m_cell_volume;
	double m_x_interface_area;
	double m_y_interface_area;
	double m_z_interface_area;

	std::vector<std::array<double, 3> > m_normals;

	void create_vertices();

	void create_cells();

	void create_interfaces();
	int count_interfaces_direction(const Node::Coordinate &direction);
	void create_interfaces_direction(const Node::Coordinate &direction);

	long cell_ijk_to_id(const int &i, const int &j, const int &k) const;
	long cell_ijk_to_id(const int ijk[]) const;
	long vertex_ijk_to_id(const int &i, const int &j, const int &k) const;
	long vertex_ijk_to_id(const int ijk[]) const;
	long interface_nijk_to_id(const int &normal, const int &i, const int &j, const int &k) const;

};

}

#endif