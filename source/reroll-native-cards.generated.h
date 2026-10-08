void createActualClassicCards(MyGUI::Widget* list,int viewportW,int viewportH){int w=std::min(1540,viewportW-24),h=std::min(980,viewportH-24);int leftW=w*28/100,bodyH=h-106,offersY=52,offerH=(bodyH-offersY-54)/6-6;
        for(int i=0;i<6;++i){
            MyGUI::Widget* row=boardPanelV6(list,8,offersY+i*(offerH+6),leftW-16,offerH);
            contractButtons[i]=row->createWidget<MyGUI::Button>("Kenshi_Button1",1,1,row->getWidth()-2,offerH-2,MyGUI::Align::Default);
            contractButtons[i]->eventMouseButtonClick+=MyGUI::newDelegate(contractOfferClicked);
            int iconSize=std::min(60,offerH-22),textX=iconSize+20,tw=row->getWidth()-textX-20;
            offerIconV6[i]=boardIconV6(row,0,8,(offerH-iconSize)/2,iconSize,missionColourV6(0));
            offerNameV6[i]=registerText(row,textX,9,std::max(1,tw-34),offerH/3-2,18,"",registerIvory);
            offerTypeV6[i]=registerText(row,textX,offerH/3+5,std::max(1,tw-34),offerH/3-4,16,"",registerIvory);
            offerRarityV6[i]=registerText(row,textX,offerH*2/3+2,tw*42/100,offerH/3-6,14,"",registerIvory);
            offerPriceV6[i]=registerText(row,textX+tw*42/100,offerH*2/3+2,tw*58/100,offerH/3-6,19,"",registerAmber);offerPriceV6[i]->setTextAlign(MyGUI::Align::Right|MyGUI::Align::Top);
            offerEdgesV6[i][0]=registerSolid(row,0,0,row->getWidth(),2,registerAmber);offerEdgesV6[i][1]=registerSolid(row,0,offerH-2,row->getWidth(),2,registerAmber);offerEdgesV6[i][2]=registerSolid(row,0,0,2,offerH,registerAmber);offerEdgesV6[i][3]=registerSolid(row,row->getWidth()-2,0,2,offerH,registerAmber);
            offerRerollV6[i]=createContractRerollButton(row,i,false);
            contractButtons[i]->eventToolTip+=MyGUI::newDelegate(bookRerollTooltip);
        }
}
