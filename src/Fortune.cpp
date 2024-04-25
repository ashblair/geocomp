#include "first.hpp"
#include "CompGeo.hpp"
#include "Sorter.hpp"
#include "NGons.hpp"
#include "Fortune.hpp"

using namespace std;

/*
Fortune::VertexPoint::~VertexPoint(void)
{
	IE = NULL; delete IE;
	vl = NULL; delete vl;
	vl_prev = NULL; delete vl_prev;
}
*/

Fortune::EventQPoint & Fortune::EventQPoint::operator= (const EventQPoint & a)
{
	type = a.type;
	site = a.site;
	arc = a.arc;
	if (arc != NULL) arc->circleEvent = this;
	x = a.x;
	y = a.y;
	//r = a.r;
	return *this;
}

/*
friend bool operator<(const EventQPoint&a, const EventQPoint&b) {
			return fabs(a.y - b.y) < MAX_FLT_PRECISION ? fabs(a.x - b.x) < MAX_FLT_PRECISION ?
				a.type == b.type? a.arc < b.arc: a.type == 'C': a.x < b.x: a.y > b.y;}
				
bool Fortune::operator<(const Fortune::EventQPoint & a, const Fortune::EventQPoint & b) 
{
	//if (fabs(a.y - b.y) >= MAX_FLT_PRECISION) return a.y > b.y;
	if (a.y != b.y) return a.y > b.y;
	//if (fabs(a.x - b.x) >= MAX_FLT_PRECISION) return a.x < b.x;
	if (a.x != b.x) return a.x < b.x;
	if (a.type != b.type) return (a.type == 'C');
	if (a.type == 'S') return false;
	return (*a.arc->cell->site < *b.arc->cell->site);
}

bool Fortune::operator ==(const EventQPoint & a, const EventQPoint & b)
{ 
	return (fabs(a.x - b.x) < MAX_FLT_PRECISION) && (fabs(a.y - b.y) < MAX_FLT_PRECISION)
			&& (a.type == b.type) && (a.arc == b.arc);
}
*/
Fortune::RadialPoint::RadialPoint(void)
{
	// 0 = center, 1 = +x axis, 2 = Q1, 3 = +y axis, 4 = Q2, 5 = -x, 6 = Q3, 7 = -y, 8 = Q4
	Quad2 = 0; 
	YoverX = 0.0; // for even Quad2 this is y/x otherwise it is 0.0
	he = NULL;  // point for ordering will be origin
	ctr = NULL;
	cell = NULL;

}

Fortune::RadialPoint::RadialPoint(pHalfEdgeType & h)
{
	assert(h != NULL);
	assert(h->IncidentCell != NULL);
	assert(h->IncidentCell->site != NULL);
	he = h;
	ctr = h->IncidentCell->site;
	YoverX = 0.0;
	cell = NULL;
	CompGeo::XY P0(ctr->x, ctr->y), P1(h->origin->xy.x, h->origin->xy.y), DP = P1 - P0;
	// 0 = center, 1 = +x axis, 2 = Q1, 3 = +y axis, 4 = Q2, 5 = -x, 6 = Q3, 7 = -y, 8 = Q4
	if (fabs(DP.x) < MAX_FLT_PRECISION)
	{
		if (fabs(DP.y) < MAX_FLT_PRECISION) Quad2 = 0;
		else
		{
			if (DP.y > 0.0) Quad2 = 3;
			else Quad2 = 7;
		}
	}
	else
	{
		if (fabs(DP.y) < MAX_FLT_PRECISION)
		{
			if (DP.x > 0) Quad2 = 1;
			else Quad2 = 5;
		}
		else
		{
			if (DP.x > 0) 
			{
				if (DP.y > 0) Quad2 = 2;
				else Quad2 = 8;
			}
			else
			{
				if (DP.y > 0) Quad2 = 4;
				else Quad2 = 6;
			}
			YoverX = DP.y / DP.x;
		}
	}


}

Fortune::RadialPoint::RadialPoint(pHalfEdgeType & h, const pSitePoint & center)
{
	assert(h != NULL);
	assert(center != NULL);
	he = h;
	ctr = center;
	YoverX = 0.0;
	cell = NULL;
	CompGeo::XY P0(ctr->x, ctr->y), P1(h->origin->xy.x, h->origin->xy.y), DP = P1 - P0;
	// 0 = center, 1 = +x axis, 2 = Q1, 3 = +y axis, 4 = Q2, 5 = -x, 6 = Q3, 7 = -y, 8 = Q4
	if (fabs(DP.x) < MAX_FLT_PRECISION)
	{
		if (fabs(DP.y) < MAX_FLT_PRECISION) Quad2 = 0;
		else
		{
			if (DP.y > 0.0) Quad2 = 3;
			else Quad2 = 7;
		}
	}
	else
	{
		if (fabs(DP.y) < MAX_FLT_PRECISION)
		{
			if (DP.x > 0) Quad2 = 1;
			else Quad2 = 5;
		}
		else
		{
			if (DP.x > 0) 
			{
				if (DP.y > 0) Quad2 = 2;
				else Quad2 = 8;
			}
			else
			{
				if (DP.y > 0) Quad2 = 4;
				else Quad2 = 6;
			}
			YoverX = DP.y / DP.x;
		}
	}


}

Fortune::RadialPoint::RadialPoint(const RadialPoint & r)
{
	Quad2 = r.Quad2; 
	YoverX = r.YoverX; // for even Quad2 this is y/x otherwise it is 0.0
	he = r.he;  // point for ordering will be origin
	ctr = r.ctr;
	cell = r.cell;
}

Fortune::RadialPoint & Fortune::RadialPoint::operator=(const RadialPoint & r)
{
	Quad2 = r.Quad2; 
	YoverX = r.YoverX; // for even Quad2 this is y/x otherwise it is 0.0
	he = r.he;  // point for ordering will be origin
	ctr = r.ctr;
	cell = r.cell;
	return *this;
}
/*
bool Fortune::operator<(const RadialPoint & a, const RadialPoint & b)
{
	if (a.Quad2 == b.Quad2) 
	{
		return (a.YoverX < b.YoverX);
	}
	return (a.Quad2 < b.Quad2);

}
*/
Fortune::BinaryBPlus::BinaryBPlus(void)
{
	//hf = NULL;
	hfSetUp = false;

	beforeLeaf = new BeachPointType; //(pBeachPointType)new BeachPointType));
	afterLeaf = new BeachPointType; //(pBeachPointType)new BeachPointType));
	leftmost = new ArcType; //(pArcType)new ArcType));
	rightmost = new ArcType; //(pArcType)new ArcType));
	root = new NodeType; //(pNodeType)new NodeType));
	root->left = new NodePointerType; //(pNodePointerType)new NodePointerType));
	root->right = new NodePointerType; //(pNodePointerType)new NodePointerType));

	//before and afterLeaf are BeachPoints representing - & + infinity:
	beforeLeaf->LeafFlag = 'b'; beforeLeaf->arcLeft = NULL; beforeLeaf->arcRight = NULL;
	afterLeaf->LeafFlag = 'a'; afterLeaf->arcLeft = NULL; afterLeaf->arcRight = NULL;

	leftmost->ArcFlag = 'b'; leftmost->cell = NULL; leftmost->circleEvent = NULL;
	leftmost->next = rightmost; leftmost->prev = NULL; leftmost->order = 0.0;

	rightmost->ArcFlag = 'a'; rightmost->cell = NULL; rightmost->circleEvent = NULL;
	rightmost->next = NULL; rightmost->prev = leftmost; rightmost->order = 1024.0;

	root->greatestLeftPoint = beforeLeaf; // i.e. - infinity
	root->height = 1;
	root->left->node = NULL;
	root->left->leaf = beforeLeaf;  // i.e. - infinity
	root->right->node = NULL;
	root->right->leaf = afterLeaf;  // i.e. + infinity
	// so, all points will be between and the tree will build at root->right
	// so rebalancing ops should stop at root->right
	
	pathTop = NULL;
	pathBottom = NULL;
	arcTop = NULL;
	IsStarting = true;
	SharedTopLevel = false;
	dBugQ = NULL;

}

Fortune::BinaryBPlus::~BinaryBPlus(void)
{
	DeleteSubTree(root);
	if (root != NULL) delete root;
	root = NULL;
	pArcType a = leftmost;
	while (leftmost != NULL)
	{
		leftmost = leftmost->next;
		delete a;
		a = leftmost;
	}
	rightmost = NULL;
	ClearPath();
	if (hfSetUp) hf.close();
	//if (hfSetUp) CloseHandle(hf);
	//delete beforeLeaf; this is done by DeleteSubTree
	//delete afterLeaf;
}

void Fortune::BinaryBPlus::Reset(void)
{
	DeleteSubTree(root);
	if (root != NULL) delete root;
	root = NULL;
	pArcType a = leftmost->next, a_nxt = NULL;
	while (a != rightmost)
	{
		a_nxt = a->next;
		delete a;
		a = a_nxt;
	}
	ClearPath();

	beforeLeaf = new BeachPointType; //(pBeachPointType)new BeachPointType));
	afterLeaf = new BeachPointType; //(pBeachPointType)new BeachPointType));
	root = new NodeType; //(pNodeType)new NodeType));
	root->left = new NodePointerType; //(pNodePointerType)new NodePointerType));
	root->right = new NodePointerType; //(pNodePointerType)new NodePointerType));

	//before and afterLeaf are BeachPoints representing - & + infinity:
	beforeLeaf->LeafFlag = 'b'; beforeLeaf->arcLeft = NULL; beforeLeaf->arcRight = NULL;
	afterLeaf->LeafFlag = 'a'; afterLeaf->arcLeft = NULL; afterLeaf->arcRight = NULL;

	leftmost->ArcFlag = 'b'; leftmost->cell = NULL; leftmost->circleEvent = NULL;
	leftmost->next = rightmost; leftmost->prev = NULL; leftmost->order = 0.0;

	rightmost->ArcFlag = 'a'; rightmost->cell = NULL; rightmost->circleEvent = NULL;
	rightmost->next = NULL; rightmost->prev = leftmost; rightmost->order = 1024.0;

	root->greatestLeftPoint = beforeLeaf; // i.e. - infinity
	root->height = 1;
	root->left->node = NULL;
	root->left->leaf = beforeLeaf;  // i.e. - infinity
	root->right->node = NULL;
	root->right->leaf = afterLeaf;  // i.e. + infinity
	// so, all points will be between and the tree will build at root->right
	// so rebalancing ops should stop at root->right
	
	pathTop = NULL;
	pathBottom = NULL;
	arcTop = NULL;
	IsStarting = true;
	SharedTopLevel = false;

}

void Fortune::BinaryBPlus::ClearPath(void)
{
	pPathType p = pathTop;
	while (pathTop != NULL)
	{
		pathTop = pathTop->next;
		delete p;
		p = pathTop;
	}
	pathBottom = NULL;

}

void Fortune::BinaryBPlus::DeleteSubTree(pNodeType & sTree)
{ // delete tree not edges, vertices or cells

	if (sTree == NULL) return;

	if ((sTree->left->leaf != NULL) && (sTree->right->leaf != NULL))
	{
		delete sTree->left->leaf;
		delete sTree->right->leaf;
		delete sTree->left;
		delete sTree->right;
		//delete sTree;
		return;
	}
	else
	{
		if (sTree->left->leaf != NULL)
		{
			delete sTree->left->leaf;
			delete sTree->left;
			DeleteSubTree(sTree->right->node);
			delete sTree->right->node;
			delete sTree->right;
			
			//delete sTree;
			return;
		}
		else
		{
			if (sTree->right->leaf != NULL)
			{
				delete sTree->right->leaf;
				delete sTree->right;
				DeleteSubTree(sTree->left->node);
				delete sTree->left->node;
				delete sTree->left;
				//delete sTree;
				return;
			}
			else
			{
				DeleteSubTree(sTree->left->node);
				delete sTree->left->node;
				delete sTree->left;
				DeleteSubTree(sTree->right->node);
				delete sTree->right->node;
				delete sTree->right;
				return;
			}
		}
	}
}

void Fortune::BinaryBPlus::AddArc(pArcType & a, AVL<EventQPoint> &q) 
{ // dispatcher function: 3 kinds of AddArc's available
	y_d = a->cell->site->y;
	if (IsStarting)
	{
		if (arcTop == NULL) 
		{
			arcTop = a;
			AddFirstArc(a);
		}
		else
		{
			double yTop = arcTop->cell->site->y;
			if (fabs(yTop - a->cell->site->y) < MAX_FLT_PRECISION)
			{
				SharedTopLevel = true;
				AddTopLevelArc(a);
			}
			else
			{
				IsStarting = false;
				arcTop = NULL;
				if (SharedTopLevel) AddNormal(a, q);
				else AddFirstPoint(a);
			}
		}
	}
	else AddNormal(a, q);

}

int Fortune::BinaryBPlus::GetHeight(pNodeType x)
{
	if (x == NULL) return 0;
	return x->height;
}

void Fortune::BinaryBPlus::SetHeight(pNodeType x)
{
	if (x == NULL) return;
	x->height = 1 + GetMax(GetHeight(x->left->node), GetHeight(x->right->node));

}

Fortune::pNodeType Fortune::BinaryBPlus::rotateLeft(pNodeType x)
{// prerequisites: x!=NULL && x->right!=NULL
	assert((x != NULL) && (x->right->node != NULL)); // so x->right->leaf is NULL
	pNodeType y = x->right->node;
	SetNode(x->right, y->left->node, y->left->leaf);
	SetNode(y->left, x, NULL);
	SetHeight(x);
	SetHeight(y);
	return y;
}

Fortune::pNodeType Fortune::BinaryBPlus::rotateRight(pNodeType y)
{ // prerequisites: y!=NULL && y->left!=NULL
	assert((y != NULL) && (y->left->node != NULL));
	pNodeType x = y->left->node;
	SetNode(y->left, x->right->node, x->right->leaf);
	SetNode(x->right, y, NULL);
	SetHeight(y);
	SetHeight(x);
	return x;

}
Fortune::pNodeType Fortune::BinaryBPlus::Rotate(pPathType z) // rotates at z
{
	pPathType y = z->next, w = z->prev, x = y->next;
	pNodeType r, zNde = z->node, yNde = y->node, xNde = NULL;
	if (x == NULL)
	{
		assert(yNde->height > 1);
		int lH = GetHeight(yNde->left->node), rH = GetHeight(yNde->right->node);
		xNde = lH > rH? yNde->left->node: yNde->right->node;
		int nlH = GetHeight(xNde->left->node), nrH = GetHeight(xNde->right->node);
		AddToPath(xNde, nlH > nrH? 'L': 'R');
		x = pathBottom;
	}
	else xNde = x->node;
	if (z->direction == y->direction)
	{
		if (z->direction == 'L') r = rotateRight(z->node); // Left Left
		else r = rotateLeft(z->node); // Right Right
		RemoveFromPath(z);
		return r;
	}
	char xDir = x->direction, zDir = z->direction;
	if (zDir == 'L') // Left Right
	{
		z->node->left->node = rotateLeft(y->node);
		r = rotateRight(z->node);
	}
	else // Right Left
	{
		z->node->right->node = rotateRight(y->node);
		r = rotateLeft(z->node);
	}
	assert(r == xNde);
	TruncatePath(w);
	if (zDir == 'L') // Left Right
	{
		if (xDir == 'L') 
		{
			AddToPath(xNde, 'L');
			AddToPath(yNde, 'R');
		}
		else
		{
			AddToPath(xNde, 'R');
			AddToPath(zNde, 'L');
		}
	}
	else  // Right Left
	{
		if (xDir == 'L')
		{
			AddToPath(xNde, 'L');
			AddToPath(zNde, 'R');
		}
		else
		{
			AddToPath(xNde, 'R');
			AddToPath(yNde, 'L');
		}
	}
	return r;

}

void Fortune::BinaryBPlus::TruncatePath(pPathType p_t)
{ // removes all path nodes below p_t & makes p_t pathBottom
	//HRESULT b_p;
	//char eMsg[256];

	pPathType p_nxt = p_t->next;
	while (p_nxt != NULL)
	{
		p_t->next = p_nxt->next;
		//b_p = StringCbPrintf(eMsg, 256 * sizeof(char),L"TruncatePath - deleting path element %p \r\n", p_nxt);
		//OutputDebugString(eMsg);
		delete p_nxt;
		p_nxt = p_t->next;
	}
	pathBottom = p_t;
}

void Fortune::BinaryBPlus::RemoveFromPath(pPathType p_r)
{ // call from bottom up
	//HRESULT b_p;
	//char eMsg[256];
	assert(p_r != NULL);
	assert(p_r != pathTop);
	pPathType p_nxt = p_r->next, p_prev = p_r->prev;
	if (p_r == pathBottom)
	{
		//b_p = StringCbPrintf(eMsg, 256 * sizeof(char),
		//	L"RemoveFromPath - deleting pathBottom %p \r\n", p_r);
		//OutputDebugString(eMsg);
		delete p_r;
		pathBottom = p_prev;
		p_r = p_prev;
		char ckDir = p_r->direction;
		pNodeType ckNd = ckDir == 'L'? p_r->node->left->node: p_r->node->right->node;
		if (ckNd == NULL) return;
		int hL = GetHeight(ckNd->left->node), hR = GetHeight(ckNd->right->node);
		char nxtDir = hL > hR? 'L': 'R';
		AddToPath(ckNd, nxtDir);
		return;
	}
	p_prev->next = p_nxt;
	p_nxt->prev = p_prev;
	//b_p = StringCbPrintf(eMsg, 256 * sizeof(char),
	//	L"RemoveFromPath - deleting path element %p \r\n", p_r);
	//OutputDebugString(eMsg);
	delete p_r;
	if (p_nxt != pathBottom) 
	{
		pNodeType nd1 = p_nxt->node, nd2 = p_nxt->next->node;
		p_nxt->direction = nd1->left->node == nd2? 'L': 'R';
	}
}

void Fortune::BinaryBPlus::AddToPath(pNodeType node, char way)
{
	pPathType p = new PathType; //(pPathType)new PathType));
	p->direction = way;
	p->prev = NULL;
	p->next = NULL;
	p->node = node;
	if (pathTop == NULL)
	{
		pathTop = p;
		pathBottom = p;
		return;
	}
	p->prev = pathBottom;
	pathBottom->next = p;
	pathBottom = p;

}

Fortune::pArcType Fortune::BinaryBPlus::FindArc(pCellType c) // sets path 
{
	ClearPath();
	char d = 'R';
	AddToPath(root, d);
	pNodeType p = root->right->node, prev = root;
	double X = c->site->x;
	while (p != NULL)
	{
		GetX(p->greatestLeftPoint);
		d = X < x_bp ? 'L': 'R';
		AddToPath(p, d);
		prev = p;
		if (d == 'L') p = p->left->node;
		else p = p->right->node;
	}
	return d == 'L'? prev->greatestLeftPoint->arcLeft: prev->greatestLeftPoint->arcRight;

}

void Fortune::BinaryBPlus::FindLeaf(pArcType a) 
{ // path is to beachpoint with arcLeft or arcRight = a, call for circle event after circle_ctr calc
  
	pPathType polePoint = NULL;
	ClearPath();
	char d = 'R';
	AddToPath(root, d);
	pNodeType p = root->right->node, prev = root;
	double o = a->order;
	pBeachPointType bpR = a->rightBP, bpL = a->leftBP;
	assert ((bpR != NULL) && (bpL != NULL));
	while (p != NULL)
	{
		pBeachPointType bp = p->greatestLeftPoint;

		if ((bp == bpR) || (bp == bpL))
		{
			d = 'L';
			AddToPath(p, d);
			prev = p;
			p = p->left->node;
			while (p != NULL)
			{
				d = 'R';
				AddToPath(p, d);
				prev = p;
				p = p->right->node;
			}
		}
		else
		{
			d = o < bp->arcLeft->order? 'L': 'R';
			AddToPath(p, d);
			prev = p;
			p = (d == 'L')? p->left->node: p->right->node;
		}
	}

	/*
	pBeachPointType bpR = a->rightBP, bpL = a->leftBP, bpFound = NULL;
	assert ((bpR != NULL) && (bpL != NULL));
	double X = circle_ctr.x;
	while (p != NULL)
	{
		pBeachPointType bp = p->greatestLeftPoint;
		
		if ((bp == bpR) || (bp == bpL))
		{
			d = 'L';
			AddToPath(p, d);
			prev = p;
			p = p->left->node;
			while (p != NULL)
			{
				d = 'R';
				AddToPath(p, d);
				prev = p;
				p = p->right->node;
			}
		}
		else
		{
			GetX(bp);
			d = X < x_bp ? 'L': 'R';
			AddToPath(p, d);
			if ((polePoint == NULL) && (fabs(X - x_bp) < MAX_FLT_PRECISION)) polePoint = pathBottom;
			//(fabs(X - x_bp) < MAX_FLT_PRECISION)) polePoint = pathBottom;
			prev = p;
			p = (d == 'L')? p->left->node: p->right->node;
		}

	}
	bpFound = pathBottom->direction == 'L'? pathBottom->node->left->leaf: pathBottom->node->right->leaf;
	while (!((bpFound == bpL) || (bpFound == bpR)) && (polePoint != NULL))
	{
		TruncatePath(polePoint);
		d = polePoint->direction == 'L'? 'R': 'L'; // try the other direction
		polePoint->direction = d;
		p = polePoint->node;
		polePoint = NULL;
		p = (d == 'L')? p->left->node: p->right->node;
		while (p != NULL)
		{
			pBeachPointType bp = p->greatestLeftPoint;
		
			if ((bp == bpR) || (bp == bpL))
			{
				d = 'L';
				AddToPath(p, d);
				prev = p;
				p = p->left->node;
				while (p != NULL)
				{
					d = 'R';
					AddToPath(p, d);
					prev = p;
					p = p->right->node;
				}
			}
			else
			{
				GetX(bp);
				d = X < x_bp ? 'L': 'R';
				AddToPath(p, d);
				if ((polePoint == NULL) && (fabs(X - x_bp) < MAX_FLT_PRECISION)) polePoint = pathBottom;
					//(fabs(X - x_bp) < MAX_FLT_PRECISION)) polePoint = pathBottom;
				prev = p;
				p = (d == 'L')? p->left->node: p->right->node;
			}
		}
		bpFound = pathBottom->direction == 'L'? pathBottom->node->left->leaf: pathBottom->node->right->leaf;
	}
	*/

	assert(pathBottom->direction == 'L'? (pathBottom->node->left->leaf->arcLeft == a) || (pathBottom->node->left->leaf->arcRight == a):
		(pathBottom->node->right->leaf->arcLeft == a) || (pathBottom->node->right->leaf->arcRight == a));
	
}

