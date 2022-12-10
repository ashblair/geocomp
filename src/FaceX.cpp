#include "first.hpp"
#include "CompGeo.hpp"
#include "FaceX.hpp"

using namespace std;

// static initialization:
template <typename T> CompGeo::DCEL<T> * FaceX::faceX<T>::dcel = NULL;
template <typename T> FaceX::pPoint<T> FaceX::faceX<T>::eventPt = NULL;
template <typename T> CompGeo::ITYPE FaceX::faceX<T>::sweep_iteration = 0;
template<typename T> CompGeo::ITYPE FaceX::faceX<T>::heIdx = 0;
template<typename T> CompGeo::ITYPE FaceX::faceX<T>::ptIdx = 0;

//class Point methods:
template <typename T> FaceX::Point<T>::Point(void) {}

template<typename T> FaceX::Point<T>::Point(const pPtLnk<T> & a)
{
    zb.push_back(a);
    a->prj_point = this;
}


template <typename T> FaceX::Point<T>::~Point(void) 
{
    for(typename vector<pPtLnk<T>>::iterator pit = zb.begin(); pit != zb.end(); ++ pit)
    {
        pPtLnk<T> pzb = *pit;
        pzb->model_point->additionalTPointInfo = NULL;
        delete pzb;
    }
    zb.clear();
    ups.clear(); // edges to be deleted elsewhere separately
}

template <typename T> bool FaceX::Point<T>::IsB4(const Point<T> & b) const
{ // lexicographic order:
    if ((zb.size() == 0) || (b.zb.size() == 0)) return false;
    T xThis = GetX(), yThis = GetY(), xCmp = b.GetX(), yCmp = b.GetY(); 
    return CompGeo::BEq(yThis, yCmp)? xThis < xCmp: yThis > yCmp;
}

template <typename T> bool FaceX::Point<T>::IsEq(const Point<T> & b) const
{
    if ((zb.size() == 0) || (b.zb.size() == 0)) return false;
    T xThis = GetX(), yThis = GetY(), xCmp = b.GetX(), yCmp = b.GetY(); 
    return CompGeo::BEq(xThis, xCmp) && CompGeo::BEq(yThis, yCmp);
}

template <typename T> T FaceX::Point<T>::GetX(void) const 
{
    pPtLnk<T> b = zb.at(0);
    return b->GetX();
}

template <typename T> T FaceX::Point<T>::GetY(void) const
{
    pPtLnk<T> b = zb.at(0);
    return b->GetY();
}

template<typename T> void FaceX::Point<T>::AddLink(const pPtLnk<T> & p0)
{ // order by Z
    p0->prj_point = this;
    T Z0 = p0->model_point->xyz.Z, X0 = p0->model_point->xyz.X, Y0 = p0->model_point->xyz.Y;
    for (typename vector<pPtLnk<T>>::iterator pit = zb.begin(); pit < zb.end(); ++pit)
    {
        pPtLnk<T> p1 = *pit;
        T Z1 = p1->model_point->xyz.Z, X1 = p1->model_point->xyz.X, Y1 = p1->model_point->xyz.Y;
        // model points must be unique but equal in x and y
        assert (!CompGeo::BEq(Z0, Z1) && CompGeo::BEq(X0, X1) && CompGeo::BEq(Y0, Y1)); 
        if (Z0 < Z1) 
        {
            zb.insert(pit, p0);
            return;
        }
    }
    zb.push_back(p0);
}

template<typename T> FaceX::pPtLnk<T> FaceX::Point<T>::CheckBuffer(const T z)
{
    for (typename vector<pPtLnk<T>>::iterator pit = zb.begin(); pit != zb.end(); ++pit)
    {
        pPtLnk<T> p = *pit;
        if (CompGeo::BEq(z, p->GetZ())) return p;

    }
    return NULL;
}

template<typename T> FaceX::pPtLnk<T> FaceX::Point<T>::GetLink(void)
{
    pPtLnk<T> L = zb[0];
    return L;
}

template<typename T> void FaceX::Point<T>::AddUpper(const pEdge<T> & e)
{ // sort uppers after containers have been added
    SType<T> st = SType<T>(e);
    ups.push_back(st);
}

// end class Point methods

// struct Cycle methods:

template<typename T> void FaceX::Cycle<T>::AddElement(const pCycleEdge<T> & c)
{
    CompGeo::ITYPE eSz = elements.size();
    elements.push_back(c);
    if (eSz == 0) 
    {
        highest = 0; lowest = 0; return;
    }
    pCycleEdge<T> cLo = elements[lowest], cHi = elements[highest];
    CompGeo::pTPoint<T> oIn = c->oPt, oLo = cLo->oPt, oHi = cHi->oPt;
    pPtLnk<T> lIn = static_cast<pPtLnk<T>>(oIn->additionalTPointInfo),
        lLo = static_cast<pPtLnk<T>>(oLo->additionalTPointInfo),
        lHi = static_cast<pPtLnk<T>>(oHi->additionalTPointInfo);
    Point<T> pIn = *(lIn->prj_point), pLo = *(lLo->prj_point), pHi = *(lHi->prj_point);
    if (pIn < pLo) lowest = eSz;
    if (pHi < pIn) highest = eSz;     

}

template<typename T> void FaceX::Cycle<T>::GetOrientation(void)
{
    CompGeo::ITYPE eSz = elements.size();
    assert (eSz >= 3);
    pCycleEdge<T> cLo = elements[lowest], 
        cPv = elements[(lowest + eSz - 1) % eSz], 
        cNx = elements[(lowest + 1) % eSz];
    CompGeo::XY pLo = CompGeo::XY(cLo->oPt->xyz), 
        pPv = CompGeo::XY(cPv->oPt->xyz),
        pNx = CompGeo::XY(cNx->oPt->xyz),
        AB = pLo - pPv, AC = pNx - pLo;
    double cross = CompGeo::Cross(AB, AC);
    assert (fabs(cross) > MAX_FLT_PRECISION);
    orientation = cross < 0.0? 'L': 'R'; // handedness: L = left handed, R = right
}


/*
from geomalgorithms.com/a03-_inclusion.html:
Here is a "C++" implementation of the winding number algorithm for the inclusion of a point in polygon. We just give the 2D case, and use the simplest structures for a point and a polygon which may differ in your application.

// Copyright 2000 softSurfer, 2012 Dan Sunday
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
 

// a Point is defined by its coordinates {int x, y;}
//===================================================================
 

// isLeft(): tests if a point is Left|On|Right of an infinite line.
//    Input:  three points P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2  on the line
//            <0 for P2  right of the line
//    See: Algorithm 1 "Area of Triangles and Polygons"
inline int
isLeft( Point P0, Point P1, Point P2 )
{
    return ( (P1.x - P0.x) * (P2.y - P0.y)
            - (P2.x -  P0.x) * (P1.y - P0.y) );
}
//===================================================================


// cn_PnPoly(): crossing number test for a point in a polygon
//      Input:   P = a point,
//               V[] = vertex points of a polygon V[n+1] with V[n]=V[0]
//      Return:  0 = outside, 1 = inside
// This code is patterned after [Franklin, 2000]
int
cn_PnPoly( Point P, Point* V, int n )
{
    int    cn = 0;    // the  crossing number counter

    // loop through all edges of the polygon
    for (int i=0; i<n; i++) {    // edge from V[i]  to V[i+1]
       if (((V[i].y <= P.y) && (V[i+1].y > P.y))     // an upward crossing
        || ((V[i].y > P.y) && (V[i+1].y <=  P.y))) { // a downward crossing
            // compute  the actual edge-ray intersect x-coordinate
            float vt = (float)(P.y  - V[i].y) / (V[i+1].y - V[i].y);
            if (P.x <  V[i].x + vt * (V[i+1].x - V[i].x)) // P.x < intersect
                 ++cn;   // a valid crossing of y=P.y right of P.x
        }
    }
    return (cn&1);    // 0 if even (out), and 1 if  odd (in)

}
//===================================================================


// wn_PnPoly(): winding number test for a point in a polygon
//      Input:   P = a point,
//               V[] = vertex points of a polygon V[n+1] with V[n]=V[0]
//      Return:  wn = the winding number (=0 only when P is outside)
int
wn_PnPoly( Point P, Point* V, int n )
{
    int    wn = 0;    // the  winding number counter

    // loop through all edges of the polygon
    for (int i=0; i<n; i++) {   // edge from V[i] to  V[i+1]
        if (V[i].y <= P.y) {          // start y <= P.y
            if (V[i+1].y  > P.y)      // an upward crossing
                 if (isLeft( V[i], V[i+1], P) > 0)  // P left of  edge
                     ++wn;            // have  a valid up intersect
        }
        else {                        // start y > P.y (no test needed)
            if (V[i+1].y  <= P.y)     // a downward crossing
                 if (isLeft( V[i], V[i+1], P) < 0)  // P right of  edge
                     --wn;            // have  a valid down intersect
        }
    }
    return wn;
}
//===================================================================
*/

