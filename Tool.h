#ifndef TOOL_H
#define TOOL_H

#include <QString>

class Tool
{
public:
    Tool(const QString name, const QString description, const QString parameters_json);
    virtual ~Tool();

    // Tool interface
    virtual const char *name() const;
    virtual const char *description() const;
    virtual const char *parameters_schema() const;
    virtual QString execute(const QString &args_json);

    // Tool metadata
    QString toolName() const { return m_name; }
    QString toolDescription() const { return m_description; }
    QString toolParameters() const { return m_parameters; }

    // JSON schema validation
    static bool validateToolCall(const QString &tool_name, const QString &args_json);

private:
    QString m_name;
    QString m_description;
    QString m_parameters;
};

#endif // TOOL_H
