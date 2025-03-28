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
class Sampler;

class AdaptiveSamplingCompletedPostprocessor : public GeneralPostprocessor
{
public:
  static InputParameters validParams();
  AdaptiveSamplingCompletedPostprocessor(const InputParameters & parameters);

  virtual void initialize() override {}
  virtual void execute() override;
  virtual Real getValue() const override;
  virtual void initialSetup() override{};
  virtual void finalize() override;

protected:
  /// Sampler for  which we want to know if the sampling has been completed
  const Sampler & _sampler;
  /// Whether the sampling has been completed
  bool _sampling_completed;
};
