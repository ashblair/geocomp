#include "first.hpp"
#include "CompGeo.hpp"
#include "NGons.hpp"
#include "SegmentIntersect.hpp"
#include "Triangulation.hpp"

using namespace std;

//unsigned int Triangulation::tPhase = 0;
TriXY Triangulation::refPt;
//double Triangulation::XofY

TriXY::TriXY(void)
{
	x = 0.0;
	y = 0.0;
}

TriXY::TriXY(const CompGeo::XY & a)
{
	x = a.x;
	y = a.y;
}

bool operator<(TriXY & a, TriXY & b)
{
	if (fabs(b.y - a.y) < MAX_FLT_PRECISION) return (a.x < b.x);
	return (a.y > b.y);
}

bool operator > (TriXY & a, TriXY & b)
{
	if (fabs(b.y - a.y) < MAX_FLT_PRECISION) return (a.x > b.x);
	return (a.y > b.y);

}

bool operator<(TriPointType & a, TriPointType & b)
{
	return (a.point < b.point);
}

bool operator==(TriPointType & a, TriPointType & b)
{
	return (a.point == b.point);
}

TriPointStruct::TriPointStruct(void)
{
	point.x = 0.0;
	point.y = 0.0;
	segs = NULL;
}

TriPointStruct::TriPointStruct(const TriPointStruct & pt)
{
	point = pt.point;
	segs = NULL;
	pTriSegListType pl = NULL, plTrail = NULL, pt_pl = pt.segs;
	while (pt_pl != NULL)
	{
		pl = new TriSegListType;
		pl->next = NULL;
		pl->seg = new TriSegType(*pt_pl->seg);
		//*pl->seg = *pt_pl->seg;
		if (plTrail == NULL) segs = pl;
		else plTrail->next = pl;
		plTrail = pl;
		pt_pl = pt_pl->next;
	}
}

TriPointStruct & TriPointStruct::operator=(const TriPointStruct & pt)
{
	point = pt.point;
	pTriSegListType pl = NULL, plTrail = segs, pt_pl = pt.segs;
	while (plTrail != NULL)
	{
		pl = plTrail->next;
		delete plTrail->seg;
		delete plTrail;
		plTrail = pl;
	}
	segs = NULL;
	while (pt_pl != NULL)
	{
		pl = new TriSegListType;
		pl->next = NULL;
		pl->seg = new TriSegType(*pt_pl->seg);
		//*pl->seg = *pt_pl->seg;
		if (plTrail == NULL) segs = pl;
		else plTrail->next = pl;
		plTrail = pl;
		pt_pl = pt_pl->next;

	}
	return *this;
}

TriPointStruct::~TriPointStruct(void)
{
	point = CompGeo::XY(0.0, 0.0);
	pTriSegListType pl = NULL, plTrail = segs;
	while (plTrail != NULL)
	{
		pl = plTrail->next;
		delete plTrail->seg;
		delete plTrail;
		plTrail = pl;
	}
	segs = NULL;

}

TriSegStruct::TriSegStruct(void)
{
	lo.x = 0.0;
	lo.y = 0.0;
	hi.x = 0.0;
	hi.y = 0.0;
	hEdge = NULL;
}

TriSegStruct::TriSegStruct(const TriSegStruct & seg)
{
	lo.x = seg.lo.x;
	lo.y = seg.lo.y;
	hi.x = seg.hi.x;
	hi.y = seg.hi.y;
	hEdge = seg.hEdge;
}

TriSegStruct::TriSegStruct(pTriHalfEdgeType & he)
{
	hEdge = he;
	TriXY A = he->Origin->Coordinates, B = he->Twin->Origin->Coordinates;
	if (fabs(A.y - B.y) < MAX_FLT_PRECISION)
	{
		if (A.x > B.x) 
		{
			lo = A;
			hi = B;
		}
		else
		{
			lo = B;
			hi = A;
		}
	}
	else
	{
		if (A.y < B.y)
		{
			lo = A;
			hi = B;
		}
		else
		{
			lo = B;
			hi = A;
		}
	}
}

TriSegStruct & TriSegStruct::operator = (const TriSegStruct & seg)
{
	lo.x = seg.lo.x;
	lo.y = seg.lo.y;
	hi.x = seg.hi.x;
	hi.y = seg.hi.y;
	hEdge = seg.hEdge;
	return *this;
}

bool operator == (const TriSegType & a, const TriSegType & b)
{
	return ((a.hi == b.hi) && (a.lo == b.lo));
}

bool operator < (const TriSegType & a, const TriSegType & b)
{ // this is to be used with AVL findleaf to get left neighbor, make a a dummy vertical seg at x=refPt.x
  // or insert to build list
	bool aHorizontal = (fabs(a.hi.y - a.lo.y) < MAX_FLT_PRECISION),
		bHorizontal = (fabs(b.hi.y - b.lo.y) < MAX_FLT_PRECISION);
	if (aHorizontal || bHorizontal)
	{
		bool sameYs = (fabs(a.hi.y - b.hi.y) < MAX_FLT_PRECISION),
			sameXHis = (fabs(a.hi.x - b.hi.x) < MAX_FLT_PRECISION);
		if (aHorizontal && bHorizontal) 
		{
			if (sameYs)
			{
				if (sameXHis) return (a.lo.x < b.lo.x);
				else return (a.hi.x < b.hi.x);
			}
			else return (a.hi.y > b.hi.y);
		}
		else
		{
			if (aHorizontal) return false;
			else return true;
		}
	}
	double b_x = Triangulation::XofY(b), a_x = Triangulation::XofY(a);
	return (a_x <= b_x);
}

TriStack::TriStack(void)
{
	stackRoot = NULL;
}

TriStack::~TriStack(void)
{
	pTriPointListType p = NULL;
	while (stackRoot != NULL)
	{
		p = stackRoot->next;
		delete stackRoot;
		stackRoot = p;
	}
}

void TriStack::push(pTriPointType pt)
{
	pTriPointListType p = new TriPointListType;
	p->next = stackRoot;
	p->point = pt;
	stackRoot = p;
}

pTriPointType TriStack::pop(void)
{
	if (stackRoot == NULL) return NULL;
	pTriPointListType p = stackRoot;
	stackRoot = stackRoot->next;
	pTriPointType pt = p->point;
	delete p;
	return pt;
}

pTriPointType TriStack::peek(void)
{
	if (stackRoot == NULL) return NULL;
	pTriPointListType p = stackRoot;
	pTriPointType pt = p->point;
	return pt;

}

Triangulation::Triangulation(void)
{
	tLst.NumVertices = 0;
	tLst.NumFaces = 0;
	tLst.Faces = NULL;
	tLst.Vertices = NULL;
	fMonotonized = false;
	nxtNewEdge = 0;

}
	
Triangulation::Triangulation(NGons & ng, bool fMonotoneOnly, string & eMsg)
{
	tLst.NumVertices = 0;
	tLst.NumFaces = 0;
	tLst.Faces = NULL;
	tLst.Vertices = NULL;
	fMonotonized = true;
	nxtNewEdge = 0;
	TriListLoad(ng, eMsg);
	if (tLst.Faces != NULL) 
	{
		setForMonotone(tLst.Faces);
		if (!fMonotoneOnly) setForTriangulation(tLst.Faces);
	}
}

template<typename T> Triangulation::Triangulation(vector<CompGeo::pFaceType> & vf, CompGeo::DCEL<T> *& dcel)
{
	if (dcel == NULL) return;
	tLst.NumVertices = 0;
	tLst.NumFaces = 0;
	tLst.Faces = NULL;
	tLst.Vertices = NULL;
	fMonotonized = true;
	nxtNewEdge = 0;
	TriListLoad(vf, dcel);
	if (tLst.Faces != NULL) 
	{
		setForMonotone(tLst.Faces);
	}

}

Triangulation::Triangulation(const Triangulation & t)
{
	tLst.NumVertices = t.tLst.NumVertices;
	tLst.NumFaces = t.tLst.NumFaces;
	tLst.Faces = NULL;
	fMonotonized = t.fMonotonized;
	nxtNewEdge = t.nxtNewEdge;
	// Array copying:
	tLst.Vertices = new TriVertexType[tLst.NumVertices];
	memcpy(tLst.Vertices, t.tLst.Vertices, tLst.NumVertices * sizeof(TriVertexType));
	// Recursive List building:
	if (!fMonotonized) tLst.Faces = buildFace(t.tLst.Faces);
}
	
Triangulation::~Triangulation(void)
{
	delete [] tLst.Vertices;
	tLst.Vertices = NULL;
	tLst.NumVertices = 0;
	if (tLst.Faces != NULL)
	{
		pTriHalfEdgeType ph0 = NULL, phN = NULL;
		ph0 = chainHalfs(tLst.Faces, phN);
		deleteFace(tLst.Faces);
		while (ph0 != NULL)
		{
			phN = ph0->Next;
			delete ph0;
			ph0 = phN;
		}
	}
	tLst.NumFaces = 0;
}

pTriHalfEdgeType Triangulation::chainHalfs(pTriFaceType pf, pTriHalfEdgeType & phLast)
{// do not call this with an empty face
	pTriHalfEdgeType ph = pf->OuterComponent, phRoot = ph,
		ph0 = NULL, phOnOut = NULL, phBP = NULL, phBPNxt = NULL;
	if (ph != NULL) 
	{
		phLast = ph->Prev;
		phLast->Next = NULL;
	}
	pTriHalfEdgeListType pl = pf->InnerComponents, plNext = NULL;
	while (pl != NULL)
	{
		plNext = pl->next;
		ph = pl->OneEdge;
		if ((phOnOut == NULL) && (ph->IncidentFace == pf)) phOnOut = ph;
		if (phRoot == NULL) phRoot = chainHalfs(ph->Twin->IncidentFace, phBP);
		else phLast->Next = chainHalfs(ph->Twin->IncidentFace, phBP);
		phLast = phBP;
		pl = plNext;
	}
	if (((pf->FaceType == 'H') || (!fMonotonized)) && (phOnOut != NULL))
	{ // all edges around holes within a fill will be edges of new or modified faces
	  // after monotone and triangulation algorithms are run
		ph0 = phOnOut; // must not be NULL
		if (phRoot == NULL) phRoot = ph0;
		else phLast->Next = ph0;
		phLast = ph0->Prev;
		phLast->Next = NULL;
	}


	return phRoot;
}

