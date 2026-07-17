//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormSSPRMultiRuns.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmSSPRMultiRuns *frmSSPRMultiRuns;

char strbuf[64];


//---------------------------------------------------------------------------
__fastcall TfrmSSPRMultiRuns::TfrmSSPRMultiRuns(TComponent* Owner)
	: TForm(Owner)
{
  bActive = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmSSPRMultiRuns::SetDefaults()
{
  double A;
  int B, C, D, E, F;

  // Chosen levels combination:  3-5-40-40-10-10
  A = 3;
  B = 5;
  C = 20;
  D = 40;
  E = 10;
  F = 10;

  edtEpsilon->Text = IntToStr(B);
  edtTabuSize->Text = IntToStr(C);
  edtnImpIterations->Text = IntToStr(D);
  edtTSnBestMoves->Text = IntToStr(E);
  edtTSnRandomMoves->Text = IntToStr(F);

  edtPopSize->Text = "20";
  edtnTSIterations->Text = "300";
  edtnSSPRIterations->Text = "10";

  MOSP_SSPR *tmp_ssprDbgAlg = new MOSP_SSPR(Problem);
  tmp_ssprDbgAlg->AllocateMemory();
  int Dth = tmp_ssprDbgAlg->SuggestDeltaThreshold(A);
  edtDeltaThreshold->Text = IntToStr(Dth);
  tmp_ssprDbgAlg->FreeMemory();
  delete tmp_ssprDbgAlg;
}
//---------------------------------------------------------------------------
void __fastcall TfrmSSPRMultiRuns::FormShow(TObject *)
{
  SetDefaults();

  lblLB->Caption = "";
  lblbestCmax->Caption = "";

  pbSSPRIterations->Position = 0;
  pbConstruction->Position = 0;

  pbRun->Position = 0;

  grdRuns->Cells[0][0] = "Run #";
  grdRuns->Cells[1][0] = "Cmax";
  grdRuns->Cells[2][0] = "RPD";
  grdRuns->Cells[3][0] = "Time";
  for(int i=1; i < grdRuns->RowCount; i++){
	grdRuns->Cells[0][i] = IntToStr(i);
	grdRuns->Cells[1][i] = "";
	grdRuns->Cells[2][i] = "";
	grdRuns->Cells[3][i] = "";
  }

  memoMakespan->Clear();
  memoTime->Clear();

  btnClose->Enabled = true;
  btnRun->Enabled = true;
  btnTerminate->Enabled = false;

  if(ssprAlg) delete ssprAlg;
  ssprAlg = NULL;

  bActive = true;

  rgNRuns->ItemIndex = 1;
}
//---------------------------------------------------------------------------
void __fastcall TfrmSSPRMultiRuns::btnRunClick(TObject *)
{
  btnRun->Enabled = false;
  btnClose->Enabled = false;
  btnTerminate->Enabled = true;

  for(int i=1; i < grdRuns->RowCount; i++){
	grdRuns->Cells[0][i] = IntToStr(i);
	grdRuns->Cells[1][i] = "";
	grdRuns->Cells[2][i] = "";
	grdRuns->Cells[3][i] = "";
  }

  if(rgNRuns->ItemIndex == 2){
	TaguchiRuns();
	return;
  }

  int nRuns = (rgNRuns->ItemIndex == 0)? 10 : 30;

  pbRun->Max = nRuns;
  pbRun->Position = 0;

  // read SS/PR parameters
  AnsiString s;
  int psz, Dth, tbsz, tsitr, tsimpitr, nbstmvs, nrndmvs, nitr, epsl;

  try{
	s = edtPopSize->Text;
	psz = StrToInt(s);
	s = edtDeltaThreshold->Text;
	Dth = StrToInt(s);
	s = edtTabuSize->Text;
	tbsz = StrToInt(s);
	s = edtnTSIterations->Text;
	tsitr = StrToInt(s);
	s = edtnImpIterations->Text;
	tsimpitr = StrToInt(s);
	s = edtTSnBestMoves->Text;
	nbstmvs = StrToInt(s);
	s = edtTSnRandomMoves->Text;
	nrndmvs = StrToInt(s);
	s = edtnSSPRIterations->Text;
	nitr = StrToInt(s);
	s = edtEpsilon->Text;
	epsl = StrToInt(s);
  } catch(EConvertError&){
	  Application->MessageBox(L"Use appropriate numerical values",
						  L"Error reading numbers", MB_OK);
	  return;
	}

  wchar_t *ermsg = L"Error entered numbers";

  if(Dth <= 0){
	Application->MessageBox(L"Delta threshold value must be greater than zero.", ermsg, MB_OK);
	return;
  }
  if(psz <= 1){
	Application->MessageBox(L"Population size must be greater than 1.", ermsg, MB_OK);
	return;
  }
  if(tbsz <= 1){
	Application->MessageBox(L"Tabu size must be greater than 1", ermsg, MB_OK);
	return;
  }
  if(tsitr <= 1){
	Application->MessageBox(L"Tabu search iterations must be greater than 1.", ermsg, MB_OK);
	return;
  }
  if(tsimpitr <= 1){
	Application->MessageBox(L"Tabu search improvement iterations must be greater than 1.", ermsg, MB_OK);
	return;
  }
  if(nbstmvs <= 1){
	Application->MessageBox(L"Tabu search number of best moves must be greater than 1.", ermsg, MB_OK);
	return;
  }
  if(nrndmvs <= 1){
	Application->MessageBox(L"Tabu search number of random moves must be greater than 1.", ermsg, MB_OK);
	return;
  }
  if(nitr < 1){
	Application->MessageBox(L"SSPR number of iterations must be greater than 0.", ermsg, MB_OK);
	return;
  }
  if(epsl < 1){
	Application->MessageBox(L"SSPR Epsilon must be greater than 0.", ermsg, MB_OK);
	return;
  }

  // Conduct runs
  for(int r=1; r<=nRuns; r++){
	pbSSPRIterations->Position = 0;
    pbConstruction->Position = 0;

	ssprAlg = new MOSP_SSPR(Problem, psz, Dth, tbsz, tsitr, tsimpitr, nbstmvs, nrndmvs, nitr, epsl);
	ssprAlg->Run();
	if(ssprAlg->bInterrupt){
	  delete ssprAlg;
	  break;
	}
	String sTime = FloatToStr(ssprAlg->GetTime());
	int Cmax = ssprAlg->Solution->ComputeObjectiveValue(MOSP_Problem::MIN_MAKESPAN);
	String sCmax = IntToStr(Cmax);
	double rpd = Cmax;
	rpd -= ssprAlg->LB;
	rpd /= ((double) ssprAlg->LB);
	rpd *= 100.0;
	char buf[16];
	sprintf(buf, "%.2f", rpd);
	String sRPD = buf;

	grdRuns->Cells[1][r] = sCmax;
	grdRuns->Cells[2][r] = sRPD;
	grdRuns->Cells[3][r] = sTime;

    memset(strbuf, 0, sizeof(strbuf));
	sprintf(strbuf, "%i\r\n", Cmax);
	memoMakespan->Lines->Strings[r-1] = strbuf;
	double ctime = ssprAlg->GetTime();
	ctime /= 1000.0;
    memset(strbuf, 0, sizeof(strbuf));
	sprintf(strbuf, "%0.4f\r\n", ctime);
	memoTime->Lines->Strings[r-1] = strbuf;

	pbRun->Position = r;

	delete ssprAlg;
  }
  ssprAlg = NULL;

  btnClose->Enabled = true;
  btnRun->Enabled = true;
  btnTerminate->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmSSPRMultiRuns::btnTerminateClick(TObject *)
{
  if(ssprAlg) ssprAlg->bInterrupt = true;
  btnClose->Enabled = true;
  btnRun->Enabled = true;
  btnTerminate->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmSSPRMultiRuns::FormClose(TObject *, TCloseAction &)

{
  if(ssprAlg) delete ssprAlg;
  ssprAlg = NULL;
  bActive = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmSSPRMultiRuns::TaguchiRuns()
{
  pbRun->Max = 27;
  pbRun->Position = 0;

  memoMakespan->Lines->Clear();
  memoTime->Lines->Clear();

  // Conducte all 27 Taguchi runs here!
  for(int r=0; r<27; r++){
	MOSP_SSPR *tmp_ssprDbgAlg = new MOSP_SSPR(Problem);
	tmp_ssprDbgAlg->AllocateMemory();
	int Dth = tmp_ssprDbgAlg->SuggestDeltaThreshold(Taguchi[r].A);
	edtDeltaThreshold->Text = IntToStr(Dth);
	tmp_ssprDbgAlg->FreeMemory();
	delete tmp_ssprDbgAlg;

	edtEpsilon->Text = IntToStr(Taguchi[r].B);
	edtTabuSize->Text = IntToStr(Taguchi[r].C);
	edtnImpIterations->Text = IntToStr(Taguchi[r].D);
	edtTSnBestMoves->Text = IntToStr(Taguchi[r].E);
	edtTSnRandomMoves->Text = IntToStr(Taguchi[r].F);

	pbSSPRIterations->Position = 0;
	pbConstruction->Position = 0;

	int psz, tbsz, tsitr, tsimpitr, nbstmvs, nrndmvs, nitr, epsl;
	psz = 20;
	tbsz = Taguchi[r].C;
	tsitr = 300;
	tsimpitr = Taguchi[r].D;
	nbstmvs = Taguchi[r].E;
	nrndmvs = Taguchi[r].F;
	epsl = Taguchi[r].B;
	nitr = 10;

	ssprAlg = new MOSP_SSPR(Problem, psz, Dth, tbsz, tsitr, tsimpitr, nbstmvs, nrndmvs, nitr, epsl);
	ssprAlg->Run();
	if(ssprAlg->bInterrupt){
	  delete ssprAlg;
	  break;
	}
	String sTime = FloatToStr(ssprAlg->GetTime());
	int Cmax = ssprAlg->Solution->ComputeObjectiveValue(MOSP_Problem::MIN_MAKESPAN);
	String sCmax = IntToStr(Cmax);
	double rpd = Cmax;
	rpd -= ssprAlg->LB;
	rpd /= ((double) ssprAlg->LB);
	rpd *= 100.0;

    memset(strbuf, 0, sizeof(strbuf));
	sprintf(strbuf, "%.2f", rpd);
	String sRPD = strbuf;

	grdRuns->Cells[1][r+1] = sCmax;
	grdRuns->Cells[2][r+1] = sRPD;
	grdRuns->Cells[3][r+1] = sTime;

    memset(strbuf, 0, sizeof(strbuf));
	sprintf(strbuf, "%i\r\n", Cmax);
	memoMakespan->Lines->Strings[r] = strbuf;
	double ctime = ssprAlg->GetTime();
	ctime /= 1000.0;
    memset(strbuf, 0, sizeof(strbuf));
	sprintf(strbuf, "%0.4f\r\n", ctime);
	memoTime->Lines->Strings[r] = strbuf;

	pbRun->Position = r+1;

	delete ssprAlg;
  }

  btnClose->Enabled = true;
  btnRun->Enabled = true;
  btnTerminate->Enabled = false;

  SetDefaults();
}
//---------------------------------------------------------------------------

