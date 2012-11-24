#include "viewer.hpp"
#include "algebra.hpp"
#include <iostream>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>



double pos[2]  = {0};
bool mouse_pressed[3] ={false,false,false};
int hit_name, hit_count = 0;
int joint[2] = {0}; //x , y
double trans[3] = {0};
bool clear_redo_stack = false;
GLuint testing;

double fov_pos[3] = {0};
double rotate[3]  = {0};
int angle = 0;

Viewer::Viewer()
{
  Glib::RefPtr<Gdk::GL::Config> glconfig;

  // Ask for an OpenGL Setup with
  //  - red, green and blue component colour
  //  - a depth buffer to avoid things overlapping wrongly
  //  - double-buffered rendering to avoid tearing/flickering
  glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB |
                                     Gdk::GL::MODE_DEPTH |
                                     Gdk::GL::MODE_DOUBLE);
  if (glconfig == 0) {
    // If we can't get this configuration, die
    std::cerr << "Unable to setup OpenGL Configuration!" << std::endl;
    abort();
  }

  // Accept the configuration
  set_gl_capability(glconfig);

  // Register the fact that we want to receive these events
  add_events(Gdk::BUTTON1_MOTION_MASK    |
             Gdk::BUTTON2_MOTION_MASK    |
             Gdk::BUTTON3_MOTION_MASK    |
             Gdk::BUTTON_PRESS_MASK      | 
             Gdk::BUTTON_RELEASE_MASK    |
             Gdk::VISIBILITY_NOTIFY_MASK);

	//TODO: we can load it here
	/*root = import_lua("puppet.lua"); 
  if (!root) {
    std::cerr << "Could not open " << std::endl;
		abort();
  }*/
	
	m_mode = GOD_MODE;
}

Viewer::~Viewer()
{
  // Nothing to do here right now.
}

void Viewer::invalidate()
{
  // Force a rerender
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
	Matrix4x4 i;
	//m_rotation = i;
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
	Matrix4x4 i;
	//m_rotation = i;
	root->reset_joint();
	while (!redo_stack.empty()){
			redo_stack.pop();
  }
	while (!undo_stack.empty()){
			undo_stack.pop();
  }
	invalidate();
}	

//For redo and undo joint 
//We can do this the easy way
//Every time we pick a node, create a stack
//It makes sense since picking a node will change the material

void Viewer::undo(){
	if(!undo_stack.empty()){
		joint_stack j = undo_stack.top();
		//std::cerr<<j.x<<" "<<j.y<<std::endl;
		root->rotate_joint('x', -j.x);
		root->rotate_joint('y', -j.y);
		// rotate x , y
		undo_stack.pop();
		redo_stack.push(j);
		if(!j.selected){
			root->pick(j.name);
		} 
		invalidate();//if do that need to judge whether it is by mouse button[0]
	}
}

void Viewer::redo(){
	if(!redo_stack.empty()){
		joint_stack j = redo_stack.top();
		
		root->rotate_joint('x',j.x);
		root->rotate_joint('y',j.y);
		
		redo_stack.pop();
		undo_stack.push(j);
		if(!j.selected){
			root->pick(j.name);
		} 
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

  glShadeModel(GL_SMOOTH);
  glClearColor( 0.5, 0.6, 0.7, 0.0 );
  glEnable(GL_DEPTH_TEST);
	
	//so the lightint is here, damn it...
	GLfloat ambient[] = { 0.0, 0.0, 0.0, 1.0 };  
	  GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };  
	  GLfloat position[] = {5, 10, -10, 0.0 };  
  		
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);  
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);  
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);  
	glEnable(GL_LIGHT0);  
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	gldrawable->gl_end();

	//TODO:Move later
	//m_mode = Edit_Pos;
	for(int i =0;i<4;i+=1){
		options[i] = false;
	}

	//set up call list
	glNewList(OBJ_SPHERE, GL_COMPILE);
	GLUquadricObj *quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_FILL );
	gluSphere( quadric , 1 , 20 , 20 );  
	glEndList();
	
	//TODO:recover
	//Generate a randome map
	m_map.randomize();
	test();

	marble.set_texture("marble.jpg",GL_RGB,64);
	marble.bind_texture();

	
	sky.set_texture("oright19.jpg",GL_RGB ,512);
	sky.bind_texture();

	

	//glDisable(GL_TEXTURE_2D);
}

