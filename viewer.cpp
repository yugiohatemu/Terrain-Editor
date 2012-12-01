#include "viewer.hpp"
#include "algebra.hpp"
#include <iostream>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
//#include <GL/glut.h>


double pos[2]  = {0};
bool mouse_pressed[3] ={false,false,false};
int hit_name, hit_count = 0;
double trans[3] = {0};
bool clear_redo_stack = false;
GLuint testing;

double fov_pos[3] = {0};
double rotate[3]  = {0};

double look_at[2] = {1,1};

GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };  
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };  
GLfloat light_position[] = {5, 10, -10, 0.0 }; 

Viewer::Viewer()
{
  Glib::RefPtr<Gdk::GL::Config> glconfig;
  glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB | Gdk::GL::MODE_DEPTH |Gdk::GL::MODE_DOUBLE|Gdk::GL::MODE_STENCIL);

  if (glconfig == 0) {
    // If we can't get this configuration, die
    std::cerr << "Unable to setup OpenGL Configuration!" << std::endl;
    abort();
  }

  // Accept the configuration
  set_gl_capability(glconfig);

  // Register the fact that we want to receive these events
	//Pointer motion for mouse motion without clicking
  add_events(Gdk::BUTTON1_MOTION_MASK    |
             Gdk::BUTTON2_MOTION_MASK    |
             Gdk::BUTTON3_MOTION_MASK    |
             Gdk::BUTTON_PRESS_MASK      | 
             Gdk::BUTTON_RELEASE_MASK    |
			 Gdk::POINTER_MOTION_MASK	 |
             Gdk::VISIBILITY_NOTIFY_MASK);

	m_mode = GOD_MODE;
	l_mode = LIGHT_POSITION;
	a_mode = ADD_SPHERE;
}

Viewer::~Viewer(){
}

void Viewer::invalidate(){
  Gtk::Allocation allocation = get_allocation();
  get_window()->invalidate_rect( allocation, false);
}


void Viewer::reset_position(){
	trans[0] = 0;
	trans[1] = 0;
	trans[2] = 0;
	invalidate();
}

void Viewer::reset_orientation(){
	invalidate();
}

void Viewer::reset_joint(){
	root->reset_joint();
	//actually i prefer inifinite undo here, but lets fix here first
	while (!redo_stack.empty()){
			redo_stack.pop();
  	}
	while (!undo_stack.empty()){
			undo_stack.pop();
  	}
	invalidate();
}

void Viewer::reset_all(){
	trans[0] = 0;
	trans[1] = 0;
	trans[2] = 0;
	while (!redo_stack.empty()){
			redo_stack.pop();
  	}
	while (!undo_stack.empty()){
			undo_stack.pop();
  	}
	invalidate();
}	

//For redo and undo joint 

void Viewer::undo(){
	if(!undo_stack.empty()){
		undo_stack.pop();
		
		invalidate();//if do that need to judge whether it is by mouse button[0]
	}
}

void Viewer::redo(){
	if(!redo_stack.empty()){
		redo_stack.pop();
		
		invalidate();
	}
}

//Setup for calllist, enablelist and etc

void Viewer::on_realize()
{
  // Do some OpenGL setup.
  // First, let the base class do whatever it needs to
	Gtk::GL::DrawingArea::on_realize();

	Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

	if (!gldrawable)
		return;

	if (!gldrawable->gl_begin(get_gl_context()))
		return;

	
	glClearColor( 0.5, 0.6, 0.7, 0.0 );

	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);  
	glEnable(GL_LIGHT0);  
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
 	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);
	gldrawable->gl_end();

	//Generate a randome map
	m_map.randomize();
	m_water.water_test();
	//test();

	t_terrain.set_type(T_BUMP);
	t_terrain.bind_texture();
	
	t_water.set_type(T_WATER);
	t_water.bind_texture();
	
	rain.make_particle();

}

void Viewer::set_light(){
	 
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);  
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);  
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	
}

