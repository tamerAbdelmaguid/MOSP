//----------------------------------------------------------------------------
#ifndef FormProblemH
#define FormProblemH
//----------------------------------------------------------------------------
#include <vcl\Controls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\Classes.hpp>
#include <vcl\Windows.hpp>
#include <vcl\System.hpp>
#include <StdCtrls.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "Problem.h"
#include <Grids.hpp>
//----------------------------------------------------------------------------
class TfrmProblem : public TForm
{
__published:
        TPageControl *Pages;
        TTabSheet *tsWCs;
        TTabSheet *tsJOs;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
	TEdit *edtWCID;
	TEdit *edtWCDescription;
	TSpeedButton *btnWCEnd;
	TSpeedButton *btnWCNext;
	TSpeedButton *btnWCPrev;
	TSpeedButton *btnWCStart;
	TSpeedButton *btnWCCancel;
	TSpeedButton *btnWCApprove;
	TBitBtn *btnWCModify;
	TBitBtn *btnWCDelete;
	TBitBtn *btnWCInsert;
	TBitBtn *btnWCAppend;
	TListBox *lstWCs;
		TLabel *Label4;
		TLabel *Label13;
		TLabel *Label14;
		TLabel *Label15;
		TSpeedButton *btnJOEnd;
		TSpeedButton *btnJONext;
		TSpeedButton *btnJOPrev;
		TSpeedButton *btnJOStart;
		TSpeedButton *btnJOCancel;
		TSpeedButton *btnJOApprove;
		TLabel *Label20;
		TLabel *Label21;
		TLabel *Label22;
		TListBox *lstJOs;
		TEdit *edtJOID;
		TEdit *edtJODescription;
		TBitBtn *btnJOInsert;
		TBitBtn *btnJOAppend;
		TBitBtn *btnJODelete;
		TBitBtn *btnJOModify;
		TPanel *pnlOperations;
		TLabel *Label16;
		TLabel *Label17;
		TLabel *Label18;
		TSpeedButton *btnOPEnd;
		TSpeedButton *btnOPNext;
		TSpeedButton *btnOPPrev;
		TSpeedButton *btnOPStart;
		TSpeedButton *btnOPCancel;
		TSpeedButton *btnOPApprove;
		TListBox *lstOPs;
		TEdit *edtOPID;
		TEdit *edtOPDescription;
		TBitBtn *btnOPInsert;
		TBitBtn *btnOPAppend;
		TBitBtn *btnOPDelete;
		TBitBtn *btnOPModify;
		TEdit *edtJOWeight;
		TEdit *edtJOReleaseTime;
		TEdit *edtJODueTime;
		TPanel *pnlMCs;
		TLabel *Label5;
		TLabel *Label6;
		TLabel *Label7;
		TSpeedButton *btnMCEnd;
		TSpeedButton *btnMCNext;
		TSpeedButton *btnMCPrev;
		TSpeedButton *btnMCStart;
		TSpeedButton *btnMCCancel;
		TSpeedButton *btnMCApprove;
		TLabel *Label9;
		TListBox *lstMCs;
		TEdit *edtMCID;
		TEdit *edtMCDescription;
		TBitBtn *btnMCInsert;
		TBitBtn *btnMCAppend;
		TBitBtn *btnMCDelete;
		TBitBtn *btnMCModify;
		TEdit *edtMCReadyTime;
		TLabel *lblnMCs;
		TLabel *Label19;
	TComboBox *cbxOPWC;
		TStringGrid *grdOPMC;
		TLabel *Label8;
		void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
		void __fastcall FormShow(TObject *Sender);
		void __fastcall FormActivate(TObject *Sender);
		void __fastcall PagesChange(TObject *Sender);
		void __fastcall btnWCStartClick(TObject *Sender);
		void __fastcall btnWCEndClick(TObject *Sender);
		void __fastcall btnWCNextClick(TObject *Sender);
		void __fastcall btnWCPrevClick(TObject *Sender);
		void __fastcall btnWCAppendClick(TObject *Sender);
		void __fastcall btnWCInsertClick(TObject *Sender);
		void __fastcall btnWCDeleteClick(TObject *Sender);
		void __fastcall btnWCModifyClick(TObject *Sender);
		void __fastcall btnWCCancelClick(TObject *Sender);
		void __fastcall btnWCApproveClick(TObject *Sender);
		void __fastcall btnJOStartClick(TObject *Sender);
		void __fastcall btnJOEndClick(TObject *Sender);
		void __fastcall btnJONextClick(TObject *Sender);
		void __fastcall btnJOPrevClick(TObject *Sender);
		void __fastcall btnJOAppendClick(TObject *Sender);
		void __fastcall btnJOInsertClick(TObject *Sender);
		void __fastcall btnJODeleteClick(TObject *Sender);
		void __fastcall btnJOModifyClick(TObject *Sender);
		void __fastcall btnJOApproveClick(TObject *Sender);
		void __fastcall btnJOCancelClick(TObject *Sender);
		void __fastcall btnOPAppendClick(TObject *Sender);
		void __fastcall btnOPInsertClick(TObject *Sender);
		void __fastcall btnOPDeleteClick(TObject *Sender);
		void __fastcall btnOPModifyClick(TObject *Sender);
		void __fastcall btnOPCancelClick(TObject *Sender);
		void __fastcall btnOPStartClick(TObject *Sender);
		void __fastcall btnOPEndClick(TObject *Sender);
		void __fastcall btnOPNextClick(TObject *Sender);
		void __fastcall btnOPPrevClick(TObject *Sender);
		void __fastcall btnOPApproveClick(TObject *Sender);
		void __fastcall lstWCsClick(TObject *Sender);
		void __fastcall lstJOsClick(TObject *Sender);
		void __fastcall lstOPsClick(TObject *Sender);
	void __fastcall lstMCsClick(TObject *Sender);
	void __fastcall btnMCAppendClick(TObject *Sender);
	void __fastcall btnMCInsertClick(TObject *Sender);
	void __fastcall btnMCDeleteClick(TObject *Sender);
	void __fastcall btnMCModifyClick(TObject *Sender);
	void __fastcall btnMCEndClick(TObject *Sender);
	void __fastcall btnMCStartClick(TObject *Sender);
	void __fastcall btnMCNextClick(TObject *Sender);
	void __fastcall btnMCPrevClick(TObject *Sender);
	void __fastcall btnMCApproveClick(TObject *Sender);
	void __fastcall btnMCCancelClick(TObject *Sender);
	void __fastcall cbxOPWCChange(TObject *Sender);
private:
public:
		virtual __fastcall TfrmProblem(TComponent *Owner, MOSP_Problem *p, String fn);

