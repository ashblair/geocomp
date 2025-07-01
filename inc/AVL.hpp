#pragma once

#include "vector_c.hpp"
#include "avlExceptions.hpp"

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef _MIN
#define _MIN(a, b) (a < b? a: b)
#endif

#ifndef _MAX
#define _MAX(a, b) (a > b? a: b)
#endif


template<typename T> class AVLNode
{
public:
	AVLNode(void);
	AVLNode(const AVLNode<T> &);
	~AVLNode(void);

	void clearBalance(void) {flags &= 0xfcu;} // balance is in last 2 bits of flags
	void setRightHeavy(void) {clearBalance(); flags |= 0x1u;} // bit0 is RIGHT-HEAVY flag
	void setLeftHeavy(void) {clearBalance(); flags |= 0x2u;} // bit1 is LEFT-HEAVY flag
	int getBalance(void) 
	{ // returns {-1, 0, +1} for left-heavy, balanced or right-heavy respectively (might throw AVLX::balance())
		int r = flags & 0x3u; // last 2 bits s/b 00, 10 or 01 not 11
		if (r == 3) throw AVLX::balance();
		if (r == 2) r = -1;
		return r;
	}

    T * Data;
    AVLNode * right, * left;
    BYTE flags;

};

template<typename T> using pAVLNode = class AVLNode<T> *;


template<typename T> 
AVLNode<T>::AVLNode(void): Data(NULL), right(NULL), left(NULL), flags(0) {}

template<typename T>
AVLNode<T>::AVLNode(const AVLNode<T> & a): Data(NULL), right(NULL), left(NULL)
{
	if (a.Data != NULL) Data = new T(*a.Data);
	if (a.right != NULL) right = new AVLNode<T>(*a.right); // recursive
	if (a.left != NULL) left = new AVLNode<T>(*a.left); // recursive
	flags = a.flags;
}

template<typename T>
AVLNode<T>::~AVLNode(void)
{
	if (Data != NULL) delete Data;
	if (right != NULL) delete right; // recursive
	if (left != NULL) delete left; // recursive
	Data = NULL;
	right = NULL;
	left = NULL;
	flags = 0;

}

template<typename T>
class AVLPathElement
{
public:
	AVLPathElement(void): node(NULL), direction(' ') {}
	AVLPathElement(const AVLPathElement<T> & a): node(a.node), direction(a.direction) {}
	AVLPathElement(const pAVLNode<T> n, const char d): node(n), direction(d) {}
	AVLPathElement & operator = (const AVLPathElement<T> & a) {node = a.node; direction = a.direction; return *this;}

	pAVLNode<T> node;
	char direction;

};
template<typename T> using pAVLPathElement = class AVLPathElement<T> *;

// for a path: typename S tracks log(N) - so the tree max here is ~ 2^256
template<typename T> 
class AVLPath: public vector_c<AVLPathElement<T>, BYTE> 
{ // implementing a one-at-a-time vector:
public:
	//AVLPath(void): vector_c<AVLPathElement<T>, BYTE>() {}
	//AVLPath(const AVLPath<T> & a): vector_c<AVLPathElement<T>, BYTE>(a) {}
	BYTE push(const AVLPathElement<T> & a) {return vector_c<AVLPathElement<T>, BYTE>::push(a);}
	BYTE push(const pAVLNode<T> & n, const char & d) {return vector_c<AVLPathElement<T>, BYTE>::push(AVLPathElement<T>(n, d));}

};
template<typename T> using pAVLPath = class AVLPath<T> *;

