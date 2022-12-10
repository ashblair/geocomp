#include "first.hpp"
#include "Error.hpp"
#include "CompGeo.hpp"
#include "ViewInterface.hpp"
#include "PLYHandler.hpp"

using namespace std;

// static variable intialization:
const regex PLYHandler::signedIntRX = regex("^[+|-]?[0-9]+$"), 
	PLYHandler::unsignedIntRX = regex("^[0-9]+$"), 
	PLYHandler::floatingPointRx = regex("[+-]?([0-9]*[.])?[0-9]+([E|e][+-]?[0-9]+)?"),
	PLYHandler::nonWSRx = regex("\\S+");

const vector<regex> PLYHandler::hdrRx = {	
		regex("^ply$"), 
		regex("^format\\s+ascii\\s+\\d+.\\d+\\s*$"),
		regex("^comment"), 
		regex("^element\\s+\\w+\\s+\\d+\\s*$"),
		regex("^property\\s+(\\w+)|(list\\s+\\w+\\s+\\w+)\\s+\\w+\\s*$"), 
		regex("^end_header\\s*$")},
	PLYHandler::vLstBeginRx = {
		regex("^vertices\\S+$", regex_constants::icase), 
		regex("^vertex\\S+$", regex_constants::icase), 
		regex("^vtx\\S+$", regex_constants::icase)},
	PLYHandler::vLstEndRx = {
		regex("^\\S+index$", regex_constants::icase), 
		regex("^\\S+indices$", regex_constants::icase), 
		regex("^\\S+lst$", regex_constants::icase), 
		regex("^\\S+list$", regex_constants::icase)};




const vector<string> PLYHandler::plyErrs = {
	"Error: Wrong File Type - Not PLY",
	"Error: Bad Header - Need x, y, z in vertex element And list in face element",
	"Error: Index Out Of Range",
	"Error: Bad Polygon - Need 3+ Vertices",
	"Error: File data does not match type",
	"Error: Header out of order",
	"Error: Incorrect Words Per Line",
	"Error: Unknown Data Type"},
	PLYHandler::hdrKeys = {"ply", "format", "comment", "element", "property", "end_header"},
	PLYHandler::hdrTypes = {"char", "uchar", "short", "ushort", "int", "uint", "float", "double", "list"};

/**/
// end of static intialization

PLYHandler::PLYHandler(const VID::pViewInt & pV, const string & fN): 
pView(pV), fileName(fN), PE(plyNoError)
{
	setDeltas();
}

PLYHandler::~PLYHandler(void)
{
	ClearBuffers();

}

void PLYHandler::ClearBuffers(void)
{
	xyzs.clear();
	elmnts.clear();
	fileStrings.clear();
	twins.clear();
}

string PLYHandler::setError(ply_enum & errorOut, const ply_enum & errorIn, const size_t & currentLineNumber, const string & currentLine)
{
	string r = to_string(currentLineNumber) + ") " + currentLine;
	errorOut = errorIn;
	return r;
}

void PLYHandler::updateStatus(const string & sMsg)
{
	return;
	//statusWnd->set_text(sMsg.c_str());
	//while(gtk_events_pending()) {gtk_main_iteration_do(false);}
}

void PLYHandler::postVertexExtractionProcessing(PLYElementType & emt)
{
	// looking for xyz properties in vertex element
	bool foundBase[] = {false, false, false};
	
	string varNames[] = {"x", "y", "z"};
	for (int i = 0; i < 3; ++i)
	{
		string vN = varNames[i];
		for (vector<PLYPropertyType>::iterator pit = emt.props.begin(); pit != emt.props.end(); ++pit)
		{
			pPLYPropertyType PT = &(*pit);
			if (PT->name.compare(vN) == 0) 
			{
				foundBase[i] = true;
				xyzs.push_back(PT);
				break;
			}
		}
	}

	bool foundXYZ = foundBase[0] && foundBase[1] && foundBase[2];

	if (!foundXYZ)
	{
		PE = badplyHeader;
		handleError();
		return;
	}

	size_t xloi = xyzs[0]->min, yloi = xyzs[1]->min, zloi = xyzs[2]->min,
		xhii = xyzs[0]->max, yhii = xyzs[1]->max, zhii = xyzs[2]->max;

	bool xd = xyzs[0]->tIdx > 5, yd = xyzs[1]->tIdx > 5, zd = xyzs[2]->tIdx > 5;

	PLYTempValueType xlot = xyzs[0]->tmps[xloi][0], ylot = xyzs[1]->tmps[yloi][0], zlot = xyzs[2]->tmps[zloi][0],
		xhit = xyzs[0]->tmps[xhii][0], yhit = xyzs[1]->tmps[yhii][0], zhit = xyzs[2]->tmps[zhii][0];

	double xlod = xd? xlot.floatPointData: static_cast<double>(xlot.integerData),
		ylod = yd? ylot.floatPointData: static_cast<double>(ylot.integerData),
		zlod = zd? zlot.floatPointData: static_cast<double>(zlot.integerData),
		xhid = xd? xhit.floatPointData: static_cast<double>(xhit.integerData),
		yhid = yd? yhit.floatPointData: static_cast<double>(yhit.integerData),
		zhid = zd? zhit.floatPointData: static_cast<double>(zhit.integerData),
		maxd = fabs(xlod);

	if (maxd < fabs(ylod)) maxd = fabs(ylod);
	if (maxd < fabs(zlod)) maxd = fabs(zlod);
	if (maxd < fabs(xhid)) maxd = fabs(xhid);
	if (maxd < fabs(yhid)) maxd = fabs(yhid);
	if (maxd < fabs(zhid)) maxd = fabs(zhid);


	scale_fact = MAXINTFROMFLOAT / maxd;

	pView->LoIdx[0] = static_cast<CompGeo::ITYPE>(xloi) + vtxDelta; 
	pView->LoIdx[1] = static_cast<CompGeo::ITYPE>(yloi) + vtxDelta; 
	pView->LoIdx[2] = static_cast<CompGeo::ITYPE>(zloi) + vtxDelta;
	pView->HiIdx[0] = static_cast<CompGeo::ITYPE>(xhii) + vtxDelta; 
	pView->HiIdx[1] = static_cast<CompGeo::ITYPE>(yhii) + vtxDelta; 
	pView->HiIdx[2] = static_cast<CompGeo::ITYPE>(zhii) + vtxDelta;

	xyzLO = CompGeo::XYZ(xlod, ylod, zlod);
	xyzHI = CompGeo::XYZ(xhid, yhid, zhid);

	pView->bb.LO = CompGeo::BasicTPoint<int>(static_cast<int>(xlod * scale_fact), static_cast<int>(ylod * scale_fact), static_cast<int>(zlod * scale_fact));
	pView->bb.HI = CompGeo::BasicTPoint<int>(static_cast<int>(xhid * scale_fact), static_cast<int>(yhid * scale_fact), static_cast<int>(zhid * scale_fact));

}

