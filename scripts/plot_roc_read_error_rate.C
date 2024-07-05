///////////////////////////////////////////////////////////////////////////////
// read ROC 1000 times, count numner of reported erros
// repeat NExp times, plot resulting histogram
///////////////////////////////////////////////////////////////////////////////
#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"

using namespace DTCLib;
using namespace trkdaq;

//------------------------------------------------------------------------------
void plot_roc_read_error_rate(int Link, uint16_t Reg, int Clock, int NExp = 10, int Debug = 0) {

  DtcInterface* dtc_i = DtcInterface::Instance(-1);
  DTC* dtc = dtc_i->Dtc();

  dtc_i->ConfigureJA(Clock,1);
  
  auto link = DTC_Link_ID(Link);

  int nr(1000);

  TH1F* h_nr = new TH1F(Form("h_nr_r%2i_%i",Reg,Clock), Form("roc reg %2i clock:%i reads "     ,Reg,Clock),NExp,0,NExp);
  TH1F* h_ne = new TH1F(Form("h_ne_r%2i_%i",Reg,Clock), Form("roc reg %2i clock:%i read errors",Reg,Clock),NExp,0,NExp);

  h_nr->Reset();
  h_ne->Reset();

  TCanvas* c = new TCanvas(Form("c_%02i_%i",Reg,Clock),Form("c reg:%02i clock:%i",Reg,Clock),1000,700);
  c->Clear();
  c->SetLogy(1);

  h_nr->GetYaxis()->SetRangeUser(0.1,2*nr);

  for (int iexp=0; iexp<NExp; iexp++) {
    
    int ne = 0;
    for (int i=0; i<nr; i++) {

      try         { uint32_t dat = dtc->ReadROCRegister(link,Reg,50); }
      catch (...) { ne += 1; }
    }

    h_nr->Fill(iexp,nr);
    h_ne->Fill(iexp,ne);
    
    if (Debug > 0) printf("nreads: %5i nerrors: %5i\n",nr, ne);

    h_nr->Draw("hist");
    h_ne->Draw("hist,sames");
    c->Modified();
    c->Update();
  }

  c->Print(Form("c_%02i_%i.png",Reg,Clock));
}
