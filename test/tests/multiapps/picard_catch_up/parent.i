[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 2
  ny = 2
  parallel_type = replicated
[]

[Variables]
  [u]
  []
[]

[AuxVariables]
  [v]
  []
[]

[Kernels]
  [diff]
    type = CoefDiffusion
    variable = u
    coef = 0.1
  []
  [time]
    type = TimeDerivative
    variable = u
  []
  [force_u]
    type = CoupledForce
    variable = u
    v = 'v'
  []
[]

[BCs]
  [left]
    type = DirichletBC
    variable = u
    preset = false
    boundary = 'left'
    value = 0
  []
  [right]
    type = DirichletBC
    variable = u
    preset = false
    boundary = 'right'
    value = 1
  []
[]

[Postprocessors]
  [picard_its]
    type = NumFixedPointIterations
    execute_on = 'initial timestep_end'
  []
[]

[Executioner]
  type = Transient
  num_steps = 2
  dt = 0.1
  solve_type = PJFNK
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_type'
  petsc_options_value = 'lu mumps'
  fixed_point_max_its = 30
  nl_abs_tol = 1e-14
[]

[Outputs]
  exodus = true
[]

[MultiApps]
  [sub]
    type = TransientMultiApp
    app_type = MooseTestApp
    positions = '0 0 0'
    input_files = 'sub.i'
    max_catch_up_steps = 100
    max_failures = 100
    catch_up = true
  []
[]

[Transfers]
  [v_from_sub]
    type = MultiAppGeneralFieldNearestLocationTransfer
    from_multi_app = sub
    source_variable = v
    variable = v
  []
  [u_to_sub]
    type = MultiAppGeneralFieldNearestLocationTransfer
    to_multi_app = sub
    source_variable = u
    variable = u
  []
[]