template<typename T> bool FaceX::Cycle<T>::IsWithin(Cycle<T> & c)
{ // cycles are guaranteed to be non-overlapping when this is called
  // so, checking one point will be sufficient 
  // if the bouding box check is not
    pCycleEdge<T> cLo0 = elements[lowest], cHi0 = elements[highest],
        cLo1 = c.elements[lowest], cHi1 = c.elements[highest];
    CompGeo::pTPoint<T> oLo0 = cLo0->oPt, oHi0 = cHi0->oPt,
        oLo1 = cLo1->oPt, oHi1 = cHi1->oPt;
    CompGeo::XY LH0 = CompGeo::XY(oLo0->xyz), RL0 = CompGeo::XY(oHi0->xyz),
        LH1 = CompGeo::XY(oLo1->xyz), RL1 = CompGeo::XY(oHi1->xyz);
    bool xWithin = (LH0.x > LH1.x) && (RL0.x < RL1.x),
        yWithin = (LH0.y < LH1.y) && (RL0.y > RL1.y);
    if (!(xWithin && yWithin)) return false;
    
    pCycleEdge<T> samp = elements[0];
    int wn = c.getWindingNumber(samp);
    return (wn != 0);
    //for (typename vector<pCycleEdge<T>>::iterator eit = elements.begin(); eit != elements.end(); ++eit)
    //{
    //    int wn = c.getWindingNumber(*eit);
    //    if (wn == 0) return false;
    //}
    
    //return true;
}

template<typename T> int FaceX::Cycle<T>::IsLeft(const CompGeo::BasicTPoint<T> & P0, const CompGeo::BasicTPoint<T> & P1, const CompGeo::BasicTPoint<T> & P2)
{ // P0 & P1 are ordered points on polygon p; l is the line through P0 & P1; 
  // P2 is not on p; IsLeft returns 0 iff P2 is on l, +1 iff P2 is left of l & -1 iff P2 is right
    CompGeo::BasicTPoint<T> D1 = P1 - P0, D2 = P2 - P0;
    T crs = D1.X * D2.Y - D2.X * D1.Y, zero = static_cast<T>(0);
    bool IZ = CompGeo::BEq(zero, crs), IN = crs < zero, IP = crs > zero;
    if (IZ) return 0;
    if (IN) return -1;
    if (IP) return 1;
    assert(1 == 2);
    return 0;
}

template<typename T> int FaceX::Cycle<T>::getWindingNumber(const pCycleEdge<T> & C)
{ // returns 0 for outside points and >0 for inside points
    int    wn = 0;    // the  winding number counter

    // loop through all edges of the polygon
    CompGeo::ITYPE eSz = elements.size();
    CompGeo::BasicTPoint<T> P = C->oPt->xyz;
    for (CompGeo::ITYPE i = 0; i < eSz; ++i)    // edge from V[i] to  V[i+1]
    {
        pCycleEdge<T> c_i = elements[i], c_n = elements[(i + 1) % eSz];
        CompGeo::BasicTPoint<T> v_i = c_i->oPt->xyz, v_n = c_n->oPt->xyz;
        if ((v_i.Y < P.Y) || (CompGeo::BEq(v_i.Y, P.Y)))  // start y <= P.y
        {
            if (v_n.Y > P.Y)  // an upward crossing
                 if (IsLeft( v_i, v_n, P) > 0)  // P left of  edge
                     ++wn;            // have  a valid up intersect
        }
        else                         // start y > P.y (no test needed)
        {
            if ((v_n.Y < P.Y) || (CompGeo::BEq(v_n.Y, P.Y)))     // a downward crossing
                 if (IsLeft(v_i, v_n, P) < 0)  // P right of  edge
                     --wn;            // have  a valid down intersect
        }
    }
    assert (wn >= 0);
    return wn;

}
// end struct Cycle methods

// struct CycleEdge methods:

template<typename T> void FaceX::CycleEdge<T>::MovePair(const pCycleEdge<T> & c)
{ // moves pair from this to c
    if (pair != NULL)
    {
        // changing POV: r_l is relative to Edge & pair is relative to face
        if (r_l == 'R') pair->leftEdge = c; 
        else pair->rightEdge = c;
    }
    c->pair = pair;
    pair = NULL;

}

// end struct CycleEdge methods

// class OriginEdge methods:

template<typename T> FaceX::OriginEdge<T>::OriginEdge(void)
{
    oID = 0;
}

template<typename T> FaceX::OriginEdge<T>::OriginEdge(const CompGeo::ITYPE & vv)
{
    oID = vv;

}

template<typename T> FaceX::OriginEdge<T>::OriginEdge(const OriginEdge & oe)
{
    oID = oe.oID;
    edges = oe.edges;
}

template<typename T> FaceX::OriginEdge<T>::~OriginEdge(void)
{

}

template<typename T> FaceX::OriginEdge<T> & FaceX::OriginEdge<T>::operator =(const OriginEdge<T> & oe)
{
    oID = oe.oID;
    edges = oe.edges;
    return *this;
}

template<typename T> bool FaceX::OriginEdge<T>::IsLT(const OriginEdge & oe) const
{
    return oID < oe.oID;
}

template<typename T> bool FaceX::OriginEdge<T>::IsEq(const OriginEdge & oe) const
{
    return oID == oe.oID;

}

// end of class OriginEdge methods:

// class Face methods:

template<typename T> FaceX::Face<T>::Face(void)
{
    pface = NULL;
    pairCount = 0;
    xface = false;
    below = false;
}

template<typename T> FaceX::Face<T>::Face(const CompGeo::pFaceType & f)
{
    pface = f;
    pairCount = 0;
    xface = false;
    below = false;
}

template<typename T> FaceX::Face<T>::~Face(void)
{

}

template<typename T> bool FaceX::Face<T>::IsLT(const Face<T> & f) const
{
    return pface->viewface < f.pface->viewface;
}

template<typename T> bool FaceX::Face<T>::IsEq(const Face<T> & f) const
{
    return pface->viewface == f.pface->viewface;
}

template <typename T> CompGeo::TPoint<T> FaceX::Face<T>::getFacePoint(const CompGeo::pTPoint<T> & onPt, const pPtLnk<T> & offPt)
{ // onPt is a point on this face, offPt should be within this face on the projection plane (above or below)
  // return point will equal offPt in x and y, it's z will be recalculated to be on the face's plane
    pPtLnk<T> pn = static_cast<pPtLnk<T>>(onPt->additionalTPointInfo);
    CompGeo::XYZ p0 = pn->GetXYZ(), p1 = offPt->GetXYZ();
    p1.z = 0.0;
    CompGeo::XYZ r = p1, n = pface->norm;
    r.z = n * (p0 - p1) / n.z; // n.z < 0 due to normal cull
    CompGeo::TPoint<T> rtn = CompGeo::TPoint<T>(offPt->GetX(), offPt->GetY(), static_cast<T>(r.z));
    return rtn; 
}

template<typename T> void FaceX::Face<T>::AddCycleEdge(const pCycleEdge<T> & c)
{
    cycle_edges[c->pHE->viewedge] = c;
}

