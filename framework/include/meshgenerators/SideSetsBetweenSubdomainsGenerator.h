//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "SideSetsGeneratorBase.h"

/**
 * MeshGenerator that creates a sideset composed of the nodes located between
 * two or more subdomains.
 */
class SideSetsBetweenSubdomainsGenerator : public SideSetsGeneratorBase
{
public:
  static InputParameters validParams();

  SideSetsBetweenSubdomainsGenerator(const InputParameters & parameters);

  std::unique_ptr<MeshBase> generate() override;
};
