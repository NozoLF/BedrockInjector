#pragma once
#include "BedrockBridge/Bridge.h"
#include <optional>
namespace modules {
struct ScreenPoint {float x{},y{};};
std::optional<ScreenPoint> project(const bedrock::Camera& camera,bedrock::Vec3 position);
}
