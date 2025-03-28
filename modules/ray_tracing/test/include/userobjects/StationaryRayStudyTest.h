//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "RayTracingStudy.h"

class StationaryRayStudyTest : public RayTracingStudy
{
public:
  static InputParameters validParams();

  StationaryRayStudyTest(const InputParameters & parameters);

protected:
  virtual void generateRays() override;

  std::vector<std::pair<RayDataIndex, const Function *>> _data_functions;
  std::vector<std::pair<RayDataIndex, const Function *>> _aux_data_functions;
};
