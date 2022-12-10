#pragma once

class Matrix
{
public:
	Matrix(void);
	Matrix(const unsigned int &, const unsigned int &);
	Matrix(const unsigned int &, const unsigned int &, double **);
	//Matrix(const unsigned int &, const unsigned int &, double *(double []));
	Matrix(const Matrix &);
	Matrix(const unsigned int &); // makes identity matrix
	//Matrix(const CompGeo::TPoint<int> &); // makes 1X4 matrix from TPoint
	//Matrix(const CompGeo::TPoint<int> &, double **); // makes 1X4 matrix w/out heap
	Matrix(const CompGeo::XYZ &);  // makes 4X1 matrix from XYZ
	~Matrix(void);
	Matrix operator = (const Matrix &);
	Matrix & operator += (const Matrix &);
	friend Matrix operator + (const Matrix &, const Matrix &);
	Matrix & operator -= (const Matrix &);
	friend Matrix operator - (const Matrix &, const Matrix &);
	Matrix & operator *= (const double &);
	void TimesEquals(const Matrix &, double *);
	Matrix & operator *= (const Matrix &);
	friend Matrix operator * (const double &, const Matrix &);
	friend Matrix operator * (const Matrix &a, const double & b) {return b * a;};
	friend Matrix operator * (const Matrix &, const Matrix &);
	void ErrorEncountered(const char *);
	Matrix transpose(void);
	friend Matrix GetIdentity(const unsigned int &);
	Matrix GetInverse(void);
	void RowInterchange(const unsigned int &, const unsigned int &);
	void RowMultiplyAndAddTo(const double &, const unsigned int &, const unsigned int &);
	void RowMultiplyBy(const double &, const unsigned int &);
	bool IsRowZero(const unsigned int &);
	bool IsColumnZero(const unsigned int &);
	Matrix GetRow(const unsigned int &);
	Matrix GetColumn(const unsigned int &);
	char * GetElements(const char *, const unsigned int &, const unsigned int &, unsigned int &); // for print out
	CompGeo::BasicTPoint<int> GetTPoint(void);
	CompGeo::XYZ GetXYZ(void);
	void CopyTPoint(const CompGeo::TPoint<int> &);
	void CopyBasicTPoint(const CompGeo::BasicTPoint<int> &);
	void AlphaAboutX(const double &, const double &);
	void PhiAboutY(const double &, const double &);
	void ThetaAboutZ(const double &, const double &);


	double ** alpha;
	unsigned int rows, cols;
	char * error;
};

Matrix GetRotationMatrix(const char &, const double &, const double &);
