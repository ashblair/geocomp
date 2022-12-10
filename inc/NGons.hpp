#pragma once

//using namespace std;

typedef struct NameNode *pNameNode;
struct NameNode
{
	std::string polyname;
	bool isfile, isdirty;
	//pNameNode nxtName;
};

struct VertexInfoType
{
	//D2D1_POINT_2F vertex;
	CompGeo::XY vertex;
	bool is_selected, moused;
	double depth;  // for 3D imaging
}; 

typedef struct SelectedVertexStructType * pSelectedVertexType;

typedef struct PGonWork * pPGonWork;

typedef struct VertexNodeStruct * pVertexNode; 

typedef class NGons Plane, * pPlane;

typedef struct VertexNodeStruct
{
	VertexNodeStruct(void);
	VertexNodeStruct(const VertexNodeStruct &);
	~VertexNodeStruct(void);
	VertexInfoType vtxInfo;
	pVertexNode next;
	pSelectedVertexType s;
	int vIdx;  // for listbox indexing
	pPGonWork PG, OEPG;  // Other Edge Polygon - all edges connect exactly 2 polygons in 3D printing
} VertexNode;





typedef struct SelectedVertexStructType
{ // order: sp (by address) then sv->vIdx
	pPGonWork sp;
	pVertexNode sv;
	pSelectedVertexType next, prev;
} SelectedVertexType;

struct PGonWork
{
	PGonWork(void);
	PGonWork(const PGonWork &);
	~PGonWork(void);
	//int * selV; // selected vertices
	//unsigned int numselV;
	pNameNode pName;
	unsigned int pIdx; // for files w/ multiple polygons, this is the polygon number
	int cIdx; // for combobox, this corresponds w/ the list index 
	unsigned int numVertices; // this must never be less than 3
	//int selVertex;  // -1 means no selection o/w top vertex selected
	pVertexNode vNode;
	//pPGonWork next;
	pPlane plane;
	CompGeo::pBoundingBox bb;
};

typedef struct PGonFile * pPGonFile;
struct PGonFile
{
	//int * selV;
	//unsigned int numselV;
	pNameNode pName;
    unsigned int pIdx; // for files w/ multiple polygons, this is the polygon number
	std::vector<VertexInfoType> vertices;
	//unsigned int numVertices; // this must never be less than 3
	//VertexInfoType * vertices; // array
};

/*
typedef struct PGonList * pPGonList;
struct PGonList 
{
	//unsigned int numPolygons;
	//pPGonWork poly;
	std::vector<pPGonWork> poly;
};
*/

class NGons
{
public:
	static unsigned int memPolyCount;
	//static pVertexNode MakeVertex(void); // makes vertex node with vertex information initializes to NULL or 0
	//static pPGonWork MakePGon(void);

	NGons(void);
	~NGons(void);
	/* CalcNGon adds a regular n-gon to polygon list 
	   in: number of sides, radius of circumscribing circle, center*/
	void CalcNGon(const int, const float, const CompGeo::XY);
	void AddNGon(pPGonWork);
	void AddNGons(const std::vector<pPGonWork> &);
	void LoadNGon(const std::string &); //from file
	//void SaveNGon(void); // to file selected polygon
	void SavePolygonList(const std::string &); //to file all polygons
	pPGonFile GetSelectedPolygon(void); 
	pPGonFile GetFirstPolygon(void);  // sets indexPolygon to 0 on success
	pPGonFile GetNextPolygon(void);   // increments indexPolygon if successful
	pPGonFile GetNthPolygon(const int);  // gets nth polygon [0 - (numPolygons-1)] indexPolygon=n
	int SelectNthPolygon(const int);  // selects nth polygon returning index of resulting selected polygon
	void RemovePolygon(void); // removes selected Polygon from list
	void RemoveAllPolygons(void);
	void SelectNthVertex(const int);
	unsigned int GetFirstSelectedVertex(void);
	void AddVertexBefore(const CompGeo::XY); // adds a vertex to selected Polygon before selected Vertex
	void AddVertexAfter(const CompGeo::XY); // adds a vertex to selected Polygon after selected Vertex
	void UpdateVertex(const CompGeo::XY); // changes selected Vertex to new value
	void DeleteVertex(void); // deletes selected Vertex
	void DeleteFilePolygon(pPGonFile *); // deletes file polygon
	void DeletePolyList(pPGonWork *); // deletes poly list
	void SetSelection(int *, int); // sets selection flags in vertex list based on array and count
	void RotateSelected(const CompGeo::XY &, double); // selected around this point by these degrees
	void MoveSelected(const CompGeo::XY); // selected by this displacement (delta x, delta y)
	bool confirm2D(void); // edges cannot all be colinear
	bool enforceCC(void); // counterclockwise ordering
	CompGeo::pXY VertexDump(int &);
	double sqr(double a) {return a * a;};
	
	std::vector<pNameNode> * names;
	std::vector<pPGonWork> * polygons;
	pPGonWork selectedPolygon;
	pVertexNode selVertex;  // selected vertex within selectedPolygon, if later is NULL this s/b also
	int indexPolygon, * mVerts, mVertcount;
	pSelectedVertexType selVertices;
	CompGeo::pBoundingBox pBB;

	// members relating this plane to the world:
	double M; // M is the multiplier
	CompGeo::pXYZ N, // this is the normal, it's magnitude is 1, the origin for this plane is M*N
		I, J;  // these are the basis vectors for this plane, their magnitude is 1

	pPGonFile TranslateWorkPolygon(pPGonWork); //changes format of a polygon
	pPGonWork TranslateFilePolygon(pPGonFile); 
	std::string GetNextMemName(const std::string &);
	pNameNode GetNewNameNode(const std::string &, bool);
	void RemoveSelection(void);
	void RemoveSelection(pVertexNode);
	void SetSelVertex(void);
	pSelectedVertexType SetSelection(pVertexNode, pPGonWork);  // adds to selVertices list
	pSelectedVertexType MakeSelectedVertex(void);
	bool RotateVertex(CompGeo::XY &, const CompGeo::XY &, const double &);
	bool MoveVertex(CompGeo::XY &, const CompGeo::XY);
	//void CheckMinMax(pVertexNode);
	//void ResetMinMax(void);
	void NumberVertices(bool = true);

	// methods relating this plane to the world:
	void SituatePlane(const CompGeo::XYZ &); // world origin of plane such that normal is parallel
	void SituatePlane(const CompGeo::XYZ &, const CompGeo::XYZ &); //normal & point on plane
	CompGeo::XY GetXY(const CompGeo::pXYZ &); // transform world point to plane
	void Translate3DPlanePoints(const CompGeo::XYZ &, const CompGeo::pXYZ &, const int &); // normal w/ points array & size thereof
	// tests
	void SaveFloat(void);
};