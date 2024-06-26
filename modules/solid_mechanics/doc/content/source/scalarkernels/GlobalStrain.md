# GlobalStrain

!syntax description /ScalarKernels/GlobalStrain

## Description

`GlobalStrain` solves the ODE equation setting the integral of the stress generated by applied load or volume change unrelated to deformation to zero. It stores the strain components into scalar variables of different order. Strain components calculated from this scalar kernel contributes towards the periodic strain. Combined with periodic displacements, this helps maintaining full strain periodicity.

It requires an input of the `UserObject` that calculates the integral value and corresponding jacobians.

Please refer to [GlobalStrainUserObject](/GlobalStrainUserObject.md) for information about the residual and jacobian calculation.


## Example Input File Syntax

!listing modules/solid_mechanics/test/tests/global_strain/global_strain.i block=ScalarKernels/global_strain

!syntax parameters /ScalarKernels/GlobalStrain

!syntax inputs /ScalarKernels/GlobalStrain

!syntax children /ScalarKernels/GlobalStrain
