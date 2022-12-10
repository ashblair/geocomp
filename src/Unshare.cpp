#include "first.hpp"
#include "CompGeo.hpp"
#include "NGons.hpp"
#include "Unshare.hpp"

using namespace std;

UnsXY::UnsXY(void)
{
	x = 0.0;
	y = 0.0;

}

UnsXY::UnsXY(const CompGeo::XY & a)
{
	x = a.x;
	y = a.y;
}

bool operator < (const UnsXY & a, const UnsXY & b)
{
	if (fabs(a.y - b.y) < MAX_FLT_PRECISION) return (a.x < b.x);
	return (a.y > b.y);
}

UnsSegStruct::UnsSegStruct(void)
{
	lo.x = 0.0;
	lo.y = 0.0;
	hi.x = 0.0;
	hi.y = 0.0;
}

UnsSegStruct::UnsSegStruct(const UnsSegStruct & a)
{
	lo = a.lo;
	hi = a.hi;
}

UnsSegStruct::UnsSegStruct(const UnsXY & a, const UnsXY & b)
{
	if (a < b)
	{
		hi = a;
		lo = b;
	}
	else
	{
		hi = b;
		lo = a;
	}
}

UnsSegStruct & UnsSegStruct::operator=(const UnsSegStruct & a)
{
	lo = a.lo;
	hi = a.hi;
	return *this;
}

UnsSegStruct UnsSegStruct::BuildFromPoints(const UnsXY & a, const UnsXY & b)
{

	if (a < b)
	{
		hi = a;
		lo = b;
	}
	else
	{
		hi = b;
		lo = a;
	}
	return *this;

}

bool operator < (UnsSegType & a, UnsSegType & b)
{
	if (a.hi == b.hi) return (a.lo < b.lo);
	return (a.hi < b.hi);
}

bool operator == (const UnsSegType & a, const UnsSegType & b)
{
	return ((a.hi == b.hi) && (a.lo == b.lo));
}

UnsPointStruct::UnsPointStruct(void)
{
	point.x = 0.0;
	point.y = 0.0;
	c = point;
	cc = c;
}

UnsPointStruct::UnsPointStruct(const UnsXY & a)
{
	point = a;
	c.x = 0.0;
	c.y = 0.0;
	cc = c;
}

UnsPointStruct::UnsPointStruct(const UnsPointStruct & a)
{
	point = a.point;
	c = a.c;
	cc = a.cc;
}

UnsPointStruct & UnsPointStruct::operator=(const UnsPointStruct & a)
{
	point = a.point;
	c = a.c;
	cc = a.cc;
	return *this;
}

UnsPointStruct::~UnsPointStruct(void)
{
	point.x = 0.0;
	point.y = 0.0;
	c = point;
	cc = c;
}

bool operator < (const UnsPointType & a, const UnsPointType & b)
{
	return (a.point < b.point);
}

bool operator == (const UnsPointType & a, const UnsPointType & b)
{
	return (a.point == b.point);
}

Unshare::Unshare(void)
{
	pNode = NULL;
}

void Unshare::RemoveSharedEdges(vector<pPGonWork> *& wGon0)
{

	bool segIn = true;
	pUnsSegType ps = NULL, psIn = NULL, pdseg = NULL, pdIn = NULL;
	pPGonWork wGon = NULL;
	//while (wGon != NULL)
	for (vector<pPGonWork>::const_iterator it = wGon0->begin(); it != wGon0->end(); ++it)
	{
		wGon = *it;
		pVertexNode pvTop = wGon->vNode, pv = pvTop->next, pvTrail = pvTop;
		unsigned int V = wGon->numVertices; // s/b 3 for all the triangles
		for (unsigned int i = 0; i < V; ++i)
		{
			if (pv == NULL) pv = pvTop;
			if (!segIn)
			{
				ps->BuildFromPoints(pvTrail->vtxInfo.vertex, 
					pv->vtxInfo.vertex);
			}
			else
			{
				ps = new UnsSegType(pvTrail->vtxInfo.vertex, 
					pv->vtxInfo.vertex);
			}
			psIn = ps;
			segIn = SegTree.Insert(psIn); // true if inserted, false if found (& not inserted)
			if (!segIn)
			{
				pdseg = new UnsSegType(*psIn);
				pdIn = pdseg;
				if (!DelTree.Insert(pdIn)) 
				{
					delete pdseg;
					pdseg = NULL;
				}
				SegTree.Delete(); // pair found -- delete
			}
			pvTrail = pv;
			pv = pv->next;
		}
		//wGon = wGon->next;
	}
	if (!segIn) delete ps;
}

