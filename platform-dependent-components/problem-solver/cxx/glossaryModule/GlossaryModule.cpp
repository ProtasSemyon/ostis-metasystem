/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "GlossaryModule.hpp"

#include "keynodes/GlossaryKeynodes.hpp"

using namespace glossaryModule;

SC_IMPLEMENT_MODULE(GlossaryModule)

sc_result GlossaryModule::InitializeImpl()
{
  if (!GlossaryModule::InitGlobal())
  {
    SC_LOG_ERROR("GlossaryModule is deactivated");
    return SC_RESULT_ERROR;
  }

  return SC_RESULT_OK;
}
sc_result GlossaryModule::ShutdownImpl()
{
  return SC_RESULT_OK;
}
