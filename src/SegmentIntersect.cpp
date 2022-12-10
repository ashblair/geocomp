#include "first.hpp"
#include "CompGeo.hpp"
#include "NGons.hpp"
#include "SegmentIntersect.hpp"

using namespace std;

// Static and Global Initializations:
pSegIntVertexElementType SegmentIntersect::SegIntVertices = NULL;
pSegIntEdgeType SegmentIntersect::SegIntEdges = NULL;
unsigned int SegmentIntersect::SegIntPolygonCount = 0, SegmentIntersect::SegIntEdgeCount = 0;
SegIntXY SegmentIntersect::refPt;
bool SegmentIntersect::fFindSegment = true;

SegIntXY::SegIntXY(void)
{
	x = 0.0;
	y = 0.0;
}

SegIntXY::SegIntXY(const CompGeo::XY & a)
{
	x = a.x;
	y = a.y;
}

bool operator<(SegIntXY & a, SegIntXY & b)
{
	if (fabs(b.y - a.y) < MAX_FLT_PRECISION) return (a.x < b.x);
	return (a.y > b.y);

}

string SegIntXY::printData(void)
{
		stringstream strmBuffx, strmBuffy;
		strmBuffx.fill('0');
		strmBuffy.fill('0');
		strmBuffx << setw(8) << setprecision(3) << fixed << right << x;
		strmBuffy << setw(8) << setprecision(3) << fixed << right << y;
		string r = "(" + strmBuffx.str() + "," + strmBuffy.str() + ")";
		return r;

}


SegIntPoint::SegIntPoint(void): pIdx(0), vIdx(0), pEdges(NULL) {}

SegIntPoint::SegIntPoint(const SegIntPoint & a)
{
	pIdx = a.pIdx;
	vIdx = a.vIdx;
	pEdges = NULL;
	pIndexListType p = NULL, pTrail = NULL, aTrack = a.pEdges;
	while (aTrack != NULL)
	{
		p = new IndexListType;
		p->index = aTrack->index;
		p->next = NULL;
		if (pTrail == NULL) pEdges = p;
		else pTrail->next = p;
		pTrail = p;
		aTrack = aTrack->next;
	}
}


SegIntPoint::~SegIntPoint(void)
{
	pIndexListType p = NULL;
	while (pEdges != NULL)
	{
		p = pEdges->next;
		delete pEdges;
		pEdges = p;
	}
}

SegIntPoint & SegIntPoint::operator= (const SegIntPoint & a)
{
	pIdx = a.pIdx;
	vIdx = a.vIdx;
	pIndexListType p = NULL, pTrail = NULL, aTrack = a.pEdges;
	while (pEdges != NULL)
	{
		p = pEdges->next;
		delete pEdges;
		pEdges = p;
	}
	while (aTrack != NULL)
	{
		p = new IndexListType; 
		p->index = aTrack->index;
		p->next = NULL;
		if (pTrail == NULL) pEdges = p;
		else pTrail->next = p;
		pTrail = p;
		aTrack = aTrack->next;
	}
	return *this;
}

SegIntXY GetPoint(SegIntPoint & a)
{
	return SegmentIntersect::SegIntVertices[a.pIdx].verts.at(a.vIdx);

}


bool operator<(SegIntPoint & a, SegIntPoint & b)
{
	SegIntXY p = GetPoint(a), q = GetPoint(b);
	if (fabs(q.y - p.y) < MAX_FLT_PRECISION) return (p.x < q.x);
	return (p.y > q.y);
}

bool operator>(SegIntPoint & a, SegIntPoint & b)
{
	SegIntXY p = GetPoint(a), q = GetPoint(b);
	if (fabs(q.y - p.y) < MAX_FLT_PRECISION) return (q.x < p.x);
	return (q.y > p.y);

}

bool operator==(SegIntPoint & a, SegIntPoint & b)
{
	SegIntXY p = GetPoint(a), q = GetPoint(b);
	return (p == q);

}


SegIntEdge::SegIntEdge(void)
{
	eIdx = 0;
}
		
SegIntEdge::SegIntEdge(const SegIntEdge & a)
{
	eIdx = a.eIdx;
}

SegIntEdge::SegIntEdge(unsigned int e)
{
	eIdx = e;
}

SegIntEdge::~SegIntEdge(void)
{
	eIdx = 0;
}

SegIntEdge &  SegIntEdge::operator = (const SegIntEdge & a)
{
	eIdx = a.eIdx;
	return *this;
}

