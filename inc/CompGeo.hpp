#pragma once

//using namespace std;

#ifndef MAX_FLT_PRECISION 
#define MAX_FLT_PRECISION 0.000000001
#endif

#ifndef BPLUS_LO_LEAF
#define BPLUS_LO_LEAF -1048576
#endif

#ifndef BPLUS_HI_LEAF
#define BPLUS_HI_LEAF 1048576
#endif

#ifndef DIRECTDRAW_VERSION
#ifndef DIRECT3D_VERSION
// these structures are substitutes for the d2d1 library:
typedef float FLOAT;
typedef struct D2D1_POINT_2FstructType
{
	FLOAT x, y;
} D2D1_POINT_2F, * pD2D1_POINT_2F;
#endif
#endif

// These structures are to implement algorithms in 
// the book Computational Geometry

namespace CompGeo
{
	bool BEq(const char, const char);
	bool BEq(const int, const int);
	bool BEq(const long, const long);
	bool BEq(const long long, const long long);
	bool BEq(const float, const float);
	bool BEq(const double, double);
	bool BEq(const long double, long double);

	template<typename T> struct BasicTPoint;

	typedef struct XYStructType //Plain Old Data POD
	{
		float x, y;
	} XYType;

	typedef class XY * pXY;
	class XY // could be 2D point or vector
	{
	public:
		XY(void);
		XY(const XY&);
		XY(D2D1_POINT_2F);
		XY(XYType);
		XY(double, double);
		XY(const BasicTPoint<int> &);
		XY(const BasicTPoint<double> &);
		XY & operator = (const XY&);
		bool IsEq(const XY&) const;
		//use inheritance for operator <
		friend bool operator==(const XY&a, const XY&b) {return (a.IsEq(b));}
		friend bool operator!=(const XY&a, const XY&b) {return !(a.IsEq(b));}
		XY & operator += (const XY&);
		XY & operator -= (const XY&);
		XY & operator *= (const double&); // scaler multiplication
		XY & operator /= (const double&); // scaler division
		friend XY operator*(const double&a, const XY&b) {CompGeo::XY r(b); r *= a; return r; }
		friend XY operator*(const XY&a, const double&b) {CompGeo::XY r(a); r *= b; return r;}
		friend XY operator/(const XY&a, const double&b) {CompGeo::XY r(a); r *= 1.0 / b; return r;}
		friend double operator*(const XY&a, const XY&b) {return a.x * b.x + a.y * b.y;} // dot product
		friend XY operator+(const XY&a, const XY&b) {CompGeo::XY r(a); r += b; return r;}
		friend XY operator-(const XY&a, const XY&b) {CompGeo::XY r(a); r -= b; return r;}
		D2D1_POINT_2F GetPOINT(void) const;
		double GetMagnitude(void) const;
		double sqr(const double a) const {return a * a;}
		void PrintXY(void) const {std::cout << "(x=" << std::to_string(x) << ", y=" << std::to_string(y) << ") ";}

		double x, y;
		//bool IsNonDefault;
		
	};



	typedef class XYZ * pXYZ;
	class XYZ // could be 3D point or vector
	{
	public:
		XYZ(void);
		XYZ(double, double, double);
		XYZ(const XYZ &);
		XYZ(const BasicTPoint<int> &);
		XYZ(const BasicTPoint<double> &);
		XYZ & operator = (const XYZ&);
		bool IsEq(const XYZ&) const;
		friend bool operator == (const XYZ&a, const XYZ&b) {return (a.IsEq(b));}
		friend bool operator !=(const XYZ&a, const XYZ&b) {return !(a.IsEq(b));}
		XYZ & operator -(void) { x = -x; y = -y; z = -z; return *this; }
		XYZ & operator += (const XYZ&);
		XYZ & operator -= (const XYZ&);
		XYZ & operator *= (const double&); // scaler multiplication
		XYZ & operator /= (const double&); // scaler division
		friend XYZ operator * (const double&a, const XYZ&xyz) {CompGeo::XYZ rVal = xyz; rVal *= a; return rVal;}
		friend XYZ operator * (const XYZ&a, const double &b) {CompGeo::XYZ r(a); r *= b; return r;}
		friend XYZ operator / (const XYZ&a, const double & b) {CompGeo::XYZ r(a); r *= 1.0 / b; return r;}
		friend double operator * (const XYZ&a, const XYZ&b) {return a.x * b.x + a.y * b.y + a.z * b.z;} //dot prouduct
		friend XYZ operator + (const XYZ&a, const XYZ&b) {CompGeo::XYZ rVal = a; rVal += b; return rVal;}
		friend XYZ operator - (const XYZ&a, const XYZ&b) {CompGeo::XYZ rVal = a; rVal -= b; return rVal;}
		double GetMagnitude(void) const;
		double sqr(const double a) const {return a * a;}
		BasicTPoint<int> GetBasicTPoint(void) const;
		void PrintXYZ(void) const {std::cout << "(x=" << std::to_string(x) << ",y=" << std::to_string(y) << ",z=" << std::to_string(z) << ") ";}

		double x, y, z;
	};

	class XYZPlus:public XYZ
	{
	public:
		XYZPlus(void) {dims[0] = &x; dims[1] = &y; dims[2] = &z;}
		XYZPlus(const XYZ & a) {x = a.x; y = a.y; z = a.z; dims[0] = &x; dims[1] = &y; dims[2] = &z;}

		double * dims[3];
	};

	typedef struct UnitCircleMeasureStructType
	{ // for measuring angular differences for a group of common-centered rays
	  // relative to one of them without degrees or radians or arc functions
	  // w/ 2 vectors AB and BC construct a UnitCircleMeasureStructType w/ A, B & C
	  // (the measure will be like a protractor for angle CBA centered @B w/ baseline toward A)	
		UnitCircleMeasureStructType(void);
		UnitCircleMeasureStructType(const UnitCircleMeasureStructType&);
		UnitCircleMeasureStructType(CompGeo::XY, CompGeo::XY, CompGeo::XY);
		UnitCircleMeasureStructType(CompGeo::XYZ, CompGeo::XYZ, CompGeo::XYZ);
		UnitCircleMeasureStructType & operator = (const UnitCircleMeasureStructType&);
		
		bool IsEq(const UnitCircleMeasureStructType&) const;
		bool IsGT(const UnitCircleMeasureStructType&) const;
		bool IsLT(const UnitCircleMeasureStructType&) const;

		friend bool operator==(const UnitCircleMeasureStructType&a, const UnitCircleMeasureStructType&b) {return a.IsEq(b);}
		friend bool operator<(const UnitCircleMeasureStructType&a, const UnitCircleMeasureStructType&b) {return a.IsLT(b);}
		friend bool operator>(const UnitCircleMeasureStructType&a, const UnitCircleMeasureStructType&b) {return a.IsGT(b);}
		void PrintUnitCircleMeasure(void) {std::cout << "Quad++ [" << std::to_string(QuadPP) << "] ~tangent [" << std::to_string(the_tan) << "] ";}

		unsigned int QuadPP;	// 0 - 8: 0=origin point, 1=x axis, 2=Q1, 3=y axis, 4=Q2, 5=-x axis, 6=Q3, 7=-y axis, 8=Q4
		double the_tan;			// 0 for even QuadPP otherwise y/x

	} UnitCircleMeasureType, * pUnitCircleMeasureType;

	typedef struct FaceStructType * pFaceType;
	typedef struct HalfEdgeStructType * pHalfEdgeType;
	typedef struct HalfEdgeListStructType * pHalfEdgeListType;
	template<typename T> struct TPoint;
	template<class T> using pTPoint = TPoint<T>*; 

	typedef unsigned int ITYPE;

	typedef struct HalfEdgeStructType
	{
		HalfEdgeStructType(void) {halfedge = 0; origin = 0; twin = 0; incidentface = 0; next = 0; prev = 0; viewedge = 0; flag = '\0'; additionalHalfEdgeInfo = NULL;}
		HalfEdgeStructType(const HalfEdgeStructType & a) {halfedge = a.halfedge; origin = a.origin; twin = a.twin; incidentface = a.incidentface; next = a.next; prev = a.prev; viewedge = a.viewedge; flag = a.flag; additionalHalfEdgeInfo = a.additionalHalfEdgeInfo;}
		HalfEdgeStructType & operator = (const HalfEdgeStructType & h)
		{
			halfedge = h.halfedge; origin = h.origin; twin = h.twin; incidentface = h.incidentface; next = h.next; prev = h.prev; viewedge = h.viewedge;
			flag = h.flag;
			additionalHalfEdgeInfo = h.additionalHalfEdgeInfo;
			return *this;
		}
		void PrintHalfEdge(void)
		{
			std::cout << "flag [" << flag << "halfedge [" << std::to_string(halfedge) << "] origin [" << std::to_string(origin) << "] twin [" << 
			std::to_string(twin) << "] incidentface [" << std::to_string(incidentface) << "] next [" << std::to_string(next) << "] prev [" << 
			std::to_string(prev) << "] viewedge [" << std::to_string(viewedge) << "] " << 
			(std::string)(additionalHalfEdgeInfo==NULL?"N":"+") << "\n";
		}

		ITYPE halfedge, origin, twin, incidentface, next, prev, viewedge;
		char flag;
		void * additionalHalfEdgeInfo;

	} HalfEdgeType;

	typedef struct HalfEdgeListStructType
	{
		HalfEdgeListStructType(void) { helsttop = 0; nxt = 0; additionalHalfEdgeListInfo = NULL; halfedgelist = 0;}
		HalfEdgeListStructType(const HalfEdgeListStructType & a) {helsttop = a.helsttop; nxt = a.nxt; additionalHalfEdgeListInfo = a.additionalHalfEdgeListInfo; halfedgelist = a.halfedgelist;}
		HalfEdgeListStructType & operator = (const HalfEdgeListStructType & l)
		{
			halfedgelist = l.halfedgelist;
			helsttop = l.helsttop;
			nxt = l.nxt;
			additionalHalfEdgeListInfo = l.additionalHalfEdgeListInfo;
			return *this;
		}
		void PrintHalfEdgeList(void)
		{
			std::cout << "halfedgelist [" << std::to_string(halfedgelist) << "] helsttop [" << std::to_string(helsttop) << "] nxt [" << 
			std::to_string(nxt) << "] " << (std::string)(additionalHalfEdgeListInfo==NULL?"N":"+") << "\n";
		}

		ITYPE halfedgelist, helsttop, nxt;
		void * additionalHalfEdgeListInfo;

	} HalfEdgeListType;