void Fortune::BinaryBPlus::DeleteArc(pEventQPoint c_e, AVL<EventQPoint> & q, pDiagramType d)
{	// call with circle point, add vertex (i.e. circle center), set as origins for halfedges
	// necessitates deleting 1 leaf from the tree and modifying another
	
	//HRESULT b_p;
	//char eMsg[256], pBuff[25], fNme[] = L"F:\\Projects\\Polygons\\Polys\\EventQTree.txt";

	assert (c_e->type == 'C');
	y_d = c_e->y;
	RecoverRadius(c_e);
	circle_ctr.x = c_e->x;
	circle_ctr.y = y_d + radius;
	pArcType a = c_e->arc, aL = a->prev, aR = a->next; a->circleEvent = NULL;
	pVertexListType vLst = new VertexListType; //(pVertexListType)new VertexListType));
	vLst->vertex.IncidentEdge = NULL; vLst->vertex.xy = circle_ctr;
	vLst->next = d->vertices; d->vertices = vLst; 
	pVertexType v = &(vLst->vertex);
	FindLeaf(a);
	pPathType p = pathBottom;
	pNodeType nD = p->node;
	pBeachPointType leafToDel = (p->direction == 'L')? nD->left->leaf: nD->right->leaf;
	//int pSuffix = (int)leafToDel & 0xffff;
	//bool fStopHere = (pSuffix == 0x9bd8);

	char NodeToPt = p->direction, 
		ArcToPt = (a->leftBP == leafToDel)? 'L': 'R',
		PtToArc = (ArcToPt == 'L')? 'R': 'L';
	assert(PtToArc == 'L'? leafToDel->arcLeft == a: leafToDel->arcRight == a);
	pBeachPointType leafToMod = (ArcToPt == 'L')? a->rightBP: a->leftBP;
	assert(leafToDel->halfEdge->origin == NULL);
	assert(leafToMod->halfEdge->origin == NULL);
	//assert(leafToDel->halfEdge != leafToMod->halfEdge->twin);
	leafToDel->halfEdge->origin = v; //released_he[0] = leafToDel->halfEdge;
	leafToMod->halfEdge->origin = v; //released_he[1] = leafToMod->halfEdge;
	pHalfEdgeType h = new HalfEdgeType, //(pHalfEdgeType)new HalfEdgeType)),
		h_d = new HalfEdgeType; //(pHalfEdgeType)new HalfEdgeType));
	h->twin = h_d;  h_d->twin = h;  
	h->flag = -1; h->next = NULL; h->prev = NULL; h->origin = NULL; h->IncidentCell = aL->cell;
	h_d->flag = -1; h_d->next = NULL; h_d->prev = NULL; h_d->origin = v; v->IncidentEdge = h_d;
	h_d->IncidentCell = aR->cell;  
	//released_he[2] = h_d;

	pHalfEdgeListType hLR = new HalfEdgeListType, //(pHalfEdgeListType)new HalfEdgeListType)),
		hLL = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
	hLR->halfEdge = h_d;  hLL->halfEdge = h;
	hLR->next = aR->cell->InnerComponents; aR->cell->InnerComponents = hLR;
	hLL->next = aL->cell->InnerComponents; aL->cell->InnerComponents = hLL;
	leafToMod->halfEdge = h;

	if (ArcToPt == 'L')
	{
		leafToMod->arcLeft = aL;
		aL->rightBP = leafToMod;
	}
	else
	{
		leafToMod->arcRight = aR;
		aR->leftBP = leafToMod;
	}

	aL->next = aR;
	aR->prev = aL;
	if (aL->circleEvent != NULL)
	{
		AVLNode<EventQPoint> * aNode = q.Find(aL->circleEvent);
		assert(aNode != NULL);
		assert(aNode->Data->arc == aL);
		/*
		if (dBugQ != NULL)
		{
			int i = 0;
			char * idiot = pBuff; 
			q.PathToStr(idiot, 25, &i);
			b_p = StringCbPrintf(eMsg, 256 * sizeof(char),L"Delete %c (aL) - tree after dBugQ %p y=%f path=%s\r\n", 
				a->cell->ID, aNode->Data, aNode->Data->y, pBuff);
			q.TreeWrite(eMsg, fNme);
		}
		*/
		q.Delete();
		aL->circleEvent = NULL;
	}
	if (aR->circleEvent != NULL)
	{
		AVLNode<EventQPoint> * aNode = q.Find(aR->circleEvent);
		assert(aNode != NULL);
		assert(aNode->Data->arc == aR);
		/*
		if (dBugQ != NULL)
		{
			int i = 0;
			char * idiot = pBuff; 
			q.PathToStr(idiot, 25, &i);
			b_p = StringCbPrintf(eMsg, 256 * sizeof(char),L"Delete %c (aR) - tree after dBugQ %p y=%f path=%s\r\n", 
				a->cell->ID, aNode->Data, aNode->Data->y, pBuff);
			q.TreeWrite(eMsg, fNme);
		}
		*/
		q.Delete();
		aR->circleEvent = NULL;
	}
	GetCircle(aL);
	if (circleFound)
	{
		pEventQPoint c_e0 = new EventQPoint(circle_ctr);
		//if (c_e->y == -399.49977374477078) dBugQ = c_e0;
		c_e0->y -= radius;
		c_e0->type = 'C';
		//c_e0->r = radius;
		c_e0->arc = aR;
		aR->circleEvent = c_e0;
		bool Inserted = q.Insert(c_e0);
		assert (Inserted);
	}
	GetCircle(aL->prev);
	if (circleFound)
	{
		pEventQPoint c_e1 = new EventQPoint(circle_ctr);
		//if (c_e->y == -399.99753657785448) dBugQ = c_e1;
		c_e1->y -= radius;
		c_e1->type = 'C';
		//c_e1->r = radius;
		c_e1->arc = aL;
		aL->circleEvent = c_e1;
		bool Inserted = q.Insert(c_e1);
		assert (Inserted);
	}

	p = p->prev;
	SetNode(p->direction == 'L'? p->node->left: p->node->right, 
		NodeToPt == 'L'? nD->right->node: nD->left->node,
		NodeToPt == 'L'? nD->right->leaf: nD->left->leaf);
	SetHeight(p->node);

	while (p != pathTop)
	{
		pNodeType nde = p->node, ndeNxt;
		pPathType q = p->prev;
		if (nde->greatestLeftPoint == leafToDel) 
		{
			nde->greatestLeftPoint = nD->greatestLeftPoint;
		}
		int hL = GetHeight(nde->left->node), hR = GetHeight(nde->right->node), delta = hR - hL;
		if ((delta < -1) || (delta > 1))
		{
			pPathType r = p->next;
			while (r != NULL)
			{
				r = r->next;
				delete (p->next);
				p->next = r;
			}
			pathBottom = p;
			p->direction = hL > hR? 'L': 'R';
			ndeNxt = hL > hR? nde->left->node: nde->right->node;
			int hLn = GetHeight(ndeNxt->left->node), hRn = GetHeight(ndeNxt->right->node);
			AddToPath(ndeNxt, hLn > hRn? 'L': 'R');
			if (q->direction == 'L') q->node->left->node = Rotate(p);
			else q->node->right->node = Rotate(p);
		}
		SetHeight(q->node);
		p = q;
	}
	SetHeight(root);
	//b_p = StringCbPrintf(eMsg, 256 * sizeof(char),L"DeleteArc - deleting arc %p \r\n", a);
	//OutputDebugString(eMsg);
	delete a;
	//b_p = StringCbPrintf(eMsg, 256 * sizeof(char),L"DeleteArc - deleting leaf %p %x \r\n", leafToDel, ((int)leafToDel) & (0xffff));
	//OutputDebugString(eMsg);
	delete leafToDel;
	//b_p = StringCbPrintf(eMsg, 256 * sizeof(char),L"DeleteArc - deleting node pointer %p \r\n", nD->left);
	//OutputDebugString(eMsg);
	delete nD->left;
	//b_p = StringCbPrintf(eMsg, 256 * sizeof(char),L"DeleteArc - deleting node pointer %p \r\n", nD->right);
	//OutputDebugString(eMsg);
	delete nD->right;
	//b_p = StringCbPrintf(eMsg, 256 * sizeof(char),L"DeleteArc - deleting node %p \r\n", nD);
	//OutputDebugString(eMsg);
	delete nD;
	// sanity check:
	//SanityCheck();
}

double Fortune::BinaryBPlus::GetYofXForArc(pArcType a, double x)
{
	double xF = a->cell->site->x, yF = a->cell->site->y;
	return ((sqr(x) - 2 * xF * x + sqr(xF) + sqr(yF) - sqr(y_d)) / (2 * (yF - y_d)));
}

void Fortune::BinaryBPlus::GetX(pBeachPointType bp)
{	// uses y_d, sets x_bp

	assert(bp->LeafFlag == 'n');
	double x1 = bp->arcLeft->cell->site->x, x2 = bp->arcRight->cell->site->x, 
		y1 = bp->arcLeft->cell->site->y, y2 = bp->arcRight->cell->site->y,
		l = y_d, dx = x1 - x2, dy = y1 - y2, v1 = y1 - l, v2 = y2 - l, 
		determinate = sqrt(v1 * v2 * (sqr(dx) + sqr(dy))), x;
	if (fabs(dy) < MAX_FLT_PRECISION) 
	//if (dy == 0.0)
	{ // return midpoint:
		x = (x1 + x2) / 2.0;
		x_bp = x;
		return;
	}
	x_bp = ((x2 * v1 - x1 * v2 - determinate) / dy);

}

void Fortune::BinaryBPlus::GetY(pBeachPointType bp)
{	// uses x_bp or (sometimes) y_d to return y coordinate of intersection

	assert(bp->LeafFlag == 'n');
	double x1 = bp->arcLeft->cell->site->x, x2 = bp->arcRight->cell->site->x, 
		y1 = bp->arcLeft->cell->site->y, y2 = bp->arcRight->cell->site->y,
		l = y_d, dx = x1 - x2, dy = y2 - y1, v1 = y1 - l, v2 = y2 - l; 
	if (fabs(dy) < MAX_FLT_PRECISION) 
	{ // return parabola1's y of x @ x = (x1 + x2) / 2.0
		if (fabs(v1) < MAX_FLT_PRECISION) y_bp = l;
		else
			y_bp = (sqr(y1) + sqr(x1) / 4.0 - x1 * x2 / 2.0 + sqr(x2) / 4.0 - sqr(l)) / (2.0 * v1);
		return;
	}
	y_bp = ((dx * x_bp + (sqr(x2) + sqr(y2) - sqr(x1) - sqr(y1)) / 2.0) / dy);

}

void Fortune::BinaryBPlus::GetCircle(pArcType arcIn) 
{	// 3 consecutive focal points from list in: 
	//sets circle_ctr and radius (0 iff no circle) and circleFound

	radius = 0.0;  
	circleFound = false; // this will be our "no circle" flag 
	circle_ctr.x = 0.0;
	circle_ctr.y = 0.0;
	pArcType a1 = arcIn, a2 = a1->next, a3 = a2->next;
	if ((a1 == leftmost) || (a3 == rightmost)) return;
	CompGeo::XY p1(a1->cell->site->x, a1->cell->site->y),
		p2(a2->cell->site->x, a2->cell->site->y),
		p3(a3->cell->site->x, a3->cell->site->y),
		pIJ = p1 - p2, pKL = p2 - p3, pLK = p3 - p2;
	double cross = CompGeo::Cross(pIJ, pKL), x1 = p1.x, y1 = p1.y, x2 = p2.x, y2 = p2.y, x3 = p3.x,
		y3 = p3.y, X12 = pIJ.x, X23 = pKL.x, Y12 = pIJ.y, Y23 = pKL.y, x, y, r;//, r1, r2, r3;
	// linearity check:
	if (fabs(cross) < MAX_FLT_PRECISION) return;  // cross product is 0
	if (CompGeo::Cross(pIJ, pLK) < 0.0) return;  // divergence
	// in order check:
	bool O1 = ((p1.x < p2.x) || ((fabs(p1.x - p2.x) < MAX_FLT_PRECISION) && (p1.y > p2.y))),
		O2 = ((p2.x < p3.x) || ((fabs(p2.x - p3.x) < MAX_FLT_PRECISION) && (p2.y > p3.y))),
		InOrder = (O1 && O2);
	//if (!InOrder) return;
	// get center point:
	x = Y23 * (sqr(x1) - sqr(x2) + sqr(y1) - sqr(y2)) - Y12 * (sqr(x2) - sqr(x3) + sqr(y2) - sqr(y3));
	x /= 2 * cross;
	y = -X23 * (sqr(x1) - sqr(x2) + sqr(y1) - sqr(y2)) + X12 * (sqr(x2) - sqr(x3) + sqr(y2) - sqr(y3));
	y /= 2 * cross;
	//r1 = sqrt(sqr(p1.x - x) + sqr(p1.y - y));
	//r2 = sqrt(sqr(p2.x - x) + sqr(p2.y - y));
	//r3 = sqrt(sqr(p3.x - x) + sqr(p3.y - y));
	r = sqrt(sqr(p2.x - x) + sqr(p2.y - y));

	// circle point below or on directrix?
	double y_c = y - r;
	if ((y_c < y_d) || (fabs(y_c - y_d) < MAX_FLT_PRECISION))
	{
		//HRESULT b_p;
		//char cMsg[256];
		
		circleFound = true;
		circle_ctr.x = x;
		circle_ctr.y = y;
		radius = r;
		//b_p = StringCbPrintf(cMsg, 256 * sizeof(char),
		//	L"GetCircle (%.15f,%.15f) r=%.15f, y(event)=%.15f, focus(%.15f,%.15f) %c %c %c\r\n", 
		//	x, y, r, y_c, p2.x, p2.y, a1->cell->ID, a2->cell->ID, a3->cell->ID);
		//OutputDebugString(cMsg);
	}

}
									

void Fortune::BinaryBPlus::RecoverRadius(pEventQPoint c_e)
{	// calculates circle radius from circle event
	// focus: (xf, yf), circle event point: (xc, yc) which is 1 radius directly below circle center
	
	double xc = c_e->x, yc = c_e->y, xf = c_e->arc->cell->site->x, yf = c_e->arc->cell->site->y;
	radius = (sqr(xf - xc) + sqr(yf - yc)) / (2 * (yf - yc));
	//radius = c_e->r;

	//double y_r = yc + radius;
	//double x_r = xc;
	

	//char desc[256];
	//HRESULT b_p;

	//b_p = StringCbPrintf(desc, 256 * sizeof(char), 
	//	TEXT("RecoverRadius: r=%.15f ctr(%.15f, %.15f) f(%.15f,%.15f)\r\n"), 
	//	radius, x_r, y_r, xf, yf);
	//OutputDebugString(desc);

}

void Fortune::BinaryBPlus::AddFirstArc(pArcType & a)
{ // only along beach line
	a->prev = leftmost;
	a->next = rightmost;
	leftmost->next = a;
	rightmost->prev = a;
	a->order = (leftmost->order + rightmost->order) / 2.0;
}

void Fortune::BinaryBPlus::AddFirstPoint(pArcType & a)
{ // call this if 1st focus is higher then this 2nd focus
	pArcType a1 = rightmost->prev, a3 = new ArcType; //(pArcType)new ArcType));
	a3->ArcFlag = 'n'; a3->cell = a1->cell; a3->circleEvent = NULL;
	a3->next = rightmost; a3->prev = a; rightmost->prev = a3; a3->rightBP = NULL;
	a1->next = a;
	a->next = a3; a->prev = a1;
	a->order = (rightmost->order + 2.0 * a1->order) / 3.0;
	a3->order = (a->order + rightmost->order) / 2.0;
	// beachline: leftmost<->a1<->a<->a3<->rightmost

	pHalfEdgeType hHi = new HalfEdgeType, //(pHalfEdgeType)new HalfEdgeType)),
		hLo = new HalfEdgeType; //(pHalfEdgeType)new HalfEdgeType));
	hHi->twin = hLo;  hLo->twin = hHi;
	hHi->flag = -1; hHi->next = NULL; hHi->prev = NULL; hHi->origin = NULL; hHi->IncidentCell = a1->cell;
	hLo->flag = -1; hLo->next = NULL; hLo->prev = NULL; hLo->origin = NULL; hLo->IncidentCell = a->cell;
	pHalfEdgeListType lHi = new HalfEdgeListType, //(pHalfEdgeListType)new HalfEdgeListType)),
		lLo = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
	lHi->halfEdge = hHi; lHi->next = a1->cell->InnerComponents; a1->cell->InnerComponents = lHi;
	lLo->halfEdge = hLo; lLo->next = a->cell->InnerComponents; a->cell->InnerComponents = lLo;

	pBeachPointType bp1 = new BeachPointType, //(pBeachPointType)new BeachPointType)),
		bp2 = new BeachPointType; //(pBeachPointType)new BeachPointType));
	bp1->LeafFlag = 'n'; bp1->arcLeft = a1; bp1->arcRight = a; bp1->halfEdge = hHi; 
	a1->rightBP = bp1; a->leftBP = bp1;
	bp2->LeafFlag = 'n'; bp2->arcLeft = a; bp2->arcRight = a3; bp2->halfEdge = hLo; 
	a->rightBP = bp2; a3->leftBP = bp2;

	pNodeType nTop = new NodeType, //(pNodeType)new NodeType)),
		nBottom = new NodeType; //(pNodeType)new NodeType));
	nTop->left = new NodePointerType; //(pNodePointerType)new NodePointerType));
	nTop->right = new NodePointerType; //(pNodePointerType)new NodePointerType));
	nBottom->left = new NodePointerType; //(pNodePointerType)new NodePointerType));
	nBottom->right = new NodePointerType; //(pNodePointerType)new NodePointerType));
	SetNode(root->right, nTop, NULL);
	root->height = 3;

	SetNode(nTop->right, nBottom, NULL);
	SetNode(nTop->left, NULL, bp1);
	nTop->height = 2;
	nTop->greatestLeftPoint = bp1;

	SetNode(nBottom->right, NULL, afterLeaf);
	SetNode(nBottom->left, NULL, bp2);
	nBottom->height = 1;
	nBottom->greatestLeftPoint = bp2;


}

void Fortune::BinaryBPlus::AddTopLevelArc(pArcType & a)
{ // call this if foci after the first have equal y values

	pArcType a1 = rightmost->prev;
	a->next = rightmost; a->prev = a1; a1->next = a; a->rightBP = NULL;
	rightmost->prev = a;
	a->order = (a1->order + rightmost->order) / 2.0;
	// beachline: leftmost<->...<->a1<->a<->rightmost

	pHalfEdgeType hL = new HalfEdgeType, //(pHalfEdgeType)new HalfEdgeType)),
		hR = new HalfEdgeType; //(pHalfEdgeType)new HalfEdgeType));
	hL->twin = hR;  hR->twin = hL;
	hL->flag = -1; hL->next = NULL; hL->prev = NULL; hL->origin = NULL; hL->IncidentCell = a1->cell;
	hR->flag = -1; hR->next = NULL; hR->prev = NULL; hR->origin = NULL; hR->IncidentCell = a->cell;
	pHalfEdgeListType lL = new HalfEdgeListType, //(pHalfEdgeListType)new HalfEdgeListType)),
		lR = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
	lL->halfEdge = hL; lL->next = a1->cell->InnerComponents; a1->cell->InnerComponents = lL;
	lR->halfEdge = hR; lR->next = a->cell->InnerComponents; a->cell->InnerComponents = lR;

	pBeachPointType bp = new BeachPointType; //(pBeachPointType)new BeachPointType));
	bp->LeafFlag = 'n'; bp->arcLeft = a1; bp->arcRight = a; bp->halfEdge = hL;
	a1->rightBP = bp; a->leftBP = bp;
	// the halfedge on the left has origin at bottom of vertical perpendicular bisector
	// the twin has origin along the top of the bounding box (to be calculated afterwards)

	pNodeType n = new NodeType; //(pNodeType)new NodeType));
	n->left = new NodePointerType; //(pNodePointerType)new NodePointerType));
	n->right = new NodePointerType; //(pNodePointerType)new NodePointerType));
	SetNode(n->left, NULL, bp);
	SetNode(n->right, NULL, afterLeaf);
	n->greatestLeftPoint = bp;
	n->height = 1;
	pArcType aF = FindArc(a->cell); 
	//assert(aF == a1);
	pPathType p = pathBottom;
	//assert((p->node->right->leaf == afterLeaf) && (p->direction == 'R')); 
	// node we're adding here should be last i.e. rightmost
	SetNode(p->node->right, n, NULL);
	AddToPath(n, 'R');
	// now: rebalance
	while (p != pathTop)
	{
		pNodeType nde = p->node;
		pPathType q = p->prev;
		int h0 = GetHeight(nde);
		SetHeight(nde);
		if (h0 == GetHeight(nde)) return;
		int delta = GetHeight(nde->right->node) - GetHeight(nde->left->node);
		if ((delta < -1) || (delta > 1))
		{
			if (q->direction == 'L') q->node->left->node = Rotate(p);
			else q->node->right->node = Rotate(p);
			return;
		}
		p = q;
	}
	SetHeight(root);

}

