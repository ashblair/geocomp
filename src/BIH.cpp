#include "first.hpp"
#include "CompGeo.hpp"
#include "ViewInterface.hpp"
#include "BIH.hpp"

using namespace std;

template<typename T> BIH::BIH<T>::BIH(void): pBB(NULL), pvd(NULL), zm(0), nm_per_pixel(1), nodecount(0), 
    leafcount(0), zerocount(0), rcount(0), pmax(0), maxleaf(0), bbCount(0), 
    root(NULL), InDebug(false), phf(NULL) {}

// linker helper function at end of file

template<typename T> BIH::BIH<T>::BIH(VID::pAABBBasic<T> the_BB, VID::pViewType<T> vd, 
    const unsigned int & zoom_fact): pBB(the_BB), pvd(vd), zm(zoom_fact), 
    nodecount(0), leafcount(0), zerocount(0), rcount(0), pmax(0), maxleaf(0),
    bbCount(vd->bCld.size()), InDebug(false), phf(NULL)
{
    assert(pvd->dcel_view->getFaceCount() == 1);

    nm_per_pixel = 1;
    for (size_t i = 0; i < zm; ++i)
    {
        nm_per_pixel *= 10;
    }
    //BBLO = CompGeo::TPoint<T>(pBB->LO);
    //BBHI = CompGeo::TPoint<T>(pBB->HI);
 
    if (bbCount == 0) root = NULL;
    else root = BuildBIH(*pBB, 0, bbCount - 1);
    BIHSimpleTest1();
}

template<typename T> BIH::BIH<T>::~BIH(void) {clearBIH();}

template<typename T> void BIH::BIH<T>::clearBIH(void)
{
    if (root != NULL)
    {
        delSubTree(root);
        delete root;
        root = NULL;
    }

}

template<typename T> void BIH::BIH<T>::ShowTreeCounts(void)
{
    string msg = "the node count is " + to_string(nodecount) +
    "\nthe leaf count is " + to_string(leafcount) +
    "\nthe zero leaf count is " + to_string(zerocount) + 
    "\nthe max process count is " + to_string(pmax) +
    "\nthe max num/leaf is " + to_string(maxleaf) +
    "\n";
    cout << msg;
}

template<typename T> void BIH::BIH<T>::getBoxesWithin(const VID::AABBBasic<T> & qBox)
{
    memcpy(&viewBox, &qBox, sizeof(VID::AABBBasic<T>));
    //viewLO = CompGeo::TPoint<T>(viewBox.LO);
    //viewHI = CompGeo::TPoint<T>(viewBox.HI);
    rptCount = 0;  // face 0 is reserved for the view volume
    vtxCount = 0; // number of vertices in pvCld
    hedgeCount = 1; // number of halfedged in hvCld
    VID::AABBBasic<T> treeBox;
    memcpy(&treeBox, pBB, sizeof(VID::AABBBasic<T>));
    SearchBIHTree(root, treeBox);
}

template<typename T> void BIH::BIH<T>::BIHSimpleTest1(void)
{
    unsigned int num_pixels = 100;
    VID::AABBBasic<T> qBox;
    CompGeo::TPoint<T> ctr; //, HiPoint = CompGeo::TPoint<T>(qBox.HI), LoPoint = CompGeo::TPoint<T>(qBox.LO);
    for (unsigned char i = 0; i < 3; ++i)
    {
        if (i > 0)
        {
            ctr.dims[i] = (qBox.HI.dim(i) + qBox.LO.dim(i)) / static_cast<T>(2);
            qBox.HI.dim(i) = ctr.dims[i] + static_cast<T>(num_pixels * nm_per_pixel / 2);
            qBox.LO.dim(i) = ctr.dims[i] - static_cast<T>(num_pixels * nm_per_pixel / 2);
        }
        else
        {
            qBox.LO.dim(i) = static_cast<T>(-600000000); // pBB->HI.dims[i];
            qBox.HI.dim(i) = static_cast<T>(-400000000); // pBB->LO.dims[i];
        }
    }
    getBoxesWithin(qBox);// , rptSegs);

}