template<typename T> void FaceX::Face<T>::FinalizeFace(CompGeo::DCEL<T> * d)
{
    if (!xface) 
    {
        pface->flag = 'V'; // visible
        return;
    }
    if (cycle_edges.size() == 0)
    {
        pface->flag = 'H'; // hidden
        return;
    }

    CompGeo::ITYPE fIdx = pface->viewface;
    vector<Cycle<T>> partitions;
    GetCycles(partitions);

    // finding the top cycle + maintenance on the lower cycles:
    for (typename vector<Cycle<T>>::iterator cit0 = partitions.begin(); cit0 != partitions.end(); ++cit0)
    {
        map<CompGeo::ITYPE, OverlapCycle<T>> olaps;
        Cycle<T> cycle = *cit0;
        CompGeo::ITYPE lCount = GetOverlaps(olaps, cycle);
        typename map<CompGeo::ITYPE, OverlapCycle<T>>::iterator olitMax = olaps.begin(), olit0 = olitMax, olit1 = olit0;
        if (lCount > 1)
        {
            ++olit1;
            while (olit1 != olaps.end())
            {
                pOverlapCycle<T> o0 = &(olit0->second), o1 = &(olit1->second);
                if (o1->IsAbove(*o0)) olitMax = olit1;

                olit0 = olit1;
                ++olit1;
            }
            pOverlapCycle<T> oT = &(olitMax->second);
            oT->IsTop = true;
            for (olit0 = olaps.begin(); olit0 != olaps.end(); ++olit0)
            {
                pOverlapCycle<T> o0 = &(olit0->second);
                pFace<T> o_face = o0->face;
                if (!o0->IsTop)
                {
                    o_face->below = true;
                    o_face->RemoveCycles(o0->cycles);
                    if (o_face == this)
                    { // check for and remove twin cycleedges from top face overlap cycle:
                        for (typename vector<pCycleEdge<T>>::iterator tit = oT->cycles.begin(); tit != oT->cycles.end(); ++tit)
                        { 
                            pCycleEdge<T> TC0 = *tit;
                            char drct = TC0->r_l;
                            pEdge3D<T> e = TC0->e3;
                            pCycleEdge<T> TC1 = drct == 'R'? e->leftCycle: e->rightCycle; 
                            CompGeo::ITYPE tCount = oT->face->cycle_edges.count(TC1->pHE->viewedge);
                            if (tCount > 0)
                            {
                                vector<pCycleEdge<T>> dbuff = {TC0, TC1};
                                oT->face->RemoveCycles(dbuff);
                            }
                        }
                    }
                }
            }
        }
    }

    if (!below) 
    {
        pface->flag = 'V'; // visible
        return;
    }

    pface->flag = 'H'; // hidden

    int rtCount = RemoveTwins();
    GetCycles(partitions); // cycles have been removed

    // check for holes:
    vector<Cycle<T>> checked;
    vector<CompGeo::ITYPE> holes, fills;
    CompGeo::ITYPE pSz = partitions.size();
    for (CompGeo::ITYPE i = 0; i < pSz; ++i)
    {
        Cycle<T> cycle = partitions[i];
        char o = cycle.orientation;
        assert((o == 'L') || (o == 'R'));
        if (cycle.orientation == 'L') holes.push_back(i);
        else fills.push_back(i);
    }
    if (holes.size() == 0) checked = partitions;
    else
    {
        for (vector<CompGeo::ITYPE>::iterator hit = holes.begin(); hit != holes.end(); ++hit)
        {
            pCycle<T> cyc_h = &(partitions[*hit]);
            bool outerFound = false;
            for (vector<CompGeo::ITYPE>::iterator fit = fills.begin(); fit != fills.end(); ++fit)
            {
                pCycle<T> cyc_f = &(partitions[*fit]);
                if (cyc_h->IsWithin(*cyc_f))
                {
                    outerFound = true;
                    cyc_f->holes.push_back(cyc_h);
                    break;
                } 
            }
            assert(outerFound);
        }
        for (vector<CompGeo::ITYPE>::iterator fit = fills.begin(); fit != fills.end(); ++fit)
        {
            checked.push_back(partitions[*fit]);
        }
    }
    
    // chain connected faces through additionalFaceInfo:
    CompGeo::pFaceType f_prev = pface;
    
    for (typename vector<Cycle<T>>::iterator cit0 = checked.begin(); cit0 != checked.end(); ++cit0)
    {
        //vector<pCycleEdge<T>> part = *cit0;
        Cycle<T> cycle = *cit0;
        CompGeo::pFaceType f = MakeFace(d, cycle);
        f->additionalFaceInfo = f_prev;
        f_prev = f;
    }

    f_prev->additionalFaceInfo = pface;
}

template<typename T> void FaceX::Face<T>::GetCycles(vector<Cycle<T>> & partitions)
{
    partitions.clear();

    map<CompGeo::ITYPE, OriginEdge<T>> origin_lookup; // key = viewvertex of halfedge origin

    for (typename map<CompGeo::ITYPE, pCycleEdge<T>>::iterator cit = cycle_edges.begin(); cit != cycle_edges.end(); ++cit)
    {
        pCycleEdge<T> c = cit->second;
        CompGeo::ITYPE vv = c->oPt->viewvertex;
        typename map<CompGeo::ITYPE, OriginEdge<T>>::iterator oit = origin_lookup.find(vv);
        if (oit == origin_lookup.end())
        { // not found:
            OriginEdge<T> o;
            o.oID = vv;
            o.edges.push_back(c);
            origin_lookup[vv] = o;
        }
        else
        { // found:
            pOriginEdge<T> o = &(oit->second);
            assert (o->oID == c->oPt->viewvertex);
            o->edges.push_back(c);
        }
        
    }

    while (origin_lookup.size() > 0)
    {
        Cycle<T> cycle;
        //vector<pCycleEdge<T>> part;
        pOriginEdge<T> o = &(origin_lookup.begin()->second);
        pCycleEdge<T> c = o->edges[o->edges.size() - 1], c_n = NULL;
        o->edges.pop_back();
        //part.push_back(c);
        cycle.AddElement(c);
        if (o->edges.size() == 0) origin_lookup.erase(origin_lookup.begin());
        CompGeo::ITYPE oIdx0 = c->oPt->viewvertex, nIdx = c->nPt->viewvertex;
        while (nIdx != oIdx0)
        {
            typename map<CompGeo::ITYPE, OriginEdge<T>>::iterator oit = origin_lookup.find(nIdx);
            assert (oit != origin_lookup.end()); // there must be a match
            pOriginEdge<T> o_n = &(oit->second);
            CompGeo::XY A = CompGeo::XY(c->oPt->xyz), O = CompGeo::XY(c->nPt->xyz);
            CompGeo::UnitCircleMeasureType uMax;
            typename vector<pCycleEdge<T>>::iterator citMax = o_n->edges.begin();
            for (typename vector<pCycleEdge<T>>::iterator cit = o_n->edges.begin(); cit != o_n->edges.end(); ++cit)
            {
                pCycleEdge<T> cLoop = *cit;
                CompGeo::XY B = CompGeo::XY(cLoop->nPt->xyz);
                CompGeo::UnitCircleMeasureType u = CompGeo::UnitCircleMeasureType(A, O, B);
                if (u > uMax) 
                { // using principle of greatest left
                    c_n = cLoop;
                    uMax = u;
                    citMax = cit;
                }
            }
            o_n->edges.erase(citMax);
            if (o_n->edges.size() == 0) origin_lookup.erase(oit);
            c = c_n;
            //part.push_back(c);
            cycle.AddElement(c);
            nIdx = c->nPt->viewvertex;
        }
        cycle.GetOrientation();
        //partitions.push_back(part);
        partitions.push_back(cycle);
    }

}

template<typename T> void FaceX::Face<T>::RemoveCycles(const vector<pCycleEdge<T>> & h)
{
    for (typename vector<pCycleEdge<T>>::const_iterator hit = h.begin(); hit != h.end(); ++hit)
    {
        pCycleEdge<T> hSrc = *hit;
        CompGeo::ITYPE hIdx = hSrc->pHE->viewedge, eCount = cycle_edges.erase(hIdx);
        assert (eCount == 1);
    }
}

template<typename T> CompGeo::ITYPE FaceX::Face<T>::GetOverlaps(map<CompGeo::ITYPE, OverlapCycle<T>> & laps0, const Cycle<T> & cycle)
{
    laps0.clear();
    CompGeo::ITYPE lCount = 0;
    for (typename vector<pCycleEdge<T>>::const_iterator cit = cycle.elements.begin(); cit != cycle.elements.end(); ++cit)
    {
        pCycleEdge<T> c = *cit;
        char drct = c->r_l;
        pEdge<T> e = c->e3->e;
        map<CompGeo::ITYPE, OverlapCycle<T>> laps;
        for (typename vector<pEdge3D<T>>::const_iterator eit = e->model_edges.begin(); eit != e->model_edges.end(); ++eit)
        {
            pEdge3D<T> e3 = *eit;
            pCycleEdge<T> cLoop = drct == 'R'? e3->rightCycle: e3->leftCycle;
                //cTwin = drct == 'R'? e3->leftCycle: e3->rightCycle;
            CompGeo::ITYPE ifIdx = cLoop->face->pface->viewface;
            assert (ifIdx > 0); 

            OverlapCycle<T> oc;
            oc.face = cLoop->face;
            //oc.halfs.push_back(cLoop->pHE->viewedge);
            oc.cycles.push_back(cLoop);
            laps[ifIdx] = oc; 

        }
        if (cit == cycle.elements.begin())  // initialization
        {
            laps0 = laps; 
            lCount = laps0.size();
        }
        else 
        {   // compare laps0 to laps: keep shared OverlapCycles & discard others
            typename map<CompGeo::ITYPE, OverlapCycle<T>>::iterator lit0 = laps0.begin(), lit0_n = lit0,
                lit1 = laps.begin(), lit1_n = lit1;
            while (lit0 != laps0.end())
            {
                CompGeo::ITYPE f0 = lit0->first, f1 = 0; // these are face indices
                while (lit1 != laps.end())
                {
                    f1 = lit1->first;
                    if (f1 < f0) 
                    {
                        lit1_n = lit1; ++lit1_n;
                        laps.erase(lit1);
                        lit1 = lit1_n;
                    }
                    else break;
                }
                if (f0 == f1) 
                {
                    pOverlapCycle<T> oc0 = &(lit0->second), oc1 = &(lit1->second);
                    //assert(oc1->halfs.size() == 1); // one face cycle per Edge
                    //oc0->halfs.insert(oc0->halfs.end(), oc1->halfs.begin(), oc1->halfs.end());
                    //oc0->halfs.push_back(oc1->halfs[0]);
                    oc0->cycles.push_back(oc1->cycles[0]);

                    ++lit0;
                }
                else 
                { // unshared OverlapCycle in laps0 - erase
                    lit0_n = lit0; ++lit0_n;
                    laps0.erase(lit0);
                    lit0 = lit0_n;
                    --lCount;
                }
            }
        }
        if (lCount == 0) break;
    }
    return lCount;

}

