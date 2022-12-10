#include "first.hpp"
#include "CompGeo.hpp"
#include "NGons.hpp"
#include "Fortune.hpp"
#include "Trapezoid.hpp"


using namespace std;
using namespace Trapezoid;
// static member initialization:
template<typename T> int Tzoid<T>::idIdx = -1;
template<typename T> CompGeo::DCEL<T> * Map<T>::dcel = NULL;
template<typename T> size_t Map<T>::cFaceIdx = 0;

template<typename T> PointType<T>::PointType(void)
{
	ID = -1;
	suffix = 'X';
	pt = NULL;
}

template<typename T> PointType<T>::PointType(const PointType<T> & a)
{

	ID = a.ID;
	suffix = a.suffix;
	pt = a.pt;
}

template<typename T> PointType<T>::PointType(pEdgeType<T> e, char RorL)
{
	CompGeo::DCEL<T> * d = Map<T>::dcel;
	ID = e->ID;
	suffix = RorL;
	assert ((RorL == 'L') || (RorL == 'R'));
	if (RorL == 'L') pt = d->origin(e->leftMost);
	else pt = d->origin(d->twin(e->leftMost));
}

template<typename T> PointType<T>::~PointType(void)
{
}

template<typename T> string PointType<T>::PrintID(const int szBuff)
{
	int count = -1, id = ID, d26 = 0, cMod = 0;
	string r = "";
	if (szBuff <= 2) return r;

	if (id == -2)
	{
		r = "_"; return r;
	}

	if (id == -3)
	{
		r = "^"; return r;
	}

	do
	{
		if ((count + 2) >= szBuff)
		{
			return r;
		}
		cMod = id % 26;
		r += (char)(cMod + (int)'a');
		id /= 26;
	} while (id > 0);
	return r;
}

template<typename T> EdgeType<T>::EdgeType(void)
{
	ID = 0;
	leftMost = NULL;
	lPt = NULL;
	rPt = NULL;
}

template<typename T> EdgeType<T>::EdgeType(const EdgeType<T> & a)
{
	ID = a.ID;
	leftMost = a.leftMost;
	lPt = new PointType<T>(*a.lPt);
	rPt = new PointType<T>(*a.rPt);
}

template<typename T> EdgeType<T>::EdgeType(CompGeo::pHalfEdgeType & h, const int & id)
{
	ID = id;
	leftMost = h;
	lPt = new PointType<T>(this, 'L');
	rPt = new PointType<T>(this, 'R');
}

template<typename T> EdgeType<T>::~EdgeType(void)
{
	if (lPt != NULL) delete lPt;
	if (rPt != NULL) delete rPt;
	lPt = NULL;
	rPt = NULL;
}

template<typename T> EdgeType<T> & EdgeType<T>::operator=(const EdgeType<T> & a)
{
	if (lPt != NULL) delete lPt;
	if (rPt != NULL) delete rPt;

	ID = a.ID;
	leftMost = a.leftMost;
	lPt = new PointType<T>(*a.lPt);
	rPt = new PointType<T>(*a.rPt);

	return *this;
}

template<typename T> bool operator == (const EdgeType<T> & a, const EdgeType<T> & b)
{
	return (a.leftMost == b.leftMost);
}

template<typename T> bool operator < (const EdgeType<T> & a, const EdgeType<T> & b)
{
	CompGeo::BasicTPoint<T> A = a.lPt->pt->xyz, B = b.lPt->pt->xyz;
	if (A == B)
	{
		A = a.rPt->pt->xyz;
		B = b.rPt->pt->xyz;
	}
	T ax = A.X, ay = A.Y, bx = B.X, by = B.Y;
	return (CompGeo::BEq(ay, by)? ax < bx: ay > by);

}

template<typename T> void EdgeType<T>::PrintEdge(void)
{
	string buff = "\tEdge " + PrintID(10) + " i.e. {left: " + lPt->PrintID(10) + lPt->suffix + 
		"(" + to_string(lPt->pt->xyz.X) + "," + to_string(lPt->pt->xyz.Y) + ") -> right: " +
		rPt->PrintID(10) + rPt->suffix + "(" + to_string(rPt->pt->xyz.X) + "," +
		to_string(rPt->pt->xyz.Y) + ")}\r\n";
	cout << buff;

}

template<typename T> string EdgeType<T>::PrintID(const int szBuff)
{
	int count = -1, id = ID, d26 = 0, cMod = 0;

	string r;
	if (szBuff <= 2) return r;

	if (id == -2)
	{
		r = "_"; return r;
	}

	if (id == -3)
	{
		r = "^"; return r;
	}

	do
	{
		if ((count + 2) >= szBuff)
		{
			return r;
		}
		cMod = id % 26;
		r += (char)(cMod + (int)'a');
		id /= 26;
	} while (id > 0);
	return r;
}

template<typename T> NodeCollection<T>::NodeCollection(void)
{
	first = NULL;
}

template<typename T> void NodeCollection<T>::PrintCollection(void)
{

	string desc = "", endol = "\n";

	pNodeList NL = NULL, TL = first;
	pNodeType n = NULL;
	
	char tc;
	pTzoid<T> tN = NULL;
	pXNode<T> xN = NULL;
	pYNode<T> yN = NULL;
	
	while (TL != NULL)
	{
		NL = TL->next;
		n = TL->node_record;
		
		tc = n->node_type;
		switch (tc) 
		{
		case 'T':
			tN = (pTzoid<T>)n->node;
			desc += "T" + to_string(tN->id) + "\t";
			break;
		case 'X':
			xN = (pXNode<T>)n->node;
			desc += "X" + to_string(xN->x_nodePt->ID) + "\t";
			break;
		case 'Y':
			yN = (pYNode<T>)n->node;
			desc += "Y" + to_string(yN->seg->ID) + "\t";
			break;
		default:
			assert(1 == 2); // program break in debug mode
			break;
		}
		cout << desc;
		TL = NL;
	}
	cout << endol;
}

