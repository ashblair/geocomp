#pragma once

//#ifndef DELTA_Y_BELOW_SWEEP
//#define DELTA_Y_BELOW_SWEEP 1000.0
//#endif

namespace FaceX // for face intersection algorithms
{
    template<typename T> class Pair;
    template<typename T> using pPair = Pair<T> *;
    template<typename T> class Face;
    template<typename T> using pFace = Face<T> *;
    template <typename T> class Edge;
    template <typename T> using pEdge = Edge<T> *;
    template <typename T> class Edge3D;
    template <typename T> using pEdge3D = Edge3D<T> *;
    template<typename T> class Point;
    template<typename T> using pPoint = Point<T> *;
    template<typename T> struct PtLnk;
    template<typename T> using pPtLnk = PtLnk<T> *;
    template<typename T> class SType;

    template<typename T> struct PtLnk
    { // upon initialization the additionalTPointInfo member of all points in pvCld  will point to one of these
        PtLnk(void):prj_point(NULL), model_point(NULL), flag('o') {}
        pPoint<T> prj_point; // on projection plane
        CompGeo::pTPoint<T> model_point; // in 3D space
        char flag; // o = original, i = intersection, z = reset model's z coord

        T GetX(void) {return model_point->xyz.X;}
        T GetY(void) {return model_point->xyz.Y;}
        T GetZ(void) {return model_point->xyz.Z;}
        CompGeo::XYZ GetXYZ(void) 
        {return CompGeo::XYZ{static_cast<double>(model_point->xyz.X), static_cast<double>(model_point->xyz.Y), static_cast<double>(model_point->xyz.Z)};}
        CompGeo::XY GetXY(void)
        {return CompGeo::XY{static_cast<double>(model_point->xyz.X), static_cast<double>(model_point->xyz.Y)};}
    };

    template <typename T> class Point
    {
    public:
        Point(void);
        Point(const pPtLnk<T> &);
        ~Point(void);

        bool IsB4(const Point &) const;
        friend bool operator < (const Point & a, const Point & b) {return a.IsB4(b);}
        bool IsEq(const Point &) const;
        friend bool operator == (const Point & a, const Point & b) {return a.IsEq(b);}
        friend bool operator != (const Point & a, const Point & b) {return !a.IsEq(b);}
        
        T GetX(void) const; 
        T GetY(void) const;
        void AddLink(const pPtLnk<T> &);
        pPtLnk<T> CheckBuffer(const T);
        pPtLnk<T> GetLink(void);
        void AddUpper(const pEdge<T> &);

        std::vector<pPtLnk<T>> zb; // in order: nearest to farthest all w/ equal x, y
        std::vector<SType<T>> ups; // in order: left to right
    };

    // finalize face structures:

    template<typename T> struct CycleEdge;
    template<typename T> using pCycleEdge = CycleEdge<T> *;

    template<typename T> struct OverlapCycle
    {
        OverlapCycle(void): IsTop(false), face(NULL) {}
        OverlapCycle(const OverlapCycle & o): IsTop(o.IsTop), face(o.face), cycles(o.cycles) {}

        OverlapCycle & operator =(const OverlapCycle & a) {IsTop = a.IsTop; face = a.face; cycles = a.cycles; return *this;}
        friend bool operator < (const OverlapCycle & a, const OverlapCycle & b) {return *(a.face) < *(b.face);}
        friend bool operator ==(const OverlapCycle & a, const OverlapCycle & b) {return *(a.face) == *(b.face);}
        bool IsAbove(OverlapCycle & a)
        {
            typename std::vector<pCycleEdge<T>>::iterator ait = a.cycles.begin();
            for (typename std::vector<pCycleEdge<T>>::iterator cit = cycles.begin(); cit != cycles.end(); ++cit)
            {
                pCycleEdge<T> c0 = *ait, c1 = *cit; 
                CompGeo::pTPoint<T> P0[] = {c0->oPt, c0->nPt}, P1[] = {c1->oPt, c1->nPt};
                for (int j = 0; j < 2; ++j)
                {
                    CompGeo::pTPoint<T> p0 = P0[j], p1 = P1[j];
                    T z0 = p0->xyz.Z, z1 = p1->xyz.Z;
                    if (!CompGeo::BEq(z0, z1))
                    {
                        return z1 > z0;
                    }
                }
                ++ait;
            }
            assert (1 == 2); // equal cycles not allowed
            return false;
        }

        bool IsTop;
        pFace<T> face;
        std::vector<pCycleEdge<T>> cycles;
        //std::vector<CompGeo::ITYPE> halfs;
    };
    template<typename T> using pOverlapCycle = OverlapCycle<T> *;

