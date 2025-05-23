[TriSubChannelMesh]
  [subchannel]
    type = SCMTriSubChannelMeshGenerator
    nrings = 3
    n_cells = 10
    flat_to_flat = 0.60
    heated_length = 1.0
    pin_diameter = 0.1
    pitch = 0.13
    dwire = 0.03
    hwire = 0.3
    spacer_k = '0.5'
    spacer_z = '0'
  []
[]

[Variables]
  [q_prime]
  []
[]

[ICs]
  [q_prime_IC]
    type = SCMTriPowerIC
    variable = q_prime
    power = 5.0e5 # W
    filename = "pin_power_profile.txt"
  []
[]

[Problem]
  solve = false
[]

[Executioner]
  type = Steady
[]

[Outputs]
  exodus = true
[]