void Triangulation::deleteFace(pTriFaceType p)
{
	pTriHalfEdgeListType pHEL = p->InnerComponents;
	while (pHEL != NULL)
	{
		pTriFaceType pIn = pHEL->OneEdge->Twin->IncidentFace;
		deleteFace(pIn);
		pHEL = pHEL->next;
	}
	delete p;
}
/*
void Triangulation::deleteFace(pTriFaceType p)
{
	if (p == NULL) return;
	pTriHalfEdgeType ph = p->OuterComponent, phNext = NULL, // ring topology
		ph0 = NULL, phOnOut = NULL;
	pTriHalfEdgeListType pl = p->InnerComponents, plNext = NULL;
	for (unsigned int i = 0; i < p->NumEdges; ++i)
	{
		phNext = ph->Next;
		if (ph->Twin != NULL) ph->Twin->Twin = NULL;
		delete ph;
		ph = phNext;
	}
	pTriFaceListType pfl = NULL, pflTrail = NULL, pflRoot = NULL;
	while (pl != NULL)
	{
		pfl = (pTriFaceListType)new TriFaceListType));
		pfl->next = NULL;
		pfl->a_face = pl->OneEdge->Twin->IncidentFace;
		if (pflTrail == NULL) pflRoot = pfl;
		else pflTrail->next = pfl;
		pflTrail = pfl;
	}
	pfl = pflRoot;
	pl = p->InnerComponents;
	while (pl != NULL)
	{
		plNext = pl->next;
		ph = pl->OneEdge;
		if ((phOnOut == NULL) && (ph->IncidentFace == p)) phOnOut = ph;
		deleteFace(pfl->a_face);
		delete pl;
		pl = plNext;
		pfl = pfl->next;
		delete pflRoot;
		pflRoot = pfl;
	}
	if (((p->FaceType == 'H') || (!fMonotonized)) && (phOnOut != NULL))
	{ // all edges around holes within a fill will be edges of new or modified faces
	  // after monotone and triangulation algorithms are run
		ph0 = phOnOut; // must not be NULL
		ph = ph0->Next;
		do
		{
			ph0->Next = ph->Next;
			delete ph;
			ph = ph0->Next;
		} while (ph != ph0);
		delete ph0;
	}
	delete p;
}
*/
pTriFaceType Triangulation::buildFace(pTriFaceType p)
{ // call this after the Vertices in tLst have been set but 
  // before monotone or triangularize procedures have been run

	if (p == NULL) return NULL;
	pTriFaceType p_f = new TriFaceType, pfn = NULL;
	pTriHalfEdgeType ph = p->OuterComponent, // ring topology
		phn = NULL, phnTrail = NULL;
	pTriHalfEdgeListType pl = p->InnerComponents, pln = NULL, plnTrail = NULL;
	p_f->Face = p->Face;
	p_f->FaceType = p->FaceType;
	p_f->NumEdges = p->NumEdges;
	for (unsigned int i = 0; i < p->NumEdges; ++i)
	{
		phn = new TriHalfEdgeType;
		memcpy(phn, ph, sizeof(TriHalfEdgeType));
		phn->Twin = new TriHalfEdgeType;
		memcpy(phn->Twin, ph->Twin, sizeof(TriHalfEdgeType));
		phn->IncidentFace = p_f;
		phn->Origin = &tLst.Vertices[ph->Origin->Vertex];
		phn->Twin->Twin = phn;
		phn->Twin->Origin = &tLst.Vertices[ph->Twin->Origin->Vertex];
		phn->Twin->IncidentFace = NULL;
		phn->Prev = NULL; //phnTrail;
		phn->Twin->Next = NULL; //phnTrail->Twin;
		if (phnTrail == NULL) p_f->OuterComponent = phn;
		else
		{
			phnTrail->Next = phn;
			phnTrail->Twin->Prev = phn->Twin;
			phn->Twin->Next = phnTrail->Twin;
			phn->Prev = phnTrail;
		}
		phnTrail = phn;
		ph = ph->Next;
	}
	if (p->NumEdges > 0)
	{
		phn = p_f->OuterComponent;
		phn->Prev = phnTrail;
		phn->Twin->Next = phnTrail->Twin;
		phnTrail->Next = phn;
		phnTrail->Twin->Prev = phn->Twin;
	}

	while (pl != NULL)
	{
		pfn = buildFace(pl->OneEdge->Twin->IncidentFace);
		ph = pfn->OuterComponent->Twin;
		for (unsigned int i = 0; i < pfn->NumEdges; ++i) 
		{
			ph->IncidentFace = p_f;
			ph = ph->Next;
		}
		pln = new TriHalfEdgeListType;
		pln->next = NULL;
		pln->OneEdge = ph;
		if (plnTrail == NULL) p_f->InnerComponents = pln;
		else plnTrail->next = pln;
		plnTrail = pln;
		pl = pl->next;
	}
	return p_f;
	
}

