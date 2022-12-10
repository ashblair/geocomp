#pragma once

	class SegIntXY:public CompGeo::XY
	{
	public:
		SegIntXY(void);
		SegIntXY(const CompGeo::XY &);
		friend bool operator < (const SegIntXY &, const SegIntXY &);
		std::string printData(void);
	};

// AVL tree support structures:
	typedef struct AddIntersectionListStructType * pAddIntersectionListType;
	typedef struct AddIntersectionListStructType
	{
		SegIntXY xy;
		pAddIntersectionListType next;

	} AddIntersectionListType;

	typedef struct AddIntersectionHeaderStructType
	{
		pAddIntersectionListType pTop, pLast;
		unsigned int aiCount;
	} AddIntersectionHeaderType, * pAddIntersectionHeaderType;

	typedef struct IndexListStructType * pIndexListType;
	typedef struct IndexListStructType 
	{
		unsigned int index;
		pIndexListType next;

	} IndexListType;

	// Create an array of these:
	typedef struct SegIntVertexElementStructType * pSegIntVertexElementType;
	typedef struct SegIntVertexElementStructType
	{
		std::vector<CompGeo::XY> verts;
		unsigned int initial_size;
	} SegIntVertexElementType;

	typedef struct SegIntEdgeStructType * pSegIntEdgeType;
	typedef struct SegIntEdgeStructType
	{
		unsigned int pIdx,	// index into SegIntVertices
			lo,		// index of lo vertex
			hi;		// index of hi vertex

	} SegIntEdgeType;

	class SegIntPoint
	{
	public:
		SegIntPoint(void);
		SegIntPoint(const SegIntPoint &);
		~SegIntPoint(void);
		unsigned int pIdx,	// index into SegIntVertices
			vIdx;	// index of vertex - if >= numVertices it's in AddIntersections
		pIndexListType pEdges; // every vertex can point to as many edges as you'd like
			// there will be 1 list entry for a point that is an end point
			// there will be a pair of entries for a point of intersection not an end point 
			// for at least one of the edges
		SegIntPoint & operator = (const SegIntPoint &);

	};

	class SegIntEdge
	{
	public:
		SegIntEdge(void);
		SegIntEdge(const SegIntEdge &);
		SegIntEdge(unsigned int);
		~SegIntEdge(void);
		unsigned int eIdx;
		SegIntEdge & operator = (const SegIntEdge &);
	};

	typedef struct SegIntPointListStruct * pSegIntPointListType;
	typedef struct SegIntPointListStruct
	{
		SegIntPoint * pPoint;
		pSegIntPointListType next;

	} SegIntPointListType;  // use this to report intersecting points with their edges, turn into array

	typedef struct SegIntPointArrayElementStruct
	{
		SegIntXY iPt;  // the intersection point
		CompGeo::pEdgeType EdgeArray;
		pSegIntEdgeType EdgeIndexArray;
		unsigned int edgeCount;

	} SegIntPointArrayElement, *pSegIntPointArrayElement;

	typedef struct SegIntPointArrayHeaderStruct
	{
		pSegIntPointArrayElement xsectArray;
		unsigned int xsectCount;
	} SegIntPointArrayHeader, *pSegIntPointArrayHeader;

	class SegmentIntersect
	{
	public:
		SegmentIntersect(void);
		SegmentIntersect(NGons &);
		~SegmentIntersect(void);
		void InitializeVertices(NGons &);
		void DeleteVertices(void);
		bool getIntersection(SegIntXY &, unsigned int *, unsigned int *);
		bool isReportable(SegIntPoint &); // call before adding to rptI
		unsigned int AddPointToEnd(unsigned int, SegIntXY &); 
		void DeleteLastPoint(unsigned int); 
		char ClassifySegment(unsigned int); // returns u for Upper, l for Lower and c for Contains
		unsigned int getIndexCount(pIndexListType);
		pIndexListType combineIndexLists(pIndexListType, pIndexListType);
		void addIndex(pIndexListType &, unsigned int);
		void deleteIndexList(pIndexListType *);
		void addtoReport(SegIntPoint *&);
		void getAllIntersects(void);
		pPGonWork makePolygon(unsigned int &);
		std::vector<pPGonWork> makePolygons(unsigned int &);
		pIndexListType getContainerEdges(CompGeo::AVL<SegIntEdge> &, unsigned int *&, unsigned int *&);

		// a couple of important members:
		static pSegIntVertexElementType SegIntVertices;
		static pSegIntEdgeType SegIntEdges;
		static unsigned int SegIntPolygonCount, SegIntEdgeCount;
		static SegIntXY refPt;
		static bool fFindSegment;

		pSegIntPointListType rptI;  // our intersections
	};

