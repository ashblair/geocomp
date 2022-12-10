#include "first.hpp"
#include "CompGeo.hpp"
//static XYZ W_ORIGIN, W_I_HAT, W_J_HAT, W_K_HAT;
CompGeo::XYZ CompGeo::WorldType::W_ORIGIN = CompGeo::XYZ(0.0, 0.0, 0.0);
const CompGeo::XYZ CompGeo::WorldType::W_I_HAT = CompGeo::XYZ(1.0, 0.0, 0.0);
const CompGeo::XYZ CompGeo::WorldType::W_J_HAT = CompGeo::XYZ(0.0, 1.0, 0.0);
const CompGeo::XYZ CompGeo::WorldType::W_K_HAT = CompGeo::XYZ(0.0, 0.0, 1.0);


bool CompGeo::BEq(const char a, const char b)
{
	return (a == b);
}
bool CompGeo::BEq(const int a, const int b)
{
	return (a == b);
}
bool CompGeo::BEq(const long a, const long b)
{
	return (a == b);
}
bool CompGeo::BEq(const long long a, const long long b)
{
	return (a == b);
}
bool CompGeo::BEq(const float a, const float b)
{
	return (fabs(a - b) < MAX_FLT_PRECISION);
}
bool CompGeo::BEq(const double a, double b)
{
	return (fabs(a - b) < MAX_FLT_PRECISION);
}
bool CompGeo::BEq(const long double a, long double b)
{
	return (fabs(a - b) < MAX_FLT_PRECISION);
}

/*

bool AVLEdgeLessThan(void * v1, void * v2)
{
	CompGeo::EdgeType a = *((CompGeo::EdgeType *) v1), b = *((CompGeo::EdgeType *) v2);

	return false;
}

bool AVLVertexLessThan(void * v1, void * v2)
{

	CompGeo::XYType a = *((CompGeo::XYType *) v1), b = *((CompGeo::XYType *) v2);

	if (fabs(a.y - b.y) < FLT_EPSILON) return (a.x < b.x);
	return (a.y > b.y);
	
}
		XY(void);
		XY(const XY&);
		XY(D2D1_POINT_2F);
		XY(XYType);
		XY(float, float);
		friend bool operator<(const XY&, const XY&);

	private:
		float x, y;
		bool IsNonDefault;

*/

//CompGeo::CompGeoType CompGeo::CGT = CompGeo::NO_SELECTION;
/*
CompGeo::typeCompGeo::typeCompGeo(CompGeoType cgt)
{
	CGT = cgt;
}



void CompGeo::InitializeCompGeoData(void)
{
	//CompGeo::typeCompGeo::CGT = CompGeo::typeCompGeo::NO_SELECTION;
}
*/

CompGeo::XYZ::XYZ(void)
{
	x = 0.0;
	y = 0.0;
	z = 0.0;
}

CompGeo::XYZ::XYZ(double x_in, double y_in, double z_in)
{
	x = x_in;
	y = y_in;
	z = z_in;
}

CompGeo::XYZ::XYZ(const XYZ & xyz_in)
{
	x = xyz_in.x;
	y = xyz_in.y;
	z = xyz_in.z;
}

CompGeo::XYZ::XYZ(const BasicTPoint<int> & tpt_in)
{
	x = (double)tpt_in.X;
	y = (double)tpt_in.Y;
	z = (double)tpt_in.Z;
}

CompGeo::XYZ::XYZ(const BasicTPoint<double> & tpt_in)
{
	x = tpt_in.X;
	y = tpt_in.Y;
	z = tpt_in.Z;
}

CompGeo::XYZ & CompGeo::XYZ::operator = (const XYZ & a)
{
	x = a.x;
	y = a.y;
	z = a.z;

	return *this;
}

bool CompGeo::XYZ::IsEq(const XYZ & a) const
{
	return (BEq(x, a.x) && BEq(y, a.y) && BEq(z, a.z));
	//return ((fabs(a.x - x) < MAX_FLT_PRECISION) && (fabs(a.y - y) < MAX_FLT_PRECISION) && (fabs(a.z - z) < MAX_FLT_PRECISION));
}
//bool CompGeo::operator == (const XYZ& a, const XYZ& b)

