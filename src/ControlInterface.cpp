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
#include "ViewInterface.hpp"
#include "Error.hpp"
#include "PLYHandler.hpp"
#include "widgets.hpp"
#include "ControlInterface.hpp"

using namespace std;

//#include <openacc.h>
//#include <cuda.h>
//#include <CL/opencl.h>
//#include <cuda.h>
// Forward declarations of functions included in this code module:
//BOOL CALLBACK			VisibilityProc(HWND hwnd, LPARAM lParam);			// to show or hide controls 

/*
string GetPlatformName(cl_platform_id cpi)
{
	char c[256];
	size_t rSz = 0;
	cl_int rVal = clGetPlatformInfo(cpi, CL_PLATFORM_NAME, 256, c, &rSz);
	string s = c;
	rVal = clGetPlatformInfo(cpi, CL_PLATFORM_VENDOR, 256, c, &rSz);
	s += "\t";
	s += c;
	return s;
}

string GetDeviceName(cl_device_id cdi)
{
	char c[256];
	size_t rSz = 0, w, t[10], *st = t;
	for (unsigned int i = 0; i < 10; ++i)
	{
		t[i] = 314;
	}
	cl_int rVal = clGetDeviceInfo(cdi, CL_DEVICE_NAME, 256, c, &rSz);
	string s = c;
	cl_uint u[10];
	cl_ulong l[10];
	rVal = clGetDeviceInfo(cdi, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &u[0], &rSz);
	s += "\t(w/ ";
	s += to_string(u[0]) + " Compute Units ";
	rVal = clGetDeviceInfo(cdi, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &u[1], &rSz);
	s += to_string(u[1]) + " Dimensions ";
	rVal = clGetDeviceInfo(cdi, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &w, &rSz);
	s += to_string(w) + " Work Group Size ";
	rVal = clGetDeviceInfo(cdi, CL_DEVICE_MAX_WORK_ITEM_SIZES, 10 * sizeof(size_t*), st, &rSz);
	s += "(";
	for (unsigned int i = 0; i < u[1]; ++i)
	{
		s += (string)" " + to_string(st[i]);

	}
	s += " ) Work Item Sizes ";
	rVal = clGetDeviceInfo(cdi, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &l[4], &rSz);
	s += to_string(l[4]) + " Global Mem Size ";
	rVal = clGetDeviceInfo(cdi, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &l[5], &rSz);
	s += to_string(l[5]) + " Local Mem Size)\n";

	return s;
}
//std::vector<Device> devices;
int find_devices(void) {
	// http://www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clGetPlatformIDs.html
	cl_uint platformIdCount = 0;
	clGetPlatformIDs(0, nullptr, &platformIdCount);

	if (platformIdCount == 0) {
		cerr << "No OpenCL platform found" << endl;
		return 1;
	}
	else {
		cout << "Found " << platformIdCount << " platform(s)" << endl;
	}

	vector<cl_platform_id> platformIds(platformIdCount);
	clGetPlatformIDs(platformIdCount, platformIds.data(), nullptr);

	for (cl_uint i = 0; i < platformIdCount; ++i) {
		cout << "\t (" << (i + 1) << ") : " << GetPlatformName(platformIds[i]) << endl;
	}

	// http://www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clGetDeviceIDs.html
	cl_uint deviceIdCount = 0;
	clGetDeviceIDs(platformIds[0], CL_DEVICE_TYPE_ALL, 0, nullptr, &deviceIdCount);

	if (deviceIdCount == 0) {
		cerr << "No OpenCL devices found" << endl;
		return 1;
	}
	else {
		cout << "Found " << deviceIdCount << " device(s)" << endl;
	}

	vector<cl_device_id> deviceIds(deviceIdCount);
	clGetDeviceIDs(platformIds[0], CL_DEVICE_TYPE_ALL, deviceIdCount,
		deviceIds.data(), nullptr);

	for (cl_uint i = 0; i < deviceIdCount; ++i) {
		cout << "\t (" << (i + 1) << ") : " << GetDeviceName(deviceIds[i]) << endl;
	}
	return 0;
}
*/
ControlInterface::LBRow::LBRow(void)
{
	LBCols = new LBColumns;
	rIdx = 0;
}

ControlInterface::LBRow::~LBRow(void)
{
	delete LBCols;
}

void ControlInterface::LBRow::add_m_all(void)
{
	LBCols->NLabel->set_width_chars(6);
	LBCols->pack_start(*LBCols->NLabel, Gtk::PACK_SHRINK);
	LBCols->XLabel->set_width_chars(12);
	LBCols->pack_start(*LBCols->XLabel, Gtk::PACK_SHRINK);
	LBCols->YLabel->set_width_chars(12);
	LBCols->pack_start(*LBCols->YLabel, Gtk::PACK_SHRINK);
	add(*LBCols);
}

ControlInterface::LBRow::LBColumns::LBColumns(void):Gtk::Box(Gtk::ORIENTATION_HORIZONTAL)
{
	//set_orientation(Gtk::ORIENTATION_HORIZONTAL); 
	//set_homogeneous(); 
	
	NLabel = NULL;
	XLabel = NULL;
	YLabel = NULL;
	//NLabel = new Gtk::Label; 
	//XLabel = new Gtk::Label; 
	//YLabel = new Gtk::Label;

}

ControlInterface::LBRow::LBColumns::~LBColumns(void)
{
	if (NLabel != NULL) delete NLabel;
	if (XLabel != NULL) delete XLabel;
	if (YLabel != NULL) delete YLabel;
}

ControlInterface::ControlInterface(void)
{
	buttonsVisible = false;
	holdCombo = false;
	holdList = false;
	timerOn = false;
	//hInst = NULL;

	//dlg = NULL;
	vdlg = NULL;
	sdlg = NULL;
	mainWnd = NULL;
	dispWnd = NULL;
	drawWnd = NULL;
	fcDlg = NULL;
	msgDlg = NULL;
	ngonDlg = NULL;
	pG = NULL;

	//toolWnd = NULL;
	png = NULL;
	ppd = NULL;
    srand(time(NULL)); // seeding the random number generator
	V_noi.bLine = &voronoi_frame[0];
	V_noi.LinePts = &voronoi_frame[1];
	V_noi.sPts = &voronoi_frame[2];
	tzoid_map = NULL;
	current_dcel = NULL;
	current_point = NULL;
	pointSelection = false;
	MousePostDraw = false;
	mouseState = 0;
	screenSave = 0;
	//animation_timer_id = 1023; 
	//animation_Interval = 100;
	animation_delta = 5;
	zoom_exp = 0;
	ZOOMS = new int[7];
	ZOOMS[0] = 1;
	for (int i = 1; i < 7; ++i) ZOOMS[i] = ZOOMS[i - 1] * 10; 
	//pView = NULL;
}

ControlInterface::~ControlInterface(void)
{
	UnloadComboBox();
	UnloadListBox();
	Cleanup();
	if (tzoid_map != NULL) delete tzoid_map;
	if (current_dcel != NULL) delete current_dcel;
	//if (pView != NULL) delete pView;
}

void ControlInterface::output_allocation(const string & descriptor, const Gtk::Allocation & allo)
{
	int x = allo.get_x(),
		y = allo.get_y(),
		w = allo.get_width(),
		h = allo.get_height();

	cout << descriptor << " X:" << x << ", Y:" << y << ", width:" << w << ", height:" << h << "\n";
}
// this will properly initialize the members of this class
void ControlInterface::ResetControlInterface(pGEXT p_g)
{
	pG = p_g;
	pG->get("MainWindow", mainWnd);
	//mainWnd->set_title("Computational Geometry");
	mainWnd->maximize();
	pG->get("FileChooseDlg", fcDlg);
	pG->get("MsgBox", msgDlg);
	pG->get("RegNGonDlg", ngonDlg);
	pG->get("MainDialog", vdlg);
	pG->get("DisplayWindow", dispWnd);
	pG->get("DrawArea", drawWnd);
	pG->get("StatusBarContainer", sdlg);
	pG->get("StatusBar", status);
	//pG->get("ShellMenu", menuBar);

	vArea = vdlg->get_allocation(); 
	sArea = sdlg->get_allocation();
	wArea = mainWnd->get_allocation();
	dArea = dispWnd->get_allocation();
	fcArea = fcDlg->get_allocation();
	msgArea = msgDlg->get_allocation();
	ngonArea = ngonDlg->get_allocation();

	//output_allocation("vertex dialog ", vArea);
	//output_allocation("status dialog ", sArea);
	//output_allocation("main window ", wArea);
	//output_allocation("display window ", dArea);
	//mArea = menuBar->get_allocation();
	
	png = new NGons;
	ppd = new drawgon;
	
	ppd->draw_win = drawWnd;
	ppd->png = png;
	ppd->dots = &user_dots;
	ppd->vFrame = voronoi_frame;

	buttonsVisible = false;
	MakeButtonsVisible();
}

void ControlInterface::redraw(const int & ctrlIndex, int colorIdx, int startAt)
{
	int c_i = ctrlIndex;
	ppd->startDrawAt = startAt;
	ppd->drawColorIndex = colorIdx;
	if (ctrlIndex > 3)
	{
		//buttonsVisible = false;
		//MakeButtonsVisible();
		MakeControlsVisible(false);
	}
	else
	{
		if ((c_i == 1) && (user_dots.size() > 0)) c_i = 3;
		if ((c_i == 2) && (png->selectedPolygon != NULL)) c_i = 3;
		MakeControlsVisible(true);
		buttonsVisible = png->selVertex != NULL;
		if (!buttonsVisible) MakeButtonsVisible();
	}
	
	ppd->drawCtrlIdx = c_i;
	drawWnd->queue_draw();
}

void ControlInterface::resize(void)
{
	wArea = mainWnd->get_allocation();
	dArea = dispWnd->get_allocation();
	vArea = vdlg->get_allocation(); 
	sArea = sdlg->get_allocation();

	//output_allocation("vertex dialog ", vArea);
	//output_allocation("status dialog ", sArea);
	//output_allocation("main window ", wArea);
	//output_allocation("display window ", dArea);
	//output_allocation("status label ", status->get_allocation());

	Gtk::Allocation d(0, 0, dArea.get_width(), dArea.get_height()); 
	drawWnd->size_allocate(d);

	//Gtk::Allocation s(0, 0, sArea.get_width(), sArea.get_height());
	status->size_allocate(sArea);

	//output_allocation("main window ", wArea);
	//output_allocation("display ", dArea);
}

void ControlInterface::HandleDrawNumber(const int & d_n)
{
	ppd->drawNumber = d_n;
	redraw(6);
}

void ControlInterface::MsgBox(const string & msg, const string & secondary = "", const string & title = "Message")
{
	msgDlg->set_message(msg);
	msgDlg->set_secondary_text(secondary);
	msgDlg->set_title(title);
	msgDlg->run();
	msgDlg->hide();
}


