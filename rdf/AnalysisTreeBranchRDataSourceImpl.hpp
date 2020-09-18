//
// Created by eugene on 18/09/2020.
//

#ifndef ANALYSISTREE_RDATAFRAME_ANALYSISTREERDATASOURCEIMPL_HPP
#define ANALYSISTREE_RDATAFRAME_ANALYSISTREERDATASOURCEIMPL_HPP

#include <ROOT/RDataSource.hxx>

#include <AnalysisTree/BranchConfig.hpp>

namespace AnalysisTree::RDF::Details {

class AnalysisTreeBranchRDataSourceImpl : public ROOT::RDF::RDataSource {

 public:
  void SetNSlots(unsigned int nSlots) override;
  const std::vector<std::string>& GetColumnNames() const override;
  bool HasColumn(std::string_view View) const override;
  std::string GetTypeName(std::string_view View) const override;
  std::vector<std::pair<ULong64_t, ULong64_t>> GetEntryRanges() override;
  bool SetEntry(unsigned int slot, ULong64_t entry) override;
  Record_t GetColumnReadersImpl(std::string_view name, const std::type_info& Info) override;

  std::string GetTypeString(AnalysisTree::Types t) {
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
    auto columns_map = config_.GetMap<T>();
    for (auto & map_entry : columns_map) {
      auto column_name = map_entry.first;
      auto column_type = config_.GetFieldType(map_entry.first);

      ColumnInfo_t info;
      info.name = column_name;
      info.type_name = GetTypeString(column_type);
      columns_info_.emplace(column_name, std::move(info));

      column_names_.push_back(column_name);
    }
  }

  struct ColumnInfo_t {
    std::string name;
    std::string type_name;
  };

  unsigned int n_slots_{0};
  BranchConfig config_;

  std::map<std::string,ColumnInfo_t> columns_info_;
  std::vector<std::string> column_names_;
};

}

#endif//ANALYSISTREE_RDATAFRAME_ANALYSISTREERDATASOURCEIMPL_HPP