void PLYHandler::setDeltas(void)
{
	vtxDelta = pView->dcel0->getVertexCount();
	edgeDelta = pView->dcel0->getHalfEdgeCount();
}

size_t PLYHandler::getElements(void)
{
	size_t lineIdx = 0, lineLmt = fileStrings.size(), wordsOnLine = 0;
	string word1, line, eMsg;
	vector<string> parsed_line;
	int keyIdx = 0;
	bool foundFirst = false, foundFormat = false, foundLast = false, 
		inRange = lineIdx < lineLmt, validLine = false;
	ply_enum peGE;
	while (inRange && !foundLast)
	{
		line = fileStrings[lineIdx];
		parsed_line = ParseByWhiteSpace(line);
		wordsOnLine = parsed_line.size();
		peGE = plyNoError;
		if (wordsOnLine > 0)
		{
			word1 = parsed_line[0];
			keyIdx = headerFirst(word1);
			validLine = validateHeaderLine(line, keyIdx);
			if (!validLine) setError(peGE, badplyHeader, lineIdx, line);
			if (peGE == plyNoError)
			{ // keyIdx: 0=ply 1=format 2=comment 3=element 4=property 5=end_header
				if (lineIdx == 0)
				{
					foundFirst = keyIdx == 0;
					if (!foundFirst) setError(peGE, wrongplyFile, lineIdx, line);
				}
				else
				{
					if (keyIdx == 0) peGE = badplyOrder;
					if (keyIdx == 1)
					{
						if (foundFormat) peGE = badplyOrder;
						else foundFormat = true;
					}
					if (keyIdx == 3)
					{
						if (wordsOnLine == 3)
						{
							PLYElementType PET;
							PET.e_name = parsed_line[1];
							if (goodInteger(parsed_line[2], 5)) 
							{
								PET.N = static_cast<size_t>(stoul(parsed_line[2]));
								elmnts.push_back(PET);
							}
							else setError(peGE, badplyData, lineIdx, line);

						}
						else setError(peGE, badplyWordsPerLine, lineIdx, line);
					}
					if (keyIdx == 4)
					{
						if ((wordsOnLine < 3) || (elmnts.size() == 0)) wordsOnLine < 3? 
							setError(peGE, badplyWordsPerLine, lineIdx, line): 
							setError(peGE, badplyOrder, lineIdx, line);
						else
						{
							int typeIdx = headerType(parsed_line[1]), listIdx = 0;
							string varName;
							if (typeIdx < 0) setError(peGE, badplyUnknownDataType, lineIdx, line);
							else
							{
								if (typeIdx < 8)
								{ // scaler
									if (wordsOnLine == 3) varName = parsed_line[2];
									else setError(peGE, badplyWordsPerLine, lineIdx, line);
								}
								else
								{ // list
									if (wordsOnLine == 5)
									{
										listIdx = headerType(parsed_line[3]);
										if ((listIdx < 0) || (listIdx > 7)) setError(peGE, badplyUnknownDataType, lineIdx, line);
										else varName = parsed_line[4];
									}
									else setError(peGE, badplyWordsPerLine, lineIdx, line);
								}
							}
							if (peGE == plyNoError)
							{
								PLYPropertyType PT;
								PT.name = varName;
								PT.tIdx = typeIdx;
								PT.tIdxList = listIdx;
								PLYElementType & rE = elmnts[elmnts.size() - 1];
								rE.props.push_back(PT);
							}
						}
					}
					if (keyIdx == 5) foundLast = true;
				}
			}
			
			if (peGE != plyNoError) 
			{
				PE = peGE;
				handleError(eMsg);
				return 0;
			}
		}
		++lineIdx;
		inRange = lineIdx < lineLmt;
	}
	if (foundFirst && foundLast && foundFormat) return lineIdx;
	return 0;

}

void PLYHandler::ShowFile(const size_t & fromIdx, const size_t & toIdx)
{
	cout << "Text from file " << fileName << "\n";
	for (size_t i = fromIdx; i < toIdx; ++i) cout << to_string(i) << ") " << fileStrings[i] << "\n";

}

