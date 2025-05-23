//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "BCUserObject.h"

InputParameters
BCUserObject::validParams()
{
  InputParameters params = GeneralUserObject::validParams();
  return params;
}

BCUserObject::BCUserObject(const InputParameters & parameters) : GeneralUserObject(parameters) {}

void
BCUserObject::initialize()
{
}

void
BCUserObject::execute()
{
}

void
BCUserObject::finalize()
{
}