template<typename T> NodeCollection<T>::~NodeCollection(void)
{
	pNodeList NL = NULL;
	pNodeType n = NULL;
	
	char tc;
	pTzoid<T> tN = NULL;
	pXNode<T> xN = NULL;
	pYNode<T> yN = NULL;
	
	while (first != NULL)
	{
		NL = first->next;
		n = first->node_record;
		
		tc = n->node_type;
		switch (tc) 
		{
		case 'T':
			tN = (pTzoid<T>)n->node;
			delete tN;
			break;
		case 'X':
			xN = (pXNode<T>)n->node;
			delete xN;
			break;
		case 'Y':
			yN = (pYNode<T>)n->node;
			delete yN;
			break;
		default:
			assert(1 == 2); // program break in debug mode
			break;
		}
		delete first;
		first = NL;
	}
}

template<typename T> void NodeCollection<T>::AddNode(pNodeType pN)
{ 
	pNodeList pNL = new NodeList; 
	pNL->node_record = pN;
	pNL->next = first;
	first = pNL;
}

template<typename T> Tzoid<T>::Tzoid(void)
{
	leftp = NULL;
	rightp = NULL;
	top = NULL;
	bottom = NULL;
	LeftUpper = NULL;
	LeftLower = NULL;
	RightUpper = NULL;
	RightLower = NULL;
	id = ++idIdx;
	GEbuff = NULL;
}

template<typename T> Tzoid<T>::Tzoid(const Tzoid<T> & a)
{
	leftp = a.leftp;
	rightp = a.rightp;
	top = a.top;
	bottom = a.bottom;
	LeftUpper = a.LeftUpper;
	LeftLower = a.LeftLower;
	RightUpper = a.RightUpper;
	RightLower = a.RightLower;
	id = ++idIdx;
	GEbuff = NULL;
}

template<typename T> Tzoid<T> &Tzoid<T>::operator=(const Tzoid<T> & a)
{
	leftp = a.leftp;
	rightp = a.rightp;
	top = a.top;
	bottom = a.bottom;
	LeftUpper = a.LeftUpper;
	LeftLower = a.LeftLower;
	RightUpper = a.RightUpper;
	RightLower = a.RightLower;
	id = a.id;
	GEbuff = NULL;

	return *this;
}

template<typename T> Tzoid<T>::~Tzoid(void)
{
	
}

template<typename T> int Tzoid<T>::ResetIndex(void)
{
	idIdx = -1;
	return -1;
}

template<typename T> TrapPointType Tzoid<T>::MakePoints(void)
{
	TrapPointType shape;
	shape.num_pts = 0;  // to indicate possible error
	CompGeo::XY trapPts[2] = {leftp->pt->xyz, rightp->pt->xyz}, 
		topPts[2] = {top->lPt->pt->xyz, top->rPt->pt->xyz},
		dtop = topPts[1] - topPts[0],
		botPts[2] = {bottom->lPt->pt->xyz, bottom->rPt->pt->xyz},
		dbot = botPts[1] - botPts[0];
	bool degenerate[2] = {(botPts[0] == topPts[0]), (botPts[1] == topPts[1])};
	if (fabs(trapPts[0].x - trapPts[1].x) < MAX_FLT_PRECISION) return shape;
	if (degenerate[0] && degenerate[1]) return shape;
	if (fabs(dtop.x) < MAX_FLT_PRECISION) return shape;
	if (fabs(dbot.x) < MAX_FLT_PRECISION) return shape;
	if ((bottom->leftMost->incidentface == 0) || 
		(bottom->leftMost->incidentface == Map<T>::cFaceIdx)) return shape;
	
	shape.num_pts = 4;
	int i = 0, startAt[2] = {0, degenerate[0]? 1: 2};
	for(i = 0; i < 2; ++i)
	{
		if (degenerate[i]) 
		{
			--shape.num_pts;
			shape.pts[startAt[i]] = trapPts[i];
		}
		else
		{
			double t_top = (trapPts[i].x - topPts[0].x) / dtop.x,
				t_bot = (trapPts[i].x - botPts[0].x) / dbot.x;
			shape.pts[startAt[i]] = topPts[0] + t_top * dtop;
			shape.pts[startAt[i] + 1] = botPts[0] + t_bot * dbot;
		}
	}
	if (!degenerate[1])
	{
		CompGeo::XY buff = shape.pts[startAt[1]];
		shape.pts[startAt[1]] = shape.pts[startAt[1] + 1];
		shape.pts[startAt[1] + 1] = buff;
	}
	return shape;
}

template<typename T> string Tzoid<T>::GetNeighbors(void)
{
	string buff = "", nullChr = "N";
	
	if (LeftUpper == NULL) buff += "LU:<" + nullChr + "> ";
	else buff += "LU:<" + to_string(LeftUpper->id) + "> ";
	
	if (LeftLower == NULL) buff += "LL:<" + nullChr + "> ";
	else buff += "LL:<" + to_string(LeftLower->id) + "> ";


	if (RightUpper == NULL) buff += "RU:<" + nullChr + "> ";
	else buff += "RU:<" + to_string(RightUpper->id) + "> ";
	
	if (RightLower == NULL) buff += "RL:<" + nullChr + "> ";
	else buff += "RL:<" + to_string(RightLower->id) + "> ";
	
	return buff;

}

template<typename T> void Tzoid<T>::PrintZoid(void)
{
	string desc = "\tTrapezoid " + to_string(id) + ": {lp:<" + leftp->PrintID(10) + leftp->suffix + "> rp:<" +
		rightp->PrintID(10) + rightp->suffix + "> t:<" + top->PrintID(10) + "> b:<" +
		bottom->PrintID(10) + "> " + GetNeighbors() + "}\n";
	cout << desc;

}

template<typename T> XNode<T>::XNode(pPointType<T> pPt)
{
	x_nodePt = pPt;
	left = NULL;
	right = NULL;
}