void PLYHandler::ShowBBs(void)
{
	string msg, m1, m2, m3;
	m1 = "floating point bounding box is\n\rLO: (" +
		to_string(xyzLO.x) +", " + to_string(xyzLO.y) + ", " + to_string(xyzLO.z) +
		")  HI: (" + 
		to_string(xyzHI.x) +", " + to_string(xyzHI.y) + ", " + to_string(xyzHI.z) +
		")\n\n";

	size_t xloi = xyzs[0]->min, 
		yloi = xyzs[1]->min, 
		zloi = xyzs[2]->min, 
		xhii = xyzs[0]->max, 
		yhii = xyzs[1]->max, 
		zhii = xyzs[2]->max;

	m2 = "the billion box is\nLO: (" +
		to_string(xyzs[0]->tmps[xloi][0].integerData) + ", " + 
		to_string(xyzs[1]->tmps[yloi][0].integerData) + ", " + 
		to_string(xyzs[2]->tmps[zloi][0].integerData) +  
		")  HI: (" +
		to_string(xyzs[0]->tmps[xhii][0].integerData) + ", " + 
		to_string(xyzs[1]->tmps[yhii][0].integerData) + ", " + 
		to_string(xyzs[2]->tmps[zhii][0].integerData) + ")\n" +
		"which should be equal to:\nlo: (" +
		to_string(pView->bb.LO.X) + ", " +
		to_string(pView->bb.LO.Y) + ", " +
		to_string(pView->bb.LO.Z) + ") hi: (" +
		to_string(pView->bb.HI.X) + ", " +
		to_string(pView->bb.HI.Y) + ", " + 
		to_string(pView->bb.HI.Z) + ")\n\n" +
		"Scale Factor: " + commatize(to_string(scale_fact)) + "\n";

	xloi += vtxDelta; yloi += vtxDelta; zloi += vtxDelta;
	xhii += vtxDelta; yhii += vtxDelta; zhii += vtxDelta;

	CompGeo::TPoint<int> xlo = *(pView->dcel0->getPoint(xloi)), 
		ylo = *(pView->dcel0->getPoint(yloi)), 
		zlo = *(pView->dcel0->getPoint(zloi)), 
		xhi = *(pView->dcel0->getPoint(xhii)), 
		yhi = *(pView->dcel0->getPoint(yhii)), 
		zhi = *(pView->dcel0->getPoint(zhii));

	m3 = "points are\n\txLO[" + to_string(xloi) +"](" +
		to_string(xlo.xyz.X) + ", " + to_string(xlo.xyz.Y) + ", " + to_string(xlo.xyz.Z) + 
		")\n\tyLO[" + to_string(yloi) + "](" +
		to_string(ylo.xyz.X) + ", " + to_string(ylo.xyz.Y) + ", " + to_string(ylo.xyz.Z) + 
		")\n\tzLO[" + to_string(zloi) + "](" +
		to_string(zlo.xyz.X) + ", " + to_string(zlo.xyz.Y) + ", " + to_string(zlo.xyz.Z) + 
		")\n\n\r\txHI[" + to_string(xhii) + "](" +
		to_string(xhi.xyz.X) + ", " + to_string(xhi.xyz.Y) + ", " + to_string(xhi.xyz.Z) + 
		")\n\tyHI[" + to_string(yhii) + "](" +
		to_string(yhi.xyz.X) + ", " + to_string(yhi.xyz.Y) + ", " + to_string(yhi.xyz.Z) + 
		")\n\tzHI[" + to_string(zhii) + "](" +
		to_string(zhi.xyz.X) + ", " + to_string(zhi.xyz.Y) + ", " + to_string(zhi.xyz.Z) + 
		")\n\n";
	
	msg = m1 + m2 + m3;

	cout << msg;
}

void PLYHandler::HandleUnTwinned(void)
{
	size_t uCount = 0, tSz = twins.size();
	string msgs = "";
	unHalfIdx.clear();
	//cout << "Untwinned halfedges in doubly connected edge list:\n";
	//cout << "in twins:" << to_string(twins.size()) << "\n";
	for (map<CompGeo::ITYPE, map<CompGeo::ITYPE, vector<CompGeo::ITYPE>>>::const_iterator tit = twins.begin(); tit != twins.end(); ++tit)
	{
		const CompGeo::ITYPE & AID = tit->first;
		CompGeo::TPoint<int> A = *(pView->dcel0->getPoint(AID));
		const map<CompGeo::ITYPE, vector<CompGeo::ITYPE>> & HLU = tit->second;
		//cout << "\tin half lookup for A:" << to_string(HLU.size()) << "\n";
		for(map<CompGeo::ITYPE, vector<CompGeo::ITYPE>>::const_iterator hit = HLU.begin(); hit != HLU.end(); ++hit)
		{
			const CompGeo::ITYPE & BID = hit->first;
			CompGeo::TPoint<int> B = *(pView->dcel0->getPoint(BID));
			const vector<CompGeo::ITYPE> & hPossibles = hit->second;
			//cout << "\t\tin dups container for B:" << to_string(hPossibles.size()) << "\n";
			for (vector<CompGeo::ITYPE>::const_iterator pit = hPossibles.begin(); pit != hPossibles.end(); ++pit)
			{
				const CompGeo::ITYPE & hIdx = *pit;
				string msg = "\thalf[" + to_string(hIdx) + "] from A[" + to_string(AID) + "]:(" +
					to_string(A.xyz.X) + ", " + to_string(A.xyz.Y) + ", " + to_string(A.xyz.Z) +
					") to B[" + to_string(BID) + "]:(" +
					to_string(B.xyz.X) + ", " + to_string(B.xyz.Y) + ", " + to_string(B.xyz.Z) +
					")\n";
				msgs += msg;
				++uCount;
				unHalfIdx.push_back(hIdx);
			}
		}
	}
	if (uCount > 0) cout << "UNTWINNED HALFEDGES:\n" << msgs;
	cout << "Number of untwinned halfedges: " << to_string(uCount) << "\n";
	cout << "Number of duplicate halfedges - watch out for incorrect face crossing: " << to_string(duplicateEdges) << "\n";
	unattachedEdges = uCount;
}

