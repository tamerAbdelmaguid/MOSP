//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormSSPRCalcProgress.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FormCalcProgress"
#pragma resource "*.dfm"
TfrmSSPRCalcProgress *frmSSPRCalcProgress;
//---------------------------------------------------------------------------
__fastcall TfrmSSPRCalcProgress::TfrmSSPRCalcProgress(TComponent* Owner)
	: TfrmCalcProgress(Owner)
{
  bActive = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSSPRCalcProgress::FormShow(TObject *)
{
  ClearForm();
  bActive = true;
}
//---------------------------------------------------------------------------

void TfrmSSPRCalcProgress::ClearForm()
{
  lblLB->Caption = "";
  lblbestCmax->Caption = "";

  lblTSInitialCmax->Caption = "";
  lblTSCurrentCmax->Caption = "";
  lblTSBestCmax->Caption = "";
  lblTSIter->Caption = "";
  lblTSImpIter->Caption = "";
  pbTabuSearch->Position = 0;

  grdInitialSolutions->Cells[0][0] = "Sol. #";
  grdInitialSolutions->Cells[1][0] = "Initial Cmax";
  grdInitialSolutions->Cells[2][0] = "Final Cmax";
  for(int i=1; i < grdInitialSolutions->RowCount; i++){
	grdInitialSolutions->Cells[0][i] = "";
	grdInitialSolutions->Cells[1][i] = "";
	grdInitialSolutions->Cells[2][i] = "";
  }

  pbSSPRIterations->Position = 0;
  pbConstruction->Position = 0;

  lblPRIter->Caption = "";
  pbPRProgress->Position = 0;
  sgPRIterations->Cells[0][0] = "Iter. #";
  sgPRIterations->Cells[1][0] = "Cmax(S1)";
  sgPRIterations->Cells[2][0] = "Cmax(S2)";
  sgPRIterations->Cells[3][0] = "Cmax(S3)-i";
  sgPRIterations->Cells[4][0] = "Cmax(S3)-f";
  for(int i=1; i < sgPRIterations->RowCount; i++){
	sgPRIterations->Cells[0][i] = "";
	sgPRIterations->Cells[1][i] = "";
	sgPRIterations->Cells[2][i] = "";
	sgPRIterations->Cells[3][i] = "";
	sgPRIterations->Cells[4][i] = "";
  }
}

void __fastcall TfrmSSPRCalcProgress::FormClose(TObject *, TCloseAction &)

{
  bActive = false;
}
//---------------------------------------------------------------------------