template<typename T> XNode<T>::~XNode(void)
{
}

template<typename T> char XNode<T>::Test(const PointType<T> & P)
{ // 'E' same point, 'L' point is left of node, 'R' point is right of node

	CompGeo::BasicTPoint<T> xyP = P.pt->xyz, xyMe = x_nodePt->pt->xyz;
	T xP = xyP.X, yP = xyP.Y, xN = xyMe.X, yN = xyMe.Y;
	xyP.Z = xyMe.Z;
	//if (P.pt->xy == x_nodePt->pt->xy) return 'E';
	if (xyP == xyMe) return 'E';
	if (fabs(xP - xN) < MAX_FLT_PRECISION) return yP < yN? 'L': 'R'; //'R'; //yP < yN? 'L': 'R';
	return xP < xN? 'L': 'R';
}

template<typename T> YNode<T>::YNode(pEdgeType<T> e)
{
	seg = e;
	above = NULL;
	below = NULL;
}

template<typename T> YNode<T>::~YNode(void)
{

}

template<typename T> char YNode<T>::PointTest(const PointType<T> & p_in) 
{ // 'E' point is on node, 'A' point is above node, 'B' point is below node
	CompGeo::BasicTPoint<T> P0 = seg->lPt->pt->xyz, P1 = seg->rPt->pt->xyz, P2 = p_in.pt->xyz;
	T z = static_cast<T>(0);
	P0.Z = z; P1.Z = z; P2.Z = z;
	if (P2 == P0) return 'E';
	P1 -= P0;
	P2 -= P0;
	CompGeo::BasicTPoint<T> c = CompGeo::Cross(P1, P2);
	T k = c.Z;
	if (k < z) return 'B';
	if (k > z) return 'A';
	return 'P'; // parallel
}

template<typename T> char YNode<T>::EdgeTest(const EdgeType<T> & e) 
{ // 'A' edge is above node, 'B' edge is below node
  // call this in build if PointTest returns 'E'

	CompGeo::XY P0 = seg->lPt->pt->xyz, P1 = seg->rPt->pt->xyz,
		P2 = e.lPt->pt->xyz, P3 = e.rPt->pt->xyz,
		D0 = P1 - P0, D1 = P3 - P2;

	assert ((P2 == P0) && (P1 != P3));	// equal edges not allowed
	bool segVert = fabs(D0.x) < MAX_FLT_PRECISION,
		eVert = fabs(D1.x) < MAX_FLT_PRECISION;

	if (segVert || eVert)
	{
		assert (!(segVert && eVert));	// overlaps not allowed
		if (segVert) return 'B';
		return 'A';
	}

	double m0 = D0.y / D0.x, m1 = D1.y  / D1.x;

	if (m0 > m1) return 'B';
	return 'A';

}

template<typename T> Graph<T>::Graph(void)
{
	root = NULL;
	leaf = NULL;

	topEdge = NULL;
	bottomEdge = NULL;
	NC = new NodeCollection<T>;
}

template<typename T> void Graph<T>::Reset(void)
{
	if (NC != NULL) delete NC;
	NC = NULL;
	root = NULL;
	leaf = NULL;

	// memory management of dcel will be in the calling thread
	if (topEdge != NULL) delete topEdge;
	if (bottomEdge != NULL) delete bottomEdge;
	topEdge = NULL;
	bottomEdge = NULL;

}

template<typename T> Graph<T>::~Graph(void)
{
	Reset();
}

template<typename T> pTzoid<T> Graph<T>::Initialize(RectangleType & bb)
{ // 1st trapezoid is bounding rectangle, s/b bigger than the screen

	
	assert (topEdge == NULL);
	assert (bottomEdge == NULL);
	assert (root == NULL);
/*
	CompGeo::DCEL<T> * d = Map<T>::dcel;
	T zro = static_cast<T>(0), one = static_cast<T>(1), bbr = static_cast<T>(bb.right),
		bbb = static_cast<T>(bb.bottom), bbt = static_cast<T>(bb.top), bbl = static_cast<T>(bb.left);
	CompGeo::pTPoint<T> oLR = new CompGeo::TPoint<T>(bbr + one, bbb - one, zro), 
		oUR = new CompGeo::TPoint<T>(bbr + one, bbt + one, zro), 
		oUL = new CompGeo::TPoint<T>(bbl - one, bbt + one, zro), 
		oLL = new CompGeo::TPoint<T>(bbl - one, bbb - one, zro);
	size_t iLR = d->push(oLR), iUR = d->push(oUR), iUL = d->push(oUL), iLL = d->push(oLL);
	oLR->vertex = iLR; oUR->vertex = iUR; oUL->vertex = iUL; oLL->vertex = iLL;
	CompGeo::pHalfEdgeType tlr = new CompGeo::HalfEdgeType, trl = new CompGeo::HalfEdgeType, 
		blr = new CompGeo::HalfEdgeType, brl = new CompGeo::HalfEdgeType;
	size_t itlr = d->push(tlr), itrl = d->push(trl), iblr = d->push(blr), ibrl = d->push(brl);	
	oLR->incidentedge = ibrl; oUR->incidentedge = itrl; oUL->incidentedge = itlr; oLL->incidentedge = iblr;
	tlr->halfedge = itlr; trl->halfedge = itrl; blr->halfedge = iblr; brl->halfedge = ibrl;
	tlr->origin = iUL; trl->origin = iUR; blr->origin = iLL; brl->origin = iLR;
	tlr->twin = itrl; trl->twin = itlr; blr->twin = ibrl; brl->twin = iblr;
*/	
	
	topEdge = new EdgeType<T>(bb.top, -3);
	bottomEdge = new EdgeType<T>(bb.bottom, -2);

	pTzoid<T> first = new Tzoid<T>;
	root = MakeNode(first, 'T');
	first->bottom = bottomEdge;
	first->top = topEdge;
	first->leftp = bottomEdge->lPt;
	first->rightp = topEdge->rPt;
	first->GEbuff = root;
	//first->PrintZoid();
	return first;
}

