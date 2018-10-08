
#include "CompLKFunc.h"

#include "RungeKutta.h"

#include "CompLKFrame.h"

#include "OptionsFrame.h"

#include <vtkAutoInit.h>

#include <thread>

VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkRenderingContextOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);

#define MY_VTK_WINDOW 102

#define ID_CALCULATE 105

wxBEGIN_EVENT_TABLE(CompLKFrame, wxFrame)
EVT_MENU(ID_CALCULATE, CompLKFrame::OnCalculate)
EVT_UPDATE_UI(ID_CALCULATE, CompLKFrame::OnUpdateCalculate)
EVT_MENU(wxID_EXIT, CompLKFrame::OnExit)
EVT_MENU(wxID_PREFERENCES, CompLKFrame::OnOptions)
EVT_MENU(wxID_ABOUT, CompLKFrame::OnAbout)
EVT_TIMER(101, CompLKFrame::OnTimer)
EVT_SIZE(CompLKFrame::OnSize)
EVT_ERASE_BACKGROUND(CompLKFrame::OnEraseBackground)
wxEND_EVENT_TABLE()


CompLKFrame::CompLKFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size),
	timer(this, 101), runningThreads(0)
{
	wxMenu *menuFile = new wxMenu;

	menuFile->Append(ID_CALCULATE, "C&alculate\tCtrl+a", "Starts computing");
	menuFile->Append(wxID_SEPARATOR);
	menuFile->Append(wxID_EXIT);

	wxMenu *menuView = new wxMenu;
	menuView->Append(wxID_PREFERENCES);

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuView, "&View");
	menuBar->Append(menuHelp, "&Help");

	SetMenuBar(menuBar);

	CreateStatusBar();
	SetStatusText("Welcome to Competitive Lotka–Volterra!");

	m_pVTKWindow = new wxVTKRenderWindowInteractor(this, MY_VTK_WINDOW);
	m_pVTKWindow->UseCaptureMouseOn();
	//m_pVTKWindow->DebugOn();
	m_pVTKWindow->DebugOff();

	currentOptions.Open();
	currentOptions.Load();

	ConstructVTK();
}


CompLKFrame::~CompLKFrame()
{
	DestroyVTK();
	if (m_pVTKWindow) m_pVTKWindow->Delete();
	currentOptions.Close();
}

void CompLKFrame::ConstructVTK()
{
	pRenderer = vtkRenderer::New();
	pContextView = vtkContextView::New();

	vtkRenderWindow *pRenderWindow = m_pVTKWindow->GetRenderWindow();
	pRenderWindow->AddRenderer(pRenderer);
	pRenderWindow->SetMultiSamples(0);
	pContextView->SetInteractor(pRenderWindow->GetInteractor());
	//pContextView->GetInteractor()->Initialize();

	table = vtkTable::New();
	vtkNew<vtkFloatArray> arrX;
	arrX->SetName("X Axis");
	table->AddColumn(arrX.GetPointer());
	vtkNew<vtkFloatArray> arrY;
	arrY->SetName("Y Axis");
	table->AddColumn(arrY.GetPointer());
	vtkNew<vtkFloatArray> arrZ;
	arrZ->SetName("Z Axis");
	table->AddColumn(arrZ.GetPointer());
	vtkNew<vtkFloatArray> arrColor;
	arrColor->SetName("Color");
	table->AddColumn(arrColor.GetPointer());

	pChart = nullptr;
}

void CompLKFrame::DestroyVTK()
{
	if (pChart) pChart->Delete();
	if (table) table->Delete();
	if (pRenderer) pRenderer->Delete();
	if (pContextView) pContextView->Delete();
}


void CompLKFrame::OnCalculate(wxCommandEvent& /*event*/)
{
	Compute();
}

void CompLKFrame::OnUpdateCalculate(wxUpdateUIEvent& event)
{
	event.Enable(isFinished());
}


bool CompLKFrame::isFinished() const
{
	return 0 == runningThreads;
}

void CompLKFrame::OnOptions(wxCommandEvent& /*event*/)
{
	OptionsFrame *optionsFrame = new OptionsFrame("Options", this);
	optionsFrame->options = currentOptions;
	if (wxID_OK == optionsFrame->ShowModal())
	{
		currentOptions = optionsFrame->options;
		currentOptions.Save();
	}

	delete optionsFrame;
}

void CompLKFrame::Compute()
{
	if (!isFinished()) return;

	wxBeginBusyCursor();

	SetTitle("Computing - Competitive Lotka–Volterra");

	computeOptions = currentOptions;

	runningThreads = 1;

	timer.Start(100);

	pContextView->GetScene()->ClearItems();
	if (pChart) pChart->Delete();
	pChart = vtkChartXYZ::New();
	pContextView->GetScene()->AddItem(pChart);

	std::thread([this]()
	{
		const unsigned int numVals = computeOptions.nrPoints;

		switch (computeOptions.method)
		{
			case 0:
				Compute<RungeKutta::Euler<MyVector4d>>(numVals);
				break;
			case 1:
				Compute<RungeKutta::Midpoint<MyVector4d>>(numVals);
				break;
			case 2:
				Compute<RungeKutta::Heun<MyVector4d>>(numVals);
				break;
			case 3:
				Compute<RungeKutta::Ralston<MyVector4d>>(numVals);
				break;
			case 4:
			default:
				Compute<RungeKutta::RK4<MyVector4d>>(numVals);
				break;
			case 5:
				Compute<RungeKutta::RK3per8<MyVector4d>>(numVals);
				break;
			case 6:
				Compute<RungeKutta::AdaptiveHeunEuler<MyVector4d>>(numVals);
				break;
			case 7:
				Compute<RungeKutta::AdaptiveBogackiShampine<MyVector4d>>(numVals);
				break;
			case 8:
				Compute<RungeKutta::AdaptiveCashKarp<MyVector4d>>(numVals);
				break;
			case 9:
				Compute<RungeKutta::AdaptiveFehlberg<MyVector4d>>(numVals);
				break;
			case 10:
				Compute<RungeKutta::AdaptiveDormandPrince<MyVector4d>>(numVals);
				break;
		}
		
		runningThreads = 0;
	}).detach();
}

