#include "first.hpp"
#include "CompGeo.hpp"
#include "Matrix.hpp"

Matrix::Matrix(void)
{
	alpha = NULL;
	error = NULL;
	rows = 0;
	cols = 0;
}

Matrix::Matrix(const unsigned int &r, const unsigned int &c)
{// intitiallizes an rXc matrix to all zeroes

	rows = r;
	cols = c;
	error = NULL;
	alpha = NULL;
	if ((r == 0) || (c == 0)) return;
	alpha = new double*[r];
	for (unsigned int i = 0; i < r; ++i)
	{
		alpha[i] = new double[c];
		for (unsigned int j = 0; j < c; ++j) alpha[i][j] = 0.0;
	}

}

Matrix::Matrix(const unsigned int & r, const unsigned int & c, double ** data_in)
{ 
	rows = r;
	cols = c;
	error = NULL;
	alpha = NULL;
	if ((r == 0) || (c == 0)) return;
	alpha = new double*[r];
	for (unsigned int i = 0; i < r; ++i)
	{
		alpha[i] = new double[c];
		for (unsigned int j = 0; j < c; ++j) alpha[i][j] = data_in[i][j];
	}

}

Matrix::Matrix(const Matrix & a)
{
	rows = a.rows;
	cols = a.cols;
	error = NULL;
	if (a.error != NULL)
	{
		size_t l = strlen(a.error) + 1;
		error = new char[l];
		memcpy(error, a.error, l);
	}
	alpha = NULL;
	if ((rows == 0) || (cols == 0)) return;
	alpha = new double*[rows];
	for (unsigned int i = 0; i < rows; ++i)
	{
		alpha[i] = new double[cols];
		for (unsigned int j = 0; j < cols; ++j) alpha[i][j] = a.alpha[i][j];
	}

}

Matrix::Matrix(const unsigned int & side)
{ // makes identity matrix
	rows = side;
	cols = side;
	error = NULL;
	alpha = NULL;
	if (side == 0) return;
	alpha = new double*[side];
	for (unsigned int i = 0; i < side; ++i)
	{
		alpha[i] = new double[side];
		for (unsigned int j = 0; j < side; ++j) alpha[i][j] = (i == j ? 1.0 : 0.0);
	}
}

/*
Matrix::Matrix(const CompGeo::TPoint<int> & a)
{ // makes a 1X4 matrix from TPoint a
	rows = 1;
	cols = 4;
	error = NULL;
	alpha = new double*[1];
	alpha[0] = new double[4];
	for (unsigned int j = 0; j < 3; ++j)
	{

		alpha[0][j] = static_cast<double>(a.dims[j]);
	}
	alpha[0][3] = 1.0;
}

Matrix::Matrix(const CompGeo::TPoint<int> & a, double ** alphaBuff)
{// makes 1X4 matrix w/out heap
	rows = 1;
	cols = 4;
	error = NULL;
	alpha = alphaBuff;
	for (unsigned int j = 0; j < 3; ++j)
	{

		alpha[0][j] = static_cast<double>(a.dims[j]);
	}
	alpha[0][3] = 1.0;

}
/**/

Matrix::Matrix(const CompGeo::XYZ & p)
{ // makes a 4X1 matrix from XYZ p
	rows = 4;
	cols = 1;
	error = NULL;
	alpha = new double*[4];
	alpha[0] = new double[1]; alpha[0][0] = p.x;
	alpha[1] = new double[1]; alpha[1][0] = p.y;
	alpha[2] = new double[1]; alpha[2][0] = p.z;
	alpha[3] = new double[1]; alpha[3][0] = 1.0;
}

Matrix::~Matrix(void)
{
	if ((rows == 0) || (cols == 0)) return;
	for (unsigned int i = 0; i < rows; ++i) delete[] alpha[i];
	delete[] alpha;
	rows = 0;
	cols = 0;
	alpha = NULL;
	if (error != NULL) delete[] error;
	error = NULL;
}

