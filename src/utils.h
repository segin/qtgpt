#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QStringList>
#include <QByteArray>

class Utils
{
public:
    // Configuration paths
    static QString getConfigPath(const QString &filename);
    static QString getCachePath(const QString &filename);
    static bool ensureConfigDirExists();
    static QString getUserConfigDir();

    // Base64 encoding/decoding
    static QString base64Encode(const QByteArray &data);
    static QByteArray base64Decode(const QString &data);

    // File operations
    static bool readFileToBuffer(const QString &path, QByteArray &buffer, qint64 &size);
    static QString readFileToString(const QString &path);

    // Image handling
    static QString getImageMimeType(const QByteArray &data);
    static bool isImageSupported(const QString &mime);

    // Message serialization
    static QString serializeMessages(const QStringList &messages);
    static QStringList deserializeMessages(const QString &json);

    // JSON handling
    static QString escapeJson(const QString &str);
    static QString unescapeJson(const QString &str);

    // System prompt generation
    static QString generateSystemPrompt(const QString &custom_prompt, const QString &system_prompt, bool append_tools);
    static QString generateDefaultSystemPrompt();

    // Token counting (basic implementation)
    static int countTokens(const QString &text);

    // Model validation
    static bool isValidModelName(const QString &model);
    static bool isValidApiKey(const QString &key);

    // MIME type mapping
    static QString mapMimeType(const QString &extension);

private:
    static QString s_userConfigDir;
    static QStringList s_builtinTools;
};

#endif // UTILS_H
