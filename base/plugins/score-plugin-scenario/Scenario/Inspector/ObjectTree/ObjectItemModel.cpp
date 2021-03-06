#include "ObjectItemModel.hpp"

#include <Scenario/Process/ScenarioInterface.hpp>
#include <Process/Process.hpp>
#include <Process/ProcessList.hpp>
#include <Process/StateProcess.hpp>
#include <Scenario/Document/Interval/IntervalModel.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/TimeSync/TimeSyncModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Commands/Interval/CreateProcessInNewSlot.hpp>
#include <Scenario/Commands/Interval/RemoveProcessFromInterval.hpp>
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <Process/Style/ScenarioStyle.hpp>
#include <Process/StateProcessFactoryList.hpp>
#include <Scenario/Document/State/ItemModel/MessageItemModel.hpp>
#include <QToolButton>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <score/command/Dispatchers/MacroCommandDispatcher.hpp>
#include <Scenario/Commands/State/AddStateProcess.hpp>
#include <Scenario/Commands/State/RemoveStateProcess.hpp>
#include <Scenario/DialogWidget/AddProcessDialog.hpp>

namespace Scenario
{

ObjectItemModel::ObjectItemModel(QObject* parent)
  : QAbstractItemModel{parent}
{
}

void ObjectItemModel::setSelected(QList<const IdentifiedObjectAbstract*> objs)
{
  QList<const QObject*> root{};
  for(const QObject* sel : objs)
  {
    if(auto cst = dynamic_cast<const Scenario::IntervalModel*>(sel))
    {
      root.push_back(cst);
    }
    else if(auto ev = dynamic_cast<const Scenario::EventModel*>(sel))
    {
      Scenario::ScenarioInterface& scenar = Scenario::parentScenario(*ev);
      root.push_back(&Scenario::parentTimeSync(*ev, scenar));
    }
    else if(auto tn = dynamic_cast<const Scenario::TimeSyncModel*>(sel))
    {
      root.push_back(tn);
    }
    else if(auto st = dynamic_cast<const Scenario::StateModel*>(sel))
    {
      Scenario::ScenarioInterface& scenar = Scenario::parentScenario(*st);
      root.push_back(&Scenario::parentTimeSync(*st, scenar));
    }
    else if(auto stp = dynamic_cast<const Process::StateProcess*>(sel))
    {
      auto state = static_cast<Scenario::StateModel*>(stp->parent());
      Scenario::ScenarioInterface& scenar = Scenario::parentScenario(*state);
      root.push_back(&Scenario::parentTimeSync(*state, scenar));
    }
    else if(auto p = dynamic_cast<const Process::ProcessModel*>(sel))
    {
      root.push_back(p->parent());
    }
  }

  root = root.toSet().toList();
  if(root != m_root)
  {
    cleanConnections();

    beginResetModel();
    m_root = root;
    endResetModel();

    setupConnections();
  }
}

void ObjectItemModel::setupConnections()
{
  if(m_root.empty())
    return;
  m_aliveMap.clear();

  for(auto obj : m_root)
  {
    m_aliveMap.insert(obj, obj);
    bool is_cst = dynamic_cast<const Scenario::IntervalModel*>(obj);
    if(is_cst)
    {
      auto cst = static_cast<const Scenario::IntervalModel*>(obj);
      cst->processes.added.connect<ObjectItemModel, &ObjectItemModel::recompute>(*this);
      cst->processes.removed.connect<ObjectItemModel, &ObjectItemModel::recompute>(*this);
      for(auto& proc : cst->processes)
        m_aliveMap.insert(&proc, &proc);
    }
    else
    {
      auto tn = static_cast<const Scenario::TimeSyncModel*>(obj);
      auto& scenar = Scenario::parentScenario(*tn);
      m_itemCon.push_back(connect(tn, &TimeSyncModel::newEvent, this, [=] { recompute(); }));
      m_itemCon.push_back(connect(tn, &TimeSyncModel::eventRemoved, this, [=] { recompute(); }));

      for(const auto& ev : tn->events())
      {
        auto& e = scenar.event(ev);
        m_aliveMap.insert(&e, &e);
        m_itemCon.push_back(con(e, &EventModel::statesChanged, this, [=] { recompute(); }));
        for(const auto& st : e.states())
        {
          auto& s = scenar.state(st);
          m_aliveMap.insert(&s, &s);
          s.stateProcesses.added.connect<ObjectItemModel, &ObjectItemModel::recompute>(*this);
          s.stateProcesses.removed.connect<ObjectItemModel, &ObjectItemModel::recompute>(*this);

          for(const auto& sp : s.stateProcesses)
            m_aliveMap.insert(&sp, &sp);
        }
      }
    }

    m_itemCon.push_back(
          connect(obj, &QObject::destroyed,
                  this, [=] {
      m_root.removeOne(obj);
      cleanConnections();

      beginResetModel();
      endResetModel();

      setupConnections();
    }));
  }
}

void ObjectItemModel::cleanConnections()
{
  this->removeAll();
  disconnect(m_con);
  for(auto& con : m_itemCon)
    QObject::disconnect(con);
  m_itemCon.clear();
}

QModelIndex ObjectItemModel::index(int row, int column, const QModelIndex& parent) const
{
  auto sel = (QObject*)parent.internalPointer();
  if(isAlive(sel))
  {
    if(auto cst = dynamic_cast<Scenario::IntervalModel*>(sel))
    {
      auto it = cst->processes.begin();
      SCORE_ASSERT(row < cst->processes.size());
      std::advance(it, row);
      return createIndex(row, column, &*(it));
    }
    else if(auto ev = dynamic_cast<Scenario::EventModel*>(sel))
    {
      Scenario::ScenarioInterface& scenar = Scenario::parentScenario(*ev);
      auto it = ev->states().begin();
      SCORE_ASSERT(row < ev->states().size());
      std::advance(it, row);

      if(auto st = scenar.findState(*it))
        return createIndex(row, column, st);
    }
    else if(auto tn = dynamic_cast<Scenario::TimeSyncModel*>(sel))
    {
      Scenario::ScenarioInterface& scenar = Scenario::parentScenario(*tn);
      auto it = tn->events().begin();
      std::advance(it, row);

      if(auto ev = scenar.findEvent(*it))
        return createIndex(row, column, ev);
    }
    else if(auto st = dynamic_cast<Scenario::StateModel*>(sel))
    {
      auto it = st->stateProcesses.begin();
      std::advance(it, row);
      auto& proc = *it;
      return createIndex(row, column, &proc);
    }
    else
    {
      return QModelIndex{};
    }
  }
  else if(!m_root.empty() && row >= 0)
  {
    return createIndex(row, column, (void*)m_root[row]);
  }
  else
  {
    return QModelIndex{};
  }
  return QModelIndex{};
}

bool ObjectItemModel::isAlive(QObject* obj) const
{
  if(!obj)
    return false;

  auto it = m_aliveMap.find(obj);
  if(it != m_aliveMap.end())
  {
    if(it->isNull())
    {
      m_aliveMap.erase(it);
      return false;
    }
    return true;
  }
  return false;
}

QModelIndex ObjectItemModel::parent(const QModelIndex& child) const
{
  auto sel = (QObject*)child.internalPointer();
  if(!isAlive(sel))
    return QModelIndex{};

  if(dynamic_cast<Scenario::IntervalModel*>(sel))
  {
    return QModelIndex{};
  }
  else if(auto ev = dynamic_cast<Scenario::EventModel*>(sel))
  {
    Scenario::ScenarioInterface& scenar = Scenario::parentScenario(*ev);
    auto& tn = Scenario::parentTimeSync(*ev, scenar);
    auto idx = m_root.indexOf(&tn);
    return createIndex(0, 0, (void*)m_root[idx]);
  }
  else if(dynamic_cast<Scenario::TimeSyncModel*>(sel))
  {
    return QModelIndex{};
  }
  else if(auto st = dynamic_cast<Scenario::StateModel*>(sel))
  {
    Scenario::ScenarioInterface& scenar = Scenario::parentScenario(*st);
    auto& tn = Scenario::parentTimeSync(*st, scenar);
    auto it = ossia::find(tn.events(), st->eventId());
    SCORE_ASSERT(it != tn.events().end());
    auto idx = std::distance(tn.events().begin(), it);

    return createIndex(idx, 0, (void*)&Scenario::parentEvent(*st, scenar));
  }
  else if(auto stp = dynamic_cast<Process::StateProcess*>(sel))
  {
    auto state = static_cast<Scenario::StateModel*>(stp->parent());
    Scenario::ScenarioInterface& scenar = Scenario::parentScenario(*state);
    auto& ev = Scenario::parentEvent(*state, scenar);
    auto it = ossia::find(ev.states(), state->id());
    SCORE_ASSERT(it != ev.states().end());
    auto idx = std::distance(ev.states().begin(), it);

    return createIndex(idx, 0, state);
  }
  else if(auto proc = dynamic_cast<Process::ProcessModel*>(sel))
  {
    auto idx = m_root.indexOf(proc->parent());
    return createIndex(idx, 0, (void*)m_root[idx]);
  }

  return QModelIndex{};
}

QVariant ObjectItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(role == Qt::DisplayRole)
  {
    if(section == 0)
    {
      return tr("Object");
    }
    return tr("Info");
  }
  else
  {
    return QAbstractItemModel::headerData(section, orientation, role);
  }
}

