#include <CoreGraphics/CoreGraphics.h>
#include <ImageIO/ImageIO.h>
#include "../Sources/Core/GameSession.hpp"
#include "../Sources/Core/SpriteRaster.hpp"
#include "../Sources/Core/TrailRaster.hpp"
#include "../Sources/Core/TextureFormat.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <iomanip>
using namespace heavymach;
using Clock=std::chrono::steady_clock;
static double ms(Clock::time_point start){return std::chrono::duration<double,std::milli>(Clock::now()-start).count();}
struct Texture {int w=0,h=0;std::vector<uint8_t> pixels;SpriteTextureFormat format;SpriteTextureView view(bool trail=false)const{return {w,h,size_t(w)*4,pixels,trail?SpriteTextureFormat::PremultipliedRGBA8:format};}};
struct Cache {
 std::filesystem::path root;std::map<std::string,Texture> entries;double decodeMS=0;
 const Texture& get(const std::string& key){auto found=entries.find(key);if(found!=entries.end())return found->second;
  auto start=Clock::now();auto path=root/key;if(!std::filesystem::exists(path))path+=".png";
  CFURLRef url=CFURLCreateFromFileSystemRepresentation(nullptr,reinterpret_cast<const UInt8*>(path.c_str()),path.string().size(),false);
  CGImageSourceRef source=CGImageSourceCreateWithURL(url,nullptr);CFRelease(url);if(!source)throw std::runtime_error("Missing image "+path.string());
  CGImageRef image=CGImageSourceCreateImageAtIndex(source,0,nullptr);CFRelease(source);if(!image)throw std::runtime_error("Decode failed "+key);
  Texture t;t.w=int(CGImageGetWidth(image));t.h=int(CGImageGetHeight(image));t.format=originalTextureUsesRGB565(CGImageGetColorSpace(image)!=nullptr,CGImageGetAlphaInfo(image))?SpriteTextureFormat::RGB565:SpriteTextureFormat::PremultipliedRGBA8;
  t.pixels.resize(size_t(t.w)*t.h*4);CGColorSpaceRef space=CGColorSpaceCreateDeviceRGB();CGContextRef ctx=CGBitmapContextCreate(t.pixels.data(),t.w,t.h,8,size_t(t.w)*4,space,kCGBitmapByteOrder32Big|kCGImageAlphaPremultipliedLast);CGColorSpaceRelease(space);
  if(!ctx)throw std::runtime_error("Texture bitmap failed");CGContextDrawImage(ctx,CGRectMake(0,0,t.w,t.h),image);CGContextRelease(ctx);CGImageRelease(image);decodeMS+=ms(start);return entries.emplace(key,std::move(t)).first->second;
 }
};
struct Frame {std::vector<SessionRenderItem> items;double tickMS=0;size_t rng=0;};
struct Timing {double raster=0,composite=0,total=0;size_t pixels=0,sprites=0,trails=0;};
template<class Raster> static void composite(Raster& raster,CGContextRef ctx){
 if(raster.empty())return;size_t stride=size_t(raster.bounds.width)*4;
 for(int y=0;y<raster.bounds.height/2;++y)for(size_t x=0;x<stride;++x)std::swap(raster.rgba[size_t(y)*stride+x],raster.rgba[size_t(raster.bounds.height-1-y)*stride+x]);
 CFDataRef data=CFDataCreate(nullptr,raster.rgba.data(),raster.rgba.size());CGDataProviderRef provider=CGDataProviderCreateWithCFData(data);CFRelease(data);CGColorSpaceRef space=CGColorSpaceCreateDeviceRGB();
 CGImageRef image=CGImageCreate(raster.bounds.width,raster.bounds.height,8,32,stride,space,CGBitmapInfo(kCGBitmapByteOrder32Big)|CGBitmapInfo(kCGImageAlphaPremultipliedLast),provider,nullptr,false,kCGRenderingIntentDefault);CGColorSpaceRelease(space);CGDataProviderRelease(provider);
 if(!image)throw std::runtime_error("Output image failed");CGContextSetBlendMode(ctx,raster.blend==2?kCGBlendModePlusLighter:kCGBlendModeNormal);CGContextDrawImage(ctx,CGRectMake(raster.bounds.x,raster.bounds.y,raster.bounds.width,raster.bounds.height),image);CGImageRelease(image);
}
static Timing render(const Frame& frame,Cache& cache,CGContextRef ctx){Timing t;auto start=Clock::now();CGContextClearRect(ctx,CGRectMake(0,0,480,320));
 for(const auto& item:frame.items){if(const auto* d=std::get_if<DrawSprite>(&item)){const auto& tex=cache.get(d->tile);SpriteRasterInput in;in.columns=d->cols;in.rows=d->rows;in.tile=d->frame;in.position=d->position;in.scale=d->scale;in.rotation=d->rotation;in.alpha=d->alpha;in.blend=d->blend;
   auto a=Clock::now();auto result=rasterizeSprite(in,tex.view());t.raster+=ms(a);t.pixels+=result.rgba.size()/4;++t.sprites;a=Clock::now();composite(result,ctx);t.composite+=ms(a);
  }else{const auto& mesh=std::get<TrailMesh>(item);const auto& tex=cache.get(mesh.tile);auto a=Clock::now();auto result=rasterizeTrail(mesh,tex.view(true));t.raster+=ms(a);t.pixels+=result.rgba.size()/4;++t.trails;a=Clock::now();composite(result,ctx);t.composite+=ms(a);}}
 CGContextFlush(ctx);t.total=ms(start);return t;
}
int main(int argc,char** argv){try{
 if(argc<3)throw std::runtime_error("usage: benchmark_renderer Resources output-prefix [build-label]");
 std::filesystem::path resources=argv[1];std::string prefix=argv[2],label=argc>3?argv[3]:"unknown";Cache cache{resources/"Images"};auto data=GameData::loadDirectory(resources/"RawData");
 std::map<std::string,std::vector<Frame>> cases;
 for(const std::string name:{"Stage01","Stage28","Stage35"}){SaveSlot slot;uint32_t seed=11;size_t calls=0;GameSession game(data,slot,[&](std::string_view key){auto& t=cache.get(std::string(key));return Vector2{float(t.w),float(t.h)};},[&]{++calls;seed=seed*1664525u+1013904223u;return int32_t(seed>>1);});
  if(name=="Stage01"){GlobalInfo info(slot);info.selectSlot(0);game.start(0,0,0);}else{auto stages=validationStages(data);auto it=std::find_if(stages.begin(),stages.end(),[&](auto& s){return s.mapName==name;});ValidationScenario s;s.mission=it->mission;s.stage=it->stage;s.grades={6,6,6};s.allWeapons=true;s.bossIndex=0;game.startValidation(s);for(int i=0;i<slot.weaponCount();++i)if(slot.weaponName(size_t(i))=="P_Wpn_Missile03")game.selectWeapon(i);}
  for(int i=0;i<180;++i){game.touch({479,160},i==0?0:1);auto start=Clock::now();game.tick(double(i+1)/60,name=="Stage01"?.1f:0);double elapsed=ms(start);if(i>=120)cases[name].push_back({std::vector<SessionRenderItem>(game.renderItems().begin(),game.renderItems().end()),elapsed,calls});game.takeEvents();}
 }
 for(int i=0;i<60;++i){Frame f;DrawSprite d;d.tile="Main_BG.png";d.position={240,160};d.scale={3,3};d.rotation=float(i)*.02f;f.items.push_back(d);cases["title_large_rotated"].push_back(std::move(f));}
 // Decode every texture before warm rendering and measurement. No simulation/RNG in render.
 for(const auto& [name,frames]:cases)for(const auto& frame:frames)for(const auto& item:frame.items)cache.get(std::holds_alternative<DrawSprite>(item)?std::get<DrawSprite>(item).tile:std::get<TrailMesh>(item).tile);
 std::vector<uint8_t> output(480*320*4);CGColorSpaceRef space=CGColorSpaceCreateDeviceRGB();CGContextRef ctx=CGBitmapContextCreate(output.data(),480,320,8,480*4,space,kCGBitmapByteOrder32Big|kCGImageAlphaPremultipliedLast);CGColorSpaceRelease(space);if(!ctx)throw std::runtime_error("Output bitmap failed");
 std::ofstream csv(prefix+".csv"),summary(prefix+".jsonl");csv<<"case,frame,tick_ms,rng_calls,sprites,trails,pixels,raster_ms,composite_ms,total_ms\n";csv<<std::fixed<<std::setprecision(6);
 for(const auto& [name,frames]:cases){for(const auto& frame:frames)render(frame,cache,ctx);std::vector<double> times,rasterTimes;double rasterSum=0,cgSum=0,tickSum=0;size_t pixels=0,trails=0,sprites=0;int index=0;
  for(const auto& frame:frames){auto t=render(frame,cache,ctx);times.push_back(t.total);rasterTimes.push_back(t.raster);rasterSum+=t.raster;cgSum+=t.composite;tickSum+=frame.tickMS;pixels+=t.pixels;trails+=t.trails;sprites+=t.sprites;csv<<name<<','<<index++<<','<<frame.tickMS<<','<<frame.rng<<','<<t.sprites<<','<<t.trails<<','<<t.pixels<<','<<t.raster<<','<<t.composite<<','<<t.total<<'\n';}
  std::sort(times.begin(),times.end());std::sort(rasterTimes.begin(),rasterTimes.end());double count=double(frames.size());summary<<"{\"case\":\""<<name<<"\",\"build\":\""<<label<<"\",\"frames\":"<<frames.size()<<",\"total_mean_ms\":"<<std::accumulate(times.begin(),times.end(),0.)/count<<",\"total_p50_ms\":"<<times[30]<<",\"total_p95_ms\":"<<times[56]<<",\"total_max_ms\":"<<times.back()<<",\"raster_mean_ms\":"<<rasterSum/count<<",\"raster_p95_ms\":"<<rasterTimes[56]<<",\"composite_mean_ms\":"<<cgSum/count<<",\"tick_mean_ms\":"<<tickSum/count<<",\"pixels_mean\":"<<pixels/count<<",\"sprites_mean\":"<<sprites/count<<",\"trails_mean\":"<<trails/count<<",\"cache_decode_total_ms\":"<<cache.decodeMS<<"}\n";summary.flush();std::cout<<name<<" mean="<<std::accumulate(times.begin(),times.end(),0.)/count<<"ms p95="<<times[56]<<"ms\n";
 }
 CGContextRelease(ctx);return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
