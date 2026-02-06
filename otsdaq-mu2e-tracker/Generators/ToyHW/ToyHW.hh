#ifndef otsdaq_mu2e_tracker_Generators_ToyHW_ToyHW_hh
#define otsdaq_mu2e_tracker_Generators_ToyHW_ToyHW_hh

#include "artdaq-core-demo/Overlays/FragmentType.hh"

#include "fhiclcpp/fwd.h"

#include <chrono>
#include <random>

class ToyHW {
public:
  typedef uint16_t data_t;  ///< The type used to represent ADC counts (which are 12 or 14 bits, for TOY1 or TOY2)

  explicit ToyHW(fhicl::ParameterSet const& ps);

  void StartDatataking();
  void StopDatataking ();

  void FillBuffer           (char* buffer, size_t* bytes_read);
  void AllocateReadoutBuffer(char** buffer);
  void FreeReadoutBuffer    (const char* buffer);

  int SerialNumber() const;
  int NumADCBits  () const;
  int BoardType   () const;

  enum class DistributionType {
    uniform,        ///< A uniform distribution
    gaussian,       ///< A Gaussian distribution
    monotonic,      ///< A monotonically-increasing distribution
    uninitialized,  ///< A use-after-free expliot distribution
    uninit2         // like uninitialized, but do memcpy
  };

private:
  bool               taking_data_;
  std::size_t        nADCChannels_;
  std::size_t        maxADCcounts_;
  std::size_t        change_after_N_seconds_;
  std::size_t        pause_after_N_seconds_;  // sleep this many seconds every change_after_N_seconds_
  std::size_t        nADCChannels_after_N_seconds_;
  bool               exception_after_N_seconds_;
  bool               exit_after_N_seconds_;
  bool               abort_after_N_seconds_;
  bool               hang_after_N_seconds_;
  demo::FragmentType fragment_type_;
  std::size_t        maxADCvalue_;
  std::size_t        throttle_usecs_;
  std::size_t        usecs_between_sends_;
  //	DistributionType distribution_type_;

  using time_type = decltype(std::chrono::steady_clock::now());

  const time_type fake_time_ = std::numeric_limits<time_type>::max();

  time_type          start_time_;
  int                send_calls_;
  int                serial_number_;
};

#endif
