#include "BedrockMenu/Menu.h"
namespace menu {
static std::atomic_bool shown{false};
void toggle() noexcept {shown.store(!shown.load());}
bool visible() noexcept {return shown.load();}
}