void ControlInterface::MakeControlsVisible(bool isVisible)
{
	//EnumChildWindows(dlg, VisibilityProc, (LPARAM)&isVisible);
	vector<Gtk::Widget*> children = vdlg->get_children();
	for (vector<Gtk::Widget*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->set_visible(isVisible);
	}
	//vdlg->show_all();
}


/*
BOOL CALLBACK VisibilityProc(HWND hCtl, LPARAM lParam)
{
	char clsName[20];
	char sysCls[9][20] = {"BUTTON", "COMBOBOX", "EDIT", "LISTBOX", "MDICLIENT", "RichEdit",
		"RICHEDIT_CLASS", "SCROLLBAR", "STATIC"};
	
	
	int c_in = GetClassNameA(hCtl, clsName, 20);
	for (int i = 0; i < 9; ++i)
	{
		if (0 == _stricmp(sysCls[i], clsName))
		{
			bool * pB = (bool *)lParam;
			if (*pB)
			{
				if (FALSE == IsWindowVisible(hCtl)) ShowWindow(hCtl, SW_SHOW);
			}
			else
			{
				if (TRUE == IsWindowVisible(hCtl)) ShowWindow(hCtl, SW_HIDE);
			}

		}
	}


	return TRUE;
}
*/
void ControlInterface::ClearEdits(void)
{
/*
		MainSelectedNEdit, a GtkEntry
		MainSelectedYEdit, a GtkEntry
		MainSelectedXEdit, a GtkEntry
		MainDataXEdit, a GtkEntry
		MainDataYEdit, a GtkEntry
		MainDataAngleEdit, a GtkEntry
*/
	Gtk::Entry * MSN, * MSX, * MSY, * MDX, * MDY, * MDA;
	
	pG->get("MainSelectedNEdit", MSN);
	pG->get("MainSelectedXEdit", MSX);
	pG->get("MainSelectedYEdit", MSY);
	//pG->get("MainDataXEdit", MDX);
	//pG->get("MainDataYEdit", MDY);
	//pG->get("MainDataAngleEdit", MDA);

	MSN->set_text("");
	MSX->set_text("");
	MSY->set_text("");
	//MDX->set_text("");
	//MDY->set_text("");
	//MDA->set_text("");

}

void ControlInterface::UnloadListBox(void)
{
	Gtk::ListBox * vList;
	pG->get("VertexList", vList);

	vector<Gtk::Widget*> lbrows = vList->get_children();
	for (vector<Gtk::Widget*>::iterator it = lbrows.begin(); it != lbrows.end(); ++it)
	{
		vList->remove(**it); // memory leak?
	}

	
	for (vector<LBRow*>::iterator it2 = lbRows.begin(); it2 != lbRows.end(); ++it2 )
	{
		delete *it2;
	}
	lbRows.clear();

}

void ControlInterface::LoadListBox(void)
{
	holdList = true;
	UnloadListBox();

	Gtk::ListBox * vList;
	pG->get("VertexList", vList);

	pPGonWork wGon = png->selectedPolygon;
	pVertexNode pVN = NULL, v_n = NULL;
	unsigned int i, SelIdx = 0;

	if (wGon == NULL) 
	{
		ClearEdits();
		buttonsVisible = false;
		MakeButtonsVisible();
		holdList = false;
		return;
	}
	pVN = wGon->vNode;
	bool hlite;
	int selVIdx = 0;
	string lTxt;
	for (i = 0; i < wGon->numVertices; ++i)
	{
		LBRow * lbr = new LBRow;
		
		unsigned int j = i + 1;
		lTxt = to_string(j);
		
		lbr->LBCols->NLabel = new Gtk::Label(lTxt.c_str(), Gtk::ALIGN_END);
		lTxt = to_string(pVN->vtxInfo.vertex.x);
		lbr->LBCols->XLabel = new Gtk::Label(lTxt.c_str(), Gtk::ALIGN_END);
		lTxt = to_string(pVN->vtxInfo.vertex.y);
		lbr->LBCols->YLabel = new Gtk::Label(lTxt.c_str(), Gtk::ALIGN_END);
		lbr->rIdx = i;
		lbr->add_m_all();
		lbRows.push_back(lbr);
		//vList->insert(*lbr, -1);
		vList->add(*lbr);
		pVN->vIdx = i;  // this can by bypassed
		if (pVN->vtxInfo.is_selected)
		{
			//vList->show_all();
			vList->select_row(*lbr);
			if (v_n == NULL) 
			{
				v_n = pVN;
				SelIdx = i;
			}
		}
		pVN = pVN->next;
	}
	
	vList->show_all();

	if (v_n == NULL) 
	{
		holdList = false;
		return;
	}
	buttonsVisible = true;
	MakeButtonsVisible();
	LoadSelectedVertex(v_n->vtxInfo.vertex, ++SelIdx);
	holdList = false;

}

void ControlInterface::UnloadComboBox(void)
{
	Gtk::ComboBox * cBox;
	pG->get("PolygonCombo", cBox);
	//cBox->unset_model();
	if (cb_refTreeModel) 
	{
		cBox->unset_active();
		cBox->clear();
		cBox->unset_model();
		cb_refTreeModel->clear();
	}
	else cb_refTreeModel = Gtk::ListStore::create(CBColumns);

	//cBox->remove();
	
	//if (CBColumns != NULL) delete CBColumns;
}

void ControlInterface::LoadComboBox(void)
{
	holdCombo = true;
	string addStr, topStr;
	unsigned int i, SelIdx = 0, numPGons = 0;
	pVertexNode v_n = NULL;
	

	if (png == NULL) return;

	pPGonWork sGon = png->selectedPolygon;
	HandleDefaultInfo();
	Gtk::ComboBox * cBox;
	pG->get("PolygonCombo", cBox);
	//cout << "lcbb4 "; output_vertexInfo();
	UnloadComboBox();
	//cout << "lcbaft "; output_vertexInfo();

	cBox->set_model(cb_refTreeModel);
	cBox->pack_start(CBColumns.property_id_column);

	pPGonWork wGon = NULL;
	if (png->polygons == NULL) 
	{
		LoadListBox();
		return;
	}
	numPGons = png->polygons->size();
	for (i = 0; i < numPGons; ++i)
	{
		wGon = png->polygons->at(i);
		if (sGon == wGon) SelIdx = i;
		Gtk::TreeModel::Row row = *(cb_refTreeModel->append());
		addStr = wGon->pName->polyname + "-" + to_string(wGon->pIdx);
		if (i == 0) topStr = addStr;
		row[CBColumns.property_id_column] = addStr;
		wGon->cIdx = i;
	}
	cBox->set_active(SelIdx); 
	//cout << "lcbfinal "; output_vertexInfo();
	cBox->show_all();
	if (sGon == NULL)
	{
		v_n = wGon->vNode;
		png->SelectNthPolygon(i);
	}
	LoadListBox();
	holdCombo = false;
}

void ControlInterface::MakeButtonsVisible()
{

	Gtk::Button* ctl;
	const char * bNames[] = {
		"SelectedUpdateButton", 
		"SelectedDeleteButton", 
		"DataLoadButton", 
		"DataInsertBeforeButton", 
		"DataInsertAfterButton", 
		"DataRotateButton", 
		"DataMoveButton",
		"SelectedClearButton"
		};


	int numButtons = sizeof(bNames) / sizeof(bNames[0]);
	for (int i = 0; i < numButtons; ++i)
	{
		pG->get(bNames[i], ctl);
		ctl->set_visible(buttonsVisible);
		//if (buttonsVisible) ctl->show_all();
		//else ctl->hide();
	}
	//vdlg->show_all();

}

	// CompGeo::XY sv is the selected vertex that loads X and Y EDIT SelIdx loads N EDIT
void ControlInterface::LoadSelectedVertex(CompGeo::XY sv, int SelIdx)
{
/*
		MainSelectedNEdit, a GtkEntry
		MainSelectedYEdit, a GtkEntry
		MainSelectedXEdit, a GtkEntry
*/
	stringstream addN, addX, addY;
	addN << setprecision(0) << setw(5);
	addX << setprecision(6) << setw(11);
	addY << setprecision(6) << setw(11);


	Gtk::Entry * MSN, * MSX, * MSY;
	
	pG->get("MainSelectedNEdit", MSN);
	pG->get("MainSelectedXEdit", MSX);
	pG->get("MainSelectedYEdit", MSY);

	addN << to_string(SelIdx);
	addX << to_string(sv.x);
	addY << to_string(sv.y);

	string addNStr = addN.str(), addXStr = addX.str(), addYStr = addY.str();

	MSN->set_text(addNStr);
	MSX->set_text(addXStr);
	MSY->set_text(addYStr);


}

	// CompGeo::XY v is the vertex that loads NEWX and NEWY EDIT
void ControlInterface::LoadNEWS(CompGeo::XY v)
{
/*
		MainSelectedNEdit, a GtkEntry
		MainSelectedYEdit, a GtkEntry
		MainSelectedXEdit, a GtkEntry
		MainDataXEdit, a GtkEntry
		MainDataYEdit, a GtkEntry
		MainDataAngleEdit, a GtkEntry

	Gtk::Entry * MSN, * MSX, * MSY, * MDX, * MDY, * MDA;
	
	pG->get("MainSelectedNEdit", MSN);
	pG->get("MainSelectedXEdit", MSX);
	pG->get("MainSelectedYEdit", MSY);
	pG->get("MainDataXEdit", MDX);
	pG->get("MainDataYEdit", MDY);
	pG->get("MainDataAngleEdit", MDA);

	stringstream addN, addX, addY;
	addN << setprecision(0) << setw(10);
	addX << setprecision(6) << setw(11);
	addY << setprecision(6) << setw(11);
*/

	Gtk::Entry  * MDX, * MDY;
	
	pG->get("MainDataXEdit", MDX);
	pG->get("MainDataYEdit", MDY);

	stringstream addX, addY;
	addX << setprecision(6) << setw(11);
	addY << setprecision(6) << setw(11);

	addX << to_string(v.x);
	addY << to_string(v.y);

	MDX->set_text(addX.str().c_str());
	MDY->set_text(addY.str().c_str());

}

