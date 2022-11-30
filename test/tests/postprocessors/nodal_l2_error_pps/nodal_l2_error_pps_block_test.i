[Mesh]
  [./init_mesh]
    type = GeneratedMeshGenerator
    dim = 1
    nx = 10
    xmax = 2
  [../]
  [./subdomain1]
    type = SubdomainBoundingBoxGenerator
    bottom_left = '1.0 0 0'
    block_id = 1
    top_right = '2.0 1.0 0'
    input = init_mesh
  [../]
  [./interface]
    type = SideSetsBetweenSubdomainsGenerator
    primary_block = '0'
    paired_block = '1'
    new_boundary = 'master0_interface'
    input = subdomain1
  [../]
[]

[Variables]
  [./u] # u(x) = 1 in block 0, 0 otherwise.
    order = FIRST
    family = LAGRANGE
    block = 0
  [../]
  [./dummy] # dummy variable used in block 1, dummy(x) = 0
    order = FIRST
    family = LAGRANGE
    block = 1
  [../]
[]

[Kernels]
  [./diff_u]
    type = Diffusion
    variable = u
    block = '0'
  [../]
  [./diff_dummy]
    type = Diffusion
    variable = dummy
    block = '1'
  [../]
[]

[BCs]
  [./left_u]
    type = DirichletBC
    variable = u
    boundary = 'left'
    value = 1
  [../]
  [./right_dummy]
    type = DirichletBC
    variable = dummy
    boundary = 'right'
    value = 0
  [../]
[]

[Executioner]
  type = Steady
  solve_type = NEWTON
[]

[Outputs]
  print_linear_residuals = true
  exodus = true
  csv = true
  hide = dummy
[]

[Postprocessors]
  [./nodal_error_u]
    type = NodalL2Error
    function = 1
    variable = u
    block = 0
  [../]
[]
