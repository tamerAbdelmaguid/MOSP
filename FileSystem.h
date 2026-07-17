//--- This code is Copyright 2007, Tamer F. Abdelmaguid
//    Use of this code or portions of it is granted without fee for academic,
//    non-commercial purposes only provided that the above copyright statement
//    is clearly indicated
//------------------------------------------------------------------------------

#ifndef FileSystemH
#define FileSystemH

#include "Problem.h"
#include "MOSPSolver.h"

enum MOSP_FIO_RESULT {
  SUCCESS,
  FAIL_TO_OPEN,
  UNKNOWN_FILE_TYPE,
  DATA_ERROR,
  INCOMPATIBLE_PROBLEM,
  INFEASIBLE_SOLUTION
};

struct MOSP_DataHeader {
  char ID[8];
  int nWCs;
  int nMCs;
  int nJobs;
  int nOperations;
};

struct MOSP_WCData {
  char ID[8];
  char Description[64];
  int nMCs;
};

struct MOSP_MCData {
  char ID[8];
  char Description[64];
  float ReadyTime;
};

struct MOSP_JobData {
  char ID[8];
  char Description[64];
  float Weight;
  float ReleaseTime;
  float DueDate;
  int nOperations;  // used to verify data compatibility
};

struct MOSP_OperationData {
  char ID[8];
  char Description[64];
  char WCID[8];
  int nAltMCs;
};

struct MOSP_ProcessingTimeData{
  char MCID[8];
  float ProcessingTime;
};

struct MOSP_WCData_s {
  int nMCs;
};

struct MOSP_MCData_s {
  float ReadyTime;
};

struct MOSP_JobData_s {
  float Weight;
  float ReleaseTime;
  float DueDate;
  int nOperations;  // used to verify data compatibility
};

struct MOSP_OperationData_s {
  int WCIndex;
  int nAltMCs;
};

struct MOSP_ProcessingTimeData_s {
  int MCIndex;
  float ProcessingTime;
};

struct MOSP_ScheduledTask {
  int JobIndex;
  int OperationIndex;
  int MCIndex;
  float Start, End;
};

void SaveProblem(MOSP_Problem *Problem, const char *fileName, MOSP_FIO_RESULT &Result);
MOSP_Problem *LoadProblem(char *fileName, MOSP_FIO_RESULT &Result);

void SaveSolution(MOSP_Problem *Problem, MOSP_Solution *Sol, char *fileName, MOSP_FIO_RESULT &Result);
MOSP_Solution *LoadSolution(MOSP_Problem *Problem, char *fileName, MOSP_FIO_RESULT &Result);

void SaveMOSP_DAT(MOSP_Problem *Problem, wchar_t *fName, MOSP_FIO_RESULT &Result);

//---------------------------------------------------------------------------
#endif