int ObjectItemModel::rowCount(const QModelIndex& parent) const
{
  auto sel = (QObject*)parent.internalPointer();
  if(isAlive(sel))
  {
    if(auto cst = dynamic_cast<Scenario::IntervalModel*>(sel))
    {
      return cst->processes.size();
    }
    else if(auto ev = dynamic_cast<Scenario::EventModel*>(sel))
    {
      return ev->states().size();
    }
    else if(auto tn = dynamic_cast<Scenario::TimeSyncModel*>(sel))
    {
      return tn->events().size();
    }
    else if(auto st = dynamic_cast<Scenario::StateModel*>(sel))
    {
      return st->stateProcesses.size();
    }
    else
    {
      return 0;
    }
  }

  return m_root.size();
}

int ObjectItemModel::columnCount(const QModelIndex& parent) const
{
  return 1;
}

QVariant ObjectItemModel::data(const QModelIndex& index, int role) const
{
  auto sel = (QObject*)index.internalPointer();
  if(!isAlive(sel))
    return {};
  if(role == Qt::SizeHintRole)
  {
    return QSize{200, 20};
  }
  if(role == Qt::FontRole)
  {
    return ScenarioStyle::instance().Bold10Pt;
  }

  if(index.column() == 0)
  {
    if(role == Qt::DisplayRole)
    {
      if(auto cst = dynamic_cast<Scenario::IntervalModel*>(sel))
      {
        return cst->metadata().getName();
      }
      else if(auto ev = dynamic_cast<Scenario::EventModel*>(sel))
      {
        return ev->metadata().getName();
      }
      else if(auto tn = dynamic_cast<Scenario::TimeSyncModel*>(sel))
      {
        return tn->metadata().getName();
      }
      else if(auto st = dynamic_cast<Scenario::StateModel*>(sel))
      {
        return st->metadata().getName();
      }
      else if(auto stp = dynamic_cast<Process::StateProcess*>(sel))
      {
        auto name = stp->prettyName();
        if(name.isEmpty())
          name = stp->prettyShortName();
        return name;
      }
      else if(auto p = dynamic_cast<Process::ProcessModel*>(sel))
      {
        auto name = p->prettyName();
        if(name.isEmpty())
          name = p->prettyShortName();
        return name;
      }
    }
    else if(role == Qt::DecorationRole)
    {
      if(dynamic_cast<Scenario::IntervalModel*>(sel))
      {
        static const QIcon icon(":/images/interval.svg");
        return icon;
      }
      else if(auto ev = dynamic_cast<Scenario::EventModel*>(sel))
      {
        if(ev->condition() == State::Expression{})
        {
          static const QIcon icon(":/images/event.svg");
          return icon;
        }
        else
        {
          static const QIcon icon(":/images/cond.svg");
          return icon;
        }
      }
      else if(auto tn = dynamic_cast<Scenario::TimeSyncModel*>(sel))
      {
        if(!tn->active())
        {
          static const QIcon icon(":/images/timenode.svg");
          return icon;
        }
        else
        {
          static const QIcon icon(":/images/trigger.svg");
          return icon;
        }
      }
      else if(auto st = dynamic_cast<Scenario::StateModel*>(sel))
      {
        if(st->messages().rootNode().hasChildren())
        {
          static const QIcon icon(":/images/state.svg");
          return icon;
        }
        else
        {
          static const QIcon icon(":/images/state-empty.svg");
          return icon;
        }
      }
      else if(dynamic_cast<Process::StateProcess*>(sel))
      {
        static const QIcon icon(":/images/process.svg");
        return icon;
      }
      else if(dynamic_cast<Process::ProcessModel*>(sel))
      {
        static const QIcon icon(":/images/process.svg");
        return icon;
      }
    }
    else if(role == Qt::ToolTipRole)
    {
      if(auto cst = dynamic_cast<Scenario::IntervalModel*>(sel))
      {
        return tr("Start : ") + cst->date().toString();
      }
      else if(auto ev = dynamic_cast<Scenario::EventModel*>(sel))
      {
        return ev->condition().toPrettyString();
      }
      else if(auto tn = dynamic_cast<Scenario::TimeSyncModel*>(sel))
      {
        return tn->expression().toPrettyString();
      }
      else if(auto st = dynamic_cast<Scenario::StateModel*>(sel))
      {
        return {};
      }
      else if(auto stp = dynamic_cast<Process::StateProcess*>(sel))
      {
        return {};
      }
      else if(auto p = dynamic_cast<Process::ProcessModel*>(sel))
      {
        return {};
      }
    }
  }
  return {};
}