template<typename T> void Triangulation::TriListLoad(vector<CompGeo::pFaceType> & vf, CompGeo::DCEL<T> *& dcel)
{
	//if (tLst.Vertices != NULL) delete [] tLst.Vertices;
	//tLst.NumFaces = 0;
	//tLst.NumVertices = 0;
	vector<vector<CompGeo::XY>> polys;
	unsigned int e_c = 0;
	for (vector<CompGeo::pFaceType>::iterator fit = vf.begin(); fit != vf.end(); ++fit)
	{
		CompGeo::pFaceType pf = *fit;
		vector<CompGeo::pHalfEdgeType> halfVec = dcel->all_outer(pf);
		e_c += halfVec.size(); 
		vector<CompGeo::XY> outer;
		CompGeo::pHalfEdgeType p_h = dcel->getHalfEdge(pf->outer);
		for (unsigned int i = 0; i < e_c; ++i)
		{
			CompGeo::XY xy;
			CompGeo::pTPoint<T> tPt = dcel->origin(p_h);
			xy.x = static_cast<double>(tPt->xyz.X);
			xy.y = static_cast<double>(tPt->xyz.Y);
			//ignoring Z?
			outer.push_back(xy);
			p_h = dcel->next();
		}
		polys.push_back(outer);

		CompGeo::ITYPE innerCmpIdx = pf->inner;
		while (innerCmpIdx != 0)
		{
			CompGeo::pHalfEdgeListType pHEL = dcel->getHalfEdgeList(innerCmpIdx);
			CompGeo::pHalfEdgeType p_h0 = dcel->top(pHEL);
			p_h = NULL;
			vector<CompGeo::XY> poly;
			while (p_h0 != p_h)
			{
				p_h = dcel->prev(); // inners are clockwise but we want a counter-clockwise vector
				CompGeo::XY xy;
				CompGeo::pTPoint<T> tPt = dcel->origin(p_h);
				xy.x = static_cast<double>(tPt->xyz.X);
				xy.y = static_cast<double>(tPt->xyz.Y);
				poly.push_back(xy);
			}
			e_c += poly.size();
			innerCmpIdx = pHEL->nxt;
			polys.push_back(poly);
		}	
	}
	tLst.Vertices = new TriVertexType[e_c];
	unsigned int vIdx = 0;
	pTriSegType segs = new TriSegType[e_c];
	tLst.NumVertices = e_c;
	tLst.NumFaces = polys.size(); // add one to this after for loop
	pTriFaceType pf0 = new TriFaceType; // this is the exterior
	pTriHalfEdgeListType pfTrail = NULL;
	pf0->Face = 0;
	pf0->FaceType = 'H';
	pf0->InnerComponents = NULL;
	//pf0->next = NULL;
	pf0->NumEdges = 0;
	pf0->OuterComponent = NULL;
	tLst.Faces = pf0;
	CompGeo::AVL<TriPointType> EventQ;
	for (unsigned int i = 0; i < tLst.NumFaces; ++i)
	{
		//pPGonFile fGon = SegmentIntersect::SegIntVertices[i].polyVertices;
		vector<CompGeo::XY> vSI = polys.at(i); //SegmentIntersect::SegIntVertices[i].verts;
		pTriFaceType pf = new TriFaceType; //(pTriFaceType)new TriFaceType));
		//pf->Face = fGon->pIdx + 1;
		pf->Face = i + 1;
		pf->FaceType = ' ';
		pf->InnerComponents = NULL;
		//pf->next = NULL;
		//pf->NumEdges = fGon->vertices.size();
		pf->NumEdges = vSI.size();
		pf->OuterComponent = NULL;
		pTriHalfEdgeType ph = NULL, phTrail = NULL;
		pTriPointType pPt0 = NULL, pPtTrail = NULL;
		//for (unsigned int j = 0; j < fGon->vertices.size(); ++j)
		for (unsigned int j = 0; j < vSI.size(); ++j)
		{
			pTriVertexType pv = &tLst.Vertices[vIdx + j];
			//SegIntXY sXY = vSI.at(j);
			//pv->Coordinates = CompGeo::XY(fGon->vertices[j].vertex);
			pv->Coordinates = vSI.at(j); //CompGeo::XY(sXY.x, sXY.y);
			pv->IncidentEdge = NULL;
			pv->TurnType = ' ';
			pv->Vertex = vIdx + j;
			pTriPointType pPt = new TriPointType; //(pTriPointType)new TriPointType));
			pPt->point = pv->Coordinates;
			pPt->segs = NULL;
			if (pPt0 == NULL) pPt0 = pPt;
			EventQ.AlwaysInsert(pPt);
			if (j > 0)
			{// edges take 1:
				ph = new TriHalfEdgeType; //(pTriHalfEdgeType)new TriHalfEdgeType));
				ph->Polygon = pf->Face;
				ph->Edge = j - 1;
				ph->Half = 1;  // 0 is outside, 1 is inside
				ph->Origin = &tLst.Vertices[vIdx + j - 1];
				tLst.Vertices[vIdx + j - 1].IncidentEdge = ph;
				ph->Helper = NULL;
				ph->IncidentFace = pf;
				ph->Twin = new TriHalfEdgeType; //(pTriHalfEdgeType)new TriHalfEdgeType));
					//ph->Twin->Edge = (fGon->vertices.size() - j + 1) % fGon->vertices.size();
					ph->Twin->Edge = (vSI.size() - j + 1) % vSI.size();
					ph->Twin->Half = 0;
					ph->Twin->Origin = pv;
					ph->Twin->Helper = NULL;
					ph->Twin->IncidentFace = pf0;  // unknown right now
					ph->Twin->Polygon = 0; // also unknown right now -- these are maybes
					ph->Twin->Next = NULL; //phTrail->Twin;
					ph->Twin->Prev = NULL;
					ph->Twin->Twin = ph;
				ph->Prev = NULL;
				ph->Next = NULL;
				if (phTrail == NULL) 
				{
					pf->OuterComponent = ph;
					pTriHalfEdgeListType pHEL = new TriHalfEdgeListType;
					//	(pTriHalfEdgeListType)new TriHalfEdgeListType));
					pHEL->next = NULL;
					pHEL->OneEdge = ph->Twin;
					if (pfTrail == NULL) pf0->InnerComponents = pHEL;
					else pfTrail->next = pHEL;  // correct only if no faces are contained by another
					pfTrail = pHEL;
				}
				else 
				{
					ph->Prev = phTrail;
					phTrail->Twin->Prev = ph->Twin;
					ph->Twin->Next = phTrail->Twin;
					phTrail->Next = ph;
				}
				phTrail = ph;
				pTriSegType pSeg = &segs[vIdx + j - 1];
				pSeg->hEdge = ph;
				if (fabs(pPt->point.y - pPtTrail->point.y) < MAX_FLT_PRECISION)
				{
					if (pPt->point.x > pPtTrail->point.x)
					{
						pSeg->lo = pPt->point;
						pSeg->hi = pPtTrail->point;
					}
					else
					{
						pSeg->lo = pPtTrail->point;
						pSeg->hi = pPt->point;
					}
				}
				else
				{
					if (pPt->point.y < pPtTrail->point.y)
					{
						pSeg->lo = pPt->point;
						pSeg->hi = pPtTrail->point;
					}
					else
					{
						pSeg->lo = pPtTrail->point;
						pSeg->hi = pPt->point;
					}
				}

			}
			pPtTrail = pPt;
		}  // for j loop end

		pTriVertexType pv = &tLst.Vertices[vIdx]; // the zero vertex
		// now create the edges that join vertex 0 to vertex n-1
		ph = new TriHalfEdgeType; //(pTriHalfEdgeType)new TriHalfEdgeType));
		ph->Polygon = pf->Face;
		ph->Edge = vSI.size() - 1;
		ph->Half = 1;  // 0 is outside, 1 is inside
		ph->Origin = &tLst.Vertices[vIdx + vSI.size() - 1];
		tLst.Vertices[vIdx + vSI.size() - 1].IncidentEdge = ph;
		ph->Helper = NULL;
		ph->IncidentFace = pf;
		ph->Twin = new TriHalfEdgeType; //(pTriHalfEdgeType)new TriHalfEdgeType));
			ph->Twin->Edge = 1;
			ph->Twin->Half = 0;
			ph->Twin->Origin = &tLst.Vertices[vIdx];
			ph->Twin->Helper = NULL;
			ph->Twin->IncidentFace = pf0;  // unknown right now
			ph->Twin->Polygon = 0; // also unknown right now -- these are maybes
			ph->Twin->Next = phTrail->Twin;
			ph->Twin->Prev = pf->OuterComponent->Twin;
			ph->Twin->Twin = ph;
		ph->Next = pf->OuterComponent;
		ph->Prev = phTrail;
		phTrail->Next = ph; // phTrail is never NULL here
		phTrail->Twin->Prev = ph->Twin;
		pf->OuterComponent->Prev = ph;
		pf->OuterComponent->Twin->Next = ph->Twin;

		pTriSegType pSeg = &segs[vIdx + vSI.size() - 1];
		pSeg->hEdge = ph;
		if (fabs(pPt0->point.y - pPtTrail->point.y) < MAX_FLT_PRECISION)
		{
			if (pPt0->point.x > pPtTrail->point.x)
			{
				pSeg->lo = pPt0->point;
				pSeg->hi = pPtTrail->point;
			}
			else
			{
				pSeg->lo = pPtTrail->point;
				pSeg->hi = pPt0->point;
			}
		}
		else
		{
			if (pPt0->point.y < pPtTrail->point.y)
			{
				pSeg->lo = pPt0->point;
				pSeg->hi = pPtTrail->point;
			}
			else
			{
				pSeg->lo = pPtTrail->point;
				pSeg->hi = pPt0->point;
			}

		}
		vIdx += vSI.size();
	} // for i loop end
	++tLst.NumFaces; // one more for the exterior "hole"
	// get relative positioning of faces:
	doInnerComponents(EventQ, segs);
	setFaceType('H', tLst.Faces);
	setTurnType(tLst.Faces);

}
		