void Viewer::test(){
	//Water texture special
	reflection.set_texture("reflection.jpg",GL_RGB,256 );
	alpha.set_texture("alpha.jpg",GL_ALPHA,256 );	

	reflection.load_texture();
	alpha.load_texture();	

	unsigned char total_texture[4 * 256 * 256];
	unsigned char * caustic_texture = reflection.get_data();
	unsigned char * alpha_texture = alpha.get_data();
	glGenTextures (1, &testing);
	
	for (int i = 0; i < 256 * 256; i++){
      total_texture[4 * i] = caustic_texture[3 * i];
      total_texture[4 * i + 1] = caustic_texture[3 * i + 1];
      total_texture[4 * i + 2] = caustic_texture[3 * i + 2];
      total_texture[4 * i + 3] = alpha_texture[i];
    }
	glBindTexture (GL_TEXTURE_2D, testing);
	gluBuild2DMipmaps (GL_TEXTURE_2D, GL_RGBA, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE,  total_texture);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glEnable (GL_TEXTURE_GEN_S);
	glEnable (GL_TEXTURE_GEN_T);
	glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
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
	
}



bool Viewer::on_expose_event(GdkEventExpose* event)
{
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
	// Clear framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	//TODO: remeber to move to gl select
	
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_TEXTURE_2D);
	//I think the best way is to change this
	if(m_mode == GOD_MODE){
		gluLookAt(0,trans[1],trans[2]+40, 0,0,-1000,0,1,0);
		glRotated(trans[0],0,1,0);
		glRotated(-60,1,0,0);
	}else{
		
		double height= m_map.get_height(fov_pos[0],fov_pos[2]);
		gluLookAt(fov_pos[0],fov_pos[1]+height+1,fov_pos[2]+40,0,0,-1000,0,1,0);
		glRotated(-90,1,0,0);
		//Now get info from the HM
	}
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	//glActiveTexture(1);
	marble.apply_texture();
	//std::cerr<<marble.get_id()<<std::endl;
	float shineBuffer[1] = {20};
	float specularBuffer[4] = {1,1,1,1};
	float diffuseBuffer[4] = {0,0.5,0,1};
	
	glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, shineBuffer);
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specularBuffer);	
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseBuffer);
	
	m_map.draw();
	//test() for skybox
	glBindTexture(GL_TEXTURE_2D, 0);
	//glBlendFunc (GL_ONE, GL_ONE);
	glColor4f(1.0, 1.0, 1.0, 0.5);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
	glPushMatrix();
	glTranslated(0,0,2);
	glBindTexture(GL_TEXTURE_2D, testing);
	//std::cerr<<testing<<std::endl;
	m_water.draw();
	glPopMatrix();
		
	//Terrain coment out for now
	//Unbind texture  ; 
		
	
	
	//Finish drawing
	glPopMatrix();
	gldrawable->swap_buffers();
	gldrawable->gl_end();
	//glDisable(GL_TEXTURE_2D);
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
	//glMultMatrixd(m_translation.begin());
	//glMultMatrixd(m_rotation.transpose().begin());
	root->walk_gl(true);
	
	//TODO: Note that track ball will increase the hit by 1, so remeber to add name for ball
	
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
 	glPopMatrix();
	
	GLint hits = glRenderMode(GL_RENDER);
	list_hits(hits, buff);
	glMatrixMode(GL_MODELVIEW);
	//TODO:finaly draw again for applying phong change
	invalidate();
}

void Viewer::get_key_input(char input){
	//std::cerr<<input<<std::endl;
	if(m_mode == NORMAL_MODE){
		switch(input){
			case 'w':
				fov_pos[2] -= 1;
				break;
			case 'a':
				fov_pos[0] -= 1;
				break;			
			case 's':
				fov_pos[2] += 1;
				break;
			case 'd':
				fov_pos[0] += 1;
				break;
		}
		invalidate();
	}
}


