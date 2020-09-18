//
// Created by eugene on 18/09/2020.
//

#ifndef ANALYSISTREE_RDATAFRAME_ANALYSISTREERDATASOURCEIMPL_HPP
#define ANALYSISTREE_RDATAFRAME_ANALYSISTREERDATASOURCEIMPL_HPP

#include <ROOT/RDataSource.hxx>
#include <TTree.h>

#include <AnalysisTree/BranchConfig.hpp>

namespace AnalysisTree::RDF::Details {

template<typename Entity>
class AnalysisTreeBranchRDataSourceImpl : public ROOT::RDF::RDataSource {

  /* per-thread */
  struct Slot_t {
    bool is_initialized{false};
    unsigned int i_slot{0u};
    Entity* obj{nullptr};
    TTree* tree_ptr{nullptr};
  };

  struct ColumnInfo_t {
    std::string name;
    std::string type_name;
    short int field_id;
  };

 public:
  void SetNSlots(unsigned int nSlots) override {
    slots_.resize(nSlots);
  }
  const std::vector<std::string>& GetColumnNames() const override {
    return column_names_;
  }
  bool HasColumn(std::string_view View) const override {
    return std::find(std::begin(column_names_), std::end(column_names_), View) != std::end(column_names_);
  }
  std::string GetTypeName(std::string_view View) const override {
    return columns_info_.at(View.data()).type_name;
  }
  std::vector<std::pair<ULong64_t, ULong64_t>> GetEntryRanges() override {
    throw std::runtime_error(std::string(__func__) + " is not yet implemented");
  }

  bool SetEntry(unsigned int i_slot, ULong64_t entry) override {
    if (slots_[i_slot].is_initialized) {
      slots_[i_slot].tree_ptr->GetEntry(entry);
    }
  }

  Record_t GetColumnReadersImpl(std::string_view name, const std::type_info& Info) override {
    throw std::runtime_error(std::string(__func__) + " is not yet implemented");
  }

  static std::string GetTypeString(AnalysisTree::Types t) {
    using AnalysisTree::Types;
    if (Types::kBool == t) {
      return "bool";
    } else if (Types::kFloat == t) {
      return "float";
    } else if (Types::kInteger == t) {
      return "int";
    }
    throw std::runtime_error("Unknown type");
  }

  void ReadColumnData() {
    columns_info_.clear();
    ReadColumnDataT<int>();
    ReadColumnDataT<float>();
    ReadColumnDataT<bool>();
  }

  template<typename T>
  void ReadColumnDataT() {
    auto columns_map = branch_config_.GetMap<T>();
    for (auto& map_entry : columns_map) {
      auto column_name = map_entry.first;
      auto column_type = branch_config_.GetFieldType(map_entry.first);
      auto field_id = branch_config_.GetFieldId(column_name);

      ColumnInfo_t info;
      info.name = column_name;
      info.type_name = GetTypeString(column_type);
      info.field_id = field_id;
      columns_info_.emplace(column_name, std::move(info));

      column_names_.push_back(column_name);
    }
  }

  TTree *OpenTree() { return nullptr; }

  void InitSlot(unsigned int i_slot, ULong64_t i_entry) override {
    Slot_t& slot = slots_[i_slot];

    if (!slot.is_initialized) {
      slot.i_slot = i_slot;
      slot.obj = new Entity;
      slot.tree_ptr = OpenTree();
      if (!slot.tree_ptr) {
        throw std::runtime_error("Unable to open TTree");
      }
      slot.tree_ptr->SetBranchAddress(branch_config_.GetName().c_str(), &slot.obj);
      slot.tree_ptr->GetEntry(i_entry);
    }
  }

  BranchConfig branch_config_;

  std::map<std::string, ColumnInfo_t> columns_info_;
  std::vector<std::string> column_names_;
  std::vector<Slot_t> slots_;
};

}// namespace AnalysisTree::RDF::Details

#endif//ANALYSISTREE_RDATAFRAME_ANALYSISTREERDATASOURCEIMPL_HPP