template<typename T>
class AVL
{
public:
    // members:
    pAVLNode<T> pRoot; //, ** levels;
    //AVLPath<T> * sPath;
	AVLPath<T> sPath;
    //int count, // debug check loop check
    //    hCheck;
    //unsigned int pathTop;
	//BYTE pathTop;
    // constructors / destructor:
    AVL(void);
    //void BuildTree(pAVLNode<T>);
    AVL(const AVL &);
    //void DeleteSubTree(pAVLNode<T>&);
    ~AVL(void);
    pAVLNode<T> Find(T *);
    void FindLeaf(T *);
	pAVLNode<T> LeftNeighbor(bool = false);
	pAVLNode<T> RightNeighbor(bool = false);
    void AlwaysInsert(T *);
    bool Insert(T *&);
    bool AVLEmpty(void);
    T GetLeast(bool &);
	T * GetLE(T);
	T * GetGE(T);
	T * GetLT(T);
	T * GetGT(T);
    void Delete(void); // deletes node at sPath[pathTop]
    //void DebugPrintPath(unsigned int);
    //void TreeWrite(const ::std::string, const ::std::string);

private:
	//AVLPath<T> pthBuff;
    BYTE GetRealHeight(pAVLNode<T>);
	int getRealBF(pAVLNode<T>);
    //void nodeToElement(unsigned int, unsigned int, pAVLNode<T>);
    void DeletePath(void);
    //void setHeight(pAVLNode<T>); 
    pAVLNode<T> rotateRight(pAVLNode<T>, pAVLNode<T>, int);
    pAVLNode<T> rotateLeft(pAVLNode<T>, pAVLNode<T>, int);
	pAVLNode<T> rotateRightLeft(pAVLNode<T>, pAVLNode<T>);
	pAVLNode<T> rotateLeftRight(pAVLNode<T>, pAVLNode<T>);
    pAVLNode<T> Rotate(pAVLPathElement<T>, int &);
    //void PathToStr(char *&, int, int *);
    
};
template<typename T> using pAVL = class AVL<T> *;

template<typename T>
AVL<T>::AVL(void):pRoot(NULL) {}

//template<typename T>
//void AVL<T>::BuildTree(pAVLNode<T> p)
//{
//	if (p == NULL) return;
//	AlwaysInsert(p->Data);
//	BuildTree(p->left);
//	BuildTree(p->right);
//}

template<typename T>
AVL<T>::AVL(const AVL & a)
{
	//this->~AVL();
	//pathTop = a.pathTop;
	//delete pRoot;
	pRoot = NULL;
	if (a.pRoot != NULL) pRoot = new AVLNode<T>(*a.pRoot);
	sPath = a.sPath;
	
	//sPath = NULL;
	//if (pathTop > 0)
	//{
	//	sPath = new AVLPath<T>[pathTop];
	//	p = pRoot;
	//	for (unsigned int i = 0; i < pathTop; ++i)
	//	{
	//		char d = a.sPath[i].direction;
	//		sPath[i].direction = d;
	//		sPath[i].node = p;
	//		if (d == 'l') p = p->left;
	//		else p = p->right;
	//	}
	//}
	//count = a.count;
	//checkForDuplicates = a.checkForDuplicates;
}

//template<typename T>
//void AVL<T>::DeleteSubTree(pAVLNode<T>& sTree)
//{
//	if (sTree == NULL) return;
//	DeleteSubTree(sTree->right);
//	DeleteSubTree(sTree->left);
	//delete sTree->Data;
//	delete sTree;
//	sTree = NULL;
	//++count;  // check on debug
//}

template<typename T>
AVL<T>::~AVL(void)
{
	//DeleteSubTree(pRoot);
	delete pRoot;
	pRoot = NULL;
	DeletePath();
	//count = 0;
}

template<typename T>
pAVLNode<T> AVL<T>::Find(T * a)
{ // class T must have ops == and < defined
  // this method creates a path in class vector_c sPath
  // if it finds input parameter *a the last path entry
  // will have the node as element 'node'
  // and the default direction ' ' as element 'direction'
  // o/w last path entry will contain leaf last checked
  // return will be NULL and
  // if direction is 'r' *a is > *leaf.node->data
  // if direction is 'l' *a is < *leaf.node->data
	DeletePath();
	if (a == NULL) return NULL;
	if (pRoot == NULL) return NULL;
	pAVLNode<T> p = pRoot; 
	sPath.push(pRoot, ' ');
	pAVLPathElement<T> pth = &sPath[0];
	BYTE pathTop = 0;
	do
	{
		if (*a == *p->Data) break;
		pth->direction = 'r';
		if (*a < *p->Data) 
		{
			p = p->left;
			pth->direction = 'l';
		}
		else p = p->right;
		if (p != NULL)
		{
			sPath.push(p, ' ');
			pth = &sPath[++pathTop];
		}
	} while (p != NULL);
	return p;
}

