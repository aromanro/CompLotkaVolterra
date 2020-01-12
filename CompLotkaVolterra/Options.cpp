#include "Options.h"


#include <wx/stdpaths.h> 

Options::Options()
	: nrPoints(300000), method(4), m_fileconfig(nullptr)
{
}


void Options::Open()
{
	if (m_fileconfig) return;

	wxString dir = wxStandardPaths::Get().GetConfigDir() + wxFileName::GetPathSeparator();

	if(!wxFileName::DirExists(dir))
		wxFileName::Mkdir(dir, 0777, wxPATH_MKDIR_FULL);

	wxString iniFilePath = dir + "CompLotkaVolterra.ini";

	m_fileconfig = new wxFileConfig("CompLotkaVolterra", wxEmptyString, iniFilePath);

	wxConfigBase::Set(m_fileconfig);
}

void Options::Close()
{
	delete m_fileconfig;
	m_fileconfig = NULL;
	wxConfigBase::Set(NULL);
}

void Options::Load()
{
	Open();
	wxConfigBase *conf=wxConfigBase::Get(false);
	if (conf)
	{
		nrPoints = conf->ReadLong("/nrPoints", 300000);
		method = conf->ReadLong("/method", 4);
	}
	Close();
}

void Options::Save()
{
	Open();
	wxConfigBase *conf=wxConfigBase::Get(false);
	if (conf)
	{
		conf->Write("/nrPoints", static_cast<long int>(nrPoints));
		conf->Write("/method", static_cast<long int>(method));
	}

	if (m_fileconfig)
		m_fileconfig->Flush();
	Close();
}
