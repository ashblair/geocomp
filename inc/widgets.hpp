#pragma once
//#include "ControlInterface.h"
/*
#include <gtkmm.h>
#include <assert.h>
#include <iostream>
#include <std::string>
#include <map>

*/
//#include "stdafx.h"
//using namespace std;
/*
	usage: (in main start with:) GEXT::argc = argc; GEXT::argv = argv;
	from the glade file, an extract:
	MAINS:
		MainWindow, a GtkWindow
		FileChooseDlg, a GtkFileChooserDialog
		MsgBox, a GtkMessageDialog
		RegNGonDlg, a GtkDialog


	STYLES:
		ShellMenu, a GtkMenuBar
		MainContainer, a GtkBox
		StatusBarContainer, a GtkFixed
		FileChooseButtonBox, a GtkButtonBox
		NGonButtonBox, a GtkButtonBox


	CONTAINERS:
		MenuFileSub, a GtkMenu
		MenuToolsSub, a GtkMenu
		tools_drawnumberSub, a GtkMenu
		MenuComputationalGeometrySub, a GtkMenu
		compgeo_triangulationSub, a GtkMenu
		MenuHelpSub, a GtkMenu
		MainDialog, a GtkLayout
		DisplayWindow, a GtkLayout
		NGonVBox, a GtkBox
		ListBoxViewPort, a GtkViewport
		ListBoxScrolled, a GtkScrolledWindow


	CONTROLS:
		FileNew, a GtkImageMenuItem
		File_Open, a GtkImageMenuItem
		File_Save, a GtkImageMenuItem
		File_Save_As, a GtkImageMenuItem
		File_Quit, a GtkImageMenuItem
		tools_test, a GtkImageMenuItem
		tools_drawsmiley, a GtkImageMenuItem
		drawnumber_1, a GtkMenuItem
		drawnumber_2, a GtkMenuItem
		drawnumber_3, a GtkMenuItem
		drawnumber_4, a GtkMenuItem
		drawnumber_5, a GtkMenuItem
		drawnumber_6, a GtkMenuItem
		drawnumber_7, a GtkMenuItem
		drawnumber_8, a GtkMenuItem
		drawnumber_9, a GtkMenuItem
		drawnumber_10, a GtkMenuItem
		drawnumber_11, a GtkMenuItem
		drawnumber_12, a GtkMenuItem
		tools_drawngon, a GtkImageMenuItem
		tools_showpolygons, a GtkImageMenuItem
		tools_clearpolygons, a GtkImageMenuItem
		tools_drawdots, a GtkImageMenuItem
		tools_connect, a GtkImageMenuItem
		compgeo_segmentintersect, a GtkImageMenuItem
		triangulation_ymonotone, a GtkMenuItem
		triangulation_triangles, a GtkMenuItem
		compgeo_clearsharededges, a GtkImageMenuItem
		compgeo_showvoronoi, a GtkImageMenuItem
		compgeo_animatevoronoi, a GtkImageMenuItem
		compgeo_trapezoids, a GtkImageMenuItem
		compgeo_enablemouse, a GtkImageMenuItem
		compgeo_hull, aGtkImageMenuItem
		help_about, a GtkImageMenuItem
		LoadButton, a GtkButton
		StoreButton, a GtkButton
		RemoveButton, a GtkButton
		Static0, a GtkLabel
		Static1, a GtkLabel
		Static2, a GtkLabel
		Static3, a GtkLabel
		Static4, a GtkLabel
		VertexList, a GtkListBox
		Static5, a GtkLabel
		Static6, a GtkLabel
		Static7, a GtkLabel
		Static8, a GtkLabel
		MainSelectedNEdit, a GtkEntry
		MainSelectedYEdit, a GtkEntry
		MainSelectedXEdit, a GtkEntry
		SelectedUpdateButton, a GtkButton
		SelectedDeleteButton, a GtkButton
		Static9, a GtkLabel
		DataLoadButton, a GtkButton
		Static10, a GtkLabel
		Static11, a GtkLabel
		MainDataXEdit, a GtkEntry
		MainDataYEdit, a GtkEntry
		Static12, a GtkLabel
		MainDataAngleEdit, a GtkEntry
		DataInsertBeforeButton, a GtkButton
		DataInsertAfterButton, a GtkButton
		DataRotateButton, a GtkButton
		DataMoveButton, a GtkButton
		PolygonCombo, a GtkComboBox
		SelectedClearButton, a GtkButton
		DrawArea, a GtkDrawingArea
		StatusBar, a GtkLabel
		FileChooseAcceptButton, a GtkButton
		FileChooseCancelButton, a GtkButton
		NGonOKButton, a GtkButton
		NGonCancelButton, a GtkButton
		NGonSidesLabel, a GtkLabel
		NGonSidesEntry, a GtkEntry
		NGonRadiusLabel, a GtkLabel
		NGonRadiusEntry, a GtkEntry
*/
// forward declaration
typedef class ControlInterface * pControlInterface;

