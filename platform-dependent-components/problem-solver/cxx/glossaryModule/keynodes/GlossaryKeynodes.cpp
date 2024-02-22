/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "GlossaryKeynodes.hpp"
#include "GlossaryKeynodes.hpp"

#include "sc-memory/sc_memory.hpp"

namespace glossaryModule
{
  ScAddr GlossaryKeynodes::action_create_glossary;

  ScAddr GlossaryKeynodes::valid_glossary_parameters;

  ScAddr GlossaryKeynodes::default_glossary_parameters;

  ScAddr GlossaryKeynodes::subject_domain;

  ScAddr GlossaryKeynodes::nrel_glossary;

  //Valid glossary parameters
  ScAddr GlossaryKeynodes::nrel_main_idtf;

  ScAddr GlossaryKeynodes::nrel_idtf;
}  // namespace glossaryModule
