#include <map>
#include <vector>
#include <string>
#include <cassert>
#include <iostream>
#include "ArtisanLayout.h"
#include "ArtisanOrders.h"
struct Data {itemType type;std::string id;Data(itemType t,const char* s):type(t),id(s){}};
int main(){
 typedef std::map<itemType,std::vector<Data*> > Known;
 Known native;Data initial(ARMOUR,"initial"),locked(ARMOUR,"mod.author.samurai"),weapon(WEAPON,"mod.author.blade"),model(MATERIAL_SPECS_WEAPON,"model");
 assert(!ArtisanLayout::known(native,&initial));
 native[ARMOUR].push_back(&initial);assert(ArtisanLayout::known(native,&initial));assert(!ArtisanLayout::known(native,&locked));
 // No cache: a completed research changes the native table, and the next query sees it.
 native[ARMOUR].push_back(&locked);assert(ArtisanLayout::known(native,&locked));
 native[MATERIAL_SPECS_WEAPON].push_back(&model);assert(ArtisanLayout::known(native,&model));assert(!ArtisanLayout::known(native,&weapon));
 native[WEAPON].push_back(&weapon);assert(ArtisanLayout::known(native,&weapon));
 native.clear();assert(!ArtisanLayout::known(native,&locked)); // Save switch must not retain old knowledge.
 assert(ArtisanLayout::category(ATTACH_BODY)==2);assert(ArtisanLayout::category(ATTACH_LEGS)==4);
 assert(ArtisanLayout::category(ATTACH_HAT)==1);assert(ArtisanLayout::category(ATTACH_SHIRT)==3);
 assert(ArtisanLayout::category(ATTACH_GLOVES)==7);assert(ArtisanLayout::category(ATTACH_BOOTS)==5);
 assert(ArtisanLayout::category(ATTACH_BACKPACK)==8);assert(ArtisanLayout::category(ATTACH_NONE)==6);assert(ArtisanLayout::category(999)==6);
 ArtisanLayout::Layout reference(1920,1080),ultrawide(3440,1440),fourK(3840,2160);
 assert(reference.width==1612&&reference.height==907);
 assert(ultrawide.width==reference.width&&ultrawide.height==reference.height);
 assert(fourK.width==reference.width&&fourK.height==reference.height);
 assert(ArtisanLayout::Layout(1280,720).width==1256);
 const int sizes[][2]={{800,600},{1024,768},{1280,720},{1920,1080},{2560,1440}};
 for(int i=0;i<5;++i){ArtisanLayout::Layout l(sizes[i][0],sizes[i][1]);
  assert(l.width<=sizes[i][0]-24&&l.height<=sizes[i][1]-24);
  int available=l.width-32-(l.side+12)-22;ArtisanLayout::Row row(available,l.wideRows);
  assert(row.qualityX>=8&&row.qualityWidth>=126);assert(row.addWidth>=80);
  assert(row.addX+row.addWidth<=available-8);assert(row.quantityX+92<=row.addX);
  assert(l.height-130-54-(l.sideSummary?0:154)>=200);
  int search=l.width-(l.side+28)-238;assert(search>=200);
 }
 // Same equipment at two independent grades remains two basket lines.
 ArtisanOrders::Ledger ledger;ArtisanOrders::Line a;a.item="mod.armour";a.quality="armour:5";a.quantity=a.remaining=10;a.base=80;a.unit=100;a.hours=ArtisanOrders::series(80,10);
 assert(ledger.put("smith",a,10));a.quality="armour:20";assert(ledger.put("smith",a,10));assert(ledger.baskets["smith"].size()==2);
 assert(ledger.baskets["smith"][0].quality=="armour:5");assert(ledger.baskets["smith"][1].quality=="armour:20");
 std::cout<<"Artisan catalogue: native-table fixtures, unlock refresh, type/model separation, slots, five resolutions, independent grades passed\n";
}
