#include "TemperatureMessage.h"

#include <iomanip>
#include <sstream>

namespace {

// 把 double 转成固定一位小数的字符串，避免 26.500000 这种难看的输出
std::string oneDecimal(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << value;
    return oss.str();
}

}  // namespace

// 先构造基类那部分：内容由温度和湿度拼成（基类仍会校验非空、盖时间戳）
TemperatureMessage::TemperatureMessage(double temperature, double humidity)
    : Message("温度 " + oneDecimal(temperature) + "℃，湿度 " + oneDecimal(humidity) + "%"),
      temperature_(temperature),
      humidity_(humidity) {
}
