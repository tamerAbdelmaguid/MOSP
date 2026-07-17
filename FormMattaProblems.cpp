//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormMattaProblems.h"
#include "FormProblem.h"
#include "Problem.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMattaProblems *frmMattaProblems;
//---------------------------------------------------------------------------
__fastcall TfrmMattaProblems::TfrmMattaProblems(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmMattaProblems::FormShow(TObject *)
{
  edtProblemName->Text = "";
  edtnJobs->Text = "";
  cbxNoOfPCs->ItemIndex = 0; // two processing centers
  grdDataTable->Cells[0][0] = "Center #";
  grdDataTable->Cells[1][0] = "no. of machines";
  grdDataTable->Cells[2][0] = "processing time";
  grdDataTable->RowCount = 3;
  cbxNoOfPCsChange(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMattaProblems::cbxNoOfPCsChange(TObject *)
{
  switch(cbxNoOfPCs->ItemIndex){
	case 0:   // 2 centers
			  grdDataTable->RowCount = 3;
			  break;
	case 1:   // 4 centers
			  grdDataTable->RowCount = 5;
			  break;
	case 2:   // 8 centers
			  grdDataTable->RowCount = 9;
			  break;
	case 3:   // 16 centers
			  grdDataTable->RowCount = 17;
			  break;
  }
  for(int i=1; i<grdDataTable->RowCount; i++){
	grdDataTable->Cells[0][i] = IntToStr(i);
	grdDataTable->Cells[1][i] = "";
	grdDataTable->Cells[2][i] = "";
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMattaProblems::btnGenerateClick(TObject *)
{
  AnsiString s;
  AnsiString prbname = edtProblemName->Text;

  int nPCs;
  switch(cbxNoOfPCs->ItemIndex){
	case 0:   // 2 centers
			  nPCs = 2;
			  break;
	case 1:   // 4 centers
			  nPCs = 4;
			  break;
	case 2:   // 8 centers
			  nPCs = 8;
			  break;
	case 3:   // 16 centers
			  nPCs = 16;
			  break;
  }

  int nJobs;
  s = edtnJobs->Text;
  try {
	nJobs = StrToInt(s);
  } catch(EConvertError&){
	  Application->MessageBox(L"Use appropriate values for number of jobs",
							  L"Error reading numbers", MB_OK);
	  return;
	}

  int *nMCs, *pt;
  nMCs = new int[nPCs];
  pt = new int[nPCs];
  for(int i=0; i<nPCs; i++){
	try{
	  s = grdDataTable->Cells[1][i+1];
	  nMCs[i] = StrToInt(s);
	  s = grdDataTable->Cells[2][i+1];
	  pt[i] = StrToInt(s);
	} catch(EConvertError&){
		Application->MessageBox(L"Use appropriate values for number of machines/processing times",
							  L"Error reading numbers", MB_OK);
		delete [] nMCs;
		delete [] pt;
		return;
	  }
  }

  MOSP_Problem *newProblem = new MOSP_Problem(prbname.c_str(), "Matta\'s problem", nPCs, nJobs, nMCs, pt);

  delete [] nMCs;
  delete [] pt;

  TfrmProblem *Child = new TfrmProblem(Application, newProblem, prbname);
  Child->Caption = prbname;
}
//---------------------------------------------------------------------------