Qt::ItemFlags ObjectItemModel::flags(const QModelIndex& index) const
{
  return QAbstractItemModel::flags(index);
  Qt::ItemFlags f{};
  f |= Qt::ItemIsSelectable;
  f |= Qt::ItemIsEnabled;
  return f;
}

SelectionStackWidget::SelectionStackWidget(
    score::SelectionStack& s, QWidget* parent)
    : QWidget{parent}, m_stack{s}
{
  m_prev = new QToolButton{this};
  m_prev->setArrowType(Qt::LeftArrow);
  m_prev->setEnabled(m_stack.canUnselect());

  m_next = new QToolButton{this};
  m_next->setArrowType(Qt::RightArrow);
  m_next->setEnabled(m_stack.canReselect());

  auto lay = new score::MarginLess<QHBoxLayout>{this};
  lay->setSizeConstraint(QLayout::SetMinimumSize);
  lay->addWidget(m_prev);
  lay->addWidget(m_next);
  setLayout(lay);

  connect(m_prev, &QToolButton::pressed, [&]() { m_stack.unselect(); });
  connect(m_next, &QToolButton::pressed, [&]() { m_stack.reselect(); });

  con(m_stack, &score::SelectionStack::currentSelectionChanged, this,
      [=] {
    m_prev->setEnabled(m_stack.canUnselect());
    m_next->setEnabled(m_stack.canReselect());
  });
}