Matrix Matrix::operator = (const Matrix & a)
{
	this->~Matrix();
	rows = a.rows;
	cols = a.cols;
	error = NULL;
	if (a.error != NULL)
	{
		size_t l = strlen(a.error) + 1;
		error = new char[l];
		memcpy(error, a.error, l);
	}
	alpha = NULL;
	if ((rows != 0) && (cols != 0)) 
	{
		alpha = new double*[rows];
		for (unsigned int i = 0; i < rows; ++i)
		{
			alpha[i] = new double[cols];
			for (unsigned int j = 0; j < cols; ++j) alpha[i][j] = a.alpha[i][j];
		}
	}
	return *this;
}

void Matrix::ErrorEncountered(const char * msg)
{
	size_t len = strlen(msg) + 1;
	this->error = new char[len];
	memcpy(this->error, msg, len);
}

Matrix & Matrix::operator+=(const Matrix & b)
{
	if ((rows != b.rows) || (cols != b.cols))
	{
		Matrix r;
		*this = r;
		ErrorEncountered("Error: Matrix addition requires number of rows and columns to match");
		return *this;
	}
	for (unsigned int i = 0; i < rows; ++i)
		for (unsigned int j = 0; j < cols; ++j)
			alpha[i][j] += b.alpha[i][j];
	return *this;
}

Matrix operator + (const Matrix & a, const Matrix & b)
{
	Matrix r = a;
	r += b;
	return r;
}

Matrix & Matrix::operator-=(const Matrix & b)
{
	if ((rows != b.rows) || (cols != b.cols))
	{
		Matrix r;
		*this = r;
		ErrorEncountered("Error: Matrix subtraction requires number of rows and columns to match");
		return *this;
	}
	for (unsigned int i = 0; i < rows; ++i)
		for (unsigned int j = 0; j < cols; ++j)
			alpha[i][j] -= b.alpha[i][j];
	return *this;

}

Matrix operator - (const Matrix & a, const Matrix & b)
{
	Matrix r = a;
	r -= b;
	return r;
}

Matrix & Matrix::operator*= (const double & b)
{
	for (unsigned int i = 0; i < rows; ++i)
		for (unsigned int j = 0; j < cols; ++j)
			alpha[i][j] *= b;
	return *this;

}

Matrix operator * (const double & a, const Matrix & b)
{
	Matrix r = b;
	r *= a;
	return r;
}

void Matrix::TimesEquals(const Matrix & b, double * rowBuff)
{ // rowBuff needs to be big enough to hold one row from this matrix (that's cols doubles)
	if (cols != b.rows)
	{
		ErrorEncountered("Error: Matrix multiplication requires the magnitude of the left row vectors to match the magnitude of the right column vectors");
		return;
	}
	//double ** beta = new double*[rows];
	for (unsigned int i = 0; i < rows; ++i)
	{
		memcpy(rowBuff, alpha[i], sizeof(double) * cols);
		//beta[i] = new double[cols];
		for (unsigned int j = 0; j < (b.cols < cols? b.cols: cols); ++j)
		{ // this is right_row dot left_col
			alpha[i][j] = 0.0;
			for (unsigned int iDOT = 0; iDOT < cols; ++iDOT)
			{
				alpha[i][j] += rowBuff[iDOT] * b.alpha[iDOT][j];
			}
		}
	}
	if (b.cols < cols)
	{ // resize
		double ** beta = new double*[rows];
		for (unsigned int i = 0; i < rows; ++i)
		{
			beta[i] = new double[b.cols];
			memcpy(beta[i], alpha[i], sizeof(double) * b.cols);
			delete alpha[i];
		}
		delete alpha;
		alpha = beta;
		cols = b.cols;
	}

	//for (unsigned int i = 0; i < rows; ++i)
	//{
	//	delete[] alpha[i];
	//}
	//delete[] alpha;
	//alpha = beta;
	//return *this;

}

Matrix & Matrix::operator *=(const Matrix & b)
{
	double * rBuff = new double[cols];
	TimesEquals(b, rBuff);
	delete rBuff;
	return *this;
	/*
	if (cols != b.rows)
	{
		ErrorEncountered("Error: Matrix multiplication requires number of left columns to match number of right rows");
		return *this;
	}
	double ** beta = new double*[rows];
	for (unsigned int i = 0; i < rows; ++i)
	{
		beta[i] = new double[cols];
		for (unsigned int j = 0; j < cols; ++j)
		{
			beta[i][j] = 0.0;
			for (unsigned int iDOT = 0; iDOT < cols; ++iDOT)
			{
				beta[i][j] += alpha[i][iDOT] * b.alpha[iDOT][j];
			}
		}
	}
	for (unsigned int i = 0; i < rows; ++i)
	{
		delete[] alpha[i];
	}
	delete[] alpha;
	alpha = beta;
	return *this;
	*/
}