template<typename T> void BIH::BIH<T>::ReLoad(VID::pViewType<T> vd, 
    const unsigned int & zoom_fact)
{
    clearBIH();
    pBB = &vd->bb;// the bounding box BB;
    //BBLO = CompGeo::TPoint<T>(pBB->LO);
    //BBHI = CompGeo::TPoint<T>(pBB->HI);

    pvd = vd;
    assert(pvd->dcel_view->getFaceCount() == 1); // clear view face in calling function

    zm = zoom_fact;
    nm_per_pixel = 1;
    for (size_t i = 0; i < zm; ++i)
    {
        nm_per_pixel *= 10;
    }
    nodecount = 0;
    leafcount = 0;
    zerocount = 0;
    rcount = 0;
    pmax = 0;
    maxleaf = 0;
    bbCount = pvd->bCld.size();
    if (bbCount == 0) root = NULL;
    else root = BuildBIH(*pBB, 0, bbCount - 1);

}


template<typename T> void BIH::BIH<T>::StartFile(void)
{
    string txtBuff = "***** BEGIN BIH DEBUG *****\n";
    streamsize l = txtBuff.size() + 1;
    WriteToFile(txtBuff.c_str(), l);

}

template<typename T> void BIH::BIH<T>::EndFile(void)
{
    string txtBuff = "\\\\\\\\\\ END BIH DEBUG /////\n";
    streamsize l = txtBuff.size() + 1;
    WriteToFile(txtBuff.c_str(), l);

}

// END PUBLIC METHODS:

template<typename T> bool BIH::BIH<T>::IsContained(VID::AABBBasic<T> & treeBox)
{ //returns true iff the treeBox is contained in the viewBox
    //CompGeo::TPoint<T> treeLO = CompGeo::TPoint<T>(treeBox.LO), treeHI = CompGeo::TPoint<T>(treeBox.HI);
    for (unsigned int i = 0; i < 3; ++i)
    {
        bool IsWithin = (viewBox.LO.dim(i) <= treeBox.LO.dim(i)) &&
            (treeBox.HI.dim(i) <= viewBox.HI.dim(i));
        if (!IsWithin) return false;
    }
    return true;
}

template<typename T> char BIH::BIH<T>::intervalCompare(VID::AABBBasic<T> & treeBox, const unsigned char & axis)
{
    // A description of the intersection of the treeBox and the viewBox
    // From the viewBox's perspective as if the given sides were
    // projected to that axis and then rotated to the vertical if necessary (x or z)
    // returns:
    // 'i' for inside i.e. vLO <= tLO < tHI <= vHI
    // 'o' for outside i.e. tLO < vLO < vHI < tHI
    // 'e' for equal i.e. vLO == tLO && tHI == vHI
    // 't' for topped i.e. tLO < vLO < tHI <= vHI
    // 'c' for chop-blocked i.e. vLO <= tLO < vHI < tHI
    // 'a' for above i.e. vLO < vHI <= tLO < tHI
    // 'b' for below i.e. tLO < tHI <= vLO < vHI

    //CompGeo::TPoint<T> treeLO = CompGeo::TPoint<T>(treeBox.LO), treeHI = CompGeo::TPoint<T>(treeBox.HI);

    T vLO = viewBox.LO.dim(axis), vHI = viewBox.HI.dim(axis),
        tLO = treeBox.LO.dim(axis), tHI = treeBox.HI.dim(axis);
    char flagChar = 'e';
    if (!((vLO == tLO) && (vHI == tHI)))
    { // not equal:
        if ((vHI < tLO) || (tHI < vLO))
        { // non-intersecting:
            flagChar = vHI < tLO? 'a': 'b';
        }
        else
        { // intersecting:
            if (vLO <= tLO)
            {
                if (tHI <= vHI)
                {
                    flagChar = 'i';
                }
                else
                {
                    flagChar = 'c';
                }
            }
            else
            {
                if (tHI <= vHI)
                {
                    flagChar = 't';
                }
                else
                {
                    flagChar = 'o';
                }
            }
        }
    }
    return flagChar;
}

