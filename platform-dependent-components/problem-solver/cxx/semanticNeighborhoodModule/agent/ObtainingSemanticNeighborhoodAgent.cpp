#include "keynodes/Keynodes.hpp"
#include "ObtainingSemanticNeighborhoodAgent.hpp"

#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "utils/Utils.hpp"

using namespace utils;

using namespace semanticNeighborhood;

ScAddr ObtainingSemanticNeighborhoodAgent::outputLang;

SC_AGENT_IMPLEMENTATION(ObtainingSemanticNeighborhoodAgent)
{
    ScAddr const & questionNode = otherAddr;

    if (!checkAction(questionNode))
        return SC_RESULT_OK;

    SC_LOG_DEBUG("ObtainingSemanticNeighborhoodAgent started");

    //Initialization
    ScAddr conceptAddr = IteratorUtils::getAnyByOutRelation(
        &m_memoryCtx, 
        questionNode, 
        scAgentsCommon::CoreKeynodes::rrel_1);
    
    ScAddr parametersAddr = IteratorUtils::getAnyByOutRelation(
        &m_memoryCtx, 
        questionNode, 
        scAgentsCommon::CoreKeynodes::rrel_2);

    ScAddr langAddr = IteratorUtils::getAnyByOutRelation(
        &m_memoryCtx, 
        questionNode, 
        scAgentsCommon::CoreKeynodes::rrel_3);

    //Validation
    if (!m_memoryCtx.IsElement(conceptAddr))
        return exitInvalidParams("concept node not found.", questionNode);

    if (!m_memoryCtx.IsElement(parametersAddr))
        return exitInvalidParams("parameters node not found.", questionNode);

    if (!m_memoryCtx.IsElement(langAddr))
        return exitInvalidParams("language node not found.", questionNode);
    else
        outputLang = langAddr;

    // Output generation
    ScAddrVector answer;

    ScAddr currentParamAddr = IteratorUtils::getAnyByOutRelation(
        &m_memoryCtx, 
        parametersAddr, 
        scAgentsCommon::CoreKeynodes::rrel_1);

    do {
        obtainSemanticNeighborhoodByParameter(conceptAddr, currentParamAddr, answer);
        currentParamAddr = IteratorUtils::getNextFromSet(&m_memoryCtx, parametersAddr, currentParamAddr); 
    } while (currentParamAddr.IsValid());

    AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, answer, true);

    SC_LOG_DEBUG("ObtainingSemanticNeighborhoodAgent finished");

    return SC_RESULT_OK;
};

bool ObtainingSemanticNeighborhoodAgent::checkAction(ScAddr const & questionNode) 
{
    return m_memoryCtx.HelperCheckEdge(
        Keynodes::action_get_semantic_neighborhood,
        questionNode,
        ScType::EdgeAccessConstPosPerm
        );
}

sc_result ObtainingSemanticNeighborhoodAgent::exitInvalidParams(std::string const & message, ScAddr const & questionNode)
{
    SC_LOG_ERROR(formatToLog(message));
    AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, false);
    return SC_RESULT_ERROR_INVALID_PARAMS;
}

std::string ObtainingSemanticNeighborhoodAgent::formatToLog(std::string const & message) {
    return "ObtainingSemanticNeighborhoodAgent: " + message;
}

void ObtainingSemanticNeighborhoodAgent::obtainSemanticNeighborhoodByParameter(
    ScAddr const & concept, 
    ScAddr const & parameter, 
    ScAddrVector & answer
)
{
    static const FunctionsByParameterMap functionsByParameterMap = {
        {scAgentsCommon::CoreKeynodes::nrel_system_identifier, processIdtfParameter},
        {scAgentsCommon::CoreKeynodes::nrel_idtf, processIdtfParameter},
        {scAgentsCommon::CoreKeynodes::nrel_main_idtf, processIdtfParameter},
        {Keynodes::definition, processEntityParameter},
        {Keynodes::explanation, processEntityParameter},
        {Keynodes::note, processEntityParameter},
    };

    if (functionsByParameterMap.find(parameter) == functionsByParameterMap.end()) {
        SC_LOG_WARNING(formatToLog("processing of parameter '"
            + m_memoryCtx.HelperGetSystemIdtf(parameter))
            + "' not implemented.");
        return;
    }

    functionsByParameterMap.at(parameter)(&m_memoryCtx, concept, parameter, answer);
}

void ObtainingSemanticNeighborhoodAgent::processIdtfParameter (
    ScMemoryContext * ms_context,
    ScAddr const & concept, 
    ScAddr const & parameter, 
    ScAddrVector & answer
)
{
    ScIterator5Ptr idtfIterator = ms_context->Iterator5(
        concept, 
        ScType::EdgeDCommonConst, 
        ScType::Unknown, 
        ScType::EdgeAccessConstPosPerm, 
        parameter);

    if (!idtfIterator->IsValid())
        return;

    while (idtfIterator->Next())
    {
        bool isLangCorrect = ms_context->HelperCheckEdge(
            outputLang, 
            idtfIterator->Get(2), 
            ScType::EdgeAccessConstPosPerm
        );

        if (!isLangCorrect && parameter != scAgentsCommon::CoreKeynodes::nrel_system_identifier)
            continue;

        for (size_t addrIndexInIterator = 1; addrIndexInIterator < 4; addrIndexInIterator++)
        {
            ScAddr addrForAdd = idtfIterator->Get(addrIndexInIterator);
            answer.insert(answer.end(), {addrForAdd});
        }
    }
}

void ObtainingSemanticNeighborhoodAgent::processEntityParameter(
    ScMemoryContext * ms_context,
    ScAddr const & concept,
    ScAddr const & parameter,
    ScAddrVector & answer
)
{
    ScTemplate templ;
    Utils::makeConceptEntityTemplate(concept, parameter, outputLang, templ);

    ScTemplateSearchResult result;
    ms_context->HelperSearchTemplate(templ, result);

    if (result.IsEmpty())
        return;

    answer.insert(answer.end(), {
        parameter,
        scAgentsCommon::CoreKeynodes::rrel_key_sc_element,
        scAgentsCommon::CoreKeynodes::nrel_sc_text_translation,
        Keynodes::rrel_example
    });

    for (size_t currentTemplResult = 0;currentTemplResult < result.Size(); currentTemplResult++)
    {
        for (auto const & currentVarName: Utils::conceptEntityTemplateVarNames)
        {
            answer.emplace_back(result[currentTemplResult][currentVarName]);
        }
    }
}