void Viewer::test(){
	//start
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); //disable the color mask
    glDepthMask(GL_FALSE); //disable the depth mask

    glEnable(GL_STENCIL_TEST); //enable the stencil testing

    glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE); //set the stencil buffer to replace our next lot of data
	
	glTranslated(0,0,2);
	//t_water.apply_texture();
	glBindTexture(GL_TEXTURE_2D, 0);
	float m_shineBuffer[1] = {60};
	float m_specularBuffer[4] = {1,1,1,1};
	float m_diffuseBuffer[4] = {1,1,1,0.95};
	
	glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, m_shineBuffer);
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, m_specularBuffer);	
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, m_diffuseBuffer);
    m_water.draw(); //set the data plane to be replaced
	
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); //enablethe color mask
    glDepthMask(GL_TRUE); //enable the depth mask

    glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); //set the stencilbuffer to keep our next lot of data

    glDisable(GL_DEPTH_TEST); //disable depth testing of thereflection
    glPushMatrix();
    glScalef(1.0f, -1.0f, 1.0f); //flip the reflection vertically

   	//glTranslated(5,5,0); //translate the reflection onto thedrawing plane
    //glRotatef(60,0,1,0); //rotate the reflection
	//t_terrain.apply_texture();
	//glBindTexture(GL_TEXTURE_2D, 0);
	float shineBuffer[1] = {0};
	float specularBuffer[4] = {1,1,1,1};
	float diffuseBuffer[4] = {0,1,0,1};
	
	glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, shineBuffer);
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specularBuffer);	
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseBuffer);
	t_terrain.apply_texture();
	//m_map.draw();
    GLUquadricObj *quadric = gluNewQuadric();
	gluSphere( quadric , 3 , 20 , 20 );   //draw the square as our reflection
    glPopMatrix();
    glEnable(GL_DEPTH_TEST); //enable the depth testing

    glDisable(GL_STENCIL_TEST); //disable the stencil testing
}

bool Viewer::on_expose_event(GdkEventExpose* event){
	Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

	if (!gldrawable) return false;

	if (!gldrawable->gl_begin(get_gl_context()))
	return false;

	// Set up for perspective drawing 
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();	
	glViewport(0, 0, get_width(), get_height());
	gluPerspective(40.0, (GLfloat)get_width()/(GLfloat)get_height(), 0.1, 1000.0);

	// change to model view for drawing
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	set_light();
	//I think the best way is to change this
	if(m_mode == GOD_MODE){
		gluLookAt(0,0,trans[2]+40, 0,0,-1000,0,1,0);
		glRotated(trans[0],0,1,0);
		glRotated(trans[1],0,0,1);
		glRotated(-90,1,0,0);
	}else{
		
		double height= m_map.get_height(fov_pos[0],fov_pos[2]);
		gluLookAt(fov_pos[0],fov_pos[1]+height+1,fov_pos[2]+40,0,0,-1000,0,1,0);
		
		glRotated(-90,1,0,0);
		glRotated(look_at[0],0,0,1); //left right
		glRotated(look_at[1],0,1,0); //up down
		//Now get info from the HM
	}
	glPushMatrix();
	//test();
	glPopMatrix();

	//Draw indicator for light. or sun 
/*
	glPushMatrix();
	glTranslated(-light_position[0],-light_position[1],-light_position[2]);
	glBindTexture(GL_TEXTURE_2D, 0);
	GLUquadricObj *quadric = gluNewQuadric();
	gluSphere( quadric , 1 , 20 , 20 );
	glPopMatrix();

	glPushMatrix();
	glTranslated(0,0,10);
	m_scene.draw();
	glPopMatrix();
	
	//Rain
	glPushMatrix();
	rain.draw();
	glPopMatrix();
	*/
	//Terrain
	glPushMatrix();
	float shineBuffer[1] = {20};
	float specularBuffer[4] = {1,1,1,1};
	float diffuseBuffer[4] = {0,0.8,0,1};
	
	glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, shineBuffer);
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specularBuffer);	
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseBuffer);
	t_terrain.apply_texture();
	m_map.draw();
	glPopMatrix();
	
	//Water
	//glBlendFunc (GL_ONE, GL_ONE);
	glEnable(GL_BLEND);;
	glPushMatrix();//glBlendFunc (GL_ONE, GL_ONE);
	
	glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA) ;
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTranslated(0,0,2);

	float m_shineBuffer[1] = {60};
	//float m_specularBuffer[4] = {1,1,1,1};
	float m_diffuseBuffer[4] = {0,0.5,0.8,0.9};
	
	glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, m_shineBuffer);
	//glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, m_specularBuffer);	
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, m_diffuseBuffer);	

	t_water.apply_texture();
	m_water.water_test();
	m_water.draw();
	glPopMatrix();
	glDisable(GL_BLEND);
	//Finish drawing
	glPopMatrix();
	gldrawable->swap_buffers();
	gldrawable->gl_end();
	
	return true;
}

bool Viewer::on_configure_event(GdkEventConfigure* event)
{
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable) return false;
  
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  // Set up perspective projection, using current size and aspect
  // ratio of display

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, event->width, event->height);
  gluPerspective(40.0, (GLfloat)event->width/(GLfloat)event->height, 0.1, 1000.0);
  
  glMatrixMode(GL_MODELVIEW);

  gldrawable->gl_end();

  return true;
}

void Viewer::list_hits(GLint hits, GLuint *names){
	
	hit_name = (GLubyte)names[3];
	hit_count = hits;
 	
}