template<typename T> void BIH::BIH<T>::addFacesFromBoxes(const size_t BoxIdx, const unsigned int & count)
{ 
    for (unsigned int i = 0; i < count; ++i)
    {
        //VID::AABB<T> & ab = pvd->bCld[BoxIdx + i];
        CompGeo::ITYPE FID = pvd->bCld[BoxIdx + i].objID;
        CompGeo::pFaceType pf = pvd->dcel0->getFace(FID), pvf = new CompGeo::FaceType(*pf);
        // starts at 1, fvCld must have face 0 initiallized before BIH is queried:
        pf->viewface = ++rptCount;  
        CompGeo::ITYPE fvIdx = pvd->dcel_view->push(pvf);
        pvf->viewface = fvIdx;
        assert(fvIdx == rptCount);
        pvf->outer = hedgeCount;
        // maybe we should do all copying here?
        // so halfedges and vertices too?
        vector<CompGeo::pHalfEdgeType> vec_ph = pvd->dcel0->all_outer(pf);
        CompGeo::ITYPE edgeCount = static_cast<CompGeo::ITYPE>(vec_ph.size());
        for (CompGeo::ITYPE j = 0; j < edgeCount; ++j)
        {
            CompGeo::pHalfEdgeType ph = vec_ph[j], pvh = new CompGeo::HalfEdgeType(*ph);
            ph->viewedge = hedgeCount + j;
            CompGeo::ITYPE hvIdx = pvd->dcel_view->push(ph);
            ph->viewedge = hvIdx;
            assert(hvIdx == (j + hedgeCount));
            pvh->incidentface = rptCount;
            pvh->next = hedgeCount + ((j + 1) % edgeCount);
            pvh->prev = hedgeCount + ((j + edgeCount - 1) % edgeCount);
            //pvh->twin must be set in another loop
            CompGeo::pTPoint<int> pp = pvd->dcel0->getPoint(ph->origin), pvp = new CompGeo::TPoint<int>(*pp);
            if (pp->viewvertex == 0) // add only once
            {
                pp->viewvertex = ++vtxCount;
                CompGeo::ITYPE pvIdx = pvd->dcel_view->push(pvp);
                pvp->viewvertex = pvIdx;
                assert(pvIdx == vtxCount);
                pvp->incidentedge = hedgeCount + j;
            }
            pvh->origin = pp->viewvertex;
        }
        hedgeCount += edgeCount;
    }
}

template<typename T> void BIH::BIH<T>::addAllInSubTree(const pBIH_Node<T> & st)
{
    if (st == NULL) return;
    if ((st->index & 0x3) == 0x3) // leaf
    {
        size_t * l = reinterpret_cast<size_t *>(st->index & 0xFFFFFFFFFFFFFFFC);
        //addCloudSegment(*l, st->items);
        addFacesFromBoxes(*l, st->items);
        return;
    }
    pBIHPointerType<T> p =
        reinterpret_cast<pBIHPointerType<T>>(st->index & 0xFFFFFFFFFFFFFFFC);
    pBIH_Node<T> l = p->left, r = p->right;
    addAllInSubTree(l);
    addAllInSubTree(r);
}

