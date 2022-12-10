#include "first.hpp"
#include "CompGeo.hpp"
#include "widgets.hpp"
#include "NGons.hpp"

using namespace std;

// Static and Global Initializations:
unsigned int NGons::memPolyCount = 0;
/*
pVertexNode NGons::MakeVertex(void)
{
	pVertexNode pv = new VertexNode; //(pVertexNode)new VertexNode));
	pv->next = NULL;
	pv->s = NULL;
	pv->vIdx = -1;
	pv->vtxInfo.is_selected = false;
	pv->vtxInfo.depth = 0.0;
	pv->PG = NULL;
	pv->OEPG = NULL;
	// pv->vtxInfo.vtx will be set to (0,0) by default
	return pv;
}
*/
VertexNodeStruct::VertexNodeStruct(void)
{
	next = NULL;
	s = NULL;
	vIdx = -1;
	vtxInfo.is_selected = false;
	vtxInfo.moused = false;
	vtxInfo.depth = 0.0;
	vtxInfo.vertex = CompGeo::XY(0.0, 0.0);
	PG = NULL;
	OEPG = NULL;

}

VertexNodeStruct::VertexNodeStruct(const VertexNodeStruct & a)
{
	next = a.next;
	s = a.s;
	vIdx = a.vIdx;
	vtxInfo.is_selected = a.vtxInfo.is_selected;
	vtxInfo.depth = a.vtxInfo.depth;
	vtxInfo.vertex = a.vtxInfo.vertex;
	PG = a.PG;
	OEPG = a.OEPG;

}

VertexNodeStruct::~VertexNodeStruct(void)
{

}
/*
pPGonWork NGons::MakePGon(void)
{
	pPGonWork wGon = new PGonWork; //(pPGonWork)new PGonWork));
	wGon->cIdx = -1;
	//wGon->next = NULL;
	wGon->numVertices = 0;
	wGon->pIdx = 0;
	wGon->pName = NULL;
	wGon->vNode = NULL;
	wGon->plane = NULL;
	wGon->bb = NULL;
	return wGon;
}
*/
PGonWork::PGonWork(void)
{
	cIdx = -1;
	//wGon->next = NULL;
	numVertices = 0;
	pIdx = 0;
	pName = NULL;
	vNode = NULL;
	plane = NULL;
	bb = NULL;

}

PGonWork::PGonWork(const PGonWork & a)
{
	cIdx = a.cIdx;
	numVertices = a.numVertices;
	pIdx = a.pIdx;
	pName = a.pName;
	vNode = NULL;
	plane = a.plane;
	bb = a.bb;

	if (numVertices > 0)
	{
		pVertexNode travel = a.vNode, creator = NULL, trailer = NULL;
		for (unsigned int i = 0; i < numVertices; ++i)
		{
			creator = new VertexNode(*travel);
			if (trailer == NULL) vNode = creator;
			else trailer->next = creator;
			trailer = creator;
			travel = travel->next;
		}
		if (trailer != NULL) trailer->next = NULL;
	}
}

PGonWork::~PGonWork(void)
{
	if (numVertices > 0)
	{
		pVertexNode traveler = vNode, trailer = NULL;
		for (unsigned int i = 0; i < numVertices; ++i)
		{
			trailer = traveler;
			traveler = traveler->next;
			delete trailer;
		}	
	}
	numVertices = 0;
	vNode = NULL;
}

NGons::NGons(void): names(NULL), polygons(NULL), selectedPolygon(NULL), selVertex(NULL), 
	mVerts(NULL), mVertcount(-1), indexPolygon(-1), selVertices(NULL),
	pBB(NULL), M(0.0), N(NULL), I(NULL), J(NULL)/*,
	xMin(0.0), xMax(0.0), yMin(0.0), yMax(0.0), MinMaxChanged(false)*/
{
}

NGons::~NGons(void)
{
	/*
	while (nameList != NULL)
	{
		pNameNode pNN = nameList->nxtName;
		delete nameList;
		nameList = pNN;

	}
	*/
	if (names != NULL)
	{
		names->clear();
		delete names;
		names = NULL;
	}
	if (mVerts != NULL) delete [] mVerts;
	mVerts = NULL;
	RemoveSelection();
	if (polygons != NULL)
	{
		//if (polygonList->poly != NULL) DeletePolyList(&(polygonList->poly));
		polygons->clear();
		delete polygons;
		polygons = NULL;
	}
	selectedPolygon = NULL;
	selVertex = NULL;
	indexPolygon = -1;
	//indexselectedPolygon = -1;
	mVertcount = -1;
	if (pBB != NULL) delete pBB;
	if (N != NULL) delete N;
	if (I != NULL) delete I;
	if (J != NULL) delete J;
}

string NGons::GetNextMemName(const string & prefix)
{
	int i = ++memPolyCount, w = (int)(log10((double)i)) + 1;
	//char prefix[] = TEXT("mem_poly"), * nmn;
	string nmn = prefix + "[" + to_string(i) + "]";
	//w += (int)_tcslen(prefix) + 3;
	//nmn = new char[w];
	//HRESULT b_p = StringCbPrintf(nmn, w * sizeof(char), TEXT("%s[%d]"), prefix, i);
	return nmn;
}

pNameNode NGons::GetNewNameNode(const string & thename, bool f_file)
{
	pNameNode pNN = new NameNode(); //(pNameNode)new NameNode));
	//cout << "NameNode size:" << to_string(sizeof(NameNode)) << "\n";
	pNN->isdirty = false;
	pNN->isfile = f_file;
	pNN->polyname = thename;
	if (names == NULL) names = new vector<pNameNode>;
	names->push_back(pNN);
	//pNN->nxtName = NULL;

	/*
	//insert at end routine:
	pNameNode pTrail = NULL, p = nameList;
	while (p != NULL)
	{
		pTrail = p;
		p = p->nxtName;
	}
	if (pTrail == NULL)
	{
		nameList = pNN;
	}
	else
	{
		pTrail ->nxtName = pNN;
	}
	*/
	return pNN;
}


