//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "KokkosNodalBCBase.h"

namespace Moose
{
namespace Kokkos
{

/**
 * The base class for a user to derive their own Kokkos nodal boundary conditions.
 *
 * The user should define computeQpResidual(), computeQpJacobian(), and computeQpOffDiagJacobian()
 * as inlined public methods in their derived class (not virtual override). The signature of
 * computeQpResidual() expected to be defined in the derived class is as follows:
 *
 * @param node The contiguous node ID
 * @returns The residual contribution
 *
 * KOKKOS_FUNCTION Real computeQpResidual(const ContiguousNodeID node) const;
 *
 * The signatures of computeQpJacobian() and computeOffDiagQpJacobian() can be found in the code
 * below, and their definition in the derived class is optional. If they are defined in the derived
 * class, they will hide the default definitions in the base class.
 */
class NodalBC : public NodalBCBase
{
public:
  static InputParameters validParams();

  /**
   * Constructor
   */
  NodalBC(const InputParameters & parameters);

  /**
   * Dispatch residual calculation
   */
  virtual void computeResidual() override;
  /**
   * Dispatch diagonal and off-diagonal Jacobian calculation
   */
  virtual void computeJacobian() override;

  /**
   * Default methods to prevent compile errors even when these methods were not defined in the
   * derived class
   */
  ///@{
  /**
   * Compute diagonal Jacobian contribution on a node
   * @param node The contiguous node ID
   * @returns The diagonal Jacobian contribution
   */
  KOKKOS_FUNCTION Real computeQpJacobian(const ContiguousNodeID /* node */) const { return 1; }
  /**
   * Compute off-diagonal Jacobian contribution on a node
   * @param jvar The variable number for column
   * @param node The contiguous node ID
   * @returns The off-diagonal Jacobian contribution
   */
  KOKKOS_FUNCTION Real computeQpOffDiagJacobian(const unsigned int /* jvar */,
                                                const ContiguousNodeID /* node */) const
  {
    return 0;
  }
  /**
   * Get the function pointer of the default computeQpJacobian()
   * @returns The function pointer
   */
  static auto defaultJacobian() { return &NodalBC::computeQpJacobian; }
  /**
   * Get the function pointer of the default computeQpOffDiagJacobian()
   * @returns The function pointer
   */
  static auto defaultOffDiagJacobian() { return &NodalBC::computeQpOffDiagJacobian; }
  ///@}

  /**
   * The parallel computation entry functions called by Kokkos
   */
  ///@{
  template <typename Derived>
  KOKKOS_FUNCTION void operator()(ResidualLoop, const ThreadID tid, const Derived & bc) const;
  template <typename Derived>
  KOKKOS_FUNCTION void operator()(JacobianLoop, const ThreadID tid, const Derived & bc) const;
  template <typename Derived>
  KOKKOS_FUNCTION void
  operator()(OffDiagJacobianLoop, const ThreadID tid, const Derived & bc) const;
  ///@}

protected:
  /**
   * Current solution at nodes
   */
  const VariableNodalValue _u;
};

template <typename Derived>
KOKKOS_FUNCTION void
NodalBC::operator()(ResidualLoop, const ThreadID tid, const Derived & bc) const
{
  auto node = kokkosBoundaryNodeID(tid);
  auto & sys = kokkosSystem(_kokkos_var.sys());

  if (!sys.isNodalDefined(node, _kokkos_var.var()))
    return;

  Real local_re = bc.computeQpResidual(node);

  accumulateTaggedNodalResidual(false, local_re, node);
}

template <typename Derived>
KOKKOS_FUNCTION void
NodalBC::operator()(JacobianLoop, const ThreadID tid, const Derived & bc) const
{
  auto node = kokkosBoundaryNodeID(tid);
  auto & sys = kokkosSystem(_kokkos_var.sys());

  if (!sys.isNodalDefined(node, _kokkos_var.var()))
    return;

  Real local_ke = bc.computeQpJacobian(node);

  // This initializes the row to zero except the diagonal
  accumulateTaggedNodalMatrix(false, local_ke, node, _kokkos_var.var());
}

template <typename Derived>
KOKKOS_FUNCTION void
NodalBC::operator()(OffDiagJacobianLoop, const ThreadID tid, const Derived & bc) const
{
  auto node = kokkosBoundaryNodeID(_thread(tid, 1));
  auto & sys = kokkosSystem(_kokkos_var.sys());
  auto jvar = sys.getCoupling(_kokkos_var.var())[_thread(tid, 0)];

  if (!sys.isNodalDefined(node, _kokkos_var.var()))
    return;

  Real local_ke = bc.computeQpOffDiagJacobian(jvar, node);

  accumulateTaggedNodalMatrix(true, local_ke, node, jvar);
}

} // namespace Kokkos
} // namespace Moose