void PLYHandler::ShowCloudCounts(void)
{
	size_t vC = pView->dcel0->getVertexCount(), vS = sizeof(CompGeo::TPoint<int>) + sizeof(CompGeo::pTPoint<int>), 
		vP = sizeof(CompGeo::pTPoint<int>) * pView->dcel0->getVertexCapacity(), vT = vC * vS + vP,
		fC = pView->dcel0->getFaceCount(), fS = sizeof(CompGeo::FaceType) + sizeof(CompGeo::pFaceType), 
		fP = sizeof(CompGeo::pFaceType) * pView->dcel0->getFaceCapacity(), fT = fC * fS + fP,
		bC = pView->bCld.size(), bS = sizeof(VID::AABB<int>), bP = bS * pView->bCld.capacity(), bT = bP,
		hC = pView->dcel0->getHalfEdgeCount(), hS = sizeof(CompGeo::HalfEdgeType) + sizeof(CompGeo::pHalfEdgeType), 
		hP = sizeof(CompGeo::pHalfEdgeType) * pView->dcel0->getHalfEdgeCapacity(), hT = hC * hS + hP, tT = vT + fT + bP + hT;

	string msg = "the vertex count in the DCEL is\t\t\t" + to_string(vC) +
	"\nthe sizeof a vertex and its pointer is\t\t" + to_string(vS) +
	"\nthe vertex vector is using\t\t\t" + to_string(vP) + 
	"\nthe vertex total is\t\t\t\t" + to_string(vT) +  
	"\n\nthe face count in the DCEL is\t\t\t" + to_string(fC) + 
	"\nthe sizeof a face and its pointer is\t\t" + to_string(fS) +
	"\nthe face vector is using\t\t\t" + to_string(fP) + 
	"\nthe face total is\t\t\t\t" + to_string(fT) +
	"\n\nthe box count in the ViewInt is\t\t\t" + to_string(bC) +
	"\nthe sizeof a box is\t\t\t\t" + to_string(bS) + 
	"\nthe box vector is using\t\t\t\t" + to_string(bP) +
	"\nthe box total is\t\t\t\t" + to_string(bT) + 
	"\n\nthe halfedge count in the DCEL is\t\t" + to_string(hC) + 
	"\nthe sizeof a halfedge and its pointer is\t" + to_string(hS) + 
	"\nthe halfedge vector is using\t\t\t" + to_string(hP) + 
	"\nthe halfedge total is\t\t\t\t" + to_string(hT) + 
	"\n\n\n\tGRAND TOTAL: " + commatize(to_string(tT)) + " bytes\n";

	cout << "ALLOCATED ON THE HEAP:\n" << msg << "\n";


}

size_t PLYHandler::getVertexList(PLYElementType & emt, const size_t & lineIdxTop)
{
	size_t lineIdx = 0, lineLmt = fileStrings.size(), 
		wordsOnLine = 0, propsInElement = emt.props.size(),
		parsedIdx = 0, delta = 0;
	if ((lineIdxTop + emt.N) >= lineLmt)
	{
		PE  = indexplyOOR;
		handleError("specified vertex list length exceeds remaining lines in file");
		return 0;
	}
	string line;
	vector<string> parsed_line;
	for (size_t i = 0; i < emt.N; ++i)
	{
		lineIdx = i + lineIdxTop; 
		line = fileStrings[lineIdx];
		//cout << to_string(lineIdx) << ") " << line << "\n"; 
		parsed_line = ParseByWhiteSpace(line);
		wordsOnLine = parsed_line.size();
		if (wordsOnLine < propsInElement) 
		{
			PE = badplyWordsPerLine;
			handleError("specified number of properties exceeds words on line");
			return 0;
		}
		parsedIdx = 0;
		for (size_t j = 0; j < propsInElement; ++j)
		{
			if ((parsedIdx + j) >= wordsOnLine)
			{
				PE = badplyWordsPerLine;
				handleError("specified number of entries (as in a list) exceeds words on line");
				return 0;
			}
			PLYPropertyType & PT = emt.props[j];
			if (PT.tIdx < 8)
			{ // scaler type
				if (!AddValue(parsed_line[j], PT, i))
				{
					PE = badplyData;
					handleError("signed/unsigned integer or floating point problem");
					return 0;
				}
			}
			else
			{ // list type
				vector<string> toEnd;
				for (size_t k = parsedIdx; k < wordsOnLine; ++k) 
				{
					toEnd.push_back(parsed_line[k]);
				}
				delta = AddValue(toEnd, PT);
				if (delta == 0)
				{
					PE = badplyData;
					handleError("signed/unsigned integer or floating point problem in list");
					return 0;
				}
				else parsedIdx += delta;
			}
		}
	}
	return emt.N;

}

void PLYHandler::storeVertices(const PLYElementType & emt)
{
	for (CompGeo::ITYPE i = 0; i < static_cast<CompGeo::ITYPE>(emt.N); ++i)
	{
		CompGeo::ITYPE dcelIdx = i + vtxDelta;
		PLYTempValueType & xt = xyzs[0]->tmps[i][0],
			& yt = xyzs[1]->tmps[i][0],
			& zt = xyzs[2]->tmps[i][0];
		bool xb = xyzs[0]->tIdx > 5,
			yb = xyzs[1]->tIdx > 5,
			zb = xyzs[2]->tIdx > 5;
		double xd = xb? xt.floatPointData: static_cast<double>(xt.integerData),
			yd = yb? yt.floatPointData: static_cast<double>(yt.integerData),
			zd = zb? zt.floatPointData: static_cast<double>(zt.integerData);
		xt.integerData = static_cast<int>(xd * scale_fact);
		yt.integerData = static_cast<int>(yd * scale_fact);
		zt.integerData = static_cast<int>(zd * scale_fact);

		CompGeo::pTPoint<int> vPt = new CompGeo::TPoint<int>(xt.integerData, 
			yt.integerData, zt.integerData);
		
		vPt->vertex = dcelIdx;
		CompGeo::ITYPE vtx = pView->dcel0->push(vPt);
		assert (vtx == dcelIdx);

	}

}

