#include "Cuts.hpp"
#include "Configuration.hpp"

namespace AnalysisTree{

void Cuts::Init(const AnalysisTree::Configuration& conf) {
  for (auto &cut : cuts_) {
    for (auto &var : cut.Variables()) {
      var.Init(conf);
    }
  }
  is_init_ = true;
}

void Cuts::Print() const {
  std::cout << "Cut " << name_ << " defined as:" << std::endl;
  for (const auto &cut : cuts_) {
    cut.Print();
  }
}

}