bool segmentIsHorizontal(unsigned int eIdx)
{
	SegIntEdgeType e = SegmentIntersect::SegIntEdges[eIdx];
	SegIntXY lo(SegmentIntersect::SegIntVertices[e.pIdx].verts.at(e.lo)),
		hi(SegmentIntersect::SegIntVertices[e.pIdx].verts.at(e.hi));
	double deltaY = hi.y - lo.y;
	return (fabs(deltaY) < MAX_FLT_PRECISION);
}

double XofY(SegIntXY & refPt, unsigned int eIdx)
{
	SegIntEdgeType e = SegmentIntersect::SegIntEdges[eIdx];
	SegIntXY lo(SegmentIntersect::SegIntVertices[e.pIdx].verts.at(e.lo)),
		hi(SegmentIntersect::SegIntVertices[e.pIdx].verts.at(e.hi));
	double deltaY = hi.y - lo.y, deltaX = hi.x - lo.x, x;
	if (fabs(deltaY) < MAX_FLT_PRECISION)
	{
		if (refPt.x < hi.x) // segment drawing ==> <(hi)--------------(lo)>
		{
			x = hi.x;
		}
		else
		{
			if (refPt.x > lo.x)
			{
				x = lo.x;
			}
			else
			{
				x = refPt.x;
			}
		}
			
	}
	else
	{
		x = (deltaX / deltaY * (refPt.y - lo.y)) + lo.x; // function x(y): y is the scan line thru refPt
	}
	return x;
}


bool operator ==(SegIntEdge & dummy, SegIntEdge & b)
{// ignore dummy edge, check b vs refPt, return true iff refPt is in b

	if (!SegmentIntersect::fFindSegment) return false;
	SegIntXY refPt = SegmentIntersect::refPt;
	double x = XofY(refPt, b.eIdx), delta = fabs(x - refPt.x);
	return (delta < MAX_FLT_PRECISION);

}

bool operator <(SegIntEdge & a, SegIntEdge & b)
{// 2 orders with switch fFindSegment: if true return refPt.x < x_b (i.e. x(y) for segment b on scan line
 	SegIntXY refPt = SegmentIntersect::refPt;
	double x_b = XofY(refPt, b.eIdx);
	if (SegmentIntersect::fFindSegment) return (refPt.x < x_b);
	SegIntXY dropPt = refPt;
	dropPt.y -= 1000000.0;
	double x_a = XofY(refPt, a.eIdx);
	SegIntEdgeType e_a = SegmentIntersect::SegIntEdges[a.eIdx],
		e_b = SegmentIntersect::SegIntEdges[b.eIdx];
	SegIntXY lo_a(SegmentIntersect::SegIntVertices[e_a.pIdx].verts.at(e_a.lo)),
		lo_b(SegmentIntersect::SegIntVertices[e_b.pIdx].verts.at(e_b.lo));

	if (fabs(x_a - x_b) < MAX_FLT_PRECISION)
	{
		bool bh = segmentIsHorizontal(b.eIdx), ah = segmentIsHorizontal(a.eIdx);
		if (bh || ah)
		{
			if (bh)
			{
				if (ah) return (lo_a.x < lo_b.x);
				else return true;
			}
			else return false;
		}
		else
		{
			x_a = XofY(dropPt, a.eIdx), x_b = XofY(dropPt, b.eIdx);
			return (x_a < x_b);
		}
	}
	else return (x_a < x_b);
}



SegmentIntersect::SegmentIntersect(void)
{
	DeleteVertices();
	rptI = NULL;
}

SegmentIntersect::SegmentIntersect(NGons & ng)
{
	InitializeVertices(ng);
	rptI = NULL;
}

SegmentIntersect::~SegmentIntersect(void)
{
	DeleteVertices();
	while (rptI != NULL)
	{
		pSegIntPointListType p = rptI->next;
		delete rptI->pPoint;
		delete rptI;
		rptI = p;
	}
}