template<typename T> void BIH::BIH<T>::SearchBIHTree(const pBIH_Node<T> & st, VID::AABBBasic<T> & treeBox)
{
    if (st == NULL) return;
    unsigned char a = st->index & 0x3;
    if (a == 0x3) // leaf
    { // we'll add the whole thing (which means we may be including some out-of-range boxes):
        size_t * l = reinterpret_cast<size_t *>(st->index & 0xFFFFFFFFFFFFFFFC);
        //addCloudSegment(*l, st->items);
        addFacesFromBoxes(*l, st->items);
        return;
    }
    char cmp = intervalCompare(treeBox, a);
    if ((cmp == 'a') || (cmp == 'b')) return;
    if (cmp == 'i')
    {
        if (IsContained(treeBox))
        {
            addAllInSubTree(st);
            return;
        }
    }
    pBIHPointerType<T> p =
        reinterpret_cast<pBIHPointerType<T>>(st->index & 0xFFFFFFFFFFFFFFFC);
    pBIH_Node<T> l = p->left, r = p->right;

    VID::AABBBasic<T> bbL, bbR;
    memcpy(&bbL, &treeBox, sizeof(VID::AABBBasic<T>));
    bbL.HI.dim(a) = st->Clip[0];
    memcpy(&bbR, &treeBox, sizeof(VID::AABBBasic<T>));
    bbR.LO.dim(a) = st->Clip[1];

    SearchBIHTree(l, bbL);
    SearchBIHTree(r, bbR);


}

template<typename T> void BIH::BIH<T>::delSubTree(const pBIH_Node<T> & st)
{
    if (st == NULL) return;
    if ((st->index & 0x3) == 0x3) // leaf
    {
        size_t * l = reinterpret_cast<size_t *>(st->index & 0xFFFFFFFFFFFFFFFC);
        delete l;
        return;
    }
    pBIHPointerType<T> p =
        reinterpret_cast<pBIHPointerType<T>>(st->index & 0xFFFFFFFFFFFFFFFC);
    pBIH_Node<T> l = p->left, r = p->right;
    delSubTree(l);
    delete l;
    delSubTree(r);
    delete r;
    delete p;
} // delSubTree

template<typename T> void BIH::BIH<T>::Partition(const size_t & first, const size_t & last, const unsigned char & a, 
    size_t & pivIdx, const T & sp, const VID::AABBBasic<T> & bb, BIH_Node<T> & nodeBIH)
{ // affects class member pcldBIH - exchanges out of order bounding boxes
    T	valHI = static_cast<T>(0),
        valLO = static_cast<T>(0),
        valMP = static_cast<T>(0);
    

    bool	Initialized[] = { false, false };	//clipping plane flags
    // now to adapt the QuickSort (see Koffman):
    size_t up = first, down = last, rl = 0;
    VID::pAABB<T> bbu = NULL, bbd = NULL; // bounding boxes up and down
    while (up < down)
    {
        rl = 0;
        while ((rl == 0) && (up <= down))
        { // check up side first
            bbu = &(pvd->bCld[up]);
            valHI = bbu->BB.HI.dim(a);
            valLO = bbu->BB.LO.dim(a);
            // calculate midpoint:
            valMP = (valHI + valLO) / static_cast<T>(2);
            rl = valMP < sp ? 0 : 1;
            if (rl == 0) ++up;
            // update clipping planes:
            if (Initialized[rl])
            {
                if (rl == 0)
                {
                    if (valHI > nodeBIH.Clip[0])
                    {
                        nodeBIH.Clip[0] = valHI;
                    }
                }
                else // rl is 1
                {
                    if (valLO < nodeBIH.Clip[1])
                    {
                        nodeBIH.Clip[1] = valLO;
                    }
                }
            }
            else // not intiallized:
            {
                Initialized[rl] = true;
                nodeBIH.Clip[rl] = rl == 0 ? valHI : valLO;  // right of left or left of right
            }
        } // if in-range: out of order box found in array at index = up

        rl = 1;
        while ((rl == 1) && (up < down))
        { // check down side last
            bbd = &(pvd->bCld[down]);
            valHI = bbd->BB.HI.dim(a);
            valLO = bbd->BB.LO.dim(a);
            // calculate midpoint:
            valMP = (valHI + valLO) / static_cast<T>(2);
            rl = valMP < sp ? 0 : 1;
            if (rl == 1) --down;
            // update clipping planes:
            if (Initialized[rl])
            {
                if (rl == 0)
                {
                    if (valHI > nodeBIH.Clip[0])
                    {
                        nodeBIH.Clip[0] = valHI;
                    }
                }
                else // rl is 1
                {
                    if (valLO < nodeBIH.Clip[1])
                    {
                        nodeBIH.Clip[1] = valLO;
                    }
                }
            }
            else
            {
                Initialized[rl] = true;
                nodeBIH.Clip[rl] = rl == 0 ? valHI : valLO;  // right of left or left of right
            }
        }  // out of order box found in array at index = down

        if (up < down)
        {
            // make the switch:
            VID::AABB<T> & aabbUp = pvd->bCld[up],
                aabbBuff = VID::AABB<T>(aabbUp),
                & aabbDown = pvd->bCld[down];
            
            aabbUp = aabbDown;
            aabbDown = aabbBuff;
            ++up;
            --down;
        }
    } // end of while(up < down) loop

    pivIdx = up;// all before this index go left all on or after go right
                // possible range: first .. last + 1

} // end of method Partition

