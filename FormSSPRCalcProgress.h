//---------------------------------------------------------------------------

#ifndef FormSSPRCalcProgressH
#define FormSSPRCalcProgressH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "FormCalcProgress.h"
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------
class TfrmSSPRCalcProgress : public TfrmCalcProgress
{
__published:	// IDE-managed Components
	TPanel *pnlTabuSearch;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *lblTSCurrentCmax;
	TLabel *lblTSBestCmax;
	TLabel *lblTSIter;
	TLabel *Label4;
	TLabel *lblTSInitialCmax;
	TLabel *Label6;
	TLabel *lblTSImpIter;
	TLabel *Label7;
	TProgressBar *pbTabuSearch;
	TLabel *lblLB;
	TLabel *Label5;
	TLabel *Label10;
	TLabel *lblbestCmax;
	TPanel *pnlConstruction;
	TStringGrid *grdInitialSolutions;
	TLabel *Label8;
	TProgressBar *pbConstruction;
	TLabel *Label9;
	TLabel *Label11;
	TPanel *Panel1;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label14;
	TStringGrid *sgPRIterations;
	TProgressBar *pbPRProgress;
	TLabel *lblPRIter;
	TProgressBar *pbSSPRIterations;
	TLabel *Label15;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
    bool bActive;

	void ClearForm();

	__fastcall TfrmSSPRCalcProgress(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmSSPRCalcProgress *frmSSPRCalcProgress;
//---------------------------------------------------------------------------
#endif