bool Triangulation::TriListLoad(NGons & ng, string & errMsg)
{
	//tPhase = 0;
	if (!ng.enforceCC())
	{
		//MessageBox(NULL, TEXT("All intersections must be removed before triangulation can occur!"), 
		//	TEXT("Cannot order vertices counterclockwise"), MB_OK);
		errMsg = "Cannot order vertices counterclockwise: all intersections must be removed before triangulation can occur!"; 
		return false;

	}
	SegmentIntersect S_I(ng);
	S_I.getAllIntersects();
	if (S_I.rptI != NULL)
	{
		//MessageBox(NULL, TEXT("All intersections must be removed before triangulation can occur!"), 
		//	TEXT("Fix Intersection Problem"), MB_OK);
		errMsg = "Fix Intersection Problem: all intersections must be removed before triangulation can occur!";
		return false;
	}
	
	if (tLst.Vertices != NULL) delete [] tLst.Vertices;
	unsigned int e_c = SegmentIntersect::SegIntEdgeCount;
	tLst.Vertices = new TriVertexType[e_c];
	unsigned int vIdx = 0;
	pTriSegType segs = new TriSegType[e_c];
	tLst.NumVertices = e_c;
	tLst.NumFaces = SegmentIntersect::SegIntPolygonCount; // add one to this after for loop
	pTriFaceType pf0 = new TriFaceType; //(pTriFaceType)new TriFaceType));  // this is the exterior
	pTriHalfEdgeListType pfTrail = NULL;
	pf0->Face = 0;
	pf0->FaceType = 'H';
	pf0->InnerComponents = NULL;
	//pf0->next = NULL;
	pf0->NumEdges = 0;
	pf0->OuterComponent = NULL;
	tLst.Faces = pf0;
	CompGeo::AVL<TriPointType> EventQ;
	for (unsigned int i = 0; i < tLst.NumFaces; ++i)
	{
		//pPGonFile fGon = SegmentIntersect::SegIntVertices[i].polyVertices;
		vector<CompGeo::XY> vSI = SegmentIntersect::SegIntVertices[i].verts;
		pTriFaceType pf = new TriFaceType; //(pTriFaceType)new TriFaceType));
		//pf->Face = fGon->pIdx + 1;
		pf->Face = i + 1;
		pf->FaceType = ' ';
		pf->InnerComponents = NULL;
		//pf->next = NULL;
		//pf->NumEdges = fGon->vertices.size();
		pf->NumEdges = vSI.size();
		pf->OuterComponent = NULL;
		pTriHalfEdgeType ph = NULL, phTrail = NULL;
		pTriPointType pPt0 = NULL, pPtTrail = NULL;
		//for (unsigned int j = 0; j < fGon->vertices.size(); ++j)
		for (unsigned int j = 0; j < vSI.size(); ++j)
		{
			pTriVertexType pv = &tLst.Vertices[vIdx + j];
			SegIntXY sXY = vSI.at(j);
			//pv->Coordinates = CompGeo::XY(fGon->vertices[j].vertex);
			pv->Coordinates = CompGeo::XY(sXY.x, sXY.y);
			pv->IncidentEdge = NULL;
			pv->TurnType = ' ';
			pv->Vertex = vIdx + j;
			pTriPointType pPt = new TriPointType; //(pTriPointType)new TriPointType));
			pPt->point = pv->Coordinates;
			pPt->segs = NULL;
			if (pPt0 == NULL) pPt0 = pPt;
			EventQ.AlwaysInsert(pPt);
			if (j > 0)
			{// edges take 1:
				ph = new TriHalfEdgeType; //(pTriHalfEdgeType)new TriHalfEdgeType));
				ph->Polygon = pf->Face;
				ph->Edge = j - 1;
				ph->Half = 1;  // 0 is outside, 1 is inside
				ph->Origin = &tLst.Vertices[vIdx + j - 1];
				tLst.Vertices[vIdx + j - 1].IncidentEdge = ph;
				ph->Helper = NULL;
				ph->IncidentFace = pf;
				ph->Twin = new TriHalfEdgeType; //(pTriHalfEdgeType)new TriHalfEdgeType));
					//ph->Twin->Edge = (fGon->vertices.size() - j + 1) % fGon->vertices.size();
					ph->Twin->Edge = (vSI.size() - j + 1) % vSI.size();
					ph->Twin->Half = 0;
					ph->Twin->Origin = pv;
					ph->Twin->Helper = NULL;
					ph->Twin->IncidentFace = pf0;  // unknown right now
					ph->Twin->Polygon = 0; // also unknown right now -- these are maybes
					ph->Twin->Next = NULL; //phTrail->Twin;
					ph->Twin->Prev = NULL;
					ph->Twin->Twin = ph;
				ph->Prev = NULL;
				ph->Next = NULL;
				if (phTrail == NULL) 
				{
					pf->OuterComponent = ph;
					pTriHalfEdgeListType pHEL = new TriHalfEdgeListType;
					//	(pTriHalfEdgeListType)new TriHalfEdgeListType));
					pHEL->next = NULL;
					pHEL->OneEdge = ph->Twin;
					if (pfTrail == NULL) pf0->InnerComponents = pHEL;
					else pfTrail->next = pHEL;  // correct only if no faces are contained by another
					pfTrail = pHEL;
				}
				else 
				{
					ph->Prev = phTrail;
					phTrail->Twin->Prev = ph->Twin;
					ph->Twin->Next = phTrail->Twin;
					phTrail->Next = ph;
				}
				phTrail = ph;
				pTriSegType pSeg = &segs[vIdx + j - 1];
				pSeg->hEdge = ph;
				if (fabs(pPt->point.y - pPtTrail->point.y) < MAX_FLT_PRECISION)
				{
					if (pPt->point.x > pPtTrail->point.x)
					{
						pSeg->lo = pPt->point;
						pSeg->hi = pPtTrail->point;
					}
					else
					{
						pSeg->lo = pPtTrail->point;
						pSeg->hi = pPt->point;
					}
				}
				else
				{
					if (pPt->point.y < pPtTrail->point.y)
					{
						pSeg->lo = pPt->point;
						pSeg->hi = pPtTrail->point;
					}
					else
					{
						pSeg->lo = pPtTrail->point;
						pSeg->hi = pPt->point;
					}
				}

			}
			pPtTrail = pPt;
		}  // for j loop end

		pTriVertexType pv = &tLst.Vertices[vIdx]; // the zero vertex
		// now create the edges that join vertex 0 to vertex n-1
		ph = new TriHalfEdgeType; //(pTriHalfEdgeType)new TriHalfEdgeType));
		ph->Polygon = pf->Face;
		ph->Edge = vSI.size() - 1;
		ph->Half = 1;  // 0 is outside, 1 is inside
		ph->Origin = &tLst.Vertices[vIdx + vSI.size() - 1];
		tLst.Vertices[vIdx + vSI.size() - 1].IncidentEdge = ph;
		ph->Helper = NULL;
		ph->IncidentFace = pf;
		ph->Twin = new TriHalfEdgeType; //(pTriHalfEdgeType)new TriHalfEdgeType));
			ph->Twin->Edge = 1;
			ph->Twin->Half = 0;
			ph->Twin->Origin = &tLst.Vertices[vIdx];
			ph->Twin->Helper = NULL;
			ph->Twin->IncidentFace = pf0;  // unknown right now
			ph->Twin->Polygon = 0; // also unknown right now -- these are maybes
			ph->Twin->Next = phTrail->Twin;
			ph->Twin->Prev = pf->OuterComponent->Twin;
			ph->Twin->Twin = ph;
		ph->Next = pf->OuterComponent;
		ph->Prev = phTrail;
		phTrail->Next = ph; // phTrail is never NULL here
		phTrail->Twin->Prev = ph->Twin;
		pf->OuterComponent->Prev = ph;
		pf->OuterComponent->Twin->Next = ph->Twin;

		pTriSegType pSeg = &segs[vIdx + vSI.size() - 1];
		pSeg->hEdge = ph;
		if (fabs(pPt0->point.y - pPtTrail->point.y) < MAX_FLT_PRECISION)
		{
			if (pPt0->point.x > pPtTrail->point.x)
			{
				pSeg->lo = pPt0->point;
				pSeg->hi = pPtTrail->point;
			}
			else
			{
				pSeg->lo = pPtTrail->point;
				pSeg->hi = pPt0->point;
			}
		}
		else
		{
			if (pPt0->point.y < pPtTrail->point.y)
			{
				pSeg->lo = pPt0->point;
				pSeg->hi = pPtTrail->point;
			}
			else
			{
				pSeg->lo = pPtTrail->point;
				pSeg->hi = pPt0->point;
			}

		}
		vIdx += vSI.size();
	} // for i loop end
	++tLst.NumFaces; // one more for the exterior "hole"
	// get relative positioning of faces:
	doInnerComponents(EventQ, segs);
	setFaceType('H', tLst.Faces);
	setTurnType(tLst.Faces);
	return true;
}

double Triangulation::XofY(const TriSegType & seg)
{
	double dY = seg.hi.y - seg.lo.y, dX = seg.hi.x - seg.lo.x;
	bool IsHorizontal = (fabs(dY) < MAX_FLT_PRECISION);
	if (IsHorizontal)
	{
		if ((fabs(refPt.x - seg.hi.x) < MAX_FLT_PRECISION) || 
			(fabs(refPt.x - seg.lo.x) < MAX_FLT_PRECISION)) return refPt.x;
		if (refPt.x < seg.hi.x) return seg.hi.x; // should never happen
		if (seg.lo.x < refPt.x) return seg.lo.x;
		return refPt.x;
	}
	return dX / dY * (refPt.y - seg.lo.y) + seg.lo.x;
}

void Triangulation::setMatches(const pTriSegListType & psl)
{
	if (psl == NULL) return;
	pTriSegListType lr = psl, lrLft = NULL, lrRgh = NULL;
	while (lr != NULL)
	{
		lr->match = NULL;
		lr = lr->next;
	}
	lr = psl;
	while (lr != NULL)
	{
		if (lr->match == NULL)
		{
			lrLft = lr;
			pTriFaceType pf = lr->seg->hEdge->IncidentFace;
			lrRgh = lrLft->next;
			while (lrRgh->seg->hEdge->IncidentFace != pf) lrRgh = lrRgh->next;
			lrLft->match = lrRgh;
			lrRgh->match = lrLft;
		}
		lr = lr->next;
	}
}

void Triangulation::clearLeftMatches(const pTriSegListType & psl)
{
	if (psl == NULL) return;
	pTriSegListType lr = psl, lrMtch = NULL;
	while (lr != NULL)
	{
		lrMtch = lr->match;
		if (lrMtch != NULL)
		{
			double lrx = XofY(*lr->seg), mtchx = XofY(*lrMtch->seg);
			if (lrx < mtchx) lr->match = NULL;
		}
		lr = lr->next;
	}

}

pTriSegListType Triangulation::getLeftMatch(const pTriSegListType & lrNxt)
{
	pTriSegListType lr = lrNxt, lrMtch = NULL;
	while (lr != NULL)
	{
		lrMtch = lr->match;
		double x = XofY(*lrMtch->seg);
		if (x < refPt.x) return lr;
		lr = lr->next;
	}
	return lr;
}