CompGeo::XYZ & CompGeo::XYZ::operator += (const XYZ& a)
{
	x += a.x;
	y += a.y;
	z += a.z;

	return *this;
}
		
CompGeo::XYZ & CompGeo::XYZ::operator -= (const XYZ& a)
{
	x -= a.x;
	y -= a.y;
	z -= a.z;

	return *this;
}
		
CompGeo::XYZ & CompGeo::XYZ::operator *= (const double & c) // scaler multiplication
{
	x *= c;
	y *= c;
	z *= c;

	return *this;
}
		
CompGeo::XYZ & CompGeo::XYZ::operator /= (const double & d) // scaler division
{
	x /= d;
	y /= d;
	z /= d;

	return *this;
}
		
//CompGeo::XYZ CompGeo::operator * (const double & c, const XYZ & a)
/*
bool operator == (const CompGeo::XYZ & a, const CompGeo::XYZ & b)
{
	return ((fabs(a.x - b.x) < MAX_FLT_PRECISION) && 
		(fabs(a.y - b.y) < MAX_FLT_PRECISION) &&
		(fabs(a.z - b.z) < MAX_FLT_PRECISION));
}

bool operator !=(const CompGeo::XYZ&a, const CompGeo::XYZ&b) 
{
	return !(a == b);
}

CompGeo::XYZ operator * (const double & c, const CompGeo::XYZ & a)
{
	CompGeo::XYZ rVal = a;
	rVal *= c;

	return rVal;

}

CompGeo::XYZ operator * (const CompGeo::XYZ&a, const double &b) 
{
	return b * a;
}

CompGeo::XYZ operator / (const CompGeo::XYZ&a, const double & b) 
{
	return ((1 / b) * a);
}

double operator * (const CompGeo::XYZ & a, const CompGeo::XYZ & b) // dot product
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

CompGeo::XYZ operator + (const CompGeo::XYZ & a, const CompGeo::XYZ & b)
{
	CompGeo::XYZ rVal = a;
	rVal += b;

	return rVal;
}
		
CompGeo::XYZ operator - (const CompGeo::XYZ & a, const CompGeo::XYZ & b)
{
	CompGeo::XYZ rVal = a;
	rVal -= b;

	return rVal;
}
*/		

double CompGeo::XYZ::GetMagnitude(void) const
{
	return sqrt(sqr(x) + sqr(y) + sqr(z));
}

CompGeo::BasicTPoint<int> CompGeo::XYZ::GetBasicTPoint(void) const
{
	BasicTPoint<int> rVal;
	rVal.X = (int)x;
	rVal.Y = (int)y;
	rVal.Z = (int)z;
	return rVal;
}


CompGeo::XY::XY(void)
{
	x = 0.0;
	y = 0.0;
	//IsNonDefault = false;
}

CompGeo::XY::XY(const XY& xy)
{
	x = xy.x;
	y = xy.y;
	//IsNonDefault = xy.IsNonDefault;
}

CompGeo::XY::XY(D2D1_POINT_2F v)
{
	x = (double)v.x;
	y = (double)v.y;
	//IsNonDefault = true;

}

CompGeo::XY::XY(XYType p)
{
	x = (double)p.x;
	y = (double)p.y;
	//IsNonDefault = true;
}

CompGeo::XY::XY(double x_in, double y_in)
{
	x = x_in;
	y = y_in;
	//IsNonDefault = true;
}

CompGeo::XY::XY(const BasicTPoint<int> & a)
{
	x = (double)a.X;
	y = (double)a.Y;
}

CompGeo::XY::XY(const BasicTPoint<double> & a)
{
	x = a.X;
	y = a.Y;
}

CompGeo::XY & CompGeo::XY::operator = (const XY & a)
{
	x = a.x;
	y = a.y;
	//IsNonDefault = a.IsNonDefault;

	return *this;
}

bool CompGeo::XY::IsEq(const XY & a) const
{
	return (BEq(x, a.x) && BEq(y, a.y));
	//return ((fabs(a.x - x) < MAX_FLT_PRECISION) && (fabs(a.y - y) < MAX_FLT_PRECISION));	
}

