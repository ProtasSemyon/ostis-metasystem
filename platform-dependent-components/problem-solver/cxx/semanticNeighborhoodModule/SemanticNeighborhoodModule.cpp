#include "SemanticNeighborhoodModule.hpp"

#include "sc-memory/kpm/sc_agent.hpp"

#include "keynodes/Keynodes.hpp"
#include "agent/ObtainingSemanticNeighborhoodAgent.hpp"

using namespace semanticNeighborhood;

SC_IMPLEMENT_MODULE(SemanticNeighborhoodModule)

sc_result SemanticNeighborhoodModule::InitializeImpl()
{
  if (!SemanticNeighborhoodModule::InitGlobal())
  {
    SC_LOG_ERROR("SemanticNeighborhoodModule is deactivated");
    return SC_RESULT_ERROR;
  }
  if (!Keynodes::InitGlobal())
  {
    SC_LOG_ERROR("Keynodes is deactivated");
    return SC_RESULT_ERROR;
  }
  else 
  {
    SC_AGENT_REGISTER(ObtainingSemanticNeighborhoodAgent);
  }

  return SC_RESULT_OK;
}
sc_result SemanticNeighborhoodModule::ShutdownImpl()
{
  SC_AGENT_UNREGISTER(ObtainingSemanticNeighborhoodAgent);
  return SC_RESULT_OK;
}
