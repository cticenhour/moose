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

class GeneralizedPlaneStrainAction : public Action
{
public:
  static InputParameters validParams();

  GeneralizedPlaneStrainAction(const InputParameters & params);

  void act() override;

protected:
  std::vector<VariableName> _displacements;

  /// Number of displacement variables
  unsigned int _ndisp;
  const unsigned int _out_of_plane_direction;
};
