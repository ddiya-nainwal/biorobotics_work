/// \file Roll.cpp
/// \brief Function implementations for the "Roll" gait.

#include "Roll.h"
#include<base/parameter/Parameter_double.h>

/// \brief Roll constructor
/// \detail The Amplitude parameter controls the diameter of the arc; higher
///   amplitudes are smaller diameter arcs.
Roll::Roll(int modules):Compound_serpenoid(modules,4/3.8){
	help("Rolls sideways in an arc.");
	// Compute the amplitude for the arc, based on the number of modules.
	double default_amplitude=.18*(16.0/(modules?modules:1));
	if (default_amplitude > 1.0) default_amplitude = 1.0;

	add_parameter(
		Parameter_double("Amplitude",0,0.0,1.0,default_amplitude)
		.help("How much each module should bend, in radians")
	);

  name_ = "Roll";

  speed_enable();
  pause_enable();
  direction_enable(LATERAL);

	/// To enact the rolling motion, the phase shift between the horizontal and
	/// vertical waves is 1/4 of a full period.  The negative sign causes the
	/// direction of motion to match the conventions in the software (otherwise,
	/// selecting "Left" would move the snake to the right).
	temporal_phase_offset = -0.25;

    /// To match the direction of motion of the rolling gait to that of the other
    /// gaits, we invert the temporal frequency multipliers for the serpenoid
	///x.cycles_per_period = y.cycles_per_period = -1;

  /// Initialize other compound serpenoid parameters.
	x.amplitude_start = x.amplitude_end =
	y.amplitude_start = y.amplitude_end =
		ANGLE_MAX*Parameter_double_value("Amplitude");
}

/// \brief Copy constructor
/// \detail There is nothing to copy, as parameters are handled by the base copy
///   constructor.
Roll::Roll(const Roll &from):Compound_serpenoid(from){ }

/// \brief The clone function just calls the copy constructor.
Roll *Roll::clone() const{ return new Roll(*this); }

/// \brief Automatically adjust compound serpenoid parameters when
///   gait parameters are changed.
void Roll::parameter_adjust(Command const& c)
{
  Gait::parameter_adjust(c);

  //Update the compound serpenoid parameters.
	x.amplitude_start = x.amplitude_end =
	y.amplitude_start = y.amplitude_end =
		ANGLE_MAX*Parameter_double_value("Amplitude");
}
