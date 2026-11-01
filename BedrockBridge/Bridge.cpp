#include "BedrockBridge/Bridge.h"
namespace bedrock {
bool Bridge::attach(){connected_=false;error_="No verified Bedrock in-process rendering or game-data interface is available for this version";return false;}
}
