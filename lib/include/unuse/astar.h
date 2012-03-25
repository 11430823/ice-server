/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	A*Ëã·¨
	brief:		
*********************************************************************/

#pragma once

#if 0

#include <cstdlib>

#include <algorithm>
#include <fstream>
#include <vector>
#include <stdint.h>

/**
 * @brief Default Grid Data For A* algorithm
 */
class DefGridData {
public:
	DefGridData()
		{ m_data = 0; }

	/**
	 * @brief judge if a grid is walkable
	 */
	bool is_walkable() const
		{ return (m_data == 0); }

public:
	friend std::istream& operator >>(std::istream& is, DefGridData& obj)
		{ is >> obj.m_data; return is; }
	friend std::ostream& operator <<(std::ostream& os, const DefGridData& obj)
		{ os << obj.m_data; return os; }

private:
	int	m_data;
};

/**
 * @class Noncopyable
 * @brief private copy ctor and assignment operator ensure classes derived from it cannot be copied
 */
class Noncopyable {
protected:
	Noncopyable() { }
private:
	Noncopyable(const Noncopyable&);
	Noncopyable& operator=(const Noncopyable&);
};

/**
 * @brief A* algorithm for path finding
 */
template <typename GridData = DefGridData>
class AStar : Noncopyable {
private:
	/**
	  * @brief constants
	  */
	enum astar_constant_t {
		astar_grid_dist_strl	= 10,
		astar_grid_dist_oblique	= 14,
	};

public:
	/**
	  * @brief Point
	  */
	class Point {
	public:
		/**
		  * @brief default constructor
		  */
	Point(uint32_t xpos = 0, uint32_t ypos = 0)
			{ x = xpos; y = ypos; }

	public:
		/*! coordinate x */
		uint32_t x;
		/*! coordinate y */
		uint32_t y;
	};

	/**
	  * @brief Points
	  */
	typedef std::vector<Point> Points;

public:
	/**
	  * @brief constructor
	  */
	AStar(const char* filename);
	/**
	  * @brief destructor
	  */
	~AStar();

	/**  
	 * @brief find a path
	 * @param src start point
	 * @param dst end point
	 * @return points that form the path in reverse, or 0 if no path
	 */
	const Points* findpath(const Point& src, const Point& dst);
	/**  
	 * @brief test if one point can walkable
	 * @param curpos current position to test
	 * @return true is walkable, or false
	 */
	bool is_pos_walkable(const Point& cur_pos);

	/**
	  * @brief conversion operators, for judging whether the AStar obj is fine to use
	  */
	operator void*() const
		{ return reinterpret_cast<void*>(!m_err); }

private:
	/**
	  * @brief error number for AStar
	  */
	enum astar_error_t {
		astar_err_ok			= 0,
		astar_err_open_file		= 1,
		astar_err_read_file		= 2,
	};

	/**
	  * @brief Index to a Grid
	  */
	class GridIndex {
	public:
		/**
		  * @brief Default Constructor
		  */
		GridIndex(uint32_t r = 0, uint32_t c = 0)
			{ row = r; column = c; }

	public:
		friend bool operator ==(const GridIndex& i1, const GridIndex& i2)
			{ return (i1.row == i2.row) && (i1.column == i2.column); }

		friend bool operator !=(const GridIndex& i1, const GridIndex& i2)
			{ return !(i1 == i2); }

		friend GridIndex operator -(const GridIndex& lhs, const GridIndex& rhs)
			{ GridIndex ret(lhs); ret.row -= rhs.row; ret.column -= rhs.column; return ret; }

	public:
		/*! row */
		uint32_t	row;
		/*! column */
		uint32_t	column;
	};

	/**
	 * @brief A* algorithm for path finding
	 */
	class Grid {
	public:
		Grid()
			{ parent = 0; child = 0; f = 0; g = 0; }

	public:
		friend bool operator ==(const Grid& g1, const Grid& g2)
			{ return (g1.idx == g2.idx); }

	public:
		/*! used during the search to record the parent of successor nodes */
		Grid*	parent;
		/*!
		  *  1. used to link all closed node together
		  *  2. (not used now) used after the search for the application to view the search in reverse (not used now)
		  */
		Grid*	child;

		/*! sum of cumulative cost of predecessors and self and heuristic */
		uint32_t	f;
		/*! cost of this node + it's predecessors */
		uint32_t	g;

		/*! index to this grid */
		GridIndex	idx;
		/*! Real Data in a Grid */
		GridData	grid_data;
	};

