#pragma once

#include <string>
#include "Node.h"

class Publisher : public Node {
private:
    std::string topic_;

public:
    Publisher(const std::string& name, const std::string& topic);

    const std::string& getTopic() const;

    void init() override;

    void publish(const std::string& data);

    void shutdown() override;
};