template<typename T>
void AVL<T>::FindLeaf(T * a)
{
	if (a == NULL) return;
	DeletePath();
	if (pRoot == NULL) return;
	pAVLNode<T> p = pRoot;
	sPath.push(pRoot, ' ');
	pAVLPathElement<T> pth = &sPath[0];
	BYTE pathTop = 0;
	do
	{
		pth->direction = 'r';
		if (*a < *p->Data) 
		{
			p = p->left;
			pth->direction = 'l';
		}
		else p = p->right;
		if (p != NULL)
		{
			sPath.push(p, ' ');
			pth = &sPath[++pathTop];
		}
	} while (p != NULL);
}

template<typename T>
pAVLNode<T> AVL<T>::LeftNeighbor(bool updatePath)
{ // pre-req: valid sPath
  // returns left neighbor of last searched data in call to Find
	if (sPath.size() == 0) return NULL;
	BYTE pathTop = sPath.size() - 1, pathIdx = pathTop;
	pAVLPathElement<T> pth = &sPath[pathTop];
	char dir = pth->direction;
	bool foundIt = false;
	pAVLNode<T> p = pth->node;
	if (p == NULL) return NULL; // invalid path
	if (dir == 'r') return p; // Find did not
	if (p->left != NULL)
	{
		foundIt = true;
		p = p->left;
		++pathIdx;
	}
	// p->left is NULL, ascend the tree
	else 
	{
		while (pathIdx > 0)
		{
			pth = &sPath[--pathIdx];
			p = pth->node;
			dir = pth->direction;
			if ((dir != 'r') && (dir != 'l')) throw AVLX::path_dir();
			if (dir == 'r') 
			{ // this is the left neighbor
				foundIt = true;
				break; 
			}
			// dir is 'l' - keep ascending
		}
	}
	if (updatePath)
	{
		if (pathIdx > pathTop)
		{
			pth->direction = 'l';
			sPath.push(p, ' ');
		}
		else
		{
			if (foundIt)
			{
				pth->direction = ' ';
				pathTop = pathIdx + 1;
				sPath.sz = pathTop;
			}
			else DeletePath();
		}
	}
	if (foundIt) return p;
	return NULL;
}


template<typename T>
pAVLNode<T> AVL<T>::RightNeighbor(bool updatePath)
{
	//BYTE pthIdx = sPath.size() - 1;
	if (sPath.size() == 0) return NULL;
	BYTE pathTop = sPath.size() - 1, pathIdx = pathTop;
	pAVLPathElement<T> pth = &sPath[pathTop];
	char dir = pth->direction;
	//if (dir != ' ') return NULL; // successful Find required
	bool foundIt = false;
	pAVLNode<T> p = pth->node;
	if (p == NULL) return NULL;
	if (dir == 'l') return p; // Find returned NULL left of leaf
	if (p->right != NULL)
	{
		foundIt = true;
		p = p->right;
		//pth->direction = 'r';
		//sPath.push(p, ' ');
		++pathTop;
		//return p;
	}
	// p->right is NULL, ascend tree
	else 
	{
		while (pathTop > 0)
		{
			pth = &sPath[--pathTop];
			p = pth->node;
			//--sPath.sz;
			dir = pth->direction;
			if ((dir != 'r') && (dir != 'l')) throw AVLX::path_dir();
			if (dir == 'l') 
			{ // this is the right neighbor
				//pth->direction = ' ';
				//return pth->node;
				foundIt = true;
				break;
			}
		}
	}
	if (updatePath)
	{
		if (pathIdx > pathTop)
		{
			pth->direction = 'r';
			sPath.push(p, ' ');
		}
		else
		{
			if (foundIt)
			{
				pth->direction = ' ';
				pathTop = pathIdx + 1;
				sPath.sz = pathTop;
			}
			else DeletePath();
		}
	}
	if (foundIt) return p;
	return NULL;
}

