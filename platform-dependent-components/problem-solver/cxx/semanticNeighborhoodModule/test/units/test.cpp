#include "scs_loader.hpp"
#include "sc_test.hpp"
#include "sc-agents-common/utils/CommonUtils.hpp"
#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "keynodes/Keynodes.hpp"
#include "agent/ObtainingSemanticNeighborhoodAgent.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

using namespace semanticNeighborhood;

namespace ModuleTest
{
    ScsLoader loader;
    const std::string TEST_FILES_DIR_PATH = MODULE_TEST_SRC_PATH "/testStructures/";

    const int WAIT_TIME = 1000;

    using AgentTest = ScMemoryTest;

    void initialize()
    {
        scAgentsCommon::CoreKeynodes::InitGlobal();
        Keynodes::InitGlobal();
    }

    TEST_F(AgentTest, IdtfTest)
    {
        ScMemoryContext & context = *m_ctx;
        loader.loadScsFile(context, TEST_FILES_DIR_PATH + "test.scs");

        ScAddr const & testActionNode = context.HelperFindBySystemIdtf("test_action_node");
        ScAddr const & paramsNode = context.HelperFindBySystemIdtf("test_idtf_params");

        ScAgentInit(true);
        initialize();

        SC_AGENT_REGISTER(ObtainingSemanticNeighborhoodAgent);

        ScAddr accessArc = context.CreateEdge(ScType::EdgeAccessConstPosPerm, testActionNode, paramsNode);
        context.CreateEdge(ScType::EdgeAccessConstPosTemp, scAgentsCommon::CoreKeynodes::rrel_2, accessArc);

        context.CreateEdge(
            ScType::EdgeAccessConstPosPerm, 
            scAgentsCommon::CoreKeynodes::question_initiated, 
            testActionNode);

        utils::AgentUtils::applyAction(&context, testActionNode, WAIT_TIME);

        EXPECT_TRUE(context.HelperCheckEdge(
            scAgentsCommon::CoreKeynodes::question_finished_successfully, 
            testActionNode, 
            ScType::EdgeAccessConstPosPerm));
        
        ScAddr const & answerNode = utils::IteratorUtils::getAnyByOutRelation(&context, testActionNode, scAgentsCommon::CoreKeynodes::nrel_answer);

        EXPECT_TRUE(answerNode.IsValid());

        auto findLinkByContent = [&](std::string const & content){
            auto links = context.FindLinksByContent(content);
            if (links.empty())
                return ScAddr::Empty;
            return links.front();
        };

        ScAddr const & ru_mainIdtfLink = findLinkByContent("Тестовое понятие 1");
        ScAddr const & ru_idtfLink = findLinkByContent("Идентификатор 1");
        ScAddr const & en_mainIdtfLink = findLinkByContent("Test concept 1");

        EXPECT_TRUE(context.HelperCheckEdge(answerNode, ru_mainIdtfLink, ScType::EdgeAccessConstPosPerm));
        EXPECT_TRUE(context.HelperCheckEdge(answerNode, ru_idtfLink, ScType::EdgeAccessConstPosPerm));
        EXPECT_FALSE(context.HelperCheckEdge(answerNode, en_mainIdtfLink, ScType::EdgeAccessConstPosPerm));

        SC_AGENT_UNREGISTER(ObtainingSemanticNeighborhoodAgent);
    }

