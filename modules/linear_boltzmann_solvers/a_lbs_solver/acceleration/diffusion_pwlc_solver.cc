#include "diffusion_pwlc_solver.h"
#include "modules/linear_boltzmann_solvers/a_lbs_solver/acceleration/acceleration.h"
#include "framework/mesh/mesh_continuum/mesh_continuum.h"
#include "framework/math/spatial_discretization/spatial_discretization.h"
#include "framework/math/spatial_discretization/finite_element/quadrature_point_data.h"
#include "modules/linear_boltzmann_solvers/a_lbs_solver/lbs_structs.h"
#include "framework/app.h"
#include "framework/logging/log.h"
#include "framework/utils/timer.h"

#define DefaultBCDirichlet                                                                         \
  BoundaryCondition                                                                                \
  {                                                                                                \
    BCType::DIRICHLET,                                                                             \
    {                                                                                              \
      0, 0, 0                                                                                      \
    }                                                                                              \
  }

namespace lbs::acceleration
{

DiffusionPWLCSolver::DiffusionPWLCSolver(opensn::App& app,
                                         std::string text_name,
                                         const chi_math::SpatialDiscretization& sdm,
                                         const chi_math::UnknownManager& uk_man,
                                         std::map<uint64_t, BoundaryCondition> bcs,
                                         MatID2XSMap map_mat_id_2_xs,
                                         const std::vector<UnitCellMatrices>& unit_cell_matrices,
                                         bool verbose)
  : DiffusionSolver(app,
                    std::move(text_name),
                    sdm,
                    uk_man,
                    std::move(bcs),
                    std::move(map_mat_id_2_xs),
                    unit_cell_matrices,
                    verbose,
                    true)
{
  using SDM_TYPE = chi_math::SpatialDiscretizationType;
  const auto& PWLC = SDM_TYPE ::PIECEWISE_LINEAR_CONTINUOUS;

  if (sdm_.Type() != PWLC)
    throw std::logic_error("lbs::acceleration::DiffusionPWLCSolver: can only be"
                           " used with PWLC.");
}

void
DiffusionPWLCSolver::AssembleAand_b(const std::vector<double>& q_vector)
{
  const size_t num_local_dofs = sdm_.GetNumLocalAndGhostDOFs(uk_man_);
  ChiInvalidArgumentIf(q_vector.size() != num_local_dofs,
                       std::string("q_vector size mismatch. ") + std::to_string(q_vector.size()) +
                         " vs " + std::to_string(num_local_dofs));

  const std::string fname = "lbs::acceleration::DiffusionMIPSolver::"
                            "AssembleAand_b";
  if (A_ == nullptr or rhs_ == nullptr or ksp_ == nullptr)
    throw std::logic_error(fname + ": Some or all PETSc elements are null. "
                                   "Check that Initialize has been called.");
  if (options.verbose)
    App().Log().Log() << App().ProgramTimer().GetTimeString() << " Starting assembly";

  const size_t num_groups = uk_man_.unknowns_.front().num_components_;

  VecSet(rhs_, 0.0);
  for (const auto& cell : grid_.local_cells)
  {
    const size_t num_faces = cell.faces_.size();
    const auto& cell_mapping = sdm_.GetCellMapping(cell);
    const size_t num_nodes = cell_mapping.NumNodes();
    const auto cc_nodes = cell_mapping.GetNodeLocations();
    const auto& unit_cell_matrices = unit_cell_matrices_[cell.local_id_];

    const auto& cell_K_matrix = unit_cell_matrices.K_matrix;
    const auto& cell_M_matrix = unit_cell_matrices.M_matrix;
    const auto& cell_Vi = unit_cell_matrices.Vi_vectors;

    const auto& xs = mat_id_2_xs_map_.at(cell.material_id_);

    // Mark dirichlet nodes
    typedef std::pair<bool, double> DirichFlagVal;
    std::vector<DirichFlagVal> node_is_dirichlet(num_nodes, {false, 0.0});
    for (size_t f = 0; f < num_faces; ++f)
    {
      const auto& face = cell.faces_[f];
      if (not face.has_neighbor_)
      {
        auto bc = DefaultBCDirichlet;
        if (bcs_.count(face.neighbor_id_) > 0) bc = bcs_.at(face.neighbor_id_);

        if (bc.type != BCType::DIRICHLET) continue;

        const size_t num_face_nodes = cell_mapping.NumFaceNodes(f);
        for (size_t fi = 0; fi < num_face_nodes; ++fi)
          node_is_dirichlet[cell_mapping.MapFaceNode(f, fi)] = {true, bc.values[0]};
      }
    }

    for (size_t g = 0; g < num_groups; ++g)
    {
      // Get coefficient and nodal src
      const double Dg = xs.Dg[g];
      const double sigr_g = xs.sigR[g];

      std::vector<double> qg(num_nodes, 0.0);
      for (size_t j = 0; j < num_nodes; j++)
        qg[j] = q_vector[sdm_.MapDOFLocal(cell, j, uk_man_, 0, g)];

      // Assemble continuous terms
      for (size_t i = 0; i < num_nodes; i++)
      {
        if (node_is_dirichlet[i].first) continue;
        const int64_t imap = sdm_.MapDOF(cell, i, uk_man_, 0, g);
        double entry_rhs_i = 0.0;
        for (size_t j = 0; j < num_nodes; j++)
        {
          const int64_t jmap = sdm_.MapDOF(cell, j, uk_man_, 0, g);

          const double entry_aij = Dg * cell_K_matrix[i][j] + sigr_g * cell_M_matrix[i][j];

          if (not node_is_dirichlet[j].first) MatSetValue(A_, imap, jmap, entry_aij, ADD_VALUES);
          else
          {
            const double bcvalue = node_is_dirichlet[j].second;
            VecSetValue(rhs_, imap, -entry_aij * bcvalue, ADD_VALUES);
          }

          entry_rhs_i += qg[j] * cell_M_matrix[i][j];
        } // for j

        VecSetValue(rhs_, imap, entry_rhs_i, ADD_VALUES);
      } // for i

      // Assemble face terms
      for (size_t f = 0; f < num_faces; ++f)
      {
        const auto& face = cell.faces_[f];
        const size_t num_face_nodes = cell_mapping.NumFaceNodes(f);

        const auto& face_M = unit_cell_matrices.face_M_matrices[f];
        const auto& face_Si = unit_cell_matrices.face_Si_vectors[f];

        if (not face.has_neighbor_)
        {
          auto bc = DefaultBCDirichlet;
          if (bcs_.count(face.neighbor_id_) > 0) bc = bcs_.at(face.neighbor_id_);

          if (bc.type == BCType::DIRICHLET)
          {
            const double bc_value = bc.values[0];

            for (size_t fi = 0; fi < num_face_nodes; ++fi)
            {
              const int i = cell_mapping.MapFaceNode(f, fi);
              const int64_t imap = sdm_.MapDOF(cell, i, uk_man_, 0, g);

              // MatSetValue(A_, imap, imap, cell_Vi[i], ADD_VALUES);
              // VecSetValue(rhs_, imap, bc_value * cell_Vi[i], ADD_VALUES);
              MatSetValue(A_, imap, imap, 1.0, ADD_VALUES);
              VecSetValue(rhs_, imap, bc_value, ADD_VALUES);
            } // for fi

          } // Dirichlet BC
          else if (bc.type == BCType::ROBIN)
          {
            const double aval = bc.values[0];
            const double bval = bc.values[1];
            const double fval = bc.values[2];

            if (std::fabs(bval) < 1.0e-12) continue; // a and f assumed zero

            for (size_t fi = 0; fi < num_face_nodes; fi++)
            {
              const int i = cell_mapping.MapFaceNode(f, fi);
              const int64_t ir = sdm_.MapDOF(cell, i, uk_man_, 0, g);

              if (std::fabs(aval) >= 1.0e-12)
              {
                for (size_t fj = 0; fj < num_face_nodes; fj++)
                {
                  const int j = cell_mapping.MapFaceNode(f, fj);
                  const int64_t jr = sdm_.MapDOF(cell, j, uk_man_, 0, g);

                  const double aij = (aval / bval) * face_M[i][j];

                  MatSetValue(A_, ir, jr, aij, ADD_VALUES);
                } // for fj
              }   // if a nonzero

              if (std::fabs(fval) >= 1.0e-12)
              {
                const double rhs_val = (fval / bval) * face_Si[i];

                VecSetValue(rhs_, ir, rhs_val, ADD_VALUES);
              } // if f nonzero
            }   // for fi
          }     // Robin BC
        }       // boundary face
      }         // for face
    }           // for g
  }             // for cell

  MatAssemblyBegin(A_, MAT_FINAL_ASSEMBLY);
  MatAssemblyEnd(A_, MAT_FINAL_ASSEMBLY);
  VecAssemblyBegin(rhs_);
  VecAssemblyEnd(rhs_);

  if (options.verbose)
  {
    MatInfo info;
    MatGetInfo(A_, MAT_GLOBAL_SUM, &info);

    App().Log().Log() << "Number of mallocs used = " << info.mallocs
                      << "\nNumber of non-zeros allocated = " << info.nz_allocated
                      << "\nNumber of non-zeros used = " << info.nz_used
                      << "\nNumber of unneeded non-zeros = " << info.nz_unneeded;
  }

  if (options.perform_symmetry_check)
  {
    PetscBool symmetry = PETSC_FALSE;
    MatIsSymmetric(A_, 1.0e-6, &symmetry);
    if (symmetry == PETSC_FALSE) throw std::logic_error(fname + ":Symmetry check failed");
  }

  KSPSetOperators(ksp_, A_, A_);

  if (options.verbose)
    App().Log().Log() << App().ProgramTimer().GetTimeString() << " Assembly completed";

  PC pc;
  KSPGetPC(ksp_, &pc);
  PCSetUp(pc);

  KSPSetUp(ksp_);
}

void
DiffusionPWLCSolver::Assemble_b(const std::vector<double>& q_vector)
{
  const size_t num_local_dofs = sdm_.GetNumLocalAndGhostDOFs(uk_man_);
  ChiInvalidArgumentIf(q_vector.size() != num_local_dofs,
                       std::string("q_vector size mismatch. ") + std::to_string(q_vector.size()) +
                         " vs " + std::to_string(num_local_dofs));
  const std::string fname = "lbs::acceleration::DiffusionMIPSolver::"
                            "Assemble_b";
  if (A_ == nullptr or rhs_ == nullptr or ksp_ == nullptr)
    throw std::logic_error(fname + ": Some or all PETSc elements are null. "
                                   "Check that Initialize has been called.");
  if (options.verbose)
    App().Log().Log() << App().ProgramTimer().GetTimeString() << " Starting assembly";

  const size_t num_groups = uk_man_.unknowns_.front().num_components_;

  VecSet(rhs_, 0.0);
  for (const auto& cell : grid_.local_cells)
  {
    const size_t num_faces = cell.faces_.size();
    const auto& cell_mapping = sdm_.GetCellMapping(cell);
    const size_t num_nodes = cell_mapping.NumNodes();
    const auto cc_nodes = cell_mapping.GetNodeLocations();
    const auto& unit_cell_matrices = unit_cell_matrices_[cell.local_id_];

    const auto& cell_K_matrix = unit_cell_matrices.K_matrix;
    const auto& cell_M_matrix = unit_cell_matrices.M_matrix;
    const auto& cell_Vi = unit_cell_matrices.Vi_vectors;

    const auto& xs = mat_id_2_xs_map_.at(cell.material_id_);

    // Mark dirichlet nodes
    typedef std::pair<bool, double> DirichFlagVal;
    std::vector<DirichFlagVal> node_is_dirichlet(num_nodes, {false, 0.0});
    for (size_t f = 0; f < num_faces; ++f)
    {
      const auto& face = cell.faces_[f];
      if (not face.has_neighbor_)
      {
        auto bc = DefaultBCDirichlet;
        if (bcs_.count(face.neighbor_id_) > 0) bc = bcs_.at(face.neighbor_id_);

        if (bc.type != BCType::DIRICHLET) continue;

        const size_t num_face_nodes = cell_mapping.NumFaceNodes(f);
        for (size_t fi = 0; fi < num_face_nodes; ++fi)
          node_is_dirichlet[cell_mapping.MapFaceNode(f, fi)] = {true, bc.values[0]};
      }
    }

    for (size_t g = 0; g < num_groups; ++g)
    {
      // Get coefficient and nodal src
      std::vector<double> qg(num_nodes, 0.0);
      for (size_t j = 0; j < num_nodes; j++)
        qg[j] = q_vector[sdm_.MapDOFLocal(cell, j, uk_man_, 0, g)];

      // Assemble continuous terms
      const double Dg = xs.Dg[g];
      const double sigr_g = xs.sigR[g];

      for (size_t i = 0; i < num_nodes; i++)
      {
        if (node_is_dirichlet[i].first) continue;
        const int64_t imap = sdm_.MapDOF(cell, i, uk_man_, 0, g);
        double entry_rhs_i = 0.0;
        for (size_t j = 0; j < num_nodes; j++)
        {
          if (node_is_dirichlet[j].first)
          {
            const double entry_aij = Dg * cell_K_matrix[i][j] + sigr_g * cell_M_matrix[i][j];

            const double bcvalue = node_is_dirichlet[j].second;
            VecSetValue(rhs_, imap, -entry_aij * bcvalue, ADD_VALUES);
          }

          entry_rhs_i += qg[j] * cell_M_matrix[i][j];
        } // for j

        VecSetValue(rhs_, imap, entry_rhs_i, ADD_VALUES);
      } // for i

      // Assemble face terms
      for (size_t f = 0; f < num_faces; ++f)
      {
        const auto& face = cell.faces_[f];
        const size_t num_face_nodes = cell_mapping.NumFaceNodes(f);

        const auto& face_Si = unit_cell_matrices.face_Si_vectors[f];

        if (not face.has_neighbor_)
        {
          auto bc = DefaultBCDirichlet;
          if (bcs_.count(face.neighbor_id_) > 0) bc = bcs_.at(face.neighbor_id_);

          if (bc.type == BCType::DIRICHLET)
          {
            const double bc_value = bc.values[0];

            // Assembly penalty terms
            for (size_t fi = 0; fi < num_face_nodes; ++fi)
            {
              const int i = cell_mapping.MapFaceNode(f, fi);
              const int64_t imap = sdm_.MapDOF(cell, i, uk_man_, 0, g);

              // VecSetValue(rhs_, imap, bc_value * cell_Vi[i], ADD_VALUES);
              VecSetValue(rhs_, imap, bc_value, ADD_VALUES);
            } // for fi

          } // Dirichlet BC
          else if (bc.type == BCType::ROBIN)
          {
            const double bval = bc.values[1];
            const double fval = bc.values[2];

            if (std::fabs(bval) < 1.0e-12) continue; // a and f assumed zero

            for (size_t fi = 0; fi < num_face_nodes; fi++)
            {
              const int i = cell_mapping.MapFaceNode(f, fi);
              const int64_t ir = sdm_.MapDOF(cell, i, uk_man_, 0, g);

              if (std::fabs(fval) >= 1.0e-12)
              {
                const double rhs_val = (fval / bval) * face_Si[i];

                VecSetValue(rhs_, ir, rhs_val, ADD_VALUES);
              } // if f nonzero
            }   // for fi
          }     // Robin BC
        }       // boundary face
      }         // for face
    }           // for g
  }             // for cell

  VecAssemblyBegin(rhs_);
  VecAssemblyEnd(rhs_);

  if (options.verbose)
    App().Log().Log() << App().ProgramTimer().GetTimeString() << " Assembly completed";
}

void
DiffusionPWLCSolver::Assemble_b(Vec petsc_q_vector)
{
  const std::string fname = "lbs::acceleration::DiffusionMIPSolver::"
                            "Assemble_b";
  if (A_ == nullptr or rhs_ == nullptr or ksp_ == nullptr)
    throw std::logic_error(fname + ": Some or all PETSc elements are null. "
                                   "Check that Initialize has been called.");
  if (options.verbose)
    App().Log().Log() << App().ProgramTimer().GetTimeString() << " Starting assembly";

  const size_t num_groups = uk_man_.unknowns_.front().num_components_;

  const double* q_vector;
  VecGetArrayRead(petsc_q_vector, &q_vector);

  VecSet(rhs_, 0.0);
  for (const auto& cell : grid_.local_cells)
  {
    const size_t num_faces = cell.faces_.size();
    const auto& cell_mapping = sdm_.GetCellMapping(cell);
    const size_t num_nodes = cell_mapping.NumNodes();
    const auto cc_nodes = cell_mapping.GetNodeLocations();
    const auto& unit_cell_matrices = unit_cell_matrices_[cell.local_id_];

    const auto& cell_M_matrix = unit_cell_matrices.M_matrix;
    const auto& cell_Vi = unit_cell_matrices.Vi_vectors;

    // Mark dirichlet nodes
    std::vector<bool> node_is_dirichlet(num_nodes, false);
    for (size_t f = 0; f < num_faces; ++f)
    {
      const auto& face = cell.faces_[f];
      if (not face.has_neighbor_)
      {
        auto bc = DefaultBCDirichlet;
        if (bcs_.count(face.neighbor_id_) > 0) bc = bcs_.at(face.neighbor_id_);

        if (bc.type != BCType::DIRICHLET) continue;

        const size_t num_face_nodes = cell_mapping.NumFaceNodes(f);
        for (size_t fi = 0; fi < num_face_nodes; ++fi)
          node_is_dirichlet[cell_mapping.MapFaceNode(f, fi)] = true;
      }
    }

    for (size_t g = 0; g < num_groups; ++g)
    {
      // Get coefficient and nodal src
      std::vector<double> qg(num_nodes, 0.0);
      for (size_t j = 0; j < num_nodes; j++)
        qg[j] = q_vector[sdm_.MapDOFLocal(cell, j, uk_man_, 0, g)];

      // Assemble continuous terms
      for (size_t i = 0; i < num_nodes; i++)
      {
        if (node_is_dirichlet[i]) continue;
        const int64_t imap = sdm_.MapDOF(cell, i, uk_man_, 0, g);
        double entry_rhs_i = 0.0;
        for (size_t j = 0; j < num_nodes; j++)
          entry_rhs_i += qg[j] * cell_M_matrix[i][j];

        VecSetValue(rhs_, imap, entry_rhs_i, ADD_VALUES);
      } // for i

      // Assemble face terms
      for (size_t f = 0; f < num_faces; ++f)
      {
        const auto& face = cell.faces_[f];
        const size_t num_face_nodes = cell_mapping.NumFaceNodes(f);

        const auto& face_Si = unit_cell_matrices.face_Si_vectors[f];

        if (not face.has_neighbor_)
        {
          auto bc = DefaultBCDirichlet;
          if (bcs_.count(face.neighbor_id_) > 0) bc = bcs_.at(face.neighbor_id_);

          if (bc.type == BCType::DIRICHLET)
          {
            const double bc_value = bc.values[0];

            // Assembly penalty terms
            for (size_t fi = 0; fi < num_face_nodes; ++fi)
            {
              const int i = cell_mapping.MapFaceNode(f, fi);
              const int64_t imap = sdm_.MapDOF(cell, i, uk_man_, 0, g);

              VecSetValue(rhs_, imap, bc_value * cell_Vi[i], ADD_VALUES);
            } // for fi

          } // Dirichlet BC
          else if (bc.type == BCType::ROBIN)
          {
            const double bval = bc.values[1];
            const double fval = bc.values[2];

            if (std::fabs(bval) < 1.0e-12) continue; // a and f assumed zero

            for (size_t fi = 0; fi < num_face_nodes; fi++)
            {
              const int i = cell_mapping.MapFaceNode(f, fi);
              const int64_t ir = sdm_.MapDOF(cell, i, uk_man_, 0, g);

              if (std::fabs(fval) >= 1.0e-12)
              {
                const double rhs_val = (fval / bval) * face_Si[i];

                VecSetValue(rhs_, ir, rhs_val, ADD_VALUES);
              } // if f nonzero
            }   // for fi
          }     // Robin BC
        }       // boundary face
      }         // for face
    }           // for g
  }             // for cell

  VecRestoreArrayRead(petsc_q_vector, &q_vector);

  VecAssemblyBegin(rhs_);
  VecAssemblyEnd(rhs_);

  if (options.verbose)
    App().Log().Log() << App().ProgramTimer().GetTimeString() << " Assembly completed";
}

} // namespace lbs::acceleration
