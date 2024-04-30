#include "scs_loader.hpp"
#include "sc_test.hpp"
#include "sc-agents-common/utils/CommonUtils.hpp"
#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "agent/GenerateResponseAgent.hpp"
#include "test/agent/OneParameterTestAgent.hpp"
#include "test/agent/ZeroParameterTestAgent.hpp"
#include "test/keynodes/TestKeynodes.hpp"
#include "keynodes/Keynodes.hpp"

#include <filesystem>

namespace generateResponseModuleTest
{
    const std::string TEST_FILES_DIR_PATH = MODULE_TEST_SRC_PATH "/testStructures/";
    const std::string TEMPLATES_DIR_PATH = GENERATE_RESPONSE_TEMPLATE_PATH;

    const int WAIT_TIME = 1000;

    using AgentTest = ScMemoryTest;

    ScsLoader loader;
    void loadScsFile(ScMemoryContext & context, std::string const & filename)
    {
        loader.loadScsFile(context, TEST_FILES_DIR_PATH + filename);
    }

    void initialize(ScMemoryContext & context)
    {
        scAgentsCommon::CoreKeynodes::InitGlobal();
        generateResponseModule::Keynodes::InitGlobal();
        TestKeynodes::InitGlobal();

        for (auto const & file : std::filesystem::directory_iterator(TEMPLATES_DIR_PATH))
        {
            loader.loadScsFile(context, file.path());
        }
    }

    TEST_F(AgentTest, OneParameterAgentTest)
    {
        ScMemoryContext & context = *m_ctx;
        loadScsFile(context, "baseConcepts.scs");
        loadScsFile(context, "oneParameterTest.scs");

        ScAgentInit(true);
        initialize(context);

        SC_AGENT_REGISTER(OneParameterTestAgent);
        SC_AGENT_REGISTER(generateResponseModule::GenerateResponseAgent);

        ScAddr const testActionNode = context.HelperFindBySystemIdtf("one_param_action_node");
        ScAddr const message = context.HelperFindBySystemIdtf("message");
        ScAddr const answer = context.HelperFindBySystemIdtf("_answer");
        
        utils::AgentUtils::applyAction(&context, testActionNode, WAIT_TIME);

        EXPECT_TRUE(context.HelperCheckEdge(
                scAgentsCommon::CoreKeynodes::question_finished_successfully,
                testActionNode,
                ScType::EdgeAccessConstPosPerm));

        ScAddr messageAnswer = utils::IteratorUtils::getAnyByOutRelation(&context, message, generateResponseModule::Keynodes::nrel_reply_structure);

        EXPECT_TRUE(messageAnswer.IsValid());

        ScAddr test_entity = utils::IteratorUtils::getAnyFromSet(&context, messageAnswer);

        EXPECT_TRUE(context.HelperGetSystemIdtf(test_entity) == "test_entity");

        EXPECT_TRUE(context.HelperCheckEdge(answer, messageAnswer, ScType::EdgeAccessConstPosTemp));
        
        SC_AGENT_UNREGISTER(OneParameterTestAgent);
    }

    TEST_F(AgentTest, ZeroParameterTestAgent)
    {
        ScMemoryContext & context = *m_ctx;
        loadScsFile(context, "baseConcepts.scs");
        loadScsFile(context, "zeroParameterTest.scs");

        ScAgentInit(true);
        initialize(context);

        SC_AGENT_REGISTER(ZeroParameterTestAgent);
        SC_AGENT_REGISTER(generateResponseModule::GenerateResponseAgent);

        ScAddr const & testActionNode = context.HelperFindBySystemIdtf("zero_param_action_node");
        ScAddr const & message = context.HelperFindBySystemIdtf("message");
        ScAddr const & answer = context.HelperFindBySystemIdtf("_answer");
        
        utils::AgentUtils::applyAction(&context, testActionNode, WAIT_TIME);

        EXPECT_TRUE(context.HelperCheckEdge(
                scAgentsCommon::CoreKeynodes::question_finished_successfully,
                testActionNode,
                ScType::EdgeAccessConstPosPerm));

        ScAddr messageAnswer = utils::IteratorUtils::getAnyByOutRelation(&context, message, generateResponseModule::Keynodes::nrel_reply_structure);

        EXPECT_TRUE(messageAnswer.IsValid());

        ScAddr test_success = utils::IteratorUtils::getAnyFromSet(&context, messageAnswer);

        ScAddr correctTestSuccess = context.HelperFindBySystemIdtf("test_success");
        EXPECT_TRUE(correctTestSuccess.IsValid() && test_success == correctTestSuccess);

        EXPECT_TRUE(context.HelperCheckEdge(answer, messageAnswer, ScType::EdgeAccessConstPosTemp));

        SC_AGENT_UNREGISTER(OneParameterTestAgent);
    }
}
