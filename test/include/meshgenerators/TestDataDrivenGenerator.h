//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "MeshGenerator.h"

class TestDataDrivenGenerator : public MeshGenerator
{
public:
  static InputParameters validParams();

  TestDataDrivenGenerator(const InputParameters & parameters);

protected:
  virtual void generateData() override;
  virtual std::unique_ptr<MeshBase> generate() override;

  std::unique_ptr<MeshBase> * _subgenerator_no_data_only_mesh;
  std::unique_ptr<MeshBase> * _subgenerator_no_data_only_submesh = nullptr;
};