ObjectPanelDelegate::ObjectPanelDelegate(const score::GUIApplicationContext &ctx)
  : score::PanelDelegate{ctx}
  , m_widget{new SizePolicyWidget}
  , m_lay{new score::MarginLess<QVBoxLayout>{m_widget}}
{
  m_widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  m_widget->setMinimumHeight(100);
  m_widget->setSizeHint({200, 100});
}

QWidget *ObjectPanelDelegate::widget()
{
  return m_widget;
}

const score::PanelStatus &ObjectPanelDelegate::defaultPanelStatus() const
{
  static const score::PanelStatus status{true, Qt::RightDockWidgetArea, 12,
        QObject::tr("Objects"),
        QObject::tr("Ctrl+Shift+O")};

  return status;
}

void ObjectPanelDelegate::on_modelChanged(score::MaybeDocument oldm, score::MaybeDocument newm)
{
  using namespace score;
  delete m_objects;
  m_objects = nullptr;

  delete m_stack;
  m_stack = nullptr;
  if (newm)
  {
    SelectionStack& stack = newm->selectionStack;
    m_stack = new SelectionStackWidget{stack, m_widget};

    m_objects = new ObjectWidget{*newm, m_widget};

    m_objects->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);

    m_lay->addWidget(m_stack);
    m_lay->addWidget(m_objects);

    setNewSelection(stack.currentSelection());
  }
}

void ObjectPanelDelegate::setNewSelection(const Selection &sel)
{
  if (m_objects)
  {
    m_objects->model.setSelected(sel.toList());
    m_objects->expandAll();

    auto cur_sel = document()->selectionStack.currentSelection();
    auto idx = m_objects->model.index(0, 0, {});

    auto selection = m_objects->selectionModel();
    m_objects->updatingSelection = true;
    while(idx.isValid())
    {
      auto ptr = idx.internalPointer();
      if(cur_sel.contains((IdentifiedObjectAbstract*)ptr))
      {
        selection->select(idx, QItemSelectionModel::Select | QItemSelectionModel::Rows);
      }
      else
      {
        selection->select(idx, QItemSelectionModel::Deselect | QItemSelectionModel::Rows);
      }
      idx = m_objects->indexBelow(idx);
    }
    m_objects->updatingSelection = false;

    m_objects->header()->resizeSection(1, QHeaderView::Stretch);
    m_objects->header()->resizeSection(0, QHeaderView::ResizeToContents);
    if(m_objects->header()->sectionSize(0) < 140)
      m_objects->header()->resizeSection(0, 140);

  }
}

