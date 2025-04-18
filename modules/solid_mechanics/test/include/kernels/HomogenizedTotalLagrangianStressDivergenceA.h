//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "TotalLagrangianStressDivergenceS.h"

// Helpers common to the whole homogenization system
namespace HomogenizationA
{
/// Moose constraint type, for input
const MultiMooseEnum constraintType("strain stress none");
/// Constraint type: stress/PK stress or strain/deformation gradient
enum class ConstraintType
{
  Strain,
  Stress,
  None
};
typedef std::map<std::pair<unsigned int, unsigned int>, std::pair<ConstraintType, const Function *>>
    ConstraintMap;
}

/// Total Lagrangian formulation with most homogenization terms (one disp_xyz field and one scalar)
/// The macro_gradient variable is split into two scalars: the first component called '_hvar'
/// herein and all other components called '_avar' herein. For parameter _beta = 0, the primary
/// scalar (_kappa) is _hvar and the coupled scalar is _avar. For parameter _beta = 1, the primary
/// scalar (_kappa) is _avar and the coupled scalar is _hvar. Just like the primary field variable
/// (_var) is either disp_x or disp_y or disp_z depending on _alpha.
///
/// Thus, each instance of HomogenizedTotalLagrangianStressDivergenceA acts on one field variable
/// (_disp_alpha) and one scalar variable (_hvar_beta). The job of the kernel is to assemble the
/// residual of all dofs of _disp_alpha and of all dofs of _hvar_beta (namely, selected entries).
/// It assembles a symmetric portion of the Jacobian for _disp_alpha and _hvar_beta, with some
/// logical checks to access only the particular desired terms (often for _alpha or _beta = 0).
/// The entries for the other field/scalar variables are handled by other instances of the
/// kernel, which have other values of _alpha AND _beta. The logical checks ensure the proper
/// decomposition of the jobs.
///
/// In summary, for x=disp_x etc. and h=_hvar and a=_avar, then the contributions of the instances are
/// _alpha=0, _beta=0
/// R = [Rx,  00,  00,  Rh,  00 ]^T
/// J = [Jxx, Jxy, Jxz, Jxh, 000
///      Jhx, 000, 000, Jhh, Jha]
/// _alpha=1, _beta=0
/// R = [00,  Ry,  00,  00,  00 ]^T
/// J = [Jyx, Jyy, Jyz, Jyh, 000
///      000, Jhy, 000, 000, 000]
/// _alpha=2, _beta=0
/// R = [00,  00,  Rz,  00,  00 ]^T
/// J = [Jzx, Jzy, Jzz, Jzh, 000
///      000, 000, Jhz, 000, 000]
/// _alpha=0, _beta=1
/// R = [00,  00,  00,  00,  Ra ]^T
/// J = [000, 000, 000, 000, Jxa
///      Jax, 000, 000, Jah, Jaa]
/// _alpha=1, _beta=1
/// R = [00,  00,  00,  00,  00 ]^T
/// J = [000, 000, 000, 000, Jya
///      000, Jay, 000, 000, 000]
/// _alpha=2, _beta=1
/// R = [00,  00,  00,  00,  00 ]^T
/// J = [000, 000, 000, 000, Jza
///      000, 000, Jaz, 000, 000]
///
/// In this manner, the full R and J are obtained with NO duplication of jobs:
/// R = [Rx,  Ry,  Rz,  Rh,  Ra ]^T
/// J = [Jxx, Jxy, Jxz, Jxh, Jxa
///      Jyx, Jyy, Jyz, Jyh, Jya
///      Jzx, Jzy, Jzz, Jzh, Jza
///      Jhx, Jhy, Jhz, Jhh, Jha
///      Jax, Jay, Jaz, Jah, Jaa]
///
class HomogenizedTotalLagrangianStressDivergenceA : public TotalLagrangianStressDivergenceS
{
public:
  static InputParameters validParams();
  HomogenizedTotalLagrangianStressDivergenceA(const InputParameters & parameters);

protected:
  // Add overrides to base class contributions to only happen for _beta==0, to happen only once
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobianDisplacement(unsigned int alpha, unsigned int beta) override;

  /**
   * Method for computing the scalar part of residual for _kappa
   */
  virtual void computeScalarResidual() override;

  /**
   * Method for computing the scalar variable part of Jacobian for d-_kappa-residual / d-_kappa
   */
  virtual void computeScalarJacobian() override;

  /**
   * Method for computing an off-diagonal jacobian component d-_kappa-residual / d-jvar
   * jvar is looped over all field variables, which herein is just disp_x and disp_y
   */
  virtual void computeScalarOffDiagJacobian(const unsigned int jvar_num) override;

  /**
   * Method for computing an off-diagonal jacobian component at quadrature points.
   */
  virtual Real computeScalarQpOffDiagJacobian(const unsigned int jvar_num) override;

  /**
   * Method for computing an off-diagonal jacobian component d-_var-residual / d-svar.
   * svar is looped over all scalar variables, which herein is just _kappa and _kappa_other
   */
  virtual void computeOffDiagJacobianScalarLocal(const unsigned int svar_num) override;

  /**
   * Method for computing d-_var-residual / d-_svar at quadrature points.
   */
  virtual Real computeQpOffDiagJacobianScalar(const unsigned int svar_num) override;

  /**
   * Method for computing an off-diagonal jacobian component d-_kappa-residual / d-svar
   * svar is looped over other scalar variables, which herein is just _kappa_other
   */
  virtual void computeScalarOffDiagJacobianScalar(const unsigned int svar_num) override;

protected:
  /// Which component of the scalar vector residual this constraint is responsible for
  const unsigned int _beta;

  /// (Pointer to) Scalar variable this kernel operates on
  const MooseVariableScalar * const _kappao_var_ptr;

  /// The unknown scalar variable ID
  const unsigned int _kappao_var;

  /// Order of the scalar variable, used in several places
  const unsigned int _ko_order;

  /// Reference to the current solution at the current quadrature point
  const VariableValue & _kappa_other;

  /// Type of each constraint (stress or strain) for each component
  HomogenizationA::ConstraintMap _cmap;

  /// The constraint type; initialize with 'none'
  HomogenizationA::ConstraintType _ctype = HomogenizationA::ConstraintType::None;

  /// Used internally to iterate over each scalar component
  unsigned int _m;
  unsigned int _n;
  unsigned int _a;
  unsigned int _b;
};
