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

// #define compile

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
void decode(char* filename);

// Main function, should be named after filename
void sPhenixTileTest(const char* filename) {
  decode((char*)filename);
}

//
void decode(char* filename) {
  THEADER th;
  EHEADER eh;
  char hdr[4];
  unsigned short voltage[1024];
  double waveform[4][1024], time[4][1024];
  float bin_width[4][1024];
  float adc[4];
  char rootfile[256];
  int i, j, ch, n, chn_index;
  double t1, t2, dt;
  TGraph* graphs[4];
  TH1F* adc_graphs[4];

  #ifndef compile
  const int events = 100;
  const int eventInfo = 10;
  #else
  const int events = 1000000;
  const int eventInfo = 1000;
  #endif

  const char *names[] =         {"OH-2-01 Spectrum",
                                 "IH-1-16 Spectrum",
                                 "OH-1-15 Spectrum",
                                 "IH-2-12 Spectrum"};  

  // {Min X,Max X, Min Y, Max Y}
  const float graphBounds[4][4] = {{200,450,-0.05,0.5},
                                   {200,350,-0.05,0.1},
                                   {200,350,-0.05,0.1},
                                   {200,350,-0.05,0.1}};

  // {Min X, Max X}
  const int fitBounds[4][2] =      {{240,450},
                                    {240,340},
                                    {250,340},
                                    {200,500}};

  // open the binary waveform file
  FILE* f = fopen(Form("%s", filename), "r");
  if (f == NULL) {
    printf("Cannot find file \'%s\'\n", filename);
    return;
  }

  // open the root file
  strcpy(rootfile, filename);
  if (strchr(rootfile, '.'))
    *strchr(rootfile, '.') = 0;
  strcat(rootfile, ".root");
  TFile* outfile = new TFile(rootfile, "RECREATE");

  // define the rec tree
  TTree* rec = new TTree("rec", "rec");
  rec->Branch("t1", time[0], "t1[1024]/D");
  rec->Branch("t2", time[1], "t2[1024]/D");
  rec->Branch("t3", time[2], "t3[1024]/D");
  rec->Branch("t4", time[3], "t4[1024]/D");
  rec->Branch("w1", waveform[0], "w1[1024]/D");
  rec->Branch("w2", waveform[1], "w2[1024]/D");
  rec->Branch("w3", waveform[2], "w3[1024]/D");
  rec->Branch("w4", waveform[3], "w4[1024]/D");

  for(i = 0 ; i < 4 ; i++){
    // Define the waveform graph
    graphs[i] = new TGraph(1024, (double*)time[i], (double*)waveform[i]);
    // Define the spectra graphs    
    adc_graphs[i] = new TH1F(names[i], names[i], 3000, 0, 30.0);
  }

  // Waveforms TTree
  TTree* waveforms = new TTree("waveforms", "Waveforms");
  // TBranch* channel1 = waveforms->Branch("channel1", "Channel 1", &g1, 500, 0);

  // read time header
  fread(&th, sizeof(th), 1, f);
  printf("Found data for board #%d\n", th.board_serial_number);

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
    printf("Found timing calibration for channel #%d\n", i + 1);
    fread(&bin_width[i][0], sizeof(float), 1024, f);
  }

  #ifndef compile
  TCanvas* c1 = new TCanvas("c1", "sPHENIX Tile", 10, 10, 1920, 1080);
  c1->Divide(1, 3);
  #endif

  // loop over events in data file
  for (n = 0; n < events; n++) {
    // read event header
    i = fread(&eh, sizeof(eh), 1, f);
    if (i < 1)
      break;

    if (n % eventInfo == 0) {
      printf("Found event #%d ", eh.event_serial_number);
      printf("  %d", eh.year);
      printf("/%d", eh.month);
      printf("/%d", eh.day);
      printf("  %d", eh.hour);
      printf(":%d", eh.minute);
      printf(":%d", eh.second);
      printf(":%d\n", eh.millisecond);
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

      for (i = 0; i < 1024; i++) {
        // convert data to volts
        // -0.05 for -0.05-.95V
        // -0.5 for -0.5 to 0.5V
        waveform[chn_index][i] = (voltage[i] / 65536. - 0.05);

        // calculate time for this cell
        for (j = 0, time[chn_index][i] = 0; j < i; j++)
          time[chn_index][i] +=
              bin_width[chn_index][(j + eh.trigger_cell) % 1024];
      }
    }

    // align cell #0 of all channels
    t1 = time[0][(1024 - eh.trigger_cell) % 1024];
    for (ch = 1; ch < 4; ch++) {
      t2 = time[ch][(1024 - eh.trigger_cell) % 1024];
      dt = t1 - t2;
      for (i = 0; i < 1024; i++)
        time[ch][i] += dt;
    }

    // fill root tree
    rec->Fill();

    // fill graph
    for (i = 0; i < 1024; i++) {
      for(j = 0; j < 4 ; j++){
        graphs[j]->SetPoint(i, time[j][i], waveform[j][i]);
      }
    }

    for(i = 0 ; i < 4; i++){
      //Number the Branches
      char event_serial_number[20];
      sprintf(event_serial_number,"Event %d Channel %d",eh.event_serial_number, i);
      waveforms->Branch(event_serial_number, &graphs[i]);
    }

    // Fit waveform with landau and intergrate. Fill adc histo with intergal
    for(int i = 0 ; i <3 ; i++){
      TF1* f1 = new TF1("f1", "landau", fitBounds[i][0], fitBounds[i][1]);      
      graphs[i]->Fit("f1", "RQ");
      adc[i] = f1->Integral(fitBounds[i][0], fitBounds[i][1]);
      adc_graphs[i]->Fill(adc[i]);
    }

    float max = graphs[0]->GetHistogram()->GetMaximum();
    adc_graphs[0]->Fill(max);

    #ifndef compile
    // draw graph and wait for user click
    for(int i = 0 ; i < 3 ; i++){
      c1->cd(i+1);      
      graphs[i]->GetXaxis()->SetRangeUser(graphBounds[i][0], graphBounds[i][1]);
      graphs[i]->GetYaxis()->SetRangeUser(graphBounds[i][2], graphBounds[i][3]);
      graphs[i]->SetTitle(names[i]);
      graphs[i]->DrawClone("ACP");
    }
    
    c1->Update();
    #endif
  }

  // print number of events
  printf("%d events processed, \"%s\" written.\n", n, rootfile);

  // save and close root file
  rec->Write();
  waveforms->Write();
  for(i = 0 ; i < 4 ; i++){
    adc_graphs[i]->Write();
  }

  #ifndef compile
  for(i = 0 ; i < 4 ; i++){
    c1->cd(i+1);
    adc_graphs[i]->Draw();
  }
  c1->Update();  
  #endif
  
   outfile->Close();
}
