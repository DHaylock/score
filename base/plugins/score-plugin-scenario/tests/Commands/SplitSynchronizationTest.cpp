// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest/QTest>

#include <Scenario/Commands/Scenario/Creations/CreateEvent.hpp>
#include <Scenario/Commands/Synchronization/SplitSynchronization.hpp>

#include <Scenario/Document/Event/EventData.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/Synchronization/SynchronizationModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <score/document/DocumentInterface.hpp>

using namespace score;
using namespace Scenario::Command;

class SplitSynchronizationTest : public QObject
{
  Q_OBJECT
private slots:
  void SplitTest()
  {
    Scenario::ProcessModel* scenar = new ScenarioModel(
        std::chrono::seconds(15), Id<ProcessModel>{0}, qApp);

    EventData data{};
    data.dDate.setMSecs(10);
    data.relativeY = 0.8;
    data.endSynchronizationId = Id<SynchronizationModel>(-1);

    CreateEvent eventCmd(
        {
            {"ScenarioModel", {0}},
        },
        data);
    eventCmd.redo(ctx);

    auto event1_id = eventCmd.createdEvent();
    auto event1 = scenar->event(event1_id);
    auto tn1_id = eventCmd.createdSynchronization();

    data.endSynchronizationId = tn1_id;
    data.relativeY = 0.4;

    CreateEvent event2Cmd(
        {
            {"ScenarioModel", {0}},
        },
        data);
    event2Cmd.redo(ctx);

    auto event2_id = event2Cmd.createdEvent();
    auto event2 = scenar->event(event2_id);
    auto tn2_id = event2Cmd.createdSynchronization();

    QCOMPARE(tn1_id, tn2_id);

    QVector<Id<EventModel>> evListForNewTn;
    evListForNewTn.push_back(event2_id);

    SplitSynchronization cmd(
        {
            {"SynchronizationModel", {tn1_id.val()}},
        },
        evListForNewTn);

    cmd.redo(ctx);

    auto newTn_id = cmd.createdSynchronization();
    auto newTn = scenar->synchronization(newTn_id);

    QCOMPARE((int)scenar->synchronizations().size(), 4);

    QCOMPARE(event2->synchronization(), newTn_id);
    QCOMPARE(newTn->events().size(), 1);
    QCOMPARE(newTn->events().at(0), event2_id);

    cmd.undo(ctx);
    QCOMPARE((int)scenar->synchronizations().size(), 3);

    QCOMPARE(event2->synchronization(), event1->synchronization());
    try
    {
      scenar->synchronization(newTn_id);
      QFAIL("Synchronization call did not throw !");
    }
    catch (...)
    {
    }

    cmd.redo(ctx);
    newTn = scenar->synchronization(newTn_id);

    QCOMPARE((int)scenar->synchronizations().size(), 4);

    QCOMPARE(event2->synchronization(), newTn_id);
    QCOMPARE(newTn->events().size(), 1);
    QCOMPARE(newTn->events().at(0), event2_id);
  }
};

QTEST_MAIN(SplitSynchronizationTest)
#include "SplitSynchronizationTest.moc"
