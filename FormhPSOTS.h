//---------------------------------------------------------------------------

#ifndef FormhPSOTSH
#define FormhPSOTSH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "FormTS.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "hPSOTS.h"
//---------------------------------------------------------------------------
class TfrmhPSOTS : public TfrmTS
{
__published:	// IDE-managed Components
	TEdit *edtnPSOIterations;
	TLabel *Label6;
	TEdit *edtnParticles;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TEdit *edtOmega;
	TEdit *edtDelta0;
	TEdit *edtc2;
	TEdit *edtc1;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnRunClick(TObject *Sender);
private:	// User declarations
	MOSP_hPSOTS *hPSOTS;

public:		// User declarations
	__fastcall TfrmhPSOTS(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmhPSOTS *frmhPSOTS;
//---------------------------------------------------------------------------
#endif
