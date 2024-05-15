#ifndef __print_buffer_C__
#define __print_buffer_C__
//-----------------------------------------------------------------------------
// print data starting from ptr , 'nw' 2-byte words
// printout: 16 bytes per line, grouped in 2-byte words
//-----------------------------------------------------------------------------
void print_buffer(const void* ptr, int nw) {

  // int     nw  = nbytes/2;
  ushort* p16 = (ushort*) ptr;
  int     n   = 0;

  for (int i=0; i<nw; i++) {
    if (n == 0) printf(" 0x%08x: ",i*2);

    ushort  word = p16[i];
    printf("0x%04x ",word);

    n   += 1;
    if (n == 8) {
      printf("\n");
      n = 0;
    }
  }

  if (n != 0) printf("\n");
}

#endif
