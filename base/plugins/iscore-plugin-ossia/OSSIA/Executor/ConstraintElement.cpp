#include <ossia/editor/scenario/time_constraint.hpp>
#include <Automation/AutomationModel.hpp>
#include <OSSIA/OSSIA2iscore.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <OSSIA/iscore2OSSIA.hpp>
#include <utility>

#include "ConstraintElement.hpp"
#include <ossia/editor/scenario/time_value.hpp>
#include "Loop/LoopProcessModel.hpp"
#include <Process/Process.hpp>
#include <Process/TimeValue.hpp>
#include <OSSIA/Executor/ProcessElement.hpp>
#include <OSSIA/Executor/ProcessWrapper.hpp>
#include <OSSIA/Executor/BasicProcessWrapper.hpp>
#include <OSSIA/Executor/LoopingProcessWrapper.hpp>
#include <Scenario/Document/Constraint/ConstraintDurations.hpp>
#include "ScenarioElement.hpp"
#include <iscore/tools/SettableIdentifier.hpp>
#include <iscore/tools/SettableIdentifierGeneration.hpp>
#include <OSSIA/Executor/DocumentPlugin.hpp>
#include <OSSIA/Executor/ExecutorContext.hpp>
#include <iscore/document/DocumentContext.hpp>

namespace RecreateOnPlay
{
ConstraintElement::ConstraintElement(
        std::shared_ptr<OSSIA::TimeConstraint> ossia_cst,
        Scenario::ConstraintModel& iscore_cst,
        const Context&ctx,
        QObject* parent):
    QObject{parent},
    m_iscore_constraint{iscore_cst},
    m_ossia_constraint{ossia_cst},
    m_ctx{ctx}
{
    OSSIA::TimeValue min_duration(iscore::convert::time(m_iscore_constraint.duration.minDuration()));
    OSSIA::TimeValue max_duration(iscore::convert::time(m_iscore_constraint.duration.maxDuration()));

    m_ossia_constraint->setDurationMin(min_duration);
    m_ossia_constraint->setDurationMax(max_duration);
    m_ossia_constraint->setSpeed(iscore_cst.duration.executionSpeed());

    con(iscore_cst.duration, &Scenario::ConstraintDurations::executionSpeedChanged,
            this, [&] (double sp) {
        m_ossia_constraint->setSpeed(sp);
    });

    // BaseScenario needs a special callback.
    if(dynamic_cast<Scenario::ProcessModel*>(iscore_cst.parent())
    || dynamic_cast<Loop::ProcessModel*>(iscore_cst.parent()))
    {
        ossia_cst->setCallback([&] (
                               OSSIA::TimeValue position,
                               OSSIA::TimeValue date,
                               const OSSIA::State& state)
        {
            constraintCallback(position, date, state);
        });
    }

    for(const auto& process : iscore_cst.processes)
    {
        on_processAdded(process);
    }

}

std::shared_ptr<OSSIA::TimeConstraint> ConstraintElement::OSSIAConstraint() const
{
    return m_ossia_constraint;
}

Scenario::ConstraintModel& ConstraintElement::iscoreConstraint() const
{
    return m_iscore_constraint;
}

void ConstraintElement::play(TimeValue t)
{
    m_offset = iscore::convert::time(t);
    m_iscore_constraint.duration.setPlayPercentage(0);

    auto start_state = m_ossia_constraint->getStartEvent()->getState();
    auto offset_state = m_ossia_constraint->offset(m_offset);

    OSSIA::State accumulator;
    flattenAndFilter(accumulator, start_state);
    flattenAndFilter(accumulator, offset_state);
    accumulator.launch();

    try {
        m_ossia_constraint->start();
        executionStarted();
    }
    catch(const std::exception& e) {
        qDebug() << e.what();
    }

}

void ConstraintElement::pause()
{
    m_ossia_constraint->pause();
}

void ConstraintElement::resume()
{
    m_ossia_constraint->resume();
}

void ConstraintElement::stop()
{
    m_ossia_constraint->stop();
    auto st = m_ossia_constraint->getEndEvent()->getState();
    st.launch();

    for(auto& process : m_processes)
    {
        process.second.element->stop();
    }
    m_iscore_constraint.reset();

    executionStopped();
}

void ConstraintElement::executionStarted()
{
    m_iscore_constraint.duration.setPlayPercentage(0);
    m_iscore_constraint.executionStarted();
    for(Process::ProcessModel& proc : m_iscore_constraint.processes)
    {
        proc.startExecution();
    }
}

void ConstraintElement::executionStopped()
{
    m_iscore_constraint.executionStopped();
    for(Process::ProcessModel& proc : m_iscore_constraint.processes)
    {
        proc.stopExecution();
    }
}

void ConstraintElement::on_processAdded(
        const Process::ProcessModel& iscore_proc) // TODO ProcessExecutionView
{
    // The DocumentPlugin creates the elements in the processes.
    // TODO maybe have an execution_view template on processes, that
    // gives correct const / non_const access ?
    auto proc = const_cast<Process::ProcessModel*>(&iscore_proc);
    auto fac = m_ctx.processes.factory(*proc);
    if(fac)
    {
        auto plug = fac->make(*this, *proc, m_ctx, getStrongId(iscore_proc.components), this);
        if(plug)
        {
            auto id = iscore_proc.id();
            std::unique_ptr<ProcessWrapper> wp;
            if(proc->useParentDuration())
            {
                wp = std::make_unique<BasicProcessWrapper>(
                            m_ossia_constraint,
                            plug->OSSIAProcess(),
                            iscore::convert::time(plug->process().duration()),
                            m_iscore_constraint.looping() );
            }
            else
            {
                wp = std::make_unique<LoopingProcessWrapper>(
                            m_ossia_constraint,
                            plug->OSSIAProcess(),
                            iscore::convert::time(plug->process().duration()),
                            m_iscore_constraint.looping()) ;
            }
            m_processes.insert(
                        std::make_pair(
                            id,
                            OSSIAProcess(plug, std::move(wp))));
        }
    }
}

void ConstraintElement::constraintCallback(
        OSSIA::TimeValue position,
        OSSIA::TimeValue date,
        const OSSIA::State& state)
{
    auto currentTime = Ossia::convert::time(date);

    auto& cstdur = m_iscore_constraint.duration;
    const auto& maxdur = cstdur.maxDuration();

    if(!maxdur.isInfinite())
        cstdur.setPlayPercentage(currentTime / cstdur.maxDuration());
    else
        cstdur.setPlayPercentage(currentTime / cstdur.defaultDuration());
}
}