	typedef struct FaceStructType
	{// set up the doubly connected edged list in point clouds
		//unsigned int verts[3];
		FaceStructType(void) {face = 0; outer = 0; viewface = 0; inner = 0; flag = '\0'; additionalFaceInfo = NULL;}
		FaceStructType(const FaceStructType & a) {face = a.face; outer = a.outer; viewface = a.viewface; inner = a.inner; norm = a.norm; flag = a.flag; additionalFaceInfo = a.additionalFaceInfo;}
		FaceStructType & operator = (const FaceStructType& f)
		{
			face = f.face; outer = f.outer; viewface = f.viewface; inner = f.inner;
			norm = f.norm;
			flag = f.flag;
			additionalFaceInfo = f.additionalFaceInfo;
			return *this;
		}
		void PrintFace(void)
		{
			std::cout << "flag [" << flag << "] face [" << std::to_string(face) << "] outer [" << std::to_string(outer) << "] viewface [" 
				<< std::to_string(viewface) << "] inner [" << std::to_string(inner) << "] norm: ";
			norm.PrintXYZ();
			std::cout << (std::string)(additionalFaceInfo==NULL?"N":"+");
			std::cout << "\n";
		}

		ITYPE face, outer, viewface, inner;
		CompGeo::XYZ norm;
		char flag;
		void * additionalFaceInfo;

	} FaceType;

	
	template<typename T> struct BasicTPoint
	{
		BasicTPoint(void) {X = static_cast<T>(0); Y = static_cast<T>(0); Z = static_cast<T>(0);}
		BasicTPoint(const TPoint<T> & a) {X = a.xyz.X; Y = a.xyz.Y; Z = a.xyz.Z;}
		BasicTPoint(const BasicTPoint<T> & a) {X = a.X; Y = a.Y; Z = a.Z;}
		BasicTPoint(const T & x, const T & y, const T & z) {X = x; Y = y; Z = z;}
		BasicTPoint<T> & operator = (const BasicTPoint<T> & a) {X = a.X; Y = a.Y; Z = a.Z; return *this;}
		bool IsEq(const BasicTPoint<T>&a) const {return (BEq(X, a.X) && BEq(Y, a.Y) && BEq(Z, a.Z));}
		friend bool operator == (const BasicTPoint<T>&a, const BasicTPoint<T>&b) {return (a.IsEq(b));}
		friend bool operator !=(const BasicTPoint<T>&a, const BasicTPoint<T>&b) {return !(a.IsEq(b));}
		BasicTPoint<T> & operator -(void) { X = -X; Y = -Y; Z = -Z; return *this; }
		BasicTPoint<T> & operator += (const BasicTPoint<T>&a){X += a.X; Y += a.Y; Z += a.Z; return *this;}
		BasicTPoint<T> & operator -= (const BasicTPoint<T>&a) {X -= a.X; Y -= a.Y; Z -= a.Z; return *this;}
		BasicTPoint<T> & operator *= (const T a) {X *= a; Y *= a; Z *= a; return *this;} // scaler multiplication
		BasicTPoint<T> & operator /= (const T a) {return (*this *= (static_cast<T>(1) / a));} // scaler division
		friend BasicTPoint<T> operator * (const T&a, const BasicTPoint<T>&b) {CompGeo::BasicTPoint<T> r(b); r *= a; return r;}
		friend BasicTPoint<T> operator * (const BasicTPoint<T>&a, const T&b) {CompGeo::BasicTPoint<T> r(a); r *= b; return r;}
		friend BasicTPoint<T> operator / (const BasicTPoint<T>&a, const T&b) {CompGeo::BasicTPoint<T> r(a); r *=(static_cast<T>(1) / b); return r;}
		friend T operator * (const BasicTPoint<T>&a, const BasicTPoint<T>&b) {return a.X * b.X + a.Y * b.Y + a.Z * b.Z;} //dot prouduct
		friend BasicTPoint<T> operator + (const BasicTPoint<T>&a, const BasicTPoint<T>&b) {CompGeo::BasicTPoint<T> r(a); r += b; return r;}
		friend BasicTPoint<T> operator - (const BasicTPoint<T>&a, const BasicTPoint<T>&b) {CompGeo::BasicTPoint<T> r(a); r -= b; return r;}
		double GetMagnitude(void) {return (sqrt(sqr(X) + sqr(Y) + sqr(Z)));}
		double sqr(const T a) {return static_cast<double>(a * a);}
		void PrintBasicPoint(void) {std::cout << "(X=" << std::to_string(X) << ",Y=" << std::to_string(Y) << ",Z=" << std::to_string(Z) << ")";}
		T & dim(unsigned char a) 
		{
			switch (a)
			{
			case 0: return X; break;
			case 1: return Y; break;
			case 2: return Z; break;
			}
			assert (1==0);
			//std::cout << "Error in CompGeo::BasicTPoint trying to get a dim of " << a << " returning dim(0)=&X which is " << X << "\n";
			return X; // this is an error
		}

		T X, Y, Z;
	};

	template<typename T> struct TPoint
	{
		TPoint(void) {xyz = CompGeo::BasicTPoint<T>(); flag = ' '; vertex = 0; incidentedge = 0; viewvertex = 0; additionalTPointInfo = NULL;}
		TPoint(const BasicTPoint<T> & a) {xyz = a; flag = ' '; vertex = 0; incidentedge = 0; viewvertex = 0; additionalTPointInfo = NULL;}
		TPoint(const TPoint<T> & a) {xyz = CompGeo::BasicTPoint<T>(a.xyz); flag = a.flag; vertex = a.vertex; incidentedge = a.incidentedge; viewvertex = a.viewvertex; additionalTPointInfo = a.additionalTPointInfo;}
		TPoint(const T & x, const T & y, const T & z) {xyz = CompGeo::BasicTPoint<T>(x,y,z); flag = ' '; vertex = 0; incidentedge = 0; viewvertex = 0; additionalTPointInfo = NULL;}
		TPoint<T> & operator = (const TPoint<T> & a)
		{
			xyz = a.xyz;
			flag = a.flag;
			vertex = a.vertex; incidentedge = a.incidentedge; viewvertex = a.viewvertex;
			additionalTPointInfo = a.additionalTPointInfo;
			return *this;
		}
		bool IsEq(const TPoint<T>&a) const {return xyz.IsEq(a.xyz);}
		friend bool operator == (const TPoint<T>&a, const TPoint<T>&b) {return (a.IsEq(b));}
		friend bool operator !=(const TPoint<T>&a, const TPoint<T>&b) {return !(a.IsEq(b));}
		TPoint<T> & operator -(void) { xyz = -xyz; return *this; }
		TPoint<T> & operator += (const TPoint<T>&a){xyz += a.xyz; return *this;}
		TPoint<T> & operator -= (const TPoint<T>&a) {xyz -= a.xyz; return *this;}
		TPoint<T> & operator *= (const T a) {xyz *= a; return *this;} // scaler multiplication
		TPoint<T> & operator /= (const T a) {xyz /= a; return *this;} // scaler division
		friend TPoint<T> operator * (const T&a, const TPoint<T>&b) {CompGeo::TPoint<T> r(b); r *= a; return r;}
		friend TPoint<T> operator * (const TPoint<T>&a, const T&b) {CompGeo::TPoint<T> r(a); r *= b; return r;}
		friend TPoint<T> operator / (const TPoint<T>&a, const T&b) {CompGeo::TPoint<T> r(a); r *=(1.0 / b); return r;}
		friend T operator * (const TPoint<T>&a, const TPoint<T>&b) {return a.xyz * b.xyz;} //dot prouduct
		friend TPoint<T> operator + (const TPoint<T>&a, const TPoint<T>&b) {CompGeo::TPoint<T> r(a); r += b; return r;}
		friend TPoint<T> operator - (const TPoint<T>&a, const TPoint<T>&b) {CompGeo::TPoint<T> r(a); r -= b; return r;}
		double GetMagnitude(void) {return (xyz.GetMagnitude());}
		double sqr(const T a) {return static_cast<double>(a * a);}
		void PrintPoint(void)
		{
			std::cout << "flag[" << flag << "] vertex[" << std::to_string(vertex) << "] incidentedge[" << 
			std::to_string(incidentedge) << "] viewvertex[" << std::to_string(viewvertex) << "] ";
			xyz.PrintBasicPoint();
			std::cout << (std::string)(additionalTPointInfo==NULL?"N":"+") << "\n";
		}


		union
		{
			BasicTPoint<T> xyz;
			T dims[3];
		};
		char flag;  //state info e.g:: 'f'=from file, 'F'=in Face, 't'=transformed
		ITYPE vertex, incidentedge, viewvertex;
		void * additionalTPointInfo;
	};


