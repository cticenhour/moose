//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SolveObject.h"

#include "Executioner.h"
#include "FEProblem.h"
#include "DisplacedProblem.h"
#include "NonlinearSystemBase.h"
#include "LinearSystem.h"

SolveObject::SolveObject(Executioner & ex)
  : MooseObject(ex.parameters()),
    PerfGraphInterface(this),
    PostprocessorInterface(this),
    _executioner(ex),
    _problem(*getCheckedPointerParam<FEProblemBase *>(
        "_fe_problem_base", "This might happen if you don't have a mesh")),
    _displaced_problem(_problem.getDisplacedProblem().get()),
    _mesh(_problem.mesh()),
    _displaced_mesh(_displaced_problem ? &_displaced_problem->mesh() : nullptr),
    _solver_sys(_problem.numNonlinearSystems()
                    ? static_cast<SystemBase &>(_problem.getNonlinearSystemBase(/*nl_sys=*/0))
                    : static_cast<SystemBase &>(_problem.getLinearSystem(/*l_sys_num=*/0))),
    _aux(_problem.getAuxiliarySystem()),
    _inner_solve(nullptr)
{
}
