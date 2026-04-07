#include "utils.h"
#include <QStandardPaths>
#include <QFile>
#include <QTextCodec>
#include <QByteArray>

QString Utils::getConfigPath(const QString &filename)
{
    QString configDir = getUserConfigDir();
    return configDir + "/" + filename;
}

QString Utils::getCachePath(const QString &filename)
{
    return QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/" + filename;
}

QString Utils::getUserConfigDir()
{
    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/qtgpt";
}

bool Utils::ensureConfigDirExists()
{
    QString dir = getUserConfigDir();
    if (!QDir(dir).exists()) {
        QDir().mkpath(dir);
        return true;
    }
    return true;
}

QString Utils::base64Encode(const QByteArray &data)
{
    return QString::fromLatin1(data.toBase64());
}

QByteArray Utils::base64Decode(const QString &data)
{
    return QByteArray::fromBase64(data.toLatin1());
}

bool Utils::readFileToBuffer(const QString &path, QByteArray &buffer, qint64 &size)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    buffer = file.readAll();
    size = buffer.size();
    return true;
}

QString Utils::readFileToString(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }
    return QString::fromUtf8(file.readAll());
}

QString Utils::getImageMimeType(const QByteArray &data)
{
    if (data.isEmpty()) {
        return QString();
    }
    
    // Check PNG signature
    if (data.size() >= 8) {
        if (data[0] == 0x89 && data[1] == 0x50 && data[2] == 0x4E && data[3] == 0x47 &&
            data[4] == 0x0D && data[5] == 0x0A && data[6] == 0x1A && data[7] == 0x0A) {
            return "image/png";
        }
    }
    
    // Check JPEG signature
    if (data.size() >= 3 && data[0] == 0xFF && data[1] == 0xD8 && data[2] == 0xFF) {
        return "image/jpeg";
    }
    
    // Check GIF signature
    if (data.size() >= 6 && data[0] == 'G' && data[1] == 'I' && data[2] == 'F') {
        return "image/gif";
    }
    
    return QString();
}

bool Utils::isImageSupported(const QString &mime)
{
    return mime == "image/png" || mime == "image/jpeg" || mime == "image/gif";
}

QString Utils::serializeMessages(const QStringList &messages)
{
    QString json = "[";
    for (int i = 0; i < messages.count(); ++i) {
        if (i > 0) json += ",";
        json += "\"" + escapeJson(messages[i]) + "\"";
    }
    json += "]";
    return json;
}

QStringList Utils::deserializeMessages(const QString &json)
{
    // Simple implementation - parse JSON array
    QStringList messages;
    if (json.startsWith('[') && json.endsWith(']')) {
        QString content = json.mid(1, json.size() - 2);
        QStringList parts = content.split(',', Qt::SkipEmptyParts);
        for (const QString &part : parts) {
            if (part.startsWith('"') && part.endsWith('"')) {
                messages.append(unescapeJson(part.mid(1, part.size() - 2)));
            }
        }
    }
    return messages;
}

QString Utils::escapeJson(const QString &str)
{
    QString escaped;
    for (QChar c : str) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

QString Utils::unescapeJson(const QString &str)
{
    QString unescaped;
    bool escaped = false;
    for (int i = 0; i < str.size(); ++i) {
        if (escaped) {
            switch (str[i]) {
                case '"': unescaped += '"'; break;
                case '\\': unescaped += '\\'; break;
                case 'n': unescaped += '\n'; break;
                case 'r': unescaped += '\r'; break;
                case 't': unescaped += '\t'; break;
                default: unescaped += str[i]; break;
            }
            escaped = false;
        } else if (str[i] == '\\') {
            escaped = true;
        } else {
            unescaped += str[i];
        }
    }
    return unescaped;
}

QString Utils::generateSystemPrompt(const QString &custom_prompt, const QString &system_prompt, bool append_tools)
{
    QString prompt = system_prompt;
    if (!custom_prompt.isEmpty()) {
        if (!prompt.isEmpty()) {
            prompt += "\n\n";
        }
        prompt += custom_prompt;
    }
    return prompt;
}

QString Utils::generateDefaultSystemPrompt()
{
    return "You are a helpful AI assistant. You have access to various tools and can provide helpful responses to user queries.";
}

int Utils::countTokens(const QString &text)
{
    // Very rough estimate - 4 characters per token
    return text.size() / 4;
}

bool Utils::isValidModelName(const QString &model)
{
    if (model.isEmpty()) return false;
    return !model.contains(" ");
}

bool Utils::isValidApiKey(const QString &key)
{
    if (key.isEmpty()) return false;
    return key.size() > 10; // Minimum length check
}

QString Utils::mapMimeType(const QString &extension)
{
    if (extension == ".png") return "image/png";
    if (extension == ".jpg" || extension == ".jpeg") return "image/jpeg";
    if (extension == ".gif") return "image/gif";
    if (extension == ".txt") return "text/plain";
    if (extension == ".pdf") return "application/pdf";
    return QString();
}
