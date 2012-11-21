//hm - Height Map
//Created by Yue Huang on Nov 17
//Used for storing height map information and terrain generation
#ifndef CS488_HM_HPP
#define CS488_HM_HPP

#include "algebra.hpp"
//#include "texture.hpp"

#define OBJ_PLANE_GRID 2
#define OBJ_PLANE_QUAD 3
const int MAP_BOUND = 21;
//Should we let the user decide the size
//use a small map for now

class HM{

public:
	HM();
	~HM();
	void draw();
	void randomize();
	void water_test();
private:
	Point3D height_map[MAP_BOUND][MAP_BOUND];
	//Texture m_texture;
	Vector3D normal_map[(MAP_BOUND-1)][(MAP_BOUND-1)];
	
	int make_rand(int range);
	void draw_grid_list();
	void draw_quad_list();
	void make_circle();
	//void set_texture();
	

};

#endif // CS488_HM_HPP
