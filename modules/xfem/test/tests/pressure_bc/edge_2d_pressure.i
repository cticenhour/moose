[GlobalParams]
  order = FIRST
  family = LAGRANGE
  displacements = 'disp_x disp_y'
  volumetric_locking_correction = False
[]

[XFEM]
  qrule = volfrac
  output_cut_plane = true
[]

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 9
  xmin = 0.0
  xmax = 1.0
  ymin = 0.0
  ymax = 1.0
  elem_type = QUAD4
[]

[UserObjects]
  [./line_seg_cut_uo]
    type = LineSegmentCutUserObject
    cut_data = '0.0 0.5 0.5 0.5'
  [../]
[]

[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
[]

[Physics/SolidMechanics/QuasiStatic]
  [./all]
    strain = SMALL
    generate_output = 'stress_xx stress_yy'
  [../]
[]

[Functions]
  [./pressure]
    type = PiecewiseLinear
    x = '0 1.0 2.0'
    y = '0 500 1000'
  [../]
[]

[BCs]
  [./bottom_y]
    type = DirichletBC
    boundary = 0
    variable = disp_y
    value = 0.0
  [../]
  [./top_y]
    type = DirichletBC
    boundary = 2
    variable = disp_y
    value = 0.0
  [../]
  [./bottom_x]
    type = DirichletBC
    boundary = 0
    variable = disp_x
    value = 0.0
  [../]
[]

[DiracKernels]
  [./pressure_x]
    type = XFEMPressure
    variable = disp_x
    component = 0
    function = pressure
  [../]

  [./pressure_y]
    type = XFEMPressure
    variable = disp_y
    component = 1
    function = pressure
  [../]
[]


[Materials]
  [./elasticity_tensor]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 1e6
    poissons_ratio = 0.3
  [../]
  [./stress]
    type = ComputeLinearElasticStress
  [../]
[]

[Executioner]
  type = Transient

  solve_type = 'PJFNK'
  petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  petsc_options_value = '201                hypre    boomeramg      8'

  line_search = 'none'

  [./Predictor]
    type = SimplePredictor
    scale = 1.0
  [../]

# controls for linear iterations
  l_max_its = 100
  l_tol = 1e-2

# controls for nonlinear iterations
  nl_max_its = 15
  nl_rel_tol = 1e-10
  nl_abs_tol = 1e-12

# time control
  start_time = 0.0
  dt = 1
  end_time = 2
[]

[Outputs]
  file_base = edge_2d_pressure_out
  exodus = true
  [./console]
    type = Console
    output_linear = true
  [../]
[]
