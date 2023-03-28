mu = 3.475e-5
rho = 4.8535
k = 0.251
cp = 5195
rho_steel = 7860
k_steel = 15.5
cp_steel = 420
D = 0.012
Re = 40000
advected_interp_method = 'average'
velocity_interp_method = 'average'

bulk_v = '${fparse Re * mu / (D * rho)}'

sigma_k = 1.0
sigma_eps = 1.3
C1_eps = 1.44
C2_eps = 1.92
C_mu = 0.09

chan_l = 3
l_0 = .00084
I = 0.042
k_0 = '${fparse 1.5 *( I * bulk_v) ^ 2}'
eps_0 = '${fparse C_mu^0.75 * k_0 ^ 1.5 / l_0}'

[GlobalParams]
  rhie_chow_user_object = 'rc'
[]

[UserObjects]
  [rc]
    type = INSFVRhieChowInterpolator
    u = u
    v = v
    pressure = pressure
    block = '0'
  []
[]

[Mesh]
  [mesh]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = '${fparse 0.5 * D+.001}'
    ymin = 0
    ymax = ${chan_l} 
    nx = 18 
    ny = 300
    bias_x = '${fparse 1/1.2}'
  []
  [fluid]
    type = SubdomainBoundingBoxGenerator
    input = mesh
    block_id = 0
    location = INSIDE
    bottom_left = '0 -0.1 0'
    top_right = '0.006 ${fparse chan_l + 0.001} 0'
  []
  [solid]
    type = SubdomainBoundingBoxGenerator
    input = fluid 
    block_id = 1
    location = INSIDE
    bottom_left = '.006 -0.1 0'
    top_right = '0.007 ${fparse chan_l + 0.001} 0'
  []
  [solid_sidesets]
    type = SideSetsAroundSubdomainGenerator
    input = solid
    block = 1
    new_boundary = 'hf'
    normal = '1 0 0'
  []
  [solid_sidesets1]
    type = SideSetsAroundSubdomainGenerator
    input = solid_sidesets
    block = 1
    new_boundary = 'solid_top'
    normal = '0 1 0'
  []
  [solid_sidesets2]
    type = SideSetsAroundSubdomainGenerator
    input = solid_sidesets1
    block = 1
    new_boundary = 'solid_bottom'
    normal = '0 -1 0'
  []
  [solid_sidesets3]
    type = SideSetsAroundSubdomainGenerator
    input = solid_sidesets2
    block = 1
    new_boundary = 'solid_side_int'
    normal = '-1 0 0'
  [] 
  [fluid_sidesets]
    type = SideSetsAroundSubdomainGenerator
    input = solid_sidesets3
    block = 0
    new_boundary = 'fluid_side_int'
    normal = '1 0 0'
  []
  [fluid_sidesets1]
    type = SideSetsAroundSubdomainGenerator
    input = fluid_sidesets
    block = 0
    new_boundary = 'outlet'
    normal = '0 1 0'
  []
  [fluid_sidesets2]
    type = SideSetsAroundSubdomainGenerator
    input = fluid_sidesets1
    block = 0
    new_boundary = 'inlet'
    normal = '0 -1 0'
  []
  [fluid_sidesets3]
    type = SideSetsAroundSubdomainGenerator
    input = fluid_sidesets2
    block = 0
    new_boundary = 'sym'
    normal = '-1 0 0'
  []
[]

[Materials]
  [helium_const]
    type = ADGenericFunctorMaterial
    prop_names = 'cp rho k_eff'
    prop_values = '${cp} ${rho} k'
    block = '0'
  []
  [k_turb]
    type = TurbulentFunctorMaterial 
    mu_t = 0.001
    mu = ${mu}
    k = ${k}
    cp = ${cp}
    Pr_t = '0.85'
    block = '0'
  []
  [steel_functor_constants]
    type = ADGenericFunctorMaterial
    prop_names = 'cp k rho'
    prop_values = '${cp_steel} ${k_steel} ${rho_steel}'
    block = '1'
  [] 
  [ins_fv]
    type = INSFVEnthalpyMaterial
    rho = ${rho}
    temperature = 'T'
    block = '0'
  []
[]

[Problem]
  coord_type = 'RZ'
[]

