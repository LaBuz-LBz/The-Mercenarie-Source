#include "MainMenuBackgroundRules.h"
#include <cassert>
#include <cmath>
#include <iostream>
int main(){
 using namespace MainMenuBackgroundRules;
 for(int w=1;w<8000;w+=71)for(int h=1;h<4500;h+=137){
  Crop p=cover(w,h);assert(p.left>=0&&p.top>=0&&p.right<=1.00001f&&p.bottom<=1.00001f);
  assert(p.right>p.left&&p.bottom>p.top);
  float aspect=(p.right-p.left)*ImageWidth/((p.bottom-p.top)*ImageHeight);
  assert(std::fabs(aspect-float(w)/h)<0.002f);
 }
 const int sizes[][2]={{640,480},{1920,1080},{1920,1200},{3440,1440},{3840,2160},{5120,1440},{1280,1024}};
 for(int i=0;i<7;++i){Crop p=cover(sizes[i][0],sizes[i][1]);assert(p.left<=0.64f&&p.right>=0.85f&&p.top==0&&p.bottom>=0.40f);}
 std::cout<<"PASS: fullscreen cover without distortion; title region retained from 5:4 to 32:9\n";
}