void NGons::CalcNGon(const int n, const float r, const CompGeo::XY ctr)
{
	unsigned int sides;
	if (n < 3) sides = 3;
	if (n > 100) sides = 100;
	if ((n >= 3) && (n <=100)) sides = (unsigned int) n;
	double PI = M_PI; 
	double delta = 2.0f * PI / (double)sides;
	double theta;
	/*
	if (polygonList == NULL)
	{
		polygonList = (pPGonList)new PGonList));
		polygonList->numPolygons = 0;
		polygonList->poly = NULL;
	}
	*/
	if (polygons == NULL) polygons = new vector<pPGonWork>;
	pPGonWork wGon = new PGonWork; //MakePGon();
	wGon->numVertices = sides;
	wGon->pName = GetNewNameNode(GetNextMemName("mem_poly"), false);
	pVertexNode v_trail = NULL, v = NULL;

	for (unsigned int i = 0; i < sides; ++i)
	{
		v = new VertexNode; //MakeVertex();
		v->PG = wGon;
		theta = delta * (double)i;
		v->vtxInfo.vertex = CompGeo::XY(ctr.x + r * cos(theta), ctr.y + r * sin(theta));
		if (i == 0) 
		{
			wGon->vNode = v;
		}
		else
		{
			v_trail->next = v;
		}
		v_trail = v;
	}
	polygons->push_back(wGon);
	/*
	++(polygonList->numPolygons);
	if (polygonList->poly == NULL)
	{
		polygonList->poly = wGon;
	}
	else
	{
		pPGonWork p_trail = polygonList->poly, p = polygonList->poly->next;
		while (p != NULL)
		{
			p_trail = p;
			p = p->next;
		}
		p_trail->next = wGon;
	}
	*/
	selectedPolygon = wGon;
	indexPolygon = polygons->size() - 1;
	NumberVertices();
	//selectedVertex = wGon->vNode;
	//indexPolygon = polygonList->numPolygons - 1;
	//indexselectedPolygon = indexPolygon;

}

void NGons::DeletePolyList(pPGonWork * pPlst)
	// used to delete poly list
	// now deletes pPGonWork pPlst
{
	pPGonWork lTop = *pPlst;
	lTop->numVertices = 0;
	while (lTop->vNode != NULL)
	{
		pVertexNode vxl = lTop->vNode->next;
		delete lTop->vNode;
		lTop->vNode = vxl;
	}
	delete lTop;
	lTop = NULL;

}

void NGons::LoadNGon(const string & fileName)
{
	ifstream nGonIn;
	nGonIn.open(fileName, ifstream::in | ifstream::binary);
	
	char tBuff[20] = "", tID[] = "POLYGON2017"; //, iBuff[] = "\0\0\0\0\0", dBuff[] = "\0\0\0\0\0\0\0\0\0";
	assert (sizeof(unsigned int) == 4);
	assert (sizeof(double) == 8);
	nGonIn.read(tBuff, 11);
	tBuff[11] = '\0';
	string ckStr = tBuff, errorMsg = "Not a polygon file";
	bool goodFile = (ckStr.compare(tID) == 0);
	unsigned int pNum = 0;
	//assert (sizeof(unsigned int) == 4);
	if (goodFile)
	{
		errorMsg = "No polygons in file";
		nGonIn.read((char *)&pNum, 4);
		goodFile = (pNum > 0);
	}
	if (goodFile)
	{
		pNameNode pNN = GetNewNameNode(fileName, true);
		if (polygons == NULL) polygons = new vector<pPGonWork>;
		pPGonWork wGon;
		errorMsg = "File contains a bad polygon";
		for (unsigned int i = 0; i < pNum; ++i)
		{
			wGon = new PGonWork; //MakePGon();
			wGon->pName = pNN;
			wGon->pIdx = i;
			nGonIn.read((char *)&wGon->numVertices, 4);
			if (wGon->numVertices < 3) // big no-no eventually check for non-linearity and non-intersecting
			{
				goodFile = false;
				break;
			}
			pVertexNode vxl = NULL, vxlTrail = NULL;
			for (unsigned int j = 0; j < wGon->numVertices; ++j)
			{
				vxl = new VertexNode; //MakeVertex();
				vxl->PG = wGon;
				nGonIn.read((char *)&vxl->vtxInfo.vertex.x, 8);
				nGonIn.read((char *)&vxl->vtxInfo.vertex.y, 8);
				vxl->vIdx = j;
				if (vxlTrail == NULL)
				{
					wGon->vNode = vxl;
				}
				else
				{
					vxlTrail->next = vxl;
				}
				vxlTrail = vxl;
			}
			polygons->push_back(wGon);
		}
		if (goodFile)
		{
			indexPolygon = 0;
			selectedPolygon = polygons->at(0);
			selVertex = NULL;
		}
	}
	if (!goodFile)
	{
		cout << errorMsg << "\n";
		//sBar->set_text(errorMsg.c_str());
		//sBar->show_all();
	}
	nGonIn.close();
	//fcd->set_filter(ff0);
     	
}
/*
void NGons::SaveNGon(void)
{
	if (selectedPolygon == NULL) return;
	unsigned int saveNum = 1;
	pNameNode pNN = selectedPolygon->pName;
	pPGonWork wGon = polygons->at(0), wGonBuff = selectedPolygon;
	FileIO fIO(NULL, NULL, _T("polyGON Files (*.gon)\0*.gon\0\0"), _T("Store a polygon"), 
		OFN_OVERWRITEPROMPT, _T("gon"));
	if (pNN->isfile)
	{
		// moves selection to first and gets count:
		// (depends on all polygons from a given file being sequential in polygon list)
		selectedPolygon->pName->isdirty = false;
		//pPGonWork wGon = polygonList->poly;
		while (wGon->pName != pNN) wGon = wGon->next;
		bool stillOn = true;
		saveNum = 0;
		selectedPolygon = wGon;
		while (stillOn) 
		{
			++saveNum;
			wGon = wGon->next;
			if (wGon == NULL)
			{
				stillOn = false;
			}
			else
			{
				stillOn = (wGon->pName == pNN);
			}
		}
		size_t count = _tcslen(pNN->polyname) + 1;
		if (fIO.File != NULL) delete [] fIO.File;
		fIO.File = new char[count];
		HRESULT c_r = StringCbCopy(fIO.File, count * sizeof(char), pNN->polyname); 
	}
	else
	{
		fIO.DoSaveDlg();
	}
	if (fIO.File != NULL) //MessageBox(NULL, fIO.File, _T("Hello"), MB_OK | MB_ICONINFORMATION);
	{
		fIO.OpenFileForWrite();
		if (fIO.hf == INVALID_HANDLE_VALUE) return;
		DWORD b_w; // bytes written
		BOOL rfw; // result from write
		char tID[] = "POLYGON2017"; //header
		rfw = WriteFile(fIO.hf, &tID, 11, &b_w, NULL);
		//unsigned int pNum = 1;
		rfw = WriteFile(fIO.hf, &saveNum, 4, &b_w, NULL);

		for (unsigned int pNum = 0; pNum < saveNum; ++pNum)
		{
			rfw = WriteFile(fIO.hf, &(selectedPolygon->numVertices), 4, &b_w, NULL);
			pVertexNode v = selectedPolygon->vNode;
			for (unsigned int i = 0; i < selectedPolygon->numVertices; ++i)
			{
				rfw = WriteFile(fIO.hf, &(v->vtxInfo.vertex), sizeof(CompGeo::XY), &b_w, NULL);
				v = v->next;
			}
			selectedPolygon = selectedPolygon->next;
		}
		selectedPolygon = wGonBuff;
	}
}
*/

