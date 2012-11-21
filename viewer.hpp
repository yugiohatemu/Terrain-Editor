#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <gtkmm.h>
#include <gtkglmm.h>
#include "scene_lua.hpp"
#include <stack>
#include "hm.hpp"
#include "texture.hpp"

//hit name
//how much x , y has moved
struct joint_stack{
	int name, x, y;
	bool selected;
	joint_stack(){
		x = 0;
		y = 0;
		name = 0;
		selected = true;
	}

	joint_stack(int m_name, int m_x,int m_y, bool m_selected){
		name = m_name;
		x = m_x;
		y = m_y;
		selected = m_selected;
	}
	
	joint_stack(const joint_stack & j){
		name = j.name;
		x = j.x;
		y = j.y;
		selected = j.selected;
	}
};


// The "main" OpenGL widget
class Viewer : public Gtk::GL::DrawingArea {
public:
  Viewer();
  virtual ~Viewer();
	SceneNode* root ;
  // A useful function that forces this widget to rerender. If you
  // want to render a new frame, do not call on_expose_event
  // directly. Instead call this, which will cause an on_expose_event
  // call when the time is right.
  void invalidate();
  
	void reset_position();
	void reset_orientation();
	void reset_joint();
	void reset_all();	
	void undo();
	void redo();

	enum Edit_Mode{
		Edit_Pos,
		Edit_Joint,
	};

	void set_edit_mode(Edit_Mode m){ 
		m_mode = m;
	}
	
	enum Option{
		Draw_Circle = 0,
		Z_Buffer,
		Back_Face,
		Front_Face
	};
		
	void set_option(Option p){
		options[p] = !options[p];
		invalidate();
	}
	void vCalcRotVec(float fNewX, float fNewY,float fOldX, float fOldY,float fDiameter,
                 float *fVecX, float *fVecY, float *fVecZ);
	void vAxisRotMatrix(float fVecX, float fVecY, float fVecZ);
	void vPerformRotate(float fOldX, float fNewX, float fOldY, float fNewY) ;
	void vTranslate(float fTrans, char cAxis);
protected:

  // Called when GL is first initialized
  virtual void on_realize();
  // Called when our window needs to be redrawn
  virtual bool on_expose_event(GdkEventExpose* event);
  // Called when the window is resized
  virtual bool on_configure_event(GdkEventConfigure* event);
  // Called when a mouse button is pressed
  virtual bool on_button_press_event(GdkEventButton* event);
  // Called when a mouse button is released
  virtual bool on_button_release_event(GdkEventButton* event);
  // Called when the mouse moves
  virtual bool on_motion_notify_event(GdkEventMotion* event);

  // Draw a circle for the trackball, with OpenGL commands.
  // Assumes the context for the viewer is active.
  void draw_trackball_circle();
	void gl_select(int x, int y);
	void list_hits(GLint hits, GLuint *names);
	

	//Testing
	void test();
private:
	
	Edit_Mode m_mode;
	bool options[4];
	HM m_map;
	Texture marble;
	Texture sky;
	/*HM m_water;
	Texture reflection;
	Texture alpha;*/
	
	//redo and undo stack
	std::stack<joint_stack> undo_stack;
	std::stack<joint_stack> redo_stack;
};

#endif