void SegmentIntersect::InitializeVertices(NGons & ng)
{
	DeleteVertices();
	if (ng.polygons == NULL) return;
	unsigned int pMax = ng.polygons->size();
	if (pMax == 0) return;
	vector<pPGonWork> * plist = ng.polygons;
	pPGonWork wGon = NULL;
	SegIntPolygonCount = pMax; //plist->numPolygons;
	SegIntVertices = new SegIntVertexElementType[SegIntPolygonCount];
	unsigned int vCount = 0, i;
	for (i = 0; i < SegIntPolygonCount; ++i)
	{
		wGon = plist->at(i);
		vCount += wGon->numVertices;
		SegIntVertices[i].initial_size = wGon->numVertices;
		pVertexNode v_n = wGon->vNode;
		while (v_n != NULL)
		{
			SegIntVertices[i].verts.push_back(v_n->vtxInfo.vertex);
			v_n = v_n->next;
		}
	}
	SegIntEdges = new SegIntEdgeType[vCount];
	SegIntEdgeCount = vCount;
	unsigned int accumulator = 0, j;
	for (i = 0; i < SegIntPolygonCount; ++i)
	{
		unsigned int nV = SegIntVertices[i].verts.size();
		for (j = 0; j < nV; ++j)
		{
			unsigned int loIdx = j, hiIdx = (j + 1) % nV, tmp;
			SegIntXY v1(SegIntVertices[i].verts.at(loIdx)),
				v2(SegIntVertices[i].verts.at(hiIdx));
			//cout << to_string(i) << "-" << to_string(j) << " " << v1.printData() << " - " << v2.printData() << "\n"; 
			if (v1 < v2)
			{
				tmp = loIdx;
				loIdx = hiIdx;
				hiIdx = tmp;
			}
			SegIntEdges[accumulator + j].pIdx = i;
			SegIntEdges[accumulator + j].hi = hiIdx;
			SegIntEdges[accumulator + j].lo = loIdx;
		}
		accumulator += nV;
	}

}

void SegmentIntersect::DeleteVertices(void)
{
	if (SegIntVertices == NULL) return;
	delete [] SegIntVertices;
	SegIntVertices = NULL;
	SegIntPolygonCount = 0;

	delete [] SegIntEdges;
	SegIntEdges = NULL;
	SegIntEdgeCount = 0;

}

bool SegmentIntersect::getIntersection(SegIntXY & iPt, unsigned int * e_l, unsigned int * e_r)
{
	if ((e_l == NULL) || (e_r == NULL)) return false;
	SegIntEdgeType el = SegmentIntersect::SegIntEdges[*e_l], er = SegmentIntersect::SegIntEdges[*e_r];
	SegIntXY lo_l(SegmentIntersect::SegIntVertices[el.pIdx].verts.at(el.lo)),
		hi_l(SegmentIntersect::SegIntVertices[el.pIdx].verts.at(el.hi)),
		lo_r(SegmentIntersect::SegIntVertices[er.pIdx].verts.at(er.lo)),
		hi_r(SegmentIntersect::SegIntVertices[er.pIdx].verts.at(er.hi));
	bool horizontal_l = fabs(hi_l.y - lo_l.y) < MAX_FLT_PRECISION,
		horizontal_r = fabs(hi_r.y - lo_r.y) < MAX_FLT_PRECISION;
	if (!(horizontal_l || horizontal_r))
	{// adjustment to scan line except when there's a horizontal (which must be on the scan line anyway)
		hi_l.x = XofY(refPt, *e_l);
		hi_l.y = refPt.y;
		hi_r.x = XofY(refPt, *e_r);
		hi_r.y = refPt.y;
	}
	double dXl = hi_l.x - lo_l.x, dYl = hi_l.y - lo_l.y, dXr = hi_r.x - lo_r.x, dYr = hi_r.y - lo_r.y;
	SegIntXY Ll(CompGeo::XY(dXl, dYl)), Lr(CompGeo::XY(dXr, dYr));
	double cProd = CompGeo::Cross(Lr, Ll);
	if (fabs(cProd) < MAX_FLT_PRECISION) return false;  // parallel & not colinear
	double dX = lo_r.x - lo_l.x, dY = lo_r.y - lo_l.y;
	double t_r = (dY * dXl - dX * dYl) / cProd,
		t_l = (dY * dXr - dX * dYr) / cProd;
	bool inRange_l = (t_l > 0.0) && (t_l < 1.0), inRange_r = (t_r > 0.0) && (t_r < 1.0);
	if (inRange_l && inRange_r) // strictly interior points, no ends
	{
		SegIntXY iPt1, iPt2;  // output average for more accuracy
		iPt1.x = lo_l.x + (t_l * dXl);
		iPt1.y = lo_l.y + (t_l * dYl);
		iPt2.x = lo_r.x + (t_r * dXr);
		iPt2.y = lo_r.y + (t_r * dYr);
		iPt.x = (iPt1.x + iPt2.x) / 2.0;
		iPt.y = (iPt1.y + iPt2.y) / 2.0;
		return true;
	}
	return false;
}