void NGons::SavePolygonList(const string & fileName)
{
	ofstream nGonOUT;
	nGonOUT.open(fileName, ofstream::out | ofstream::trunc | ofstream::binary);
	char tBuff[20] = "", tID[] = "POLYGON2017";
	nGonOUT.write(tID, 11);
	unsigned int pNum = polygons->size();
	void * vI = &pNum;
	nGonOUT.write((const char *)vI, sizeof(unsigned int));
	for (vector<pPGonWork>::iterator it = polygons->begin(); it != polygons->end(); ++it)
	{
		unsigned int nV = (*it)->numVertices;
		vI = &nV;
		nGonOUT.write((const char *)vI, sizeof(unsigned int));
		pVertexNode v = (*it)->vNode;
		for (unsigned int j = 0; j < nV; ++j)
		{
			nGonOUT.write((const char *)&v->vtxInfo.vertex.x, sizeof(double));
			nGonOUT.write((const char *)&v->vtxInfo.vertex.y, sizeof(double));
			v = v->next;
		}
		
	}

}

pPGonFile NGons::GetSelectedPolygon(void)
{
	if (polygons == NULL) return NULL;
	if (polygons->size() == 0) return NULL;
	if (selectedPolygon == NULL) return NULL;
	return TranslateWorkPolygon(selectedPolygon);
}

pPGonFile NGons::GetFirstPolygon(void)  
	// sets indexPolygon to 0 on success
{
	if (polygons == NULL) return NULL;
	if (polygons->size() == 0) return NULL;
	selectedPolygon = polygons->at(0);
	indexPolygon = 0;
	//indexselectedPolygon = 0;
	//selectedVertex = selectedPolygon->vNode;
	return TranslateWorkPolygon(selectedPolygon);
}

pPGonFile NGons::GetNextPolygon(void)   
	// increments indexPolygon if successful
{
	if (selectedPolygon == NULL) return NULL;
	selectedPolygon = polygons->at(++indexPolygon);
	//selectedVertex = NULL;
	//if (selectedPolygon != NULL) selectedVertex = selectedPolygon->vNode;
	//++indexPolygon;
	//++indexselectedPolygon;
	return TranslateWorkPolygon(selectedPolygon);
}

pPGonFile NGons::GetNthPolygon(const int n)  
	// gets nth polygon [0 - (numPolygons-1)] indexPolygon=n
{
	if (polygons == NULL) return NULL;
	int maxP = polygons->size();
	if ((n < 0) || (n >= maxP)) return NULL;
	selectedPolygon = polygons->at(n);
	//selectedVertex = wGon->vNode;
	indexPolygon = n;
	//indexselectedPolygon = n;
	return TranslateWorkPolygon(selectedPolygon);
}

int NGons::SelectNthPolygon(const int n)  
	// selects nth polygon returning index of resulting selected polygon
{
	if (polygons == NULL) return -1;
	int maxP = polygons->size();
	if (maxP == 0) return -1;
	unsigned int a_n;
	if (n < 0) 
	{
		a_n = 0;
	}
	else
	{
		a_n = (unsigned int)n;
	}
	if (a_n >= maxP) a_n = maxP - 1;
	selectedPolygon = polygons->at(a_n);
	//selectedVertex = wGon->vNode;
	indexPolygon = a_n;
	//indexselectedPolygon = a_n;
	return a_n;
}

void NGons::RemovePolygon(void) 
	// removes selected Polygon from list
{
	if (selectedPolygon == NULL) return;
	int maxP = polygons->size();
	if (maxP <= 1)
	{
		DeletePolyList(&polygons->at(0));
		polygons->clear();
		delete polygons;
		polygons = NULL;
		indexPolygon = -1;
		selectedPolygon = NULL;
		return;
	}

	pPGonWork t = polygons->at(indexPolygon);
	t->pName->isdirty = true;
	DeletePolyList(&t);
	vector<pPGonWork>::iterator it = polygons->begin() + indexPolygon;
	polygons->erase(it);
	--indexPolygon;
	if (indexPolygon < 0) indexPolygon = 0;
	selectedPolygon = polygons->at(indexPolygon);

	SetSelVertex();
	//selectedVertex = NULL;
	//if (selectedPolygon != NULL) selectedVertex = selectedPolygon->vNode;
	//indexselectedPolygon = indexPolygon;

}

