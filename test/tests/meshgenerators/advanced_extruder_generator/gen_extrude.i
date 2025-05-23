[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 6
    ny = 6
    nz = 0
    zmin = 0
    zmax = 0
    elem_type = QUAD4
  []

  [extrude]
    type = AdvancedExtruderGenerator
    input = gmg
    heights = '1 2 3'
    num_layers = '1 2 3'
    direction = '0 0 1'
    bottom_boundary = '4'
    top_boundary = '5'
    subdomain_swaps = '0 1;
                       0 2;
                       0 3'
  []
[]
