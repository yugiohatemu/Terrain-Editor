#include <iostream>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "hm.hpp"
#include <stdio.h>



GLuint texture[1];

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
	
	
}

void HM::set_texture(){
	/*glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	//glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);  
	glGenTextures(1, &texture[0]);
	//std::cerr<<"Texture name = "<<texture[0]<<std::endl;
	glBindTexture(GL_TEXTURE_2D, texture[0]); 

	int width =64; 
	int height =64;
	//sizeof(unsigned char) 
	unsigned char * data = (unsigned char *)malloc( width * height * 3 );
	FILE * file = fopen( "terrain.png", "rb" );
	fread( data, width * height * 3, 1, file );
	fclose( file );

	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,width,height, 0,GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); //For testing ppurpose
	// build our texture MIP maps
	//gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB, width,height, GL_RGB, GL_UNSIGNED_BYTE, data );

	//Free the malloc
	free(data);*/
}


HM::~HM(){
	//Do nothing for now
}

//Draw height map
void HM::draw(){
glColor3d(1,0,0);
	//glCallList(OBJ_PLANE_GRID);
	glCallList(OBJ_PLANE_QUAD);
}

//Generate a random map
void HM::randomize(){
	//TODO:disp should decrease

	//TEST: 10 times iteration
	for(int i = 0;i<50;i+=1){
		make_circle();
	}
	//The call list seems to be linked with unchange points
	set_texture();
	make_grid_list();
	make_quad_list();
}

//Draw the map in planar mode
void HM::make_quad_list(){
	glNewList(OBJ_PLANE_QUAD, GL_COMPILE);
	    
	//Bind texture
	
	//glBindTexture(GL_TEXTURE_2D, texture[0]); 

	//Try bind material color
	float shineBuffer[1] = {20};
	float specularBuffer[3] = {1,1,1};
	float diffuseBuffer[3] = {0,1,0};
	glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, shineBuffer);
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specularBuffer);	
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseBuffer);
	//Actual drawing
	glBegin(GL_TRIANGLES); 
	
	
	for(int i =0;i<MAP_BOUND-1;i+=1){ 
		for(int j = 0;j<MAP_BOUND-1;j+=1){
		//so basicallly we just need to calculate the normal for each small face?
		//Try adding normal
			Vector3D x =  height_map[i+1][j]-height_map[i][j] ;
			Vector3D y =  height_map[i+1][j+1] - height_map[i+1][j];
			Vector3D normal = x.cross(y);
			normal.normalize();
			glNormal3d(normal[0],normal[1],normal[2]);	
		//Use this simple normal for now

			//glTexCoord2d(0, 0);
			glVertex3d(height_map[i][j][0],height_map[i][j][1],height_map[i][j][2]);
			//glTexCoord2d(1, 0);
			glVertex3d(height_map[i+1][j][0],height_map[i+1][j][1],height_map[i+1][j][2]);
			//glTexCoord2d(1, 1);
			glVertex3d(height_map[i+1][j+1][0],height_map[i+1][j+1][1],height_map[i+1][j+1][2]);

			//glTexCoord2d(0, 0);
			glVertex3d(height_map[i][j][0],height_map[i][j][1],height_map[i][j][2]);
			//glTexCoord2d(0, 1);
			glVertex3d(height_map[i][j+1][0],height_map[i][j+1][1],height_map[i][j+1][2]);
			//glTexCoord2d(1, 1);
			glVertex3d(height_map[i+1][j+1][0],height_map[i+1][j+1][1],height_map[i+1][j+1][2]);
			
		}
	}

	glEnd();	
    glEndList();
}

//Private functions

//Draw the plane in wire frame mode
void HM::make_grid_list(){
	
	glNewList(OBJ_PLANE_GRID, GL_COMPILE);
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
    glEndList();
}

//Return a random in from 0 to (range-1)
int HM::make_rand(int range){
	return (std::rand()%range);
}

//Use circle algorithm for genertaion
void HM::make_circle(){
	//Choose a random point from grid to be the circle center
	int i = make_rand(MAP_BOUND);
	int j = make_rand(MAP_BOUND);
	Point3D center = height_map[i][j];
	
	//Random radius
	double radius =make_rand(MAP_BOUND/3)+1;
	//Shoud decrease based on iteration
	double disp = 1;

	for(int i =0;i< MAP_BOUND ;i+=1){ 
		for(int j = 0;j< MAP_BOUND ;j+=1){
			double dis = (height_map[i][j] - center).length();
			double pd = dis*2/radius; 
			
			if(fabs(pd)<= 1.0){
				height_map[i][j][2] += disp/2+cos(pd* M_PI)*disp/2;
			}
		}
	}

}

//std::cerr<<"Texture name = "<<texture[0]<<std::endl;
	//std::cerr<<(glIsTexture(texture[0])==GL_FALSE)<<std::endl;
	//std::cerr<<"ERROR"<< (glGetError()==GL_NO_ERROR)<<std::endl;
//load the texture map
	/*bool load_texture =  m_texture.loadPng("terrain.png");
	if(!load_texture){
		std::cerr<<"Fail to load texture"<<std::endl;
	}*/
		//Texture mapping, for every pixel we assign them with a texture point
			
//Now I know the reason , lets try to load the terrain png
			/*glTexCoord2d((double)i/4, (double)j/4);
			glVertex3d(height_map[i][j][0],height_map[i][j][1],height_map[i][j][2]);
			glTexCoord2d((double)(i+1)/4, (double)j/4);
			glVertex3d(height_map[i+1][j][0],height_map[i+1][j][1],height_map[i+1][j][2]);
			glTexCoord2d((double)(i+1)/4, (double)(j+1)/4);
			glVertex3d(height_map[i+1][j+1][0],height_map[i+1][j+1][1],height_map[i+1][j+1][2]);

			glTexCoord2d((double)i/4, (double)j/4);
			glVertex3d(height_map[i][j][0],height_map[i][j][1],height_map[i][j][2]);
			glTexCoord2d((double)i/4, (double)(j+1)/4);
			glVertex3d(height_map[i][j+1][0],height_map[i][j+1][1],height_map[i][j+1][2]);
			glTexCoord2d((double)(i+1)/4, (double)(j+1)/4);
			glVertex3d(height_map[i+1][j+1][0],height_map[i+1][j+1][1],height_map[i+1][j+1][2]);

*/
