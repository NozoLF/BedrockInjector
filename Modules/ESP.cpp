#include "Modules/ESP.h"
#include <cmath>
namespace modules {
std::optional<ScreenPoint> project(const bedrock::Camera& c,bedrock::Vec3 p){
 const float* m=c.viewProjection;
 const float x=m[0]*p.x+m[4]*p.y+m[8]*p.z+m[12];
 const float y=m[1]*p.x+m[5]*p.y+m[9]*p.z+m[13];
 const float w=m[3]*p.x+m[7]*p.y+m[11]*p.z+m[15];
 if(w<=0.0001f||!std::isfinite(w)||c.width<=0||c.height<=0)return std::nullopt;
 return ScreenPoint{(x/w*0.5f+0.5f)*c.width,(0.5f-y/w*0.5f)*c.height};
}
}
