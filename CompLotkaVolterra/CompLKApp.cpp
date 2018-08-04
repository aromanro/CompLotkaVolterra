#include "CompLKApp.h"
#include "CompLKFrame.h"


wxIMPLEMENT_APP(CompLKApp);


bool CompLKApp::OnInit()
{
	frame = new CompLKFrame("CompLotkaVolterra", wxPoint(50, 50), wxSize(1024, 800));
	frame->Show(true);

	return true;
}