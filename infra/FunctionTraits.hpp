//
// Created by eugene on 19/10/2020.
//

#ifndef ANALYSISTREE_INFRA_FUNCTIONTRAITS_HPP
#define ANALYSISTREE_INFRA_FUNCTIONTRAITS_HPP

#include "EntityTraits.hpp"
#include <AnalysisTree/BranchConfig.hpp>
#include <AnalysisTree/Detector.hpp>
#include <Rtypes.h>
#include <TTree.h>
#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
namespace AnalysisTree::Details {

template<typename T>
struct FunctionTraits : FunctionTraits<decltype(&T::operator())> {};
template<typename R, typename... Args>
struct FunctionTraits<R (*)(Args...)> {
  constexpr static size_t Arity = sizeof...(Args);
  typedef R ret_type;
};
template<typename R, typename C, typename... Args>
struct FunctionTraits<R (C::*)(Args...)> {
  constexpr static size_t Arity = sizeof...(Args);
  typedef R ret_type;
};
template<typename R, typename C, typename... Args>
struct FunctionTraits<R (C::*)(Args...) const> {
  constexpr static size_t Arity = sizeof...(Args);
  typedef R ret_type;
};

}

#endif//ANALYSISTREE_INFRA_FUNCTIONTRAITS_HPP
