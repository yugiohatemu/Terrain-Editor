//hm - Height Map
//Created by Yue Huang on Nov 17
//Used for storing height map information and terrain generation
#ifndef CS488_HM_HPP
#define CS488_HM_HPP

#include "algebra.hpp"

#define OBJ_PLANE_GRID 2
#define OBJ_PLANE_QUAD 3
const int MAP_BOUND = 251;

//Should we let the user decide the size
//use a small map for now

class HM{

public:
	HM();
	~HM();
	void draw();
	void randomize();
	void water_test();
	double get_height(int x,int y);
	bool is_above_water(int water_level,int i, int j);
	void draw_above_water();
	void draw_below_water();
	void reset();
	void set_size(int size);
	void set_water_speed(int speed);

private:
	Point3D height_map[MAP_BOUND][MAP_BOUND];
	Vector3D normal_map[(MAP_BOUND-1)][(MAP_BOUND-1)];
	double displacement;
	int map_size;
	int water_speed;
	int make_rand(int range);
	void draw_grid_list();
	void draw_quad_list();
	void make_circle(int round);
	double get_displacement(int round);
	void smoothing();
};

#endif // CS488_HM_HPP