    TEST_F(AgentTest, descriptionTest)
    {
                ScMemoryContext & context = *m_ctx;
        loader.loadScsFile(context, TEST_FILES_DIR_PATH + "test.scs");

        ScAddr const & testActionNode = context.HelperFindBySystemIdtf("test_action_node");
        ScAddr const & paramsNode = context.HelperFindBySystemIdtf("test_description_params");

        ScAgentInit(true);
        initialize();

        SC_AGENT_REGISTER(ObtainingSemanticNeighborhoodAgent);

        ScAddr accessArc = context.CreateEdge(ScType::EdgeAccessConstPosPerm, testActionNode, paramsNode);
        context.CreateEdge(ScType::EdgeAccessConstPosTemp, scAgentsCommon::CoreKeynodes::rrel_2, accessArc);

        context.CreateEdge(
            ScType::EdgeAccessConstPosPerm, 
            scAgentsCommon::CoreKeynodes::question_initiated, 
            testActionNode);

        utils::AgentUtils::applyAction(&context, testActionNode, WAIT_TIME);

        EXPECT_TRUE(context.HelperCheckEdge(
            scAgentsCommon::CoreKeynodes::question_finished_successfully, 
            testActionNode, 
            ScType::EdgeAccessConstPosPerm));
        
        ScAddr const & answerNode = utils::IteratorUtils::getAnyByOutRelation(&context, testActionNode, scAgentsCommon::CoreKeynodes::nrel_answer);

        EXPECT_TRUE(answerNode.IsValid());

        auto findLinkByContent = [&](std::string const & content){
            auto links = context.FindLinksByContent(content);
            if (links.empty())
                return ScAddr::Empty;
            return links.front();
        };

        ScAddr const & ru_definitionLink = findLinkByContent("Пример определения");
        ScAddr const & ru_explanationLink = findLinkByContent("Пример пояснения");
        ScAddr const & ru_noteLink = findLinkByContent("Пример примечания");

        EXPECT_TRUE(context.HelperCheckEdge(answerNode, ru_definitionLink, ScType::EdgeAccessConstPosPerm));
        EXPECT_TRUE(context.HelperCheckEdge(answerNode, ru_explanationLink, ScType::EdgeAccessConstPosPerm));
        EXPECT_TRUE(context.HelperCheckEdge(answerNode, ru_noteLink, ScType::EdgeAccessConstPosPerm));

        SC_AGENT_UNREGISTER(ObtainingSemanticNeighborhoodAgent);
    }

    TEST_F(AgentTest, subjDomainTest)
    {
        ScMemoryContext & context = *m_ctx;
        loader.loadScsFile(context, TEST_FILES_DIR_PATH + "test.scs");

        ScAddr const & testActionNode = context.HelperFindBySystemIdtf("test_action_node");
        ScAddr const & paramsNode = context.HelperFindBySystemIdtf("test_subj_domain_params");

        ScAgentInit(true);
        initialize();

        SC_AGENT_REGISTER(ObtainingSemanticNeighborhoodAgent);

        ScAddr accessArc = context.CreateEdge(ScType::EdgeAccessConstPosPerm, testActionNode, paramsNode);
        context.CreateEdge(ScType::EdgeAccessConstPosTemp, scAgentsCommon::CoreKeynodes::rrel_2, accessArc);

        context.CreateEdge(
            ScType::EdgeAccessConstPosPerm, 
            scAgentsCommon::CoreKeynodes::question_initiated, 
            testActionNode);

        utils::AgentUtils::applyAction(&context, testActionNode, WAIT_TIME);

        EXPECT_TRUE(context.HelperCheckEdge(
            scAgentsCommon::CoreKeynodes::question_finished_successfully, 
            testActionNode, 
            ScType::EdgeAccessConstPosPerm));
        
        ScAddr const & answerNode = utils::IteratorUtils::getAnyByOutRelation(&context, testActionNode, scAgentsCommon::CoreKeynodes::nrel_answer);
        
        EXPECT_TRUE(answerNode.IsValid());

        ScAddr const & test_subject_domain = context.HelperFindBySystemIdtf("test_subject_domain");
        EXPECT_TRUE(test_subject_domain.IsValid());

        EXPECT_TRUE(context.HelperCheckEdge(answerNode, test_subject_domain, ScType::EdgeAccessConstPosPerm));

        SC_AGENT_UNREGISTER(ObtainingSemanticNeighborhoodAgent);
    }