void NGons::RemoveAllPolygons(void)
{
	if (polygons == NULL) return;
	int maxP = polygons->size();
	if (maxP == 0) return;
	for (vector<pPGonWork>::iterator it = polygons->begin(); it != polygons->end(); ++it)
	{
		DeletePolyList(&(*it));
	}
	polygons->clear();
	delete polygons;
	polygons = NULL;

	if (names != NULL)
	{
		pNameNode pNN = NULL;
		for (vector<pNameNode>::iterator it = names->begin(); it != names->end(); ++it)
		{
			pNN = *it;
			delete pNN;
		}
		names->clear();
		delete names;
		names = NULL;
	}
	RemoveSelection();
	memPolyCount = 0;
	selectedPolygon = NULL;
	selVertex = NULL;
	indexPolygon = -1;
	//indexselectedPolygon = indexPolygon;
	//polygonList->numPolygons = 0;
	//polygonList->poly = NULL;
}

void NGons::SelectNthVertex(const int vIdx)
{
	if (selectedPolygon == NULL) return;
	selVertex = NULL;
	if (vIdx >= selectedPolygon->numVertices) return;
	selVertex = selectedPolygon->vNode;
	for (int i = 0; i < vIdx; ++i) 
	{
		selVertex = selVertex->next;
	}
	
}

unsigned int NGons::GetFirstSelectedVertex(void)
{
	if (selectedPolygon == NULL) return 0;
	selVertex = NULL;
	pVertexNode v_n = selectedPolygon->vNode;
	unsigned int i = 0;
	for (; i < selectedPolygon->numVertices; ++i) 
	{
		if (v_n->vtxInfo.is_selected)
		{
			selVertex = v_n;
			break;
		}
		v_n = v_n->next;
	}
	return i;
}

void NGons::AddVertexBefore(const CompGeo::XY v) 
	// adds a vertex to selected Polygon before selected Vertex
{
	if (selectedPolygon == NULL) return;
	if (selVertex == NULL) return;
	pVertexNode vL = selectedPolygon->vNode, vLTrail = NULL, 
		nv = new VertexNode; //MakeVertex();
	nv->PG = selectedPolygon;
	//nv->vtxInfo.is_selected = false;
	memcpy(&(nv->vtxInfo.vertex), &v, sizeof(CompGeo::XY));
	while (vL != selVertex)
	{
		vLTrail = vL;
		vL = vL->next;
	}
	if (vLTrail == NULL)  // insert at the beginning
	{
		nv->next = selectedPolygon->vNode;
		selectedPolygon->vNode = nv;
	}
	else
	{
		nv->next = vL;
		vLTrail->next = nv;
	}
	++(selectedPolygon->numVertices);
	selectedPolygon->pName->isdirty = true;
	//NumberVertices(selectedPolygon, false); list will get this
	//++(selectedPolygon->selVertex);
}

void NGons::AddVertexAfter(const CompGeo::XY v)
	// adds a vertex to selected Polygon after selected Vertex
{
	if (selectedPolygon == NULL) return;
	if (selVertex == NULL) return;  
	pVertexNode vL = selectedPolygon->vNode, vLNxt = NULL, 
		nv = new VertexNode; //MakeVertex();
	nv->PG = selectedPolygon;
	//nv->vtxInfo.is_selected = false;
	memcpy(&(nv->vtxInfo.vertex), &v, sizeof(CompGeo::XY));
	while (vL != selVertex)
	{
		vL = vL->next;
	}
	vLNxt = vL->next;
	nv->next = vLNxt;
	vL->next = nv;
	++(selectedPolygon->numVertices);
	selectedPolygon->pName->isdirty = true;

}

void NGons::UpdateVertex(const CompGeo::XY v)
{
	if (selectedPolygon == NULL) return;
	if (selVertex == NULL) return;
	pVertexNode vL = selVertex;
	//for (int i = 0; i < selectedPolygon->selVertex; ++i) vL = vL->next;
	CompGeo::XY vBuff = vL->vtxInfo.vertex;
	memcpy(&vL->vtxInfo.vertex, &v, sizeof(CompGeo::XY));
	if (!confirm2D()) 
	{
		memcpy(&vL->vtxInfo.vertex, &vBuff, sizeof(CompGeo::XY));
		return;
	}
	selectedPolygon->pName->isdirty = true;
}

void NGons::DeleteVertex(void)
{
	if (selectedPolygon == NULL) return;
	if (selVertex == NULL) return;
	if (selectedPolygon->numVertices <= 3) return;
	pVertexNode vL = selectedPolygon->vNode, vLTrail = NULL, vLNxt = NULL;
	//int i;
	while (vL != selVertex) 
	{
		vLTrail = vL;
		vL = vL->next;
	}
	vLNxt = vL->next;
	if (vLTrail == NULL)
	{
		selectedPolygon->vNode = vLNxt;
	}
	else
	{
		vLTrail->next = vLNxt;
	}
	if (!confirm2D())
	{
		if (vLTrail == NULL) selectedPolygon->vNode = vL;
		else vLTrail->next = vL;
		return;
	}
	RemoveSelection(vL);
	delete vL;
	--(selectedPolygon->numVertices);
	//selectedPolygon->selVertex = -1;
	SetSelVertex();
	/*
	while (vLNxt != NULL)
	{
		if (vLNxt->vtxInfo.is_selected)
		{
			selectedPolygon->selVertex = i;
			vLNxt = NULL;
		}
		else
		{
			vLNxt = vLNxt->next;
			++i;
		}
	}
	*/
	selectedPolygon->pName->isdirty = true;
}

pPGonFile NGons::TranslateWorkPolygon(pPGonWork wGon)
{
	if (wGon == NULL) return NULL;
	pPGonFile fGon = new PGonFile; //(pPGonFile)::operator new (sizeof(PGonFile));
	//fGon->numVertices = wGon->numVertices;
	fGon->pIdx = wGon->pIdx;
	fGon->pName = wGon->pName;
	//fGon->vertices = new VertexInfoType[fGon->numVertices];
	pVertexNode v = wGon->vNode;
	for (unsigned int i = 0; i < wGon->numVertices; ++i)
	{
		fGon->vertices.push_back(v->vtxInfo);
		//memcpy(&(fGon->vertices[i]), &(v->vtxInfo), sizeof(VertexInfoType));
		v = v->next;
	}
	return fGon;
}

