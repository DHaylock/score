#include <Scenario/Commands/Constraint/SetMaxDuration.hpp>
#include <Scenario/Commands/Constraint/SetMinDuration.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/Constraint/ViewModels/FullView/FullViewConstraintViewModel.hpp>
#include <Scenario/Inspector/Constraint/ConstraintInspectorWidget.hpp>
#include <boost/optional/optional.hpp>
#include <QCheckBox>
#include <QDateTime>
#include <QGridLayout>
#include <QLabel>
#include <qnamespace.h>
#include <QString>
#include <QWidget>
#include <chrono>

#include "DurationSectionWidget.hpp"
#include <Inspector/InspectorSectionWidget.hpp>
#include <Process/TimeValue.hpp>
#include <Scenario/Application/ScenarioEditionSettings.hpp>
#include <Scenario/Document/Constraint/ConstraintDurations.hpp>
#include <Scenario/Inspector/Constraint/ConstraintInspectorDelegate.hpp>
#include <iscore/command/Dispatchers/CommandDispatcher.hpp>
#include <iscore/command/Dispatchers/OngoingCommandDispatcher.hpp>
#include <iscore/tools/ModelPath.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include <iscore/tools/Todo.hpp>
#include <iscore/widgets/SpinBoxes.hpp>
#include <iscore/widgets/MarginLess.hpp>
#include <iscore/tools/ModelPathSerialization.hpp>

