#include <iostream>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "hm.hpp"
#include <time.h>

const double MAX_DISPLACEMENT = 15.0;
static int t = 0;
HM::HM(){
	map_size  = 251;
	water_speed = 5;
	reset();
	
}


HM::~HM(){
	//Do nothing for now
}

//Draw height map
void HM::draw(){
	draw_quad_list();
	//glCallList(OBJ_PLANE_GRID);
	//glCallList(OBJ_PLANE_QUAD);
}

void HM::water_test(){
	t=(t+1)%180;
	for(int i =0;i<map_size;i+=1){ 
		for(int j = 0;j<map_size;j+=1){
			 height_map[i][j][2] = cos(((i+j)*5+t*water_speed) * M_PI/180);
		}
	}

	for(int i =0;i<map_size-1;i+=1){ 
		for(int j = 0;j<map_size-1;j+=1){
			Vector3D x = height_map[i+1][j]-height_map[i][j] ;
			Vector3D y = height_map[i+1][j+1] - height_map[i+1][j];
			Vector3D n = x.cross(y);
			n.normalize();
			normal_map[i][j] = n;
	
		}
	}

}

bool HM::is_above_water(int water_level,int i, int j){
	return (height_map[i][j][2]>water_level&&height_map[i+1][j][2]>=water_level&&height_map[i][j+1][2]>=water_level&&height_map[i+1][j+1][2]>=water_level);

}


void HM::draw_above_water(){
	glBegin(GL_QUADS); 
	
	for(int i =0;i<map_size-1;i+=1){ 
		for(int j = 0;j<map_size-1;j+=1){
			if(is_above_water(2,i,j)){
				Vector3D n = normal_map[i][j];
				glNormal3d(n[0],n[1],n[2]);
				glTexCoord2d(0, 0);glVertex3d(height_map[i][j][0],height_map[i][j][1],height_map[i][j][2]);
				glTexCoord2d(1, 0);glVertex3d(height_map[i+1][j][0],height_map[i+1][j][1],height_map[i+1][j][2]);
				glTexCoord2d(1, 1);glVertex3d(height_map[i+1][j+1][0],height_map[i+1][j+1][1],height_map[i+1][j+1][2]);
				glTexCoord2d(0, 1);glVertex3d(height_map[i][j+1][0],height_map[i][j+1][1],height_map[i][j+1][2]);
			}
			
		}
	}

	glEnd();	
}

void HM::draw_below_water(){
	glBegin(GL_QUADS); 
	
	for(int i =0;i<map_size-1;i+=1){ 
		for(int j = 0;j<map_size-1;j+=1){
			if(!is_above_water(2,i,j)){
				Vector3D n = normal_map[i][j];
				glNormal3d(n[0],n[1],n[2]);
				glTexCoord2d(0, 0);glVertex3d(height_map[i][j][0],height_map[i][j][1],height_map[i][j][2]);
				glTexCoord2d(1, 0);glVertex3d(height_map[i+1][j][0],height_map[i+1][j][1],height_map[i+1][j][2]);
				glTexCoord2d(1, 1);glVertex3d(height_map[i+1][j+1][0],height_map[i+1][j+1][1],height_map[i+1][j+1][2]);
				glTexCoord2d(0, 1);glVertex3d(height_map[i][j+1][0],height_map[i][j+1][1],height_map[i][j+1][2]);
			}
		}
	}

	glEnd();	
}


//Generate a random map
void HM::randomize(){
	make_circle(50);
	//Calculate the normal
	for(int i =0;i<map_size-1;i+=1){ 
		for(int j = 0;j<map_size-1;j+=1){
			Vector3D x =  height_map[i+1][j]-height_map[i][j] ;
			Vector3D y =  height_map[i+1][j+1] - height_map[i+1][j];
			Vector3D n = x.cross(y);
			n.normalize();
			normal_map[i][j] = n;
		}
	}
}

//Draw the map in planar mode
void HM::draw_quad_list(){
	//glNewList(OBJ_PLANE_QUAD, GL_COMPILE);
	    
	glBegin(GL_QUADS); 
	
	for(int i =0;i<map_size-1;i+=1){ 
		for(int j = 0;j<map_size-1;j+=1){
			//Normal for lighting

			Vector3D n = normal_map[i][j];
			glNormal3d(n[0],n[1],n[2]);
		
			glTexCoord2d(0, 0);glVertex3d(height_map[i][j][0],height_map[i][j][1],height_map[i][j][2]);
			glTexCoord2d(1, 0);glVertex3d(height_map[i+1][j][0],height_map[i+1][j][1],height_map[i+1][j][2]);
			glTexCoord2d(1, 1);glVertex3d(height_map[i+1][j+1][0],height_map[i+1][j+1][1],height_map[i+1][j+1][2]);
			glTexCoord2d(0, 1);glVertex3d(height_map[i][j+1][0],height_map[i][j+1][1],height_map[i][j+1][2]);
			
		}
	}

	glEnd();	
   // glEndList();
}

//Private functions

