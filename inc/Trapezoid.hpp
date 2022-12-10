#pragma once

namespace Trapezoid
{
	typedef struct NodeListType * pNodeList;

	template<typename T> class EdgeType;
	template<typename T> using pEdgeType = EdgeType<T>*;

	template<typename T> class Tzoid;
	template<typename T> using pTzoid = Tzoid<T>*;

	typedef struct RectangleStructType
	{
		//int left, right, top, bottom;
		CompGeo::pHalfEdgeType top, bottom;

	} RectangleType, *pRectangleType;

	template<typename T>
	class  PointType
	{
	public:
		PointType(void);
		PointType(const PointType<T> &);
		PointType(pEdgeType<T>, char);
		~PointType(void);
		std::string PrintID(const int);

		int ID;
		char suffix;
		CompGeo::pTPoint<T> pt;
	};
	template<typename T> using pPointType = PointType<T> *; // alias declaration
	
	template<typename T>
	class EdgeType 
	{
	public:
		EdgeType(void);
		EdgeType(const EdgeType<T> &);
		EdgeType(CompGeo::pHalfEdgeType &, const int &);
		~EdgeType(void);
		EdgeType<T> & operator = (const EdgeType<T> &);
		void PrintEdge(void);
		std::string PrintID(const int);

		int ID;
		CompGeo::pHalfEdgeType leftMost;
		pPointType<T> lPt, rPt;
	};
	


	typedef struct TrapPointStructType
	{
		int num_pts;  // s/b 3 or 4 c/b 0 if no points are displayable
		CompGeo::XY pts[4];
	} TrapPointType, * pTrapPointType;

	typedef struct NodeStructType
	{
		char node_type; // 'T' = trapezoid; 'X' = xnode; 'Y' = ynode
		void * node;

	} NodeType, * pNodeType;


	typedef struct NodeListType
	{
		pNodeType node_record;
		pNodeList next;

	} NodeList;

	template<typename T>
	class NodeCollection
	{
	public:
		NodeCollection(void);
		~NodeCollection(void);
		void AddNode(pNodeType);
		void PrintCollection(void);

		pNodeList first;

	};
	template<typename T> using pNodeCollection = NodeCollection<T>*;

	template<typename T>
	class Tzoid
	{
	public:
		Tzoid(void);
		Tzoid(const Tzoid<T> &);
		~Tzoid(void);
		Tzoid<T> & operator=(const Tzoid<T> &);
		TrapPointType MakePoints(void);
		std::string GetNeighbors(void);
		void PrintZoid(void);
		static int ResetIndex(void);

		static int idIdx;
		int id; 
		pPointType<T> leftp, rightp;
		pEdgeType<T> top, bottom;
		pTzoid<T> LeftUpper, LeftLower, RightUpper, RightLower;
		pNodeType GEbuff; // for temporary graph entry storage
	};

	template<typename T>
	struct TrapezoidSequenceType
	{
		char yRel;
		pTzoid<T> qZoid, lu, ll;
	};

	template<typename T>
	struct TrapezoidListType
	{
		TrapezoidListType(void) 
			{t_zoid_upper = NULL; t_zoid_lower = NULL; 
			y_Tst = ' '; Mod_This = NULL; next_upper = 0; next_lower = 0;}
		
		pTzoid<T> t_zoid_upper, t_zoid_lower;
		char y_Tst;
		pNodeType Mod_This;
		unsigned int next_upper, next_lower;
	};

	template<typename T>
	struct TrapezoidChainType
	{
		TrapezoidChainType(void) {reset();}
		~TrapezoidChainType(void) {reset();}
		void reset(void) {CHN.clear(); leftTrap = NULL; rightTrap = NULL;}

		std::vector<TrapezoidListType<T>> CHN;
		pTzoid<T> leftTrap, rightTrap;
	};

	template<typename T>
	class XNode
	{
	public:
		XNode(pPointType<T>);
		~XNode(void);
		char Test(const PointType<T> &);  // 'E' same point, 'L' point is left of node, 'R' point is right of node