size_t PLYHandler::getFaceList(PLYElementType & emt, const size_t & lineIdxTop)
{
	int vLstIdx = findVertexList(emt);	
	bool foundVLst = vLstIdx >= 0;
	if (!foundVLst) 
	{
		PE = badplyHeader;
		handleError();
		return 0;
	}
	size_t lineIdx = 0, lineLmt = fileStrings.size(), 
		wordsOnLine = 0, propsInElement = emt.props.size(),
		parsedIdx = 0, delta = 0,
		lSz = emt.N, vLAt = static_cast<size_t>(vLstIdx);
	
	CompGeo::ITYPE vIdx = 0, hIdx = 0;

	if ((lineIdxTop + emt.N) >= lineLmt)
	{
		PE  = indexplyOOR;
		handleError("specified number of faces exceeds remaining lines in file");
		return 0;
	}
	string line;
	vector<string> parsed_line;
	for (size_t i = 0; i < emt.N; ++i)
	{
		lineIdx = i + lineIdxTop;
		line = fileStrings[lineIdx];
		parsed_line = ParseByWhiteSpace(line);
		wordsOnLine = parsed_line.size();
		if (wordsOnLine < propsInElement) 
		{
			PE = badplyWordsPerLine;
			handleError("specified number of properties exceeds words on line\n" + to_string(lineIdx) + ") " + line);
			return 0;
		}
		parsedIdx = 0;
		for (size_t j = 0; j < propsInElement; ++j)
		{
			if ((parsedIdx + j) >= wordsOnLine)
			{
				PE = badplyWordsPerLine;
				handleError("specified number of list entries exceeds remaining words on line\n" + to_string(lineIdx) + ") " + line);
				return 0;
			}
			PLYPropertyType & PT = emt.props[j];
			if (PT.tIdx < 8)
			{ // scaler type
				if (!AddValue(parsed_line[j], PT, i))
				{
					PE = badplyData;
					handleError("signed/unsigned integer or floating point problem\n" + to_string(lineIdx) + ") " + line);
					return 0;
				}
			}
			else
			{ // list type
				vector<string> toEnd;
				for (size_t k = parsedIdx; k < wordsOnLine; ++k) 
				{
					toEnd.push_back(parsed_line[k]);
				}
				delta = AddValue(toEnd, PT);
				if (delta == 0)
				{
					PE = badplyData;
					handleError("signed/unsigned integer or floating point problem in list\n" + to_string(lineIdx) + ") " + line);
					return 0;
				}
				else 
				{
					parsedIdx += delta;
					if (j == vLAt)
					{ // this is the vertex list
						vector<PLYTempValueType> v_s = PT.tmps[i]; // all integer
						if (v_s.size() < 3)
						{
							PE = badplyPolygonVertices;
							handleError(to_string(lineIdx) + ") " + line);
							return 0;
						}
						vector<CompGeo::pTPoint<int>> v;
						CompGeo::TPoint<int> lo, hi;
						vector<CompGeo::pHalfEdgeType> h;
						CompGeo::pHalfEdgeType phBuff = NULL, phPrev = NULL;
						vector<CompGeo::XYZ> e;
						CompGeo::XYZ e01, e02, N;
						VID::AABB<int> bb;
						//vector<size_t> vtx_idxs, h_idxs;
						for (size_t tmpIdx = 0; tmpIdx < v_s.size(); ++tmpIdx)
						{
							PLYTempValueType vt = v_s[tmpIdx];
							vIdx = vtxDelta + static_cast<CompGeo::ITYPE>(vt.integerData);
							CompGeo::pTPoint<int> vBuff = pView->dcel0->getPoint(vIdx);
							//vtx_idxs.push_back(vIdx);
							v.push_back(vBuff);
							e.push_back(CompGeo::XYZ(vBuff->xyz));
							phBuff = new CompGeo::HalfEdgeType;
							hIdx = pView->dcel0->push(phBuff);
							phBuff->origin = vIdx;
							phBuff->halfedge = hIdx;
							if (vBuff->incidentedge == 0) vBuff->incidentedge = hIdx;
							h.push_back(phBuff);
							if (phPrev == NULL)
							{
								lo = *vBuff; hi = *vBuff;
							}
							else
							{
								phPrev->next = hIdx;
								phBuff->prev = phPrev->halfedge;
								for (int k = 0; k < 3; ++k)
								{
									if (vBuff->dims[k] < lo.dims[k]) lo.dims[k] = vBuff->dims[k];
									if (vBuff->dims[k] > hi.dims[k]) hi.dims[k] = vBuff->dims[k];
								}
							}
							phPrev = phBuff;
						}
						phBuff = h[0];
						phPrev->next = phBuff->halfedge;
						phBuff->prev = phPrev->halfedge;
						CompGeo::pFaceType pface = new CompGeo::FaceType;
						CompGeo::ITYPE fIdx = pView->dcel0->push(pface);
						pface->face = fIdx;
						pface->outer = phBuff->halfedge;
						bb.objID = fIdx;
						bb.BB.LO = lo.xyz;
						bb.BB.HI = hi.xyz;
						pView->push(bb);
						e01 = e[1] - e[0];
						e02 = e[2] - e[0];
						N = CompGeo::Cross(e01, e02);
						N /= N.GetMagnitude();
						pface->norm = N;
					}
				}
			}
		}
	}
	return emt.N;

}

//void PLYHandler::storeFaces(const PLYElementType & emt)
//{

//}

size_t PLYHandler::skipUnknownList(const PLYElementType & emt, const size_t & lineIdxTop)
{
	return emt.N;
}