void ControlInterface::HandleVertexList(Gtk::ListBoxRow* lbr)
{
	/*
	if (holdList)
	{
		//cout << "HandleVertexList called with holdList TRUE \n";
		return;
	}
	if (lbr == NULL)
	{
		//cout << "HandleVertexList called with NULL\n";
		return;
	}
	*/
	if ((holdList) || (lbr == NULL)) return;
	holdList = true;
	//cout << "hvl1 "; output_vertexInfo();
	Gtk::ListBox * vList;
	pG->get("VertexList", vList);
	int lboxIndex = lbr->get_index();
	png->SelectNthVertex(lboxIndex);
	if (png->selVertex->vtxInfo.is_selected)
	{ // unselect
		png->selVertex->vtxInfo.is_selected = false;
		//vList->unselect_row(lbr);
	}
	else
	{
		png->selVertex->vtxInfo.is_selected = true;
		//vList->select_row(*lbr);
	}
	vList->unselect_all();
	unsigned int vMax = png->selectedPolygon->numVertices,
		fsIdx = 0;
	pVertexNode v_n = png->selectedPolygon->vNode;
	png->selVertex = NULL;
	vector<Gtk::Widget*> LBROWS = vList->get_children();
	for (unsigned int i = 0; i < vMax; ++i)
	{
		if (v_n->vtxInfo.is_selected)
		{
			if (png->selVertex == NULL) 
			{
				png->selVertex = v_n;
				fsIdx = i;
			}
			Gtk::ListBoxRow * LBR = dynamic_cast<Gtk::ListBoxRow*>(LBROWS.at(i));
			vList->select_row(*LBR);
		}
		v_n = v_n->next;
	}

	//unsigned int fsIdx = png->GetFirstSelectedVertex();
	ClearEdits();
	//buttonsVisible = false;
	if (png->selVertex != NULL)
	{
		//buttonsVisible = true;
		LoadSelectedVertex(png->selVertex->vtxInfo.vertex, fsIdx + 1);
	}
	//MakeButtonsVisible();
	redraw(1);

	/*
	vector<Gtk::ListBoxRow*> LBROWS = vList->get_selected_rows();
	
	int selCount = LBROWS.size();
	bool bVis = false;
	if (selCount == 0)
	{
		png->SetSelection(NULL, selCount);
		//ppd->DrawSelectPolygon(*png, dArea);
		redraw(1);
		ClearEdits();

	}
	else
	{
		bVis = true;
		int * selIndices = new int[selCount];
		for (int i = 0; i < selCount; ++i)
		{
			Gtk::ListBoxRow * lbR = LBROWS.at(i);
			Gtk::Box * box = ((Gtk::Box*)lbR->get_child());
			Gtk::Label * label = ((Gtk::Label*)box->get_children().at(0));
			selIndices[i] = stoi(label->get_text()) - 1;
		}
		png->SetSelection(selIndices, selCount);
		//ppd->DrawSelectPolygon(*png, dArea);
		redraw(1);
		int selIdx = selIndices[0];
		pVertexNode v_n = png->selectedPolygon->vNode;
		for (int index = 0; index < selIdx; ++index)
		{
			v_n = v_n->next;
		}

		LoadSelectedVertex(v_n->vtxInfo.vertex, ++selIdx);
		delete []selIndices;
	}
	if (bVis != buttonsVisible)
	{
		buttonsVisible = bVis;
		MakeButtonsVisible();
	}
	*/
	//cout << "hvl2 "; output_vertexInfo();
	holdList = false;

}

void ControlInterface::HandleVertexList(pVertexNode v_n)
{ // sets selection to true for vertex v_n only if v_n is not selected coming in
	if (v_n == NULL) return;
	if (v_n->s != NULL) return;
	if (holdList) return;
	holdList = true;
	png->selVertex = v_n;
	v_n->s = png->SetSelection(v_n, png->selectedPolygon);
	v_n->vtxInfo.is_selected = true;

	Gtk::ListBox * vList;
	pG->get("VertexList", vList);
	//HWND ctl = GetDlgItem(dlg, IDC_VERTEXLIST);
	//BOOL hlite = TRUE;
	int i = v_n->vIdx;
	//SendMessage(ctl, LB_SETSEL, (WPARAM)hlite, (LPARAM)i);
	vList->select_row(*vList->get_row_at_index(i));
	LoadSelectedVertex(v_n->vtxInfo.vertex, ++i);
	redraw(1);
	holdList = false;
	//buttonsVisible = true;
	//MakeButtonsVisible();
}

void ControlInterface::output_vertexInfo(void)
{
	if (png == NULL) 
	{
		cout << "png is NULL\n";
		return;
	}
	if (png->selectedPolygon == NULL)
	{
		cout << "selectedPolygon is NULL\n";
		return;
	}
	pPGonWork wGon = png->selectedPolygon;
	cout << wGon->pName->polyname << " num_Verts:" << to_string(wGon->numVertices) << " ";
	pVertexNode v = wGon->vNode;
	for (unsigned int i = 0; i < wGon->numVertices; ++i)
	{
		cout << to_string(i) << "-" << (v->vtxInfo.is_selected? "true": "false") << " ";
		v = v->next;
	}
	cout << "\n";

}

void ControlInterface::HandleComboBox(void)
{
	if (holdCombo)
	{
		//cout << "holdCombo true in HandleComboBox \n";
		return;
	}
	//cout << "hcb1 "; output_vertexInfo();
	Gtk::ComboBox * pCombo;
	pG->get("PolygonCombo", pCombo);
	int index = pCombo->get_active_row_number();
	int selIdx = png->SelectNthPolygon(index);
	LoadListBox();
	//cout << "hcb2 "; output_vertexInfo();
}

void ControlInterface::HandleComboBox(int cIdx)
{
	Gtk::ComboBox * pCombo;
	pG->get("PolygonCombo", pCombo);
	int index = pCombo->get_active_row_number();
	if (index != cIdx)
	{
		//l_r = SendMessage(ctl, CB_SETCURSEL, (WPARAM)cIdx, (LPARAM)0); 
		pCombo->set_active(cIdx);
		index = cIdx;
		int selIdx = png->SelectNthPolygon(index);
		LoadListBox();
	}
}
/*
void ControlInterface::HandleMouseLButtonUp(void)
{
	if (pointSelection)
	{
		pPGonWork wGon = current_point->w_Gon;
		pVertexNode v_n = current_point->v_Node;
		HandleComboBox(wGon->cIdx);
		//wGon->selVertex = v_n->vIdx;
		//png->selVertex = v_n;
		//v_n->vtxInfo.is_selected = true;
		HandleVertexList(v_n);
		ppd->SelectPointOn(current_point->GetPOINT(), dArea);
		CompGeo::XY xy = current_point->v_Node->vtxInfo.vertex;
		double cx = (double)(dArea.right - dArea.left) / 2.0, cy = (double)(dArea.bottom - dArea.top) / 2.0;
			//vd = (double)vArea.right;
		//int xFrm = GetSystemMetrics(SM_CXFRAME), yFrm = GetSystemMetrics(SM_CYFRAME);
		xy.x += cx; // + vd - (double)(wArea.left + xFrm);
		xy.y = cy - xy.y; // + (double)(dArea.top - wArea.top - yFrm);

		// this next should position the mouse cursor on the site point:
		Glib::RefPtr<Gdk::Seat> seat = GEXT::display->get_default_seat();
		
		Glib::RefPtr<const Gdk::Device> mouse0 = seat->get_pointer();
		const Gdk::Device*  mouse2 = mouse0.get();
		
		Gdk::Device * mouse = const_cast<Gdk::Device*>(mouse2);
		mouse->warp(GEXT::screen, xy.x, xy.y);
		
		//POINT pt;
		//pt.x = (int)xy.x;
		//pt.y = (int)xy.y;
		//ClientToScreen(dWnd, &pt);
		//BOOL r = SetCursorPos(pt.x, pt.y);
		
		//char txtBuff[200];
		//HRESULT b_p = StringCbPrintf(txtBuff, 200 * sizeof(char),
		//	TEXT("cursor set to (%8.3f, %8.3f), ctr (%8.3f, %8.3f), wArea left:%d & top:%d & dArea top:%d\n"), 
		//	xy.x, xy.y, cx, cy, wArea.left, wArea.top, dArea.top);
		//OutputDebugString(txtBuff);
		
	}
}
*/
void ControlInterface::HandleLoadButton(void)
{
	//amSaving = false;
	fileOpSwitch = 0;
	Gtk::Button * aBtn;
	pG->get("FileChooseAcceptButton", aBtn);
	aBtn->set_label("Open");

	fcDlg->set_action(Gtk::FILE_CHOOSER_ACTION_OPEN);
	fcDlg->set_title("Load a polygon file (*.gon)");
	//Glib::RefPtr<Gtk::FileFilter> ff0 = fcd->get_filter();
	Glib::RefPtr<Gtk::FileFilter> ff1 = Gtk::FileFilter::create();
	ff1->add_pattern("*.gon");
	//Gtk::FileFilter * ff2 = ff1.get();
	//Gtk::FileFilter * ff3 = const_cast<Gtk::FileFilter *>(ff2);
	//ff3->add_pattern("*.gon");
	//ff2 = const_cast<const Gtk::FileFilter * >(ff3);
	//ff1 = Glib::RefPtr<const Gtk::FileFilter>(ff2);
	fcDlg->set_filter(ff1);

	int fcd_response = fcDlg->run();	
	//fcDlg->hide();	
	//cout << to_string(fcd_response) << "\n";


}

void ControlInterface::HandleSave(void)
{
	pPGonWork wGon = png->selectedPolygon;
	if (wGon == NULL) return;
	if (png->polygons == NULL) return;
	unsigned int pNum = png->polygons->size();
	if (pNum == 0) return;
	if (wGon->pName->isfile)
	{
		string fName = wGon->pName->polyname;
		png->SavePolygonList(fName);
		wGon->pName->isdirty = false;
	}
	else
	{
		HandleStoreButton();
	}
	
}

void ControlInterface::HandleStoreButton(void)
{
	//amSaving = true;
	fileOpSwitch = 1;
	if (png->polygons == NULL) return;
	unsigned int pNum = png->polygons->size();
	if (pNum == 0) return;
	
	Gtk::Button * aBtn;
	pG->get("FileChooseAcceptButton", aBtn);
	aBtn->set_label("Save");
	
	fcDlg->set_action(Gtk::FILE_CHOOSER_ACTION_SAVE);
	fcDlg->set_title("Save a polygon file (*.gon)");
	//Glib::RefPtr<Gtk::FileFilter> ff0 = fcDlg->get_filter();
	Glib::RefPtr<Gtk::FileFilter> ff1 = Gtk::FileFilter::create();
	ff1->add_pattern("*.gon");
	fcDlg->set_filter(ff1);
	//const Gtk::FileFilter * ff2 = ff1.get();
	//Gtk::FileFilter * ff3 = const_cast<Gtk::FileFilter *>(ff2);
	//ff3->add_pattern("*.gon");
	//ff2 = const_cast<const Gtk::FileFilter * >(ff3);
	//ff1 = Glib::RefPtr<const Gtk::FileFilter>(ff2);
	//fcd->set_filter(ff1);
	string fileName = "Polygon" + to_string(time(NULL)) + ".gon";
	fcDlg->set_current_name(fileName.c_str());
	int fcd_response = fcDlg->run();
	//fcDlg->hide();	


}

