//
// Created by eugene on 19/10/2020.
//

#ifndef ANALYSISTREE_INFRA_ANALYSISTREEEVENT_HPP
#define ANALYSISTREE_INFRA_ANALYSISTREEEVENT_HPP

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "FunctionTraits.hpp"

class variable_exists_exception : public std::exception {};
class variable_does_not_exist_exception : public std::exception {};

class IBaseVariable {
 public:
  virtual size_t GetNChannels() const = 0;
  virtual double GetValue(size_t i_channel) const = 0;

  double operator[](size_t i_channel) const {
    return GetValue(i_channel);
  }
};

class IMutableVariable : public IBaseVariable {
 public:
  virtual void SetValue(size_t i_channel, double value) = 0;
};

template<typename Function>
class FunctionVariable : public IBaseVariable {
  using Traits = AnalysisTree::Details::FunctionTraits<Function>;

 public:

  [[nodiscard]] size_t GetNChannels() const final {
    if (!function_args_.empty()) {
      return function_args_.front()->GetNChannels();
    }
    return 0;
  }
  [[nodiscard]] double GetValue(size_t i_channel) const final {
    return GetValueImpl(i_channel, std::make_index_sequence<Traits::Arity>());
  }

 private:
  template<size_t... IArgs>
  double GetValueImpl(size_t i_channel, std::index_sequence<IArgs...>) const {
    return function_(function_args_[IArgs]->GetValue(i_channel)...);
  }
  Function function_;
  std::vector<std::shared_ptr<IBaseVariable>> function_args_;
};

class IBranch {
 public:
  [[nodiscard]] bool HasVariable(std::string_view name) const {
    auto variable_names = GetVariableNames();
    return std::find(std::begin(variable_names), std::end(variable_names), name) != std::end(variable_names);
  }
  [[nodiscard]] std::vector<std::string> GetVariableNames() const {
    std::vector<std::string> result;
    std::copy(std::begin(defined_variable_names_), std::end(defined_variable_names_), std::back_inserter(result));
    auto impl_variables = GetVariableNamesImpl();
    std::copy(std::begin(impl_variables), std::end(impl_variables), std::back_inserter(result));
    return result;
  }
  std::shared_ptr<IBaseVariable> GetVariable(std::string_view variable_name) {
    if (defined_variables_.find(std::string(variable_name)) != defined_variables_.end()) {
      return defined_variables_[std::string(variable_name)];
    }
    return GetVariableImpl(variable_name);
  };

  template<typename Function>
  std::shared_ptr<IBaseVariable> Define(std::string_view variable_name, Function&& function, const std::vector<std::string>& arg_names) {
  }

  virtual size_t GetNChannels() const = 0;

 protected:
  virtual std::vector<std::string> GetVariableNamesImpl() const = 0;
  virtual std::shared_ptr<IBaseVariable> GetVariableImpl(std::string_view name) = 0;

 private:
  std::vector<std::string> defined_variable_names_;
  std::map<std::string, std::shared_ptr<IBaseVariable>> defined_variables_;
};

using BranchPtr = std::shared_ptr<IBranch>;

class InMemoryVariable : public IMutableVariable {

 public:
  [[nodiscard]] size_t GetNChannels() const final {
    return val_.size();
  }
  [[nodiscard]] double GetValue(size_t i_channel) const final {
    return val_[i_channel];
  }
  void SetValue(size_t i_channel, double value) final {
    val_[i_channel] = value;
  }
  double& operator[](size_t i_channel) {
    return val_[i_channel];
  }

 private:
  friend class VirtualBranch;
  std::vector<double> val_{0.};
};

class VirtualBranch : public IBranch {

 public:
  std::shared_ptr<InMemoryVariable> AddVariable(std::string_view variable_name) {
    if (HasVariable(variable_name)) {
      throw variable_exists_exception();
    }
    auto emplace_result = variables_.emplace(variable_name, std::make_shared<InMemoryVariable>());
    variable_names_.emplace_back(variable_name);
    return emplace_result.first->second;
  }

  void SetNChannels(std::size_t new_size) {
    n_channels = new_size;
    for (auto& variable : variables_) {
      variable.second->val_.resize(n_channels);
    }
  };

  [[nodiscard]] size_t GetNChannels() const final {
    return n_channels;
  }

 protected:
  [[nodiscard]] std::vector<std::string> GetVariableNamesImpl() const override {
    return variable_names_;
  }
  std::shared_ptr<IBaseVariable> GetVariableImpl(std::string_view name) override {
    return variables_.at(std::string(name));
  }

 private:
  /* copy and assignment is not allowed for the user */
  VirtualBranch(const VirtualBranch&) = default;
  VirtualBranch(VirtualBranch&&) = default;

  size_t n_channels{0};
  std::vector<std::string> variable_names_;
  std::map<std::string, std::shared_ptr<InMemoryVariable>> variables_;
};

class AnalysisTreeEvent {
 public:
  BranchPtr GetBranch(std::string_view branch_name) const;

  std::shared_ptr<VirtualBranch> NewVirtualBranch(std::string_view branch_name);

 private:
};

#endif//ANALYSISTREE_INFRA_ANALYSISTREEEVENT_HPP
