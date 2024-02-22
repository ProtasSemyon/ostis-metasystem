/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "GlossaryModule.hpp"

#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "sc-agents-common/utils/CommonUtils.hpp"
#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "keynodes/GlossaryKeynodes.hpp"
#include "CreateGlossaryAgent.hpp"

#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_template.hpp"
#include "sc-memory/sc_type.hpp"
#include "sc-memory/utils/sc_log.hpp"

using namespace utils;

using namespace glossaryModule;

SC_AGENT_IMPLEMENTATION(CreateGlossaryAgent)
{
    ScAddr const & questionNode = otherAddr;

    if (!checkAction(questionNode))
        return SC_RESULT_OK;

    SC_LOG_DEBUG("CreateGlossaryAgent started");

    //Initialization
    ScAddr subjectDomainsSetAddr = 
        IteratorUtils::getAnyByOutRelation(
                &m_memoryCtx,
                questionNode, 
                scAgentsCommon::CoreKeynodes::rrel_1
            );

    ScAddr langAddr = 
        IteratorUtils::getAnyByOutRelation(
            &m_memoryCtx,
            questionNode, 
            scAgentsCommon::CoreKeynodes::rrel_2
        );

    ScAddr parametersSetAddr = 
        IteratorUtils::getAnyByOutRelation(
                &m_memoryCtx,
                questionNode, 
                scAgentsCommon::CoreKeynodes::rrel_3
            );
    
    //Validation
    if (!m_memoryCtx.IsElement(subjectDomainsSetAddr))
    {
        return exitInvalidParams("subject domains set node not found.", questionNode);
    }
    
    if (!m_memoryCtx.IsElement(langAddr))
    {
        return exitInvalidParams("language node not found..", questionNode);
    }

    if (!m_memoryCtx.IsElement(parametersSetAddr))
    {
        parametersSetAddr = GlossaryKeynodes::default_glossary_parameters;
        SC_LOG_WARNING(formatToLog(
            "parameters set node not found. By default, "
            + m_memoryCtx.HelperGetSystemIdtf(parametersSetAddr) + " is used."));
    }

    //Sets elements validation
    ScAddrVector subjectDomains = 
        getValidSetElementsByTypeAndValidSet(
            subjectDomainsSetAddr, 
            ScType::NodeConstStruct, 
            GlossaryKeynodes::subject_domain, 
            "is not a subject domain"
        );        
    
    ScAddrVector parameters = 
        getValidSetElementsByTypeAndValidSet(
            parametersSetAddr, 
            ScType::NodeConst, 
            GlossaryKeynodes::valid_glossary_parameters, 
            "is not a valid parameter"
        );

    AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
    SC_LOG_DEBUG("CreateGlossaryAgent finished");

    return SC_RESULT_OK;
}

bool CreateGlossaryAgent::checkAction(ScAddr const & questionNode) 
{
    return m_memoryCtx.HelperCheckEdge(
            GlossaryKeynodes::action_create_glossary,
            questionNode,
            ScType::EdgeAccessConstPosPerm
            );
}
ScAddrVector CreateGlossaryAgent::getValidSetElementsByTypeAndValidSet(
    ScAddr const & set, 
    ScType type, 
    ScAddr const & validSet, 
    std::string warnMessage
    ) 
{
    ScAddrVector validSetElements;
    ScAddrVector setElements =
        IteratorUtils::getAllWithType(
            &m_memoryCtx,
            set,
            type
        );

    for (ScAddr const & currentSetElement : setElements) 
    {
        bool isParameterValid = 
            m_memoryCtx.HelperCheckEdge(
                validSet,
                currentSetElement,
                ScType::EdgeAccessConstPosPerm
            );

        if (isParameterValid)
        {
            validSetElements.emplace_back(currentSetElement);
        }
        else 
        { 
            SC_LOG_WARNING(formatToLog(m_memoryCtx.HelperGetSystemIdtf(currentSetElement) + " " + warnMessage));
        }
    }

    return validSetElements;
}

std::string CreateGlossaryAgent::formatToLog(std::string const & message)
{
    return "CreateGlossaryAgent: " + message;
}

sc_result CreateGlossaryAgent::exitInvalidParams(std::string const & message, ScAddr const & questionNode) 
{
    SC_LOG_ERROR(formatToLog(message));
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, false);
    return SC_RESULT_ERROR_INVALID_PARAMS;
}




