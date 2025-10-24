//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "FunctorInterface.h"

/**
 * \class NonADFunctorInterface
 * \brief An interface for accessing \p Moose::Functors for systems that do not care about automatic
 * differentiation, e.g. auxiliary kernels
 */
class NonADFunctorInterface : public FunctorInterface
{
public:
  static InputParameters validParams();

  NonADFunctorInterface(const MooseObject * moose_object);

#ifdef MOOSE_KOKKOS_ENABLED
  /**
   * Special constructor used for Kokkos functor copy during parallel dispatch
   */
  NonADFunctorInterface(const NonADFunctorInterface & object,
                        const Moose::Kokkos::FunctorCopy & key);
#endif

private:
  virtual bool isADObject() const override { return false; }
};
