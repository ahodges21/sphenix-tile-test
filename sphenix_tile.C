/*
 
   Name:           read_binary.C
   Created by:     Stefan Ritt <stefan.ritt@psi.ch>
   Date:           July 30th, 2014
 
   Purpose:        Example program under ROOT to read a binary data file written 
                   by the DRSOsc program. Decode time and voltages from waveforms 
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
  This code is modified for Robert to analize the LiquidScintillator detector data

  01/24/2017 
  This code is modified by Anthony Hodges and Sawaiz Syed to analyze four sPHENIX prototype
  hadronic calorimeter tiles 
*/

#include <string.h>
#include <stdio.h>
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "Getline.h"

typedef struct {
  char           time_header[4];
  char           bn[2];
  unsigned short board_serial_number;
} THEADER;

typedef struct {
  char           event_header[4];
  unsigned int   event_serial_number;
  unsigned short year;
  unsigned short month;
  unsigned short day;
  unsigned short hour;
  unsigned short minute;
  unsigned short second;
  unsigned short millisecond;
  unsigned short reserved1;
  char           bs[2];
  unsigned short board_serial_number;
  char           tc[2];
  unsigned short trigger_cell;
} EHEADER;

/*-----------------------------------------------------------------------------*/
//prototype
void decode(char * filename);

//main function
void sphenix_tile(const char *filename){
  decode((char*)filename);
}