template<typename T> bool Graph<T>::ZoidIsZeroWidth(pTzoid<T> t_in)
{
	T xR = t_in->rightp->pt->xyz.X, xL = t_in->leftp->pt->xyz.X;

	return CompGeo::BEq(xR, xL); 
}

template<typename T> void Graph<T>::makeLinks(const vector<TrapezoidSequenceType<T>> & vst, size_t & prev_upper, size_t & prev_lower)
{
	size_t maxSequence = vst.size(), iSeq = 0;
	char yT = ' ';
	pTzoid<T> cZoid = NULL;

	for (size_t i = 0; i < maxSequence; ++i)
	{	// fill loop (no changes to trapezoids):
		TrapezoidListType<T> tL;	// default (void) constructor
		TrapezoidSequenceType<T> qL = vst[iSeq];
		yT = qL.yRel;
		tL.y_Tst = yT;
		cZoid = qL.qZoid;
		tL.Mod_This = cZoid->GEbuff;
		cZoid->GEbuff = MakeNode(cZoid, 'T');
		if (yT == 'A')
		{
			tL.t_zoid_upper = cZoid;
			if (i > 0) chain.CHN[prev_upper].next_upper = i;
			for (size_t j = prev_upper; j < i; ++j)
			{
				bool addMe = (((j == 0) && (chain.CHN[0].t_zoid_upper == NULL)) || (j > prev_upper));
				if (addMe) chain.CHN[j].t_zoid_upper = cZoid;
			}
			prev_upper = i;
		}
		else
		{
			tL.t_zoid_lower = cZoid;
			if (i > 0) chain.CHN[prev_lower].next_lower = i;
			for (size_t j = prev_lower; j < i; ++j)
			{
				bool addMe = (((j == 0) && (chain.CHN[0].t_zoid_lower == NULL)) || (j > prev_lower));
				if (addMe) chain.CHN[j].t_zoid_lower = cZoid;
			}
			prev_lower = i;
		}
		
		chain.CHN.push_back(tL);
		++iSeq;
	}

}

template<typename T> pTzoid<T> Graph<T>::fillFinalLink(const Tzoid<T> & zTemplate,
	size_t & prev_upper, size_t & prev_lower)
{
	pTzoid<T> nZoid = new Tzoid<T>(zTemplate);
	nZoid->GEbuff = MakeNode(nZoid, 'T');
	size_t numZoids = chain.CHN.size();
	char yT = chain.CHN[numZoids - 1].y_Tst;
	if (yT == 'A')
	{ // put new trapezoid on bottom:
		if (nZoid->RightLower != NULL) nZoid->RightLower->LeftLower = nZoid;
		for (size_t j = prev_lower; j < numZoids; ++j)
		{
			bool addMe = (((j == 0) && (chain.CHN[0].t_zoid_lower == NULL)) || (j > prev_lower));
			if (addMe) chain.CHN[j].t_zoid_lower = nZoid;
		}
	}
	else
	{ // put new trapezoid on top:
		if (nZoid->RightUpper != NULL) nZoid->RightUpper->LeftUpper = nZoid;
		for (size_t j = prev_upper; j < numZoids; ++j)
		{
			bool addMe = (((j == 0) && (chain.CHN[0].t_zoid_upper == NULL)) || (j > prev_upper));
			if (addMe) chain.CHN[j].t_zoid_upper = nZoid;
		}
	}
	return nZoid;
}

template<typename T> void Graph<T>::HandleLeftEnd(const pTzoid<T> & lTrap, pPointType<T> & pt_l, const pEdgeType<T> & e,
	const TrapezoidSequenceType<T> & qL, size_t & prev_upper, size_t & prev_lower)
{
	pTzoid<T> UZ = chain.CHN[0].t_zoid_upper, LZ = chain.CHN[0].t_zoid_lower;

	if (lTrap == NULL)
	{
		pt_l = e->lPt;
		UZ->LeftUpper = qL.lu;
		if (qL.lu != NULL) qL.lu->RightUpper = UZ;
		LZ->LeftLower = qL.ll;
		if (qL.ll != NULL) qL.ll->RightLower = LZ;
	}
	else
	{ // set up left Trapezoid:
		if (lTrap->LeftUpper != NULL) lTrap->LeftUpper->RightUpper = lTrap;
		if (lTrap->LeftLower != NULL) lTrap->LeftLower->RightLower = lTrap;
		if (ordered(lTrap->leftp, pt_l)) lTrap->rightp = pt_l;
		else lTrap->leftp = pt_l;
		lTrap->RightUpper = UZ;
		lTrap->RightLower = LZ;
		UZ->LeftUpper = lTrap;
		LZ->LeftLower = lTrap;
		lTrap->GEbuff = MakeNode(lTrap, 'T');
		chain.leftTrap = lTrap;
	}
	UZ->LeftLower = NULL;
	LZ->LeftUpper = NULL;
	UZ->bottom = e;
	LZ->top = e;
	UZ->leftp = pt_l;
	LZ->leftp = pt_l;

}

