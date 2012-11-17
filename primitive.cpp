#include "primitive.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

void Sphere::walk_gl(bool picking) const
{

		glCallList(OBJ_SPHERE);

}

/*
  glEnable(GL_LINE_SMOOTH);
  
  
  glDisable(GL_LINE_SMOOTH);*/
/*
	GLUquadricObj *quadric = gluNewQuadric();

	gluQuadricDrawStyle(quadric, GLU_FILL );
	gluSphere( quadric , 1 , 20 , 20 );*/
/*
	double radius = 1;
	int lats = 20;
	int longs = 20;
	//TODO: display list should be implanted here
	for(int i = 0;i<lats;i+=1){
		float z0 = radius * sin(i*2*M_PI/lats);
		float zr0 = radius * cos(i*2*M_PI/lats);
    float z1 = radius * sin((i+1)*2*M_PI/lats);
		float zr1 = radius * cos((i+1)*2*M_PI/lats);
		//can use line strip to test whether it is drawn correctly
		glBegin(GL_LINE_STRIP);		
		//glBegin(GL_QUAD_STRIP);
		for (int j = 0;j<longs;j+=1){
			
			float x0 = cos(j*2*M_PI/longs);
			float y0 = sin(j*2*M_PI/longs);
			float x1 = cos((j+1)*2*M_PI/longs);
			float y1 = sin((j+1)*2*M_PI/longs);
			
			glVertex3f(x0*zr0 , y0*zr0, z0);			
			glVertex3f(x0*zr1 , y0*zr1, z1);
			glVertex3f(x1*zr0 , y1*zr0, z0);
			glVertex3f(x1*zr1 , y1*zr1, z1);
			
		}
		glEnd();	
		
	}*/
