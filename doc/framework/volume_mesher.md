# Volume Meshers

## General Concepts

A volume mesher generates cells (`Cell`) which can be
either 2D polygons (`CellPolygon`) or 3D polyhedrons
(`CellPolyhedron`). All cell objects are pushed into
a vector located in a `MeshContinuum` after creation.
Right now only a single continuum can be operated on and this is normally
by means of attaching a region to a solver.

Right now 2D meshes can be partitioned but the full 2D cell geometry
exists on each processor. This is mostly because of the way the extruder
works. This means that for a 2D problem a continuum will have ALL of the
cells (as fully defined cells), in every process, in the member
vector "cells" of the continuum.
The same can not be said for 3D extruded meshes where each process has all
the nodes but if cells are not local then only placeholders are uploaded.
Placeholders are basically the base class `Cell` and contains only
the cell's `partition_id` and its `centroid`. The concept that these placeholders
are uploaded allows a sweeping order to figure out dependencies. A similar
strategy would have to be devised for using third-party meshes.

Global mesh references are maintained in `MeshContinuum::cells`.
This contains the actual mesh object. Local indices are stored in
`MeshContinuum::local_cell_glob_indices` and are the global indices
of local cells. Conversely the local indices, given a global index,
are stored in `MeshContinuum::glob_cell_local_indices`.

## Extruder Mesher
