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

template<typename Entity, typename... Fields>
struct EntityStaticInfo {
  virtual ~EntityStaticInfo() = default;
  virtual std::string GetEntityName() const = 0;

  //  template<typename Field>
  //  typename Field::field_type GetValue(typename Field::parent_entity_const_ref_type entity_ref) { return 0; }
  // Get item from tuple by type is not supported in std11 (since std14) :(

  std::tuple<Fields...> fields;
};

}// namespace AnalysisTree

#define AT_ENTITY_BEGIN(ENTITY_CLASS)                              \
 public:                                                           \
  static inline std::string EntityName() { return #ENTITY_CLASS; } \
  typedef ENTITY_CLASS entity_type;

#define AT_FIELD_GETTER_NAME(FIELD_NAME) Get##FIELD_NAME
#define AT_FIELD_SETTER_NAME(FIELD_NAME) Set##FIELD_NAME

#define AT_FIELD_DEFAULT_ACCESSORS(FIELD_NAME, FIELD_TYPE)                      \
 private:                                                                       \
  FIELD_TYPE f##FIELD_NAME;                                                     \
                                                                                \
 public:                                                                        \
  FIELD_TYPE AT_FIELD_GETTER_NAME(FIELD_NAME)() const { return f##FIELD_NAME; } \
  void AT_FIELD_SETTER_NAME(FIELD_NAME)(FIELD_TYPE new_value) { (f##FIELD_NAME) = new_value; }

#define AT_FIELD(FIELD_NAME, FIELD_ID, FIELD_TYPE)                                                                      \
  AT_FIELD_DEFAULT_ACCESSORS(FIELD_NAME, FIELD_TYPE)                                                                    \
 public:                                                                                                                \
  struct FIELD_NAME : public FieldStaticInfo {                                                                          \
    typedef FIELD_TYPE field_type;                                                                                      \
    typedef entity_type parent_entity_type;                                                                             \
    typedef const entity_type& parent_entity_const_ref_type;                                                            \
    enum { field_id = (FIELD_ID) };                                                                                     \
    int GetFieldId() const final { return (FIELD_ID); }                                                                 \
    std::string GetFieldName() const final { return #FIELD_NAME; }                                                      \
    static inline field_type GetValue(parent_entity_const_ref_type entity) { return entity.AT_FIELD_GETTER_NAME(FIELD_NAME)(); }                \
    static inline void SetValue(parent_entity_type& entity, field_type new_value) { entity.AT_FIELD_SETTER_NAME(FIELD_NAME)(new_value); } \
  };

#define AT_TRANSIENT_FIELD(FIELD_NAME, FIELD_ID, FUNCTION)

#define AT_DECLARE_ENTITY(NAME, ENTITY, ...)                       \
  struct StaticInfo : public EntityStaticInfo<NAME, __VA_ARGS__> { \
    std::string GetEntityName() const final { return #NAME; };     \
  };

#endif//ANALYSISTREE_CORE_ENTITYSTATICINFO_HPP