enum wdg_enum{MainWindow, FileChooseDlg, MsgBox, RegNGonDlg, ShellMenu, MainContainer, StatusBarContainer, FileChooseButtonBox, NGonButtonBox, MenuFileSub, MenuToolsSub, tools_drawnumberSub, MenuComputationalGeometrySub, compgeo_triangulationSub, MenuHelpSub, MainDialog, DisplayWindow, NGonVBox, FileNew, File_Open, File_Save, File_Save_As, File_Quit, tools_test, tools_drawsmiley, drawnumber_1, drawnumber_2, drawnumber_3, drawnumber_4, drawnumber_5, drawnumber_6, drawnumber_7, drawnumber_8, drawnumber_9, drawnumber_10, drawnumber_11, drawnumber_12, tools_drawngon, tools_showpolygons, tools_clearpolygons, tools_drawdots, tools_connect, compgeo_hull, compgeo_segmentintersect, triangulation_ymonotone, triangulation_triangles, compgeo_clearsharededges, compgeo_showvoronoi, compgeo_animatevoronoi, compgeo_trapezoids, compgeo_enablemouse, help_about, LoadButton, StoreButton, RemoveButton, Static0, Static1, Static2, Static3, Static4, VertexList, ListBoxViewPort, ListBoxScrolled, Static5, Static6, Static7, Static8, MainSelectedNEdit, MainSelectedYEdit, MainSelectedXEdit, SelectedUpdateButton, SelectedDeleteButton, Static9, DataLoadButton, Static10, Static11, MainDataXEdit, MainDataYEdit, Static12, MainDataAngleEdit, DataInsertBeforeButton, DataInsertAfterButton, DataRotateButton, DataMoveButton, PolygonCombo, SelectedClearButton, DrawArea, StatusBar, FileChooseAcceptButton, FileChooseCancelButton, NGonOKButton, NGonCancelButton, NGonSidesLabel, NGonSidesEntry, NGonRadiusLabel, NGonRadiusEntry};
class glade_info
{
public:
	std::string classType, id;
	Gtk::Widget * pW;

	glade_info(void):classType{}, id{}, pW{NULL}{}
	glade_info(const glade_info &a):classType{a.classType}, id{a.id}, pW{a.pW}{}
	~glade_info(void){}

	Gtk::Widget * getWidget(void) {return pW;}
	std::string getName() {return id;}
	std::string getClassType() {return classType;}
};
typedef class glade_extract GEXT;
typedef GEXT * pGEXT;