//Draw the plane in wire frame mode
void HM::draw_grid_list(){
	
	//glNewList(OBJ_PLANE_GRID, GL_COMPILE);
	glBegin(GL_LINES);
	
	//vertical line
	for(int i =0;i<map_size;i+=1){ 
		for(int j = 0;j<map_size-1;j+=1){
			glVertex3d(height_map[i][j][0],height_map[i][j][1],height_map[i][j][2]);
			glVertex3d(height_map[i][j+1][0],height_map[i][j+1][1],height_map[i][j+1][2]);
		}
	}

	//horizontal line
	for(int i =0;i<map_size-1;i+=1){ 
		for(int j = 0;j<map_size;j+=1){
			glVertex3d(height_map[i][j][0],height_map[i][j][1],height_map[i][j][2]);
			glVertex3d(height_map[i+1][j][0],height_map[i+1][j][1],height_map[i+1][j][2]);
		}
	}

	//diagonal line
	for(int i =0;i<map_size-1;i+=1){ 
		for(int j = 0;j<map_size-1;j+=1){
			glVertex3d(height_map[i][j][0],height_map[i][j][1],height_map[i][j][2]);
			glVertex3d(height_map[i+1][j+1][0],height_map[i+1][j+1][1],height_map[i+1][j+1][2]);
		}
	}

	glEnd();	
}

//Return a random in from 0 to (range-1)
int HM::make_rand(int range){
	return (std::rand()%range);
}

//Use circle algorithm for genertaion
void HM::make_circle(int round){
	//Choose a random point from grid to be the circle center
	for(int i = round;i>0;i-=1){	
		Point3D center = height_map[make_rand(map_size)][ make_rand(map_size)];
	
		//Random radius
		double radius =make_rand(map_size/3)+1;
		double disp = get_displacement(i);

		for(int x =0;x< map_size ;x+=1){ 
			for(int y = 0;y< map_size ;y+=1){
				double dis = (height_map[x][y] - center).length();
				double pd = dis*2/radius; 
		
				if(fabs(pd)<= 1.0){
					height_map[x][y][2] += disp/2+cos(pd* M_PI)*disp/2;
				}
			}
		}
		smoothing();
	}
	
}

double HM::get_displacement(int round){
	double disp = displacement- round * 0.1;
	if(disp< 0.1){
		disp = 0.1;
	}
	return disp;
}

void HM::smoothing(){
	double k = 0.5;
	//Left to right
	for(int i = 1;i<map_size;i+=1){
		for(int j = 0;j<map_size;j+=1){
			height_map[i][j][2] = height_map[i+1][j][2]*(1-k) + height_map[i][j][2]*k;
		}
	}
	//right to left
	for(int i = map_size-1;i>1;i-=1){
		for(int j = 0;j<map_size;j+=1){
			height_map[i][j][2] = height_map[i-1][j][2]*(1-k) + height_map[i][j][2]*k;
		}
	}
	//bottom to top
	for(int i = 0;i<map_size;i+=1){
		for(int j = 1;j<map_size;j+=1){
			height_map[i][j][2] = height_map[i][j+1][2]*(1-k) + height_map[i][j][2]*k;
		}
	}
	//top to bottom
	for(int i = 0;i<map_size;i+=1){
		for(int j = map_size-1;j>1; j-=1){
			height_map[i][j][2] = height_map[i][j-1][2]*(1-k) + height_map[i][j][2]*k;
		}
	}
}

double HM::get_height(int x,int y){
	x = x+map_size/2 ;
	y = y+40;
	if(x<0||y<0||x>=map_size||y>=map_size){
		return 0;
	}
	//std::cerr<<x<<" "<<y<<" "<<height_map[x][y][2]<<std::endl;
	if(height_map[x][y][2]< 2){//water level
		return 2;
	}
	return height_map[x][y][2];
}

void HM::reset(){
	int offset =  (MAP_BOUND/2);
	int low_bound= -offset;
	int up_bound = 1 + offset;
	
	for(int i = low_bound;i<up_bound;i+=1){
		for(int j = low_bound;j<up_bound;j+=1){
			Point3D p(i,j,0);
			height_map[i+offset][j+offset] = p;
		}
	}
	
	for(int i =0;i<MAP_BOUND-1;i+=1){ 
		for(int j = 0;j<MAP_BOUND-1;j+=1){
			normal_map[i][j][2] = 1;
		}
	}
	
	displacement = make_rand(MAX_DISPLACEMENT)+1;
	
}

void HM::set_size(int size){
	map_size = size;
}

void HM::set_water_speed(int speed){
	water_speed = speed;
}
/*int offset =  (map_size/2);
	int low_bound= -offset;
	int up_bound = 1 + offset;
	
	for(int i = low_bound;i<up_bound;i+=1){
		for(int j = low_bound;j<up_bound;j+=1){
			Point3D p(i,j,0);
			height_map[i+offset][j+offset] = p;
		}
	}

	//Initializing standard normal
	
	for(int i =0;i<map_size-1;i+=1){ 
		for(int j = 0;j<map_size-1;j+=1){
			normal_map[i][j][2] = 1;
		}
	}*/
