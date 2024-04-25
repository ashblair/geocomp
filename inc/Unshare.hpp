#pragma once
#include "AVL.hpp"

	class UnsXY:public CompGeo::XY
	{
	public:
		UnsXY(void);
		UnsXY(const CompGeo::XY &);
		friend bool operator < (const UnsXY &, const UnsXY &);
	};

	typedef struct UnsPointStruct * pUnsPointType;
	
	typedef struct UnsSegStruct * pUnsSegType;
	typedef struct UnsSegStruct
	{
		UnsXY lo, hi;
		UnsSegStruct(void);
		UnsSegStruct(const UnsSegStruct &);
		UnsSegStruct(const UnsXY &, const UnsXY &);
		UnsSegStruct & operator = (const UnsSegStruct &);
		UnsSegStruct BuildFromPoints(const UnsXY &, const UnsXY &);

	} UnsSegType;

	typedef struct UnsSegListStruct * pUnsSegListType;
	typedef struct UnsSegListStruct
	{
		pUnsSegType seg;
		pUnsSegListType next;

	} UnsSegListType;

	typedef struct UnsPointStruct
	{
		UnsXY point, c, cc;
		UnsPointStruct & operator=(const UnsPointStruct &);
		UnsPointStruct(const UnsPointStruct &);
		UnsPointStruct(void);
		UnsPointStruct(const UnsXY &);
		~UnsPointStruct(void);

	} UnsPointType;



	class Unshare
	{
	public:
		Unshare(void);
		void RemoveSharedEdges(std::vector<pPGonWork> *&);
		std::vector<pPGonWork> ConsolidatePolygons(void);
		void InitializePointTree(void);
		
		AVL<UnsSegType> SegTree, DelTree;
		AVL<UnsPointType> PointTree;
		AVLNode<UnsPointType> * pNode;
	};