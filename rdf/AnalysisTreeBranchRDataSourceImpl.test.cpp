//
// Created by eugene on 18/09/2020.
//

#include <AnalysisTree/BranchConfig.hpp>

#include "AnalysisTreeBranchRDataSourceImpl.hpp"

#include <gtest/gtest.h>


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

  AnalysisTreeBranchRDataSourceImpl rds;
  rds.config_ = c;
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