CompGeo::XY & CompGeo::XY::operator += (const XY & a)
{
	x += a.x;
	y += a.y;
	return *this;
}

CompGeo::XY & CompGeo::XY::operator -= (const XY & a)
{
	x -= a.x;
	y -= a.y;
	return *this;
}

CompGeo::XY & CompGeo::XY::operator *= (const double & a)
{// scaler multiplication:
	x *= a;
	y *= a;
	return *this;
}

CompGeo::XY & CompGeo::XY::operator /= (const double & a)
{
	x /= a;
	y /= a;
	return *this;
}
/*
bool operator==(const CompGeo::XY&a, const CompGeo::XY&b)
{
	return ((fabs(a.x - b.x) < MAX_FLT_PRECISION) && 
		(fabs(a.y - b.y) < MAX_FLT_PRECISION));

}

bool operator!=(const CompGeo::XY&a, const CompGeo::XY&b) 
{
	return !(a == b);
}

CompGeo::XY operator * (const double & a, const CompGeo::XY & b)
{
	CompGeo::XY r(b);
	r *= a;
	return r;
}

CompGeo::XY operator*(const CompGeo::XY&a, const double&b) 
{
	return b*a;
}

CompGeo::XY operator/(const CompGeo::XY&a, const double&b) 
{
	return ((1/b) * a);
}

double operator * (const CompGeo::XY & a, const CompGeo::XY & b)
{ // dot product
	return a.x * b.x + a.y * b.y;
}

CompGeo::XY operator + (const CompGeo::XY & a, const CompGeo::XY & b)
{
	CompGeo::XY r(a);
	r += b;
	return r;
}

CompGeo::XY operator - (const CompGeo::XY & a, const CompGeo::XY & b)
{
	CompGeo::XY r(a);
	r -= b;
	return r;
}
*/
D2D1_POINT_2F CompGeo::XY::GetPOINT(void) const
{
	D2D1_POINT_2F r;
	r.x = (float)x;
	r.y = (float)y;
	return r;

}

double CompGeo::XY::GetMagnitude(void) const
{
	return sqrt(sqr(x) + sqr(y));
}

/*
bool CompGeo::operator<(const CompGeo::XY & a, const CompGeo::XY & b)
{
	if (typeCompGeo::CGT = typeCompGeo::CONVEX_HULL)
	{
		if (fabs(b.x - a.x) < FLT_EPSILON) return (a.y < b.y);
		return (a.x < b.x);
	}
	if (typeCompGeo::CGT = typeCompGeo::SEGMENT_INTERSECT)
	{
		if (fabs(b.y - a.y) < FLT_EPSILON) return (a.x < b.x);
		return (a.y > b.y);
	}
	MessageBox(NULL, TEXT("Order Selection Variable CGT not set in CompGeo"), TEXT("Fatal Error"), MB_OK);
	exit (EXIT_FAILURE);
	return false;
}

bool operator==(const CompGeo::XY & a, const CompGeo::XY & b)
{
	return ((fabs(b.x - a.x) < MAX_FLT_PRECISION) && (fabs(b.y - a.y) < MAX_FLT_PRECISION));
}
*/

//		unsigned int QuadPP;	// 0 - 8: 0=origin point, 1=x axis, 2=Q1, 3=y axis, 4=Q2, 5=-x axis, 6=Q3, 7=-y axis, 8=Q4

CompGeo::UnitCircleMeasureStructType::UnitCircleMeasureStructType(void)
{
	QuadPP = 0;
	the_tan = 0.0;
}

CompGeo::UnitCircleMeasureStructType::UnitCircleMeasureStructType(const UnitCircleMeasureStructType& a)
{
	QuadPP = a.QuadPP;
	the_tan = a.the_tan;
}

CompGeo::UnitCircleMeasureStructType & CompGeo::UnitCircleMeasureStructType::operator = (const UnitCircleMeasureStructType& a)
{
	QuadPP = a.QuadPP;
	the_tan = a.the_tan;
	return *this;
}

