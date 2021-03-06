// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <Engine/Executor/DocumentPlugin.hpp>
#include <Engine/OSSIA2score.hpp>
#include <Engine/score2OSSIA.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <vector>

#include "Component.hpp"
#include "JSAPIWrapper.hpp"
#include <ossia/detail/logger.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/state/message.hpp>
#include <ossia/editor/state/state.hpp>
#include <JS/JSProcessModel.hpp>
#include <QQmlComponent>

namespace JS
{
namespace Executor
{
ProcessExecutor::ProcessExecutor(const Explorer::DeviceDocumentPlugin& devices)
  : m_devices{devices.list()}
{
  auto obj = m_engine.newQObject(new JS::APIWrapper{m_engine, devices});
  m_engine.globalObject().setProperty("score", obj);
}

void ProcessExecutor::setTickFun(const QString& val)
{
  if(val.startsWith("import"))
  {
    // QML case
    QQmlComponent c{&m_engine};
    c.setData(val.toUtf8(), QUrl());
    const auto& errs = c.errors();
    if(!errs.empty())
    {
      ossia::logger()
          .error("Uncaught exception at line {} : {}",
                 errs[0].line(),
                 errs[0].toString().toStdString());
    }
    else
    {
      m_object = c.create();
      if(m_object)
        m_object->setParent(&m_engine);
    }
  }
  else
  {
    // JS case
    m_tickFun = m_engine.evaluate(val);
    if (m_tickFun.isError())
      ossia::logger()
          .error("Uncaught exception at line {} : {}",
                 m_tickFun.property("lineNumber").toInt() ,
                 m_tickFun.toString().toStdString());
  }
}

ossia::state_element ProcessExecutor::state(ossia::time_value date, double pos)
{
  return state(pos);
}

ossia::state_element ProcessExecutor::state(double t)
{
  if (m_tickFun.isCallable())
  {
    ossia::state st;

    // 2. Get the value of the js fun
    auto res = m_tickFun.call({QJSValue{t}});

    if(unmuted())
    {
      auto messages = JS::convert::messages(res);

      m_engine.collectGarbage();

      for (const auto& mess : messages)
      {
        st.add(Engine::score_to_ossia::message(mess, m_devices));
      }

      return st;
    }
  }
  else if(m_object)
  {
    QVariant ret;
    QMetaObject::invokeMethod(m_object, "onTick", Qt::DirectConnection, Q_RETURN_ARG(QVariant, ret), Q_ARG(QVariant, t));
    if(ret.canConvert<QJSValue>() && unmuted())
    {
      ossia::state st;
      auto messages = JS::convert::messages(ret.value<QJSValue>());

      m_engine.collectGarbage();

      for (const auto& mess : messages)
      {
        st.add(Engine::score_to_ossia::message(mess, m_devices));
      }

      return st;
    }
  }

  return {};
}

Component::Component(
    ::Engine::Execution::IntervalComponent& parentInterval,
    JS::ProcessModel& element,
    const ::Engine::Execution::Context& ctx,
    const Id<score::Component>& id,
    QObject* parent)
  : ::Engine::Execution::
      ProcessComponent_T<JS::ProcessModel, ProcessExecutor>{
        parentInterval, element, ctx, id, "JSComponent", parent}
{
  m_ossia_process = std::make_shared<ProcessExecutor>(ctx.devices);
  OSSIAProcess().setTickFun(element.script());

  con(element, &JS::ProcessModel::scriptChanged,
      this, [=] (const QString& str) {
    system().executionQueue.enqueue(
          [proc=std::dynamic_pointer_cast<ProcessExecutor>(m_ossia_process),
          &str]
    { proc->setTickFun(str); });
  });
}
}
}
