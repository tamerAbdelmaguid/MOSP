//--- This code is Copyright 2007, Tamer F. Abdelmaguid
//    Use of this code or portions of it is granted without fee for academic,
//    non-commercial purposes only provided that the above copyright statement
//    is clearly indicated
//------------------------------------------------------------------------------

#ifndef ALGORITHM
#define ALGORITHM

#include <vcl.h>
#pragma hdrstop

#include <sys\timeb.h>
#include <stdlib.h>
#include <time.h>
#include "Problem.h"

//------------------------------------------ Generic definitions for algorithms

class Algorithm {
  protected:
	MOSP_Problem *Problem;
	char Name[64];
	float ObjectiveValue;
	float ComputationalTime;
    timeb start_time, end_time;

  public:
	bool bInterrupt;

	Algorithm(MOSP_Problem *P, char *N=""){
	  strncpy(Name, N, 64);
	  Problem = P;
	  ComputationalTime = 0;
	  bInterrupt = false;
	};
	~Algorithm() {};

	const char *GetName(){ return Name; }
	const float GetObjectiveValue(){ return ObjectiveValue; }
	const float GetTime(){ return ComputationalTime; }
	const MOSP_Problem *GetProblem(){ return Problem; }

	virtual void AllocateMemory() = 0;
	virtual void Initialize() = 0;
	virtual void Improve() = 0;
	virtual bool Stop() = 0;
	virtual void Finalize() = 0;
	virtual void FreeMemory() = 0;
	virtual void ComputeObjectiveValue() = 0;

	void Run(){
	  ObjectiveValue = MAXFLOAT;
	  AllocateMemory();              // 1. allocate necessary memory space
	  ::ftime(&start_time);               // 2. record start time
	  Initialize();                  // 3. do initialization step
	  while(!Stop()){                // 4. conduct iterations
		Improve();
		Application->ProcessMessages();
		if(bInterrupt) break;
	  }
	  Finalize();                    // 5. do finalization step
	  ::ftime(&end_time);            // 6. record end time
	  ComputeObjectiveValue();       // 7. Compute final solution O.F. value
	  FreeMemory();                  // 8. free up allocated memory space

	  ComputationalTime = (end_time.time - start_time.time)*1000.0 +
						  (end_time.millitm - start_time.millitm);
	};
};

//------------------------------------------------------------------------

#endif