bool SegmentIntersect::isReportable(SegIntPoint & a)
{// return false if a has 1 edge or a has 2 edges both from the same polygon with one shared point
	unsigned int count = 0;
	pIndexListType p = a.pEdges;
	while (p != NULL)
	{
		++count;
		p = p->next;
	}
	if (count > 2) return true;
	if (count < 2) return false;

	SegIntEdgeType e1, e2;
	p = a.pEdges;
	e1 = SegIntEdges[p->index];
	p = p->next;
	e2 = SegIntEdges[p->index];
	if (e1.pIdx != e2.pIdx) return true;
	unsigned int pI = e1.pIdx;
	SegIntXY lo1(SegIntVertices[pI].verts.at(e1.lo)),
		hi1(SegIntVertices[pI].verts.at(e1.hi)),
		lo2(SegIntVertices[pI].verts.at(e2.lo)),
		hi2(SegIntVertices[pI].verts.at(e2.hi)),
		A, B, C;
	if (lo1 == lo2)
	{
		C = lo1;
		A = hi1;
		B = hi2;
	}
	else
	{
		if (lo1 == hi2)
		{
			C = lo1;
			A = hi1;
			B = lo2;
		}
		else
		{
			if (hi1 == lo2)
			{
				C = hi1;
				A = lo1;
				B = hi2;
			}
			else
			{
				if (hi1 == hi2)
				{
					C = hi1;
					A = lo1;
					B = lo2;
				}
				else return true;

			}
		}
	}
	if (A == B) return true; // same segment
	double cProd = CompGeo::Cross(C - A, C - B);
	if (fabs(cProd) >= MAX_FLT_PRECISION) return false; // not colinear
	double dXba = B.x - A.x, dYba = B.y - A.y, t = C.y - A.y;
	if (fabs(dXba) < MAX_FLT_PRECISION) t /= dYba;
	else t = (C.x - A.x) / dXba;
	return ((t < 0.0f) || (t > 1.0f));  // true for overlapping segments

}

unsigned int SegmentIntersect::AddPointToEnd(unsigned int segIdx, SegIntXY & a)
{
	SegIntVertices[segIdx].verts.push_back(a);
	return SegIntVertices[segIdx].verts.size() - 1;
}


void SegmentIntersect::DeleteLastPoint(unsigned int segIdx)
{
	SegIntVertices[segIdx].verts.pop_back();
}

char SegmentIntersect::ClassifySegment(unsigned int e)
{// returns u for Upper, l for Lower and c for Contains
	SegIntEdgeType et = SegIntEdges[e];
	SegIntXY lo(SegIntVertices[et.pIdx].verts.at(et.lo)),
		hi(SegIntVertices[et.pIdx].verts.at(et.hi));
	if (refPt == lo) return 'l';
	if (refPt == hi) return 'u';
	return 'c';
}

unsigned int SegmentIntersect::getIndexCount(pIndexListType pI)
{
	unsigned int count = 0;
	pIndexListType p = pI;
	while (p != NULL)
	{
		++count;
		p = p->next;
	}
	return count;
}

pIndexListType SegmentIntersect::combineIndexLists(pIndexListType a, pIndexListType b)
{
	pIndexListType root = NULL, np = NULL, p = a, pTrail = NULL;

	while (p != NULL)
	{
		addIndex(root, p->index);
		p = p->next;
	}
	p = b;
	while (p != NULL)
	{
		addIndex(root, p->index);
		p = p->next;
	}
	return root;
}


void SegmentIntersect::addIndex(pIndexListType & root, unsigned int eI)
{
	pIndexListType np = new IndexListType;
	np->index = eI;
	np->next = NULL;
	if (root == NULL) 
	{
		root = np;
		return;
	}
	pIndexListType pTrail = NULL, p = root;
	while (p != NULL)
	{
		if (eI < p->index)
		{
			if (pTrail == NULL)
			{
				np->next = root;
				root = np;
			}
			else
			{
				np->next = p;
				pTrail->next = np;
			}
			return;
		}
		pTrail = p;
		p = p->next;
	}
	pTrail->next = np;
}