pPGonWork NGons::TranslateFilePolygon(pPGonFile fGon)
{
	if (fGon == NULL) return NULL;
	pPGonWork wGon = new PGonWork; //MakePGon();
	//wGon->selVertex = -1;
	wGon->numVertices = fGon->vertices.size();
	wGon->pIdx = fGon->pIdx;
	wGon->cIdx = 0;
	wGon->pName = fGon->pName;
	//wGon->next = NULL;
	pVertexNode v_trail = NULL, v = NULL;
	for (unsigned int i = 0; i < wGon->numVertices; ++i)
	{
		v = new VertexNode; //MakeVertex();
		v->PG = wGon;
		memcpy(&(v->vtxInfo),&(fGon->vertices.at(i)), sizeof(VertexInfoType));
		v->vIdx = i;
		//if ((wGon->selVertex == -1) && (v->vtxInfo.is_selected)) wGon->selVertex = i;
		if (i == 0) 
		{
			wGon->vNode = v;
		}
		else
		{
			v_trail->next = v;
		}
		v->next = NULL;
		v_trail = v;

	}
	return wGon;
}


void NGons::DeleteFilePolygon(pPGonFile * pdelpol)
{
	
	pPGonFile delpol = *pdelpol;
	delpol->vertices.clear();
	//if (delpol->vertices != NULL) delete [] delpol->vertices;
	delete delpol;

}

void NGons::SetSelection(int * selArray, int selCount) 
	// sets selection flags in vertex list based on array and count
{
	if (selectedPolygon == NULL) return;
	int count = 0;
	pVertexNode v = selectedPolygon->vNode;
	//selectedPolygon->selVertex = -1;
	pSelectedVertexType svt = NULL, svt_trail = NULL, svt_nxt = NULL;
	for (unsigned int i = 0; i < selectedPolygon->numVertices; ++i)
	{
		v->vtxInfo.is_selected = false;
		if (selArray != NULL)
		{
			if (i == selArray[count])
			{
				if (selVertex == NULL) selVertex = v;
				v->vtxInfo.is_selected = true;
				count = (count + 1) % selCount;
				if (svt_trail == NULL) svt_trail = SetSelection(v, selectedPolygon);
				else
				{
					svt = MakeSelectedVertex();
					svt_nxt = svt_trail->next;
					svt->next = svt_nxt;
					svt->prev = svt_trail;
					svt->sp = selectedPolygon;
					svt->sv = v;
					svt_trail->next = svt;
					svt_trail = svt;
					if (svt_nxt != NULL) svt_nxt->prev = svt;
				}
			}
		}
		v = v->next;
	}
}

bool NGons::RotateVertex(CompGeo::XY & vtx, const CompGeo::XY & ctrXY, const double & degrees)
{
	CompGeo::XY delta = vtx - ctrXY;
	double pi = M_PI, radians = degrees * pi / 180.0,
		dY = delta.y, dX = delta.x, theta = pi / 2.0, 
		radius = sqrt(sqr(dX) + sqr(dY));
	if (fabs(dX) < MAX_FLT_PRECISION) 
	{
		if (dY < 0.0) theta *= 3.0;
	}
	else
	{
		if (fabs(dY) < MAX_FLT_PRECISION)
		{
			theta = pi;
			if (dX > 0.0) theta = 0.0;
		}
		else
		{
			double phi = atan(fabs(dY / dX));
			if (dY > 0)
			{
				if (dX > 0) theta = phi;
				else theta = pi - phi;
			}
			else
			{
				if (dX > 0) theta = -phi;
				else theta = pi + phi;
			}
		}

	}
	vtx.x = ctrXY.x + radius * cos(radians + theta);
	vtx.y = ctrXY.y + radius * sin(radians + theta);
	/*
	if(!confirm2D()) 
	{
		vtx.x = ctrXY.x - radius * cos(radians + theta);
		vtx.y = ctrXY.y - radius * sin(radians + theta);
		return false;
	}
	*/
	return true;

}

void NGons::RotateSelected(const CompGeo::XY & ctr, double degrees)
	// rotate selected vertices around this point by these degrees
{
	/*
	pSelectedVertexType svL = selVertices;
	while (svL != NULL)
	{
		RotateVertex(svL->sv->vtxInfo.vertex, ctr, degrees);
		svL = svL->next;
	}
	*/
	for (vector<pPGonWork>::iterator it = polygons->begin(); it != polygons->end(); ++it)
	{
		pPGonWork wGon = *it;
		pVertexNode v_n = wGon->vNode;
		for (unsigned int i = 0; i < wGon->numVertices; ++i)
		{
			if (v_n->vtxInfo.is_selected) RotateVertex(v_n->vtxInfo.vertex, ctr, degrees);
			v_n = v_n->next;
		}
	}
}

bool NGons::MoveVertex(CompGeo::XY & vtx, const CompGeo::XY disp)
{
	vtx.x += disp.x;
	vtx.y += disp.y;
	/*
	if (!confirm2D()) 
	{
		vtx.x -= disp.x;
		vtx.y -= disp.y;
		return false;
	}
	*/
	return true;
}

void NGons::MoveSelected(const CompGeo::XY disp)
	// move selected vertices by this displacement (delta x, delta y)
{
	/*
	pSelectedVertexType svL = selVertices;
	while (svL != NULL)
	{
		MoveVertex(svL->sv->vtxInfo.vertex, disp);
		svL = svL->next;
	}
	*/
	for (vector<pPGonWork>::iterator it = polygons->begin(); it != polygons->end(); ++it)
	{
		pPGonWork wGon = *it;
		pVertexNode v_n = wGon->vNode;
		for (unsigned int i = 0; i < wGon->numVertices; ++i)
		{
			if (v_n->vtxInfo.is_selected) MoveVertex(v_n->vtxInfo.vertex, disp);
			v_n = v_n->next;
		}
	}

}

