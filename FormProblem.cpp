//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "FormProblem.h"
#include "Problem.h"
#include "FileSystem.h"
#include "Main.h"
//---------------------------------------------------------------------
#pragma resource "*.dfm"
//--------------------------------------------------------------------- 
__fastcall TfrmProblem::TfrmProblem(TComponent *Owner, MOSP_Problem *p, String fn)
	: TForm(Owner)
{
  Problem = p;
  FileName = fn;
  bFileNameGiven = (fn=="")? false : true;
  bNeedToBeSaved = !bFileNameGiven;
}
//--------------------------------------------------------------------- 
void __fastcall TfrmProblem::FormClose(TObject *, TCloseAction &Action)
{
  if(NeedToBeSaved){
    String s = "Problem " + Caption + " has been modified. Would you like to save it?";
	int response = Application->MessageBox((const wchar_t *)s.c_str(), L"Save query",  MB_YESNOCANCEL);
    if(response == IDYES){
      MOSP_FIO_RESULT Result;
	  SaveProblem(Problem, (char *)FileName.c_str(), Result);
      if(Result != SUCCESS){
        switch(Result){
          case FAIL_TO_OPEN:
            s = "failed to save file";
            break;
          case DATA_ERROR:
            s = "Error in data";
            break;
        }
		Application->MessageBox((const wchar_t *)s.c_str(), L"Error saving file", MB_OK);
        Action = caNone;
        return;
      }
    }
    else if(response == IDCANCEL){
      Action = caNone;
      return;
    }
  }
  delete Problem;       Problem = NULL;
  if(MDIChildCount == 0){
    MainForm->btnSave->Enabled = false;
    MainForm->FileSaveItem->Enabled = false;
    MainForm->FileSaveAsItem->Enabled = false;
    MainForm->Exportto->Enabled = false;
    MainForm->mnuSolve->Enabled = false;
    MainForm->CurrentChild = NULL;
  }
  Action = caFree;
}
//---------------------------------------------------------------------

void __fastcall TfrmProblem::FormShow(TObject *)
{
  icurWC = 0; icurMC = 0; icurJob = 0; icurOp = 0;
  Pages->ActivePageIndex = 0;
  grdOPMC->Cells[0][0] = "Machine ID";
  grdOPMC->Cells[1][0] = "Processing Time";
  DisplayProblem();
  curFunction = FN_NAVIGATE;
  UpdateControls();
}
//---------------------------------------------------------------------------
void __fastcall TfrmProblem::DisplayProblem()
{
  DisplayWCs();
  DisplayMCs();
  DisplayJobs();
  DisplayOps();
}

void __fastcall TfrmProblem::RedisplayProblem()
{
  switch(Pages->ActivePageIndex){
	case 0: // workcenters
        DisplayWCs();
		DisplayMCs();
	  break;

	case 1: // Job orders and operations
		DisplayJobs();
		DisplayOps();
	  break;
  }
}
//---------------------------------------------------------------------------
void __fastcall TfrmProblem::DisplayWCs()
{
  lstWCs->Clear();
  SList::SNode *pcnd = Problem->WCs.head();
  while(pcnd){
	MOSP_Problem::SWC *pc = (MOSP_Problem::SWC *)pcnd->Data();
	lstWCs->Items->Add(pc->ID());
	pcnd = pcnd->Next();
  }
  lstWCs->ItemIndex = icurWC;
  DisplayCurrentWC();
}

void __fastcall TfrmProblem::DisplayCurrentWC()
{
  if(Problem->WCs.Count() == 0){
	edtWCID->Text = "";
	edtWCDescription->Text = "";
	lblnMCs->Caption = "";
	return;
  }
  MOSP_Problem::SWC *curWC;
  curWC = (MOSP_Problem::SWC *)(Problem->WCs[icurWC]);
  edtWCID->Text = curWC->ID();
  edtWCDescription->Text = curWC->Description();
  lblnMCs->Caption = IntToStr(curWC->MCs.Count());

  DisplayMCs();
}
//---------------------------------------------------------------------------
void __fastcall TfrmProblem::DisplayMCs()
{
  if(Problem->WCs.Count() == 0){
	lstMCs->Clear();
	edtMCID->Text = "";
	edtMCDescription->Text = "";
	edtMCReadyTime->Text = "";
	return;
  }

  lstMCs->Clear();
  MOSP_Problem::SWC *curWC = (MOSP_Problem::SWC *)(Problem->WCs[icurWC]);
  SList::SNode *mcnd = curWC->MCs.head();
  while(mcnd){
	MOSP_Problem::SMC *mc = (MOSP_Problem::SMC *)mcnd->Data();
	lstMCs->Items->Add(mc->ID());
	mcnd = mcnd->Next();
  }
  lstMCs->ItemIndex = icurMC;
  DisplayCurrentMC();
}

void __fastcall TfrmProblem::DisplayCurrentMC()
{
  if(Problem->WCs.Count() == 0){
	lstMCs->Clear();
	edtMCID->Text = "";
	edtMCDescription->Text = "";
	edtMCReadyTime->Text = "";
	return;
  }

  MOSP_Problem::SWC *curWC = (MOSP_Problem::SWC *)(Problem->WCs[icurWC]);
  if(curWC->MCs.Count() == 0){
	edtMCID->Text = "";
	edtMCDescription->Text = "";
	edtMCReadyTime->Text = "";
	return;
  }

  MOSP_Problem::SMC *curMC = (MOSP_Problem::SMC *)(curWC->MCs[icurMC]);
  edtMCID->Text = curMC->ID();
  edtMCDescription->Text = curMC->Description();
  edtMCReadyTime->Text = FloatToStr(curMC->ReadyTime);
}
//---------------------------------------------------------------------------
void __fastcall TfrmProblem::DisplayJobs()
{
  if(Problem->Jobs.Count() == 0){
	lstJOs->Clear();
	edtJOID->Text = "";
	edtJODescription->Text = "";
	edtJOWeight->Text = "";
	edtJOReleaseTime->Text = "";
	edtJODueTime->Text = "";

	lstOPs->Clear();
	edtOPID->Text = "";
	edtOPDescription->Text = "";
	cbxOPWC->Clear();
	grdOPMC->RowCount = 2;
	grdOPMC->FixedRows = 1;
	grdOPMC->Cells[0][1] = "";
	grdOPMC->Cells[1][1] = "";
	return;
  }

  lstJOs->Clear();
  SList::SNode *jbnd = Problem->Jobs.head();
  while(jbnd){
	MOSP_Problem::SJob *jb = (MOSP_Problem::SJob *)jbnd->Data();
	lstJOs->Items->Add(jb->ID());
	jbnd = jbnd->Next();
  }

  lstJOs->ItemIndex = icurJob;
  DisplayCurrentJob();
}