    template<typename T> struct Cycle;
    template<typename T> using pCycle = Cycle<T> *;

    template<typename T> struct Cycle
    {
        Cycle(void): lowest(0), highest(0), orientation(' ') {}

        std::vector<pCycleEdge<T>> elements;
        CompGeo::ITYPE lowest, highest;
        char orientation; // R = counterclockwise or positive; L = clockwise or negative 
        std::vector<pCycle<T>> holes;
        
        void AddElement(const pCycleEdge<T> &);
        void GetOrientation(void);
        bool IsWithin(Cycle &);
        int IsLeft(const CompGeo::BasicTPoint<T> &, const CompGeo::BasicTPoint<T> &, const CompGeo::BasicTPoint<T> &);
        int getWindingNumber(const pCycleEdge<T> &);

    };

    template<typename T> struct CycleEdge
    { // each half will have a pointer to one of these in additionalHalfEdgeInfo
        CycleEdge(void): pHE(NULL), e3(NULL), oPt(NULL), nPt(NULL), face(NULL), pair(NULL), flag(' '), r_l(' ') {}
        CycleEdge(const CycleEdge & c): pHE(c.pHE), e3(c.e3), oPt(c.oPt), nPt(c.nPt), face(c.face), pair(c.pair), flag(c.flag), r_l(c.r_l) {}

        CycleEdge & operator=(const CycleEdge & c) {pHE = c.PHE; e3 = c.e3; oPt = c.oPt; nPt = c.nPt; 
            face = c.face; pair = c.pair; flag = c.flag; r_l = c.r_l; return *this;}
        void MovePair(const pCycleEdge<T> &);

        CompGeo::pHalfEdgeType pHE;
        pEdge3D<T> e3; // this CycleEdge is left or right Cycle in this Edge3D
        CompGeo::pTPoint<T> oPt, nPt; // origin and next points unique id = viewvertex
        pFace<T> face; // corresponds w/ pHE's incidentface
        pPair<T> pair;
        char flag, // 'A'=added or 'S'=status quo
            r_l; // rightCycle 'R' or leftCycle 'L' in Edge3D
    };

    template<typename T> class OriginEdge
    { // this will form a binary tree lookup for all edges w/ a given origin
      // usage: until empty getleast form new cycles with all edges
      // if a cycleedge within a cycle has >1 edge found in tree:
      // use principle of leftmost turn to choose cycleedge to add to cycle,
      // put un-chosen edges back into tree after removing cycleedge from edges

    public:
        OriginEdge(void);
        OriginEdge(const CompGeo::ITYPE &);
        OriginEdge(const OriginEdge &);
        ~OriginEdge(void);

        OriginEdge & operator = (const OriginEdge &);
        bool IsLT(const OriginEdge &) const;
        friend bool operator < (const OriginEdge & a, const OriginEdge & b) {return a.IsLT(b);}
        bool IsEq(const OriginEdge &) const;
        friend bool operator == (const OriginEdge & a, const OriginEdge & b) {return a.IsEq(b);}

        CompGeo::ITYPE oID; // origin id = viewvertex of all origin points in edges
        std::vector<pCycleEdge<T>> edges;
    };

    template<typename T> using pOriginEdge = OriginEdge<T> *;

    // end of finalize face structures

