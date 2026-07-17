//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Main.h"
#include "About.h"
#include "Problem.h"
#include "FileSystem.h"
#include <io.h>
#include "FormPSEditor.h"
#include "FormMattaProblems.h"
#include "FormTS.h"
#include "FormhPSOTS.h"
#include "FormDissimilarityMeasure.h"
#include "FormSSPR.h"
#include "FormGeneralMOSPInstances.h"
#include "FormSSPRMultiRuns.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------

__fastcall TMainForm::TMainForm(TComponent *Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CreateMDIChild(AnsiString Name)
{
  TfrmProblem *Child;

  AnsiString s = IntToStr(MDIChildCount);
  MOSP_Problem *newProblem = new MOSP_Problem(s.c_str(), Name.c_str());

  //--- create a new MDI child window ----
  Child = new TfrmProblem(Application, newProblem, "");
  Child->Caption = Name;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FileNew1Execute(TObject *)
{
  CreateMDIChild("MOSP" + IntToStr(MDIChildCount + 1));
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FileOpen1Execute(TObject *)
{
  if (OpenDialog->Execute()){
	String msg;
	AnsiString s = OpenDialog->FileName;
	MOSP_FIO_RESULT Result;
	MOSP_Problem *newProblem = LoadProblem(s.c_str(), Result);
	if(Result != SUCCESS){
	  switch(Result){
		case FAIL_TO_OPEN:
		  msg = "failed to open file";
		  break;
		case UNKNOWN_FILE_TYPE:
		  msg = "unknown file type!";
		  break;
		case DATA_ERROR:
		  msg = "Error in data";
		  break;
	  }
	  Application->MessageBox(msg.c_str(), L"Error reading file", MB_OK);
	  return;
	}
	TfrmProblem *Child = new TfrmProblem(Application, newProblem, s);
	Child->Caption = ExtractFileName(s);
	UpdateWorkingDirectory(ExtractFilePath(s));
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::HelpAbout1Execute(TObject *)
{
  AboutBox->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FileExit1Execute(TObject *)
{
  Close();
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::FormShow(TObject *)
{
  UpdateWorkingDirectory(ExtractFilePath(Application->ExeName));
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormCreate(TObject *)
{
  CurrentChild = NULL;
  btnSave->Enabled = false;
  FileSaveItem->Enabled = false;
  FileSaveAsItem->Enabled = false;
  Exportto->Enabled = false;
  mnuSolve->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FileSaveItemClick(TObject *Sender)
{
  if(!CurrentChild) return;
  if(!CurrentChild->bFileNameGiven) FileSaveAsItemClick(Sender);
  else{
    String s;
    MOSP_FIO_RESULT Result;
	SaveProblem(CurrentChild->Problem, (const char *)CurrentChild->FileName.c_str(), Result);
    if(Result != SUCCESS){
      switch(Result){
        case FAIL_TO_OPEN:
          s = "failed to save file";
          break;
        case DATA_ERROR:
          s = "Error in data";
          break;
      }
	  Application->MessageBox(s.c_str(), L"Error saving file", MB_OK);
      return;
    }
    CurrentChild->NeedToBeSaved = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FileSaveAsItemClick(TObject *)
{
  if(!CurrentChild) return;
  SaveDialog->FileName = CurrentChild->Caption;
  if(SaveDialog->Execute()){
	AnsiString s = SaveDialog->FileName;
	if(access(s.c_str(), 0) == 0){
	  String msg = "The file "+s+" already exists, would you like to replace it?";
	  int r = Application->MessageBox(msg.c_str(), L"File exists", MB_YESNO);
	  if(r == IDNO) return;
    }
    MOSP_FIO_RESULT Result;
	SaveProblem(CurrentChild->Problem, s.c_str(), Result);
    CurrentChild->bFileNameGiven = true;
    CurrentChild->FileName = s;
    CurrentChild->Caption = ExtractFileName(s);
    CurrentChild->NeedToBeSaved = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CPLEXDatFileClick(TObject *)
{
  if(!CurrentChild) return;
  AnsiString s = CurrentChild->Caption;
  int i = s.AnsiPos(".dmosp")-1;
  if(i != -1) s = s.SubString(1, i) + ".dat";
  else s = s + ".dat";
  SaveDATDialog->FileName = s;
  if(SaveDATDialog->Execute()){
	s = SaveDATDialog->FileName;
	if(access(s.c_str(), 0) == 0){
	  String msg = "The file "+s+" already exists, would you like to replace it?";
	  int r = Application->MessageBox(msg.c_str(), L"File exists", MB_YESNO);
	  if(r == IDNO) return;
	}
	String str = s;
	MOSP_FIO_RESULT Result;
	SaveMOSP_DAT(CurrentChild->Problem, str.c_str(), Result);
	if(Result != SUCCESS){
	  Application->MessageBox(str.c_str(), L"Error writing file", MB_OK);
	  return;
	}
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UpdateWorkingDirectory(String newDir)
{
  WorkingDirectory = newDir;
  OpenDialog->InitialDir = WorkingDirectory;
  OpenDialogTestProblem->InitialDir = WorkingDirectory;
  SaveDialog->InitialDir = WorkingDirectory;
  SaveDATDialog->InitialDir = WorkingDirectory;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mnuPSEditorClick(TObject *)
{
  if(!CurrentChild) return;
  frmPSEditor->Problem = CurrentChild->Problem;
  frmPSEditor->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::GenerateMattasProblems1Click(TObject *)
{
  frmMattaProblems->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::abuSearchAbdelmaguidetal20141Click(TObject *)
{
  if(!CurrentChild) return;
  frmTS->Problem = CurrentChild->Problem;
  frmTS->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::HybridParticleSwarmOptimizationTabuSearch1Click(TObject *)
{
  if(!CurrentChild) return;
  if(CurrentChild->Problem->isProportionate()){
	frmhPSOTS->Problem = CurrentChild->Problem;
	frmhPSOTS->ShowModal();
  }
  else{
	Application->MessageBox(L"The hPSO-TS approach is suitable to proportionate problems only",
							L"Not a proportionate problem", MB_OK);
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mnuDissimilaritymeasureClick(TObject *)
{
  if(!CurrentChild) return;
  frmDissimilarityMeasure->Problem = CurrentChild->Problem;
  frmDissimilarityMeasure->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ScatterSearchwithPathRelinking1Click(TObject *)
{
  if(!CurrentChild) return;
  frmSSPR->Problem = CurrentChild->Problem;
  frmSSPR->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::GenerategeneralMOSPinstances1Click(TObject *)
{
  frmGeneralMOSPInstances->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SSPRmultipleruns1Click(TObject *)
{
  if(!CurrentChild) return;
  frmSSPRMultiRuns->Problem = CurrentChild->Problem;
  frmSSPRMultiRuns->ShowModal();
}
//---------------------------------------------------------------------------

