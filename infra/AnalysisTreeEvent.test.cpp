//
// Created by eugene on 19/10/2020.
//

#include "AnalysisTreeEvent.hpp"
#include <TFile.h>
#include <gtest/gtest.h>

namespace {

using namespace AnalysisTree;

TEST(AnalysisTreeEvent, AddVariable) {
  VirtualBranch vb;
  auto v1 = vb.AddVariable("v1");
  EXPECT_THROW(vb.AddVariable("v1"), variable_exists_exception);
  EXPECT_TRUE(vb.HasVariable("v1"));
  EXPECT_NO_THROW(vb.AddVariable("v2"));
  EXPECT_TRUE(vb.HasVariable("v2"));
  EXPECT_EQ(vb.GetVariableNames(), std::vector<std::string>({"v1","v2"}));
}


TEST(AnalysisTreeEvent, VirtualBranch) {
  VirtualBranch vb;
  auto v1 = vb.AddVariable("v1");
  EXPECT_TRUE(vb.HasVariable("v1"));

  vb.SetNChannels(10);
  v1->Init();
  EXPECT_EQ(v1->GetNChannels(), 10);
}

TEST(AnalysisTreeEvent, Define) {
  VirtualBranch vb;
  auto vX = vb.AddVariable("vX");
  auto vY = vb.AddVariable("vY");
  auto vX1 = vb.Define("vX1", [] (double vX) -> double { return vX; }, {"vX"});
  auto vX2 = vb.Define("vX2", [] (double vX) -> double { return vX; }, {"vX"});
  vb.SetNChannels(10);
  size_t ich = 0;
  for (auto &x : *vX) {
    x = ich;
    EXPECT_EQ(vX1[ich], x);
    EXPECT_EQ(vX2[ich], x);
  }
  auto ones = vb.Define("Ones", [] () -> double { return 1.; }, {});
  EXPECT_EQ(ones.GetNChannels(), 10);
}

TEST(AnalysisTreeEvent, AnalysisTreeEvent) {

  TFile f("analysistree.root", "READ");
  if (!f.IsOpen()) {
    std::cout << "File " << f.GetName() << " is not found" << std::endl;
    return;
  }
  auto at_event = AnalysisTreeEvent::FromDirectory(&f);
  auto tree = at_event.GetInputTree();

  for (const auto& bname : at_event.GetBranches()) {
    at_event[bname]->Print(std::cout);
  }

  for (Long64_t iE = 0l; iE < 10; ++iE) {
    EXPECT_NO_THROW(tree->GetEntry(iE));
  }

}


}