void Fortune::BinaryBPlus::AddNormal(pArcType & a, AVL<EventQPoint> & q)
{	// Q needed to add or delete circle events
	//HRESULT b_p;
	//char eMsg[256];
	pArcType a1 = FindArc(a->cell), a0 = a1, a3 = new ArcType; //(pArcType)new ArcType));
	a3->ArcFlag = 'n'; a3->cell = a1->cell; a3->circleEvent = NULL;
	a3->next = a1->next; a3->prev = a; a1->next->prev = a3;
	a1->next = a;
	a->next = a3; a->prev = a1;
	pBeachPointType bpL = a1->leftBP, bpR = a1->rightBP;
	a3->leftBP = bpL; a3->rightBP = bpR;
	if (bpR == NULL) bpR = afterLeaf;
	a->order = (a3->next->order + 2 * a1->order) / 3.0;
	a3->order = (a->order + a3->next->order) / 2.0;
	// beachline: leftmost<->...<->a1<->a<->a3<->...<->rightmost
	if (a1->circleEvent != NULL)  // false alarm - delete:
	{
		AVLNode<EventQPoint> * aNde = q.Find(a1->circleEvent);
		assert (aNde != NULL);
		//b_p = StringCbPrintf(eMsg, 256 * sizeof(char),
		//	L"AddNormal - deleting circleEvent %p \r\n", a1->circleEvent);
		//OutputDebugString(eMsg);
		q.Delete();
		a1->circleEvent = NULL;
	}
	if (a1->ArcFlag == 'n')  // checking for circles:
	{
		a0 = a1->prev;
		GetCircle(a0);
		if (circleFound)
		{
			pEventQPoint cE = new EventQPoint(circle_ctr);
			if (cE->y > radius) cE->y -= radius;
			else cE->y = -(radius - cE->y);
			cE->arc = a1;
			cE->type = 'C';
			//cE->r = radius;
			a1->circleEvent = cE;
			bool inserted = q.Insert(cE);
			assert(inserted);
		}
		GetCircle(a);
		if (circleFound)
		{
			pEventQPoint cE = new EventQPoint(circle_ctr);
			if (cE->y > radius) cE->y -= radius;
			else cE->y = -(radius - cE->y);
			cE->arc = a3;
			cE->type = 'C';
			//cE->r = radius;
			a3->circleEvent = cE;
			bool inserted = q.Insert(cE);
			assert(inserted);
		}
	}

	pHalfEdgeType hHi = new HalfEdgeType, //(pHalfEdgeType)new HalfEdgeType)),
		hLo = new HalfEdgeType; //(pHalfEdgeType)new HalfEdgeType));
	hHi->twin = hLo;  hLo->twin = hHi;
	hHi->flag = -1; hHi->next = NULL; hHi->prev = NULL; hHi->origin = NULL; hHi->IncidentCell = a1->cell;
	hLo->flag = -1; hLo->next = NULL; hLo->prev = NULL; hLo->origin = NULL; hLo->IncidentCell = a->cell;
	pHalfEdgeListType lHi = new HalfEdgeListType, //(pHalfEdgeListType)new HalfEdgeListType)),
		lLo = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
	lHi->halfEdge = hHi; lHi->next = a1->cell->InnerComponents; a1->cell->InnerComponents = lHi;
	lLo->halfEdge = hLo; lLo->next = a->cell->InnerComponents; a->cell->InnerComponents = lLo;

	pPathType p = pathBottom;
	pBeachPointType bp1 = new BeachPointType, //(pBeachPointType)new BeachPointType)),
		bp2 = new BeachPointType; //(pBeachPointType)new BeachPointType));
	bp1->LeafFlag = 'n'; bp1->arcLeft = a1; bp1->arcRight = a; bp1->halfEdge = hHi; 
	a1->rightBP = bp1; a->leftBP = bp1;
	bp2->LeafFlag = 'n'; bp2->arcLeft = a; bp2->arcRight = a3; bp2->halfEdge = hLo; 
	a->rightBP = bp2; a3->leftBP = bp2; //a3->rightBP = bpR->LeafFlag == 'n'? bpR: NULL; 
	if (bpR != NULL) bpR->arcLeft = a3;

	pNodeType nTop = new NodeType, //(pNodeType)new NodeType)),
		nBottom = new NodeType; //(pNodeType)new NodeType));
	nTop->left = new NodePointerType; //(pNodePointerType)new NodePointerType));
	nTop->right = new NodePointerType; //(pNodePointerType)new NodePointerType));
	nBottom->left = new NodePointerType; //(pNodePointerType)new NodePointerType));
	nBottom->right = new NodePointerType; //(pNodePointerType)new NodePointerType));


	SetNode(nTop->right, nBottom, NULL);
	SetNode(nTop->left, NULL, bp1);
	nTop->height = 2;
	nTop->greatestLeftPoint = bp1;

	SetNode(nBottom->right, NULL, bpR);
	SetNode(nBottom->left, NULL, bp2);
	nBottom->height = 1;
	nBottom->greatestLeftPoint = bp2;
	
	SetNode(p->direction == 'L'? p->node->left: p->node->right, nTop, NULL);


	AddToPath(nTop, 'R'); 
	AddToPath(nBottom, 'L');
	// rebalance:
	bool ShortCircuit = false, 
		UpdateGreatestLeft = false; //(p->direction == 'R') && (bpR->LeafFlag == 'n');
	while (p != pathTop)
	{
		pNodeType nde = p->node;
		pPathType q = p->prev;
		int h0 = GetHeight(nde);
		SetHeight(nde);
		//if (!ShortCircuit) ShortCircuit = (h0 == GetHeight(nde));
		//if ((ShortCircuit) && !(UpdateGreatestLeft)) return;
		/*if (UpdateGreatestLeft)
		{
			if (nde->greatestLeftPoint == bpL) 
			{
				nde->greatestLeftPoint = bp2;
				UpdateGreatestLeft = false;
			}
		}*/
		int delta = GetHeight(nde->right->node) - GetHeight(nde->left->node);
		if ((delta < -1) || (delta > 1))
		{
			assert(p->next != NULL);
			/*
			if (p->next == NULL)
			{
				char dir_calc = delta > 1? 'R': 'L';
				assert (dir_calc == p->direction);
				pNodeType nde_nxt = p->direction == 'L'? nde->left->node: nde->right->node;
				int rHn = GetHeight(nde_nxt->right->node), lHn = GetHeight(nde_nxt->left->node);
				AddToPath(nde_nxt, lHn > rHn? 'L': 'R');
			}
			*/
			if (q->direction == 'L') q->node->left->node = Rotate(p);
			else q->node->right->node = Rotate(p);
			//return; no return here because we're adding 2 nodes so balance all the way to the top
		}
		p = q;
	}
	SetHeight(root);

}

void Fortune::BinaryBPlus::SetNode(pNodePointerType & nPtr, pNodeType n, pBeachPointType bp)
{
	assert(((n == NULL) || (bp == NULL)) && !((n == NULL) && (bp == NULL)));
	nPtr->leaf = bp;
	nPtr->node = n;

}

int Fortune::BinaryBPlus::GetRealHeight(pNodeType tNode)
{
	if (tNode == NULL) return 0;
	return (1 + GetMax(GetRealHeight(tNode->right->node), GetRealHeight(tNode->left->node)));
}

void Fortune::BinaryBPlus::nodeToElement(int level, int column, pNodeType node)
{ // i love recursion
	if (node == NULL) return;
	int lIdx = GetTwoPow(level) - 1 + column;
	if (lIdx > max_level) max_level = lIdx;
	//levels[(int)pow(2.0, level) - 1 + column] = node;
	levels[lIdx] = node;
	nodeToElement(level + 1, 2 * column, node->left->node);
	nodeToElement(level + 1, 2 * column + 1, node->right->node);
}

int Fortune::BinaryBPlus::GetTwoPow(int n)
{
	if ((n < 0) || (n > 30)) return 0;
	int TwoPow[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384,
		32768, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216,
		33554432, 67108864, 134217728, 268435456, 536870912, 1073741824};
	return TwoPow[n];

}

void Fortune::BinaryBPlus::TreeWrite(const string & Desc, const string & fN)
{ // just heights for now:
	if (root == NULL) return;
	int h = GetRealHeight(root) - 1, n = h - 1;
	if (h > 13)
	{
		//MessageBox(NULL, TEXT("Tree Too Large For Writing"), TEXT("Error in TreeWrite"), MB_OK);
		cout << "Error in TreeWrite: Tree Too Large For Writing\n";
		return;
	}
	if (h < 2) return;

	int TwoPow[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384,
	32768, 65536}; //, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216,
	//33554432, 67108864, 134217728, 268435456, 536870912, 1073741824, 2147483648, 4294967296}

	double dtCount = TwoPow[h] - 1;
	int tCount = (int)dtCount;
	levels = new pNodeType[tCount];
	for (int i = 0; i < tCount; ++i) levels[i] = NULL;
	max_level = 0;
	nodeToElement(0, 0, root->right->node);

	int w1 = 6, // ' #### ' to output the contents of left & right
				// a node will be indicated w/ 00 o/w sitepoint ID chars will be printed like ABBC
		w2 = TwoPow[n] * w1, // line length
		w2_ = w2 + 1;
	char * line = new char[w2_], * ptrs = new char[w2_], * blanks = new char[w2_];
	char sChars[] = " /_\\\0";
	char zChars[] = "00", * lChars /*[] = TEXT("AB")*/, * rChars/*[] = TEXT("BA")*/,
		fChars[] = "\r\n", bChars[] = "<ABBA>";  lChars = &bChars[1]; rChars = &bChars[3];
	char blank = sChars[0], fslash = sChars[1], u_s = sChars[2], bslash = sChars[3], nchr = sChars[4];
	for (int i = 0; i < w2; ++i) blanks[i] = blank;
	blanks[w2] = nchr;
	int  dwBytesToWrite, dwBytesWritten, dwPos;
	char * buff = new char[w2 + 10];

	if (!hfSetUp) 
	{
		hf.open(fN.c_str(), ios_base::app);
		//hf = CreateFile(fN, 
		//		FILE_APPEND_DATA,         // open for writing
		//		0,						  // do not share
		//		NULL,                     // no security
		//		OPEN_ALWAYS,              // open or create
		//		FILE_ATTRIBUTE_NORMAL,    // normal file
		//		NULL);					  // no attr. template
		hfSetUp = true;
	}
	

	//char txtBuff[256];
	string txtBuff;
	//char fslash = (char)'/', bslash = (char)'\\', dash = (char)'-';
	size_t l = Desc.length();
	//HRESULT b_p = StringCbLength(Desc, 256 * sizeof(char), &l);
	//dwBytesToWrite = (DWORD)l;
	//dwPos = SetFilePointer(hf, 0, NULL, FILE_END);
	//WriteFile(hf, Desc, dwBytesToWrite, &dwBytesWritten, NULL);
	hf << Desc;

	txtBuff = "\tBeachline is a series of parabolic arcs whose foci are as follows:\r\n";
	hf << txtBuff;
	//b_p = StringCbPrintf(txtBuff, 256 * sizeof(char),
	//	TEXT("\tBeachline is a series of parabolic arcs whose foci are as follows:\r\n"));
	//b_p = StringCbLength(txtBuff, 256 * sizeof(char), &l);
	//dwBytesToWrite = (DWORD)l;
	//WriteFile(hf, txtBuff, dwBytesToWrite, &dwBytesWritten, NULL);

	char blChars[256] = "A:(123,456)-", *bl_end;
	size_t bl_l = 12, bl_max = 256, bl_rem;
	int bl_count = 1;
	txtBuff[0] = '\t';
	pArcType bl_a = leftmost->next;
	stringstream xStrm, yStrm;
	xStrm << setw(3) << fixed;
	yStrm << setw(3) << fixed;
	string arcStr = "";
	while (bl_a != rightmost)
	{
		//blChars[0] = bl_a->cell->ID;
		xStrm.str("");
		yStrm.str("");
		xStrm << (int)bl_a->cell->site->x;
		yStrm << (int)bl_a->cell->site->y;
		arcStr += to_string(bl_a->cell->ID) + ":(" + xStrm.str() + "," + yStrm.str() + ")-";
		//b_p = StringCbPrintfEx(blChars, bl_max * sizeof(char), 
		//	&bl_end, &bl_rem, STRSAFE_IGNORE_NULLS, TEXT("%c:(%03i,%03i)-"), 
		//	bl_a->cell->ID, (int)bl_a->cell->site->x, (int)bl_a->cell->site->y);
		//b_p = StringCchLength(blChars, 256, &bl_l); //bl_l >=12
		//if ((bl_count + bl_l + 3) >= 80)
		if (arcStr.length() >= 80)
		{
			hf << arcStr << fChars;
			arcStr = "";
			//txtBuff[bl_count] = ' ';
			//b_p = StringCbPrintfEx(txtBuff + bl_count, 256 * sizeof(char), &bl_end, &bl_rem,
			//	STRSAFE_IGNORE_NULLS, TEXT("%s"), fChars);
			//b_p = StringCbLength(txtBuff, 256 * sizeof(char), &l);
			//dwBytesToWrite = (DWORD)l;
			//WriteFile(hf, txtBuff, dwBytesToWrite, &dwBytesWritten, NULL);
			//bl_count = 1;
		}
		//txtBuff[bl_count] = ' '; // wipe out terminator
		//b_p = StringCbPrintfEx(txtBuff + bl_count, 256 * sizeof(char), 
		//	&bl_end, &bl_rem, STRSAFE_IGNORE_NULLS, TEXT("%s"),
		//	blChars);
		//bl_count += (int)bl_l;
		bl_a = bl_a->next;
	}
	hf << arcStr << fChars;
	//b_p = StringCbLength(txtBuff, 256 * sizeof(char), &l);
	//dwBytesToWrite = (DWORD)l;
	//if (l > 0)
	//{
	//	txtBuff[l] = ' ';
	//	b_p = StringCbPrintfEx(txtBuff + bl_count - 1, 256 * sizeof(char), &bl_end, &bl_rem,
	//		STRSAFE_IGNORE_NULLS, TEXT("%s"), fChars);
	//	b_p = StringCbLength(txtBuff, 256 * sizeof(char), &l);
	//	dwBytesToWrite = (DWORD)l;
	//	WriteFile(hf, txtBuff, dwBytesToWrite, &dwBytesWritten, NULL);
	//	bl_count = 0;
	//}

	//b_p = StringCbPrintf(txtBuff, 256 * sizeof(char),
	//	TEXT("\tTree is at root->right. Root is a buffer node with root->height = %u (1 higher than below)\r\n"),
	//	root->height);
	//b_p = StringCbLength(txtBuff, 256 * sizeof(char), &l);
	//dwBytesToWrite = (DWORD)l;
	//WriteFile(hf, txtBuff, dwBytesToWrite, &dwBytesWritten, NULL);
	hf << "\tTree is at root->right. Root is a buffer node with root->height = " + to_string(root->height) + " (1 higher than below)\r\n";
	pCellType c;
	char bpType;
	int lIdx = 0;
	for (int i = 0; i < h; ++i)
	{
		int w3 = w2 / TwoPow[i], d2 = w3 / 2, d1 = d2 - w1 / 2,
			wL = d1 - w2 / TwoPow[i + 2], wR = 3 * w2 / TwoPow[i + 2] - d1 - w1;
		if (wL < 0) wL = 0;
		if (wR < 0) wR = 0;
		memcpy(line, blanks, w2_ * sizeof(char));
		memcpy(ptrs, blanks, w2_ * sizeof(char));
		for (int j = 0; j < TwoPow[i]; ++j)
		{
			lIdx = TwoPow[i] - 1 + j; if (lIdx > max_level) max_level = lIdx;
			pNodeType p = levels[TwoPow[i] - 1 + j], * q = NULL;
			if (p != NULL)
			{
				pNodePointerType pp = p->left;
				if (p->left->node != NULL) memcpy(lChars, zChars, 2 * sizeof(char));
				else
				{
					c = p->left->leaf->arcLeft->cell;
					lChars[0] = c == NULL? '*': c->ID;
					c = p->left->leaf->arcRight->cell;
					lChars[1] = c == NULL? '*': c->ID;
				}
				if (p->right->node != NULL) memcpy(rChars, zChars, 2 * sizeof(char));
				else
				{
					bpType = p->right->leaf->LeafFlag;
					c = bpType == 'n'? p->right->leaf->arcLeft->cell: NULL;
					rChars[0] = c == NULL? '*': c->ID;
					c = bpType == 'n'? p->right->leaf->arcRight->cell: NULL;
					rChars[1] = c == NULL? '*': c->ID;
				}
				while(pp->leaf == NULL) pp = pp->node->right;
				char good_key = (pp->leaf == p->greatestLeftPoint)?'>':'X';
				bChars[5] = good_key;
				//b_p = StringCbPrintf(txtBuff, 256 * sizeof(char),TEXT("(%02u)"), p->height);
				//memcpy(line + j * w3 + d1, txtBuff, w1 * sizeof(char));
				memcpy(line + j * w3 + d1, bChars, w1 * sizeof(char));
				if (i < n)
				{
					int lftS = j * w3 + w2 / TwoPow[i + 2], rghS = j * w3 + d1 + w1;
					if (p->left != NULL)
					{
						ptrs[lftS] = fslash;
						for (int k = 1; k < wL; ++k) line[lftS + k] = u_s;
					}
					if (p->right != NULL)
					{
						for (int k = 0; k < wR; ++k) line[rghS + k] = u_s;
						ptrs[rghS + wR] = bslash;
					}
				}
			}
		}
		//b_p = StringCbPrintf(buff, (w2 + 10) * sizeof(char), TEXT("%s\r\n"), line);
		//b_p = StringCbLength(buff, (w2 + 10) * sizeof(char), &l);
		//dwBytesToWrite = (DWORD)l;
		//WriteFile(hf, buff, dwBytesToWrite, &dwBytesWritten, NULL);
		hf << line << fChars;
		if (i < n)
		{
			//b_p = StringCbPrintf(buff, (w2 + 10) * sizeof(char), TEXT("%s\r\n"), ptrs);
			//b_p = StringCbLength(buff, (w2 + 10) * sizeof(char), &l);
			//dwBytesToWrite = (DWORD)l;
			//WriteFile(hf, buff, dwBytesToWrite, &dwBytesWritten, NULL);
			hf << ptrs << fChars;
		}
	}
	//b_p = StringCbPrintf(buff, (w2 + 10) * sizeof(char), TEXT("%s\r\n"), blanks);
	//b_p = StringCbLength(buff, (w2 + 10) * sizeof(char), &l);
	//dwBytesToWrite = (DWORD)l;
	//WriteFile(hf, buff, dwBytesToWrite, &dwBytesWritten, NULL);
	//WriteFile(hf, buff, dwBytesToWrite, &dwBytesWritten, NULL);
	hf << blanks << fChars;
	hf << blanks << fChars;
	//CloseHandle(hf);
	//BOOL fSuccess = FlushFileBuffers(hf);
	//DWORD eCode = GetLastError();
	delete [] levels;
	delete [] line;
	delete [] ptrs;
	delete [] blanks;
	delete [] buff;
}





void Fortune::BinaryBPlus::SanityCheck(void)
{
	pArcType aL = leftmost->next, aR = NULL;
	while (aL != rightmost)
	{
		aR = aL->next;
		if (aL->rightBP != NULL) assert(aL->rightBP->arcRight = aR);
		aL = aR;
	}
	aR = rightmost->prev;
	while (aR != leftmost)
	{
		aL = aR->prev;
		if (aR->leftBP != NULL) assert(aR->leftBP->arcLeft = aL);
		aR = aL;
	}

}

Fortune::Voronoi::Voronoi(void)
{
	InProcess = false;
	DiagramComplete = false;
	D.cells = NULL;
	D.halfedges = NULL;
	D.vertices = NULL;
	//BBvtx_i = NULL;
	//BBvtx_f = NULL;
	//ocount = 0;
	//hcount = 0;
	SitePoints = NULL;
	pBB = NULL;
	IsStarting = true;
	bLine = NULL;
	CtlPtCount = 0;
	LinePtCount = 0;
	//h_Wnd = NULL;
	old_directrix = -1.0;
	EventNxt.type = 'N';
	AnimationHold = false;
	//timerID = 1023;
	//p_Draw = NULL;
	//p_NG = NULL;
	//first_point = NULL;
	LinePts = NULL;
	roundDecimal = 3;  // round at this number decimals after the point
}

Fortune::Voronoi::~Voronoi(void)
{

	ClearDiagram();
	ClearSites();
	//if (bLine != NULL) delete [] bLine;
	//if (first_point != NULL) delete first_point;
	pVertexListType v = NULL;
	

}

void Fortune::Voronoi::ClearDiagram(void)
{
	pCellListType c = NULL;
	while (D.cells != NULL)
	{
		c = D.cells->next;
		pCellType c0 = D.cells->cell;
		pHalfEdgeListType hL = c0->InnerComponents;
		while (hL != NULL)
		{
			c0->InnerComponents = c0->InnerComponents->next;
			delete hL;
			hL = c0->InnerComponents;
		}
		delete D.cells;
		D.cells = c;
	}

	pHalfEdgeListType h = NULL;
	while (D.halfedges != NULL)
	{
		h = D.halfedges->next;
		delete D.halfedges;
		D.halfedges = h;
	}

	pVertexListType v = NULL;
	while (D.vertices != NULL)
	{
		v = D.vertices->next;
		delete D.vertices;
		D.vertices = v; 
	}
	DiagramComplete = false;
}