template<typename T> void Graph<T>::HandleRightEnd(const pTzoid<T> & rTrap, pPointType<T> & pt_r, const pEdgeType<T> & e,
	const TrapezoidSequenceType<T> & qL, size_t & prev_upper, size_t & prev_lower)
{
	size_t numZoids = chain.CHN.size();
	pTzoid<T> UZ = chain.CHN[numZoids - 1].t_zoid_upper, LZ = chain.CHN[numZoids - 1].t_zoid_lower;
	
	if (rTrap == NULL)
	{
		pt_r = e->rPt;
		
	}
	else
	{
		if (rTrap->RightUpper != NULL) rTrap->RightUpper->LeftUpper = rTrap;
		if (rTrap->RightLower != NULL) rTrap->RightLower->LeftLower = rTrap;
		if (ordered(pt_r, rTrap->rightp)) rTrap->leftp = pt_r;
		else rTrap->rightp = pt_r;
		rTrap->LeftUpper = UZ;
		rTrap->LeftLower = LZ;
		UZ->RightUpper = rTrap;
		LZ->RightLower = rTrap;
		rTrap->GEbuff = MakeNode(rTrap, 'T');
		chain.rightTrap = rTrap;
	}
	UZ->RightLower = NULL;
	LZ->RightUpper = NULL;
	UZ->bottom = e;
	LZ->top = e;
	UZ->rightp = pt_r;
	LZ->rightp = pt_r;

}

template<typename T> void Graph<T>::fixInnerLinks(const pEdgeType<T> & e, const vector<TrapezoidSequenceType<T>> & iZoids)
{
	// modifying the trapezoids in the links (only if there are more than 1):
	size_t numZoids = chain.CHN.size();
	char yT = ' ';
	for (size_t i = 1; i < numZoids; ++i)
	{
		TrapezoidSequenceType<T> qL = iZoids[i];
		TrapezoidListType<T> * ptL = &chain.CHN[i],
			* ptLTrail = &chain.CHN[i - 1];
		yT = ptL->y_Tst;
		pTzoid<T> UZ = ptL->t_zoid_upper, UZTrail = ptLTrail->t_zoid_upper;
		if (UZ != UZTrail)
		{
			UZ->bottom = e;
			UZ->leftp = UZTrail->rightp;
			UZ->LeftLower = UZTrail;
			UZTrail->RightLower = UZ;
			if (yT != 'A')
			{
				UZ->LeftUpper = qL.lu;
				if (qL.lu != NULL) qL.lu->RightUpper = UZ;
			}
		}
		pTzoid<T> LZ = ptL->t_zoid_lower, LZTrail = ptLTrail->t_zoid_lower;
		if (LZ != LZTrail)
		{
			LZ->top = e;
			LZ->leftp = LZTrail->rightp;
			LZ->LeftUpper = LZTrail;
			LZTrail->RightUpper = LZ;
			if (yT != 'B')
			{
				LZ->LeftLower = qL.ll;
				if (qL.ll != NULL) qL.ll->RightLower = LZ;
			}
		}
	}

}


template<typename T> vector<pTzoid<T>> Graph<T>::fillChain(const pEdgeType<T> & e)
{	// make sure the chain is reset beforehand
	pPointType<T> pt_l = e->lPt, pt_r = e->rPt;
	vector<TrapezoidSequenceType<T>> iZoids = getIntersectedTrapezoids(e);
	vector<pTzoid<T> > nZoids;	// new trapezoids (1 - 3) to return to calling process
	size_t numZoids = iZoids.size();
	assert (numZoids > 0);
	pTzoid<T> cZoid = NULL, zoid1st = iZoids[0].qZoid, zoidLst = iZoids[numZoids - 1].qZoid;
	char yT = ' ';
	size_t prev_upper = 0, prev_lower = 0;
	makeLinks(iZoids, prev_upper, prev_lower);
	pTzoid<T> nZoid = fillFinalLink(*zoidLst, prev_upper, prev_lower);
	nZoids.push_back(nZoid);
	pTzoid<T> lTrap = pt_l->pt->xyz == zoid1st->leftp->pt->xyz? NULL: new Tzoid<T>{*zoid1st}, 
		rTrap = pt_r->pt->xyz == zoidLst->rightp->pt->xyz? NULL: new Tzoid<T>{*zoidLst};
	HandleLeftEnd(lTrap, pt_l, e, iZoids[0], prev_upper, prev_lower);
	HandleRightEnd(rTrap, pt_r, e, iZoids[numZoids - 1], prev_upper, prev_lower);
	if (lTrap != NULL) nZoids.push_back(lTrap);
	if (rTrap != NULL) nZoids.push_back(rTrap);
	fixInnerLinks(e, iZoids);

	return nZoids;	
}

template<typename T> char Graph<T>::pointCmp(pYNode<T> yn, pPointType<T> cPt)
{ // to ensure a value of 'A' or 'B':
	char r = yn->PointTest(*cPt);
	assert (r != 'E'); 	// equal points not allowed
	if (r != 'B') r = 'A';	// inline points 'P' will be considered above
	return r;
}

template<typename T> bool Graph<T>::ordered(pPointType<T> a, pPointType<T> b)
{	// return true only if b follows a so that a - b is in order
	// equal points are not in order
	CompGeo::XY A = a->pt->xyz, B = b->pt->xyz;
	double ax = A.x, ay = A.y, bx = B.x, by = B.y;
	bool isVertical = fabs(ax - bx) < MAX_FLT_PRECISION,
		isHorizontal = fabs(ay - by) < MAX_FLT_PRECISION;
	if (isVertical && isHorizontal) return false;
	return isVertical? ay < by: ax < bx;

}

