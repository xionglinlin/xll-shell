#include "mainwindow.h"
#include "pluginloader.h"
#include "appletloader.h"
#include "qmlengine.h"
#include "containment.h"

#include <iostream>

#include <QApplication>
#include <QLoggingCategory>

DS_USE_NAMESPACE

DS_BEGIN_NAMESPACE
Q_DECLARE_LOGGING_CATEGORY(dsLog)
DS_END_NAMESPACE

void outputPluginTreeStruct(const DPluginMetaData &plugin, int level)
{
    const QString separator(level * 4, ' ');
    std::cout << qPrintable(separator + plugin.pluginId()) << std::endl;
    for (const auto &item : DPluginLoader::instance()->childrenPlugin(plugin.pluginId())) {
        outputPluginTreeStruct(item, level + 1);
    }
}

class AppletManager
{
public:
    explicit AppletManager(const QStringList &pluginIds)
    {
        qCDebug(dsLog) << "Preloading plugins:" << pluginIds;
        auto rootApplet = qobject_cast<DContainment *>(DPluginLoader::instance()->rootApplet());
        Q_ASSERT(rootApplet);

        for (const auto &pluginId : pluginIds) {
            auto applet = rootApplet->createApplet(DAppletData{pluginId});
            if (!applet) {
                qCWarning(dsLog) << "Loading plugin failed:" << pluginId;
                continue;
            }

            auto loader = new DAppletLoader(applet);
            m_loaders << loader;

            QObject::connect(loader, &DAppletLoader::failed, qApp, [this, loader, pluginIds](const QString &pluginId) {
                if (pluginIds.contains(pluginId)) {
                    m_loaders.removeOne(loader);
                    loader->deleteLater();
                }
            });
        }
    }
    void enableSceneview()
    {
        auto rootApplet = qobject_cast<DContainment *>(DPluginLoader::instance()->rootApplet());
        rootApplet->setRootObject(DQmlEngine::createObject(QUrl("qrc:/shell/SceneWindow.qml")));
    }
    void exec()
    {
        for (auto loader : std::as_const(m_loaders)) {
            loader->exec();
        }
    }
    void quit()
    {
        for (auto item : std::as_const(m_loaders)) {
            item->deleteLater();
        }
    }
    QList<DAppletLoader *> m_loaders;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("deepin");
    a.setApplicationName("org.deepin.xll-shell");
    a.setApplicationVersion("1.0.0");

    qSetMessagePattern("[%{time yyyy-MM-dd hh:mm:ss.zzz}] [%{type}] [%{function}:%{line}] - %{message}");

    // 树型结构打印所有可用的插件
    for (const auto &item : DPluginLoader::instance()->rootPlugins()) {
        outputPluginTreeStruct(item, 0);
    }

    QList<QString> pluginIds;
    for (const auto &item : DPluginLoader::instance()->rootPlugins()) {
        const auto catetroy = item.value("Category").toString();
        if (catetroy.isEmpty())
            continue;

        pluginIds << item.pluginId();
    }

    AppletManager manager(pluginIds);

    QMetaObject::invokeMethod(&a, [&manager](){
            manager.exec();
        }, Qt::QueuedConnection);

    QObject::connect(qApp, &QCoreApplication::aboutToQuit, qApp, [&manager]() {
        qCInfo(dsLog) << "Exit dde-shell.";
        DPluginLoader::instance()->destroy();
        manager.quit();
    });

    // MainWindow w;
    // w.show();
    return a.exec();
}
