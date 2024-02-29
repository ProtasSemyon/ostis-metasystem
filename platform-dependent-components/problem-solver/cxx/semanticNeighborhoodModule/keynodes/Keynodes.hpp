#pragma once

#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_defines.hpp"
#include "sc-memory/sc_object.hpp"

#include "Keynodes.generated.hpp"

namespace semanticNeighborhood
{
class Keynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:
  SC_PROPERTY(Keynode("action_get_semantic_neighborhood"), ForceCreate)
  static ScAddr action_get_semantic_neighborhood;

  SC_PROPERTY(Keynode("action_translate_semantic_neighborhood_to_scn"), ForceCreate)
  static ScAddr action_translate_semantic_neighborhood_to_scn;

  SC_PROPERTY(Keynode("rrel_example"), ForceCreate)
  static ScAddr rrel_example;

  SC_PROPERTY(Keynode("definition"), ForceCreate)
  static ScAddr definition;

  SC_PROPERTY(Keynode("explanation"), ForceCreate)
  static ScAddr explanation;

  SC_PROPERTY(Keynode("note"), ForceCreate)
  static ScAddr note;

  SC_PROPERTY(Keynode("nrel_subdividing"), ForceCreate)
  static ScAddr nrel_subdividing;

  SC_PROPERTY(Keynode("subject_domain"), ForceCreate)
  static ScAddr subject_domain;

  SC_PROPERTY(Keynode("nrel_strict_inclusion"), ForceCreate)
  static ScAddr nrel_strict_inclusion;

  SC_PROPERTY(Keynode("nrel_authors"), ForceCreate)
  static ScAddr nrel_authors;
};

} // namespace semanticNeighborhood