#include "first.hpp"
#include "CompGeo.hpp"
#include "Sorter.hpp"
#include "NGons.hpp"
#include "ConvexHull.hpp"

using namespace std;

ConvexHullXY::ConvexHullXY(void)
{
	x = 0.0;
	y = 0.0;
}

ConvexHullXY::ConvexHullXY(const CompGeo::XY & a)
{
	x = a.x;
	y = a.y;
}

bool operator<(const ConvexHullXY & a, const ConvexHullXY & b)
{
	if (fabs(b.x - a.x) < DBL_EPSILON) return (a.y < b.y);
	return (a.x < b.x);

}

ConvexHull::ConvexHull(void)
{
	L_upper = NULL;
	L_lower = NULL;
	num_in_upper = 0;
	num_in_lower = 0;
	numVerts = 0;
	vertices = NULL;
	//CompGeo::typeCompGeo::CGT = CompGeo::typeCompGeo::CONVEX_HULL;

}


ConvexHull::ConvexHull(vector<pPGonWork> *& gLst)
{
	L_upper = NULL;
	L_lower = NULL;
	num_in_upper = 0;
	num_in_lower = 0;
	numVerts = 0;
	vertices = NULL;
	if (gLst == NULL) return;
	int pMax = gLst->size();
	if (pMax == 0) return;
	pPGonWork wGon = NULL;
	for (vector<pPGonWork>::iterator it = gLst->begin(); it != gLst->end(); ++it)
	{
		wGon = *it;
		numVerts += wGon->numVertices;
	}
	if (numVerts == 0) return;
	vertices = new ConvexHullXY[numVerts];

	int i = 0;
	for (vector<pPGonWork>::iterator it = gLst->begin(); it != gLst->end(); ++it)
	{
		wGon = *it;
		pVertexNode v = wGon->vNode;
		while (v != NULL)
		{
			vertices[i].x = v->vtxInfo.vertex.x;
			vertices[i++].y = v->vtxInfo.vertex.y;
			v = v->next;
		}
	}
	// setting the next static flag enables a specific type of order on the XY type:
	//CompGeo::typeCompGeo::CGT = CompGeo::typeCompGeo::CONVEX_HULL;
	Sorter<ConvexHullXY> s(vertices, 0, numVerts - 1, numVerts);
	s.doSort();
	//CompGeo::typeCompGeo::CGT = CompGeo::typeCompGeo::NO_SELECTION;
}

ConvexHull::~ConvexHull(void)
{
	//DeleteXYArray(vertices);
	delete [] vertices;
	vertices = NULL;
	numVerts = 0;
	DeleteList(L_upper);
	DeleteList(L_lower);
	num_in_lower = 0;
	num_in_upper = 0;
}


void ConvexHull::AddVertex(ConvexHullXY xyT)
{	// assumes vertices holds a sorted list
	int index = numVerts, oldExts = index;
	for (int i = 0; i < numVerts; ++i)
	{
		if (xyT < vertices[i])
		{
			index = i;
			i = numVerts;
		}
	}
	++numVerts;
	ConvexHullXY * newXY = new ConvexHullXY[numVerts];
	newXY[index] = xyT;
	if (index > 0) memcpy(newXY, vertices, index * sizeof(ConvexHullXY));
	int numElmts = oldExts - index;
	if (numElmts > 0) memcpy(&newXY[index + 1], &vertices[index], numElmts * sizeof(ConvexHullXY));
	delete [] vertices;
	vertices = newXY;
	
}

void ConvexHull::AddToList(CompGeo::pXYListType & a, ConvexHullXY v)
{	// calling function must increment count
	// puts new element at end of circular list
	CompGeo::pXYListType e = new CompGeo::XYListType;
	memcpy(&e->xy, &v, sizeof(ConvexHullXY));
	e->last = e;
	e->next = e;
	if (a == NULL)
	{
		a = e;
		return;
	}
	CompGeo::pXYListType l = a->last, n = a->next;
	e->next = a;
	e->last = l;
	a->last = e;
	l->next = e;
	
}
	
void ConvexHull::DeletePenultimate(CompGeo::pXYListType & a)
{	// calling function must decrement count
	CompGeo::pXYListType nth = a->last, minus1 = nth->last, minus2 = minus1->last;

	nth->last = minus2;
	minus2->next = nth;
	delete minus1;
}
	
int ConvexHull::DeleteFirstAndLast(CompGeo::pXYListType & a, ConvexHullXY l, ConvexHullXY f)
{	// subtract return value from list count in calling function

	CompGeo::pXYListType nth = a->last, minus1 = nth->last, plus1 = a->next;
	int dCount = 0;

	if (a->xy == f)
	{
		delete a;
		nth->next = plus1;
		a = plus1;
		++dCount;
	}
	if (nth->xy == l)
	{
		delete nth;
		minus1->next = a;
		++dCount;
	}
	return dCount;
}
	
