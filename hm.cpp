#include <iostream>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "hm.hpp"
#include <stdio.h>
#include <time.h>

const double MAX_DISPLACEMENT = 6.0;

HM::HM(){
	
	int offset =  (MAP_BOUND/2);
	int low_bound= -offset;
	int up_bound = 1 + offset;
	
	for(int i = low_bound;i<up_bound;i+=1){
		for(int j = low_bound;j<up_bound;j+=1){
			Point3D p(i,j,0);
			height_map[i+offset][j+offset] = p;
		}
	}

	//Initializing standard normal
	
	for(int i =0;i<MAP_BOUND-1;i+=1){ 
		for(int j = 0;j<MAP_BOUND-1;j+=1){
			normal_map[i][j][2] = 1;
		}
	}
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
	//Update in a loop manner
	//get current time , out put in cos
	time_t second = time (NULL);
	double delta = cos((second%360)*M_PI);

	for(int i =0;i<MAP_BOUND;i+=1){ 
		for(int j = 0;j<MAP_BOUND;j+=1){
			//TODO: when implant wave foam, we can add x and y in it
			//For now only care about z
			height_map[i][j][2] =((i%3)+(j%3))*delta ;
		}
	}
	
	//Update normal
	for(int i =0;i<MAP_BOUND-1;i+=1){ 
		for(int j = 0;j<MAP_BOUND-1;j+=1){
			Vector3D x = height_map[i+1][j]-height_map[i][j] ;
			Vector3D y = height_map[i+1][j+1] - height_map[i+1][j];
			Vector3D n = x.cross(y);
			n.normalize();
			normal_map[i][j] = n;
			
		}
	}

}



//Generate a random map
void HM::randomize(){
	//TODO:disp should decrease
	/*
	for(int i = 0;i<50;i+=1){
		make_circle();
	}*/
	make_circle(50);
	//Calculate the normal
	for(int i =0;i<MAP_BOUND-1;i+=1){ 
		for(int j = 0;j<MAP_BOUND-1;j+=1){
			Vector3D x =  height_map[i+1][j]-height_map[i][j] ;
			Vector3D y =  height_map[i+1][j+1] - height_map[i+1][j];
			Vector3D n = x.cross(y);
			n.normalize();
			normal_map[i][j] = n;
			
			//TODO:we may also take consideration of the normal on the other plane	
		}
	}
}

