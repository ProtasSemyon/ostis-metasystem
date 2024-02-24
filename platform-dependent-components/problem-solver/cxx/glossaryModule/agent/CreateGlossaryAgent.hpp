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
    using ProcessConceptMapSc = 
      std::map<ScAddr, 
        std::function<void(
          ScMemoryContext * , 
          ScAddr const & , 
          ScAddr const &, 
          ScAddr const &, 
          ScAddrVector&)
        >,
        ScAddrLessFunc
      >;
    
    using ProcessConceptMapScnTex =
      std::map<ScAddr,
        std::function<std::string(
          ScMemoryContext * ,
          ScAddr const &,
          ScAddr const &,
          int)
        >,
        ScAddrLessFunc
      >;

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

    //SC processing start

    void formGlossaryScFormat(
      ScAddr const & setOfSubjDomains, 
      ScAddrVector const & subjectDomains, 
      ScAddrVector const & parameters, 
      ScAddrVector & answer);

    ScAddr formScResultNode(ScAddr const & setOfSubjDomains, ScAddrVector & answerElements);

    void clearPreviousResultNode(ScAddr const & setOfSubjDomains, ScType const & resultNodeType);

    void processConceptByParametersSc(
      ScAddr const & concept, 
      ScAddrVector const & parameters, 
      ScAddr const & resultStructure,
      ScAddrVector & answer);

    void processConceptByParameterSc(
      ScAddr const & concept, 
      ScAddr const & parameter, 
      ScAddr const & resultStructure,
      ScAddrVector & answer);

    static void processConceptIdtfSc(
      ScMemoryContext * ms_context,
      ScAddr const & concept, 
      ScAddr const & parameter, 
      ScAddr const & resultStructure,
      ScAddrVector & answer);

    static void processConceptEntitySc(
      ScMemoryContext * ms_context,
      ScAddr const & concept, 
      ScAddr const & parameter, 
      ScAddr const & resultStructure,
      ScAddrVector & answer);

    //SC processing end

    //SCn processing start
    using ScnTexConceptsMap = std::multimap<std::string, std::string>;

    ScAddr formScnTexResultNode(ScAddr const & setOfSubjDomains, ScAddrVector & answerElements);

    void formGlossaryScnTexFormat(
      ScAddr const & setOfSubjDomains, 
      ScAddrVector const & subjectDomains, 
      ScAddrVector const & parameters,
      ScAddrVector & answer);

    void processConceptByParametersScnTex(
      ScAddr const & concept, 
      ScAddrVector const & parameters, 
      ScnTexConceptsMap & conceptsMap,
      int indentLevel);
    
    std::string processConceptByParameterScnTex(
      ScAddr const & concept, 
      ScAddr const & parameter,
      int indentLevel);

    static std::string processConceptIdtfScnTex(
      ScMemoryContext * ms_context,
      ScAddr const & concept, 
      ScAddr const & parameter,
      int indentLevel);

    static std::string processConceptEntityScnTex(
      ScMemoryContext * ms_context,
      ScAddr const & concept,
      ScAddr const & parameter,
      int indentLevel);

    static std::string getMainIdtf(ScMemoryContext * ms_context, ScAddr const & concept);

    //SCn processing end

    std::string formatToLog(std::string const & message);

    sc_result exitInvalidParams(std::string const & message, ScAddr const & questionNode);

    static void makeDefinitionTemplate(ScAddr const & concept, ScAddr const & parameter, ScTemplate & templ);

    static ScAddr answerLang;

    static std::vector<std::string> idtfsVarForAdd;

    static std::string entityVarIdtf;
};

}  // namespace glossaryModule
