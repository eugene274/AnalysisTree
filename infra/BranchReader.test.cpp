//
// Created by eugene on 30/09/2020.
//

#include <gtest/gtest.h>

#include <AnalysisTree/BranchConfig.hpp>
#include <AnalysisTree/EventHeader.hpp>
#include "BranchReader.hpp"

namespace {

using namespace AnalysisTree;
using namespace AnalysisTree::Experimental;


TEST(BranchReaderT, Basic) {

  BranchConfig config("test", DetType::kEventHeader);

  auto event_header = new AnalysisTree::EventHeader;
  event_header->Init(config);

  BranchReaderT<AnalysisTree::EventHeader> event_header_reader(config, event_header);

  auto vtx_x = event_header_reader.GetVariable("vtx_x");
  vtx_x->GetValue(0);

}


}