[Variables]
  [u]
    type = INSFVVelocityVariable
    initial_condition = 1e-6
    block = '0'
  []
  [v]
    type = INSFVVelocityVariable
    initial_condition = ${bulk_v}
    block = '0'
  []
  [pressure]
    type = INSFVPressureVariable
    block = '0'
  []
  [TKE]
    type = INSFVEnergyVariable
    block = '0'
  []
  [TKED]
    type = INSFVEnergyVariable
    block = '0'
  []
  [T]
    type = INSFVEnergyVariable
    initial_condition = 623
    block = '0 1'
  []
[]

[AuxVariables]
  [mu_t]
    order = CONSTANT
    family = MONOMIAL
    fv = true
    initial_condition = 0.001
    block = '0'
  []

  [y_plus]
    order = CONSTANT
    family = MONOMIAL
    fv = true
    initial_condition = 1.0
    block = '0'
  []
[]  

[FVKernels]
  [mass]
    type = INSFVMassAdvection
    variable = pressure
    advected_interp_method = ${advected_interp_method}
    velocity_interp_method = ${velocity_interp_method}
    rho = ${rho}
    block = '0'
  []

  [u_advection]
    type = INSFVMomentumAdvection
    variable = u
    advected_interp_method = ${advected_interp_method}
    velocity_interp_method = ${velocity_interp_method}
    rho = ${rho}
    momentum_component = 'x'
    block = '0'
  []
  [u_viscosity]
    type = INSFVMomentumDiffusion
    variable = u
    mu = ${mu}
    momentum_component = 'x'
    block = '0'
  []
  [u_pressure]
    type = INSFVMomentumPressure
    variable = u
    momentum_component = 'x'
    pressure = pressure
    block = '0'
  []
  [u_viscosity_rans]
    type = INSFVMomentumDiffusion
    variable = u
    momentum_component = 'x'
    mu = 'mu_t'
    block = '0'
  []

  [v_advection]
    type = INSFVMomentumAdvection
    variable = v
    advected_interp_method = ${advected_interp_method}
    velocity_interp_method = ${velocity_interp_method}
    rho = ${rho}
    momentum_component = 'y'
    block = '0'
  []
  [v_viscosity]
    type = INSFVMomentumDiffusion
    variable = v
    mu = ${mu}
    momentum_component = 'y'
    block = '0'
  []
  [v_pressure]
    type = INSFVMomentumPressure
    variable = v
    momentum_component = 'y'
    pressure = pressure
    block = '0'
  []
  [v_viscosity_rans]
    type = INSFVMomentumDiffusion
    variable = v
    momentum_component = 'y'
    mu = 'mu_t'
    block = '0'
  []

  [TKE_advection]
    type = PINSFVTurbulentAdvection
    variable = TKE
    rho = ${rho}
    velocity_interp_method = ${velocity_interp_method}
    advected_interp_method = ${advected_interp_method}
    block = '0'
  []
  [TKE_diffusion]
    type = PINSFVTurbulentDiffusion
    variable = TKE
    mu_t = ${mu} 
    porosity = 1
    turb_coef = ${sigma_k}
    block = '0'
  []
  [TKE_SourceSink]
    type = PINSFVTKESourceSink
    variable = TKE
    u = u
    v = v
    rho = ${rho}
    mu_t = ${mu}
    effective_balance = false
    epsilon = TKED
    porosity = 1
    block = '0'
  []
  
  [TKED_advection]
    type = PINSFVTurbulentAdvection
    variable = TKED
    rho = ${rho}
    velocity_interp_method = ${velocity_interp_method}
    advected_interp_method = ${advected_interp_method}
    block = '0'
  []
  [TKED_diffusion]
    type = PINSFVTurbulentDiffusion
    variable = TKED
    mu_t = ${mu}
    porosity = 1
    turb_coef = ${sigma_eps}
    block = '0'
  []
  [TKED_SourceSink]
    type = PINSFVTKEDSourceSink
    variable = TKED
    u = u
    v = v
    rho = ${rho}
    mu_t = ${mu}
    effective_balance = false
    C1_eps = ${C1_eps}
    C2_eps = ${C2_eps}
    k = TKE
    porosity = 1
    block = '0'
  []

  [temp_conduction]
    type = FVDiffusion
    coeff = 'k'
    variable = T
    block = '0 1'
  []
  [temp_advection]
    type = INSFVEnergyAdvection
    variable = T
    velocity_interp_method = ${velocity_interp_method}
    advected_interp_method = ${advected_interp_method}
    block = '0'
  []
