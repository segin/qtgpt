// Plugin implementation (stub)
#include "Plugin.h"
#include "Tool.h"
#include <QLibrary>
#include <QDir>

Plugin::Plugin(const QString &plugin_path)
{
    m_pluginPath = plugin_path;
    m_library = nullptr;
}

Plugin::~Plugin()
{
    unload();
}

bool Plugin::load()
{
    m_library = new QLibrary(m_pluginPath);
    if (!m_library->load()) {
        return false;
    }
    
    // Initialize plugin
    initialize();
    
    return true;
}

bool Plugin::unload()
{
    if (m_library) {
        m_library->unload();
        delete m_library;
        m_library = nullptr;
    }
    return true;
}

void Plugin::initialize()
{
    // Initialize hook - to be overridden by subclasses
}

QList<Tool*> Plugin::tools() const
{
    return m_tools;
}

void Plugin::addTool(Tool *tool)
{
    m_tools.append(tool);
}

int Plugin::toolCount() const
{
    return m_tools.count();
}
