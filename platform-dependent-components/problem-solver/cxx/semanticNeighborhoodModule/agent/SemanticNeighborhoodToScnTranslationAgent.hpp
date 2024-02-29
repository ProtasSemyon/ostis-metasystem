#pragma once

#include "sc-memory/kpm/sc_agent.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "SemanticNeighborhoodToScnTranslationAgent.generated.hpp"

namespace semanticNeighborhood
{
class SemanticNeighborhoodToScnTranslationAgent : public ScAgent
{
  SC_CLASS(Agent, Event(scAgentsCommon::CoreKeynodes::question_initiated, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

  private:
    using FunctionsByParameterMap = std::map<ScAddr, 
      std::function<std::string(
        ScMemoryContext * /*ms_context*/,
        ScAddr const & /*concept*/,
        ScAddr const & /*parameter*/,
        int /*indentLevel*/
      )>, ScAddrLessFunc>;

    bool checkAction(ScAddr const & questionNode);

    sc_result exitInvalidParams(std::string const & message, ScAddr const & questionNode);

    static std::string formatToLog(std::string const & message);

    static std::string formatRelation(std::string const & relIdtf);

    static std::string formatToTex(
        std::string const & keyword, 
        std::string const & content,
        int indentLevel = 0);

    static std::string formatToTex(
        std::string const & keyword, 
        std::string const & parameter, 
        std::string const & content,
        int indentLevel);


    static ScAddr outputLang;

    std::string obtainSemanticNeighborhoodByParameter(
      ScAddr const & concept, 
      ScAddr const & parameter,
      int indentLevel);

    static std::string processIdtfParameter(
      ScMemoryContext * ms_context,
      ScAddr const & concept,
      ScAddr const & parameter,
      int indentLevel);

    static std::string processEntityParameter(
      ScMemoryContext * ms_context,
      ScAddr const & concept,
      ScAddr const & parameter,
      int indentLevel);

    static std::string processSubdividingParameter(
      ScMemoryContext * ms_context,
      ScAddr const & concept,
      ScAddr const & parameter,
      int indentLevel);

    static std::string processSubjDomainParameter(
      ScMemoryContext * ms_context,
      ScAddr const & concept,
      ScAddr const & parameter,
      int indentLevel);

    static std::string processOutNoroleRelationParameter(
      ScMemoryContext * ms_context,
      ScAddr const & concept,
      ScAddr const & parameter,
      int indentLevel);
    
};

}  // namespace semanticNeighborhood