template<typename T> vector<TrapezoidSequenceType<T>> Graph<T>::getIntersectedTrapezoids(const pEdgeType<T> & e)
{
	pPointType<T> pt_l = e->lPt, pt_r = e->rPt;
	YNode<T> Y_N(e);
	pYNode<T> y_n = &Y_N;
	FindTrapezoid(e);
	vector<TrapezoidSequenceType<T>> zoids;
	pTzoid<T> cZoid = leaf; // cZoid is the current trapeZoid
	char yTst = pointCmp(y_n, cZoid->rightp); 
	TrapezoidSequenceType<T> qz;
	qz.qZoid = cZoid;
	qz.ll = cZoid->LeftLower;
	qz.lu = cZoid->LeftUpper;
	qz.yRel = yTst;
	while (ordered(cZoid->rightp, pt_r)) 
	{
		zoids.push_back(qz);
		assert ((yTst == 'A') || (yTst == 'B'));
		if (yTst == 'A') // trapezoid's right point is above edge
		{
			assert(cZoid->RightLower != NULL);
			cZoid = cZoid->RightLower;
		}
		else 
		{ // yTst is 'B': trapezoid's right point is below edge

			assert(cZoid->RightUpper != NULL);
			cZoid = cZoid->RightUpper;
		}
		qz.qZoid = cZoid;
		qz.ll = cZoid->LeftLower;
		qz.lu = cZoid->LeftUpper;
		yTst = pointCmp(y_n, cZoid->rightp);
		qz.yRel = yTst;
	} 
	zoids.push_back(qz);
	// for debug:
		//cout << "trapezoid sequence: ";
		//for (vector<TrapezoidSequenceType>::iterator it = zoids.begin(); it < zoids.end(); ++it)
		//{
		//	cout << to_string((*it).qZoid->id) << " ";
		//}
		//cout << "\n";
	return zoids;

}  

template<typename T> pNodeType Graph<T>::MakeNode(void * theNode, char theType)
{
	assert ((theType == 'T') || (theType == 'Y') || (theType == 'X'));
	pNodeType n = new NodeType; //(pNodeType)new NodeType));
	n->node = theNode;
	n->node_type = theType;
	NC->AddNode(n);
	return n;
}

template<typename T> void Graph<T>::ModifyNode(pNodeType theNodeRecord, void * theNode, char theType)
{
	assert ((theType == 'T') || (theType == 'Y') || (theType == 'X'));
	theNodeRecord->node = theNode;
	theNodeRecord->node_type = theType;
}

template<typename T> void Graph<T>::Graph1Link(pEdgeType<T> e)
{ // call this only when the chain has 1 link only  so that first's = last's
	pTzoid<T> topZ = chain.CHN[0].t_zoid_upper, botZ = chain.CHN[0].t_zoid_lower;
	pNodeType subtreeroot = chain.CHN[0].Mod_This;
	pYNode<T> y = new YNode<T>{e};
	y->above = topZ->GEbuff;
	y->below = botZ->GEbuff;
	if ((chain.leftTrap == NULL) && (chain.rightTrap == NULL)) ModifyNode(subtreeroot, y, 'Y');
	else
	{
		if ((chain.leftTrap != NULL) && (chain.rightTrap != NULL))
		{
			pXNode<T> x_lft = new XNode<T>{e->lPt}, x_rght = new XNode<T>{e->rPt};
			ModifyNode(subtreeroot, x_lft, 'X');
			x_lft->left = chain.leftTrap->GEbuff;
			x_lft->right = MakeNode(x_rght, 'X');
			x_rght->left = MakeNode(y, 'Y');
			x_rght->right = chain.rightTrap->GEbuff;
		}
		else
		{
			if (chain.leftTrap != NULL)
			{// only leftTrap
				pXNode<T> x_lft = new XNode<T>{e->lPt};
				ModifyNode(subtreeroot, x_lft, 'X');
				x_lft->left = chain.leftTrap->GEbuff;
				x_lft->right = MakeNode(y, 'Y');
			}
			else
			{// only rightTrap
				pXNode<T> x_rght = new XNode<T>{e->rPt};
				ModifyNode(subtreeroot, x_rght, 'X');
				x_rght->right = chain.rightTrap->GEbuff;
				x_rght->left = MakeNode(y, 'Y');
			}
		}
	}
}

template<typename T> void Graph<T>::Graph1st(pEdgeType<T> e)
{ // call this when the chain has more than one link and you're on the first link
	pTzoid<T> topZ = chain.CHN[0].t_zoid_upper, botZ = chain.CHN[0].t_zoid_lower;
	pNodeType subtreeroot = chain.CHN[0].Mod_This;
	pYNode<T> y = new YNode<T>{e};
	y->above = topZ->GEbuff;
	y->below = botZ->GEbuff;
	if (chain.leftTrap == NULL) ModifyNode(subtreeroot, y, 'Y');
	else
	{
		pXNode<T> x_lft = new XNode<T>{e->lPt};
		ModifyNode(subtreeroot, x_lft, 'X');
		x_lft->left = chain.leftTrap->GEbuff;
		x_lft->right = MakeNode(y, 'Y');
	}

}

template<typename T> void Graph<T>::GraphLst(pEdgeType<T> e)
{ // call this when the chain has more than one link and you're on the last link
	size_t maxChn = chain.CHN.size();
	pTzoid<T> topZ = chain.CHN[maxChn - 1].t_zoid_upper, botZ = chain.CHN[maxChn - 1].t_zoid_lower;
	pNodeType subtreeroot = chain.CHN[maxChn - 1].Mod_This;
	pYNode<T> y = new YNode<T>{e};
	y->above = topZ->GEbuff;
	y->below = botZ->GEbuff;
	if (chain.rightTrap == NULL) ModifyNode(subtreeroot, y, 'Y');
	else
	{
		pXNode<T> x_rght = new XNode<T>{e->rPt};
		ModifyNode(subtreeroot, x_rght, 'X');
		x_rght->right = chain.rightTrap->GEbuff;
		x_rght->left = MakeNode(y, 'Y');
	}
}

template<typename T> void Graph<T>::GraphInner(pEdgeType<T> e, size_t zIdx)
{ // call this when the chain has more than two links and you're on inner link zIdx (which is 1 to n-2)
	pTzoid<T> topZ = chain.CHN[zIdx].t_zoid_upper, botZ = chain.CHN[zIdx].t_zoid_lower;
	pNodeType subtreeroot = chain.CHN[zIdx].Mod_This;
	pYNode<T> y = new YNode<T>{e};
	y->above = topZ->GEbuff;
	y->below = botZ->GEbuff;
	ModifyNode(subtreeroot, y, 'Y');
}

