#include "first.hpp"
#include "CompGeo.hpp"
#include "NGons.hpp"
#include "drawgon.hpp"
#include "ConvexHull.hpp"
#include "SegmentIntersect.hpp"
#include "Unshare.hpp"
#include "Triangulation.hpp"
#include "Fortune.hpp"
#include "Trapezoid.hpp"
#include "ControlInterface.hpp"
#include "widgets.hpp"

using namespace std;

extern char glade_start		asm("_binary_rsc_glade_glade_start");
extern char glade_end		asm("_binary_rsc_glade_glade_end");
extern char style_start		asm("_binary_rsc_style_css_start");
extern char style_end		asm("_binary_rsc_style_css_end");

//using namespace std;

Glib::RefPtr<Gtk::CssProvider> GEXT::provider;
Glib::RefPtr<Gdk::Display> GEXT::display;
Glib::RefPtr<Gdk::Screen> GEXT::screen;
Glib::RefPtr<Gtk::Application> GEXT::the_app;
Glib::RefPtr<Gtk::Builder> GEXT::refBuilder;
Gtk::Window* GEXT::CMW = NULL;
pGEXT GEXT::pGEXTInstance = NULL;
//initialize these next statics first in main before getInst():
int GEXT::argc = 0;
char ** GEXT::argv = NULL;