Matrix operator * (const Matrix & a, const Matrix & b)
{
	Matrix r = a;
	return r *= b;
	/*
	if (a.cols != b.rows)
	{
		r.ErrorEncountered("Error: Matrix multiplication requires number of left columns to match number of right rows");
		return r;
	}
	r.rows = a.rows;
	r.cols = b.cols;
	r.alpha = new double*[r.rows];
	for (unsigned int i = 0; i < r.rows; ++i)
	{
		r.alpha[i] = new double[r.cols];
		for (unsigned int j = 0; j < r.cols; ++j) 
		{
			double dot_product = 0.0;
			for (unsigned int k = 0; k < a.cols; ++k)
			{
				dot_product += a.alpha[i][k] * b.alpha[k][j];
			}
			if (fabs(dot_product) < MAX_FLT_PRECISION) dot_product = 0.0;
			r.alpha[i][j] = dot_product;
		}
	}
	return r;
	*/
}

Matrix Matrix::transpose(void)
{
	Matrix r;
	r.rows = cols;
	r.cols = rows;
	r.error = NULL;
	if (error != NULL)
	{
		size_t l = strlen(error) + 1;
		r.error = new char[l];
		memcpy(r.error, error, l);
	}
	if ((rows == 0) || (cols == 0)) return r;
	r.alpha = new double*[r.rows];
	for (unsigned int i = 0; i < r.rows; ++i)
	{
		r.alpha[i] = new double[r.cols];
		for (unsigned int j = 0; j < r.cols; ++j)
		{
			r.alpha[i][j] = alpha[j][i];
		}
	}
	return r;
}

Matrix GetIdentity(const unsigned int & side)
{
	Matrix ID;
	ID.rows = side;
	ID.cols = side;
	ID.error = NULL;
	ID.alpha = NULL;
	if (side == 0) return ID;
	ID.alpha = new double*[side];
	for (unsigned int i = 0; i < side; ++i)
	{
		ID.alpha[i] = new double[side];
		for (unsigned int j = 0; j < side; ++j) ID.alpha[i][j] = (i == j ? 1.0: 0.0);
	}
	return ID;
}

bool Matrix::IsRowZero(const unsigned int & r)
{
	bool IsZero = true;
	for (unsigned int j = 0; j < cols; ++j) 
	{
		if (fabs(alpha[r][j]) > DBL_EPSILON)
		{
			IsZero = false;
			j = cols;
		}
	}
	return IsZero;
}

bool Matrix::IsColumnZero(const unsigned int & c)
{
	bool IsZero = true;
	for (unsigned int i = 0; i < rows; ++i)
	{
		if (fabs(alpha[i][c]) > DBL_EPSILON)
		{
			IsZero = false;
			i = rows;
		}

	}
	return IsZero;
}

void Matrix::RowInterchange(const unsigned int & r1, const unsigned int & r2)
{

	for (unsigned int j = 0; j < cols; ++j)
	{
		double buff = alpha[r1][j];
		alpha[r1][j] = alpha[r2][j];
		alpha[r2][j] = buff;
	}

}

void Matrix::RowMultiplyAndAddTo(const double & c, const unsigned int & r1, const unsigned int & r2)
{
	double tmp;
	for (unsigned int j = 0; j < cols; ++j) 
	{
		tmp = alpha[r2][j];
		alpha[r2][j] += c * alpha[r1][j];
		tmp = alpha[r2][j];
	}

}

void Matrix::RowMultiplyBy(const double & c, const unsigned int & r)
{
	double tmp;
	for (unsigned int j = 0; j < cols; ++j) 
	{
		tmp = alpha[r][j];
		alpha[r][j] *= c;
		tmp = alpha[r][j];
	}

}