template<typename T> vector<pTzoid<T>> Graph<T>::AddEdge(pEdgeType<T> e)
{ // chain should be all NULL here at the beginning
	
	vector<pTzoid<T>> r = fillChain(e);
	//PrintChain();

	size_t maxChn = chain.CHN.size();
	assert (maxChn > 0);
	if (maxChn == 1) Graph1Link(e);
	else
	{
		Graph1st(e);
		for (size_t i = 1; i < (maxChn - 1); ++i)
		{
			GraphInner(e, i);
		}
		GraphLst(e);
	}

	ClearChain();
	//NC->PrintCollection();
	return r;
}

template<typename T> void Graph<T>::ClearChain(void)
{
	chain.reset();
}

template<typename T> void Graph<T>::PrintChain(void)
{
	string desc = "", sep = "\t===============================\r\n",
		ec = "EndChain\r\n", bc = "Begin Chain\r\n"; 
	cout << bc << sep;

	if (chain.leftTrap == NULL) 
	{
		desc = "\tleftTrap is NULL\r\n";
		cout << desc;

	}
	else
	{
		cout << "\tlT";

		chain.leftTrap->PrintZoid();
	}

	if (chain.rightTrap == NULL)
	{
		desc = "\trightTrap is NULL\r\n";
		cout << desc;
	}
	else
	{
		cout << "\trT";
		chain.rightTrap->PrintZoid();
	}

	size_t maxChn = chain.CHN.size();
	for (size_t i = 0; i < maxChn; ++i)
	{
		TrapezoidListType<T>tL = chain.CHN[i];
		desc = "\t" + to_string(i) + "U";
		cout << desc;
		tL.t_zoid_upper->PrintZoid();

		desc = "\t" + to_string(i) + "L";
		cout << desc;
		tL.t_zoid_lower->PrintZoid();
	}

	cout << sep << ec;
}

template<typename T> void Graph<T>::FindTrapezoid(pEdgeType<T> e)
{ 
	pPointType<T> p_in = e->lPt;
	pNodeType n = root;
	char tC = n->node_type;
	pXNode<T> x = NULL;
	pYNode<T> y = NULL;

	while (tC != 'T')
	{
		if (tC == 'X')
		{
			x = (pXNode<T>)n->node;
			char xTst = x->Test(*p_in);
			if (xTst == 'L') n = x->left;
			else n = x->right; // for same point queries n would go right
		}
		else
		{ // y_node
			y = (pYNode<T>)n->node;
			char yTst = y->PointTest(*p_in);
			if (yTst == 'E') yTst = y->EdgeTest(*e);
			if (yTst != 'B') n = y->above;
			else n = y->below;
		}
		tC = n->node_type;
	}
	leaf = (pTzoid<T>)n->node;
}

template<typename T> void Graph<T>::FindTrapezoid(pPointType<T> p_in)
{
	pNodeType n = root;
	char tC = n->node_type;
	pXNode<T> x = NULL;
	pYNode<T> y = NULL;

	while (tC != 'T')
	{
		if (tC == 'X')
		{
			x = (pXNode<T>)n->node;
			char xTst = x->Test(*p_in);
			if (xTst == 'L') n = x->left;
			else n = x->right;
		}
		else
		{ // y_node
			y = (pYNode<T>)n->node;
			char yTst = y->PointTest(*p_in);
			if (yTst != 'B') n = y->above;
			else n = y->below;
		}
		tC = n->node_type;
	}
	leaf = (pTzoid<T>)n->node;
}

template<typename T> Map<T>::Map(void)
{
	leftHalves = NULL;
	nEdges = 0;
	//R = NULL;
	pBuff = new PointType<T>;
	pBuff->pt = new CompGeo::TPoint<T>;
}

template<typename T> Map<T>::~Map(void)
{
	if (leftHalves != NULL) delete[]leftHalves;
	leftHalves = NULL;
	//delete R;
	//R = NULL;
	int rIdx = Tzoid<T>::ResetIndex();
	delete pBuff->pt;
	pBuff->pt = NULL;
	delete pBuff;
	pBuff = NULL;
}

template<typename T> void Map<T>::GetEdges(void)
{
	size_t hMax = dcel->getHalfEdgeCount();
	assert ((hMax % 2) == 1); // 0 entry is 'NULL' & thus unpaired
	nEdges = (hMax - 1) / 2; // class data member (array size for leftHalves)
	if (leftHalves != NULL) delete[]leftHalves;
	leftHalves = new EdgeType<T>[nEdges]; // all void constructor
	int id_idx = -1; //(int)L'a' - 1;
	for (CompGeo::ITYPE i = 1; i < hMax; ++i)
	{ // first entry is faux NULL for DCEL so ignore
		CompGeo::pHalfEdgeType h1 = dcel->getHalfEdge(i), h2 = dcel->twin();
		CompGeo::XY p1 = dcel->origin(h1)->xyz, p2 = dcel->origin()->xyz;
		bool isLeft = false;
		if (fabs(p1.x - p2.x) < MAX_FLT_PRECISION) isLeft = (p1.y < p2.y);
		else isLeft = (p1.x < p2.x);
		if (isLeft) 
		{
			++id_idx;
			leftHalves[id_idx] = EdgeType<T>(h1, id_idx);
			//if (id_idx > 255) id_idx = (int)L'a' - 1;
		}
	}
}

template<typename T> void Map<T>::RandomizeEdges(void)
{
	EdgeType<T> eBuff;
	
	for (int i = nEdges - 1; i >= 1; --i)
	{
		int r = rand() % (i + 1);
		if (r != i)
		{
			eBuff = leftHalves[i];
			leftHalves[i] = leftHalves[r];
			leftHalves[r] = eBuff;
		}
	}
}