void PLYHandler::handleError(string additionalTxt)
{
	string msg = plyErrs[PE] + "\n" + additionalTxt;
	size_t peLength = msg.size() + 1;
	plyErr.addError(msg.c_str(), peLength);
	plyErr.displayError();
}

void PLYHandler::LoadPLY(void)
{
	ClearBuffers();
	ifstream fIn;
	fIn.open(fileName, ifstream::in);
	string fLine;
	while (!fIn.eof())
	{
		getline(fIn, fLine);
		fileStrings.push_back(fLine);
	}
	fIn.close();

	bool gotVertices = false, gotFaces = false;

	size_t fileStringsIdx = 0, 
		IdxDelta = getElements();

	if (IdxDelta == 0)
	{ // getElements got nothing
		ClearBuffers();
		return;
	}
	ShowFile(0, IdxDelta); // this will print the header
	fileStringsIdx += IdxDelta;
	for (vector<PLYElementType>::iterator eit = elmnts.begin(); eit != elmnts.end(); ++eit)
	{
		PLYElementType & e = *eit;
		if (e.e_name.compare("vertex") == 0)
		{
			cout << "Found vertex element: extracting " << to_string(e.N) << " vertices.\n...\n";
			gotVertices = true;
			IdxDelta = getVertexList(e, fileStringsIdx);
			if (IdxDelta == 0)
			{
				ClearBuffers();
				return;
			}
			cout << "Finished Extracting vertices.\n";
			postVertexExtractionProcessing(e);
			if (PE != plyNoError)
			{
				ClearBuffers();
				return;
			}
			storeVertices(e);
			cout << "Finshed Storing vertices in the DCEL.\n\n";
			updateStatus("Extracted " + to_string(e.N) + " vertices");
		}
		else if (e.e_name.compare("face") == 0)
		{
			cout << "Found face element: extracting " << to_string(e.N) << " faces.\n...\n";
			if (!gotVertices)
			{
				PE = badplyHeader;
				handleError();
				return;
			}
			gotFaces = true;
			IdxDelta = getFaceList(e, fileStringsIdx);
			if (IdxDelta == 0)
			{
				ClearBuffers();
				return;
			}
			cout << "Finshed Extracting And Storing " << to_string(e.N) << " faces.\nAm now hooking up the twins.\n";
			getAllTwins();
			cout << "Finished hooking up the twins.\n";
			updateStatus("Extracted " + to_string(e.N) + " faces. Unattached edges: " + to_string(unattachedEdges));
		}
		else
		{
			IdxDelta = skipUnknownList(e, fileStringsIdx);
			if (IdxDelta == 0)
			{
				ClearBuffers();
				return;
			}
		}
		fileStringsIdx += IdxDelta;
	}
	if (!(gotFaces && gotVertices))
	{
		PE = badplyHeader;
		handleError();
	}
	ShowBBs();
	ShowCloudCounts();
	HandleUnTwinned();
}

char PLYHandler::GetEndianess(void)
{ // checks this computer; returns 'B' or 'L' for big_endian OR little_endian respectively

	PLYEndianType eTest;
	char f_b[4] = { (char)0x3F, (char)0x80, (char)0x00, (char)0x01};
	eTest.f = (float)1.00000011920929; // about 1 + 2^-23
	bool le = true, be = true;
	for (int i = 0; i < 4; ++i)
	{
		le = le && (f_b[i] == eTest.b[i]);
		be = be && (f_b[i] == eTest.b[3 - i]);
	}
	assert(le || be);
	if (le) return 'L';
	return 'B';
}

/*
void PLYHandler::GetBIH(void)
{
	//bbScaled = BIH::AABBbyIndex<int>(bbIdx, *pCld);
	BIH::BIH<int> bih(&pView->bb, pView, 6);
	//CompGeo::AABBBasic<int> bb;
	//BIH::BIH<int> bih(pView->bbIdx, bb, pView->pCld, pView->bCld, 6);
	bih.ShowTreeCounts();
}
*/
/*
// AddEdge takes 2 indices into pCld & 1 into hCld:
void PLYHandler::AddEdge(const unsigned int &pIdx1, const unsigned int &pIdx2, const unsigned int &hIdx)
{
	EdgeFromPoints * efpFromHere = new EdgeFromPoints(pIdx1, pIdx2), *efpFromAVL = efpFromHere;
	pUINTListHeadType plht = NULL;
	pUINTListType plt = (pUINTListType)::operator new(sizeof(UINTListType));
	plt->Idx = hIdx;
	plt->next = NULL;
	if (twins.Insert(efpFromAVL))
	{ // insert succeeded efpFromAVL is eftFromHere
		plht = (pUINTListHeadType)::operator new(sizeof(UINTListHeadType));
		plht->count = 1;
		plht->ul = plt;
		efpFromAVL->hEdge = plht;
	}
	else
	{ // insert failed efpFromAVL is from the tree (not efpFromHere)
		delete efpFromHere;
		plht = efpFromAVL->hEdge;
		++(plht->count);
		plt->next = plht->ul;
		plht->ul = plt;
	}
}
*/
// getHalfEdgeList takes 2 indices into pCld:
/*
pUINTListHeadType PLYHandler::getHalfEdgeList(const unsigned int &pIdx1, const unsigned int &pIdx2)
{
	EdgeFromPoints * efpFromHere = new EdgeFromPoints(pIdx1, pIdx2), *efpFromAVL = NULL;
	pUINTListHeadType plht = NULL;
	CompGeo::AVLNode<EdgeFromPoints> * node = twins.Find(efpFromHere);
	delete efpFromHere;
	if (node == NULL) return NULL;
	efpFromAVL = node->Data;
	return efpFromAVL->hEdge;
}


map<size_t, VID::HalfTwinLUType>::iterator PLYHandler::getHalfEdgeList(const size_t & hID)
{
	return twins.find(hID);
}

unsigned int PLYHandler::getTwin(CompGeo::pHalfEdgeType & h) 
{
//	CompGeo::pHalfEdgeType h = &((*pView->hCld)[hIdx]);
	CompGeo::pHalfEdgeType h_n = pView->dcel0->next(h);
	size_t BID = h_n->origin;
	unsigned int pIdx1 = h->origin, 
		pIdx2 = (*pView->hCld)[h->next].origin,
		hIdx = h->halfedge;
	pUINTListHeadType plht = getHalfEdgeList(pIdx1, pIdx2);
	assert(plht != NULL);
	if (plht->count != 2)
	{
		h->flag = 'u'; // untwinned
		++unattachedEdges;
		return 0;  // this is an error:
	}
	pUINTListType plt = plht->ul;
	for (unsigned int i = 0; i < plht->count; ++i)
	{
		if (plt->Idx != hIdx) return plt->Idx;
		plt = plt->next;
	}
	assert(1 == 2); // error here
	return 0;
}
*/