void __fastcall TfrmProblem::DisplayCurrentJob()
{
  if(Problem->Jobs.Count() == 0){
	lstJOs->Clear();
	edtJOID->Text = "";
	edtJODescription->Text = "";
	edtJOWeight->Text = "";
	edtJOReleaseTime->Text = "";
	edtJODueTime->Text = "";

	lstOPs->Clear();
	edtOPID->Text = "";
	edtOPDescription->Text = "";
	cbxOPWC->Clear();
	grdOPMC->RowCount = 2;
	grdOPMC->FixedRows = 1;
	grdOPMC->Cells[0][1] = "";
	grdOPMC->Cells[1][1] = "";

	return;
  }
  MOSP_Problem::SJob *curJob = (MOSP_Problem::SJob *)(Problem->Jobs[icurJob]);

  edtJOID->Text = AnsiString(curJob->ID());
  edtJODescription->Text = AnsiString(curJob->Description());
  edtJOWeight->Text = AnsiString(curJob->Weight);
  edtJOReleaseTime->Text = AnsiString(curJob->ReleaseTime);
  edtJODueTime->Text = AnsiString(curJob->DueDate);

  DisplayOps();
}
//---------------------------------------------------------------------------
void __fastcall TfrmProblem::DisplayOps()
{
  if(Problem->Jobs.Count() == 0){
	lstOPs->Clear();
	edtOPID->Text = "";
	edtOPDescription->Text = "";
	cbxOPWC->Clear();
	grdOPMC->RowCount = 2;
	grdOPMC->FixedRows = 1;
	grdOPMC->Cells[0][1] = "";
	grdOPMC->Cells[1][1] = "";
	return;
  }

  lstOPs->Clear();
  MOSP_Problem::SJob *curJob = (MOSP_Problem::SJob *)(Problem->Jobs[icurJob]);
  SList::SNode *opnd = curJob->Operations.head();
  while(opnd){
	MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)opnd->Data();
	lstOPs->Items->Add(op->ID());
	opnd = opnd->Next();
  }

  lstOPs->ItemIndex = icurOp;
  DisplayCurrentOp();
}

void __fastcall TfrmProblem::DisplayCurrentOp()
{
  if(Problem->Jobs.Count() == 0){
	lstOPs->Clear();
	edtOPID->Text = "";
	edtOPDescription->Text = "";
	cbxOPWC->Clear();
	grdOPMC->RowCount = 2;
	grdOPMC->FixedRows = 1;
	grdOPMC->Cells[0][1] = "";
	grdOPMC->Cells[1][1] = "";
	return;
  }
  MOSP_Problem::SJob *curJob = (MOSP_Problem::SJob *)(Problem->Jobs[icurJob]);

  if(curJob->Operations.Count() == 0){
	lstOPs->Clear();
	edtOPID->Text = "";
	edtOPDescription->Text = "";
	cbxOPWC->Clear();
	grdOPMC->RowCount = 2;
	grdOPMC->FixedRows = 1;
	grdOPMC->Cells[0][1] = "";
	grdOPMC->Cells[1][1] = "";
	return;
  }

  MOSP_Problem::SOperation *curOp =
		 (MOSP_Problem::SOperation *)(curJob->Operations[icurOp]);
  edtOPID->Text = curOp->ID();
  edtOPDescription->Text = curOp->Description();

  // Add all allowed WCs in the list to permit choosing from them later
  cbxOPWC->Clear();
  SList::SNode *pcnd = Problem->WCs.head();
  while(pcnd){
	MOSP_Problem::SWC *pc = (MOSP_Problem::SWC *)pcnd->Data();

	bool bAdd = true;
	SList::SNode *opnd = curJob->Operations.head();
	while(opnd){
	  MOSP_Problem::SOperation *op = (MOSP_Problem::SOperation *)opnd->Data();
	  if(op != curOp && op->WC == pc){ bAdd = false;   break; }
	  opnd = opnd->Next();
	}
	if(bAdd) cbxOPWC->Items->Add(pc->ID());

	pcnd = pcnd->Next();
  }

  // Select current pc from the list
  int ipc=-1;
  AnsiString pcid = curOp->WC->ID();
  for(int i=0; i<cbxOPWC->Items->Count; i++)
	if(pcid == cbxOPWC->Items->Strings[i]){
	  ipc = i;
	  break;
	}
  cbxOPWC->ItemIndex = ipc;

  grdOPMC->RowCount = curOp->AltMCs.Count() + 1;
  if(grdOPMC->RowCount == 1){
	grdOPMC->RowCount = 2;
	grdOPMC->Cells[0][1] = "";
	grdOPMC->Cells[1][1] = "";
	grdOPMC->FixedRows = 1;
	return;
  }
  grdOPMC->FixedRows = 1;
  SList::SNode *mcnd = curOp->AltMCs.head();
  int r = 1;
  while(mcnd){
	MOSP_Problem::SOpMC *opmc = (MOSP_Problem::SOpMC *)mcnd->Data();
	grdOPMC->Cells[0][r] = opmc->MC->ID();
	grdOPMC->Cells[1][r] = FloatToStr(opmc->pt);
	mcnd = mcnd->Next();
	r++;
  }
}

//---------------------------------------------------------------------------
void __fastcall TfrmProblem::UpdateControls()
{
  switch(Pages->ActivePageIndex){
	case 0: // workcenters
		UpdateWCsControls();
	  break;

	case 1: // Job orders and operations
		UpdateJOsControls();
	  break;
  }
}