void Fortune::Voronoi::ClearSites(void)
{
	pSitePointListType sL = SitePoints, sL_nxt = NULL;
	while (sL != NULL)
	{
		sL_nxt = sL->next;
		delete sL;
		sL = sL_nxt;
	}
	SitePoints = NULL;
	DiagramComplete = false;
}

void Fortune::Voronoi::SetBoundingBox(double left, double bottom, double right, double top)
{
	if (pBB != NULL) delete pBB;
	pBB = new BoundingBoxType; //(pBoundingBoxType)new BoundingBoxType));
	pBB->bottom = bottom;
	pBB->top = top;
	pBB->right = right;
	pBB->left = left;
}

void Fortune::Voronoi::AddSites(const vector<pPGonWork> * pgons, const vector<CompGeo::XY> & dots)
{
	ClearSites();
	/*
	//debug:
	double delta = M_PI * 2 / 50;

	for (int i = 0; i < 50; ++i)
	{
		double theta = i * delta;
		pSitePointListType sL = (pSitePointListType)new SitePointListType));
		sL->site = 400.0 * CompGeo::XY(cos(theta), sin(theta));
		sL->site.v_Node = NULL;
		sL->site.w_Gon = NULL;
		sL->next = SitePoints;
		SitePoints = sL;
	}
	*/
	
	//pPGonWork wGon = pgons->poly;
	pPGonWork wGon = NULL;
	//for(unsigned int i = 0; i < pgons->numPolygons; ++i)
	if (pgons != NULL)
	{
		for (vector<pPGonWork>::const_iterator pit = pgons->begin(); pit != pgons->end(); ++pit)
		{
			wGon = *pit;
			pVertexNode vtxNde = wGon->vNode;
			for (unsigned int j = 0; j < wGon->numVertices; ++j)
			{
				//s_in[i].x = CompGeo::Rounding(s_in[i].x, roundDecimal);
				//s_in[i].y = CompGeo::Rounding(s_in[i].y, roundDecimal);
				pSitePointListType sL = new SitePointListType; //(pSitePointListType)new SitePointListType));
				sL->site = vtxNde->vtxInfo.vertex;
				sL->site.v_Node = vtxNde;
				sL->site.w_Gon = wGon;
				sL->next = SitePoints;
				SitePoints = sL;
				vtxNde = vtxNde->next;
			}
			//wGon = wGon->next;
		}
	}
	for (vector<CompGeo::XY>::const_iterator dit = dots.begin(); dit != dots.end(); ++dit)
	{
		pSitePointListType dsL = new SitePointListType;
		dsL->site = *dit;
		dsL->site.v_Node = NULL;
		dsL->site.w_Gon = NULL;
		dsL->next = SitePoints;
		SitePoints = dsL;
	}
	
}

void Fortune::Voronoi::Run(void) //Gtk::Allocation b_b)
{
	//h_Wnd = hwnd;
	//p_Draw = pdraw;
	//InProcess = true;
	//bb = b_b;
	int i;
	cellIDbase = (int)L'A';
	pSitePointListType sL = SitePoints;
	i = 0;
	//SetBoundingBox(bb.left, bb.top, bb.right, bb.bottom);  //deliberately flipped top and bottom
	while (sL != NULL)
	{
		CompGeo::XY xy(sL->site.x, sL->site.y);
		if (ValidSite(xy))
		{
			++i;
			pEventQPoint sPt = new EventQPoint(sL->site), sPtBuff = sPt;
			bool Inserted = Q.Insert(sPt);
			if (!Inserted) delete sPtBuff;
		}
		sL = sL->next;
	}


	if (IsStarting) IsStarting = false;
	else
	{
		T.Reset();
		ClearDiagram();
		//ClearSites();
	}
	if (i == 0) return;  // no points
	bool IsEmpty = false;
	EventNxt = Q.GetLeast(IsEmpty);
	while (!IsEmpty)
	{
		AttendToTree();
		EventNxt = Q.GetLeast(IsEmpty);
	}
	Finalize();
	//InProcess = false;
	DiagramComplete = true;
}

bool Fortune::Voronoi::InBounds(CompGeo::XY xy)
{
	return (xy.x <= (pBB->right + MAX_FLT_PRECISION)) && 
		(xy.x >= (pBB->left - MAX_FLT_PRECISION)) && 
		(xy.y <= (pBB->top + MAX_FLT_PRECISION)) && 
		(xy.y >= (pBB->bottom - MAX_FLT_PRECISION));
}

bool Fortune::Voronoi::ValidSite(CompGeo::XY xy)
{ // site points must be within (not on) the bounding box
	return (xy.x <= (pBB->right - MAX_FLT_PRECISION)) && 
		(xy.x >= (pBB->left + MAX_FLT_PRECISION)) && 
		(xy.y <= (pBB->top - MAX_FLT_PRECISION)) && 
		(xy.y >= (pBB->bottom + MAX_FLT_PRECISION));

}

int Fortune::Voronoi::InCorner(CompGeo::XY xy)
{// 0=bottomright, 1=topright, 2=topleft, 3=bottomleft, -1=not
	if (!InBounds(xy)) return -1;
	bool OnRight = (fabs(xy.x - pBB->right) < MAX_FLT_PRECISION), 
		OnTop = (fabs(xy.y - pBB->top) < MAX_FLT_PRECISION), 
		OnLeft = (fabs(xy.x - pBB->left) < MAX_FLT_PRECISION), 
		OnBottom = (fabs(xy.y - pBB->bottom) < MAX_FLT_PRECISION);
	if (OnBottom && OnRight) return 0;
	if (OnRight && OnTop) return 1;
	if (OnTop && OnLeft) return 2;
	if (OnLeft && OnBottom) return 3;
	return -1;
}

int Fortune::Voronoi::OnBox(CompGeo::XY xy)
{// returns 0=right, 1=top, 2=left, 3=bottom, -1=not
	if (!InBounds(xy)) return -1;
	bool OnRight = (fabs(xy.x - pBB->right) < MAX_FLT_PRECISION), 
		OnTop = (fabs(xy.y - pBB->top) < MAX_FLT_PRECISION), 
		OnLeft = (fabs(xy.x - pBB->left) < MAX_FLT_PRECISION), 
		OnBottom = (fabs(xy.y - pBB->bottom) < MAX_FLT_PRECISION);
	if (OnBottom && OnRight) return 0;
	if (OnRight && OnTop) return 1;
	if (OnTop && OnLeft) return 2;
	if (OnLeft && OnBottom) return 3;
	if (OnRight) return 0;
	if (OnTop) return 1;
	if (OnLeft) return 2;
	if (OnBottom) return 3;
	return -1;
}

void Fortune::Voronoi::DeleteHalfEdgeFromList(pHalfEdgeType h)
{
	pCellType c = h->IncidentCell;
	pHalfEdgeListType hL = c->InnerComponents, hLTrail = NULL;
	while (hL != NULL)
	{
		if(hL->halfEdge == h)
		{
			if (hLTrail == NULL) c->InnerComponents = hL->next;
			else hLTrail->next = hL->next;
			delete hL->halfEdge;
			delete hL;
			return;
		}
		hLTrail = hL;
		hL = hL->next;
	}
	assert(false);
}

void Fortune::Voronoi::AddDupRecord(pDupType & root, const pDupType recIn)
{ // adds record & enforces order by point and then by cell address

	pDupType trv = root, trail = NULL;
	recIn->next_d = NULL;
	while (trv != NULL)
	{
		bool eqlPts = (recIn->xy == trv->xy);
		pCellType cIn = recIn->h_next->IncidentCell, cTrv = trv->h_next->IncidentCell;
		if (((eqlPts) && (cIn < cTrv)) || ((!eqlPts) && (recIn->xy < trv->xy))) break;
		trail = trv;
		trv = trv->next_d;
	}
	if (trail == NULL) root = recIn;
	else trail->next_d = recIn;
	recIn->next_d = trv;
}

void Fortune::Voronoi::CheckForDups(pHalfEdgeType h_0, pDupType & root)
{
	pDupType d = NULL;
	pHalfEdgeType h0 = h_0;
	assert (h0 != NULL);
	while (h0->origin->xy == h0->twin->origin->xy) 
	{
		h0 = h0->prev; // start with a non-zero-length edge
	}
	pHalfEdgeType h = h0, hn = h->next, hIn = NULL, hOut = NULL, hOOO = NULL, hp = NULL;
	pCellType c = h0->IncidentCell;
	// reset cell component: works for Voronoi because only ext cell has an InnerComponent
	if (c->OuterComponent == NULL) c->InnerComponents->halfEdge = h0;
	else c->OuterComponent = h0;
	bool StillGoing = true, InProcessBuild = false, foundGroup = false;
	do
	{ // 1st ensure leading & trailing edges into zero-length groups are non-zero length themselves
		if (h->origin->xy == hn->origin->xy)
		{
			if (hIn == NULL) 
			{
				hIn = h->prev;
				hOut = hn;
				if (!(h->origin->xy == h->twin->origin->xy)) hOOO = h;
				InProcessBuild = true;
				foundGroup = true;
			}
			else
			{
				hOut = hn;
				if (!(h->origin->xy == h->twin->origin->xy)) 
				{
					assert (hOOO == NULL); // there can be only one
					hOOO = h;
				}
			}
		}
		else
		{
			if (hOut != NULL)
			{
				if (hOOO != NULL)
				{
					assert (hOut->origin->xy == hOut->twin->origin->xy);
					hp = hOut->prev;
					if (hp == hOOO)
					{
						hp = hOOO->prev;
						hp->next = hOut;
						hOut->next = hOOO;
						hOOO->next = hn;
						hn->prev = hOOO;
						hOOO->prev = hOut;
						hOut->prev = hp;
					}
					else
					{
						hOut->next = hOOO->next;
						hOut->prev = hOOO->prev;
						hOOO->next->prev = hOut;
						hOOO->prev->next = hOut;
						hOOO->next = hn;
						hn->prev = hOOO;
						hp->next = hOOO;
						hOOO->prev = hp;
					}
					hOut = hOOO;
					hOOO = NULL;
				}
				hIn = NULL;
				hOut = NULL;
				InProcessBuild = false;
			}
		}
		h = hn; 
		hn = hn->next;
		StillGoing = StillGoing && (h != h0);
		
	} while (StillGoing || InProcessBuild);

	if (foundGroup)
	{
		h = h0; hn = h->next; hIn = NULL; hOut = NULL; hOOO = NULL; hp = NULL;
		StillGoing = true; InProcessBuild = false;

		do
		{ // now build list
			if (h->origin->xy == hn->origin->xy)
			{
				if (hIn == NULL) 
				{
					hIn = h->prev;
					assert (d == NULL);
					d = new DupType; //(pDupType)new DupType));
					d->h_prev = hIn;
					d->singles = NULL;
					d->vtxLstElement = new VertexListType; //(pVertexListType)new VertexListType));
					d->vtxLstElement->next = D.vertices; D.vertices = d->vtxLstElement;
					d->vtxLstElement->vertex.IncidentEdge = hIn->twin;
					d->xy = h->origin->xy;
					d->vtxLstElement->vertex.xy = d->xy;
					d->dups = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
					d->dups->halfEdge = h;
					d->dups->next = NULL;
					d->next_d = NULL;
					hOut = hn;
					assert (h->origin->xy == h->twin->origin->xy);
					InProcessBuild = true;
				}
				else
				{
					pHalfEdgeListType hdL = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
					hdL->halfEdge = h;
					hdL->next = d->dups;
					d->dups = hdL;
					hOut = hn;
					assert(h->origin->xy == h->twin->origin->xy); 
				}
			}
			else
			{
				if (hOut != NULL)
				{
					d->h_next = hOut;
					AddDupRecord(root, d);
					d = NULL;
					hIn = NULL;
					hOut = NULL;
					InProcessBuild = false;
				}
			}
			h = hn; 
			hn = hn->next;
			StillGoing = StillGoing && (h != h0);
		
		} while (StillGoing || InProcessBuild);
	}
}

void Fortune::Voronoi::RoundVertices(void)
{	// sets vtx_count
	vtx_count = 0;
	pVertexListType vL = D.vertices;

	while (vL != NULL)
	{
		++vtx_count;
		double x = vL->vertex.xy.x;
		vL->vertex.xy.x = CompGeo::Rounding(x, roundDecimal);
		double y = vL->vertex.xy.y;
		vL->vertex.xy.y = CompGeo::Rounding(y, roundDecimal);
		vL = vL->next;
	}

}

void Fortune::Voronoi::DebugPrintLoop(pHalfEdgeType h0)
{
	//HRESULT b_p;
	//char desc[256];
	string desc;
	pHalfEdgeType h = h0;
	int i = 0;
	do
	{
		desc = to_string(++i) + ":(" + to_string(h->origin->xy.x) + ", " + to_string(h->origin->xy.y) + ")@" + to_string((long)h->origin);
		//b_p = StringCbPrintf(desc, 256 * sizeof(char), L" %u:(%f, %f)@%p ", 
		//	++i, h->origin->xy.x, h->origin->xy.y, h->origin);
		//OutputDebugString(desc);
		cout << desc << "\n";
		h = h->next;
	} while (h != h0);
}

void Fortune::Voronoi::DebugPrintHalfEdges(void)
{
	//HRESULT b_p;
	//char desc[256];
	string desc;
	
	pCellListType cL = D.cells;
	while (cL != NULL)
	{
		pCellType c = cL->cell;
		desc = (string)"HalfEdges around Cell " + c->ID + " w/ site (" + to_string(c->site == NULL? 0.0: c->site->x) + ", " + 
			to_string(c->site == NULL? 0.0: c->site->y) + ")\r\n";
		//b_p = StringCbPrintf(desc, 256 * sizeof(char), L"HalfEdges around Cell %c w/ site (%f, %f)\r\n", 
		//	c->ID, c->site == NULL? 0.0: c->site->x, c->site==NULL?0.0:c->site->y);
		//OutputDebugString(desc);
		cout << desc;
		pHalfEdgeListType hL = c->InnerComponents;
		pHalfEdgeType h0 = NULL;
		while (hL != NULL)
		{
			h0 = hL->halfEdge;
			DebugPrintLoop(h0);
			hL = hL->next;
		}
		h0 = c->OuterComponent;
		if (h0 != NULL) DebugPrintLoop(h0);
		cL = cL->next;
		//b_p = StringCbPrintf(desc, 256 * sizeof(char), L"\r\n\r\n");
		//OutputDebugString(desc);
		cout << "\r\n\r\n";
	}


}

void Fortune::Voronoi::CheckForSingles(pHalfEdgeType h0, pDupType & root)
{
	if (root == NULL) return;
	if (h0 == NULL) return;
	if (h0->origin == NULL) return;

	SitePoint P0 = h0->origin->xy;
	

	pDupType trv = root;
	bool eqlPts = false;
	while (trv != NULL)
	{
		if (P0 == trv->xy)
		{
			eqlPts = true;
			break;
		}
		if (P0 <trv->xy) break;
		trv = trv->next_d;
	}
	if (eqlPts)
	{
		pHalfEdgeType h = h0;
		pHalfEdgeListType hL = NULL, hLTrail = NULL;
		bool PoorForm = false;
		do
		{
			hL = trv->singles;
			hLTrail = NULL;
			while (hL != NULL)
			{
				if (h == hL->halfEdge)
				{
					PoorForm = true;
					break;
				}
				if (h < hL->halfEdge) break;
				hLTrail = hL;
				hL = hL->next;
			}
			if (PoorForm) break;
			pHalfEdgeListType hA = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
			hA->halfEdge = h;
			hA->next = hL;
			if (hLTrail == NULL) trv->singles = hA;
			else hLTrail->next = hA;
			h = h->prev->twin;
		} while (h != h0);
	}
}

void Fortune::Voronoi::ResetAllHalfEdgeOrigins(pHalfEdgeType h0, const pVertexType & vtx_n)
{
	SitePoint P0 = vtx_n->xy;
	pHalfEdgeType h = h0;
	// should not be more than 3 halfedges tied to a vertex
	for (int i = 0; i < 3; ++i)
	{
		if (!(h->origin->xy == P0)) break;
		h->origin = vtx_n;
		h = h->prev->twin;
	}
}

void Fortune::Voronoi::CleanUpVertices(void)
{
	pVertexListOfListType pvLOLRoot = NULL, pvLOL = NULL, pvLOLTrail = NULL;
	pVertexListType vL = D.vertices, vLTrail = NULL, vLNext = NULL;
	int vCount = vtx_count, vDels = 0, i;
	//HRESULT b_p;
	//char desc[256];
	
	pVertexPoint vPts = new VertexPoint[vCount];

	//b_p = StringCbPrintf(desc, 256 * sizeof(char), L"pVertexPoint Array @ %p \r\n", vPts);
	//OutputDebugString(desc);

	vL = D.vertices;
	for (i = 0; i < vCount; ++i)
	{
		vPts[i] = VertexPoint(vL, vLTrail);
		vLTrail = vL;
		vL = vL->next;
	}
	Sorter<VertexPoint> s(vPts, 0, vCount - 1, vCount);
	s.doSort();
	/*
	b_p = StringCbPrintf(desc, 256 * sizeof(char), L"Sorted VertexPoint Array:\r\n");
	OutputDebugString(desc);
	for (i = 0; i < vCount; ++i)
	{
		b_p = StringCbPrintf(desc, 256 * sizeof(char), L"\t %u=(%f, %f)", 
			i + 1, vPts[i].vl->vertex.x, vPts[i].vl->vertex.y);
		OutputDebugString(desc);
	}
	b_p = StringCbPrintf(desc, 256 * sizeof(char), L"\r\n");
	OutputDebugString(desc);
	*/
	pDupType allDups = NULL;
	pCellType c = NULL;
	pCellListType cL = D.cells;
	while (cL != NULL)
	{ // build dup list:
		pHalfEdgeType h0 = NULL; 
		c = cL->cell;
		pHalfEdgeListType hL = c->InnerComponents;
		while (hL != NULL)
		{
			h0 = hL->halfEdge; 
			CheckForDups(h0, allDups);
			hL = hL->next;
		} // done inner components
		h0 = c->OuterComponent;
		if (h0 != NULL) CheckForDups(h0, allDups);
		cL = cL->next;
	}

	//pHalfEdgeListType upDateLst = NULL, updl = NULL, removeLst = NULL, rmv = NULL;
	i = 0;
	int span = 1;
	pDupType dupTrav = allDups;
	/*
	b_p = StringCbPrintf(desc, 256 * sizeof(char), L"Sorted Duplicate List:\r\n");
	OutputDebugString(desc);
	//for (i = 0; i < vCount; ++i)
	i = 0;
	while (dupTrav != NULL)
	{
		int j = 0;
		pHalfEdgeListType hL = dupTrav->dups;
		while (hL != NULL)
		{
			++j;
			hL = hL->next;
		}
		b_p = StringCbPrintf(desc, 256 * sizeof(char), 
			L"\t Record # %u for cell %c around (%f, %f) with %u duplicate point(s): (%f, %f)\r\n", 
			++i, dupTrav->h_prev->IncidentCell->ID, 
			dupTrav->h_prev->IncidentCell->site->x, dupTrav->h_prev->IncidentCell->site->y,
			j, dupTrav->xy.x, dupTrav->xy.y);
		OutputDebugString(desc);
		dupTrav = dupTrav->next_d;
	}
	*/
	//dupTrav = allDups;
	i = 0;
	while (i < vCount)
	//while (dupTrav != NULL)
	{
		vL = vPts[i].vl;
		//pHalfEdgeType hTop = vL->IncidentEdge;
		//int idxNxt = (i + span) % vCount; 
		//vLNext = vPts[idxNxt].vl;
		if (InBounds(vL->vertex.xy))
		{
			SitePoint P0 = vL->vertex.xy, P1 = P0;
			if (dupTrav == NULL) ++i;
			else
			{
				if (dupTrav->xy != P0) ++i;
				else
				{
					int idxNxt = i;
					pVertexType v = &dupTrav->vtxLstElement->vertex;
					while (dupTrav->xy == P1)
					{
						vLTrail = vPts[idxNxt].vl_prev;
						while (vLTrail != NULL)
						{
							if (vLTrail->next == vL) break;
							vLTrail = vLTrail->next;
						}
						if (vLTrail == NULL) D.vertices = vL->next;
						else vLTrail->next = vL->next;
						vL->next = vLTrail; // reverse order
						pvLOL = new VertexListOfListType; //(pVertexListOfListType)new VertexListOfListType));
						pvLOL->vListElement = vL;
						pvLOL->next = pvLOLRoot;
						pvLOLRoot = pvLOL;
						++vDels;
						idxNxt = (i + span) % vCount;
						++span;
						ResetAllHalfEdgeOrigins(vL->vertex.IncidentEdge, v);
						vL = vPts[idxNxt].vl;
						P1 = vL->vertex.xy;
					}

					while (dupTrav->xy == P0)
					{
						dupTrav = dupTrav->next_d;
						if (dupTrav == NULL) break;
					}
					i += span;
					span = 1;
				}
			}
		}
		else // vertex(vL) out of bounds
		{
			vLTrail = vPts[i].vl_prev;
			if (vLTrail != NULL)
			{
				while (vLTrail->next != vL)
				{
					vLTrail = vLTrail->next;
					if (vLTrail == NULL) break;
				}
			}
			if (vLTrail == NULL) D.vertices = vL->next;
			else vLTrail->next = vL->next;
			vL->next = vLTrail; // reverse order
			pvLOL = new VertexListOfListType; //(pVertexListOfListType)new VertexListOfListType));

//	b_p = StringCbPrintf(desc, 256 * sizeof(char), L"OOB:pVertexListOfListType @ %p\r\n", pvLOL);
//	OutputDebugString(desc);

			pvLOL->vListElement = vL;
			pvLOL->next = pvLOLRoot;
			pvLOLRoot = pvLOL;
			++vDels;
			++i;
		}
	}

	delete [] vPts;
	//DebugPrintHalfEdges();
	pvLOL = pvLOLRoot;
	while (pvLOL != NULL)
	{
		delete pvLOL->vListElement;
		pvLOL = pvLOL->next;
	}
	pvLOL = pvLOLRoot;
	while (pvLOL != NULL)
	{
		pvLOLTrail = pvLOL;
		pvLOL = pvLOL->next;
		delete pvLOLTrail;
	}

	
	pVertexType v0 = NULL;
	CompGeo::XY cPt;
	bool beginGroup = true;
	dupTrav = allDups;
	while (dupTrav != NULL)
	{
		if (beginGroup)
		{
			cPt = dupTrav->xy;
			beginGroup = false;
			//pVertexListType vL = (pVertexListType)new VertexListType));
			//vL->next = D.vertices; D.vertices = vL;
			//vL->vertex.xy = cPt;
			//vL->vertex.IncidentEdge = dupTrav->h_next;
			v0 = &dupTrav->vtxLstElement->vertex;
		}
		dupTrav->h_next->origin = v0;
		dupTrav->h_prev->twin->origin = v0;
		dupTrav->h_prev->next = dupTrav->h_next;
		dupTrav->h_next->prev = dupTrav->h_prev;
		pHalfEdgeListType hL = dupTrav->dups;
		while (hL != NULL)
		{
			delete hL->halfEdge;
			hL = hL->next;
		}

		dupTrav = dupTrav->next_d;
		if (dupTrav != NULL) beginGroup = (cPt != dupTrav->xy);

	}

	dupTrav = allDups;
	while (dupTrav != NULL)
	{
		pHalfEdgeListType hL = dupTrav->dups;
		while (hL != NULL)
		{
			hL = hL->next;
			delete dupTrav->dups;
			dupTrav->dups = hL;
		}
		dupTrav = dupTrav->next_d;
		delete allDups;
		allDups = dupTrav;
	}
	/**/
	//DebugPrintHalfEdges();

}

