#include <Process/LayerModel.hpp>
#include <Process/LayerPresenter.hpp>
#include <Process/Process.hpp>
#include "ProcessFocusManager.hpp"
namespace Process
{

const ProcessModel* ProcessFocusManager::focusedModel() const
{
    return m_currentModel;
}


const LayerModel* ProcessFocusManager::focusedViewModel() const
{
    return m_currentViewModel;
}


LayerPresenter* ProcessFocusManager::focusedPresenter() const
{
    return m_currentPresenter;
}


void ProcessFocusManager::focus(LayerPresenter* p)
{
    if(p == m_currentPresenter)
        return;

    if(m_currentPresenter)
    {
        defocusPresenter(m_currentPresenter);
    }
    if(m_currentViewModel)
    {
        emit sig_defocusedViewModel(m_currentViewModel);
    }

    m_currentPresenter = p;

    if(m_currentPresenter)
    {
        m_currentViewModel = &m_currentPresenter->layerModel();
        m_currentModel = &m_currentViewModel->processModel();

        emit sig_focusedViewModel(m_currentViewModel);

        focusPresenter(m_currentPresenter);
    }
    else
    {
        m_currentViewModel = nullptr;
        m_currentModel = nullptr;
    }
}

void ProcessFocusManager::focus(ScenarioDocumentPresenter*)
{
    focusNothing();
}


void ProcessFocusManager::focusNothing()
{
    if(m_currentViewModel)
        emit sig_defocusedViewModel(m_currentViewModel);
    if(m_currentPresenter)
        defocusPresenter(m_currentPresenter);

    m_currentModel = nullptr;
    m_currentViewModel = nullptr;
    m_currentPresenter = nullptr;
}

void ProcessFocusManager::focusPresenter(LayerPresenter* p)
{
    p->setFocus(true);
    emit sig_focusedPresenter(p);
}

void ProcessFocusManager::defocusPresenter(LayerPresenter* p)
{
    p->setFocus(false);
    emit sig_defocusedPresenter(p);
}
}
