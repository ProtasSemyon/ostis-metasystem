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
    ScAddr const & actionNode = otherAddr;

    if (!checkAction(actionNode))
        return SC_RESULT_OK;

    SC_LOG_DEBUG("CreateGlossaryAgent started");

    ScAddrVector subjectDomains = 
        IteratorUtils::getAllWithType(
            &m_memoryCtx,
            IteratorUtils::getAnyByOutRelation(
                &m_memoryCtx, 
                actionNode, 
                scAgentsCommon::CoreKeynodes::rrel_1
            ),
            ScType::NodeConstStruct
        );

    ScAddr outputLangAddr =
        IteratorUtils::getAnyByOutRelation(
            &m_memoryCtx,
            actionNode, 
            scAgentsCommon::CoreKeynodes::rrel_2
        );

    ScAddrVector parameters = 
        getValidOptions(
        IteratorUtils::getAnyByOutRelation(
            &m_memoryCtx,
            actionNode, 
            scAgentsCommon::CoreKeynodes::rrel_3
            )
        );

    AgentUtils::finishAgentWork(&m_memoryCtx, actionNode, true);
    SC_LOG_DEBUG("CreateGlossaryAgent finished");

    return SC_RESULT_OK;
}

bool CreateGlossaryAgent::checkAction(ScAddr const & actionNode) 
{
    return m_memoryCtx.HelperCheckEdge(
            GlossaryKeynodes::action_create_glossary,
            actionNode,
            ScType::EdgeAccessConstPosPerm
            );
}

ScAddrVector CreateGlossaryAgent::getValidOptions(ScAddr const & parametersSetAddr) 
{
    ScAddrVector resultOptions;
    ScAddrVector parametersSetElements = IteratorUtils::getAllWithType(
        &m_memoryCtx,
        parametersSetAddr, 
        ScType::NodeConst
    );
    
    for (ScAddr const & currentParameter : parametersSetElements) 
    {
        bool isParameterValid = m_memoryCtx.HelperCheckEdge(
            GlossaryKeynodes::set_of_valid_glossary_parameters,
            currentParameter,
            ScType::EdgeAccessConstPosPerm);

        if (isParameterValid)
        {
            resultOptions.emplace_back(currentParameter);
        }
        else 
        { 
            auto idtf = m_memoryCtx.HelperGetSystemIdtf(currentParameter);
            SC_LOG_WARNING(idtf + " isn't valid parameter for CreateGlossaryAgent");
        }
    }

    return resultOptions;
}

