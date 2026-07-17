//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------
USEFORM("Main.cpp", MainForm);
USEFORM("FormProblem.cpp", frmProblem);
USEFORM("About.cpp", AboutBox);
USEFORM("FormSol.cpp", frmSolution);
USEFORM("FormAlgorithm.cpp", frmAlgorithm);
USEFORM("FormPSEditor.cpp", frmPSEditor);
USEFORM("FormCalcProgress.cpp", frmCalcProgress);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Application->Initialize();
	Application->Title = "Job Shop Scheduling Problem Solver";
                 Application->CreateForm(__classid(TMainForm), &MainForm);
		Application->CreateForm(__classid(TAboutBox), &AboutBox);
		Application->CreateForm(__classid(TfrmPSEditor), &frmPSEditor);
		Application->CreateForm(__classid(TfrmCalcProgress), &frmCalcProgress);
		Application->Run();

	return 0;
}
//---------------------------------------------------------------------