template<typename T> int FaceX::Face<T>::RemoveTwins(void)
{
    int rCount = 0;
    typename map<CompGeo::ITYPE, pCycleEdge<T>>::iterator eit = cycle_edges.begin(), eit_n = eit;
    while (eit != cycle_edges.end())
    {
        pCycleEdge<T> c0 = eit->second;
        assert (c0->face == this);
        pEdge3D<T> e3 = c0->e3; 
        char direction = c0->r_l;
        pCycleEdge<T> c1 = direction == 'R'? e3->leftCycle: e3->rightCycle;
        CompGeo::pHalfEdgeType h = c1->pHE;
        if (c1->face == this)
        {
            typename map<CompGeo::ITYPE, pCycleEdge<T>>::iterator fit = cycle_edges.find(h->viewedge);
            if (fit != cycle_edges.end())
            {
                eit_n = eit;
                ++eit_n;
                if (eit_n == fit) ++eit_n;
                cycle_edges.erase(eit);
                cycle_edges.erase(fit);
                eit = eit_n;
                rCount += 2;
            }
            else ++eit;
        }
        else ++eit;
    }
    return rCount;
}

template<typename T> CompGeo::pFaceType FaceX::Face<T>::MakeFace(CompGeo::DCEL<T> * d, const Cycle<T> & cycle)
{
    CompGeo::pFaceType f = new CompGeo::FaceType(*pface);
    CompGeo::ITYPE fIdx = d->push(f), cSz = cycle.elements.size();
    f->viewface = fIdx;
    CompGeo::pHalfEdgeType * hArray = new CompGeo::pHalfEdgeType[cSz], hBuff = NULL;
    for (CompGeo::ITYPE i = 0; i < cSz; ++i)
    {
        pCycleEdge<T> c = cycle.elements[i];
        hArray[i] = c->pHE;
    }
    hBuff = hArray[0];
    f->outer = hBuff->viewedge;    
    for (CompGeo::ITYPE i = 0; i < cSz; ++i)
    {
        CompGeo::pHalfEdgeType h = hArray[i], 
            h_n = hArray[(i + 1) % cSz],
            h_p = hArray[(i + cSz - 1) % cSz];
        h->incidentface = fIdx;
        h->next = h_n->viewedge;
        h->prev = h_p->viewedge;

    }

    CompGeo::pHalfEdgeListType he0 = NULL, hePrev = NULL;
    for (typename vector<pCycle<T>>::const_iterator hit = cycle.holes.begin(); hit != cycle.holes.end(); ++hit)
    {
        pCycle<T> innerComp = *hit;
        CompGeo::pHalfEdgeListType he = new CompGeo::HalfEdgeListType;
        CompGeo::ITYPE heIdx = d->push(he), icSz = innerComp->elements.size();
        he->halfedgelist = heIdx;
        CompGeo::pHalfEdgeType * hICArray = new CompGeo::pHalfEdgeType[icSz], hICBuff = NULL;
        for (CompGeo::ITYPE i = 0; i < icSz; ++i)
        {
            pCycleEdge<T> c = innerComp->elements[i];
            hICArray[i] = c->pHE;
        }
        hICBuff = hICArray[0];
        he->helsttop = hICBuff->viewedge;    
        for (CompGeo::ITYPE i = 0; i < icSz; ++i)
        {
            CompGeo::pHalfEdgeType h = hICArray[i], 
                h_n = hArray[(i + 1) % icSz],
                h_p = hArray[(i + icSz - 1) % icSz];
            h->incidentface = fIdx;
            h->next = h_n->viewedge;
            h->prev = h_p->viewedge;
        }
        if (he0 == NULL)
        {
            he0 = he; f->inner = heIdx;
        }
        else
        {
            hePrev->nxt = heIdx;
        }
        hePrev = he;
        delete [] hICArray;        
    }

    delete [] hArray;
    f->flag = 'V'; // Visible
    return f;
}


// end of class Face methods

// class Edge3D methods:

template<typename T> FaceX::Edge3D<T>::Edge3D(void)
{
    e = NULL;
    rightCycle = NULL;
    leftCycle = NULL;
    flag = ' '; 
}

template<typename T> FaceX::Edge3D<T>::Edge3D(const pCycleEdge<T> & r, const pCycleEdge<T> & l)
{
    e = NULL;
    rightCycle = r;
    leftCycle = l;
    flag = 'C'; // for complete
    r->e3 = this;
    l->e3 = this;
}

template<typename T> FaceX::Edge3D<T>::~Edge3D(void)
{

}


// end of Edge3D methods

//class Edge methods:

template <typename T> FaceX::Edge<T>::Edge(void) 
{
    horizontal = false;
    xOnSweep = 0.0;
    w = 0.0; 
    topPoint = NULL;
    bottomPoint = NULL;
    edge_iteration= 0;
    flag = ' '; 
}

template<typename T> FaceX::Edge<T>::Edge(const pEdge3D<T> & a)
{
    pCycleEdge<T> c = a->rightCycle;
    pPtLnk<T> tL = static_cast<pPtLnk<T>>(c->oPt->additionalTPointInfo), 
            bL = static_cast<pPtLnk<T>>(c->nPt->additionalTPointInfo);
    topPoint = tL->prj_point;
    bottomPoint = bL->prj_point;

    assert (*topPoint < *bottomPoint);
    initializeEdge();
    flag = ' ';
    model_edges.push_back(a);
    a->e = this;

}

template <typename T> FaceX::Edge<T>::~Edge(void) 
{

}

template<typename T> void FaceX::Edge<T>::initializeEdge(void)
{
    xOnSweep = static_cast<double>(topPoint->GetX());
    if (CompGeo::BEq(topPoint->GetY(), bottomPoint->GetY()))
    {
        horizontal = true;
        w = static_cast<double>(bottomPoint->GetX());
    }
    else
    {
        horizontal = false;
        double  y1 = static_cast<double>(topPoint->GetY()),
                y2 = static_cast<double>(bottomPoint->GetY()),
                x1 = static_cast<double>(topPoint->GetX()),
                x2 = static_cast<double>(bottomPoint->GetX());
        w = (x1 - x2) / (y1 - y2);
        //think of an idealized clock with infinitesimal hand points
        //the clock center is like the top point, the end of the hand is the bottom
        //w therefore goes from almost +infinity just before 9 o'clock 
        //to -infinity at 3 (it's 0 at 6 o'clock i.e. the vertical)

    }
    
    edge_iteration = 0;

}

template <typename T> void FaceX::Edge<T>::updateXs(void)
{ // calculations of f(y) for x on sweep line
    if (edge_iteration == faceX<T>::sweep_iteration) return;
    edge_iteration = faceX<T>::sweep_iteration;
    pPoint<T> ep = faceX<T>::eventPt;
    double xVal = static_cast<double>(ep->GetX()),
        yVal = static_cast<double>(ep->GetY()),
        y_t = static_cast<double>(topPoint->GetY()),
        x_t = static_cast<double>(topPoint->GetX());
    if (horizontal) 
    { // a horizontal is in S only while the sweep line stays at y = yVal
      // it is inserted from the uppers in its left point (top point)
      // the sweep is horizontal from the left point (top) to the right point (bottom)
      // after which the Edge is removed from S
        //assert (xVal <= w); // w is bottom (i.e. right-most) x of Edge
        //if (xVal > x_t) xOnSweep = xVal;
        xOnSweep = xVal;
    }
    else
    {
        xOnSweep = w * (yVal - y_t) + x_t;
    }
}

template<typename T> void FaceX::Edge<T>::AddModelEdge(const pEdge3D<T> & e3)
{
    e3->e = this;
    model_edges.push_back(e3);
}

template <typename T> bool FaceX::Edge<T>::IsB4(Edge & b) 
{
    updateXs();
    b.updateXs();
    if (fabs(xOnSweep - b.xOnSweep) < MAX_FLT_PRECISION)
    {
        // horizontals need to be last i.e. not less than other halfedges:
        if (horizontal && b.horizontal) return w < b.w; // w is right-most x
        if (horizontal && !b.horizontal) return false;
        if (!horizontal && b.horizontal) return true;
        return w > b.w; // w is inverse slope (i.e. run/rise)
    }
    return xOnSweep < b.xOnSweep;

}

template <typename T> bool FaceX::Edge<T>::IsEq(Edge & b)
{
    updateXs();
    b.updateXs();
    return fabs(xOnSweep - b.xOnSweep) < MAX_FLT_PRECISION;
}

template<typename T> bool FaceX::Edge<T>::IsOver(Edge & b)
{ // call to order/consolidate uppers
  // so the top points will be equal
    assert (*topPoint == *b.topPoint);
    bool both = horizontal && b.horizontal, one = horizontal || b.horizontal;
    if (both) return true;
    if (one) return false;
    return fabs(w - b.w) < MAX_FLT_PRECISION;
}

// end of class Edge methods


// class faceX methods:

template <typename T> FaceX::faceX<T>::faceX(void) 
{
    if (dcel == NULL)
    {
        cout << "Error: static variable FaceX::dcel needs to be initialized!\n";
        return;
    }
    heIdx = dcel->getHalfEdgeCount(); // at completion: all halfedges above this will be new
    ptIdx = dcel->getVertexCount(); // at completion: all points above this will be new

}

template<typename T> FaceX::faceX<T>::faceX(CompGeo::DCEL<T> & lst)
{ // DCEL needs to be a view list from BIH
    dcel = &lst;
    heIdx = dcel->getHalfEdgeCount();
    ptIdx = dcel->getVertexCount();
    E_CMP.edge_iteration = 0;
    E_CMP.xOnSweep = 0.0;
    E_CMP.w = 0.0; // representing a vertical edge
    run();
}

template <typename T> FaceX::faceX<T>::~faceX(void) 
{
    dcel = NULL;
    eventPt = NULL;
    sweep_iteration = 0;
}

template<typename T> void FaceX::faceX<T>::run(void)
{
    initializeQ();
    while (!getPoint())
    {
        clearA();
        getEdges();
        separateLowersAndContainers();
        bool newlows = splitContainers();
        handleLowers(newlows);
        handleUppers();
        insertUppers();
        handleNeighbors();
    }
    cleanup();
}

template<typename T> void FaceX::faceX<T>::handleNeighbors(void)
{ // check for intersections with the neighbors
    pEdge<T> l_n = A.s_q.left_N, r_n = A.s_q.right_N;
    CompGeo::ITYPE uSz = A.uppers.size();

    bool NLN = l_n == NULL,
        NRN = r_n == NULL,
        UHE = uSz > 0;
    
    if (NLN && NRN) return;
    if (!NRN && UHE)
    {
        pEdge<T> rightmost = A.uppers[uSz - 1].edge;
        checkX(rightmost, r_n); 
    }
    if (!NLN && UHE)
    {
        pEdge<T> leftmost = A.uppers[0].edge;
        checkX(l_n, leftmost);
    }
    if (!NLN && !NRN && !UHE) checkX(l_n, r_n);

}

template<typename T> void FaceX::faceX<T>::handleLowerHorizontals(void)
{ // putting horizontals first after sorting has made all other lowers right to left
  // there should be at most one horizontal Edge on end
    CompGeo::ITYPE lSz = A.lowers.size();
    if (lSz < 2) return;
    SType<T> first = A.lowers[0], last = A.lowers[lSz - 1];
    if ((first.edge->horizontal) || !(last.edge->horizontal)) return;
    A.lowers.insert(A.lowers.begin(), last);
    A.lowers.pop_back();
    assert (!(A.lowers[lSz - 1].edge->horizontal));
    //vector<SType<T>> newlo;
    //for (CompGeo::ITYPE i = lSz; i > 0; --i)
    //{
    //    SType<T> st = A.lowers[i - 1];
    //    if (st.edge->horizontal) 
    //    {
    //        newlo.push_back(st);
    //        A.lowers.pop_back();
    //    }
    //    else break;
    //}
    //newlo.insert(newlo.end(), A.lowers.begin(), A.lowers.end());
    //A.lowers = newlo;
}

template<typename T> void FaceX::faceX<T>::insertUppers(void)
{
    for (typename vector<SType<T>>::iterator sit = A.uppers.begin(); sit != A.uppers.end(); ++ sit)
    {
        SType<T> st = *sit;
        pSType<T> pst = new SType<T>(st);
        bool inserted = S.Insert(pst);
    }
}

template<typename T> FaceX::pFace<T> FaceX::faceX<T>::consolidateFaces(pFace<T> & f1, pFace<T> & f2)
{ // because of possible concavities there may be 2 pFaces for 1 face
  // this will be detected in handleLowers which will call this to fix

    pFace<T> fD = f1, fK = f2;
    if (fD->cycle_edges.size() > fK->cycle_edges.size())
    {
        fD = f2; fK = f1;
    }
    fK->pairCount += fD->pairCount;
    fK->below = fK->below || fD->below; 
    fK->xface = fK->xface || fD->xface;
    for (typename map<CompGeo::ITYPE, pCycleEdge<T>>::iterator cit = fD->cycle_edges.begin(); cit != fD->cycle_edges.end(); ++cit)
    {
        pCycleEdge<T> c = cit->second;
        c->face = fK;
        if (c->pair != NULL) c->pair->face = fK;
        fK->cycle_edges.insert(*cit);
    }
    delete fD;
    return fK;

}


template<typename T> void FaceX::faceX<T>::consolidateUppers(void)
{ // consolidation puts all overlapping edges into one
    if (A.uppers.size() < 2) return;
    typename vector<SType<T>>::iterator uit = A.uppers.begin(), uit0 = uit;
    ++uit;
    while (uit != A.uppers.end())
    {
        pEdge<T> E[] = {(*uit0).edge, (*uit).edge}, e;
        pPoint<T> PT, PB, P;
        bool both = E[1]->horizontal && E[0]->horizontal, 
            one = E[1]->horizontal || E[0]->horizontal,
            consolidate = false;
        int lIdx = 0, sIdx = 0;
        if (both)
        { // consolidate horizontals (w is x of bottomPt):
            consolidate = true;
            if (fabs(E[1]->w - E[0]->w) > MAX_FLT_PRECISION)
            { // edges are not same length
                lIdx = E[0]->w > E[1]->w? 0: 1;
                sIdx = (lIdx + 1) % 2;
            }
        }
        else
        {
            if ((!one) && (fabs(E[1]->w - E[0]->w) < MAX_FLT_PRECISION))
            { // consolidate 2 non-horizontals:
                consolidate = true;
                T Y0 = E[0]->bottomPoint->GetY(), Y1 = E[1]->bottomPoint->GetY();
                if (!CompGeo::BEq(Y0, Y1))
                {
                    lIdx = Y0 > Y1? 0: 1;
                    sIdx = (lIdx + 1) % 2;
                }
            }
        }
        if (consolidate)
        {
            P = E[sIdx]->bottomPoint;
            e = splitEdge(E[lIdx], P);
            P->AddUpper(e);
            E[0]->model_edges.insert(E[0]->model_edges.end(), E[1]->model_edges.begin(), E[1]->model_edges.end());
            delete E[1];
            uit = A.uppers.erase(uit);
        }
        else
        {
            uit0 = uit;
            ++uit;
        }
        
        
    }
}

template<typename T> void FaceX::faceX<T>::consolidateLowers(void)
{ // consolidation puts all overlapping edges into one
    if (A.lowers.size() < 2) return;
    typename vector<SType<T>>::iterator lit = A.lowers.begin(), lit0 = lit;
    ++lit;
    while (lit != A.lowers.end())
    {
        pEdge<T> E0 = (*lit0).edge, E1 = (*lit).edge;
        bool both = E1->horizontal && E0->horizontal, 
            one = E1->horizontal || E0->horizontal;
        if ((both) || ((!one) && (fabs(E1->w - E0->w) < MAX_FLT_PRECISION)))
        {
            E0->model_edges.insert(E0->model_edges.end(), E1->model_edges.begin(), E1->model_edges.end());
            delete E1;
            lit = A.lowers.erase(lit);
        }
        else
        {
            lit0 = lit;
            ++lit;
        }
    }
}

template<typename T> void FaceX::faceX<T>::handleRightUpperCycles(const pEdge<T> & e, const bool isUpper)
{ // right cycles are left edges in pairs
  // uppers will have NULLs in face and pair in cycleedges
  // unless the cycle has been added by makeOverlaps
  // they will need to be added to their faces
  // a new face may need to be instantiated

    for (typename vector<pEdge3D<T>>::iterator eit = e->model_edges.begin(); eit != e->model_edges.end(); ++eit)
    {
        pEdge3D<T> E3 = *eit;
        pCycleEdge<T> cr = E3->rightCycle;
        CompGeo::ITYPE fIdx = cr->pHE->incidentface;
        if ((fIdx != 0) && ((cr->face == NULL) || !isUpper))
        {
            typename map<CompGeo::ITYPE, PairBuild<T>>::iterator pit = A.pass_on.find(fIdx);
            if (pit != A.pass_on.end()) // found a match, must be rightmost from lowers for edges in buffer
                                        // or leftmost from lowers for left neighbor
            {
                pPairBuild<T> pb = &(pit->second);
                pFace<T> F = pb->f;
                assert ((isUpper && (pb->flag == 'R')) || (!isUpper && (pb->flag == 'L')));
                pb->flag = 'l'; // if it was 'R' pb->e must be our right neighbor which will soon be handled in handleLeftUpperCycles 
                pb->e = cr;
                A.lPO[fIdx] = pit;
                if (isUpper) F->AddCycleEdge(cr);
                else
                { // this is the left neighbor
                    delete cr->pair;
                    cr->pair = NULL;
                }
                
                // buffer overlappers not here because their cr->face != NULL
                //else
                //{
                //    assert(pb->flag == 'D');
                //    --A.DCount;
                //    pb->flag = 'l';

                //}
                
            }
            else  // no entry in pass_on: make new face
            {
                CompGeo::pFaceType pf = dcel->getFace(fIdx);
                pFace<T> F = new Face<T>(pf);
                PairBuild<T> pb;
                pb.f = F; pb.e = cr; pb.flag = 'l';
                cr->face = F;
                A.pass_on[fIdx] = pb;
                A.lPO[fIdx] = A.pass_on.find(fIdx);
                if (isUpper) F->AddCycleEdge(cr);
            }
            
        }
    }

}
        
