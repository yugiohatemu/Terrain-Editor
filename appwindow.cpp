#include "appwindow.hpp"

AppWindow::AppWindow(){
	set_title("Advanced Ergonomics Laboratory");

	// A utility class for constructing things that go into menus, which
	// we'll set up next.
	using Gtk::Menu_Helpers::MenuElem;
	using Gtk::Menu_Helpers::RadioMenuElem;
	using Gtk::Menu_Helpers::CheckMenuElem;
	// Set up the application menu
	m_menu_app.items().push_back(MenuElem("_Random", Gtk::AccelKey("r"), sigc::mem_fun(m_viewer, &Viewer::reset_position)));
	m_menu_app.items().push_back(MenuElem("_Quit", Gtk::AccelKey("q"), sigc::mem_fun(*this, &AppWindow::hide)));

	// Set up the edit menu 
	sigc::slot1<void, Viewer::Game_Mode> game_slot = sigc::mem_fun(m_viewer, &Viewer::set_game_mode);
	Gtk::RadioButtonGroup game_group;
	m_menu_mode.items().push_back( RadioMenuElem(game_group,"_God mode", Gtk::AccelKey("g"),sigc::bind( game_slot, Viewer::GOD_MODE )));
	m_menu_mode.items().push_back( RadioMenuElem(game_group,"_Normal mode", Gtk::AccelKey("n"),sigc::bind( game_slot, Viewer::NORMAL_MODE )));
	//m_menu_mode.items().push_back( RadioMenuElem(game_group,"_Edit mode", Gtk::AccelKey("e"),sigc::bind( game_slot, Viewer::EDIT_MODE )));

	// Set up the light menu 
	sigc::slot1<void, Viewer::Light_Mode> light_slot = sigc::mem_fun(m_viewer, &Viewer::set_light_mode);	
	m_menu_light.items().push_back(CheckMenuElem("_Light position", Gtk::AccelKey("p"), sigc::bind(light_slot, Viewer::LIGHT_POSITION )));	
	m_menu_light.items().push_back(CheckMenuElem("_Water reflection", sigc::bind(light_slot, Viewer::LIGHT_REFLECTION )));
	m_menu_light.items().push_back(CheckMenuElem("_Water shadow", sigc::bind(light_slot, Viewer::LIGHT_SHADOW )));

	//Add models 
	//sigc::slot1<void, Viewer::Size> size_slot = sigc::mem_fun(m_viewer, &Viewer::set_size);
	//Gtk::RadioButtonGroup size_group;
	//m_menu_size.items().push_back( MenuElem("_Randomize", sigc::bind( size_slot, Viewer::SIZE_S)));
	//m_menu_size.items().push_back( RadioMenuElem(size_group,"_Medium 100x100",sigc::bind( size_slot, Viewer::SIZE_M)));
	//m_menu_size.items().push_back( RadioMenuElem(size_group,"_Large 200x200", sigc::bind( size_slot, Viewer::SIZE_L)));
	
	sigc::slot1<void, Viewer::Environment> environment_slot = sigc::mem_fun(m_viewer, &Viewer::set_environment);
	Gtk::RadioButtonGroup environment_group;
	m_menu_environment.items().push_back( RadioMenuElem(environment_group,"_Grass", sigc::bind( environment_slot, Viewer::GRASS)));
	m_menu_environment.items().push_back( RadioMenuElem(environment_group,"_Jungle", sigc::bind( environment_slot, Viewer::SAND)));
	m_menu_environment.items().push_back( RadioMenuElem(environment_group,"_Snow", sigc::bind( environment_slot, Viewer::SNOW)));
	
	sigc::slot1<void, Viewer::Water> water_slot = sigc::mem_fun(m_viewer, &Viewer::set_water);
	Gtk::RadioButtonGroup water_group;
	m_menu_water.items().push_back( RadioMenuElem(water_group,"_Slow", sigc::bind( water_slot, Viewer::SLOW)));
	m_menu_water.items().push_back( RadioMenuElem(water_group,"_Medium", sigc::bind( water_slot, Viewer::MEDIUM)));
	m_menu_water.items().push_back( RadioMenuElem(water_group,"_Fast", sigc::bind( water_slot, Viewer::FAST)));

	sigc::slot1<void, Viewer::Weather> weather_slot = sigc::mem_fun(m_viewer, &Viewer::set_weather);
	Gtk::RadioButtonGroup weather_group;
	m_menu_weather.items().push_back( RadioMenuElem(weather_group,"_No Weather", sigc::bind( weather_slot, Viewer::W_NO)));
	m_menu_weather.items().push_back( RadioMenuElem(weather_group,"_Rain", sigc::bind( weather_slot, Viewer::W_RAIN)));
	m_menu_weather.items().push_back( RadioMenuElem(weather_group,"_Snow", sigc::bind( weather_slot, Viewer::W_SNOW)));
	

m_menu_water;
	// Set up the menu bar
	m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Application", m_menu_app));
	m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Mode", m_menu_mode));
	m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Light", m_menu_light));
	//m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Terrain Size", m_menu_size));
	m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Environment", m_menu_environment));
	m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Water Speed", m_menu_water));
	m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Weather", m_menu_weather));	
	// Pack in our widgets
	add(m_vbox);
	m_vbox.pack_start(m_menubar, Gtk::PACK_SHRINK);
	m_viewer.set_size_request(500, 500);
	m_vbox.pack_start(m_viewer);

	show_all();
}

bool AppWindow::on_key_press_event( GdkEventKey *ev ){
	
	//If satisfying in the event list, just pass it to the view, otherwise, do not pass
	//in wasd and WASD
	
  	if(ev->keyval == GDK_w ||ev->keyval == GDK_W||ev->keyval == GDK_Up){
		m_viewer.get_key_input('w'); 
	}else if(ev->keyval == GDK_a ||ev->keyval == GDK_A||ev->keyval == GDK_Left){
		m_viewer.get_key_input('a');
		return true;
	}else if(ev->keyval == GDK_s ||ev->keyval == GDK_S||ev->keyval == GDK_Down){
		m_viewer.get_key_input('s'); 
		return true;
	}else if(ev->keyval == GDK_d ||ev->keyval == GDK_D||ev->keyval == GDK_Right){
		m_viewer.get_key_input('d'); 
		return true;
	}else {
		return Gtk::Window::on_key_press_event( ev );
  }
}
		
