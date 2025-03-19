//
#include "TTree.h"

void plot_time_residuals(int RunNumber) {

  TFile* f = TFile::Open(Form("/scratch/mu2e/mu2etrk_pasha_304/digi_ntuples/make_digi_ntuple_%06i.root",RunNumber));
  TTree* t = (TTree*) f->Get("MakeDigiNtuple/digis");
  
  t->SetAlias("dt01"    ,"evt.sd.tdc0-evt.sd.tdc1");
  
  t->SetAlias("ts1"     ,"(evt.sd.sid<400)")    ; // TS1 on DTC0:link2
  t->SetAlias("ts2"     ,"(evt.sd.sid>400)")    ; // TS2 on DTC1:link4
  t->SetAlias("sd0_ts1" ,"(evt.sd[0].sid<400)") ; // TS1
  t->SetAlias("sd0_ts2" ,"(evt.sd[0].sid>400)") ; // TS2 on DTC1:link4

  t->SetAlias("dt12_tdc0","evt.sd[0].tdc0-evt.sd[1].tdc0") ; // 
  t->SetAlias("dt12_tdc1","evt.sd[0].tdc1-evt.sd[1].tdc1") ; // 

  TCanvas* c = new TCanvas(Form("c_%06i",RunNumber),Form("c_%06i",RunNumber),1500,800);
  c->Divide(3,2);
  gStyle->SetOptFit(11111111);
  gStyle->SetFitFormat("7.6g");

  c->cd(1);
  TH1F* h_dt01_ts1 = new TH1F("h_dt01_ts1",Form("run %06i h_dt01_ts1",RunNumber),200,-100,100);
  t->Draw("dt01 >> h_dt01_ts1","ts1 && fabs(dt01)<4.e5");
  h_dt01_ts1->Fit("gaus");

  c->cd(4);
  TH1F* h_dt01_ts2 = new TH1F("h_dt01_ts2",Form("run %06i h_dt01_ts2",RunNumber),200,-550,-350);
  t->Draw("dt01 >> h_dt01_ts2","ts2 && fabs(dt01)<4.e5");
  h_dt01_ts2->Fit("gaus");

  c->cd(2);
  TH1F* h_dt12_tdc0_2 = new TH1F("h_dt12_tdc0_2",Form("run %06i h_dt12_tdc0_2",RunNumber),600,-12800,-12200);
  t->Draw("dt12_tdc0 >> h_dt12_tdc0_2","sd0_ts2 && fabs(dt12_tdc0)<1.e5");
  h_dt12_tdc0_2->Fit("gaus");

  c->cd(5);
  TH1F* h_dt12_tdc0_1 = new TH1F("h_dt12_tdc0_1",Form("run %06i h_dt12_tdc0_1",RunNumber),600,-12800,-12200);
  t->Draw("-dt12_tdc0 >> h_dt12_tdc0_1","sd0_ts1 && fabs(dt12_tdc0)<1.e5");
  h_dt12_tdc0_1->Fit("gaus");

  c->cd(3);
  TH1F* h_dt12_tdc1_2 = new TH1F("h_dt12_tdc1_2",Form("run %06i h_dt12_tdc1_2",RunNumber),600,-12300,-11700);
  t->Draw("dt12_tdc1 >> h_dt12_tdc1_2","sd0_ts2 && fabs(dt12_tdc1)<1.e5");
  h_dt12_tdc1_2->Fit("gaus");

  c->cd(6);
  TH1F* h_dt12_tdc1_1 = new TH1F("h_dt12_tdc1_1",Form("run %06ih_dt12_tdc1_1",RunNumber),600,-12300,-11700);
  t->Draw("-dt12_tdc1 >> h_dt12_tdc1_1","sd0_ts1 && fabs(dt12_tdc1)<1.e5");
  h_dt12_tdc1_1->Fit("gaus");

  //  h_dt10_ts2->Fit("gaus");
}
