#pragma once

#include "sc-memory/kpm/sc_agent.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "CreateGlossaryAgent.generated.hpp"

namespace glossaryModule
{
class CreateGlossaryAgent : public ScAgent
{
  SC_CLASS(Agent, Event(scAgentsCommon::CoreKeynodes::question_initiated, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

  private:
    bool checkAction(ScAddr const & questionNode);

    ScAddrVector getValidSetElementsByTypeAndValidSet(
      ScAddr const & set, 
      ScType type, 
      ScAddr const & validSet, 
      std::string warnMessage);

    ScAddrVector getConceptsFromSubjDomain(ScAddr const & subjDomain);

    void getConceptsFromSubjDomainByRelationType(
      ScAddr const & subjDomain, 
      ScAddr const & relationType,
      ScAddrVector & output);

    ScAddr formScResultNode(ScAddr const & setOfSubjDomains, ScAddrVector & answerElements);

    ScAddr formScnTexResultNode(ScAddr const & setOfSubjDomains, ScAddrVector & answerElements);

    void formGlossary(ScAddr const & setOfSubjDomains, ScAddr const & params, ScAddrVector & answerElements);

    void clearPreviousResultNode(ScAddr const & setOfSubjDomains, ScType const & nodeType);

    std::string formatToLog(std::string const & message);

    std::string getMainIdtf(ScAddr const & concept);

    sc_result exitInvalidParams(std::string const & message, ScAddr const & questionNode);

    static ScAddr answerLang;
};



}  // namespace glossaryModule
