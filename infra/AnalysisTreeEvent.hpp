//
// Created by eugene on 19/10/2020.
//

#ifndef ANALYSISTREE_INFRA_ANALYSISTREEEVENT_HPP
#define ANALYSISTREE_INFRA_ANALYSISTREEEVENT_HPP

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "FunctionTraits.hpp"
#include <Configuration.hpp>
#include <Detector.hpp>
#include <EventHeader.hpp>
#include <Particle.hpp>

namespace AnalysisTree {

class variable_exists_exception : public std::exception {};
class variable_does_not_exist_exception : public std::exception {};

class IBaseVariable {
 public:
  [[nodiscard]] virtual size_t GetNChannels() const = 0;
  [[nodiscard]] virtual double GetValue(size_t i_channel) const = 0;

  double operator[](size_t i_channel) const {
    return GetValue(i_channel);
  }
};

class ReadOnlyVarProxy : public IBaseVariable {
 public:
  ReadOnlyVarProxy() = default;
  explicit ReadOnlyVarProxy(std::shared_ptr<const IBaseVariable> ptr) : ptr_(std::move(ptr)) {}
  [[nodiscard]] size_t GetNChannels() const override {
    return ptr_->GetNChannels();
  }
  [[nodiscard]] double GetValue(size_t i_channel) const override {
    return ptr_->GetValue(i_channel);
  }

 private:
  std::shared_ptr<const IBaseVariable> ptr_;
};

class IMutableVariable : public IBaseVariable {
 public:
  virtual void SetValue(size_t i_channel, double value) = 0;
};

class IBranch {
 public:
  [[nodiscard]] bool HasVariable(std::string_view name) const {
    auto variable_names = GetVariableNames();
    return std::find(std::begin(variable_names), std::end(variable_names), name) != std::end(variable_names);
  }
  [[nodiscard]] std::vector<std::string> GetVariableNames() const {
    std::vector<std::string> result;
    std::copy(std::begin(transient_var_names_), std::end(transient_var_names_), std::back_inserter(result));
    auto impl_variables = GetVariableNamesImpl();
    std::copy(std::begin(impl_variables), std::end(impl_variables), std::back_inserter(result));
    return result;
  }
  ReadOnlyVarProxy GetVariable(std::string_view variable_name) {
    if (transient_variables_.find(std::string(variable_name)) != transient_variables_.end()) {
      return transient_variables_[std::string(variable_name)];
    }
    return ReadOnlyVarProxy(GetVariableImpl(variable_name));
  };
  ReadOnlyVarProxy operator[](std::string_view variable_name) {
    return GetVariable(variable_name);
  }

  template<typename Function>
  ReadOnlyVarProxy Define(std::string_view variable_name, Function&& function, const std::vector<std::string>& arg_names);

  [[nodiscard]] virtual size_t GetNChannels() const = 0;

 protected:
  virtual std::vector<std::string> GetVariableNamesImpl() const = 0;
  virtual ReadOnlyVarProxy GetVariableImpl(std::string_view name) const = 0;

 private:
  std::vector<std::string> transient_var_names_;
  std::map<std::string, ReadOnlyVarProxy> transient_variables_;
};

template<typename Function>
class FunctionVariable : public IBaseVariable {
  using Traits = AnalysisTree::Details::FunctionTraits<Function>;

 public:
  FunctionVariable(const IBranch* ptr, Function function, std::vector<ReadOnlyVarProxy> args) : branch_ptr_(ptr),
                                                                                                function_(std::move(function)),
                                                                                                function_args_(std::move(args)) {}
  [[nodiscard]] size_t GetNChannels() const final {
    return branch_ptr_->GetNChannels();
  }
  [[nodiscard]] double GetValue(size_t i_channel) const final {
    return GetValueImpl(i_channel, std::make_index_sequence<Traits::Arity>());
  }

 private:
  template<size_t... IArgs>
  double GetValueImpl([[maybe_unused]] size_t i_channel, std::index_sequence<IArgs...>) const {
    return function_(function_args_[IArgs].GetValue(i_channel)...);
  }
  const IBranch* branch_ptr_;
  Function function_;
  std::vector<ReadOnlyVarProxy> function_args_;
};

template<typename Function>
ReadOnlyVarProxy IBranch::Define(std::string_view variable_name, Function&& function, const std::vector<std::string>& arg_names) {
  if (HasVariable(variable_name)) {
    throw variable_exists_exception();
  }
  std::vector<ReadOnlyVarProxy> args;
  std::transform(std::begin(arg_names), std::end(arg_names),
                 std::back_inserter(args),
                 [this](const std::string& arg_name) { return GetVariable(arg_name); });
  auto emplace_result = transient_variables_.emplace(variable_name, std::make_shared<FunctionVariable<Function>>(this, std::forward<Function>(function), args));
  if (emplace_result.second) {
    transient_var_names_.emplace_back(variable_name);
    return emplace_result.first->second;
  }

  assert(false);
  __builtin_unreachable();
}

using BranchPtr = std::shared_ptr<IBranch>;

class InMemoryVariable : public IMutableVariable {

