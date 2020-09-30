//
// Created by eugene on 30/09/2020.
//

#ifndef ANALYSISTREE_INFRA_ENTITYTRAITS_HPP
#define ANALYSISTREE_INFRA_ENTITYTRAITS_HPP

#include <AnalysisTree/BranchConfig.hpp>
#include <AnalysisTree/Detector.hpp>


namespace AnalysisTree {

namespace Details {

template<typename T>
struct EntityTraits {
  static constexpr bool is_channel_or_track = false;
  typedef T ChannelType;

  inline static size_t GetNChannels(const T& /* entity */) {
    return 1ul;
  }

  inline static T& GetChannel(T& entity, size_t i_channel) {
    if (i_channel == 0) {
      return entity;
    }
    throw std::out_of_range("Non-channel entity cannot have > 1 channels");
  }
};
template<typename T>
struct EntityTraits<Detector<T>> {
  static constexpr bool is_channel_or_track = true;
  typedef T ChannelType;

  inline static T& GetChannel(Detector<T>& det, size_t i_channel) {
    return det.GetChannel(i_channel);
  }

  inline static size_t GetNChannels(const Detector<T> &detector) {
    return detector.GetNumberOfChannels();
  }
};
}

}

#endif//ANALYSISTREE_INFRA_ENTITYTRAITS_HPP
