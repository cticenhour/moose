//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Action.h"

class AdaptiveSamplerAction : public Action
{
public:
  static InputParameters validParams();

  AdaptiveSamplerAction(const InputParameters & params);

  /**
   * Returns the names of all the existing adaptive samplers
   */
  static std::set<SamplerName> adaptiveSamplerNames()
  {
    return {"ParallelSubsetSimulation",
            "AdaptiveImportance",
            "AISActiveLearning",
            "ActiveLearningMonteCarloSampler"};
  }

  /**
   * Returns the name of the AdaptiveSamplingCompletedPostprocessor
   */
  static PostprocessorName adaptiveSamplingCompletedPostprocessorName()
  {
    return "_sampling_completed_pp";
  }

  virtual void act() override;
};
