[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 2
  ny = 2
  parallel_type = replicated
[]

[Variables]
  [./u]
    order = FIRST
    family = LAGRANGE
  [../]
  [./pot]
  [../]
[]

[Kernels]
[]

[BCs]
  [./left_pot]
    boundary = left
    type = ExampleShapeSideIntegratedBC
    variable = pot
    num_user_object = num_user_object
    denom_user_object = denom_user_object
    v = u
    Vb = 1
  [../]
[]

[UserObjects]
  [./num_user_object]
    type = NumShapeSideUserObject
    u = u
    boundary = left
    execute_on = 'linear nonlinear'
  [../]
  [./denom_user_object]
    type = DenomShapeSideUserObject
    u = u
    boundary = left
    execute_on = 'linear nonlinear'
  [../]
[]

[Problem]
  type = FEProblem
  kernel_coverage_check = false
  use_hash_table_matrix_assembly = true
[]

[Preconditioning]
  [./smp]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Steady
  solve_type = NEWTON
[]

[Outputs]
  exodus = true
  perf_graph = true
[]

[ICs]
  [./u]
    type = RandomIC
    variable = u
  [../]
  [./pot]
    type = RandomIC
    variable = pot
  [../]
[]