void SegmentIntersect::deleteIndexList(pIndexListType * ppI)
{
	pIndexListType p = NULL, pI = *ppI;
	while (pI != NULL)
	{
		p = pI->next;
		delete pI;
		pI = p;
	}
}

void SegmentIntersect::addtoReport(SegIntPoint *& rI)
{
	pSegIntPointListType np = new SegIntPointListType;
	np->pPoint = rI;
	np->next = rptI;
	rptI = np;

}


pIndexListType SegmentIntersect::getContainerEdges(CompGeo::AVL<SegIntEdge> & t, unsigned int *& left_neighbor, 
	unsigned int *& right_neighbor)
{
	SegIntEdge siE;
	pIndexListType edges = NULL, eTrail = NULL, ePtr = NULL;
	left_neighbor = NULL;
	right_neighbor = NULL;
	CompGeo::AVLNode<SegIntEdge> * p = NULL;
	bool foundSeg = false;
	// loop until there are no more segments containing refPt
	do
	{
		foundSeg = false;
		p = t.Find(&siE);
		if (p != NULL)
		{
				foundSeg = true;
				ePtr = new IndexListType;
				ePtr->next = NULL;
				ePtr->index = p->Data->eIdx;
				if (eTrail == NULL) edges = ePtr;
				else eTrail->next = ePtr;
				eTrail = ePtr;
				t.Delete();
			}
	} while (foundSeg);
	// setting the neighbors:
	if (t.pathTop > 0) // check for empty tree
	{
		char d = t.sPath[t.pathTop].direction;
		unsigned int i;
		for (i = t.pathTop - 1; i > 0; --i) if (t.sPath[i].direction != d) break;
		if (d == 'r')
		{
			left_neighbor = &t.sPath[t.pathTop].pNode->Data->eIdx;
			if (i > 0) right_neighbor = &t.sPath[i].pNode->Data->eIdx;
		}
		else // d is 'l'
		{
			right_neighbor = &t.sPath[t.pathTop].pNode->Data->eIdx;
			if (i > 0) left_neighbor = &t.sPath[i].pNode->Data->eIdx;
		}
	}
	return edges;

}


