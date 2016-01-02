#pragma once
#include <core/presenter/Action.hpp>
#include <iscore/menu/MenuInterface.hpp>
#include <QAction>
#include <QList>
#include <QMenu>
#include <QMenuBar>
#include <QObject>
#include <QString>
#include <QPointer>
#include <algorithm>
#include <map>
#include <type_traits>

class QMenuBar;

namespace iscore
{
    /**
     * @brief The MenubarManager class
     *
     * These are mainly convenience methods to add elements in a non-anarchic way
     * to the menu bar, using the information in \c{MenuInterface}.
     */
    class ISCORE_LIB_BASE_EXPORT MenubarManager final : public QObject
    {
            Q_OBJECT
        public:
            explicit MenubarManager(QMenuBar* bar, QObject* parent = nullptr);
            ~MenubarManager();
            QMenuBar* menuBar() const
            {
                return m_menuBar;
            }

            void insertActionIntoToplevelMenu(ToplevelMenuElement tl, QAction* before, QAction* act);
            void insertActionIntoToplevelMenu(ToplevelMenuElement, QAction* act);

            template<typename MenuElement, typename Functor>
            QAction* addActionIntoToplevelMenu(ToplevelMenuElement tl,
                                               MenuElement elt,
                                               Functor f,
                                               std::enable_if_t<std::is_enum<MenuElement>::value>* = nullptr)
            {
                QAction* act = new QAction {MenuInterface::name(elt), this};
                connect(act, &QAction::triggered, f);
                insertActionIntoToplevelMenu(tl, act);
                return act;
            }

            template<typename MenuElement>
            void insertActionIntoToplevelMenu(ToplevelMenuElement tl,
                                              MenuElement before,
                                              QAction* act,
                                              std::enable_if_t<std::is_enum<MenuElement>::value>* = nullptr)
            {
                auto actions = m_menusMap[tl]->actions();
                auto beforeact_it = std::find_if(
                                        actions.begin(),
                                        actions.end(),
                                        [&before](QAction * theAct)
                {
                    return theAct->objectName() == MenuInterface::name(before);
                });

                if(beforeact_it != actions.end())
                {
                    m_menusMap[tl]->insertAction(*beforeact_it, act);
                }
                else
                {
                    m_menusMap[tl]->addAction(act);
                }
            }

            template<typename MenuElement>
            void addSeparatorIntoToplevelMenu(ToplevelMenuElement tl,
                                              MenuElement sep_type)
            {
                QAction* sep_act = new QAction {this};
                sep_act->setObjectName(MenuInterface::name(sep_type));
                sep_act->setSeparator(true);
                insertActionIntoToplevelMenu(tl,
                                             sep_act);
            }

            template<typename MenuElement>
            void addMenuIntoToplevelMenu(ToplevelMenuElement tl,
                                         MenuElement menu)
            {
                auto act = m_menusMap[tl]->addMenu(MenuInterface::name(menu))->menuAction();
                act->setObjectName(MenuInterface::name(menu));
            }

            const auto& menuAt(ToplevelMenuElement i) const
            {
                return m_menusMap.at(i);
            }

        signals:

        public slots:
            void insertActionIntoMenubar(PositionedMenuAction);

        private:
            QPointer<QMenuBar> m_menuBar {};
            std::map<ToplevelMenuElement, QMenu*> m_menusMap;
    };
}