		pPointType<T> x_nodePt;
		pNodeType left, right;
	};
	template<typename T> using pXNode = XNode<T>*;

	template<typename T>
	class YNode
	{
	public:
		YNode(pEdgeType<T>);
		~YNode(void);
		char PointTest(const PointType<T> &); // 'E' point is on node, 'A' point is above node, 'B' point is below node
		char EdgeTest(const EdgeType<T> &); // 'E' edge is on node, 'A' edge is above node, 'B' edge is below node

		pEdgeType<T> seg;
		pNodeType above, below;
	};
	template<typename T> using pYNode = YNode<T>*;

	template<typename T>
	class Graph
	{ // acyclic directed graph used to search for a trapezoid in the trapezoidal map
	public:
		Graph(void);
		~Graph(void);

		void Reset(void);
		pTzoid<T> Initialize(RectangleType &); // 1st trapezoid is bounding rectangle leftp=LL rightp=UR
		std::vector<pTzoid<T>> AddEdge(pEdgeType<T>); // returns new trapezoids (1-3)
		void FindTrapezoid(pEdgeType<T>); // trapezoid in member leaf
		void FindTrapezoid(pPointType<T>);	// trapezoid in member leaf

	private: // AddEdge helper functions
		std::vector<TrapezoidSequenceType<T>> getIntersectedTrapezoids(const pEdgeType<T> &);
		std::vector<pTzoid<T>> fillChain(const pEdgeType<T> &);
		void makeLinks(const std::vector<TrapezoidSequenceType<T>> &, size_t &, size_t &);
		pTzoid<T> fillFinalLink(const Tzoid<T> &, size_t &, size_t &);
		void HandleLeftEnd(const pTzoid<T> &, pPointType<T> &, const pEdgeType<T> &, const TrapezoidSequenceType<T> &, size_t &, size_t &);
		void HandleRightEnd(const pTzoid<T> &, pPointType<T> &, const pEdgeType<T> &, const TrapezoidSequenceType<T> &, size_t &, size_t &);
		void fixInnerLinks(const pEdgeType<T> &, const std::vector<TrapezoidSequenceType<T>> &);
		char pointCmp(pYNode<T>, pPointType<T>);
		bool ordered(pPointType<T>, pPointType<T>);
		void Graph1Link(pEdgeType<T>);
		void Graph1st(pEdgeType<T>);
		void GraphLst(pEdgeType<T>);
		void GraphInner(pEdgeType<T>, size_t);
		pNodeType MakeNode(void *, char);
		void ModifyNode(pNodeType, void *, char);
		void ClearChain(void);
		void PrintChain(void);
		bool ZoidIsZeroWidth(pTzoid<T>);

	public:
		pNodeCollection<T> NC; // this is a master list of all nodes in the graph
		pNodeType root;
		pTzoid<T> leaf; // leaf is trapezoid result from FindTrapezoid
		TrapezoidChainType<T> chain;  // for build: all trapezoids through which the edge passes
		pEdgeType<T> topEdge, bottomEdge;
	};

	template<typename T>
	class Map
	{ // trapezoidal map
	public:
		Map(void);
		~Map(void);

		void GetEdges(void);
		void RandomizeEdges(void);
		void GetBox(void); //const Gtk::Allocation &);
		void Run(CompGeo::DCEL<T> *); // bounding box is a part of the DCEL, Gtk::Allocation *);
		Fortune::pSitePoint GetSitePoint(CompGeo::BasicTPoint<T> *);
		std::vector<pPGonWork> TranslateTrapezoids(void);
		void DebugPrintZoids(void);
		void DebugPrintEdge(pEdgeType<T>);

		static CompGeo::DCEL<T> * dcel;
		static size_t cFaceIdx;
		pEdgeType<T> leftHalves; 
		size_t nEdges;
		Graph<T> DAG;
		RectangleType R;
		pPointType<T> pBuff;
		std::vector<pTzoid<T>> map;
		// need a lookup to get the trapezoid that begins any halfedge
		// this will let us assign a top face to the trapezoids
		// in the hidden surface removal algorithm
	};
};

