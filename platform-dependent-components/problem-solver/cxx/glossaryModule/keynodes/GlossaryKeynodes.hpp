#pragma once

#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_defines.hpp"
#include "sc-memory/sc_object.hpp"

#include "GlossaryKeynodes.generated.hpp"

namespace glossaryModule
{
class GlossaryKeynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:
  SC_PROPERTY(Keynode("action_create_glossary"), ForceCreate)
  static ScAddr action_create_glossary;

  SC_PROPERTY(Keynode("action_get_semantic_neighborhood"), ForceCreate)
  static ScAddr action_get_semantic_neighborhood;

  SC_PROPERTY(Keynode("action_translate_semantic_neighborhood_to_scn"), ForceCreate)
  static ScAddr action_translate_semantic_neighborhood_to_scn;

  SC_PROPERTY(Keynode("default_glossary_parameters"), ForceCreate)
  static ScAddr default_glossary_parameters;

  SC_PROPERTY(Keynode("subject_domain"), ForceCreate)
  static ScAddr subject_domain;

  SC_PROPERTY(Keynode("nrel_glossary"), ForceCreate)
  static ScAddr nrel_glossary;

  SC_PROPERTY(Keynode("rrel_maximum_studied_object_class"), ForceCreate)
  static ScAddr rrel_maximum_studied_object_class;

  SC_PROPERTY(Keynode("rrel_explored_relation"), ForceCreate)
  static ScAddr rrel_explored_relation;

  SC_PROPERTY(Keynode("rrel_not_maximum_studied_object_class"), ForceCreate)
  static ScAddr rrel_not_maximum_studied_object_class;

  SC_PROPERTY(Keynode("format_html"), ForceCreate)
  static ScAddr format_html;

  SC_PROPERTY(Keynode("nrel_format"), ForceCreate)
  static ScAddr nrel_format;

  SC_PROPERTY(Keynode("rrel_example"), ForceCreate)
  static ScAddr rrel_example;

};

}  // namespace glossaryModule