template<typename T>
void AVL<T>::AlwaysInsert(T * a)
{ // equal a's will be added left to right
	pAVLPathElement<T> pth = NULL;
	pAVLNode<T> p = new AVLNode<T>;
	DeletePath();
	p->Data = a;
	if (AVLEmpty())
	{
		pRoot = p;
		sPath.push(p, ' ');
		//pathTop = 0;
		return;
	}
	FindLeaf(a);
	BYTE top = sPath.size() - 1;
	pth = &sPath[top];
	char d = pth->direction;
	if (d == 'r')
	{
		pth->node->right = p;
	}
	else // left
	{
		pth->node->left = p;	
	}
	sPath.push(p, ' ');
	//++pathTop;
	++top;
	do
	{// exit conditions: insert balances a subtree, hit root, imbalance rotated
		pth = &sPath[--top];
		p = pth->node;
		d = pth->direction;
		int bf = p->getBalance(), bfY = 0;
		if ((d != 'l') && (d != 'r')) throw AVLX::path_dir();
		if (d == 'r') ++bf;
		else --bf;
		if (bf == 0)
		{ // tree is balanced:
			p->clearBalance();
			return;
		}
		if ((bf < -1) || (bf > 1)) // imbalance detected
		{
			p = Rotate(pth, bfY);
			if (top == 0) pRoot = p;
			else
			{
				pAVLPathElement<T> lst = &sPath[top - 1];
				d = lst->direction;
				if ((d != 'l') && (d != 'r')) throw AVLX::path_dir();
				if (d == 'r') lst->node->right = p;
				else lst->node->left = p;
			}
			DeletePath();
			return;
		}
		if ((bf != -1) && (bf != 1)) throw AVLX::balance();
		if (bf == 1) p->setRightHeavy();
		else p->setLeftHeavy();
	} while (top > 0);
}

template<typename T>
bool AVL<T>::Insert(T *& a) 
	// returns true iff T is inserted i.e. not in tree before call
	// do not call Find before as this will duplicate call
{
	BYTE top;
	pAVLNode<T> p = NULL;
	pAVLPathElement<T> pth = NULL;
	DeletePath();
	if (AVLEmpty())
	{
		pRoot = new AVLNode<T>;
		pRoot->Data = a;
		//++pathTop;
		sPath.push(pRoot, ' ');
		return true;
	}
	p = Find(a);
	if(p != NULL) 
	{ // It's in the tree!
		a = p->Data;
		return false; // insert did not occur
	}
	// sPath leads to leaf entry point:
	p = new AVLNode<T>;
	p->Data = a;
	// tree is always >1 node:
	top = sPath.size() - 1;
	pth = &sPath[top];
	char d = pth->direction;
	if (d == 'r')
	{
		pth->node->right = p;
	}
	else // left
	{
		pth->node->left = p;	
	}
	sPath.push(p, ' ');
	//++pathTop;
	++top;
	do
	{// exit conditions: insert balances a subtree, hit root, imbalance rotated
		pth = &sPath[--top];
		p = pth->node;
		d = pth->direction;
		int bf = p->getBalance(), bfY = 0;
		if ((d != 'l') && (d != 'r')) throw AVLX::path_dir();
		if (d == 'r') ++bf;
		else --bf;
		if (bf == 0)
		{ // tree is balanced:
			p->clearBalance();
			return true;
		}
		if ((bf < -1) || (bf > 1)) // imbalance detected
		{
			p = Rotate(pth, bfY);
			if (top == 0) pRoot = p;
			else
			{
				pAVLPathElement<T> lst = &sPath[top - 1];
				d = lst->direction;
				if ((d != 'l') && (d != 'r')) throw AVLX::path_dir();
				if (d == 'r') lst->node->right = p;
				else lst->node->left = p;
			}
			DeletePath();
			return true;
		}
		if ((bf != -1) && (bf != 1)) throw AVLX::balance();
		if (bf == 1) p->setRightHeavy();
		else p->setLeftHeavy();
	} while (top > 0);
	return true;
}

