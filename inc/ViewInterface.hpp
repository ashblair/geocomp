#pragma once

//class glade_extract;
//typedef class glade_extract GEXT;
//typedef GEXT * pGEXT;

namespace VID // view interface data
{
    typedef struct RectStructType
    { // a rectangle in 3D space
        CompGeo::XYZ LR, UR, UL, LL;

    } RectType, * pRectType;

    typedef struct VolumeStructType
    {
        RectType front, back;

    } VolumeType, * pVolumeType;

    typedef struct FaceAnalysisStructType
    { // to determine VRP on the bounding box based on VRN
        char Name[8];
        int ID, MainAxis, OtherAxes[2], planeConstant;

    } FaceAnalysisType, *pFaceAnalysisType;



	// these next data structures are based on indices into a CompGeo::DCEL vertex cloud:
	typedef struct BasicPointbyIndexStructType
	{ // the vertices in these locations are optima @ x, y & z respectively:
	  // fill 2 of these when reading in a view file: maxVtx and minVtx along each axis
		CompGeo::ITYPE X_IDX, Y_IDX, Z_IDX;
	} BasicPointbyIndex, *pBasicPointbyIndex;

	typedef struct PointbyIndexStructType
	{  // like above w/ additional function of array access
		union
		{
			BasicPointbyIndex xyz_IDX;
			CompGeo::ITYPE dims_IDX[3];
		};
	} PointbyIndex, *pPointbyIndex;

	//typedef struct AABBBbyIndexStructType
	//{
	//	PointbyIndex B_LO_IDX, B_HI_IDX;
	//} AABBbyIndex, *pAABBbyIndex;

	template<typename T> struct AABBBasic
	{ // represents a diagonal of the box like (xLo, yLo, zLo) to (xHi, yHi, zHi)
		AABBBasic<T> & operator=(const AABBBasic<T> & aabb) {LO = aabb.LO; HI = aabb.HI; return *this;}
		CompGeo::BasicTPoint<T> LO, HI;

	};
	template<typename T> using pAABBBasic = AABBBasic<T> *;

	template<typename T> struct AABB
	{
		AABB<T> & operator=(const AABB<T> & aabb) {BB = aabb.BB; objID = aabb.objID; return *this;}
		AABBBasic<T> BB;
		CompGeo::ITYPE objID; // index into fCld

	};
	template<typename T> using pAABB = AABB<T> *;

	template<typename T> struct ViewType
	{
		ViewType<T>(void): dcel0(NULL), dcel_view(NULL) {}
		CompGeo::DCEL<T> * dcel0, * dcel_view;
		
		std::vector<AABB<T>> bCld;
		size_t push(const AABB<T> &);

		AABBBasic<T> bb;

		CompGeo::ITYPE LoIdx[3], HiIdx[3];

	};
	template<typename T> using pViewType = ViewType<T> *;


	typedef ViewType<int> ViewInt;// ViewType<int>;
	typedef pViewType<int> pViewInt;

	enum defaults {NoDflt = 0, 
		LoX = 1, LoY = 2, LoZ = 3, 
		HiX = 4, HiY = 5, HiZ = 6};
	
	enum projections {Parallel = 0, Perspective = 1};

	typedef struct ViewMetricsStructType
	{
		CompGeo::XYZ LO, HI, VRP, VUP, VRN, PRP;
		unsigned int SCALE, // in nm per pixel 
				DFLTVIEWFLAG, // 0=no default, lowest 1=X, 2=Y, 4=Z, highest 8=X, 16=Y, 32=Z
				PRJTYPEFLAG, // 0=parallel, 1=perspective
				DEPTH; 
		double THETA, PHI, ALPHA;
		int THETADEG, PHIDEG, ALPHADEG;


	} ViewMetrics, * pViewMetrics;
/*
	{ // every view file interpreter (like PLYHandler) needs to fill one of these
	  // floating point data should be scaled to the integer billion box
	  // where each octet holds a cube a billion integers on a side 
		CompGeo::DCEL<int> * dcel0, * dcel_view;
		
		std::vector<AABB<int>> bCld;
		size_t push(const AABB<int> &);

		AABBBasic<int> bb;

		size_t LoIdx[3], HiIdx[3];

	} ViewInt, *pViewInt;
*/
/*
	typedef struct HalfTwinLUStructType
	{ // half is from point A to point B
		size_t halfIdx, AIdx, BIdx, twinIdx;   
		HalfTwinLUStructType(void);
		HalfTwinLUStructType(const size_t &, const size_t &, const size_t &);
		HalfTwinLUStructType(const HalfTwinLUStructType &);
		HalfTwinLUStructType & operator=(const HalfTwinLUStructType &);
		bool IsEq(const HalfTwinLUStructType &) const;
		friend bool operator==(const HalfTwinLUStructType &a, const HalfTwinLUStructType &b) {return a.IsEq(b);}
		bool IsLT(const HalfTwinLUStructType &) const;
		friend bool operator<(const HalfTwinLUStructType &a, const HalfTwinLUStructType &b) {return a.IsLT(b);}

	} HalfTwinLUType, * pHalfTwinLUType;

	template<typename T> struct TPoint2
	{
		union
		{
			BasicTPoint<T> xyz;
			T dims[3];
		};
		union
		{
			BasicTPoint<T> xyzBuff;
			T dimsBuff[3];
		};
		char flag;  //state info e.g:: 'f'=from file, 'F'=in Face, 't'=transformed
		unsigned int vertex, incidentedge, viewvertex;
	};
*/


/*
	typedef struct ViewDataStructType
	{
		PointCloud::Cloud<FaceType>  * fCld, * fvCld;
		PointCloud::Cloud<TPoint<int>> * pCld, * pvCld;
		PointCloud::Cloud<HalfEdgeType> * hCld, * hvCld;
		PointCloud::Cloud<AABB<int>> * bCld;

		AABBBasic<int> bbScaled;
		AABBbyIndex bbIdx;
		TPoint<int> bbCtr;

	} ViewData, * pViewData;

	typedef struct ViewStaticsDataStructType
	{
		PointCloud::Cloud<FaceType>  * fCld;
		PointCloud::Cloud<TPoint2<int>> * pCld;
		PointCloud::Cloud<HalfEdgeType> * hCld;

		// startAt, count and cubeSide will be arrays for each static set
		// the static images are contained w/in a cube w/ a side of cubeSide
		int numStatics, numView,
			*fstartAt, *fcount, 
			*pstartAt, *pcount, 
			*hstartAt, *hcount, 
			*cubeSide, *viewArray;

		GdkRGBA * clrs;
		//LPCOLORREF clrs;
		//AABBBasic<int> bbScaled;
		//AABBbyIndex bbIdx;

	} ViewStatics, *pViewStatics;
*/


};