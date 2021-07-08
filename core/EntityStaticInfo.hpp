//
// Created by eugene on 08/07/2021.
//

#ifndef ANALYSISTREE_CORE_ENTITYSTATICINFO_HPP
#define ANALYSISTREE_CORE_ENTITYSTATICINFO_HPP

#include "BranchConfig.hpp"

namespace AnalysisTree {

struct FieldStaticInfo {
  virtual ~FieldStaticInfo() = default;
  virtual int GetFieldId() const = 0;
  virtual std::string GetFieldName() const = 0;
};

template<typename Entity, typename... Field>
struct EntityStaticInfo {
  virtual ~EntityStaticInfo() = default;
  virtual std::string GetEntityName() const = 0;

  std::tuple<Field...> fields;
};



}// namespace AnalysisTree

#define AT_DECLARE_FIELD_INCLASS_DATA(FIELD_NAME, FIELD_TYPE, GETTER_NAME, SETTER_NAME) \
  FIELD_TYPE FIELD_NAME;                                             \
  FIELD_TYPE GETTER_NAME() const { return FIELD_NAME; }                                 \
  void SETTER_NAME(FIELD_TYPE new_value) { (FIELD_NAME) = new_value; }

#define AT_DECLARE_FIELD(FIELD_NAME, FIELD_ID, FIELD_TYPE)         \
  struct FIELD_NAME : public FieldStaticInfo {                     \
    typedef FIELD_TYPE field_type;                                 \
    int GetFieldId() const final { return FIELD_ID; }              \
    std::string GetFieldName() const final { return #FIELD_NAME; } \
  };                                                               \
  AT_DECLARE_FIELD_INCLASS_DATA(f##FIELD_NAME, FIELD_TYPE, Get##FIELD_NAME, Set##FIELD_NAME)

#define AT_DECLARE_ENTITY(NAME, ENTITY, ...)                          \
  struct StaticInfo : public EntityStaticInfo<NAME, __VA_ARGS__> { \
    std::string GetEntityName() const final { return #NAME; };        \
  };

#endif//ANALYSISTREE_CORE_ENTITYSTATICINFO_HPP
