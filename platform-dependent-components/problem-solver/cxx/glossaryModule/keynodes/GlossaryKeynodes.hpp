/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_addr.hpp"
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

  SC_PROPERTY(Keynode("set_of_valid_glossary_parameters"))
  static ScAddr set_of_valid_glossary_parameters;

};

}  // namespace glossaryModule
