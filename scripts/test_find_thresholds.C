//

//-----------------------------------------------------------------------------
int test_find_thresholds(int PcieAddr, int Link, int Channel = -1, float VThreshold = 15, float VTolerance = 1) {

  uint16_t    val[96][2];
  const char* type[2] = {"cal","hv"};

  int  link_mask = (1 << 4*link_mask);
  bool skip_init(false);
  
  auto dtc_i = DtcInterface::Instance(PcieAddr,link_mask,skip_init);
  
  dtc_i->FindAlignments(1,Link);

  int ich1(Channel), ich2(Channel+1);
  if (Channel == -1) {
    ich1 = 0;
    ich2 = 96;
  }

  int nerrors;
  for (int ich=ich1; ich<ich2; ich++) {
    printf("-- channel %2i\n",ich);
    for (int k=0; k<2; ++k) {
      nerrors = 0;
      bool ok(false);
      while ((not ok) and (nerrors < 10)) {
        ok = dtc_i->FindThreshold(Link,ich,k,VThreshold,VTolerance,val[ich][k]);
        if (not ok) {
          printf(" -- ERROR ich=%i k=%i nerrors:%i\n",ich,k,nerrors);
          nerrors += 1;
        }
        else {
          printf("-- ich:%2i k:%i thr:%i\n",ich,k,val[ich][k]);
        }
      }
    }
  }
//-----------------------------------------------------------------------------
// after which one only needs to print the thresholds
//-----------------------------------------------------------------------------
  printf("[\n");
  for (int ich=ich1; ich<ich2; ich++) {
    for (int k=0; k<2; ++k) {
      printf("{\"channel\":%i,\"type\":\"%s\",\"threshold\":%i,\"gain\":370},\n",
             ich,type[k],val[ich][k]);
    }
  }
  printf("]\n");
  return 0;
}
