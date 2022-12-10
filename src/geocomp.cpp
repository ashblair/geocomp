#include "first.hpp"
#include "CompGeo.hpp"
#include "NGons.hpp"
#include "drawgon.hpp"
#include "ConvexHull.hpp"
#include "SegmentIntersect.hpp"
#include "Unshare.hpp"
#include "Triangulation.hpp"
#include "Fortune.hpp"
#include "Trapezoid.hpp"
#include "ControlInterface.hpp"
#include "widgets.hpp"

int main (int argc, char **argv)
{
    GEXT::argc = argc;
    GEXT::argv = argv;

    pGEXT pG = GEXT::getInst();
	ControlInterface * p_CI = new ControlInterface;
	p_CI->ResetControlInterface(pG);
    pG->pCI = p_CI;
	GEXT::the_app->run(*GEXT::CMW);

    exit(EXIT_SUCCESS);
}