/*

  Name:           read_binary.C
  Created by:     Stefan Ritt <stefan.ritt@psi.ch>
  Date:           July 30th, 2014

  Purpose:        Example program under ROOT to read a binary data file written
  by the DRSOsc program. Decode time and voltages from
  waveforms
  and display them as a graph. Put values into a ROOT Tree for
  further analysis.

  To run it, do:

  - Crate a file test.dat via the "Save" button in DRSOsc
  - start ROOT
  root [0] .L read_binary.C+
  root [1] decode("test.dat");

*/

/*
  5/24/2016, Xiaochun He
  This code is modified for analyzing spectra from the liquid scintilatortest

  8/29/2016, hexc
  This code is modified for Robert to analize the LiquidScintillator detector
  data

  01/24/2017, sawaiz
  Modified to be compileable, and run from a single command. Check readme.md for instructoins.

  03/2/2017 Sawaiz and Anthony
  Modified file to calculate total time elapsed from start to finish and events per hour, and renamed original file to voltageScan.C
*/

#include <string.h>
#include <stdio.h>
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TH1.h"
#include "TF1.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "Getline.h"
#include <time.h>

using namespace std;

typedef struct {
  char time_header[4];
  char bn[2];
  unsigned short board_serial_number;
} THEADER;

typedef struct {
  char event_header[4];
  unsigned int event_serial_number;
  unsigned short year;
  unsigned short month;
  unsigned short day;
  unsigned short hour;
  unsigned short minute;
  unsigned short second;
  unsigned short millisecond;
  unsigned short reserved1;
  char bs[2];
  unsigned short board_serial_number;
  char tc[2];
  unsigned short trigger_cell;
} EHEADER;

/*-----------------------------------------------------------------------------*/

// Prototypes
void decode(char* filename, int usr_duration);
time_t printEvent(EHEADER eh);

// Main function, should be named after filename
void timeDiff(const char* filename, int usr_duration) {
  decode((char*)filename, usr_duration);
}

time_t printEvent(EHEADER eh){
  time_t rawTime;  
  struct tm * time_struct;
  time(&rawTime);
  time_struct = localtime(&rawTime);
  time_struct->tm_sec = eh.second;
  time_struct->tm_min = eh.minute;
  time_struct->tm_hour = eh.hour;
  time_struct->tm_mday = eh.day;
  time_struct->tm_mon = eh.month - 1;
  time_struct->tm_year = eh.year - 1900;
  rawTime = mktime(time_struct);
  //printf("Found event #%d %s", eh.event_serial_number,ctime(&rawTime));
  return rawTime;
}

void decode(char* filename,int usr_duration) {
  THEADER th;
  EHEADER eh;
  char hdr[4];
  unsigned short voltage[1024];
  double waveform[4][1024], time[4][1024];
  float bin_width[4][1024];
  int i, j, ch, n, chn_index;
  double t1, t2, dt;
  time_t start;
  time_t end;
  float duration=0;
  
  
  // open the binary waveform file
  FILE* f = fopen(Form("%s", filename), "r");
  if (f == NULL) {
    printf("Cannot find file \'%s\'\n", filename);
    return;
  }

  // read time header
  fread(&th, sizeof(th), 1, f);
  // printf("Found data for board #%d\n", th.board_serial_number);

  // read time bin widths
  memset(bin_width, sizeof(bin_width), 0);
  for (ch = 0; ch < 5; ch++) {
    fread(hdr, sizeof(hdr), 1, f);
    if (hdr[0] != 'C') {
      // event header found
      fseek(f, -4, SEEK_CUR);
      break;
    }
    i = hdr[3] - '0' - 1;
    //printf("Found timing calibration for channel #%d\n", i + 1);
    fread(&bin_width[i][0], sizeof(float), 1024, f);
  }

  // loop over events in data file
  bool firstEvent = 0;
  if(usr_duration == 0){
    while(1){
      // read event header
      i = fread(&eh, sizeof(eh), 1, f);
      if (i < 1)
	break;
      if(firstEvent == 0)
	{
	  start = printEvent(eh);
	  firstEvent = 1;
	}

      // reach channel data
      for (ch = 0; ch < 5; ch++) {
	i = fread(hdr, sizeof(hdr), 1, f);
	if (i < 1)
	  break;
	if (hdr[0] != 'C') {
	  // event header found
	  fseek(f, -4, SEEK_CUR);
	  break;
	}
	chn_index = hdr[3] - '0' - 1;
	fread(voltage, sizeof(short), 1024, f);  
      }
    
    }
    end = printEvent(eh);
    duration = difftime(end,start)/3600;
    printf("%d events in %f hours yields %f events per hour.\n",
	   eh.event_serial_number,
	   duration,
	   eh.event_serial_number/duration);
  }
 else
   {
     while(duration < usr_duration + 1)
       {
	 // read event header
	 i = fread(&eh, sizeof(eh), 1, f);
	 if (i < 1)
	   break;
	 if(firstEvent == 0)
	   {
	     start = printEvent(eh);
	     firstEvent = 1;
	   }

	 // reach channel data
	 for (ch = 0; ch < 5; ch++) {
	   i = fread(hdr, sizeof(hdr), 1, f);
	   if (i < 1)
	     break;
	   if (hdr[0] != 'C') {
	     // event header found
	     fseek(f, -4, SEEK_CUR);
	     break;
	   }
	   chn_index = hdr[3] - '0' - 1;
	   fread(voltage, sizeof(short), 1024, f);  
	 }
    
       
	 end = printEvent(eh);
	 duration = difftime(end,start);
       }
     duration = duration/3600;
     printf("%d events in %f hours yields %f events per hour.\n",
	    eh.event_serial_number,
	    duration,
	    eh.event_serial_number/duration);
   }
}	 
  
	 
   
	 
	 



