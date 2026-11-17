#pragma once

#include <string>

#include "Message.h"

// 自定义消息类型（加分项 B-01）：在"内容 + 时间戳"之外，额外携带温度与湿度
class TemperatureMessage : public Message {
public:
    TemperatureMessage(double temperature, double humidity);
    ~TemperatureMessage() override = default;

    std::string type() const override { return "temperature"; }

    double temperature() const { return temperature_; }
    double humidity() const { return humidity_; }

private:
    double temperature_;
    double humidity_;
};
