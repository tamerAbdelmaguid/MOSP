//---------------------------------------------------------------------------

#ifndef FormMattaProblemsH
#define FormMattaProblemsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------
class TfrmMattaProblems : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TComboBox *cbxNoOfPCs;
	TBitBtn *btnGenerate;
	TBitBtn *btnClose;
	TStringGrid *grdDataTable;
	TLabel *Label2;
	TEdit *edtnJobs;
	TEdit *edtProblemName;
	TLabel *Label3;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall cbxNoOfPCsChange(TObject *Sender);
	void __fastcall btnGenerateClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmMattaProblems(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMattaProblems *frmMattaProblems;
//---------------------------------------------------------------------------
#endif
