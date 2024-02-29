#pragma once

#include "sc-memory/kpm/sc_agent.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "ObtainingSemanticNeighborhoodAgent.generated.hpp"

namespace semanticNeighborhood
{
class ObtainingSemanticNeighborhoodAgent : public ScAgent
{
  SC_CLASS(Agent, Event(scAgentsCommon::CoreKeynodes::question_initiated, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

  private:
    using FunctionsByParameterMap = std::map<ScAddr, 
      std::function<void(
        ScMemoryContext * /*ms_context*/,
        ScAddr const & /*concept*/,
        ScAddr const & /*parameter*/,
        ScAddrVector & /*answer*/
      )>, ScAddrLessFunc>;

    bool checkAction(ScAddr const & questionNode);

    sc_result exitInvalidParams(std::string const & message, ScAddr const & questionNode);

    std::string formatToLog(std::string const & message);

    static ScAddr outputLang;

    void obtainSemanticNeighborhoodByParameter(
      ScAddr const & concept, 
      ScAddr const & parameter, 
      ScAddrVector & answer
    );

    static void processIdtfParameter(
      ScMemoryContext * ms_context,
      ScAddr const & concept,
      ScAddr const & parameter,
      ScAddrVector & answer
    );

    static void processEntityParameter(
      ScMemoryContext * ms_context,
      ScAddr const & concept,
      ScAddr const & parameter,
      ScAddrVector & answer
    );

    static void processSubdividingParameter(
      ScMemoryContext * ms_context,
      ScAddr const & concept,
      ScAddr const & parameter,
      ScAddrVector & answer
    );

    static void processSubjDomainParameter(
      ScMemoryContext * ms_context,
      ScAddr const & concept,
      ScAddr const & parameter,
      ScAddrVector & answer
    );

    static void processOutNoroleRelationParameter(
      ScMemoryContext * ms_context,
      ScAddr const & concept,
      ScAddr const & parameter,
      ScAddrVector & answer
    );
    
};

}  // namespace semanticNeighborhood