void Fortune::Voronoi::DiagramHalfEdges(void)
{
	pHalfEdgeListType hL = D.halfedges, hLN = NULL;
	while (hL != NULL)
	{
		hLN = hL->next;
		delete hL;
		hL = hLN;
	}
	D.halfedges = NULL;

	pCellListType cL = D.cells;
	while (cL != NULL)
	{
		pCellType c = cL->cell;
		hL = c->InnerComponents;
		pHalfEdgeType h0 = NULL, h = NULL;
		while (hL != NULL)
		{
			h0 = hL->halfEdge;
			h = h0;
			do
			{
				pHalfEdgeListType dhL = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
				dhL->next = D.halfedges;
				D.halfedges = dhL;
				dhL->halfEdge = h;
				h = h->next;
			} while (h != h0);
			hL = hL->next;
		}
		h0 = c->OuterComponent;
		if (h0 != NULL)
		{
			h = h0;
			do
			{
				pHalfEdgeListType dhL = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
				dhL->next = D.halfedges;
				D.halfedges = dhL;
				dhL->halfEdge = h;
				h = h->next;
			} while (h != h0);
		}
		cL = cL->next;
	}
}

// next method takes arrays to 4 R's, t's, InSeg's, an R0 & an L to solve the line equations
// for the bounding box {(right, y), (x, top), (left, y), (x, bottom)} R = R0 + tL
void Fortune::Voronoi::GetBBSolutions(CompGeo::pXY ePts, double * t, bool * OnSegment, 
	const CompGeo::XY & R0, const CompGeo::XY & L)
{
	// arrays will be 0-3 for right, top, left, bottom 
	// need to be careful that at most 2 solutions are returned
	// right & left
	int iCrn = -1;
	if (fabs(L.x) >= MAX_FLT_PRECISION)
	{
		t[0] = (pBB->right - R0.x) / L.x;
		ePts[0] = R0 + t[0] * L;
		iCrn = InCorner(ePts[0]);
		OnSegment[0] = iCrn == -1? ((ePts[0].y >= pBB->bottom) && (ePts[0].y < pBB->top)): (iCrn == 0);
		t[2] = (pBB->left - R0.x) / L.x;
		ePts[2] = R0 + t[2] * L;
		iCrn = InCorner(ePts[2]);
		OnSegment[2] = iCrn == -1? ((ePts[2].y > pBB->bottom) && (ePts[2].y <= pBB->top)): (iCrn == 2);
	}
	// top & bottom
	if (fabs(L.y) >= MAX_FLT_PRECISION)
	{
		t[1] = (pBB->top - R0.y) / L.y;
		ePts[1] = R0 + t[1] * L;
		iCrn = InCorner(ePts[1]);
		OnSegment[1] = iCrn == -1? ((ePts[1].x > pBB->left) && (ePts[1].x <= pBB->right)): (iCrn == 1);
		t[3] = (pBB->bottom - R0.y) / L.y;
		ePts[3] = R0 + t[3] * L;
		iCrn = InCorner(ePts[3]);
		OnSegment[3] = iCrn == -1? ((ePts[3].x >= pBB->left) && (ePts[3].x < pBB->right)): (iCrn == 3);
	}
}

void Fortune::Voronoi::ResolveEdge(pLinkParameterType pLP)
{
	pLP->makeTwin1 = false;
	pHalfEdgeType h1 = pLP->hL->halfEdge, h2 = h1->twin, hBuff = NULL;
	pVertexType v1 = h1->origin, v2 = h2->origin;
	pLP->P1 = *h1->IncidentCell->site;
	pLP->P2 = *h2->IncidentCell->site;
	CompGeo::XY d1 = pLP->P1 - pLP->P2, d2 = pLP->P2 - pLP->P1;
	CompGeo::XY resetPt(0.0, 0.0);
	for (int i = 0; i < 4; ++i)
	{ // reset loop:
		pLP->ePt[i] = resetPt;
		pLP->OnSeg[i] = false;
		pLP->t[0] = 0.0;
	}

	CompGeo::XY A = pLP->P1, B = pLP->P2; //, C = CompGeo::XY(270.0, 982.0);
	//bool ckPt = ((A == C) || (B == C));
	if ((v1 == NULL) || (v2 == NULL))
	{
		if ((v1 == NULL) && (v2 == NULL)) ResolveNull2Null(pLP);
		else
		{
			if (v1 == NULL)
			{
				if (!InBounds(v2->xy))
				{
					pLP->R0 = v2->xy;
					pLP->d = d1;
					ResolveNull2Out(pLP);
				}
				else // v2 is in BB
				{
					pLP->R0 = v2->xy;
					pLP->d = d1;
					ResolveNull2In(pLP);
				}
			}
			else
			{
				pLP->makeTwin1 = true;
				hBuff = h2;
				h2 = h1;
				h1 = hBuff;
				v1 = h1->origin;
				v2 = h2->origin;
				pLP->P1 = *h1->IncidentCell->site;
				pLP->P2 = *h2->IncidentCell->site;
				CompGeo::XY dBuff = d2;
				d2 = d1;
				d1 = dBuff;
				if (!InBounds(v2->xy))
				{
					pLP->R0 = v2->xy;
					pLP->d = d1;
					ResolveNull2Out(pLP);
				}
				else // v2 is in BB
				{
					pLP->R0 = v2->xy;
					pLP->d = d1;
					ResolveNull2In(pLP);
				}
			}
		}
	}
	else // both origins are non-NULL
	{
		bool In1 = InBounds(v1->xy), In2 = InBounds(v2->xy);
		if ((In1) && (In2))
		{
			if (OnBox(v1->xy) != -1)
			{
				pHalfEdgeListType ihl = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
				ihl->halfEdge = h1;
				ihl->next = pLP->Intersectors;
				pLP->Intersectors = ihl;
				++pLP->iCount;
			}
			if (OnBox(v2->xy) != -1)
			{
				pHalfEdgeListType ihl = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
				ihl->halfEdge = h2;
				ihl->next = pLP->Intersectors;
				pLP->Intersectors = ihl;
				++pLP->iCount;
			}
			pLP->hLPrev = pLP->hL;
			pLP->hL = pLP->hL->next;
			++pLP->eCount;
		}
		else
		{
			if (In2) // v1 is out v2 is in
			{
				pLP->R0 = v2->xy;
				pLP->d = d1;
				ResolveOut2In(pLP);
			}
			else
			{
				if (In1)
				{
					pLP->makeTwin1 = true;
					hBuff = h2;
					h2 = h1;
					h1 = hBuff;
					v1 = h1->origin;
					v2 = h2->origin;
					pLP->P1 = *h1->IncidentCell->site;
					pLP->P2 = *h2->IncidentCell->site;
					CompGeo::XY dBuff = d2;
					d2 = d1;
					d1 = dBuff;
					pLP->R0 = v2->xy;
					pLP->d = d1;
					ResolveOut2In(pLP);
				}
				else ResolveOut2Out(pLP);
			}
		}
	}
}

void Fortune::Voronoi::ResolveNull2Null(pLinkParameterType pLP)
{
	pLP->R0 = (pLP->P1 + pLP->P2) / 2.0;  //midpoint between P1 & P2
	pLP->d = pLP->P1 - pLP->P2; // i.e. d1
	CompGeo::XY L(pLP->d.y, -pLP->d.x);
	GetBBSolutions(pLP->ePt, pLP->t, pLP->OnSeg, pLP->R0, L);
	int solIdx[2] = {-1}, count = 0, iBuff = 0;
	for (int i = 0; i < 4; ++i)
	{
		if (pLP->OnSeg[i])
		{
			assert (count < 2);
			solIdx[count++] = i;
		}
	}
	assert (count == 2);  // there have to be 2 solutions (one + & one -)
	if (pLP->t[solIdx[0]] > pLP->t[solIdx[1]])
	{
		iBuff = solIdx[0];
		solIdx[0] = solIdx[1];
		solIdx[1] = iBuff;
	}
	pHalfEdgeType h1 = pLP->hL->halfEdge, h2 = h1->twin;
	pVertexListType vL1 = new VertexListType, //(pVertexListType)new VertexListType)),
		vL2 = new VertexListType; //(pVertexListType)new VertexListType));
	vL1->vertex.IncidentEdge = h1;
	vL1->vertex.xy = pLP->ePt[solIdx[0]]; h1->origin = &vL1->vertex;
	vL1->next = D.vertices;
	//if (BBvtx_f == NULL) BBvtx_f = vL1;
	vL2->vertex.IncidentEdge = h2;
	vL2->vertex.xy = pLP->ePt[solIdx[1]]; h2->origin = &vL2->vertex;
	vL2->next = vL1;
	D.vertices = vL2;
	pHalfEdgeListType ihL1 = new HalfEdgeListType, //(pHalfEdgeListType)new HalfEdgeListType)),
		ihL2 = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
	ihL1->halfEdge = h1;
	ihL1->next = pLP->Intersectors;
	ihL2->halfEdge = h2;
	ihL2->next = ihL1;
	pLP->Intersectors = ihL2;
	pLP->iCount += 2;
	pLP->hLPrev = pLP->hL;
	pLP->hL = pLP->hL->next;
	++pLP->eCount;
}

void Fortune::Voronoi::ResolveNull2Out(pLinkParameterType pLP)
{
	CompGeo::XY L(pLP->d.y, -pLP->d.x);
	GetBBSolutions(pLP->ePt, pLP->t, pLP->OnSeg, pLP->R0, L);
	int solIdx[2] = {-1}, count = 0, iBuff = 0;
	pHalfEdgeType h1 = NULL, h2 = NULL;
	for (int i = 0; i < 4; ++i)
	{
		if ((pLP->OnSeg[i]) && (pLP->t[i] < 0.0))
		{
			assert (count < 2);
			solIdx[count++] = i;
		}
	}
	if (count == 2)  // 2 solutions both < 0 -> intersection w/ BB
	{
		if (pLP->t[solIdx[0]] > pLP->t[solIdx[1]])
		{
			iBuff = solIdx[0];
			solIdx[0] = solIdx[1];
			solIdx[1] = iBuff;
		}
		if (pLP->makeTwin1)
		{
			h2 = pLP->hL->halfEdge;
			h1 = h2->twin;
		}
		else
		{
			h1 = pLP->hL->halfEdge;
			h2 = h1->twin;
		}
		pVertexListType vL1 = new VertexListType, //(pVertexListType)new VertexListType)),
			vL2 = new VertexListType; //(pVertexListType)new VertexListType));
		vL1->vertex.IncidentEdge = h1;
		vL1->vertex.xy = pLP->ePt[solIdx[0]]; h1->origin = &vL1->vertex;
		vL1->next = D.vertices;
		//if (BBvtx_f == NULL) BBvtx_f = vL1;
		vL2->vertex.IncidentEdge = h2;
		vL2->vertex.xy = pLP->ePt[solIdx[1]]; h2->origin = &vL2->vertex;
		vL2->next = vL1;
		D.vertices = vL2;
		pHalfEdgeListType ihL1 = new HalfEdgeListType, //(pHalfEdgeListType)new HalfEdgeListType)),
			ihL2 = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
		ihL1->halfEdge = h1;
		ihL1->next = pLP->Intersectors;
		ihL2->halfEdge = h2;
		ihL2->next = ihL1;
		pLP->Intersectors = ihL2;
		pLP->iCount += 2;
		pLP->hLPrev = pLP->hL;
		pLP->hL = pLP->hL->next;
		++pLP->eCount;
	}
	else // count = 0, edge does not intersect BB
	{
		assert (count == 0);
		pHalfEdgeListType hLDel = pLP->hL;
		pHalfEdgeType h1 = hLDel->halfEdge, h2 = h1->twin;
		pLP->hL = pLP->hL->next;
		if (pLP->hLPrev == NULL) h1->IncidentCell->InnerComponents = pLP->hL;
		else pLP->hLPrev->next = pLP->hL;
		delete h1;
		DeleteHalfEdgeFromList(h2);
		delete hLDel;
	}

}

void Fortune::Voronoi::ResolveNull2In(pLinkParameterType pLP)
{
	CompGeo::XY L(pLP->d.y, -pLP->d.x);
	GetBBSolutions(pLP->ePt, pLP->t, pLP->OnSeg, pLP->R0, L);
	int solIdx[2] = {-1}, count = 0, iBuff = 0;
	pHalfEdgeType h1 = NULL, h2 = NULL;
	for (int i = 0; i < 4; ++i)
	{
		if ((pLP->OnSeg[i]) && (pLP->t[i] < 0.0))
		{
			assert (count < 2);
			solIdx[count++] = i;
		}
	}
	assert (count == 1); // exactly 1 solution here
	if (pLP->makeTwin1)
	{
		h2 = pLP->hL->halfEdge;
		h1 = h2->twin;
	}
	else
	{
		h1 = pLP->hL->halfEdge;
		h2 = h1->twin;
	}
	pVertexListType vL1 = new VertexListType; //(pVertexListType)new VertexListType));
	vL1->vertex.IncidentEdge = h1;
	vL1->vertex.xy = pLP->ePt[solIdx[0]]; h1->origin = &vL1->vertex;
	vL1->next = D.vertices;
	//if (BBvtx_f == NULL) BBvtx_f = vL1;
	D.vertices = vL1;
	if (OnBox(pLP->R0) != -1) // this is the in-BB vertex
	{
		pHalfEdgeListType ihL = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
		ihL->halfEdge = h2;
		ihL->next = pLP->Intersectors;
		pLP->Intersectors = ihL;
		++pLP->iCount;
	}
	pHalfEdgeListType ihL = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
	ihL->halfEdge = h1;
	ihL->next = pLP->Intersectors;
	pLP->Intersectors = ihL;
	++pLP->iCount;
	pLP->hLPrev = pLP->hL;
	pLP->hL = pLP->hL->next;
	++pLP->eCount;
}

void Fortune::Voronoi::ResolveOut2Out(pLinkParameterType pLP)
{
	// test for intersection with BB
	pHalfEdgeType h1 = pLP->hL->halfEdge, h2 = h1->twin;
	CompGeo::XY 
		P1= h1->origin->xy, P2 = h2->origin->xy, L = P2 - P1;
	pLP->R0 = P1;
	GetBBSolutions(pLP->ePt, pLP->t, pLP->OnSeg, pLP->R0, L);
	int solIdx[2] = {-1}, count = 0, iBuff = 0;
	for (int i = 0; i < 4; ++i)
	{
		if ((pLP->OnSeg[i]) && (pLP->t[i] > 0.0) && (pLP->t[i] < 1.0))
		{
			assert (count < 2);
			solIdx[count++] = i;
		}
	}
	if (count == 0)  // no intersection with BB
	{
		pHalfEdgeListType hLDel = pLP->hL;
		pHalfEdgeType h1 = hLDel->halfEdge, h2 = h1->twin;
		pLP->hL = pLP->hL->next;
		if (pLP->hLPrev == NULL) h1->IncidentCell->InnerComponents = pLP->hL;
		else pLP->hLPrev->next = pLP->hL;
		delete h1;
		DeleteHalfEdgeFromList(h2);
		delete hLDel;
	}
	else 
	{
		assert (count == 2);	// intersects with BB in 2 places
		if (pLP->t[solIdx[0]] > pLP->t[solIdx[1]])
		{
			iBuff = solIdx[1];
			solIdx[1] = solIdx[0];
			solIdx[0] = iBuff;
		}
		pVertexListType vL1 = new VertexListType, //(pVertexListType)new VertexListType)),
			vL2 = new VertexListType; //(pVertexListType)new VertexListType));
		vL1->vertex.IncidentEdge = h1;
		vL1->vertex.xy = pLP->ePt[solIdx[0]]; h1->origin = &vL1->vertex;
		vL1->next = D.vertices;
		vL2->vertex.IncidentEdge = h2;
		vL2->vertex.xy = pLP->ePt[solIdx[1]]; h2->origin = &vL2->vertex;
		vL2->next = vL1;
		//if (BBvtx_f == NULL) BBvtx_f = vL2;
		D.vertices = vL2;
		pHalfEdgeListType ihL1 = new HalfEdgeListType, //(pHalfEdgeListType)new HalfEdgeListType)),
			ihL2 = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
		ihL1->halfEdge = h1;
		ihL1->next = pLP->Intersectors;
		ihL2->halfEdge = h2;
		ihL2->next = ihL1;
		pLP->Intersectors = ihL2;
		pLP->iCount += 2;
		pLP->hLPrev = pLP->hL;
		pLP->hL = pLP->hL->next;
		++pLP->eCount;

	}
}

void Fortune::Voronoi::ResolveOut2In(pLinkParameterType pLP)
{
	ResolveNull2In(pLP);
}

void Fortune::Voronoi::FixOrder(pHalfEdgeType h0)
{ // ensures that origin(next(half))=origin(twin(half))
	pHalfEdgeType hS = h0, hNxt = NULL, hT = NULL, hM = NULL, h = NULL, 
		hMPlus = NULL, hMMinus = NULL;

	assert (h0 != NULL);
	hNxt = hS->next; hT = hS->twin;
	if (hNxt->origin != hT->origin)
	{ // find a start point:
		hS = hNxt;
		while (hS != h0)
		{
			hNxt = hS->next; hT = hS->twin;
			if (hNxt->origin == hT->origin) break;
			hS = hNxt;
		}
		assert (hS != h0);
	}
	h = hS;
	do
	{
		hM = hNxt; // h match
		if (hNxt->origin != hT->origin)
		{
			while (hM != h)
			{ // the match should be somewhere next in the chain
				hM = hM->next;
				if (hM->origin == hT->origin) break;
			}
			assert(hM != h);
		}
		if (hM != hNxt)
		{ // switch order:
			//h->next = hM;
			hMPlus = hM->next;
			hMMinus = hM->prev;
			//hM->prev = h;
			if (hNxt == hMMinus)
			{
				h->next = hM;
				hM->next = hNxt;
				hNxt->next = hMPlus;
				hMPlus->prev = hNxt;
				hNxt->prev = hM;
				hM->prev = h;
				//hNxt->prev = hM;
				//hNxt->next = hMPlus;
				//hMPlus->prev = hNxt;
			}
			else
			{
				hM->next = hNxt->next;
				hNxt->next->prev = hM;
				h->next = hM;
				hM->prev = h;
				hMMinus->next = hNxt;
				hNxt->next = hMPlus;
				hMPlus->prev = hNxt;
				hNxt->prev = hMMinus;
				//hNxt->next = hMPlus;
				//hMPlus->prev = hNxt;
				//hNxt->prev = hMMinus;
				//hMMinus->next = hNxt;
			}
		}
		h = h->next; hNxt = h->next; hT = h->twin;
	} while (h != hS);
}

