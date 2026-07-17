//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormSSPR.h"
#include "FormSol.h"
#include "SList.h"
#include "Problem.h"
#include "FormSSPRCalcProgress.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FormAlgorithm"
#pragma resource "*.dfm"
TfrmSSPR *frmSSPR;
//---------------------------------------------------------------------------
__fastcall TfrmSSPR::TfrmSSPR(TComponent* Owner)
	: TfrmAlgorithm(Owner)
{
#ifdef DEBUG_SSPR_RESULTS
  ssprDbgAlg = NULL;
#endif
  ssprInitDbgAlg = NULL;
  ssprAlg = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmSSPR::btnRunClick(TObject *)
{
  AnsiString s;
  int psz, Dth, tbsz, tsitr, tsimpitr, nbstmvs, nrndmvs, nitr, epsl;

  lblStatus->Caption = "";

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

  if(ssprAlg)  delete ssprAlg;
  ssprAlg = new MOSP_SSPR(Problem, psz, Dth, tbsz, tsitr, tsimpitr, nbstmvs, nrndmvs, nitr, epsl);
  frmSSPRCalcProgress->Show(ssprAlg);
  frmSolution->SetCurrentSolution(ssprAlg->Solution);
  lblTime->Caption = FloatToStr(ssprAlg->GetTime());

  double rpd = ssprAlg->Solution->ComputeObjectiveValue(MOSP_Problem::MIN_MAKESPAN);
  rpd -= ssprAlg->LB;
  rpd /= ((double) ssprAlg->LB);
  rpd *= 100.0;
  s = "RPD = " + FloatToStr(rpd);
  lblStatus->Caption = s;
}
//---------------------------------------------------------------------------
void __fastcall TfrmSSPR::btnLoadClick(TObject *)
{
  frmSolution->btnLoadSolClick(this);
#ifdef DEBUG_SSPR_RESULTS
  if(!frmSolution->bSuccessfulSolLoading) return;
  if(ssprDbgAlg){ ssprDbgAlg->FreeMemory();  delete ssprDbgAlg;  }
  ssprDbgAlg = new MOSP_SSPR(Problem);
  if(ssprDbgAlg->SetSeedSolution(frmSolution->CurrentSol)){
	frmSolution->SetCurrentSolution(ssprDbgAlg->Solution);

#ifdef PRNT_NSCB_DBG
	cbxOps->Enabled = true;
	cbxOps->Clear();
	for(int i=0; i<ssprDbgAlg->nOps; i++)
	   cbxOps->AddItem(ssprDbgAlg->O[i]->ID(), NULL);
	btnNSParameters->Enabled = true;
	cbxJobs->Enabled = true;
	for(int j=0; j<ssprDbgAlg->nJobs; j++)
	  cbxJobs->AddItem(ssprDbgAlg->J[j]->ID(), NULL);
	cbxFromMC->Enabled = true;
	for(int m=0; m<ssprDbgAlg->nMCs; m++)
	  cbxFromMC->AddItem(ssprDbgAlg->M[m]->ID(), NULL);
	btnB1Parameters->Enabled = true;
	btnB2Parameters->Enabled = true;
	btnEnumerate->Enabled = true;
	edtJobx1->Enabled = true;
	edtJobx2->Enabled = true;
	edtMCx1->Enabled = true;
	edtMCx2->Enabled = true;
#endif

	btnStartNSTest->Enabled = true;
	lstCurrentNSMoves->Clear();
	lstAppliedNSMoves->Clear();

	edtTStestTabuSize->Enabled = true;
	btnTStestStart->Enabled = true;
	lstCurrentnonTabuNSMoves->Clear();
	lstTabuNSMoves->Clear();
  }
#endif
}
//---------------------------------------------------------------------------
void __fastcall TfrmSSPR::FormShow(TObject *Sender)
{
  TfrmAlgorithm::FormShow(Sender);
  edtJobx1->Text = "";
  edtJobx2->Text = "";
  edtMCx1->Text = "";
  edtMCx2->Text = "";

  cbxOps->Enabled = false;
  btnNSParameters->Enabled = false;
  cbxJobs->Enabled = false;
  cbxFromMC->Enabled = false;
  btnB1Parameters->Enabled = false;
  btnB2Parameters->Enabled = false;
  btnEnumerate->Enabled = false;
  edtJobx1->Enabled = false;
  edtJobx2->Enabled = false;
  edtMCx1->Enabled = false;
  edtMCx2->Enabled = false;

  btnStartNSTest->Enabled = false;
  lstCurrentNSMoves->Enabled = false;
  btnApplyNSMove->Enabled = false;
  lstAppliedNSMoves->Enabled = false;

  lstCurrentNSMoves->Clear();
  lstAppliedNSMoves->Clear();

  lblLB1->Caption = "";
  lblLB2->Caption = "";
  lblLB3->Caption = "";
  lblLB->Caption = "";

  edtTStestTabuSize->Text = "5";
  edtTStestTabuSize->Enabled = false;
  btnTStestStart->Enabled = false;
  btnSuggestTabuSize->Enabled = true;
  lblTStestLB->Caption = "";
  btnTStestUpdateMoves->Enabled = false;
  btnTStestApplyMove->Enabled = false;
  lstCurrentnonTabuNSMoves->Enabled = false;
  lstTabuNSMoves->Enabled = false;

  edtPopSize->Text = "20";
  edtDeltaThreshold->Text = "";

  lblStatus->Caption = "";

  edtTabuSize->Text = "40";
  edtnTSIterations->Text = "300";
  edtnImpIterations->Text = "40";

  edtTSnBestMoves->Text = "10";
  edtTSnRandomMoves->Text = "10";
  edtnSSPRIterations->Text = "10";
  edtEpsilon->Text = "5";

  btnSuggestDeltaThresholdClick(Sender);

  lstTreatmentCombination->ItemIndex = -1;

#ifdef DEBUG_SSPR_RESULTS
  btnLoad->Enabled = true;
#endif
}
//---------------------------------------------------------------------------

void __fastcall TfrmSSPR::FormClose(TObject *Sender, TCloseAction &Action)
{
  TfrmAlgorithm::FormClose(Sender, Action);

#ifdef DEBUG_SSPR_RESULTS
  if(ssprDbgAlg){
	if(!btnTStestUpdateMoves->Enabled && btnTStestApplyMove->Enabled){ // just to avoid
	  if(ssprDbgAlg->ApplyNandUpdateTabuList(0) != -1){                // memory leakage
		frmSolution->SetCurrentSolution(ssprDbgAlg->Solution);         // and unknown
                                                                       // program halting !
		lstCurrentnonTabuNSMoves->Clear();
		lstCurrentnonTabuNSMoves->Enabled = false;
		btnTStestApplyMove->Enabled = false;
		btnTStestUpdateMoves->Enabled = true;
	  }
    }
	ssprDbgAlg->FreeMemory();
	delete ssprDbgAlg;
  }
  ssprDbgAlg = NULL;
#endif
  if(ssprInitDbgAlg)  delete ssprInitDbgAlg;    ssprInitDbgAlg = NULL;
  if(ssprAlg)  delete ssprAlg;    ssprAlg = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmSSPR::btnNSParametersClick(TObject *)
{
#ifdef PRNT_NSCB_DBG
  if(cbxOps->ItemIndex == -1) return;
  AnsiString opID;
  opID = cbxOps->Items->operator [](cbxOps->ItemIndex);
  MOSP_Problem::SOperation *v_y = Problem->OpByID(opID.c_str());
  if(v_y) ssprDbgAlg->GenerateNSParameters(v_y);
  else  Application->MessageBox(L"Unknown operation",
							L"Please select an operation from the drop down box",
							MB_OK);
#endif
}
//---------------------------------------------------------------------------

void __fastcall TfrmSSPR::btnB1ParametersClick(TObject *)
{
#ifdef PRNT_NSCB_DBG
  if(cbxJobs->ItemIndex == -1) return;
  AnsiString jbID;
  jbID = cbxJobs->Items->operator [](cbxJobs->ItemIndex);
  MOSP_Problem::SJob *Job = Problem->JObyID(jbID.c_str());
  if(Job){
    int x1, x2;
	try{
	  x1 = StrToInt(edtJobx1->Text);
	  x2 = StrToInt(edtJobx2->Text);
	} catch(EConvertError&){
		Application->MessageBox(L"Use appropriate integer values for the stard and"\
								" end positions",
								L"Error reading numbers",
								MB_OK);
		return;
	  }

	if(x1 > x2 || x1 < 1 || x2 < 1){
	  Application->MessageBox(L"Start position must be less than end position"\
							   " and they both must be greater than 0",
								L"Error in selecting positions",
								MB_OK);
	  return;
	}
	int l = Job->Operations.Count();
	if(x1 > l || x2 > l){
	  Application->MessageBox(L"start/end position is out of range",
							  L"Error in selecting positions",
							  MB_OK);
	  return;
	}

    ssprDbgAlg->GenerateB1Parameters(Job, x1, x2);
  }
  else  Application->MessageBox(L"Unknown Job",
							L"Please select a job from the drop down box",
							MB_OK);
#endif
}
//---------------------------------------------------------------------------

void __fastcall TfrmSSPR::btnB2ParametersClick(TObject *)
{
#ifdef PRNT_NSCB_DBG
  if(cbxFromMC->ItemIndex == -1) return;
  AnsiString mcID;
  mcID = cbxFromMC->Items->operator [](cbxFromMC->ItemIndex);
  MOSP_Problem::SMC *mc = Problem->MCbyID(mcID.c_str());
  if(mc){
    int x1, x2;
	try{
	  x1 = StrToInt(edtMCx1->Text);
	  x2 = StrToInt(edtMCx2->Text);
	} catch(EConvertError&){
		Application->MessageBox(L"Use appropriate integer values for the stard and"\
								" end positions",
								L"Error reading numbers",
								MB_OK);
		return;
	  }

	if(x1 > x2 || x1 < 1 || x2 < 1){
	  Application->MessageBox(L"Start position must be less than end position"\
							   " and they both must be greater than 0",
								L"Error in selecting positions",
								MB_OK);
	  return;
	}
	int l = ssprDbgAlg->nOpsByMCinSeedSol(mc->i);
	if(x1 > l || x2 > l){
	  Application->MessageBox(L"start/end position is out of range",
							  L"Error in selecting positions",
							  MB_OK);
	  return;
	}

    ssprDbgAlg->GenerateB2Parameters(mc, x1, x2);
  }
  else  Application->MessageBox(L"Unknown machine",
							L"Please select a machine from the drop down box",
							MB_OK);
#endif
}
//---------------------------------------------------------------------------

void __fastcall TfrmSSPR::btnEnumerateClick(TObject *)
{
#ifdef DEBUG_SSPR_RESULTS
   ssprDbgAlg->EnumerateMoves();
#endif
}
//---------------------------------------------------------------------------

void __fastcall TfrmSSPR::btnStartNSTestClick(TObject *)
{
#ifdef DEBUG_SSPR_RESULTS
  ssprDbgAlg->EnumerateMoves(chkStrictImprovement->Checked);
  lstCurrentNSMoves->Clear();

  char buf[256];
  for(int i=0; i<ssprDbgAlg->nMoves; i++){
	MOSP_SSPR::NSMove &mv = ssprDbgAlg->bestMoves[i];
	if(mv.F == MOSP_SSPR::N1){
	  sprintf(buf, "N1(%s, %i) -> ~Cmax = %i",
		  ssprDbgAlg->O[mv.v]->ID(), mv.x, mv.aprCmax);
	}
	else{
	  sprintf(buf, "N2(%s, %i, %s) -> ~Cmax = %i",
		  ssprDbgAlg->O[mv.v]->ID(), mv.x, ssprDbgAlg->M[mv.m2]->ID(), mv.aprCmax);
	}
	lstCurrentNSMoves->AddItem(buf, NULL);
  }

  lstCurrentNSMoves->Enabled = true;
  btnApplyNSMove->Enabled = true;
  lstAppliedNSMoves->Enabled = true;
#endif
}
//---------------------------------------------------------------------------

void __fastcall TfrmSSPR::btnApplyNSMoveClick(TObject *)
{
#ifdef DEBUG_SSPR_RESULTS
  int i = lstCurrentNSMoves->ItemIndex;
  if(i < 0 || i > ssprDbgAlg->nMoves) return;

  if(ssprDbgAlg->ApplyN(i) != -1){
	frmSolution->SetCurrentSolution(ssprDbgAlg->Solution);
	lstAppliedNSMoves->AddItem(lstCurrentNSMoves->Items->Strings[i], NULL);

	lstCurrentNSMoves->Clear();
	lstCurrentNSMoves->Enabled = false;
	btnApplyNSMove->Enabled = false;
  }
#endif
}
//---------------------------------------------------------------------------

void __fastcall TfrmSSPR::btnLBClick(TObject *)
{
  MOSP_SSPR *tmp_ssprDbgAlg = new MOSP_SSPR(Problem);
  tmp_ssprDbgAlg->AllocateMemory();
  int lb1, lb2, lb3, lb;
  lb = tmp_ssprDbgAlg->CalculateLowerBound(lb1, lb2, lb3);
  lblLB1->Caption = IntToStr(lb1);
  lblLB2->Caption = IntToStr(lb2);
  lblLB3->Caption = IntToStr(lb3);
  lblLB->Caption = IntToStr(lb);
  tmp_ssprDbgAlg->FreeMemory();
  delete tmp_ssprDbgAlg;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSSPR::btnSuggestDeltaThresholdClick(TObject *)
{
  MOSP_SSPR *tmp_ssprDbgAlg = new MOSP_SSPR(Problem);
  tmp_ssprDbgAlg->AllocateMemory();
  int Dth = tmp_ssprDbgAlg->SuggestDeltaThreshold();
  edtDeltaThreshold->Text = IntToStr(Dth);
  tmp_ssprDbgAlg->FreeMemory();
  delete tmp_ssprDbgAlg;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSSPR::btnConstructClick(TObject *)
{
  AnsiString s;
  int psz, Dth, tbsz, tsitr, tsimpitr, nbstmvs, nrndmvs;

  lblStatus->Caption = "";

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

  if(ssprInitDbgAlg)  delete ssprInitDbgAlg;

  ssprInitDbgAlg = new MOSP_SSPR(Problem, psz, Dth, tbsz, tsitr, tsimpitr, nbstmvs, nrndmvs);
  ssprInitDbgAlg->AllocateMemory();
  ssprInitDbgAlg->Initialize();
  ssprInitDbgAlg->Finalize();
  ssprInitDbgAlg->FreeMemory();

  frmSolution->SetCurrentSolution(ssprInitDbgAlg->Solution);

  s = AnsiString("Solutions are constructed.");
  lblStatus->Caption = s;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSSPR::btnTStestStartClick(TObject *)
{
#ifdef DEBUG_SSPR_RESULTS
  AnsiString s;
  int ts;
  try{
	s = edtTStestTabuSize->Text;
	ts = StrToInt(s);
  } catch(EConvertError&){
	  Application->MessageBox(L"Use appropriate numerical values",
						  L"Error reading numbers", MB_OK);
	  return;
	}

  if(ts <= 0){
	Application->MessageBox(L"Tabu size must be greater than zero.",
						  L"Error entered numbers", MB_OK);
	return;
  }

  lstCurrentnonTabuNSMoves->Enabled = true;
  btnTStestUpdateMoves->Enabled = true;
  lstTabuNSMoves->Enabled = true;
  lstCurrentnonTabuNSMoves->Clear();
  lstTabuNSMoves->Clear();

  int lb1, lb2, lb3, lb;
  lb = ssprDbgAlg->CalculateLowerBound(lb1, lb2, lb3);
  lblTStestLB->Caption = IntToStr(lb);

  ssprDbgAlg->SetTabuSize(ts);

  btnTStestStart->Enabled = false;
  btnSuggestTabuSize->Enabled = false;
#endif
}
//---------------------------------------------------------------------------

void __fastcall TfrmSSPR::btnTStestApplyMoveClick(TObject *)
{
#ifdef DEBUG_SSPR_RESULTS
  int i = lstCurrentnonTabuNSMoves->ItemIndex;
  if(i < 0 || i > ssprDbgAlg->nMoves) return;

  if(ssprDbgAlg->ApplyNandUpdateTabuList(i) != -1){
	frmSolution->SetCurrentSolution(ssprDbgAlg->Solution);

	lstCurrentnonTabuNSMoves->Clear();
	lstCurrentnonTabuNSMoves->Enabled = false;
	btnTStestApplyMove->Enabled = false;
	btnTStestUpdateMoves->Enabled = true;
  }

  // Make a list of all tabu moves
  lstTabuNSMoves->Clear();
  char buf[256];
  for(int i=0; i<ssprDbgAlg->nTabus; i++){
	MOSP_SSPR::NSMove &mv = ssprDbgAlg->TabuMoves[i];
	if(mv.F == MOSP_SSPR::N1){
	  sprintf(buf, "N1(%s, %i)   %s",
		  ssprDbgAlg->O[mv.v]->ID(), mv.x, ((ssprDbgAlg->TabuPntr == i+1)? "<-" : ""));
	}
	else{
	  sprintf(buf, "N2(%s, %i, %s)   %s",
		  ssprDbgAlg->O[mv.v]->ID(), mv.x, ssprDbgAlg->M[mv.m2]->ID(),
					  ((ssprDbgAlg->TabuPntr == i+1)? "<-" : ""));
	}
	lstTabuNSMoves->AddItem(buf, NULL);
  }
#endif
}
//---------------------------------------------------------------------------

void __fastcall TfrmSSPR::btnTStestUpdateMovesClick(TObject *)
{
#ifdef DEBUG_SSPR_RESULTS
  ssprDbgAlg->EnumerateNonTabuMoves(ssprDbgAlg->seedSol);
  lstCurrentnonTabuNSMoves->Clear();

  // Make a list of the best non-tabu moves
  char buf[256];
  for(int i=0; i<ssprDbgAlg->nMoves; i++){
	MOSP_SSPR::NSMove &mv = ssprDbgAlg->bestMoves[i];
	if(mv.F == MOSP_SSPR::N1){
	  sprintf(buf, "N1(%s, %i) -> ~Cmax = %i",
		  ssprDbgAlg->O[mv.v]->ID(), mv.x, mv.aprCmax);
	}
	else{
	  sprintf(buf, "N2(%s, %i, %s) -> ~Cmax = %i",
		  ssprDbgAlg->O[mv.v]->ID(), mv.x, ssprDbgAlg->M[mv.m2]->ID(), mv.aprCmax);
	}
	lstCurrentnonTabuNSMoves->AddItem(buf, NULL);
  }

  lstCurrentnonTabuNSMoves->Enabled = true;
  btnApplyNSMove->Enabled = true;
  btnTStestUpdateMoves->Enabled = false;
  btnTStestApplyMove->Enabled = true;
#endif
}
//---------------------------------------------------------------------------

void __fastcall TfrmSSPR::btnSuggestTabuSizeClick(TObject *)
{
  MOSP_SSPR *tmp_ssprDbgAlg = new MOSP_SSPR(Problem);
  tmp_ssprDbgAlg->AllocateMemory();
  int tsz = tmp_ssprDbgAlg->SuggestTabuSize();
  edtTStestTabuSize->Text = IntToStr(tsz);
  tmp_ssprDbgAlg->FreeMemory();
  delete tmp_ssprDbgAlg;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSSPR::btnSuggestMaxTabuSizeClick(TObject *)
{
  MOSP_SSPR *tmp_ssprDbgAlg = new MOSP_SSPR(Problem);
  tmp_ssprDbgAlg->AllocateMemory();
  int tsz = tmp_ssprDbgAlg->SuggestTabuSize();
  edtTabuSize->Text = IntToStr(tsz);
  tmp_ssprDbgAlg->FreeMemory();
  delete tmp_ssprDbgAlg;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSSPR::lstTreatmentCombinationClick(TObject *)
{
  int i = lstTreatmentCombination->ItemIndex;

  edtEpsilon->Text = IntToStr(Taguchi[i].B);
  edtTabuSize->Text = IntToStr(Taguchi[i].C);
  edtnImpIterations->Text = IntToStr(Taguchi[i].D);
  edtTSnBestMoves->Text = IntToStr(Taguchi[i].E);
  edtTSnRandomMoves->Text = IntToStr(Taguchi[i].F);

  edtPopSize->Text = "20";
  edtnTSIterations->Text = "300";
  edtnSSPRIterations->Text = "10";

  MOSP_SSPR *tmp_ssprDbgAlg = new MOSP_SSPR(Problem);
  tmp_ssprDbgAlg->AllocateMemory();
  int Dth = tmp_ssprDbgAlg->SuggestDeltaThreshold(Taguchi[i].A);
  edtDeltaThreshold->Text = IntToStr(Dth);
  tmp_ssprDbgAlg->FreeMemory();
  delete tmp_ssprDbgAlg;
}
//---------------------------------------------------------------------------

