#include <math.h>
#include <time.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "particle.hpp"

const double HEIGHT_LIMIT = 10.0;
const double VALID_HEIGHT = 8.0;
const double PARTICLE_HEIGHT = 0.3;
const double SNOW_HEIGHT = 0.1;
const double SIZE_LIMIT = 20;
const double SPEED = 0.3;

Particle::Particle(){
	
	
}
	

Particle::~Particle(){

}		

void Particle::make_particle(){
	
	for(int i = 0;i<MAX_PARTICLE_COUNT;i+=1){
		double x = make_rand(SIZE_LIMIT);
		double y = make_rand(SIZE_LIMIT);
		double z = make_rand(HEIGHT_LIMIT);
		
		Point3D p(x,y,z);
		particle_array[i] = p;
	}
}

double Particle::make_rand(double range){
	//Use srand for real random
	//srand(time(NULL));
	//generate a before decimal part and an after decimal part
	double r= ((double)rand()/(double)RAND_MAX)+(double)(std::rand()%(int)range);
	return r;
}	

void Particle::update_particle(){
		
	for(int i = 0;i<MAX_PARTICLE_COUNT;i+=1){
		particle_array[i][2] -= SPEED;
		if(particle_array[i][2]<0){
			//Recycle the particle
			particle_array[i][2] = HEIGHT_LIMIT;
		}
	}
}

void Particle::draw_rain(){
	//Only draw the visible particle rain if we draw snow , then use glut?
	update_particle();	
	glBegin(GL_LINES);
	for(int i = 0;i<MAX_PARTICLE_COUNT;i+=1){
		
		glVertex3d(particle_array[i][0],particle_array[i][1],particle_array[i][2]);
		glVertex3d(particle_array[i][0],particle_array[i][1],particle_array[i][2]-PARTICLE_HEIGHT);
		
	}
	glEnd();
}

void Particle::draw_snow(){
	//Only draw the visible particle rain if we draw snow , then use glut?
	update_particle();	
	glBegin(GL_QUADS);
	for(int i = 0;i<MAX_PARTICLE_COUNT;i+=1){
		glNormal3d(0,1,1);
		glVertex3d(particle_array[i][0],particle_array[i][1],particle_array[i][2]+SNOW_HEIGHT);
		glVertex3d(particle_array[i][0]+SNOW_HEIGHT,particle_array[i][1],particle_array[i][2]);
		glVertex3d(particle_array[i][0],particle_array[i][1],particle_array[i][2]-SNOW_HEIGHT);
		glVertex3d(particle_array[i][0]-SNOW_HEIGHT,particle_array[i][1],particle_array[i][2]);
		
	}
	glEnd();
}
		