		MOSP_Problem *Problem;
		void __fastcall DisplayProblem();
		void __fastcall RedisplayProblem();
		void __fastcall DisplayWCs();
		void __fastcall DisplayCurrentWC();
		void __fastcall DisplayMCs();
		void __fastcall DisplayCurrentMC();
		void __fastcall DisplayJobs();
		void __fastcall DisplayCurrentJob();
		void __fastcall DisplayOps();
		void __fastcall DisplayCurrentOp();

		void __fastcall UpdateControls();
		void __fastcall UpdateWCsControls();
		void __fastcall UpdateMCsControls();
		void __fastcall UpdateJOsControls();
		void __fastcall UpdateOpsControls();

		void __fastcall LoadWCsList();

		enum EDT_Function {
		  FN_NAVIGATE,
		  FN_INSERT,
		  FN_APPEND,
		  FN_MODIFY,
		  FN_OPINSERT,
		  FN_OPAPPEND,
		  FN_OPMODIFY,
		  FN_MCINSERT,
		  FN_MCAPPEND,
		  FN_MCMODIFY
		} curFunction;

		int icurWC, icurMC, icurJob, icurOp;      // Indexes for current entities
												  // --> they have to be very well
												  //     controlled.

		bool bNeedToBeSaved;
		bool bFileNameGiven;
		String FileName;

		void __fastcall UpdateSavingStatus(bool b);
		__property bool NeedToBeSaved =
			   {read = bNeedToBeSaved, write = UpdateSavingStatus};
};
//----------------------------------------------------------------------------
#endif