void Triangulation::doInnerComponents(CompGeo::AVL<TriPointType> & EventQ, const pTriSegType & segs)
{
	//EventQ.checkForDuplicates = true;
	for (unsigned int i = 0; i < tLst.NumVertices; ++i)
	{// listing the segments with their hi points in the EventQ - 2 max
		//pTriSegType ps = &segs[i];
		pTriSegListType psl = new TriSegListType,
			pslIn = NULL;
		psl->seg = new TriSegType(segs[i]);
		psl->next = NULL;
		TriPointType hiSegPt;
		hiSegPt.point = segs[i].hi;
		bool iHorizontal = (fabs(segs[i].hi.y - segs[i].lo.y) < MAX_FLT_PRECISION);
		CompGeo::AVLNode<TriPointType> * pAVL_N = EventQ.Find(&hiSegPt);
		pTriPointType pPt = pAVL_N->Data;
		// there can be 0, 1 or 2 segments with a point
		unsigned int s_in = 0;
		pslIn = pPt->segs;
		while (pslIn != NULL)
		{
			++s_in;
			pslIn = pslIn->next;
		}
		pslIn = pPt->segs;
		if (pslIn == NULL) pPt->segs = psl;
		else
		{
			pslIn = pPt->segs;
			bool qHorizontal = (fabs(pslIn->seg->hi.y - pslIn->seg->lo.y) < MAX_FLT_PRECISION);
			if (iHorizontal || qHorizontal)
			{// horizontal segments go on right
				if (qHorizontal)
				{
					psl->next = pPt->segs;
					pPt->segs = psl;
				}
				if (iHorizontal) pPt->segs->next = psl;
			}
			else
			{
				if (psl->seg->lo.x < pslIn->seg->lo.x)
				{
					psl->next = pPt->segs;
					pPt->segs = psl;
				}
				else pPt->segs->next = psl;
			}
		}
		/*
		// debug stuff:
		char txtBuff[256];
		++s_in;
		pslIn = pAVL_N->Data->segs;
		HRESULT b_p = StringCbPrintf(txtBuff, 256 * sizeof(char),
			TEXT("%u segs in Q with point (%8.3f,%8.3f)\n"),
			s_in, pAVL_N->Data->point.x, pAVL_N->Data->point.y);
		OutputDebugString(txtBuff);
		for (unsigned int j = 0; j < s_in; ++j){
		HRESULT b_p = StringCbPrintf(txtBuff, 256 * sizeof(char),
			TEXT("\tsegment %u: lo(%8.3f,%8.3f) hi(%8.3f,%8.3f)\n"),
			j + 1, pslIn->seg->lo->point.x, pslIn->seg->lo->point.y, 
			pslIn->seg->hi->point.x, pslIn->seg->hi->point.y);
		OutputDebugString(txtBuff);
		pslIn = pslIn->next;}
		// end debug stuff
		*/
	}
	pTriSegListType LToR = NULL,  // all segments left to right level with point Pt
		LRTrail = NULL, LRNext = NULL, pLR = NULL, sLR = NULL, dLR = NULL;
	bool fEmpty = false;
	TriPointType Pt = EventQ.GetLeast(fEmpty);
	while (!fEmpty)
	{
		refPt = Pt.point;
		unsigned int s_count = 0;
		pTriSegListType psl = Pt.segs, npslONE = NULL, npslTWO = NULL;
		if (psl != NULL)
		{
			s_count = 1;
			npslONE = new TriSegListType;
			npslONE->next = NULL;
			npslONE->seg = new TriSegType(*psl->seg);
			if (psl->next != NULL)
			{
				s_count = 2;
				npslTWO = new TriSegListType;
				npslTWO->next = NULL;
				npslTWO->seg = new TriSegType(*psl->next->seg);
				npslONE->next = npslTWO;
			}
		}
		LRTrail = NULL;
		LRNext = NULL;
		pLR = LToR;
		while (pLR != NULL)
		{
			double x = XofY(*pLR->seg);
			bool isEqual = (fabs(x - refPt.x) < MAX_FLT_PRECISION);
			if (!isEqual)
			{
				if (x < refPt.x) LRTrail = pLR;
				else
				{
					if (refPt.x < x) 
					{
						LRNext = pLR;
						break;
					}
				}
			}
			pLR = pLR->next;
		}
		if (LRTrail == NULL)
		{
			if (npslONE != NULL)
			{
				if (npslTWO != NULL) npslTWO->next = LRNext;
				else npslONE->next = LRNext;
				LToR = npslONE;
			}
		}
		else
		{
			if ((LRNext == NULL) && (npslONE != NULL)) LRTrail->next = npslONE;
			else
			{// LRTrail & LRNext are not NULL and not equal
				dLR = LRTrail->next;
				while (dLR != LRNext)
				{
					sLR = dLR->next;
					delete dLR->seg;
					delete dLR;
					dLR = sLR;
				}
				if (npslONE != NULL)
				{
					LRTrail->next = npslONE;
					if (npslTWO != NULL) npslTWO->next = LRNext;
					else npslONE->next = LRNext;
				}
				else LRTrail->next = LRNext;
			}
		}
		// possibly moving a face:
		if ((s_count == 2) && (LRTrail != NULL) && (LRNext != NULL))
		{// if trail or next is NULL face is not within another
			setMatches(LToR);
			pLR = getLeftMatch(LRNext);
			if (pLR != NULL)
			{// definitely moving a face if it hasn't already been moved:
				clearLeftMatches(LToR);
				pTriFaceType pf0 = tLst.Faces, pfIns = npslONE->seg->hEdge->IncidentFace;
				pTriHalfEdgeListType pHEL = pf0->InnerComponents, pHELTrail = NULL, pHELTraverse = NULL;
				while (pHEL->OneEdge->Twin->IncidentFace != pfIns)
				{// this is where all the faces are initially
					pHELTrail = pHEL;
					pHEL = pHEL->next;
					if (pHEL == NULL) break; // already taken care of
				}
				if (pHEL != NULL)
				{// not already taken care of:
					if (pHELTrail == NULL) pf0->InnerComponents = pHEL->next;
					else pHELTrail->next = pHEL->next;
					pHEL->next = NULL;
					pLR = LToR;
					while (pLR->seg->hEdge->IncidentFace != pfIns)
					{
						if (pLR->match != NULL) pf0 = pLR->seg->hEdge->Twin->IncidentFace;
						else pf0 = pLR->seg->hEdge->IncidentFace;
						pLR = pLR->next;
					}
					pHELTraverse = pf0->InnerComponents;
					pHELTrail = NULL;
					while (pHELTraverse != NULL)
					{
						pHELTrail = pHELTraverse;
						pHELTraverse = pHELTraverse->next;
					}
					if (pHELTrail == NULL) pf0->InnerComponents = pHEL;
					else pHELTrail->next = pHEL;
					pTriHalfEdgeType pHE = pfIns->OuterComponent->Twin;
					for (unsigned int i = 0; i < pfIns->NumEdges; ++i) 
					{
						pHE->IncidentFace = pf0;
						pHE->Polygon = pf0->Face;
						pHE = pHE->Next;
					}
				} // end of not already taken care of
			} // end of definite face move
		} // end of possible face move
		Pt = EventQ.GetLeast(fEmpty);
	}

}

void Triangulation::setFaceType(const char ft, pTriFaceType pf)
{
	if (pf == NULL) return;
	pf->FaceType = ft;
	char nft = 'H';
	if (ft == 'H') nft = 'F';
	pTriHalfEdgeListType pHEL = pf->InnerComponents;
	while (pHEL != NULL)
	{
		setFaceType(nft, pHEL->OneEdge->Twin->IncidentFace);
		pHEL = pHEL->next;
	}
}

void Triangulation::setTurnTypeForEdge(pTriHalfEdgeType ph)
{
	pTriVertexType vp = NULL, v = NULL, vn = NULL;
	TriXY A, B, xy, pxy, nxy;
	v = ph->Origin;
	vp = ph->Prev->Origin;
	vn = ph->Next->Origin;
	xy = v->Coordinates;
	pxy = vp->Coordinates;
	nxy = vn->Coordinates;
	A = nxy;
	A -= pxy;
	B = xy;
	B -= pxy;
	double k = CompGeo::Cross(A, B);
	bool nLess, pLess;
	if (fabs(nxy.y - xy.y) < MAX_FLT_PRECISION) nLess = (nxy.x > xy.x);
	else nLess = (nxy.y < xy.y);
	if (fabs(pxy.y - xy.y) < MAX_FLT_PRECISION) pLess = (pxy.x > xy.x);
	else pLess = (pxy.y < xy.y);
	if (nLess != pLess) v->TurnType = 'R';
	else
	{
		if (nLess)
		{
			if (k < 0.0) v->TurnType = 'B';
			else v->TurnType = 'S';
		}
		else
		{
			if (k < 0.0) v->TurnType = 'E';
			else v->TurnType = 'M';
		}
	}

}

void Triangulation::setTurnType(pTriFaceType pf)
{
	if (pf == NULL) return;
	//tPhase = 1;
	char ft = pf->FaceType;
	if (ft == 'F')
	{
		pTriHalfEdgeType ph = pf->OuterComponent;
		for (unsigned int i = 0; i < pf->NumEdges; ++i)
		{
			setTurnTypeForEdge(ph);
			ph = ph->Next;
		}
		pTriHalfEdgeListType phel = pf->InnerComponents;
		while (phel != NULL)
		{
			ph = phel->OneEdge;
			pTriFaceType pfIC = ph->Twin->IncidentFace;
			unsigned int nE = pfIC->NumEdges;
			for (unsigned int i = 0; i < nE; ++i)
			{
				setTurnTypeForEdge(ph);
				ph = ph->Next;
			}
			setTurnType(pfIC); // check hole
			phel = phel->next;
		}
	}
	else // ft is 'H' for hole
	{
		assert (ft == 'H');
		pTriHalfEdgeListType phel = pf->InnerComponents;
		while (phel != NULL)
		{
			pTriHalfEdgeType ph = phel->OneEdge;
			pTriFaceType pfIC = ph->Twin->IncidentFace;
			setTurnType(pfIC);
			phel = phel->next;
		}

	}
}

void Triangulation::AddNewHalf(pTriHalfEdgeType e_n, pTriSegListType & halfsnew)
{// use pointer order
	pTriSegListType psl = halfsnew, pslTrail = NULL, 
		psn = new TriSegListType;
	psn->match = NULL;
	psn->next = NULL;
	psn->seg = new TriSegType(e_n);
	//TriXY HI = psn->seg->hi;
	while (psl != NULL)
	{
		if (e_n < psl->seg->hEdge)
		{
			if (pslTrail == NULL)
			{
				psn->next = halfsnew;
				halfsnew = psn;
			}
			else
			{
				psn->next = psl;
				pslTrail->next = psn;
			}
			return;
		}
		pslTrail = psl;
		psl = psl->next;
	}
	if (pslTrail == NULL) halfsnew = psn;
	else pslTrail->next = psn;
}

void Triangulation::DeleteNewHalf(pTriHalfEdgeType ph, pTriSegListType & halfsnew)
{// in pointer order
	pTriSegListType psl = halfsnew, pslTrail = NULL;
	TriSegType s(ph);
	//TriXY HI = s.hi;

	while (psl != NULL)
	{
		if (psl->seg->hEdge == ph)
		{
			if (pslTrail == NULL) halfsnew = halfsnew->next;
			else pslTrail->next = psl->next;
			delete psl->seg;
			delete psl;
			return;
		}
		assert(ph > psl->seg->hEdge);  // not in list
		pslTrail = psl;
		psl = psl->next;
	}
}

vector<pTriHalfEdgeType> Triangulation::getHalfEdgesAtOrigin(pTriHalfEdgeType h0)
{
	vector<pTriHalfEdgeType> vH;
	pTriHalfEdgeType h = h0;
	bool goodFace = false;

	do
	{
		h = h->Prev->Twin;
		goodFace = h->IncidentFace == NULL;
		if (!goodFace) goodFace = h->IncidentFace->FaceType == 'F';
		if (goodFace) vH.push_back(h);
	} while (h != h0);
	
	return vH;
}
	
