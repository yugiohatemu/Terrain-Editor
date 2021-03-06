#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <gtkmm.h>
#include <gtkglmm.h>
//#include "scene_lua.hpp"
#include <stack>
#include "hm.hpp"
#include "texture.hpp"
#include "particle.hpp"
#include "object.hpp"

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
	//SceneNode* root ;

  	void invalidate();
	bool call_invalidate();
  	void reset_position();
	void reset_orientation();
	void reset_joint();
	void reset_all();	
	void undo();
	void redo();

	enum Game_Mode{
		GOD_MODE,
		NORMAL_MODE,
		EDIT_MODE
	};

	void set_game_mode(Game_Mode m){ 
		m_mode = m;
	}
	
	enum Light_Mode{
		LIGHT_POSITION,
		LIGHT_REFLECTION,
		LIGHT_SHADOW,
	};
	
	enum Size{
		SIZE_S = 51,
		SIZE_M = 105,
		SIZE_L = 201
	};

	void set_light_mode(Light_Mode l){
		l_mode = l;
	}

	enum Add_Mode{
		ADD_SPHERE,
		ADD_CUBE
	};
	
	enum Environment{
		GRASS,
		SAND,
		SNOW
	};

	enum Water{
		SLOW = 5,
		MEDIUM = 10,
		FAST = 15,
	};

	enum Weather{
		W_NO,
		W_RAIN,
		W_SNOW
	};

	void set_add_mode(Light_Mode l){
		if(l_mode != l){
			l_mode = l;
		}else{		
			l_mode = LIGHT_POSITION;
		}
		
	}
	void set_size(Size s);
	void set_environment(Environment e);
	void set_water(Water w);
	void set_weather(Weather w);
	void get_key_input(char input);
	void set_light();
protected:

	virtual void on_realize();
	virtual bool on_expose_event(GdkEventExpose* event);
	virtual bool on_configure_event(GdkEventConfigure* event);
	virtual bool on_button_press_event(GdkEventButton* event);
	virtual bool on_button_release_event(GdkEventButton* event);
	virtual bool on_motion_notify_event(GdkEventMotion* event);

  // Draw a circle for the trackball, with OpenGL commands.
  // Assumes the context for the viewer is active.
	void gl_select(int x, int y);
	void list_hits(GLint hits, GLuint *names);
	
	//Testing
	void test();	
	void draw_skybox();
private:
	
	Game_Mode m_mode;
	Light_Mode l_mode;
	Add_Mode a_mode;
	Size t_size;
	Environment e_env;
	Water w_water;
	Weather w_weather;

	HM m_map;
	HM m_water;

	Texture t_grass;
	Texture t_sand;
	//Texture t_snow;	
	Texture t_water;
	Texture t_sky_top;
	Texture t_sky_left;
	Texture t_sky_right;
	Texture t_sky_front;
	Texture t_sky_back;
	//Texture t_sand;
	//Rain testing
	Particle rain;
	//Timer
	sigc::connection m_timer;
	//Add object testing
	Scene m_scene;	

	//redo and undo stack
	std::stack<joint_stack> undo_stack;
	std::stack<joint_stack> redo_stack;
};

#endif