void __fastcall TfrmProblem::UpdateWCsControls()
{
  int n = Problem->WCs.Count();

  switch(curFunction){
	 case FN_INSERT:
	 case FN_APPEND:
	 case FN_MODIFY:
		 btnWCInsert->Enabled = false;
		 btnWCAppend->Enabled = false;
		 btnWCDelete->Enabled = false;
		 btnWCModify->Enabled = false;
		 btnWCApprove->Visible = true;
		 btnWCCancel->Visible = true;
		 btnWCEnd->Visible = false;
		 btnWCStart->Visible = false;
		 btnWCNext->Visible = false;
		 btnWCPrev->Visible = false;
		 edtWCID->Enabled = true;
		 edtWCDescription->Enabled = true;
		 lstWCs->Enabled = false;
	   break;

	 case FN_NAVIGATE:
		 btnWCInsert->Enabled = true;
		 btnWCAppend->Enabled = true;
		 btnWCDelete->Enabled = true;
		 btnWCModify->Enabled = true;
		 btnWCApprove->Visible = false;
		 btnWCCancel->Visible = false;
		 btnWCEnd->Visible = true;
		 btnWCStart->Visible = true;
		 btnWCNext->Visible = true;
		 btnWCPrev->Visible = true;
		 edtWCID->Enabled = false;
		 edtWCDescription->Enabled = false;
		 lstWCs->Enabled = true;

		 if(n==0){
		   btnWCDelete->Enabled = false;
		   btnWCModify->Enabled = false;
		 }
		 if(n==0 || n==1){
		   btnWCPrev->Enabled = false;
		   btnWCStart->Enabled = false;
		   btnWCNext->Enabled = false;
		   btnWCEnd->Enabled = false;
		 }
		 else{
		   if(icurWC == 0){
			 btnWCPrev->Enabled = false;
			 btnWCStart->Enabled = false;
			 btnWCNext->Enabled = true;
			 btnWCEnd->Enabled = true;
		   }
		   else if(icurWC == n-1){
			 btnWCPrev->Enabled = true;
			 btnWCStart->Enabled = true;
			 btnWCNext->Enabled = false;
			 btnWCEnd->Enabled = false;
		   }
		   else{
			 btnWCPrev->Enabled = true;
			 btnWCStart->Enabled = true;
			 btnWCNext->Enabled = true;
			 btnWCEnd->Enabled = true;
		   }
		 }
	   break;
  }
  UpdateMCsControls();
}

void __fastcall TfrmProblem::UpdateMCsControls()
{
  MOSP_Problem::SWC *curWC;
  int n;
  if(Problem->WCs.Count() == 0) n = -1;
  else{
	curWC = (MOSP_Problem::SWC *)(Problem->WCs[icurWC]);
	n = curWC->MCs.Count();
  }

  switch(curFunction){
	 case FN_INSERT:
	 case FN_APPEND:
	 case FN_MODIFY:
		 btnMCInsert->Enabled = false;
		 btnMCAppend->Enabled = false;
		 btnMCDelete->Enabled = false;
		 btnMCModify->Enabled = false;
		 btnMCApprove->Visible = false;
		 btnMCCancel->Visible = false;
		 btnMCEnd->Visible = true;
		 btnMCStart->Visible = true;
		 btnMCNext->Visible = true;
		 btnMCPrev->Visible = true;
		 btnMCEnd->Enabled = false;
		 btnMCStart->Enabled = false;
		 btnMCNext->Enabled = false;
		 btnMCPrev->Enabled = false;
		 edtMCID->Enabled = false;
		 edtMCDescription->Enabled = false;
		 edtMCReadyTime->Enabled = false;
		 lstMCs->Enabled = true;
	   break;

	 case FN_MCINSERT:
	 case FN_MCAPPEND:
	 case FN_MCMODIFY:
		 btnMCInsert->Enabled = false;
		 btnMCAppend->Enabled = false;
		 btnMCDelete->Enabled = false;
		 btnMCModify->Enabled = false;
		 btnMCApprove->Visible = true;
		 btnMCCancel->Visible = true;
		 btnMCEnd->Visible = false;
		 btnMCStart->Visible = false;
		 btnMCNext->Visible = false;
		 btnMCPrev->Visible = false;

		 if(curFunction==FN_MCMODIFY){
		   edtMCID->Enabled = false;
		 }
		 else{
		   edtMCID->Enabled = true;
		 }
		 edtMCDescription->Enabled = true;
		 edtMCReadyTime->Enabled = true;
		 lstMCs->Enabled = false;

		 btnWCInsert->Enabled = false;
		 btnWCAppend->Enabled = false;
		 btnWCDelete->Enabled = false;
		 btnWCModify->Enabled = false;
		 btnWCApprove->Visible = false;
		 btnWCCancel->Visible = false;
		 btnWCEnd->Visible = true;
		 btnWCStart->Visible = true;
		 btnWCNext->Visible = true;
		 btnWCPrev->Visible = true;
		 edtWCID->Enabled = false;
		 edtWCDescription->Enabled = false;
		 lstWCs->Enabled = false;

		 btnWCPrev->Enabled = false;
		 btnWCStart->Enabled = false;
		 btnWCNext->Enabled = false;
		 btnWCEnd->Enabled = false;
	   break;

	 case FN_NAVIGATE:
		 btnMCInsert->Enabled = true;
		 btnMCAppend->Enabled = true;
		 btnMCDelete->Enabled = true;
		 btnMCModify->Enabled = true;
		 btnMCApprove->Visible = false;
		 btnMCCancel->Visible = false;
		 btnMCEnd->Visible = true;
		 btnMCStart->Visible = true;
		 btnMCNext->Visible = true;
		 btnMCPrev->Visible = true;
		 edtMCID->Enabled = false;
		 edtMCDescription->Enabled = false;
		 edtMCReadyTime->Enabled = false;
		 lstMCs->Enabled = true;

		 if(n==-1){
		   btnMCInsert->Enabled = false;
		   btnMCAppend->Enabled = false;
		   btnMCDelete->Enabled = false;
		   btnMCModify->Enabled = false;
		 }
		 else if(n==0){
		   btnMCDelete->Enabled = false;
		   btnMCModify->Enabled = false;
		 }
		 if(n<=1){
		   btnMCPrev->Enabled = false;
		   btnMCStart->Enabled = false;
		   btnMCNext->Enabled = false;
		   btnMCEnd->Enabled = false;
		 }
		 else{
		   if(icurMC == 0){
			 btnMCPrev->Enabled = false;
			 btnMCStart->Enabled = false;
			 btnMCNext->Enabled = true;
			 btnMCEnd->Enabled = true;
		   }
		   else if(icurMC == n-1){
			 btnMCPrev->Enabled = true;
			 btnMCStart->Enabled = true;
			 btnMCNext->Enabled = false;
			 btnMCEnd->Enabled = false;
		   }
		   else{
			 btnMCPrev->Enabled = true;
			 btnMCStart->Enabled = true;
			 btnMCNext->Enabled = true;
			 btnMCEnd->Enabled = true;
		   }
		 }
	   break;
  }
}