void SegmentIntersect::getAllIntersects(void)
{/* main procedure for algorithm
 1) set up Event Queue Q as an AVL BST templated w/ class SegIntPoint 
	a) each point is a vertex of a polygon edge in arrays SegIntVertices & SegIntEdges
	b) when a point is already in the Q add the edge to its pEdges list if it is upper
 2) while Q is not empty get and delete the next point p from it
 3) retrieve and delete all segments in Status Tree T that contain p (getting left & right neighbors as well)
 4) these segments will be lowers and containers, the uppers are with p
 5) combine all segments and construct a SegIntPoint, if it's reportable add it to the report
 6) of the containers and uppers determine left-most and right-most as they'll be in T
 7) get possible intersections between these segments and the appropriate neighbor
 8) add intersection points to SegIntVertices and Q - if in Q delete point from SegIntVertices
 9) insert uppers & containers into T
*/  

	while (rptI != NULL) // clearing rptI
	{
		pSegIntPointListType pSIPLT = rptI->next;
		deleteIndexList(&rptI->pPoint->pEdges);
		delete rptI->pPoint;
		delete rptI;
		rptI = pSIPLT;
	}
	CompGeo::AVL<SegIntPoint> E_Q; // the event queue Q
	CompGeo::AVL<SegIntEdge> S_T; // the status tree
	bool fInserted = true;
	SegIntPoint * pIP = NULL, * pIPIns, segIP;

	for (unsigned int i = 0; i < SegIntEdgeCount; ++i) // inserting segment end points into Q
	{
		if (fInserted) pIP = new SegIntPoint;
		else deleteIndexList(&pIP->pEdges);
		pIP->pIdx = SegIntEdges[i].pIdx;
		pIP->vIdx = SegIntEdges[i].hi;
		pIP->pEdges = NULL;
		addIndex(pIP->pEdges, i);
		pIPIns = pIP;
		fInserted = E_Q.Insert(pIPIns);
		if (!fInserted) addIndex(pIPIns->pEdges, i);

		if (fInserted) pIP = new SegIntPoint;
		else deleteIndexList(&pIP->pEdges);
		pIP->pIdx = SegIntEdges[i].pIdx;
		pIP->vIdx = SegIntEdges[i].lo;
		pIP->pEdges = NULL;
		pIPIns = pIP;
		fInserted = E_Q.Insert(pIPIns);
	}
	if (!fInserted) 
	{
		deleteIndexList(&pIP->pEdges);
		delete pIP;
	}
	bool fIsEmpty = true;
	segIP = E_Q.GetLeast(fIsEmpty);
	while (!fIsEmpty)  // the main loop continue until event queue is empty
	{
		refPt = GetPoint(segIP);
		SegIntXY dropPt = refPt, iPt;
		dropPt.y -= 1000000.0; // for order calculation as in insert into T
		unsigned int * lftN = NULL, * rghN = NULL;  // neighbors
		fFindSegment = true;
		pIndexListType pIdxLst = getContainerEdges(S_T, lftN, rghN), oldpEdges = segIP.pEdges,
			pAll = combineIndexLists(segIP.pEdges, pIdxLst), pIL = NULL;
		segIP.pEdges = pAll;
		if (isReportable(segIP)) // handling report
		{
			pIP = new SegIntPoint(segIP);
			addtoReport(pIP);
		}
		bool fInitialized = false, fHorizontals = false;
		double xMin = 0.0f, xMax = 0.0f, hMin = 0.0f, hMax = 0.0f;
		unsigned int eMin = 0, eMax = 0, ehMax = 0, ehMin = 0;
		pIL = pAll;
		while (pIL != NULL)  // getting left-most and right-most segments destined for T
		{
			if (ClassifySegment(pIL->index) != 'l')
			{
				SegIntEdgeType et = SegIntEdges[pIL->index];
				SegIntXY lo(SegIntVertices[et.pIdx].verts.at(et.lo)),
					hi(SegIntVertices[et.pIdx].verts.at(et.hi));
				if (fabs(hi.y - lo.y) < MAX_FLT_PRECISION)
				{// found a horizontal:
					if (fHorizontals)
					{
						if (lo.x < hMin) 
						{
							hMin = lo.x;
							ehMin = pIL->index;
						}
						if (lo.x > hMax) 
						{
							hMax = lo.x;
							ehMax = pIL->index;
						}
					}
					else
					{
						hMin = lo.x;
						hMax = lo.x;
						ehMin = pIL->index;
						ehMax = ehMin;
						fHorizontals = true;
					}
				}
				else
				{
					double x = XofY(dropPt, pIL->index);
					if (fInitialized)
					{
						if (x < xMin)
						{
							xMin = x;
							eMin = pIL->index;
						}
						else
						{
							if (x > xMax)
							{
								xMax = x;
								eMax = pIL->index;
							}
						}
					}
					else
					{
						xMin = x;
						xMax = x;
						eMin = pIL->index;
						eMax = eMin;
						fInitialized = true;
					}
				}
			}
			pIL = pIL->next;
		}
		if (fHorizontals)
		{
			eMax = ehMax;
			if (!fInitialized)
			{
				fInitialized = true;
				eMin = ehMin;
			}
		}
		if (fInitialized)
		{
			if (getIntersection(iPt, lftN, &eMin)) // intersection on left?
			{
				pIP = new SegIntPoint;
				pIP->pIdx = segIP.pIdx;
				pIP->vIdx = AddPointToEnd(segIP.pIdx, iPt);
				pIP->pEdges = NULL;
				pIPIns = pIP;
				fInserted = E_Q.Insert(pIPIns);
				if (!fInserted) 
				{
					DeleteLastPoint(segIP.pIdx);
					delete pIP;
				}
			}
			if (getIntersection(iPt, &eMax, rghN)) // intersection on right?
			{
				pIP = new SegIntPoint;
				pIP->pIdx = segIP.pIdx;
				pIP->vIdx = AddPointToEnd(segIP.pIdx, iPt);
				pIP->pEdges = NULL;
				pIPIns = pIP;
				fInserted = E_Q.Insert(pIPIns);
				if (!fInserted) 
				{
					DeleteLastPoint(segIP.pIdx);
					delete pIP;
				}
			}
			// adding containers and uppers to T:
			pIL = pAll;
			fFindSegment = false;
			while (pIL != NULL)
			{
				if (ClassifySegment(pIL->index) != 'l') 
				{
					SegIntEdge * siE = new SegIntEdge(pIL->index); 
					S_T.AlwaysInsert(siE);
				}
				pIL = pIL->next;
			}
		}
		else
		{
			if (getIntersection(iPt, lftN, rghN)) // nothing going back in to T between neighbors
			{
				pIP = new SegIntPoint;
				pIP->pIdx = segIP.pIdx;
				pIP->vIdx = AddPointToEnd(segIP.pIdx, iPt);
				pIP->pEdges = NULL;
				pIPIns = pIP;
				fInserted = E_Q.Insert(pIPIns);
				if (!fInserted) 
				{
					DeleteLastPoint(segIP.pIdx);
					delete pIP;
				}
			}
		}
		deleteIndexList(&pIdxLst);
		deleteIndexList(&oldpEdges);
		deleteIndexList(&pAll);
		segIP.pEdges = NULL;
		segIP = E_Q.GetLeast(fIsEmpty);
	} // end of while Q not empty loop
}