glade_extract::glade_extract(void)
{
	the_app = Gtk::Application::create(argc, argv, "from.the.glade");
	//Load the GtkBuilder file and instantiate its widgets:
	refBuilder = Gtk::Builder::create();

	char * g_s = &glade_start, * g_e = &glade_end;
	gsize g_z = g_e - g_s;
	refBuilder->add_from_string(g_s, g_z);
	/*
	try
	{
		refBuilder->add_from_file("rsc/glade.glade");
	}
	catch(const Glib::FileError& ex)
	{
		cout << "FileError: " << ex.what() << endl;
		exit(EXIT_FAILURE);
	}
	catch(const Glib::MarkupError& ex)
	{
		cout << "MarkupError: " << ex.what() << endl;
		exit(EXIT_FAILURE);
	}
	catch(const Gtk::BuilderError& ex)
	{
		cout << "BuilderError: " << ex.what() << endl;
		exit(EXIT_FAILURE);
	}
	*/
	provider = Gtk::CssProvider::create();
	//Gtk::CssProvider * pProvider = provider.get();
	display = Gdk::Display::get_default();
	//Gdk::Display * pDisplay = display.get();
	screen = display->get_default_screen();
	Gtk::StyleContext::add_provider_for_screen(screen, provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	char * s_s = &style_start, * s_e = &style_end;
	//gsize s_z = s_e - s_s;
	string s_str(s_s, s_e);
	provider->load_from_data(s_str);
	//pProvider->load_from_path("resources/style.css");
	GI[MainWindow].classType = "GtkWindow";
	GI[MainWindow].id = "MainWindow";
	refBuilder->get_widget("MainWindow", GI[MainWindow].pW);
	mains["MainWindow"] = &GI[MainWindow];
	((Gtk::Window*)GI[MainWindow].pW)->signal_delete_event().connect(sigc::mem_fun<GdkEventAny *, bool>(*this, &glade_extract::on_MainWindow_delete_event));
	((Gtk::Window*)GI[MainWindow].pW)->signal_size_allocate().connect(sigc::mem_fun<Gtk::Allocation&, void>(*this, &glade_extract::on_MainWindow_size_allocate));
	CMW = ((Gtk::Window*)GI[MainWindow].pW);
	GI[FileChooseDlg].classType = "GtkFileChooserDialog";
	GI[FileChooseDlg].id = "FileChooseDlg";
	refBuilder->get_widget("FileChooseDlg", GI[FileChooseDlg].pW);
	mains["FileChooseDlg"] = &GI[FileChooseDlg];
	((Gtk::FileChooserDialog*)GI[FileChooseDlg].pW)->signal_delete_event().connect(sigc::mem_fun<GdkEventAny *, bool>(*this, &glade_extract::on_FileChooseDlg_delete_event));
	((Gtk::FileChooserDialog*)GI[FileChooseDlg].pW)->signal_size_allocate().connect(sigc::mem_fun<Gtk::Allocation&, void>(*this, &glade_extract::on_FileChooseDlg_size_allocate));
	GI[MsgBox].classType = "GtkMessageDialog";
	GI[MsgBox].id = "MsgBox";
	refBuilder->get_widget("MsgBox", GI[MsgBox].pW);
	mains["MsgBox"] = &GI[MsgBox];
	((Gtk::MessageDialog*)GI[MsgBox].pW)->signal_delete_event().connect(sigc::mem_fun<GdkEventAny *, bool>(*this, &glade_extract::on_MsgBox_delete_event));
	((Gtk::MessageDialog*)GI[MsgBox].pW)->signal_size_allocate().connect(sigc::mem_fun<Gtk::Allocation&, void>(*this, &glade_extract::on_MsgBox_size_allocate));
	GI[RegNGonDlg].classType = "GtkDialog";
	GI[RegNGonDlg].id = "RegNGonDlg";
	refBuilder->get_widget("RegNGonDlg", GI[RegNGonDlg].pW);
	mains["RegNGonDlg"] = &GI[RegNGonDlg];
	((Gtk::Dialog*)GI[RegNGonDlg].pW)->signal_delete_event().connect(sigc::mem_fun<GdkEventAny *, bool>(*this, &glade_extract::on_RegNGonDlg_delete_event));
	((Gtk::Dialog*)GI[RegNGonDlg].pW)->signal_size_allocate().connect(sigc::mem_fun<Gtk::Allocation&, void>(*this, &glade_extract::on_RegNGonDlg_size_allocate));
	GI[ShellMenu].classType = "GtkMenuBar";
	GI[ShellMenu].id = "ShellMenu";
	refBuilder->get_widget("ShellMenu", GI[ShellMenu].pW);
	styles["ShellMenu"] = &GI[ShellMenu];
	GI[MainContainer].classType = "GtkBox";
	GI[MainContainer].id = "MainContainer";
	refBuilder->get_widget("MainContainer", GI[MainContainer].pW);
	styles["MainContainer"] = &GI[MainContainer];
	GI[StatusBarContainer].classType = "GtkFixed";
	GI[StatusBarContainer].id = "StatusBarContainer";
	refBuilder->get_widget("StatusBarContainer", GI[StatusBarContainer].pW);
	styles["StatusBarContainer"] = &GI[StatusBarContainer];
	GI[FileChooseButtonBox].classType = "GtkButtonBox";
	GI[FileChooseButtonBox].id = "FileChooseButtonBox";
	refBuilder->get_widget("FileChooseButtonBox", GI[FileChooseButtonBox].pW);
	styles["FileChooseButtonBox"] = &GI[FileChooseButtonBox];
	GI[NGonButtonBox].classType = "GtkButtonBox";
	GI[NGonButtonBox].id = "NGonButtonBox";
	refBuilder->get_widget("NGonButtonBox", GI[NGonButtonBox].pW);
	styles["NGonButtonBox"] = &GI[NGonButtonBox];
	GI[MenuFileSub].classType = "GtkMenu";
	GI[MenuFileSub].id = "MenuFileSub";
	refBuilder->get_widget("MenuFileSub", GI[MenuFileSub].pW);
	containers["MenuFileSub"] = &GI[MenuFileSub];
	GI[MenuToolsSub].classType = "GtkMenu";
	GI[MenuToolsSub].id = "MenuToolsSub";
	refBuilder->get_widget("MenuToolsSub", GI[MenuToolsSub].pW);
	containers["MenuToolsSub"] = &GI[MenuToolsSub];
	GI[tools_drawnumberSub].classType = "GtkMenu";
	GI[tools_drawnumberSub].id = "tools_drawnumberSub";
	refBuilder->get_widget("tools_drawnumberSub", GI[tools_drawnumberSub].pW);
	containers["tools_drawnumberSub"] = &GI[tools_drawnumberSub];
	GI[MenuComputationalGeometrySub].classType = "GtkMenu";
	GI[MenuComputationalGeometrySub].id = "MenuComputationalGeometrySub";
	refBuilder->get_widget("MenuComputationalGeometrySub", GI[MenuComputationalGeometrySub].pW);
	containers["MenuComputationalGeometrySub"] = &GI[MenuComputationalGeometrySub];
	GI[compgeo_triangulationSub].classType = "GtkMenu";
	GI[compgeo_triangulationSub].id = "compgeo_triangulationSub";
	refBuilder->get_widget("compgeo_triangulationSub", GI[compgeo_triangulationSub].pW);
	containers["compgeo_triangulationSub"] = &GI[compgeo_triangulationSub];
	GI[MenuHelpSub].classType = "GtkMenu";
	GI[MenuHelpSub].id = "MenuHelpSub";
	refBuilder->get_widget("MenuHelpSub", GI[MenuHelpSub].pW);
	containers["MenuHelpSub"] = &GI[MenuHelpSub];
	GI[MainDialog].classType = "GtkLayout";
	GI[MainDialog].id = "MainDialog";
	refBuilder->get_widget("MainDialog", GI[MainDialog].pW);
	containers["MainDialog"] = &GI[MainDialog];
	GI[DisplayWindow].classType = "GtkLayout";
	GI[DisplayWindow].id = "DisplayWindow";
	refBuilder->get_widget("DisplayWindow", GI[DisplayWindow].pW);
	containers["DisplayWindow"] = &GI[DisplayWindow];
	GI[NGonVBox].classType = "GtkBox";
	GI[NGonVBox].id = "NGonVBox";
	refBuilder->get_widget("NGonVBox", GI[NGonVBox].pW);
	containers["NGonVBox"] = &GI[NGonVBox];
	GI[FileNew].classType = "GtkImageMenuItem";
	GI[FileNew].id = "FileNew";
	refBuilder->get_widget("FileNew", GI[FileNew].pW);
	controls["FileNew"] = &GI[FileNew];
	((Gtk::ImageMenuItem*)GI[FileNew].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_FileNew_activate));
	GI[File_Open].classType = "GtkImageMenuItem";
	GI[File_Open].id = "File_Open";
	refBuilder->get_widget("File_Open", GI[File_Open].pW);
	controls["File_Open"] = &GI[File_Open];
	((Gtk::ImageMenuItem*)GI[File_Open].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_File_Open_activate));
	GI[File_Save].classType = "GtkImageMenuItem";
	GI[File_Save].id = "File_Save";
	refBuilder->get_widget("File_Save", GI[File_Save].pW);
	controls["File_Save"] = &GI[File_Save];
	((Gtk::ImageMenuItem*)GI[File_Save].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_File_Save_activate));
	GI[File_Save_As].classType = "GtkImageMenuItem";
	GI[File_Save_As].id = "File_Save_As";
	refBuilder->get_widget("File_Save_As", GI[File_Save_As].pW);
	controls["File_Save_As"] = &GI[File_Save_As];
	((Gtk::ImageMenuItem*)GI[File_Save_As].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_File_Save_As_activate));
	GI[File_Quit].classType = "GtkImageMenuItem";
	GI[File_Quit].id = "File_Quit";
	refBuilder->get_widget("File_Quit", GI[File_Quit].pW);
	controls["File_Quit"] = &GI[File_Quit];
	((Gtk::ImageMenuItem*)GI[File_Quit].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_File_Quit_activate));
	GI[tools_test].classType = "GtkImageMenuItem";
	GI[tools_test].id = "tools_test";
	refBuilder->get_widget("tools_test", GI[tools_test].pW);
	controls["tools_test"] = &GI[tools_test];
	((Gtk::ImageMenuItem*)GI[tools_test].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_tools_test_activate));
	GI[tools_drawsmiley].classType = "GtkImageMenuItem";
	GI[tools_drawsmiley].id = "tools_drawsmiley";
	refBuilder->get_widget("tools_drawsmiley", GI[tools_drawsmiley].pW);
	controls["tools_drawsmiley"] = &GI[tools_drawsmiley];
	((Gtk::ImageMenuItem*)GI[tools_drawsmiley].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_tools_drawsmiley_activate));
	GI[drawnumber_1].classType = "GtkMenuItem";
	GI[drawnumber_1].id = "drawnumber_1";
	refBuilder->get_widget("drawnumber_1", GI[drawnumber_1].pW);
	controls["drawnumber_1"] = &GI[drawnumber_1];
	((Gtk::MenuItem*)GI[drawnumber_1].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_drawnumber_1_activate));
	GI[drawnumber_2].classType = "GtkMenuItem";
	GI[drawnumber_2].id = "drawnumber_2";
	refBuilder->get_widget("drawnumber_2", GI[drawnumber_2].pW);
	controls["drawnumber_2"] = &GI[drawnumber_2];
	((Gtk::MenuItem*)GI[drawnumber_2].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_drawnumber_2_activate));
	GI[drawnumber_3].classType = "GtkMenuItem";
	GI[drawnumber_3].id = "drawnumber_3";
	refBuilder->get_widget("drawnumber_3", GI[drawnumber_3].pW);
	controls["drawnumber_3"] = &GI[drawnumber_3];
	((Gtk::MenuItem*)GI[drawnumber_3].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_drawnumber_3_activate));
	GI[drawnumber_4].classType = "GtkMenuItem";
	GI[drawnumber_4].id = "drawnumber_4";
	refBuilder->get_widget("drawnumber_4", GI[drawnumber_4].pW);
	controls["drawnumber_4"] = &GI[drawnumber_4];
	((Gtk::MenuItem*)GI[drawnumber_4].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_drawnumber_4_activate));
	GI[drawnumber_5].classType = "GtkMenuItem";
	GI[drawnumber_5].id = "drawnumber_5";
	refBuilder->get_widget("drawnumber_5", GI[drawnumber_5].pW);
	controls["drawnumber_5"] = &GI[drawnumber_5];
	((Gtk::MenuItem*)GI[drawnumber_5].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_drawnumber_5_activate));
	GI[drawnumber_6].classType = "GtkMenuItem";
	GI[drawnumber_6].id = "drawnumber_6";
	refBuilder->get_widget("drawnumber_6", GI[drawnumber_6].pW);
	controls["drawnumber_6"] = &GI[drawnumber_6];
	((Gtk::MenuItem*)GI[drawnumber_6].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_drawnumber_6_activate));
	GI[drawnumber_7].classType = "GtkMenuItem";
	GI[drawnumber_7].id = "drawnumber_7";
	refBuilder->get_widget("drawnumber_7", GI[drawnumber_7].pW);
	controls["drawnumber_7"] = &GI[drawnumber_7];
	((Gtk::MenuItem*)GI[drawnumber_7].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_drawnumber_7_activate));
	GI[drawnumber_8].classType = "GtkMenuItem";
	GI[drawnumber_8].id = "drawnumber_8";
	refBuilder->get_widget("drawnumber_8", GI[drawnumber_8].pW);
	controls["drawnumber_8"] = &GI[drawnumber_8];
	((Gtk::MenuItem*)GI[drawnumber_8].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_drawnumber_8_activate));
	GI[drawnumber_9].classType = "GtkMenuItem";
	GI[drawnumber_9].id = "drawnumber_9";
	refBuilder->get_widget("drawnumber_9", GI[drawnumber_9].pW);
	controls["drawnumber_9"] = &GI[drawnumber_9];
	((Gtk::MenuItem*)GI[drawnumber_9].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_drawnumber_9_activate));
	GI[drawnumber_10].classType = "GtkMenuItem";
	GI[drawnumber_10].id = "drawnumber_10";
	refBuilder->get_widget("drawnumber_10", GI[drawnumber_10].pW);
	controls["drawnumber_10"] = &GI[drawnumber_10];
	((Gtk::MenuItem*)GI[drawnumber_10].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_drawnumber_10_activate));
	GI[drawnumber_11].classType = "GtkMenuItem";
	GI[drawnumber_11].id = "drawnumber_11";
	refBuilder->get_widget("drawnumber_11", GI[drawnumber_11].pW);
	controls["drawnumber_11"] = &GI[drawnumber_11];
	((Gtk::MenuItem*)GI[drawnumber_11].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_drawnumber_11_activate));
	GI[drawnumber_12].classType = "GtkMenuItem";
	GI[drawnumber_12].id = "drawnumber_12";
	refBuilder->get_widget("drawnumber_12", GI[drawnumber_12].pW);
	controls["drawnumber_12"] = &GI[drawnumber_12];
	((Gtk::MenuItem*)GI[drawnumber_12].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_drawnumber_12_activate));
	GI[tools_drawngon].classType = "GtkImageMenuItem";
	GI[tools_drawngon].id = "tools_drawngon";
	refBuilder->get_widget("tools_drawngon", GI[tools_drawngon].pW);
	controls["tools_drawngon"] = &GI[tools_drawngon];
	((Gtk::ImageMenuItem*)GI[tools_drawngon].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_tools_drawngon_activate));
	GI[tools_showpolygons].classType = "GtkImageMenuItem";
	GI[tools_showpolygons].id = "tools_showpolygons";
	refBuilder->get_widget("tools_showpolygons", GI[tools_showpolygons].pW);
	controls["tools_showpolygons"] = &GI[tools_showpolygons];
	((Gtk::ImageMenuItem*)GI[tools_showpolygons].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_tools_showpolygons_activate));
	GI[tools_clearpolygons].classType = "GtkImageMenuItem";
	GI[tools_clearpolygons].id = "tools_clearpolygons";
	refBuilder->get_widget("tools_clearpolygons", GI[tools_clearpolygons].pW);
	controls["tools_clearpolygons"] = &GI[tools_clearpolygons];
	((Gtk::ImageMenuItem*)GI[tools_clearpolygons].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_tools_clearpolygons_activate));
	GI[tools_drawdots].classType = "GtkImageMenuItem";
	GI[tools_drawdots].id = "tools_drawdots";
	refBuilder->get_widget("tools_drawdots", GI[tools_drawdots].pW);
	controls["tools_drawdots"] = &GI[tools_drawdots];
	((Gtk::ImageMenuItem*)GI[tools_drawdots].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_tools_drawdots_activate));
	GI[tools_connect].classType = "GtkImageMenuItem";
	GI[tools_connect].id = "tools_connect";
	refBuilder->get_widget("tools_connect", GI[tools_connect].pW);
	controls["tools_connect"] = &GI[tools_connect];
	((Gtk::ImageMenuItem*)GI[tools_connect].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_tools_connect_activate));
	GI[compgeo_hull].classType = "GtkImageMenuItem";
	GI[compgeo_hull].id = "compgeo_hull";
	refBuilder->get_widget("compgeo_hull", GI[compgeo_hull].pW);
	controls["compgeo_hull"] = &GI[compgeo_hull];
	((Gtk::ImageMenuItem*)GI[compgeo_hull].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_compgeo_hull_activate));
	GI[compgeo_segmentintersect].classType = "GtkImageMenuItem";
	GI[compgeo_segmentintersect].id = "compgeo_segmentintersect";
	refBuilder->get_widget("compgeo_segmentintersect", GI[compgeo_segmentintersect].pW);
	controls["compgeo_segmentintersect"] = &GI[compgeo_segmentintersect];
	((Gtk::ImageMenuItem*)GI[compgeo_segmentintersect].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_compgeo_segmentintersect_activate));
	GI[triangulation_ymonotone].classType = "GtkMenuItem";
	GI[triangulation_ymonotone].id = "triangulation_ymonotone";
	refBuilder->get_widget("triangulation_ymonotone", GI[triangulation_ymonotone].pW);
	controls["triangulation_ymonotone"] = &GI[triangulation_ymonotone];
	((Gtk::MenuItem*)GI[triangulation_ymonotone].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_triangulation_ymonotone_activate));
	GI[triangulation_triangles].classType = "GtkMenuItem";
	GI[triangulation_triangles].id = "triangulation_triangles";
	refBuilder->get_widget("triangulation_triangles", GI[triangulation_triangles].pW);
	controls["triangulation_triangles"] = &GI[triangulation_triangles];
	((Gtk::MenuItem*)GI[triangulation_triangles].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_triangulation_triangles_activate));
	GI[compgeo_clearsharededges].classType = "GtkImageMenuItem";
	GI[compgeo_clearsharededges].id = "compgeo_clearsharededges";
	refBuilder->get_widget("compgeo_clearsharededges", GI[compgeo_clearsharededges].pW);
	controls["compgeo_clearsharededges"] = &GI[compgeo_clearsharededges];
	((Gtk::ImageMenuItem*)GI[compgeo_clearsharededges].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_compgeo_clearsharededges_activate));
	GI[compgeo_showvoronoi].classType = "GtkImageMenuItem";
	GI[compgeo_showvoronoi].id = "compgeo_showvoronoi";
	refBuilder->get_widget("compgeo_showvoronoi", GI[compgeo_showvoronoi].pW);
	controls["compgeo_showvoronoi"] = &GI[compgeo_showvoronoi];
	((Gtk::ImageMenuItem*)GI[compgeo_showvoronoi].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_compgeo_showvoronoi_activate));
	GI[compgeo_animatevoronoi].classType = "GtkImageMenuItem";
	GI[compgeo_animatevoronoi].id = "compgeo_animatevoronoi";
	refBuilder->get_widget("compgeo_animatevoronoi", GI[compgeo_animatevoronoi].pW);
	controls["compgeo_animatevoronoi"] = &GI[compgeo_animatevoronoi];
	((Gtk::ImageMenuItem*)GI[compgeo_animatevoronoi].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_compgeo_animatevoronoi_activate));
	GI[compgeo_trapezoids].classType = "GtkImageMenuItem";
	GI[compgeo_trapezoids].id = "compgeo_trapezoids";
	refBuilder->get_widget("compgeo_trapezoids", GI[compgeo_trapezoids].pW);
	controls["compgeo_trapezoids"] = &GI[compgeo_trapezoids];
	((Gtk::ImageMenuItem*)GI[compgeo_trapezoids].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_compgeo_trapezoids_activate));
	GI[compgeo_enablemouse].classType = "GtkImageMenuItem";
	GI[compgeo_enablemouse].id = "compgeo_enablemouse";
	refBuilder->get_widget("compgeo_enablemouse", GI[compgeo_enablemouse].pW);
	controls["compgeo_enablemouse"] = &GI[compgeo_enablemouse];
	((Gtk::ImageMenuItem*)GI[compgeo_enablemouse].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_compgeo_enablemouse_activate));
	GI[help_about].classType = "GtkImageMenuItem";
	GI[help_about].id = "help_about";
	refBuilder->get_widget("help_about", GI[help_about].pW);
	controls["help_about"] = &GI[help_about];
	((Gtk::ImageMenuItem*)GI[help_about].pW)->signal_activate().connect(sigc::mem_fun(*this, &glade_extract::on_help_about_activate));
	GI[LoadButton].classType = "GtkButton";
	GI[LoadButton].id = "LoadButton";
	refBuilder->get_widget("LoadButton", GI[LoadButton].pW);
	controls["LoadButton"] = &GI[LoadButton];
	((Gtk::Button*)GI[LoadButton].pW)->signal_clicked().connect(sigc::mem_fun(*this, &glade_extract::on_LoadButton_clicked));
	GI[StoreButton].classType = "GtkButton";
	GI[StoreButton].id = "StoreButton";
	refBuilder->get_widget("StoreButton", GI[StoreButton].pW);
	controls["StoreButton"] = &GI[StoreButton];
	((Gtk::Button*)GI[StoreButton].pW)->signal_clicked().connect(sigc::mem_fun(*this, &glade_extract::on_StoreButton_clicked));
	GI[RemoveButton].classType = "GtkButton";
	GI[RemoveButton].id = "RemoveButton";
	refBuilder->get_widget("RemoveButton", GI[RemoveButton].pW);
	controls["RemoveButton"] = &GI[RemoveButton];
	((Gtk::Button*)GI[RemoveButton].pW)->signal_clicked().connect(sigc::mem_fun(*this, &glade_extract::on_RemoveButton_clicked));
	GI[Static0].classType = "GtkLabel";
	GI[Static0].id = "Static0";
	refBuilder->get_widget("Static0", GI[Static0].pW);
	controls["Static0"] = &GI[Static0];
	GI[Static1].classType = "GtkLabel";
	GI[Static1].id = "Static1";
	refBuilder->get_widget("Static1", GI[Static1].pW);
	controls["Static1"] = &GI[Static1];
	GI[Static2].classType = "GtkLabel";
	GI[Static2].id = "Static2";
	refBuilder->get_widget("Static2", GI[Static2].pW);
	controls["Static2"] = &GI[Static2];
	GI[Static3].classType = "GtkLabel";
	GI[Static3].id = "Static3";
	refBuilder->get_widget("Static3", GI[Static3].pW);
	controls["Static3"] = &GI[Static3];
	GI[Static4].classType = "GtkLabel";
	GI[Static4].id = "Static4";
	refBuilder->get_widget("Static4", GI[Static4].pW);
	controls["Static4"] = &GI[Static4];
	GI[ListBoxViewPort].classType = "GtkViewport";
	GI[ListBoxViewPort].id = "ListBoxViewPort";
	refBuilder->get_widget("ListBoxViewPort", GI[ListBoxViewPort].pW);
	containers["ListBoxViewPort"] = &GI[ListBoxViewPort];
	GI[ListBoxScrolled].classType = "GtkScrolledWindow";
	GI[ListBoxScrolled].id = "ListBoxScrolled";
	refBuilder->get_widget("ListBoxScrolled", GI[ListBoxScrolled].pW);
	containers["ListBoxScrolled"] = &GI[ListBoxScrolled];
	GI[VertexList].classType = "GtkListBox";
	GI[VertexList].id = "VertexList";
	refBuilder->get_widget("VertexList", GI[VertexList].pW);
	controls["VertexList"] = &GI[VertexList];
	((Gtk::ListBox*)GI[VertexList].pW)->signal_selected_rows_changed().connect(sigc::mem_fun(*this, &glade_extract::on_VertexList_selected_rows_changed));
	((Gtk::ListBox*)GI[VertexList].pW)->signal_row_selected().connect(sigc::mem_fun<Gtk::ListBoxRow*, void>(*this, &glade_extract::on_VertexList_row_selected));
	GI[Static5].classType = "GtkLabel";
	GI[Static5].id = "Static5";
	refBuilder->get_widget("Static5", GI[Static5].pW);
	controls["Static5"] = &GI[Static5];
	GI[Static6].classType = "GtkLabel";
	GI[Static6].id = "Static6";
	refBuilder->get_widget("Static6", GI[Static6].pW);
	controls["Static6"] = &GI[Static6];
	GI[Static7].classType = "GtkLabel";
	GI[Static7].id = "Static7";
	refBuilder->get_widget("Static7", GI[Static7].pW);
	controls["Static7"] = &GI[Static7];
	GI[Static8].classType = "GtkLabel";
	GI[Static8].id = "Static8";
	refBuilder->get_widget("Static8", GI[Static8].pW);
	controls["Static8"] = &GI[Static8];
	GI[MainSelectedNEdit].classType = "GtkEntry";
	GI[MainSelectedNEdit].id = "MainSelectedNEdit";
	refBuilder->get_widget("MainSelectedNEdit", GI[MainSelectedNEdit].pW);
	controls["MainSelectedNEdit"] = &GI[MainSelectedNEdit];
	((Gtk::Entry*)GI[MainSelectedNEdit].pW)->signal_changed().connect(sigc::mem_fun(*this, &glade_extract::on_MainSelectedNEdit_changed));
	GI[MainSelectedYEdit].classType = "GtkEntry";
	GI[MainSelectedYEdit].id = "MainSelectedYEdit";
	refBuilder->get_widget("MainSelectedYEdit", GI[MainSelectedYEdit].pW);
	controls["MainSelectedYEdit"] = &GI[MainSelectedYEdit];
	((Gtk::Entry*)GI[MainSelectedYEdit].pW)->signal_changed().connect(sigc::mem_fun(*this, &glade_extract::on_MainSelectedYEdit_changed));
	GI[MainSelectedXEdit].classType = "GtkEntry";
	GI[MainSelectedXEdit].id = "MainSelectedXEdit";
	refBuilder->get_widget("MainSelectedXEdit", GI[MainSelectedXEdit].pW);
	controls["MainSelectedXEdit"] = &GI[MainSelectedXEdit];
	((Gtk::Entry*)GI[MainSelectedXEdit].pW)->signal_changed().connect(sigc::mem_fun(*this, &glade_extract::on_MainSelectedXEdit_changed));
	GI[SelectedUpdateButton].classType = "GtkButton";
	GI[SelectedUpdateButton].id = "SelectedUpdateButton";
	refBuilder->get_widget("SelectedUpdateButton", GI[SelectedUpdateButton].pW);
	controls["SelectedUpdateButton"] = &GI[SelectedUpdateButton];
	((Gtk::Button*)GI[SelectedUpdateButton].pW)->signal_clicked().connect(sigc::mem_fun(*this, &glade_extract::on_SelectedUpdateButton_clicked));
	GI[SelectedDeleteButton].classType = "GtkButton";
	GI[SelectedDeleteButton].id = "SelectedDeleteButton";
	refBuilder->get_widget("SelectedDeleteButton", GI[SelectedDeleteButton].pW);
	controls["SelectedDeleteButton"] = &GI[SelectedDeleteButton];
	((Gtk::Button*)GI[SelectedDeleteButton].pW)->signal_clicked().connect(sigc::mem_fun(*this, &glade_extract::on_SelectedDeleteButton_clicked));
	GI[Static9].classType = "GtkLabel";
	GI[Static9].id = "Static9";
	refBuilder->get_widget("Static9", GI[Static9].pW);
	controls["Static9"] = &GI[Static9];
	GI[DataLoadButton].classType = "GtkButton";
	GI[DataLoadButton].id = "DataLoadButton";
	refBuilder->get_widget("DataLoadButton", GI[DataLoadButton].pW);
	controls["DataLoadButton"] = &GI[DataLoadButton];
	((Gtk::Button*)GI[DataLoadButton].pW)->signal_clicked().connect(sigc::mem_fun(*this, &glade_extract::on_DataLoadButton_clicked));
	GI[Static10].classType = "GtkLabel";
	GI[Static10].id = "Static10";
	refBuilder->get_widget("Static10", GI[Static10].pW);
	controls["Static10"] = &GI[Static10];
	GI[Static11].classType = "GtkLabel";
	GI[Static11].id = "Static11";
	refBuilder->get_widget("Static11", GI[Static11].pW);
	controls["Static11"] = &GI[Static11];
	GI[MainDataXEdit].classType = "GtkEntry";
	GI[MainDataXEdit].id = "MainDataXEdit";
	refBuilder->get_widget("MainDataXEdit", GI[MainDataXEdit].pW);
	controls["MainDataXEdit"] = &GI[MainDataXEdit];
	((Gtk::Entry*)GI[MainDataXEdit].pW)->signal_changed().connect(sigc::mem_fun(*this, &glade_extract::on_MainDataXEdit_changed));
	GI[MainDataYEdit].classType = "GtkEntry";
	GI[MainDataYEdit].id = "MainDataYEdit";
	refBuilder->get_widget("MainDataYEdit", GI[MainDataYEdit].pW);
	controls["MainDataYEdit"] = &GI[MainDataYEdit];
	((Gtk::Entry*)GI[MainDataYEdit].pW)->signal_changed().connect(sigc::mem_fun(*this, &glade_extract::on_MainDataYEdit_changed));
	GI[Static12].classType = "GtkLabel";
	GI[Static12].id = "Static12";
	refBuilder->get_widget("Static12", GI[Static12].pW);
	controls["Static12"] = &GI[Static12];
	GI[MainDataAngleEdit].classType = "GtkEntry";
	GI[MainDataAngleEdit].id = "MainDataAngleEdit";
	refBuilder->get_widget("MainDataAngleEdit", GI[MainDataAngleEdit].pW);
	controls["MainDataAngleEdit"] = &GI[MainDataAngleEdit];
	((Gtk::Entry*)GI[MainDataAngleEdit].pW)->signal_changed().connect(sigc::mem_fun(*this, &glade_extract::on_MainDataAngleEdit_changed));
	GI[DataInsertBeforeButton].classType = "GtkButton";
	GI[DataInsertBeforeButton].id = "DataInsertBeforeButton";
	refBuilder->get_widget("DataInsertBeforeButton", GI[DataInsertBeforeButton].pW);
	controls["DataInsertBeforeButton"] = &GI[DataInsertBeforeButton];
	((Gtk::Button*)GI[DataInsertBeforeButton].pW)->signal_clicked().connect(sigc::mem_fun(*this, &glade_extract::on_DataInsertBeforeButton_clicked));
	GI[DataInsertAfterButton].classType = "GtkButton";
	GI[DataInsertAfterButton].id = "DataInsertAfterButton";
	refBuilder->get_widget("DataInsertAfterButton", GI[DataInsertAfterButton].pW);
	controls["DataInsertAfterButton"] = &GI[DataInsertAfterButton];
	((Gtk::Button*)GI[DataInsertAfterButton].pW)->signal_clicked().connect(sigc::mem_fun(*this, &glade_extract::on_DataInsertAfterButton_clicked));
	GI[DataRotateButton].classType = "GtkButton";
	GI[DataRotateButton].id = "DataRotateButton";
	refBuilder->get_widget("DataRotateButton", GI[DataRotateButton].pW);
	controls["DataRotateButton"] = &GI[DataRotateButton];
	((Gtk::Button*)GI[DataRotateButton].pW)->signal_clicked().connect(sigc::mem_fun(*this, &glade_extract::on_DataRotateButton_clicked));
	GI[DataMoveButton].classType = "GtkButton";
	GI[DataMoveButton].id = "DataMoveButton";
	refBuilder->get_widget("DataMoveButton", GI[DataMoveButton].pW);
	controls["DataMoveButton"] = &GI[DataMoveButton];
	((Gtk::Button*)GI[DataMoveButton].pW)->signal_clicked().connect(sigc::mem_fun(*this, &glade_extract::on_DataMoveButton_clicked));
	GI[PolygonCombo].classType = "GtkComboBoxText";
	GI[PolygonCombo].id = "PolygonCombo";
	refBuilder->get_widget("PolygonCombo", GI[PolygonCombo].pW);
	controls["PolygonCombo"] = &GI[PolygonCombo];
	((Gtk::ComboBox*)GI[PolygonCombo].pW)->signal_changed().connect(sigc::mem_fun(*this, &glade_extract::on_PolygonCombo_changed));
	GI[SelectedClearButton].classType = "GtkButton";
	GI[SelectedClearButton].id = "SelectedClearButton";
	refBuilder->get_widget("SelectedClearButton", GI[SelectedClearButton].pW);
	controls["SelectedClearButton"] = &GI[SelectedClearButton];
	((Gtk::Button*)GI[SelectedClearButton].pW)->signal_clicked().connect(sigc::mem_fun(*this, &glade_extract::on_SelectedClearButton_clicked));
	GI[DrawArea].classType = "GtkDrawingArea";
	GI[DrawArea].id = "DrawArea";
	refBuilder->get_widget("DrawArea", GI[DrawArea].pW);
	controls["DrawArea"] = &GI[DrawArea];
	((Gtk::DrawingArea*)GI[DrawArea].pW)->signal_draw().connect(sigc::mem_fun<const Cairo::RefPtr<Cairo::Context>&, bool>(*this, &glade_extract::on_DrawArea_draw));
	((Gtk::DrawingArea*)GI[DrawArea].pW)->signal_motion_notify_event().connect(sigc::mem_fun<GdkEventMotion*, bool>(*this, &glade_extract::on_DrawArea_motion_notify_event));
	((Gtk::DrawingArea*)GI[DrawArea].pW)->signal_button_release_event().connect(sigc::mem_fun<GdkEventButton*, bool>(*this, &glade_extract::on_DrawArea_button_release_event));
	GI[StatusBar].classType = "GtkLabel";
	GI[StatusBar].id = "StatusBar";
	refBuilder->get_widget("StatusBar", GI[StatusBar].pW);
	controls["StatusBar"] = &GI[StatusBar];
	GI[FileChooseAcceptButton].classType = "GtkButton";
	GI[FileChooseAcceptButton].id = "FileChooseAcceptButton";
	refBuilder->get_widget("FileChooseAcceptButton", GI[FileChooseAcceptButton].pW);
	controls["FileChooseAcceptButton"] = &GI[FileChooseAcceptButton];
	((Gtk::Button*)GI[FileChooseAcceptButton].pW)->signal_clicked().connect(sigc::mem_fun(*this, &glade_extract::on_FileChooseAcceptButton_clicked));
	GI[FileChooseCancelButton].classType = "GtkButton";
	GI[FileChooseCancelButton].id = "FileChooseCancelButton";
	refBuilder->get_widget("FileChooseCancelButton", GI[FileChooseCancelButton].pW);
	controls["FileChooseCancelButton"] = &GI[FileChooseCancelButton];
	((Gtk::Button*)GI[FileChooseCancelButton].pW)->signal_clicked().connect(sigc::mem_fun(*this, &glade_extract::on_FileChooseCancelButton_clicked));
	GI[NGonOKButton].classType = "GtkButton";
	GI[NGonOKButton].id = "NGonOKButton";
	refBuilder->get_widget("NGonOKButton", GI[NGonOKButton].pW);
	controls["NGonOKButton"] = &GI[NGonOKButton];
	((Gtk::Button*)GI[NGonOKButton].pW)->signal_clicked().connect(sigc::mem_fun(*this, &glade_extract::on_NGonOKButton_clicked));
	GI[NGonCancelButton].classType = "GtkButton";
	GI[NGonCancelButton].id = "NGonCancelButton";
	refBuilder->get_widget("NGonCancelButton", GI[NGonCancelButton].pW);
	controls["NGonCancelButton"] = &GI[NGonCancelButton];
	((Gtk::Button*)GI[NGonCancelButton].pW)->signal_clicked().connect(sigc::mem_fun(*this, &glade_extract::on_NGonCancelButton_clicked));
	GI[NGonSidesLabel].classType = "GtkLabel";
	GI[NGonSidesLabel].id = "NGonSidesLabel";
	refBuilder->get_widget("NGonSidesLabel", GI[NGonSidesLabel].pW);
	controls["NGonSidesLabel"] = &GI[NGonSidesLabel];
	GI[NGonSidesEntry].classType = "GtkEntry";
	GI[NGonSidesEntry].id = "NGonSidesEntry";
	refBuilder->get_widget("NGonSidesEntry", GI[NGonSidesEntry].pW);
	controls["NGonSidesEntry"] = &GI[NGonSidesEntry];
	((Gtk::Entry*)GI[NGonSidesEntry].pW)->signal_changed().connect(sigc::mem_fun(*this, &glade_extract::on_NGonSidesEntry_changed));
	GI[NGonRadiusLabel].classType = "GtkLabel";
	GI[NGonRadiusLabel].id = "NGonRadiusLabel";
	refBuilder->get_widget("NGonRadiusLabel", GI[NGonRadiusLabel].pW);
	controls["NGonRadiusLabel"] = &GI[NGonRadiusLabel];
	GI[NGonRadiusEntry].classType = "GtkEntry";
	GI[NGonRadiusEntry].id = "NGonRadiusEntry";
	refBuilder->get_widget("NGonRadiusEntry", GI[NGonRadiusEntry].pW);
	controls["NGonRadiusEntry"] = &GI[NGonRadiusEntry];
	((Gtk::Entry*)GI[NGonRadiusEntry].pW)->signal_changed().connect(sigc::mem_fun(*this, &glade_extract::on_NGonRadiusEntry_changed));
	pCI = NULL;
	//the_app->run(*CMW);
}