CompGeo::UnitCircleMeasureStructType::UnitCircleMeasureStructType(CompGeo::XY X_0, CompGeo::XY O_0, CompGeo::XY P_0)
{ // OX is the baseline; the measure proxies for the angle measure from baseline counterclockwise to OP
  // O is like the origin where you center your protractor & X provides the axis around which you measure 
	assert ((X_0 != O_0) && (P_0 != O_0));
	CompGeo::XY A = X_0 - O_0, B = P_0 - O_0;
	A /= A.GetMagnitude();
	B /= B.GetMagnitude();
	double x = A * B, y = CompGeo::Cross(A, B);
	bool xZero = fabs(x) < MAX_FLT_PRECISION, yZero = fabs(y) < MAX_FLT_PRECISION,
		xNeg = x < 0.0, yNeg = y < 0.0;
	QuadPP = 0;
	the_tan = 0.0;
	QuadPP = xZero && yZero? 0: 
		yZero? xNeg? 5: 1 :
		xZero? yNeg? 7: 3 :
		xNeg? yNeg? 6: 4 :
		yNeg? 8: 2;
	if ((QuadPP > 0) && (0 == (QuadPP % 2))) the_tan = y / x;
}

CompGeo::UnitCircleMeasureStructType::UnitCircleMeasureStructType(CompGeo::XYZ X_0, CompGeo::XYZ O_0, CompGeo::XYZ P_0)
{ // OX is the baseline; the measure proxies for the angle measure from baseline counterclockwise to OP
// analogous to the unit circle angle designated by the 3 points (1,0)(0,0)P where P is any point on the circle
	assert ((X_0 != O_0) && (P_0 != O_0));
	CompGeo::XYZ A = X_0 - O_0, B = P_0 - O_0;
	A /= A.GetMagnitude();
	B /= B.GetMagnitude();
	double x = A * B, y = CompGeo::Cross(A, B).GetMagnitude();
	bool xZero = fabs(x) < MAX_FLT_PRECISION, yZero = fabs(y) < MAX_FLT_PRECISION,
		xNeg = x < 0.0, yNeg = y < 0.0;
	QuadPP = 0;
	the_tan = 0.0;
	QuadPP = xZero && yZero? 0: 
		yZero? xNeg? 5: 1 :
		xZero? yNeg? 7: 3 :
		xNeg? yNeg? 6: 4 :
		yNeg? 8: 2;
	if ((QuadPP > 0) && (0 == (QuadPP % 2))) the_tan = y / x;
}

bool CompGeo::UnitCircleMeasureStructType::IsEq(const UnitCircleMeasureStructType& a) const
{
	return ((QuadPP == a.QuadPP) && BEq(the_tan, a.the_tan));
	//return ((QuadPP == a.QuadPP) && (fabs(the_tan - a.the_tan) < MAX_FLT_PRECISION));
}

bool CompGeo::UnitCircleMeasureStructType::IsGT(const UnitCircleMeasureStructType& a) const
{
	if (IsEq(a)) return false;
	if (QuadPP == a.QuadPP) return (the_tan > a.the_tan);
	else return (QuadPP > a.QuadPP);
}
bool CompGeo::UnitCircleMeasureStructType::IsLT(const UnitCircleMeasureStructType& a) const
{
	if (IsEq(a)) return false;
	if (QuadPP == a.QuadPP) return (the_tan < a.the_tan);
	else return (QuadPP < a.QuadPP);
	
}


CompGeo::Line2D::Line2D(void)
{ // x axis
	L = XY(1.0, 0.0); // i.e. i hat vector
	R0 = XY(0.0, 0.0);
}

CompGeo::Line2D::Line2D(const Line2D & a)
{
	L = a.L;
	R0 = a.R0;
}

CompGeo::Line2D::Line2D(const XY & Point0, const XY & ParallelVector)
{
	L = ParallelVector;
	R0 = Point0;
}

CompGeo::XY CompGeo::Line2D::GetPoint(double t)
{ // R = R0 + tL

	//const double tc = t;
	//const XY L_C(L);// = XY(L);
	//return R0 + tc * L_C;
	return R0 + (t * L);
}
//friend XY operator*(double, XY);
//friend XY operator*(const XY&a, const double&b) {return b*a;}

CompGeo::Line3D::Line3D(void)
{
	L = XYZ(1.0, 0.0, 0.0);
	R0 = XYZ(0.0, 0.0, 0.0);
}

