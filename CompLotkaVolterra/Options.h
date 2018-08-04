#pragma once

#include <string>
#include <vector>

#include <wx/fileconf.h>

class Options
{
public:
	Options();
		
	void Load();
	void Save();

	void Open();
	void Close();

	int nrPoints;
	int method;

protected:
	wxFileConfig *m_fileconfig;
};

