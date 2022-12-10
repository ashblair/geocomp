#pragma once

//bool ConvexHullLessThan(CompGeo::XYType, CompGeo::XYType);
class ConvexHullXY:public CompGeo::XY
{
public:
	ConvexHullXY(void);
	ConvexHullXY(const CompGeo::XY &);
	friend bool operator < (const ConvexHullXY &, const ConvexHullXY &);
};

class ConvexHull
{
public:
	ConvexHull(void);
	ConvexHull(std::vector<pPGonWork>*&);
	~ConvexHull(void);
	void AddVertex(ConvexHullXY);
	pPGonWork CalculateHull(void);
	//void DeleteXYArray(CompGeo::XYArrayType&);

private:
	CompGeo::pXYListType L_upper, L_lower;
	int num_in_upper, num_in_lower, numVerts;
	//CompGeo::XYArrayType vertices;
	ConvexHullXY * vertices;
	//ConvexHullXY * pXYBuff;
	/*
	void MergeSort(CompGeo::XYArrayType&, int, int);
	void Merge(CompGeo::XYArrayType&, int, int, int);
	bool LessThan(CompGeo::XYType, CompGeo::XYType);
	bool Equal(CompGeo::XYType, CompGeo::XYType);
	bool LessThanOrEqual(CompGeo::XYType, CompGeo::XYType);
	float Cross(CompGeo::XYType, CompGeo::XYType); // 2D cross product returns scaler along k vector
	bool IsRightTurn(CompGeo::EdgeType, CompGeo::XYType); // edge versus vertex
	*/
	void AddToList(CompGeo::pXYListType&, ConvexHullXY);
	void DeletePenultimate(CompGeo::pXYListType&);
	int DeleteFirstAndLast(CompGeo::pXYListType&, ConvexHullXY, ConvexHullXY);
	pPGonWork Append(CompGeo::pXYListType&, CompGeo::pXYListType&);
	void DeleteList(CompGeo::pXYListType&);
	//ConvexHullXY *& TranslateArray(void);
	//void TranslateArray(ConvexHullXY *&, int);
};

