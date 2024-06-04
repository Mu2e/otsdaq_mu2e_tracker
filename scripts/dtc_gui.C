


//-----------------------------------------------------------------------------
DtcGui* dtc_gui(const char* Project = "test", int DebugLevel = 0) {
  // 1200x800: dimensions of the main frame
  DtcGui* x = new DtcGui(Project,gClient->GetRoot(),950,1000,DebugLevel);
  return x;
} 
