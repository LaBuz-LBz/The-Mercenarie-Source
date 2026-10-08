// Presentation only: the original bill, debt and payment callbacks remain authoritative.
#include "src/PayrollPresentation.h"
bool payrollSheetDetails=false;
MyGUI::TextBox* psText(MyGUI::Widget* parent,int x,int y,int w,int h,const std::string& value,const MyGUI::Colour& colour,int font=16){
 MyGUI::TextBox* t=registerText(parent,x,y,w,h,font,"",colour);
 for(int f=font;f>=12;--f){t->setFontHeight(f);if(popupWrap(t,value,w)<=h)break;}
 t->setTextAlign(MyGUI::Align::Left|MyGUI::Align::VCenter);return t;
}
void psIcon(MyGUI::Widget* parent,int id,int x,int y,int size){MyGUI::ImageBox* i=parent->createWidget<MyGUI::ImageBox>("ImageBox",x,y,size,size,MyGUI::Align::Default);i->setImageTexture("MercenariePayrollIcons.png");i->setImageCoord(MyGUI::IntCoord(id*64,0,64,64));i->setNeedMouseFocus(false);}
std::string payrollDuration(double hours){long long m=PayrollPresentation::minutes(hours);if(hours>0&&m==0)return Loc::text("payroll.sheet.less_minute");Loc::Catalogue a;a["days"]=payrollNumber(m/1440);a["hours"]=payrollNumber(m/60%24);a["minutes"]=payrollNumber(m%60);return Loc::format("payroll.sheet.duration",a);}
void buildPayrollSheet(){
 GuildPayroll::Bill* bill=guildPayroll.pending();if(!bill)return;
 if(payrollDisplayedBill!=bill->id)payrollSheetDetails=false;
 payrollClose(payrollSheetWindow);payrollSheetWindow=payrollWindow("payroll.sheet");payrollDisplayedBill=bill->id;
 MyGUI::ScrollView* s=payrollScroll(payrollSheetWindow,64,85);int width=s->getWidth()-18;
 Loc::Catalogue a;a["start"]=payrollDate(bill->start);a["end"]=payrollDate(bill->end);
 GuildPayroll::Money unpaidThisBill=0;for(size_t d=0;d<guildPayroll.debts.size();++d)if(guildPayroll.debts[d].category==GuildPayroll::Salary&&guildPayroll.debts[d].hour==bill->end)unpaidThisBill+=guildPayroll.debts[d].remaining;
 const int left=width*68/100,right=width-left,rowH=30,infoH=7*rowH+14;
 psIcon(s,5,8,14,32);psText(s,48,4,left-56,32,Loc::text("payroll.sheet.general"),registerIvory,19);
 const char* keys[]={"period","new","cash","old","limbs","due"};const int icons[]={6,5,5,8,10,8};
 std::string values[]={Loc::format("payroll.sheet.period_value",a),payrollMoney(bill->total),payrollMoney(financeBalance()),payrollMoney(guildPayroll.debt(GuildPayroll::Salary)-unpaidThisBill),payrollMoney(guildPayroll.debt(GuildPayroll::Limb)),payrollMoney(guildPayroll.debt())};
 for(int i=0;i<6;++i){int y=36+i*rowH;if(i==5){registerSolid(s,44,y,left-52,rowH,MyGUI::Colour(.11f,.10f,.065f));}
  psIcon(s,icons[i],48,y+7,16);psText(s,70,y,left-78,rowH,std::string(Loc::text((std::string("payroll.sheet.")+keys[i]).c_str()))+" : "+values[i],i==5?registerAmber:registerIvory,16);
 }
 registerSolid(s,left,10,1,infoH-20,MyGUI::Colour(.35f,.30f,.20f));
 int logo=std::min(88,right-24);MyGUI::ImageBox* mark=s->createWidget<MyGUI::ImageBox>("ImageBox",left+(right-logo)/2,18,logo,logo,MyGUI::Align::Default);mark->setImageTexture("MercenarieOverviewIcons.png");mark->setImageCoord(MyGUI::IntCoord(0,0,128,128));mark->setColour(registerAmber);mark->setNeedMouseFocus(false);
 psText(s,left+12,logo+25,right-24,32,"THE MERCENARIE",registerIvory,18)->setTextAlign(MyGUI::Align::Center);
 psText(s,left+18,logo+62,right-36,infoH-logo-66,Loc::text("payroll.sheet.motto"),registerIvory,14);
 int y=infoH+8;psText(s,8,y,width-16,30,Loc::text("payroll.sheet.by_rank"),registerAmber,18);y+=32;
 const int cols[]={0,width*35/100,width*54/100,width*75/100,width};
 const char* titles[]={"rank","count","daily","period_total"};
 for(int c=0;c<4;++c)psText(s,cols[c]+8,y,cols[c+1]-cols[c]-16,44,Loc::text((std::string("payroll.sheet.")+titles[c]).c_str()),registerIvory,14);y+=46;
 PayrollPresentation::Summary summary=PayrollPresentation::summarize(*bill);
 for(int r=4;r>=0;--r){const PayrollPresentation::Rank& group=summary.ranks[r];registerSolid(s,4,y,width-8,1,MyGUI::Colour(.24f,.27f,.26f));psIcon(s,r,8,y+9,22);
  std::string rate=group.rates.empty()?"—":group.rates.size()==1&&!group.changed?payrollMoney(*group.rates.begin()):Loc::text("payroll.sheet.variable");
  psText(s,36,y+2,cols[1]-44,36,Loc::text(payrollRankKey(r)),registerIvory,16);
  const std::string cells[]={payrollNumber(group.count),rate,payrollMoney(group.total)};
  for(int c=1;c<4;++c)psText(s,cols[c]+8,y+2,cols[c+1]-cols[c]-16,36,cells[c-1],registerIvory,16);y+=40;
 }
 registerSolid(s,4,y,width-8,40,MyGUI::Colour(.11f,.10f,.065f));psIcon(s,9,8,y+9,22);
 psText(s,36,y+2,cols[1]-44,36,Loc::text("payroll.sheet.grand_total"),registerAmber,17);psText(s,cols[1]+8,y+2,cols[2]-cols[1]-16,36,payrollNumber(summary.count),registerAmber,17);psText(s,cols[3]+8,y+2,width-cols[3]-16,36,payrollMoney(summary.total),registerAmber,17);y+=46;
 psText(s,8,y,width-16,40,Loc::text("payroll.sheet.group_note"),registerIvory,13);y+=44;
 payrollUiButton(s,8,y,width-16,38,Loc::text(payrollSheetDetails?"payroll.sheet.hide_details":"payroll.sheet.details"),"sheet_details",registerAmber);y+=46;
 if(payrollSheetDetails)for(size_t i=0;i<bill->lines.size();++i){const GuildPayroll::PayLine& line=bill->lines[i];y=payrollLine(s,y,width,line.name+" — "+Loc::text(payrollRankKey(line.rank)),true);
  if(line.robot)y=payrollLine(s,y,width,Loc::text("payroll.robot"));
  else for(size_t j=0;j<line.segments.size();++j){const GuildPayroll::Segment& seg=line.segments[j];Loc::Catalogue p;p["rank"]=Loc::text(payrollRankKey(seg.rank));p["rate"]=payrollMoney(seg.rate);p["duration"]=payrollDuration(seg.hours);p["amount"]=payrollMoney(GuildPayroll::rounded(seg.hours*seg.rate/24));y=payrollLine(s,y,width,Loc::format("payroll.sheet.segment",p));}
  y=payrollLine(s,y,width,std::string(Loc::text("payroll.total"))+" "+payrollMoney(line.amount));
 }
 s->setCanvasSize(width,y+12);
 MyGUI::Widget* p=payrollSheetWindow->getClientWidget();int bw=(p->getWidth()-48)/2;
 MyGUI::Button* pay=payrollUiButton(p,16,p->getHeight()-70,bw,52,"","pay",registerGreen);
 popupIcon(pay,1,14,16,20,registerGreen);psText(pay,40,4,bw-80,44,Loc::text("payroll.pay"),registerGreen,18)->setTextAlign(MyGUI::Align::Center);
 MyGUI::Colour red(.85f,.18f,.20f);MyGUI::Button* refuse=payrollUiButton(p,32+bw,p->getHeight()-70,bw,52,"","refuse",red);
 popupIcon(refuse,2,14,16,20,red);psText(refuse,40,4,bw-80,44,Loc::text("payroll.refuse"),red,18)->setTextAlign(MyGUI::Align::Center);
 MyGUI::InputManager::getInstance().addWidgetModal(payrollSheetWindow);payrollSheetViewport=MyGUI::RenderManager::getInstance().getViewSize();payrollSheetLanguage=Loc::engine().language;
}
