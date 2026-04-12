// PluginManager implementation (stub)
#include "PluginManager.h"
#include "Plugin.h"
#include "Tool.h"
#include "WeatherTool.h"
#include "StockTool.h"
#include <QDir>

PluginManager::PluginManager(QObject *parent)
    : QObject(parent)
{
    m_pluginDirectory = QString("/home/segin/.config/qtgpt/plugins");
    m_pluginDir = m_pluginDirectory;
}

PluginManager::~PluginManager()
{
    unloadPlugins();
}

bool PluginManager::loadPlugins(const QString &plugin_dir)
{
    m_pluginDirectory = plugin_dir;
    m_pluginDir = plugin_dir;
    
    QDir dir(m_pluginDir);
    if (!dir.exists()) {
        return false;
    }

    QStringList filters = QStringList() << "*.so";
    dir.setFilter(QDir::Files | QDir::Readable);
    dir.setNameFilters(filters);

    QStringList plugin_files = dir.entryList();
    
    for (const QString &file : plugin_files) {
        Plugin *plugin = new Plugin(m_pluginDir.filePath(file));
        if (plugin->load()) {
            m_plugins.append(plugin);
            for (Tool *tool : plugin->tools()) {
                m_tools.append(tool);
            }
        } else {
            delete plugin;
        }
    }
    
    return true;
}

void PluginManager::unloadPlugins()
{
    for (Plugin *plugin : m_plugins) {
        plugin->unload();
        delete plugin;
    }
    m_plugins.clear();
    
    for (Tool *tool : m_tools) {
        delete tool;
    }
    m_tools.clear();
}

QList<Tool*> PluginManager::tools() const
{
    return m_tools;
}

Tool *PluginManager::getTool(const QString &name)
{
    for (Tool *tool : m_tools) {
        if (tool->toolName() == name) {
            return tool;
        }
    }
    return nullptr;
}

void PluginManager::setPluginDirectory(const QString &dir)
{
    m_pluginDirectory = dir;
    m_pluginDir = dir;
}

QString PluginManager::pluginDirectory() const
{
    return m_pluginDirectory;
}

void PluginManager::refreshPlugins()
{
    unloadPlugins();
    loadPlugins(m_pluginDirectory);
}

void PluginManager::registerBuiltInTools()
{
    // Register WeatherTool
    WeatherTool *weather = new WeatherTool();
    m_tools.append(weather);
    
    // Register StockTool
    StockTool *stocks = new StockTool();
    m_tools.append(stocks);
}