glade_extract::~glade_extract(void)
{
	delete pCI;
}

pGEXT glade_extract::getInst(void)
{
	if(argv == NULL)
	{
		cout << "Error: intitialize GEXT::argc & GEXT::argv before calling getInst\n";
		exit(EXIT_FAILURE);
	}

	if(pGEXTInstance == NULL) pGEXTInstance = new glade_extract();
	return pGEXTInstance;
}
Gtk::Widget * glade_extract::get(const string & wName)
{
	map<string, glade_info *>::iterator mit = controls.find(wName);
	if (mit != controls.end()) return (*mit).second->getWidget();
	mit = containers.find(wName);
	if (mit != containers.end()) return (*mit).second->getWidget();
	mit = mains.find(wName);
	if (mit != mains.end()) return (*mit).second->getWidget();
	mit = styles.find(wName);
	if (mit != styles.end()) return (*mit).second->getWidget();
	return NULL;
}
bool glade_extract::on_MainWindow_delete_event(GdkEventAny* any_event)
{
	//cout << "MainWindow delete connected\n";
	return false;
}
void glade_extract::on_MainWindow_size_allocate(Gtk::Allocation & allocation)
{
	pCI->resize();
	////cout << "MainWindow size connected\n";
}
bool glade_extract::on_FileChooseDlg_delete_event(GdkEventAny* any_event)
{
	//cout << "FileChooseDlg delete connected\n";
	return false;
}
void glade_extract::on_FileChooseDlg_size_allocate(Gtk::Allocation & allocation)
{
	//cout << "FileChooseDlg size connected\n";
}
bool glade_extract::on_MsgBox_delete_event(GdkEventAny* any_event)
{
	//cout << "MsgBox delete connected\n";
	return false;
}
void glade_extract::on_MsgBox_size_allocate(Gtk::Allocation & allocation)
{
	//cout << "MsgBox size connected\n";
}
bool glade_extract::on_RegNGonDlg_delete_event(GdkEventAny* any_event)
{
	//cout << "RegNGonDlg delete connected\n";
	return false;
}
void glade_extract::on_RegNGonDlg_size_allocate(Gtk::Allocation & allocation)
{
	//cout << "RegNGonDlg size connected\n";
}
void glade_extract::on_FileNew_activate(void)
{
	//cout << "FileNew connected\n";
	pCI->HandleClearAllPolygons();
}
void glade_extract::on_File_Open_activate(void)
{
	//cout << "File_Open connected\n";
	pCI->HandleLoadPLY();
}
void glade_extract::on_File_Save_activate(void)
{
	//cout << "File_Save connected\n";
	pCI->HandleSave();
}
void glade_extract::on_File_Save_As_activate(void)
{
	//cout << "File_Save_As connected\n";
	pCI->HandleStoreButton();
}
void glade_extract::on_File_Quit_activate(void)
{
	//cout << "File_Quit connected\n";
	//gtk_main_quit();
	//Gtk::Main::quit();
	exit(EXIT_SUCCESS);
}
void glade_extract::on_tools_test_activate(void)
{
	//cout << "tools_test connected\n";
	//pCI->HandleScreenSave();
	//pCI->MsgBox("Testing", "1, 2, 3 ...", "The BIG test");
	//pCI->redraw(8);
	pCI->MsgBox("sizeof XYZ TPoint<double> BasicTPoint<double>", to_string(sizeof(CompGeo::XYZ)) + 
		" " + to_string(sizeof(CompGeo::TPoint<double>)) + " " + to_string(sizeof(CompGeo::BasicTPoint<double>)), "test");
}
void glade_extract::on_tools_drawsmiley_activate(void)
{
	pCI->redraw(5);
	//cout << "tools_drawsmiley connected\n";
}
void glade_extract::on_drawnumber_1_activate(void)
{
	//cout << "drawnumber_1 connected\n";
	pCI->HandleDrawNumber(1);
}
void glade_extract::on_drawnumber_2_activate(void)
{
	//cout << "drawnumber_2 connected\n";
	pCI->HandleDrawNumber(2);
}
void glade_extract::on_drawnumber_3_activate(void)
{
	//cout << "drawnumber_3 connected\n";
	pCI->HandleDrawNumber(3);
}
void glade_extract::on_drawnumber_4_activate(void)
{
	//cout << "drawnumber_4 connected\n";
	pCI->HandleDrawNumber(4);
}
void glade_extract::on_drawnumber_5_activate(void)
{
	//cout << "drawnumber_5 connected\n";
	pCI->HandleDrawNumber(5);
}
void glade_extract::on_drawnumber_6_activate(void)
{
	//cout << "drawnumber_6 connected\n";
	pCI->HandleDrawNumber(6);
}
void glade_extract::on_drawnumber_7_activate(void)
{
	//cout << "drawnumber_7 connected\n";
	pCI->HandleDrawNumber(7);
}
void glade_extract::on_drawnumber_8_activate(void)
{
	//cout << "drawnumber_8 connected\n";
	pCI->HandleDrawNumber(8);
}
void glade_extract::on_drawnumber_9_activate(void)
{
	//cout << "drawnumber_9 connected\n";
	pCI->HandleDrawNumber(9);
}
void glade_extract::on_drawnumber_10_activate(void)
{
	//cout << "drawnumber_10 connected\n";
	pCI->HandleDrawNumber(10);
}
void glade_extract::on_drawnumber_11_activate(void)
{
	//cout << "drawnumber_11 connected\n";
	pCI->HandleDrawNumber(11);
}
void glade_extract::on_drawnumber_12_activate(void)
{
	//cout << "drawnumber_12 connected\n";
	pCI->HandleDrawNumber(12);
}
void glade_extract::on_tools_drawngon_activate(void)
{
	//cout << "tools_drawngon connected\n";
	((Gtk::Dialog*)GI[RegNGonDlg].pW)->run();
	//pCI->redraw(1);
	//pCI->HandleRegularNGon(5, 100);
	//pCI->redraw(1);
}
void glade_extract::on_tools_showpolygons_activate(void)
{
	//cout << "tools_showpolygons connected\n";
	pCI->ResetPolygons();
}
void glade_extract::on_tools_clearpolygons_activate(void)
{
	//cout << "tools_clearpolygons connected\n";
	pCI->HandleClearAllPolygons();
}
void glade_extract::on_tools_drawdots_activate(void)
{
	pCI->mouseState = 1;
}
void glade_extract::on_tools_connect_activate(void)
{
	pCI->HandleDotConnect();
}
void glade_extract::on_compgeo_hull_activate(void)
{
	pCI->HandleConvexHull();
}
void glade_extract::on_compgeo_segmentintersect_activate(void)
{
	//cout << "compgeo_segmentintersect connected\n";
	pCI->HandleSegmentIntersect();
}
void glade_extract::on_triangulation_ymonotone_activate(void)
{
	//cout << "triangulation_ymonotone connected\n";
	pCI->HandleTriangulation(true);
}
void glade_extract::on_triangulation_triangles_activate(void)
{
	//cout << "triangulation_triangles connected\n";
	pCI->HandleTriangulation(false);
}
void glade_extract::on_compgeo_clearsharededges_activate(void)
{
	//cout << "compgeo_clearsharededges connected\n";
	pCI->HandleClearSharedEdges();
}
void glade_extract::on_compgeo_showvoronoi_activate(void)
{
	//cout << "compgeo_showvoronoi connected\n";
	pCI->HandleVoronoi();
}
void glade_extract::on_compgeo_animatevoronoi_activate(void)
{
	//cout << "compgeo_animatevoronoi connected\n";
	pCI->HandleVoronoiAnimationStart();
}
void glade_extract::on_compgeo_trapezoids_activate(void)
{
	//cout << "compgeo_trapezoids connected\n";
	pCI->HandleTrapezoids();
}
void glade_extract::on_compgeo_enablemouse_activate(void)
{
	//cout << "compgeo_enablemouse connected\n";
	pCI->TurnOnPointSelection();
}
void glade_extract::on_help_about_activate(void)
{
	//cout << "help_about connected\n";
	pCI->MsgBox("2D Algorithms using polygons and dots", "from the book 'Computational Geometry'", "About");

}
void glade_extract::on_LoadButton_clicked(void)
{
	//cout << "LoadButton connected\n";
	pCI->HandleLoadButton();
}
void glade_extract::on_StoreButton_clicked(void)
{
	//cout << "StoreButton connected\n";
	pCI->HandleStoreButton();
}
void glade_extract::on_RemoveButton_clicked(void)
{
	//cout << "RemoveButton connected\n";
	pCI->HandleRemoveButton();
}
void glade_extract::on_VertexList_selected_rows_changed(void)
{
	//cout << "VertexList connected\n";
	//pCI->HandleVertexList();
}
void glade_extract::on_VertexList_row_selected(Gtk::ListBoxRow* lbrow)
{
	//cout << "on_VertexList_row_selected\n";
	pCI->HandleVertexList(lbrow);
}
void glade_extract::on_MainSelectedNEdit_changed(void)
{
	//cout << "MainSelectedNEdit connected\n";
}
void glade_extract::on_MainSelectedYEdit_changed(void)
{
	//cout << "MainSelectedYEdit connected\n";
}
void glade_extract::on_MainSelectedXEdit_changed(void)
{
	//cout << "MainSelectedXEdit connected\n";
}
void glade_extract::on_SelectedUpdateButton_clicked(void)
{
	//cout << "SelectedUpdateButton connected\n";
	pCI->HandleUpdateButton();
}
void glade_extract::on_SelectedDeleteButton_clicked(void)
{
	//cout << "SelectedDeleteButton connected\n";
	pCI->HandleDeleteButton();
}
void glade_extract::on_DataLoadButton_clicked(void)
{
	//cout << "DataLoadButton connected\n";
	pCI->HandleSelLoadButton();
}
void glade_extract::on_MainDataXEdit_changed(void)
{
	//cout << "MainDataXEdit connected\n";
}
void glade_extract::on_MainDataYEdit_changed(void)
{
	//cout << "MainDataYEdit connected\n";
}
void glade_extract::on_MainDataAngleEdit_changed(void)
{
	//cout << "MainDataAngleEdit connected\n";
}
void glade_extract::on_DataInsertBeforeButton_clicked(void)
{
	//cout << "DataInsertBeforeButton connected\n";
	pCI->HandleBeforeButton();
}
void glade_extract::on_DataInsertAfterButton_clicked(void)
{
	//cout << "DataInsertAfterButton connected\n";
	pCI->HandleAfterButton();
}
void glade_extract::on_DataRotateButton_clicked(void)
{
	//cout << "DataRotateButton connected\n";
	pCI->HandleRotateButton();
}
void glade_extract::on_DataMoveButton_clicked(void)
{
	//cout << "DataMoveButton connected\n";
	pCI->HandleMoveButton();
}
void glade_extract::on_PolygonCombo_changed(void)
{
	//cout << "PolygonCombo connected\n";
	pCI->HandleComboBox();
}
void glade_extract::on_SelectedClearButton_clicked(void)
{
	//cout << "SelectedClearButton connected\n";
	pCI->HandleClearSelectionButton();
}
bool glade_extract::on_DrawArea_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	//cout << "Entered on_DrawArea_draw ... ";
	pCI->ppd->switchboard(cr);
	//cout << "DrawArea connected\n";
	return false;
}
bool glade_extract::on_DrawArea_motion_notify_event(GdkEventMotion* motion_event)
{
	////cout << "Entered on_DrawArea_motion_notify_event ... \n";
	pCI->HandleMouseCoordinates(motion_event);
	return false;
}
bool glade_extract::on_DrawArea_button_release_event(GdkEventButton* release_event)
{
	pCI->HandleMouseButtonRelease(release_event);
	return false;
}
void glade_extract::on_FileChooseAcceptButton_clicked(void)
{
	//cout << "FileChooseAcceptButton connected\n";
	//((Gtk::FileChooserDialog*)GI[FileChooseDlg].pW)->hide();
	pCI->HandleAcceptButton();
}
void glade_extract::on_FileChooseCancelButton_clicked(void)
{
	//cout << "FileChooseCancelButton connected\n";
	//((Gtk::FileChooserDialog*)GI[FileChooseDlg].pW)->hide();
}
void glade_extract::on_NGonOKButton_clicked(void)
{
	//cout << "NGonOKButton connected\n";
	string sTxt = ((Gtk::Entry*)GI[NGonSidesEntry].pW)->get_text(), 
		rTxt = ((Gtk::Entry*)GI[NGonRadiusEntry].pW)->get_text();
	int sides = stoi(sTxt);
	float radius = stof(rTxt);	
	((Gtk::Dialog*)GI[RegNGonDlg].pW)->hide();
	pCI->HandleRegularNGon(sides, radius);
}
void glade_extract::on_NGonCancelButton_clicked(void)
{
	//cout << "NGonCancelButton connected\n";
	((Gtk::Dialog*)GI[RegNGonDlg].pW)->hide();
}
void glade_extract::on_NGonSidesEntry_changed(void)
{
	//cout << "NGonSidesEntry connected\n";
}
void glade_extract::on_NGonRadiusEntry_changed(void)
{
	//cout << "NGonRadiusEntry connected\n";
}