    TEST_F(AgentTest, subdividingTest)
    {
        ScMemoryContext & context = *m_ctx;
        loader.loadScsFile(context, TEST_FILES_DIR_PATH + "test.scs");

        ScAddr const & testActionNode = context.HelperFindBySystemIdtf("test_action_node");
        ScAddr const & paramsNode = context.HelperFindBySystemIdtf("test_subdividing_params");

        ScAgentInit(true);
        initialize();

        SC_AGENT_REGISTER(ObtainingSemanticNeighborhoodAgent);

        ScAddr accessArc = context.CreateEdge(ScType::EdgeAccessConstPosPerm, testActionNode, paramsNode);
        context.CreateEdge(ScType::EdgeAccessConstPosTemp, scAgentsCommon::CoreKeynodes::rrel_2, accessArc);

        context.CreateEdge(
            ScType::EdgeAccessConstPosPerm, 
            scAgentsCommon::CoreKeynodes::question_initiated, 
            testActionNode);

        utils::AgentUtils::applyAction(&context, testActionNode, WAIT_TIME);

        EXPECT_TRUE(context.HelperCheckEdge(
            scAgentsCommon::CoreKeynodes::question_finished_successfully, 
            testActionNode, 
            ScType::EdgeAccessConstPosPerm));
        
        ScAddr const & answerNode = utils::IteratorUtils::getAnyByOutRelation(&context, testActionNode, scAgentsCommon::CoreKeynodes::nrel_answer);
        
        EXPECT_TRUE(answerNode.IsValid());

        ScAddr const & subdiv_1 = context.HelperFindBySystemIdtf("subdiv_1");
        ScAddr const & subdiv_2 = context.HelperFindBySystemIdtf("subdiv_2");
        ScAddr const & subdiv_3 = context.HelperFindBySystemIdtf("subdiv_3");
        ScAddr const & subdiv_4 = context.HelperFindBySystemIdtf("subdiv_4");

        EXPECT_TRUE(context.HelperCheckEdge(answerNode, subdiv_1, ScType::EdgeAccessConstPosPerm));
        EXPECT_TRUE(context.HelperCheckEdge(answerNode, subdiv_2, ScType::EdgeAccessConstPosPerm));
        EXPECT_TRUE(context.HelperCheckEdge(answerNode, subdiv_3, ScType::EdgeAccessConstPosPerm));
        EXPECT_TRUE(context.HelperCheckEdge(answerNode, subdiv_4, ScType::EdgeAccessConstPosPerm));

        SC_AGENT_UNREGISTER(ObtainingSemanticNeighborhoodAgent);
    }

    TEST_F(AgentTest, maxTest)
    {
        ScMemoryContext & context = *m_ctx;
        loader.loadScsFile(context, TEST_FILES_DIR_PATH + "test.scs");

        ScAddr const & testActionNode = context.HelperFindBySystemIdtf("test_action_node");
        ScAddr const & paramsNode = context.HelperFindBySystemIdtf("max_parameters");

        ScAgentInit(true);
        initialize();

        SC_AGENT_REGISTER(ObtainingSemanticNeighborhoodAgent);

        ScAddr accessArc = context.CreateEdge(ScType::EdgeAccessConstPosPerm, testActionNode, paramsNode);
        context.CreateEdge(ScType::EdgeAccessConstPosTemp, scAgentsCommon::CoreKeynodes::rrel_2, accessArc);

        context.CreateEdge(
            ScType::EdgeAccessConstPosPerm, 
            scAgentsCommon::CoreKeynodes::question_initiated, 
            testActionNode);

        utils::AgentUtils::applyAction(&context, testActionNode, WAIT_TIME);

        EXPECT_TRUE(context.HelperCheckEdge(
            scAgentsCommon::CoreKeynodes::question_finished_successfully, 
            testActionNode, 
            ScType::EdgeAccessConstPosPerm));
        
        ScAddr const & answerNode = utils::IteratorUtils::getAnyByOutRelation(&context, testActionNode, scAgentsCommon::CoreKeynodes::nrel_answer);

        EXPECT_TRUE(answerNode.IsValid());

        SC_AGENT_UNREGISTER(ObtainingSemanticNeighborhoodAgent);
    }

    TEST_F(AgentTest, noParametersTest)
    {
        ScMemoryContext & context = *m_ctx;
        loader.loadScsFile(context, TEST_FILES_DIR_PATH + "test.scs");

        ScAddr const & testActionNode = context.HelperFindBySystemIdtf("test_action_node");

        ScAgentInit(true);
        initialize();

        SC_AGENT_REGISTER(ObtainingSemanticNeighborhoodAgent);

        context.CreateEdge(
            ScType::EdgeAccessConstPosPerm, 
            scAgentsCommon::CoreKeynodes::question_initiated, 
            testActionNode);

        utils::AgentUtils::applyAction(&context, testActionNode, WAIT_TIME);

        EXPECT_TRUE(context.HelperCheckEdge(
            scAgentsCommon::CoreKeynodes::question_finished_unsuccessfully, 
            testActionNode, 
            ScType::EdgeAccessConstPosPerm));

        SC_AGENT_UNREGISTER(ObtainingSemanticNeighborhoodAgent);
    }
}