void PLYHandler::getAllTwins(void)
{
	size_t hCount = pView->dcel0->getHalfEdgeCount();
	CompGeo::ITYPE AID = 0, BID = 0, hID = 0, tID = 0;
	duplicateEdges = 0;
	dupHalfIdx.clear();
	CompGeo::pHalfEdgeType h = NULL, t = NULL, hn = NULL;
	for (hID = edgeDelta; hID < static_cast<CompGeo::ITYPE>(hCount); ++hID)
	{ 
		h = pView->dcel0->getHalfEdge(hID);
		assert(h->halfedge == hID);
		AID = h->origin;
		hn = pView->dcel0->next(); 
		BID = hn->origin;

		map<CompGeo::ITYPE, map<CompGeo::ITYPE, vector<CompGeo::ITYPE>>>::iterator titB = twins.find(BID);
		if (titB == twins.end()) addTwinLookup(hID, AID, BID); 
		else
		{
			map<CompGeo::ITYPE, vector<CompGeo::ITYPE>> & hLU = titB->second;
			map<CompGeo::ITYPE, vector<CompGeo::ITYPE>>::iterator hit = hLU.find(AID);
			if (hit == hLU.end()) addTwinLookup(hID, AID, BID);
			else
			{
				vector<CompGeo::ITYPE> & tPossibles = hit->second;
				tID = tPossibles[tPossibles.size() - 1];
				tPossibles.pop_back();
				if (tPossibles.size() == 0)
				{
					hLU.erase(hit);
					if (hLU.size() == 0) twins.erase(titB);
				}
				t = pView->dcel0->getHalfEdge(tID);
				t->twin = hID;
				h->twin = tID;
				//cout << "TWINNED: half=" << to_string(hID) << " AID=" << to_string(AID) << " BID=" << to_string(BID) << "\n"; 
			}
		}
	}
}

void PLYHandler::addTwinLookup(const CompGeo::ITYPE & hID, const CompGeo::ITYPE & AID, const CompGeo::ITYPE & BID)
{ // half should not be present in twins when this is called

	map<CompGeo::ITYPE, map<CompGeo::ITYPE, vector<CompGeo::ITYPE>>>::iterator titA = twins.find(AID);
	if (titA == twins.end())
	{
		vector<CompGeo::ITYPE> hPossibles;
		hPossibles.push_back(hID);
		map<CompGeo::ITYPE, vector<CompGeo::ITYPE>> hLU;
		hLU[BID] = hPossibles;
		twins[AID] = hLU;
		//cout << "\tt&lu hID:" << to_string(hID) << " AID:" << to_string(AID) << " BID:" << to_string(BID) << "\n";
	}
	else
	{
		map<CompGeo::ITYPE, vector<CompGeo::ITYPE>> & hLU = titA->second;
		map<CompGeo::ITYPE, vector<CompGeo::ITYPE>>::iterator pit = hLU.find(BID);
		if (pit == hLU.end())
		{
			vector<CompGeo::ITYPE> hPossibles;
			hPossibles.push_back(hID);
			hLU[BID] = hPossibles;
		}
		else
		{
			++duplicateEdges;
			vector<CompGeo::ITYPE> & hPossibles = pit->second;
			hPossibles.push_back(BID);
			for (vector<CompGeo::ITYPE>::iterator it = hPossibles.begin(); it != hPossibles.end(); ++it)
			{
				CompGeo::ITYPE bVtxIdx = *it;
				dupHalfIdx[bVtxIdx] = AID; // will store all the B vertex indices from hPossibles uniquely
			}
			// note: twins are handled JIT 
			// when found they are processed and removed from container twins
			// as a result the real "duplicate"(s) may already have been erroneously assigned and cleared
		}
	}


}

bool PLYHandler::AddValue(const string & s, PLYPropertyType & p, const size_t & lIdx)
{ // for scalers
	PLYTempValueType v, vmin, vmax;
	size_t vSz = p.tmps.size();
	bool minSet = vSz > p.min, maxSet = vSz > p.max;
	if (minSet) vmin = p.tmps[p.min][0];
	if (maxSet) vmax = p.tmps[p.max][0];
	vector<PLYTempValueType> vec;

	bool goodVal = true;
	assert(p.tIdx < 8); // handle lists differently

	if (p.tIdx < 6)
	{
		v.integerData = stol(s); goodVal = goodInteger(s, p.tIdx); 
		if (minSet) 
		{
			if (v.integerData < vmin.integerData) p.min = lIdx;
		}	
		if (maxSet)
		{
			if (v.integerData > vmax.integerData) p.max = lIdx;
		}
	}
	else
	{
		v.floatPointData = stod(s); goodVal = goodFloatingPoint(s); 
		if (minSet)
		{
			if (v.floatPointData < vmin.floatPointData) p.min = lIdx;
		}
		if (maxSet)
		{
			if (v.floatPointData > vmax.floatPointData) p.max = lIdx;
		}

	}
	vec.push_back(v);
	p.tmps.push_back(vec);
	return goodVal;
}

