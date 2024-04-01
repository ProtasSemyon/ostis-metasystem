/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-agents-common/utils/CommonUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "sc-agents-common/utils/AgentUtils.hpp"

#include "keynodes/Keynodes.hpp"
#include "constants/Constants.hpp"

#include "GenerateResponseAgent.hpp"

using namespace generateResponseModule;

SC_AGENT_IMPLEMENTATION(GenerateResponseAgent)
{
  ScAddr const & actionAddr = otherAddr;

  if (!checkAction(actionAddr))
  {
    return SC_RESULT_OK;
  }

  SC_LOG_DEBUG(Constants::generateAnswerAgentClassName + " started");

  ScAddr messageAddr =
      utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, otherAddr, scAgentsCommon::CoreKeynodes::rrel_1);
  ScAddr answerAddr =
      utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, otherAddr, scAgentsCommon::CoreKeynodes::rrel_2);

  try
  {
    validateAddrWithInvalidParamException(messageAddr, Constants::messageAddrParamName);
    validateAddrWithInvalidParamException(answerAddr, Constants::answerAddrParamName);
  }
  catch (utils::ExceptionInvalidParams const & ex)
  {
    finishWorkWithMessage(ex.Message(), actionAddr, false);
    return SC_RESULT_ERROR;
  };

  ScAddr messageActionAddr;
  try
  {
    messageActionAddr = getMessageActionAddr(messageAddr);
  }
  catch (utils::ExceptionItemNotFound const & ex)
  {
    finishWorkWithMessage(ex.Message(), actionAddr, false);
    return SC_RESULT_ERROR;
  }

  ScAddr messageAnswer = utils::AgentUtils::applyActionAndGetResultIfExists(
      &m_memoryCtx, messageActionAddr, getMessageParameters(messageAddr));

  bool isSuccess = attachAnswer(messageAnswer, messageAddr, answerAddr);

  utils::AgentUtils::finishAgentWork(&m_memoryCtx, actionAddr, isSuccess);
  SC_LOG_DEBUG(Constants::generateAnswerAgentClassName + " finished");
  return SC_RESULT_OK;
}

ScAddr GenerateResponseAgent::getMessageActionAddr(ScAddr const & messageAddr)
{
  ScAddr messageClassAddr = getMessageClassAddr(messageAddr);
  validateAddrWithItemNotFoundException(messageClassAddr, "message class is not supported or not found");

  ScAddr messageActionAddr =
      utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, messageClassAddr, Keynodes::nrel_response_action);
  validateAddrWithItemNotFoundException(messageActionAddr, "action corresponding to message class not found");

  return messageActionAddr;
}

void GenerateResponseAgent::validateAddrWithItemNotFoundException(ScAddr const & addr, std::string const & message)
{
  if (addr.IsValid())
    return;

  SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, message);
}

void GenerateResponseAgent::validateAddrWithInvalidParamException(ScAddr const & addr, std::string const & paramName)
{
  SC_CHECK_PARAM(addr, "parameter '" + paramName + "' is not valid");
}

void GenerateResponseAgent::finishWorkWithMessage(
    std::string const & message,
    ScAddr const & questionNode,
    bool isSuccess)
{
  SC_LOG_ERROR(Constants::generateAnswerAgentClassName + ": " + message);
  utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, false);
}

bool GenerateResponseAgent::checkAction(ScAddr const & actionAddr)
{
  return m_memoryCtx.HelperCheckEdge(Keynodes::action_generate_response, actionAddr, ScType::EdgeAccessConstPosPerm);
}

ScAddr GenerateResponseAgent::getMessageClassAddr(ScAddr const & messageAddr)
{
  ScIterator3Ptr classIterator =
      m_memoryCtx.Iterator3(ScType::NodeConstClass, ScType::EdgeAccessConstPosPerm, messageAddr);

  while (classIterator->Next())
  {
    ScAddr messageClassAddr = classIterator->Get(0);
    bool isMessageClass = m_memoryCtx.HelperCheckEdge(
        Keynodes::concept_ask_ai_message_class, messageClassAddr, ScType::EdgeAccessConstPosPerm);
    if (isMessageClass)
      return messageClassAddr;
  }

  return {};
}

bool GenerateResponseAgent::attachAnswer(
    ScAddr const & messageAnswer,
    ScAddr const & messageAddr,
    ScAddr const & answerAddr)
{
  if (!messageAnswer.IsValid())
    return false;

  ScAddr edgeBetweenMessageAndMessageAnswer =
      m_memoryCtx.CreateEdge(ScType::EdgeDCommonConst, messageAddr, messageAnswer);
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::nrel_response, edgeBetweenMessageAndMessageAnswer);

  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosTemp, answerAddr, messageAnswer);

  return true;
}

ScAddrVector GenerateResponseAgent::getMessageParameters(ScAddr const & messageAddr)
{
  // TODO: implement receiving several parameters from a message
  ScAddr messageParameter = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, messageAddr, Keynodes::rrel_entity);

  ScAddrVector parameters;
  if (messageParameter.IsValid())
    parameters.emplace_back(messageParameter);

  return parameters;
}