//Draw the map in planar mode
void HM::draw_quad_list(){
	//glNewList(OBJ_PLANE_QUAD, GL_COMPILE);
	    
	glBegin(GL_QUADS); 
	
	for(int i =0;i<MAP_BOUND-1;i+=1){ 
		for(int j = 0;j<MAP_BOUND-1;j+=1){
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
	for(int i =0;i<MAP_BOUND;i+=1){ 
		for(int j = 0;j<MAP_BOUND-1;j+=1){
			glVertex3d(height_map[i][j][0],height_map[i][j][1],height_map[i][j][2]);
			glVertex3d(height_map[i][j+1][0],height_map[i][j+1][1],height_map[i][j+1][2]);
		}
	}

	//horizontal line
	for(int i =0;i<MAP_BOUND-1;i+=1){ 
		for(int j = 0;j<MAP_BOUND;j+=1){
			glVertex3d(height_map[i][j][0],height_map[i][j][1],height_map[i][j][2]);
			glVertex3d(height_map[i+1][j][0],height_map[i+1][j][1],height_map[i+1][j][2]);
		}
	}

	//diagonal line
	for(int i =0;i<MAP_BOUND-1;i+=1){ 
		for(int j = 0;j<MAP_BOUND-1;j+=1){
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
		Point3D center = height_map[make_rand(MAP_BOUND)][ make_rand(MAP_BOUND)];
	
		//Random radius
		double radius =make_rand(MAP_BOUND/3)+1;
		double disp = get_displacement(i);

		for(int x =0;x< MAP_BOUND ;x+=1){ 
			for(int y = 0;y< MAP_BOUND ;y+=1){
				double dis = (height_map[x][y] - center).length();
				double pd = dis*2/radius; 
		
				if(fabs(pd)<= 1.0){
					height_map[x][y][2] += disp/2+cos(pd* M_PI)*disp/2;
				}
			}
		}
	}
	smoothing();
}

double HM::get_displacement(int round){
	double disp = MAX_DISPLACEMENT- round * 0.1;
	if(disp< 0.1){
		disp = 0.1;
	}
	return disp;
}

void HM::smoothing(){
	double k = 0.5;
	//Left to right
	for(int i = 1;i<MAP_BOUND;i+=1){
		for(int j = 0;j<MAP_BOUND;j+=1){
			height_map[i][j][2] = height_map[i+1][j][2]*(1-k) + height_map[i][j][2]*k;
		}
	}
	//right to left
	for(int i = MAP_BOUND-1;i>1;i-=1){
		for(int j = 0;j<MAP_BOUND;j+=1){
			height_map[i][j][2] = height_map[i-1][j][2]*(1-k) + height_map[i][j][2]*k;
		}
	}
	//bottom to top
	for(int i = 0;i<MAP_BOUND;i+=1){
		for(int j = 1;j<MAP_BOUND;j+=1){
			height_map[i][j][2] = height_map[i][j+1][2]*(1-k) + height_map[i][j][2]*k;
		}
	}
	//top to bottom
	for(int i = 0;i<MAP_BOUND;i+=1){
		for(int j = MAP_BOUND-1;j>1; j-=1){
			height_map[i][j][2] = height_map[i][j-1][2]*(1-k) + height_map[i][j][2]*k;
		}
	}
}

/*
	*//*for (j = 0; j < RESOLUTION; j++){
		for (i = 0; i <= RESOLUTION; i++){
			indice = 6 * (i + j * (RESOLUTION + 1));

			v1x = surface[indice + 3];
			v1y = surface[indice + 4];
			v1z = surface[indice + 5];

			v2x = v1x;
			v2y = surface[indice + 1];
			v2z = surface[indice + 2];

			if (i < RESOLUTION){
			v3x = surface[indice + 9];
			v3y = surface[indice + 10];
			v3z = v1z;
			}else{
				v3x = xn;
				v3y = z (xn, v1z, t);
				v3z = v1z;
			}

			vax =  v2x - v1x;
			vay =  v2y - v1y;
			vaz =  v2z - v1z;

			vbx = v3x - v1x;
			vby = v3y - v1y;
			vbz = v3z - v1z;

			nx = (vby * vaz) - (vbz * vay);
			ny = (vbz * vax) - (vbx * vaz);
			nz = (vbx * vay) - (vby * vax);

			l = sqrtf (nx * nx + ny * ny + nz * nz);
			if (l != 0){
				l = 1 / l;
				normal[indice + 3] = nx * l;
				normal[indice + 4] = ny * l;
				normal[indice + 5] = nz * l;
			}else{
				normal[indice + 3] = 0;
				normal[indice + 4] = 1;
				normal[indice + 5] = 0;
			}


			if (j != 0){
			// Values were computed during the previous loop 
				preindice = 6 * (i + (j - 1) * (RESOLUTION + 1));
				normal[indice] = normal[preindice + 3];
				normal[indice + 1] = normal[preindice + 4];
				normal[indice + 2] = normal[preindice + 5];
			}
			else{
				// 	    v1x = v1x; 
				v1y = z (v1x, (j - 1) * delta - 1, t);
				v1z = (j - 1) * delta - 1;

				// 	    v3x = v3x; 
				v3y = z (v3x, v2z, t);
				v3z = v2z;

				vax = v1x - v2x;
				vay = v1y - v2y;
				vaz = v1z - v2z;

				vbx = v3x - v2x;
				vby = v3y - v2y;
				vbz = v3z - v2z;

				nx = (vby * vaz) - (vbz * vay);
				ny = (vbz * vax) - (vbx * vaz);
				nz = (vbx * vay) - (vby * vax);

				l = sqrtf (nx * nx + ny * ny + nz * nz);
				if (l != 0){
					l = 1 / l;
					normal[indice] = nx * l;
					normal[indice + 1] = ny * l;
					normal[indice + 2] = nz * l;
				}else{
					normal[indice] = 0;
					normal[indice + 1] = 1;
					normal[indice + 2] = 0;
				}
			}
		}
	}*/
