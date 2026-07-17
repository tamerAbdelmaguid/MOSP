//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------
USEFORM("FormMattaProblems.cpp", frmMattaProblems);
USEFORM("FormSol.cpp", frmSolution);
USEFORM("Main.cpp", MainForm);
USEFORM("About.cpp", AboutBox);
USEFORM("FormProblem.cpp", frmProblem);
USEFORM("FormAlgorithm.cpp", frmAlgorithm);
USEFORM("FormCalcProgress.cpp", frmCalcProgress);
USEFORM("FormPSEditor.cpp", frmPSEditor);
USEFORM("FormTS.cpp", frmTS);
USEFORM("FormTSCalcProgress.cpp", frmTSCalcProgress);
USEFORM("FormhPSOTS.cpp", frmhPSOTS);
USEFORM("FormhPSOTSCalcProgress.cpp", frmhPSOTSCalcProgress);
USEFORM("FormDissimilarityMeasure.cpp", frmDissimilarityMeasure);
USEFORM("FormSSPR.cpp", frmSSPR);
USEFORM("FormSSPRCalcProgress.cpp", frmSSPRCalcProgress);
USEFORM("FormGeneralMOSPInstances.cpp", frmGeneralMOSPInstances);
USEFORM("FormSSPRMultiRuns.cpp", frmSSPRMultiRuns);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Application->Initialize();
	Application->Title = "Multiprocessor Open Shop Scheduling Problem Solver";
		Application->CreateForm(__classid(TMainForm), &MainForm);
		Application->CreateForm(__classid(TAboutBox), &AboutBox);
		Application->CreateForm(__classid(TfrmPSEditor), &frmPSEditor);
		Application->CreateForm(__classid(TfrmCalcProgress), &frmCalcProgress);
		Application->CreateForm(__classid(TfrmMattaProblems), &frmMattaProblems);
		Application->CreateForm(__classid(TfrmTS), &frmTS);
		Application->CreateForm(__classid(TfrmTSCalcProgress), &frmTSCalcProgress);
		Application->CreateForm(__classid(TfrmhPSOTS), &frmhPSOTS);
		Application->CreateForm(__classid(TfrmhPSOTSCalcProgress), &frmhPSOTSCalcProgress);
		Application->CreateForm(__classid(TfrmDissimilarityMeasure), &frmDissimilarityMeasure);
		Application->CreateForm(__classid(TfrmSSPR), &frmSSPR);
		Application->CreateForm(__classid(TfrmSSPRCalcProgress), &frmSSPRCalcProgress);
		Application->CreateForm(__classid(TfrmGeneralMOSPInstances), &frmGeneralMOSPInstances);
		Application->CreateForm(__classid(TfrmSSPRMultiRuns), &frmSSPRMultiRuns);
		Application->Run();

	return 0;
}
//---------------------------------------------------------------------