template<typename T>
bool AVL<T>::AVLEmpty(void)
{
	return (pRoot == NULL);
}

template<typename T>
T AVL<T>::GetLeast(bool & IsEmpty)
{ // typename T must have a default and copy contructor
	DeletePath();
	T rVal;
	if (AVLEmpty())
	{
		IsEmpty = true;
		return rVal;
	}
	pAVLNode<T> p = pRoot;
	IsEmpty = false;
	pAVLPathElement<T> pth = NULL;
	BYTE pathTop = 0;
	while (p != NULL)
	{
		sPath.push(p, 'l');
		pth = &sPath[pathTop];
		p = p->left;
		if (p != NULL) ++pathTop;
	}
	//pth = &sPath[pathTop];
	p = pth->node;
	rVal = *p->Data;
	//pth->direction = ' ';
	Delete();
	return rVal;
}

template<typename T>
T * AVL<T>::GetLE(T dataIn)
{ // returns tree data which is Less Than or Equal To dataIn
	if (AVLEmpty()) return NULL;
	pAVLNode<T> n = Find(&dataIn);
	if (n != NULL) return n->Data;
	BYTE top = sPath.size() - 1;
	pAVLPathElement<T> pth = &sPath[top];
	char d = pth->direction;
	n = pth->node;
	if (n == NULL) return NULL;
	if (d == 'r') return n->Data;
	n = LeftNeighbor();
	if (n != NULL) return n->Data;
	return NULL;
}

template<typename T>
T * AVL<T>::GetGE(T dataIn)
{ // returns tree data which is Greater Than or Equal To dataIn
	if (AVLEmpty()) return NULL;
	pAVLNode<T> n = Find(&dataIn);
	if (n != NULL) return n->Data;
	BYTE top = sPath.size() - 1;
	pAVLPathElement<T> pth = &sPath[top];
	char d = pth->direction;
	n = pth->node;
	if (n == NULL) return NULL;
	if (d == 'l') return n->Data;
	n = RightNeighbor();
	if (n != NULL) return n->Data;
	return NULL;

}

template<typename T>
T * AVL<T>::GetLT(T dataIn)
{
	if (AVLEmpty()) return NULL;
	pAVLNode<T> n = Find(&dataIn);
	//if (n != NULL) return n->Data;
	BYTE top = sPath.size() - 1;
	pAVLPathElement<T> pth = &sPath[top];
	char d = pth->direction;
	n = pth->node;
	if (n == NULL) return NULL;
	if (d == 'r') return n->Data;
	n = LeftNeighbor();
	if (n != NULL) return n->Data;
	return NULL;

}

template<typename T>
T * AVL<T>::GetGT(T dataIn)
{
	if (AVLEmpty()) return NULL;
	pAVLNode<T> n = Find(&dataIn);
	//if (n != NULL) return n->Data;
	BYTE top = sPath.size() - 1;
	pAVLPathElement<T> pth = &sPath[top];
	char d = pth->direction;
	n = pth->node;
	if (n == NULL) return NULL;
	if (d == 'l') return n->Data;
	n = RightNeighbor();
	if (n != NULL) return n->Data;
	return NULL;

}

template<typename T>
BYTE AVL<T>::GetRealHeight(pAVLNode<T> tNode)
{
	if (tNode == NULL) return 0;
	return (1 + _MAX(GetRealHeight(tNode->right), GetRealHeight(tNode->left)));
}
template<typename T>
int AVL<T>::getRealBF(pAVLNode<T> N)
{
	if (N == NULL) return 0;
	return (GetRealHeight(N->right) - GetRealHeight(N->left));
}
template<typename T>
void AVL<T>::DeletePath(void)
{
	sPath.clear();
	//pathTop = 0;
}

