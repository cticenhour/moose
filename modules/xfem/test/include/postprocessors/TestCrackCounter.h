//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "GeneralPostprocessor.h"

// Forward Declarations
class XFEM;

class TestCrackCounter : public GeneralPostprocessor
{
public:
  static InputParameters validParams();

  TestCrackCounter(const InputParameters & parameters);

  /// Initialize the number of Cracks.
  virtual void initialize() override;

  /// Calculates the number of Cracks
  virtual void execute() override;

  /// Get number of Cracks
  virtual Real getValue() const override;

protected:
  /// Variable used to write out the number of Cracks
  unsigned int _number_of_cracks;

private:
  std::shared_ptr<XFEM> _xfem;
};