Matrix Matrix::GetInverse(void)
{
	Matrix e;
	if (rows != cols)
	{
		e.ErrorEncountered("Error: only square matrices are invertible");
		return e;
	}
	Matrix r = GetIdentity(rows), buff = *this;
	unsigned int i, k;
	double factor;
	// First we'll zero out the left lower:
	// Proceed along diagonals:
	i = 0;
	//j = 0;
	while (i < rows)
	{
		if (buff.IsColumnZero(i))
		{
			e.ErrorEncountered("Error: not invertible co-linear columns");
			return e;
		}
		
		if (buff.IsRowZero(i))
		{
			e.ErrorEncountered("Error: not invertible co-linear rows");
			return e;
		}
		if (fabs(buff.alpha[i][i]) < DBL_EPSILON)
		{ // zero cell so exchange with a non-zero row down the column
			k = i + 1;
			while (fabs(buff.alpha[k][i]) < DBL_EPSILON) ++k;
			buff.RowInterchange(i, k);
			r.RowInterchange(i, k);
		}
		// this will make the entry on the diagonal = 1:
		factor = 1 / buff.alpha[i][i];
		buff.RowMultiplyBy(factor, i);
		r.RowMultiplyBy(factor, i);
		if (i < (rows - 1))
		{
			for (k = i + 1; k < rows; ++k)
			{// zeroing out the rest of the column:
				factor = buff.alpha[k][i];
				if (fabs(factor) > DBL_EPSILON)
				{
					buff.RowMultiplyAndAddTo(-factor,i, k);
					r.RowMultiplyAndAddTo(-factor, i, k);
				}
			}
		}
		++i;
	}
	// Second: the right upper
	i = rows - 1;
	while (i > 0)
	{
		for (int L = (i - 1); L >= 0; --L)
		{
			k = L;
			factor = buff.alpha[k][i];
			if (fabs(factor) > DBL_EPSILON)
			{
				buff.RowMultiplyAndAddTo(-factor, i, k);
				r.RowMultiplyAndAddTo(-factor, i, k);
			}
		}
		--i;
	}

	return r;
}

Matrix Matrix::GetColumn(const unsigned int & c)
{
	Matrix r;
	r.cols = 1;
	r.rows = rows;
	r.alpha = new double*[rows];
	for (unsigned int i = 0; i < rows; ++i)
	{
		r.alpha[i] = new double [1];
		r.alpha[i][0] = alpha[i][c];
	}
	return r;
}

Matrix Matrix::GetRow(const unsigned int & rw)
{
	Matrix r;
	r.cols = cols;
	r.rows = 1;
	r.alpha = new double*[1];
	r.alpha[0] = new double [cols];
	for (unsigned int j = 0; j < cols; ++j)
	{
		r.alpha[0][j] = alpha[rw][j];
	}
	return r;

}

char * Matrix::GetElements(const char * desc, const unsigned int & Width, const unsigned int & Precision, unsigned int & total)
{
	char * elmnts;
	char Buff[256];
	unsigned int count = 0, i, j;

	total = 0;
	
	if (error != NULL) return error;
	total += snprintf(Buff, 256, "%s\n", desc);
	//loop 1 to get total characters:
	for (i = 0; i < rows; ++i) 
	{
		for (j = 0; j < cols; ++j) 
		{
			total += snprintf(Buff, 256, "%0 *.*g", Width, Precision, alpha[i][j]);
			//total += snprintf(Buff, 100, "\t");
		}
		total += 1; // for the \n chars
	}
	//total += 2;
	elmnts = new char[total]; 

	count += snprintf(elmnts, total, "%s\n", desc);
	for (i = 0; i < rows; ++i) 
	{
		for (j = 0; j < cols; ++j)
		{
			count += snprintf(elmnts + count, total - count, "%0 *.*g", Width, Precision, alpha[i][j]);
			//count += snprintf(elmnts + count, total - count, "\t");
		}
		count += snprintf(elmnts + count, total - count, "\n");
	}
	return elmnts;
}

