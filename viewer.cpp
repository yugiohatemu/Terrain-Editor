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
GLfloat light_position[] = {0, 10, -10, 0.0 }; 

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
	t_size = SIZE_L;
	e_env = GRASS;
	w_water = SLOW;
	w_weather = W_NO;
	//m_timer.disconnect();
	sigc::slot0<bool> tick_slot = sigc::mem_fun(*this,&Viewer::call_invalidate);
	m_timer=Glib::signal_timeout().connect(tick_slot, 50);

	trans[0] = 0;
	trans[1] = 0;
	trans[2] = t_size;

	fov_pos[0] = 4;
	fov_pos[1] = 5;
	fov_pos[2] = 4;
}

Viewer::~Viewer(){
}

bool Viewer::call_invalidate(){
 	invalidate();
	return true;
}

void Viewer::invalidate(){
  Gtk::Allocation allocation = get_allocation();
  get_window()->invalidate_rect( allocation, false);
}


void Viewer::reset_position(){
	m_map.reset();
	m_map.randomize();
}

void Viewer::reset_orientation(){
}

void Viewer::reset_joint(){
}

void Viewer::reset_all(){
}	

//For redo and undo joint 

void Viewer::undo(){
}

void Viewer::redo(){
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
	//glEnable(GL_BLEND);
	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
 	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);
	gldrawable->gl_end();

	//Generate a randome map
	m_map.randomize();
	m_water.water_test();
	//test();
	
	t_sand.set_type(T_SAND_BUMP);
	t_sand.bind_texture();	

	t_grass.set_type(T_GRASS_BUMP);
	t_grass.bind_texture();
	
	t_water.set_type(T_WATER);
	t_water.bind_texture();

	t_sky_top.set_type(T_SKYBOX_TOP);
	t_sky_left.set_type(T_SKYBOX_LEFT);
	t_sky_right.set_type(T_SKYBOX_RIGHT);
	t_sky_front.set_type(T_SKYBOX_FRONT);
	t_sky_back.set_type(T_SKYBOX_BACK);

	t_sky_top.bind_texture();
	t_sky_left.bind_texture();
	t_sky_right.bind_texture();
	t_sky_front.bind_texture();
	t_sky_back.bind_texture();

	

	rain.make_particle();

}

void Viewer::set_light(){
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);  
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);  
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void Viewer::draw_skybox(){
    glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	int map_size = 125; //half the size of map
	glRotated(-90,1,0,0);

	glPushMatrix();	
	
	t_sky_front.apply_texture();
	glBegin(GL_QUADS);
	
	glTexCoord2d(0, 0);glVertex3d(map_size,-map_size,-map_size); 
	glTexCoord2d(1, 0);glVertex3d(-map_size,-map_size,-map_size);
	glTexCoord2d(1, 1);glVertex3d(-map_size,map_size,-map_size);	
	glTexCoord2d(0, 1);glVertex3d(map_size,map_size,-map_size);
	
	glEnd();
	glPopMatrix();

	glPushMatrix();	
	t_sky_back.apply_texture();
	glBegin(GL_QUADS);
	
	glTexCoord2d(0, 0);glVertex3d(-map_size,-map_size,map_size); 
	glTexCoord2d(1, 0);glVertex3d(map_size,-map_size,map_size);
	glTexCoord2d(1, 1);glVertex3d(map_size,map_size,map_size);	
	glTexCoord2d(0, 1);glVertex3d(-map_size,map_size,map_size);
	
	glEnd();
	glPopMatrix();
	
	//Left
	glPushMatrix();	
	t_sky_left.apply_texture();
	glBegin(GL_QUADS);
	
	//The coordinate seems to be bad in a contray order
	glTexCoord2d(0, 0);glVertex3d(map_size,-map_size,map_size); 
	glTexCoord2d(1, 0);glVertex3d(map_size,-map_size,-map_size);
	glTexCoord2d(1, 1);glVertex3d(map_size,map_size,-map_size);	
	glTexCoord2d(0, 1);glVertex3d(map_size,map_size,map_size);
	
	glEnd();
	glPopMatrix();

	//Right
	glPushMatrix();	
	t_sky_right.apply_texture();
	glBegin(GL_QUADS);
	
	glTexCoord2d(0, 0);glVertex3d(-map_size,-map_size,-map_size); 
	glTexCoord2d(1, 0);glVertex3d(-map_size,-map_size,map_size);
	glTexCoord2d(1, 1);glVertex3d(-map_size,map_size,map_size);	
	glTexCoord2d(0, 1);glVertex3d(-map_size,map_size,-map_size);
	
	glEnd();
	glPopMatrix();

	//Top
	glPushMatrix();	
	t_sky_top.apply_texture();
	glBegin(GL_QUADS);
	
	glTexCoord2d(0, 0);glVertex3d(-map_size,-map_size,-map_size); 
	glTexCoord2d(1, 0);glVertex3d(-map_size,-map_size,map_size);
	glTexCoord2d(1, 1);glVertex3d(map_size,-map_size,map_size);	
	glTexCoord2d(0, 1);glVertex3d(map_size,-map_size,-map_size);
	
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);

}

