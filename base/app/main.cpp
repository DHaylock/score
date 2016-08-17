#include "Application.hpp"
#include <QApplication>
#include <qnamespace.h>

#if defined(ISCORE_STATIC_PLUGINS)
  #include <iscore_static_plugins.hpp>
#endif

#if defined(ISCORE_STATIC_QT)
  #if defined(__linux__)
    #include <QtPlugin>

    Q_IMPORT_PLUGIN (QXcbIntegrationPlugin)
  #endif
#endif

#if defined(ISCORE_OPENGL)
#include <QSurfaceFormat>
#endif

static void init_plugins()
{
// TODO generate this too
#if defined(ISCORE_STATIC_PLUGINS)
    Q_INIT_RESOURCE(iscore);
    Q_INIT_RESOURCE(ScenarioResources);
    Q_INIT_RESOURCE(DeviceExplorer);
#if defined(ISCORE_PLUGIN_TEMPORALAUTOMATAS)
    Q_INIT_RESOURCE(TAResources);
#endif
#endif
}

int main(int argc, char** argv)
{
    init_plugins();
#if defined(ISCORE_OPENGL)
    QSurfaceFormat fmt;
    fmt.setSamples(2);
    QSurfaceFormat::setDefaultFormat(fmt);
#endif
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    Application app(argc, argv);
    app.init();
    return app.exec();
}
