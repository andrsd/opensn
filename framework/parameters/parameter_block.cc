#include "framework/parameters/parameter_block.h"

#include <algorithm>
#include <sstream>

namespace chi
{
// #################################################################
std::string
ParameterBlockTypeName(ParameterBlockType type)
{
  switch (type)
  {
    case ParameterBlockType::BOOLEAN:
      return "BOOLEAN";
    case ParameterBlockType::FLOAT:
      return "FLOAT";
    case ParameterBlockType::STRING:
      return "STRING";
    case ParameterBlockType::INTEGER:
      return "INTEGER";
    case ParameterBlockType::ARRAY:
      return "ARRAY";
    case ParameterBlockType::BLOCK:
      return "BLOCK";
    default:
      throw std::logic_error(std::string(__PRETTY_FUNCTION__) + ": No name associated with type");
  }
}

// #################################################################
void
ParameterBlock::SetBlockName(const std::string& name)
{
  name_ = name;
}

// #################################################################
ParameterBlock::ParameterBlock(const std::string& name)
  : type_(ParameterBlockType::BLOCK), name_(name)
{
}

// #################################################################
/**Copy constructor*/
ParameterBlock::ParameterBlock(const ParameterBlock& other)
{
  type_ = other.type_;
  name_ = other.name_;

  using namespace chi_data_types;
  if (other.value_ptr_) value_ptr_ = std::make_unique<Varying>(*other.value_ptr_);

  parameters_ = other.parameters_;
  error_origin_scope_ = other.error_origin_scope_;
}

// #################################################################
/**Copy assignment operator*/
ParameterBlock&
ParameterBlock::operator=(const ParameterBlock& other)
{
  if (this != &other)
  {
    type_ = other.type_;
    name_ = other.name_;

    using namespace chi_data_types;
    if (other.value_ptr_) value_ptr_ = std::make_unique<Varying>(*other.value_ptr_);

    parameters_ = other.parameters_;
    error_origin_scope_ = other.error_origin_scope_;
  }

  return *this;
}

// #################################################################
/**Move constructor*/
ParameterBlock::ParameterBlock(ParameterBlock&& other) noexcept
{
  std::swap(type_, other.type_);
  std::swap(name_, other.name_);
  std::swap(value_ptr_, other.value_ptr_);
  std::swap(parameters_, other.parameters_);
  std::swap(error_origin_scope_, other.error_origin_scope_);
}

// #################################################################
/**Move assigment operator.*/
ParameterBlock&
ParameterBlock::operator=(ParameterBlock&& other) noexcept
{
  if (this != &other)
  {
    std::swap(type_, other.type_);
    std::swap(name_, other.name_);
    std::swap(value_ptr_, other.value_ptr_);
    std::swap(parameters_, other.parameters_);
    std::swap(error_origin_scope_, other.error_origin_scope_);
  }

  return *this;
}

// Accessors
ParameterBlockType
ParameterBlock::Type() const
{
  return type_;
}
/**Returns true if the parameter block comprises a single value of any of
 * the types BOOLEAN, FLOAT, STRING, INTEGER.*/
bool
ParameterBlock::IsScalar() const
{
  return (type_ >= ParameterBlockType::BOOLEAN and type_ <= ParameterBlockType::INTEGER);
}
std::string
ParameterBlock::TypeName() const
{
  return ParameterBlockTypeName(type_);
}
std::string
ParameterBlock::Name() const
{
  return name_;
}

// #################################################################
const chi_data_types::Varying&
ParameterBlock::Value() const
{
  switch (this->Type())
  {
    case ParameterBlockType::BOOLEAN:
    case ParameterBlockType::FLOAT:
    case ParameterBlockType::STRING:
    case ParameterBlockType::INTEGER:
    {
      if (value_ptr_ == nullptr)
        throw std::runtime_error(error_origin_scope_ + std::string(__PRETTY_FUNCTION__) +
                                 ": Uninitialized Varying value for block " + this->Name());
      return *value_ptr_;
    }
    default:
      throw std::logic_error(error_origin_scope_ + std::string(__PRETTY_FUNCTION__) + ":\"" +
                             this->Name() +
                             "\""
                             " Called for block of type " +
                             ParameterBlockTypeName(this->Type()) + " which has no value.");
  }
}

// #################################################################
/**Returns the number of parameters in a block. This is normally
 * only useful for the ARRAY type.*/
size_t
ParameterBlock::NumParameters() const
{
  return parameters_.size();
}

// ################################################################
/**Returns the sub-parameters of this block.*/
const std::vector<ParameterBlock>&
ParameterBlock::Parameters() const
{
  return parameters_;
}

// ################################################################
/**Returns whether or not the block has a value. If this block has
 * sub-parameters it should not have a value. This is a good way to
 * check if the block is actually a single value because some
 * Parameter blocks can be passed as empty.*/
bool
ParameterBlock::HasValue() const
{
  return value_ptr_ != nullptr;
}

// Mutators
// #################################################################
/**Changes the block type to array, making it accessible via integer
 * keys.*/
void
ParameterBlock::ChangeToArray()
{
  const std::string fname = __PRETTY_FUNCTION__;
  if (parameters_.empty())
  {
    type_ = ParameterBlockType::ARRAY;
    return;
  }

  const auto& first_param = parameters_.front();
  for (const auto& param : parameters_)
    if (param.Type() != first_param.Type())
      throw std::logic_error(error_origin_scope_ + fname +
                             ": Cannot change ParameterBlock to "
                             "array. It has existing parameters and they are not of the same"
                             "type.");

  type_ = ParameterBlockType::ARRAY;
}

// #################################################################
// NOLINTBEGIN(misc-no-recursion)
/**Sets a string to be displayed alongside exceptions that give some
 * notion of the origin of the error.*/
void
ParameterBlock::SetErrorOriginScope(const std::string& scope)
{
  error_origin_scope_ = scope;
  for (auto& param : parameters_)
    param.SetErrorOriginScope(scope);
}
// NOLINTEND(misc-no-recursion)

// #################################################################
/**Checks that the block is of the given type. If it is not it
 * will throw an exception `std::logic_error`.*/
void
ParameterBlock::RequireBlockTypeIs(ParameterBlockType type) const
{
  if (Type() != type)
    throw std::logic_error(error_origin_scope_ + ":" + Name() + " Is required to be of type " +
                           ParameterBlockTypeName(type) + " but is " +
                           ParameterBlockTypeName(Type()));
}

/**Check that the parameter with the given name exists otherwise
 * throws a `std::logic_error`.*/
void
ParameterBlock::RequireParameter(const std::string& param_name) const
{
  if (not Has(param_name))
    throw std::logic_error(error_origin_scope_ + ":" + Name() + " Is required to have parameter " +
                           param_name);
}

// #################################################################
/**Adds a parameter to the sub-parameter list.*/
void
ParameterBlock::AddParameter(ParameterBlock block)
{
  for (const auto& param : parameters_)
    if (param.Name() == block.Name())
      throw std::invalid_argument(error_origin_scope_ + std::string(__PRETTY_FUNCTION__) +
                                  ": Attempting to add duplicate parameter " + param.Name() +
                                  " to "
                                  "block " +
                                  this->Name());
  parameters_.push_back(std::move(block));

  SortParameters();
}

// #################################################################
/**Sorts the sub-parameter list according to name. This is useful
 * for regression testing.*/
void
ParameterBlock::SortParameters()
{
  struct AlphabeticFunctor
  {
    bool operator()(const ParameterBlock& paramA, const ParameterBlock& paramB)
    {
      return paramA.Name() < paramB.Name();
    }
  };

  struct AlphabeticNumericFunctor
  {
    bool operator()(const ParameterBlock& paramA, const ParameterBlock& paramB)
    {
      return std::stoi(paramA.Name()) < std::stoi(paramB.Name());
    }
  };

  // The different functor here is necessary when the parameters are guaranteed
  // to have integer names that were converted to strings. It never showed up
  // because we were not testing with enough values. Essentially "11" < "2" in
  // the realm of strings but not in integer world.
  if (this->Type() != ParameterBlockType::ARRAY)
    std::sort(parameters_.begin(), parameters_.end(), AlphabeticFunctor());
  else
    std::sort(parameters_.begin(), parameters_.end(), AlphabeticNumericFunctor());
}

// #################################################################
/**Returns true if a parameter with the specified name is in the
 * list of sub-parameters. Otherwise, false.*/
bool
ParameterBlock::Has(const std::string& param_name) const
{
  return std::any_of(parameters_.begin(),
                     parameters_.end(),
                     [&param_name](const ParameterBlock& param)
                     { return param.name_ == param_name; });
}

// #################################################################
/**Gets a parameter by name.*/
ParameterBlock&
ParameterBlock::GetParam(const std::string& param_name)
{
  for (auto& param : parameters_)
    if (param.Name() == param_name) return param;

  throw std::out_of_range(error_origin_scope_ + ":" + std::string(__PRETTY_FUNCTION__) +
                          ": Parameter \"" + param_name + "\" not present in block");
}

// #################################################################
/**Gets a parameter by index.*/
ParameterBlock&
ParameterBlock::GetParam(size_t index)
{
  try
  {
    return parameters_.at(index);
  }
  catch (const std::out_of_range& oor)
  {
    throw std::out_of_range(error_origin_scope_ + std::string(__PRETTY_FUNCTION__) +
                            ": Parameter with index " + std::to_string(index) +
                            " not present in block");
  }
}

// #################################################################
/**Gets a parameter by name.*/
const ParameterBlock&
ParameterBlock::GetParam(const std::string& param_name) const
{
  for (const auto& param : parameters_)
    if (param.Name() == param_name) return param;

  throw std::out_of_range(error_origin_scope_ + std::string(__PRETTY_FUNCTION__) +
                          ": Parameter \"" + param_name + "\" not present in block");
}

// #################################################################
/**Gets a parameter by index.*/
const ParameterBlock&
ParameterBlock::GetParam(size_t index) const
{
  try
  {
    return parameters_.at(index);
  }
  catch (const std::out_of_range& oor)
  {
    throw std::out_of_range(error_origin_scope_ + std::string(__PRETTY_FUNCTION__) +
                            ": Parameter with index " + std::to_string(index) +
                            " not present in block");
  }
}

// #################################################################
//  NOLINTBEGIN(misc-no-recursion)
/**Print the block tree structure into a designated string.*/
void
ParameterBlock::RecursiveDumpToString(std::string& outstr, const std::string& offset) const
{
  outstr += offset + this->Name() + " = \n";
  outstr += offset + "{\n";

  if (HasValue()) outstr += value_ptr_->PrintStr();

  for (const auto& param : parameters_)
  {

    switch (param.Type())
    {
      case ParameterBlockType::BOOLEAN:
      {
        outstr += offset + "  " + param.Name() + " = ";
        const bool value = param.Value().BoolValue();
        outstr += std::string(value ? "true" : "false") + ",\n";
        break;
      }
      case ParameterBlockType::FLOAT:
      {
        outstr += offset + "  " + param.Name() + " = ";
        const double value = param.Value().FloatValue();
        outstr += std::to_string(value) + ",\n";
        break;
      }
      case ParameterBlockType::STRING:
      {
        outstr += offset + "  " + param.Name() + " = ";
        const auto& value = param.Value().StringValue();
        outstr += "\"" + value + "\",\n";
        break;
      }
      case ParameterBlockType::INTEGER:
      {
        outstr += offset + "  " + param.Name() + " = ";
        const int64_t value = param.Value().IntegerValue();
        outstr += std::to_string(value) + ",\n";
        break;
      }
      case ParameterBlockType::ARRAY:
      case ParameterBlockType::BLOCK:
      {
        param.RecursiveDumpToString(outstr, offset + "  ");
        break;
      }
      default:
        break;
    }
  } // for parameter

  outstr += offset + "}\n";
}
// NOLINTEND(misc-no-recursion)

// #################################################################
//  NOLINTBEGIN(misc-no-recursion)
/**Print the block tree structure into a designated string.*/
void
ParameterBlock::RecursiveDumpToJSON(std::string& outstr) const
{
  if (HasValue())
  {
    outstr += value_ptr_->PrintStr(/*with_type=*/false);
    return;
  }

  outstr += (this->Type() == ParameterBlockType::ARRAY ? "[" : "{");
  for (const auto& param : parameters_)
  {

    switch (param.Type())
    {
      case ParameterBlockType::BOOLEAN:
      {
        outstr += "\"" + param.Name() + "\" = ";
        const bool value = param.Value().BoolValue();
        outstr += std::string(value ? "true" : "false") + ",\n";
        break;
      }
      case ParameterBlockType::FLOAT:
      {
        outstr += "\"" + param.Name() + "\" = ";
        const double value = param.Value().FloatValue();
        outstr += std::to_string(value) + ",\n";
        break;
      }
      case ParameterBlockType::STRING:
      {
        outstr += "\"" + param.Name() + "\" = ";
        const auto& value = param.Value().StringValue();
        outstr += "\"" + value + "\",\n";
        break;
      }
      case ParameterBlockType::INTEGER:
      {
        outstr += "\"" + param.Name() + "\" = ";
        const int64_t value = param.Value().IntegerValue();
        outstr += std::to_string(value) + ",\n";
        break;
      }
      case ParameterBlockType::ARRAY:
      case ParameterBlockType::BLOCK:
      {
        param.RecursiveDumpToJSON(outstr);
        break;
      }
      default:
        break;
    }
  } // for parameter
  outstr += (this->Type() == ParameterBlockType::ARRAY ? "]" : "}");
}
// NOLINTEND(misc-no-recursion)

} // namespace chi
