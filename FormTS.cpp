//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormTS.h"
#include "FormTSCalcProgress.h"
#include "FormSol.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FormAlgorithm"
#pragma resource "*.dfm"
TfrmTS *frmTS;
//---------------------------------------------------------------------------
__fastcall TfrmTS::TfrmTS(TComponent* Owner)
	: TfrmAlgorithm(Owner)
{
  TS = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmTS::FormShow(TObject *Sender)
{
  TfrmAlgorithm::FormShow(Sender);
  edtTabuSize->Text = "15";
  edtnIterations->Text = "5000";
  edtnImpIterations->Text = "500";
  edtMaxTime->Text = FloatToStr(Problem->Jobs.Count() * Problem->WCs.Count() * 100);
}
//---------------------------------------------------------------------------
void __fastcall TfrmTS::btnRunClick(TObject *)
{
  AnsiString s;
  int ts;
  long nitr, nimpitr;
  float maxt;

  try{
	s = edtTabuSize->Text;
	ts = StrToInt(s);
	s = edtnIterations->Text;
	nitr = StrToInt(s);
	s = edtMaxTime->Text;
	maxt = StrToFloat(s);
	s = edtnImpIterations->Text;
	nimpitr = StrToFloat(s);
  } catch(EConvertError&){
	  Application->MessageBox(L"Use appropriate numerical values",
						  L"Error reading numbers", MB_OK);
	  return;
	}

  if(TS) delete TS;
  TS = new MOSP_TS(Problem, ts, nitr, nimpitr, maxt);
  frmTSCalcProgress->Show(TS);
  lblTime->Caption = FloatToStr(TS->GetTime());
  frmSolution->SetCurrentSolution(TS->Solution);
}
//---------------------------------------------------------------------------

