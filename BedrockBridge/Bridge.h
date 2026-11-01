#pragma once
#include <cstdint>
#include <optional>
#include <vector>
#include <string>
namespace bedrock {
struct Vec3 {float x{},y{},z{};};
struct Entity {std::uint64_t id{};Vec3 position{};std::string name;bool player{};};
struct Block {int x{},y{},z{};std::string identifier;};
struct Camera {float viewProjection[16]{};int width{},height{};};
class DataProvider {
public:
 virtual ~DataProvider()=default;
 virtual std::optional<Camera> camera() const=0;
 virtual std::vector<Entity> entities() const=0;
 virtual std::vector<Block> blocks() const=0;
};
class Bridge {
public:
 bool attach();
 bool connected() const noexcept {return connected_;}
 const std::string& error() const noexcept {return error_;}
private: bool connected_{};std::string error_;
};
}
