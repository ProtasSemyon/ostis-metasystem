/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

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

  SC_PROPERTY(Keynode("valid_glossary_parameters"), ForceCreate)
  static ScAddr valid_glossary_parameters;

  SC_PROPERTY(Keynode("default_glossary_parameters"), ForceCreate)
  static ScAddr default_glossary_parameters;

  SC_PROPERTY(Keynode("subject_domain"), ForceCreate)
  static ScAddr subject_domain;

  //Valid glossary parameters
  SC_PROPERTY(Keynode("nrel_main_idtf"), ForceCreate)
  static ScAddr nrel_main_idtf;

  SC_PROPERTY(Keynode("nrel_idtf"), ForceCreate)
  static ScAddr nrel_idtf;

};

}  // namespace glossaryModule
