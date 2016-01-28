#pragma once
#include <iscore_lib_base_export.h>
#include <QString>
class QObject;

namespace iscore
{
    class DocumentModel;
    class PanelModel;
    class PanelPresenter;
    class PanelView;
    
    
    struct ApplicationContext;
    struct DocumentContext;

    /**
     * @brief The PanelFactory class
     *
     * Factory for a side panel. Think of the palettes in photoshop.
     */
    class ISCORE_LIB_BASE_EXPORT PanelFactory
    {
        public:
            virtual QString panelName() const = 0;
            virtual int panelId() const = 0;

            virtual ~PanelFactory();
            virtual PanelView* makeView(
                    const iscore::ApplicationContext& ctx,
                    QObject* parent) = 0;

            virtual PanelPresenter* makePresenter(
                    const iscore::ApplicationContext& ctx,
                    iscore::PanelView* view,
                    QObject* parent) = 0;

            virtual PanelModel* makeModel(
                    const iscore::DocumentContext& ctx,
                    QObject* parent) = 0;
    };
}