	template<typename T> class DCEL // entry 0 is your "NULL" entry
	{ // a doubly connected edge list using std::vectors
	  // initial state: fIdx points to face that is the containing rectangle
	  // outer = 0 (i.e. NULL), inner = screen rect or parts thereof + faces w/ edges on the screen's border
	public:

		DCEL(void) {fIdx = 1; pIdx = 1; hIdx = 1; lIdx = 1; // simulate a NULL(list termination) with entry 0
			pFaceType f = new FaceType; pHalfEdgeType h = new HalfEdgeType; pHalfEdgeListType l = new HalfEdgeListType; pTPoint<T> t = new TPoint<T>;
			push(f); push(h); push(l); push(t);}
		DCEL(const DCEL &d0)
		{ // this will copy NULL entry 0 from a default constructed DCEL
			for (std::vector<pFaceType>::iterator fit = d0.fCld.begin(); fit < d0.fCld.end(); ++fit) {pFaceType f = new FaceType(**fit); push(f);}
			for (typename std::vector<pTPoint<T>>::iterator pit = d0.pCld.begin(); pit < d0.pCld.end(); ++pit) {pTPoint<T> t = new TPoint<T>(**pit); push(t);}
			for (std::vector<pHalfEdgeType>::iterator hit = d0.hCld.begin(); hit < d0.hCld.end(); ++hit) {pHalfEdgeType h = new HalfEdgeType(**hit); push(h);}
			for (std::vector<pHalfEdgeListType>::iterator lit = d0.lCld.begin(); lit < d0.lCld.end(); ++lit) {pHalfEdgeListType l = new HalfEdgeListType(**lit); push(l);}

		}
		~DCEL(void) 
		{	for (std::vector<pFaceType>::iterator fit = fCld.begin(); fit < fCld.end(); ++fit) delete (*fit);
			for (typename std::vector<pTPoint<T>>::iterator pit = pCld.begin(); pit < pCld.end(); ++pit) delete (*pit);
			for (std::vector<pHalfEdgeType>::iterator hit = hCld.begin(); hit < hCld.end(); ++hit) delete (*hit);
			for (std::vector<pHalfEdgeListType>::iterator lit = lCld.begin(); lit < lCld.end(); ++lit) delete (*lit);
			fCld.clear(); pCld.clear(); hCld.clear(); lCld.clear(); 
			fIdx = 0; pIdx = 0; hIdx = 0; lIdx = 0;}
		DCEL<T> & operator=(const DCEL<T> & d0)
		{
		{	for (std::vector<pFaceType>::iterator fit = fCld.begin(); fit < fCld.end(); ++fit) delete (*fit);
			for (typename std::vector<pTPoint<T>>::iterator pit = pCld.begin(); pit < pCld.end(); ++pit) delete (*pit);
			for (std::vector<pHalfEdgeType>::iterator hit = hCld.begin(); hit < hCld.end(); ++hit) delete (*hit);
			for (std::vector<pHalfEdgeListType>::iterator lit = lCld.begin(); lit < lCld.end(); ++lit) delete (*lit);
			fCld.clear(); pCld.clear(); hCld.clear(); lCld.clear(); 
			fIdx = 0; pIdx = 0; hIdx = 0; lIdx = 0;}
			for (std::vector<pFaceType>::iterator fit = d0.fCld.begin(); fit < d0.fCld.end(); ++fit) {pFaceType f = new FaceType(**fit); push(f);}
			for (typename std::vector<pTPoint<T>>::iterator pit = d0.pCld.begin(); pit < d0.pCld.end(); ++pit) {pTPoint<T> t = new TPoint<T>(**pit); push(t);}
			for (std::vector<pHalfEdgeType>::iterator hit = d0.hCld.begin(); hit < d0.hCld.end(); ++hit) {pHalfEdgeType h = new HalfEdgeType(**hit); push(h);}
			for (std::vector<pHalfEdgeListType>::iterator lit = d0.lCld.begin(); lit < d0.lCld.end(); ++lit) {pHalfEdgeListType l = new HalfEdgeListType(**lit); push(l);}

		}
		void clear_all(void) 
		{	for (std::vector<pFaceType>::iterator fit = fCld.begin(); fit < fCld.end(); ++fit) delete (*fit);
			for (typename std::vector<pTPoint<T>>::iterator pit = pCld.begin(); pit < pCld.end(); ++pit) delete (*pit);
			for (std::vector<pHalfEdgeType>::iterator hit = hCld.begin(); hit < hCld.end(); ++hit) delete (*hit);
			for (std::vector<pHalfEdgeListType>::iterator lit = lCld.begin(); lit < lCld.end(); ++lit) delete (*lit);
			fCld.clear(); pCld.clear(); hCld.clear(); lCld.clear(); fIdx = 1; pIdx = 1; hIdx = 1; lIdx = 1;
			pFaceType f = new FaceType; pHalfEdgeType h = new HalfEdgeType; pHalfEdgeListType l = new HalfEdgeListType; TPoint<T> * t = new TPoint<T>;
			push(f); push(h); push(l); push(t);}
		size_t getFaceCount(void) {return fCld.size();}
		size_t getVertexCount(void) {return pCld.size();}
		size_t getHalfEdgeCount(void) {return hCld.size();}
		size_t getHalfEdgeListCount(void) {return lCld.size();}
		size_t getFaceCapacity(void) {return fCld.capacity();}
		size_t getVertexCapacity(void) {return pCld.capacity();}
		size_t getHalfEdgeCapacity(void) {return hCld.capacity();}
		size_t getHalfEdgeListCapacity(void) {return lCld.capacity();}
		pFaceType getFace(const ITYPE & i) {fIdx = i; return fCld[i];}
		pFaceType getFace(void) {return fCld[fIdx];}
		pTPoint<T> getPoint(const ITYPE & i) {pIdx = i; return pCld[i];}
		pTPoint<T> getPoint(void) {return pCld[pIdx];}
		pHalfEdgeType getHalfEdge(const ITYPE & i) {hIdx = i; return hCld[i];}
		pHalfEdgeType getHalfEdge(void) {return hCld[hIdx];}
		pHalfEdgeListType getHalfEdgeList(const ITYPE & i) {lIdx = i; return lCld[i];}
		pHalfEdgeListType getHalfEdgeList(void) {return lCld[lIdx];}
		void pop(const char & c) 
		{	if (c == 'f') fCld.pop_back(); 
			if(c == 'p') pCld.pop_back(); 
			if (c == 'h') hCld.pop_back(); 
			if (c == 'l') lCld.pop_back();}
		void erase(const char & c, const size_t i) 
		{	if (c == 'f') fCld.erase(fCld.begin() + i); 
			if (c == 'p') pCld.erase(pCld.begin() + i); 
			if (c == 'h') hCld.erase(hCld.begin() + i); 
			if (c == 'l') lCld.erase(lCld.begin() + i);}
		ITYPE push(pFaceType & f) {fIdx = fCld.size(); fCld.push_back(f); return fIdx;}
		ITYPE push(pTPoint<T> & p) {pIdx = pCld.size(); pCld.push_back(p); return pIdx;}
		ITYPE push(pHalfEdgeType & h) {hIdx = hCld.size(); hCld.push_back(h); return hIdx;}
		ITYPE push(pHalfEdgeListType & l) {lIdx = lCld.size(); lCld.push_back(l); return lIdx;}
		void insert(pFaceType & f, const ITYPE & b4) {fIdx = b4; fCld.insert(fCld.begin() + b4, f);}
		void insert(TPoint<T>* & p, const ITYPE & b4) {pIdx = b4; pCld.insert(pCld.begin() + b4, p);}
		void insert(pHalfEdgeType & h, const ITYPE & b4) {hIdx = b4; hCld.insert(hCld.begin() + b4, h);}
		void insert(pHalfEdgeListType & l, const ITYPE & b4) {lIdx = b4; lCld.insert(lCld.begin() + b4, l);}
		void poke(pFaceType & f, const ITYPE & at) {fIdx = at; fCld[fIdx] = f;}
		void poke(pTPoint<T> & p, const ITYPE & at) {pIdx = at; pCld[pIdx] = p;} 
		void poke(pHalfEdgeType & h, const ITYPE & at) {hIdx = at; hCld[hIdx] = h;}
		void poke(pHalfEdgeListType & l, const ITYPE & at) {lIdx = at; lCld[lIdx] = l;}
		// face functions:
		pHalfEdgeListType inner(const pFaceType & f) {return lCld[f->inner];}
		pHalfEdgeListType inner(void) {return lCld[fCld[fIdx]->inner];}
		pHalfEdgeType outer(const pFaceType & f) {return hCld[f->outer];}
		pHalfEdgeType outer(void) {return hCld[fCld[fIdx]->outer];}
		std::vector<pHalfEdgeType> all_outer(const pFaceType & f)
		{
			std::vector<pHalfEdgeType> r;
			pHalfEdgeType h0 = outer(f), h = h0;
			do
			{
				r.push_back(h);
				h = hCld[h->next];
			} while (h != h0);
			return r;
		}
		std::vector<std::vector<pHalfEdgeType>> all_inner(const pFaceType & f)
		{
			std::vector<std::vector<pHalfEdgeType>> r;
			ITYPE li = f->inner;
			while (li != 0)
			{
				pHalfEdgeListType l = lCld[li];
				li = l->nxt;
				std::vector<pHalfEdgeType> rh;
				pHalfEdgeType h0 = hCld[l->helsttop], h = h0;
				do
				{
					rh.push_back(h);
					h = hCld[h->next];
				} while (h != h0);
				r.push_back(rh);
			}
			return r;
		}
		// halfedge functions:
		pTPoint<T> origin(const pHalfEdgeType & h) {return pCld[h->origin];}
		pTPoint<T> origin(void) {return pCld[hCld[hIdx]->origin];}
		pHalfEdgeType twin(const pHalfEdgeType & h) {hIdx = h->twin; return hCld[hIdx];}
		pHalfEdgeType twin(void) {hIdx = hCld[hIdx]->twin; return hCld[hIdx];}
		pFaceType incidentface(const pHalfEdgeType & h) {return fCld[h->incidentface];}
		pFaceType incidentface(void) {return fCld[hCld[hIdx]->incidentface];}
		pHalfEdgeType next(const pHalfEdgeType & h) {hIdx = h->next; return hCld[hIdx];}
		pHalfEdgeType next(void) {hIdx = hCld[hIdx]->next; return hCld[hIdx];}
		pHalfEdgeType prev(const pHalfEdgeType & h) {hIdx = h->prev; return hCld[hIdx];}
		pHalfEdgeType prev(void) {hIdx = hCld[hIdx]->prev; return hCld[hIdx];}
		// point functions:
		pHalfEdgeType incidentedge(const pTPoint<T> & p) {hIdx = p->incidentedge; return hCld[hIdx];}
		pHalfEdgeType incidentedge(void) {hIdx = pCld[pIdx]->incidentedge; return hCld[hIdx];}
		// halfedgelist functions:
		pHalfEdgeType top(const pHalfEdgeListType & l) {hIdx = l->helsttop; return hCld[hIdx];}
		pHalfEdgeType top(void) {return hCld[lCld[lIdx]->helsttop];}
		pHalfEdgeListType nxt(const pHalfEdgeListType & l) {lIdx = l->nxt; return lCld[lIdx];}
		pHalfEdgeListType nxt(void) {lIdx = lCld[lIdx]->nxt; return lCld[lIdx];}
		
		void DebugPrint(void)
		{
			for (std::vector<pFaceType>::iterator fit = fCld.begin(); fit < fCld.end(); ++fit) (*fit)->PrintFace();
			for (typename std::vector<pTPoint<T>>::iterator pit = pCld.begin(); pit < pCld.end(); ++pit) (*pit)->PrintPoint();
			for (std::vector<pHalfEdgeType>::iterator hit = hCld.begin(); hit < hCld.end(); ++hit) (*hit)->PrintHalfEdge();
			for (std::vector<pHalfEdgeListType>::iterator lit = lCld.begin(); lit < lCld.end(); ++lit) (*lit)->PrintHalfEdgeList();

		}
		pFaceType BuildFrame(const pFaceType &pf)
		{ // pf is the exterior face with no outer and inner w/ all the model's faces
			assert (pf->inner != 0);
			pFaceType pfOUT = new FaceType;
			ITYPE inFace = pf->face, outFace = push(pfOUT);
			pfOUT->face = outFace;
			// 1) get all halfs incidental to face pf:
				std::vector<std::vector<pHalfEdgeType>> INNERS = all_inner(pf);
				std::vector<pHalfEdgeType> INCIDENTALS; 
				for (std::vector<std::vector<pHalfEdgeType>>::iterator heit = INNERS.begin(); heit != INNERS.end(); ++heit)
				{
					std::vector<pHalfEdgeType> PERIM = *heit;
					for (std::vector<pHalfEdgeType>::iterator hit = PERIM.begin(); hit != PERIM.end(); ++hit)
					{
						pHalfEdgeType h = *hit;
						if (h->incidentface == inFace) INCIDENTALS.push_back(h);
					}
				}
				
			// 2) get min and max points:
				BasicTPoint<T> pO = origin(INCIDENTALS[0])->xyz, pN = pO, pMAX = pO, pMIN = pO;
				for (std::vector<pHalfEdgeType>::iterator hit = INCIDENTALS.begin(); hit != INCIDENTALS.end(); ++hit)
				{
					pO = origin(*hit)->xyz;
					pN = origin(next(*hit))->xyz;
					for (unsigned char i = 0; i < 2; ++i)
					{
						if (pO.dim(i) > pMAX.dim(i)) pMAX.dim(i) = pO.dim(i);
						if (pO.dim(i) < pMIN.dim(i)) pMIN.dim(i) = pO.dim(i);
						if (pN.dim(i) > pMAX.dim(i)) pMAX.dim(i) = pN.dim(i);
						if (pN.dim(i) < pMIN.dim(i)) pMIN.dim(i) = pN.dim(i);
					}
				}
			// 3) add rectangle points in order LR, UR, UL, LL:
				pMAX.X += static_cast<T>(2);
				pMAX.Y += static_cast<T>(2);
				pMIN.X -= static_cast<T>(2);
				pMIN.Y -= static_cast<T>(2);
				pTPoint<T> CRNS[] = {new TPoint(BasicTPoint<T>(pMAX.X, pMIN.Y, static_cast<T>(0))), 
									 new TPoint(BasicTPoint<T>(pMAX.X, pMAX.Y, static_cast<T>(0))),
									 new TPoint(BasicTPoint<T>(pMIN.X, pMAX.Y, static_cast<T>(0))),
									 new TPoint(BasicTPoint<T>(pMIN.X, pMIN.Y, static_cast<T>(0)))};
				ITYPE iCRNS[4];
				for (size_t i = 0; i < 4; ++i)
				{
					iCRNS[i] = push(CRNS[i]);
					CRNS[i]->vertex = iCRNS[i];
					// to do: incidentedge
				}
			// 4) add halfs [LR, UR, UL, LL][inner counterclockwise, outer clockwise]:
				pHalfEdgeType RECT[][2] = {{new HalfEdgeType, new HalfEdgeType}, 
										  {new HalfEdgeType, new HalfEdgeType}, 
										  {new HalfEdgeType, new HalfEdgeType}, 
										  {new HalfEdgeType, new HalfEdgeType}};
				ITYPE iRECT[4][2];
				for (size_t i = 0; i < 4; ++i)
				{
					for (size_t j = 0; j < 2; ++j)
					{
						iRECT[i][j] = push(RECT[i][j]);
						RECT[i][j]->halfedge = iRECT[i][j];
						RECT[i][j]->origin = iCRNS[i];
					}
					CRNS[i]->incidentedge = iRECT[i][0];
				}
				for (size_t i = 0; i < 4; ++i)
				{ // next, prev, twin
					size_t n = (i + 1) % 4, p = (i + 3) % 4;
					RECT[i][0]->incidentface = inFace;
					RECT[i][0]->next = iRECT[n][0];
					RECT[i][0]->prev = iRECT[p][0];
					RECT[i][0]->twin = iRECT[n][1];
					RECT[i][1]->incidentface = outFace;
					RECT[i][1]->next = iRECT[p][1];
					RECT[i][1]->prev = iRECT[n][1];
					RECT[i][1]->twin = iRECT[p][0];
				}
			// 5) root the lists appropriately:
				pHalfEdgeListType pHEL = new HalfEdgeListType;
				pHEL->helsttop = iRECT[1][1];
				ITYPE iHEL = push(pHEL);
				pHEL->halfedgelist = iHEL;
				pfOUT->inner = iHEL;
				pf->outer = iRECT[0][0];

			return pfOUT;
		}
		/*
		char HalfEdgeCmp(const pHalfEdgeType & h1, const pHalfEdgeType & h2)
		{
			BasicTPoint<T> o1 = origin(h1)->xyz, o2 = origin(h2)->xyz, delta = o2 - o1;
			T zero = static_cast<T>(0);
			bool isV = BEq(zero, delta.X), isH = BEq(zero, delta.Y),
				xNeg = (delta.X < zero), yNeg = (delta.Y < zero);
			return (isV == isH)? 'N': isV? yNeg? 'v': 'V':xNeg? 'h': 'H';
		}

		void getMaxHalfEdge(const pFaceType & pf, pHalfEdgeType & hMax)
		{
			pHalfEdgeType p = top(getHalfEdgeList(pf->inner)), p0 = p;
			hMax = p;
			BasicTPoint<T> maxi = origin()->xyz;
			do
			{
				p = next();
				BasicTPoint<T> cmp = origin()->xyz;
				if ((cmp.X >= maxi.X) && (cmp.Y >= maxi.Y) && (cmp.Z >= maxi.Z))
				{
					hMax = p;
					maxi = cmp;
				}

			} while (p != p0);
			

		}

		pFaceType BuildFrame(const pHalfEdgeType & d_e, const pFaceType & pf)
		{ // adds a frame around faces incident to the twins of the d_e cycle
			// those faces should form a rectangle representing the display/back plane
			// d_e s/b on the right going down from top right point

			pHalfEdgeType o[] = {getHalfEdge(d_e->halfedge), NULL, NULL, NULL}, //outers
				dLo = o[0], dHi = next();
			char transitions[] = "vhVHv", cT = 'v', c = HalfEdgeCmp(dLo, dHi);
			assert (cT == c);
			int rectIdx = 0;
			do
			{
				dLo = dHi;
				dHi = next();
				c = HalfEdgeCmp(dLo, dHi);
				if (c != cT)
				{
					cT = transitions[++rectIdx];
					assert (c == cT);
					if (rectIdx < 4) o[rectIdx] = dLo;
				}
			} while (dLo != o[0]);

			//inners: LR-UR, UR-UL, UL-LL, LL-LR
			pHalfEdgeType i1 = next(twin(o[1])), i2 = next(twin(o[0])), i3 = next(twin(o[3])), i4 = next(twin(o[2])); 
			ITYPE topFace = pf->face;
			assert (0 == pf->outer); // to be changed

			pHalfEdgeType hrbt = new HalfEdgeType(*i1), htrl = new HalfEdgeType(*i2),
				hltb = new HalfEdgeType(*i3), hblr = new HalfEdgeType(*i4),
				hrtb = new HalfEdgeType(*o[0]), hbrl = new HalfEdgeType(*o[1]),
				hlbt = new HalfEdgeType(*o[2]), htlr = new HalfEdgeType(*o[3]);
			hrbt->incidentface = topFace; htrl->incidentface = topFace; hltb->incidentface = topFace; hblr->incidentface = topFace;
			ITYPE irbt = push(hrbt), itrl = push(htrl), iltb = push(hltb), iblr = push(hblr),
				irtb = push(hrtb), ibrl = push(hbrl), ilbt = push(hlbt), itlr = push(htlr);
			hrbt->halfedge = irbt; htrl->halfedge = itrl; hltb->halfedge = iltb; hblr->halfedge = iblr;
			hrtb->halfedge = irtb; hbrl->halfedge = ibrl; hlbt->halfedge = ilbt; htlr->halfedge = itlr;
			hrbt->next = itrl; htrl->next = iltb; hltb->next = iblr; hblr->next = irbt;
			hbrl->next = ilbt; hlbt->next = itlr; htlr->next = irtb; hrtb->next = ibrl;
			hrbt->prev = iblr; hblr->prev = iltb; hltb->prev = itrl; htrl->prev = irbt;
			hbrl->prev = irtb; hlbt->prev = ibrl; htlr->prev = ilbt; hrtb->prev = itlr;
			hrbt->twin = irtb; htrl->twin = itlr; hltb->twin = ilbt; hblr->twin = ibrl;
			hrtb->twin = irbt; hbrl->twin = iblr; hlbt->twin = iltb; htlr->twin = itrl;

			pTPoint<T> oRL = new TPoint<T>(*origin(i1)), oRU = new TPoint<T>(*origin(i2)),
				oLU = new TPoint<T>(*origin(i3)), oLL = new TPoint<T>(*origin(i4));
			T two = static_cast<T>(2), zero = static_cast<T>(0);
			*oRL += TPoint<T>(two, -two, zero); *oRU += TPoint<T>(two, two, zero);
			*oLU += TPoint<T>(-two, two, zero); *oLL += TPoint<T>(-two, -two, zero);
			oRL->incidentedge = irbt; oRU->incidentedge = itrl; oLU->incidentedge = iltb; oLL->incidentedge = iblr;
			ITYPE iRL = push(oRL), iRU = push(oRU), iLU = push(oLU), iLL = push(oLL);
			oRL->vertex = iRL; oRU->vertex = iRU; oLU->vertex = iLU; oLL->vertex = iLL;

			hrbt->origin = iRL; hbrl->origin = iRL;
			htrl->origin = iRU; hrtb->origin = iRU;
			hltb->origin = iLU; htlr->origin = iLU;
			hblr->origin = iLL; hlbt->origin = iLL;
			pHalfEdgeListType pHEL = new HalfEdgeListType;
			pHEL->helsttop = irtb;
			ITYPE iHEL = push(pHEL);
			pHEL->halfedgelist = iHEL;
			pFaceType cf = new FaceType;
			cf->inner = iHEL;
			ITYPE iFace = push(cf);
			cf->face = iFace;
			pf->outer = irbt;
			pHalfEdgeType oT = getHalfEdge(o[0]->halfedge);
			do
			{
				oT->incidentface = iFace;
				oT = next();
			} while (o[0] != oT);
			return cf;
		}
		*/
	private:
		ITYPE fIdx, pIdx, hIdx, lIdx;
		std::vector<pFaceType> fCld; 				// faces
		std::vector<pTPoint<T>> pCld;				// points
		std::vector<pHalfEdgeType> hCld;			// halfedges
		std::vector<pHalfEdgeListType> lCld;		// lists of halfedges
		
	};