void ControlInterface::HandleLoadPLY(void)
{
	fileOpSwitch = 2;
	Gtk::Button * aBtn;
	pG->get("FileChooseAcceptButton", aBtn);
	aBtn->set_label("Open");

	fcDlg->set_action(Gtk::FILE_CHOOSER_ACTION_OPEN);
	fcDlg->set_title("Load a PLY file (*.ply)");
	Glib::RefPtr<Gtk::FileFilter> ff1 = Gtk::FileFilter::create();
	ff1->add_pattern("*.ply");
	fcDlg->set_filter(ff1);

	int fcd_response = fcDlg->run();	
	//fcDlg->hide();	

}

void ControlInterface::HandleAcceptButton(void)
{
	fcDlg->hide();
	string fNme = fcDlg->get_filename();
	switch (fileOpSwitch)
	{
		case 0: png->LoadNGon(fNme);
				TurnOffPointSelection();
				LoadComboBox();
				redraw(1);
				break; 
		case 1: png->SavePolygonList(fNme);
				LoadComboBox();
				redraw(1);
				break;
		case 2: // call view w/ view file name & maybe ppd
				string statusTxt = "parsing graphics file " + fNme + " ... ";
				status->set_text(statusTxt.c_str());
				while(gtk_events_pending()) {gtk_main_iteration_do(false);}
				VID::pViewInt pView = new VID::ViewInt;
				pView->dcel0 = new CompGeo::DCEL<int>;
				PLYHandler plyH = PLYHandler(pView, fNme);
				plyH.LoadPLY();
				// to do: check error status w/ boolean plyH.plyErr.triggered
				if ((plyH.duplicateEdges > 0) || (plyH.unattachedEdges > 0))
				{
					string dMsg = "There are" + to_string(plyH.duplicateEdges) + " duplicate edges (more than 2 faces sharing).",
						uMsg = "There are " + to_string(plyH.unattachedEdges) + " unattached edges (only 1 face).";
					bool dups = plyH.duplicateEdges > 0,
						uns = plyH.unattachedEdges > 0,
						both = dups && uns,
						either = dups || uns;
					string msg = string(both? string(dMsg + "\n" + uMsg): dups? dMsg: uMsg);
					if (either) MsgBox("PROBLEMS WITH THE TOPOLOGY:\n" + msg, fNme, "WARNING - BAD FORM");

				}
				delete pView->dcel0;
				delete pView;
				status->set_text("");
				break;
	}
}

void ControlInterface::HandleRemoveButton(void)
{
	TurnOffPointSelection();
	png->RemovePolygon();
	//HWND ctl = GetDlgItem(dlg, IDC_POLYGONCOMBO);
	LoadComboBox();
	//ppd->DrawPolygons(*png, dArea);
	redraw(1);
	bool sbVis = ((png->selectedPolygon != NULL) && (png->selVertices != NULL));
	//if (sbVis) sbVis = (png->selVertices != NULL);
	if (sbVis)
	{
		if (!buttonsVisible)
		{
			buttonsVisible = true;
			MakeButtonsVisible();
		}
	}
	else
	{
		if (buttonsVisible)
		{
			buttonsVisible = false;
			MakeButtonsVisible();
		}
	}

}

void ControlInterface::HandleClearSelectionButton(void)
{
	//if (png->selVertices == NULL) return;

	Gtk::ListBox * vList;
	pG->get("VertexList", vList);
	//vector<Gtk::ListBoxRow*> lbRows = vList->get_selected_rows();
	vList->unselect_all();

	//HWND ctl = GetDlgItem(dlg, IDC_VERTEXLIST);
	//BOOL hlite = FALSE;
	//int index = -1;
	//LRESULT lr = SendMessage(ctl, LB_SETSEL, (WPARAM)hlite, (LPARAM)index); 

	png->RemoveSelection();
	//ppd->DrawSelectPolygon(*png, dArea);
	redraw(1);
	ClearEdits();
	//if (buttonsVisible)
	//{
	//	buttonsVisible = false;
	//	MakeButtonsVisible();
	//}

}

void ControlInterface::HandleUpdateButton(void)
{ // for a single selection


	CompGeo::XY v;
	Gtk::Entry * MSX, * MSY;
	
	pG->get("MainSelectedXEdit", MSX);
	pG->get("MainSelectedYEdit", MSY);

	string xStr = MSX->get_text(), 
		yStr = MSY->get_text();
	xStr = numstrTrim(xStr);
	yStr = numstrTrim(yStr);
	v.x = getDouble(xStr);
	v.y = getDouble(yStr);
	png->UpdateVertex(v);
	TurnOffPointSelection();																
	LoadListBox();

	// draw the polygons
	redraw(1);


}

void ControlInterface::HandleDeleteButton(void)
{
	int affected[2], v = png->selVertex->vIdx;

	png->DeleteVertex();
	TurnOffPointSelection();																
	LoadListBox();
	// draw polygon
	redraw(1);

	if (png->selVertex == NULL)
	{
		buttonsVisible = false;
		MakeButtonsVisible();
	}
}

void ControlInterface::HandleSelLoadButton(void)
{

	CompGeo::XY v;
	Gtk::Entry * MSX, * MSY;
	
	pG->get("MainSelectedXEdit", MSX);
	pG->get("MainSelectedYEdit", MSY);

	string xStr = numstrTrim(MSX->get_text()), yStr = numstrTrim(MSY->get_text());
	v.x = getDouble(xStr);
	v.y = getDouble(yStr);
	LoadNEWS(v);


}
void ControlInterface::HandleBeforeButton(void)
{
/*
		MainSelectedNEdit, a GtkEntry
		MainSelectedYEdit, a GtkEntry
		MainSelectedXEdit, a GtkEntry
		MainDataXEdit, a GtkEntry
		MainDataYEdit, a GtkEntry
		MainDataAngleEdit, a GtkEntry

	Gtk::Entry * MSN, * MSX, * MSY, * MDX, * MDY, * MDA;
	
	pG->get("MainSelectedNEdit", MSN);
	pG->get("MainSelectedXEdit", MSX);
	pG->get("MainSelectedYEdit", MSY);
	pG->get("MainDataXEdit", MDX);
	pG->get("MainDataYEdit", MDY);
	pG->get("MainDataAngleEdit", MDA);

	stringstream addN, addX, addY;
	addN << setprecision(0) << setw(10);
	addX << setprecision(6) << setw(11);
	addY << setprecision(6) << setw(11);
*/

	Gtk::Entry * MDX, * MDY;
	
	pG->get("MainDataXEdit", MDX);
	pG->get("MainDataYEdit", MDY);

	string xStr = numstrTrim(MDX->get_text()), yStr = numstrTrim(MDY->get_text());

	CompGeo::XY v;

	v.x = getDouble(xStr);
	v.y = getDouble(yStr);

	png->AddVertexBefore(v);
	TurnOffPointSelection();																
	LoadListBox();
	// draw polygon
	redraw(1);
}

void ControlInterface::HandleAfterButton(void)
{
	Gtk::Entry * MDX, * MDY;
	
	pG->get("MainDataXEdit", MDX);
	pG->get("MainDataYEdit", MDY);

	string xStr = numstrTrim(MDX->get_text()), yStr = numstrTrim(MDY->get_text());

	CompGeo::XY v;

	v.x = getDouble(xStr);
	v.y = getDouble(yStr);

	png->AddVertexAfter(v);
	LoadListBox();
	TurnOffPointSelection();																
	// draw polygon
	redraw(1);


}

void ControlInterface::HandleRotateButton(void)
{
	Gtk::Entry * MDX, * MDY, * MDA;
	
	pG->get("MainDataXEdit", MDX);
	pG->get("MainDataYEdit", MDY);
	pG->get("MainDataAngleEdit", MDA);

	string xStr = numstrTrim(MDX->get_text()), yStr = numstrTrim(MDY->get_text()), aStr = numstrTrim(MDA->get_text());

	//if (xStr.length() == 0) xStr = "0";
	//if (yStr.length() == 0) yStr = "0";
	//if (aStr.length() == 0) aStr = "0";

	CompGeo::XY v;
	v.x = getDouble(xStr);
	v.y = getDouble(yStr);

	double angle = getDouble(aStr);

	png->RotateSelected(v, angle);
	TurnOffPointSelection();																
	LoadListBox();
	// draw polygon
	redraw(1);

}

void ControlInterface::HandleMoveButton(void)
{
	Gtk::Entry * MDX, * MDY;
	
	pG->get("MainDataXEdit", MDX);
	pG->get("MainDataYEdit", MDY);

	string xStr = numstrTrim(MDX->get_text()), yStr = numstrTrim(MDY->get_text());

	CompGeo::XY v;

	v.x = getDouble(xStr);
	v.y = getDouble(yStr);

	png->MoveSelected(v);
	TurnOffPointSelection();																
	LoadListBox();
	// draw polygon
	redraw(1);

}

void ControlInterface::HandleRegularNGon(int sides, float radius)
{
	png->CalcNGon(sides, radius, CompGeo::XY(0.0, 0.0));
	TurnOffPointSelection();																
	LoadComboBox();
	//ResetPolygons(); // coordinate on one polygon-drawing routine
	redraw(1);

}

void ControlInterface::ResetPolygons(void)
{

	//ppd->DrawPolygons(*png, dArea);  // this clears drawing screen
	redraw(1);
	//MakeControlsVisible(true);
	//buttonsVisible = true;
	//MakeButtonsVisible();

}

void ControlInterface::HandleConvexHull(void)
{
	//TurnOffPointSelection(); not actually adding any new points
	int sAt = png->polygons->size();
	ConvexHull cH(png->polygons);
	png->AddNGon(cH.CalculateHull());
	LoadComboBox();
	redraw(1, 8, sAt);

}


void ControlInterface::HandleVoronoi(void)
{
	//int vCount = 0;
	//CompGeo::pXY vd = png->VertexDump(vCount);
	//V_noi.ClearSites();
	//V_noi.AddSites(vd, vCount);
	TurnOffPointSelection();
	if (png->polygons == NULL) return;
	int sAt = png->polygons->size(), dMax = user_dots.size();
	if ((dMax + sAt) == 0) return;
	V_noi.AddSites(png->polygons, user_dots);
	double width = dArea.get_width(), // right - dArea.left, 
		height = dArea.get_height(); // .bottom - dArea.top;
	V_noi.SetBoundingBox(-width / 2.0, -height / 2.0, width / 2.0, height / 2.0);
	V_noi.Run();
	//vector<pPGonWork> wGons = V_noi.translateCells(); //, wGon = wGonTop;
	//png->AddNGons(wGons);
	//LoadComboBox();
	//redraw(3, 11, sAt);
	CompGeo::DCEL<double> dPlus;
	V_noi.translateDiagram(dPlus);
	vector<pPGonWork> wGons = V_noi.translateCells(dPlus); //, wGon = wGonTop;
	png->AddNGons(wGons);
	LoadComboBox();
	redraw(3, 11, sAt);
	//ppd->ColorPolygonList(wGonTop, dArea, D2D1::ColorF(1.0f, 0.5f, 0.0f, 1.0f));
}

