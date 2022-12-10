#pragma once

#ifndef MIN_OBJS_IN_BIH_LEAF
#define MIN_OBJS_IN_BIH_LEAF 1
#endif

namespace BIH
{// Bounding Interval Hierarchy, a tree for geometrial primitives (i.e. triangles)
	
	template<typename T> struct BIHPointerType;
	template<typename T> using pBIHPointerType = BIHPointerType<T> *;
 
	template<typename T> struct BIH_Node
	{ // T is a numeric type like int or float or double
		size_t index; // actually pointer OR'd w/ 00(x), 01(y), 10(z) or 11(leaf)
		union
		{
			unsigned int items; // for leafs: number of items in leaf
			T Clip[2]; // for non-leafs: right of left & left of right
		};
	};
	template<typename T> using pBIH_Node = BIH_Node<T> *;

	template<typename T> struct BIHPointerType
	{
		pBIH_Node<T> left, right;
	};

	template<typename T> class BIH
	{
	public:
		BIH(void);

		BIH(VID::pAABBBasic<T>, VID::pViewType<T>, const unsigned int &);
		~BIH(void);

		void clearBIH(void);
		void ShowTreeCounts(void);
		void getBoxesWithin(const VID::AABBBasic<T> &);
		void BIHSimpleTest1(void);
		void ReLoad(VID::pViewType<T>, const unsigned int &);
		void StartFile(void);
		void EndFile(void);

		// MEMBERS:
		BIH_Node<T> * root;
		bool InDebug; // set InDebug and phf in calling function then use ReLoad to debug
		std::ofstream * phf; // this needs to be opened in the calling function for debugs

	private:
		VID::pAABBBasic<T> pBB;
		VID::AABBBasic<T> viewBox;
		//CompGeo::TPoint<T> BBLO, BBHI, viewLO, viewHI;
		//PointCloud::Cloud<CompGeo::AABB<T>> * pcldBIH;
		VID::pViewType<T> pvd;
		unsigned int nm_per_pixel,
			bbCount,
			zm, // clog(nm/pixel) 0 .. 6 = nm .. mm
			nodecount, leafcount, rcount, zerocount, pmax, maxleaf;

		CompGeo::ITYPE rptCount, vtxCount, hedgeCount;
		// END MEMBERS

		// PRIVATE METHODS:
		bool IsContained(VID::AABBBasic<T> &);
		char intervalCompare(VID::AABBBasic<T> &, const unsigned char &);
		void addFacesFromBoxes(const size_t, const unsigned int &);
		void addAllInSubTree(const pBIH_Node<T> &);
		void SearchBIHTree(const pBIH_Node<T> &, VID::AABBBasic<T> &);
		void delSubTree(const pBIH_Node<T> &);
		void Partition(const size_t &, const size_t &, const unsigned char &, size_t &, const T &, const VID::AABBBasic<T> &, BIH_Node<T> &);
		pBIH_Node<T> ZeroLeaf(void);
		pBIH_Node<T> LeafWithBeginIndex(const size_t &, const unsigned int &);
		T GetMaxSide(const VID::AABBBasic<T> &, unsigned char &);
		bool IsWithinPixel(const VID::AABBBasic<T> &, unsigned char &);
		pBIH_Node<T> BuildBIH(VID::AABBBasic<T> &, const size_t &, const size_t &);
		// DEBUG Stuff:
		std::string AABBBasicToStr(const VID::AABBBasic<T> &);
		std::string AABBToStr(const VID::AABB<T> &);
		void ZeroLeafToFile(void);
		void LeafToFile(const size_t &, const size_t &);
		void BuildBIHToFile(const VID::AABBBasic<int> &, const size_t &, const size_t &, const unsigned char &, const T &);
		void WriteToFile(const char *, const std::streamsize &);
	};
};
