// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QCheckBox>
#include <QDateTime>
#include <QGridLayout>
#include <QLabel>
#include <QStackedLayout>
#include <QString>
#include <QWidget>
#include <Scenario/Commands/Interval/SetMaxDuration.hpp>
#include <Scenario/Commands/Interval/SetMinDuration.hpp>
#include <Scenario/Document/Interval/IntervalModel.hpp>
#include <Scenario/Document/DisplayedElements/DisplayedElementsPresenter.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentModel.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentPresenter.hpp>
#include <Scenario/Inspector/Interval/IntervalInspectorWidget.hpp>
#include <chrono>
#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>
#include <core/document/DocumentPresenter.hpp>
#include <score/document/DocumentContext.hpp>
#include <score/tools/std/Optional.hpp>
#include <score/widgets/TextLabel.hpp>
#include <qnamespace.h>

#include "DurationSectionWidget.hpp"
#include <Inspector/InspectorSectionWidget.hpp>
#include <Process/TimeValue.hpp>
#include <Scenario/Application/ScenarioEditionSettings.hpp>
#include <Scenario/Document/Interval/IntervalDurations.hpp>
#include <Scenario/Inspector/Interval/IntervalInspectorDelegate.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <score/command/Dispatchers/OngoingCommandDispatcher.hpp>
#include <score/model/path/Path.hpp>
#include <score/model/path/PathSerialization.hpp>
#include <score/model/Identifier.hpp>
#include <score/tools/Todo.hpp>
#include <score/widgets/MarginLess.hpp>
#include <score/widgets/SpinBoxes.hpp>

namespace Scenario
{
class EditionGrid : public QWidget
{
public:
  EditionGrid(
      const IntervalModel& m,
      const score::DocumentContext& fac,
      const Scenario::EditionSettings& set,
      const IntervalInspectorDelegate& delegate)
      : m_model{m}
      , m_dur{m.duration}
      , m_editionSettings{set}
      , m_delegate{delegate}
      , m_dispatcher{fac.commandStack}
      , m_simpleDispatcher{fac.commandStack}
  {
    using namespace score;
    auto editableGrid = new score::MarginLess<QGridLayout>{this};

    // SPINBOXES
    m_minSpin = new TimeSpinBox{this};
    m_maxSpin = new TimeSpinBox{this};
    m_valueSpin = new TimeSpinBox{this};
    m_maxSpin->setTime(m_dur.maxDuration().toQTime());
    m_minSpin->setTime(m_dur.minDuration().toQTime());
    m_valueSpin->setTime(m_dur.defaultDuration().toQTime());

    // CHECKBOXES
    m_minNonNullBox = new QCheckBox{};
    m_maxFiniteBox = new QCheckBox{};

    m_minNonNullBox->setChecked(!m_dur.isMinNull());
    m_maxFiniteBox->setChecked(!m_dur.isMaxInfinite());

    connect(
        m_minNonNullBox, &QCheckBox::toggled, this,
        &EditionGrid::on_minNonNullToggled);

    connect(
        m_maxFiniteBox, &QCheckBox::toggled, this,
        &EditionGrid::on_maxFiniteToggled);

    // DISPLAY
    m_minNull = new TextLabel{tr("Null")};
    m_minNull->hide();
    m_maxInfinity = new TextLabel{tr("Infinity")};
    m_maxInfinity->hide();

    auto dur = new TextLabel(tr("Default Duration"));
    editableGrid->addWidget(dur, 0, 1, 1, 1);
    editableGrid->addWidget(m_valueSpin, 0, 2, 1, 1);

    m_minTitle = new TextLabel(tr("Min Duration"));
    editableGrid->addWidget(m_minNonNullBox, 1, 0, 1, 1);
    editableGrid->addWidget(m_minTitle, 1, 1, 1, 1);
    editableGrid->addWidget(m_minSpin, 1, 2, 1, 1);
    editableGrid->addWidget(m_minNull, 1, 2, 1, 1);

    m_maxTitle = new TextLabel(tr("Max Duration"));
    editableGrid->addWidget(m_maxFiniteBox, 2, 0, 1, 1);
    editableGrid->addWidget(m_maxTitle, 2, 1, 1, 1);
    editableGrid->addWidget(m_maxSpin, 2, 2, 1, 1);
    editableGrid->addWidget(m_maxInfinity, 2, 2, 1, 1);

    editableGrid->setColumnStretch(0, 0);
    editableGrid->setColumnStretch(1, 1);
    editableGrid->setColumnStretch(2, 2);

    connect(
        m_valueSpin, &TimeSpinBox::editingFinished, this,
        &EditionGrid::on_durationsChanged);
    connect(
        m_minSpin, &TimeSpinBox::editingFinished, this,
        &EditionGrid::on_durationsChanged);
    connect(
        m_maxSpin, &TimeSpinBox::editingFinished, this,
        &EditionGrid::on_durationsChanged);

    m_min = m_model.duration.minDuration();
    m_max = m_model.duration.maxDuration();

    con(m_dur, &IntervalDurations::minNullChanged, this,
        &EditionGrid::on_modelMinNullChanged);
    con(m_dur, &IntervalDurations::maxInfiniteChanged, this,
        &EditionGrid::on_modelMaxInfiniteChanged);

    m_minNull->setVisible(m_model.duration.isMinNull());
    m_minSpin->setVisible(!m_model.duration.isMinNull());

    m_maxInfinity->setVisible(m_model.duration.isMaxInfinite());
    m_maxSpin->setVisible(!m_model.duration.isMaxInfinite());

    on_modelRigidityChanged(m_model.duration.isRigid());

    // We disable changing duration for the full view unless
    // it is the top-level one, because else
    // it may cause unexpected changes in parent scenarios.
    auto mod_del = dynamic_cast<Scenario::ScenarioDocumentModel*>(
        &fac.document.model().modelDelegate());
    if (isInFullView(m_model) && &m_model != &mod_del->baseInterval())
    {
      m_valueSpin->setEnabled(false);
    }
  }

