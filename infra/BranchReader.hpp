#ifndef ANALYSISTREEQA_SRC_BRANCHREADER_H_
#define ANALYSISTREEQA_SRC_BRANCHREADER_H_

#include <string>
#include <utility>
#include <vector>

#include <TTree.h>

#include "Constants.hpp"
#include "Detector.hpp"
#include "EventHeader.hpp"
#include "Variable.hpp"
#include "VariantMagic.hpp"

#include "EntityTraits.hpp"

namespace AnalysisTree {

class Variable;
class Cuts;

#if USEBOOST
using BranchPointer = boost::variant<TrackDetector*, Particles*, ModuleDetector*, HitDetector*, EventHeader*>;
#else
using BranchPointer = std::variant<TrackDetector*, Particles*, ModuleDetector*, HitDetector*, EventHeader*>;
#endif

/**
 * @brief BranchReader keeps data-object associated with tree branch and list of cuts for this branch
 * lots of visitor boilerplate
 */
class BranchReader {

 public:
  BranchReader() = default;
  BranchReader(std::string name, void* data, DetType type, Cuts* cuts = nullptr);

  [[nodiscard]] const std::string& GetName() const { return name_; }
  [[nodiscard]] DetType GetType() const { return type_; }
  [[nodiscard]] const Cuts* GetCut() const { return cuts_; }
  [[nodiscard]] double GetValue(const Variable& var, int i_channel);

  size_t GetNumberOfChannels();
  bool ApplyCut(int i_channel);

  [[nodiscard]] const BranchPointer& GetData() const {
    return data_;
  }

  [[nodiscard]] int GetId() const { return id_; }

 protected:
  std::string name_;
  BranchPointer data_{};
  Cuts* cuts_{nullptr};
  int id_{-1};
  DetType type_{DetType(-1)};
};

namespace Experimental {

/**
 * @brief Facade of Variable
 */
class IVariable {
 public:
  virtual size_t size() const = 0;
  virtual double GetValue(size_t i_channel) const = 0;
  virtual void SetValue(size_t i_channel, double value) = 0;
};

typedef std::shared_ptr<IVariable> VariablePtr;

/**
 * @brief Facade of branch reader
 */
class IBranchReader {

 public:
  virtual size_t GetNChannels() const = 0;
  virtual VariablePtr GetVariable(const std::string& name) const = 0;
  virtual std::vector<std::string> GetVariables() const = 0;
};

/**
 * @brief Typed implementation of BranchReader
 * @tparam Entity
 */
template<typename Entity>
class BranchReaderT : public IBranchReader {
  using Traits = Details::EntityTraits<Entity>;
  typedef typename Traits::ChannelType ChannelType;

  struct DataVariableImpl;
  typedef std::shared_ptr<DataVariableImpl> DataVariablePtr;

  class DataVariableImpl : public IVariable {
   public:
    DataVariableImpl(Entity* data, ShortInt_t field_id, Types field_type) : entity_(data), field_id_(field_id), field_type_(field_type) {}
    size_t size() const final {
      return Traits::GetNChannels(*entity_);
    }
    double GetValue(size_t ich) const final {
      ChannelType& channel = Traits::GetChannel(*entity_, ich);
      if (field_type_ == Types::kInteger) {
        return channel.template GetField<int>(field_id_);
      } else if (field_type_ == Types::kFloat) {
        return channel.template GetField<float>(field_id_);
      } else if (field_type_ == Types::kBool) {
        return channel.template GetField<bool>(field_id_);
      }
    }
    void SetValue(size_t ich, double new_value) final {
      ChannelType& channel = Traits::GetChannel(*entity_, ich);
      if (field_type_ == Types::kInteger) {
        channel.template SetField<int>(new_value, field_id_);
      } else if (field_type_ == Types::kFloat) {
        channel.template SetField<float>(new_value, field_id_);
      } else if (field_type_ == Types::kBool) {
        /* conversion rules? */
        channel.template SetField<bool>(new_value, field_id_);
      }
    }

   private:

    Entity* entity_{nullptr};
    ShortInt_t field_id_{AnalysisTree::UndefValueShort};
    Types field_type_{Types::kNumberOfTypes};
  };

 public:
  BranchReaderT(BranchConfig config, Entity* data) : config_(std::move(config)), data_(data) {
    InitVariables<int>();
    InitVariables<float>();
    InitVariables<bool>();
  }

  VariablePtr GetVariable(const std::string& name) const final {
    return data_variables_.at(name);
  }

  std::vector<std::string> GetVariables() const final {
    std::vector<std::string> result;
    for (auto &entry : data_variables_) {
      result.push_back(entry.first);
    }
    return result;
  }
 public:
  size_t GetNChannels() const final {
    return Traits::GetNChannels(*data_);
  }

 private:
  template<typename T>
  void InitVariables() {
    auto vmap = config_.GetMap<T>();
    for (auto& entry : vmap) {
      std::string field_name = entry.first;
      ShortInt_t field_id = entry.second;
      Types field_type = config_.GetFieldType(field_name);
      data_variables_.emplace(field_name, std::make_shared<DataVariableImpl>(data_, field_id, field_type));
    }
  }

  BranchConfig config_;
  Entity* data_{nullptr};/// shared among variables
  std::map<std::string, DataVariablePtr> data_variables_;
};

}// namespace Experimental

}// namespace AnalysisTree

#endif//ANALYSISTREEQA_SRC_BRANCHREADER_H_