template<typename T> void FaceX::faceX<T>::handleLeftUpperCycles(const pEdge<T> & e, const bool isUpper)
{ // left cycles are right edges in pairs
  // uppers will have NULLs in face and pair in cycleedges
  // they will need to be added to their faces

    for (typename vector<pEdge3D<T>>::iterator eit = e->model_edges.begin(); eit != e->model_edges.end(); ++eit)
    {
        pEdge3D<T> E3 = *eit;
        pCycleEdge<T> cl = E3->leftCycle;
        CompGeo::ITYPE fIdx = cl->pHE->incidentface;
        if (fIdx != 0)
        {
            typename map<CompGeo::ITYPE, PairBuild<T>>::iterator pit = A.pass_on.find(fIdx);
            assert (pit != A.pass_on.end()); // must have a match
            pPairBuild<T> pb = &(pit->second);
            pFace<T> F = pb->f;
            pCycleEdge<T> left_edge = pb->e;

            assert (F->pface->viewface == fIdx);
            assert (((isUpper) && (pb->flag == 'l')) || ((!isUpper) &&(pb->flag == 'L')));
            pPair<T> npr = new Pair<T>(F, left_edge, cl);
            cl->pair = npr;
            cl->face = F;
            left_edge->pair = npr;
            pb->e = NULL;
            CompGeo::ITYPE eCount = 0;
            if (pb->flag == 'l') eCount = A.lPO.erase(fIdx); 
            assert (eCount == 1);
            pb->flag = 'D';
            ++A.DCount;
            if (isUpper) F->AddCycleEdge(cl);
            
        }
    }

}


template<typename T> void FaceX::faceX<T>::makeOverlaps(const pEdge<T> & e)
{
    for (typename map<CompGeo::ITYPE, typename map<CompGeo::ITYPE, PairBuild<T>>::iterator>::iterator lit = A.lPO.begin(); lit != A.lPO.end(); ++lit)
    {
        typename map<CompGeo::ITYPE, PairBuild<T>>::iterator pit = lit->second;
        pPairBuild<T> pb = &(pit->second);
        assert(pb->flag == 'l');
        pFace<T> F = pb->f;
        pPoint<T> tPt = e->topPoint, bPt = e->bottomPoint; // tPt should be eventPt
        pPtLnk<T> L = pb->pl, LB = bPt->GetLink();
        if (L == NULL)
        {
            CompGeo::TPoint<T> stkP = F->getFacePoint(pb->e->oPt, tPt->GetLink());
            T Z = stkP.xyz.Z;
            L = handlePoint(tPt, Z);
            pb->pl = L;
        }
        CompGeo::pTPoint<T> PT = L->model_point, PB = LB->model_point;
        pEdge3D<T> E3D = makeEdge(PT, PB);
        pCycleEdge<T> cr = E3D->rightCycle, cl = E3D->leftCycle;
        cr->flag = 'A'; cl->flag = 'A'; cr->face = F; cl->face = F;

        pPair<T> pr = new Pair<T>;
        pr->face = F;
        pr->leftEdge = pb->e;
        pr->rightEdge = cl;
        pb->e->pair = pr;
        cl->pair = pr;
        pb->e = cr;

        E3D->flag = 'B';

        e->AddModelEdge(E3D);
        F->AddCycleEdge(cr);
        F->AddCycleEdge(cl);
        ++(F->pairCount);
    }
}

template<typename T> void FaceX::faceX<T>::handleUppers(void)
{
    sort(A.uppers.begin(), A.uppers.end()); // left to right
    consolidateUppers();
    if (A.s_q.left_N != NULL) handleRightUpperCycles(A.s_q.left_N, false);
    
    for (typename vector<SType<T>>::iterator uit = A.uppers.begin(); uit != A.uppers.end(); ++uit)
    {
        SType<T> st = *uit;
        pEdge<T> e = st.edge;
        handleLeftUpperCycles(e);
        makeOverlaps(e);
        handleRightUpperCycles(e);
    }

    if (A.s_q.right_N != NULL) handleLeftUpperCycles(A.s_q.right_N, false);
}

template<typename T> void FaceX::faceX<T>::handleLowers(const bool & added_containers)
{
    sort(A.lowers.begin(), A.lowers.end()); // right to left above eventPt horizontals still last
    if (added_containers) consolidateLowers(); 
    //handleLowerHorizontals(); //in cube diagram @X1 horizontal s/b last
    for (typename vector<SType<T>>::iterator lit = A.lowers.begin(); lit != A.lowers.end(); ++lit)
    {
        SType<T> st = *lit;
        pEdge<T> E = st.edge;

        // first the rightCycles (corresponding to leftEdges of pairs):
        for (typename vector<pEdge3D<T>>::iterator e3it = E->model_edges.begin(); e3it != E->model_edges.end(); ++e3it)
        {
            pEdge3D<T> E3 = *e3it;
            pCycleEdge<T> cr = E3->rightCycle;
            pPair<T> pr = cr->pair;
            PairBuild<T> pb;
            if (pr != NULL)
            {
                pFace<T> F = cr->face;
                assert (pr->face == F);
                assert (pr->leftEdge == cr);
                CompGeo::ITYPE fIdx = F->pface->viewface;
                typename map<CompGeo::ITYPE, PairBuild<T>>::iterator pit = A.pass_on.find(fIdx);
                if (pit == A.pass_on.end()) // not in pass_on
                {
                    pb.e = pr->rightEdge; pb.f = F; pb.flag = 'R'; pb.pl = NULL;
                    A.pass_on[fIdx] = pb;
                }
                else // in pass_on
                {
                    pb = pit->second;
                    assert(pb.flag == 'R');
                    --(F->pairCount);
                    if ((pb.f == F) && (F->pairCount == 0))
                    {
                        F->FinalizeFace(dcel);
                        delete F;
                        A.pass_on.erase(pit);
                    }
                    
                }
                pr->rightEdge->pair = NULL;
                delete pr;
                cr->pair = NULL;    
            }
        }

        // then the leftCycles (corresponding to rightEdges of pairs):
        for (typename vector<pEdge3D<T>>::iterator e3it = E->model_edges.begin(); e3it != E->model_edges.end(); ++e3it)
        {
            pEdge3D<T> E3 = *e3it;
            pCycleEdge<T> cl = E3->leftCycle;
            pPair<T> pl = cl->pair;
            PairBuild<T> pb;
            if (pl != NULL)
            {
                pFace<T> F = cl->face;
                assert (pl->face == F);
                assert (pl->rightEdge == cl);
                CompGeo::ITYPE fIdx = F->pface->viewface;
                //assert (A.pass_on.count(fIdx) == 0);  // right to left
                pl->leftEdge->pair = NULL;
                typename map<CompGeo::ITYPE, PairBuild<T>>::iterator pit = A.pass_on.find(fIdx);
                if (pit == A.pass_on.end())
                {
                    pb.e = pl->leftEdge; pb.f = F; pb.flag = 'L'; pb.pl = NULL;
                    A.pass_on[fIdx] = pb;
                }
                else
                { // consolidate faces
                    pb = pit->second;
                    assert ((F != pb.f) && ('R' == pb.flag));
                    F = consolidateFaces(F, pb.f);
                    pPair<T> nPR = new Pair<T>(F, pl->leftEdge, pb.e);
                    pl->leftEdge->pair = nPR;
                    pb.e->pair = nPR;
                    A.pass_on.erase(pit);
                }

                delete pl;
                cl->pair = NULL;
            }
        }

    }

}

template<typename T> bool FaceX::faceX<T>::splitContainers(void)
{ // after split: top part -> lowers & bottom part -> uppers

    for (typename vector<SType<T>>::iterator cit = A.containers.begin(); cit != A.containers.end(); ++cit)
    {
        SType<T> st = *cit;
        pEdge<T> c = st.edge;
        A.lowers.push_back(st);
        st.edge = splitEdge(c);
        A.uppers.push_back(st);
    }

    return (A.containers.size() > 0);
}

template<typename T> void FaceX::faceX<T>::separateLowersAndContainers(void)
{ // using member intersectors of A.s_q
    pPoint<T> P = eventPt;
    //A.lowers.clear();
    //A.containers.clear();
    //A.pass_on.clear();
    //A.lPO.clear();
    for (typename vector<SType<T>>::iterator it = A.s_q.intersectors.begin(); it != A.s_q.intersectors.end(); ++it)
    {
        SType<T> st = *it;
        pEdge<T> X = st.edge;
        if (X->bottomPoint == P) // pointer cmp
        {
            A.lowers.push_back(st);
        }
        else
        {
            A.containers.push_back(st);
        }
    }
}

