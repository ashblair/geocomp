#include "first.hpp"
#include "CompGeo.hpp"
#include "Matrix.hpp"
#include "NGons.hpp"
#include "widgets.hpp"
#include "ViewInterface.hpp"

using namespace std;

template<typename T> size_t VID::ViewType<T>::push(const AABB<T> & aabb) 
{
    size_t aIdx = bCld.size(); 
    bCld.push_back(aabb); 
    return aIdx;
}

//size_t VID::ViewIntStructType::push(const AABB<int> & aabb) {size_t aIdx = bCld.size(); bCld.push_back(aabb); return aIdx;}


//VID::HalfTwinLUStructType::HalfTwinLUStructType(void): halfIdx(0), AIdx(0), BIdx(0), twinIdx(0) {}
//VID::HalfTwinLUStructType::HalfTwinLUStructType(const size_t & h, const size_t & A, const size_t & B): halfIdx(h), AIdx(A), BIdx(B), twinIdx(0) {}
//VID::HalfTwinLUStructType::HalfTwinLUStructType(const HalfTwinLUStructType & a): halfIdx(a.halfIdx), AIdx(a.AIdx), BIdx(a.BIdx), twinIdx(a.twinIdx) {}
//VID::HalfTwinLUStructType & VID::HalfTwinLUStructType::operator=(const HalfTwinLUStructType & a) {halfIdx = a.halfIdx; AIdx = a.AIdx; BIdx = a.BIdx; twinIdx = a.twinIdx; return *this;}
//bool VID::HalfTwinLUStructType::IsEq(const HalfTwinLUStructType & a) const {return halfIdx == a.halfIdx;}
//bool VID::HalfTwinLUStructType::IsLT(const HalfTwinLUStructType & a) const {return halfIdx < a.halfIdx;}

//helper global function to let the linker know where these templated calls must go:
void link_my_VID(void)
{
    VID::ViewType<int> vtInt; VID::AABB<int> aabbInt; size_t bbIntSz = vtInt.push(aabbInt); 
    VID::ViewType<double> vtDbl; VID::AABB<double> aabbDbl; size_t bbDblSz = vtDbl.push(aabbDbl); 
	
    //Map<int> intMap; intMap.Run(NULL); intMap.TranslateTrapezoids(); intMap.GetSitePoint(NULL);
	//Map<double> dblMap;  dblMap.Run(NULL); dblMap.TranslateTrapezoids(); dblMap.GetSitePoint(NULL);
}
