#pragma once

#include <string>
#include <vector>

#define wxNEEDS_DECL_BEFORE_TEMPLATE

#include <wx/fileconf.h>

class Options
{
public:
	Options();
	
	~Options()
	{
		delete m_fileconfig;
	}
		
	// avoid double deletion of m_fileconfig at destruction if copied
	Options(const Options& other)
		:
		nrPoints(other.nrPoints),
		method(other.method)
	{
	}

	Options& operator=(const Options& other)
	{
		nrPoints = other.nrPoints;
		method = other.method;
		m_fileconfig = nullptr;

		return *this;
	}


	void Load();
	void Save();

	int nrPoints;
	int method;

private:
	void Open();
	void Close();

	wxFileConfig *m_fileconfig = nullptr;
};