template<typename T> BIH::pBIH_Node<T> BIH::BIH<T>::ZeroLeaf(void)
{
    ++zerocount;
    if (InDebug) ZeroLeafToFile();
    return NULL;
}

template<typename T> BIH::pBIH_Node<T> BIH::BIH<T>::LeafWithBeginIndex(const size_t & bIdx, const unsigned int & item_count)
{
    size_t * pI = new size_t;
    *pI = bIdx;
    pBIH_Node<T> l = new BIH_Node<T>;
    l->index = reinterpret_cast<size_t>(pI) | 3; // making it leaf
    l->items = item_count;
    ++leafcount;
    if (item_count > maxleaf) maxleaf = item_count;
    if (InDebug) LeafToFile(bIdx, item_count);
    return l;
}

template<typename T> T BIH::BIH<T>::GetMaxSide(const VID::AABBBasic<T> & subBB, unsigned char & axis)
{
    axis = 0;
    T	max,
        xside = subBB.HI.X - subBB.LO.X,
        yside = subBB.HI.Y - subBB.LO.Y,
        zside = subBB.HI.Z - subBB.LO.Z;
    max = xside;
    if (yside > max)
    {
        axis = 1;
        max = yside;
    }
    if (zside > max)
    {
        axis = 2;
        max = zside;
    }
    return max;
}

template<typename T> bool BIH::BIH<T>::IsWithinPixel(const VID::AABBBasic<T> & subBB, unsigned char & axis)
{ // this is meant for models with integer vertices in nanometers
    T	max = GetMaxSide(subBB, axis),
        units_per_pixel = static_cast<T>(nm_per_pixel);

    return max < units_per_pixel; // regardless T s/b integer or floatPoint type
}

template<typename T> BIH::pBIH_Node<T> BIH::BIH<T>::BuildBIH(VID::AABBBasic<T> & subBB, 
    const size_t & first, const size_t & last)
{
    if (pmax < rcount) pmax = rcount;
    size_t objCount = last - first + 1;
    unsigned char a = 0;
    //bool IWP = IsWithinPixel(subBB, a);
    if (IsWithinPixel(subBB, a) || (objCount <= MIN_OBJS_IN_BIH_LEAF))
    {
        return LeafWithBeginIndex(first, objCount);
    }
    ++rcount;

    pBIH_Node<T> node = new BIH_Node<T>;
    T sPlane = (subBB.HI.dim(a) + subBB.LO.dim(a)) / static_cast<T>(2) ;
    
    if (InDebug) BuildBIHToFile(subBB, first, last, a, sPlane);

    size_t	beforeIdx = 0;

    Partition(first, last, a, beforeIdx, sPlane, subBB, *node);
    
    pBIHPointerType<T> ptr = new BIHPointerType<T>;
    ptr->left = NULL;
    ptr->right = NULL;
    node->index = reinterpret_cast<size_t>(ptr) | a;

    VID::AABBBasic<T> bbL, bbR;
    memcpy(&bbL, &subBB, sizeof(VID::AABBBasic<T>));
    bbL.HI.dim(a) = sPlane;
    memcpy(&bbR, &subBB, sizeof(VID::AABBBasic<T>));
    bbR.LO.dim(a) = sPlane;

    ptr->left = beforeIdx == first ? ZeroLeaf() : BuildBIH(bbL, first, beforeIdx - 1);
    ptr->right = beforeIdx > last ? ZeroLeaf() : BuildBIH(bbR, beforeIdx, last);

    ++nodecount;
    --rcount;
    return node;

}