  void defaultDurationSpinboxChanged(int val)
  {
    m_delegate.on_defaultDurationChanged(
        m_dispatcher,
        TimeVal::fromMsecs(val),
        m_editionSettings.expandMode());
  }

  void on_modelRigidityChanged(bool b)
  {
    m_minNonNullBox->setHidden(b);
    m_minSpin->setHidden(b);
    m_minTitle->setHidden(b);

    m_maxSpin->setHidden(b);
    m_maxFiniteBox->setHidden(b);
    m_maxTitle->setHidden(b);
  }

  void on_modelMinNullChanged(bool b)
  {
    m_minNull->setVisible(b);
    {
      QSignalBlocker block{m_minNonNullBox};
      m_minNonNullBox->setChecked(!b);
    }
    m_minSpin->setVisible(!b);
  }

  void on_modelMaxInfiniteChanged(bool b)
  {
    m_maxInfinity->setVisible(b);
    {
      QSignalBlocker block{m_maxFiniteBox};
      m_maxFiniteBox->setChecked(!b);
    }
    m_maxSpin->setVisible(!b);
  }

  void on_modelDefaultDurationChanged(const TimeVal& dur)
  {
    if (dur.toQTime() == m_valueSpin->time())
      return;

    m_valueSpin->setTime(dur.toQTime());
  }

  void on_modelMinDurationChanged(const TimeVal& dur)
  {
    if (dur.toQTime() == m_minSpin->time())
      return;

    m_minSpin->setTime(dur.toQTime());
    m_min = dur;
  }

  void on_modelMaxDurationChanged(const TimeVal& dur)
  {
    if (dur.toQTime() == m_maxSpin->time())
      return;

    m_maxSpin->setTime(dur.toQTime());
    m_max = dur;
  }

  void on_durationsChanged()
  {
    if (m_dur.defaultDuration().toQTime() != m_valueSpin->time())
    {
      defaultDurationSpinboxChanged(
          m_valueSpin->time().msecsSinceStartOfDay());
      m_dispatcher.commit();
    }

    if (m_dur.minDuration().toQTime() != m_minSpin->time())
    {
      minDurationSpinboxChanged(m_minSpin->time().msecsSinceStartOfDay());
      m_dispatcher.commit();
    }
    if (m_dur.maxDuration().toQTime() != m_maxSpin->time())
    {
      maxDurationSpinboxChanged(m_maxSpin->time().msecsSinceStartOfDay());
      m_dispatcher.commit();
    }
  }

  void on_minNonNullToggled(bool val)
  {
    m_minSpin->setVisible(val);
    m_minNull->setVisible(!val);

    m_min = !val ? m_dur.minDuration() : m_min;

    auto cmd = new Scenario::Command::SetMinDuration(m_model, m_min, !val);

    m_simpleDispatcher.submitCommand(cmd);
  }

  void on_maxFiniteToggled(bool val)
  {
    m_maxSpin->setVisible(val);
    m_maxInfinity->setVisible(!val);

    m_max = !val ? m_dur.maxDuration() : m_dur.defaultDuration() * 1.2;

    auto cmd = new Scenario::Command::SetMaxDuration(m_model, m_max, !val);

    m_simpleDispatcher.submitCommand(cmd);
  }

  void minDurationSpinboxChanged(int val)
  {
    using namespace Scenario::Command;
    m_dispatcher.submitCommand<SetMinDuration>(
        m_model,
        TimeVal{std::chrono::milliseconds{val}},
        !m_minNonNullBox->isChecked());
  }

  void maxDurationSpinboxChanged(int val)
  {
    using namespace Scenario::Command;
    m_dispatcher.submitCommand<SetMaxDuration>(
        m_model,
        TimeVal{std::chrono::milliseconds{val}},
        !m_maxFiniteBox->isChecked());
  }

  const IntervalModel& m_model;
  const IntervalDurations& m_dur;
  const Scenario::EditionSettings& m_editionSettings;
  const IntervalInspectorDelegate& m_delegate;