void Viewer::set_size(Size s){
	m_map.reset();
	m_map.randomize();
}

void Viewer::set_environment(Environment e){
	e_env = e;
}

void Viewer::set_water(Water w){
	if(w_water != w){
		w_water = w;
		m_water.set_water_speed(w_water);
		//invalidate();
	}  

}

void Viewer::set_weather(Weather w){
	w_weather = w;
}

void Viewer::test(){
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); //disable the color mask
    glDepthMask(GL_FALSE); //disable the depth mask

    glEnable(GL_STENCIL_TEST); //enable the stencil testing

    glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE); 
	
	glTranslated(0,0,2);
    m_water.draw(); //set the data plane to be replaced
	
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); //enablethe color mask
    glDepthMask(GL_TRUE); //enable the depth mask

    glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); //set the stencilbuffer to keep our next lot of data

    glDisable(GL_DEPTH_TEST); //disable depth testing of thereflection
    glPushMatrix();
    glScalef(1.0f, -1.0f, 1.0f); //flip the reflection vertically
   	//translate the reflection onto thedrawing plane
	if(l_mode == LIGHT_REFLECTION){
		float shineBuffer[1] = {60};
		float diffuseBuffer[4] = {0,0.8,0,1};
		float specularBuffer[4] = {1,1,1,1};
		glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, shineBuffer);
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseBuffer);
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specularBuffer);
	}else{
		//Shadow
		float diffuseBuffer[4] = {0,0,0,1};
		float specularBuffer[4] = {1,1,1,1};
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseBuffer);
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specularBuffer);
	}
	m_map.draw();  //draw the square as our reflection
    glPopMatrix();
    glEnable(GL_DEPTH_TEST); //enable the depth testing
    glDisable(GL_STENCIL_TEST); //disable the stencil testing*/
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
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	set_light();
	//I think the best way is to change this
	if(m_mode == GOD_MODE){
		gluLookAt(0,0,trans[2]+40, 0,0,-1000,0,1,0);
		glRotated(trans[0],0,1,0);
		glRotated(trans[1],0,0,1);
		glRotated(-90,1,0,0);
	}else{
		
		//double height= m_map.get_height(fov_pos[0],fov_pos[2]);
		
		gluLookAt(fov_pos[0],fov_pos[1],fov_pos[2],0,0,-1000,0,1,0);
		//std::cerr<<fov_pos[0]<<" "<<fov_pos[1]<<" "<<fov_pos[2]<<std::endl;
		glRotated(-90,1,0,0);
		//glTranslated(0,fov_pos[2],0);
		glRotated(look_at[0],0,0,1);
		 //left right
		//glRotated(look_at[1],0,1,0); //up down
		//Now get info from the HM
	}
	//Reflection first
	if(l_mode != LIGHT_POSITION){
		glPushMatrix();	
		test();
		glPopMatrix();
	}
	//Terrain

	if(l_mode == LIGHT_POSITION){
		glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		if(e_env == GRASS){
			float shineBuffer[1] = {60};
			float diffuseBuffer[4] = {0,0.8,0,1};
			float specularBuffer[4] = {1,1,1,1};
			glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, shineBuffer);
			glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseBuffer);
			glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specularBuffer);
			t_grass.apply_texture();
		}else if(e_env == SAND){
			float shineBuffer[1] = {60};
			float diffuseBuffer[4] = {1,1,0,1};
			float specularBuffer[4] = {1,1,1,1};
			glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, shineBuffer);
			glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseBuffer);
			glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specularBuffer);
			t_sand.apply_texture();

		}else {
			float shineBuffer[1] = {90};
			float diffuseBuffer[4] = {0.9,0.95,0.95,1};
			float specularBuffer[4] = {1,1,1,1};
			glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, shineBuffer);
			glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseBuffer);
			glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specularBuffer);
			//t_snow.apply_texture();
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		m_map.draw();
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}
	//Particle System
	if(m_mode == NORMAL_MODE){
		glPushMatrix();
		glTranslated(fov_pos[0]-4,-fov_pos[2],0);
		glRotated(-look_at[0],0,0,1);
		float shineBuffer[1] = {90};
		float diffuseBuffer[4] = {0.9,0.95,0.95,1};
		float specularBuffer[4] = {1,1,1,1};
		glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, shineBuffer);
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseBuffer);
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specularBuffer);
		if(w_weather == W_RAIN){
			rain.draw_rain();
		}else if(w_weather == W_SNOW){
			rain.draw_snow();
		}
		glPopMatrix();
	}
	
	if(m_mode == NORMAL_MODE){
		glPushMatrix();
		draw_skybox();
		glPopMatrix();
	}

	//Water

	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	if(e_env == SNOW){
		glBlendFunc(GL_ONE, GL_ONE) ;
	}else{
		glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA) ;
	}
	glTranslated(0,0,2);

	float m_shineBuffer[1] = {60};
	float m_diffuseBuffer[4] = {0,0.5,0.9,0.9};
	glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, m_shineBuffer);
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, m_diffuseBuffer);	

	t_water.apply_texture();
	m_water.water_test();
	m_water.draw();
	glPopMatrix();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
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
	for (int i = 0; i < hits; i++){
 		printf(	"%d, %d, %d, %d\n",(GLubyte)names[i*4],(GLubyte)names[i*4+1],(GLubyte)names[i*4+2],(GLubyte)names[i*4+3]);
 	}
	hit_name = (GLubyte)names[3];
	hit_count = hits;
 	
}

