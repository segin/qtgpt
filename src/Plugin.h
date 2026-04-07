#ifndef PLUGIN_H
#define PLUGIN_H

#include <QString>
#include <QList>
#include <QLibrary>
#include <QtPlugin>

class Tool;

class Plugin
{
public:
    Plugin(const QString &plugin_path);
    virtual ~Plugin();

    // Plugin interface
    virtual const char *plugin_name() const;
    virtual const char *plugin_description() const;
    virtual const char *plugin_version() const;
    virtual bool load();
    virtual bool unload();
    virtual void initialize();

    // Tools provided by this plugin
    QList<Tool*> tools() const;
    void addTool(Tool *tool);
    int toolCount() const;

    // Plugin metadata
    QString pluginPath() const { return m_pluginPath; }
    QLibrary *library() const { return m_library; }

protected:
    QString m_pluginPath;
    QLibrary *m_library;
    QList<Tool*> m_tools;
};

#endif // PLUGIN_H