  QLabel* m_maxTitle{};
  QLabel* m_minTitle{};
  QLabel* m_maxInfinity{};
  QLabel* m_minNull{};

  score::TimeSpinBox* m_minSpin{};
  score::TimeSpinBox* m_valueSpin{};
  score::TimeSpinBox* m_maxSpin{};

  QCheckBox* m_minNonNullBox{};
  QCheckBox* m_maxFiniteBox{};

  TimeVal m_max;
  TimeVal m_min;

  OngoingCommandDispatcher m_dispatcher;
  CommandDispatcher<> m_simpleDispatcher;
};

class PlayGrid : public QWidget
{
public:
  PlayGrid(const IntervalDurations& dur) : m_dur{dur}
  {
    auto playingGrid = new score::MarginLess<QGridLayout>(this);

    m_minLab = new TextLabel{m_dur.minDuration().toString(), this};
    m_maxLab = new TextLabel{m_dur.maxDuration().toString(), this};
    m_defaultLab = new TextLabel{m_dur.defaultDuration().toString(), this};
    m_currentPosLab = new TextLabel{this};

    on_progress(m_dur.playPercentage());

    playingGrid->addWidget(new TextLabel(tr("Default Duration")), 0, 0, 1, 1);
    playingGrid->addWidget(m_defaultLab, 0, 1, 1, 1);
    playingGrid->addWidget(new TextLabel(tr("Min Duration")), 1, 0, 1, 1);
    playingGrid->addWidget(m_minLab, 1, 1, 1, 1);
    playingGrid->addWidget(new TextLabel(tr("Max Duration")), 2, 0, 1, 1);
    playingGrid->addWidget(m_maxLab, 2, 1, 1, 1);
    playingGrid->addWidget(new TextLabel{tr("Progress")}, 3, 0, 1, 1);
    playingGrid->addWidget(m_currentPosLab, 3, 1, 1, 1);

    con(m_dur, &IntervalDurations::playPercentageChanged, this,
        &PlayGrid::on_progress);
  }

  void on_progress(double p)
  {
    auto coeff = m_dur.isMaxInfinite() ? m_dur.defaultDuration().msec()
                                       : m_dur.maxDuration().msec();
    m_currentPosLab->setText(
        QString::number(p * coeff / 1000) + QString(" s"));
  }

  void on_modelRigidityChanged(bool b)
  {

    m_minLab->setHidden(b);
    m_maxLab->setHidden(b);
  }

  void on_modelDefaultDurationChanged(const TimeVal& dur)
  {
    m_defaultLab->setText(dur.toString());
  }

  void on_modelMinDurationChanged(const TimeVal& dur)
  {
    m_minLab->setText(dur.toString());
  }

  void on_modelMaxDurationChanged(const TimeVal& dur)
  {
    m_maxLab->setText(dur.toString());
  }

private:
  const IntervalDurations& m_dur;

  QLabel* m_maxLab{};
  QLabel* m_minLab{};
  QLabel* m_defaultLab{};
  QLabel* m_currentPosLab{};
};

DurationWidget::DurationWidget(
    const Scenario::EditionSettings& set,
    const IntervalInspectorDelegate& delegate,
    IntervalInspectorWidget* parent)
    : QWidget{parent}
    , m_editingWidget{new EditionGrid{parent->model(), parent->context(), set,
                                      delegate}}
    , m_playingWidget{new PlayGrid{parent->model().duration}}
{
  using namespace score;
  auto mainWidg = this;
  auto mainLay = new score::MarginLess<QStackedLayout>{mainWidg};
  auto& model = parent->model();
  auto& dur = model.duration;

  // CONNECTIONS FROM MODEL
  // TODO these need to be updated when the default duration changes
  con(dur, &IntervalDurations::defaultDurationChanged, this,
      [](const TimeVal& t) {

      });
  con(dur, &IntervalDurations::minDurationChanged, this, [this](auto v) {
    m_playingWidget->on_modelMinDurationChanged(v);
    m_editingWidget->on_modelMinDurationChanged(v);
  });
  con(dur, &IntervalDurations::maxDurationChanged, this, [this](auto v) {
    m_playingWidget->on_modelMaxDurationChanged(v);
    m_editingWidget->on_modelMaxDurationChanged(v);
  });
  con(dur, &IntervalDurations::rigidityChanged, this, [this](auto v) {
    m_playingWidget->on_modelRigidityChanged(v);
    m_editingWidget->on_modelRigidityChanged(v);
  });

  con(set, &EditionSettings::toolChanged, this, [=](Scenario::Tool t) {
    mainLay->setCurrentWidget(
        t == Tool::Playing ? (QWidget*)m_playingWidget
                           : (QWidget*)m_editingWidget);
  });

  mainLay->addWidget(m_playingWidget);
  mainLay->addWidget(m_editingWidget);

  mainLay->setCurrentWidget(
      set.tool() == Tool::Playing ? (QWidget*)m_playingWidget
                                  : (QWidget*)m_editingWidget);
}
}