bool Viewer::on_button_press_event(GdkEventButton* event){
	pos[0] =  event->x;
	pos[1] =  event->y;
	mouse_pressed[event->button-1] = true;
	joint[0] = 0;
	joint[1] = 0;
	//need to minus event button to be put in the array
	/*if(mouse_pressed[0] ){ //
			gl_select(event->x, get_height()-event->y); 
			root->pick(hit_name); 
	}*/
 	
  return true;
}

bool Viewer::on_button_release_event(GdkEventButton* event){
	/*if(m_mode == Edit_Joint){ 
		//Update undo stack , use minus since we are undoing
		if(mouse_pressed[0]){
			joint_stack j(hit_name, 0 , 0,false);
			undo_stack.push(j);	
		}else{
			joint_stack j(hit_name, joint[1]%30 , joint[0]%30,true);
			undo_stack.push(j);	
		}
		
		//clean the redo stack if a the user moves
		if(clear_redo_stack){
			while (!redo_stack.empty()){
			redo_stack.pop();
  			}
		}
		clear_redo_stack = false;
	}*/
	
	mouse_pressed[event->button-1] = false;
	return true;
}


bool Viewer::on_motion_notify_event(GdkEventMotion* event){
	
	double disc_x= event->x - pos[0];
  	double disc_y = event->y - pos[1];
	
	if(m_mode == GOD_MODE){
		int ry = (disc_y > 0) ? 1 : -1;
		int rx = (disc_x > 0) ? 1 : -1;
		angle += rx;
		if(mouse_pressed[0]){ //rotate x
			trans[0] += rx ;
			
		}
		if(mouse_pressed[1]){ //rotate y
			trans[1] += ry ; 
			
		}
		if(mouse_pressed[2]){ //rotate z
			trans[2] += ry;
		}
		
	}else if(m_mode == NORMAL_MODE){
    
	}

	invalidate();
	pos[0] = event->x;
	pos[0] = event->y;
	return true;
}

//Bascially it is mixing alpha and rbg texture togethre
//Which is infact multitexture mapping?
//setup water and reflection texture
//TODO:Remeber to fix texture for proper mapping DO NOT DELETE
/*
	reflection.set_texture("reflection.jpg",GL_RGB,256 );
	alpha.set_texture("alpha.jpg",GL_ALPHA,256 );	

	reflection.load_texture();
	alpha.load_texture();	

	unsigned char total_texture[4 * 256 * 256];
	unsigned char * caustic_texture = reflection.get_data();
	unsigned char * alpha_texture = alpha.get_data();
	glGenTextures (1, &test);
	
	for (int i = 0; i < 256 * 256; i++){
      total_texture[4 * i] = caustic_texture[3 * i];
      total_texture[4 * i + 1] = caustic_texture[3 * i + 1];
      total_texture[4 * i + 2] = caustic_texture[3 * i + 2];
      total_texture[4 * i + 3] = alpha_texture[i];
    }
	glBindTexture (GL_TEXTURE_2D, test);
	gluBuild2DMipmaps (GL_TEXTURE_2D, GL_RGBA, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE,  total_texture);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glEnable (GL_TEXTURE_GEN_S);
	glEnable (GL_TEXTURE_GEN_T);
	glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);*/
/*
	//Water
	glPushMatrix();
	glTranslated(0,0, 5);
	float shineBuffer[1] = {60};
	float specularBuffer[3] = {1,1,1};
	float diffuseBuffer[3] = {1,1,1};
	
	diffuseBuffer[0] = 1; diffuseBuffer[1] = 1;diffuseBuffer[2] = 1; diffuseBuffer[3] = 0;shineBuffer[0] = 60;
 	glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, shineBuffer);
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specularBuffer);	
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseBuffer);
	//reflection.apply_texture();
	glBindTexture(GL_TEXTURE_2D, test);
	m_water.draw();
	//m_water.water_test();
	glPopMatrix();
*/
