#pragma once
#include "Types.hpp"
#include "Audio.hpp"
#include "Trail.hpp"
#include <android/native_window.h>
#include <EGL/egl.h>
#define GL_GLEXT_PROTOTYPES 1
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <unordered_map>
namespace hm {
class Media {
 struct Texture {int width=0,height=0,pw=0,ph=0;GLuint id=0;bool rgb=false;std::vector<uint8_t> pixels;};
 AAssetManager* assets_;std::unordered_map<std::string,Texture> textures_;
 EGLDisplay display_=EGL_NO_DISPLAY;EGLContext context_=EGL_NO_CONTEXT;EGLSurface surface_=EGL_NO_SURFACE;ANativeWindow* window_=nullptr;
 GLuint sceneFBO_=0,sceneBuffer_=0,sceneTexture_=0;int bufferWidth_=0,bufferHeight_=0;bool copied_=false;
 Texture* texture(const std::string&);void transform(CGPoint={0,0},CGSize={1,1},double=0);
 void allocateScene();void handleEGLFailure(const char*);
public:
 explicit Media(AAssetManager*);~Media();
 std::unique_ptr<Audio> audio;
 std::string resourceScope="shared";CGPoint presentationOffset{};
 double width=480,height=320;int drawableWidth=0,drawableHeight=0;
 CGPoint _drawingOffset{},camera_{};double _drawingScale=1;bool _active=false;
 uint64_t _drawCalls=0,frames=0,uploads=0,contextCreations=0;
 void recreateContext();void attach(ANativeWindow*);void detach();bool begin(double,double);bool present();
 CGSize imageSize(const std::string&);CGSize imageSize(const std::string&,const std::string&);
 void releaseScope(const std::string&);void setCameraOffset(CGPoint p){camera_=p;}
 void setDrawingOffset(CGPoint p){_drawingOffset={p.x+presentationOffset.x,p.y+presentationOffset.y};}
 void setDrawingScale(double s){_drawingScale=s;}void setClipRect(CGRect);void resetClip();
 void sound(const std::string& n,float pan=0){audio->sound(n,pan);}
 void draw(const std::string&,double,double,int=1,int=1,int=0,double=1,double=1,double=0,double=1,int=0);
 void drawRegion(const std::string&,CGRect,CGRect);
 void drawJoinedBackground(const std::string&,double,double,double,double);
 void drawExpandedPanel(const std::string&,double,double,double,double,double,double);
 void drawEdgeContinuation(const std::string&,double,double,double,double,double);
 void drawTrail(const heavymach::TrailMesh&);
 void lines(const float*,int,float,float,float,float=1);
 std::vector<uint8_t> snapshot();
};
}
