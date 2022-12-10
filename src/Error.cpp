#include "first.hpp"
#include "winredefs.hpp"
#include "Error.hpp"

using namespace std;

Error::Error(void)
{
	triggered = false;
	msg = NULL;
}

Error::~Error(void)
{
	if (msg != NULL) delete[] msg;
	msg = NULL;
	triggered = false;
}

void Error::addError(const char * e, const size_t l)
{
	if (msg != NULL) 
	{
		delete [] msg;
		msg = NULL;
	}
	triggered = true;
	msg = new char[l];
	memcpy(msg, e, l);
}

void Error::displayError(void)
{
	if (msg == NULL) return;
	//MessageBoxA(NULL, msg, "Error", MB_OK);
    MessageBox("Error", msg);
	

}
