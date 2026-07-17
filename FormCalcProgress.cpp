//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormCalcProgress.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmCalcProgress *frmCalcProgress;
//---------------------------------------------------------------------------
__fastcall TfrmCalcProgress::TfrmCalcProgress(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmCalcProgress::btnTerminateClick(TObject *)
{
  Alg->bInterrupt = true;
  btnClose->Enabled = true;
  btnStart->Enabled = true;
  btnTerminate->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmCalcProgress::Show(Algorithm *a)
{
  Alg = a;
  btnClose->Enabled = true;
  btnStart->Enabled = true;
  btnTerminate->Enabled = false;
  ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCalcProgress::btnStartClick(TObject *)
{
  btnStart->Enabled = false;
  btnClose->Enabled = false;
  btnTerminate->Enabled = true;
  Alg->Run();
}
//---------------------------------------------------------------------------

void __fastcall TfrmCalcProgress::btnCloseClick(TObject *)
{
  Close();        
}
//---------------------------------------------------------------------------

