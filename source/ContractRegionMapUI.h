// Included inside the existing contract-board namespace, after its UI helpers.
MyGUI::ImageBox* contractRegionOverlay=0;
std::vector<MyGUI::TextBox*> contractRegionLabels;
struct ContractRegionTexture : MyGUI::ITextureInvalidateListener {
    std::vector<unsigned char> pixels;
    std::set<int> selection;
    bool ready;
    ContractRegionTexture():ready(false){}
    void upload(MyGUI::ITexture* texture){
        if(!texture||pixels.empty())return;
        void* p=texture->lock(MyGUI::TextureUsage::Write);
        if(p){
            // Kenshi's D3D texture buffer stores BGRA; source raster is RGBA.
            unsigned char* out=static_cast<unsigned char*>(p);
            for(size_t i=0;i<pixels.size();i+=4){out[i]=pixels[i+2];out[i+1]=pixels[i+1];out[i+2]=pixels[i];out[i+3]=pixels[i+3];}
            texture->unlock();
        }
    }
    virtual void textureInvalidate(MyGUI::ITexture* texture){upload(texture);}
    void update(const ContractRegionRoute::Result& regions){
        MyGUI::RenderManager& manager=MyGUI::RenderManager::getInstance();
        MyGUI::ITexture* texture=manager.getTexture("MercenarieRegionOverlay");
        if(!texture){texture=manager.createTexture("MercenarieRegionOverlay");texture->createManual(2048,2048,MyGUI::TextureUsage::Dynamic|MyGUI::TextureUsage::Write,MyGUI::PixelFormat::R8G8B8A8);texture->setInvalidateListener(this);ready=false;}
        if(!ready){ContractRegionRoute::raster(regions,pixels);upload(texture);selection=regions.regions;ready=true;}
    }
} contractRegionTexture;

void createContractRegionMap(MyGUI::Widget* parent){
    contractRegionOverlay=parent->createWidget<MyGUI::ImageBox>("ImageBox",0,0,1,1,MyGUI::Align::Default);
    contractRegionOverlay->setNeedMouseFocus(false);contractRegionOverlay->setVisible(false);

}
void updateContractRegionMap(const BoardOffer& offer,bool search,int cropHeight,double scaleY){
    if(!contractRegionOverlay)return;
    contractRegionOverlay->setVisible(false);
    for(size_t i=0;i<contractRegionLabels.size();++i)contractRegionLabels[i]->setVisible(false);
    // Borders removed at the user's request. No rasterization or GPU upload.
    // Region risk calculation remains in the offer model.
    return;
}