pPGonWork SegmentIntersect::makePolygon(unsigned int & vC)
{
	unsigned int count = 0;
	pSegIntPointListType pIPL = rptI;
	while (pIPL != NULL)
	{
		++count;
		pIPL = pIPL->next;
	}
	if (count == 0) return NULL;
	pPGonWork wGon = new PGonWork;
	wGon->numVertices = count;
	pNameNode pNameOut = new NameNode;
	string nmn = "seg_Xsect";
	pNameOut->polyname = nmn;
	wGon->pName = pNameOut;
	pIPL = rptI;
	SegIntXY xy;
	pVertexNode pvn = NULL, pvTrail = NULL;
	vC = count;
	while (pIPL != NULL)
	{
		xy = GetPoint(*pIPL->pPoint);
		pIPL = pIPL->next;
		pvn = new VertexNode;
		pvn->vtxInfo.vertex.x = xy.x;
		pvn->vtxInfo.vertex.y = xy.y;
		pvn->next = NULL;
		if (pvTrail == NULL) wGon->vNode = pvn;
		else pvTrail->next = pvn;
		pvTrail = pvn;
	}
	if (count < 3)
	{// adding vertices to make a triangle:
		CompGeo::XY xy1, xy2;
		if (count == 1)
		{
			double radius = 100.0, pi = 4.0f * atan(1.0), theta1 = pi * (37.0 / 30.0), 
				theta2 = pi * (19.0 / 15.0);
			xy1.x = radius * cos(theta1) + xy.x;
			xy1.y = radius * sin(theta1) + xy.y;
			xy2.x = radius * cos(theta2) + xy.x;
			xy2.y = radius * sin(theta2) + xy.y;
			pvn = new VertexNode;
			pvn->vtxInfo.vertex.x = xy1.x;
			pvn->vtxInfo.vertex.y = xy1.y;
			pvn->next = NULL;
			pvTrail->next = pvn;
			pvTrail = pvn;
		}
		if (count == 2)
		{
			SegIntXY sxy = GetPoint(*rptI->pPoint);
			CompGeo::XY dxy(xy.x - sxy.x, xy.y - sxy.y), ctr(sxy.x, sxy.y);
			double d = sqrt(pow(dxy.x, 2) + pow(dxy.y, 2)), radius = sqrt(3.0) * d / 2.0;
			ctr += dxy / 2.0;
			dxy /= d; // if we have the same 2 points this will crash and burn
			CompGeo::XY norm(-dxy.y, dxy.x);
			norm *= radius;
			xy2 = ctr + norm;
		}
		pvn = new VertexNode;
		pvn->vtxInfo.vertex.x = xy2.x;
		pvn->vtxInfo.vertex.y = xy2.y;
		pvn->next = NULL;
		pvTrail->next = pvn;
		wGon->numVertices = 3;
	}
	return wGon;
}

