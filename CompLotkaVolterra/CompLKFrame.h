#pragma once


#define _MATH_DEFINES_DEFINED

#define wxNEEDS_DECL_BEFORE_TEMPLATE

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "wxVTKRenderWindowInteractor.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkActor.h"

#include "vtkGraph.h"
#include "vtkGraphItem.h"
#include "vtkContextView.h"
#include "vtkContextScene.h"
#include "vtkContextMouseEvent.h"


#include "vtkTable.h"
#include "vtkArray.h"
#include "VtkFloatArray.h"
#include "vtkNew.h"
#include "vtkChart.h"
#include "vtkChartXYZ.h"
#include "vtkPlotPoints3D.h"

#include "vtkStringArray.h"
#include "vtkDoubleArray.h"
#include "vtkPlot.h"
#include "vtkAxis.h"
#include "vtkVector.h"


#include <atomic>


#include "Options.h"

class CompLKFrame : public wxFrame
{
public:
	CompLKFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	~CompLKFrame() override;


	std::atomic_int runningThreads;

	Options currentOptions; // what's edited

private:
	wxVTKRenderWindowInteractor *m_pVTKWindow;

	
	// vtk classes
	vtkRenderer     *pRenderer;
	vtkContextView	*pContextView;

	vtkChartXYZ *pChart;

	vtkTable *table;

	wxTimer timer;

	Options computeOptions; // what's actually displayed

	void ConstructVTK();
	void DestroyVTK();

	bool isFinished() const;
	void StopThreads(bool cancel = false);
	void Compute();
	template<typename Solver> void Compute(unsigned int nrPoints);

	void OnExit(wxCommandEvent& event);
	void OnOptions(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnTimer(wxTimerEvent& event);
	void OnSize(wxSizeEvent &event);
	void OnEraseBackground(wxEraseEvent &event);



	void OnCalculate(wxCommandEvent& event);
	void OnUpdateCalculate(wxUpdateUIEvent& event);

	wxDECLARE_EVENT_TABLE();
};