	//typedef class Line2D * pLine2D;
	class Line2D
	{ // R = R0 + tL where R is any point on line (x, y), R0 is a specific point on line (x0, y0)
		// L is a vector parallel to the line and t is any real number
	public:
		Line2D(void); // x axis
		Line2D(const Line2D&);
		Line2D(const XY &, const XY &);
		XY GetPoint(double);

		XY L, R0;
	};

	class Line3D
	{
	public:
		Line3D(void); // x axis
		Line3D(const Line3D&);
		Line3D(const XYZ&, const XYZ&);
		XYZ GetPoint(double);

		XYZ L, R0;
	};

	typedef class BoundingBox * pBoundingBox;
	class BoundingBox
	{ // Bounding Boxes are in plane coordinates (x, y) & encapsulate the 4 corner points: LL, LR, UR, UL
	public:
		BoundingBox(void); // unit box around (0, 0)
		BoundingBox(const BoundingBox&);
		BoundingBox(const XY&, const XY&);
		BoundingBox(double, double, double, double);
		bool Intersected(const Line2D&, bool&, bool&);  // true if line intersects box

		double xMin, yMin, xMax, yMax;
	};

	typedef struct WorldStructType
	{
		static XYZ W_ORIGIN;
		const static XYZ W_I_HAT, W_J_HAT, W_K_HAT;
	
	} WorldType;


	typedef struct EdgeStructType * pEdgeType;
	typedef struct EdgeStructType
	{
		XY lo, hi;
	} EdgeType;

	typedef struct XYArrayStructType
	{
		int numElements;
		XYType * xy;
	} XYArrayType;

	typedef struct XYListStructType * pXYListType;
	typedef struct XYListStructType
	{ // circular (ring) topology in ConvexHull
		XY xy;
		pXYListType next, last;
	} XYListType;



