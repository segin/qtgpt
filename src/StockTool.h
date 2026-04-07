#ifndef STOCKTOOL_H
#define STOCKTOOL_H

#include "Tool.h"

class StockTool : public Tool
    {
    public:
    explicit StockTool(QObject *parent = nullptr);
    ~StockTool();

    const char *name() const override;
    const char *description() const override;
    const char *parameters_schema() const override;
    QString execute(const QString &args_json) override;

    // Stock data structure
    struct StockData {
        QString symbol;
        QString price;
        QString change;
        QString change_percent;
        QString volume;
    };

    static StockData fetchStock(const QString &symbol, const QString &api_key);

private:
    QString m_symbol;
    QString m_api_key;
};

#endif // STOCKTOOL_H