template<typename T> void Map<T>::GetBox(void) //const Gtk::Allocation & allo)
{ // extracting containing rectangle from the DCEL
	CompGeo::DCEL<T> * d = dcel;
	CompGeo::pFaceType pf = d->getFace(); // s/b containing rectangle
	vector<vector<CompGeo::pHalfEdgeType>> vecO = d->all_inner(pf);
	assert (vecO.size() == 1);
	vector<CompGeo::pHalfEdgeType> vecH = vecO[0]; //d->all_outer(pf);
	assert(vecH.size() == 4);
	cFaceIdx = pf->face;
	CompGeo::pHalfEdgeType ph = d->getHalfEdge(d->inner()->helsttop), ph0 = ph; // halfedge list top
	int itop = -1, ibottom = -1, ythis = 0, ynext = 0, ytop = 0, ybottom = 0, xthis = 0, xnext = 0;
	R.top = NULL; R.bottom = NULL;
	CompGeo::BasicTPoint<T> corners[4], deltas[4], check, zero;
	bool isH = false, go4T = false, go4B = false, isV = false;
	for (int i = 0; i < 4; ++i)
	{
		corners[i] = d->origin()->xyz;
		ph = d->next();
	}
	assert (ph == ph0); // 4 edges right?
	for (int i = 0; i < 4; ++i)
	{
		deltas[i] = corners[(i + 1) % 4] - corners[i];
		ythis = (int)corners[i].Y; ynext = (int)corners[(i + 1) % 4].Y;
		if (itop >= 0) ytop = (int)corners[itop].Y; 
		if (ibottom >= 0) ybottom = (int)corners[ibottom].Y;
		isH = (ythis == ynext);
		xthis = (int)corners[i].X; xnext = (int)corners[(i + 1) % 4].X;
		isV = (xthis == xnext);
		assert (isH != isV); // axis aligned rectangles only please!
		go4T = (isH && ((itop < 0) || (ythis >= ytop)));
		go4B = (isH && ((ibottom < 0) || (ythis <= ybottom)));
		if (go4T)
		{
			itop = i;
			R.top = ph;
		}
		if (go4B)
		{
			ibottom = i;
			R.bottom = ph;
		}
		check += deltas[i];
		ph = d->next();
	}
	int wtop = (int)deltas[itop].X, wbottom = (int)deltas[ibottom].X;
	
	assert((check == zero) && ((deltas[0] + deltas[2]) == zero) && ((deltas[1] + deltas[3]) == zero) 
		&& (abs(itop - ibottom) == 2) && ((wtop + wbottom) == 0));

	if (wtop < 0) R.top = d->twin(R.top);
	if (wbottom < 0) R.bottom = d->twin(R.bottom);

/*
	if (R != NULL) delete R;
	int w = allo.get_width(),
		h = allo.get_height();
	R = new RectangleType;
	R->bottom = -h / 2;
	R->left = -w / 2;
	R->right = w / 2;
	R->top = h / 2;
*/
}

template<typename T> void Map<T>::Run(CompGeo::DCEL<T> * d) //, Gtk::Allocation * allo)
{ // delete the old map before re-running 
	if (d == NULL) return;
	dcel = d;

	GetBox();
	GetEdges();
	// the following call can be commented out to help w/ debugging
	RandomizeEdges();

	map.clear();
	map.push_back(DAG.Initialize(R));

	for (int i = 0; i < nEdges; ++i) 
	{
		//DebugPrintEdge(&leftHalves[i]);
		vector<pTzoid<T>> nTraps = DAG.AddEdge(&leftHalves[i]);
		map.insert(map.end(), nTraps.begin(), nTraps.end());
		//DebugPrintZoids();
	}
	//DebugPrintZoids();
}

template<typename T> Fortune::pSitePoint Map<T>::GetSitePoint(CompGeo::BasicTPoint<T> * mPts)
{
	if (mPts == NULL) return NULL;
	pBuff->pt->xyz = *mPts; //CompGeo::XY(mPts.x, mPts.y);
	DAG.FindTrapezoid(pBuff);
	CompGeo::pFaceType pf = dcel->getFace(DAG.leaf->bottom->leftMost->incidentface);
	Fortune::pCellType pC = (Fortune::pCellType)pf->additionalFaceInfo;
	return pC->site;
}

template<typename T> vector<pPGonWork> Map<T>::TranslateTrapezoids(void)
{
	size_t zMax = map.size();
	vector<pPGonWork> r;
	pVertexNode pvn = NULL, pvTrail = NULL;
	
	for (size_t i = 0; i < zMax; ++i)
	{
		pTzoid<T> pZ = map[i];
		TrapPointType tPts = pZ->MakePoints();
		int numPts = tPts.num_pts;
		if (numPts > 0)
		{
			pPGonWork wGon = new PGonWork;
			pNameNode pNameOut = new NameNode;
			string nmn = "T_Zoid";
			pNameOut->polyname = nmn;
			wGon->pName = pNameOut;
			wGon->numVertices = numPts;
			pvTrail = NULL;
			for(int i = 0; i < numPts; ++i)
			{
				pvn = new VertexNode;
				pvn->vtxInfo.vertex = tPts.pts[i];
				if (pvTrail == NULL) wGon->vNode = pvn;
				else pvTrail->next = pvn;
				pvTrail = pvn;
			}
			r.push_back(wGon);
		}
	} 

	return r;

}

template<typename T> void Map<T>::DebugPrintZoids(void)
{
	cout << "Trapezoids so far:\n";
	size_t zMax = map.size();
	for (size_t i = 0; i < zMax; ++i)
	{
		map[i]->PrintZoid();
	}
}

template<typename T> void Map<T>::DebugPrintEdge(pEdgeType<T> e)
{
	string desc = "Adding Edge ";
	cout << desc;
	e->PrintEdge();

}

//helper global function to let the linker know where these templated calls must go:
void link_my_trapezoid(void)
{
	Map<int> intMap; intMap.Run(NULL); intMap.TranslateTrapezoids(); intMap.GetSitePoint(NULL);
	Map<double> dblMap;  dblMap.Run(NULL); dblMap.TranslateTrapezoids(); dblMap.GetSitePoint(NULL);
}