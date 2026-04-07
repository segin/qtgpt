#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QString>
#include <QList>
#include <QDir>

class Plugin;
class Tool;

class PluginManager
{
public:
    explicit PluginManager(QObject *parent = nullptr);
    ~PluginManager();

    // Plugin management
    bool loadPlugins(const QString &plugin_dir);
    void unloadPlugins();

    // Tools management
    QList<Tool*> tools() const;
    Tool *getTool(const QString &name);

    // Plugin directory
    void setPluginDirectory(const QString &dir);
    QString pluginDirectory() const;
    void refreshPlugins();

    // Built-in tools
    void registerBuiltInTools();

private:
    QList<Plugin*> m_plugins;
    QList<Tool*> m_tools;
    QString m_pluginDirectory;
    QDir m_pluginDir;
};

#endif // PLUGINMANAGER_H