    template<typename T> class Face
    {
    public:
        Face(void);
        Face(const CompGeo::pFaceType &);
        ~Face(void);

        bool IsLT(const Face &) const;
        friend bool operator < (const Face & a, const Face & b) {return a.IsLT(b);}
        bool IsEq(const Face &) const;
        friend bool operator == (const Face & a, const Face & b) {return a.IsEq(b);}

        CompGeo::TPoint<T> getFacePoint(const CompGeo::pTPoint<T> &, const pPtLnk<T> &);
        void AddCycleEdge(const pCycleEdge<T> &);
        void FinalizeFace(CompGeo::DCEL<T> *);
        void GetCycles(std::vector<Cycle<T>> &);
        void RemoveCycles(const std::vector<pCycleEdge<T>> &);
        CompGeo::ITYPE GetOverlaps(std::map<CompGeo::ITYPE, OverlapCycle<T>> &, const Cycle<T> &);
        int RemoveTwins(void);
        CompGeo::pFaceType MakeFace(CompGeo::DCEL<T> *, const Cycle<T> &);

        CompGeo::pFaceType pface;
        int pairCount; // face is ready for finalization when this goes to zero
        bool xface, // true iff this face intersects another on the projection plane
            below; // true iff any part of the face is below another
        // partitions for sub-faces:
        std::map<CompGeo::ITYPE, pCycleEdge<T>> cycle_edges; // key = halfedge DCEL index
        //CompGeo::AVL<OriginEdge<T>> singles; // unlinked face edges (empty this to finalize face & spawn new)
        // cycles will all be in finalize_face method:

    };


    template <typename T> class Edge3D
    { // on the projection plane as many of these as you wish may overlap
      // which may indicate faces intersecting
    public:
        Edge3D(void);
        Edge3D(const pCycleEdge<T> &, const pCycleEdge<T> &); // rightCycle and leftCycle in
        ~Edge3D(void);

        pEdge<T> e; // this Edge3D is a model_edge in Edge e
        pCycleEdge<T> rightCycle, leftCycle; // oPt, nPt are equal but reversed across these members; right is top-down for all edges
        char flag; // 'C'=complete, 'B'=recalculation of bottom point needed
    };

    template <typename T> class Edge
    {
    public:
        Edge(void);
        Edge(const pEdge3D<T> &);
        ~Edge(void);

        void initializeEdge(void);
        void updateXs(void);
        void AddModelEdge(const pEdge3D<T> &);
        bool IsB4(Edge &); 
        friend bool operator < (Edge & a, Edge & b) {return a.IsB4(b);}
        bool IsEq(Edge &);
        friend bool operator == (Edge & a, Edge & b) {return a.IsEq(b);}
        bool IsOver(Edge &);

        std::vector<pEdge3D<T>> model_edges;
        pPoint<T> topPoint, bottomPoint;
        bool horizontal;
        double xOnSweep, w; // w is m inverted or bottom x for horizontals
        CompGeo::ITYPE edge_iteration;
        char flag; // p=perimeter, i=inner
    };

    template<typename T> class Pair
    {
    public:
        Pair(void): leftEdge(NULL), rightEdge(NULL), face(NULL) {}
        Pair(const Pair & p): leftEdge(p.leftEdge), rightEdge(p.rightEdge), face(p.face) {}
        Pair(pFace<T> f, pCycleEdge<T> l, pCycleEdge<T> r): face(f), leftEdge(l), rightEdge(r) {}
        ~Pair(void){}

        Pair & operator = (const Pair & a) {leftEdge = a.leftEdge; rightEdge = a.rightEdge; face = a.face; return *this;}
        
        pFace<T> face;
        pCycleEdge<T> leftEdge, rightEdge;
    };


    template<typename T> class QType
    { // for use in event queue tree so that the event queue tree AVL does not delete any Points
    public:
        QType(void): pt(NULL) {}
        QType(const QType & q): pt(q.pt) {}
        QType(const pPoint<T> & p): pt(p) {}
        ~QType(void) {}

        QType & operator = (const QType & q) {pt = q.pt; return *this;} // copying pointers

        friend bool operator < (QType & a, QType & b) {return *(a.pt) < *(b.pt);}
        friend bool operator == (QType & a, QType & b) {return *(a.pt) == *(b.pt);}

        pPoint<T> pt;
    };
    template<typename T> using pQType = QType<T> *;

