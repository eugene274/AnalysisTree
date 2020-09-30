#ifndef ANALYSISTREE_FILLTASK_H_
#define ANALYSISTREE_FILLTASK_H_

#include <AnalysisTree/Configuration.hpp>

class TChain;
class TTree;
class TFile;

namespace AnalysisTree {

class Configuration;
class DataHeader;

/*
 * Notes on variable management
 *
 * Variable:
 *  T *data - pointer to the data associated with branch
 *
 *  GetValue(channel)
 *  SetValue(channel)
 *  Partially implemented in BranchView, unlike there only one type of variable is planned
 *  with direct interaction to underlying object
 *
 *  New variable manager is responsible for issuing variables by branch name / field name
 *  It keeps pointer to the tree and configuration object
 */

class FillTask {

 public:
  FillTask() = default;
  virtual ~FillTask() = default;

  virtual void Init(std::map<std::string, void*>&) = 0;
  virtual void Exec() = 0;
  virtual void Finish() = 0;

  void SetInChain(TChain* in_chain);

  void SetInConfiguration(Configuration* config) {
    config_ = config;
  }

  void SetOutTree(TTree* out_tree) { out_tree_ = out_tree; }
  void SetOutConfiguration(Configuration* config) { out_config_ = config; }
  void SetDataHeader(DataHeader* data_header) { data_header_ = data_header; }
  void SetOutFile(TFile* f) { out_file_ = f; }
  void SetInputBranchNames(const std::vector<std::string>& br) { in_branches_ = br; }
  void SetOutputBranchName(const std::string& br) { out_branch_ = br; }

  [[nodiscard]] const std::string& GetOutputBranchName() const { return out_branch_; }

  [[nodiscard]] const std::vector<std::string>& GetInputBranchNames() const { return in_branches_; }

 protected:
  TChain* in_chain_{nullptr};
  Configuration* config_{nullptr};
  DataHeader* data_header_{nullptr};

  TFile* out_file_{nullptr};
  TTree* out_tree_{nullptr};
  Configuration* out_config_{nullptr};

  std::vector<std::string> in_branches_{};
  std::string out_branch_{};
};

}// namespace AnalysisTree

#endif//ANALYSISTREE_FILLTASK_H_