void Fortune::Voronoi::LinkHalfEdges(void)
{
	//HRESULT b_p;
	//char desc[1024];

	pCellListType cL = D.cells;
	pRadialPoint FirstRad = NULL;
	CompGeo::XY ctr_calc((pBB->left + pBB->right) / 2.0, (pBB->top + pBB->bottom) / 2.0),
		P_TL(pBB->left, pBB->top), P_TR(pBB->right, pBB->top), P_BL(pBB->left, pBB->bottom), 
		P_BR(pBB->right, pBB->bottom),
		CNRS[4];
	CNRS[0] = P_TR; CNRS[1] = P_TL; CNRS[2] = P_BL; CNRS[3] = P_BR;
	SitePoint ctr_site(ctr_calc);
	pCellListType pCellLst = new CellListType; //(pCellListType)new CellListType));
	pCellLst->next = D.cells;
	D.cells = pCellLst;
	pCellLst->cell = new CellType; //(pCellType)new CellType));
	pCellType c_ext = pCellLst->cell;
	c_ext->ID = ' '; c_ext->InnerComponents = NULL; c_ext->OuterComponent = NULL; c_ext->site = NULL;
	int i = 0, j = 0, ext_count = 0;
	//BBvtx_i = NULL;
	//BBvtx_f = NULL;
	//ocount = 0;
	//hcount = 0;
	pCellType c0 = NULL;

	while (cL != NULL)
	{	
		pCellType c1 = cL->cell;
		c0 = c1;  // keeps track of last cell
		CompGeo::XY P1 = *c1->site;
		pHalfEdgeListType hL = c1->InnerComponents, hLTrail = NULL, hLNxt = NULL;
		LinkParameterType LP;
		CompGeo::XY ePts[4] = {CompGeo::XY(0.0, 0.0)};
		bool OnSeg[4] = {false};
		double t[4] = {0.0};
		LP.d = CompGeo::XY(0.0, 0.0);
		LP.eCount = 0;
		LP.ePt = ePts;
		LP.hL = hL;
		LP.hLPrev = hLTrail;
		LP.iCount = 0;
		LP.Intersectors = NULL;
		LP.makeTwin1 = false;
		LP.OnSeg = OnSeg;
		LP.P1 = CompGeo::XY(0.0, 0.0);
		LP.P2 = CompGeo::XY(0.0, 0.0);
		LP.R0 = CompGeo::XY(0.0, 0.0);
		LP.t = t;

		while (LP.hL != NULL) ResolveEdge(&LP);	// loop thru all calculated halfedges
		if (LP.iCount > 0)
		{	// handle the intersectors:
			//ocount += LP.iCount;
			assert ((LP.iCount %2) == 0); // even count 1 starter to 1 ender
			pRadialPoint rPts = new RadialPoint[LP.iCount];
			pHalfEdgeListType ihL = LP.Intersectors;
			for (i = 0; i < LP.iCount; ++i)
			{
				pHalfEdgeType h = ihL->halfEdge;
				pCellType c_h = h->IncidentCell;
				if (c_h == c1) rPts[i] = RadialPoint(h);
				else rPts[i] = RadialPoint(h, c1->site);
				rPts[i].cell = c1;
				ihL = ihL->next;
			}
			Sorter<RadialPoint> s(rPts, 0, LP.iCount - 1, LP.iCount);
			s.doSort();
			int iStart = 0;
			if (rPts[0].he->IncidentCell == c1) ++iStart;
			for (i = 0; i < (LP.iCount / 2); ++i)
			{ // loop thru pairs of sorted intersection points (starter, ender, starter, ender, ...)
				int idx_start = (2 * i + iStart) % LP.iCount,
					idx_end = (idx_start + 1) % LP.iCount;
				pHalfEdgeType h_s = rPts[idx_start].he, h_e = rPts[idx_end].he;
				assert((h_s->IncidentCell != c1) && (h_e->IncidentCell == c1));
				pHalfEdgeType h1 = new HalfEdgeType, //(pHalfEdgeType)new HalfEdgeType)),
					h2 = new HalfEdgeType; //(pHalfEdgeType)new HalfEdgeType));
				h1->twin = h2;  h2->twin = h1; 
				h1->flag = -1; h1->prev = NULL; h1->next = NULL;
				h2->flag = -1; h2->prev = NULL; h2->next = NULL;
				h1->IncidentCell = c1;  h2->IncidentCell = c_ext;
				h1->origin = h_s->origin;  h2->origin = h_e->origin;
				pHalfEdgeListType hL1 = new HalfEdgeListType, //(pHalfEdgeListType)new HalfEdgeListType)),
					hL2 = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
				hL1->halfEdge = h1; hL1->next = c1->InnerComponents; c1->InnerComponents = hL1;
				hL2->halfEdge = h2; hL2->next = c_ext->InnerComponents; c_ext->InnerComponents = hL2;
				++LP.eCount;
				++ext_count;
				int iCurrentEdge = OnBox(h_s->origin->xy), iFinalEdge = OnBox(h_e->origin->xy);
				assert ((iCurrentEdge != -1) && (iFinalEdge != -1));
				int iCorner_f = InCorner(h_e->origin->xy);
				if (iCorner_f == iFinalEdge) iFinalEdge = (4 + iFinalEdge - 1) % 4;
				while (iCurrentEdge != iFinalEdge)
				{ // adding corners:
					pVertexListType vL = new VertexListType; //(pVertexListType)new VertexListType));
					vL->vertex.IncidentEdge = h2;
					vL->next = D.vertices;
					D.vertices = vL;
					vL->vertex.xy = CNRS[iCurrentEdge];
					h2->origin = &vL->vertex;
					//if (BBvtx_f == NULL) BBvtx_f = vL;
					h1 = new HalfEdgeType; //(pHalfEdgeType)new HalfEdgeType));
					h2 = new HalfEdgeType; //(pHalfEdgeType)new HalfEdgeType));
					h1->twin = h2;  h2->twin = h1; 
					h1->flag = -1; h1->prev = NULL; h1->next = NULL;
					h2->flag = -1; h2->prev = NULL; h2->next = NULL;
					h1->IncidentCell = c1;  h2->IncidentCell = c_ext;
					h1->origin = &vL->vertex;  h2->origin = h_e->origin;
					hL1 = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
					hL2 = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
					hL1->halfEdge = h1; hL1->next = c1->InnerComponents; c1->InnerComponents = hL1;
					hL2->halfEdge = h2; hL2->next = c_ext->InnerComponents; c_ext->InnerComponents = hL2;
					++LP.eCount;
					++ext_count;
					iCurrentEdge = (iCurrentEdge + 1) % 4;
				} // done adding corners
			} // done going thru pairs of intersectors
			delete [] rPts;
		} // done handling intersectors
		if (LP.eCount > 0)
		{
			// link cell c1's halfedges:
			pRadialPoint rPts = new RadialPoint[LP.eCount];
			hL = c1->InnerComponents;
			hLTrail = NULL;
			for (i = 0; i < LP.eCount; ++i)
			{
				pHalfEdgeType h = hL->halfEdge;
				rPts[i] = RadialPoint(h);
				hLTrail = hL;
				hL = hL->next;
			}
			assert (hLTrail != NULL);
			//hLTrail->next = D.halfedges;
			//D.halfedges = c1->InnerComponents;
			c1->InnerComponents = NULL;
			//hcount += LP.eCount;
			Sorter<RadialPoint> s(rPts, 0, LP.eCount - 1, LP.eCount);
			s.doSort();
			c1->OuterComponent = rPts[0].he;
			//b_p = StringCbPrintf(desc, 1024 * sizeof(char), L"halfEdges around cell %c @ (%f, %f):\t", 
			//	c1->ID, c1->site->x, c1->site->y);
			//OutputDebugString(desc);
			for (i = 0; i < LP.eCount; ++i)
			{
				int idx_next = (i + 1) % LP.eCount, idx_prev = (LP.eCount + i - 1) % LP.eCount;
				pHalfEdgeType h = rPts[i].he, h_p = rPts[idx_prev].he, h_n = rPts[idx_next].he;
				h->next = h_n; h->prev = h_p;

				//b_p = StringCbPrintf(desc, 1024 * sizeof(char), L"\r\n");
				//if (0 == (i % 4)) OutputDebugString(desc);
				
				//b_p = StringCbPrintf(desc, 1024 * sizeof(char), L"-(%f, %f)Q%u %f-", 
				//	h->origin->x, h->origin->y, rPts[i].Quad2, rPts[i].YoverX);
				//OutputDebugString(desc);
			}
			//b_p = StringCbPrintf(desc, 1024 * sizeof(char), L"\r\n");
			//OutputDebugString(desc);

			delete [] rPts;
		}
		cL = cL->next;
	}  // done looping thru internal cells
	// link exterior cell's halfedges:
	pHalfEdgeListType hL_ext = c_ext->InnerComponents, hL_extTrail = NULL;
	// handle single cell exception:
	if (ext_count == 0)
	{
		//ocount = 4;
		pHalfEdgeType h = NULL, h_t = NULL, h_prev = NULL, h_next = NULL, h_box[4] = {NULL};
		for(i = 0; i < 4; ++i)
		{
			h = new HalfEdgeType, //(pHalfEdgeType)new HalfEdgeType)),
				h_t = new HalfEdgeType; //(pHalfEdgeType)new HalfEdgeType));
			h->twin = h_t; h_t->twin = h;
			h->flag = -1; h->IncidentCell = c0; h->next = NULL; h->prev = NULL; h->origin = NULL;
			h_t->flag = -1; h_t->IncidentCell = c_ext; h_t->next = NULL; h_t->prev = NULL; h_t->origin = NULL;
			h_box[i] = h;
			if (i == 0) 
			{
				c0->OuterComponent = h;
				hL_ext = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
				hL_ext->next = NULL;
				hL_ext->halfEdge = h_t;
				c_ext->InnerComponents = hL_ext;
			}
			//pHalfEdgeListType hLC1 = (pHalfEdgeListType)new HalfEdgeListType)),
			//	hLC2 = (pHalfEdgeListType)new HalfEdgeListType));
			//hLC1->halfEdge = h; hLC1->next = D.halfedges; 
			//hLC2->halfEdge = h_t; hLC2->next = hLC1; D.halfedges = hLC2;
			//hcount += 2;
		}
		for(i = 0; i < 4; ++i)
		{
			int idx_nxt = (i + 1) % 4, idx_prev = (i + 3) % 4;
			h = h_box[i]; h_prev = h_box[idx_prev]; h_next = h_box[idx_nxt];
			pVertexListType vL = new VertexListType; //(pVertexListType)new VertexListType));
			vL->next = D.vertices; D.vertices = vL;
			vL->vertex.IncidentEdge = h; vL->vertex.xy = CNRS[idx_prev]; h->origin = &vL->vertex;
			h->prev = h_prev;
			h->next = h_next;
		}
		for(i = 0; i < 4; ++i)
		{
			int idx_nxt = (i + 1) % 4, idx_prev = (i + 3) % 4;
			h = h_box[i]; h_prev = h_box[idx_prev]; h_next = h_box[idx_nxt];
			h_t = h->twin; h_t->origin = h_next->origin;
			h_t->prev = h_next->twin; 
			h_t->next = h_prev->twin;
		}
	}
	else
	{
		pRadialPoint rPts = new RadialPoint[ext_count];
		for (i = 0; i < ext_count; ++i)
		{
			pHalfEdgeType h = hL_ext->halfEdge;
			hL_extTrail = hL_ext;
			hL_ext = hL_ext->next;
			rPts[i] = RadialPoint(h, &ctr_site);
		}
		assert (hL_extTrail != NULL);
		//hL_extTrail->next = D.halfedges;
		//D.halfedges = c_ext->InnerComponents;
		c_ext->InnerComponents = NULL;
		Sorter<RadialPoint> s(rPts, 0, ext_count - 1, ext_count);
		s.doSort();
		for (i = 0; i < ext_count; ++i)
		{ // want to go clockwise with exterior cell so next is last and prev is next
			int idx_next = (ext_count + i - 1) % ext_count, idx_prev = (i + 1) % ext_count;
			pHalfEdgeType h = rPts[i].he, h_p = rPts[idx_prev].he, h_n = rPts[idx_next].he;
			h->prev = h_p; h->next = h_n;
			if (c_ext->InnerComponents == NULL)
			{
				hL_ext = new HalfEdgeListType; //(pHalfEdgeListType)new HalfEdgeListType));
				hL_ext->halfEdge = h;
				hL_ext->next = NULL;
				c_ext->InnerComponents = hL_ext;
			}
		}
		delete [] rPts;
	}

	
	// Now check/fix order:
	cL = D.cells;
	while (cL != NULL)
	{
		pCellType c1 = cL->cell;
		pHalfEdgeListType hL = c1->InnerComponents;
		pHalfEdgeType h = NULL;
		while (hL != NULL)
		{
			h = hL->halfEdge;
			FixOrder(h);
			hL = hL->next;
		}
		h = c1->OuterComponent;
		if (h != NULL) FixOrder(h);
		cL = cL->next;
	}
	/**/
}

void Fortune::Voronoi::ConductTest1(Gtk::Allocation b_b)
{ // called with a separate class than Run or StartAnimation
  // the class is begun and ended per run
	// testData to be x1,y1,x2,y2,...xN,yN
	//double testData[] = {50.0, 150.0, 60.0, 130.0, 10.0, 70.0}; int numPts = 3;
	//double testData[] = {283.0, 217.0, 685.0, 213.0, 482.0, 400.0}; int numPts = 3;
	//double testData[] = {600.0, 600.0, 450.0, 550.0, 300.0, 375.0}; int numPts = 3;
	//double testData[] = {300.0, 150.0, 200.0, 150.0, 240.0, 100.0, 300.0, 50.0, 200.0, 50.0}; int numPts = 5;
	//double testData[] = {500.0, 375.0, 400.0, 350.0, 450.0, 300.0, 500.0, 275.0, 400.0, 250.0}; int numPts = 5;
	//double testData[] = {500.0, 375.0, 400.0, 375.0, 300.0, 375.0, 200.0, 375.0, 100.0, 375.0}; int numPts = 5;
	//double testData[] = {500.0, 375.0, 200.0, 375.0, 350.0, 100.0}; int numPts = 3; // Mercedes
	//double testData[] = {455,662,856,568,191,552,357,500}; int numPts = 4;
	//double testData[] = {1044.000000,371.000000,443.000000,369.000000,
	//	699.000000,367.000000,97.000000,366.000000}; int numPts = 4;
	
	double testData[] = {867.000000,611.000000,339.000000,585.000000,58.000000,544.000000,
		166.000000,431.000000,593.000000,410.000000,842.000000,387.000000,211.000000,316.000000,753.000000,
		202.000000,187.000000,166.000000,842.000000,161.000000,322.000000,152.000000,537.000000,96.000000,
		108.000000,94.000000,320.000000,61.000000,519.000000,55.000000}; 
	int numPts = sizeof(testData) / sizeof(testData[0]) / sizeof(double);
	
	//h_Wnd = hwnd;
	//p_Draw = pdraw;
	bb = b_b;
	int i, cellIDbase = (int)L'A';
	//SetBoundingBox(bb.left, bb.top, bb.right, bb.bottom);
	SetBoundingBox(bb.get_x(), bb.get_y(), bb.get_x() + bb.get_width(), bb.get_y() + bb.get_height());
	for(i = 0; i < numPts; ++i)
	{
		CompGeo::XY xy(testData[2 * i], testData[2 * i + 1]);
		if (ValidSite(xy))
		{
			pEventQPoint sPt = new EventQPoint(xy), sPtBuff = sPt;
			bool Inserted = Q.Insert(sPt);
			if (!Inserted) delete sPtBuff;
		}
	}

	if (IsStarting) IsStarting = false;
	else
	{
		T.Reset();
		ClearDiagram();
		ClearSites();
	}

	bool IsEmpty = false;
	EventNxt = Q.GetLeast(IsEmpty);
	while (!IsEmpty)
	{
		AttendToTree();
		EventNxt = Q.GetLeast(IsEmpty);
	}
	Finalize();
	//DrawFinished();

}
/* STUFF BELONGS IN CONTROL INTERFACE
void Fortune::Voronoi::DrawFinished(void)
{
	p_Draw->DrawPolygons(*p_NG, bb);
	pPGonWork wGonTop = translateCells(), wGon = wGonTop;
	p_Draw->ColorPolygonList(wGonTop, bb, D2D1::ColorF(1.0f, 0.5f, 0.0f, 1.0f));
	p_NG->AddNGon(wGon);
	//p_CI->LoadComboBox();
	PostMessage(h_Wnd, WM_USER, (WPARAM)0, (LPARAM)0);
}
*/
void Fortune::Voronoi::Finalize(void)
{
	RoundVertices();
	LinkHalfEdges();
	CleanUpVertices();
	DiagramHalfEdges();
	T.Reset();
	DiagramComplete = true;
	//ClearSites();
}

void Fortune::Voronoi::Animate(void)
{
	if ((AnimationHold) || (DiagramComplete)) return;
	AnimationHold = true;
	//InProcess = AnimationHold;

	current_directrix -= directrix_delta;
	double current_y = EventNxt.y;
	if (current_directrix < pBB->bottom) current_directrix = current_y;
	if (current_y >= current_directrix)
	{
		bool IsEmpty = false;
		do
		{ // deal with all events that have the same y value in one animation step
			AttendToTree();
			IsEmpty = DrawNextEvent();
			if (IsEmpty) break;
		} while (fabs(EventNxt.y - current_y) < MAX_FLT_PRECISION);

		if (IsEmpty) 
		{
			//KillTimer(h_Wnd, timerID);
			Finalize();
			//DrawFinished();
			//p_Draw->EndVoronoiAnimation();
		}
		else
		{
			DrawBeachLine();
			InProcessDraw(true);
			current_directrix = T.y_d;
		}
	}
	else
	{
		T.y_d = current_directrix;
		DrawBeachLine();
		InProcessDraw(true);
	}


	AnimationHold = false;
	//InProcess = AnimationHold;
}

void Fortune::Voronoi::StartAnimation(double delta) //, Gtk::Allocation b_b) //(PolygonDraw * pdraw, NGons * png, 
	//HWND hwnd, RECT b_b, unsigned int_PTR timer_id, unsigned int Interval, double delta)
{
	AnimationHold = true;
	//timerID = timer_id;
	directrix_delta = delta;
	
	//h_Wnd = hwnd;
	//p_Draw = pdraw;
	//p_NG = png;
	//bb = b_b;
	cellIDbase = (int)L'A';
	//SetBoundingBox(bb.left, bb.top, bb.right, bb.bottom);
	current_directrix = pBB->top;
	string txtBuff = "pBB: l=" + to_string(pBB->left) + " b=" + to_string(pBB->bottom) + 
		" r=" + to_string(pBB->right) + " t=" + to_string(pBB->top) + "\n";
	//cout << txtBuff;
	pSitePointListType sL = SitePoints;
	txtBuff = "***SITE POINTS***:\r\n";
	//cout << txtBuff;
	int i = 0;
	while (sL != NULL)
	{
		txtBuff = to_string(++i) + "(" + (char)(cellIDbase + i - 1) + ") - (" + 
			to_string(sL->site.x) + "," + to_string(sL->site.y) + ") \r\n";
		//cout << txtBuff;
		sL = sL->next;
	}
	//char txtBuff[256] ={L'\0'};
	//HRESULT b_p = StringCbPrintf(txtBuff, 256 * sizeof(char), TEXT("***SITE POINTS***:\r\n"));
	//OutputDebugString(txtBuff);
	//SetBoundingBox(bb.left, bb.top, bb.right, bb.bottom);  //deliberately flipped top and bottom
	i = 0;
	sL = SitePoints;	
	while (sL != NULL)
	{
		CompGeo::XY xy(sL->site.x, sL->site.y);
		if (ValidSite(xy))
		{
			txtBuff = to_string(++i) + "(" + (char)(cellIDbase + i - 1) + ") - (" + 
				to_string(sL->site.x) + "," + to_string(sL->site.y) + ") \r\n";
			//b_p = StringCbPrintf(txtBuff, 256 * sizeof(char), TEXT("%u (%c) - (%f,%f) \r\n"),
			//	++i, (char) cellIDbase + i - 1, sL->site.x, sL->site.y);
			//OutputDebugString(txtBuff);
			pEventQPoint sPt = new EventQPoint(sL->site), sPtBuff = sPt;
			bool Inserted = Q.Insert(sPt);
			if (!Inserted) delete sPtBuff;
		}
		sL = sL->next;
	}

	/**/
	// testData to be x1,y1,x2,y2,...xN,yN
	//double testData[] = {50.0, 150.0, 60.0, 130.0, 10.0, 70.0}; int numPts = 3;
	//double testData[] = {283.0, 217.0, 685.0, 213.0, 482.0, 400.0}; int numPts = 3;
	//double testData[] = {600.0, 600.0, 450.0, 550.0, 300.0, 375.0}; int numPts = 3;
	//double testData[] = {300.0, 150.0, 200.0, 150.0, 240.0, 100.0, 300.0, 50.0, 200.0, 50.0}; int numPts = 5;
	//double testData[] = {500.0, 375.0, 400.0, 350.0, 450.0, 300.0, 500.0, 275.0, 400.0, 250.0}; int numPts = 5;
	//double testData[] = {500.0, 375.0, 400.0, 375.0, 300.0, 375.0, 200.0, 375.0, 100.0, 375.0}; int numPts = 5;
	//double testData[] = {500.0, 375.0, 200.0, 375.0, 350.0, 100.0}; int numPts = 3; // Mercedes
	//double testData[] = {455,662,856,568,191,552,357,500}; int numPts = 4;
	//double testData[] = {1044.000000,371.000000,443.000000,369.000000,
	//	699.000000,367.000000,97.000000,366.000000}; int numPts = 4;
	//double testData[] = {1044.000000,370.000000,443.000000,370.000000,
	//	699.000000,370.000000,97.000000,370.000000}; int numPts = 4;
	
	/*
	double testData[] = {867.000000,611.000000,339.000000,585.000000,58.000000,544.000000,
		166.000000,431.000000,593.000000,410.000000,842.000000,387.000000,211.000000,316.000000,753.000000,
		202.000000,187.000000,166.000000,842.000000,161.000000,322.000000,152.000000,537.000000,96.000000,
		108.000000,94.000000,320.000000,61.000000,519.000000,55.000000}; 
	int numPts = 15;
	*/
	/*
	for(i = 0; i < numPts; ++i)
	{
		CompGeo::XY xy(testData[2 * i], testData[2 * i + 1]);
		if (ValidSite(xy))
		{
			pEventQPoint sPt = new EventQPoint(xy), sPtBuff = sPt;
			bool Inserted = Q.Insert(sPt);
			if (!Inserted) delete sPtBuff;
		}
	}
	*/


	if (IsStarting) IsStarting = false;
	else
	{
		T.Reset();
		ClearDiagram();
		//ClearSites();
	}
	if (i == 0) return;  // no site points
	bool IsEmpty = DrawNextEvent();
	if (IsEmpty) Finalize();
	//else SetTimer(hwnd, timer_id, Interval, NULL); // will send WM_TIMER messages to hwnd
	AnimationHold = false;
}


