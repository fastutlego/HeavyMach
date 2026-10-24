#import "HMOpenALAudio.h"
#include "../../vendor/openal-soft/include/AL/al.h"
#include "../../vendor/openal-soft/include/AL/alc.h"
#include "../Core/AudioContract.hpp"
#include <cstdint>
#include <cstring>
#include <limits>
#include <map>
#include <string>
#include <mutex>

namespace {
// iOS OpenAL may expose only one output device. Engines keep separate contexts
// but share that process-wide device; close it only after its final owner exits.
std::mutex deviceMutex;
ALCdevice *sharedDevice=nullptr;
size_t deviceOwners=0;
ALCdevice *acquireDevice(){
    std::lock_guard<std::mutex> lock(deviceMutex);
    if(!sharedDevice)sharedDevice=alcOpenDevice(nullptr);
    if(sharedDevice)++deviceOwners;
    return sharedDevice;
}
void releaseDevice(ALCdevice *device){
    std::lock_guard<std::mutex> lock(deviceMutex);
    if(device&&device==sharedDevice&&--deviceOwners==0){alcCloseDevice(sharedDevice);sharedDevice=nullptr;}
}
struct Voice {ALuint buffer=0,source=0;NSUInteger starts=0;};
struct ContextScope {
    ALCcontext *previous=alcGetCurrentContext();bool valid=false;
    explicit ContextScope(ALCcontext *context){valid=context&&alcMakeContextCurrent(context)==ALC_TRUE;}
    ContextScope(ALCcontext *context,ALCcontext *saved):previous(saved){valid=context&&alcMakeContextCurrent(context)==ALC_TRUE;}
    ~ContextScope(){if(valid)alcMakeContextCurrent(previous);}
};
uint16_t u16(const uint8_t *p){return uint16_t(p[0])|uint16_t(p[1])<<8;}
uint32_t u32(const uint8_t *p){return uint32_t(p[0])|uint32_t(p[1])<<8|uint32_t(p[2])<<16|uint32_t(p[3])<<24;}
struct PCM {const void *samples=nullptr;ALsizei bytes=0,rate=0;};
bool decodeWAV(NSData *data,PCM& out){
    const auto *p=(const uint8_t*)data.bytes;size_t n=data.length;
    if(n<12||memcmp(p,"RIFF",4)||memcmp(p+8,"WAVE",4))return false;
    uint32_t declared=u32(p+4);if(declared<4||size_t(declared)>n-8)return false;
    size_t end=size_t(declared)+8;bool format=false,haveData=false;
    for(size_t pos=12;pos<end;){
        if(end-pos<8)return false;
        uint32_t size=u32(p+pos+4);pos+=8;
        if(size>end-pos)return false;
        if(!memcmp(p+pos-8,"fmt ",4)){
            if(format||size<16||u16(p+pos)!=1||u16(p+pos+2)!=1||u16(p+pos+14)!=16||u16(p+pos+12)!=2)return false;
            uint32_t rate=u32(p+pos+4);if(!rate||rate>uint32_t(std::numeric_limits<ALsizei>::max())||uint64_t(rate)*2!=u32(p+pos+8))return false;
            out.rate=ALsizei(rate);format=true;
        }else if(!memcmp(p+pos-8,"data",4)){
            if(haveData||!size||(size&1)||size>uint32_t(std::numeric_limits<ALsizei>::max()))return false;
            out.samples=p+pos;out.bytes=ALsizei(size);haveData=true;
        }
        pos+=size;
        if(size&1){if(pos>=end)return false;++pos;}
    }
    return format&&haveData;
}
}
@implementation HMOpenALAudio {
    ALCdevice *_device;
    ALCcontext *_context;
    std::map<std::string,Voice> _voices;
    NSMutableArray<NSString*> *_creationOrder;
    NSString *_error,*_vendor,*_renderer,*_version,*_deviceName;
    NSUInteger _startCount;
}
- (void)recordError:(NSString*)message {_error=message;NSLog(@"HMOpenALAudio: %@",message);}
- (instancetype)init {
    if(!(self=[super init]))return nil;
    _creationOrder=[NSMutableArray new];
    ALCcontext *previous=alcGetCurrentContext();
    _device=acquireDevice();
    if(!_device){[self recordError:@"Cannot open OpenAL device"];return self;}
    _context=alcCreateContext(_device,nullptr);
    ContextScope scope(_context,previous);
    if(!scope.valid){[self recordError:@"Cannot create or activate OpenAL context"];return self;}
    const ALchar *vendor=alGetString(AL_VENDOR),*renderer=alGetString(AL_RENDERER),*version=alGetString(AL_VERSION);
    const ALCchar *deviceName=alcGetString(_device,ALC_ALL_DEVICES_SPECIFIER);
    _vendor=vendor?[NSString stringWithUTF8String:vendor]:@"";
    _renderer=renderer?[NSString stringWithUTF8String:renderer]:@"";
    _version=version?[NSString stringWithUTF8String:version]:@"";
    _deviceName=deviceName?[NSString stringWithUTF8String:deviceName]:@"";
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);alListener3f(AL_POSITION,0,0,0);alListenerf(AL_GAIN,1);
    if(ALenum e=alGetError();e!=AL_NO_ERROR)[self recordError:[NSString stringWithFormat:@"OpenAL initialization error 0x%x",e]];
    return self;
}
- (void)dealloc {
    ALCcontext *previous=alcGetCurrentContext();
    if(_context&&alcMakeContextCurrent(_context)){
        for(auto& entry:_voices){auto& v=entry.second;alSourceStop(v.source);alDeleteSources(1,&v.source);alDeleteBuffers(1,&v.buffer);}
    }
    if(_context){alcMakeContextCurrent(previous==_context?nullptr:previous);alcDestroyContext(_context);}
    if(_device)releaseDevice(_device);
}
// Private data seam also exercises malformed RIFF input without replacing AL.
- (BOOL)loadSoundData:(NSData*)data name:(NSString*)name {
    if(!name.length)return NO;
    ContextScope scope(_context);if(!scope.valid){[self recordError:@"OpenAL context unavailable"];return NO;}
    std::string key(name.UTF8String);if(_voices.count(key))return YES;
    PCM pcm;if(!decodeWAV(data,pcm)){[self recordError:[NSString stringWithFormat:@"Invalid mono PCM16 WAV: %@",name]];return NO;}
    Voice voice;alGenBuffers(1,&voice.buffer);alBufferData(voice.buffer,AL_FORMAT_MONO16,pcm.samples,pcm.bytes,pcm.rate);
    alGenSources(1,&voice.source);alSourcei(voice.source,AL_BUFFER,voice.buffer);
    alSourcef(voice.source,AL_MAX_DISTANCE,heavymach::originalSoundMaxDistance(key));
    alSourcef(voice.source,AL_REFERENCE_DISTANCE,1);alSourcef(voice.source,AL_ROLLOFF_FACTOR,1);alSourcef(voice.source,AL_GAIN,1);
    alSource3f(voice.source,AL_POSITION,0,0,0);
    ALenum error=alGetError();if(error!=AL_NO_ERROR){if(voice.source)alDeleteSources(1,&voice.source);if(voice.buffer)alDeleteBuffers(1,&voice.buffer);[self recordError:[NSString stringWithFormat:@"OpenAL load %@ error 0x%x",name,error]];return NO;}
    _voices.emplace(key,voice);[_creationOrder addObject:name];return YES;
}
- (BOOL)prepareOriginalSounds {
    BOOL ready=YES;
    for(auto key:heavymach::originalInitialSoundNames){
        NSString *name=[[NSString alloc] initWithBytes:key.data() length:key.size() encoding:NSUTF8StringEncoding];
        if(_voices.count(std::string(key)))continue;
        NSURL *url=[NSBundle.mainBundle URLForResource:name withExtension:@"wav" subdirectory:@"Audio"];
        NSData *data=url?[NSData dataWithContentsOfURL:url]:nil;
        if(![self loadSoundData:data name:name])ready=NO;
    }
    return ready;
}
- (void)sound:(NSString*)name pan:(float)sourceX {
    if(!name.length||!std::isfinite(sourceX))return;
    auto found=_voices.find(name.UTF8String);
    if(found==_voices.end()){
        NSURL *url=[NSBundle.mainBundle URLForResource:name withExtension:@"wav" subdirectory:@"Audio"];
        if(![self loadSoundData:url?[NSData dataWithContentsOfURL:url]:nil name:name])return;
        found=_voices.find(name.UTF8String);
    }
    ContextScope scope(_context);if(!scope.valid){[self recordError:@"Cannot activate OpenAL for playback"];return;}
    auto& voice=found->second;
    // SoundManager 0x13d0c positions first; Start 0x1e9e0 then stops/restarts.
    alSource3f(voice.source,AL_POSITION,sourceX,0,0);alSourceStop(voice.source);alSourcePlay(voice.source);
    ALenum error=alGetError();if(error!=AL_NO_ERROR){[self recordError:[NSString stringWithFormat:@"OpenAL playback %@ error 0x%x",name,error]];return;}
    voice.starts++;_startCount++;
}
// HeavyMach's resign/active callbacks (0x225c/0x225e) are empty.
// The compatibility host does not add an audio-session observer or source
// pause/resume policy for this game. Let the same OpenAL backend handle the OS.
- (NSDictionary*)diagnostics {
    return @{@"backend":@"OpenAL Soft 1.22.2 static buffers",@"vendor":_vendor?:@"",@"renderer":_renderer?:@"",@"version":_version?:@"",@"device":_deviceName?:@"",@"available":@(_context!=nullptr),@"bufferCount":@(_voices.size()),@"sourceCount":@(_voices.size()),@"startCount":@(_startCount),@"creationOrder":[_creationOrder copy],@"error":_error?:@""};
}
- (NSDictionary*)soundInfo:(NSString*)name {
    if(!name.length)return @{};
    auto found=_voices.find(name.UTF8String);if(found==_voices.end())return @{};
    ContextScope scope(_context);if(!scope.valid)return @{};
    auto& v=found->second;ALfloat x,y,z,maxDistance,gain,reference,rolloff;ALint state,offset,rate,bytes,bits,channels,attached;
    alGetSource3f(v.source,AL_POSITION,&x,&y,&z);alGetSourcef(v.source,AL_MAX_DISTANCE,&maxDistance);alGetSourcef(v.source,AL_GAIN,&gain);alGetSourcef(v.source,AL_REFERENCE_DISTANCE,&reference);alGetSourcef(v.source,AL_ROLLOFF_FACTOR,&rolloff);
    alGetSourcei(v.source,AL_SOURCE_STATE,&state);alGetSourcei(v.source,AL_SAMPLE_OFFSET,&offset);alGetSourcei(v.source,AL_BUFFER,&attached);
    alGetBufferi(v.buffer,AL_FREQUENCY,&rate);alGetBufferi(v.buffer,AL_SIZE,&bytes);alGetBufferi(v.buffer,AL_BITS,&bits);alGetBufferi(v.buffer,AL_CHANNELS,&channels);
    ALenum error=alGetError();if(error!=AL_NO_ERROR){[self recordError:[NSString stringWithFormat:@"OpenAL inspection error 0x%x",error]];return @{};}
    return @{@"bufferID":@(v.buffer),@"sourceID":@(v.source),@"attachedBufferID":@(attached),@"position":@[@(x),@(y),@(z)],@"maxDistance":@(maxDistance),@"gain":@(gain),@"referenceDistance":@(reference),@"rolloff":@(rolloff),@"state":@(state),@"sampleOffset":@(offset),@"rate":@(rate),@"bytes":@(bytes),@"bits":@(bits),@"channels":@(channels),@"startCount":@(v.starts)};
}
#if HM_VALIDATION
// Test-only bridge into this app's single static AL library. The XCTest bundle
// must not link a second library instance with different context/device state.
+ (ALCcontext*)validationCurrentContext {return alcGetCurrentContext();}
+ (BOOL)validationMakeContextCurrent:(ALCcontext*)context {return alcMakeContextCurrent(context)==ALC_TRUE;}
+ (void)validationDestroyContext:(ALCcontext*)context {alcDestroyContext(context);}
- (ALCcontext*)validationCreateExternalContext {return _device?alcCreateContext(_device,nullptr):nullptr;}
- (BOOL)validationPauseSound:(NSString*)name sampleOffset:(NSInteger)offset {
    if(!name.length||offset<0||offset>std::numeric_limits<ALint>::max())return NO;
    auto found=_voices.find(name.UTF8String);if(found==_voices.end())return NO;
    ContextScope scope(_context);if(!scope.valid)return NO;
    alSourcePause(found->second.source);alSourcei(found->second.source,AL_SAMPLE_OFFSET,ALint(offset));
    return alGetError()==AL_NO_ERROR;
}
#endif
@end
