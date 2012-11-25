#include "particle.hpp"
#include <math.h>
//particle_array[MAX_PARTICLE_COUNT];


Particle::Particle(){
	Point3D p(0,0,10);//This depends on the max height of the mountain?
	for(int i = 0;i<MAX_PARTICLE_COUNT;i+=1){
		particle_array[i] = p;
	}
}
	

Particle::~Particle(){

}		

void Particle::make_particle(){
	
}
	

void Particle::update_particle(){
		
	for(int i = 0;i<MAX_PARTICLE_COUNT;i+=1){
		particle_array[i][2] -= height;
		if(particle_array[i][2]<0){
			//Recycle the particle
			int x = rand();
			int y = rand();
			Point3d p(x,y,10);
			particle_array[i] = p;
		}
	}
}

void Particle::draw(){
	//Only draw the visible particle
	update_particle();	
	glBegin(GL_LINES)
	for(int i = 0;i<MAX_PARTICLE_COUNT;i+=1){
		if(valid rain level){
			glVertex3d();
			glVertex3d();
		}
	}
	glEnd();
}
		
