#include "Audio.hpp"
#include "AudioContract.hpp"
#include <android/log.h>
#include <cmath>
#include <cstring>
#include <limits>
#include <stdexcept>
namespace hm {
std::vector<uint8_t> assetBytes(AAssetManager* manager,const std::string& path){
 AAsset* a=AAssetManager_open(manager,path.c_str(),AASSET_MODE_BUFFER);
 if(!a)throw std::runtime_error("Missing asset: "+path);
 auto n=AAsset_getLength64(a);if(n<0||n>128*1024*1024){AAsset_close(a);throw std::runtime_error("Invalid asset size: "+path);}
 std::vector<uint8_t> out(size_t(n),0);size_t pos=0;
 while(pos<out.size()){int read=AAsset_read(a,out.data()+pos,out.size()-pos);if(read<=0){AAsset_close(a);throw std::runtime_error("Truncated asset: "+path);}pos+=size_t(read);}
 AAsset_close(a);return out;
}
static uint16_t u16(const uint8_t*p){return uint16_t(p[0])|uint16_t(p[1])<<8;}
static uint32_t u32(const uint8_t*p){return uint32_t(p[0])|uint32_t(p[1])<<8|uint32_t(p[2])<<16|uint32_t(p[3])<<24;}
Audio::Audio(AAssetManager* assets):assets_(assets){
 device_=alcOpenDevice(nullptr);if(!device_)throw std::runtime_error("OpenAL output device unavailable");
 context_=alcCreateContext(device_,nullptr);if(!context_||!alcMakeContextCurrent(context_)){if(context_)alcDestroyContext(context_);alcCloseDevice(device_);throw std::runtime_error("OpenAL context unavailable");}
 try {
 alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);alListener3f(AL_POSITION,0,0,0);alListenerf(AL_GAIN,1);
 for(auto key:heavymach::originalInitialSoundNames){
  std::string name(key);auto bytes=assetBytes(assets_,"Audio/"+name+".wav");const auto*p=bytes.data();size_t n=bytes.size();
  if(n<12||memcmp(p,"RIFF",4)||memcmp(p+8,"WAVE",4)||u32(p+4)<4||size_t(u32(p+4))>n-8)throw std::runtime_error("Invalid WAV: "+name);
  size_t end=size_t(u32(p+4))+8;const void* pcm=nullptr;ALsizei count=0,rate=0;bool format=false;
  for(size_t pos=12;pos<end;){
   if(end-pos<8)throw std::runtime_error("Truncated WAV chunk: "+name);
   uint32_t size=u32(p+pos+4);pos+=8;if(size>end-pos)throw std::runtime_error("Invalid WAV chunk size: "+name);
   if(!memcmp(p+pos-8,"fmt ",4)){
    if(format||size<16||u16(p+pos)!=1||u16(p+pos+2)!=1||u16(p+pos+14)!=16||u16(p+pos+12)!=2)throw std::runtime_error("Expected mono PCM16: "+name);
    uint32_t r=u32(p+pos+4);if(!r||r>uint32_t(INT32_MAX)||uint64_t(r)*2!=u32(p+pos+8))throw std::runtime_error("Invalid WAV rate: "+name);rate=ALsizei(r);format=true;
   }else if(!memcmp(p+pos-8,"data",4)){if(pcm||!size||(size&1)||size>uint32_t(INT32_MAX))throw std::runtime_error("Invalid PCM data: "+name);pcm=p+pos;count=ALsizei(size);}
   pos+=size;if(size&1){if(pos>=end)throw std::runtime_error("Missing WAV padding: "+name);++pos;}
  }
  if(!format||!pcm)throw std::runtime_error("Missing WAV data: "+name);
  auto& v=voices_[name];alGenBuffers(1,&v.buffer);alBufferData(v.buffer,AL_FORMAT_MONO16,pcm,count,rate);alGenSources(1,&v.source);alSourcei(v.source,AL_BUFFER,v.buffer);
  alSourcef(v.source,AL_MAX_DISTANCE,heavymach::originalSoundMaxDistance(name));alSourcef(v.source,AL_REFERENCE_DISTANCE,1);alSourcef(v.source,AL_ROLLOFF_FACTOR,1);alSourcef(v.source,AL_GAIN,1);alSource3f(v.source,AL_POSITION,0,0,0);
  if(alGetError()!=AL_NO_ERROR)throw std::runtime_error("Cannot prepare OpenAL voice: "+name);
 }
 }catch(...){for(auto&[n,v]:voices_){if(v.source)alDeleteSources(1,&v.source);if(v.buffer)alDeleteBuffers(1,&v.buffer);}alcMakeContextCurrent(nullptr);alcDestroyContext(context_);alcCloseDevice(device_);throw;}
}
Audio::~Audio(){if(context_){alcMakeContextCurrent(context_);for(auto&[n,v]:voices_){alSourceStop(v.source);alDeleteSources(1,&v.source);alDeleteBuffers(1,&v.buffer);}alcMakeContextCurrent(nullptr);alcDestroyContext(context_);}if(device_)alcCloseDevice(device_);}
void Audio::sound(const std::string& name,float pan){if(!active_||!std::isfinite(pan))return;auto i=voices_.find(name);if(i==voices_.end())throw std::runtime_error("Unprepared sound: "+name);alcMakeContextCurrent(context_);alSource3f(i->second.source,AL_POSITION,pan,0,0);alSourceStop(i->second.source);alSourcePlay(i->second.source);++starts_;if(alGetError()!=AL_NO_ERROR)throw std::runtime_error("OpenAL playback: "+name);}
void Audio::active(bool value){if(active_==value)return;active_=value;alcMakeContextCurrent(context_);if(!value){++stops_;for(auto&[n,v]:voices_)alSourceStop(v.source);alcSuspendContext(context_);}else alcProcessContext(context_);}
}
