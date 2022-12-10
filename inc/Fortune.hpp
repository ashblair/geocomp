#pragma once

namespace Fortune
{
	// inputs:
		class SitePoint: public CompGeo::XY
		{
		public:
			SitePoint(void) {x = 0.0; y = 0.0; v_Node = NULL; w_Gon = NULL;}
			SitePoint(CompGeo::XY a) {x = a.x; y = a.y; v_Node = NULL; w_Gon = NULL;}
			SitePoint(const SitePoint & a) {x = a.x; y = a.y; v_Node = a.v_Node; w_Gon = a.w_Gon;}
			friend bool operator<(const SitePoint&a, const SitePoint&b) 
				{return fabs(a.y - b.y) < MAX_FLT_PRECISION ? a.x < b.x: a.y > b.y;}
			
			// modify these next 2 for viewer:
			pVertexNode v_Node; // pointer to original polygon's vertex
			pPGonWork w_Gon; // pointer to original polygon

		};

		typedef SitePoint * pSitePoint;
	
		typedef struct SitePointListStructType * pSitePointListType;	
	
		typedef struct SitePointListStructType
		{
			SitePoint site;
			pSitePointListType next;

		} SitePointListType;

		//pSitePointListType SitePointRoot; 

		typedef struct BoundingBoxStructType
		{
			double left, right, top, bottom;

		} BoundingBoxType, *pBoundingBoxType;

		// all duplicate site points and points outside of the Bounding Box will be deleted
		// site points will then be arrayed and sorted inside class Voronoi

	// end inputs.

	typedef struct CellStructType *pCellType; 
	
	typedef struct HalfEdgeStructType * pHalfEdgeType;
	
	typedef struct HalfEdgeListStructType * pHalfEdgeListType;

	typedef struct VertexListStructType *pVertexListType;

	typedef struct VertexStructType
	{
		SitePoint xy;
		pHalfEdgeType IncidentEdge;
	} VertexType, * pVertexType;

	typedef struct VertexListStructType
	{
		VertexType vertex;
		//pHalfEdgeType IncidentEdge;  // probably one of 3, this will be NULL for site points 
		pVertexListType next;

	} VertexListType;

	typedef class VertexPoint * pVertexPoint;

	typedef struct VertexListOfListStructType * pVertexListOfListType;

	typedef struct VertexListOfListStructType
	{
		pVertexListType vListElement;
		pVertexListOfListType next;

	} VertexListOfListType;

	typedef struct HalfEdgeListStructType
	{
		pHalfEdgeType halfEdge;
		pHalfEdgeListType next;

	} HalfEdgeListType;

	typedef struct HalfEdgeStructType
	{
		int flag;	// possible use: drawing - initially all set to N, 
					// if N, draw edge, set to N+1 this and twin
		pVertexType origin;
		pHalfEdgeType twin, next, prev;
		pCellType IncidentCell;
		
	} HalfEdgeType;

	class VertexPoint: public SitePoint
	{ // this is to sort the vertices to help clean them up
	public:
		pHalfEdgeType IE;
		pVertexListType vl, vl_prev;
		VertexPoint(void) {x = 0.0; y = 0.0; IE = NULL; vl = NULL; vl_prev = NULL;};
		VertexPoint(pVertexListType vL) {x = vL->vertex.xy.x; y = vL->vertex.xy.y; 
			IE = vL->vertex.IncidentEdge; vl = vL; vl_prev = NULL;};
		VertexPoint(pVertexListType vL, pVertexListType vLPrev) {x = vL->vertex.xy.x; y = vL->vertex.xy.y; 
			IE = vL->vertex.IncidentEdge; vl = vL; vl_prev = vLPrev;};
		VertexPoint & operator =(const VertexPoint & a) {x = a.x; y = a.y; 
			IE = a.IE; vl = a.vl; vl_prev = a.vl_prev; 
			return *this;};
	};

	typedef struct CellStructType
	{
		char ID; // for debug
		pSitePoint site;  // ordinal/identifier
		pHalfEdgeType OuterComponent;
		pHalfEdgeListType InnerComponents;  // all perimeter halfedges initially are here

	} CellType;

	typedef struct DuplicateStructType * pDupType;

