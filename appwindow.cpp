#include "appwindow.hpp"

AppWindow::AppWindow()
{
  set_title("Advanced Ergonomics Laboratory");

  // A utility class for constructing things that go into menus, which
  // we'll set up next.
  using Gtk::Menu_Helpers::MenuElem;
  using Gtk::Menu_Helpers::RadioMenuElem;
	using Gtk::Menu_Helpers::CheckMenuElem;
  // Set up the application menu
  m_menu_app.items().push_back(MenuElem("_Reset Position", Gtk::AccelKey("i"),
    sigc::mem_fun(m_viewer, &Viewer::reset_position)));
	m_menu_app.items().push_back(MenuElem("_Reset Orientation", Gtk::AccelKey("o"),
    sigc::mem_fun(m_viewer, &Viewer::reset_orientation)));
	m_menu_app.items().push_back(MenuElem("_Reset Joints", Gtk::AccelKey("n"),
    sigc::mem_fun(m_viewer, &Viewer::reset_joint)));
	m_menu_app.items().push_back(MenuElem("_Reset All", Gtk::AccelKey("a"),
    sigc::mem_fun(m_viewer, &Viewer::reset_all)));
  m_menu_app.items().push_back(MenuElem("_Quit", Gtk::AccelKey("q"),
    sigc::mem_fun(*this, &AppWindow::hide)));

	// Set up the edit menu 
	sigc::slot1<void, Viewer::Edit_Mode> edit_slot = sigc::mem_fun(m_viewer, &Viewer::set_edit_mode);
	Gtk::RadioButtonGroup edit_group;
	m_menu_mode.items().push_back( RadioMenuElem(edit_group,"_Position/Orientation", Gtk::AccelKey("p"),sigc::bind( edit_slot, Viewer::Edit_Pos )));
	m_menu_mode.items().push_back( RadioMenuElem(edit_group,"_Joints", Gtk::AccelKey("j"),sigc::bind( edit_slot, Viewer::Edit_Joint )));

	//edit
	m_menu_edit.items().push_back( MenuElem("_Undo", Gtk::AccelKey("u"),sigc::mem_fun(m_viewer, &Viewer::undo)));
	m_menu_edit.items().push_back( MenuElem("_Redo", Gtk::AccelKey("r"), sigc::mem_fun(m_viewer, &Viewer::redo)));
	
	// Set up option menu
	sigc::slot1<void, Viewer::Option> option_slot = sigc::mem_fun(m_viewer, &Viewer::set_option);	
	//Gtk::CheckButtonGroup option_group;
	m_menu_options.items().push_back(CheckMenuElem("_Circle", Gtk::AccelKey("c"),
    sigc::bind(option_slot, Viewer::Draw_Circle )));	
	m_menu_options.items().push_back(CheckMenuElem("_Z-buffer", Gtk::AccelKey("z"),
    sigc::bind(option_slot, Viewer::Z_Buffer )));	
	m_menu_options.items().push_back(CheckMenuElem("_Backface cull", Gtk::AccelKey("b"),
    sigc::bind(option_slot, Viewer::Back_Face )));	
	m_menu_options.items().push_back(CheckMenuElem("_Fronface cull", Gtk::AccelKey("f"),
    sigc::bind(option_slot, Viewer::Front_Face )));	

  // Set up the menu bar
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Application", m_menu_app));
  m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Mode", m_menu_mode));
	m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Edit", m_menu_edit));
	m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Options", m_menu_options));
  // Pack in our widgets
  
  // First add the vertical box as our single "top" widget
  add(m_vbox);

  // Put the menubar on the top, and make it as small as possible
  m_vbox.pack_start(m_menubar, Gtk::PACK_SHRINK);

  // Put the viewer below the menubar. pack_start "grows" the widget
  // by default, so it'll take up the rest of the window.
  m_viewer.set_size_request(500, 500);
  m_vbox.pack_start(m_viewer);

  show_all();
}
