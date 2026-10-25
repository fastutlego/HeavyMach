// Independent actual-GL oracle. No SpriteRaster or native renderer code included.
#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <ImageIO/ImageIO.h>
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <iostream>
using Bytes=std::vector<unsigned char>;
static void require(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
static void raw(const std::filesystem::path& path,const void* data,size_t n){std::ofstream f(path,std::ios::binary);f.write((const char*)data,n);require(bool(f),"artifact write failed");}
static void json(const std::filesystem::path& path,id object){NSError* error=nil;NSData* data=[NSJSONSerialization dataWithJSONObject:object options:NSJSONWritingPrettyPrinted error:&error];require(data!=nil,"JSON encoding failed");raw(path,data.bytes,data.length);}
static unsigned pot(unsigned n){unsigned v=1;while(v<n)v*=2;return v;}
struct Texture {unsigned w,h,pw,ph;Bytes decoded;std::vector<uint16_t> packed;GLuint id;bool rgb565;};
static Texture texture(unsigned w,unsigned h,Bytes input,bool rgb565,const std::filesystem::path& out,const char* name){
 Texture t{w,h,pot(w),pot(h),Bytes(pot(w)*pot(h)*4),{},0,rgb565};
 auto cs=CGColorSpaceCreateDeviceRGB();auto provider=CGDataProviderCreateWithData(nullptr,input.data(),input.size(),nullptr);
 auto flags=CGBitmapInfo(kCGBitmapByteOrder32Big)|CGBitmapInfo(rgb565?kCGImageAlphaNoneSkipLast:kCGImageAlphaPremultipliedLast);
 auto source=CGImageCreate(w,h,8,32,w*4,cs,flags,provider,nullptr,false,kCGRenderingIntentDefault);
 require(source!=nullptr,"source CGImageCreate failed");
 auto ctx=CGBitmapContextCreate(t.decoded.data(),t.pw,t.ph,8,t.pw*4,cs,flags);require(ctx!=nullptr,"decode bitmap unavailable");
 CGContextClearRect(ctx,CGRectMake(0,0,t.pw,t.ph));
 CGContextTranslateCTM(ctx,0,t.ph-h); // Original0x69b0 places content at upper bitmap rows.
 CGContextDrawImage(ctx,CGRectMake(0,0,w,h),source);
 CGContextRelease(ctx);CGImageRelease(source);CGDataProviderRelease(provider);CGColorSpaceRelease(cs);
 glGenTextures(1,&t.id);glBindTexture(GL_TEXTURE_2D,t.id);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
 raw(out/(std::string(name)+".rgba"),t.decoded.data(),t.decoded.size());
 if(rgb565){for(size_t i=0;i<t.decoded.size();i+=4)t.packed.push_back(uint16_t((t.decoded[i]>>3)<<11|(t.decoded[i+1]>>2)<<5|(t.decoded[i+2]>>3)));raw(out/(std::string(name)+".rgb565le"),t.packed.data(),t.packed.size()*2);glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,t.pw,t.ph,0,GL_RGB,GL_UNSIGNED_SHORT_5_6_5,t.packed.data());}
 else glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,t.pw,t.ph,0,GL_RGBA,GL_UNSIGNED_BYTE,t.decoded.data());
 require(glGetError()==GL_NO_ERROR,"texture upload GL error");return t;
}
static void png(const std::filesystem::path& path,const Bytes& bottom){
 Bytes top(bottom.size());for(int y=0;y<320;++y)std::copy_n(bottom.data()+(319-y)*480*4,480*4,top.data()+y*480*4);
 auto cs=CGColorSpaceCreateDeviceRGB();auto provider=CGDataProviderCreateWithData(nullptr,top.data(),top.size(),nullptr);
 // Readback RGB is post-blend, not necessarily premultiplied by readback alpha.
 auto im=CGImageCreate(480,320,8,32,480*4,cs,CGBitmapInfo(kCGBitmapByteOrder32Big)|CGBitmapInfo(kCGImageAlphaLast),provider,nullptr,false,kCGRenderingIntentDefault);
 auto url=[NSURL fileURLWithPath:[NSString stringWithUTF8String:path.c_str()]];
 auto dest=CGImageDestinationCreateWithURL((__bridge CFURLRef)url,CFSTR("public.png"),1,nullptr);
 require(im&&dest,"PNG output creation failed");CGImageDestinationAddImage(dest,im,nullptr);require(CGImageDestinationFinalize(dest),"PNG finalize failed");CFRelease(dest);CGImageRelease(im);CGDataProviderRelease(provider);CGColorSpaceRelease(cs);
}
static NSDictionary* draw(Texture& t,int cols,int rows,int tile,float x,float y,float sx,float sy,float angle,float alpha,int blend,const char* name,const std::filesystem::path& out,bool trail=false){
 glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT);glMatrixMode(GL_MODELVIEW);glLoadIdentity();
 glPushMatrix();glTranslatef(x,y,0);glScalef(sx,sy,1);glRotatef(float(double(angle)*57.29577951308232),0,0,1);
 float u=float(t.w)/t.pw/cols,v=float(t.h)/t.ph/rows,hw=t.pw*u*.5f,hh=t.ph*v*.5f;
 tile=std::min(tile,cols*rows-1);float u0=(tile%cols)*u,v0=(tile/cols)*v;
 GLfloat vertices[]={-hw,-hh,hw,-hh,-hw,hh,hw,hh};
 GLfloat uv[]={u0,v0+v,u0+u,v0+v,u0,v0,u0+u,v0};
 GLfloat color[]={1,1,1,alpha,1,1,1,alpha,1,1,1,alpha,1,1,1,alpha};
 if(trail){
  require(angle==0,"trail passthrough fixture requires zero rotation");
  // GobTrailEffect draw uses translated world-space strip vertices, no sprite S/R.
  glLoadIdentity();glTranslatef(x,y,0);
  for(int i=0;i<4;++i){vertices[2*i]*=sx;vertices[2*i+1]*=sy;color[4*i]=.2f+.2f*i;color[4*i+1]=.8f-.2f*i;color[4*i+2]=.3f;color[4*i+3]=.1f+.2f*i;}
 }
 glBindTexture(GL_TEXTURE_2D,t.id);glBlendFunc(GL_SRC_ALPHA,blend==2?GL_ONE:GL_ONE_MINUS_SRC_ALPHA);
 glVertexPointer(2,GL_FLOAT,0,vertices);glTexCoordPointer(2,GL_FLOAT,0,uv);glColorPointer(4,GL_FLOAT,0,color);glDrawArrays(GL_TRIANGLE_STRIP,0,4);glPopMatrix();glFinish();
 Bytes pixels(480*320*4);glReadPixels(0,0,480,320,GL_RGBA,GL_UNSIGNED_BYTE,pixels.data());require(glGetError()==GL_NO_ERROR,"draw/readback GL error");
 raw(out/(std::string(name)+".rgba"),pixels.data(),pixels.size());png(out/(std::string(name)+".png"),pixels);
 NSMutableArray* probes=[NSMutableArray array];for(auto p:std::vector<std::pair<int,int>>{{239,159},{240,160},{241,160},{244,160},{240,164}}){size_t i=(p.second*480+p.first)*4;[probes addObject:@{@"x":@(p.first),@"y":@(p.second),@"rgba":@[@(pixels[i]),@(pixels[i+1]),@(pixels[i+2]),@(pixels[i+3])]}];}
 return @{@"name":@(name),@"trailPassthrough":@(trail),@"texture":t.rgb565?@"opaque":(t.w==2?@"alpha_edge":@"atlas"),@"contentWidth":@(t.w),@"contentHeight":@(t.h),@"paddedWidth":@(t.pw),@"paddedHeight":@(t.ph),@"columns":@(cols),@"rows":@(rows),@"tile":@(tile),@"x":@(x),@"y":@(y),@"scaleX":@(sx),@"scaleY":@(sy),@"rotationRadians":@(angle),@"vertexAlpha":@(alpha),@"blend":@(blend),@"probes":probes};
}
int main(int argc,char** argv){@autoreleasepool{
 std::filesystem::path out=argc>1?argv[1]:"build/render-reference";std::filesystem::create_directories(out);
 CGLContextObj context=nullptr;CGLPixelFormatObj format=nullptr;
 try{
  CGLPixelFormatAttribute attributes[]={kCGLPFAOpenGLProfile,(CGLPixelFormatAttribute)kCGLOGLPVersion_Legacy,kCGLPFAColorSize,(CGLPixelFormatAttribute)32,kCGLPFAAlphaSize,(CGLPixelFormatAttribute)8,(CGLPixelFormatAttribute)0};GLint count=0;
  auto err=CGLChoosePixelFormat(attributes,&format,&count);require(err==kCGLNoError&&format,"CGLChoosePixelFormat unavailable");
  err=CGLCreateContext(format,nullptr,&context);require(err==kCGLNoError&&context,"CGLCreateContext unavailable");require(CGLSetCurrentContext(context)==kCGLNoError,"CGLSetCurrentContext failed");
  GLuint framebuffer,attachment;glGenFramebuffersEXT(1,&framebuffer);glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,framebuffer);glGenRenderbuffersEXT(1,&attachment);glBindRenderbufferEXT(GL_RENDERBUFFER_EXT,attachment);glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,GL_RGBA8,480,320);glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,GL_RENDERBUFFER_EXT,attachment);require(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)==GL_FRAMEBUFFER_COMPLETE_EXT,"offscreen framebuffer incomplete");
  glViewport(0,0,480,320);glMatrixMode(GL_PROJECTION);glLoadIdentity();glOrtho(0,480,0,320,-1,1);glMatrixMode(GL_MODELVIEW);glLoadIdentity();
  glEnable(GL_TEXTURE_2D);glEnable(GL_BLEND);glDisable(GL_DEPTH_TEST);glDisable(GL_CULL_FACE);glEnable(GL_DITHER);glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);glEnableClientState(GL_VERTEX_ARRAY);glEnableClientState(GL_TEXTURE_COORD_ARRAY);glEnableClientState(GL_COLOR_ARRAY);glPixelStorei(GL_PACK_ALIGNMENT,1);glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  auto edge=texture(2,1,{255,255,255,255,0,0,0,0},false,out,"alpha_edge");
  auto atlas=texture(3,3,{255,0,0,255,0,128,0,128,0,0,64,64,255,255,0,255,128,0,128,128,0,255,255,255,64,64,64,64,255,255,255,255,0,0,0,0},false,out,"atlas");
  auto opaque=texture(3,1,{123,45,67,255,17,93,181,255,255,255,255,255},true,out,"opaque");
  GLint wrapS,wrapT,mag,min;glGetTexParameteriv(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,&wrapS);glGetTexParameteriv(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,&wrapT);glGetTexParameteriv(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,&mag);glGetTexParameteriv(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,&min);
  GLint envMode;glGetTexEnviv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,&envMode);
  NSMutableArray* cases=[NSMutableArray array];
  [cases addObject:draw(edge,1,1,0,240.5f,160.5f,8,8,0,1,0,"alpha_midpoint",out)];
  [cases addObject:draw(edge,1,1,0,240.5f,160.5f,8,8,0,.5f,2,"alpha_vertex_additive",out)];
  [cases addObject:draw(atlas,2,2,0,240.25f,160.75f,12,9,0,1,0,"atlas_neighbor_padding",out)];
  [cases addObject:draw(atlas,2,2,3,240.25f,160.75f,-9,6,.37f,.65f,0,"atlas_flip_rotate",out)];
  [cases addObject:draw(atlas,1,1,0,240.5f,160.5f,.7f,.7f,.2f,1,0,"atlas_minification",out)];
  [cases addObject:draw(opaque,1,1,0,240.5f,160.5f,8,8,0,1,0,"rgb565_padding",out)];
  [cases addObject:draw(opaque,1,1,0,240.5f,160.5f,8,8,0,.5f,0,"rgb565_vertex_alpha",out)];
  [cases addObject:draw(edge,1,1,0,240.5f,160.5f,8,8,0,1,0,"trail_rgba_passthrough",out,true)];
  json(out/"manifest.json",@{@"status":@"actual CGL framebuffer readback",@"vendor":@((const char*)glGetString(GL_VENDOR)),@"renderer":@((const char*)glGetString(GL_RENDERER)),@"version":@((const char*)glGetString(GL_VERSION)),@"viewport":@[@480,@320],@"clearRGBA":@[@0,@0,@0,@1],@"frameRawOrder":@"RGBA8 rows bottom-to-top (+Y up)",@"textureRawOrder":@"CGBitmapContext storage; v0 is original top content row",@"wrapS":@(wrapS),@"wrapT":@(wrapT),@"minFilter":@(min),@"magFilter":@(mag),@"dither":@YES,@"textureEnvMode":@(envMode),@"cases":cases});
  glDeleteTextures(1,&edge.id);glDeleteTextures(1,&atlas.id);glDeleteTextures(1,&opaque.id);glDeleteRenderbuffersEXT(1,&attachment);glDeleteFramebuffersEXT(1,&framebuffer);CGLSetCurrentContext(nullptr);CGLDestroyContext(context);CGLDestroyPixelFormat(format);std::cout<<"CGL reference:8 cases written to "<<out<<"\n";return 0;
 }catch(const std::exception& e){json(out/"failure.json",@{@"status":@"failed",@"reason":@(e.what())});std::cerr<<e.what()<<"\n";if(context){CGLSetCurrentContext(nullptr);CGLDestroyContext(context);}if(format)CGLDestroyPixelFormat(format);return 1;}
}}