bool PLYHandler::goodInteger(const string & s, const unsigned int & tIDX)
{
	//string withsign = "[+-]?", digits = "[0-9]+", x = digits;
	//if (0 == (tIDX % 2)) x = withsign + digits;
	bool signedIntType = (tIDX %2) == 0;
	//regex rx(x);
	return regex_match(s, signedIntType? signedIntRX: unsignedIntRX);
}

bool PLYHandler::goodFloatingPoint(const string & s)
{
	//regex rx("[+-]?([0-9]*[.])?[0-9]+([E|e][+-]?[0-9]+)?");
	return regex_match(s, floatingPointRx);
}

bool PLYHandler::isVertexList(const string & s, const unsigned int & t, const unsigned int & l)
{
	if (t != 8) return false; //list
	if (l > 5) return false; // integer type

	//const vector<string> & rxsBegin = {	"^vertices\\S+$", "^vertex\\S+$", "^vtx\\S+$"},
	//	& rxsEnd = { "^\\S+index$", "^\\S+indices$", "^\\S+lst$", "^\\S+list$"};

	for (size_t i = 0; i < vLstBeginRx.size(); ++i)
	{
		//regex rxB(rxsBegin[i], regex_constants::icase);
		bool goodBegin = regex_match(s, vLstBeginRx[i]);
		if (goodBegin)
		{
			if (i == 0) return true;
			for (size_t j = 0; j < vLstEndRx.size(); ++j)
			{
				//regex rxE(rxsEnd[j], regex_constants::icase);
				bool goodEnd = regex_match(s, vLstEndRx[j]);
				if (goodEnd) return true;
			}
		}
	}	 
	return false;
}

int PLYHandler::findVertexList(const PLYElementType & emt)
{
	for (size_t i = 0; i < emt.props.size(); ++i)
	{
		const PLYPropertyType & PT = emt.props[i];
		if (isVertexList(PT.name, PT.tIdx, PT.tIdxList)) return static_cast<int>(i);
	}
	return -1;
}

size_t PLYHandler::AddValue(const vector<string> & s, PLYPropertyType & p)
{ // for lists. vector in will be all non-whitespace matches on the list line 
  // at and after the list length, an unsigned int type 
  // there could by more data on the line than this list alone
	assert (p.tIdx == 8);
	size_t sSz = s.size();
	if (sSz < 2) return 0; // not enough words in line
	vector<PLYTempValueType> vec;
	PLYTempValueType v;
	bool goodVal  = goodInteger(s[0], 1);
	if (!goodVal) return 0;  // must be unsigned integer type
	size_t lSz = stol(s[0]);
	++lSz;
	if (lSz > sSz) return 0; // not enough words in line
	bool isIntType = p.tIdxList < 6;
	for (size_t i = 1; i < lSz; ++i)
	{
		if (isIntType)
		{
			goodVal = goodInteger(s[i], p.tIdxList);
			v.integerData = stol(s[i]);
		}
		else
		{
			goodVal = goodFloatingPoint(s[i]);
			v.floatPointData = stod(s[i]);
		}
		if (!goodVal) return 0;
		vec.push_back(v);
	}
	p.tmps.push_back(vec);
	return lSz;
}

vector<string> PLYHandler::ParseByWhiteSpace(string s)
{
	//regex rx("\\S+");
	smatch sm;
	vector<string> r;
	while(regex_search(s, sm, nonWSRx))
	{
		r.push_back(sm[0]);
		s = sm.suffix().str();
	}
	return r;
}

int PLYHandler::headerFirst(const string & str1)
{
	//smatch sm = ParseByWhiteSpace(hStr);
	//assert(sm.size() > 0);
	//const vector<string> & keywords = { "ply", "format", "comment", "element", "property", "end_header" };
	int i = 0;
	for (i = 0; i < hdrKeys.size(); ++i)
	{
		if (hdrKeys[i].compare(str1) == 0) return i;
	}
	return -1;

}

int PLYHandler::headerType(const string & nonWS)
{
	//const vector<string> & typewords = { "char", "uchar", "short", "ushort", "int", "uint", "float", "double", "list" };
	int i = 0;
	for (i = 0; i < hdrTypes.size(); ++i)
	{
		if (hdrTypes[i].compare(nonWS) == 0) return i;
	}
	return -1;
}

bool PLYHandler::validateHeaderLine(const string & hStr, const int & kIdx)
{
	//const vector<string> & rxs = {	
	//	"^ply$", 
	//	"^format\\s+ascii\\s+\\d+.\\d+\\s*$",
	//	"^comment", 
	//	"^element\\s+\\w+\\s+\\d+\\s*$",
	//	"^property\\s+(\\w+)|(list\\s+\\w+\\s+\\w+)\\s+\\w+\\s*$", 
	//	"^end_header\\s*$" 
	//};
	if ((kIdx < 0) || (kIdx >= hdrRx.size())) return false;
	//regex rx(rxs[kIdx]);
	return regex_search(hStr, hdrRx[kIdx]);

}

string PLYHandler::commatize(const string & bigNum)
{
	string r = "";
	size_t periodAt = bigNum.find_first_of('.'), 
		bNSz = bigNum.size(),
		i = 0;
	bool isfloat = periodAt != string::npos;
	if (isfloat) bNSz = periodAt;
	int delta = (3 - (bNSz % 3)) % 3;
	while(i < bNSz)
	{
		r += bigNum[i++];
		if ((((i + delta) % 3) == 0) && (i < bNSz)) r += ',';
	}
	if (isfloat) r += bigNum.substr(periodAt);
	return r;
}