void __fastcall TfrmProblem::UpdateJOsControls()
{
  int n = Problem->Jobs.Count();

  switch(curFunction){
	 case FN_INSERT:
	 case FN_APPEND:
	 case FN_MODIFY:
		 btnJOInsert->Enabled = false;
		 btnJOAppend->Enabled = false;
		 btnJODelete->Enabled = false;
		 btnJOModify->Enabled = false;
		 btnJOApprove->Visible = true;
		 btnJOCancel->Visible = true;
		 btnJOEnd->Visible = false;
		 btnJOStart->Visible = false;
		 btnJONext->Visible = false;
		 btnJOPrev->Visible = false;
		 edtJOID->Enabled = true;
		 edtJODescription->Enabled = true;
		 edtJOWeight->Enabled = true;
		 edtJOReleaseTime->Enabled = true;
		 edtJODueTime->Enabled = true;
		 lstJOs->Enabled = false;
	   break;

	 case FN_NAVIGATE:
		 btnJOInsert->Enabled = true;
		 btnJOAppend->Enabled = true;
		 btnJODelete->Enabled = true;
		 btnJOModify->Enabled = true;
		 btnJOApprove->Visible = false;
         btnJOCancel->Visible = false;
         btnJOEnd->Visible = true;
         btnJOStart->Visible = true;
         btnJONext->Visible = true;
         btnJOPrev->Visible = true;
         edtJOID->Enabled = false;
         edtJODescription->Enabled = false;
		 edtJOWeight->Enabled = false;
         edtJOReleaseTime->Enabled = false;
         edtJODueTime->Enabled = false;
		 lstJOs->Enabled = true;

         if(n==0){
           btnJODelete->Enabled = false;
           btnJOModify->Enabled = false;
         }
         if(n==0 || n==1){
           btnJOPrev->Enabled = false;
           btnJOStart->Enabled = false;
           btnJONext->Enabled = false;
           btnJOEnd->Enabled = false;
         }
         else{
		   if(icurJob == 0){
             btnJOPrev->Enabled = false;
             btnJOStart->Enabled = false;
             btnJONext->Enabled = true;
             btnJOEnd->Enabled = true;
           }
		   else if(icurJob == n-1){
             btnJOPrev->Enabled = true;
             btnJOStart->Enabled = true;
             btnJONext->Enabled = false;
             btnJOEnd->Enabled = false;
           }
           else{
             btnJOPrev->Enabled = true;
             btnJOStart->Enabled = true;
             btnJONext->Enabled = true;
             btnJOEnd->Enabled = true;
           }
         }
       break;
  }
  UpdateOpsControls();
}

void __fastcall TfrmProblem::UpdateOpsControls()
{
  MOSP_Problem::SJob *curJob;
  int n;
  if(Problem->Jobs.Count() == 0) n = -1;
  else{
	curJob = (MOSP_Problem::SJob *)(Problem->Jobs[icurJob]);
	n = curJob->Operations.Count();
  }

  switch(curFunction){
     case FN_INSERT:
     case FN_APPEND:
	 case FN_MODIFY:
         btnOPInsert->Enabled = false;
         btnOPAppend->Enabled = false;
         btnOPDelete->Enabled = false;
         btnOPModify->Enabled = false;
         btnOPApprove->Visible = false;
		 btnOPCancel->Visible = false;
         btnOPEnd->Visible = true;
         btnOPStart->Visible = true;
         btnOPNext->Visible = true;
         btnOPPrev->Visible = true;
         btnOPEnd->Enabled = false;
         btnOPStart->Enabled = false;
         btnOPNext->Enabled = false;
         btnOPPrev->Enabled = false;
         edtOPID->Enabled = false;
         edtOPDescription->Enabled = false;
         cbxOPWC->Enabled = false;
		 grdOPMC->Enabled = false;
		 lstOPs->Enabled = true;
       break;

	 case FN_OPINSERT:
	 case FN_OPAPPEND:
	 case FN_OPMODIFY:
		 btnOPInsert->Enabled = false;
		 btnOPAppend->Enabled = false;
		 btnOPDelete->Enabled = false;
		 btnOPModify->Enabled = false;
		 btnOPApprove->Visible = true;
		 btnOPCancel->Visible = true;
		 btnOPEnd->Visible = false;
		 btnOPStart->Visible = false;
		 btnOPNext->Visible = false;
		 btnOPPrev->Visible = false;

		 if(curFunction==FN_OPMODIFY){
		   edtOPID->Enabled = false;
		 }
		 else{
		   edtOPID->Enabled = true;
		 }
		 edtOPDescription->Enabled = true;
		 cbxOPWC->Enabled = true;
		 grdOPMC->Enabled = true;
		 lstOPs->Enabled = false;

		 btnJOInsert->Enabled = false;
		 btnJOAppend->Enabled = false;
		 btnJODelete->Enabled = false;
		 btnJOModify->Enabled = false;
		 btnJOApprove->Visible = false;
		 btnJOCancel->Visible = false;
		 btnJOEnd->Visible = true;
		 btnJOStart->Visible = true;
		 btnJONext->Visible = true;
		 btnJOPrev->Visible = true;
		 edtJOID->Enabled = false;
		 edtJODescription->Enabled = false;
		 edtJOWeight->Enabled = false;
		 edtJOReleaseTime->Enabled = false;
		 edtJODueTime->Enabled = false;
		 lstJOs->Enabled = false;

		 btnJOPrev->Enabled = false;
		 btnJOStart->Enabled = false;
		 btnJONext->Enabled = false;
		 btnJOEnd->Enabled = false;
	   break;

	 case FN_NAVIGATE:
		 btnOPInsert->Enabled = true;
		 btnOPAppend->Enabled = true;
		 btnOPDelete->Enabled = true;
		 btnOPModify->Enabled = true;
         btnOPApprove->Visible = false;
		 btnOPCancel->Visible = false;
         btnOPEnd->Visible = true;
         btnOPStart->Visible = true;
         btnOPNext->Visible = true;
         btnOPPrev->Visible = true;
         edtOPID->Enabled = false;
         edtOPDescription->Enabled = false;
		 cbxOPWC->Enabled = false;
		 grdOPMC->Enabled = false;
         lstOPs->Enabled = true;

         if(n==-1){
           btnOPInsert->Enabled = false;
           btnOPAppend->Enabled = false;
           btnOPDelete->Enabled = false;
           btnOPModify->Enabled = false;
         }
         else if(n==0){
           btnOPDelete->Enabled = false;
		   btnOPModify->Enabled = false;
         }
         if(n<=1){
           btnOPPrev->Enabled = false;
           btnOPStart->Enabled = false;
           btnOPNext->Enabled = false;
           btnOPEnd->Enabled = false;
         }
         else{
		   int npcs = Problem->WCs.Count();
		   if(n == npcs){
			 btnOPInsert->Enabled = false;
			 btnOPAppend->Enabled = false;
		   }

		   if(icurOp == 0){
             btnOPPrev->Enabled = false;
             btnOPStart->Enabled = false;
             btnOPNext->Enabled = true;
             btnOPEnd->Enabled = true;
           }
           else if(icurOp == n-1){
             btnOPPrev->Enabled = true;
             btnOPStart->Enabled = true;
             btnOPNext->Enabled = false;
             btnOPEnd->Enabled = false;
           }
           else{
			 btnOPPrev->Enabled = true;
             btnOPStart->Enabled = true;
             btnOPNext->Enabled = true;
             btnOPEnd->Enabled = true;
           }
         }
       break;
  }
}