[]

[AuxKernels]
  [compute_mu_t]
    type = kEpsilonViscosity
    variable = mu_t
    k = TKE
    epsilon = TKED
    rho = ${rho}
    C_mu = ${C_mu}
    mu = ${mu}
    u = u
    v = v
    walls = 'fluid_side_int'
    linearized_yplus = false
    n_iters_activate = 2 
    execute_on = 'INITIAL'
    block = '0'
  []
  [compute_y_plus]
    type = WallFunctionYPlusAux
    variable = y_plus
    rho = ${rho}
    mu = ${mu}
    u = u
    v = v
    walls = 'fluid_side_int'
    block = '0'
  []
[]

[FVBCs]
  [inlet-u]
    type = INSFVInletVelocityBC
    boundary = 'inlet'
    variable = u
    function = 0
  []
  [inlet-v]
    type = INSFVInletVelocityBC
    boundary = 'inlet'
    variable = v
    function = ${bulk_v}
  []
  [inlet-T-dirichlet]
    type = FVDirichletBC
    boundary = 'inlet'
    variable = T
    value = '623'
  []
  [inlet-TKE]
    type = NSFVTKEInletBC
    boundary = 'inlet'
    variable = TKE
    rho = ${rho}
    u = u
    v = v
    turbulent_intensity = ${I}
  []
  [inlet-TKED]
    type = NSFVTKEInletBC
    boundary = 'inlet'
    variable = TKED
    rho = ${rho}
    u = u
    v = v
    turbulent_intensity = ${I}
  []

  [no-slip-wall-u]
    type = INSFVNoSlipWallBC
    boundary = 'fluid_side_int'
    variable = u
    function = 0
  []
  [no-slip-wall-v]
    type = INSFVNoSlipWallBC
    boundary = 'fluid_side_int'
    variable = v
    function = 0
  []
  [wall-TKE]
    type = NSFVTKEWallFunctionBC
    boundary = 'fluid_side_int'
    variable = TKE
  []
  [wall-TKED]
    type = FVDirichletBC
    boundary = 'fluid_side_int'
    variable = TKED
    value = ${eps_0}
  []
  [outlet-p]
    type = INSFVOutletPressureBC
    boundary = 'outlet'
    variable = pressure
    function = 8e6
  []
  [axis-u]
    type = INSFVSymmetryVelocityBC
    boundary = 'sym'
    variable = u
    u = u
    v = v
    mu = ${mu}
    momentum_component = x
  []
  [axis-v]
    type = INSFVSymmetryVelocityBC
    boundary = 'sym'
    variable = v
    u = u
    v = v
    mu = ${mu}
    momentum_component = y
  []
  [axis-p]
    type = INSFVSymmetryPressureBC
    boundary = 'sym'
    variable = pressure
  []
  [T_wall_spec]
    type = FVDirichletBC
    boundary = 'hf'
    variable = T
    value = 823
  []
[]

[Postprocessors]
  [T_in]
    type = SideAverageValue
    variable = T
    boundary = 'inlet'
  []
  [vel_ave]
    type = ElementAverageValue 
    variable = v
    block = '0'
  []
  [T_out]
    type = SideAverageValue
    variable = T
    boundary = 'outlet'
  []
  [T_hot]
    type = SideAverageValue
    variable = T
    boundary = 'hf'
  []
  [Re]
    type = ParsedPostprocessor
    function = 'vel_ave * rho * D / mu'
    pp_names = 'vel_ave'
    constant_names = 'rho D mu'
    constant_expressions = '${rho} 0.012  ${mu}'
  []
  [mu_t]
    type = ElementAverageValue
    variable = mu_t
    block = '0'
  []
  [y_plus]
    type = ElementAverageValue
    variable = y_plus
    block = '0'
  []
[]

[Executioner]
  type = Steady
  solve_type = 'NEWTON'
  petsc_options_iname = '-pc_type -pc_factor_shift_type'
  petsc_options_value = 'lu NONZERO'
  nl_rel_tol = 1e-5
[]
[Debug]
  show_var_residual_norms = true
[]
[Outputs]
  exodus = true
  csv = true
[]


