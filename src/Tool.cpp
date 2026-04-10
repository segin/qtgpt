// Tool implementation (stub)
#include "Tool.h"

Tool::Tool(QString name, QString description, QString parameters_json)
{
    m_name = name;
    m_description = description;
    m_parameters = parameters_json;
}

Tool::~Tool()
{
}

const char *Tool::name() const
{
    return m_name.toUtf8().data();
}

const char *Tool::description() const
{
    return m_description.toUtf8().data();
}

const char *Tool::parameters_schema() const
{
    return m_parameters.toUtf8().data();
}

QString Tool::execute(const QString &args_json)
{
    return QString("Tool executed: %1").arg(args_json);
}

bool Tool::validateToolCall(const QString &tool_name, const QString &args_json)
{
    Q_UNUSED(tool_name);
    // Simple validation - check JSON structure
    if (args_json.startsWith("{") && args_json.endsWith("}")) {
        return true;
    }
    return false;
}