void decode(char *filename) {
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


   printf("%ld", sizeof(eh));
   // open the binary waveform file
   FILE *f = fopen(Form("%s", filename), "r");
   if (f == NULL) {
      printf("Cannot find file \'%s\'\n", filename);
      return;
   }

   //open the root file
   strcpy(rootfile, filename);
   if (strchr(rootfile, '.'))*strchr(rootfile, '.') == 0;
   strcat(rootfile, ".root");
   TFile *outfile = new TFile(rootfile, "RECREATE");
   
   // define the rec tree
   TTree *rec = new TTree("rec","rec");
   rec->Branch("t1", time[0]     ,"t1[1024]/D");  
   rec->Branch("t2", time[1]     ,"t2[1024]/D");  
   rec->Branch("t3", time[2]     ,"t3[1024]/D");  
   rec->Branch("t4", time[3]     ,"t4[1024]/D");  
   rec->Branch("w1", waveform[0] ,"w1[1024]/D");
   rec->Branch("w2", waveform[1] ,"w2[1024]/D");
   rec->Branch("w3", waveform[2] ,"w3[1024]/D");
   rec->Branch("w4", waveform[3] ,"w4[1024]/D");
   
   // create three graphs for three channels
   TGraph *g1 = new TGraph(1024, (double *)time[0], (double *)waveform[0]);
   TGraph *g2 = new TGraph(1024, (double *)time[1], (double *)waveform[1]);
   TGraph *g3 = new TGraph(1024, (double *)time[2], (double *)waveform[2]);
   TGraph *g4 = new TGraph(1024, (double *)time[3], (double *)waveform[3]);

   TH1F *g1_adc = new TH1F("g1_adc","IH-1-16",100, -1.0, 20.0);
   TH1F *g2_adc = new TH1F("g2_adc","IH-2-12",100, -1.0, 20.0);
   TH1F *g3_adc = new TH1F("g3_adc","OH-2-1",100, -1.0, 20.0);
   TH1F *g4_adc = new TH1F("g4_adc","OH-1-15",100, -1.0, 20.0);
   
   // read time header
   fread(&th, sizeof(th), 1, f);
   printf("Found data for board #%d\n", th.board_serial_number);

   // read time bin widths
   memset(bin_width, sizeof(bin_width), 0);
   for (ch=0 ; ch<5 ; ch++) {
      fread(hdr, sizeof(hdr), 1, f);
      if (hdr[0] != 'C') {
         // event header found
         fseek(f, -4, SEEK_CUR);
         break;      
      }
      i = hdr[3] - '0' - 1;
      printf("Found timing calibration for channel #%d\n", i+1);
      fread(&bin_width[i][0], sizeof(float), 1024, f);
   }

   // loop over 10000 events in data file
  
   
   
   for (n=0 ; n<10 ; n++) {
      // read event header
      i = fread(&eh, sizeof(eh), 1, f);
      if (i < 1)
         break;

      if (n%1 == 0) {
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
      for (ch=0 ; ch<5 ; ch++) {
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
         
         for (i=0 ; i<1024 ; i++) {
            // convert data to volts
            waveform[chn_index][i] = (voltage[i] / 65536. - 0.5);
            
            // calculate time for this cell
            for (j=0,time[chn_index][i]=0 ; j<i ; j++)
              time[chn_index][i] += bin_width[chn_index][(j+eh.trigger_cell) % 1024];            
         }
      }
    
      // align cell #0 of all channels
      t1 = time[0][(1024-eh.trigger_cell) % 1024];
      for (ch=1 ; ch<4 ; ch++) {
         t2 = time[ch][(1024-eh.trigger_cell) % 1024];
         dt = t1 - t2;
         for (i=0 ; i<1024 ; i++)
            time[ch][i] += dt;
      }

      // fill root tree
      rec->Fill();
      
      // fill graph
      for (i=0 ; i<1024 ; i++) {
	g1->SetPoint(i, time[0][i], waveform[0][i]);
	g2->SetPoint(i, time[1][i], waveform[1][i]);
	g3->SetPoint(i, time[2][i], waveform[2][i]);
	g4->SetPoint(i, time[3][i], waveform[3][i]);
      }

      adc[0] = g1->Integral(0,-1);
      g1_adc->Fill(adc[0]);

      adc[1] = g2->Integral(0,-1);
      g2_adc->Fill(adc[1]);
      
      adc[2] = g3->Integral(0,-1);
      g3_adc->Fill(adc[2]);

      adc[3] = g4->Integral(0,-1);
      g4_adc->Fill(adc[3]);
      
      // draw graph and wait for user click    
     
      
      TCanvas *c1 = new TCanvas("c1","sPHENIX Calorimeter Tile Test Waveform");
     
      c1-> Divide(1,4);

      //pad is layered just like the paddles are

      c1->cd(1);
      g4->GetYaxis()->SetRangeUser(-1,1);
      g4->SetTitle("IH-2-12 Waveform");
      g4->Draw("ACP");
      

      
      c1->cd(2);
      g1 -> GetYaxis()->SetRangeUser(-0.01,0.04);
      g1 -> SetTitle("OH-2-1 Waveform");
      g1 -> Draw("ACP");

      c1->cd(3);
      g3->GetYaxis()->SetRangeUser(-0.01,0.04);
      g3->SetTitle("OH-1-15 Waveform");
      g3->Draw("ACP");

      
      c1->cd(4);
      g2->GetYaxis()->SetRangeUser(-0.01,0.04);
      g2->SetTitle("IH-1-16 Waveform");
      g2->Draw("ACP");
      

      
      
      c1->Update();
      
      gPad->WaitPrimitive();
      
      
     
   }

   // print number of events
   printf("%d events processed, \"%s\" written.\n", n, rootfile);
   
   // save and close root file
   rec->Write();
   //g1_adc->Write();
   //g2_adc->Write();
   //g3_adc->Write();
   //g4_adc->Write();
   
   TCanvas *c2 = new TCanvas("c2","sPHENIX Three Tile Test ADC");
   c2->Divide(1,4);
   
   c2->cd(1);
   g2_adc->Draw();

   c2->cd(2);
   g3_adc->Draw();

   c2->cd(3);
   g4_adc->Draw();

   c2 -> cd(4);
   g1_adc->Draw();

   c2->Update();
   
   
   //outfile->Close();
   
}