ObjectWidget::ObjectWidget(const score::DocumentContext& ctx, QWidget* par)
  : QTreeView{par}
  , model{this}
  , m_ctx{ctx}
{
  setModel(&model);
  setAnimated(true);
  setAlternatingRowColors(true);
  setMidLineWidth(40);
  setUniformRowHeights(true);
  setWordWrap(false);
  setMouseTracking(true);

  con(model, &ObjectItemModel::changed,
      this, &QTreeView::expandAll);
}

void ObjectWidget::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
  if((selected.size() > 0 || deselected.size() > 0) && !updatingSelection)
  {
    score::SelectionDispatcher d{m_ctx.selectionStack};
    auto sel = this->selectedIndexes();
    if(!sel.empty())
    {
      auto obj = (IdentifiedObjectAbstract*) sel.at(0).internalPointer();
      d.setAndCommit(Selection{obj});
    }
    else
    {
      d.setAndCommit({});
    }
  }
}

void ObjectWidget::contextMenuEvent(QContextMenuEvent* ev)
{
  auto point = ev->pos();
  auto index = indexAt(point);
  if (index.isValid())
  {
    auto ptr = (QObject*)index.internalPointer();
    if(!ptr)
      return;

    QMenu* m = new QMenu{this};

    if(auto cst = dynamic_cast<Scenario::IntervalModel*>(ptr))
    {
      auto addproc = new QAction{tr("Add process"), m};
      m->addAction(addproc);
      connect(addproc, &QAction::triggered, this, [=] {

        auto& fact = m_ctx.app.interfaces<Process::ProcessFactoryList>();
        auto dialog = new AddProcessDialog{fact, this};

        QObject::connect(
            dialog, &AddProcessDialog::okPressed, this, [&] (const auto& proc) {
              using cmd = Scenario::Command::CreateProcessInNewSlot;
              QuietMacroCommandDispatcher<cmd> disp{m_ctx.commandStack};

              cmd::create(disp, *cst, proc);

              disp.commit();
            });

        dialog->launchWindow();
        dialog->deleteLater();
      });
    }
    else if(auto state = dynamic_cast<Scenario::StateModel*>(ptr))
    {
      auto addproc = new QAction{tr("Add process"), m};
      m->addAction(addproc);
      connect(addproc, &QAction::triggered, this, [=] {

        auto& fact = m_ctx.app.interfaces<Process::StateProcessList>();
        auto dialog = new AddStateProcessDialog{fact, this};

        QObject::connect(
            dialog, &AddStateProcessDialog::okPressed, this, [&] (const auto& proc) {
              CommandDispatcher<> disp{m_ctx.commandStack};
              disp.submitCommand<Scenario::Command::AddStateProcessToState>(*state, proc);
            });

        dialog->launchWindow();
        dialog->deleteLater();
      });
    }
    else if(auto proc = dynamic_cast<Process::ProcessModel*>(ptr))
    {
      auto deleteact = new QAction{tr("Remove"), m};
      m->addAction(deleteact);
      connect(deleteact, &QAction::triggered, this, [=] {
        CommandDispatcher<> c{m_ctx.commandStack};
        c.submitCommand<Scenario::Command::RemoveProcessFromInterval>(*(IntervalModel*)proc->parent(), proc->id());
      });
    }
    else if(auto stp = dynamic_cast<Process::StateProcess*>(ptr))
    {
      auto deleteact = new QAction{tr("Remove"), m};
      m->addAction(deleteact);
      connect(deleteact, &QAction::triggered, this, [=] {
        CommandDispatcher<> c{m_ctx.commandStack};
        c.submitCommand<Scenario::Command::RemoveStateProcess>(*(StateModel*)stp->parent(), stp->id());
      });
    }

    m->exec(mapToGlobal(point));
    m->deleteLater();
  }
}

}