void ControlInterface::HandleVoronoiAnimation(void)
{ // timer calls this:  all drawing from here: check bools before update
	if (!timerOn) return;
	if (V_noi.DiagramComplete) 
	{
		HandleVoronoiAnimationEnd();
		return;
	}
	if (V_noi.InProcess) return;
	V_noi.InProcess = true;
	V_noi.Animate();
	redraw(4);
	V_noi.InProcess = false;
}

void ControlInterface::HandleVoronoiAnimationStart(void)
{
	TurnOffPointSelection();
	//HWND h_Wnd = GetParent(dlg);
	//int vCount = 0;
	//CompGeo::pXY vd = png->VertexDump(vCount);
	//V_noi.ClearSites();
	//V_noi.AddSites(vd, vCount);
	int sAt = 0, dMax = user_dots.size();
	if (png->polygons != NULL) sAt = png->polygons->size();
	if ((dMax + sAt) == 0) return;
	V_noi.AddSites(png->polygons, user_dots);
	//V_noi.AddSites(png->polygonList);
	double width = dArea.get_width(), height = dArea.get_height();
	V_noi.SetBoundingBox(-width / 2.0, -height / 2.0, width / 2.0, height / 2.0);
	//double width = dArea.right - dArea.left, height = dArea.bottom - dArea.top;
	//V_noi.SetBoundingBox(-width / 2.0, -height / 2.0, width / 2.0, height / 2.0);
	V_noi.InProcess = false;
	ppd->V_BMP_IDX.clear();
	V_noi.StartAnimation(animation_delta); //, animation_timer_id, animation_Interval, animation_delta);
	mouseState = 2;
	if (timerOn) 
	{
		//timerOn = false;
		//my_slot.disconnect();
		conn.disconnect();
		//redraw(4);
		//return;
	}
	timerOn = true;
	m_timer_number = rand();
	my_slot = sigc::bind(sigc::mem_fun(*this,
				&ControlInterface::on_timeout), m_timer_number);

	// This is where we connect the slot to the Glib::signal_timeout()
	conn = Glib::signal_timeout().connect(my_slot, 100);

}

void ControlInterface::HandleVoronoiAnimationEnd(void)
{
	timerOn = false;
	conn.disconnect();
	vector<pPGonWork> wGons = V_noi.translateCells(); //, wGon = wGonTop;
	int sAt = 0;
	if (png->polygons != NULL) sAt = png->polygons->size();
	png->AddNGons(wGons);
	LoadComboBox();
	//redraw(7);
	redraw(3, 3, sAt);
	mouseState = 0;
	//V_noi.InProcess = false;

}

void ControlInterface::GeneratePoints(void)
{
	int w = dArea.get_width(), h = dArea.get_height(), cx = w / 2, cy = h / 2, maxP = (rand() % 30) + 20;
	user_dots.clear();
	int ckSz = user_dots.size();
	for (int i = 0; i < maxP; ++i)
	{
		CompGeo::XY mouse(rand() % w, rand() % h),
			model(mouse.x - cx, (double)cy - mouse.y);
		
		user_dots.push_back(model);
	}

}

void ControlInterface::HandleScreenSave(void)
{
	if ((screenSave > 0) && (ssIdx == 0))
	{
		ss_conn.disconnect();
		screenSave = 0;
	}
	if (screenSave == 0)
	{
		HandleClearAllPolygons();
		ssIdx = 0;
		screenSave = 1 + (rand() % 3);
		//user_dots.clear();
		int ssDuration = 0;
		switch (screenSave)
		{
		case 1: // count
			ssIdx = 10;
			ssDuration = 500;
			break;
		case 2: // smiley
			ssIdx = 1;
			ssDuration = 10000;
			break;
		case 3: // voronoi animation
			ssIdx = 100;
			ssDuration = 1000;
			break;
		}
		//timerOn = true;
		ss_timer_number = rand();
		ss_slot = sigc::bind(sigc::mem_fun(*this,
					&ControlInterface::on_sstimeout), ss_timer_number);

		// This is where we connect the slot to the Glib::signal_timeout()
		ss_conn = Glib::signal_timeout().connect(ss_slot, ssDuration);

	}
	if (screenSave == 1)
	{
		HandleDrawNumber(ssIdx--);
	}
	if (screenSave == 2)
	{
		redraw(5);
		--ssIdx;
	}
	if ((screenSave == 3) && (!timerOn))
	{
		GeneratePoints();
		HandleClearAllPolygons();
		HandleVoronoiAnimationStart();
		--ssIdx;
	}
}


