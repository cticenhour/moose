//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

// MOOSE includes
#include "MeshGenerator.h"

/**
 * MeshGenerator for assigning subdomain IDs of all elements
 */
class ElemUniqueSubdomainsGenerator : public MeshGenerator
{
public:
  static InputParameters validParams();

  ElemUniqueSubdomainsGenerator(const InputParameters & parameters);

  std::unique_ptr<MeshBase> generate() override;

protected:
  std::unique_ptr<MeshBase> & _input;
};