CompGeo::Line3D::Line3D(const Line3D & a)
{
	L = a.L;
	R0 = a.R0;
}

CompGeo::Line3D::Line3D(const XYZ & Point0, const XYZ & ParallelVector)
{
	L = ParallelVector;
	R0 = Point0;
}

CompGeo::XYZ CompGeo::Line3D::GetPoint(double t)
{
	return R0 + (t * L);
}

CompGeo::BoundingBox::BoundingBox(void)
{ // unit box around (0, 0)

	xMin = -1.0;
	yMin = -1.0;
	xMax = 1.0;
	yMax = 1.0;
}

CompGeo::BoundingBox::BoundingBox(const BoundingBox & a)
{
	xMin = a.xMin;
	yMin = a.yMin;
	xMax = a.xMax;
	yMax = a.yMax;
}

CompGeo::BoundingBox::BoundingBox(const XY & LowerLeft, const XY & UpperRight)
{
	xMin = LowerLeft.x;
	yMin = LowerLeft.y;
	xMax = UpperRight.x;
	yMax = UpperRight.y;
}

CompGeo::BoundingBox::BoundingBox(double x_Min, double y_Min, double x_Max, double y_Max)
{
	xMin = x_Min;
	yMin = y_Min;
	xMax = x_Max;
	yMax = y_Max;
}

bool CompGeo::BoundingBox::Intersected(const Line2D & l0, bool & grazed, bool & h_plus)
{
	XY CRN[] = {XY(xMin, yMin), XY(xMax, yMin), XY(xMax, yMax), XY(xMin, yMax)}, xyBuff;
	int sCurrent = 0, s;
	bool isSet = false, XSect = false;
	grazed = false;
	h_plus = false;
	Line2D l = l0;

	for (int i = 0; i < 4; ++i)
	{
		xyBuff = CRN[i] - l.R0; // this is a vector from R0 to the corner point
		double c = Cross(l.L, xyBuff); // L is a vector from R0 along l in the + direction
		if (fabs(c) < MAX_FLT_PRECISION) 
		{
			grazed = true; // line l intersects a corner of the bounding box
			s = 0;
		}
		else
		{ // corner point lies on the + or - side of line l
			if (c < 0) s = -1;
			else s = 1;

			if (isSet)
			{
				if (s != sCurrent) XSect = true;
			}
			else
			{
				isSet = true;
				sCurrent = s;
			}
		}
	}

	if (!XSect) h_plus = (sCurrent == 1);

	return XSect;

}

/*
//template class CompGeo::Sorter<CompGeo::XY>; // hack to fix linker errors

template <class T>
CompGeo::Sorter<T>::Sorter(void)
{

	s_array = NULL;
	firstIdx = 0;
	lastIdx = 0;
	numElements = 0;
}

template <class T>
CompGeo::Sorter<T>::Sorter(const Sorter & s)
{
	s_array = s.s_array;
	firstIdx = s.firstIdx;
	lastIdx = s.lastIdx;
	numElements = s.numElements;
}

template <class T>
CompGeo::Sorter<T>::Sorter(T * Table, int first, int last, int length)
{

	s_array = Table;
	
	firstIdx = first;
	lastIdx = last;
	numElements = length;

}

template <class T>
CompGeo::Sorter<T>::~Sorter(void)
{

}

template <class T>
void CompGeo::Sorter<T>::AddArray(T * Table, int first, int last, int length)
{
	
	s_array = Table;
	firstIdx = first;
	lastIdx = last;
	numElements = length;
}

/*
void CompGeo::Sorter::AddLessThanFunctionPointer(bool (*f)(void * a, void * b))
{

	Less_Than = f;

}


template <class T>
T * CompGeo::Sorter<T>::doSort(void)
{
	
	if ((s_array != NULL) && ((lastIdx - firstIdx + 1) > 1)) MergeSort(firstIdx, lastIdx);
	return s_array;

}

template <class T>
int CompGeo::Sorter<T>::GetLength(void)
{
	return numElements;

}

template <class T>
void CompGeo::Sorter<T>::MergeSort(int First, int Last)
{
	if (First < Last)
	{
		int Middle = (First + Last) / 2;
		MergeSort(First, Middle);
		MergeSort(Middle + 1, Last);
		Merge(First, Middle, Last);
	}

}

template <class T>
void CompGeo::Sorter<T>::Merge(int First, int Middle, int Last)
{   // part of recursive Merge Sort s/b called log base 2 N times
	// uses and discards from heap for temporary storage buffer
	// will need enough memory for 2 arrays the size of the initial array s_array
	int numElements = Last - First + 1;;
	T * Temp = new T[numElements];
	int NextLeft = First, NextRight = Middle + 1, Index = 0; //Index = First;
	while ((NextLeft <= Middle) && (NextRight <= Last))
	{
		if (s_array[NextLeft] < s_array[NextRight])
		{
			Temp[Index++] = s_array[NextLeft++];
		}
		else
		{
			Temp[Index++] = s_array[NextRight++];
		}
	}
	while (NextLeft <= Middle)
	{
		Temp[Index++] = s_array[NextLeft++];
	}
	while (NextRight <= Last)
	{
		Temp[Index++] = s_array[NextRight++];
	}
	memcpy(&s_array[First], Temp, (Last - First + 1) * sizeof(T));
	delete [] Temp;
	Temp = NULL;
	numElements = 0;

}
*/