vector<pTriHalfEdgeType> Triangulation::getHalfEdgesEndingAtOrigin(pTriHalfEdgeType h0)
{
	vector<pTriHalfEdgeType> vH;
	pTriHalfEdgeType h = h0;
	bool goodFace = false;

	do
	{
		h = h->Prev->Twin;
		goodFace = h->IncidentFace == NULL;
		if (!goodFace) goodFace = h->IncidentFace->FaceType == 'F';
		if (goodFace) vH.push_back(h->Prev);
	} while (h != h0);
	
	return vH;

}
		
pTriHalfEdgeType Triangulation::getLeftMostOriginating(const pTriHalfEdgeType & refHE, const vector<pTriHalfEdgeType> & vH)
{
	TriXY V1 = refHE->Origin->Coordinates, V2 = refHE->Twin->Origin->Coordinates;
	pTriHalfEdgeType heLeftMost = NULL;
	CompGeo::UnitCircleMeasureType max;

	for (vector<pTriHalfEdgeType>::const_iterator it = vH.begin(); it != vH.end(); ++it)
	{
		pTriHalfEdgeType he = *it;
		TriXY A = he->Twin->Origin->Coordinates;
		CompGeo::UnitCircleMeasureType ucp(V2, V1, A);

		if (heLeftMost == NULL)
		{
			max = ucp;
			heLeftMost = he;
		}
		else
		{
			if (ucp > max)
			{
				max = ucp;
				heLeftMost = he;
			}
		}
		
	}
	return heLeftMost;


}

pTriHalfEdgeType Triangulation::getLeftMostEnding(const pTriHalfEdgeType & refHE, const vector<pTriHalfEdgeType> & vH)
{
	TriXY V1 = refHE->Origin->Coordinates, V2 = refHE->Twin->Origin->Coordinates;
	CompGeo::UnitCircleMeasureType min;
	pTriHalfEdgeType heLeftMost = NULL;

	for (vector<pTriHalfEdgeType>::const_iterator it = vH.begin(); it != vH.end(); ++it)
	{
		pTriHalfEdgeType he = *it;
		TriXY A = he->Origin->Coordinates;
		CompGeo::UnitCircleMeasureType ucp(V2, V1, A);
		if (heLeftMost == NULL)
		{
			min = ucp;
			heLeftMost = he;
		}
		else
		{
			if (ucp < min)
			{
				min = ucp;
				heLeftMost = he;
			}
		}
		
	}
	return heLeftMost;
}

void Triangulation::AddEdge(pTriVertexType v_l, pTriVertexType v_h, pTriSegListType & halfsnew)
{
	pTriHalfEdgeType h_l = v_l->IncidentEdge, h_h = v_h->IncidentEdge,
		r = new TriHalfEdgeType, l = new TriHalfEdgeType;
	
	memcpy(l, h_l, sizeof(TriHalfEdgeType));
	memcpy(r, h_h, sizeof(TriHalfEdgeType));
	l->Half = ++nxtNewEdge;
	r->Half = ++nxtNewEdge;
	r->IncidentFace = NULL; l->IncidentFace = NULL;
	r->Twin = l; l->Twin = r;
	r->Next = NULL; l->Next = NULL;
	r->Prev = NULL;	l->Prev = NULL;

	l->Next = getLeftMostOriginating(r, getHalfEdgesAtOrigin(h_h));
	r->Next = getLeftMostOriginating(l, getHalfEdgesAtOrigin(h_l));
	l->Prev = getLeftMostEnding(l, getHalfEdgesEndingAtOrigin(h_l));
	r->Prev = getLeftMostEnding(r, getHalfEdgesEndingAtOrigin(h_h));

	l->Next->Prev = l;
	r->Next->Prev = r;
	l->Prev->Next = l;
	r->Prev->Next = r;

	AddNewHalf(r, halfsnew);
	AddNewHalf(l, halfsnew);
}

/*
void Triangulation::AddEdge(pTriVertexType v_i, pTriVertexType v_h, pTriSegListType & halfsnew)
{
	pTriHalfEdgeType e_x = v_h->IncidentEdge, e_i = v_i->IncidentEdge,
		e_s = NULL, e_p = NULL, e_n = NULL, e_h = NULL, e_x0 = e_x, 
		e2 = new TriHalfEdgeType,
		e3 = new TriHalfEdgeType;
	unsigned int sCount = 0, nCount = 0, max_n = 0;
	char tth = v_h->TurnType, tti = v_i->TurnType;
	bool inMonotone = ((tth == 'M') || (tti == 'S'));
	bool iOnRight, hOnRight, hOnTop = (tth == 'B'), iOnBottom = (tti == 'E'), fWithFlow, fAgainstFlow;
	e_s = e_x;
	if (tth == 'S')
	{
		// do not update 'S' type vertices' edges
		pTriFaceType f = e_s->IncidentFace;
		do
		{
			if (e_x->IncidentFace == NULL) 
			{
				++nCount;
				if (e_x->Half > max_n)
				{ // last NULL edge added
					max_n = e_x->Half;
					e_n = e_x;
				}
			}
			if (e_x->Prev->IncidentFace == f) e_p = e_x->Prev;
			e_x = e_x->Prev->Twin;

		} while (e_x != e_x0);
		e_h = e_s;
		if (e_n != NULL)
		{
			TriSegType n_p = e_p, n_n = e_s, s_i = e_i;
			refPt = e_s->Origin->Coordinates;
			refPt.y -= 1000000.0;
			bool pHorizontal = (fabs(e_s->Origin->Coordinates.y - 
				e_p->Origin->Coordinates.y) < MAX_FLT_PRECISION),
				iHorizontal = (fabs(e_s->Origin->Coordinates.y - 
				e_s->Twin->Origin->Coordinates.y) < MAX_FLT_PRECISION),
				uCase = (n_p < n_n);
			if (iHorizontal) 
			{ // both neighbors are lower than a split vertex
			  // the horizontal must go to right for the neighbor vertex that way to be less than the split
				hOnRight = true;
				iOnRight = false; // flags position not chain; considering line of sight cannot be on right
			}
			else
			{
				if (pHorizontal)
				{
					hOnRight = false;
					iOnRight = false;
				}
				else
				{
					hOnRight = uCase;
					// resetting s_i to segment from e_i up to split vertex:
					s_i.lo = e_i->Origin->Coordinates;
					s_i.hi = e_s->Origin->Coordinates;
					// getting highest refPt:
					refPt = n_p.lo;
					if (n_n.lo.y > refPt.y) refPt = n_n.lo;
					if (s_i.lo.y > refPt.y) refPt = s_i.lo;
					iOnRight = (n_p < s_i);  // s_i is either greater or lesser than both n_n & n_p
				}
			}
			if ((!iOnRight && hOnRight) || (iOnRight && !hOnRight)) e_h = e_n;
		}
	}
	else
	{
		// right chain: next vertex above this one; left: below
		e_h = e_s;
		iOnRight = (e_i->Next->Origin->Coordinates < v_i->Coordinates);
		hOnRight = (e_s->Next->Origin->Coordinates < v_h->Coordinates);
		if (tti == 'M')
		{
			fWithFlow = true;
			fAgainstFlow = false;
		}
		else
		{
			fWithFlow = (iOnRight || (iOnBottom && !hOnRight));
			fAgainstFlow = (!iOnRight || (iOnBottom && hOnRight)); 
		}
	}
	memcpy(e2, e_i, sizeof(TriHalfEdgeType));
	e2->Half = ++nxtNewEdge;
	e2->Next = e_h;
	e2->IncidentFace = NULL;

	memcpy(e3, e_h, sizeof(TriHalfEdgeType));
	e3->Half = ++nxtNewEdge;
	e3->Next = e_i;
	e3->IncidentFace = NULL;

	e2->Twin = e3;
	e3->Twin = e2;

	e_i->Prev->Next = e2;
	e_h->Prev->Next = e3;

	e_i->Prev = e3;
	e_h->Prev = e2;

	AddNewHalf(e2, halfsnew);
	AddNewHalf(e3, halfsnew);
	// updating IncidentEdge
	if ((tth != 'S') && (tti != 'S'))
	{
		if (fWithFlow)
		{
			if (tti != 'E') v_i->IncidentEdge = e2;
		}
		if (fAgainstFlow)
		{
			if (tti == 'E') v_i->IncidentEdge = e2;
			v_h->IncidentEdge = e3;
		}
	}

}
*/
void Triangulation::MakeFaces(pTriFaceType pf0, pTriFaceType pf, pTriSegListType & halfsnew)
{// pf is the face before the make monotone algorithm implementation
 // it will probably have been altered by the procedure

	nxtNewEdge = 0;
	if (halfsnew == NULL) return;
	pTriHalfEdgeType ph0 = pf->OuterComponent, ph = ph0;
	unsigned int eCount = 0, fIdx = pf->Face;
	do
	{
		++eCount;
		if (ph->IncidentFace == NULL)
		{
			ph->IncidentFace = pf;
			ph->Polygon = fIdx;
			DeleteNewHalf(ph, halfsnew);
		}
		ph = ph->Next;
	} while (ph != ph0);
	pf->NumEdges = eCount;
	while (halfsnew != NULL)
	{
		pTriFaceType pfn = new TriFaceType;
		pfn->Face = tLst.NumFaces++;
		fIdx = pfn->Face;
		pfn->FaceType = 'F';  // filled in
		pfn->InnerComponents = NULL;
		pfn->NumEdges = 0;
		pfn->OuterComponent = NULL;
		eCount = 0;
		ph0 = halfsnew->seg->hEdge;
		ph = ph0;
		do
		{
			++eCount;
			if ((pfn->OuterComponent == NULL) && (ph->Twin->IncidentFace == pf0))
				pfn->OuterComponent = ph;
			if (ph->IncidentFace == NULL) DeleteNewHalf(ph, halfsnew);
			ph->IncidentFace = pfn;
			ph->Polygon = fIdx;
			ph = ph->Next;
		} while (ph != ph0);
		if (pfn->OuterComponent == NULL) pfn->OuterComponent = ph0;
		pfn->NumEdges = eCount;
		pTriHalfEdgeListType pHEL = new TriHalfEdgeListType;
		pHEL->next = pf0->InnerComponents;
		pHEL->OneEdge = pfn->OuterComponent->Twin;
		pf0->InnerComponents = pHEL;
	}
}

