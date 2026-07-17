//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormhPSOTS.h"
#include "FormhPSOTSCalcProgress.h"
#include "FormSol.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FormTS"
#pragma resource "*.dfm"
TfrmhPSOTS *frmhPSOTS;
//---------------------------------------------------------------------------
__fastcall TfrmhPSOTS::TfrmhPSOTS(TComponent* Owner)
	: TfrmTS(Owner)
{
  hPSOTS = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmhPSOTS::FormShow(TObject *Sender)
{
  TfrmAlgorithm::FormShow(Sender);
  switch(Problem->WCs.Count()){
	case 2:
			edtnParticles->Text = "2";
			edtTabuSize->Text = "4";
			edtnPSOIterations->Text = "10";
			edtnIterations->Text = "100";
			edtnImpIterations->Text = "60";
			break;
	case 4:
			edtnParticles->Text = "4";
			edtTabuSize->Text = "5";
			edtnPSOIterations->Text = "50";
			edtnIterations->Text = "1000";
			edtnImpIterations->Text = "600";
			break;
	case 8:
			edtnParticles->Text = "5";
			edtTabuSize->Text = "15";
			edtnPSOIterations->Text = "20";
			edtnIterations->Text = "300";
			edtnImpIterations->Text = "100";
			break;
	case 16:
			edtnParticles->Text = "5";
			edtTabuSize->Text = "20";
			edtnPSOIterations->Text = "20";
			edtnIterations->Text = "200";
			edtnImpIterations->Text = "50";
			break;
  }
  edtOmega->Text = "0.5";
  edtDelta0->Text = "0.05";
  edtc1->Text = "2.0";
  edtc2->Text = "0.5";
  edtMaxTime->Text = FloatToStr(Problem->Jobs.Count() * Problem->WCs.Count() * 100);
}
//---------------------------------------------------------------------------
void __fastcall TfrmhPSOTS::btnRunClick(TObject *)
{
  AnsiString s;
  int np, ts;
  long nitr, nimpitr, npsoitr;
  float maxt;
  float omega, delta0, c1, c2;

  try{
	s = edtnParticles->Text;
	np = StrToInt(s);
	s = edtTabuSize->Text;
	ts = StrToInt(s);
	s = edtnIterations->Text;
	nitr = StrToInt(s);
	s = edtnPSOIterations->Text;
	npsoitr = StrToInt(s);
	s = edtMaxTime->Text;
	maxt = StrToFloat(s);
	s = edtnImpIterations->Text;
	nimpitr = StrToFloat(s);

	s = edtOmega->Text;
	omega = StrToFloat(s);
	s = edtDelta0->Text;
	delta0 = StrToFloat(s);
	s = edtc1->Text;
	c1 = StrToFloat(s);
	s = edtc2->Text;
	c2 = StrToFloat(s);
  } catch(EConvertError&){
	  Application->MessageBox(L"Use appropriate numerical values",
						  L"Error reading numbers", MB_OK);
	  return;
	}

  if(hPSOTS) delete hPSOTS;
  hPSOTS = new MOSP_hPSOTS(Problem, np, npsoitr, ts, nitr, nimpitr, maxt, omega, delta0, c1, c2);
  frmhPSOTSCalcProgress->Show(hPSOTS);
  lblTime->Caption = FloatToStr(hPSOTS->GetTime());
  frmSolution->SetCurrentSolution(hPSOTS->Solution);
}
//---------------------------------------------------------------------------