void Viewer::gl_select(int x, int y){
	GLuint buff[64] = {0};
 	GLint view[4];
	glSelectBuffer(64, buff);
 	//std::cerr<<"Select "<<x<<" "<<y<<std::endl;
 	glGetIntegerv(GL_VIEWPORT, view);
 	glRenderMode(GL_SELECT);
 	glInitNames();
 	glPushName(-1); //other wise load eroor
 
 	glMatrixMode(GL_PROJECTION);
 	glPushMatrix();
 	glLoadIdentity();
 	gluPickMatrix(x, y, 1, 1, view);
 	gluPerspective(40.0, (float)view[2]/(float)view[3], 0.1, 1000.0);
 	glMatrixMode(GL_MODELVIEW);
 		
	//Redraw the scene?
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//TODO: Picking is true , remeber to reset after doing menu
	gluLookAt(0, 0,trans[2], -trans[0], trans[1],-1000,0,1,0);
	
	glPushMatrix();
	root->walk_gl(true);
	
	//TODO: Note that track ball will increase the hit by 1, so remeber to add name for ball
	
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
 	glPopMatrix();
	
	GLint hits = glRenderMode(GL_RENDER);
	list_hits(hits, buff);
	glMatrixMode(GL_MODELVIEW);
	invalidate();
}

void Viewer::get_key_input(char input){

	if(m_mode == NORMAL_MODE){
		if(input == 'w'){
			fov_pos[2] -= 1;
		}else if(input == 'a'){
			fov_pos[0] -= 1;
		}else if(input == 's'){
			fov_pos[2] += 1;
		}else if(input == 'd'){
			fov_pos[0] += 1;
		}
		invalidate();
	}else if(m_mode == GOD_MODE){ //GOD_MODE
		if(l_mode == LIGHT_POSITION){
			if(input == 'w'){
				light_position[0] -= 0.5;
			}else if(input == 'a'){
				light_position[1] += 0.5;
			}else if(input == 's'){
				light_position[0] += 0.5;
			}else if(input == 'd'){
				light_position[1] -= 0.5;
			}
		}
		invalidate();
	}
}


bool Viewer::on_button_press_event(GdkEventButton* event){
	pos[0] =  event->x;
	pos[1] =  event->y;
	mouse_pressed[event->button-1] = true;
	//Use shift
	if(a_mode == ADD_SPHERE && m_mode == EDIT_MODE){
		if(mouse_pressed[0] ){
			m_scene.add_object(P_BALL);
		}	
	}
 	return true;
}

bool Viewer::on_button_release_event(GdkEventButton* event){
	
	mouse_pressed[event->button-1] = false;
	return true;
}


bool Viewer::on_motion_notify_event(GdkEventMotion* event){
	
	double disc_x= event->x - pos[0];
  	double disc_y = event->y - pos[1];
	
	if(m_mode == GOD_MODE ){
		int ry = (disc_y > 0) ? 1 : -1;
		int rx = (disc_x > 0) ? 1 : -1;
		
		if(mouse_pressed[0]){ //rotate x
			trans[0] += rx ;
			invalidate();
		}
		if(mouse_pressed[1]){ //rotate y
			trans[1] += ry ; 
			invalidate();
		}
		if(mouse_pressed[2]){ //rotate z
			trans[2] += ry;
			invalidate();
		}
		
	}else if(m_mode == NORMAL_MODE){
    	//need to capture the mouse motion
		//up,down,left,right

		double ry = (disc_y > 0) ? 0.1 : -0.1;
		double rx = (disc_x > 0) ? 0.1 : -0.1;
	
		look_at[0] -= rx;	
		look_at[1] -= ry;	
		invalidate();
	}

	
	pos[0] = event->x;
	pos[0] = event->y;
	return true;
}


/*	//set up call list
	glNewList(OBJ_SPHERE, GL_COMPILE);
	GLUquadricObj *quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_FILL );
	gluSphere( quadric , 1 , 20 , 20 );  
	glEndList();*/
//Sky box
/*
	//Draw the quad
	glPushMatrix();
	glTranslated(-10,0,-10);
	sky.apply_texture();
	glBegin(GL_QUADS);
	glNormal3d(1,0,0);
	glColor3d(1,1,1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//The coordinate seems to be bad in a contray order
	glTexCoord2d(0, 1); glVertex3d(0,0,0);
	glTexCoord2d(1, 1);	glVertex3d(20,0,0);
	glTexCoord2d(1, 0); glVertex3d(20,15,0);
	glTexCoord2d(0, 0); glVertex3d(0,15,0);
	glEnd();

	glPopMatrix();	*/	
/*float m_shineBuffer[1] = {60};
	float m_specularBuffer[4] = {1,1,1,1};
	float m_diffuseBuffer[4] = {1,1,1,0.95};
	
	glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, m_shineBuffer);
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, m_specularBuffer);	
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, m_diffuseBuffer);	*/
