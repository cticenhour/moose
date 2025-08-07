# AddMFEMProblemOperatorAction

!if! function=hasCapability('mfem')

## Summary

Set the ProblemOperator used in the MFEMProblemSolve to solve the FE problem.

## Overview

Action called to add a [`ProblemOperator`](source/mfem/problem_operators/ProblemOperator.md) to the
problem from an MFEM executioner. This action is run as part of the MFEM problem setup if the
`Problem` type is set to [`MFEMProblem`](source/mfem/problem/MFEMProblem.md).

!if-end!

!else
!include mfem/mfem_warning.md