CompGeo::XYZ CompGeo::Cross(const XYZ & a, const XYZ & b) // cross product
{
	double xVal = a.y * b.z - b.y * a.z,
		yVal = b.x * a.z - a.x * b.z,
		zVal = a.x * b.y - b.x * a.y;
	XYZ rVal(xVal, yVal, zVal);

	return rVal;

}

double CompGeo::Cross(XY a, XY b)
	// 2D cross product returns scaler along k vector
{
	return (a.x * b.y - b.x * a.y);

}

bool CompGeo::IsRightTurn(EdgeType e, XY r)
	// path edge towards vertex
{
	XY a, b;

	a.x = r.x - e.lo.x;
	a.y = r.y - e.lo.y;

	b.x = e.hi.x - e.lo.x;
	b.y = e.hi.y - e.lo.y;

	return (Cross(a, b) < -MAX_FLT_PRECISION);

}

bool CompGeo::IsLeftTurn(EdgeType e, XY r)
	// path edge towards vertex
{
	XY a, b;

	a.x = r.x - e.lo.x;
	a.y = r.y - e.lo.y;

	b.x = e.hi.x - e.lo.x;
	b.y = e.hi.y - e.lo.y;

	return (Cross(a, b) > MAX_FLT_PRECISION);

}

bool CompGeo::IsInLine(EdgeType e, XY r)
	// path edge towards vertex
{
	XY a, b;

	a.x = r.x - e.lo.x;
	a.y = r.y - e.lo.y;

	b.x = e.hi.x - e.lo.x;
	b.y = e.hi.y - e.lo.y;

	return (fabs(Cross(a, b)) <= MAX_FLT_PRECISION);

}

bool CompGeo::XYTypesEqual(XYType a, XYType b)
{
	return ((fabs(a.x - b.x) < MAX_FLT_PRECISION) && (fabs(a.y - b.y) < MAX_FLT_PRECISION));

}

double CompGeo::Rounding(double x, unsigned int n)
{ // rounds double at unsigned int decimal place after
	assert(n <= 10);
	double r = x, rfractpart = 0.0, rintpart = 0.0, sign = 0.0, m = 0.0,
		M[11] = {1.0, 10.0, 100.0, 1000.0, 1.0E4, 1.0E5, 1.0E6, 1.0E7, 1.0E8, 1.0E9, 1.0E10};
	//for (unsigned int i = 0; i < n; ++i) m *= 10.0;
	m = M[n];
	if (fabs(r) < (1.0 / m)) r = 0.0; // no more -0 I hope
	else
	{	
		r *= m;
		rfractpart = modf(r, &rintpart);
		sign = r > 0.0? 1.0: -1.0;
		rintpart += sign * (fabs(rfractpart) >= 0.5? 1: 0);
		r = rintpart / m; // this will produce a small artifact ~10^-15 due to the base 2 - decimal problem
	}
	return r;
}