CompGeo::BasicTPoint<int> Matrix::GetTPoint(void)
{
	CompGeo::BasicTPoint<int> rPt;
	rPt.X = 0;
	rPt.Y = 0;
	rPt.Z = 0;
	if ((error == NULL) && (rows == 1) && (cols == 4))
	{
		double w = alpha[0][3];
		rPt.X = static_cast<int>(alpha[0][0] / w);
		rPt.Y = static_cast<int>(alpha[0][1] / w);
		rPt.Z = static_cast<int>(alpha[0][2] / w);
		//for (unsigned int i = 0; i < 3; ++i) rPt.dims[i] = static_cast<int>(alpha[0][i] / w);
	}

	return rPt;
}

CompGeo::XYZ Matrix::GetXYZ(void)
{ // xyz in col 0
	CompGeo::XYZ rPt;
	if ((error == NULL) && (rows == 4) && (cols == 1))
	{
		double w = alpha[3][0];
		rPt.x = alpha[0][0] / w;
		rPt.y = alpha[1][0] / w;
		rPt.z = alpha[2][0] / w;
	}

	return rPt;

}

void Matrix::CopyTPoint(const CompGeo::TPoint<int> & tp)
{ // this s/b a 1X4 matrix

	alpha[0][3] = 1.0;
	for (unsigned int i = 0; i < 3; ++i)
	{
		alpha[0][i] = static_cast<double>(tp.dims[i]);
	}
}

void Matrix::CopyBasicTPoint(const CompGeo::BasicTPoint<int> & btp)
{ // this s/b a 1X4 matrix

	alpha[0][3] = 1.0;
	alpha[0][0] = static_cast<double>(btp.X);
	alpha[0][1] = static_cast<double>(btp.Y);
	alpha[0][2] = static_cast<double>(btp.Z);

}

// These next 3 methods are for 3D rotations w/ a 4X4 matrix
void Matrix::AlphaAboutX(const double & cos_alpha, const double & sin_alpha)
{
	alpha[1][1] = cos_alpha;
	alpha[2][1] = -sin_alpha;
	alpha[1][2] = sin_alpha;
	alpha[2][2] = cos_alpha;

	/*
	char * pc, d1[] = "Matrix::AlphaAboutX", d2[100];
	snprintf(d2, 100, "%s: cos_alpha = %-6.3g; sin_alpha = %-6.3g", d1, cos_alpha, sin_alpha);
	pc = d2;
	OutputDebugStringA(GetElements(pc, 6, 3));
	/**/


}

void Matrix::PhiAboutY(const double & cos_phi, const double & sin_phi)
{
	alpha[0][0] = cos_phi;
	alpha[2][0] = sin_phi;
	alpha[0][2] = -sin_phi;
	alpha[2][2] = cos_phi;

	/*
	char * pc, d1[] = "Matrix::PhiAboutY", d2[100];
	snprintf(d2, 100, "%s: cos_phi = %-6.3g; sin_phi = %-6.3g", d1, cos_phi, sin_phi);
	pc = d2;
	OutputDebugStringA(GetElements(pc, 6, 3));
	/**/


}

void Matrix::ThetaAboutZ(const double & cos_theta, const double & sin_theta)
{
	alpha[0][0] = cos_theta;
	alpha[1][0] = -sin_theta;
	alpha[0][1] = sin_theta;
	alpha[1][1] = cos_theta;

	/*
	char * pc, d1[] = "Matrix::ThetaAboutZ", d2[100];
	snprintf(d2, 100, "%s: cos_theta = %-6.3g; sin_theta = %-6.3g", d1, cos_theta, sin_theta);
	pc = d2;
	OutputDebugStringA(GetElements(pc, 6, 3));
	/**/


}

Matrix GetRotationMatrix(const char & axis, const double & cos_angle, const double & sin_angle)
{
	Matrix r(4); // identity matrix 4X4
	//double rCos = CompGeo::Rounding(cos_angle, 9), 
	//	rSin = CompGeo::Rounding(sin_angle, 9);
	switch (axis)
	{
	case 'x':
		r.AlphaAboutX(cos_angle, sin_angle);
		break;
	case 'y':
		r.PhiAboutY(cos_angle, sin_angle);
		break;
	case 'z':
		r.ThetaAboutZ(cos_angle, sin_angle);
		break;
	default:
		r.ErrorEncountered("Axis for rotation must be 'x', 'y' or 'z'");
		break;
	}

	return r;

}