	typedef struct DuplicateStructType
	{
		SitePoint xy;
		pHalfEdgeType h_prev, h_next;
		pHalfEdgeListType dups, singles;
		pDupType next_d;
		pVertexListType vtxLstElement;
	} DupType;

	typedef struct CellListStructType * pCellListType;

	typedef struct CellListStructType
	{
		pCellType cell;
		pCellListType next;

	} CellListType;

	typedef struct DiagramStructType
	{
		pCellListType cells; // top cell is NULL cell i.e. exterior cell
		pHalfEdgeListType halfedges;
		pVertexListType vertices;

	} DiagramType, * pDiagramType;

	typedef struct ArcStructType * pArcType;

	typedef struct BeachPointStructType
	{
		char LeafFlag;  // on the beach line 'b' is for before Leaf or -infinity arcLeft is NULL
						// 'a' is for after Leaf or +infinity arcRight is NULL
						// 'n' is for normal Leaf arcLeft and arcRight are non-NULL
		pArcType arcLeft, arcRight; // parabolic intersection w/ 2 foci and a directrix
		// when you add two of these create twinned halfedges
		pHalfEdgeType halfEdge;

	} BeachPointType, * pBeachPointType;

	typedef class EventQPoint * pEventQPoint;

	typedef struct ArcStructType
	{ // arcs go below the leafs/beach points, 2 arcs define internal points
		char ArcFlag;  // as w/ LeafFlag above {'b', 'a', 'n'}
		double order;  // gives order on beachline
		pCellType cell;  // focus is cell->site
		// when processing a new site point, initialize above members, set below members to NULL
		pEventQPoint circleEvent;
		pArcType next, prev; // doubly linked list is the Beach Line
		pBeachPointType leftBP, rightBP;// initial arc left is NULL, final arc right is NULL
										// o/w this arc = leftBP->arcRight & rightBP->arcLeft 

	} ArcType;


	class EventQPoint: public CompGeo::XY
	{
	public:
		char type; // S == site point, C == circle point, N == not a point use for animation
		pArcType arc; // the arc that is vanishing for a circle point
		pSitePoint site;  // the site point
		//double r;  // radius for circle points
		EventQPoint(void) {x = 0.0; y = 0.0; type = 'S'; arc = NULL; site = NULL;}
		EventQPoint(const CompGeo::XY & a) { x = a.x; y = a.y; type = 'S'; arc = NULL; site = NULL; }
		EventQPoint(SitePoint & a) { x = a.x; y = a.y; type = 'S'; arc = NULL; site = &a;}
		// read order top-bottom, left-right w/ circle points first for otherwise equal points
		friend bool operator==(const EventQPoint&a, const EventQPoint&b)
		{
			return (fabs(a.x - b.x) < MAX_FLT_PRECISION) && (fabs(a.y - b.y) < MAX_FLT_PRECISION)
				&& (a.type == b.type) && (a.arc == b.arc);
		}
		EventQPoint & operator = (const EventQPoint &);
		friend bool operator < (const EventQPoint&a, const EventQPoint&b)
		{
			if (a.y != b.y) return a.y > b.y;
			if (a.x != b.x) return a.x < b.x;
			if (a.type != b.type) return (a.type == 'C');
			if (a.type == 'S') return false;
			return (*a.arc->cell->site < *b.arc->cell->site);
		}

	};

	typedef struct NodeStructType * pNodeType;

	typedef struct NodePointerStructType
	{
		pNodeType node;
		pBeachPointType leaf;

	} NodePointerType, * pNodePointerType;

	typedef struct NodeStructType
	{
		int height;
		pBeachPointType greatestLeftPoint;
		pNodePointerType left, right;

	} NodeType;

	typedef struct PathStructType * pPathType;

	typedef struct PathStructType
	{
		char direction;  // 'l' for left, 'r' for right
		pNodeType node;
		pPathType next, prev;

	} PathType;

	typedef struct LinkParameterStructType
	{
		CompGeo::XY P1, P2, R0, d, * ePt;
		double * t;
		bool * OnSeg, makeTwin1;
		int eCount, iCount;
		pHalfEdgeListType hL, hLPrev, Intersectors;
	} LinkParameterType, *pLinkParameterType;

	typedef class RadialPoint * pRadialPoint;

