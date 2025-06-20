//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifdef MFEM_ENABLED

#pragma once
#include "MFEMFunctorMaterial.h"

/**
 * Declares material properties based on names and values prescribed by input parameters.
 *
 * This is identical in function to the GenericConstantMaterial in Moose.
 */
class MFEMGenericConstantFunctorMaterial : public MFEMFunctorMaterial
{
public:
  static InputParameters validParams();

  MFEMGenericConstantFunctorMaterial(const InputParameters & parameters);
  virtual ~MFEMGenericConstantFunctorMaterial();

protected:
  const std::vector<std::string> & _prop_names;
  const std::vector<Real> & _prop_values;
};

#endif