// DEBUG Stuff:
template<typename T> string BIH::BIH<T>::AABBBasicToStr(const VID::AABBBasic<T> & aabb)
{
    string txtBuff = 
    "LO: (" + to_string(aabb.LO.X) + ", " + to_string(aabb.LO.Y) + ", " + to_string(aabb.LO.Z) + ")  " +
    "HI: (" + to_string(aabb.HI.X) + ", " + to_string(aabb.HI.Y) + ", " + to_string(aabb.HI.Z) + ")";

    return txtBuff;
}

template<typename T> string BIH::BIH<T>::AABBToStr(const VID::AABB<T> & aabb)
{
    string txtBuff = AABBBasicToStr(aabb.BB) + " face[" + to_string(aabb.objID) + "]"; 
    //"LO: (" + to_string(ab.BB.LO.X) + ", " + to_string(ab.BB.LO.Y) + ", " + to_string(ab.BB.LO.Z) + ")  " +
    //"HI: (" + to_string(ab.BB.HI.X) + ", " + to_string(ab.BB.HI.Y) + ", " + to_string(ab.BB.HI.Z) + 
    //") face[" + to_string(ab.objID) + "]";

    return txtBuff;
}


template<typename T> void BIH::BIH<T>::ZeroLeafToFile(void)
{
    string txtBuff = "\t>Z" + to_string(zerocount) + "> Zero Leaf Added\n";
    WriteToFile(txtBuff.c_str(), txtBuff.size() + 1);
}

template<typename T> void BIH::BIH<T>::LeafToFile(const size_t & bIdx, const size_t & count)
{
    string txtBuff = "\t>L" + to_string(leafcount) + "> Leaf Added starting @ bCld[" + 
        to_string(bIdx) + "] for " + to_string(count) + "\n";
    WriteToFile(txtBuff.c_str(), txtBuff.size() + 1);
}

template<typename T> void BIH::BIH<T>::BuildBIHToFile(const VID::AABBBasic<int> & subBB,
    const size_t & first, const size_t & last,
    const unsigned char & axis, const T & sPlane)
{
    string 
        AXIS_ID = "xyz", 
        txtBuff = "{" + to_string(rcount) + "R|" + to_string(nodecount) + 
        "N} box: " + AABBBasicToStr(subBB) + "<" + to_string(first) + "F|" + 
        to_string(last) + "L> " + AXIS_ID[axis] + "=" + to_string(sPlane) + "\n";

    WriteToFile(txtBuff.c_str(), txtBuff.size() + 1);

}


template<typename T> void BIH::BIH<T>::WriteToFile(const char * sBuff, const streamsize & dwBytesToWrite)
{
    //DWORD  dwBytesWritten;
    phf->write(sBuff, dwBytesToWrite);
    //return dwBytesWritten;
}

//linker helper:
void bih_linker_helper(void)
{
    // for int calls of any public method:
    BIH::BIH<int> dftltintBIHDummyValue;
    VID::AABBBasic<int> aabbintDummyValue;
    VID::ViewType<int> vtintDummyValue;
    BIH::BIH<int> intBIHDummyValue = BIH::BIH<int>(&aabbintDummyValue, &vtintDummyValue, 0);
    intBIHDummyValue.clearBIH();
    intBIHDummyValue.ShowTreeCounts();
    intBIHDummyValue.getBoxesWithin(aabbintDummyValue);
    intBIHDummyValue.BIHSimpleTest1();
    intBIHDummyValue.ReLoad(&vtintDummyValue, 0);
    intBIHDummyValue.StartFile();
    intBIHDummyValue.EndFile();

}