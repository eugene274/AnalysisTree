//
// Created by eugene on 19/10/2020.
//

#include "AnalysisTreeEvent.hpp"


void AnalysisTree::AnalysisTreeEvent::LoadInputBranch(const AnalysisTree::BranchConfig& branch_config, TTree* input_tree) {
  auto branch_name = branch_config.GetName();
  auto tree_branch = input_tree->GetBranch(branch_name.c_str());
  if (!tree_branch) {
    Warning(__func__, "Branch %s not found in the input TTree", branch_name.c_str());
    return;
  }
  if (branch_config.GetType() == DetType::kTrack) {
    NewInputBranchT<TrackDetector>(branch_config, tree_branch);
    return;
  } else if (branch_config.GetType() == DetType::kEventHeader) {
    NewInputBranchT<EventHeader>(branch_config, tree_branch);
    return;
  } else if (branch_config.GetType() == DetType::kHit) {
    NewInputBranchT<HitDetector>(branch_config, tree_branch);
    return;
  } else if (branch_config.GetType() == DetType::kModule) {
    NewInputBranchT<ModuleDetector>(branch_config, tree_branch);
    return;
  } else if (branch_config.GetType() == DetType::kParticle) {
    NewInputBranchT<Particles>(branch_config, tree_branch);
    return;
  }

  assert(false);
  __builtin_unreachable();
}

AnalysisTree::AnalysisTreeEvent AnalysisTree::AnalysisTreeEvent::FromTTree(AnalysisTree::Configuration* configuration, TTree* tree) {
  AnalysisTreeEvent result;

  result.input_tree_ptr_ = tree;

  auto branches = configuration->GetBranchConfigs();
  for (auto& branch_config : branches) {
    result.LoadInputBranch(branch_config, tree);
  }

  return result;
}

AnalysisTree::AnalysisTreeEvent AnalysisTree::AnalysisTreeEvent::FromDirectory(TDirectory* dir, const std::string& configuration_name, const std::string& tree_name) {
  return FromTTree(dir->Get<Configuration>(configuration_name.c_str()), dir->Get<TTree>(tree_name.c_str()));
}