void Viewer::gl_select(int x, int y){
	GLuint buff[64] = {0};
 	GLint view[4];
	glSelectBuffer(64, buff);

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
	
	gluLookAt(0,0,trans[2]+40, 0,0,-1000,0,1,0);
	glRotated(trans[0],0,1,0);
	glRotated(trans[1],0,0,1);
	glRotated(-90,1,0,0);
	
	glPushMatrix();
	
	m_map.draw();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
 	glPopMatrix();
	
	GLint hits = glRenderMode(GL_RENDER);
	list_hits(hits, buff);
	glMatrixMode(GL_MODELVIEW);
	//invalidate();
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
		//invalidate();
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
		//invalidate();
	}
}


bool Viewer::on_button_press_event(GdkEventButton* event){
	pos[0] =  event->x;
	pos[1] =  event->y;
	mouse_pressed[event->button-1] = true;
	//Use shift
	/*if(a_mode == ADD_SPHERE && m_mode == EDIT_MODE){
		if(mouse_pressed[0] ){
			m_scene.add_object(P_BALL);
		}	
	}*/
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
		}
		if(mouse_pressed[1]){ //rotate y
			trans[1] += ry ; 
		}
		if(mouse_pressed[2]){ //rotate z
			trans[2] += ry;
		}
		//std::cerr<<trans[0]<<" "<<trans[1]<<" "<<trans[2]<<std::endl;
	}else if(m_mode == NORMAL_MODE){
    	//need to capture the mouse motion
		//up,down,left,right

		double ry = (disc_y > 0) ? 0.1 : -0.1;
		double rx = (disc_x > 0) ? 0.1 : -0.1;
	
		look_at[0] += rx;	
		//look_at[1] -= ry;	
		//invalidate();
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

/*//start
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); //disable the color mask
    glDepthMask(GL_FALSE); //disable the depth mask

    glEnable(GL_STENCIL_TEST); //enable the stencil testing

    glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE); //set the stencil buffer to replace our next lot of data
	
	glTranslated(0,0,2);
	glBindTexture(GL_TEXTURE_2D, 0);
    m_water.draw(); //set the data plane to be replaced
	
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); //enablethe color mask
    glDepthMask(GL_TRUE); //enable the depth mask

    glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); //set the stencilbuffer to keep our next lot of data

    glDisable(GL_DEPTH_TEST); //disable depth testing of thereflection
    glPushMatrix();
    //glScalef(1.0f, 1.0f, 1.0f); //flip the reflection vertically
   	glTranslated(0,0,0); //translate the reflection onto thedrawing plane
    //glRotatef(60,0,1,0); //rotate the reflection
	//glBindTexture(GL_TEXTURE_2D, 0);
	float shineBuffer[1] = {0};
	float diffuseBuffer[4] = {0,0.5,0.9,0.9};
	
	glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, shineBuffer);
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseBuffer);	
	t_grass.apply_texture();
	m_map.draw();  //draw the square as our reflection
    glPopMatrix();
    glEnable(GL_DEPTH_TEST); //enable the depth testing

    glDisable(GL_STENCIL_TEST); //disable the stencil testing*/
/*
	glPushMatrix();
	test();
	glPopMatrix();*/
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
	
	*/
	//draw_skybox();
