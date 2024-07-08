//////////////////////////////////////////////////////////////////////////////
// parse file written by the R/O test of dtc_gui
//////////////////////////////////////////////////////////////////////////////
#include "TH1.h"
#include "stdlib.h"
//-----------------------------------------------------------------------------
int plot_readout_errors(const char* Fn = "a1.txt") {
  
  FILE* f = fopen(Fn,"r");
  if (f == 0) {
    return -1;
  }
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
  struct data_t {
    ulong ewt;
    int   ewt64;
    int   ind;
    ulong offset;
    uint  w;
    uint  ew;
  } dat;


  char c[10000];


  TH1F *h_ewt64, *h_xor, *h_ind;

  h_ewt64 = new TH1F("h_ewt64","ewt % 64",100 ,0,100);
  h_ind   = new TH1F("h_ind  ","mismatched word index",2000,0,2000);
  h_xor   = new TH1F("h_xor"  ,"mismatched bit number in dat.w ^ dat.ew",32,0,32);
  
  int done = 0;
  while ( ((c[0]=getc(f)) != EOF) && !done) {
    // check if it is a comment line
    if (c[0] != '#') {
      ungetc(c[0],f);
      fscanf(f,"%lu",&dat.ewt);
      fscanf(f,"%i" ,&dat.ewt64);
      fscanf(f,"%i" ,&dat.ind);
      fscanf(f,"%lx",&dat.offset);
      fscanf(f,"%x" ,&dat.w);
      fscanf(f,"%x" ,&dat.ew);

      //      printf("%10lu %3i %3i %10lu, 0x%08x 0x%08x\n",dat.ewt,dat.ewt64,dat.ind,dat.offset,dat.w,dat.ew);
      
      h_ewt64->Fill(dat.ewt64);
      h_ind->Fill(dat.ind);

      uint xorr = dat.w^dat.ew;

      for (int i=0; i<32; i++) {
        if (((xorr >> i) & 0x1) == 1) {
                                        // mismatch
          h_xor->Fill(i);
        }
      }
    }
    // skip line or read end of line
    fgets(c,500,f);
  }

  h_ewt64->Draw();

  fclose(f);
  return 0;
}