template<typename Solver> void CompLKFrame::Compute(unsigned int numVals)
{
	table->SetNumberOfRows(numVals);

	MyVector4d point;
	point << 1., 1., 1., 1.;
	table->SetValue(0, 0, point(0));
	table->SetValue(0, 1, point(1));
	table->SetValue(0, 2, point(2));
	table->SetValue(0, 3, point(3));

	double t = 0;  // this is dummy

	// don't want too small values for this purpose
	double step = 1E-2;
	double next_step = step;
	double precision = 1E-4;
	double max_step = 1E-2;

	Solver solver;
	FunctorForLK<MyVector4d, Eigen::Matrix4d> functor;

	for (unsigned int i = 1; i < numVals; ++i)
	{
		point = solver.SolveStep(functor, point, t, step, next_step, precision, max_step);

		table->SetValue(i, 0, point(0));
		table->SetValue(i, 1, point(1));
		table->SetValue(i, 2, point(2));
		table->SetValue(i, 3, point(3));

		t += step;
		if (solver.IsAdaptive()) step = next_step;
	}
}


void CompLKFrame::OnTimer(wxTimerEvent& WXUNUSED(event))
{
	if (isFinished())
	{
		timer.Stop();
		StopThreads();

		SetTitle("Finished - Competitive Lotka–Volterra");

		vtkNew<vtkPlotPoints3D> plot;
		plot->SetInputData(table, "X Axis", "Y Axis", "Z Axis", "Color");
		pChart->AddPlot(plot.GetPointer());

		int w;
		int h;
		GetClientSize(&w, &h);

		int d = w;
		if (w > h) d = h;

		pChart->SetGeometry(vtkRectf((w - d / 1.5) / 2, (h - d / 1.5) / 2, d / 1.5, d / 1.5));

		pChart->SetFitToScene(false); // fit it manually

		pChart->RecalculateTransform();
		pChart->RecalculateBounds();

		pChart->GetAxis(0)->SetUnscaledRange(0, 1);
		pChart->GetAxis(1)->SetUnscaledRange(0, 1);
		pChart->GetAxis(2)->SetUnscaledRange(0, 1);

		pChart->Update();

		// disabled rotation before showing the chart until I figure out the labels issue

		/*
		vtkContextMouseEvent mouseEvent;
		mouseEvent.SetInteractor(m_pVTKWindow->GetRenderWindow()->GetInteractor());

		vtkVector2i pos;
		vtkVector2i lastPos;
	
		// rotate

		mouseEvent.SetButton(vtkContextMouseEvent::LEFT_BUTTON);
		lastPos.Set(10, 10);
		mouseEvent.SetLastScreenPos(lastPos);
		pos.Set(73, 124);
		mouseEvent.SetScreenPos(pos);

		vtkVector2d sP(pos.Cast<double>().GetData());
		vtkVector2d lSP(lastPos.Cast<double>().GetData());

		vtkVector2d screenPos(mouseEvent.GetScreenPos().Cast<double>().GetData());
		vtkVector2d lastScreenPos(mouseEvent.GetLastScreenPos().Cast<double>().GetData());
		pChart->MouseMoveEvent(mouseEvent);
		
		pChart->Update();
		*/
		Refresh();
	}
}


void CompLKFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
	int w, h;
	GetClientSize(&w, &h);

	if (m_pVTKWindow)
	{
		((wxWindow*)m_pVTKWindow)->SetSize(w, h);
		m_pVTKWindow->UpdateSize(w, h);
	}

#if VTK_MAJOR_VERSION > 4 || (VTK_MAJOR_VERSION == 4 && VTK_MINOR_VERSION > 0)
	if (m_pVTKWindow) m_pVTKWindow->InvokeEvent(vtkCommand::ConfigureEvent, NULL);
#endif

	if (pChart)
	{
		int d = w;
		if (w > h) d = h;
		pChart->SetGeometry(vtkRectf((w - d / 1.5) / 2, (h - d / 1.5) / 2, d / 1.5, d / 1.5));

		pChart->RecalculateTransform();
		pChart->RecalculateBounds();

		pChart->GetAxis(0)->SetUnscaledRange(0, 1);
		pChart->GetAxis(1)->SetUnscaledRange(0, 1);
		pChart->GetAxis(2)->SetUnscaledRange(0, 1);
		
		pChart->Update();

		Refresh();
	}
}


void CompLKFrame::OnEraseBackground(wxEraseEvent &event)
{
	event.Skip(false);
}


void CompLKFrame::OnExit(wxCommandEvent& /*event*/)
{
	Close(true);
}

void CompLKFrame::OnAbout(wxCommandEvent& /*event*/)
{
	wxMessageBox("Competitive Lotka–Volterra ver 1.0", "About CompLotkaVolterra", wxOK | wxICON_INFORMATION);
}


void CompLKFrame::StopThreads(bool /*cancel*/)
{
	if (wxIsBusy()) wxEndBusyCursor();
}