#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>
#include <core/document/DocumentPresenter.hpp>
#include <core/document/DocumentView.hpp>
#include <iscore/plugins/panel/PanelModel.hpp>
#include <iscore/plugins/panel/PanelPresenter.hpp>
#include <QObject>
#include <algorithm>
#include <iterator>
#include <vector>

#include <core/document/DocumentBackupManager.hpp>
#include <iscore/document/DocumentContext.hpp>
#include <iscore/plugins/panel/PanelFactory.hpp>
#include <iscore/selection/SelectionStack.hpp>
#include <iscore/tools/NamedObject.hpp>
#include <iscore/tools/Todo.hpp>

class QWidget;
class Selection;
#include <iscore/tools/SettableIdentifier.hpp>

namespace iscore
{
DocumentContext::DocumentContext(Document& d):
    app{iscore::AppContext()},
    document{d},
    commandStack{d.m_commandStackFacade},
    selectionStack{d.selectionStack()},
    objectLocker{d.locker()}
{
}

const std::vector<DocumentPlugin*>&DocumentContext::pluginModels() const
{
    return document.model().pluginModels();
}


Document::Document(
        const QString& name,
        const Id<DocumentModel>& id,
        DocumentDelegateFactory* factory,
        QWidget* parentview,
        QObject* parent) :
    NamedObject {"Document", parent},
    m_objectLocker{this},
    m_backupMgr{new DocumentBackupManager{*this}},
    m_context{*this}
{
    metadata.setFileName(name);
    /// Construction of the document model

    // Note : we have to separate allocation
    // because the model delegates init might call IDocument::path()
    // which requires the pointer to m_model to be intialized.
    std::allocator<DocumentModel> allocator;
    m_model = allocator.allocate(1);
    allocator.construct(m_model, id, factory, this);
    m_view = new DocumentView{factory, *this, parentview};
    m_presenter = new DocumentPresenter{factory,
            *m_model,
            *m_view,
            this};

    init();

//    connect(m_model, &DocumentModel::fileNameChanged,
//            this, &Document::fileNameChanged);
}

void Document::init()
{
    con(m_selectionStack, &SelectionStack::currentSelectionChanged,
            this, [&] (const Selection& s)
            {
                for(auto& panel : m_model->panels())
                {
                    panel->setNewSelection(s);
                }
                m_model->setNewSelection(s);
            });

}

void Document::setBackupMgr(DocumentBackupManager* backupMgr)
{
    m_backupMgr = backupMgr;
}

Document::~Document()
{
    // We need a custom destructor because
    // for the sake of simplicity, we want the presenter
    // to be deleted before the model.
    // (Else we would have to fine-grain the deletion of the selection stack).

    delete m_presenter;
    delete m_view;
}

const Id<DocumentModel>&Document::id() const
{
    return m_model->id();
}

void Document::setupNewPanel(PanelFactory* factory)
{
    m_model->addPanel(factory->makeModel(m_context, m_model));
}

void Document::bindPanelPresenter(PanelPresenter* pres)
{
    using namespace std;
    auto localmodel = std::find_if(begin(model().panels()),
                                   end(model().panels()),
                                   [&] (PanelModel* model)
    {
        return model->panelId() == pres->panelId();
    });
    ISCORE_ASSERT(localmodel != end(model().panels()));

    pres->setModel(*localmodel);
}
}