	XYZ Cross(const XYZ&, const XYZ&); // 3D cross product
	double Cross(XY, XY); // 2D cross product returns scaler along k vector
	template<typename T> BasicTPoint<T> Cross(const BasicTPoint<T>&a, const BasicTPoint<T>&b)
	{
		T 	xVal = a.Y * b.Z - b.Y * a.Z,
			yVal = b.X * a.Z - a.X * b.Z,
			zVal = a.X * b.Y - b.X * a.Y;
		BasicTPoint<T> rVal(xVal, yVal, zVal);

	return rVal;
	}

	bool IsRightTurn(EdgeType, XY); // path edge towards vertex
	bool IsLeftTurn(EdgeType, XY); // path edge towards vertex
	bool IsInLine(EdgeType, XY); // path edge towards vertex
	bool XYTypesEqual(XYType, XYType);
	double Rounding(double, unsigned int); // rounds double at unsigned int decimal place after
	

	template <class T>
	class Sorter
	{
	public:
		Sorter(void):
			s_array(NULL), tmp_array(NULL), firstIdx(0), lastIdx(0), numElements(0) {}
		Sorter(const Sorter& s): 
			s_array(s.s_array), tmp_array(s.tmp_array), firstIdx(s.firstIdx), lastIdx(s.lastIdx), 
				numElements(s.numElements) {}
		Sorter(T*& Table, int first, int last, int length) :
			s_array(Table), tmp_array(NULL), firstIdx(first), lastIdx(last),
				numElements(length)
		{
			T * r = new T[numElements];
			tmp_array = r;
		}
		~Sorter(void) 
		{
			if (tmp_array != NULL) delete [] tmp_array;
			tmp_array = NULL;
		}
		void AddArray(T*& Table, int first, int last, int length)
		{
	
			s_array = Table;
			firstIdx = first;
			lastIdx = last;
			numElements = length;
			if (tmp_array != NULL) delete [] tmp_array;
			T * r = new T[numElements];
			tmp_array = r;
			
		}
		//void AddLessThanFunctionPointer(bool (*f)(void *, void *));
		//template <class T>void doSort(void)
		void doSort(void)
		{
			bool proceed = s_array != NULL;
			int span = lastIdx - firstIdx + 1;
			proceed = proceed && (span > 1);
			if (proceed) MergeSort(firstIdx, lastIdx);
			//return s_array;

		}
		int GetLength(void)
		{
			return numElements;

		}

	private:
		T *& s_array, * tmp_array;
		int firstIdx, lastIdx, numElements;
		//bool (*Less_Than)(void *, void *);

		//template <class T>void MergeSort(int First, int Last)
		void MergeSort(int First, int Last)
		{
			if (First < Last)
			{
				int Middle = (First + Last) / 2;
				//MergeSort<T>(First, Middle);
				//MergeSort<T>(Middle + 1, Last);
				//Merge<T>(First, Middle, Last);
				MergeSort(First, Middle);
				MergeSort(Middle + 1, Last);
				Merge(First, Middle, Last);
			}

		}

		//template <class T>void Merge(int First, int Middle, int Last)
		void Merge(int First, int Middle, int Last)
		{   // part of recursive Merge Sort s/b called log base 2 N times
			// uses temporary storage buffer tmp_array
			//int num2Mrg = Last - First + 1;;
			int NextLeft = First, NextRight = Middle + 1, Index = 0;
			while ((NextLeft <= Middle) && (NextRight <= Last))
			{
				if (s_array[NextLeft] < s_array[NextRight])
				{
					tmp_array[Index++] = s_array[NextLeft++];
				}
				else
				{
					tmp_array[Index++] = s_array[NextRight++];
				}
			}
			if (NextLeft <= Middle) 
				for (int i = NextLeft; i <= Middle; ++i) tmp_array[Index++] = s_array[i];
			if (NextRight <= Last) 
				for (int i = NextRight; i <= Last; ++i) tmp_array[Index++] = s_array[i];
			for (int i = First; i <= Last; ++i) s_array[i] = tmp_array[i - First];
		}

	};

	template<typename T> struct AVLNode
	{
		T * Data;
		AVLNode * right, * left;
		unsigned int height;

	};
	
	template<typename T> struct AVLPath
	{
		AVLNode<T> * pNode;
		char direction;
	};

	template<typename T>
	class AVL
	{
	public:
		// members:
		AVLNode<T> * pRoot, ** levels;
		AVLPath<T> * sPath;
		int count; // debug check loop check
		unsigned int pathTop;
		//bool checkForDuplicates;
		//char directionB4NULL;

		// methods:
		unsigned int GetMax(unsigned int a, unsigned int b)
		{
			if (a > b) return a;
			return b;
		}
		unsigned int GetMin(unsigned int a, unsigned int b)
		{
			if (a < b) return a;
			return b;
		}
		unsigned int GetSubTreeHeight(AVLNode<T> * st)
		{
			unsigned int hT = 0;
			if (st != NULL) hT = st->height;
			return hT;
		}
		unsigned int GetRealHeight(AVLNode<T> *tNode)
		{
			if (tNode == NULL) return 0;
			return (1 + GetMax(GetRealHeight(tNode->right), GetRealHeight(tNode->left)));
		}
		void nodeToElement(unsigned int level, unsigned int column, AVLNode<T> * node)
		{
			if (node == NULL) return;
			levels[(int)pow(2.0, (int)level) - 1 + column] = node;
			nodeToElement(level + 1, 2 * column, node->left);
			nodeToElement(level + 1, 2 * column + 1, node->right);
		}

		// constructors / destructor:
		AVL(void):pRoot(NULL), sPath(NULL), count(0), pathTop(0){}
		void BuildTree(AVLNode<T> * p)
		{
			if (p == NULL) return;
			AlwaysInsert(p->Data);
			BuildTree(p->left);
			BuildTree(p->right);
		}
		AVL(const AVL & a)
		{
			this->~AVL();
			pathTop = a.pathTop;
			AVLNode<T> * p = a.pRoot;
			BuildTree(p);
			sPath = NULL;
			if (pathTop > 0)
			{
				sPath = new AVLPath<T>[pathTop];
				p = pRoot;
				for (unsigned int i = 0; i < pathTop; ++i)
				{
					char d = a.sPath[i].direction;
					sPath[i].direction = d;
					sPath[i].pNode = p;
					if (d == 'l') p = p->left;
					else p = p->right;
				}
			}
			count = a.count;
			//checkForDuplicates = a.checkForDuplicates;
		}
		void DeleteSubTree(AVLNode<T> *& sTree)
		{
			if (sTree == NULL) return;
			DeleteSubTree(sTree->right);
			DeleteSubTree(sTree->left);
			delete sTree->Data;
			delete sTree;
			sTree = NULL;
			++count;  // check on debug
		}
		~AVL(void)
		{
			DeleteSubTree(pRoot);
			DeletePath();
			count = 0;
		}
		void DeletePath(void)
		{
			if (sPath != NULL) delete [] sPath;
			sPath = NULL;
			pathTop = 0;
		}
		void setHeight(AVLNode<T> * x) 
		{
			if (x == NULL) return;
			x->height = 1 + GetMax(GetSubTreeHeight(x->left), GetSubTreeHeight(x->right));
		}
		AVLNode<T> * rotateRight(AVLNode<T> * y)
		{  // prerequisites: y!=NULL && y->left!=NULL
			AVLNode<T> * x = y->left;
			y->left = x->right;
			x->right = y;
			setHeight(y);
			setHeight(x);
			return x;
		}
		AVLNode<T> * rotateLeft(AVLNode<T> * x)
		{  // prerequisites: x!=NULL && x->right!=NULL
			AVLNode<T> * y = x->right;
			x->right = y->left;
			y->left = x;
			setHeight(x);
			setHeight(y);
			return y;
		}
		AVLNode<T> * Find(T * a)
		{
			DeletePath();
			//directionB4NULL = ' ';
			if (pRoot == NULL) return NULL;
			AVLNode<T> * p = pRoot->right; //pRoot is buffer, tree is on right
			pathTop = pRoot->height + 1;
			//hCheck = pRoot->height - GetRealHeight(pRoot);
			sPath = new AVLPath<T>[pathTop];
			AVLPath<T> * pth = &sPath[0];
			pth->pNode = pRoot;
			pth->direction = 'r';
			unsigned int i;
			for(i = 1; i < pathTop; ++i)
			{
				pth = &sPath[i];
				pth->pNode = NULL;
				pth->direction = ' ';
			}
			pathTop = 0;
			while (p != NULL)
			{
				pth = &sPath[++pathTop];
				pth->pNode = p;
				if (*a == *p->Data) return p;
				pth->direction = 'r';
				if (*a < *p->Data) 
				{
					p = p->left;
					pth->direction = 'l';
				}
				else p = p->right;
				//if (p != NULL) ++pathTop;
			}
			return p;
		}

		void FindLeaf(T * a)
		{
			DeletePath();
			if (pRoot == NULL) return;
			AVLNode<T> * p = pRoot->right; //pRoot is buffer, tree is on right
			pathTop = pRoot->height + 1;
			sPath = new AVLPath<T>[pathTop];
			AVLPath<T> * pth = &sPath[0];
			pth->pNode = pRoot;
			pth->direction = 'r';
			unsigned int i;
			for(i = 1; i < pathTop; ++i)
			{
				pth = &sPath[i];
				pth->pNode = NULL;
				pth->direction = ' ';
			}
			pathTop = 0;
			while (p != NULL)
			{
				pth = &sPath[++pathTop];
				pth->pNode = p;
				pth->direction = 'r';
				if (*a < *p->Data) 
				{
					p = p->left;
					pth->direction = 'l';
				}
				else p = p->right;
			}
		}

		AVLNode<T> * Rotate(unsigned int idx)
		{
				// uses sPath to perform proper rotation at index idx
			AVLPath<T> * z = &sPath[idx], * y = &sPath[idx + 1];
			//assert(y != NULL);
			//assert(y->direction != ' ');
			if (z->direction == y->direction)
			{
				if (z->direction == 'l') return rotateRight(z->pNode); // Left Left
				else return rotateLeft(z->pNode); // Right Right
			}
			if (z->direction == 'l') // Left Right
			{
				z->pNode->left = rotateLeft(y->pNode);
				return rotateRight(z->pNode);
			}
			// Right Left
			z->pNode->right = rotateRight(y->pNode);
			return rotateLeft(z->pNode);

		}

