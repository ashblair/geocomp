#pragma once
//#include "stdafx.h"

class glade_extract;
typedef class glade_extract GEXT;
typedef GEXT * pGEXT;

class ControlInterface
{
public:
	ControlInterface(void);
	~ControlInterface(void);

	void ResetControlInterface(pGEXT);
	void redraw(const int &, int colorIdx = 0, int startAt = 0);
	void resize(void);
	void output_allocation(const std::string &, const Gtk::Allocation &);
	void output_vertexInfo(void);
	void HandleDrawNumber(const int &);
	void MsgBox(const std::string &, const std::string & , const std::string &);
	void MakeControlsVisible(bool); 
	void UnloadListBox(void);
	void LoadListBox(void); 
	void UnloadComboBox(void);
	void LoadComboBox(void);

	void MakeButtonsVisible(void);

	void ClearEdits(void);


	// D2D1_POINT_2F is the selected vertex loads X and Y EDIT
	void LoadSelectedVertex(CompGeo::XY, int);

	// D2D1_POINT_2F is the selected vertex loads NEWX and NEWY EDIT
	void LoadNEWS(CompGeo::XY);

	void HandleVertexList(Gtk::ListBoxRow*);
	void HandleComboBox(void);
	void HandleLoadButton(void);
	void HandleLoadPLY(void);
	void HandleAcceptButton(void);
	void HandleStoreButton(void);
	void HandleSave(void);
	void HandleRemoveButton(void);
	void HandleClearSelectionButton(void);
	void HandleUpdateButton(void);
	void HandleDeleteButton(void);
	void HandleSelLoadButton(void);
	void HandleBeforeButton(void);
	void HandleAfterButton(void);
	void HandleRotateButton(void);
	void HandleMoveButton(void);
	void HandleRegularNGon(int, float);
	void ResetPolygons(void);
	void HandleConvexHull(void);
	void HandleSegmentIntersect(void);
	void HandleTriangulation(bool);
	void HandleClearAllPolygons(void);
	void HandleClearSharedEdges(void);
	void HandleVoronoi(void);
	void HandleVoronoiAnimation(void);
	void HandleVoronoiAnimationStart(void);
	void HandleVoronoiAnimationEnd(void);
	void HandleTrapezoids(void);
	void HandleScreenSave(void);
	void GeneratePoints(void);
	//void HandleMouse1(GdkEventButton*&); 
	//void HandleMouse2(GdkEventButton*&);
	void HandleDefaultInfo(void);
	void HandleMouseCoordinates(GdkEventMotion*);
	void HandleMouseButtonRelease(GdkEventButton*);
	bool on_timeout(int);
	bool on_sstimeout(int);
	void HandleDotConnect(void);
	void TurnOnPointSelection(void);
	void TurnOffPointSelection(void);
	void HandleComboBox(int);
	//void HandleMouseLButtonUp(void);
	void HandleVertexList(pVertexNode);
	//void AdjustForSize(void);
	//void AdjustStatusBar(void);
	//void HandleZoom(int);
	void Cleanup(void);
	//void InitializeZoomMenu(void);
	//void InitializeStatusWindow(void);
	//void InitializeScrollBars(void);
	//void Check4NewVertexPlaneExtents(void);
	//void Check4ChangedVertexPlaneExtents(void);
	//void HandleLoadPLY(void);
	//void HandleChangeDefaultView(const unsigned int &);
	//void HandleChangeAxes(HWND);
	std::string numstrTrim(const std::string &);
	int getInteger(const std::string &);
	double getDouble(const std::string &);


	// members:
	//Tree model columns:
	class CBModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:

		CBModelColumns()
		{ add(property_id_column); }

		Gtk::TreeModelColumn<Glib::ustring> property_id_column;
	};

	CBModelColumns CBColumns;

	//Child widgets:
	Glib::RefPtr<Gtk::ListStore> cb_refTreeModel;
	
	class LBRow:public Gtk::ListBoxRow
	{
	public:
		LBRow(void); //{LBCols = new LBColumns;}
		~LBRow(void);
		void add_m_all(void);

		class LBColumns:public Gtk::Box 
		{
		public:

			LBColumns(void);
			~LBColumns(void);

			Gtk::Label * NLabel;
			Gtk::Label * XLabel;
			Gtk::Label * YLabel;
		};

		unsigned int rIdx;
		LBColumns * LBCols;
		
	};

	std::vector<LBRow*> lbRows;
	//Child widgets:
	//Glib::RefPtr<Gtk::ListStore> lb_refTreeModel;

	bool buttonsVisible, holdCombo, holdList, timerOn;
	unsigned char fileOpSwitch;
	
	pGEXT pG;							
	Gtk::Window* mainWnd;
	Gtk::Layout* vdlg, * dispWnd;
	Gtk::Fixed * sdlg;
	Gtk::FileChooserDialog* fcDlg;
	Gtk::MessageDialog* msgDlg;
	Gtk::Dialog* ngonDlg;
	Gtk::DrawingArea * drawWnd;
	Gtk::MenuBar * menuBar;
	Gtk::Label * status;
	NGons * png;
	drawgon * ppd;
	Gtk::Allocation wArea, dArea, vArea, sArea, mArea,
		fcArea, msgArea, ngonArea;
	std::vector<CompGeo::XY> user_dots,
		voronoi_frame[3];
	
	sigc::connection conn, ss_conn;
	sigc::slot<bool> my_slot, ss_slot;
	int m_timer_number, ss_timer_number;
	Fortune::Voronoi V_noi;
	Fortune::pSitePoint current_point;
	int mouseState,  // 0=default, 1=dots, 2=voronoi animation, 3=select
		screenSave,  // 0=none, 1=count, 2=smiley, 3=voronoi animation
		ssIdx;		// screen save index used as a countdown
	bool pointSelection; //the mouse is enabled
	bool MousePostDraw; 
	int NGONSIDES;
	float NGONRADIUS;
	//TRACKMOUSEEVENT m_e;
	Trapezoid::Map<double> * tzoid_map;
	CompGeo::DCEL<double> * current_dcel;
	//unsigned int_PTR animation_timer_id; 
	//unsigned int animation_Interval;
	double animation_delta;
	int zoom_exp, * ZOOMS;
	//CIScrollStuff SS;
	//View * pView;
};