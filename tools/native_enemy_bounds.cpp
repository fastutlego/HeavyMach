// Read-only constructor audit. Never mutates source resources or game code.
// Build with the real HM_VALIDATION core; run: native_enemy_bounds Resources result.json
#include "GameSession.hpp"
#include <algorithm>
#include <bit>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>
using namespace heavymach;
static std::string quoted(const std::string& s){std::string out="\"";for(unsigned char c:s){if(c=='"'||c=='\\')out+='\\';if(c<32)throw std::runtime_error("control character in JSON string");out+=char(c);}return out+'"';}
static std::string bits(float v){std::ostringstream s;s<<"\"0x"<<std::hex<<std::setw(8)<<std::setfill('0')<<std::bit_cast<uint32_t>(v)<<"\"";return s.str();}
static void snapshot(std::ostream& out,const GameSession::ValidationEnemySnapshot& e){
 auto b=e.bounds;out<<"{\"name\":"<<quoted(e.name)<<",\"sessionID\":"<<e.id<<",\"boundsOrder\":[\"left\",\"bottom\",\"right\",\"top\"],\"bounds\":["<<std::setprecision(9)<<b.left<<','<<b.bottom<<','<<b.right<<','<<b.top<<"],\"boundsBinary32\":["<<bits(b.left)<<','<<bits(b.bottom)<<','<<bits(b.right)<<','<<bits(b.top)<<"],\"spawn\":["<<e.spawnPosition.x<<','<<e.spawnPosition.y<<"],\"hp\":"<<e.hp<<"}";
}
int main(int argc,char** argv){
 if(argc!=3){std::cerr<<"usage: native_enemy_bounds Resources output.json\n";return 2;}
 const std::filesystem::path resources=argv[1];auto data=GameData::loadDirectory(resources/"RawData");
 std::unordered_map<std::string,Vector2> sizes;
 auto dimensions=[&](std::string_view key){std::string name(key);auto found=sizes.find(name);if(found!=sizes.end())return found->second;
  auto path=resources/"Images"/name;if(!std::filesystem::exists(path))path+=".png";
  std::ifstream f(path,std::ios::binary);unsigned char b[24]{};f.read(reinterpret_cast<char*>(b),24);
  const unsigned char signature[]={137,80,78,71,13,10,26,10};if(f.gcount()!=24||!std::equal(std::begin(signature),std::end(signature),b))throw std::runtime_error("Missing/non-PNG image: "+path.string());
  auto be=[&](int i){return uint32_t(b[i])<<24|uint32_t(b[i+1])<<16|uint32_t(b[i+2])<<8|b[i+3];};Vector2 size{float(be(16)),float(be(20))};sizes.emplace(name,size);return size;
 };
 std::ofstream out(argv[2]);if(!out)throw std::runtime_error("Cannot write output");
 out<<"{\"schema\":1,\"method\":\"GameSession.start(0,0,0) -> World.openMap/processRegen -> spawnEnemy -> makeMachFromDefinition -> PartsDefinition -> enemyParts/addPart -> validationEnemies; no tick/input\",\"machineCount\":"<<data.machines.size()<<",\"naturalStage01Initial\":[";
 {SaveSlot slot;uint32_t seed=11;GameSession session(data,slot,dimensions,[&](){seed=seed*1664525+1013904223;return int32_t(seed>>1);});session.start(0,0,0);bool first=true;for(const auto& e:session.validationEnemies()){if(!first)out<<',';first=false;snapshot(out,e);}}
 out<<"],\"syntheticConstruction\":[";
 std::vector<std::string> names;for(const auto& [name,definition]:data.machines)names.push_back(name);std::sort(names.begin(),names.end());
 auto& map=data.maps.at("Stage01");Vector2 spawn{map.left+200,180};int errors=0;bool first=true;std::vector<std::pair<std::string,std::string>> unsupported;
 for(const auto& name:names){if(!first)out<<',';first=false;
  // Only this in-memory map placement changes. Original Mach/Parts/Weapon
  // records, Stage01 terrain, image dimensions, and constructor code stay intact.
  map.enemies={{name,spawn,false}};
  try{SaveSlot slot;uint32_t seed=11;GameSession session(data,slot,dimensions,[&](){seed=seed*1664525+1013904223;return int32_t(seed>>1);});session.start(0,0,0);
   auto enemies=session.validationEnemies();if(enemies.size()!=1||enemies[0].name!=name)throw std::runtime_error("fixture did not construct exactly requested machine");
   snapshot(out,enemies[0]);
   for(const auto& event:session.takeEvents())if(event.kind==SessionEventKind::Unsupported)unsupported.emplace_back(name,event.text);
  }catch(const std::exception& e){++errors;out<<"{\"name\":"<<quoted(name)<<",\"error\":"<<quoted(e.what())<<'}';}
 }
 out<<"],\"tileDimensionsUsed\":[";std::vector<std::string> images;for(const auto& [name,size]:sizes)images.push_back(name);std::sort(images.begin(),images.end());first=true;
 for(const auto& name:images){if(!first)out<<',';first=false;auto s=sizes.at(name);out<<"{\"name\":"<<quoted(name)<<",\"width\":"<<s.x<<",\"height\":"<<s.y<<'}';}
 out<<"],\"unsupportedEvents\":[";first=true;for(const auto& [name,message]:unsupported){if(!first)out<<',';first=false;out<<"{\"name\":"<<quoted(name)<<",\"message\":"<<quoted(message)<<'}';}
 out<<"],\"constructionErrors\":"<<errors<<"}\n";out.close();std::cout<<"constructed "<<names.size()-errors<<'/'<<names.size()<<" machines; output="<<argv[2]<<"\n";return errors?1:0;
}