void ControlInterface::HandleTrapezoids(void)
{
	//char * cBuff = NULL;
	//int sptr = sizeof(cBuff); // 4/7/2020 debug yields 8
	
	//int Num_Threads =  thread::hardware_concurrency(); // 4/7/2020 debug yeilds 16
	//int Num_Devices = acc_get_num_devices (acc_device_nvidia);
	//int good_find = find_devices(); //GeForce GT 710 has 1 compute unit
	/*
	int Num_Devices = 0;
	unsigned int Flags = 0;
	CUresult Howdy = cuInit(Flags);

	Howdy = cuDeviceGetCount(&Num_Devices);
	CUdevice device;
	Howdy = cuDeviceGet(&device, 0);
	char name[100];
	Howdy = cuDeviceGetName(name, 100, device);
	cout << "Device Name: " << name << "\n";

	const char * attribStr[] = {"CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_BLOCK", "CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_X", "CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_Y", "CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_Z", "CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_X", "CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_Y", "CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_Z", "CU_DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_BLOCK", "CU_DEVICE_ATTRIBUTE_TOTAL_CONSTANT_MEMORY", "CU_DEVICE_ATTRIBUTE_WARP_SIZE", "CU_DEVICE_ATTRIBUTE_MAX_PITCH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE1D_WIDTH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE1D_LINEAR_WIDTH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE1D_MIPMAPPED_WIDTH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_WIDTH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_HEIGHT", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LINEAR_WIDTH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LINEAR_HEIGHT", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LINEAR_PITCH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_MIPMAPPED_WIDTH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_MIPMAPPED_HEIGHT", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_WIDTH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_HEIGHT", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_DEPTH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_WIDTH_ALTERNATE", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_HEIGHT_ALTERNATE", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_DEPTH_ALTERNATE", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURECUBEMAP_WIDTH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE1D_LAYERED_WIDTH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE1D_LAYERED_LAYERS", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LAYERED_WIDTH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LAYERED_HEIGHT", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LAYERED_LAYERS", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURECUBEMAP_LAYERED_WIDTH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURECUBEMAP_LAYERED_LAYERS", "CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE1D_WIDTH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE2D_WIDTH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE2D_HEIGHT", "CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE3D_WIDTH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE3D_HEIGHT", "CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE3D_DEPTH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE1D_LAYERED_WIDTH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE1D_LAYERED_LAYERS", "CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE2D_LAYERED_WIDTH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE2D_LAYERED_HEIGHT", "CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE2D_LAYERED_LAYERS", "CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACECUBEMAP_WIDTH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACECUBEMAP_LAYERED_WIDTH", "CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACECUBEMAP_LAYERED_LAYERS", "CU_DEVICE_ATTRIBUTE_MAX_REGISTERS_PER_BLOCK", "CU_DEVICE_ATTRIBUTE_CLOCK_RATE", "CU_DEVICE_ATTRIBUTE_TEXTURE_ALIGNMENT", "CU_DEVICE_ATTRIBUTE_TEXTURE_PITCH_ALIGNMENT", "CU_DEVICE_ATTRIBUTE_GPU_OVERLAP", "CU_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT", "CU_DEVICE_ATTRIBUTE_KERNEL_EXEC_TIMEOUT", "CU_DEVICE_ATTRIBUTE_INTEGRATED", "CU_DEVICE_ATTRIBUTE_CAN_MAP_HOST_MEMORY", "CU_DEVICE_ATTRIBUTE_COMPUTE_MODE", "CU_DEVICE_ATTRIBUTE_CONCURRENT_KERNELS", "CU_DEVICE_ATTRIBUTE_ECC_ENABLED", "CU_DEVICE_ATTRIBUTE_PCI_BUS_ID", "CU_DEVICE_ATTRIBUTE_PCI_DEVICE_ID", "CU_DEVICE_ATTRIBUTE_PCI_DOMAIN_ID", "CU_DEVICE_ATTRIBUTE_TCC_DRIVER", "CU_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE", "CU_DEVICE_ATTRIBUTE_GLOBAL_MEMORY_BUS_WIDTH", "CU_DEVICE_ATTRIBUTE_L2_CACHE_SIZE", "CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_MULTIPROCESSOR", "CU_DEVICE_ATTRIBUTE_UNIFIED_ADDRESSING", "CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR", "CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MINOR", "CU_DEVICE_ATTRIBUTE_GLOBAL_L1_CACHE_SUPPORTED", "CU_DEVICE_ATTRIBUTE_LOCAL_L1_CACHE_SUPPORTED", "CU_DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_MULTIPROCESSOR", "CU_DEVICE_ATTRIBUTE_MAX_REGISTERS_PER_MULTIPROCESSOR", "CU_DEVICE_ATTRIBUTE_MANAGED_MEMORY", "CU_DEVICE_ATTRIBUTE_MULTI_GPU_BOARD", "CU_DEVICE_ATTRIBUTE_MULTI_GPU_BOARD_GROUP_ID", "CU_DEVICE_ATTRIBUTE_HOST_NATIVE_ATOMIC_SUPPORTED", "CU_DEVICE_ATTRIBUTE_SINGLE_TO_DOUBLE_PRECISION_PERF_RATIO", "CU_DEVICE_ATTRIBUTE_PAGEABLE_MEMORY_ACCESS", "CU_DEVICE_ATTRIBUTE_CONCURRENT_MANAGED_ACCESS", "CU_DEVICE_ATTRIBUTE_COMPUTE_PREEMPTION_SUPPORTED", "CU_DEVICE_ATTRIBUTE_CAN_USE_HOST_POINTER_FOR_REGISTERED_MEM", "CU_DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_BLOCK_OPTIN"},
				* cModeStr[] = {"CU_COMPUTEMODE_DEFAULT", "CU_COMPUTEMODE_PROHIBITED", "CU_COMPUTEMODE_EXCLUSIVE_PROCESS"};
	CUdevice_attribute attrib[] = {CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_BLOCK, CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_X, CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_Y, CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_Z, CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_X, CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_Y, CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_Z, CU_DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_BLOCK, CU_DEVICE_ATTRIBUTE_TOTAL_CONSTANT_MEMORY, CU_DEVICE_ATTRIBUTE_WARP_SIZE, CU_DEVICE_ATTRIBUTE_MAX_PITCH, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE1D_WIDTH, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE1D_LINEAR_WIDTH, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE1D_MIPMAPPED_WIDTH, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_WIDTH, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_HEIGHT, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LINEAR_WIDTH, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LINEAR_HEIGHT, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LINEAR_PITCH, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_MIPMAPPED_WIDTH, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_MIPMAPPED_HEIGHT, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_WIDTH, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_HEIGHT, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_DEPTH, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_WIDTH_ALTERNATE, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_HEIGHT_ALTERNATE, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_DEPTH_ALTERNATE, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURECUBEMAP_WIDTH, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE1D_LAYERED_WIDTH, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE1D_LAYERED_LAYERS, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LAYERED_WIDTH, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LAYERED_HEIGHT, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LAYERED_LAYERS, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURECUBEMAP_LAYERED_WIDTH, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURECUBEMAP_LAYERED_LAYERS, CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE1D_WIDTH, CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE2D_WIDTH, CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE2D_HEIGHT, CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE3D_WIDTH, CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE3D_HEIGHT, CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE3D_DEPTH, CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE1D_LAYERED_WIDTH, CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE1D_LAYERED_LAYERS, CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE2D_LAYERED_WIDTH, CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE2D_LAYERED_HEIGHT, CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACE2D_LAYERED_LAYERS, CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACECUBEMAP_WIDTH, CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACECUBEMAP_LAYERED_WIDTH, CU_DEVICE_ATTRIBUTE_MAXIMUM_SURFACECUBEMAP_LAYERED_LAYERS, CU_DEVICE_ATTRIBUTE_MAX_REGISTERS_PER_BLOCK, CU_DEVICE_ATTRIBUTE_CLOCK_RATE, CU_DEVICE_ATTRIBUTE_TEXTURE_ALIGNMENT, CU_DEVICE_ATTRIBUTE_TEXTURE_PITCH_ALIGNMENT, CU_DEVICE_ATTRIBUTE_GPU_OVERLAP, CU_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, CU_DEVICE_ATTRIBUTE_KERNEL_EXEC_TIMEOUT, CU_DEVICE_ATTRIBUTE_INTEGRATED, CU_DEVICE_ATTRIBUTE_CAN_MAP_HOST_MEMORY, CU_DEVICE_ATTRIBUTE_COMPUTE_MODE, CU_DEVICE_ATTRIBUTE_CONCURRENT_KERNELS, CU_DEVICE_ATTRIBUTE_ECC_ENABLED, CU_DEVICE_ATTRIBUTE_PCI_BUS_ID, CU_DEVICE_ATTRIBUTE_PCI_DEVICE_ID, CU_DEVICE_ATTRIBUTE_PCI_DOMAIN_ID, CU_DEVICE_ATTRIBUTE_TCC_DRIVER, CU_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE, CU_DEVICE_ATTRIBUTE_GLOBAL_MEMORY_BUS_WIDTH, CU_DEVICE_ATTRIBUTE_L2_CACHE_SIZE, CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_MULTIPROCESSOR, CU_DEVICE_ATTRIBUTE_UNIFIED_ADDRESSING, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MINOR, CU_DEVICE_ATTRIBUTE_GLOBAL_L1_CACHE_SUPPORTED, CU_DEVICE_ATTRIBUTE_LOCAL_L1_CACHE_SUPPORTED, CU_DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_MULTIPROCESSOR, CU_DEVICE_ATTRIBUTE_MAX_REGISTERS_PER_MULTIPROCESSOR, CU_DEVICE_ATTRIBUTE_MANAGED_MEMORY, CU_DEVICE_ATTRIBUTE_MULTI_GPU_BOARD, CU_DEVICE_ATTRIBUTE_MULTI_GPU_BOARD_GROUP_ID, CU_DEVICE_ATTRIBUTE_HOST_NATIVE_ATOMIC_SUPPORTED, CU_DEVICE_ATTRIBUTE_SINGLE_TO_DOUBLE_PRECISION_PERF_RATIO, CU_DEVICE_ATTRIBUTE_PAGEABLE_MEMORY_ACCESS, CU_DEVICE_ATTRIBUTE_CONCURRENT_MANAGED_ACCESS, CU_DEVICE_ATTRIBUTE_COMPUTE_PREEMPTION_SUPPORTED, CU_DEVICE_ATTRIBUTE_CAN_USE_HOST_POINTER_FOR_REGISTERED_MEM, CU_DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_BLOCK_OPTIN};
	CUcomputemode cModes[] = {CU_COMPUTEMODE_DEFAULT, CU_COMPUTEMODE_PROHIBITED, CU_COMPUTEMODE_EXCLUSIVE_PROCESS};
	
	int rVal = 0, numAttrib = sizeof(attrib) / sizeof(attrib[0]);
	for (int i = 0; i < numAttrib; ++i)
	{
		Howdy = cuDeviceGetAttribute(&rVal, attrib[i], device);

		cout << "\t" << to_string(i) << ") " << attribStr[i] << ": " << to_string(rVal);
		if (CU_DEVICE_ATTRIBUTE_COMPUTE_MODE == attrib[i])
		{
			cout << " i.e. ";
			int j = 0;
			for (j = 0; j < (sizeof(cModes) / sizeof(cModes[0])); ++j)
			{
				if (cModes[j] == rVal) break;
			}
			cout << cModeStr[j];
		}
		cout << "\n";
	}
	*/ // output in Documents/DevAtt.txt at bottom
	//int numDevs = 0;
	//uint numDims = get_work_dim();

	//CUresult CUR = cuDeviceGetCount(&numDevs);
	if (png->polygons == NULL) return;
	int sAt = png->polygons->size(), dMax = user_dots.size();
	if ((dMax + sAt) == 0) return;
	TurnOffPointSelection();																
	V_noi.AddSites(png->polygons, user_dots);
	double width = dArea.get_width(), height = dArea.get_height();
	V_noi.SetBoundingBox(-width / 2.0, -height / 2.0, width / 2.0, height / 2.0);
	V_noi.Run();
	if (current_dcel != NULL) current_dcel->clear_all();
	else current_dcel = new CompGeo::DCEL<double>;
	V_noi.translateDiagram(*current_dcel);
	//for (int i = 0; i < 10; ++i)
	//{ // try to get an error to debug
		if (tzoid_map != NULL)
		{
			delete tzoid_map;
			tzoid_map = NULL;
		}
		tzoid_map = new Trapezoid::Map<double>;
		tzoid_map->Run(current_dcel);
		//tzoid_map->Run(V_noi);
	//}
	vector<pPGonWork> wGons = tzoid_map->TranslateTrapezoids(); //, wGon = wGonTop;
	png->AddNGons(wGons);
	LoadComboBox();
	redraw(1, 5, sAt);
	//ppd->ColorPolygonList(wGonTop, dArea, D2D1::ColorF(0.5f, 1.0f, 0.5f, 1.0f));

}

void ControlInterface::HandleSegmentIntersect(void)
{
	TurnOffPointSelection();
	int sAt = png->polygons->size();
	SegmentIntersect S_I(*png);
	S_I.getAllIntersects();
	unsigned int iCount;
	//pPGonWork wGon = S_I.makePolygon(iCount);
	vector<pPGonWork> wGons = S_I.makePolygons(iCount);
	if (iCount > 0)
	{
		png->RemoveAllPolygons();
		png->AddNGons(wGons);
		LoadComboBox();
		//png->AddNGon(wGon);
		//LoadComboBox();
		redraw(1, 4); //, sAt);
	}
	string sMsg = to_string(iCount) + " intersections found.";
	status->set_text(sMsg.c_str());
}
/*
void ControlInterface::HandleMouse2(POINTS mPts)
{
	HWND ctl = GetDlgItem(dlg, IDC_VERTEXINFO);
	HRESULT b_p;
	char txtBuff[100];
	//int cx = (dArea.right - dArea.left) / 2, cy = (dArea.bottom - dArea.top) / 2;
	b_p = StringCbPrintf(txtBuff, 100 * sizeof(char),TEXT("vertex mouse: (%+5d, %+5d)"), 
		mPts.x, mPts.y);
	SendMessage(ctl, WM_SETTEXT, (WPARAM)0, (LPARAM)txtBuff);
}

void ControlInterface::HandleMouse1(POINTS mPts)
{
	//HWND ctl = GetDlgItem(dlg, IDC_VERTEXINFO);
	HWND ctl = GetDlgItem(sdlg, IDC_COORDBOX);
	HRESULT b_p;
	char txtBuff[100];
	int cx = (dArea.right - dArea.left) / 2, cy = (dArea.bottom - dArea.top) / 2;
	b_p = StringCbPrintf(txtBuff, 100 * sizeof(char),TEXT("(%+011.6f, %+011.6f)"), 
		(double)(mPts.x - cx), (double)(cy - mPts.y));
	SendMessage(ctl, WM_SETTEXT, (WPARAM)0, (LPARAM)txtBuff);
	if (pointSelection)
	{
		Fortune::pSitePoint sp = tzoid_map->GetSitePoint(mPts);
		if (current_point == NULL) 
		{
			ppd->MousePointOn(sp->GetPOINT(), dArea);
			current_point = sp;
		}
		else
		{
			if (!(*sp == *current_point))
			{
				ppd->MousePointOff(current_point->v_Node, dArea);
				ppd->MousePointOn(sp->GetPOINT(), dArea);
				current_point = sp;
			}
		}
	}
}
*/

void ControlInterface::HandleMouseCoordinates(GdkEventMotion* motion_event)
{
	//output_allocation("Display Area: ", dArea);
	int w = dArea.get_width(), h = dArea.get_height(), cx = w / 2, cy = h / 2,
		x = (int)motion_event->x, y = (int)motion_event->y, x_c = x - cx, y_c = cy - y;
	string coords = "(" + to_string(x_c) + ", " + to_string(y_c) + ")";
	//string coords = "(" + to_string(motion_event->x_root) + ", " + to_string(motion_event->y_root) + ")";
	status->set_text(coords);
	if (mouseState == 3)
	{
		CompGeo::BasicTPoint<double> c_m(x_c, y_c, 0);
		Fortune::pSitePoint sp = tzoid_map->GetSitePoint(&c_m);
		//assert (sp != NULL);
		if (sp == NULL) return;
		if (current_point == NULL) 
		{
			//ppd->MousePointOn(sp->GetPOINT(), dArea);
			current_point = sp;
			if (sp->v_Node != NULL) sp->v_Node->vtxInfo.moused = true;
		}
		else
		{
			if (!(*sp == *current_point))
			{
				if (current_point->v_Node != NULL) current_point->v_Node->vtxInfo.moused = false;
				//ppd->MousePointOff(current_point->v_Node, dArea);
				//ppd->MousePointOn(sp->GetPOINT(), dArea);
				current_point = sp;
				if (sp->v_Node != NULL) sp->v_Node->vtxInfo.moused = true;
			}
		}
		redraw(1);
	}
}

