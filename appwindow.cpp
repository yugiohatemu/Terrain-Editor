#include "appwindow.hpp"

AppWindow::AppWindow(){
	set_title("Advanced Ergonomics Laboratory");

	// A utility class for constructing things that go into menus, which
	// we'll set up next.
	using Gtk::Menu_Helpers::MenuElem;
	using Gtk::Menu_Helpers::RadioMenuElem;
	using Gtk::Menu_Helpers::CheckMenuElem;
	// Set up the application menu
	m_menu_app.items().push_back(MenuElem("_Reset Not Done", Gtk::AccelKey("i"), sigc::mem_fun(m_viewer, &Viewer::reset_position)));
	m_menu_app.items().push_back(MenuElem("_Quit", Gtk::AccelKey("q"), sigc::mem_fun(*this, &AppWindow::hide)));

	// Set up the edit menu 
	sigc::slot1<void, Viewer::Game_Mode> game_slot = sigc::mem_fun(m_viewer, &Viewer::set_game_mode);
	Gtk::RadioButtonGroup game_group;
	m_menu_mode.items().push_back( RadioMenuElem(game_group,"_God mode", Gtk::AccelKey("g"),sigc::bind( game_slot, Viewer::GOD_MODE )));
	m_menu_mode.items().push_back( RadioMenuElem(game_group,"_Normal mode", Gtk::AccelKey("n"),sigc::bind( game_slot, Viewer::NORMAL_MODE )));

	//edit
	m_menu_edit.items().push_back( MenuElem("_Undo", Gtk::AccelKey("u"),sigc::mem_fun(m_viewer, &Viewer::undo)));
	m_menu_edit.items().push_back( MenuElem("_Redo", Gtk::AccelKey("r"), sigc::mem_fun(m_viewer, &Viewer::redo)));

	// Set up option menu
	sigc::slot1<void, Viewer::Option> option_slot = sigc::mem_fun(m_viewer, &Viewer::set_option);	
	//Gtk::CheckButtonGroup option_group;
	m_menu_options.items().push_back(CheckMenuElem("_Circle", Gtk::AccelKey("c"), sigc::bind(option_slot, Viewer::Draw_Circle )));	
	m_menu_options.items().push_back(CheckMenuElem("_Z-buffer", Gtk::AccelKey("z"), sigc::bind(option_slot, Viewer::Z_Buffer )));	
	m_menu_options.items().push_back(CheckMenuElem("_Backface cull", Gtk::AccelKey("b"), sigc::bind(option_slot, Viewer::Back_Face )));	
	m_menu_options.items().push_back(CheckMenuElem("_Fronface cull", Gtk::AccelKey("f"), sigc::bind(option_slot, Viewer::Front_Face )));	

	// Set up the menu bar
	m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Application", m_menu_app));
	m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Mode", m_menu_mode));
	m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Edit", m_menu_edit));
	m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Options", m_menu_options));

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
  	if(ev->keyval == GDK_w ||ev->keyval == GDK_W){
		m_viewer.get_key_input('w'); 
	}else if(ev->keyval == GDK_a ||ev->keyval == GDK_A){
		m_viewer.get_key_input('a');
		return true;
	}else if(ev->keyval == GDK_s ||ev->keyval == GDK_S){
		m_viewer.get_key_input('s'); 
		return true;
	}else if(ev->keyval == GDK_d ||ev->keyval == GDK_D){
		m_viewer.get_key_input('d'); 
		return true;
	}else {
		return Gtk::Window::on_key_press_event( ev );
  }
}
		
