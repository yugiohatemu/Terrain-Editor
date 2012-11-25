//Rain.hpp 
//Created by Yue Huang on Nov 24
 

#ifndef CS488_PARTICLE_HPP
#define CS488_PARTICLE_HPP

#include "algebra.hpp"
const int MAX_PARTICLE_COUNT = 100;

class Particle{
public: 
	Particle();
	~Particle();		
	draw();

private:
	Point3D particle_array[MAX_PARTICLE_COUNT];
	void make_Particle();
	void update_Particle();	
	//an array with rains
	//a update function
	//a generate/recycle function
	
}

#endif // CS488_Particle_HPP
