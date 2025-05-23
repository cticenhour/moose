//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "CHInterfaceBase.h"

/**
 * This is the Cahn-Hilliard equation base class that implements the interfacial
 * or gradient energy term of the equation. With a scalar (isotropic) mobility.
 */
class CHInterface : public CHInterfaceBase<Real>
{
public:
  static InputParameters validParams();

  CHInterface(const InputParameters & parameters);
};
