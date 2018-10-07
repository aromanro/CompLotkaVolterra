#pragma once

#include <string>
#include <vector>

#define wxNEEDS_DECL_BEFORE_TEMPLATE

#include <wx/fileconf.h>

class Options
{
public:
	Options();
	~Options() { Close(); }
		
	void Load();
	void Save();

	void Open();
	void Close();

	int nrPoints;
	int method;

protected:
	wxFileConfig *m_fileconfig;
};