namespace Scenario
{
DurationSectionWidget::DurationSectionWidget(
        const Scenario::EditionSettings& set,
        const ConstraintInspectorDelegate& delegate,
        ConstraintInspectorWidget* parent):
    InspectorSectionWidget {"Durations", false, parent},
    m_model {parent->model()},
    m_parent {parent}, // TODO parent should have a cref to commandStack ?
    m_dispatcher{parent->commandDispatcher()->stack()},
    m_editionSettings{set},
    m_delegate{delegate}
{
    using namespace iscore;
    auto mainWidg = new QWidget{this};
    auto mainLay = new iscore::MarginLess<QVBoxLayout>{mainWidg};
    m_editingWidget = new QWidget{mainWidg};

    auto editableGrid = new iscore::MarginLess<QGridLayout>{m_editingWidget};

    // SPINBOXES
    m_minSpin = new TimeSpinBox{this};
    m_maxSpin = new TimeSpinBox{this};
    m_valueSpin = new TimeSpinBox{this};
    m_maxSpin->setTime(m_model.duration.maxDuration().toQTime());
    m_minSpin->setTime(m_model.duration.minDuration().toQTime());
    m_valueSpin->setTime(m_model.duration.defaultDuration().toQTime());


    // CHECKBOXES
    m_minNonNullBox = new QCheckBox{};
    m_maxFiniteBox = new QCheckBox{};

    m_minNonNullBox->setChecked(!m_model.duration.isMinNul());
    m_maxFiniteBox->setChecked(!m_model.duration.isMaxInfinite());

    connect(m_minNonNullBox, &QCheckBox::toggled,
            this, &DurationSectionWidget::on_minNonNullToggled);

    connect(m_maxFiniteBox, &QCheckBox::toggled,
            this, &DurationSectionWidget::on_maxFiniteToggled);

    // CONNECTIONS FROM MODEL
    // TODO these need to be updated when the default duration changes
    con(m_model.duration, &ConstraintDurations::defaultDurationChanged,
            this, &DurationSectionWidget::on_modelDefaultDurationChanged);
    con(m_model.duration, &ConstraintDurations::minDurationChanged,
            this, &DurationSectionWidget::on_modelMinDurationChanged);
    con(m_model.duration, &ConstraintDurations::maxDurationChanged,
            this, &DurationSectionWidget::on_modelMaxDurationChanged);
    con(m_model.duration, &ConstraintDurations::rigidityChanged,
            this, &DurationSectionWidget::on_modelRigidityChanged);
    con(m_model.duration, &ConstraintDurations::minNullChanged,
            this, &DurationSectionWidget::on_modelMinNullChanged);
    con(m_model.duration, &ConstraintDurations::maxInfiniteChanged,
            this, &DurationSectionWidget::on_modelMaxInfiniteChanged);
    con(m_model.duration, &ConstraintDurations::playPercentageChanged,
        this, &DurationSectionWidget::on_progress);

    con(m_editionSettings, &EditionSettings::toolChanged,
        this, &DurationSectionWidget::on_execution);

    // DISPLAY
    m_minNull = new QLabel{tr("Null")};
    m_minNull->hide();
    m_maxInfinity = new QLabel{tr("Infinity")};
    m_maxInfinity->hide();
    auto valLab = new QLabel("Default Duration");
    editableGrid->addWidget(valLab, 0,1,1,1);
    editableGrid->addWidget(m_valueSpin, 0, 2, 1, 1);

    m_minTitle = new QLabel("Min Duration");
    editableGrid->addWidget(m_minNonNullBox, 1, 0, 1, 1);
    editableGrid->addWidget(m_minTitle, 1, 1, 1, 1);
    editableGrid->addWidget(m_minSpin, 1, 2, 1, 1);
    editableGrid->addWidget(m_minNull, 1, 2, 1, 1);

    m_maxTitle = new QLabel("Max Duration");
    editableGrid->addWidget(m_maxFiniteBox, 2, 0, 1, 1);
    editableGrid->addWidget(m_maxTitle, 2, 1, 1,1);
    editableGrid->addWidget(m_maxSpin, 2, 2, 1, 1);
    editableGrid->addWidget(m_maxInfinity, 2, 2, 1, 1);

    editableGrid->setColumnStretch(0,0);
    editableGrid->setColumnStretch(1,1);
    editableGrid->setColumnStretch(2,2);

    connect(m_valueSpin,    &TimeSpinBox::editingFinished,
            this,   &DurationSectionWidget::on_durationsChanged);
    connect(m_minSpin,  &TimeSpinBox::editingFinished,
            this,   &DurationSectionWidget::on_durationsChanged);
    connect(m_maxSpin,  &TimeSpinBox::editingFinished,
            this,   &DurationSectionWidget::on_durationsChanged);


    m_playingWidget = new QWidget{mainWidg};
    auto playingGrid = new iscore::MarginLess<QGridLayout>(m_playingWidget);

    m_minLab = new QLabel{m_model.duration.minDuration().toString(), this};
    m_maxLab = new QLabel{m_model.duration.maxDuration().toString(), this};
    m_defaultLab = new QLabel{m_model.duration.defaultDuration().toString(), this};
    m_currentPosLab = new QLabel{this};

    on_progress(m_model.duration.playPercentage());

    playingGrid->addWidget(valLab, 0, 0, 1, 1);
    playingGrid->addWidget(m_defaultLab, 0, 1, 1, 1);
    playingGrid->addWidget(m_minTitle, 1, 0, 1, 1);
    playingGrid->addWidget(m_minLab, 1, 1, 1, 1);
    playingGrid->addWidget(m_maxTitle, 2, 0, 1, 1);
    playingGrid->addWidget(m_maxLab, 2, 1, 1, 1);
    playingGrid->addWidget(new QLabel{tr("progress")}, 3, 0, 1, 1);
    playingGrid->addWidget(m_currentPosLab, 3, 1, 1, 1);

    mainLay->addWidget(m_playingWidget);
    mainLay->addWidget(m_editingWidget);

    addContent(mainWidg);

    if(m_model.fullView()->isActive() && m_model.id().val() != 0)
    {
        m_valueSpin->setEnabled(false);
    }

    m_min = m_model.duration.minDuration();
    m_max = m_model.duration.maxDuration();

    m_minNull->setVisible(m_model.duration.isMinNul());
    m_minSpin->setVisible(!m_model.duration.isMinNul());

    m_maxInfinity->setVisible(m_model.duration.isMaxInfinite());
    m_maxSpin->setVisible(!m_model.duration.isMaxInfinite());

    on_modelRigidityChanged(m_model.duration.isRigid());
    on_execution(m_editionSettings.tool());
}

using namespace Scenario::Command;

void DurationSectionWidget::minDurationSpinboxChanged(int val)
{
    m_dispatcher.submitCommand<SetMinDuration>(
                m_model,
                TimeValue{std::chrono::milliseconds{val}},
                !m_minNonNullBox->isChecked());
}

void DurationSectionWidget::maxDurationSpinboxChanged(int val)
{
    m_dispatcher.submitCommand<SetMaxDuration>(
                m_model,
                TimeValue{std::chrono::milliseconds {val}},
                !m_maxFiniteBox->isChecked());
}

void DurationSectionWidget::defaultDurationSpinboxChanged(int val)
{
    m_delegate.on_defaultDurationChanged(
                m_dispatcher,
                TimeValue::fromMsecs(val),
                m_editionSettings.expandMode());
}

void DurationSectionWidget::on_modelRigidityChanged(bool b)
{
    m_minNonNullBox->setHidden(b);
    m_minSpin->setHidden(b);
    m_minTitle->setHidden(b);

    m_maxSpin->setHidden(b);
    m_maxFiniteBox->setHidden(b);
    m_maxTitle->setHidden(b);

    m_minLab->setHidden(b);
    m_maxLab->setHidden(b);
}

void DurationSectionWidget::on_modelMinNullChanged(bool b)
{
    m_minNull->setVisible(b);
    m_minNonNullBox->setChecked(!b);
    m_minSpin->setVisible(!b);
}

void DurationSectionWidget::on_modelMaxInfiniteChanged(bool b)
{
    m_maxInfinity->setVisible(b);
    m_maxFiniteBox->setChecked(!b);
    m_maxSpin->setVisible(!b);
}

void DurationSectionWidget::on_modelDefaultDurationChanged(const TimeValue& dur)
{
    m_defaultLab->setText(dur.toString());
    if (dur.toQTime() == m_valueSpin->time())
        return;

    m_valueSpin->setTime(dur.toQTime());
}

void DurationSectionWidget::on_modelMinDurationChanged(const TimeValue& dur)
{
    m_minLab->setText(dur.toString());
    if (dur.toQTime() == m_minSpin->time())
        return;

    m_minSpin->setTime(dur.toQTime());
}

void DurationSectionWidget::on_modelMaxDurationChanged(const TimeValue& dur)
{
    m_maxLab->setText(dur.toString());
    if (dur.toQTime() == m_maxSpin->time())
        return;

    m_maxSpin->setTime(dur.toQTime());
}

void DurationSectionWidget::on_durationsChanged()
{
    if (m_model.duration.defaultDuration().toQTime() != m_valueSpin->time())
    {
        defaultDurationSpinboxChanged(m_valueSpin->time().msecsSinceStartOfDay());
        m_dispatcher.commit();
    }

    if (m_model.duration.minDuration().toQTime() != m_minSpin->time())
    {
        minDurationSpinboxChanged(m_minSpin->time().msecsSinceStartOfDay());
        m_dispatcher.commit();

    }if (m_model.duration.maxDuration().toQTime() != m_maxSpin->time())
    {
        maxDurationSpinboxChanged(m_maxSpin->time().msecsSinceStartOfDay());
        m_dispatcher.commit();
    }

}

void DurationSectionWidget::on_minNonNullToggled(bool val)
{
    m_minSpin->setVisible(val);
    m_minNull->setVisible(!val);

    m_min = !val ? m_model.duration.minDuration() : m_min;

    auto cmd = new Scenario::Command::SetMinDuration(
                   m_model,
                   m_min,
                   !val);


    m_parent->commandDispatcher()->submitCommand(cmd);
}

void DurationSectionWidget::on_maxFiniteToggled(bool val)
{
    m_maxSpin->setVisible(val);
    m_maxInfinity->setVisible(!val);

    m_max = !val ? m_model.duration.maxDuration() : m_model.duration.defaultDuration() * 1.2;


    auto cmd = new Scenario::Command::SetMaxDuration(
                   m_model,
                   m_max,
                   !val);


    m_parent->commandDispatcher()->submitCommand(cmd);
}

void DurationSectionWidget::on_execution(Scenario::Tool t)
{
    m_playingWidget->setVisible(t == Tool::Playing);
    m_editingWidget->setVisible(t != Tool::Playing);
}

void DurationSectionWidget::on_progress(double p)
{
    auto coeff = m_model.duration.isMaxInfinite() ?
                     m_model.duration.defaultDuration().msec() :
                     m_model.duration.maxDuration().msec();
    m_currentPosLab->setText(QString::number((p * coeff / 1000)) + QString(" s"));
}

DurationSectionWidget::~DurationSectionWidget()
{
}

}