void ControlInterface::HandleMouseButtonRelease(GdkEventButton* releaseEvent)
{
	if (mouseState == 0) return;
	if (screenSave != 0) return;
	int w = dArea.get_width(), h = dArea.get_height(), cx = w / 2, cy = h / 2;
	CompGeo::XY mouse(releaseEvent->x, releaseEvent->y),
		model(mouse.x - cx, (double)cy - mouse.y);

	if (mouseState == 1)
	{
		if (releaseEvent->button == 1)	//left button
		{

			user_dots.push_back(model);
			redraw(2);
		}
		if (releaseEvent->button == 3)	//right button
		{
			user_dots.clear();
			redraw(png->selectedPolygon == NULL? 7: 1);

		}
		return;
	}
	if (mouseState == 2)
	{
		if (timerOn) 
		{
			timerOn = false;
			conn.disconnect();
		}
		else
		{
			timerOn = true;
			m_timer_number = rand();
			my_slot = sigc::bind(sigc::mem_fun(*this,
						&ControlInterface::on_timeout), m_timer_number);

			// This is where we connect the slot to the Glib::signal_timeout()
			conn = Glib::signal_timeout().connect(my_slot, 100);
		}
		return;
	}
	if (mouseState == 3)
	{
		//GdkDevice * mousegdk = releaseEvent->device;
		//mousegdk->gdk_event_get_source_device(releaseEvent); //->device;
		
		//releaseEvent->window;
		pPGonWork wGon = current_point->w_Gon;
		pVertexNode v_n = current_point->v_Node;
		HandleComboBox(wGon->cIdx);
		//wGon->selVertex = v_n->vIdx;
		//png->selVertex = v_n;
		//v_n->vtxInfo.is_selected = true;
		HandleVertexList(v_n);
		//ppd->SelectPointOn(current_point->GetPOINT(), dArea);
		CompGeo::XY xy = current_point->v_Node->vtxInfo.vertex;
		//double cx = (double)(dArea.right - dArea.left) / 2.0, cy = (double)(dArea.bottom - dArea.top) / 2.0;
			//vd = (double)vArea.right;
		//int xFrm = GetSystemMetrics(SM_CXFRAME), yFrm = GetSystemMetrics(SM_CYFRAME);
		//int cxw = wArea.get_width() / 2, cyw = wArea.get_height() / 2;
		//xy.x += cx; // + vd - (double)(wArea.left + xFrm);
		//xy.y = cy - xy.y; // + (double)(dArea.top - wArea.top - yFrm);
		int vtx_x = (int)xy.x + cx, vtx_y = cy - (int)xy.y, 
			root_x = 0, root_y = 0,
			origin_x = 0, origin_y = 0;

		// moving the mouse pointer to the nearest point:
		#ifdef _CROSS_COMPILE
			Glib::RefPtr<Gdk::DeviceManager> dMgr = GEXT::display->get_device_manager();
			Glib::RefPtr<Gdk::Device> mouse = dMgr->get_client_pointer();
		#else
			Glib::RefPtr<Gdk::Seat> seat = GEXT::display->get_default_seat();
			Glib::RefPtr<const Gdk::Device> mouse0 = seat->get_pointer();
			const Gdk::Device*  mouse2 = mouse0.get();
			Gdk::Device * mouse = const_cast<Gdk::Device*>(mouse2);
		#endif
		Glib::RefPtr<Gdk::Screen> scrn = GEXT::screen;
		Glib::RefPtr<Gdk::Window> gdkWin = drawWnd->get_window();
		gdkWin->get_root_origin(origin_x, origin_y);
		gdkWin->get_root_coords(vtx_x, vtx_y, root_x, root_y);
		mouse->warp(scrn, root_x, root_y);


		return;
	}
	assert (1 == 2);

	
	
// toggle timer
/*
	if (timerOn) 
	{
		timerOn = false;
		//my_slot.disconnect();
		conn.disconnect();
		redraw(7);
		return;
	}
	timerOn = true;
	m_timer_number = rand();
	my_slot = sigc::bind(sigc::mem_fun(*this,
				&ControlInterface::on_timeout), m_timer_number);

	// This is where we connect the slot to the Glib::signal_timeout()
	conn = Glib::signal_timeout().connect(my_slot, 1500);
*/
	
}

bool ControlInterface::on_sstimeout(int timer_number)
{

	HandleScreenSave();
	return true;
}

bool ControlInterface::on_timeout(int timer_number)
{
	HandleVoronoiAnimation();

	//if (timerOn) HandleVoronoiAnimation();
	//else
	//{
	//	redraw(7);
	//	redraw(3);	
	//}
	
	//int coinflip = rand() % 3;
	//if (0 == coinflip)
	//{
	//	redraw(8);
	//}
	//else
	//{
	//	if (1 == coinflip)
	//	{
	//		int dNum = rand() % 12 + 1;
	//		HandleDrawNumber(dNum);
	//	}
	//	else
	//	{
	//		redraw(5);
	//	}
		
	//}
	
	return true;
}

void ControlInterface::HandleDotConnect(void)
{
	int sAt = 0;
	if (png->polygons != NULL) sAt = png->polygons->size();
	if (user_dots.size() > 2)
	{
		pPGonWork wGon = new PGonWork;
		pNameNode nn = new NameNode;
		string nme = "dots";
		nn->polyname = nme;
		wGon->numVertices = user_dots.size();
		wGon->pName = nn;
		pVertexNode pvn = NULL, pvTrail = NULL;
		for (vector<CompGeo::XY>::iterator it = user_dots.begin(); it != user_dots.end(); ++it)
		{
			CompGeo::XY ud = *it;
			pvn = new VertexNode; 
			pvn->vtxInfo.vertex = ud;
			pvn->next = NULL;
			if (pvTrail == NULL) wGon->vNode = pvn;
			else pvTrail->next = pvn;
			pvTrail = pvn;

		}
		png->AddNGon(wGon);
		user_dots.clear();
		LoadComboBox();
		redraw(1, 5, sAt);
	}

}

void ControlInterface::HandleDefaultInfo(void)
{
	//Gtk::Label * sBar;
	//pG->get("StatusBar", sBar);
	long pVal = 0;
	if (png != NULL) pVal = (long)png->selectedPolygon;
	stringstream sStrm;
	sStrm << hex << pVal;
	status->set_text(sStrm.str().c_str());
}

void ControlInterface::TurnOnPointSelection(void)
{
	if (png->polygons == NULL) return;
	int sAt = png->polygons->size(), dMax = user_dots.size();
	if ((dMax + sAt) == 0) return;
	V_noi.AddSites(png->polygons, user_dots);
	double width = dArea.get_width(), height = dArea.get_height();
	V_noi.SetBoundingBox(-width / 2.0, -height / 2.0, width / 2.0, height / 2.0);
	V_noi.Run();
	if (tzoid_map != NULL)
	{
		delete tzoid_map;
		tzoid_map = NULL;
	}
	tzoid_map = new Trapezoid::Map<double>;
	if (current_dcel != NULL) current_dcel->clear_all();
	else current_dcel = new CompGeo::DCEL<double>;
	V_noi.translateDiagram(*current_dcel);
	tzoid_map->Run(current_dcel);
	pointSelection = true;
	mouseState = 3;
}

void ControlInterface::TurnOffPointSelection(void)
{
	pointSelection = false;
	if (current_point != NULL) current_point->v_Node->vtxInfo.moused = false;
	current_point = NULL;
	if (tzoid_map != NULL)
	{
		delete tzoid_map;
		tzoid_map = NULL;
	}
	if (current_dcel != NULL)
	{
		delete current_dcel;
		current_dcel = NULL;
	}
	mouseState = 0;
}

void ControlInterface::HandleTriangulation(bool fMonotoneOnly)
{
	//TurnOffPointSelection(); not adding new points
	string eMsg;
	Triangulation t(*png, fMonotoneOnly, eMsg);
	if (eMsg.length() > 0)
	{
		status->set_text(eMsg.c_str());
		return;
	}
	// test index triangles
	/*
	vector<unsigned int> idxs = t.indexFaces(t.tLst.Faces);
	unsigned int tCount = idxs.size() / 3; 
	cout << to_string(tCount) << " Triangles By Indices:\n";
	for (unsigned int i = 0; i < tCount; ++i)
	{
		cout << "\t" << to_string(i) << ")<" << to_string(idxs[3 * i]) << ", " << 
			to_string(idxs[3 * i + 1]) << ", " << 
			to_string(idxs[3 * i + 2]) << ">"; 
	}
	cout << "\n End of Triangles By Indices\n";
	return;
	/**/
	// end test
	vector<pPGonWork> wGons = t.translateFaces(t.tLst.Faces); //, wGon = NULL;
	int pMax = wGons.size();
	if (pMax > 0)
	{
		png->RemoveAllPolygons();
		png->AddNGons(wGons);
		LoadComboBox();
		redraw(1, 8);
		
		string sStr = to_string(pMax) + (fMonotoneOnly? " monotonics ": " triangles ") + "added!";
		status->set_text(sStr.c_str());
	}

}

void ControlInterface::HandleClearAllPolygons(void)
{
	redraw(7);
	png->RemoveAllPolygons();
	LoadComboBox();
	
	TurnOffPointSelection();
}