	/**
	  * @brief pointer to Grid
	  */
	typedef Grid* GridPointer;

private:
	void try_add_successor(const GridIndex& idx, Grid* parent, uint32_t cost = astar_grid_dist_strl);
	void form_path(Grid* goal, const Point& dst);

	GridPointer get_grid(const GridIndex& idx);

	void init_closed_list()
		{ m_closed_list.parent = &m_closed_list; m_closed_list.child = 0; }
	void add_to_closed_list(Grid* g)
		{ m_closed_list.parent->child = g; m_closed_list.parent = g; g->child = 0; }

	uint32_t distance(const GridIndex& start, const GridIndex& end) const
		{ return (abs(end.row - start.row) + abs(end.column - start.column)) * astar_grid_dist_strl; }

	GridIndex point_to_grid_idx(const Point& pt) const
		{ return GridIndex(pt.y / m_ppg_y, pt.x / m_ppg_x); }
	Point grid_idx_to_point(const GridIndex& idx) const
		{ return Point(idx.column * m_ppg_x, idx.row * m_ppg_y); }

	bool out_of_range(const GridIndex& idx) const
		{ return (idx.row >= m_max_grids_row) || (idx.column >= m_max_grids_column); }

private:
	/**
	  * @brief For sorting the heap the STL needs compare function that lets us compare the f value of two nodes
	  */
	static bool grid_cmp(const Grid* g1, const Grid* g2)
		{ return (g1->f > g2->f); }

private:
	/*! max grids of a row */
	uint32_t	m_max_grids_row;
	/*! max grids of a column */
	uint32_t	m_max_grids_column;
	/*! pixels per grid for coordinate x */
	uint32_t	m_ppg_x;
	/*! pixels per grid for coordinate y */
	uint32_t	m_ppg_y;
	/*! points that form a path */
	Points		m_points;

	/*! map load from file */
	GridPointer*	m_map;

	/*! pointer to the current position that is being searched */
	Grid*						m_cur_grid;
	/*! pointer to the target position */
	Grid*						m_goal_grid;
	/*! open list, simple vector but used as a heap */
	std::vector<GridPointer>	m_open_list;
	/*! closed list, simple Grid but used as a head to link all closed node together */
	Grid						m_closed_list;

	/*! error number */
	int	m_err;
};

template <typename GridData>
inline
AStar<GridData>::AStar(const char* filename)
{
	m_err = astar_err_ok;
	m_map = 0;

	init_closed_list();

	m_open_list.reserve(50);
	m_points.reserve(50);

	// open file
	std::ifstream fin(filename);
	if (!fin) {
		m_err = astar_err_open_file;
		return;
	}

	// read head info from map data file
	fin >> m_max_grids_column >> m_max_grids_row >> m_ppg_x >> m_ppg_y;
	if (!fin) {
		m_err = astar_err_read_file;
		return;
	}

	// alloc memory to hold the given map
	m_map    = new GridPointer[m_max_grids_row];
	m_map[0] = new Grid[m_max_grids_row * m_max_grids_column];
	for (uint32_t i = 1; i != m_max_grids_row; ++i) {
		m_map[i] = m_map[0] + i * m_max_grids_column;
	}

	// load map data from the given file
	for (uint32_t i = 0; i != m_max_grids_row; ++i) {
		for (uint32_t j = 0; j != m_max_grids_column; ++j) {
			fin >> m_map[i][j].grid_data;
			m_map[i][j].idx.row    = i;
			m_map[i][j].idx.column = j;
		}
	}

	if (!fin) {
		m_err = astar_err_read_file;
		return;
	}
}

template <typename GridData>
inline
AStar<GridData>::~AStar()
{
	if (m_map) {
		delete [] m_map[0];
		delete [] m_map;
	}
}