    template<typename T> class SType
    { // for use in status tree so that the status tree AVL does not delete any Edges
    public:
        SType(void): edge(NULL) {}
        SType(const SType & s): edge(s.edge) {}
        SType(const pEdge<T> & e): edge(e) {}
        ~SType(void) {}

        SType & operator = (const SType & s) {edge = s.edge; return *this;} // copying pointers

        friend bool operator < (SType & a, SType & b) {return *(a.edge) < *(b.edge);}
        friend bool operator == (SType & a, SType & b) {return *(a.edge) == *(b.edge);}

        pEdge<T> edge;
    };
    template<typename T> using pSType = SType<T> *;
    
    template<typename T> struct SReturnType
    { // the result of a query of the status tree S
        std::vector<SType<T>> intersectors; // all edges in S matching the query point/halfedge/edge
        pEdge<T> left_N, right_N; // neighbors of point/halfedge/edge
    };

    // analysis loop structures:
    template<typename T> struct PairBuild
    {
        PairBuild(void): f(NULL), e(NULL), pl(NULL), flag(' ') {};
        pFace<T> f;
        pCycleEdge<T> e;
        pPtLnk<T> pl;
        char flag; // 'R' right lower edge, 'L' left lower edge, 'l' left upper edge, 'D' done (e is NULL)  
    };
    template<typename T> using pPairBuild = PairBuild<T> *;
    
    template<typename T> struct AnalysisType
    { // going left to right through upper Edges
        SReturnType<T> s_q; // S Tree Query
        std::vector<SType<T>> uppers, lowers, containers;
        std::map<CompGeo::ITYPE, PairBuild<T>> pass_on; // key = dcel face index
        std::map<CompGeo::ITYPE, typename std::map<CompGeo::ITYPE, PairBuild<T>>::iterator> lPO; //key = face index
        CompGeo::ITYPE DCount; // number of "Done" entries in pass_on
    };
    // end analysis loop structures

    template <typename T> class faceX
    {
    public:
        faceX(void); 
        faceX(CompGeo::DCEL<T> &);
        ~faceX(void); 
        
        void run(void);
        void handleNeighbors(void);
        void handleLowerHorizontals(void);
        void insertUppers(void);
        pFace<T> consolidateFaces(pFace<T> &, pFace<T> &);
        void consolidateUppers(void);
        void consolidateLowers(void);
        void handleRightUpperCycles(const pEdge<T> &, const bool = true);
        void handleLeftUpperCycles(const pEdge<T> &, const bool = true);
        void makeOverlaps(const pEdge<T> &);
        void handleUppers(void);
        void handleLowers(const bool &);
        bool splitContainers(void); // in member A 
        void separateLowersAndContainers(void); // in member A
        void getEdges(void); // SQuery into A.s_q
        bool getPoint(void); // next point (with uppers) from Q
        void clearA(void);
        void checkX(const pEdge<T> &, const pEdge<T> &); // intersection check
        pEdge<T> splitEdge(const pEdge<T> &, pPoint<T> = NULL); // at eventPt usually
        void initializeQ(void);
        void cleanup(void);
        pPtLnk<T> handlePoint(CompGeo::TPoint<T> &);
        pPtLnk<T> handlePoint(const pPoint<T> &, const T);
        pEdge3D<T> makeEdge(const CompGeo::pTPoint<T> &, const CompGeo::pTPoint<T> &);

        static CompGeo::DCEL<T> * dcel; // initialize before default constructing this
        static pPoint<T> eventPt;
        static CompGeo::ITYPE sweep_iteration, ptIdx, heIdx; // all new halfedges will be @ or above heIdx in dcel
        AVL<QType<T>> Q;
        AVL<SType<T>> S;
        Edge<T> E_CMP;
        AnalysisType<T> A;
    };

}; // end of namespace FaceX