void ControlInterface::HandleClearSharedEdges(void)
{
	//TurnOffPointSelection(); not adding new points
	if (png->polygons == NULL) return;
	int pMax = png->polygons->size();
	if (pMax == 0) return;
	Unshare uns;
	uns.RemoveSharedEdges(png->polygons);
	vector<pPGonWork> wGons = uns.ConsolidatePolygons();
	if (wGons.size() > 0)
	{
		png->RemoveAllPolygons();
		png->AddNGons(wGons);
		LoadComboBox();
	}
	ResetPolygons();
}
/*
void ControlInterface::AdjustForSize(void)
{
	if (IsIconic(mainWnd)) return;

	GetClientRect(mainWnd, &wArea);
	int vWdth = vArea.right - vArea.left,
		wWdth = wArea.right - wArea.left,
		wHeight = wArea.bottom - wArea.top,
		tHeight = TOOL_HEIGHT,
		dHeight = wHeight - tHeight;
			
	//if (vWdth > wWdth) vWdth = wWdth;

	//BOOL tmp = MoveWindow(dlg, 0, 0, vWdth, wHeight, TRUE);
	//GetClientRect(dlg, &vArea);

	BOOL tmp = MoveWindow(drawWnd, vWdth, 0, wWdth - vWdth, dHeight, TRUE); 
	GetClientRect(drawWnd, &dArea);

	RECT lstRect;
	SendMessage(toolWnd, TB_GETITEMRECT, MAX_TOOLBUTTONS - 1, (LPARAM)&lstRect);

	tmp = MoveWindow(toolWnd, vWdth, dArea.bottom, 
		lstRect.right, tHeight, TRUE);
	GetClientRect(toolWnd, &tArea);

	tmp = MoveWindow(dlg, 0, 0, vWdth, wHeight, TRUE);
	GetClientRect(dlg, &vArea);
	AdjustStatusBar();

	//wdp->ctlInt.ResetControlInterface(wdp->vertexWnd, wdp->toolWnd, wdp->nGn, wdp->pDraw, 
	//	wdp->wArea, wdp->dArea, wdp->vArea, wdp->tArea);

}

void ControlInterface::AdjustStatusBar(void)
{
	HWND hDlg = sdlg, SBctl = GetDlgItem(hDlg, IDC_STATUSBOX),
		CBctl = GetDlgItem(hDlg, IDC_COORDBOX);
	RECT cbRect;
	GetClientRect(CBctl, &cbRect);
	int cbWidth = cbRect.right - cbRect.left, cbHeight = cbRect.bottom - cbRect.top,
		wWidth = wArea.right - wArea.left, vWidth = vArea.right - vArea.left,
		tWidth = tArea.right - tArea.left, sX = vWidth + tWidth,
		dHeight = dArea.bottom - dArea.top, sY = dHeight,
		sWidth = wWidth - sX, sHeight = TOOL_HEIGHT, cbX = sWidth - cbWidth,
		sbWidth = cbX;
	MoveWindow(hDlg, sX, sY, sWidth, sHeight, FALSE);
	MoveWindow(CBctl, cbX, 0, cbWidth, sHeight, FALSE);
	MoveWindow(SBctl, 0, 0, sbWidth, sHeight, FALSE);
	ShowWindow(hDlg, SW_SHOW);

}

void ControlInterface::HandleZoom(int zID)
{
	char scrollStrings[7][20] = {L"1 pixel = 1 mm", L"1 pixel = 1e-1 mm", 
		L"1 pixel = 1e-2 mm", L"1 pixel = 1e-3 mm", L"1 pixel = 1e-4 mm", L"1 pixel = 1e-5 mm", 
		L"1 pixel = 1e-6 mm"};
	HWND ctl = GetDlgItem(sdlg, IDC_STATUSBOX);
	SendMessage(ctl, WM_SETTEXT, 0, (LPARAM)scrollStrings[zID]);
	zoom_exp = zID;
    
	// Get the menu.
    HMENU hMenuLoaded = GetMenu(mainWnd); 
         
    // Get the submenu for the fifth menu item (i.e. Zoom).
    HMENU hPopupMenu = GetSubMenu(hMenuLoaded, ZOOM_MENU_INDEX);

	BOOL cmri = CheckMenuRadioItem(hPopupMenu, 0, 6, zoom_exp, MF_BYPOSITION);

}
*/
void ControlInterface::Cleanup(void)
{
	if (ppd != NULL) 
	{
		delete ppd;
		ppd = NULL;
	}
	if (png != NULL)
	{
		delete png;
		png = NULL;
	}
	//if (tzoid_map != NULL)
	//{
	//	delete tzoid_map;
	//	tzoid_map = NULL;
	//}
	
	delete ZOOMS;

}

string ControlInterface::numstrTrim(const string & s)
{ //remove all non-digit characters from s

	string r = "", allowables = "+-.0123456789";
	for(unsigned int i = 0; i < s.length(); ++i)
	{
		char a = s.at(i);
		if (string::npos != allowables.find(a)) r += a; 
	}
	return r;
}

int ControlInterface::getInteger(const string & s)
{
	string r = s, allowables = "0123456789";
	if (s.length() == 0) return 0;
	char a = r.at(0);
	bool IsNegative = false;
	if (a == '-') 
	{
		IsNegative = true;
		r = r.substr(1);
	}
	int multiplier = 1, l = r.length(), my_int = 0;
	for (int i = 0; i < l; ++i)
	{
		a = r.at(l - 1 - i);
		if (string::npos != allowables.find(a))
		{
			my_int += multiplier * (a - '0');
			multiplier *= 10;
		}
	}
	my_int *= (IsNegative? -1: 1);
	return my_int;

}

double ControlInterface::getDouble(const string & s)
{
	size_t pointAt = s.find_first_of('.');
	bool hasFract = (pointAt != string::npos);
	string intPartStr = "", fractPartStr = "";
	int intPart = 0, fractPart = 0, fractLength = 0;
	double denom = 1.0;

	if (hasFract)
	{
		intPartStr = s.substr(0, pointAt);
		intPart = getInteger(intPartStr);
		fractPartStr = s.substr(pointAt + 1);
		fractPart = getInteger(fractPartStr);
		fractLength = s.length() - pointAt - 1;
		if (fractPart < 0) --fractLength;
	}
	else
	{
		intPart = getInteger(s);
	}
	int posFract = abs(fractPart), ePart = 0;
	if (posFract > 0) ePart = (int)(log10(posFract)) + 1;
	denom = pow(10.0, ePart) * (intPart < 0.0? -1.0: 1.0);

	return intPart + fractPart / denom;

}


/*
void ControlInterface::InitializeZoomMenu(void)
{
	// Get the menu.
    HMENU hMenuLoaded = LoadMenu(hInst, MAKEINTRESOURCE(IDC_WALLS)),
		hMenuWin = GetMenu(mainWnd),
         
    // Get the submenu for the fifth menu item (i.e. Zoom).
		hPopupMenu = GetSubMenu(hMenuLoaded, ZOOM_MENU_INDEX);
	
	BOOL cmri = CheckMenuRadioItem(hPopupMenu, 0, 6, 0, MF_BYPOSITION);
	//BOOL dm = DrawMenuBar(mainWnd);
	BOOL sm = SetMenu(mainWnd, hMenuLoaded);
	BOOL dm = DestroyMenu(hMenuWin);
}
*/
/*
void ControlInterface::InitializeStatusWindow(void)
{
	HWND ctl = GetDlgItem(sdlg, IDC_STATUSBOX);
	char txtBuff[20] = L"1 pixel = 1 mm";
	SendMessage(ctl, WM_SETTEXT, 0, (LPARAM)txtBuff);

}
*/

//void ControlInterface::InitializeScrollBars(void)
//{
	/*
	SCROLLINFO v, h;  // vertical and horizontal scrollbar information
	unsigned int width_dA, height_dA,  // pixels available in the drawWnd
		width_VSB, height_HSB; // width of vertical scroll bar and height of horizontal scroll bar
	int SCALEFACT;  // s/b ZOOMS[zoom_exp]
	// V will be a rangetree for all the vertices in NGons
	double xMinVP, yMaxVP, xMaxVP, yMinVP,  // maximums and minimums on the vertex plane
		widthVP, heightVP,	// vertex plane extents
		xMinPort, yMaxPort, xMaxPort, yMinPort; // maximums and minimums for the viewport
	*/
/*
	int sosi = sizeof(SCROLLINFO);
	ZeroMemory(&SS.h, sosi);
	ZeroMemory(&SS.v, sosi);

	SS.h.cbSize = sosi;
	SS.h.fMask = SIF_RANGE;

	SS.v.cbSize = sosi;
	SS.v.fMask = SIF_RANGE;

	SetScrollInfo(drawWnd, SB_HORZ, &SS.h, TRUE);
	SetScrollInfo(drawWnd, SB_VERT, &SS.v, TRUE);

	SS.width_dA = dArea.right - dArea.left;
	SS.height_dA = dArea.bottom - dArea.top;

	SS.height_HSB = (unsigned int)GetSystemMetrics(SM_CYHSCROLL);
	SS.width_VSB = (unsigned int)GetSystemMetrics(SM_CXVSCROLL);

	SS.SCALEFACT = 1;

	double cx = SS.width_dA / 2.0, cy = SS.height_dA / 2.0;
	SS.xMinVP = -cx; SS.xMaxVP = cx; SS.yMinVP = -cy; SS.yMaxVP = cy;
	SS.widthVP = (double)SS.width_dA; SS.heightVP = (double)SS.height_dA;
	SS.xMinPort = -cx; SS.xMaxPort = cx; SS.yMinPort = -cy; SS.yMaxPort = cy;
}
*/
/*
void ControlInterface::HandleLoadPLY(void)
{
	if (pView != NULL) delete pView;
	pView = new View(dArea, 6);
	ppd->Draw3DFaces(&pView->view_data, pView->ProjectionM, dArea, pView->maxDepth);
*/
	/*
	TurnOffPointSelection();
	pPGonWork wGon = v.ProjectFaces();
	if (wGon != NULL)
	{
		png->RemoveAllPolygons();
		png->AddNGon(wGon);
		LoadComboBox();
	}
	ResetPolygons();
	
	CompGeo::ViewData vd;
	vd.bCld = NULL;
	vd.pCld = NULL;
	vd.fCld = NULL;
	vd.hCld = NULL;
	vd.pvCld = NULL;
	vd.fvCld = NULL;
	vd.hvCld = NULL;
	PLYHandler ph(&vd);
	char c = ph.GetEndianess();
	ph.LoadPLY();
	if (ph.plyErr.triggered) return;
	ph.ShowHeader();
	ph.ShowCloudCounts();
	ph.ShowBBs();
	ph.ShowOrphanVeritces(false);
	ph.GetBIH();
	if (vd.bCld != NULL) delete vd.bCld;
	if (vd.fCld != NULL) delete vd.fCld;
	if (vd.pCld != NULL) delete vd.pCld;
	if (vd.hCld != NULL) delete vd.hCld;
	*/
//}
/*
void ControlInterface::HandleChangeDefaultView(const unsigned int & dv)
{
	if (pView == NULL) return;
	pView->ChangeDefaultView(dv);
	ppd->Draw3DFaces(&pView->view_data, pView->ProjectionM, dArea, pView->maxDepth);

}

void ControlInterface::HandleChangeAxes(HWND ctl)
{
	Matrix pm(4);

	pView->UpdateStatics();
	ppd->DrawStaticFaces(ctl, &pView->statics_data, pm);
}
*/