		void AlwaysInsert(T * a)
		{
			unsigned int top;
			AVLNode<T> * p = pRoot;
			AVLPath<T> * pth = NULL;
			if (p == NULL) // setting up buffer node
			{
				p = (AVLNode<T> *)::operator new(sizeof(AVLNode<T>));
				p->Data = NULL;
				p->height = 1;
				p->left = NULL;
				p->right = NULL;
				pRoot = p;
			}
			FindLeaf(a);
			// sPath leads to leaf entry point:
			pth = &sPath[0];
			p = (AVLNode<T> *)::operator new(sizeof(AVLNode<T>));
			p->Data = a;
			p->height = 1;
			p->left = NULL;
			p->right = NULL;
			// tree is always >1 node:
			top = pathTop;
			pth = &sPath[top];
			if (pth->direction == 'r')
			{
				pth->pNode->right = p;
			}
			else // left
			{
				pth->pNode->left = p;	
			}
			sPath[++pathTop].pNode = p;
			for (top = pathTop - 1; top > 0; --top)
			{// exit conditions: insert balances a subtree, hit root, imbalance rotated
				pth = &sPath[top];
				unsigned int h = pth->pNode->height;
				setHeight(pth->pNode);
				if (pth->pNode->height == h) return; // subtree is balanced
				int bFact = GetSubTreeHeight(pth->pNode->right) - GetSubTreeHeight(pth->pNode->left);
				if ((bFact < -1) || (bFact > 1)) // imbalance detected
				{
					AVLPath<T> * lst = &sPath[top - 1];
					if (lst->direction == 'r') lst->pNode->right = Rotate(top);
					else lst->pNode->left = Rotate(top);
					return;
				}
			}
			setHeight(pRoot);
		}

		bool Insert(T *& a) 
			// returns true iff T is inserted i.e. not in tree before call
			// do not call Find before as this will duplicate call
		{
			unsigned int top;
			//char lst2[2] = "  ";
			AVLNode<T> * p = pRoot;
			AVLPath<T> * pth = NULL;
			if (p == NULL) // setting up buffer node
			{
				p = (AVLNode<T> *)::operator new(sizeof(AVLNode<T>));
				//T * tBuff = NULL;
				p->Data = NULL;
				p->height = 1;
				p->left = NULL;
				p->right = NULL;
				pRoot = p;
			}
			p = Find(a);
			// Found It!
			if(p != NULL) 
			{
				a = p->Data;
				return false; 
			}
			// sPath leads to leaf entry point:
			pth = &sPath[0];
			//assert(pth->pNode == pRoot);
			// Did not find, proceed w/ insert
			p = (AVLNode<T> *)::operator new(sizeof(AVLNode<T>));
			p->Data = a;
			p->height = 1;
			p->left = NULL;
			p->right = NULL;
			// tree is always >1 node:
			top = pathTop;
			pth = &sPath[top];
			if (pth->direction == 'r')
			{
				pth->pNode->right = p;
			}
			else // left
			{
				pth->pNode->left = p;	
			}
			sPath[++pathTop].pNode = p;
			//unsigned int hExp = 2;
			//top = pathTop - 1;
			//do
			for (top = pathTop - 1; top > 0; --top)
			{// exit conditions: insert balances a subtree, hit root, imbalance rotated
				pth = &sPath[top];
				unsigned int h = pth->pNode->height;
				setHeight(pth->pNode);
				if (pth->pNode->height == h) return true; // subtree is balanced
				int bFact = GetSubTreeHeight(pth->pNode->right) - GetSubTreeHeight(pth->pNode->left);
				if ((bFact < -1) || (bFact > 1)) // imbalance detected
				{
					AVLPath<T> * lst = &sPath[top - 1];
					//assert(lst != NULL);
					//assert(lst->direction != ' ');
					if (lst->direction == 'r') lst->pNode->right = Rotate(top);
					else lst->pNode->left = Rotate(top);
					//for (unsigned int j = top - 1; j > 0; --j) setHeight(sPath[j].pNode);
					//setHeight(pRoot);
					return true;
				}
			}
			setHeight(pRoot);
			return true;
		}
		
		void Delete(void) // deletes node at sPath[pathTop]
		{
			unsigned int top = 0;
			AVLPath<T> * pth = &sPath[pathTop], * qth = NULL;
			AVLNode<T> * p = pth->pNode, * q = NULL;
			if (p == NULL) return;  // not found
			// standard BST delete:
			if ((p->left == NULL) && (p->right == NULL))
			{
				top = pathTop - 1;
				pth = &sPath[top];
				if (pth->pNode->right == p) 
				{
					pth->pNode->right = NULL;
				}
				else pth->pNode->left = NULL;
				delete p->Data;
				delete p;
			}
			else
			{
				if (p->left == NULL) // 1 child on right
				{
					top = pathTop - 1;
					pth = &sPath[top];
					if (pth->pNode->right == p) 
					{
						pth->pNode->right = p->right;
					}
					else pth->pNode->left = p->right;
					delete p->Data;
					delete p;
				}
				else
				{
					if (p->right == NULL) // 1 child on left
					{
						top = pathTop - 1;
						pth = &sPath[top];
						if (pth->pNode->right == p) 
						{
							pth->pNode->right = p->left;
						}
						else pth->pNode->left = p->left;
						delete p->Data;
						delete p;
					}
					else // has both children
					{
						q = p->right;
						pth->direction = 'r';
						qth = &sPath[++pathTop];
						qth->pNode = q;
						
						do
						{
							q = q->left;
							if (q != NULL)
							{
								qth->direction = 'l';
								qth = &sPath[++pathTop];
								qth->pNode = q;
							}
						} while (q != NULL);

						q = qth->pNode;  // q is least of nodes greater than p
						*p->Data = *q->Data;
						top = pathTop - 1; // pathTop > 1
						qth = &sPath[top];
						if (q == p->right) 
						{
							p->right = q->right;
						}
						else qth->pNode->left = q->right;
						delete q->Data;
						delete q;
					}
				}
			} //finish of standard BST delete
			// now to balance from top up:
			// exit conditions: processed pRoot, new height = old height
			for (top = pathTop - 1; top > 0; --top)
			{
				pth = &sPath[top];
				AVLNode<T> * z = pth->pNode;//, * y = NULL, * x = NULL;
				unsigned int h = z->height;
				setHeight(z);
				//if (h == z->height) return;
				unsigned int rh = GetSubTreeHeight(z->right), lh = GetSubTreeHeight(z->left);
				int balFact = rh - lh;
				if ((balFact < -1) || (balFact > 1))
				{
					AVLPath<T> *lst = &sPath[top - 1];
					//assert(lst != NULL);
					if (rh > lh)
					{
						pth->direction = 'r';
						pth = &sPath[top + 1];
						pth->pNode = z->right;
					}
					else
					{
						pth->direction = 'l';
						pth = & sPath[top + 1];
						pth->pNode = z->left;
					}
					AVLNode<T> * nxt = pth->pNode;
					rh = GetSubTreeHeight(nxt->right), lh = GetSubTreeHeight(nxt->left);
					if (rh > lh) pth->direction = 'r';
					else pth->direction = 'l';
					if (lst->direction == 'r') lst->pNode->right = Rotate(top);
					else lst->pNode->left = Rotate(top);
				}
			} 
			setHeight(pRoot);
		}

		bool AVLEmpty(void)
		{
			if (pRoot == NULL) return true;
			if (pRoot->right == NULL) return true;
			return false;
		}

		T GetLeast(bool & IsEmpty)
		{
			DeletePath();
			T rVal;
			IsEmpty = true;
			if (pRoot == NULL) return rVal;
			AVLNode<T> * p = pRoot->right; //pRoot is buffer, tree is on right
			if (p == NULL) return rVal;
			IsEmpty = false;
			pathTop = pRoot->height + 1;
			sPath = new AVLPath<T>[pathTop];
			AVLPath<T> * pth = &sPath[0];
			pth->pNode = pRoot;
			pth->direction = 'r';
			unsigned int i;
			for(i = 1; i < pathTop; ++i)
			{
				pth = &sPath[i];
				pth->pNode = NULL;
				pth->direction = ' ';
			}
			pathTop = 1;
			while (p != NULL)
			{
				pth = &sPath[pathTop];
				pth->pNode = p;
				pth->direction = 'l';
				p = p->left;
				if (p != NULL) ++pathTop;
			}
			pth = &sPath[pathTop];
			p = pth->pNode;
			rVal = *p->Data;
			Delete();
			return rVal;
		}
		void DebugPrintPath(unsigned int vCount)
		{
			
			//char txtBuff[256];
			//HRESULT b_p = StringCbPrintf(txtBuff, 256 * sizeof(char),TEXT("Event Queue path for point %u\n"),
			//	vCount);
			std::cerr << "Event Queue path for point " + std::to_string(vCount) + "\n";

			//OutputDebugString(txtBuff);
			for(unsigned int i = 0; i < pathTop; ++i)
			{
				//b_p = StringCbPrintf(txtBuff, 256 * sizeof(char),TEXT("%u) h:%u, d:%c "),
				//	i, sPath[i].pNode->height, sPath[i].direction);
				//OutputDebugString(txtBuff);
				std::cerr << std::to_string(i) << ") h:" +
					std::to_string(sPath[i].pNode->height) +
					", d:" + sPath[i].direction;
			}
			//b_p = StringCbPrintf(txtBuff, 256 * sizeof(char),TEXT("\n"));
			//OutputDebugString(txtBuff);
			std::cerr << "\n";
			
		}
		void PathToStr(char *& strBuff, int BuffSize, int * calcLength)
		{
			int i = 0;
			for(i = 0; i < BuffSize; ++i) strBuff[i] == L'\0';
			*calcLength = 0;
			if (BuffSize < (pathTop + 1)) return;
			for(i = 0; i < (int)pathTop; ++i)
			{
				strBuff[i] = (char)((int)sPath[i].direction);
			}
			strBuff[i] = L'\0';
			*calcLength = i;
		}

		int hCheck;

