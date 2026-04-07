#ifndef WEATHERTOOL_H
#define WEATHERTOOL_H

#include "Tool.h"

class WeatherTool : public Tool
{
    Q_OBJECT

public:
    explicit WeatherTool(QObject *parent = nullptr);
    ~WeatherTool();

    const char *name() const override;
    const char *description() const override;
    const char *parameters_schema() const override;
    QString execute(const QString &args_json) override;

    // Weather data structure
    struct WeatherData {
        QString city;
        QString temperature;
        QString condition;
        QString humidity;
        QString wind_speed;
    };

    static WeatherData fetchWeather(const QString &city, const QString &api_key);

private:
    QString m_city;
    QString m_api_key;
};

#endif // WEATHERTOOL_H
