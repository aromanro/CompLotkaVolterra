#pragma once

#include <string>
#include <vector>

#define wxNEEDS_DECL_BEFORE_TEMPLATE

#include <wx/fileconf.h>

class Options
{
public:
	Options();
		
	// avoid double deletion of m_fileconfig at destruction if copied
	Options(const Options& other)
		:
		nrPoints(other.nrPoints),
		method(other.method),
		m_fileconfig(nullptr)
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

	void Open();
	void Close();

	int nrPoints;
	int method;

protected:
	wxFileConfig *m_fileconfig;
};