void NGons::AddNGons(const vector<pPGonWork> & wGons)
{
	int pMax = wGons.size();
	if (pMax == 0) return;
	if (polygons == NULL) polygons = new vector<pPGonWork>;
	pPGonWork wGon = NULL;
	for (vector<pPGonWork>::const_iterator it = wGons.begin(); it != wGons.end(); ++it)
	{
		polygons->push_back(*it);
		wGon = polygons->at(polygons->size() - 1);
		if (wGon->pName == NULL) wGon->pName = GetNewNameNode(GetNextMemName("mem_poly"), false);
		else
		{
			//string pnameIn = wGon->pName->polyname;
			//delete wGon->pName;
			wGon->pName = GetNewNameNode(GetNextMemName(wGon->pName->polyname), false);
		}
	}
	selectedPolygon = wGon;
	indexPolygon = polygons->size() - 1;

}

void NGons::AddNGon(pPGonWork wGon)
{
	if (wGon == NULL) return;
	if (polygons == NULL) polygons = new vector<pPGonWork>;
	polygons->push_back(wGon);

	if (wGon->pName == NULL) wGon->pName = GetNewNameNode(GetNextMemName("mem_poly"), false);
	else
	{
		//string pnameIn = wGon->pName->polyname;
		//delete wGon->pName;
		wGon->pName = GetNewNameNode(GetNextMemName(wGon->pName->polyname), false);
	}
	//++(polygonList->numPolygons);
	selectedPolygon = wGon;
	indexPolygon = polygons->size() - 1;

	bool goodAdd = true;
	string errorMsg = "";
	if (!confirm2D()) 
	{
		goodAdd = false;
		errorMsg = "Error please fix, not a 2 dimensional polygon.  ";
		//MessageBox(NULL, TEXT("Not a 2 dimensional polygon"), TEXT("Error please fix"), MB_OK);
	}
	if (!enforceCC()) 
	{
		goodAdd = false;
		errorMsg += "Bad Form Polygon: cannot order counterclockwise.";
		//MessageBox(NULL, TEXT("Cannot order counterclockwise"), TEXT("Bad Form Polygon"), MB_OK);
	}
	if (!goodAdd)
	{
		pGEXT pG = GEXT::getInst();
		Gtk::Label * sBar;
		pG->get("StatusBar", sBar);
		sBar->set_label(errorMsg.c_str());
		sBar->show_all();
	}
	NumberVertices();

}

bool NGons::confirm2D(void)
{// edges cannot all be colinear

	if (selectedPolygon == NULL) return false;
	pPGonWork wGon = selectedPolygon;
	if (wGon->numVertices < 3) return false;
	pVertexNode pv = wGon->vNode;
	CompGeo::XY v0(pv->vtxInfo.vertex);
	pv = pv->next;
	CompGeo::XY v1(pv->vtxInfo.vertex), A = v1;
	A -= v0;
	double k;
	do
	{
		pv = pv->next;
		if (pv != NULL)
		{
			CompGeo::XY vn(pv->vtxInfo.vertex), B = vn;
			B -= v0;
			k = CompGeo::Cross(A, B);
			if (fabs(k) >= MAX_FLT_PRECISION) return true;
		}
	} while (pv != NULL);
	return false;
}

bool NGons::enforceCC(void) 
{// counterclockwise ordering if it cannot be enforced returns false

	if (polygons == NULL) return false;
	int maxP = polygons->size();
	if (maxP == 0) return false;
	pPGonWork wGon = NULL;
	for(vector<pPGonWork>::iterator it = polygons->begin(); it != polygons->end(); ++it)
	{
		wGon = *it;
		double optima[4]; // xlo, xhi, ylo, yhi
		pVertexNode trails[4], // trailing node for each of the optima
			pv = wGon->vNode, pvTrail = NULL, pvNext = NULL;
		while (pv != NULL)
		{
			pvTrail = pv;
			pv = pv->next;
		}
		pv = wGon->vNode;
		CompGeo::XY xy(pv->vtxInfo.vertex);
		for (int i = 0; i < 4; ++i)
		{
			optima[i] = (i < 2)? xy.x: xy.y;
			trails[i] = pvTrail;
		}
		while (pv != NULL)
		{
			xy = pv->vtxInfo.vertex;
			if (xy.x < optima[0]) 
			{//update xlo?
				optima[0] = xy.x;
				trails[0] = pvTrail;
			}
			if (xy.x > optima[1])
			{//update xhi?
				optima[1] = xy.x;
				trails[1] = pvTrail;
			}
			if (xy.y < optima[2])
			{//update ylo?
				optima[2] = xy.y;
				trails[2] = pvTrail;
			}
			if (xy.y > optima[3])
			{//update yhi?
				optima[3] = xy.y;
				trails[3] = pvTrail;
			}
			pvTrail = pv;
			pv = pv->next;
		}
		double c_p[4];  // cross products
		bool allNeg = true, allPos = true;
		for (int i = 0; i < 4; ++i)
		{
			pVertexNode vp = trails[i], v = vp->next, vn = NULL;
			if (v == NULL) v = wGon->vNode;
			vn = v->next;
			if (vn == NULL) vn = wGon->vNode;
			CompGeo::XY A(v->vtxInfo.vertex), B(vn->vtxInfo.vertex);
			A -= vp->vtxInfo.vertex;
			B -= vp->vtxInfo.vertex;
			c_p[i] = CompGeo::Cross(A, B);
			allNeg = (allNeg && (c_p[i] < 0.0));
			allPos = (allPos && (c_p[i] > 0.0));
		}
		if (allPos == allNeg) return false;  // suspect intersections
		if (allNeg)
		{// vertices are clockwise, need to reverse order:
			pVertexNode pvn = NULL, pvnRoot = NULL;
			pv = wGon->vNode;
			while (pv != NULL)
			{
				pvNext = pv->next;
				pvn = new VertexNode; //MakeVertex(); //(pVertexNode)new VertexNode));
				pvn->PG = wGon;
				pvn->next = pvnRoot;
				pvn->vtxInfo.is_selected = pv->vtxInfo.is_selected;
				pvn->vtxInfo.vertex = pv->vtxInfo.vertex;
				pvnRoot = pvn;
				delete pv;
				pv = pvNext;
			}
			wGon->vNode = pvnRoot;
		}
		//wGon = wGon->next;
	}
	return true;

}

