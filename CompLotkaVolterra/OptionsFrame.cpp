#include <wx/statline.h>
#include <wx/valgen.h>
#include <wx/valnum.h>


#include "CompLKApp.h"
#include "OptionsFrame.h"
#include "CompLKFrame.h"

#define ID_NRPOINTS 101
#define ID_METHOD 102

wxDECLARE_APP(CompLKApp);

OptionsFrame::OptionsFrame(const wxString & title, wxWindow* parent)
	   : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxSize(400, 150))
{
	CreateControls();

	Centre();
}


void OptionsFrame::CreateControls()
{
	// box to contain them all
	wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
	SetSizer(vbox);

	// box with margin to contain option controls
	wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
	vbox->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL| wxGROW | wxALL, 5);

	// ******************************************************************
	// now option controls

	// nr points

	wxBoxSizer* box = new wxBoxSizer(wxHORIZONTAL);
	boxSizer->Add(box, 0, wxGROW|wxALL, 5);

	wxStaticText* label = new wxStaticText(this, wxID_STATIC, "&Nr. Points:", wxDefaultPosition, wxSize(60, -1), wxALIGN_RIGHT);
	box->Add(label, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	wxString str = wxString::Format(wxT("%i"), options.nrPoints);
	wxTextCtrl* nrPointsCtrl = new wxTextCtrl(this, ID_NRPOINTS, str, wxDefaultPosition, wxSize(60, -1), 0);
	box->Add(nrPointsCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);


	box->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5); // pushes to the right

	// method

	label = new wxStaticText(this, wxID_STATIC, "&Method:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	box->Add(label, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	wxString methodStrings[] = { "Euler", "Midpoint", "Heun", "Ralston", "RK4", "RK3/8", "Heun-Euler (adaptive)", "Bogacki-Shampine (adaptive)", "Cash-Karp (adaptive)", "Fehlberg (adaptive)", "Dormand-Prince (adaptive)" };

	wxChoice* methodChoice = new wxChoice (this, ID_METHOD, wxDefaultPosition, wxSize(160, -1), WXSIZEOF(methodStrings), methodStrings, 0 );
	methodChoice->Select(options.method);
	box->Add(methodChoice, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);


	// ******************************************************************
	// setting validators

	// there is a bug in validator ranges, so set the range from 1
	// and validate for the minimum value elsewhere
	wxIntegerValidator<int> val1(&options.nrPoints, wxNUM_VAL_DEFAULT);
	val1.SetRange(1, 10000000);
	nrPointsCtrl->SetValidator(val1);
	
	methodChoice->SetValidator(wxGenericValidator(&options.method));
	

	// ******************************************************************

	// divider line

	wxStaticLine* line = new wxStaticLine(this, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	boxSizer->Add(line, 0, wxGROW|wxALL, 5);

	// bottom box with ok & cancel buttons

	wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
	
	
	wxButton *okButton = new wxButton(this, wxID_OK, "Ok", wxDefaultPosition, wxSize(70, 30));
	wxButton *closeButton = new wxButton(this, wxID_CANCEL, "Cancel", wxDefaultPosition, wxSize(70, 30));

	hbox->Add(okButton, 1);
	hbox->Add(closeButton, 1, wxLEFT, 5);

	vbox->Add(hbox, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
}


bool OptionsFrame::TransferDataFromWindow()
{
	if (!wxDialog::TransferDataFromWindow()) return false;

	wxTextCtrl* nrPointsCtrl = (wxTextCtrl*)FindWindow(ID_NRPOINTS);
	wxString str = nrPointsCtrl->GetValue();
	long int val = 0;
	if (!str.ToLong(&val)) return false;
	if (val < 1000 || val > 10000000)
	{
		wxMessageBox("Please enter between 1000 and 10000000 points", "Validation", wxOK | wxICON_INFORMATION, this);

		return false; 
	}
	
	return true;
}