template <typename GridData>
inline const typename AStar<GridData>::Points* 
AStar<GridData>::findpath(const Point& src, const Point& dst)
{
	m_points.clear();
	m_cur_grid = 0;

	GridIndex start_idx = point_to_grid_idx(src);
	GridIndex end_idx   = point_to_grid_idx(dst);

	if (start_idx == end_idx) {
		m_points.push_back(dst);		
		return &m_points;
	}

	Grid* start_grid = get_grid(start_idx);
	m_goal_grid      = get_grid(end_idx);
	if (!start_grid || !m_goal_grid || !m_goal_grid->grid_data.is_walkable()) {
		return 0;
	}

	m_goal_grid->parent = 0;

	start_grid->parent = 0;
	start_grid->g      = 0;
	start_grid->f      = distance(start_idx, end_idx);
	// heap now unsorted
	m_open_list.push_back(start_grid);
	// sort back element into heap
	std::push_heap(m_open_list.begin(), m_open_list.end(), grid_cmp);

	do {
		// Pop the best node (the one with the lowest f) 
		m_cur_grid = m_open_list.front(); // get pointer to the node
		std::pop_heap(m_open_list.begin(), m_open_list.end(), grid_cmp);
		m_open_list.pop_back();

		// add current node to the closed list
		add_to_closed_list(m_cur_grid);

		// path found
		if (m_cur_grid == m_goal_grid) {
			break;
		}

		// try each adjacent node
		const GridIndex& idx = m_cur_grid->idx;
		try_add_successor(GridIndex(idx.row, idx.column - 1), m_cur_grid);
		try_add_successor(GridIndex(idx.row, idx.column + 1), m_cur_grid);
		try_add_successor(GridIndex(idx.row - 1, idx.column), m_cur_grid);
		try_add_successor(GridIndex(idx.row + 1, idx.column), m_cur_grid);
		try_add_successor(GridIndex(idx.row + 1, idx.column - 1), m_cur_grid, astar_grid_dist_oblique);
		try_add_successor(GridIndex(idx.row - 1, idx.column - 1), m_cur_grid, astar_grid_dist_oblique);
		try_add_successor(GridIndex(idx.row + 1, idx.column + 1), m_cur_grid, astar_grid_dist_oblique);
		try_add_successor(GridIndex(idx.row - 1, idx.column + 1), m_cur_grid, astar_grid_dist_oblique);
	} while (m_open_list.size());

	// clear f
	Grid* node = m_closed_list.child;
	while (node) {
		node->f = 0;
		node    = node->child;
	}
	for (typename std::vector<GridPointer>::iterator it = m_open_list.begin(); it != m_open_list.end(); ++it) {
		(*it)->f = 0;
	}

	m_open_list.clear();
	init_closed_list();

	// path found
// 	if (m_goal_grid->parent) {
// 		form_path(m_goal_grid, dst);
// 		return &m_points;
// 	}
	Grid* pgrid = m_goal_grid->parent;
	if (NULL != pgrid){	
		while(pgrid){
			m_points.push_back(Point(pgrid->idx.row, pgrid->idx.column));
			pgrid = pgrid->parent;
		}
		return &m_points;
	}


	return 0;
}


template <typename GridData>
inline bool
AStar<GridData>::is_pos_walkable(const Point& cur_pos)
{
	GridIndex pos_idx = point_to_grid_idx(cur_pos);

	Grid* pos_grid = get_grid(pos_idx);
	if (!pos_grid || !pos_grid->grid_data.is_walkable()) {
		return false;
	}
	
	return true;
}

//------------------------------------------------------------------------
// Private Methods
//
template <typename GridData>
inline void
AStar<GridData>::try_add_successor(const GridIndex& idx, Grid* parent, uint32_t cost)
{
	Grid* grid = get_grid(idx);
	if (!grid || grid->f || !grid->grid_data.is_walkable()) {
		return;
	}

	grid->g      = parent->g + cost;
	grid->f      = grid->g + distance(idx, m_goal_grid->idx);
	grid->parent = parent;

	// heap now unsorted
	m_open_list.push_back(grid);
	// sort back element into heap
	std::push_heap(m_open_list.begin(), m_open_list.end(), grid_cmp);
}

template <typename GridData>
inline void
AStar<GridData>::form_path(Grid* goal, const Point& dst)
{
	Grid* cur	= goal;
	Grid* prev1 = cur->parent;
	Grid* prev2 = prev1->parent;
	Grid* prev_inused_grid = cur;
	prev_inused_grid->parent = 0;
	while (prev2) {
		if (((cur->idx - prev1->idx) != (prev1->idx - prev2->idx)) && (cur != prev_inused_grid)) {
			prev_inused_grid->parent = prev1;
			prev_inused_grid = prev1;
		}
		cur   = prev1;
		prev1 = prev2;
		prev2 = prev1->parent;
	}
	prev_inused_grid->parent = prev1;

	cur   = goal;
	prev1 = cur->parent;
	prev2 = prev1->parent;
	m_points.push_back(dst);
	while (prev2) {
		if ((cur->idx - prev1->idx) != (prev1->idx - prev2->idx)) {
			m_points.push_back(grid_idx_to_point(prev1->idx));
		}
		cur   = prev1;
		prev1 = prev2;
		prev2 = prev1->parent;
	}
}

template <typename GridData>
inline typename AStar<GridData>::GridPointer
AStar<GridData>::get_grid(const GridIndex& idx)
{
	if (!out_of_range(idx)) {
		return &(m_map[idx.row][idx.column]);
	}

	return 0;
}

#endif