CompGeo::pXY NGons::VertexDump(int & v_Count)
{
	v_Count = 0;
	if (polygons == NULL) return NULL;
	int maxP = polygons->size();
	if (maxP == 0) return NULL;

	pPGonWork wGon = NULL;
	pPGonFile * pf = new pPGonFile[maxP];
	for (unsigned int i = 0; i < maxP; ++i)
	{
		wGon = polygons->at(i);
		pf[i] = TranslateWorkPolygon(wGon);
		v_Count += wGon->numVertices;
	}
	CompGeo::pXY vd = new CompGeo::XY[v_Count];
	unsigned int j = 0, polyVertsSoFar = 0;
	for (unsigned int i = 0; i < (unsigned int)v_Count; ++i)
	{
		unsigned int m = i - polyVertsSoFar;
		if (m >= pf[j]->vertices.size())
		{
			m = 0;
			polyVertsSoFar += pf[j]->vertices.size();
			DeleteFilePolygon(&pf[j]);
			++j;
		}
		vd[i] = pf[j]->vertices.at(m).vertex;
	}
	if (v_Count > 0) DeleteFilePolygon(&pf[j]);
	delete [] pf;
	return vd;
}

void NGons::RemoveSelection(void)
{
	if (polygons == NULL) return;
	for (vector<pPGonWork>::iterator it = polygons->begin(); it != polygons->end(); ++it)
	{
		pPGonWork wGon = *it;
		pVertexNode v_n = wGon->vNode;
		for(unsigned int i = 0; i < wGon->numVertices; ++i)
		{
			v_n->vtxInfo.is_selected = false;
			v_n = v_n->next;
		}
	}
	selVertex = NULL;
/*
	pSelectedVertexType p_sv = NULL;

	while (selVertices != NULL)
	{
		p_sv = selVertices->next;
		pVertexNode v_n = selVertices->sv;
		v_n->vtxInfo.is_selected = false;
		v_n->s = NULL;
		delete selVertices;
		selVertices = p_sv;
	}
	selVertex = NULL; 
*/

}

void NGons::RemoveSelection(pVertexNode vn)
{ // does not reset selVertex
	if (vn == NULL) return;

	pSelectedVertexType p_sv = vn->s, p_trail = p_sv->prev, p_nxt = p_sv->next;
	vn->s = NULL;
	vn->vtxInfo.is_selected = false;
	if (p_trail != NULL) p_trail->next = p_nxt;
	if (p_nxt != NULL) p_nxt->prev = p_trail;
	delete p_sv;

}

void NGons::SetSelVertex(void)
{  // selVertex is set to first selected vertex in selected Polygon
	selVertex = NULL;
	if ((selectedPolygon == NULL) || (selVertices == NULL)) return;
	pVertexNode v = selectedPolygon->vNode;
	for (unsigned int i = 0; i < selectedPolygon->numVertices; ++i)
	{
		if (v->vtxInfo.is_selected)
		{
			selVertex = v;
			return;
		}
		v = v->next;
	}
}

pSelectedVertexType NGons::MakeSelectedVertex(void)
{
	pSelectedVertexType sv = new SelectedVertexType;
	sv->next = NULL;
	sv->prev = NULL;
	sv->sp = NULL;
	sv->sv = NULL;
	return sv;
}

pSelectedVertexType NGons::SetSelection(pVertexNode vn, pPGonWork wGon)
{ // the selected vertex list will be in order by polygon address and then vertex index
	pSelectedVertexType sL = selVertices, sIns = MakeSelectedVertex(), sLTrail = NULL, sLNxt = NULL;
	sIns->sp = wGon;
	sIns->sv = vn;
	while (sL != NULL)
	{
		if (sL->sp == wGon)
		{
			while (sL->sv->vIdx < vn->vIdx)
			{
				sLTrail = sL;
				sL = sL->next;
				bool StopHere = (sL == NULL);
				if (!StopHere) StopHere = (sL->sp != wGon);
				if (StopHere)
				{
					sIns->next = sL;
					sIns->prev = sLTrail;
					sLTrail->next = sIns;
					if (sL != NULL) sL->prev = sIns;
					return sIns;
				}
			}
			if (sLTrail == NULL)
			{ // insert at the head of the list:
				sIns->next = selVertices;
				selVertices = sIns;
				return sIns;
			}
			else
			{
				sIns->next = sL;
				sIns->prev = sLTrail;
				sLTrail->next = sIns;
				sL->prev = sIns;
				return sIns;
			}
		}
		else
		{
			if (wGon < sL->sp)
			{
				if (sLTrail == NULL)
				{ // insert at the head of the list:
					sIns->next = selVertices;
					selVertices = sIns;
					return sIns;
				}
				else
				{
					sIns->next = sL;
					sIns->prev = sLTrail;
					sLTrail->next = sIns;
					sL->prev = sIns;
					return sIns;
				}
			}
		}
		sLTrail = sL;
		sL = sL->next;
	}
	// sL is NULL:
	if (sLTrail == NULL)
	{ // insert at the head of the list:
		sIns->next = selVertices; // selVertices here is NULL too
		selVertices = sIns;
		return sIns;
	}
	else
	{
		sIns->next = sL;
		sIns->prev = sLTrail;
		sLTrail->next = sIns;
		//sL->prev = sIns;
		return sIns;
	}

}

/*
void NGons::CheckMinMax(pVertexNode pvn)
{  // resets min max members if necessary and updates MinMaxChanged
	if (pvn == NULL) return;
	CompGeo::XY xy = pvn->vtxInfo.vertex;
	bool foundChange = false;
	if (xy.x < xMin)
	{
		foundChange = true;
		xMin = xy.x;
	}
	if (xy.x >	xMax)
	{
		foundChange = true;
		xMax = xy.x;
	}
	if (xy.y < yMin)
	{
		foundChange = true;
		yMin = xy.y;
	}
	if (xy.y > yMax)
	{
		foundChange = true;
		yMax = xy.y;
	}
	MinMaxChanged = MinMaxChanged || foundChange;
}
	
void NGons::ResetMinMax(void)
{

}
*/