bool Fortune::Voronoi::DrawNextEvent(void)
{
	bool IsEmpty = true;
	EventNxt = Q.GetLeast(IsEmpty);
	return IsEmpty;
}

void Fortune::Voronoi::AttendToTree(void)
{

	//char file_name[] = TEXT("F:\\Projects\\Polygons\\Polys\\tree_debug.txt"), desc[256];
	string file_name = "/Debug/tree_debug.txt", desc = "";
	//HRESULT b_p;
	EventQPoint p = EventNxt;
	if (p.type == 'S')
	{
		pVertexListType vL = new VertexListType; //(pVertexListType)new VertexListType));
		vL->vertex.IncidentEdge = NULL;
		vL->next = D.vertices;
		D.vertices = vL;
		vL->vertex.xy = SitePoint(*p.site);
		pCellListType cL = new CellListType; //(pCellListType)new CellListType));
		cL->next = D.cells;
		D.cells = cL;
		cL->cell = new CellType; //(pCellType)new CellType));
		pCellType c = cL->cell;
		//b_p = StringCbPrintf(txtBuff, 256 * sizeof(char),TEXT("%u"), i++);
		//c->ID = txtBuff[0];  // only works for <9 site points
		if (cellIDbase >= 127) cellIDbase = (int)L'A'; 
		c->ID = (char)cellIDbase++;
		c->InnerComponents = NULL;
		c->OuterComponent = NULL;
		c->site = &(vL->vertex.xy);
		pArcType a = new ArcType; //(pArcType)new ArcType));
		a->ArcFlag = 'n'; a->cell = c;
		a->circleEvent = NULL; a->leftBP = NULL; a->next = NULL; a->prev = NULL; a->rightBP = NULL;
		T.AddArc(a, Q);
		desc = "TREE AFTER CELL " + to_string(c->ID) + " W/ SITE (" + to_string(D.vertices->vertex.xy.x) + 
			"," + to_string(D.vertices->vertex.xy.y) + ") :\r\n";
		//b_p = StringCbPrintf(desc, 256 * sizeof(char), TEXT("TREE AFTER CELL %c W/ SITE (%f,%f) :\r\n"),
		//	c->ID, D.vertices->vertex.xy.x, D.vertices->vertex.xy.y);
		//T.TreeWrite(desc, file_name);
	}
	else // circle event
	{
		desc = "TREE BEFORE CIRCLE EVENT:\r\n";
		//b_p = StringCbPrintf(desc, 256 * sizeof(char), TEXT("TREE BEFORE CIRCLE EVENT:\r\n"));
		//T.TreeWrite(desc, file_name);
		T.DeleteArc(&p, Q, &D);
		/*
		for (int i = 0; i < 3; ++i)
		{
			pHalfEdgeType h = T.released_he[i]->twin;
			if (h->origin == NULL) 
			{
				pVertexListType vL = (pVertexListType)new VertexListType));
				vL->next = D.vertices; D.vertices = vL;
				vL->vertex.IncidentEdge = h;
				vL->vertex.xy = CropBeachPoint(h);
				h->origin = &vL->vertex;
			}
		}
		
		if (T.dBugQ != NULL) 
		{
			CompGeo::AVLNode<EventQPoint> * aNode = Q.Find(T.dBugQ);
			assert(aNode != NULL);
			b_p = StringCbPrintf(desc, 256 * sizeof(char), TEXT("EventQ TREE AFTER CIRCLE EVENT @(%f,%f) W/ Y(CIRCLE)=%f & RADIUS=%f:\r\n"),
				p.x, p.y, D.vertices->vertex.xy.y, D.vertices->vertex.xy.y - p.y);
			//Q.TreeWrite(desc, file_name);
		}
		*/
		desc = "TREE AFTER CIRCLE EVENT @(" + to_string(p.x) + "," + to_string(p.y) + ") W/ Y(CIRCLE)=" +  
			to_string(D.vertices->vertex.xy.y) + " & RADIUS=" + to_string(D.vertices->vertex.xy.y - p.y) + ":\r\n";
		//b_p = StringCbPrintf(desc, 256 * sizeof(char), TEXT("TREE AFTER CIRCLE EVENT @(%f,%f) W/ Y(CIRCLE)=%f & RADIUS=%f:\r\n"),
		//	p.x, p.y, D.vertices->vertex.xy.y, D.vertices->vertex.xy.y - p.y);
		//T.TreeWrite(desc, file_name);
		/**/	
	}

}

void Fortune::Voronoi::SetPOINTFromXY(D2D1_POINT_2F * cPts, int i, const CompGeo::XY & p)
{
	cPts[i].x = (FLOAT)p.x;
	cPts[i].y = (FLOAT)p.y;
}

void Fortune::Voronoi::TerminateBeachEdge(pHalfEdgeType h)
{
	return;
	assert (h != NULL);
	assert (h->origin != NULL);

	pHalfEdgeType ht = h->twin;
	if (ht->origin != NULL) return;

	pCellType ch = h->IncidentCell, ct = ht->IncidentCell;
	CompGeo::XY P1 = *ch->site, P2 = *ct->site, d = P2 - P1, L(d.y, -d.x),
		ePt[4] = {CompGeo::XY(0.0, 0.0)};
	double t[4] = {0.0};
	bool OnSeg[4] = {false};
	GetBBSolutions(ePt, t, OnSeg, P1, L);
	int count = 0, j = -1;
	for (int i = 0; i < 4; ++i)
	{
		if ((OnSeg[i]) && (t[i] < 0.0)) 
		{
			j = i;
			++count;
		}
	}
	assert (count == 1);
	pVertexListType vL = new VertexListType; //(pVertexListType)new VertexListType));
	vL->vertex.IncidentEdge = ht;
	vL->next = D.vertices;
	D.vertices = vL;
	vL->vertex.xy = ePt[j];
	ht->origin = &vL->vertex;

}

CompGeo::XY Fortune::Voronoi::CropBeachPoint(pHalfEdgeType h)
{
	assert (h != NULL);
	//assert (h->origin != NULL);
	//if (InBounds(h->origin)) return *h->origin;

	pHalfEdgeType ht = h->twin;

	pCellType ch = h->IncidentCell, ct = ht->IncidentCell;
	CompGeo::XY P1 = *ch->site, P2 = *ct->site, R0 = (P1 + P2) / 2.0, d = P1 - P2, L(d.y, -d.x),
		ePt[4] = {CompGeo::XY(0.0, 0.0)};
	double t[4] = {0.0};
	bool OnSeg[4] = {false};
	GetBBSolutions(ePt, t, OnSeg, R0, L);
	int count = 0, j = -1;
	for (int i = 0; i < 4; ++i)
	{
		if ((OnSeg[i]) && (t[i] < 0.0)) 
		{
			j = i;
			++count;
		}
	}
	assert (count == 1);
	return ePt[j];

}


void Fortune::Voronoi::DrawBeachLine(void)
{ // also draws directrix
	
	old_directrix = T.y_d;
	pVertexListType avL = NULL, vL = NULL;  // temporary animation vertices
	pHalfEdgeType ah = NULL;
	bLine->clear();
	//first_point->clear();
	LinePts->clear();
	CtlPtCount = 0;
	//if (bLine != NULL) 
	//{
	//	delete[] bLine;
	//	bLine = NULL;
	//	CtlPtCount = 0;
	//}
	//if (first_point == NULL) first_point = new vector<CompGeo::XY>; //(D2D1_POINT_2F*)new D2D1_POINT_2F));
	// the directrix:
	//LinePtCount = 2;
	//LinePts = new D2D1_POINT_2F[LinePtCount];
	//LinePts[0].x = (float)pBB->left; LinePts[0].y = (float)T.y_d;
	//LinePts[1].x = (float)pBB->right; LinePts[1].y = (float)T.y_d;
	LinePts->push_back(CompGeo::XY(pBB->left, T.y_d));
	LinePts->push_back(CompGeo::XY(pBB->right, T.y_d));
	
	double overTop = 10.0;
	CompGeo::XY LftIntersect(pBB->left, pBB->top + overTop), RghtIntersect(pBB->right, pBB->top + overTop), 
		EPt_L, EPt_R, CPt_L, CPt_R, Q;
	pArcType a = T.rightmost->prev;
	if ((a->ArcFlag == 'n') && (fabs(a->cell->site->y - T.y_d) >= MAX_FLT_PRECISION)) 
		RghtIntersect = CompGeo::XY(pBB->right, T.GetYofXForArc(a, pBB->right));
	a = T.leftmost->next;
	if ((a->ArcFlag == 'n') && (fabs(a->cell->site->y - T.y_d) >= MAX_FLT_PRECISION)) 
		LftIntersect = CompGeo::XY(pBB->left, T.GetYofXForArc(a, pBB->left));
	pBeachPointType b = a->rightBP;
	int iCurveCount = 0, iCurveIdx = 0, i, iArcCount = 0, iSegCount = 0;
	//double cx = ((pBB->right - pBB->left) / 2.0), cy = ((pBB->top - pBB->bottom) / 2.0);
	while (a != T.rightmost)
	{
		++iArcCount;
		double yF = a->cell->site->y;
		if (fabs(yF - T.y_d) < MAX_FLT_PRECISION) ++iSegCount;
		a = a->next;
	}
	if (iArcCount == 0) return; 
	int cPtCount = 0;
	CompGeo::XY * cPts = NULL;

	a = T.leftmost->next;
	b = a->rightBP;
	EPt_L = LftIntersect;
	//first_point->x = (float)EPt_L.x; first_point->y = (float)EPt_L.y;
	first_point = EPt_L;
	// control point parameters for no-width parabolic arcs:
	double x_p = 0.0, y_h = 0.0, y_l = 0.0, y_c = 0.0; 
	bool skipFirst = false;

	if (iArcCount == iSegCount)
	{	// special handling for top arc(s) w/ focus on directrix - (1 + 2 * iSegCount) segments:
		
		iCurveCount = 1 + 2 * iSegCount;
		cPtCount = 3 * iCurveCount; // i.e. 6 * iSegCount + 3
		cPts = new CompGeo::XY[cPtCount];
		//SetPOINTFromXY(cPts, 0, EPt_L);
		while (iCurveIdx < iSegCount)
		{ // these curves are top row parabolas where directrix is on the foci
			i = 6 * iCurveIdx++;
			y_h = pBB->top + overTop; y_l = T.y_d; y_c = (4.0 * y_l - y_h) / 3.0;
			x_p = a->cell->site->x;
			EPt_R = CompGeo::XY(x_p, y_h); cPts[i + 2] = EPt_R; //SetPOINTFromXY(cPts, i + 2, EPt_R); 
			CPt_L = EPt_R - EPt_L; CPt_L /= 3.0; CPt_L += EPt_L; cPts[i] = CPt_L; //SetPOINTFromXY(cPts, i, CPt_L);
			CPt_R = EPt_L - EPt_R; CPt_R /= 3.0; CPt_R += EPt_R; cPts[i + 1] = CPt_R; //SetPOINTFromXY(cPts, i + 1, CPt_R);
			EPt_L = EPt_R; 
			cPts[i + 5] = EPt_L; //SetPOINTFromXY(cPts, i + 5, EPt_R);
			CPt_L = CompGeo::XY(x_p, y_c); cPts[i + 3] = CPt_L; //SetPOINTFromXY(cPts, i + 3, CPt_L);
			CPt_R = CPt_L; cPts[i + 4] = CPt_R; //SetPOINTFromXY(cPts, i + 4, CPt_R);
			a = a->next;
			b = a->rightBP;
		}
		i = 6 * iCurveIdx;
		assert ((i + 3) == (int)cPtCount);
		EPt_R = RghtIntersect; cPts[i + 2] = EPt_R; //SetPOINTFromXY(cPts, i + 2, EPt_R);
		CPt_L = EPt_R - EPt_L; CPt_L /= 3.0; CPt_L += EPt_L; cPts[i] = CPt_L; //SetPOINTFromXY(cPts, i, CPt_L);
		CPt_R = EPt_L - EPt_R; CPt_R /= 3.0; CPt_R += EPt_R; cPts[i + 1] = CPt_R; //SetPOINTFromXY(cPts, i + 1, CPt_R);
	}
	else
	{
		iCurveCount = iArcCount;// + iSegCount;
		cPtCount = 3 * iCurveCount;
		//cPts = new D2D1_POINT_2F[cPtCount];
		cPts = new CompGeo::XY[cPtCount];
		//SetPOINTFromXY(cPts, 0, EPt_L);
		while (b != NULL)
		{
			T.GetX(b); T.GetY(b); EPt_R = CompGeo::XY(T.x_bp, T.y_bp);
			if ((iCurveIdx == 0) && (T.x_bp < pBB->left))
			{
				skipFirst = true;
				/*
				ah = b->halfEdge->twin;
				if (ah->origin == NULL)
				{
					vL = (pVertexListType)new VertexListType));
					vL->next = D.vertices; D.vertices = vL; 
					ah->origin = &vL->vertex; vL->IncidentEdge = ah;
					vL->vertex = CropBeachPoint(ah);
				}
				else
				{
					ah = b->halfEdge;
					if (ah->origin == NULL)
					{
						vL = (pVertexListType)new VertexListType));
						vL->next = D.vertices; D.vertices = vL; 
						ah->origin = &vL->vertex; vL->IncidentEdge = ah;
						vL->vertex = CropBeachPoint(ah);
						ah->flag = -2;
					}
				}
				*/
			}
			if ((iCurveIdx == 1) && (skipFirst))
			{
				first_point.y = T.GetYofXForArc(a, pBB->left);
				EPt_L = first_point;
			}
			ah = b->halfEdge; 
			pVertexListType vL = new VertexListType; //(pVertexListType)new VertexListType));
			vL->next = avL; avL = vL; vL->vertex.xy = EPt_R;
			ah->origin = &vL->vertex; vL->vertex.IncidentEdge = ah;
			
			if ((ah->twin->origin == NULL) && (InBounds(ah->origin->xy)))
			{
				ah = ah->twin;
				vL = new VertexListType; //(pVertexListType)new VertexListType));
				vL->next = avL; avL = vL; 
				ah->origin = &vL->vertex; vL->vertex.IncidentEdge = ah;
				vL->vertex.xy = CropBeachPoint(ah);
				//TerminateBeachEdge(ah);
				ah = ah->twin;
				ah->flag = 0;
			}
			
			if ((!InBounds(ah->origin->xy)) && (ah->twin->origin != NULL))
			{
				CompGeo::XY xyBuff = CropBeachPoint(ah);
				if (xyBuff.y > old_directrix) ah->origin->xy = xyBuff;
			}
			

			i = 3 * iCurveIdx++;
			if (fabs(a->cell->site->y - T.y_d) < MAX_FLT_PRECISION) 
			{ // focus on directrix need 2 straight segments - can do with one bezier path segment:
				x_p = a->cell->site->x;
				y_l = T.y_d; y_h = T.y_bp; y_c = (4.0 * y_l - y_h) / 3.0;
				Q = CompGeo::XY(x_p, y_l);
				CPt_L = CompGeo::XY(x_p, y_c); CPt_R = CPt_L;
			}
			else
			{
				double xF = a->cell->site->x, yF = a->cell->site->y,
					mL = (EPt_L.x - xF) / (yF - T.y_d),
					mR = (EPt_R.x - xF) / (yF - T.y_d),
					cL = EPt_L.y - mL * EPt_L.x,
					cR = EPt_R.y - mR * EPt_R.x;
				Q = CompGeo::XY(-cL + cR, -mR * cL + mL * cR); Q /= mL - mR;
				CPt_L = EPt_L - Q; CPt_L /= 3.0; CPt_L += Q;
				CPt_R = EPt_R - Q; CPt_R /= 3.0; CPt_R += Q;
			}
			cPts[i] = CPt_L; //SetPOINTFromXY(cPts, i, CPt_L);
			cPts[i + 1] = CPt_R; //SetPOINTFromXY(cPts, i + 1, CPt_R);
			cPts[i + 2] = EPt_R; //SetPOINTFromXY(cPts, i + 2, EPt_R);
			EPt_L = EPt_R;
			a = a->next;
			b = a->rightBP;
		}
		// last arc (might also be only i.e. first arc so far directrix below focus):
		EPt_R = RghtIntersect;
		i = 3 * iCurveIdx;
		assert (i == (int)(cPtCount - 3));
		assert (a == (T.rightmost->prev));
		double xF = a->cell->site->x, yF = a->cell->site->y,
			mL = (EPt_L.x - xF) / (yF - T.y_d),
			mR = (EPt_R.x - xF) / (yF - T.y_d),
			cL = EPt_L.y - mL * EPt_L.x,
			cR = EPt_R.y - mR * EPt_R.x;
		Q = CompGeo::XY(-cL + cR, -mR * cL + mL * cR); Q /= mL - mR;
		CPt_L = EPt_L - Q; CPt_L /= 3.0; CPt_L += Q;
		CPt_R = EPt_R - Q; CPt_R /= 3.0; CPt_R += Q;
		cPts[i] = CPt_L; //SetPOINTFromXY(cPts, i, CPt_L);
		cPts[i + 1] = CPt_R; //SetPOINTFromXY(cPts, i + 1, CPt_R);
		cPts[i + 2] = EPt_R; //SetPOINTFromXY(cPts, i + 2, EPt_R);

	}
	// set class members:
	CtlPtCount = cPtCount / 3 - (skipFirst? 1: 0);
	//bLine = new D2D1_BEZIER_SEGMENT[CtlPtCount];
	for (i = 0; i < CtlPtCount; ++i)
	{
		int j = i + (int)skipFirst;
		//bLine[i].point1.x = cPts[3 * j].x + cx; bLine[i].point1.y = cy - cPts[3 * j].y;
		//bLine[i].point2.x = cPts[3 * j + 1].x + cx; bLine[i].point2.y = cy - cPts[3 * j + 1].y;
		//bLine[i].point3.x = cPts[3 * j + 2].x + cx; bLine[i].point3.y = cy - cPts[3 * j + 2].y;
		bLine->push_back(cPts[3 * j]);
		bLine->push_back(cPts[3 * j + 1]);
		bLine->push_back(cPts[3 * j + 2]);
	}
	bLine->insert(bLine->begin(), first_point);
	//p_Draw->DrawBeachLine(bLine, CtlPtCount, first_point, bb);

	int LPC = 0;  // will add to the directrix after
	if (avL != NULL)
	{
		a = T.leftmost->next;
		b = a->rightBP;
		while (b != NULL)
		{
			ah = b->halfEdge;
			if ((ah->origin != NULL) && (ah->twin->origin != NULL)) //LPC+= 2;
			{
				LinePts->push_back(ah->origin->xy);
				LinePts->push_back(ah->twin->origin->xy);
				LPC += 2;
			}
			a = a->next;
			b = a->rightBP;
		}
		//CompGeo::XY * LPts = new CompGeo::XY[LPC]; //D2D1_POINT_2F[LPC];
		//a = T.leftmost->next;
		//b = a->rightBP;
		//for (i = 0; i < (LPC / 2); ++i)
		//{
		//	ah = b->halfEdge;
		//	if ((ah->origin != NULL) && (ah->twin->origin != NULL))
		//	{
				//LPts[2 * i] = ah->origin->xy; //SetPOINTFromXY(LPts, 2 * i, ah->origin->xy);
				//LPts[2 + i + 1] = ah->twin->origin->xy; //SetPOINTFromXY(LPts, 2 * i + 1, ah->twin->origin->xy);
		//		LinePts->push_back(ah->origin->xy);
		//		LinePts->push_back(ah->twin->origin->xy);
		//	}
		//	a = a->next;
		//	b = a->rightBP;
		//}
		//AddToLine(LPts, LPC);
		//delete [] LPts;
		a = T.leftmost->next;
		b = a->rightBP;
		while (b != NULL)
		{
			ah = b->halfEdge;
			ah->origin = NULL;
			if (ah->flag == 0)
			{
				ah->flag = -1;
				ah->twin->origin = NULL;
				ah->origin = NULL;
			}
			a = a->next;
			b = a->rightBP;
		}
		vL = avL;
		while (vL != NULL)
		{
			avL = avL->next;
			delete vL;
			vL = avL;
		}
	}
	
}

void Fortune::Voronoi::AddToLine(D2D1_POINT_2F * ToLine, int numPts)
{
	if ((numPts == 0) || (ToLine == NULL)) return;
	int totPts = numPts + LinePtCount;
	D2D1_POINT_2F * NPts = new D2D1_POINT_2F[totPts];
	if (LinePtCount > 0) memcpy(NPts, LinePts, LinePtCount * sizeof(D2D1_POINT_2F));
	memcpy(&NPts[LinePtCount], ToLine, numPts * sizeof(D2D1_POINT_2F));
	delete [] LinePts;
	//LinePts = NPts;
	LinePtCount = totPts;

}


