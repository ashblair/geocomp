#pragma once

	class TriXY:public CompGeo::XY
	{
	public:
		TriXY(void);
		TriXY(const CompGeo::XY &);
		friend bool operator < (const TriXY &, const TriXY &);
		friend bool operator > (const TriXY &, const TriXY &);
	};

	typedef struct TriHalfEdgeStruct * pTriHalfEdgeType;

	typedef struct TriHalfEdgeListStruct * pTriHalfEdgeListType;
	typedef struct TriHalfEdgeListStruct
	{
		pTriHalfEdgeType OneEdge;
		pTriHalfEdgeListType next;

	} TriHalfEdgeListType;
	
	typedef struct TriFaceStruct * pTriFaceType;

	typedef struct TriFaceListStruct * pTriFaceListType;
	typedef struct TriFaceListStruct
	{
		pTriFaceType a_face;
		pTriFaceListType next;

	} TriFaceListType;

	typedef struct TriVertexStruct * pTriVertexType;

	typedef struct TriVertexStruct
	{
		unsigned int Vertex; // unique id of vertex = array index
		TriXY Coordinates;
		char TurnType;  // {B, E, R, S, M} for start(begin), end, regular, split or merge vertex
		pTriHalfEdgeType IncidentEdge;

	} TriVertexType;

	typedef struct TriFaceStruct
	{
		unsigned int Face, // unique id of face {= Polygon + 1, 0=outside plane (a hole w/ no OuterComponent)}
			NumEdges;  // this is for the list in the OuterComponent
		char FaceType; // {H, F} for hole or fill
		pTriHalfEdgeType OuterComponent;  // an edge on the outside of the face
		pTriHalfEdgeListType InnerComponents;  // a list of edges to each of the face's holes

	} TriFaceType;

	typedef struct TriHalfEdgeStruct
	{
		// unique id of halfedge is Polygon {0 - P - 1} + Edge {0 - N-1} + Half {0, 1}
		unsigned int Polygon, Edge, Half; 
		pTriVertexType Origin, Helper;
		pTriFaceType IncidentFace;
		pTriHalfEdgeType Twin, Next, Prev;

	} TriHalfEdgeType;

	typedef struct TriDoublyConnectedEdgeListStruct
	{
		pTriVertexType Vertices; // an array
		pTriFaceType Faces; // pointer to exterior face
		unsigned int NumVertices, NumFaces;

	} TriDoublyConnectedEdgeListType;

	typedef struct TriPointStruct * pTriPointType;
	
	typedef struct TriSegStruct * pTriSegType;
	typedef struct TriSegStruct
	{
		TriXY lo, hi; // these are points only the segs member will be NULL
		//pTriFaceType polygon;
		pTriHalfEdgeType hEdge;
		TriSegStruct(void);
		TriSegStruct(const TriSegStruct &);
		TriSegStruct(pTriHalfEdgeType &);
		TriSegStruct & operator = (const TriSegStruct &);

	} TriSegType;

	typedef struct TriSegListStruct * pTriSegListType;
	typedef struct TriSegListStruct
	{
		pTriSegType seg;
		pTriSegListType next, match;

	} TriSegListType;

	typedef struct TriPointStruct
	{
		TriXY point;
		pTriSegListType segs;
		TriPointStruct & operator=(const TriPointStruct &);
		TriPointStruct(const TriPointStruct &);
		TriPointStruct(void);
		~TriPointStruct(void);

	} TriPointType;

	typedef struct TriPointListStruct * pTriPointListType;
	typedef struct TriPointListStruct
	{
		pTriPointType point;
		pTriPointListType next;

	} TriPointListType;

	class TriStack
	{
	public:
		TriStack(void);
		~TriStack(void);

		void push(pTriPointType);
		pTriPointType pop(void);
		pTriPointType peek(void);

	private:
		pTriPointListType stackRoot;

	};

	class Triangulation
	{
	public:
		Triangulation(void);
		Triangulation(NGons &, bool, std::string &);
		template<typename T> Triangulation(std::vector<CompGeo::pFaceType> &, CompGeo::DCEL<T> *& );
		Triangulation(const Triangulation &);
		~Triangulation(void);
		
		static double XofY(const TriSegType &);
		bool TriListLoad(NGons &, std::string &);
		template<typename T> void TriListLoad(std::vector<CompGeo::pFaceType> &, CompGeo::DCEL<T> *&);
		void doInnerComponents(CompGeo::AVL<TriPointType> &, const pTriSegType &);
		void deleteFace(pTriFaceType);
		pTriHalfEdgeType chainHalfs(pTriFaceType, pTriHalfEdgeType &);
		pTriFaceType buildFace(pTriFaceType);
		void setFaceType(const char, pTriFaceType);
		void setTurnType(pTriFaceType);
		void setTurnTypeForEdge(pTriHalfEdgeType);
		void setMatches(const pTriSegListType &);
		void clearLeftMatches(const pTriSegListType &);
		pTriSegListType getLeftMatch(const pTriSegListType &);
		void AddNewHalf(pTriHalfEdgeType, pTriSegListType &);
		void DeleteNewHalf(pTriHalfEdgeType, pTriSegListType &);
		void AddEdge(pTriVertexType, pTriVertexType, pTriSegListType &);
		std::vector<pTriHalfEdgeType> getHalfEdgesAtOrigin(pTriHalfEdgeType);
		std::vector<pTriHalfEdgeType> getHalfEdgesEndingAtOrigin(pTriHalfEdgeType);
		pTriHalfEdgeType getLeftMostOriginating(const pTriHalfEdgeType &, const std::vector<pTriHalfEdgeType> &);
		pTriHalfEdgeType getLeftMostEnding(const pTriHalfEdgeType &, const std::vector<pTriHalfEdgeType> &);
		void MakeFaces(pTriFaceType, pTriFaceType, pTriSegListType &);
		pTriSegType FindLeft(TriXY, CompGeo::AVL<TriSegType> &);
		void removeStatusEdge(pTriHalfEdgeType, CompGeo::AVL<TriSegType> &);
		void setForMonotone(pTriFaceType);  // call this with tLst.Face
		void makeMonotone(pTriFaceType, pTriHalfEdgeListType);
		void handleStartVertex(pTriHalfEdgeType, CompGeo::AVL<TriSegType> &);
		void handleEndVertex(pTriHalfEdgeType, CompGeo::AVL<TriSegType> &, pTriSegListType &);
		void handleRegularVertex(pTriHalfEdgeType, CompGeo::AVL<TriSegType> &, pTriSegListType &);
		void handleSplitVertex(pTriHalfEdgeType, CompGeo::AVL<TriSegType> &, pTriSegListType &);
		void handleMergeVertex(pTriHalfEdgeType, CompGeo::AVL<TriSegType> &, pTriSegListType &);
		std::vector<pPGonWork> translateFaces(pTriFaceType); // for InnerComponents of holes
		std::vector<pPGonWork> translateFace(pTriFaceType); // for a fill face
		std::vector<unsigned int> indexFaces(pTriFaceType); // for InnerComponents of holes
		std::vector<unsigned int> indexFace(pTriFaceType); // for a fill face
		void setForTriangulation(pTriFaceType); // call this with tLst.Face
		void addTriangles(pTriFaceType, pTriHalfEdgeListType);

		TriDoublyConnectedEdgeListType tLst;
		bool fMonotonized;
		unsigned int nxtNewEdge;
		//static unsigned int tPhase;
		static TriXY refPt;
	};