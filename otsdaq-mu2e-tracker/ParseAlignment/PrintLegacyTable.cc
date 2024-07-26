// Ed Callaghan
// Reimplement a correct version of the legacy trackerScripts alignment dump
// July 2024

#include "otsdaq-mu2e-tracker/ParseAlignment/PrintLegacyTable.hh"

char maybe_asterisk(bool condition){
  if (condition){
    return '*';
  }
  return ' ';
}

void print_legacy_table(const Alignment& alignment){
  std::string divider = "--";
  for (size_t i = 0 ; i < 7 ; i++){
    divider += "---------";
  }
  if (alignment.IfPatternCheck()){
    for (size_t i = 0 ; i < 2 ; i++){
      divider += "---------";
    }
  }

  std::cout << "('EyeMonitorWidth', "
            << alignment.EyeMonitorWidth()
            << ")"
            << std::endl;
  std::cout << "('IfPatternCheck', "
            << alignment.IfPatternCheck()
            << ")"
            << std::endl;
  std::cout << "('FaultedADC', "
            << alignment.FaultedADC()
            << ")"
            << std::endl;

  std::cout.setf(std::ios_base::left);
  for (const auto& iteration: alignment.Iterations()){
    std::cout << "******   Iteration " 
              << iteration.Index()
              << "   ******"
              << std::endl;

    print_legacy_leading_row("ADCPhase", iteration.ADCPhase(), "", "Alignment",
                             "Bitslip", "", "Pttn 0x263");
    print_legacy_row("ADC#", "Straw#", "Active", "Complete",
                     "Error", "Done", "Step", "Fail");

    bool check = alignment.IfPatternCheck();
    const auto& channels = iteration.Channels();
    for (size_t i = 0 ; i < channels.size() ; i++){
      const auto& channel = channels[i];

      const auto adc = channel.ADC();
      const auto straw = channel.Channel();
      const auto active = maybe_asterisk(channel.Active());
      const auto complete = maybe_asterisk(channel.Complete());
      const auto error = maybe_asterisk(channel.Error());
      const auto done = maybe_asterisk(channel.BitSlipDone());
      const auto step = channel.BitSlipStep();
      const auto fail = maybe_asterisk(channel.PatternMatch() && check);

      if (i % 8 == 0){
        std::cout << divider << std::endl;
      }
      print_legacy_row(adc, straw, active, complete, error, done, step, fail);
    }
  }
}
