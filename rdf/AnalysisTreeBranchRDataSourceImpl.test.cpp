//
// Created by eugene on 18/09/2020.
//

#include <AnalysisTree/BranchConfig.hpp>
#include <AnalysisTree/EventHeader.hpp>

#include "AnalysisTreeBranchRDataSourceImpl.hpp"

#include <gtest/gtest.h>

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RDataSource.hxx>
#include <memory>

using ROOT::RDataFrame;
using ROOT::RDF::RDataSource;

using namespace AnalysisTree;
using namespace AnalysisTree::RDF;
using Details::AnalysisTreeBranchRDataSourceImpl;

TEST(AnalysisTreeBranchRDataSourceImpl, HasColumn) {
  BranchConfig c("test", DetType::kEventHeader);

  c.AddField<int>("i1");
  c.AddField<int>("i2");

  c.AddField<float>("f1");
  c.AddField<float>("f2");

  c.AddField<bool>("b1");
  c.AddField<bool>("b2");

  AnalysisTreeBranchRDataSourceImpl<bool> rds;
  rds.branch_config_ = c;
  rds.ReadColumnData();

  EXPECT_TRUE(rds.HasColumn("i1"));
  EXPECT_TRUE(rds.HasColumn("i2"));
  EXPECT_TRUE(rds.HasColumn("f1"));
  EXPECT_TRUE(rds.HasColumn("f2"));
  EXPECT_TRUE(rds.HasColumn("b1"));
  EXPECT_TRUE(rds.HasColumn("b2"));

  EXPECT_EQ(rds.GetTypeName("i1"), "int");
  EXPECT_EQ(rds.GetTypeName("b1"), "bool");
  EXPECT_EQ(rds.GetTypeName("f1"), "float");
}


TEST(AnalysisTreeBranchRDataSourceImpl, RDataFrame) {

  auto ds = std::make_unique<AnalysisTreeBranchRDataSourceImpl<AnalysisTree::EventHeader>>();

  BranchConfig c("Test", DetType::kEventHeader);
  c.AddField<float>("vtx_x");

  ds->branch_config_ = c;
  ds->ReadColumnData();

  RDataFrame df(std::move(ds));
  auto columns = df.GetColumnNames();

  df.Stats("vtx_x").GetValue();




}