vector<pPGonWork> SegmentIntersect::makePolygons(unsigned int & vC)
{
	vector<pPGonWork> r;
	pPGonWork wGon = NULL; 
	pVertexNode pvn = NULL, pvTrail = NULL, ** lkup = new pVertexNode*[SegIntPolygonCount];

	// re-creating the original polygons:
	for (unsigned int i = 0; i < SegIntPolygonCount; ++i)
	{
		wGon = new PGonWork;
		pNameNode pNameOut = new NameNode; 
		string nmn = "Intersects";
		pNameOut->polyname = nmn;
		wGon->pName = pNameOut;
		unsigned int numPts = SegIntVertices[i].initial_size;
		vector<CompGeo::XY> vSI = SegIntVertices[i].verts;
		wGon->numVertices = numPts;
		pvTrail = NULL;
		lkup[i] = new pVertexNode[numPts];
		for(unsigned int j = 0; j < numPts; ++j)
		{
			pvn = new VertexNode;
			lkup[i][j] = pvn;
			pvn->vtxInfo.vertex = vSI.at(j);
			if (pvTrail == NULL) wGon->vNode = pvn;
			else pvTrail->next = pvn;
			pvTrail = pvn;
		}
		r.push_back(wGon);

	}

	// adding the intersection points:
	unsigned int count = 0;
	pSegIntPointListType pIPL = rptI;
	while (pIPL != NULL)
	{
		++count;
		SegIntPoint sPt = *(pIPL->pPoint);
		SegIntXY sxy = GetPoint(sPt);
		CompGeo::XY xy = CompGeo::XY(sxy.x, sxy.y);
		pIndexListType edges = sPt.pEdges;
		while (edges != NULL)
		{
			SegIntEdgeType sET = SegIntEdges[edges->index];
			wGon = r.at(sET.pIdx); 
			unsigned int iSZ = SegIntVertices[sET.pIdx].initial_size;
			CompGeo::XY sxyLO = SegIntVertices[sET.pIdx].verts.at(sET.lo),
				sxyHI = SegIntVertices[sET.pIdx].verts.at(sET.hi);
			double xLO = sxyLO.x, yLO = sxyLO.y, xHI = sxyHI.x, yHI = sxyHI.y,
				deltaX = xHI - xLO, deltaY = yHI - yLO, 
				x = xy.x, y = xy.y, t = 0.0;
			bool useY = fabs(deltaY) > fabs(deltaX),
				Bounded = ((sET.lo == (sET.hi + 1)) || (sET.hi == (sET.lo + 1))),
				inOrder = ((!Bounded && (sET.lo > sET.hi)) || (Bounded && (sET.lo < sET.hi)));
			if (!Bounded) assert (((sET.lo == 0) && (sET.hi == (iSZ - 1))) || 
								((sET.hi == 0) && (sET.lo == (iSZ - 1))));
			t = inOrder? useY? (y - yLO) / deltaY: (x - xLO) / deltaX:
				useY? (yHI - y) / deltaY: (xHI - x) / deltaX;
			bool isZero = fabs(t) < MAX_FLT_PRECISION,
				isOne = fabs(t - 1.0) < MAX_FLT_PRECISION,
				isInRange = ((t >= 0.0) && (t <= 1.0));
			assert (isInRange);
			if ((!isZero) && (!isOne))
			{ // getting intersections in the right order between vertexnodes
				pVertexNode pvLO = lkup[sET.pIdx][sET.lo],
					pvHI = lkup[sET.pIdx][sET.hi],
					pv = new VertexNode,
					pvNode = wGon->vNode;
				pv->vtxInfo.vertex = xy;
				pv->vtxInfo.is_selected = true;
				double current_t = 0.0;
				bool found_it = false;
				if (inOrder)
				{ // pvLO is first & corresponds w/ t=0 (t=1 => pvHI)
					pvTrail = pvLO;
					pvn = pvLO;
					while (pvn != pvHI)
					{
						pvn = pvn->next;
						if ((pvn == NULL) || (pvn == pvHI)) current_t = 1.0;
						else current_t = useY? (pvn->vtxInfo.vertex.y - yLO) / deltaY:
							(pvn->vtxInfo.vertex.x - xLO) / deltaX;
						if (t < current_t)
						{
							found_it = true;
							break;
						}
						pvTrail = pvn;
					}
				}
				else
				{ // pvHI is first & corresponds w/ t=0 (t=1 => pvLO)
					pvTrail = pvHI;
					pvn = pvHI;
					while (pvn != pvLO)
					{
						pvn = pvn->next;
						if ((pvn == NULL) || (pvn == pvLO)) current_t = 1.0;
						else current_t = useY? (yHI - pvn->vtxInfo.vertex.y) / deltaY:
							(xHI - pvn->vtxInfo.vertex.x) / deltaX;
						if (t < current_t)
						{
							found_it = true;
							break;
						}
						pvTrail = pvn;
					}
				}
				assert (found_it);
				pv->next = pvn;
				pvTrail->next = pv;
				++wGon->numVertices;
				
			}
			edges = edges->next;
		}
		pIPL = pIPL->next;
	}
	vC = count;
	
	for (unsigned int i = 0; i < SegIntPolygonCount; ++i)
	{
		delete[] lkup[i];
	}
	
	delete[] lkup;
	lkup = NULL;
	return r;
}