  using ValContainer = std::vector<double>;

 public:
  InMemoryVariable(const IBranch* ptr) : branch_ptr_(ptr) {}
  [[nodiscard]] size_t GetNChannels() const final {
    return branch_ptr_->GetNChannels();
  }
  [[nodiscard]] double GetValue(size_t i_channel) const final {
    return val_[i_channel];
  }
  void Init() {
    val_.resize(GetNChannels(), 0.);
  }
  void SetValue(size_t i_channel, double value) final {
    val_[i_channel] = value;
  }
  double& operator[](size_t i_channel) {
    return val_[i_channel];
  }

  ValContainer::iterator begin() { return std::begin(val_); }
  ValContainer::iterator end() { return std::end(val_); }

 private:
  ValContainer val_{0.};
  const IBranch* branch_ptr_;
};

class VirtualBranch : public IBranch {

 public:
  explicit VirtualBranch(const std::vector<std::string>& field_names) {
    for (auto& field : field_names) {
      AddVariable(field);
    }
  }

  std::shared_ptr<InMemoryVariable> AddVariable(std::string_view variable_name) {
    if (HasVariable(variable_name)) {
      throw variable_exists_exception();
    }
    auto emplace_result = variables_.emplace(variable_name, std::make_shared<InMemoryVariable>(this));
    variable_names_.emplace_back(variable_name);
    return emplace_result.first->second;
  }

  void SetNChannels(std::size_t new_size) {
    n_channels = new_size;
  };

  [[nodiscard]] size_t GetNChannels() const final {
    return n_channels;
  }

 protected:
  [[nodiscard]] std::vector<std::string> GetVariableNamesImpl() const override {
    return variable_names_;
  }
  ReadOnlyVarProxy GetVariableImpl(std::string_view name) const override {
    return ReadOnlyVarProxy(variables_.at(std::string(name)));
  }

 public:
  VirtualBranch() = default;
  VirtualBranch(const VirtualBranch&) = default;
  VirtualBranch(VirtualBranch&&) = default;

 private:
  size_t n_channels{0};
  std::vector<std::string> variable_names_;
  std::map<std::string, std::shared_ptr<InMemoryVariable>> variables_;
};

class AnalysisTreeEvent {

  template<typename Entity>
  class AnalysisTreeBranchImpl : public IBranch {
    using Traits = Details::EntityTraits<Entity>;

   public:
    explicit AnalysisTreeBranchImpl(BranchConfig config) : config_(std::move(config)) {
      InitFields();
    }

    [[nodiscard]] size_t GetNChannels() const override {
      return Traits::GetNChannels(*entity_);
    }
    void InitTreeBranch(TBranch* br) {
      br->SetAddress(&entity_);
    }

   protected:
    std::vector<std::string> GetVariableNamesImpl() const final {
      return {};
    }
    ReadOnlyVarProxy GetVariableImpl(std::string_view) const final {
      throw std::runtime_error("Not yet implemented");
    }

   private:
    void InitFields() {
    }

    BranchConfig config_;
    Entity* entity_{new Entity};
  };

 public:
  [[nodiscard]] std::vector<std::string> GetBranches() const {
    return branch_names_;
  };
  [[nodiscard]] BranchPtr GetBranch(std::string_view branch_name) const {
    return branches_.at(std::string(branch_name));
  };

  [[nodiscard]] TTree* GetInputTree() const {
    return input_tree_ptr_;
  }

  BranchPtr operator[](std::string_view branch_name) const {
    return GetBranch(branch_name);
  }

  static AnalysisTreeEvent FromTTree(Configuration* configuration, TTree* tree);

  static AnalysisTreeEvent FromDirectory(TDirectory* dir, const std::string& configuration_name = "Configuration", const std::string& tree_name = "aTree");

 private:
  AnalysisTreeEvent() = default;

  void LoadInputBranch(const BranchConfig& branch_config, TTree* input_tree);

  template<typename T>
  void NewInputBranchT(BranchConfig branch_config, TBranch* br) {
    auto branch_name = branch_config.GetName();
    auto result = std::make_shared<AnalysisTreeBranchImpl<T>>(branch_config);
    result->InitTreeBranch(br);
    branches_.emplace(branch_name, result);
    branch_names_.emplace_back(branch_name);
  }

  std::vector<std::string> branch_names_;
  std::map<std::string, std::shared_ptr<IBranch>> branches_;

  TTree* input_tree_ptr_{nullptr};/// Non-owing pointer
};

}// namespace AnalysisTree

#endif//ANALYSISTREE_INFRA_ANALYSISTREEEVENT_HPP
