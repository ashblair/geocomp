#pragma once
#include <regex>

#ifndef MAXINTFROMFLOAT
#define MAXINTFROMFLOAT 1000000000
#endif

typedef union PLYTempValueStructType
{
	long integerData;
	double floatPointData;
	
} PLYTempValueType, * pPLYTempValueType;

typedef struct PLYPropertyStructType
{
	PLYPropertyStructType(void): min(0), max(0), tIdxList(0) {}
	std::string name;
	int tIdx, tIdxList;
	size_t min, max;
	std::vector<std::vector<PLYTempValueType>> tmps;

} PLYPropertyType, *pPLYPropertyType;

typedef struct PLYElementStructType
{
	size_t N;
	std::string e_name;
	std::vector<PLYPropertyType> props; 

} PLYElementType, *pPLYElementType;

typedef struct PLYEndianStructType
{
	union
	{
		float f;
		char b[4];
	};
} PLYEndianType, * pPLYEndianType;

enum ply_enum {plyNoError = -1, wrongplyFile = 0, badplyHeader = 1, indexplyOOR = 2, badplyPolygonVertices = 3, 
	badplyData = 4, badplyOrder = 5, badplyWordsPerLine = 6, badplyUnknownDataType = 7};

class PLYHandler
{
public:
	PLYHandler(const VID::pViewInt &, const std::string &); //, Gtk::Label *); // filename
	~PLYHandler(void);

	void LoadPLY(void);
	void ClearBuffers(void);
	void ShowFile(const size_t &, const size_t &);
	char GetEndianess(void);
	void ShowBBs(void);
	void ShowCloudCounts(void);
	void HandleUnTwinned(void);
	//void GetBIH(void); what is this doing here?

	Error plyErr;
	size_t unattachedEdges, duplicateEdges;
	std::vector<CompGeo::ITYPE> unHalfIdx;
	std::map<CompGeo::ITYPE, CompGeo::ITYPE> dupHalfIdx;

private:

	std::vector<PLYElementType> elmnts;
	std::vector<pPLYPropertyType> xyzs;
	
	VID::pViewInt pView;
	std::string fileName;
	std::vector<std::string> fileStrings;

	std::map<CompGeo::ITYPE, std::map<CompGeo::ITYPE, std::vector<CompGeo::ITYPE>>> twins; // key=A vtx; value=halflookup which is: key=B vtx; value = halfIdx std::vector

	ply_enum PE;
	double scale_fact;
	CompGeo::XYZ xyzLO, xyzHI; // unscaled from file

	CompGeo::ITYPE vtxDelta, edgeDelta;

	static const std::regex signedIntRX, unsignedIntRX, floatingPointRx, nonWSRx;
	static const std::vector<std::regex> hdrRx, vLstBeginRx, vLstEndRx;
	static const std::vector<std::string> plyErrs, hdrKeys, hdrTypes;

	//Gtk::Label * statusWnd;

	std::string setError(ply_enum &, const ply_enum &, const size_t &, const std::string &);
	void updateStatus(const std::string &);
	void postVertexExtractionProcessing(PLYElementType &);	
	void setDeltas(void);
	size_t getElements(void);
	size_t getVertexList(PLYElementType &, const size_t &);
	void storeVertices(const PLYElementType &);
	size_t getFaceList(PLYElementType &, const size_t &);
	size_t skipUnknownList(const PLYElementType &, const size_t &);
	void handleError(std::string = "");
	void getAllTwins(void);
	void addTwinLookup(const CompGeo::ITYPE &, const CompGeo::ITYPE &, const CompGeo::ITYPE &);
	bool AddValue(const std::string &, PLYPropertyType &, const size_t &);
	bool goodInteger(const std::string &, const unsigned int &);
	bool goodFloatingPoint(const std::string &);
	bool isVertexList(const std::string &, const unsigned int &, const unsigned int &);
	int findVertexList(const PLYElementType &);
	size_t AddValue(const std::vector<std::string> &, PLYPropertyType &);
	std::vector<std::string> ParseByWhiteSpace(std::string);
	int headerFirst(const std::string &);
	int headerType(const std::string &);
	bool validateHeaderLine(const std::string &, const int &);
	std::string commatize(const std::string &);
};