pPGonWork ConvexHull::Append(CompGeo::pXYListType & a, CompGeo::pXYListType & b)
{
	pPGonWork wGon = new PGonWork;
	/*
		(pPGonWork)new PGonWork));
	wGon->next = NULL;
	wGon->numVertices = 0;
	wGon->pIdx = 0;
	wGon->selVertex = -1;
	wGon->vNode = NULL;
	*/

	pNameNode pNameOut = new NameNode; //(pNameNode)new NameNode));
	string nmn = "cvx_Hull";
	//char chName[] = TEXT("cvx_Hull"), * nmn;
	//size_t l = _tcslen(chName) + sizeof(char);
	//nmn = new char[l];
	//HRESULT b_p = StringCbPrintf(nmn, l * sizeof(char), TEXT("%s"), chName);
	pNameOut->polyname = nmn;
	wGon->pName = pNameOut;
	CompGeo::pXYListType c;
	pVertexNode pvn = NULL, pvTrail = NULL;
	if (a != NULL)
	{
		c = a;
		do
		{
			pvn = new VertexNode; //MakeVertex();
			//(pVertexNode)new VertexNode));
			//pvn->vtxInfo.is_selected = false;
			pvn->vtxInfo.vertex = c->xy;
			//pvn->vtxInfo.vertex.x = (float)c->xy.x;
			//pvn->vtxInfo.vertex.y = (float)c->xy.y;
			pvn->next = NULL;
			if (pvTrail == NULL) wGon->vNode = pvn;
			else pvTrail->next = pvn;
			pvTrail = pvn;
			c = c->next;
			++wGon->numVertices;
		} while (c != a);
	}
	if (b != NULL)
	{
		c = b;
		do
		{
			pvn = new VertexNode; //MakeVertex(); //(pVertexNode)new VertexNode));
			//pvn->vtxInfo.is_selected = false;
			pvn->vtxInfo.vertex = c->xy;
			//pvn->vtxInfo.vertex.x = (float)c->xy.x;
			//pvn->vtxInfo.vertex.y = (float)c->xy.y;
			pvn->next = NULL;
			if (pvTrail == NULL) wGon->vNode = pvn;
			else pvTrail->next = pvn;
			pvTrail = pvn;
			c = c->next;
			++wGon->numVertices;
		} while (c != b);
	}
	return wGon;
}
	
void ConvexHull::DeleteList(CompGeo::pXYListType & a)
{  // this shows the peculiarity of the circular list:
	CompGeo::pXYListType adv = a, nxt = NULL;
	if (a != NULL)
	{
		while (a->next != a)
		{
			adv = a->next->next;
			delete a->next;
			a->next = adv;
		}
		delete a;
	}
	a = NULL;
}

pPGonWork ConvexHull::CalculateHull(void)
{
	if (numVerts < 3) return NULL;

	int n = numVerts;
	CompGeo::EdgeType e;
	bool rightT;
	CompGeo::pXYListType end, minus1, minus2;
	ConvexHullXY comparator;


	// top
	AddToList(L_upper, vertices[0]);
	AddToList(L_upper, vertices[1]);
	num_in_upper = 2;
	for(int i = 2; i < n; ++i)
	{
		AddToList(L_upper, vertices[i]);
		++num_in_upper;

		do
		{
			rightT = true;
			if (num_in_upper > 2)
			{
				end = L_upper->last;
				minus1 = end->last;
				minus2 = minus1->last;
				e.lo.x = minus2->xy.x;
				e.lo.y = minus2->xy.y;
				e.hi.x = end->xy.x;
				e.hi.y = end->xy.y;
				comparator.x = minus1->xy.x;
				comparator.y = minus1->xy.y;
				rightT = CompGeo::IsRightTurn(e, comparator);
				if (!rightT)
				{
					DeletePenultimate(L_upper);
					--num_in_upper;
				}

			}
		} while (!rightT);
	}


		// bottom
	AddToList(L_lower, vertices[n - 1]);
	AddToList(L_lower, vertices[n - 2]);
	num_in_lower = 2;
	for(int i = n - 3; i >= 0; --i)
	{
		AddToList(L_lower, vertices[i]);
		++num_in_lower;

		do
		{
			rightT = true;
			if (num_in_lower > 2)
			{
				end = L_lower->last;
				minus1 = end->last;
				minus2 = minus1->last;
				e.lo.x = minus2->xy.x;
				e.lo.y = minus2->xy.y;
				e.hi.x = end->xy.x;
				e.hi.y = end->xy.y;
				comparator.x = minus1->xy.x;
				comparator.y = minus1->xy.y;
				rightT = CompGeo::IsRightTurn(e, comparator);
				if (!rightT)
				{
					DeletePenultimate(L_lower);
					--num_in_lower;
				}

			}
		} while (!rightT);
	}
	
	if ((L_lower != NULL) && (L_upper != NULL))
		num_in_lower -= DeleteFirstAndLast(L_lower, L_upper->xy, L_upper->last->xy);
	return Append(L_upper, L_lower);

}
