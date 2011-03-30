#include "TimelineTimeManagmentTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( TimelineTimeManagmentTest );

void TimelineTimeManagmentTest::setUp()
{
    model.reset(new timeline::Model("TestModel"));
}

void TimelineTimeManagmentTest::testRootScale()
{
    double scale = 2;
    model->setLocalTimeScale(scale);

    CPPUNIT_ASSERT(model->getLocalTimeScale() == scale);
    CPPUNIT_ASSERT(model->getLocalTimeScale() == model->getGlobalTimeScale());

    scale = 0.5;

    model->setGlobalTimeScale(scale);

    CPPUNIT_ASSERT(model->getLocalTimeScale() == scale);
    CPPUNIT_ASSERT(model->getLocalTimeScale() == model->getGlobalTimeScale());
}

void TimelineTimeManagmentTest::testRootOffset()
{
    double offset = 2;
    model->setLocalOffset(offset);

    CPPUNIT_ASSERT(model->getLocalOffset() == offset);
    CPPUNIT_ASSERT(model->getLocalOffset() == model->getGlobalOffset());

    offset = -4;
    model->setLocalOffset(offset);

    CPPUNIT_ASSERT(model->getLocalOffset() == offset);
    CPPUNIT_ASSERT(model->getLocalOffset() == model->getGlobalOffset());
}

