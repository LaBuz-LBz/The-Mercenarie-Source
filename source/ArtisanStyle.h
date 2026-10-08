// Presentation helpers; all equipment images still come from artisanIcon.
MyGUI::ImageBox* artisanImage(MyGUI::Widget* p,int x,int y,int w,int h,const char* texture,float alpha=1){
 MyGUI::ImageBox* i=p->createWidget<MyGUI::ImageBox>("ImageBox",x,y,w,h,MyGUI::Align::Default);i->setImageTexture(texture);i->setAlpha(alpha);i->setNeedMouseFocus(false);return i;
}
MyGUI::Widget* artisanPanel(MyGUI::Widget* p,int x,int y,int w,int h){
 return p->createWidget<MyGUI::Widget>("ArtisanPanel",x,y,w,h,MyGUI::Align::Default);
}
void artisanMark(MyGUI::Widget* p,int x,int y,int size,int kind,const MyGUI::Colour& colour){
 MyGUI::ImageBox* i=artisanImage(p,x,y,size,size,"MercenarieArtisanIcons.png");i->setImageCoord(MyGUI::IntCoord(kind*64,0,64,64));i->setColour(colour);
}
MyGUI::TextBox* artisanHeading(MyGUI::Widget* p,int x,int y,int w,int h,int size,const std::string& text){
 MyGUI::TextBox* t=artisanText(p,x,y,w,h,size,text,registerIvory);artisanSetFont(t,size,true);t->setSize(w,std::max(h,artisanWrap(t,text,w)));return t;
}
void artisanBrand(MyGUI::Widget* p,int x,int y,int width){
 MyGUI::ImageBox* i=artisanImage(p,x,y,60,60,"LevelSun.png");i->setColour(registerAmber);
 artisanHeading(p,x+74,y+5,width-74,28,24,"THE MERCENARIE");
 artisanText(p,x+74,y+37,width-74,22,14,Loc::text("artisan.tagline"),registerIvory);
}
MyGUI::Button* artisanAction(MyGUI::Widget* p,int x,int y,int w,int h,const std::string& caption,const char* action,bool primary=false,int row=-1){
 const char* skin=std::string(action)=="collect"?"ArtisanPositive":primary?"ArtisanPrimary":"ArtisanButton";
 MyGUI::Button* b=artisanButton(p,x,y,w,caption,action,row,skin);b->setSize(w,h);
 for(int f=h>=52?22:18;f>=12;f-=2){artisanSetFont(b,f);if(artisanWrap(b,caption,w-24)<=h-8)break;}
 if(primary)b->setStateSelected(true);return b;
}
int artisanCell(MyGUI::Widget* p,int x,int y,int w,const std::string& value,bool heading=false){
 MyGUI::TextBox* t=artisanText(p,x+10,y+10,w-20,26,heading?16:18,value,heading?MyGUI::Colour(.80f,.78f,.70f):registerIvory);return t->getHeight()+20;
}
void artisanGrid(MyGUI::Widget* p,const std::vector<int>& edges,int y,int height){
 for(size_t i=0;i<edges.size();++i)registerSolid(p,edges[i],y,1,height,MyGUI::Colour(.20f,.23f,.22f));artisanDivider(p,y+height,edges.back());
}
double artisanRatio(const ArtisanOrders::Order& o){
 if(o.status==ArtisanOrders::Ready||o.status==ArtisanOrders::Collected)return 1;
 if(o.status!=ArtisanOrders::Making||o.end<=o.start)return 0;
 return std::max(0.0,std::min(1.0,(artisanNow()-o.start)/(o.end-o.start)));
}
std::string artisanMoney(long long n){std::string s=artisanNumber(n);for(int i=(int)s.size()-3;i>0;i-=3)s.insert(i," ");return s+" Cats";}
int artisanEmpty(MyGUI::Widget* p,int width,int height,const char* title,const char* explanation,int icon){
 MyGUI::TextBox* t=artisanText(p,20,0,width-40,32,22,Loc::text(title),registerIvory);t->setTextAlign(MyGUI::Align::Center);
 MyGUI::TextBox* body=artisanText(p,32,0,width-64,40,16,Loc::text(explanation),registerIvory);body->setTextAlign(MyGUI::Align::Center);
 int block=64+18+t->getHeight()+12+body->getHeight(),top=std::max(12,(height-block)/2);
 artisanMark(p,(width-64)/2,top,64,icon,registerAmber);t->setPosition(20,top+82);body->setPosition(32,top+82+t->getHeight()+12);return top+block+12;
}