	class RadialPoint
	{
	public:
		// 0 = center, 1 = +x axis, 2 = Q1, 3 = +y axis, 4 = Q2, 5 = -x, 6 = Q3, 7 = -y, 8 = Q4
		int Quad2; 
		double YoverX; // for even Quad2 this is y/x otherwise it is 0.0
		pHalfEdgeType he;  // point for ordering will be origin
		pCellType cell; // used to initialize exterior twin halfedges
		pSitePoint ctr;
		RadialPoint(void);
		RadialPoint(pHalfEdgeType &);  // edge provides origin & center via cell->site
		RadialPoint(pHalfEdgeType &, const pSitePoint &);  // edge provides origin, center is other parameter
		RadialPoint(const RadialPoint &);
		RadialPoint & operator = (const RadialPoint &);
		friend bool operator<(const RadialPoint &a, const RadialPoint &b)
		{
			if (a.Quad2 == b.Quad2) 
			{
				return (a.YoverX < b.YoverX);
			}
			return (a.Quad2 < b.Quad2);
		}
		
	};


	class BinaryBPlus
	{
	public:
		BinaryBPlus(void);
		~BinaryBPlus(void);

		pBeachPointType beforeLeaf, afterLeaf;  // special points representing + & - infinity
		pArcType leftmost, rightmost,  // doubly linked list beachline root nodes
			arcTop;  // if IsStarting this holds top arc after first is processed
		pNodeType root;
		pPathType pathTop, pathBottom;
		bool IsStarting, SharedTopLevel;
		// special (start) handling is required for 1) the first arc, 2) next arc(s) w/ equal y value(s)
		CompGeo::XY circle_ctr;
		double radius, x_bp, y_bp, y_d;  // radius for circle, bp for breakpoint, d for directrix
		bool circleFound;
		pNodeType * levels;
		int max_level;
		//HANDLE hf;
		std::ofstream hf;
		bool hfSetUp;
		//pHalfEdgeType released_he[3];
		pEventQPoint dBugQ;

		void DeleteSubTree(pNodeType &);
		void Reset(void);

		int GetMax(int a, int b) { return a > b? a: b;}
		double sqr(double a) {return a * a;}
		int GetHeight(pNodeType);
		void SetHeight(pNodeType);
		pNodeType rotateLeft(pNodeType);
		pNodeType rotateRight(pNodeType);
		pNodeType Rotate(pPathType); // rotates at parameter

		pArcType FindArc(pCellType); // sets path_first to root and 
		//void SetCirclePath(pBeachPointType); // sets path for a circle event
		void FindLeaf(pArcType);
		void AddArc(pArcType &, CompGeo::AVL<EventQPoint> &); // call with new site/cell/arc
		void DeleteArc(pEventQPoint, CompGeo::AVL<EventQPoint> &, pDiagramType); // call with circle point
		void ClearPath(void);
		void AddToPath(pNodeType, char);
		void RemoveFromPath(pPathType);
		void TruncatePath(pPathType);

		double GetYofXForArc(pArcType, double); // uses y_d to return y(x) for this parabola and xValue
		void GetX(pBeachPointType); // uses y_d, returns x coordinate of intersection, sets x_bp
		void GetY(pBeachPointType); // uses x_bp or (sometimes) y_d to return y coordinate of intersection
		void GetCircle(pArcType); // 3 consecutive focal points from list in: 
									//sets circle_ctr and radius (0 iff no circle) and circleFound
		void RecoverRadius(pEventQPoint); // calculates circle radius from circle event
		void SanityCheck(void);
		int GetRealHeight(pNodeType);
		void nodeToElement(int, int, pNodeType);
		void TreeWrite(const std::string &, const std::string &);
		int GetTwoPow(int);  // returns 2 to the nth where n is the parameter 0 to 32 otherwise returns 0

	private:
		void AddFirstArc(pArcType &);
		void AddFirstPoint(pArcType &);
		void AddTopLevelArc(pArcType &);
		void AddNormal(pArcType &, CompGeo::AVL<EventQPoint> &); // Q needed to add or delete circle events
		void SetNode(pNodePointerType &, pNodeType, pBeachPointType);
	};

	class Voronoi
	{
	public:
		Voronoi(void);
		~Voronoi(void);

