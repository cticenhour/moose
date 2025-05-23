//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "TriSubChannelMesh.h"

#include "gtest/gtest.h"
#include <algorithm>
#include <sstream>

const double tol = 1e-8;
const double eps = 1e-9;

void
triRodPositionsRef(std::vector<Point> & positions, Real nrings, Real pitch, Point center)
{
  positions.clear();

  const Real start_r = (nrings - 1) * pitch;
  const Real theta0 = 2 * libMesh::pi / 3;
  const Real startx = start_r * std::cos(theta0);
  const Real starty = start_r * std::sin(theta0);
  for (int i = 0; i < nrings; i++)
  {
    int n_rods_in_row = nrings + i;
    const Real y = starty - i * pitch * std::sin(theta0);
    const Real x_row = startx + i * pitch * std::cos(theta0);
    for (int j = 0; j < n_rods_in_row; j++)
    {
      const Real x = x_row + j * pitch;
      positions.emplace_back(center(0) + x, center(1) + y);
      if (i < nrings - 1)
        positions.emplace_back(center(0) + x, center(1) - y);
    }
  }
}

bool
relativeEq(double x, double y)
{
  if (std::abs(x) < eps && std::abs(y) < eps)
    return true;

  double diff = std::abs(x - y);
  x = std::abs(x);
  y = std::abs(y);
  double largest = (y > x) ? y : x;
  return diff <= largest * tol;
}

bool
pointLess(const Point & a, const Point & b)
{
  for (int i = 0; i < LIBMESH_DIM; i++)
  {
    if (relativeEq(a(i), b(i)))
      continue;
    return a(i) < b(i);
  }
  return false;
}

TEST(MeshTests, triRodCoordinates)
{

  int nrings = 4;
  Real pitch = 1;
  Point center(0, 0, 0);

  std::vector<Point> positions;
  triRodPositionsRef(positions, nrings, pitch, center);
  std::sort(positions.begin(), positions.end(), pointLess);

  std::vector<Point> positions2;
  TriSubChannelMesh::rodPositions(positions2, nrings, pitch, center);
  std::sort(positions2.begin(), positions2.end(), pointLess);

  ASSERT_EQ(positions.size(), positions2.size());

  std::stringstream msg;
  for (size_t i = 0; i < positions.size(); i++)
    if (!positions[i].absolute_fuzzy_equals(positions2[i]))
      msg << "point " << i + 1 << " differs: " << positions[i] << " != " << positions2[i] << "\n";

  if (msg.str().size() > 0)
    FAIL() << msg.str();
}