void Unshare::InitializePointTree(void)
{
	bool sTreeEmpty = false, pIsNew = true;
	UnsSegType seg = SegTree.GetLeast(sTreeEmpty), segIn;
	pUnsPointType p = NULL, pIns = NULL;
	UnsPointType buff;

	while (!sTreeEmpty)
	{
		if (pIsNew) p = new UnsPointType;
		p->point = seg.hi;
		p->cc = seg.lo;
		pIns = p;
		pIsNew = PointTree.Insert(pIns); // true if inserted, false otherwise (i.e. found)
		if (!pIsNew)
		{// the point from the tree should always have 1 segment in its list
			UnsXY unspoint = pIns->cc; //segIn.hi;
			bool pHorizontal = (fabs(unspoint.y - seg.hi.y) < MAX_FLT_PRECISION);
			bool nHorizontal = (fabs(seg.lo.y - seg.hi.y) < MAX_FLT_PRECISION);
			if (pHorizontal || nHorizontal)
			{
				if (pHorizontal && nHorizontal)
				{// must be colinear on the horizontal
					if (seg.lo.x < unspoint.x)
					{
						pIns->cc = seg.lo;
						pIns->c = unspoint;
					}
					else
					{
						pIns->cc = unspoint;
						pIns->c = seg.lo;
					}
				}
				else
				{
					if (pHorizontal)
					{
						pIns->cc = seg.lo;
						pIns->c = unspoint;
					}
					else
					{
						pIns->cc = unspoint;
						pIns->c = seg.lo;
					}
				}
			}
			else
			{
				if (seg.lo < unspoint)
				{ // point order on the unshared point
					pIns->cc = seg.lo;
					pIns->c = unspoint;
				}
				else
				{
					pIns->cc = unspoint;
					pIns->c = seg.lo;
				}
			}
		}

		if (pIsNew) p = new UnsPointType;
		p->point = seg.lo;
		p->cc = seg.hi;
		pIns = p;
		pIsNew = PointTree.Insert(pIns); // true if inserted, false otherwise (i.e. found)
		if (!pIsNew)
		{// the point from the tree should always have 1 segment in its list
			UnsXY unspoint = pIns->cc; //segIn.hi;
			bool pHorizontal = (fabs(unspoint.y - seg.lo.y) < MAX_FLT_PRECISION);
			bool nHorizontal = (fabs(seg.lo.y - seg.hi.y) < MAX_FLT_PRECISION);
			if (pHorizontal || nHorizontal)
			{
				if (pHorizontal && nHorizontal)
				{// must be colinear on the horizontal
					if (seg.hi.x < unspoint.x)
					{
						pIns->cc = seg.hi;
						pIns->c = unspoint;
					}
					else
					{
						pIns->cc = unspoint;
						pIns->c = seg.hi;
					}
				}
				else
				{
					if (pHorizontal)
					{
						pIns->cc = seg.hi;
						pIns->c = unspoint;
					}
					else
					{
						pIns->cc = unspoint;
						pIns->c = seg.hi;
					}
				}
			}
			else
			{
				if (seg.hi < unspoint)
				{ // point order on the unshared point
					pIns->cc = seg.hi;
					pIns->c = unspoint;
				}
				else
				{
					pIns->cc = unspoint;
					pIns->c = seg.hi;
				}
			}
		}
		seg = SegTree.GetLeast(sTreeEmpty);
	}
	if (!pIsNew) delete p;
}

vector<pPGonWork> Unshare::ConsolidatePolygons(void)
{
	bool pTreeEmpty = false;
	UnsPointType pt, tpt, sp;
	pPGonWork wGon = NULL; //, wGonRoot = NULL, wGonTrail = NULL;
	pVertexNode pv = NULL, pvTrail = NULL;
	UnsXY topPt, unsPoint, trailPoint;
	vector<pPGonWork> r;
	InitializePointTree();
	// At This Point: SegTree is empty, PointTree will have 2 segments per point
	pt = PointTree.GetLeast(pTreeEmpty);
	while (!pTreeEmpty)
	{
		wGon = new PGonWork; //MakePGon(); //(pPGonWork)new PGonWork));
		//wGon->next = NULL;
		wGon->numVertices = 1;
		//wGon->pIdx = 0;
		//wGon->pName = NULL;
		//wGon->selVertex = -1;
		pv = new VertexNode; //MakeVertex(); 
		assert(pv->s == NULL);//(pVertexNode)new VertexNode));
		//pv->next = NULL;
		//pv->vtxInfo.is_selected = false;
		topPt = pt.point;
		pv->vtxInfo.vertex = topPt;
		//pv->vtxInfo.vertex.x = (float)topPt.x;
		//pv->vtxInfo.vertex.y = (float)topPt.y;
		wGon->vNode = pv;
		pvTrail = pv;
		//if (wGonTrail == NULL) wGonRoot = wGon;
		//else wGonTrail->next = wGon;
		//wGonTrail = wGon;
		trailPoint = topPt;
		unsPoint = pt.cc;
		sp = unsPoint;
		pNode = PointTree.Find(&sp);
		assert (pNode != NULL);
		tpt = *pNode->Data;
		PointTree.Delete();
		unsPoint = tpt.cc;
		if (unsPoint == trailPoint) unsPoint = tpt.c;
		trailPoint = tpt.point;
		while (unsPoint != topPt)
		{
			pv = new VertexNode; //MakeVertex(); 
			assert(pv->s == NULL);//(pVertexNode)new VertexNode));
			//pv->next = NULL;
			//pv->vtxInfo.is_selected = false;
			pv->vtxInfo.vertex = tpt.point;
			//pv->vtxInfo.vertex.x = (float)tpt.point.x;
			//pv->vtxInfo.vertex.y = (float)tpt.point.y;
			pvTrail->next = pv;
			pvTrail = pv;
			++wGon->numVertices;
			sp = unsPoint;
			pNode = PointTree.Find(&sp);
			tpt = *pNode->Data;
			PointTree.Delete();
			unsPoint = tpt.cc;
			if (unsPoint == trailPoint) unsPoint = tpt.c;
			trailPoint = tpt.point;
		}
		pv = new VertexNode; //MakeVertex(); 
		assert(pv->s == NULL);//(pVertexNode)new VertexNode));
		//pv->next = NULL;
		//pv->vtxInfo.is_selected = false;
		pv->vtxInfo.vertex.x = tpt.point.x;
		pv->vtxInfo.vertex.y = tpt.point.y;
		pvTrail->next = pv;
		pvTrail = pv;
		++wGon->numVertices;
		pt = PointTree.GetLeast(pTreeEmpty);
		r.push_back(wGon);
	}
	return r; //wGonRoot;
}