pTriSegType Triangulation::FindLeft(TriXY A, CompGeo::AVL<TriSegType> & T)
{
	refPt = A;
	TriSegType dummy;
	dummy.hi.x = A.x;
	dummy.hi.y = 1.0;
	dummy.lo.x = A.x;
	dummy.lo.y = -1.0;
	T.FindLeaf(&dummy);
	pTriSegType left = NULL;
	unsigned int i = 0;
	for (i = T.pathTop; i > 0; --i) if (T.sPath[i].direction == 'r') break;
	if (i > 0) left = T.sPath[i].pNode->Data;
	assert (left != NULL);
	return left;
}

void Triangulation::removeStatusEdge(pTriHalfEdgeType e, CompGeo::AVL<TriSegType> & T)
{// remove segment associated with e from T
 // this depends on refPt
	TriSegType finder(e);
	refPt = finder.lo;
	CompGeo::AVLNode<TriSegType> * a_n = T.Find(&finder);
	assert(a_n != NULL);
	T.Delete();

}

void Triangulation::setForTriangulation(pTriFaceType pf) // call this with tLst.Face
{
	if (pf->FaceType != 'H') return;
	pTriHalfEdgeListType pHEL = pf->InnerComponents;
	while (pHEL != NULL)
	{
		addTriangles(pf, pHEL);
		pHEL = pHEL->next;
	}
}

void Triangulation::addTriangles(pTriFaceType pf0, pTriHalfEdgeListType pHEL)
{
	pTriHalfEdgeType ph0 = pHEL->OneEdge->Twin, ph = ph0, phG = ph, phL = ph;
	pTriFaceType pf = ph0->IncidentFace;
	pTriHalfEdgeListType pHELin = pf->InnerComponents;
	// initializing the TriPoint Array:
	unsigned int N = pf->NumEdges;
	pTriPointType p_a = new TriPointType[N];
	for (unsigned int i = 0; i < N; ++i)
	{
		pTriPointType pp = &p_a[i];//(pTriPointType)new TriPointType));
		pp->point = ph->Origin->Coordinates;
		pTriSegListType psl = new TriSegListType;
		pp->segs = psl;
		psl->next = NULL;
		psl->match = NULL;
		psl->seg = new TriSegType(ph);
		if (ph->Origin->Coordinates < phG->Origin->Coordinates) phG = ph;
		if (phL->Origin->Coordinates < ph->Origin->Coordinates) phL = ph;
		ph->Origin->IncidentEdge = ph;
		ph = ph->Next;
	}
	ph = phG;
	for (unsigned int i = 0; i < N; ++i)
	{// numbering Edge member from top edge 0 - N-1 counterclockwise
	 // phL->Edge (i.e. L) is bottom, so 1 - L-1 is left & L+1 - N-1 is right 
		ph->Edge = i;
		ph->Origin->TurnType = 'R';
		ph = ph->Next;
	}
	unsigned int L = phL->Edge;
	phL->Origin->TurnType = 'E';
	phG->Origin->TurnType = 'B';
	CompGeo::Sorter<TriPointType> srt(p_a, 0, N - 1, N);
	srt.doSort();
	srt.~Sorter();
	TriStack s;
	s.push(&p_a[0]);
	s.push(&p_a[1]);
	pTriSegListType newhalfs = NULL;
	pTriVertexType v_j, v_p;
	for (unsigned int j = 2; j < (N - 1); ++j)
	{
		pTriPointType sTop = s.peek(), peekN = NULL, popN = NULL;
		bool u_jLeft = (p_a[j].segs->seg->hEdge->Edge < L),
			s_tLeft = (sTop->segs->seg->hEdge->Edge < L);
		if (u_jLeft != s_tLeft)
		{ // different chains
			pTriHalfEdgeType phInit = NULL;
			v_j = p_a[j].segs->seg->hEdge->Origin;
			do
			{
				popN = s.pop();
				if (s.peek() != NULL)
				{ // don't add edge for last
					v_p = popN->segs->seg->hEdge->Origin;
					AddEdge(v_j, v_p, newhalfs);
					if (u_jLeft)
					{
						if (phInit == NULL) phInit = v_j->IncidentEdge;
						while (v_j->IncidentEdge->Prev->IncidentFace == NULL)
							v_j->IncidentEdge = v_j->IncidentEdge->Prev->Twin;
					}
				}
			} while (s.peek() != NULL);
			s.push(&p_a[j - 1]);
			s.push(&p_a[j]);
			if (phInit != NULL) v_j->IncidentEdge = phInit;
		}
		else
		{ //same chain
			popN = s.pop();
			TriXY A = popN->point;
			A -= p_a[j].point;
			peekN = s.peek();
			TriXY B = peekN->point;
			B -= p_a[j].point;
			double k = CompGeo::Cross(A, B);
			bool fIsZero = (fabs(k) < MAX_FLT_PRECISION), 
				fIsIn = !fIsZero && ((u_jLeft && (k < 0.0)) || (!u_jLeft && (k > 0.0)));
			while (fIsIn)
			{
				popN = s.pop();
				v_j = p_a[j].segs->seg->hEdge->Origin;
				v_p = popN->segs->seg->hEdge->Origin;
				AddEdge(v_j, v_p, newhalfs);
				A = popN->point;
				A -= p_a[j].point;
				peekN = s.peek();
				if (peekN == NULL) break;
				B = peekN->point;
				B -= p_a[j].point;
				k = CompGeo::Cross(A, B);
				fIsZero = (fabs(k) < MAX_FLT_PRECISION), 
					fIsIn = !fIsZero && ((u_jLeft && (k < 0.0)) || (!u_jLeft && (k > 0.0)));
			}
			s.push(popN);
			s.push(&p_a[j]);
		}
	}
	// add diagonals from lowest vertex to all stack vertices except 1st & last
	pTriPointType popN = NULL;
	popN = s.pop();
	bool popOnLeft = (popN->segs->seg->hEdge->Edge < L);
	v_j = phL->Origin;
	while (s.peek() != NULL)
	{
		popN = s.pop();
		if (s.peek() != NULL)
		{
			v_p = popN->segs->seg->hEdge->Origin;
			AddEdge(v_j, v_p, newhalfs);
			if (!popOnLeft)
				while (v_j->IncidentEdge->Prev->IncidentFace == NULL)
					v_j->IncidentEdge = v_j->IncidentEdge->Prev->Twin;
		}
	}
	delete [] p_a;
	s.~TriStack();
	// doubly connected edge list stuff
	MakeFaces(pf0, pf, newhalfs);
	while (pHELin != NULL)
	{
		setForTriangulation(pHELin->OneEdge->Twin->IncidentFace);
		pHELin = pHELin->next;
	}
}



void Triangulation::setForMonotone(pTriFaceType pf0)
{ // call this with tLst.Faces and recursion will do the rest
	if (pf0->FaceType != 'H') return;
	pTriHalfEdgeListType pHEL = pf0->InnerComponents;
	while (pHEL != NULL)
	{
		makeMonotone(pf0, pHEL);
		pHEL = pHEL->next;
	}
}

void Triangulation::makeMonotone(pTriFaceType pf0, pTriHalfEdgeListType pHEL)
{
	pTriHalfEdgeType ph0 = pHEL->OneEdge->Twin, ph = ph0;
	pTriFaceType pf = ph0->IncidentFace;
	pTriHalfEdgeListType pHELin = pf->InnerComponents;
	// initializing the event queue:
	CompGeo::AVL<TriPointType> EventQ;
	do
	{
		for (unsigned int i = 0; i < pf->NumEdges; ++i)
		{
			pTriPointType pp = new TriPointType;
			pp->point = ph->Origin->Coordinates;
			pTriSegListType psl = new TriSegListType;
			pp->segs = psl;
			psl->next = NULL;
			psl->match = NULL;
			psl->seg = new TriSegType(ph);
			EventQ.AlwaysInsert(pp);
			ph->Helper = NULL;
			ph->Origin->IncidentEdge = ph;
			ph = ph->Next;
		}
		if (pHELin == NULL) ph = NULL;
		else
		{ // this will add the vertices around the holes of the face
			ph = pHELin->OneEdge;
			pf = ph->Twin->IncidentFace; // just for number of edges
			pHELin = pHELin->next;
		}
	} while (ph != NULL);
	bool fIsEmpty = false;
	TriPointType Pt = EventQ.GetLeast(fIsEmpty);
	CompGeo::AVL<TriSegType> S_T;  // status tree depends on refPt
	pTriSegListType newhalfs = NULL;
	while (!fIsEmpty)
	{
		pTriHalfEdgeType e_i = Pt.segs->seg->hEdge;
		char tt = e_i->Origin->TurnType;
		switch (tt)
		{
		case 'B':
			handleStartVertex(e_i, S_T);
			break;
		case 'E':
			handleEndVertex(e_i, S_T, newhalfs);
			break;
		case 'R':
			handleRegularVertex(e_i, S_T, newhalfs);
			break;
		case 'S':
			handleSplitVertex(e_i, S_T, newhalfs);
			break;
		case 'M':
			handleMergeVertex(e_i, S_T, newhalfs);
			break;
		default:
			cout << "Fatal Error in makeMonotone\n";
			exit (EXIT_FAILURE);
		}
		Pt = EventQ.GetLeast(fIsEmpty);
	}
	pf = ph0->IncidentFace;
	pHELin = pf->InnerComponents;
	MakeFaces(pf0, pf, newhalfs);
	while (pHELin != NULL)
	{
		setForMonotone(pHELin->OneEdge->Twin->IncidentFace);
		pHELin = pHELin->next;
	}
}

