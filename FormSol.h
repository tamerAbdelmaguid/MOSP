//---------------------------------------------------------------------------

#ifndef FormSolH
#define FormSolH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "MOSPSolver.h"
#include <Chart.hpp>
#include <ExtCtrls.hpp>
#include <GanttCh.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <ArrowCha.hpp>
#include <TeeFunci.hpp>
#include "SList.h"
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <Grids.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TfrmSolution : public TForm
{
__published:	// IDE-managed Components
        TPageControl *pgSolution;
		TTabSheet *tbGantt;
        TTabSheet *tbTabular;
		TChart *chrtJSSPGantt;
	TGanttSeries *Gantt;
        TPanel *pnlOFs;
        TPageControl *pbTables;
        TTabSheet *tsMachines;
        TTabSheet *tsJobs;
        TLabel *Label1;
        TLabel *lblMakespan;
        TLabel *lblMeanFlowTime;
        TLabel *Label4;
        TLabel *lblMeanLateness;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *lblMeanTardiness;
        TLabel *lblMaxLateness;
        TLabel *Label10;
        TLabel *lblMaxTardiness;
		TLabel *Label12;
        TLabel *Label13;
        TLabel *lblNTardyJobs;
        TLabel *lblMWLateness;
        TLabel *Label16;
        TLabel *lblMWTardiness;
        TLabel *Label18;
        TLabel *lblMWFlowTime;
        TLabel *Label3;
        TSpeedButton *btnSaveSol;
        TSpeedButton *btnLoadSol;
        TStringGrid *grdMachineOps;
        TListBox *lstMachines;
        TLabel *Label2;
        TLabel *Label5;
        TListBox *lstJobs;
        TLabel *Label8;
        TLabel *Label9;
        TStringGrid *grdJobOps;
        TSaveDialog *dlgSaveSol;
        TOpenDialog *dlgLoadSol;
	TPopupMenu *mnuGANTT;
	TMenuItem *ExportWMF1;
	TSaveDialog *dlgSaveWMF;
	TArrowSeries *CPaths;
	TTabSheet *tbNetwork;
	TStringGrid *grdJobs;
	TStringGrid *grdMachines;
	TStringGrid *grdNetwork;
	TLabel *Label11;
	TLabel *Label14;
	TLabel *Label15;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall lstMachinesClick(TObject *Sender);
        void __fastcall lstJobsClick(TObject *Sender);
        void __fastcall btnSaveSolClick(TObject *Sender);
        void __fastcall btnLoadSolClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall ExportWMF1Click(TObject *Sender);
private:	// User declarations
		MOSP_Problem *Problem;
		MOSP_Solution *LoadedSol;
		MOSP_NSTester *nwAlg;
		MOSP_Network *Net;
		int Makespan;

public:		// User declarations
		MOSP_Solution *CurrentSol;
		bool bSuccessfulSolLoading;

		__fastcall TfrmSolution(TComponent* Owner, MOSP_Problem *P);

		void __fastcall DrawGantt();
		void __fastcall FillTables();
		void __fastcall DisplayMeasures();
		void __fastcall SetCurrentSolution(MOSP_Solution *Sol, MOSP_Network *nt=NULL);
		void __fastcall FillNetworkTables();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmSolution *frmSolution;
//---------------------------------------------------------------------------
#endif
