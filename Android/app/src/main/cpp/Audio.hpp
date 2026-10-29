#pragma once
#include <android/asset_manager.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <map>
#include <string>
#include <vector>
#include <cstdint>
namespace hm {
std::vector<uint8_t> assetBytes(AAssetManager*,const std::string&);
class Audio {
 struct Voice {ALuint buffer=0,source=0;};
 AAssetManager* assets_;ALCdevice* device_=nullptr;ALCcontext* context_=nullptr;
 std::map<std::string,Voice> voices_;bool active_=true;uint64_t starts_=0,stops_=0;
public:
 explicit Audio(AAssetManager*);~Audio();
 void sound(const std::string&,float=0);void active(bool);
 uint64_t starts()const{return starts_;}uint64_t stops()const{return stops_;}bool isActive()const{return active_;}
 size_t voices()const{return voices_.size();}
};
}
