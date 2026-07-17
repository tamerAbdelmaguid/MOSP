//---------------------------------------------------------------------------

#ifndef FormSSPRH
#define FormSSPRH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "FormAlgorithm.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "SSPR.h"
//---------------------------------------------------------------------------
class TfrmSSPR : public TfrmAlgorithm
{
__published:	// IDE-managed Components
	TPageControl *pgctrlSSPR;
	TTabSheet *tsSSPRNbrTest;
	TTabSheet *tsSSPRRun;
	TSpeedButton *btnSave;
	TComboBox *cbxOps;
	TLabel *Label15;
	TBitBtn *btnNSParameters;
	TEdit *edtMCx2;
	TEdit *edtMCx1;
	TComboBox *cbxFromMC;
	TLabel *Label7;
	TLabel *Label6;
	TLabel *Label1;
	TEdit *edtJobx2;
	TLabel *Label10;
	TEdit *edtJobx1;
	TLabel *Label9;
	TComboBox *cbxJobs;
	TLabel *Label2;
	TBitBtn *btnB1Parameters;
	TBitBtn *btnB2Parameters;
	TTabSheet *tsImprovementTest;
	TBitBtn *btnEnumerate;
	TSpeedButton *btnLoad;
	TLabel *Label3;
	TLabel *Label4;
	TSpeedButton *btnStartNSTest;
	TSpeedButton *btnApplyNSMove;
	TListBox *lstCurrentNSMoves;
	TListBox *lstAppliedNSMoves;
	TTabSheet *tsSSPRLB;
	TSpeedButton *btnLB;
	TLabel *Label8;
	TLabel *Label11;
	TLabel *lblLB1;
	TLabel *lblLB2;
	TLabel *Label12;
	TLabel *lblLB;
	TLabel *lblLB3;
	TLabel *Label14;
	TLabel *Label13;
	TEdit *edtPopSize;
	TLabel *Label16;
	TEdit *edtDeltaThreshold;
	TEdit *edtTabuSize;
	TLabel *Label17;
	TEdit *edtnTSIterations;
	TLabel *Label18;
	TEdit *edtnImpIterations;
	TLabel *Label19;
	TTabSheet *tsTabuSearch;
	TLabel *Label20;
	TEdit *edtTStestTabuSize;
	TLabel *Label21;
	TLabel *lblTStestLB;
	TSpeedButton *btnTStestStart;
	TListBox *lstCurrentnonTabuNSMoves;
	TLabel *Label22;
	TSpeedButton *btnTStestApplyMove;
	TListBox *lstTabuNSMoves;
	TLabel *Label23;
	TSpeedButton *btnTStestUpdateMoves;
	TButton *btnSuggestTabuSize;
	TCheckBox *chkStrictImprovement;
	TEdit *edtTSnBestMoves;
	TLabel *Label24;
	TEdit *edtTSnRandomMoves;
	TLabel *Label25;
	TEdit *edtnSSPRIterations;
	TLabel *Label26;
	TEdit *edtEpsilon;
	TLabel *Label27;
	TButton *btnConstruct;
	TLabel *lblStatus;
	TLabel *Label28;
	TListBox *lstTreatmentCombination;
	void __fastcall btnRunClick(TObject *Sender);
	void __fastcall btnLoadClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnNSParametersClick(TObject *Sender);
	void __fastcall btnB1ParametersClick(TObject *Sender);
	void __fastcall btnB2ParametersClick(TObject *Sender);
	void __fastcall btnEnumerateClick(TObject *Sender);
	void __fastcall btnStartNSTestClick(TObject *Sender);
	void __fastcall btnApplyNSMoveClick(TObject *Sender);
	void __fastcall btnLBClick(TObject *Sender);
	void __fastcall btnSuggestDeltaThresholdClick(TObject *Sender);
	void __fastcall btnConstructClick(TObject *Sender);
	void __fastcall btnTStestStartClick(TObject *Sender);
	void __fastcall btnTStestApplyMoveClick(TObject *Sender);
	void __fastcall btnTStestUpdateMovesClick(TObject *Sender);
	void __fastcall btnSuggestTabuSizeClick(TObject *Sender);
	void __fastcall btnSuggestMaxTabuSizeClick(TObject *Sender);
	void __fastcall lstTreatmentCombinationClick(TObject *Sender);
private:	// User declarations
#ifdef DEBUG_SSPR_RESULTS
	MOSP_SSPR *ssprDbgAlg;
#endif
	MOSP_SSPR *ssprInitDbgAlg;
	MOSP_SSPR *ssprAlg;

public:		// User declarations
	__fastcall TfrmSSPR(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmSSPR *frmSSPR;
//---------------------------------------------------------------------------
#endif