void Triangulation::handleStartVertex(pTriHalfEdgeType e_i, CompGeo::AVL<TriSegType> & T)
{
	pTriVertexType v_i = e_i->Origin;
	refPt = v_i->Coordinates;
	T.AlwaysInsert(new TriSegType(e_i));
	e_i->Helper = v_i;
}

void Triangulation::handleEndVertex(pTriHalfEdgeType e_i, CompGeo::AVL<TriSegType> & T, 
	pTriSegListType & newhalfs)
{
	pTriHalfEdgeType e_p = e_i->Prev;
	pTriVertexType v_h = e_p->Helper, v_i = e_i->Origin;
	if (v_h->TurnType == 'M') AddEdge(v_i, v_h, newhalfs);
	removeStatusEdge(e_p, T);
}

void Triangulation::handleRegularVertex(pTriHalfEdgeType e_i, 
	CompGeo::AVL<TriSegType> & T, pTriSegListType & newhalfs)
{
	pTriHalfEdgeType e_p = e_i->Prev;
	pTriVertexType v_i = e_i->Origin;
	TriXY p_i = v_i->Coordinates, p_p = e_p->Origin->Coordinates;
	bool fHorizontal = (fabs(p_i.y - p_p.y) < MAX_FLT_PRECISION),
		fXLess = (p_p.x < p_i.x), fYMore = (p_p.y > p_i.y),
		fDescending = ((fHorizontal && fXLess) || (!fHorizontal && fYMore)),
		fFaceToRight = fDescending;
	if (fFaceToRight)
	{
		pTriVertexType v_h = e_p->Helper;
		if (v_h->TurnType == 'M') AddEdge(v_i, v_h, newhalfs);
		removeStatusEdge(e_p, T);
		e_i->Helper = e_i->Origin;
		refPt = p_i;
		T.AlwaysInsert(new TriSegType(e_i));
	}
	else
	{
		pTriSegType pLft = FindLeft(p_i, T);
		pTriHalfEdgeType e_j = pLft->hEdge;
		pTriVertexType v_h = e_j->Helper;
		if (v_h->TurnType == 'M') AddEdge(v_i, v_h, newhalfs);
		e_j->Helper = e_i->Origin;
	}
}

void Triangulation::handleSplitVertex(pTriHalfEdgeType e_i, CompGeo::AVL<TriSegType> & T, 
	pTriSegListType & newhalfs)
{
	pTriVertexType v_i = e_i->Origin;
	pTriSegType pLft = FindLeft(v_i->Coordinates, T);
	pTriHalfEdgeType e_j = pLft->hEdge;
	AddEdge(v_i, e_j->Helper, newhalfs);
	e_j->Helper = v_i;
	e_i->Helper = v_i;
	refPt = v_i->Coordinates;
	T.AlwaysInsert(new TriSegType(e_i));
}

void Triangulation::handleMergeVertex(pTriHalfEdgeType e_i, CompGeo::AVL<TriSegType> & T, 
	pTriSegListType & newhalfs)
{
	pTriHalfEdgeType e_p = e_i->Prev;
	pTriVertexType v_h = e_p->Helper, v_i = e_i->Origin;
	if (v_h->TurnType == 'M') AddEdge(v_i, v_h, newhalfs);
	removeStatusEdge(e_p, T);

	pTriSegType pLft = FindLeft(v_i->Coordinates, T);
	pTriHalfEdgeType e_j = pLft->hEdge;
	v_h = e_j->Helper;
	if (v_h->TurnType == 'M') AddEdge(v_i, v_h, newhalfs);
	e_j->Helper = v_i;

}

vector<pPGonWork> Triangulation::translateFace(pTriFaceType pff) // for a fill face
{
	vector<pPGonWork> r, a;
	if (pff == NULL) return r; //NULL;
	if (pff->FaceType != 'F') return r; //NULL;
	pPGonWork wGon = new PGonWork; //MakePGon(), //(pPGonWork)new PGonWork)), 
		//wGonTrail = wGon, wGonR = NULL;
	//wGon->next = NULL;
	wGon->numVertices = pff->NumEdges;
	//wGon->pIdx = 0;
	//wGon->selVertex = -1;
	//wGon->vNode = NULL;
	pNameNode pNameOut = new NameNode; //(pNameNode)new NameNode));
	string nmn = "Tri++";
	//char TriName[] = TEXT("Tri++"), * nmn;
	//size_t l = _tcslen(TriName) + sizeof(char);
	//nmn = new char[l];
	//HRESULT b_p = StringCbPrintf(nmn, l * sizeof(char), TEXT("%s"), TriName);
	
	pNameOut->polyname = nmn;
	wGon->pName = pNameOut;
	pVertexNode pvn = NULL, pvTrail = NULL;
	pTriHalfEdgeType ph = pff->OuterComponent;
	for (unsigned int i = 0; i < pff->NumEdges; ++i)
	{
		pvn = new VertexNode; //MakeVertex(); //(pVertexNode)new VertexNode));
		//pvn->next = NULL;
		//pvn->vtxInfo.is_selected = false;
		pvn->vtxInfo.vertex = ph->Origin->Coordinates;
		//pvn->vtxInfo.vertex.x = (float)ph->Origin->Coordinates.x;
		//pvn->vtxInfo.vertex.y = (float)ph->Origin->Coordinates.y;
		if (pvTrail == NULL) wGon->vNode = pvn;
		else pvTrail->next = pvn;
		pvTrail = pvn;
		/*
		// debug stuff:
		char txtBuff[256];
		HRESULT b_p = StringCbPrintf(txtBuff, 256 * sizeof(char),
			TEXT("Face-FaceT-Edge-Half %u-%u-%u-%u origin: (%8.3f,%8.3f)\n"),
			ph->Polygon, ph->Twin->Polygon, ph->Edge, ph->Half, 
			ph->Origin->Coordinates.x, ph->Origin->Coordinates.y);
		OutputDebugString(txtBuff);
		// end debug stuff
		*/
		ph = ph->Next;
	}
	r.push_back(wGon);
	pTriHalfEdgeListType pHEL = pff->InnerComponents;
	while (pHEL != NULL)
	{
		a = translateFaces(pHEL->OneEdge->Twin->IncidentFace);
		for (vector<pPGonWork>::iterator it = a.begin(); it != a.end(); ++it)
		{
			r.push_back(*it);
		}
		//wGonTrail->next = translateFaces(pHEL->OneEdge->Twin->IncidentFace);
		//wGonR = wGonTrail->next;
		//while (wGonR != NULL)
		//{
		//	wGonTrail = wGonR;
		//	wGonR = wGonR->next;
		//}
		pHEL = pHEL->next;
	}
	return r;
}

vector<pPGonWork> Triangulation::translateFaces(pTriFaceType pfh)
{// will output list of NGons within a hole

	vector<pPGonWork> r, a;
	if (pfh == NULL) return r;
	if (pfh->FaceType != 'H') return r;
	pTriHalfEdgeListType pHEL = pfh->InnerComponents;
	if (pHEL == NULL) return r; //NULL;
	pPGonWork wGon = NULL; //, wGonRoot = NULL, wGonL = NULL, wGonN = NULL;
	while (pHEL != NULL)
	{
		a = translateFace(pHEL->OneEdge->Twin->IncidentFace);
		for (vector<pPGonWork>::iterator it = a.begin(); it != a.end(); ++it)
		{
			r.push_back(*it);
		}

		//if (wGonL == NULL) wGonRoot = wGon;
		//else wGonL->next = wGon;
		//wGonN = wGon;
		//while (wGonN != NULL)
		//{
		//	wGonL = wGonN;
		//	wGonN = wGonN->next;
		//}
		pHEL = pHEL->next;
	}
	return r;
}

vector<unsigned int> Triangulation::indexFace(pTriFaceType pff) // for a fill face
{
	vector<unsigned int> r, a;
	if (pff == NULL) return r; //NULL;
	if (pff->FaceType != 'F') return r; //NULL;
	assert (pff->NumEdges == 3);
	pTriHalfEdgeType ph = pff->OuterComponent;
	for (unsigned int i = 0; i < pff->NumEdges; ++i)
	{
		r.push_back(ph->Origin->Vertex);
		ph = ph->Next;
	}
	pTriHalfEdgeListType pHEL = pff->InnerComponents;
	while (pHEL != NULL)
	{
		a = indexFaces(pHEL->OneEdge->Twin->IncidentFace);
		for (vector<unsigned int>::iterator it = a.begin(); it != a.end(); ++it)
		{
			r.push_back(*it);
		}
		pHEL = pHEL->next;
	}
	return r;

}

vector<unsigned int> Triangulation::indexFaces(pTriFaceType pfh)
{// will output list of triangles by index within a hole

	vector<unsigned int> r, a;
	if (pfh == NULL) return r;
	if (pfh->FaceType != 'H') return r;
	pTriHalfEdgeListType pHEL = pfh->InnerComponents;
	if (pHEL == NULL) return r; //NULL;
	while (pHEL != NULL)
	{
		a = indexFace(pHEL->OneEdge->Twin->IncidentFace);
		for (vector<unsigned int>::iterator it = a.begin(); it != a.end(); ++it)
		{
			r.push_back(*it);
		}
		pHEL = pHEL->next;
	}
	return r;

}

//helper global function to let the linker know that this is the module where these templated calls must go:
void link_my_triangulation(void)
{
	vector<CompGeo::pFaceType> vf;
	CompGeo::DCEL<int> * intDCEL = NULL;
	CompGeo::DCEL<double> * dblDCEL = NULL;
	Triangulation(vf, intDCEL);
	Triangulation(vf, dblDCEL);
}