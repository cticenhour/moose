# A beam with its ends fully clamped
[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 1
  ny = 10
  nz = 10
  xmin = -10
  xmax = 10
  ymin = -10
  ymax = 10
  zmin = -50
  zmax = 0
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
  [./disp_z]
  [../]
[]

[Kernels]
  [SolidMechanics]
  [../]
  [./gravity_y]
    type = Gravity
    use_displaced_mesh = false
    variable = disp_y
    value = -10
  [../]
[]

[BCs]
  [./zmax_xfixed]
    type = DirichletBC
    variable = disp_x
    boundary = front
    value = 0
  [../]
  [./zmax_yfixed]
    type = DirichletBC
    variable = disp_y
    boundary = front
    value = 0
  [../]
  [./zmax_zfixed]
    type = DirichletBC
    variable = disp_z
    boundary = front
    value = 0
  [../]

  [./zmin_xfixed]
    type = DirichletBC
    variable = disp_x
    boundary = back
    value = 0
  [../]
  [./zmin_yfixed]
    type = DirichletBC
    variable = disp_y
    boundary = back
    value = 0
  [../]
  [./zmin_zfixed]
    type = DirichletBC
    variable = disp_z
    boundary = back
    value = 0
  [../]
[]

[AuxVariables]
  [./stress_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_xy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_xz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strainp_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strainp_xy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strainp_xz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strainp_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strainp_yz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strainp_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./straint_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./straint_xy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./straint_xz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./straint_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./straint_yz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./straint_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_shear]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_tensile]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_compressive]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./intnl_shear]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./intnl_tensile]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./iter]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./ls]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  [./stress_xx]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xx
    index_i = 0
    index_j = 0
  [../]
  [./stress_xy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xy
    index_i = 0
    index_j = 1
  [../]
  [./stress_xz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xz
    index_i = 0
    index_j = 2
  [../]
  [./stress_yy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_yy
    index_i = 1
    index_j = 1
  [../]
  [./stress_yz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_yz
    index_i = 1
    index_j = 2
  [../]
  [./stress_zz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_zz
    index_i = 2
    index_j = 2
  [../]
  [./strainp_xx]
    type = RankTwoAux
    rank_two_tensor = plastic_strain
    variable = strainp_xx
    index_i = 0
    index_j = 0
  [../]
  [./strainp_xy]
    type = RankTwoAux
    rank_two_tensor = plastic_strain
    variable = strainp_xy
    index_i = 0
    index_j = 1
  [../]
  [./strainp_xz]
    type = RankTwoAux
    rank_two_tensor = plastic_strain
    variable = strainp_xz
    index_i = 0
    index_j = 2
  [../]
  [./strainp_yy]
    type = RankTwoAux
    rank_two_tensor = plastic_strain
    variable = strainp_yy
    index_i = 1
    index_j = 1
  [../]
  [./strainp_yz]
    type = RankTwoAux
    rank_two_tensor = plastic_strain
    variable = strainp_yz
    index_i = 1
    index_j = 2
  [../]
  [./strainp_zz]
    type = RankTwoAux
    rank_two_tensor = plastic_strain
    variable = strainp_zz
    index_i = 2
    index_j = 2
  [../]
  [./straint_xx]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = straint_xx
    index_i = 0
    index_j = 0
  [../]
  [./straint_xy]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = straint_xy
    index_i = 0
    index_j = 1
  [../]
  [./straint_xz]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = straint_xz
    index_i = 0
    index_j = 2
  [../]
  [./straint_yy]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = straint_yy
    index_i = 1
    index_j = 1
  [../]
  [./straint_yz]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = straint_yz
    index_i = 1
    index_j = 2
  [../]
  [./straint_zz]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = straint_zz
    index_i = 2
    index_j = 2
  [../]
  [./f_shear]
    type = MaterialStdVectorAux
    property = plastic_yield_function
    index = 0
    variable = f_shear
  [../]
  [./f_tensile]
    type = MaterialStdVectorAux
    property = plastic_yield_function
    index = 1
    variable = f_tensile
  [../]
  [./f_compressive]
    type = MaterialStdVectorAux
    property = plastic_yield_function
    index = 2
    variable = f_compressive
  [../]
  [./intnl_shear]
    type = MaterialStdVectorAux
    property = plastic_internal_parameter
    index = 0
    variable = intnl_shear
  [../]
  [./intnl_tensile]
    type = MaterialStdVectorAux
    property = plastic_internal_parameter
    index = 1
    variable = intnl_tensile
  [../]
  [./iter]
    type = MaterialRealAux
    property = plastic_NR_iterations
    variable = iter
  [../]
  [./ls]
    type = MaterialRealAux
    property = plastic_linesearch_needed
    variable = ls
  [../]
[]

[UserObjects]
  [./coh_irrelevant]
    type = SolidMechanicsHardeningCubic
    value_0 = 2E6
    value_residual = 2E6
    internal_limit = 0.01
  [../]
  [./tanphi]
    type = SolidMechanicsHardeningCubic
    value_0 = 0.5
    value_residual = 0.5
    internal_limit = 0.01
  [../]
  [./tanpsi]
    type = SolidMechanicsHardeningConstant
    value = 0.166666666667
  [../]
  [./t_strength]
    type = SolidMechanicsHardeningCubic
    value_0 = 0
    value_residual = 0
    internal_limit = 0.1
  [../]
  [./c_strength]
    type = SolidMechanicsHardeningCubic
    value_0 = 1E80
    value_residual = 0.0
    internal_limit = 0.01
  [../]
[]

[Materials]
  [./elasticity_tensor]
    type = ComputeElasticityTensor
    fill_method = symmetric_isotropic
    C_ijkl = '6.4E9 6.4E9'  # young 16MPa, Poisson 0.25
  [../]
  [./strain]
    type = ComputeIncrementalStrain
  [../]
  [./admissible]
    type = ComputeMultipleInelasticStress
    inelastic_models = stress
    tangent_operator = nonlinear
    perform_finite_strain_rotations = false
  [../]
  [./stress]
    type = CappedWeakPlaneStressUpdate
    cohesion = coh_irrelevant
    tan_friction_angle = tanphi
    tan_dilation_angle = tanpsi
    tensile_strength = t_strength
    compressive_strength = c_strength
    max_NR_iterations = 1000
    tip_smoother = 1E5
    smoothing_tol = 1E5
    yield_function_tol = 1E-5
    perfect_guess = true
    min_step_size = 0.1
  [../]
  [./density]
    type = GenericFunctionMaterial
    block = 0
    prop_names = density
    prop_values = 1E3*t
  [../]
[]

[Preconditioning]
  [./andy]
    type = SMP
    full = true
    #petsc_options = '-snes_converged_reason -snes_linesearch_monitor'
    petsc_options_iname = '-pc_type -pc_asm_overlap -sub_pc_type -ksp_type -ksp_gmres_restart'
    petsc_options_value = ' asm      2              lu            gmres     200'
  [../]
[]

[Executioner]
  solve_type = 'NEWTON'
  petsc_options = '-snes_converged_reason'
  line_search = bt
  nl_abs_tol = 1E-2
  nl_rel_tol = 1e-15
  l_tol = 1E-10

  l_max_its = 100
  nl_max_its = 100
  end_time = 10
  dt = 1
  type = Transient
[]

[Outputs]
  file_base = beam
  exodus = true
  csv = true
[]
