/// \file Roll.h
/// \brief Header file for the "roll" gait
/// \detail This gait forms the snake into a constant arc, and rotates this arc
///   about the longitudinal axis of the snake, causing the snake to translate
///   laterally across the ground.

#ifndef ROLL_H
#define ROLL_H

#include <base/gait/Compound_serpenoid.h>

/// \brief Rolling arc gait.
class Roll:public Compound_serpenoid{
	public:
	/// Gait constructor
	Roll(int modules);

	/// Methods to copy the gait
	Roll(const Roll &from);
	Roll *clone() const;

  /// Automatically update compound serpenoid parameters.
  void parameter_adjust(Command const& c);
};

#endif