void NGons::NumberVertices(bool NumberAll)
{
	if (polygons == NULL) return;
	if (polygons->size() == 0) return;
	pPGonWork p = NULL;
	for(vector<pPGonWork>::iterator it = polygons->begin() + indexPolygon; it != polygons->end(); ++it)
	//while (p != NULL)
	{
		p = *it;
		pVertexNode pvn = p->vNode;
		for (unsigned int i = 0; i < p->numVertices; ++i)
		{
			pvn->vIdx = i;
			pvn = pvn->next;
		}
		if (!NumberAll) return;
		//p = p->next;
	}
}

void NGons::SituatePlane(const CompGeo::XYZ & origin)
{ // not useful we would never have this origin
	double h = sqr(origin.x) + sqr(origin.y);
	M = sqrt(h + sqr(origin.z));
	h = sqrt(h);
	if (N != NULL) delete N;
	if (I != NULL) delete I;
	if (J != NULL) delete J;
	N = new CompGeo::XYZ(origin);
	*N /= M;
	bool pEql = (*N == CompGeo::WorldType::W_K_HAT),
		nEql = (*N == (-1.0 * CompGeo::WorldType::W_K_HAT));

	if (pEql || nEql)
	{
		double s = pEql? 1.0: -1.0;
		I = new CompGeo::XYZ(s, 0.0, 0.0);
		J = new CompGeo::XYZ(0.0, 1.0, 0.0);
	}
	else
	{
		// these are based on counterclockwise rotations about y & then z'
		I = new CompGeo::XYZ(N->x * N->z / h, N->y * N->z / h, -h);
		J = new CompGeo::XYZ(-N->y / h, N->x / h, 0.0);
	}
}

void NGons::SituatePlane(const CompGeo::XYZ & norm, const CompGeo::XYZ & POP)
{ // norm is N, POP is Point On Plane
	assert(fabs(norm.GetMagnitude() - 1.0) < MAX_FLT_PRECISION);
	M = POP * norm; // dot product
	CompGeo::XY ij = CompGeo::XY(norm.x, norm.z);
	double w = ij.GetMagnitude();
	if (N != NULL) delete N;
	if (I != NULL) delete I;
	if (J != NULL) delete J;
	N = new CompGeo::XYZ(norm);
	// I & J are based on rotating k to N
	// which is -alpha about the X axis: cos(alpha) is w, sin(alpha) is norm.y
	// followed by +phi about the Y axis: cos(phi) is norm.z/w or 1, sin(phi) is norm.x/w or 0
	double Z = fabs(w) < MAX_FLT_PRECISION? 1.0: norm.z / w,
		X = fabs(w) < MAX_FLT_PRECISION? 0.0: norm.x / w;
	I = new CompGeo::XYZ(Z, 0.0, X);
	J = new CompGeo::XYZ(-norm.y * X, w, -norm.y * Z);

}

CompGeo::XY NGons::GetXY(const CompGeo::pXYZ & wPt) // transform world point to plane
{
	assert((N != NULL) && (I != NULL) && (J != NULL));
	CompGeo::XYZ xyz = *wPt, origin = M * (*N), i = *I, j = *J;
	xyz -= origin;
	return CompGeo::XY(i * xyz, xyz * j);  // dot products
}

void NGons::Translate3DPlanePoints(const CompGeo::XYZ & norm, const CompGeo::pXYZ & pts, const int & numPts)
{
	if (numPts <= 3) return; // goal is to triangularize pts
	if (pts == NULL) return;
	if (fabs(norm.GetMagnitude() - 1.0) > MAX_FLT_PRECISION) return;  // must be normalized vector
	RemoveAllPolygons();
	SituatePlane(norm, *pts);

	polygons = new vector<pPGonWork>;
	pPGonWork wGon = new PGonWork; //MakePGon();
	wGon->numVertices = numPts;
	wGon->pName = GetNewNameNode(GetNextMemName("3D_poly"), false);
	pVertexNode v_trail = NULL, v = NULL;

	for (unsigned int i = 0; i < numPts; ++i)
	{
		v = new VertexNode; //MakeVertex();
		v->PG = wGon;
		v->vtxInfo.vertex = GetXY(pts + i);
		if (i == 0) 
		{
			wGon->vNode = v;
		}
		else
		{
			v_trail->next = v;
		}
		v_trail = v;
	}
	polygons->push_back(wGon);
	/*
	++(polygonList->numPolygons);
	if (polygonList->poly == NULL)
	{
		polygonList->poly = wGon;
	}
	else
	{
		pPGonWork p_trail = polygonList->poly, p = polygonList->poly->next;
		while (p != NULL)
		{
			p_trail = p;
			p = p->next;
		}
		p_trail->next = wGon;
	}
	*/
	selectedPolygon = wGon;
	indexPolygon = polygons->size() - 1;
	NumberVertices();


}

void NGons::SaveFloat(void)
{
	float f = 511.0f / 512.0f;
	ofstream floatlog;
	string fNme = "flttst1.dat";
	floatlog.open(fNme, ios_base::trunc | ios_base::out);
	floatlog << "FLOATTEST_1" << f;
	floatlog.close();
	//FileIO fIO;
	//char fNme[] = _T("F:\\Tests\\flttst1.dat");
	//fIO.OpenFileForWrite(fNme);
	//if (fIO.hf == INVALID_HANDLE_VALUE) return;
	//DWORD b_w; // bytes written
	//BOOL rfw; // result from write
	//char tID[] = "FLOATTEST_1";
	//rfw = WriteFile(fIO.hf, &tID, 11, &b_w, NULL);
	//rfw = WriteFile(fIO.hf, &f, 4, &b_w, NULL);
	
}