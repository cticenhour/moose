//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "PINSFVMassAdvection.h"

/**
 * A flux kernel transporting mass in porous media across cell faces
 */
class PWCNSFVMassAdvection : public PINSFVMassAdvection
{
public:
  static InputParameters validParams();
  PWCNSFVMassAdvection(const InputParameters & params);

protected:
  virtual bool hasMaterialTimeDerivative() const override { return true; }
};