class glade_extract
{
	Gtk::Widget * get(const std::string &);
public:
	void get(const std::string & s, Gtk::Box*&gtk){gtk=dynamic_cast<Gtk::Box*>(get(s));}
	void get(const std::string & s, Gtk::Button*&gtk){gtk=dynamic_cast<Gtk::Button*>(get(s));}
	void get(const std::string & s, Gtk::ButtonBox*&gtk){gtk=dynamic_cast<Gtk::ButtonBox*>(get(s));}
	void get(const std::string & s, Gtk::ComboBox*&gtk){gtk=dynamic_cast<Gtk::ComboBox*>(get(s));}
	void get(const std::string & s, Gtk::Dialog*&gtk){gtk=dynamic_cast<Gtk::Dialog*>(get(s));}
	void get(const std::string & s, Gtk::DrawingArea*&gtk){gtk=dynamic_cast<Gtk::DrawingArea*>(get(s));}
	void get(const std::string & s, Gtk::Entry*&gtk){gtk=dynamic_cast<Gtk::Entry*>(get(s));}
	void get(const std::string & s, Gtk::Fixed*&gtk){gtk=dynamic_cast<Gtk::Fixed*>(get(s));}
	void get(const std::string & s, Gtk::FileChooserDialog*&gtk){gtk=dynamic_cast<Gtk::FileChooserDialog*>(get(s));}
	void get(const std::string & s, Gtk::ImageMenuItem*&gtk){gtk=dynamic_cast<Gtk::ImageMenuItem*>(get(s));}
	void get(const std::string & s, Gtk::Label*&gtk){gtk=dynamic_cast<Gtk::Label*>(get(s));}
	void get(const std::string & s, Gtk::Layout*&gtk){gtk=dynamic_cast<Gtk::Layout*>(get(s));}
	void get(const std::string & s, Gtk::ListBox*&gtk){gtk=dynamic_cast<Gtk::ListBox*>(get(s));}
	void get(const std::string & s, Gtk::MenuBar*&gtk){gtk=dynamic_cast<Gtk::MenuBar*>(get(s));}
	void get(const std::string & s, Gtk::MenuItem*&gtk){gtk=dynamic_cast<Gtk::MenuItem*>(get(s));}
	void get(const std::string & s, Gtk::MessageDialog*&gtk){gtk=dynamic_cast<Gtk::MessageDialog*>(get(s));}
	void get(const std::string & s, Gtk::ScrolledWindow*&gtk){gtk=dynamic_cast<Gtk::ScrolledWindow*>(get(s));}
	void get(const std::string & s, Gtk::Viewport*&gtk){gtk=dynamic_cast<Gtk::Viewport*>(get(s));}
	void get(const std::string & s, Gtk::Window*&gtk){gtk=dynamic_cast<Gtk::Window*>(get(s));}
	bool on_MainWindow_delete_event(GdkEventAny*);
	void on_MainWindow_size_allocate(Gtk::Allocation&);
	bool on_FileChooseDlg_delete_event(GdkEventAny*);
	void on_FileChooseDlg_size_allocate(Gtk::Allocation&);
	bool on_MsgBox_delete_event(GdkEventAny*);
	void on_MsgBox_size_allocate(Gtk::Allocation&);
	bool on_RegNGonDlg_delete_event(GdkEventAny*);
	void on_RegNGonDlg_size_allocate(Gtk::Allocation&);
	void on_FileNew_activate(void);
	void on_File_Open_activate(void);
	void on_File_Save_activate(void);
	void on_File_Save_As_activate(void);
	void on_File_Quit_activate(void);
	void on_tools_test_activate(void);
	void on_tools_drawsmiley_activate(void);
	void on_drawnumber_1_activate(void);
	void on_drawnumber_2_activate(void);
	void on_drawnumber_3_activate(void);
	void on_drawnumber_4_activate(void);
	void on_drawnumber_5_activate(void);
	void on_drawnumber_6_activate(void);
	void on_drawnumber_7_activate(void);
	void on_drawnumber_8_activate(void);
	void on_drawnumber_9_activate(void);
	void on_drawnumber_10_activate(void);
	void on_drawnumber_11_activate(void);
	void on_drawnumber_12_activate(void);
	void on_tools_drawngon_activate(void);
	void on_tools_showpolygons_activate(void);
	void on_tools_clearpolygons_activate(void);
	void on_tools_drawdots_activate(void);
	void on_tools_connect_activate(void);
	void on_compgeo_hull_activate(void);
	void on_compgeo_segmentintersect_activate(void);
	void on_triangulation_ymonotone_activate(void);
	void on_triangulation_triangles_activate(void);
	void on_compgeo_clearsharededges_activate(void);
	void on_compgeo_showvoronoi_activate(void);
	void on_compgeo_animatevoronoi_activate(void);
	void on_compgeo_trapezoids_activate(void);
	void on_compgeo_enablemouse_activate(void);
	void on_help_about_activate(void);
	void on_LoadButton_clicked(void);
	void on_StoreButton_clicked(void);
	void on_RemoveButton_clicked(void);
	void on_VertexList_selected_rows_changed(void);
	void on_VertexList_row_selected(Gtk::ListBoxRow*);
	void on_MainSelectedNEdit_changed(void);
	void on_MainSelectedYEdit_changed(void);
	void on_MainSelectedXEdit_changed(void);
	void on_SelectedUpdateButton_clicked(void);
	void on_SelectedDeleteButton_clicked(void);
	void on_DataLoadButton_clicked(void);
	void on_MainDataXEdit_changed(void);
	void on_MainDataYEdit_changed(void);
	void on_MainDataAngleEdit_changed(void);
	void on_DataInsertBeforeButton_clicked(void);
	void on_DataInsertAfterButton_clicked(void);
	void on_DataRotateButton_clicked(void);
	void on_DataMoveButton_clicked(void);
	void on_PolygonCombo_changed(void);
	void on_SelectedClearButton_clicked(void);
	bool on_DrawArea_draw(const Cairo::RefPtr<Cairo::Context>&);
	bool on_DrawArea_motion_notify_event(GdkEventMotion*);
	bool on_DrawArea_button_release_event(GdkEventButton*);	
	void on_FileChooseAcceptButton_clicked(void);
	void on_FileChooseCancelButton_clicked(void);
	void on_NGonOKButton_clicked(void);
	void on_NGonCancelButton_clicked(void);
	void on_NGonSidesEntry_changed(void);
	void on_NGonRadiusEntry_changed(void);
	static Gtk::Window* CMW;
	static int argc;
	static char ** argv;
	static Glib::RefPtr<Gtk::Application> the_app;
	static Glib::RefPtr<Gtk::Builder> refBuilder;
	static Glib::RefPtr<Gtk::CssProvider> provider;
	static Glib::RefPtr<Gdk::Display> display;
	static Glib::RefPtr<Gdk::Screen> screen;
	static pGEXT getInst(void);
	std::map<std::string, glade_info *> mains, styles, containers, controls;
	pControlInterface pCI;
	~glade_extract(void);
private:
	glade_info GI[96];
	glade_extract(void);
	static pGEXT pGEXTInstance;
};
