


//-----------------------------------------------------------------------------
DtcGui* dtc_gui(int DebugLevel = 0) {
  // 1200x800: dimensions of the main frame
  
  const char* hostname = gSystem->Getenv("HOSTNAME");

  DtcGui* x = new DtcGui(hostname,gClient->GetRoot(),950,1000,DebugLevel);
  return x;
} 