		void SetBoundingBox(double, double, double, double);
		//void AddSites(CompGeo::pXY, int);
		void AddSites(const std::vector<pPGonWork> *, const std::vector<CompGeo::XY> &);
		void Run(void); //Gtk::Allocation);
		void ConductTest1(Gtk::Allocation);
		void StartAnimation(double); //, Gtk::Allocation); //(PolygonDraw *, NGons *, HWND, RECT, unsigned int_PTR, unsigned int, double);
		void Animate(void);
		void DrawFinished(void);
		void ClearDiagram(void);
		void ClearSites(void);
		std::vector<pPGonWork> translateCells(void);
		std::vector<pPGonWork> translateCells(CompGeo::DCEL<double> &);
		void translateDiagram(CompGeo::DCEL<double> &);

		BoundingBoxType * pBB;
		Gtk::Allocation bb; // used only in test
		DiagramType D;
		std::vector<CompGeo::XY> * bLine, * LinePts, * sPts;
		bool InProcess, DiagramComplete;

	private:
		double sqr(double a) {return a * a;}
		void LinkHalfEdges(void);
		void FixOrder(pHalfEdgeType); // ensures that origin(next(half))=origin(twin(half))
		void DeleteHalfEdgeFromList(pHalfEdgeType);  // halfedge is in IncidentCell's InnerComponents
		bool InBounds(CompGeo::XY);
		bool ValidSite(CompGeo::XY);
		int OnBox(CompGeo::XY); // 0=right, 1=top, 2=left, 3=bottom, -1=not
		int InCorner(CompGeo::XY); // 0=bottomright, 1=topright, 2=topleft, 3=bottomleft, -1=not
		// for the bounding box {(right, 0), (0, top), (left, 0), (0, bottom)} R = R0 + tL
		void GetBBSolutions(CompGeo::pXY, double *, bool *, const CompGeo::XY &, const CompGeo::XY &);
		void ResolveEdge(pLinkParameterType);
		void ResolveNull2Null(pLinkParameterType);
		void ResolveNull2Out(pLinkParameterType);
		void ResolveNull2In(pLinkParameterType);
		void ResolveOut2Out(pLinkParameterType);
		void ResolveOut2In(pLinkParameterType);
		void CleanUpVertices(void);
		void AddDupRecord(pDupType &, const pDupType);
		void CheckForDups(pHalfEdgeType, pDupType &);
		void CheckForSingles(pHalfEdgeType, pDupType &);
		void ResetAllHalfEdgeOrigins(pHalfEdgeType, const pVertexType &); 
		void AddToLine(D2D1_POINT_2F *, int);
		void TerminateBeachEdge(pHalfEdgeType);
		CompGeo::XY CropBeachPoint(pHalfEdgeType);
		void DrawBeachLine(void);
		void SetPOINTFromXY(D2D1_POINT_2F *, int, const CompGeo::XY &);
		void InProcessDraw(bool);
		bool DrawNextEvent(void);
		void AttendToTree(void);
		void Finalize(void);
		pPGonWork translateEdges(pHalfEdgeType, bool);
		pPGonWork translateEdges(CompGeo::DCEL<double> &, CompGeo::ITYPE, bool);
		void RoundVertices(void); // sets vtx_count
		//void CheckEdgesNAN(void);
		void DebugPrintHalfEdges(void);
		void DebugPrintLoop(pHalfEdgeType);
		void DiagramHalfEdges(void);
		void DebugPrintHalfEdgeCycle(CompGeo::DCEL<double> &, CompGeo::pHalfEdgeType);
		BinaryBPlus T;
		CompGeo::AVL<EventQPoint> Q;
		pSitePointListType SitePoints;
		//pVertexListType BBvtx_i, BBvtx_f; // all vertices on the bounding box
		//int ocount, hcount;
		int cellIDbase, vtx_count;
		bool IsStarting, AnimationHold;
		EventQPoint EventNxt;
		unsigned int roundDecimal;
		// Drawing Stuff
		//D2D1_BEZIER_SEGMENT * bLine;
		//D2D1_POINT_2F * first_point, * LinePts;
		CompGeo::XY first_point;
		int CtlPtCount, LinePtCount;
		//PolygonDraw * p_Draw;
		//NGons * p_NG;
		//HWND h_Wnd;
		double old_directrix, current_directrix, directrix_delta;
		//unsigned int_PTR timerID;

	};
};