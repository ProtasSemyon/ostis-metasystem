/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "GlossaryModule.hpp"

#include "agent/CreateGlossaryAgent.hpp"
#include "keynodes/GlossaryKeynodes.hpp"
#include "sc-memory/kpm/sc_agent.hpp"

using namespace glossaryModule;

SC_IMPLEMENT_MODULE(GlossaryModule)

sc_result GlossaryModule::InitializeImpl()
{
  if (!GlossaryModule::InitGlobal())
  {
    SC_LOG_ERROR("GlossaryModule is deactivated");
    return SC_RESULT_ERROR;
  }
  if (!GlossaryKeynodes::InitGlobal()) 
  {
    SC_LOG_ERROR("GlossaryKeynodes is deactivated");
  }
  else
  {
    SC_AGENT_REGISTER(CreateGlossaryAgent)
  }

  return SC_RESULT_OK;
}
sc_result GlossaryModule::ShutdownImpl()
{
  SC_AGENT_UNREGISTER(CreateGlossaryAgent)
  return SC_RESULT_OK;
}
