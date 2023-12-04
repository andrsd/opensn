#include "framework/math/sparse_matrix/math_sparse_matrix.h"

#include "framework/runtime.h"
#include "framework/logging/log.h"

#include <iomanip>
#include <algorithm>

namespace opensn
{

SparseMatrix::SparseMatrix(size_t num_rows, size_t num_cols)
  : row_size_(num_rows), col_size_(num_cols)
{
  rowI_values_.resize(num_rows, std::vector<double>());
  rowI_indices_.resize(num_rows, std::vector<size_t>());
}

SparseMatrix::SparseMatrix(const SparseMatrix& in_matrix)
  : row_size_(in_matrix.NumRows()), col_size_(in_matrix.NumCols())
{
  rowI_values_.resize(row_size_, std::vector<double>());
  rowI_indices_.resize(row_size_, std::vector<size_t>());

  for (size_t i = 0; i < in_matrix.rowI_values_.size(); i++)
  {
    rowI_values_[i] = (in_matrix.rowI_values_[i]);
    rowI_indices_[i] = (in_matrix.rowI_indices_[i]);
  }
}

void
SparseMatrix::Insert(size_t i, size_t j, double value)
{
  CheckInitialized();

  if ((i < 0) || (i >= row_size_) || (j < 0) || (j >= col_size_))
  {
    log.LogAllError() << "SparseMatrix::Insert encountered out of bounds,"
                      << " i=" << i << " j=" << j << " bounds(" << row_size_ << "," << col_size_
                      << ")";
    Exit(EXIT_FAILURE);
  }

  auto relative_location = std::find(rowI_indices_[i].begin(), rowI_indices_[i].end(), j);
  bool already_there = (relative_location != rowI_indices_[i].end());

  if (already_there)
  {
    size_t jr = relative_location - rowI_indices_[i].begin();
    rowI_values_[i][jr] = value;
  }
  else
  {
    rowI_values_[i].push_back(value);
    rowI_indices_[i].push_back(j);
  }
}

void
SparseMatrix::InsertAdd(size_t i, size_t j, double value)
{
  CheckInitialized();

  if ((i < 0) || (i >= row_size_) || (j < 0) || (j >= col_size_))
  {
    log.LogAllError() << "SparseMatrix::Insert encountered out of bounds,"
                      << " i=" << i << " j=" << j << " bounds(" << row_size_ << "," << col_size_
                      << ")";
    Exit(EXIT_FAILURE);
  }

  auto relative_location = std::find(rowI_indices_[i].begin(), rowI_indices_[i].end(), j);
  bool already_there = (relative_location != rowI_indices_[i].end());

  if (already_there)
  {
    size_t jr = relative_location - rowI_indices_[i].begin();
    rowI_values_[i][jr] += value;
  }
  else
  {
    rowI_values_[i].push_back(value);
    rowI_indices_[i].push_back(j);
  }
}

void
SparseMatrix::SetDiagonal(const std::vector<double>& diag)
{
  CheckInitialized();

  size_t num_rows = rowI_values_.size();
  // Check size
  if (diag.size() != rowI_values_.size())
  {
    log.LogAllError() << "Incompatible matrix-vector size encountered "
                      << "in call to SparseMatrix::SetDiagonal.";
    Exit(EXIT_FAILURE);
  }

  // Assign values
  for (size_t i = 0; i < num_rows; i++)
  {
    auto relative_location = std::find(rowI_indices_[i].begin(), rowI_indices_[i].end(), i);
    bool already_there = (relative_location != rowI_indices_[i].end());

    if (already_there)
    {
      size_t jr = relative_location - rowI_indices_[i].begin();
      rowI_values_[i][jr] = diag[i];
    }
    else
    {
      rowI_values_[i].push_back(diag[i]);
      rowI_indices_[i].push_back(i);
    }
  } // for i
}

double
SparseMatrix::ValueIJ(size_t i, size_t j) const
{
  double retval = 0.0;
  if ((i < 0) || (i >= rowI_indices_.size()))
  {
    log.LogAllError() << "Index i out of bounds"
                      << " in call to SparseMatrix::ValueIJ"
                      << " i=" << i;
    Exit(EXIT_FAILURE);
  }

  if (not rowI_indices_[i].empty())
  {
    auto relative_location = std::find(rowI_indices_[i].begin(), rowI_indices_[i].end(), j);
    bool non_zero = (relative_location != rowI_indices_[i].end());
    if (non_zero)
    {
      size_t jr = relative_location - rowI_indices_[i].begin();
      retval = rowI_values_[i][jr];
    }
  }
  return retval;
}

void
SparseMatrix::Compress()
{
  for (size_t i = 0; i < rowI_indices_.size(); ++i)
  {
    auto& indices = rowI_indices_[i];
    auto& values = rowI_values_[i];

    // Copy row indexes and values into vector of pairs
    std::vector<std::pair<size_t, double>> target;
    target.reserve(indices.size());

    auto index = indices.begin();
    auto value = values.begin();
    for (; index != indices.end(); ++index, ++value)
    {
      target.emplace_back(*index, *value);
    }

    // Define compare operator
    struct
    {
      bool operator()(std::pair<size_t, double> a, std::pair<size_t, double> b)
      {
        return a.first < b.first;
      }
    } compare_index;

    // Sort
    std::stable_sort(target.begin(), target.end(), compare_index);

    // Copy back
    indices.clear();
    values.clear();
    for (auto& iv_pair : target)
    {
      indices.push_back(iv_pair.first);
      values.push_back(iv_pair.second);
    }
  }
}

std::string
SparseMatrix::PrintStr() const
{
  std::stringstream out;

  for (size_t i = 0; i < row_size_; i++)
  {
    for (size_t j = 0; j < col_size_; j++)
    {
      auto relative_location = std::find(rowI_indices_[i].begin(), rowI_indices_[i].end(), j);
      bool non_zero = (relative_location != rowI_indices_[i].end());

      if (non_zero)
      {
        size_t jr = relative_location - rowI_indices_[i].begin();
        out << std::setprecision(2) << std::scientific << std::setw(9) << rowI_values_[i][jr]
            << " ";
      }
      else { out << std::setprecision(0) << std::fixed << std::setw(9) << 0.0 << " "; }
    }
    out << "\n";
  }

  return out.str();
}

void
SparseMatrix::CheckInitialized() const
{
  if (rowI_values_.empty())
  {
    log.LogAllError() << "Illegal call to unitialized SparseMatrix matrix.";
    Exit(EXIT_FAILURE);
  }
}

//  Iterator routines

SparseMatrix::RowIteratorContext
SparseMatrix::Row(size_t row_id)
{
  return {*this, row_id};
}

SparseMatrix::ConstRowIteratorContext
SparseMatrix::Row(size_t row_id) const
{
  return {*this, row_id};
}

SparseMatrix::EntriesIterator
SparseMatrix::begin()
{
  // Find first non-empty row
  size_t nerow = row_size_; // nerow = non-empty row
  for (size_t r = 0; r < row_size_; ++r)
    if (not rowI_indices_[r].empty())
    {
      nerow = r;
      break;
    }

  return EntriesIterator(*this, nerow);
}

SparseMatrix::EntriesIterator
SparseMatrix::end()
{
  return EntriesIterator(*this, row_size_);
}
} // namespace opensn