void Fortune::Voronoi::InProcessDraw(bool DrawPoints)
{ // during processing all halfedges are in a cell's InnerComponents list

	sPts->clear();
	pCellListType cL = D.cells;
	pCellType c = NULL;
	pHalfEdgeListType hL = NULL;
	pHalfEdgeType h = NULL;
	pVertexListType avL = NULL;
	int e = 3, eCount = 0, i = 0, scount = 0;
	//float cx = (float)((pBB->right - pBB->left) / 2.0), cy = (float)((pBB->top - pBB->bottom) / 2.0);
	pArcType a = T.leftmost->next;
	pBeachPointType b = a->rightBP;
	if (a->ArcFlag == 'n')
	{ // mark all beachline halfedges w/ flag = 0
		while (b != NULL)
		{
			b->halfEdge->flag = 0;
			a = a->next;
			b = a->rightBP;
		}
	}
	while (cL != NULL)
	{ // 1st add origins for non-beachline twins:
		c = cL->cell;
		hL = c->InnerComponents;
		while (hL != NULL)
		{
			h = hL->halfEdge->twin;
			
			for (i = 0; i < 2; ++i)
			{
				if ((h->origin == NULL) && (h->flag == -1) && (h->twin->flag == -1))
				{
					pVertexListType vL = new VertexListType; //(pVertexListType)new VertexListType));
					vL->next = avL; avL = vL;
					vL->vertex.IncidentEdge = h;
					vL->vertex.xy = CropBeachPoint(h);
					h->origin = &vL->vertex;
				}
				h = h->twin;
			}

			if ((h->origin != NULL) && (h->twin->origin != NULL)) 
			{
				eCount += 2;
			}

			hL = hL->next;
		}
		if (c->site != NULL) ++scount;

		cL = cL->next;
	}
	a = T.leftmost->next;
	if (a->ArcFlag == 'n')
	{ // resetting all the beachpoint halfedge flag to -1
		b = a->rightBP;
		while (b != NULL)
		{
			b->halfEdge->flag = -1;
			a = a->next;
			b = a->rightBP;
		}
	}
	if (eCount > 0)
	{
		//D2D1_POINT_2F * LPts = new D2D1_POINT_2F[eCount]; 
		cL = D.cells;
		i = 0;
		while (cL != NULL)
		{
			c = cL->cell;
			hL = c->InnerComponents;
			while (hL != NULL)
			{
				h = hL->halfEdge;
				if ((h->origin != NULL) && (h->twin->origin != NULL))
				{
					LinePts->push_back(h->origin->xy);
					LinePts->push_back(h->twin->origin->xy);
					//SetPOINTFromXY(LPts, i++, h->origin->xy);
					//SetPOINTFromXY(LPts, i++, h->twin->origin->xy);
				}
				hL = hL->next;
			}
			cL = cL->next;
		}
		//AddToLine(LPts, eCount);
		//delete [] LPts;
	}
	//p_Draw->DrawLines(LinePts, LinePtCount, bb);

	pVertexListType vL = avL;
	while (vL != NULL)
	{
		avL = vL->next;
		vL->vertex.IncidentEdge->origin = NULL;
		delete vL;
		vL = avL;
	}

	if ((scount > 0) && (DrawPoints))
	{
		//D2D1_POINT_2F * SPts = new D2D1_POINT_2F[scount]; 
		cL = D.cells;
		i = 0;
		while (cL != NULL)
		{
			c = cL->cell;
			if (c->site != NULL) sPts->push_back(*c->site); //SetPOINTFromXY(SPts, i++, *c->site);
			cL = cL->next;
		}
		//p_Draw->DrawPoints(SPts, scount, bb);
		
	}

}

void Fortune::Voronoi::DebugPrintHalfEdgeCycle(CompGeo::DCEL<double> & dPlus, CompGeo::pHalfEdgeType cycle1)
{
	cout << "HalfEdge Index Cycle in face " << to_string(cycle1->incidentface) << ": ";
	CompGeo::ITYPE hIdx1 = cycle1->halfedge, hIdx2 = hIdx1;
	CompGeo::pHalfEdgeType ph = dPlus.getHalfEdge(hIdx1);
	do
	{
		cout << to_string(hIdx2) << " ";
		assert (hIdx2 > 0);
		ph = dPlus.next();
		hIdx2 = ph->halfedge;
		/* code */
	} while (hIdx2 != hIdx1);
	cout << "\n";
}


void Fortune::Voronoi::translateDiagram(CompGeo::DCEL<double> & dPlus)
{
	dPlus.clear_all();
	pCellListType cL = D.cells;
	if (cL == NULL) return;
	CompGeo::ITYPE fIdx = dPlus.getFaceCount(), pIdx = dPlus.getVertexCount(), hIdx = dPlus.getHalfEdgeCount(), lIdx = dPlus.getHalfEdgeListCount(),
		count = 0, topFace = 0;
	map<pHalfEdgeType, CompGeo::ITYPE> twinlkup;
	map<pVertexType, CompGeo::ITYPE> pointlkup;
	CompGeo::pHalfEdgeType prevHalfEdge = NULL, topHalfEdge = NULL, pHE = NULL;
	CompGeo::pHalfEdgeListType prevHalfEdgeList = NULL, topHalfEdgeList = NULL, pHEL = NULL;;
	CompGeo::pFaceType pf = NULL;
	pVertexType my_o = NULL;

	while (cL != NULL)
	{
		pCellType c = cL->cell;
			CompGeo::pFaceType f = new CompGeo::FaceType; 
			f->additionalFaceInfo = c; 
			fIdx = dPlus.push(f); 
			//cout << to_string(fIdx) << "\t";
			pf = dPlus.getFace(fIdx); // pf == f? 
			pf->face = fIdx;
			if (topFace == 0) topFace = fIdx;

		pHalfEdgeListType hL = c->InnerComponents;
			prevHalfEdgeList = NULL; 
			topHalfEdgeList = NULL; 
			pHEL = NULL; 
		while (hL != NULL)
		{
				CompGeo::pHalfEdgeListType l = new CompGeo::HalfEdgeListType;
				lIdx = dPlus.push(l); 
				pHEL = dPlus.getHalfEdgeList(lIdx); // pHEL == l?
				pHEL->halfedgelist = lIdx;

				prevHalfEdge = NULL; 
				topHalfEdge = NULL; 
				pHE = NULL;

			pHalfEdgeType h = hL->halfEdge, h0 = h;
				my_o = h->origin;
				if (pointlkup.count(my_o) == 0)
				{
					CompGeo::TPoint<double> * cgPt0 = new CompGeo::TPoint<double>(my_o->xy.x, my_o->xy.y, 0.0);
					pIdx = dPlus.push(cgPt0);
					pointlkup[my_o] = pIdx;
					cgPt0->vertex = pIdx;
				}
				else pIdx = pointlkup[my_o];


				CompGeo::pHalfEdgeType cgH0 = new CompGeo::HalfEdgeType; 
				cgH0->incidentface = fIdx; 
				cgH0->origin = pIdx; 
				hIdx = dPlus.push(cgH0); 
				pHE = dPlus.getHalfEdge(hIdx); // pHE == cgH0?
				pHE->halfedge = hIdx;
				twinlkup[h] = hIdx; 

			do
			{
					if (prevHalfEdge == NULL) topHalfEdge = pHE; 
					else
					{
						my_o = h->origin;
						if (pointlkup.count(my_o) == 0)
						{
							CompGeo::TPoint<double> * cgPt1 = new CompGeo::TPoint<double>(my_o->xy.x, my_o->xy.y, 0.0);
							pIdx = dPlus.push(cgPt1);
							pointlkup[my_o] = pIdx;
							cgPt1->vertex = pIdx;
						}
						else pIdx = pointlkup[my_o];

						CompGeo::pHalfEdgeType cgH1 = new CompGeo::HalfEdgeType(*cgH0); 
						cgH1->origin = pIdx; 
						cgH1->prev = prevHalfEdge->halfedge;
						hIdx = dPlus.push(cgH1); 
						pHE = dPlus.getHalfEdge(hIdx); // pHE == cgH1? 
						pHE->halfedge = hIdx;
						twinlkup[h] = hIdx; 
						prevHalfEdge->next = hIdx;
					}
					prevHalfEdge = pHE;
				h = h->next;
			} while (h != h0);

				topHalfEdge->prev = pHE->halfedge;
				pHE->next = topHalfEdge->halfedge;
				//DebugPrintHalfEdgeCycle(dPlus, topHalfEdge);

				if (prevHalfEdgeList == NULL)
				{
					topHalfEdgeList = pHEL;
				}
				else
				{
					prevHalfEdgeList->nxt = pHEL->halfedgelist;

				}
				prevHalfEdgeList = pHEL;
			hL = hL->next;
		}
			if (topHalfEdgeList != NULL) 
			{
				pHEL->nxt = 0; // this represents NULL in dPlus
				pHEL->helsttop = topHalfEdge->halfedge;
				pf->inner = topHalfEdgeList->halfedgelist;
			}

			count = 0;
			prevHalfEdge = NULL; topHalfEdge = NULL; pHE = NULL;

		pHalfEdgeType hO = c->OuterComponent, h = hO;
		if (hO != NULL)
		{

				my_o = h->origin;
				if (pointlkup.count(my_o) == 0)
				{
					CompGeo::TPoint<double> * cgPt2 = new CompGeo::TPoint<double>(my_o->xy.x, my_o->xy.y, 0.0);
					pIdx = dPlus.push(cgPt2);
					pointlkup[my_o] = pIdx;
					cgPt2->vertex = pIdx;
				}
				else pIdx = pointlkup[my_o];


				CompGeo::pHalfEdgeType cgH2 = new CompGeo::HalfEdgeType; 
				cgH2->incidentface = fIdx; 
				cgH2->origin = pIdx; 
				hIdx = dPlus.push(cgH2); 
				pHE = dPlus.getHalfEdge(hIdx);  // pHE == cgH2?
				pHE->halfedge = hIdx;
				twinlkup[h] = hIdx; 


				do
				{
					++count;
					if (prevHalfEdge == NULL) topHalfEdge = pHE;
					else
					{
						my_o = h->origin;
						if (pointlkup.count(my_o) == 0)
						{
							CompGeo::TPoint<double> * cgPt3 = new CompGeo::TPoint<double>(my_o->xy.x, my_o->xy.y, 0.0);
							pIdx = dPlus.push(cgPt3);
							pointlkup[my_o] = pIdx;
							cgPt3->vertex = pIdx;
						}
						else pIdx = pointlkup[my_o];


						CompGeo::pHalfEdgeType cgH3 = new CompGeo::HalfEdgeType(*cgH2); 
						cgH3->origin = pIdx; 
						cgH3->prev = prevHalfEdge->halfedge; 
						hIdx = dPlus.push(cgH3); 
						pHE = dPlus.getHalfEdge(hIdx); // pHE == cgH3?
						pHE->halfedge = hIdx; 
						twinlkup[h] = hIdx; 
						prevHalfEdge->next = hIdx;
					}
					prevHalfEdge = pHE;

				h = h->next;
			} while (h != hO);
			if (NULL != topHalfEdge) 
			{
				pHE->next = topHalfEdge->halfedge;
				topHalfEdge->prev = pHE->halfedge;
				//pf->count = count;
				pf->outer = topHalfEdge->halfedge;
				//DebugPrintHalfEdgeCycle(dPlus, topHalfEdge);

			}
		}
		cL = cL->next;
	}
	// now, the twins:
	pHalfEdgeListType thl = D.halfedges;
	while (thl != NULL)
	{
		pHalfEdgeType the_half = thl->halfEdge, the_twin = the_half->twin;
		CompGeo::ITYPE idx1 = twinlkup[the_half], idx2 = twinlkup[the_twin];
		CompGeo::pHalfEdgeType cg_h1 = dPlus.getHalfEdge(idx1), cg_h2 = dPlus.getHalfEdge(idx2);
		cg_h1->twin = idx2;
		cg_h2->twin = idx1;
		thl = thl->next;
	}
	// now, the containing rectangle:
	pf = dPlus.getFace(topFace);
	//CompGeo::pHalfEdgeType hm = NULL;
	//dPlus.getMaxHalfEdge(pf, hm);
	pf = dPlus.BuildFrame(pf);
	pCellType cBuff = new CellType;
	pf->additionalFaceInfo = cBuff;
	cBuff->site = NULL; cBuff->OuterComponent = NULL; cBuff->InnerComponents = NULL;
	cBuff->ID = 'O';
	//cout << "\n";
	/*
	pf = dPlus.getFace(topFace);
	assert (pf->outer == 0);
	CompGeo::pTPoint<double> oRL = new CompGeo::TPoint<double>(pBB->right + 2.0, pBB->bottom - 2.0, 0.0),
		oRU = new CompGeo::TPoint<double>(pBB->right + 2.0, pBB->top + 2.0, 0.0),
		oLU = new CompGeo::TPoint<double>(pBB->left - 2.0, pBB->top + 2.0, 0.0),
		oLL = new CompGeo::TPoint<double>(pBB->left - 2.0, pBB->bottom - 2.0, 0.0);
	size_t iRL = dPlus.push(oRL), iRU = dPlus.push(oRU), iLU = dPlus.push(oLU), iLL = dPlus.push(oLL);
	oRL->vertex = iRL; oRU->vertex = iRU; oLU->vertex = iLU; oLL->vertex = iLL;
	CompGeo::pHalfEdgeType hrbt = new CompGeo::HalfEdgeType, htrl = new CompGeo::HalfEdgeType,
		hltb = new CompGeo::HalfEdgeType, hblr = new CompGeo::HalfEdgeType,
		hrtb = new CompGeo::HalfEdgeType, hbrl = new CompGeo::HalfEdgeType,
		hlbt = new CompGeo::HalfEdgeType, htlr = new CompGeo::HalfEdgeType;
	size_t irbt = dPlus.push(hrbt), itrl = dPlus.push(htrl), iltb = dPlus.push(hltb), iblr = dPlus.push(hblr),
		irtb = dPlus.push(hrtb), ibrl = dPlus.push(hbrl), ilbt = dPlus.push(hlbt), itlr = dPlus.push(htlr);
	hrbt->halfedge = irbt; htrl->halfedge = itrl; hltb->halfedge = iltb; hblr->halfedge = iblr;
	hrtb->halfedge = irtb; hbrl->halfedge = ibrl; hlbt->halfedge = ilbt; htlr->halfedge = itlr;
	hrbt->origin = iRL; hbrl->origin = iRL;
	htrl->origin = iRU; hrtb->origin = iRU;
	hltb->origin = iLU; htlr->origin = iLU;
	hblr->origin = iLL; hlbt->origin = iLL;
	hrbt->next = itrl; htrl->next = iltb; hltb->next = iblr; hblr->next = irbt;
	hbrl->next = ilbt; hlbt->next = itlr; htlr->next = irtb; hrtb->next = ibrl;
	hrbt->prev = iblr; hblr->prev = iltb; hltb->prev = itrl; htrl->prev = irbt;
	hbrl->prev = irtb; hlbt->prev = ibrl; htlr->prev = ilbt; hrtb->prev = itlr;
	hrbt->twin = irtb; htrl->twin = itlr; hltb->twin = ilbt; hblr->twin = ibrl;
	hrtb->twin = irbt; hbrl->twin = iblr; hlbt->twin = iltb; htlr->twin = itrl;
	hrbt->incidentface = topFace; htrl->incidentface = topFace; hltb->incidentface = topFace; hblr->incidentface = topFace;
	pHEL = new CompGeo::HalfEdgeListType;
	pHEL->helsttop = irtb;
	size_t iHEL = dPlus.push(pHEL);
	pHEL->halfedgelist = iHEL;
	CompGeo::pFaceType cf = new CompGeo::FaceType;
	cf->count = 4; cf->inner = iHEL;
	size_t iFace = dPlus.push(cf);
	cf->face = iFace;
	*/
	// deliberately leaving topFace un-updated w/ containing rect's info

	//cout << "Number of entries in twin lookup map structure:" << to_string(twinlkup.size()) << "\n";
	//dPlus.DebugPrint();
	//dPlus.erase('p',5);
	//dPlus.DebugPrint();
	//cout << "Hi there!\n";
}

pPGonWork Fortune::Voronoi::translateEdges(CompGeo::DCEL<double> & dPlus, CompGeo::ITYPE hIdx, bool forwards)
{
	if (hIdx == 0) return NULL;
	pPGonWork wGon = new PGonWork;
	pNameNode pNameOut = new NameNode;
	CompGeo::pHalfEdgeType h0 = dPlus.getHalfEdge(hIdx);
	CompGeo::pFaceType f = dPlus.incidentface();
	pCellType c = static_cast<pCellType>(f->additionalFaceInfo); 
	//if (c == NULL) return NULL;
	string nmn = (string)"V_Noi(" + c->ID + ")";
	pNameOut->polyname = nmn;
	wGon->pName = pNameOut;
	pVertexNode pvn = NULL, pvTrail = NULL;
	CompGeo::pHalfEdgeType h = h0;
	do
	{
		++wGon->numVertices;
		pvn = new VertexNode;
		CompGeo::TPoint<double> * o = dPlus.origin();
		pvn->vtxInfo.vertex = CompGeo::XY(o->xyz.X, o->xyz.Y);
		if (pvTrail == NULL) wGon->vNode = pvn;
		else pvTrail->next = pvn;
		pvTrail = pvn;
		if (forwards) h = dPlus.next();
		else h = dPlus.prev();
	} while (h != h0);
	return wGon;

}


vector<pPGonWork> Fortune::Voronoi::translateCells(CompGeo::DCEL<double> & dPlus)
{
	vector<pPGonWork> r;
	CompGeo::ITYPE fMax = dPlus.getFaceCount();
	if (fMax <= 1) return r;
	for (CompGeo::ITYPE i = 1; i < fMax; ++i)
	{
		CompGeo::pFaceType f = dPlus.getFace(i);
		CompGeo::ITYPE inner_idx = f->inner, outer_idx = f->outer;
		while (inner_idx != 0)
		{
			CompGeo::pHalfEdgeListType hEL = dPlus.getHalfEdgeList(inner_idx);
			r.push_back(translateEdges(dPlus, hEL->helsttop, false));
			inner_idx = hEL->nxt;
		}
		if (outer_idx > 0)
		{
			CompGeo::pHalfEdgeType pHE = dPlus.getHalfEdge(outer_idx);
			CompGeo::ITYPE hIdx0 = pHE->halfedge, hIdx = hIdx0;
			r.push_back(translateEdges(dPlus, hIdx, true));
		}
	}
	return r;

}

pPGonWork Fortune::Voronoi::translateEdges(pHalfEdgeType h0, bool forwards)
{
	if (h0 == NULL) return NULL;
	pPGonWork wGon = new PGonWork; //MakePGon(), //(pPGonWork)new PGonWork)), 
		//wGonTrail = wGon, wGonR = NULL;
	//wGon->next = NULL;
	//wGon->numVertices = 0;
	//wGon->pIdx = 0;
	//wGon->selVertex = -1;
	//wGon->vNode = NULL;
	pNameNode pNameOut = new NameNode; //(pNameNode)new NameNode));
	string nmn = (string)"V_Noi(" + h0->IncidentCell->ID + ")";
	//char TriName[] = TEXT("V_Noi"), * nmn;
	//size_t l = _tcslen(TriName) + 3 * sizeof(char);
	//nmn = new char[l];
	//HRESULT b_p = StringCbPrintf(nmn, l * sizeof(char), TEXT("%s(%c)"), TriName, h0->IncidentCell->ID);
	pNameOut->polyname = nmn;
	wGon->pName = pNameOut;
	pVertexNode pvn = NULL, pvTrail = NULL;
	pHalfEdgeType ph = h0;
	do
	{
		++wGon->numVertices;
		pvn = new VertexNode; //MakeVertex(); //(pVertexNode)new VertexNode));
		//pvn->next = NULL;
		//pvn->vtxInfo.is_selected = false;
		pvn->vtxInfo.vertex = ph->origin->xy;
		//pvn->vtxInfo.vertex.x = (float)ph->origin->x;
		//pvn->vtxInfo.vertex.y = (float)ph->origin->y;
		if (pvTrail == NULL) wGon->vNode = pvn;
		else pvTrail->next = pvn;
		pvTrail = pvn;
		if (forwards) ph = ph->next;
		else ph = ph->prev;
	} while (ph != h0);
	return wGon;

}

vector<pPGonWork> Fortune::Voronoi::translateCells(void)
{// will output list of NGons 

	vector<pPGonWork> r;
	pCellListType cL = D.cells;
	if (cL == NULL) return r;
	//pPGonWork wGonRoot = NULL, wGonTrail = NULL, wGonR = NULL;
	while (cL != NULL)
	{
		pCellType c = cL->cell;
		pHalfEdgeListType hL = c->InnerComponents;
		while (hL != NULL)
		{
			pHalfEdgeType h = hL->halfEdge;
			r.push_back(translateEdges(h, false));
			//wGonR = translateEdges(h, false);
			//if (wGonTrail == NULL) wGonRoot = wGonR;
			//else wGonTrail->next = wGonR;
			//wGonTrail = wGonR;
			hL = hL->next;
		}
		pHalfEdgeType hO = c->OuterComponent;
		if (hO != NULL)
		{
			r.push_back(translateEdges(hO, true));
			//wGonR = translateEdges(hO, true);
			//if (wGonTrail == NULL) wGonRoot = wGonR;
			//else wGonTrail->next = wGonR;
			//wGonTrail = wGonR;
		}
		cL = cL->next;
	}
	//return wGonRoot;
	return r;
}
