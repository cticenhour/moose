//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Moose.h"

// forward declares
class MooseMesh;

namespace libMesh
{
class Point;
class Plane;
class MeshBase;
class Elem;
}

namespace Moose
{
/**
 * Find all of the elements intersected by a plane.
 * The plane is given as a point and a normal vector.
 * @param p0 Point in plane.
 * @param normal Normal vector to plane.
 * @param intersected_elems The elements intersected by the plane.  Will be empty if there are no
 * intersections.
 */
void elementsIntersectedByPlane(const libMesh::Point & p0,
                                const libMesh::Point & normal,
                                const libMesh::MeshBase & mesh,
                                std::vector<const libMesh::Elem *> & intersected_elems);

/**
 * Find all of the elements intersected by a plane.
 * The plane is given as three points in the plane.
 * @param p0 Point in plane.
 * @param p1 Point in plane.
 * @param p2 Point in plane.
 * @param intersected_elems The elements intersected by the plane.  Will be empty if there are no
 * intersections.
 */
void elementsIntersectedByPlane(const libMesh::Point & p0,
                                const libMesh::Point & p1,
                                const libMesh::Point & p2,
                                const libMesh::MeshBase & mesh,
                                std::vector<const libMesh::Elem *> & intersected_elems);
}