void TimelineTimeManagmentTest::testOneLevelScale()
{
    std::string channelA = "channel A";

    model->addChannel(channelA, timeline::IChannelPtr(new TestClientChannel(2)));

    timeline::Model::TChannelConstPtr tChA = model->getChannel(channelA);
    timeline::ChannelConstPtr chA = tChA->getData();
    CPPUNIT_ASSERT(chA->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chA->getLocalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chA->getLength() == 2.0);
    CPPUNIT_ASSERT(model->getLength() == 2.0);

    model->setChannelLocalTimeScale(tChA,2);

    CPPUNIT_ASSERT(chA->getGlobalTimeScale() == 2.0);
    CPPUNIT_ASSERT(chA->getLocalTimeScale() == 2.0);
    CPPUNIT_ASSERT(model->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(model->getLocalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chA->getLength() == 4.0);
    CPPUNIT_ASSERT(model->getLength() == 4.0);

    model->setChannelGlobalTimeScale(tChA,3);

    CPPUNIT_ASSERT(chA->getGlobalTimeScale() == 3.0);
    CPPUNIT_ASSERT(chA->getLocalTimeScale() == 3.0);
    CPPUNIT_ASSERT(model->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(model->getLocalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chA->getLength() == 6.0);
    CPPUNIT_ASSERT(model->getLength() == 6.0);

    model->setLocalTimeScale(2);

    CPPUNIT_ASSERT(chA->getGlobalTimeScale() == 6.0);
    CPPUNIT_ASSERT(chA->getLocalTimeScale() == 3.0);
    CPPUNIT_ASSERT(model->getGlobalTimeScale() == 2.0);
    CPPUNIT_ASSERT(model->getLocalTimeScale() == 2.0);
    CPPUNIT_ASSERT(chA->getLength() == 12.0);
    CPPUNIT_ASSERT(model->getLength() == 12.0);

    model->setLocalTimeScale(3);

    CPPUNIT_ASSERT(chA->getGlobalTimeScale() == 9.0);
    CPPUNIT_ASSERT(chA->getLocalTimeScale() == 3.0);
    CPPUNIT_ASSERT(model->getGlobalTimeScale() == 3.0);
    CPPUNIT_ASSERT(model->getLocalTimeScale() == 3.0);
    CPPUNIT_ASSERT(chA->getLength() == 18.0);
    CPPUNIT_ASSERT(model->getLength() == 18.0);

    model->setLocalTimeScale(1.5);

    CPPUNIT_ASSERT(chA->getGlobalTimeScale() == 4.5);
    CPPUNIT_ASSERT(chA->getLocalTimeScale() == 3.0);
    CPPUNIT_ASSERT(model->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(model->getLocalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chA->getLength() == 9.0);
    CPPUNIT_ASSERT(model->getLength() == 9.0);

    model->setChannelLocalTimeScale(tChA,1);

    CPPUNIT_ASSERT(chA->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chA->getLocalTimeScale() == 1.0);
    CPPUNIT_ASSERT(model->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(model->getLocalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chA->getLength() == 3.0);
    CPPUNIT_ASSERT(model->getLength() == 3.0);

    model->setChannelGlobalTimeScale(tChA,1);

    CPPUNIT_ASSERT(chA->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chA->getLocalTimeScale() == 1.0 / 1.5);
    CPPUNIT_ASSERT(model->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(model->getLocalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chA->getLength() == 2.0);
    CPPUNIT_ASSERT(model->getLength() == 2.0);
}

void TimelineTimeManagmentTest::testOneLevelOffset()
{
    std::string channelA = "channel A";

    model->addChannel(channelA, timeline::IChannelPtr(new TestClientChannel(2)));

    timeline::Model::TChannelConstPtr tChA = model->getChannel(channelA);
    timeline::ChannelConstPtr chA = tChA->getData();
    CPPUNIT_ASSERT(chA->getGlobalOffset() == 0.0);
    CPPUNIT_ASSERT(chA->getLocalOffset() == 0.0);
    CPPUNIT_ASSERT(chA->getLength() == 2.0);
    CPPUNIT_ASSERT(model->getLength() == 2.0);

    model->setChannelLocalOffset(tChA,2);

    CPPUNIT_ASSERT(chA->getGlobalOffset() == 2.0);
    CPPUNIT_ASSERT(chA->getLocalOffset() == 2.0);
    CPPUNIT_ASSERT(model->getGlobalOffset() == 0.0);
    CPPUNIT_ASSERT(model->getLocalOffset() == 0.0);
    CPPUNIT_ASSERT(chA->getLength() == 2.0);
    CPPUNIT_ASSERT(model->getLength() == 4.0);

    model->setChannelGlobalOffset(tChA,-3);

    CPPUNIT_ASSERT(chA->getGlobalOffset() == -3.0);
    CPPUNIT_ASSERT(chA->getLocalOffset() == 0.0);
    CPPUNIT_ASSERT(model->getGlobalOffset() == -3.0);
    CPPUNIT_ASSERT(model->getLocalOffset() == -3.0);
    CPPUNIT_ASSERT(chA->getLength() == 2.0);
    CPPUNIT_ASSERT(model->getLength() == 2.0);

    model->setLocalOffset(2);

    CPPUNIT_ASSERT(chA->getGlobalOffset() == 2.0);
    CPPUNIT_ASSERT(chA->getLocalOffset() == 0.0);
    CPPUNIT_ASSERT(model->getGlobalOffset() == 2.0);
    CPPUNIT_ASSERT(model->getLocalOffset() == 2.0);
    CPPUNIT_ASSERT(chA->getLength() == 2.0);
    CPPUNIT_ASSERT(model->getLength() == 2.0);

    model->setLocalOffset(3);

    CPPUNIT_ASSERT(chA->getGlobalOffset() == 3.0);
    CPPUNIT_ASSERT(chA->getLocalOffset() == 0.0);
    CPPUNIT_ASSERT(model->getGlobalOffset() == 3.0);
    CPPUNIT_ASSERT(model->getLocalOffset() == 3.0);
    CPPUNIT_ASSERT(chA->getLength() == 2.0);
    CPPUNIT_ASSERT(model->getLength() == 2.0);

    model->setLocalOffset(1.5);

    CPPUNIT_ASSERT(chA->getGlobalOffset() == 1.5);
    CPPUNIT_ASSERT(chA->getLocalOffset() == 0.0);
    CPPUNIT_ASSERT(model->getGlobalOffset() == 1.5);
    CPPUNIT_ASSERT(model->getLocalOffset() == 1.5);
    CPPUNIT_ASSERT(chA->getLength() == 2.0);
    CPPUNIT_ASSERT(model->getLength() == 2.0);

    model->setChannelLocalOffset(tChA,1);

    CPPUNIT_ASSERT(chA->getGlobalOffset() == 2.5);
    CPPUNIT_ASSERT(chA->getLocalOffset() == 1.0);
    CPPUNIT_ASSERT(model->getGlobalOffset() == 1.5);
    CPPUNIT_ASSERT(model->getLocalOffset() == 1.5);
    CPPUNIT_ASSERT(chA->getLength() == 2.0);
    CPPUNIT_ASSERT(model->getLength() == 3.0);

    model->setGlobalOffset(-1);

    CPPUNIT_ASSERT(chA->getGlobalOffset() == 0);
    CPPUNIT_ASSERT(chA->getLocalOffset() == 1.0);
    CPPUNIT_ASSERT(model->getGlobalOffset() == -1);
    CPPUNIT_ASSERT(model->getLocalOffset() == -1);
    CPPUNIT_ASSERT(chA->getLength() == 2.0);
    CPPUNIT_ASSERT(model->getLength() == 3.0);
}

void TimelineTimeManagmentTest::testMultilevelLevelScale()
{
    std::string channelA = "A";
    std::string channelAA = "A/A";
    std::string channelAB = "A/B";

    std::string channelB = "B";
    std::string channelBA = "B/A";
    std::string channelBB = "B/B";

    model->addChannel(channelAA, timeline::IChannelPtr(new TestClientChannel(2)));
    model->addChannel(channelAB, timeline::IChannelPtr(new TestClientChannel(6)));

    model->addChannel(channelBA, timeline::IChannelPtr(new TestClientChannel(1)));
    model->addChannel(channelBB, timeline::IChannelPtr(new TestClientChannel(4)));

    timeline::Model::TChannelConstPtr tChA = model->getChannel(channelA);
    timeline::ChannelConstPtr chA = tChA->getData();

    timeline::Model::TChannelConstPtr tChAA = model->getChannel(channelAA);
    timeline::ChannelConstPtr chAA = tChAA->getData();

    timeline::Model::TChannelConstPtr tChAB = model->getChannel(channelAB);
    timeline::ChannelConstPtr chAB = tChAB->getData();

    timeline::Model::TChannelConstPtr tChB = model->getChannel(channelB);
    timeline::ChannelConstPtr chB = tChB->getData();

    timeline::Model::TChannelConstPtr tChBA = model->getChannel(channelBA);
    timeline::ChannelConstPtr chBA = tChBA->getData();

    timeline::Model::TChannelConstPtr tChBB = model->getChannel(channelBB);
    timeline::ChannelConstPtr chBB = tChBB->getData();


    CPPUNIT_ASSERT(model->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(model->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chA->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chA->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chAA->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chAA->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chAB->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chAB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chB->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chBA->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chBA->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chBB->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chBB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chA->getLength() == 6.0);
    CPPUNIT_ASSERT(chAA->getLength() == 2.0);
    CPPUNIT_ASSERT(chAB->getLength() == 6.0);
    CPPUNIT_ASSERT(chB->getLength() == 4.0);
    CPPUNIT_ASSERT(chBA->getLength() == 1.0);
    CPPUNIT_ASSERT(chBB->getLength() == 4.0);
    CPPUNIT_ASSERT(model->getLength() == 6.0);

    model->setChannelLocalTimeScale(tChA,2);

    CPPUNIT_ASSERT(model->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(model->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chA->getGlobalTimeScale() == 2.0);
    CPPUNIT_ASSERT(chA->getLocalTimeScale() == 2.0);

    CPPUNIT_ASSERT(chAA->getGlobalTimeScale() == 2.0);
    CPPUNIT_ASSERT(chAA->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chAB->getGlobalTimeScale() == 2.0);
    CPPUNIT_ASSERT(chAB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chB->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chBA->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chBA->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chBB->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chBB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chA->getLength() == 12.0);
    CPPUNIT_ASSERT(chAA->getLength() == 4.0);
    CPPUNIT_ASSERT(chAB->getLength() == 12.0);
    CPPUNIT_ASSERT(chB->getLength() == 4.0);
    CPPUNIT_ASSERT(chBA->getLength() == 1.0);
    CPPUNIT_ASSERT(chBB->getLength() == 4.0);
    CPPUNIT_ASSERT(model->getLength() == 12.0);

    model->setChannelGlobalTimeScale(tChA,3);

    CPPUNIT_ASSERT(model->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(model->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chA->getGlobalTimeScale() == 3.0);
    CPPUNIT_ASSERT(chA->getLocalTimeScale() == 3.0);

    CPPUNIT_ASSERT(chAA->getGlobalTimeScale() == 3.0);
    CPPUNIT_ASSERT(chAA->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chAB->getGlobalTimeScale() == 3.0);
    CPPUNIT_ASSERT(chAB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chB->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chBA->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chBA->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chBB->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chBB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chA->getLength() == 18.0);
    CPPUNIT_ASSERT(chAA->getLength() == 6.0);
    CPPUNIT_ASSERT(chAB->getLength() == 18.0);
    CPPUNIT_ASSERT(chB->getLength() == 4.0);
    CPPUNIT_ASSERT(chBA->getLength() == 1.0);
    CPPUNIT_ASSERT(chBB->getLength() == 4.0);
    CPPUNIT_ASSERT(model->getLength() == 18.0);

    model->setLocalTimeScale(2);

    CPPUNIT_ASSERT(model->getGlobalTimeScale() == 2.0);
    CPPUNIT_ASSERT(model->getLocalTimeScale() == 2.0);

    CPPUNIT_ASSERT(chA->getGlobalTimeScale() == 6.0);
    CPPUNIT_ASSERT(chA->getLocalTimeScale() == 3.0);

    CPPUNIT_ASSERT(chAA->getGlobalTimeScale() == 6.0);
    CPPUNIT_ASSERT(chAA->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chAB->getGlobalTimeScale() == 6.0);
    CPPUNIT_ASSERT(chAB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chB->getGlobalTimeScale() == 2.0);
    CPPUNIT_ASSERT(chB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chBA->getGlobalTimeScale() == 2.0);
    CPPUNIT_ASSERT(chBA->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chBB->getGlobalTimeScale() == 2.0);
    CPPUNIT_ASSERT(chBB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chA->getLength() == 36.0);
    CPPUNIT_ASSERT(chAA->getLength() == 12.0);
    CPPUNIT_ASSERT(chAB->getLength() == 36.0);
    CPPUNIT_ASSERT(chB->getLength() == 8.0);
    CPPUNIT_ASSERT(chBA->getLength() == 2.0);
    CPPUNIT_ASSERT(chBB->getLength() == 8.0);
    CPPUNIT_ASSERT(model->getLength() == 36.0);

    model->setLocalTimeScale(3);

    CPPUNIT_ASSERT(model->getGlobalTimeScale() == 3.0);
    CPPUNIT_ASSERT(model->getLocalTimeScale() == 3.0);

    CPPUNIT_ASSERT(chA->getGlobalTimeScale() == 9.0);
    CPPUNIT_ASSERT(chA->getLocalTimeScale() == 3.0);

    CPPUNIT_ASSERT(chAA->getGlobalTimeScale() == 9.0);
    CPPUNIT_ASSERT(chAA->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chAB->getGlobalTimeScale() == 9.0);
    CPPUNIT_ASSERT(chAB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chB->getGlobalTimeScale() == 3.0);
    CPPUNIT_ASSERT(chB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chBA->getGlobalTimeScale() == 3.0);
    CPPUNIT_ASSERT(chBA->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chBB->getGlobalTimeScale() == 3.0);
    CPPUNIT_ASSERT(chBB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chA->getLength() == 54.0);
    CPPUNIT_ASSERT(chAA->getLength() == 18.0);
    CPPUNIT_ASSERT(chAB->getLength() == 54.0);
    CPPUNIT_ASSERT(chB->getLength() == 12.0);
    CPPUNIT_ASSERT(chBA->getLength() == 3.0);
    CPPUNIT_ASSERT(chBB->getLength() == 12.0);
    CPPUNIT_ASSERT(model->getLength() == 54.0);

    model->setLocalTimeScale(1.5);

    CPPUNIT_ASSERT(model->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(model->getLocalTimeScale() == 1.5);

    CPPUNIT_ASSERT(chA->getGlobalTimeScale() == 4.5);
    CPPUNIT_ASSERT(chA->getLocalTimeScale() == 3.0);

    CPPUNIT_ASSERT(chAA->getGlobalTimeScale() == 4.5);
    CPPUNIT_ASSERT(chAA->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chAB->getGlobalTimeScale() == 4.5);
    CPPUNIT_ASSERT(chAB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chB->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chBA->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chBA->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chBB->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chBB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chA->getLength() == 27.0);
    CPPUNIT_ASSERT(chAA->getLength() == 9.0);
    CPPUNIT_ASSERT(chAB->getLength() == 27.0);
    CPPUNIT_ASSERT(chB->getLength() == 6.0);
    CPPUNIT_ASSERT(chBA->getLength() == 1.5);
    CPPUNIT_ASSERT(chBB->getLength() == 6.0);
    CPPUNIT_ASSERT(model->getLength() == 27.0);

    model->setChannelLocalTimeScale(tChA,1);

    CPPUNIT_ASSERT(model->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(model->getLocalTimeScale() == 1.5);

    CPPUNIT_ASSERT(chA->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chA->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chAA->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chAA->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chAB->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chAB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chB->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chBA->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chBA->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chBB->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chBB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chA->getLength() == 9.0);
    CPPUNIT_ASSERT(chAA->getLength() == 3.0);
    CPPUNIT_ASSERT(chAB->getLength() == 9.0);
    CPPUNIT_ASSERT(chB->getLength() == 6.0);
    CPPUNIT_ASSERT(chBA->getLength() == 1.5);
    CPPUNIT_ASSERT(chBB->getLength() == 6.0);
    CPPUNIT_ASSERT(model->getLength() == 9.0);

    model->setChannelGlobalTimeScale(tChA,1);

    CPPUNIT_ASSERT(model->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(model->getLocalTimeScale() == 1.5);

    CPPUNIT_ASSERT(chA->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chA->getLocalTimeScale() == 1.0 / 1.5);

    CPPUNIT_ASSERT(chAA->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chAA->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chAB->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chAB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chB->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chBA->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chBA->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chBB->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chBB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chA->getLength() == 6.0);
    CPPUNIT_ASSERT(chAA->getLength() == 2.0);
    CPPUNIT_ASSERT(chAB->getLength() == 6.0);
    CPPUNIT_ASSERT(chB->getLength() == 6.0);
    CPPUNIT_ASSERT(chBA->getLength() == 1.5);
    CPPUNIT_ASSERT(chBB->getLength() == 6.0);
    CPPUNIT_ASSERT(model->getLength() == 6.0);

    model->setChannelGlobalTimeScale(tChAA,2);

    CPPUNIT_ASSERT(model->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(model->getLocalTimeScale() == 1.5);

    CPPUNIT_ASSERT(chA->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chA->getLocalTimeScale() == 1.0 / 1.5);

    CPPUNIT_ASSERT(chAA->getGlobalTimeScale() == 2.0);
    CPPUNIT_ASSERT(chAA->getLocalTimeScale() == 2.0);

    CPPUNIT_ASSERT(chAB->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chAB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chB->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chBA->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chBA->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chBB->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chBB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chA->getLength() == 6.0);
    CPPUNIT_ASSERT(chAA->getLength() == 4.0);
    CPPUNIT_ASSERT(chAB->getLength() == 6.0);
    CPPUNIT_ASSERT(chB->getLength() == 6.0);
    CPPUNIT_ASSERT(chBA->getLength() == 1.5);
    CPPUNIT_ASSERT(chBB->getLength() == 6.0);
    CPPUNIT_ASSERT(model->getLength() == 6.0);

    model->setChannelLocalTimeScale(tChAA,3);

    CPPUNIT_ASSERT(model->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(model->getLocalTimeScale() == 1.5);

    CPPUNIT_ASSERT(chA->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chA->getLocalTimeScale() == 1.0 / 1.5);

    CPPUNIT_ASSERT(chAA->getGlobalTimeScale() == 3.0);
    CPPUNIT_ASSERT(chAA->getLocalTimeScale() == 3.0);

    CPPUNIT_ASSERT(chAB->getGlobalTimeScale() == 1.0);
    CPPUNIT_ASSERT(chAB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chB->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chBA->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chBA->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chBB->getGlobalTimeScale() == 1.5);
    CPPUNIT_ASSERT(chBB->getLocalTimeScale() == 1.0);

    CPPUNIT_ASSERT(chA->getLength() == 6.0);
    CPPUNIT_ASSERT(chAA->getLength() == 6.0);
    CPPUNIT_ASSERT(chAB->getLength() == 6.0);
    CPPUNIT_ASSERT(chB->getLength() == 6.0);
    CPPUNIT_ASSERT(chBA->getLength() == 1.5);
    CPPUNIT_ASSERT(chBB->getLength() == 6.0);
    CPPUNIT_ASSERT(model->getLength() == 6.0);
}

void TimelineTimeManagmentTest::testMultiLevelOffset()
{
    std::string channelA = "A";
    std::string channelAA = "A/A";
    std::string channelAB = "A/B";

    std::string channelB = "B";
    std::string channelBA = "B/A";
    std::string channelBB = "B/B";

    model->addChannel(channelAA, timeline::IChannelPtr(new TestClientChannel(2)));
    model->addChannel(channelAB, timeline::IChannelPtr(new TestClientChannel(6)));

    model->addChannel(channelBA, timeline::IChannelPtr(new TestClientChannel(1)));
    model->addChannel(channelBB, timeline::IChannelPtr(new TestClientChannel(4)));

    timeline::Model::TChannelConstPtr tChA = model->getChannel(channelA);
    timeline::ChannelConstPtr chA = tChA->getData();

    timeline::Model::TChannelConstPtr tChAA = model->getChannel(channelAA);
    timeline::ChannelConstPtr chAA = tChAA->getData();

    timeline::Model::TChannelConstPtr tChAB = model->getChannel(channelAB);
    timeline::ChannelConstPtr chAB = tChAB->getData();

    timeline::Model::TChannelConstPtr tChB = model->getChannel(channelB);
    timeline::ChannelConstPtr chB = tChB->getData();

    timeline::Model::TChannelConstPtr tChBA = model->getChannel(channelBA);
    timeline::ChannelConstPtr chBA = tChBA->getData();

    timeline::Model::TChannelConstPtr tChBB = model->getChannel(channelBB);
    timeline::ChannelConstPtr chBB = tChBB->getData();


    CPPUNIT_ASSERT(model->getGlobalOffset() == 0.0);
    CPPUNIT_ASSERT(model->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chA->getGlobalOffset() == 0.0);
    CPPUNIT_ASSERT(chA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chAA->getGlobalOffset() == 0.0);
    CPPUNIT_ASSERT(chAA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chAB->getGlobalOffset() == 0.0);
    CPPUNIT_ASSERT(chAB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chB->getGlobalOffset() == 0.0);
    CPPUNIT_ASSERT(chB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chBA->getGlobalOffset() == 0.0);
    CPPUNIT_ASSERT(chBA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chBB->getGlobalOffset() == 0.0);
    CPPUNIT_ASSERT(chBB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chA->getLength() == 6.0);
    CPPUNIT_ASSERT(chAA->getLength() == 2.0);
    CPPUNIT_ASSERT(chAB->getLength() == 6.0);
    CPPUNIT_ASSERT(chB->getLength() == 4.0);
    CPPUNIT_ASSERT(chBA->getLength() == 1.0);
    CPPUNIT_ASSERT(chBB->getLength() == 4.0);
    CPPUNIT_ASSERT(model->getLength() == 6.0);

    model->setChannelLocalOffset(tChA,2);

    CPPUNIT_ASSERT(model->getGlobalOffset() == 0.0);
    CPPUNIT_ASSERT(model->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chA->getGlobalOffset() == 2.0);
    CPPUNIT_ASSERT(chA->getLocalOffset() == 2.0);

    CPPUNIT_ASSERT(chAA->getGlobalOffset() == 2.0);
    CPPUNIT_ASSERT(chAA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chAB->getGlobalOffset() == 2.0);
    CPPUNIT_ASSERT(chAB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chB->getGlobalOffset() == 0.0);
    CPPUNIT_ASSERT(chB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chBA->getGlobalOffset() == 0.0);
    CPPUNIT_ASSERT(chBA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chBB->getGlobalOffset() == 0.0);
    CPPUNIT_ASSERT(chBB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chA->getLength() == 6.0);
    CPPUNIT_ASSERT(chAA->getLength() == 2.0);
    CPPUNIT_ASSERT(chAB->getLength() == 6.0);
    CPPUNIT_ASSERT(chB->getLength() == 4.0);
    CPPUNIT_ASSERT(chBA->getLength() == 1.0);
    CPPUNIT_ASSERT(chBB->getLength() == 4.0);
    CPPUNIT_ASSERT(model->getLength() == 8.0);

    model->setChannelGlobalOffset(tChA,3);

    CPPUNIT_ASSERT(model->getGlobalOffset() == 0.0);
    CPPUNIT_ASSERT(model->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chA->getGlobalOffset() == 3.0);
    CPPUNIT_ASSERT(chA->getLocalOffset() == 3.0);

    CPPUNIT_ASSERT(chAA->getGlobalOffset() == 3.0);
    CPPUNIT_ASSERT(chAA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chAB->getGlobalOffset() == 3.0);
    CPPUNIT_ASSERT(chAB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chB->getGlobalOffset() == 0.0);
    CPPUNIT_ASSERT(chB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chBA->getGlobalOffset() == 0.0);
    CPPUNIT_ASSERT(chBA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chBB->getGlobalOffset() == 0.0);
    CPPUNIT_ASSERT(chBB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chA->getLength() == 6.0);
    CPPUNIT_ASSERT(chAA->getLength() == 2.0);
    CPPUNIT_ASSERT(chAB->getLength() == 6.0);
    CPPUNIT_ASSERT(chB->getLength() == 4.0);
    CPPUNIT_ASSERT(chBA->getLength() == 1.0);
    CPPUNIT_ASSERT(chBB->getLength() == 4.0);
    CPPUNIT_ASSERT(model->getLength() == 9.0);

    model->setLocalOffset(2);

    CPPUNIT_ASSERT(model->getGlobalOffset() == 2.0);
    CPPUNIT_ASSERT(model->getLocalOffset() == 2.0);

    CPPUNIT_ASSERT(chA->getGlobalOffset() == 5.0);
    CPPUNIT_ASSERT(chA->getLocalOffset() == 3.0);

    CPPUNIT_ASSERT(chAA->getGlobalOffset() == 5.0);
    CPPUNIT_ASSERT(chAA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chAB->getGlobalOffset() == 5.0);
    CPPUNIT_ASSERT(chAB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chB->getGlobalOffset() == 2.0);
    CPPUNIT_ASSERT(chB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chBA->getGlobalOffset() == 2.0);
    CPPUNIT_ASSERT(chBA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chBB->getGlobalOffset() == 2.0);
    CPPUNIT_ASSERT(chBB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chA->getLength() == 6.0);
    CPPUNIT_ASSERT(chAA->getLength() == 2.0);
    CPPUNIT_ASSERT(chAB->getLength() == 6.0);
    CPPUNIT_ASSERT(chB->getLength() == 4.0);
    CPPUNIT_ASSERT(chBA->getLength() == 1.0);
    CPPUNIT_ASSERT(chBB->getLength() == 4.0);
    CPPUNIT_ASSERT(model->getLength() == 9.0);

    model->setLocalOffset(3);

    CPPUNIT_ASSERT(model->getGlobalOffset() == 3.0);
    CPPUNIT_ASSERT(model->getLocalOffset() == 3.0);

    CPPUNIT_ASSERT(chA->getGlobalOffset() == 6.0);
    CPPUNIT_ASSERT(chA->getLocalOffset() == 3.0);

    CPPUNIT_ASSERT(chAA->getGlobalOffset() == 6.0);
    CPPUNIT_ASSERT(chAA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chAB->getGlobalOffset() == 6.0);
    CPPUNIT_ASSERT(chAB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chB->getGlobalOffset() == 3.0);
    CPPUNIT_ASSERT(chB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chBA->getGlobalOffset() == 3.0);
    CPPUNIT_ASSERT(chBA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chBB->getGlobalOffset() == 3.0);
    CPPUNIT_ASSERT(chBB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chA->getLength() == 6.0);
    CPPUNIT_ASSERT(chAA->getLength() == 2.0);
    CPPUNIT_ASSERT(chAB->getLength() == 6.0);
    CPPUNIT_ASSERT(chB->getLength() == 4.0);
    CPPUNIT_ASSERT(chBA->getLength() == 1.0);
    CPPUNIT_ASSERT(chBB->getLength() == 4.0);
    CPPUNIT_ASSERT(model->getLength() == 9.0);

    model->setLocalOffset(1.5);

    CPPUNIT_ASSERT(model->getGlobalOffset() == 1.5);
    CPPUNIT_ASSERT(model->getLocalOffset() == 1.5);

    CPPUNIT_ASSERT(chA->getGlobalOffset() == 4.5);
    CPPUNIT_ASSERT(chA->getLocalOffset() == 3.0);

    CPPUNIT_ASSERT(chAA->getGlobalOffset() == 4.5);
    CPPUNIT_ASSERT(chAA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chAB->getGlobalOffset() == 4.5);
    CPPUNIT_ASSERT(chAB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chB->getGlobalOffset() == 1.5);
    CPPUNIT_ASSERT(chB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chBA->getGlobalOffset() == 1.5);
    CPPUNIT_ASSERT(chBA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chBB->getGlobalOffset() == 1.5);
    CPPUNIT_ASSERT(chBB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chA->getLength() == 6.0);
    CPPUNIT_ASSERT(chAA->getLength() == 2.0);
    CPPUNIT_ASSERT(chAB->getLength() == 6.0);
    CPPUNIT_ASSERT(chB->getLength() == 4.0);
    CPPUNIT_ASSERT(chBA->getLength() == 1.0);
    CPPUNIT_ASSERT(chBB->getLength() == 4.0);
    CPPUNIT_ASSERT(model->getLength() == 9.0);

    model->setChannelLocalOffset(tChA,1);

    CPPUNIT_ASSERT(model->getGlobalOffset() == 1.5);
    CPPUNIT_ASSERT(model->getLocalOffset() == 1.5);

    CPPUNIT_ASSERT(chA->getGlobalOffset() == 2.5);
    CPPUNIT_ASSERT(chA->getLocalOffset() == 1.0);

    CPPUNIT_ASSERT(chAA->getGlobalOffset() == 2.5);
    CPPUNIT_ASSERT(chAA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chAB->getGlobalOffset() == 2.5);
    CPPUNIT_ASSERT(chAB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chB->getGlobalOffset() == 1.5);
    CPPUNIT_ASSERT(chB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chBA->getGlobalOffset() == 1.5);
    CPPUNIT_ASSERT(chBA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chBB->getGlobalOffset() == 1.5);
    CPPUNIT_ASSERT(chBB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chA->getLength() == 6.0);
    CPPUNIT_ASSERT(chAA->getLength() == 2.0);
    CPPUNIT_ASSERT(chAB->getLength() == 6.0);
    CPPUNIT_ASSERT(chB->getLength() == 4.0);
    CPPUNIT_ASSERT(chBA->getLength() == 1.0);
    CPPUNIT_ASSERT(chBB->getLength() == 4.0);
    CPPUNIT_ASSERT(model->getLength() == 7.0);

    model->setChannelGlobalOffset(tChA,1);

    CPPUNIT_ASSERT(model->getGlobalOffset() == 1.0);
    CPPUNIT_ASSERT(model->getLocalOffset() == 1.0);

    CPPUNIT_ASSERT(chA->getGlobalOffset() == 1.0);
    CPPUNIT_ASSERT(chA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chAA->getGlobalOffset() == 1.0);
    CPPUNIT_ASSERT(chAA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chAB->getGlobalOffset() == 1.0);
    CPPUNIT_ASSERT(chAB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chB->getGlobalOffset() == 1.5);
    CPPUNIT_ASSERT(chB->getLocalOffset() == 0.5);

    CPPUNIT_ASSERT(chBA->getGlobalOffset() == 1.5);
    CPPUNIT_ASSERT(chBA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chBB->getGlobalOffset() == 1.5);
    CPPUNIT_ASSERT(chBB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chA->getLength() == 6.0);
    CPPUNIT_ASSERT(chAA->getLength() == 2.0);
    CPPUNIT_ASSERT(chAB->getLength() == 6.0);
    CPPUNIT_ASSERT(chB->getLength() == 4.0);
    CPPUNIT_ASSERT(chBA->getLength() == 1.0);
    CPPUNIT_ASSERT(chBB->getLength() == 4.0);
    CPPUNIT_ASSERT(model->getLength() == 6.0);

    model->setChannelGlobalOffset(tChAA,2);

    CPPUNIT_ASSERT(model->getGlobalOffset() == 1.0);
    CPPUNIT_ASSERT(model->getLocalOffset() == 1.0);

    CPPUNIT_ASSERT(chA->getGlobalOffset() == 1.0);
    CPPUNIT_ASSERT(chA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chAA->getGlobalOffset() == 2.0);
    CPPUNIT_ASSERT(chAA->getLocalOffset() == 1.0);

    CPPUNIT_ASSERT(chAB->getGlobalOffset() == 1.0);
    CPPUNIT_ASSERT(chAB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chB->getGlobalOffset() == 1.5);
    CPPUNIT_ASSERT(chB->getLocalOffset() == 0.5);

    CPPUNIT_ASSERT(chBA->getGlobalOffset() == 1.5);
    CPPUNIT_ASSERT(chBA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chBB->getGlobalOffset() == 1.5);
    CPPUNIT_ASSERT(chBB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chA->getLength() == 6.0);
    CPPUNIT_ASSERT(chAA->getLength() == 2.0);
    CPPUNIT_ASSERT(chAB->getLength() == 6.0);
    CPPUNIT_ASSERT(chB->getLength() == 4.0);
    CPPUNIT_ASSERT(chBA->getLength() == 1.0);
    CPPUNIT_ASSERT(chBB->getLength() == 4.0);
    CPPUNIT_ASSERT(model->getLength() == 6.0);

    model->setChannelLocalOffset(tChAA,3);

    CPPUNIT_ASSERT(model->getGlobalOffset() == 1.0);
    CPPUNIT_ASSERT(model->getLocalOffset() == 1.0);

    CPPUNIT_ASSERT(chA->getGlobalOffset() == 1.0);
    CPPUNIT_ASSERT(chA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chAA->getGlobalOffset() == 4.0);
    CPPUNIT_ASSERT(chAA->getLocalOffset() == 3.0);

    CPPUNIT_ASSERT(chAB->getGlobalOffset() == 1.0);
    CPPUNIT_ASSERT(chAB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chB->getGlobalOffset() == 1.5);
    CPPUNIT_ASSERT(chB->getLocalOffset() == 0.5);

    CPPUNIT_ASSERT(chBA->getGlobalOffset() == 1.5);
    CPPUNIT_ASSERT(chBA->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chBB->getGlobalOffset() == 1.5);
    CPPUNIT_ASSERT(chBB->getLocalOffset() == 0.0);

    CPPUNIT_ASSERT(chA->getLength() == 6.0);
    CPPUNIT_ASSERT(chAA->getLength() == 2.0);
    CPPUNIT_ASSERT(chAB->getLength() == 6.0);
    CPPUNIT_ASSERT(chB->getLength() == 4.0);
    CPPUNIT_ASSERT(chBA->getLength() == 1.0);
    CPPUNIT_ASSERT(chBB->getLength() == 4.0);
    CPPUNIT_ASSERT(model->getLength() == 6.0);
}

void TimelineTimeManagmentTest::testTimeSet()
{

}