//---------------------------------------------------------------------------

void __fastcall TfrmProblem::FormActivate(TObject *)
{
  if(bNeedToBeSaved){
    MainForm->btnSave->Enabled = true;
    MainForm->FileSaveItem->Enabled = true;
  }
  else{
    MainForm->btnSave->Enabled = false;
    MainForm->FileSaveItem->Enabled = false;
  }
  MainForm->FileSaveAsItem->Enabled = true;
  MainForm->Exportto->Enabled = true;
  MainForm->mnuSolve->Enabled = true;
  MainForm->CurrentChild = this;
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::UpdateSavingStatus(bool b)
{
  bNeedToBeSaved = b;
  MainForm->btnSave->Enabled = b;
  MainForm->FileSaveItem->Enabled = b;
  MainForm->FileSaveAsItem->Enabled = true;
  MainForm->Exportto->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::PagesChange(TObject *)
{
  curFunction = FN_NAVIGATE;
  RedisplayProblem();
  UpdateControls();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnWCStartClick(TObject *)
{
  icurWC = 0;
  icurMC = 0;
  lstWCs->ItemIndex = icurMC;
  DisplayCurrentWC();
  UpdateWCsControls();
}
//---------------------------------------------------------------------------


void __fastcall TfrmProblem::btnWCEndClick(TObject *)
{
  icurWC = Problem->WCs.Count()-1;
  icurMC = 0;
  lstWCs->ItemIndex = icurWC;
  DisplayCurrentWC();
  UpdateWCsControls();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnWCNextClick(TObject *)
{
  lstWCs->ItemIndex = ++icurWC;
  icurMC = 0;
  DisplayCurrentWC();
  UpdateWCsControls();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnWCPrevClick(TObject *)
{
  lstWCs->ItemIndex = --icurWC;
  icurMC = 0;
  DisplayCurrentWC();
  UpdateWCsControls();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnWCAppendClick(TObject *)
{
  curFunction = FN_APPEND;
  UpdateWCsControls();
  edtWCID->Text = "";
  edtWCDescription->Text = "";
  lblnMCs->Caption = "";

  lstMCs->Clear();
  edtMCID->Text = "";
  edtMCDescription->Text = "";
  edtMCReadyTime->Text = "";

  edtWCID->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnWCInsertClick(TObject *)
{
  curFunction = FN_INSERT;
  UpdateWCsControls();
  edtWCID->Text = "";
  edtWCDescription->Text = "";
  lblnMCs->Caption = "";

  lstMCs->Clear();
  edtMCID->Text = "";
  edtMCDescription->Text = "";
  edtMCReadyTime->Text = "";

  edtWCID->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnWCDeleteClick(TObject *)
{
  MOSP_Problem::SWC *curWC = (MOSP_Problem::SWC *)(Problem->WCs[icurWC]);
  if(!Problem->RemoveWC(curWC)){
	Application->MessageBox(L"Couldn't remove WC, check for if it is used by any job",
							L"Error removing WC/L", MB_OK);
	return;
  }
  if(icurWC >= Problem->WCs.Count()) icurWC = Problem->WCs.Count() - 1;
  icurMC = 0;
  DisplayWCs();
  UpdateWCsControls();
  NeedToBeSaved = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnWCModifyClick(TObject *)
{
  curFunction = FN_MODIFY;
  UpdateWCsControls();
  edtWCDescription->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnWCCancelClick(TObject *)
{
  curFunction = FN_NAVIGATE;
  UpdateWCsControls();
  DisplayCurrentWC();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnWCApproveClick(TObject *)
{
  AnsiString id, des;
  id = edtWCID->Text;
  des = edtWCDescription->Text;

  if(id == ""){
	Application->MessageBox(L"ID should be defined", L"Error adding/editing WC", MB_OK);
	return;
  }

  MOSP_Problem::SWC *curWC;

  switch(curFunction){
    case FN_APPEND:
		if(!Problem->AddWC((char *)id.c_str(), (char *)des.c_str())){
		  Application->MessageBox(L"Couldn't add new WC, check for ID duplication",
								  L"Error adding WC", MB_OK);
		  return;
		}
		icurWC = Problem->WCs.Count() - 1;
		lstWCs->Items->Add(id);
		lstWCs->ItemIndex = icurWC;
		curFunction = FN_NAVIGATE;
		UpdateWCsControls();
        DisplayCurrentWC();
      break;

    case FN_INSERT:
		if(!Problem->InsertWC(icurWC, (char *)id.c_str(), (char *)des.c_str())){
		  Application->MessageBox(L"Couldn't add new WC, check for ID duplication",
								  L"Error adding WC", MB_OK);
		  return;
		}
		lstWCs->Items->Insert(icurWC, id);
		lstWCs->ItemIndex = icurWC;
		curFunction = FN_NAVIGATE;
		UpdateWCsControls();
		DisplayCurrentWC();
	  break;

	case FN_MODIFY:
		curWC = (MOSP_Problem::SWC *)(Problem->WCs[icurWC]);
		if(!Problem->ModifyWC(curWC, (char *)id.c_str(), (char *)des.c_str())){
		  Application->MessageBox(L"Couldn't modify WC, check for ID duplication",
								  L"Error modifying WC", MB_OK);
		  return;
		}
		lstWCs->Items->Strings[icurWC] = id;
		curFunction = FN_NAVIGATE;
		UpdateWCsControls();
        DisplayCurrentWC();
      break;
  }
  NeedToBeSaved = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnJOStartClick(TObject *)
{
  icurJob = 0;
  icurOp = 0;
  lstJOs->ItemIndex = icurJob;
  DisplayCurrentJob();
  UpdateJOsControls();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnJOEndClick(TObject *)
{
  icurJob = Problem->Jobs.Count()-1;
  icurOp = 0;
  lstJOs->ItemIndex = icurJob;
  DisplayCurrentJob();
  UpdateJOsControls();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnJONextClick(TObject *)
{
  ++icurJob;
  icurOp = 0;
  lstJOs->ItemIndex = icurJob;
  DisplayCurrentJob();
  UpdateJOsControls();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnJOPrevClick(TObject *)
{
  --icurJob;
  icurOp = 0;
  lstJOs->ItemIndex = icurJob;
  DisplayCurrentJob();
  UpdateJOsControls();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnJOAppendClick(TObject *)
{
  curFunction = FN_APPEND;
  UpdateJOsControls();
  edtJOID->Text = "";
  edtJODescription->Text = "";
  edtJOWeight->Text = "1.0";
  edtJOReleaseTime->Text = "0";
  edtJODueTime->Text = "0";

  lstOPs->Clear();
  edtOPID->Text = "";
  edtOPDescription->Text = "";
  cbxOPWC->Clear();
  grdOPMC->RowCount = 2;
  grdOPMC->FixedRows = 1;
  grdOPMC->Cells[0][1] = "";
  grdOPMC->Cells[1][1] = "";

  edtJOID->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnJOInsertClick(TObject *)
{
  curFunction = FN_INSERT;
  UpdateJOsControls();
  edtJOID->Text = "";
  edtJODescription->Text = "";
  edtJOWeight->Text = "1.0";
  edtJOReleaseTime->Text = "0";
  edtJODueTime->Text = "0";

  lstOPs->Clear();
  edtOPID->Text = "";
  edtOPDescription->Text = "";
  cbxOPWC->Clear();
  grdOPMC->RowCount = 2;
  grdOPMC->FixedRows = 1;
  grdOPMC->Cells[0][1] = "";
  grdOPMC->Cells[1][1] = "";

  edtJOID->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnJODeleteClick(TObject *)
{
  MOSP_Problem::SJob *curJob = (MOSP_Problem::SJob *)(Problem->Jobs[icurJob]);
  if(!Problem->RemoveJob(curJob)){
	Application->MessageBox(L"Couldn't remove Job",
							L"Error removing Job", MB_OK);
	return;
  }
  if(icurJob >= Problem->Jobs.Count()) icurJob = Problem->Jobs.Count() - 1;
  icurOp = 0;
  DisplayJobs();
  UpdateJOsControls();
  NeedToBeSaved = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnJOModifyClick(TObject *)
{
  curFunction = FN_MODIFY;
  UpdateJOsControls();
  edtJODescription->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnJOApproveClick(TObject *)
{
  AnsiString id, des, wt, rt, dt;
  id = edtJOID->Text;
  des = edtJODescription->Text;
  wt = edtJOWeight->Text;
  rt = edtJOReleaseTime->Text;
  dt = edtJODueTime->Text;

  if(id == ""){
	Application->MessageBox(L"ID should be defined", L"Error adding job", MB_OK);
	return;
  }

  float w, r, d;
  try{
	w = (wt == "")? 1 : StrToFloat(wt);
	r = (rt == "")? 0 : StrToFloat(rt);
	d = (dt == "")? 0 : StrToFloat(dt);
  } catch(EConvertError&){
	  Application->MessageBox(L"Use appropriate values for weight, release"\
							  " time and due time",
							  L"Error reading numbers",
                              MB_OK);
      return;
    }

  MOSP_Problem::SJob *curJob;

  if(Problem->Jobs.Count() == 0 && curFunction == FN_INSERT) curFunction = FN_APPEND;
  switch(curFunction){
    case FN_APPEND:
		curJob = Problem->AddJob((char *)id.c_str(), (char *)des.c_str(), r, d, w);
		if(!curJob){
		  Application->MessageBox(L"Couldn't add new job, check for ID duplication",
								  L"Error adding job", MB_OK);
		  return;
		}
		icurJob = Problem->Jobs.Count() - 1;
		lstJOs->Items->Add(id);
		lstJOs->ItemIndex = icurJob;
	  break;

	case FN_INSERT:
		curJob = Problem->InsertJob(icurJob, (char *)id.c_str(), (char *)des.c_str(), r, d, w);
		if(!curJob){
		  Application->MessageBox(L"Couldn't add new job, check for ID duplication",
								  L"Error adding job", MB_OK);
		  return;
		}
		lstJOs->Items->Insert(icurJob, id);
		lstJOs->ItemIndex = icurJob;
	  break;

	case FN_MODIFY:
		curJob = (MOSP_Problem::SJob *)(Problem->Jobs[icurJob]);
		if(!Problem->ModifyJob(curJob, (char *)id.c_str(), (char *)des.c_str(), r, d, w)){
		  Application->MessageBox(L"Couldn't modify, check for ID duplication",
								  L"Error modifying job", MB_OK);
		  return;
		}
		lstJOs->Items->Strings[icurJob] = id;
	  break;
  }

  curFunction = FN_NAVIGATE;
  UpdateJOsControls();
  icurOp = 0;
  DisplayCurrentJob();

  NeedToBeSaved = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnJOCancelClick(TObject *)
{
  curFunction = FN_NAVIGATE;
  UpdateJOsControls();
  DisplayCurrentJob();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnOPAppendClick(TObject *)
{
  curFunction = FN_OPAPPEND;
  UpdateOpsControls();
  edtOPID->Text = "";
  edtOPDescription->Text = "";
  grdOPMC->RowCount = 2;
  grdOPMC->FixedRows = 1;
  grdOPMC->Cells[0][1] = "";
  grdOPMC->Cells[1][1] = "";
  LoadWCsList();
  edtOPID->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnOPInsertClick(TObject *)
{
  curFunction = FN_OPINSERT;
  UpdateOpsControls();
  edtOPID->Text = "";
  edtOPDescription->Text = "";
  grdOPMC->RowCount = 2;
  grdOPMC->FixedRows = 1;
  grdOPMC->Cells[0][1] = "";
  grdOPMC->Cells[1][1] = "";
  LoadWCsList();
  edtOPID->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnOPDeleteClick(TObject *)
{
  MOSP_Problem::SJob *curJob = (MOSP_Problem::SJob *)(Problem->Jobs[icurJob]);
  MOSP_Problem::SOperation *curOp;
  curOp = (MOSP_Problem::SOperation *)(curJob->Operations[icurOp]);
  if(!Problem->RemoveOperation(curOp)){
	Application->MessageBox(L"Couldn't remove Operation",
							L"Error removing Operation", MB_OK);
    return;
  }
  if(icurOp >= curJob->Operations.Count()) icurOp = curJob->Operations.Count() - 1;
  UpdateJOsControls();
  DisplayCurrentJob();
  NeedToBeSaved = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnOPModifyClick(TObject *)
{
  curFunction = FN_OPMODIFY;
  UpdateOpsControls();
  grdOPMC->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnOPCancelClick(TObject *)
{
  curFunction = FN_NAVIGATE;
  UpdateJOsControls();
  DisplayCurrentJob();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnOPStartClick(TObject *)
{
  icurOp = 0;
  lstOPs->ItemIndex = 0;
  DisplayCurrentOp();
  UpdateOpsControls();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnOPEndClick(TObject *)
{
  MOSP_Problem::SJob *curJob = (MOSP_Problem::SJob *)(Problem->Jobs[icurJob]);
  icurOp = curJob->Operations.Count() - 1;
  lstOPs->ItemIndex = icurOp;
  DisplayCurrentOp();
  UpdateOpsControls();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnOPNextClick(TObject *)
{
  ++icurOp;
  lstOPs->ItemIndex = icurOp;
  DisplayCurrentOp();
  UpdateOpsControls();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnOPPrevClick(TObject *)
{
  --icurOp;
  lstOPs->ItemIndex = icurOp;
  DisplayCurrentOp();
  UpdateOpsControls();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnOPApproveClick(TObject *)
{
  AnsiString id, des, spt;
  id = edtOPID->Text;
  des = edtOPDescription->Text;
  int ipc = cbxOPWC->ItemIndex;

  if(id == ""){
	Application->MessageBox(L"ID should be defined", L"Error adding operation", MB_OK);
	return;
  }
  if(ipc == -1){
	Application->MessageBox(L"workcenter should be selected",
							L"Error adding operation", MB_OK);
	return;
  }

  AnsiString pcID = cbxOPWC->Items->Strings[ipc];
  MOSP_Problem::SWC *selectedWC = Problem->WCbyID((char *)pcID.c_str());
  if(!selectedWC){
	String s = "workcenter with ID " + pcID + " is not defined";
	Application->MessageBox((const wchar_t *)s.c_str(), L"Memory error", MB_OK);
	return;
  }

  int i, nmc = selectedWC->MCs.Count();
  float *pt;
  if(nmc == 0) pt = NULL;
  else{
	pt = new float[nmc];
	try{
	  for(i=0; i<nmc; i++){
		spt = grdOPMC->Cells[1][i+1];
		pt[i] = StrToFloat(spt);
	  }
	} catch(EConvertError&){
		Application->MessageBox(L"Use appropriate values for times",
								L"Error reading number",
								MB_OK);
		delete [] pt;
		return;
	  }
  }

  MOSP_Problem::SJob *curJob = (MOSP_Problem::SJob *)(Problem->Jobs[icurJob]);
  if(!curJob){
	Application->MessageBox(L"Undefined job", L"Memory error", MB_OK);
	return;
  }

  int nOps = curJob->Operations.Count();
  MOSP_Problem::SOperation *curOp;

  switch(curFunction){
	case FN_OPAPPEND:
		curOp = Problem->AddOperation((char *)id.c_str(), (char *)des.c_str(), curJob, selectedWC, pt);
		if(pt) delete [] pt;
		if(!curOp){
		  Application->MessageBox(L"Couldn't add new operation, check for ID duplication",
								  L"Error adding operation", MB_OK);
		  return;
		}
		icurOp = curJob->Operations.Count()-1;
		lstOPs->Items->Add(id);
		lstOPs->ItemIndex = icurOp;
	  break;

	case FN_OPINSERT:
		if(nOps == 0) curOp = Problem->AddOperation((char *)id.c_str(), (char *)des.c_str(), curJob, selectedWC, pt);
		else curOp = Problem->InsertOperation(icurOp, (char *)id.c_str(), (char *)des.c_str(), curJob, selectedWC, pt);
		if(pt) delete [] pt;
		if(!curOp){
		  Application->MessageBox(L"Couldn't add new operation, check for ID duplication",
								  L"Error adding operation", MB_OK);
		  return;
		}
		lstOPs->Items->Insert(icurOp, id);
		lstOPs->ItemIndex = icurOp;
	  break;

	case FN_OPMODIFY:
		curOp = (MOSP_Problem::SOperation *)(curJob->Operations[icurOp]);
		bool ok = Problem->ModifyOperation(curOp, (char *)id.c_str(), (char *)des.c_str(), selectedWC, pt);
		if(pt) delete [] pt;
		if(!ok){
		  Application->MessageBox(L"Couldn't modify, check for ID duplication",
								  L"Error modifying operation", MB_OK);
		  return;
		}
		lstOPs->Items->Strings[icurOp] = id;
	  break;
  }
  curFunction = FN_NAVIGATE;
  UpdateJOsControls();
  DisplayCurrentJob();

  NeedToBeSaved = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::lstWCsClick(TObject *)
{
  icurWC = lstWCs->ItemIndex;
  icurMC = 0;
  DisplayCurrentWC();
  UpdateWCsControls();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::lstJOsClick(TObject *)
{
  icurJob = lstJOs->ItemIndex;
  icurOp = 0;
  DisplayCurrentJob();
  UpdateJOsControls();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::lstOPsClick(TObject *)
{
  icurOp = lstOPs->ItemIndex;
  DisplayCurrentOp();
  UpdateOpsControls();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::lstMCsClick(TObject *)
{
  icurMC = lstMCs->ItemIndex;
  DisplayCurrentMC();
  UpdateMCsControls();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnMCAppendClick(TObject *)
{
  curFunction = FN_MCAPPEND;
  UpdateMCsControls();
  edtMCID->Text = "";
  edtMCDescription->Text = "";
  edtMCReadyTime->Text = "0";
  edtMCID->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnMCInsertClick(TObject *)
{
  curFunction = FN_MCINSERT;
  UpdateMCsControls();
  edtMCID->Text = "";
  edtMCDescription->Text = "";
  edtMCReadyTime->Text = "0";
  edtMCID->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnMCDeleteClick(TObject *)
{
  MOSP_Problem::SWC *curWC = (MOSP_Problem::SWC *)(Problem->WCs[icurWC]);
  MOSP_Problem::SMC *curMC = (MOSP_Problem::SMC *)(curWC->MCs[icurMC]);
  if(!Problem->RemoveMC(curMC)){
	Application->MessageBox(L"Couldn't remove Machine",
							L"Error removing Machine", MB_OK);
    return;
  }
  if(icurMC >= curWC->MCs.Count()) icurMC = curWC->MCs.Count() - 1;
  UpdateWCsControls();
  DisplayCurrentWC();
  NeedToBeSaved = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnMCModifyClick(TObject *)
{
  curFunction = FN_MCMODIFY;
  UpdateMCsControls();
  edtMCReadyTime->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnMCEndClick(TObject *)
{
  MOSP_Problem::SWC *curWC = (MOSP_Problem::SWC *)(Problem->WCs[icurWC]);
  icurMC = curWC->MCs.Count() - 1;
  lstMCs->ItemIndex = icurMC;
  DisplayCurrentMC();
  UpdateMCsControls();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnMCStartClick(TObject *)
{
  icurMC = 0;
  lstMCs->ItemIndex = 0;
  DisplayCurrentMC();
  UpdateMCsControls();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnMCNextClick(TObject *)
{
  ++icurMC;
  lstMCs->ItemIndex = icurMC;
  DisplayCurrentMC();
  UpdateMCsControls();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnMCPrevClick(TObject *)
{
  --icurMC;
  lstMCs->ItemIndex = icurMC;
  DisplayCurrentMC();
  UpdateMCsControls();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnMCApproveClick(TObject *)
{
  AnsiString id, des, srt;
  id = edtMCID->Text;
  des = edtMCDescription->Text;
  srt = edtMCReadyTime->Text;

  if(id == ""){
	Application->MessageBox(L"ID should be defined", L"Error adding machine", MB_OK);
	return;
  }

  float rt;
  try{
	rt = StrToFloat(srt);
  } catch(EConvertError&){
	  Application->MessageBox(L"Use appropriate value for read time",
							  L"Error reading number",
							  MB_OK);
	  return;
	}

  MOSP_Problem::SWC *curWC = (MOSP_Problem::SWC *)(Problem->WCs[icurWC]);
  int nMCs = curWC->MCs.Count();
  MOSP_Problem::SMC *curMC;

  switch(curFunction){
	case FN_MCAPPEND:
		curMC = Problem->AddMC((char *)id.c_str(), (char *)des.c_str(), curWC, rt);
		if(!curMC){
		  Application->MessageBox(L"Couldn't add new machine, check for ID duplication",
								  L"Error adding machine", MB_OK);
		  return;
		}
		icurMC = curWC->MCs.Count()-1;
		lstMCs->Items->Add(id);
		lstMCs->ItemIndex = icurMC;
	  break;

	case FN_MCINSERT:
		if(nMCs == 0) curMC = Problem->AddMC((char *)id.c_str(), (char *)des.c_str(), curWC, rt);
		else curMC = Problem->InsertMC(icurMC, (char *)id.c_str(), (char *)des.c_str(), curWC, rt);
		if(!curMC){
		  Application->MessageBox(L"Couldn't add new machine, check for ID duplication",
								  L"Error adding machine", MB_OK);
		  return;
		}
		lstMCs->Items->Insert(icurMC, id);
		lstMCs->ItemIndex = icurMC;
	  break;

	case FN_MCMODIFY:
		curMC = (MOSP_Problem::SMC *)(curWC->MCs[icurMC]);
		bool ok = Problem->ModifyMC(curMC, (char *)id.c_str(), (char *)des.c_str(), rt);
		if(!ok){
		  Application->MessageBox(L"Couldn't modify, check for ID duplication",
								  L"Error modifying machine", MB_OK);
		  return;
		}
		lstMCs->Items->Strings[icurOp] = id;
	  break;
  }
  curFunction = FN_NAVIGATE;
  UpdateWCsControls();
  DisplayCurrentWC();

  NeedToBeSaved = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::btnMCCancelClick(TObject *)
{
  curFunction = FN_NAVIGATE;
  UpdateWCsControls();
  DisplayCurrentWC();
}
//---------------------------------------------------------------------------

void __fastcall TfrmProblem::LoadWCsList()
{
  AnsiString s;
  cbxOPWC->Clear();
  if(Problem->Jobs.Count() == 0) return;
  MOSP_Problem::SJob *curJob = (MOSP_Problem::SJob *)(Problem->Jobs[icurJob]);

  SList::SNode *pcnd = Problem->WCs.head();
  while(pcnd){
	MOSP_Problem::SWC *pc = (MOSP_Problem::SWC *)pcnd->Data();
	if(!curJob->bUsedWC(pc)){
	  s = pc->ID();
	  cbxOPWC->Items->Add(s);
	}
	pcnd = pcnd->Next();
  }
  cbxOPWC->ItemIndex = -1;
}

void __fastcall TfrmProblem::cbxOPWCChange(TObject *)
{
  AnsiString s;
  s = (*cbxOPWC->Items)[cbxOPWC->ItemIndex];
  if(s == "") return;
  MOSP_Problem::SWC *pc = Problem->WCbyID((char *)s.c_str());
  if(!pc) return;

  if(curFunction == FN_OPMODIFY){
	MOSP_Problem::SJob *curJob = (MOSP_Problem::SJob *)(Problem->Jobs[icurJob]);
	MOSP_Problem::SOperation *curOp = (MOSP_Problem::SOperation *)(curJob->Operations[icurOp]);
	if(curOp->WC == pc){
	  grdOPMC->RowCount = curOp->AltMCs.Count() + 1;
	  if(grdOPMC->RowCount == 1){
		grdOPMC->RowCount = 2;
		grdOPMC->Cells[0][1] = "";
		grdOPMC->Cells[1][1] = "";
		grdOPMC->FixedRows = 1;
		return;
	  }
	  grdOPMC->FixedRows = 1;
	  SList::SNode *mcnd = curOp->AltMCs.head();
	  int r = 1;
	  while(mcnd){
		MOSP_Problem::SOpMC *opmc = (MOSP_Problem::SOpMC *)mcnd->Data();
		grdOPMC->Cells[0][r] = opmc->MC->ID();
		grdOPMC->Cells[1][r] = FloatToStr(opmc->pt);
		mcnd = mcnd->Next();
		r++;
	  }
	}
	else {
	  grdOPMC->RowCount = pc->MCs.Count() + 1;
	  if(grdOPMC->RowCount == 1){
		grdOPMC->RowCount = 2;
		grdOPMC->Cells[0][1] = "";
		grdOPMC->Cells[1][1] = "";
		grdOPMC->FixedRows = 1;
		return;
	  }
	  grdOPMC->FixedRows = 1;
	  SList::SNode *mcnd = pc->MCs.head();
	  int r = 1;
	  while(mcnd){
		MOSP_Problem::SMC *mc = (MOSP_Problem::SMC *)mcnd->Data();
		grdOPMC->Cells[0][r] = mc->ID();
		grdOPMC->Cells[1][r] = "";
		mcnd = mcnd->Next();
		r++;
	  }
	}
  }
  else if(curFunction == FN_OPINSERT || curFunction == FN_OPAPPEND){
	grdOPMC->RowCount = pc->MCs.Count() + 1;
	if(grdOPMC->RowCount == 1){
	  grdOPMC->RowCount = 2;
	  grdOPMC->Cells[0][1] = "";
	  grdOPMC->Cells[1][1] = "";
	  grdOPMC->FixedRows = 1;
	  return;
	}
	grdOPMC->FixedRows = 1;
	SList::SNode *mcnd = pc->MCs.head();
	int r = 1;
	while(mcnd){
	  MOSP_Problem::SMC *mc = (MOSP_Problem::SMC *)mcnd->Data();
	  grdOPMC->Cells[0][r] = mc->ID();
	  grdOPMC->Cells[1][r] = "";
	  mcnd = mcnd->Next();
	  r++;
	}
  }
}
//---------------------------------------------------------------------------

