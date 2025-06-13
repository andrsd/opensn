#!/usr/bin/env python3

"""
1D PWLD transport test with vacuum and incident-isotropic boundary conditions
Test: Max-value=0.49903 and 7.18243e-4
"""

import os
import sys
import math

if "opensn_console" not in globals():
    from mpi4py import MPI
    size = MPI.COMM_WORLD.size
    rank = MPI.COMM_WORLD.rank
    sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../../../../../")))
    from pyopensn.mesh import OrthogonalMeshGenerator
    from pyopensn.xs import MultiGroupXS
    from pyopensn.source import VolumetricSource
    from pyopensn.aquad import GLProductQuadrature1DSlab
    from pyopensn.solver import DiscreteOrdinatesProblem, SteadyStateSolver
    from pyopensn.fieldfunc import FieldFunctionInterpolationLine, FieldFunctionInterpolationVolume
    from pyopensn.math import Vector3
    from pyopensn.logvol import RPPLogicalVolume

if __name__ == "__main__":

    # Setup mesh
    nodes = []
    N = 10
    L = 2.0
    xmin = 0.0
    dx = L / N
    for i in range(N + 1):
        nodes.append(xmin + i * dx)
    meshgen = OrthogonalMeshGenerator(node_sets=[nodes])
    grid = meshgen.Execute()

    # Cross-section data
    num_groups = 1
    grid.SetUniformBlockID(0)
    xs_3_170 = MultiGroupXS()
    xs_3_170.CreateSimpleOneGroup(1., 0.9)

    # Boundary sources
    bsrc = []
    for g in range(num_groups):
        bsrc.append(0.0)
    bsrc[0] = 1.0 / 2.0

    # Angular quadrature
    pquad = GLProductQuadrature1DSlab(8)

    # Create solver
    phys = DiscreteOrdinatesProblem(
        mesh=grid,
        num_groups=num_groups,
        groupsets=[
            {
                "groups_from_to": (0, 0),
                "angular_quadrature": pquad,
                "angle_aggregation_num_subsets": 1,
                "inner_linear_method": "petsc_gmres",
                "l_abs_tol": 1.0e-6,
                "l_max_its": 300,
                "gmres_restart_interval": 100,
            },
        ],
        xs_map=[
            {
                "block_ids": [0],
                "xs": xs_3_170
            }
        ],
        options={
            "boundary_conditions": [
                {"name": "zmin", "type": "isotropic", "group_strength": bsrc},
            ],
            "scattering_order": 0,
            "max_ags_iterations": 1
        }
    )
    ss_solver = SteadyStateSolver(lbs_problem=phys)
    ss_solver.Initialize()
    ss_solver.Execute()