		void TreeWrite(const std::string Desc, const std::string fN)
		{ // just heights for now:
			if (pRoot == NULL) return;
			int h = GetRealHeight(pRoot) - 1, n = h - 1;
			if (h > 13)
			{
				std::cout << "Tree Too Large For Writing, Error in TreeWrite \n";
				//MessageBox("Tree Too Large For Writing", "Error in TreeWrite");
				return;
			}

			int TwoPow[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384,
			32768, 65536}; //, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216,
			//33554432, 67108864, 134217728, 268435456, 536870912, 1073741824, 2147483648, 4294967296}

			double dtCount = TwoPow[h] - 1;
			int tCount = (int)dtCount;
			levels = new AVLNode<T> *[tCount];
			for (int i = 0; i < tCount; ++i) levels[i] = NULL;
			nodeToElement(0, 0, pRoot->right);

			int w1 = 4, // ' #### ' to output the height
				w2 = TwoPow[n] * w1, // line length
				w2_ = w2 + 1;
			char * line = new char[w2_], * ptrs = new char[w2_], * blanks = new char[w2_];
			char sChars[] = {" /_\\\0"};
			char blank = sChars[0], fslash = sChars[1], u_s = sChars[2], bslash = sChars[3], nchr = sChars[4];
			for (int i = 0; i < w2; ++i) blanks[i] = blank;
			blanks[w2] = nchr;
			//DWORD  dwBytesToWrite, dwBytesWritten, dwPos;
			//char * buff = new char[w2 + 10];
			
			/*
			HANDLE hf = CreateFile(fN, 
						FILE_APPEND_DATA,         // open for writing
						0,						  // do not share
						NULL,                     // no security
						OPEN_ALWAYS,              // open or create
						FILE_ATTRIBUTE_NORMAL,    // normal file
						NULL);					  // no attr. template
			*/
			std::ofstream hf;
			hf.open(fN);

			std::string txtBuff, buff;
			//char fslash = (char)'/', bslash = (char)'\\', dash = (char)'-';
			//size_t l = Desc.length();
			//HRESULT b_p = StringCbLength(Desc, 256 * sizeof(char), &l);
			//dwBytesToWrite = l;
			//dwPos = SetFilePointer(hf, 0, NULL, FILE_END);
			//WriteFile(hf, Desc, dwBytesToWrite, &dwBytesWritten, NULL);
			hf << Desc;

			//b_p = StringCbPrintf(txtBuff, 256 * sizeof(char),
			//	TEXT("Tree is at root->right. Root is a buffer node with root->height = %u\r\n"), pRoot->height);
			//b_p = StringCbLength(txtBuff, 256 * sizeof(char), &l);
			hf << "Tree is at root->right. Root is a buffer node with root->height = " +
				std::to_string(pRoot->height) + "\r\n";
			//dwBytesToWrite = l;
			//WriteFile(hf, txtBuff, dwBytesToWrite, &dwBytesWritten, NULL);

			for (int i = 0; i < h; ++i)
			{
				int w3 = w2 / TwoPow[i], d2 = w3 / 2, d1 = d2 - w1 / 2,
					wL = d1 - w2 / TwoPow[i + 2], wR = 3 * w2 / TwoPow[i + 2] - d1 - w1;
				if (wL < 0) wL = 0;
				if (wR < 0) wR = 0;
				memcpy(line, blanks, w2_ * sizeof(char));
				memcpy(ptrs, blanks, w2_ * sizeof(char));
				for (int j = 0; j < (int)pow(2.0, i); ++j)
				{
					AVLNode<T> * p = levels[TwoPow[i] - 1 + j], * q = NULL;
					if (p != NULL)
					{
						//b_p = StringCbPrintf(txtBuff, 256 * sizeof(char),TEXT("(%02u)"), p->height);
						
						txtBuff = "(";
						txtBuff += std::setw(2) +  std::to_string(p->height) +
							std::setw(0);
						txtBuff += ")";
						memcpy(line + j * w3 + d1, txtBuff.c_str(), w1 * sizeof(char));
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
				//dwBytesToWrite = l;
				//WriteFile(hf, buff, dwBytesToWrite, &dwBytesWritten, NULL);
				buff = (std::string)line;
				buff += "\r\n";
				hf << buff;

				if (i < n)
				{
					//b_p = StringCbPrintf(buff, (w2 + 10) * sizeof(char), TEXT("%s\r\n"), ptrs);
					//b_p = StringCbLength(buff, (w2 + 10) * sizeof(char), &l);
					//dwBytesToWrite = l;
					//WriteFile(hf, buff, dwBytesToWrite, &dwBytesWritten, NULL);
					buff = ptrs;
					buff += "\r\n";
					hf << buff;
				}
			}
			//b_p = StringCbPrintf(buff, (w2 + 10) * sizeof(char), TEXT("%s\r\n"), blanks);
			//b_p = StringCbLength(buff, (w2 + 10) * sizeof(char), &l);
			//dwBytesToWrite = l;
			//WriteFile(hf, buff, dwBytesToWrite, &dwBytesWritten, NULL);
			//WriteFile(hf, buff, dwBytesToWrite, &dwBytesWritten, NULL);
			//CloseHandle(hf);
			buff = (std::string)blanks; buff += "\r\n";
			buff = (std::string)blanks; buff += "\r\n";
			hf << buff;
			hf.close();
			delete [] levels;
			delete [] line;
			delete [] ptrs;
			delete [] blanks;
			//delete [] buff;
		}

	
		
	};
	
	
	// forward declarations:
	template<typename T> class BinaryBPlusLeaf;
	template<typename T> class BinaryBPlusNode;

	template<typename T> class BinaryBPlusPointer
	{
	public:
		BinaryBPlusPointer(void): pLeaf(NULL), pNode(NULL) {}

		// members: one only is to be non-NULL
		BinaryBPlusLeaf<T> * pLeaf;
		BinaryBPlusNode<T> * pNode;

	};

	template<typename T> class BinaryBPlusNode
	{
	public:
		BinaryBPlusNode(void): height(0), key(NULL), left(NULL), right(NULL) {}
		/*
		BinaryBPlusNode(const BinaryBPlusNode & a)
		{
			key = a.key;
			left = a.left;
			right = a.right;
			height = a.height;
		}
		*/

		int height;
		BinaryBPlusLeaf<T> * key;
		BinaryBPlusPointer<T> * left, *right;
		
	};

	template<typename T> class BinaryBPlusLeaf
	{
	public:
		BinaryBPlusLeaf(void): leafType(' '), order(0.0), next(NULL), prev(NULL), Data(NULL) {}
		~BinaryBPlusLeaf(void) { if (Data != NULL) delete Data; }
		bool IsDataValueLess(T * DataIn, bool & IsEqual)
		{
			IsEqual = false;
			switch (leafType)
			{
			case 'N':
				IsEqual = (*DataIn == *Data);
				return *DataIn < *Data;
				break;
			case 'B':
				return false;
				break;
			case 'A':
				return true;
				break;
			}
			assert(1 == 2);
		}

		char leafType; // 'B' = before (-∞), 'A' = after (+∞), 'N' = normal;
		double order; // number between -2^20 and +2^20 to get path for known leaf
		BinaryBPlusLeaf<T> * next, * prev;
		T * Data;
	};

	template<typename T> class BinaryBPlusPath
	{
	public:
		BinaryBPlusPath(void): direction(' '), node(NULL), next(NULL), prev(NULL) {}

		char direction;  // 'L' for left, 'R' for right
		BinaryBPlusNode<T> * node;
		BinaryBPlusPath<T> * next, * prev;

	};

	template<typename T> class BinaryBPlusPathContainer
	{
	public:

		BinaryBPlusPathContainer(void) : pathTop(NULL), pathBottom(NULL) {}
		~BinaryBPlusPathContainer(void) 
		{
			ClearPath();
		}
		void ClearPath(void)
		{
			BinaryBPlusPath<T> * p = NULL;
			while (pathTop != pathBottom)
			{
				p = pathTop->next;
				delete pathTop;
				pathTop = p;
			}
			if (pathTop != NULL)
			{
				delete pathTop;
				pathTop = NULL;
				pathBottom = NULL;
			}

		}
		void AddToPath(BinaryBPlusNode<T> * _node, char _direction)
		{
			BinaryBPlusPath<T> * p = new BinaryBPlusPath<T>;
			p->direction = _direction;
			p->node = _node;
			p->prev = pathBottom;
			pathBottom = p;
			if (pathTop == NULL) pathTop = p;
		}

		void RemoveFromPath(BinaryBPlusPath<T> * p_r)
		{
			assert(p_r != NULL);
			assert(p_r != pathTop);
			BinaryBPlusPath<T> * p_nxt = p_r->next, * p_prev = p_r->prev;
			if (p_r == pathBottom)
			{
				delete p_r;
				pathBottom = p_prev;
				p_r = p_prev;
				char ckDir = p_r->direction;
				BinaryBPlusNode<T> * ckNd = ckDir == 'L' ? p_r->node->left->pNode : p_r->node->right->pNode;
				if (ckNd == NULL) return;
				int hL = GetHeight(ckNd->left->pNode), hR = GetHeight(ckNd->right->pNode);
				char nxtDir = hL > hR ? 'L' : 'R';
				AddToPath(ckNd, nxtDir);
				return;
			}
			p_prev->next = p_nxt;
			p_nxt->prev = p_prev;
			delete p_r;
			if (p_nxt != pathBottom)
			{
				BinaryBPlusNode<T> * nd1 = p_nxt->node, * nd2 = p_nxt->next->node;
				p_nxt->direction = nd1->left->pNode == nd2 ? 'L' : 'R';
			}
		}

		void TruncatePath(BinaryBPlusPath<T> * p)
		{
			assert(p != NULL);
			pathBottom = p;
			p = p->next;
			if (pathBottom == NULL) pathTop = NULL;
			else pathBottom->next = NULL;
			BinaryBPlusPath<T> * pNxt = NULL;
			while (p != NULL)
			{
				pNxt = p->next;
				delete p;
				p = pNxt;
			}
		}

		int GetMax(int a, int b) { return a > b ? a : b; }
		
		int GetHeight(BinaryBPlusNode<T> * _node)
		{
			if (_node == NULL) return 0;
			return _node->height;
		}

		void SetHeight(BinaryBPlusNode<T> * x)
		{
			if (x == NULL) return;
			x->height = 1 + GetMax(GetHeight(x->left->pNode), GetHeight(x->right->pNode));

		}

		void SetNode(BinaryBPlusPointer<T> *& p, BinaryBPlusNode<T> * n, BinaryBPlusLeaf<T> * l)
		{
			assert(((n == NULL) || (l == NULL)) && !((n == NULL) && (l == NULL)));
			p->pLeaf = l;
			p->pNode = n;
		}

		BinaryBPlusNode<T> * rotateLeft(BinaryBPlusNode<T> * x)
		{// prerequisites: x!=NULL && x->right!=NULL
			assert((x != NULL) && (x->right->pNode != NULL)); // so x->right->leaf is NULL
			BinaryBPlusNode<T> * y = x->right->pNode;
			SetNode(x->right, y->left->pNode, y->left->pLeaf);
			SetNode(y->left, x, NULL);
			SetHeight(x);
			SetHeight(y);
			return y;
		}


		BinaryBPlusNode<T> * rotateRight(BinaryBPlusNode<T> * y)
		{// prerequisites: y!=NULL && y->left!=NULL
			assert((y != NULL) && (y->left->pNode != NULL));
			BinaryBPlusNode<T> * x = y->left->pNode;
			SetNode(y->left, x->right->pNode, x->right->pLeaf);
			SetNode(x->right, y, NULL);
			SetHeight(y);
			SetHeight(x);
			return x;
		}
		
		BinaryBPlusNode<T> * Rotate(BinaryBPlusPath<T> * z) // rotates at parameter
		{
			BinaryBPlusPath<T> * y = z->next, * w = z->prev, * x = y->next;
			BinaryBPlusNode<T> * r = NULL, * zNde = z->node, * yNde = y->node, * xNde = NULL;
			if (x == NULL)
			{
				assert(yNde->height > 1);
				int lH = GetHeight(yNde->left->pNode), rH = GetHeight(yNde->right->pNode);
				xNde = lH > rH ? yNde->left->pNode : yNde->right->pNode;
				int nlH = GetHeight(xNde->left->pNode), nrH = GetHeight(xNde->right->pNode);
				AddToPath(xNde, nlH > nrH ? 'L' : 'R');
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
				z->node->left->pNode = rotateLeft(y->node);
				r = rotateRight(z->node);
			}
			else // Right Left
			{
				z->node->right->pNode = rotateRight(y->node);
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

		BinaryBPlusPath<T> * pathTop, *pathBottom;



	};

	template<typename T>  class BinaryBPlus
	{
	public:
		BinaryBPlus(void)
		{
			beforeLeaf = new BinaryBPlusLeaf<T>;
			beforeLeaf->leafType = 'B';
			beforeLeaf->order = BPLUS_LO_LEAF;
			afterLeaf = new BinaryBPlusLeaf<T>;
			afterLeaf->leafType = 'A';
			afterLeaf->order = BPLUS_HI_LEAF;
			afterLeaf->prev = beforeLeaf;
			beforeLeaf->next = afterLeaf;
			root = new BinaryBPlusNode<T>;
			root->height = 1; // nodes not leaves
			root->key = beforeLeaf;
			root->left = new BinaryBPlusPointer<T>;
			root->right = new BinaryBPlusPointer<T>;
			root->left->pLeaf = beforeLeaf;
			root->right->pLeaf = afterLeaf;
			PC = new BinaryBPlusPathContainer<T>;
			leaf_count = 0;

		}

		~BinaryBPlus(void)
		{
			DeleteTree(root);
			BinaryBPlusLeaf<T> * lol = beforeLeaf->next;
			while (lol != NULL)
			{
				beforeLeaf->next = lol->next;
				delete lol;
				lol = beforeLeaf->next;
			}
			delete beforeLeaf;
			delete PC;
		}

		void DeleteTree(BinaryBPlusNode<T> *& st)
		{
			if (st == NULL) return;
			if ((st->left->pNode == NULL) && (st->right->pNode == NULL))
			{
				delete st->left;
				delete st->right;
				delete st;
			}
			else
			{
				if (st->left->pNode == NULL)
				{
					delete st->left;
					DeleteTree(st->right->pNode);
					delete st->right;
					delete st;
				}
				else
				{
					if (st->right->pNode == NULL)
					{
						delete st->right;
						DeleteTree(st->left->pNode);
						delete st->left;
						delete st;
					}
					else
					{
						DeleteTree(st->left->pNode);
						delete st->left;
						DeleteTree(st->right->pNode);
						delete st->right;
						delete st;
					}
				}
			}
		}

		void SetPath(BinaryBPlusLeaf<T> * knownLeaf)
		{
			PC->ClearPath();
			BinaryBPlusNode<T> * p = root;
			double o = knownLeaf->order;
			char d = ' ';
			while (p != NULL)
			{
				if (o == p->key->order)
				{
					d = 'L';
					PC->AddToPath(p, d);
					p = p->left->pNode;
					d = 'R';
					while (p != NULL)
					{
						PC->AddToPath(p, d);
						p = p->right->pNode;
					}
					return;
				}
				d = o < p->key->order ? 'L' : 'R';
				PC->AddToPath(p, d);
				if (d == 'L') p = p->left->pNode;
				else p = p->right->pNode;
			}
			// error: leaf not found
			assert(1 == 2);
		}

		BinaryBPlusLeaf<T> * FindLeaf(T * DataIn, const bool GetFullPath)
		{
			BinaryBPlusNode<T> * p = root, * prev = NULL;
			BinaryBPlusLeaf<T> * l = NULL;
			bool EQ = false;
			char d = 'R';
			PC->ClearPath();
			while (p != NULL)
			{
				l = p->key;
				d = l->IsDataValueLess(DataIn, EQ) ? 'L' : 'R';
				if (EQ)
				{
					if (GetFullPath)
					{
						d = 'L';
						PC->AddToPath(p, d);
						p = p->left->pNode;
						d = 'R';
						while (p != NULL)
						{
							PC->AddToPath(p, d);
							p = p->right->pNode;
						}
					}
					return l;
				}
				PC->AddToPath(p, d);
				prev = p;
				if (d == 'L') p = p->left->pNode;
				else p = p->right->pNode;
			}
			return NULL;
		}

		void InsertDistinct(T *& DataIn)
		{ // only inserts distinct leafs
			BinaryBPlusLeaf<T> * LF = FindLeaf(DataIn, false), * LF_N = NULL, * LF_P = NULL;
			if (LF != NULL)
			{
				DataIn = LF->Data; // calling procedure needs to watch out for this
				return;
			}
			BinaryBPlusPath<T> * p = PC->pathBottom;
			char d = p->direction;
			BinaryBPlusNode<T> * _N = p->node;
			BinaryBPlusPointer<T> * BPPtr = d == 'L' ? _N->left : _N->right;
			LF_N = BPPtr->pLeaf; //d == 'L' ? NOld->left->pLeaf : NOld->right->pLeaf;
			BPPtr->pLeaf = NULL;
			BinaryBPlusNode<T> * N = new BinaryBPlusNode<T>;
			BPPtr->pNode = N;
			LF_P = LF_N->prev;
			LF = new BinaryBPlusLeaf<T>;
			LF->Data = DataIn;
			LF->leafType = 'N';
			LF->next = LF_N;
			LF->prev = LF_P;
			LF->order = (LF_N->order + LF_P->order) / 2.0;  // midpoint calculation
			LF_P->next = LF;
			LF_N->prev = LF;
			N->height = 1;
			N->key = LF;
			N->left = new BinaryBPlusPointer<T>;
			N->right = new BinaryBPlusPointer<T>;
			N->left->pLeaf = LF;
			N->right->pLeaf = LF_N;
			++leaf_count;
			// Balance if necessary:
			while (_N != root)
			{
				int o_h = _N->height;
				PC->SetHeight(_N);
				if (o_h == _N->height) break;
				int bFact = PC->GetHeight(_N->right->pNode) - PC->GetHeight(_N->left->pNode);
				if (abs(bFact) > 1)
				{
					BinaryBPlusPath<T> * p_prev = p->prev;
					char d_prev = p_prev->direction;
					BinaryBPlusPointer<T> * ptr_prev = d_prev == 'L' ? p_prev->node->left : p_prev->node->right;
					ptr_prev->pNode = PC->Rotate(p);
					break;
				}
				p = p->prev;
				_N = p->node;
			}
			PC->SetHeight(root);
		}

		/*
		void InsertAlways(T *& DataIn)
		{ // tree could have duplicate leafs
			BinaryBPlusLeaf<T> * LF = FindLeaf(DataIn, true), *LF_N = NULL, *LF_P = NULL;
			BinaryBPlusPath<T> * p = PC->pathBottom;
			char d = p->direction;
			BinaryBPlusNode<T> * _N = p->node;
			BinaryBPlusPointer<T> * BPPtr = d == 'L' ? _N->left : _N->right;
			LF_N = BPPtr->pLeaf; //d == 'L' ? NOld->left->pLeaf : NOld->right->pLeaf;
			BPPtr->pLeaf = NULL;
			BinaryBPlusNode<T> * N = new BinaryBPlusNode<T>;
			BPPtr->pNode = N;
			LF_P = LF_N->prev;
			LF = new BinaryBPlusLeaf<T>;
			LF->Data = DataIn;
			LF->leafType = 'N';
			LF->next = LF_N;
			LF->prev = LF_P;
			LF->order = (LF_N->order + LF_P->order) / 2.0;  // midpoint calculation
			LF_P->next = LF;
			LF_N->prev = LF;
			N->height = 1;
			N->key = LF;
			N->left = new BinaryBPlusPointer<T>;
			N->right = new BinaryBPlusPointer<T>;
			N->left->pLeaf = LF;
			N->right->pLeaf = LF_N;
			++leaf_count;
			// Balance if necessary:
			while (_N != root)
			{
				int o_h = _N->height;
				PC->SetHeight(_N);
				if (o_h == _N->height) break;
				int bFact = PC->GetHeight(_N->right->pNode) - PC->GetHeight(_N->left->pNode);
				if (abs(bFact) > 1)
				{
					BinaryBPlusPath<T> * p_prev = p->prev;
					char d_prev = p_prev->direction;
					BinaryBPlusPointer<T> * ptr_prev = d_prev == 'L' ? p_prev->node->left : p_prev->node->right;
					ptr_prev->pNode = PC->Rotate(p);
					break;
				}
				p = p->prev;
				_N = p->node;
			}
			PC->SetHeight(root);

		}
		*/

	//private:
		int _DeleteLeaf(BinaryBPlusLeaf<T> *& LF)
		{ // don't call this directly, call DeleteByValue or DeleteByLeaf instead
		  // it you call this directly the full path must be set for leaf LF
		  // so LF must be of the form FindLeaf(DataIn, true)

			if (LF == NULL) return -1;  // error: not found

			BinaryBPlusLeaf<T> * LF_N = LF->next, * LF_P = LF->prev, * kLeaf = NULL;
			
			// fixing leaf line:
			--leaf_count;
			LF_P->next = LF_N;
			LF_N->prev = LF_P;

			// fixing tree:
			BinaryBPlusPath<T> * pb = PC->pathBottom, * pi = pb->prev, * p = pi, * pp = NULL;
			char dd = pb->direction, di = pi->direction;
			BinaryBPlusNode<T> * _N = pb->node, * N = pi->node, * NNxt = NULL;
			kLeaf = _N->key;
			BinaryBPlusPointer<T> * ptrd = dd == 'L' ? _N->left : _N->right,
				* ptrs = dd == 'L'? _N->right: _N->left,
				* ptri = di == 'L'? N->left: N->right,
				* ptrpp = NULL;
			ptri->pLeaf = ptrs->pLeaf;
			ptri->pNode = ptrs->pNode;
			// rebalance if necessary
			PC->RemoveFromPath(pb);
			while (N != root)
			{
				pp = p->prev;
				char dpp = pp->direction;
				ptrpp = dpp == 'L' ? pp->node->left : pp->node->right;
				if (N->key = LF) N->key = kLeaf;
				PC->SetHeight(N);
				int hR = PC->GetHeight(N->right->pNode), hL = PC->GetHeight(N->left->pNode), bFact = hR - hL;
				if (abs(bFact) > 1)
				{
					PC->TruncatePath(p);
					p->direction = hL > hR ? 'L' : 'R';
					NNxt = hL > hR ? N->left->pNode : N->right->pNode;
					int hRN = PC->GetHeight(NNxt->right->pNode), hLN = PC->GetHeight(NNxt->left->pNode);
					PC->AddToPath(NNxt, hLN > hRN ? 'L' : 'R');
					ptrpp->pNode = PC->Rotate(p);
					ptrpp->pLeaf = NULL;
				}
				
				//PC->RemoveFromPath(p); 
				p = pp;
				N = p->node;
			}
			PC->SetHeight(root);

			delete ptrs;
			delete ptrd;
			delete _N;
			delete LF;
			return leaf_count;
		}

	//public:
		int DeleteByValue(T *& DataIn)
		{
			BinaryBPlusLeaf<T> * LF = FindLeaf(DataIn, true);
			return _DeleteLeaf(LF);
		}

		int DeleteByLeaf(BinaryBPlusLeaf<T> *& knownLeaf)
		{
			SetPath(knownLeaf);
			return _DeleteLeaf(knownLeaf);
		}

		//members:
		BinaryBPlusLeaf<T> * beforeLeaf, * afterLeaf;  // special points representing -∞ & +∞
		BinaryBPlusNode<T> * root;
		BinaryBPlusPathContainer<T> * PC;
		int leaf_count;
	};
};

