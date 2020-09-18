//
// Created by eugene on 18/09/2020.
//

#include <algorithm>

#include "AnalysisTreeBranchRDataSourceImpl.hpp"

using namespace AnalysisTree::RDF::Details;

void AnalysisTreeBranchRDataSourceImpl::SetNSlots(unsigned int nSlots) {
}
const std::vector<std::string>& AnalysisTreeBranchRDataSourceImpl::GetColumnNames() const {
  return column_names_;
}
bool AnalysisTreeBranchRDataSourceImpl::HasColumn(std::string_view View) const {
  return std::find(column_names_.begin(), column_names_.end(), View) != column_names_.end();
}
std::string AnalysisTreeBranchRDataSourceImpl::GetTypeName(std::string_view View) const {
  return columns_info_.at(View.data()).type_name;
}
std::vector<std::pair<ULong64_t, ULong64_t>> AnalysisTreeBranchRDataSourceImpl::GetEntryRanges() {
  return std::vector<std::pair<ULong64_t, ULong64_t>>();
}
bool AnalysisTreeBranchRDataSourceImpl::SetEntry(unsigned int slot, ULong64_t entry) {
  return false;
}
ROOT::RDF::RDataSource::Record_t AnalysisTreeBranchRDataSourceImpl::GetColumnReadersImpl(std::string_view name, const std::type_info& Info) {
  return ROOT::RDF::RDataSource::Record_t();
}