template <typename T> void FaceX::faceX<T>::getEdges(void)
{ // this is the status tree S query
  // the results will go into class member A

    ++sweep_iteration;
    E_CMP.edge_iteration = sweep_iteration;
    E_CMP.xOnSweep = eventPt->GetX();
    SType<T> st = SType<T>(&E_CMP);

    CompGeo::AVLNode<SType<T>> * p = S.Find(&st);
    while (p != NULL)
    {
        
        A.s_q.intersectors.push_back(*(p->Data));
        S.Delete();
        p = S.Find(&st);
    }
    sort(A.s_q.intersectors.begin(), A.s_q.intersectors.end());
    if (S.pathTop > 0)
    {
        char d = S.sPath[S.pathTop].direction;
        unsigned int i;
        for (i = S.pathTop - 1; i > 0; --i) if (S.sPath[i].direction != d) break;
        if (d == 'r')
        {
            A.s_q.left_N = S.sPath[S.pathTop].pNode->Data->edge;
            if (i > 0) A.s_q.right_N = S.sPath[i].pNode->Data->edge;
        }
        else // d == 'l'
        {
            A.s_q.right_N = S.sPath[S.pathTop].pNode->Data->edge;
            if (i > 0) A.s_q.left_N = S.sPath[i].pNode->Data->edge;
        }
    }
    
}

template<typename T> bool FaceX::faceX<T>::getPoint(void)
{
    bool emptiness = false;
    QType<T> qt = Q.GetLeast(emptiness);
    if (emptiness) eventPt = NULL;
    else 
    {
        eventPt = qt.pt; 
        if (eventPt->ups.size() > 0) A.uppers = eventPt->ups;
    }
    return emptiness;
}

template<typename T> void FaceX::faceX<T>::clearA(void)
{
    A.s_q.intersectors.clear();
    A.s_q.left_N = NULL;
    A.s_q.right_N = NULL;

    A.uppers.clear();
    A.lowers.clear();
    A.containers.clear();
    
    A.pass_on.clear();
    A.lPO.clear();
    A.DCount = 0;
}

template<typename T> void FaceX::faceX<T>::checkX(const pEdge<T> & l, const pEdge<T> & r)
{ // handle Point in Q if distinct edges l & r intersect

    pPoint<T> l_tPt = l->topPoint, l_bPt = l->bottomPoint,
        r_tPt = r->topPoint, r_bPt = r->bottomPoint;
    CompGeo::XY A = CompGeo::XY(static_cast<double>(l_tPt->GetX()), static_cast<double>(l_tPt->GetY())),
        B = CompGeo::XY(static_cast<double>(l_bPt->GetX()), static_cast<double>(l_bPt->GetY())),
        C = CompGeo::XY(static_cast<double>(r_tPt->GetX()), static_cast<double>(r_tPt->GetY())),
        D = CompGeo::XY(static_cast<double>(r_bPt->GetX()), static_cast<double>(r_bPt->GetY())),
        d_AB = B - A, d_CD = D - C, d_AC = C - A,
        matrix_row1 = CompGeo::XY(d_CD.y, -d_CD.x),
        matrix_row2 = CompGeo::XY(d_AB.y, -d_AB.x),
        iPt, iPt_r;

    double det = CompGeo::Cross(d_AB, d_CD);

    if (fabs(det) < MAX_FLT_PRECISION) return; // parallel lines - no intersection

    matrix_row1 /= det;
    matrix_row2 /= det;

    double t_l = matrix_row1 * d_AC, 
        t_r = matrix_row2 * d_AC; 

    bool in_range_l = ((t_l > 0.0) && (t_l < 1.0)),
        in_range_r = ((t_r > 0.0) && (t_r < 1.0));

    if (!((in_range_l) && (in_range_r))) return; // intersection not within segments

    iPt = A + t_l * d_AB;
    iPt_r = C + t_r * d_CD;
    
    assert (iPt == iPt_r);
    // we need a special point w/ 1 link
    CompGeo::TPoint<T> stkP = CompGeo::TPoint<T>(static_cast<T>(iPt.x), static_cast<T>(iPt.y), static_cast<T>(-1));

    pPtLnk<T> L = handlePoint(stkP);
    L->flag = 'z'; // L guaranteed not to have already been in Point

}

template<typename T> FaceX::pEdge<T> FaceX::faceX<T>::splitEdge(const pEdge<T> & oe, pPoint<T> Pt0)
{ // call this for each container intersector in the SQuery
  // the split will be at the event point eventPt (default)
  // in the case of a non-calculated overlap
  // the longer edge will need to be split at the shorter edge's bottomPoint
  // the new edge should be added to the uppers at that point

    bool SAE = Pt0 == NULL; // split at eventPt
    pPoint<T> tPt = oe->topPoint, bPt = oe->bottomPoint, Pt = SAE? eventPt: Pt0;
    double x_t = static_cast<double>(tPt->GetX()), x_b = static_cast<double>(bPt->GetX()),
        y_t = static_cast<double>(tPt->GetY()), y_b = static_cast<double>(bPt->GetY()),
        x_p = static_cast<double>(Pt->GetX()), y_p = static_cast<double>(Pt->GetY()),
        x_tb = x_b - x_t, y_tb = y_b - y_t, x_tp = x_p - x_t, y_tp = y_p - y_t,
        dr = fabs(x_tb) < MAX_FLT_PRECISION? y_tp / y_tb: fabs(y_tb) < MAX_FLT_PRECISION? x_tp / x_tb:
            fabs(x_tb) < fabs(y_tb)? y_tp / y_tb: x_tp / x_tb;

    CompGeo::pTPoint<T> P;
    pPtLnk<T> L = Pt->zb.at(0);
    bool zRepl = (L->flag == 'z');
    CompGeo::TPoint<T> stkP = *L->model_point;
    pEdge3D<T> E3D0, E3D1;
    pEdge<T> E = NULL;
    pCycleEdge<T> cr0, cr1, cl0, cl1;
    CompGeo::pHalfEdgeType hr0, hr1, hl0, hl1, hrn, hlp;
    CompGeo::ITYPE vtx = 0, vr0 = 0, vr1 = 0, vl0 = 0, vl1 = 0;

    assert(oe->model_edges.size() > 0);
    for (typename vector<pEdge3D<T>>::iterator eit = oe->model_edges.begin(); eit != oe->model_edges.end(); ++eit)
    {
        E3D0 = *eit;
        cr0 = E3D0->rightCycle; // top down
        cl0 = E3D0->leftCycle;  // bottom up
        hr0 = cr0->pHE; vr0 = hr0->viewedge; hl0 = cl0->pHE; vl0 = hl0->viewedge;
        CompGeo::pTPoint<T> PT = cr0->oPt, PB = cl0->oPt;
        double oZ = static_cast<double>(PT->xyz.Z), 
            nZ = static_cast<double>(PB->xyz.Z),
            deltaZ = nZ - oZ,
            Z = oZ + dr * deltaZ;
        if (zRepl)
        {
            P = L->model_point; vtx = P->viewvertex; P->xyz.Z = static_cast<T>(Z); 
            zRepl = false;
        }
        else
        {
            L = handlePoint(Pt, Z); P = L->model_point; vtx = P->viewvertex;
        }
        L->flag = 'i';

        hl0->origin = vtx;
        cr0->nPt = P;
        cl0->oPt = P;

        if (E3D0->flag == 'C')
        { // make 2 edges from 1
            E3D1 = makeEdge(P, PB);
            cr1 = E3D1->rightCycle; cl1 = E3D1->leftCycle;
            hr1 = cr1->pHE; hl1 = cl1->pHE;
            vr1 = hr1->viewedge; vl1 = hl1->viewedge;
            hr1->next = hr0->next; hr1->prev = vr0;
            hl1->next = vl0; hl1->prev = hl0->prev;
            hr0->next = vr1; 
            hl0->prev = vl1; 
            if (hr1->next != 0)
            {
                hrn = dcel->getHalfEdge(hr1->next);
                hrn->prev = vr1;
            }
            if (hl1->prev != 0)
            {
                hlp = dcel->getHalfEdge(hl1->prev);
                hlp->next = vl1;
            }
            if (NULL == E) E = new Edge<T>(E3D1);
            else E->AddModelEdge(E3D1);
        }
        else //E3D0->flag == 'B'
        { // bottom point has been recalculated - truncate edge there
          // this model edge will be an overlap part of a group of edges
            assert(SAE);
        }
    }
    oe->bottomPoint = Pt;
    return E;
}


