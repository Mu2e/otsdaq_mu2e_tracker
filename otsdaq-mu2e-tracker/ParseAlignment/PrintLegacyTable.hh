// Ed Callaghan
// Reimplement a correct version of the legacy trackerScripts alignment dump
// July 2024

#ifndef LegacyPrintTable_h
#define LegacyPrintTable_h

#include <iomanip>
#include <ios>
#include <iostream>
#include <string>
#include "otsdaq-mu2e-tracker/ParseAlignment/Alignment.hh"
#include "otsdaq-mu2e-tracker/ParseAlignment/AlignmentIteration.hh"
#include "otsdaq-mu2e-tracker/ParseAlignment/AlignmentChannel.hh"

template<typename T0, typename T1, typename T2, typename T3,
         typename T4, typename T5, typename T6>
void print_legacy_leading_row(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6){
  std::cout << std::setfill(' ') << std::setw(8) << a0
            << " "
            << std::setfill(' ') << std::setw(8) << a1
            << " "
            << std::setfill(' ') << std::setw(8) << a2
            << "| "
            << std::setfill(' ') << std::setw(17) << a3
            << "| "
            << std::setfill(' ') << std::setw(8) << a4
            << " "
            << std::setfill(' ') << std::setw(8) << a5
            << "| "
            << std::setfill(' ') << std::setw(8) << a6
            << std::endl;
}

template<typename T0, typename T1, typename T2, typename T3,
         typename T4, typename T5, typename T6, typename T7>
void print_legacy_row(T0 a0, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7){
  std::cout << std::setfill(' ') << std::setw(8) << a0
            << " "
            << std::setfill(' ') << std::setw(8) << a1
            << " "
            << std::setfill(' ') << std::setw(8) << a2
            << "| "
            << std::setfill(' ') << std::setw(8) << a3
            << " "
            << std::setfill(' ') << std::setw(8) << a4
            << "| "
            << std::setfill(' ') << std::setw(8) << a5
            << " "
            << std::setfill(' ') << std::setw(8) << a6
            << "| "
            << std::setfill(' ') << std::setw(8) << a7
            << std::endl;
}

char maybe_asterisk(bool);

void print_legacy_table(const Alignment& alignment);

#endif