template<typename T>
pAVLNode<T> AVL<T>::rotateRight(pAVLNode<T> x, pAVLNode<T> y, int yBF)
{  
	assert (y == x->left);
	x->left = y->right;
	y->right = x;
	if (0 == yBF)
	{ // only on deletions
		x->setLeftHeavy();
		y->setRightHeavy();
	}
	else
	{
		x->clearBalance();
		y->clearBalance();
	}
	//assert (x->getBalance() == getRealBF(x));
	//assert (y->getBalance() == getRealBF(y));
	return y;
}

template<typename T>
pAVLNode<T> AVL<T>::rotateLeft(pAVLNode<T> x, pAVLNode<T> y, int yBF)
{  
	assert (y == x->right);
	x->right = y->left;
	y->left = x;
	if (0 == yBF)
	{ // only on deletions
		x->setRightHeavy();
		y->setLeftHeavy();
	}
	else
	{ // could be on deletion or insertion
		x->clearBalance();
		y->clearBalance();
	}
	//assert (x->getBalance() == getRealBF(x));
	//assert (y->getBalance() == getRealBF(y));
	return y;
}

template<typename T>
pAVLNode<T> AVL<T>::rotateRightLeft(pAVLNode<T> x, pAVLNode<T> y)
{
	assert (x->right == y);
	pAVLNode<T> z = y->left;
	y->left = z->right;
	z->right = y;
	x->right = z->left;
	z->left = x;
	switch(z->getBalance())
	{
		case -1:
			x->clearBalance();
			y->setRightHeavy();
			break;
		case  0:
			x->clearBalance();
			y->clearBalance();
			break;
		case  1:
			x->setLeftHeavy();
			y->clearBalance();
			break;
	}
	z->clearBalance();
	//assert (x->getBalance() == getRealBF(x));
	//assert (y->getBalance() == getRealBF(y));
	//assert (z->getBalance() == getRealBF(z));
	return z;
}

template<typename T>
pAVLNode<T> AVL<T>::rotateLeftRight(pAVLNode<T> x, pAVLNode<T> y)
{
	assert (x->left == y);
	pAVLNode<T> z = y->right;
	y->right = z->left;
	z->left = y;
	x->left = z->right;
	z->right = x;
	switch(z->getBalance())
	{
		case -1:
			x->setRightHeavy();
			y->clearBalance();
			break;
		case  0:
			x->clearBalance();
			y->clearBalance();
			break;
		case  1:
			x->clearBalance();
			y->setLeftHeavy();
			break;
	}
	z->clearBalance();
	//assert (x->getBalance() == getRealBF(x));
	//assert (y->getBalance() == getRealBF(y));
	//assert (z->getBalance() == getRealBF(z));
	return z;
}