template<typename T> void FaceX::faceX<T>::initializeQ(void)
{ // strategy: loop through all halfedges in dcel
  // initialize Edge if additionalHalfEdgeInfo is NULL
  // instantiate cycleedges for h[0] and its twin h[1]
  // instantiate also ptlnks for both origins as needed
  // instantiate Points and QTypes or re-use if possible
  // instantiate edge3D and Edge add to top Point.ups (call AddUpper)

    CompGeo::pHalfEdgeType h[2];
    CompGeo::pTPoint<T> p[2], P;
    pCycleEdge<T> c[2], C;
    pPtLnk<T> L;
    pPoint<T> pt[2], Pt0, Pt1, Pt2;
    pQType<T> qt1, qt2;
    pEdge3D<T> E3D;
    pEdge<T> E;

    bool qins = true;

    assert(Q.AVLEmpty());
    for (CompGeo::ITYPE i = 1; i < heIdx; ++i)
    { // 0 entry is to simulate NULL so skip
        h[0] = dcel->getHalfEdge(i);
        if (NULL == h[0]->additionalHalfEdgeInfo) 
        {
            // setting up h and p arrays:
            assert (0 < h[0]->twin); // twin must be present
            p[0] = dcel->origin(h[0]);
            h[1] = dcel->twin(h[0]);
            assert(NULL == h[1]->additionalHalfEdgeInfo); // twin must be set up w/ half
            p[1] = dcel->origin(h[1]);
            C = new CycleEdge<T>; C->pHE = h[0]; C->flag = 'S'; C->nPt = p[1]; 
                C->oPt = p[0]; h[0]->additionalHalfEdgeInfo = C; c[0] = C;
            C = new CycleEdge<T>; C->pHE = h[1]; C->flag = 'S'; C->nPt = p[0];
                C->oPt = p[1]; h[1]->additionalHalfEdgeInfo = C; c[1] = C;
            for (int j = 0; j < 2; ++j)
            { // handle points and Q:
                P = p[j];
                if (NULL == P->additionalTPointInfo)
                { // make ptlnk, make or re-use Point and QType, insert into Q
                    L = new PtLnk<T>; L->model_point = P; P->additionalTPointInfo = L;
                    if (qins)
                    {
                        Pt1 = new Point<T>; Pt2 = Pt1; 
                        qt1 = new QType<T>; qt2 = qt1;
                    }
                    Pt1->AddLink(L); pt[j] = Pt1; 
                    qt1->pt = Pt1;
                    qins = Q.Insert(qt1);
                    if (!qins) 
                    {
                        Pt1 = qt1->pt; Pt1->AddLink(L); pt[j] = Pt1;
                        qt1 = qt2;
                        Pt1 = Pt2;
                        Pt1->zb.clear();
                    }
                }
                else
                { // or get ptlnk from TPoint and Point from ptlnk
                    L = static_cast<pPtLnk<T>>(P->additionalTPointInfo);
                    pt[j] = L->prj_point;
                }
            }
            // getting right and left cycle indices:
            int rightIdx = *(pt[0]) < *(pt[1])? 0: 1,
                leftIdx = (rightIdx + 1) % 2;
            C = c[rightIdx]; C->r_l = 'R';
            C = c[leftIdx]; C->r_l = 'L';
            // handling edge3D and edge
            E3D = new Edge3D<T>(c[rightIdx], c[leftIdx]);
            E = new Edge<T>(E3D);
            Pt0 = pt[rightIdx];
            Pt0->AddUpper(E);

        }
    }
    if (!qins)
    { // a little cleanup:
        delete Pt1;
        delete qt1;
    }
}

template<typename T> void FaceX::faceX<T>::cleanup(void)
{ // strategy: loop through all halfedges in dcel
  // if cycleedge is present get associated Edge
  // for each Edge3D present delete both cycleEdges 
  // during this process delete all Points and PtLnks - set additionalTPointInfos to NULL
  // also set associated halfedge additionalHalfEdgeInfo to NULL
  // delete Edge3D and, when all Edge3Ds have been deleted, delete Edge

    CompGeo::pHalfEdgeType H;
    CompGeo::pTPoint<T> P;
    pCycleEdge<T> c[2], C;
    pPtLnk<T> L;
    pPoint<T> Pt;
    pEdge3D<T> E3D;
    pEdge<T> E;

    CompGeo::ITYPE heMax = dcel->getHalfEdgeCount();
    for (CompGeo::ITYPE i = 1; i < heMax; ++i)
    {
        H = dcel->getHalfEdge(i);
        if (H->additionalHalfEdgeInfo != NULL)
        {
            C = static_cast<pCycleEdge<T>>(H->additionalHalfEdgeInfo);
            E3D = C->e3;
            E = E3D->e;
            for (typename vector<pEdge3D<T>>::iterator eit = E->model_edges.begin(); eit != E->model_edges.end(); ++eit)
            {
                E3D = *eit;
                c[0] = E3D->rightCycle;
                c[1] = E3D->leftCycle;
                for (int j = 0; j < 2; ++j)
                {
                    C = c[j];
                    C->pHE->additionalHalfEdgeInfo = NULL;
                    assert (C->face == NULL);
                    P = C->oPt; // the other cycle's oPt is this cycle's nPt
                    if (P->additionalTPointInfo != NULL)
                    {
                        L = static_cast<pPtLnk<T>>(P->additionalTPointInfo);
                        Pt = L->prj_point;
                        for (typename vector<pPtLnk<T>>::iterator lit = Pt->zb.begin(); lit != Pt->zb.end(); ++lit)
                        {
                            L = *lit;
                            L->model_point->additionalTPointInfo = NULL;
                            delete L;
                        }
                        delete Pt;
                    }
                    delete C;
                }
                delete E3D;
            }
            delete E;
        }
    }
}

template<typename T> FaceX::pPtLnk<T> FaceX::faceX<T>::handlePoint(CompGeo::TPoint<T> & stkP)
{ // checks Q to retrieve possible pPoint, sets if not there

    PtLnk<T> stkL;
    Point<T> stkPt;
    QType<T> stkQt;

    CompGeo::pTPoint<T> P;
    pPtLnk<T> L = NULL;
    pPoint<T> Pt = NULL;
    pQType<T> Qt;

    bool qins = true;

    stkL.model_point = &stkP;
    stkPt.AddLink(&stkL);    
    stkQt.pt = &stkPt;
    Qt = &stkQt;
    qins = Q.Insert(Qt);
    if (!qins)
    {
        Pt = Qt->pt;
        L = Pt->CheckBuffer(stkP.xyz.Z);
        if (L != NULL) return L;
    }

    P = new CompGeo::TPoint<T>(stkP);
    CompGeo::ITYPE vtx = dcel->push(P);
    P->viewvertex = vtx;
    L = new PtLnk<T>;
    L->model_point = P;
    P->additionalTPointInfo = L;
    if (Pt == NULL) Pt = new Point<T>;
    Pt->AddLink(L);
    Qt = new QType<T>(Pt);
    CompGeo::AVLPath<QType<T>> * avlpth = &(Q.sPath[Q.pathTop]);
    avlpth->pNode->Data = Qt;

    return L;
}

template<typename T> FaceX::pPtLnk<T> FaceX::faceX<T>::handlePoint(const pPoint<T> & Pt, const T Z)
{
    pPtLnk<T> L = Pt->CheckBuffer(Z);
    if (L != NULL) return L;
    CompGeo::TPoint<T> stkP = CompGeo::TPoint<T>(Pt->GetX(), Pt->GetY(), Z);
    CompGeo::pTPoint<T> P = new CompGeo::TPoint<T>(stkP);
    CompGeo::ITYPE vtx = dcel->push(P);
    P->viewvertex = vtx;
    L = new PtLnk<T>;
    L->model_point = P;
    P->additionalTPointInfo = L;
    Pt->AddLink(L);

    return L;

}

template<typename T> FaceX::pEdge3D<T> FaceX::faceX<T>::makeEdge(const CompGeo::pTPoint<T> & PT, const CompGeo::pTPoint<T> & PB)
{
    pCycleEdge<T> cr = new CycleEdge<T>, cl = new CycleEdge<T>;
    cr->r_l = 'R'; cl->r_l = 'L';
    cr->oPt = PT; cr->nPt = PB;
    cl->oPt = PB; cl->nPt = PT;
    CompGeo::pHalfEdgeType hr = new CompGeo::HalfEdgeType, hl = new CompGeo::HalfEdgeType;
    CompGeo::ITYPE hrIdx = dcel->push(hr), hlIdx = dcel->push(hl);
    hr->viewedge = hrIdx; hl->viewedge = hlIdx;
    hr->twin = hlIdx; hl->twin = hrIdx;
    hr->origin = PT->viewvertex; hl->origin = PB->viewvertex;
    hr->additionalHalfEdgeInfo = cr;
    hl->additionalHalfEdgeInfo = cl;
    cr->pHE = hr; cr->flag = 'S';
    cl->pHE = hl; cl->flag = 'S';
    pEdge3D<T> E3D = new Edge3D<T>(cr, cl);

    return E3D;
}
// end of class faceX methods

//helper global function to let the linker know where these templated calls must go:
void link_my_faceX(void)
{

    CompGeo::DCEL<int> dBuffInt;
    CompGeo::DCEL<double> dBuffDbl;
    FaceX::faceX<int> fint1, fint2 = FaceX::faceX<int>(dBuffInt);
    FaceX::faceX<double> fdbl1, fdbl2 = FaceX::faceX<double>(dBuffDbl);
 
}