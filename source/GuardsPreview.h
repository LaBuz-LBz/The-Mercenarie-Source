#pragma once
namespace GuardsNative {
struct Marker {MyGUI::Button* label;MyGUI::Widget* dots[12];Marker():label(0){for(int i=0;i<12;++i)dots[i]=0;}};
std::map<GuildGuards::Id,Marker> markers;
bool project(const Ogre::Vector3& world,MyGUI::IntPoint& out){
    if(!ou||!ou->player||!ou->player->getCamera()||!ou->player->getCamera()->camera)return false;
    Ogre::Camera* camera=ou->player->getCamera()->camera;
    Ogre::Vector4 point=camera->getProjectionMatrix()*camera->getViewMatrix()*Ogre::Vector4(world.x,world.y,world.z,1);
    if(point.w<=0)return false;float x=point.x/point.w,y=point.y/point.w;if(x<-1||x>1||y<-1||y>1)return false;
    const MyGUI::IntSize& view=MyGUI::RenderManager::getInstance().getViewSize();out=MyGUI::IntPoint((int)((x+1)*.5f*view.width),(int)((1-y)*.5f*view.height));return true;
}
void destroyMarker(Marker& marker){if(v9WidgetLive(marker.label))mercenarieDestroyLiveWidget(marker.label);for(int i=0;i<12;++i)if(v9WidgetLive(marker.dots[i]))mercenarieDestroyLiveWidget(marker.dots[i]);}
void clearPreview(){for(std::map<GuildGuards::Id,Marker>::iterator i=markers.begin();i!=markers.end();++i)destroyMarker(i->second);markers.clear();}
void updatePreview(){
    if(!preview||!MyGUI::Gui::getInstancePtr())return;
    for(std::map<GuildGuards::Id,Marker>::iterator i=markers.begin();i!=markers.end();)if(!guildGuards.posts.count(i->first)){destroyMarker(i->second);markers.erase(i++);}else ++i;
    for(std::map<GuildGuards::Id,GuildGuards::Post>::const_iterator i=guildGuards.posts.begin();i!=guildGuards.posts.end();++i){
        const GuildGuards::Post& p=i->second;Marker& m=markers[i->first];MyGUI::IntPoint origin;bool visible=project(position(p.position),origin);
        if(!m.label&&visible){m.label=MyGUI::Gui::getInstance().createWidget<MyGUI::Button>("Kenshi_Button1",0,0,260,26,MyGUI::Align::Default,"Overlapped");m.label->setUserString("guardAction","post:"+number(p.id));m.label->eventMouseButtonClick+=MyGUI::newDelegate(click);for(int d=0;d<12;++d){m.dots[d]=MyGUI::Gui::getInstance().createWidget<MyGUI::Widget>("WhiteSkin",0,0,4,4,MyGUI::Align::Default,"Overlapped");m.dots[d]->setColour(registerAmber);m.dots[d]->setNeedMouseFocus(false);}}
        if(!m.label)continue;m.label->setVisible(visible);if(visible){const MyGUI::IntSize& view=MyGUI::RenderManager::getInstance().getViewSize();m.label->setPosition(std::max(0,std::min(view.width-260,origin.left-130)),std::max(0,origin.top-30));std::string caption=number(p.priority)+" | "+guildGuards.tags[p.tag].name+" | "+p.name;if(allocator.owners.count(p.id))caption+=" | "+guildGuards.guards[allocator.owners[p.id]].name;MercenarieFonts::caption(m.label,caption);}
        double heading=p.heading;if(p.id==selectedPost&&angleEdit){std::istringstream editValue(angleEdit->getOnlyText());double draft;if(editValue>>draft&&draft>=-1e9&&draft<=1e9)heading=angleEdit->getOnlyText()==displayedAngle?draftHeading:draft;}
        double angle=heading*3.141592653589793/180;
        for(int d=0;d<12;++d){double along=d<8?d*.35:2.1-(d-8)/2*.35,side=d<8?0:((d%2)?1:-1)*(.35+(d-8)/2*.35);GuildGuards::Position point=p.position;point.x+=std::sin(angle)*along+std::cos(angle)*side;point.z+=std::cos(angle)*along-std::sin(angle)*side;MyGUI::IntPoint screen;bool shown=visible&&project(position(point),screen);m.dots[d]->setVisible(shown);if(shown)m.dots[d]->setPosition(screen.left-2,screen.top-2);}
    }
}
}