template<typename T>
pAVLNode<T> AVL<T>::Rotate(pAVLPathElement<T> xPth, int & yBF)
{
	// uses xPth to perform proper rotation at x (xPth->node)
	char xDir = xPth->direction, yDir = 'r';
	pAVLNode<T> x = xPth->node, y = xDir == 'r'? x->right: x->left;
	yBF = y->getBalance();
	if (((xDir == 'l') && (yBF <= 0)) || ((xDir == 'r') && (yBF < 0))) yDir = 'l';
	//z->node->clearBalance();
	//y->node->clearBalance();
	//assert(y != NULL);
	//if ((yDir != 'r') && (yDir != 'l')) throw AVLX::path_dir();
	if (xDir == yDir)
	{
		if (xDir == 'l') return rotateRight(x, y, yBF); // Left Left
		else return rotateLeft(x, y, yBF); // Right Right
	}
	if (xDir == 'l') return rotateLeftRight(x, y); // Left Right
	//{
	//	yBF = y->right->getBalance();
	//	y = rotateLeft(y, y->right, yBF);
	//	return rotateRight(x, y, yBF);
	//}
	return rotateRightLeft(x, y); // Right Left

	//z->node->right = rotateRight(y->node);
	//return rotateLeft(z->node);

}

		
template<typename T>
void AVL<T>::Delete(void) // deletes last node in sPath
{ // usually requires operator= for typename T
	if (sPath.size() == 0) throw AVLX::path_empty();
	BYTE top = sPath.size() - 1; //pathTop;
	pAVLPathElement<T> pth = &sPath[top], qth = NULL;
	pAVLNode<T> p = pth->node, q = NULL;
	char d = pth->direction; 
	if (p == NULL) return;  // not found
	// standard BST delete:
	if ((p->left == NULL) && (p->right == NULL))
	{
		if (top == 0)
		{
			delete p;
			pRoot = NULL;
			DeletePath();
			return;
		}
		pth = &sPath[--top];
		d = pth->direction;
		if((d != 'l') && (d != 'r')) throw AVLX::path_dir();
		if (d == 'r') 
		{
			pth->node->right = NULL;
		}
		else pth->node->left = NULL;
		delete p;
	}
	else
	{
		if (p->left == NULL) // 1 child on right
		{
			if (top == 0)
			{
				pRoot = p->right;
				p->right = NULL;
				delete p;
				DeletePath();
				return;
			}
			pth = &sPath[--top];
			d = pth->direction;
			if((d != 'l') && (d != 'r')) throw AVLX::path_dir();
			if (d == 'r') 
			{
				pth->node->right = p->right;
			}
			else pth->node->left = p->right;
			p->right = NULL;
			delete p;
		}
		else
		{
			if (p->right == NULL) // 1 child on left
			{
				if (top == 0)
				{
					pRoot = p->left;
					p->left = NULL;
					delete p;
					DeletePath();
					return;
				}
				pth = &sPath[--top];
				d = pth->direction;
				if((d != 'l') && (d != 'r')) throw AVLX::path_dir();
				if (d == 'r') 
				{
					pth->node->right = p->left;
				}
				else pth->node->left = p->left;
				p->left = NULL;
				delete p;
			}
			else // has both children
			{
				q = p->right;
				pth->direction = 'r';
				sPath.push(q, ' ');
				qth = &sPath[++top];
				do
				{
					q = q->left;
					if (q != NULL)
					{
						qth->direction = 'l';
						sPath.push(q, ' ');
						qth = &sPath[++top];
					}
				} while (q != NULL);

				q = qth->node;  // q is least of nodes greater than p
				delete p->Data;
				p->Data = q->Data;
				q->Data = NULL;
				qth = &sPath[--top];
				d = qth->direction;
				if((d != 'l') && (d != 'r')) throw AVLX::path_dir();
				if (d == 'r') 
				{
					p->right = q->right;
				}
				else qth->node->left = q->right;
				q->right = NULL;
				delete q;
			}
		}
	} //finish of standard BST delete
	// now to balance from top up:
	// exit conditions: processed pRoot, new height = old height
	++top;
	do
	{
		pth = &sPath[--top];
		p = pth->node;//, * y = NULL, * x = NULL;
		d = pth->direction;
		int bf = p->getBalance(), bfY = 10; //, bfR = getRealBF(p);
		if((d != 'l') && (d != 'r')) throw AVLX::path_dir();
		if (d == 'r') --bf;
		else ++bf;
		if ((bf == -1) || (bf == 1))
		{ // height is same
			if (bf == 1) p->setRightHeavy();
			else p->setLeftHeavy();
			DeletePath();
			//assert(bf == getRealBF(p));
			return;
		}
		if (bf == 0) 
		{
			p->clearBalance(); // tree balanced, height has changed
			//assert (bf == getRealBF(p));
		}
		if ((bf < -1) || (bf > 1))
		{
			AVLPathElement pe = AVLPathElement(p, d == 'r'? 'l': 'r'); // this is path to sibling
			qth = &pe;
			p = Rotate(qth, bfY);
			if (top == 0) pRoot = p;
			else
			{
				qth = &sPath[top - 1];
				q = qth->node;
				d = qth->direction;
				if((d != 'l') && (d != 'r')) throw AVLX::path_dir();
				if (q == NULL) throw AVLX::path_node();
				if (d == 'r') q->right = p;
				else q->left = p;
			}
			if (0 == bfY)
			{
				DeletePath();
				return;
			}
		}
	} while (top > 0); 
	DeletePath();
}


