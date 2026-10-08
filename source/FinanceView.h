#include "src/FinancePresentation.h"
// Finance-only presentation. The overview owns the shared header and sidebar.
std::string fnText(const char* key){return Loc::text((std::string("finance.")+key).c_str());}
const char* fnCategories[]={"contracts","purchases","sales","wages","offices","maintenance","equipment","travel","taxes","investments","other","rents"};
MyGUI::Colour fnRed(1,.19f,.12f),fnGrey(.55f,.60f,.61f);
MyGUI::Widget* fn106Stats=0;
MyGUI::Widget *fnDashboard=0,*fnHistory=0,*fnTax=0,*fnPlot=0;
MyGUI::ScrollView* fnScroll=0;
MyGUI::ComboBox *fnPeriod=0,*fnFilterBox=0,*fnJournalPeriod=0;
MyGUI::TextBox *fnCards[5]={0},*fnSummary[Finance::Count]={0},*fnExpenses[Finance::Count]={0},*fnRecent[6][4]={{0}},*fnRows[16][5]={{0}};
MyGUI::TextBox *fnChartEmpty=0,*fnChartMax=0,*fnChartMin=0,*fnDonutTotal=0,*fnCoverage=0,*fnPageText=0,*fnTableEmpty=0,*fnTaxDetail[2]={0};
MyGUI::Widget *fnBars[2][90]={{0}},*fnLine[360]={0},*fnRing[144]={0};
MyGUI::Button *fnOpen=0,*fnBack=0,*fnPrevious=0,*fnNext=0,*fnTaxOpen=0,*fnTaxBack=0;
MyGUI::TextBox *fnDebt=0,*fnRecentEmpty=0,*fnSummaryNames[Finance::Count]={0},*fnDebtValues[2][4]={{0}};
std::vector<int> fnFilterMap;int fnJournalDays=0;std::string fnFiscalCache;
int fnFont=14,fnDays=30,fnFilter=0,fnPage=0,fnCapacity=8,fnPlotW=1,fnPlotH=1;
unsigned long fnRevision=0;long long fnBalanceCache=-2;long fnDayCache=-1;
bool fnDirty=true;std::string fnLanguage;
void refreshFinanceView();
std::string fnNumber(long long n){std::ostringstream o;o<<n;std::string s=o.str();for(int i=(int)s.size()-3;i>(n<0?1:0);i-=3)s.insert(i," ");return s;}
std::string fnCats(long long n,bool sign=true){return (sign&&n>0?"+":"")+fnNumber(n)+Loc::text("ui.cats");}
MyGUI::Colour fnColour(long long n){return n>0?registerGreen:n<0?fnRed:fnGrey;}
void fnSet(MyGUI::TextBox* t,const std::string& s,const MyGUI::Colour& colour){if(!t)return;t->setFontHeight(fnFont);t->setTextColour(colour);std::stringstream in(gcWrap(t,s,fnFont));std::string line,out;int capacity=std::max(1,t->getHeight()/(fnFont+2));for(int i=0;i<capacity&&std::getline(in,line);++i){if(i==capacity-1&&in.peek()!=EOF){do{if(line.empty())break;size_t end=line.size()-1;while(end>0&&((unsigned char)line[end]&0xc0)==0x80)--end;line.erase(end);MercenarieFonts::caption(t,line+"...");}while(t->getTextSize().width>t->getWidth()-2);line+="...";}if(!out.empty())out+='\n';out+=line;}MercenarieFonts::caption(t,out);}
void fnMoney(MyGUI::TextBox* t,const std::string& value,const MyGUI::Colour& colour){if(!t)return;MercenarieFonts::caption(t,value);t->setTextColour(colour);for(int font=fnFont;font>=10;--font){t->setFontHeight(font);if(t->getTextSize().width<=t->getWidth())return;}MercenarieFonts::caption(t,gcWrap(t,value,10));}
MyGUI::TextBox* fnLabel(MyGUI::Widget* p,int x,int y,int w,int h,const std::string& s,const MyGUI::Colour& colour=registerIvory){MyGUI::TextBox* t=overviewText(p,x,y,std::max(1,w),std::max(1,h),fnFont,"",colour);fnSet(t,s,colour);return t;}
void fnWrap(MyGUI::Widget* p,int x,int y,int w,int h,const std::string& s,const MyGUI::Colour& colour=fnGrey){MyGUI::TextBox* t=fnLabel(p,x,y,w,h,s,colour);t->setTextAlign(MyGUI::Align::Left|MyGUI::Align::Top);}
MyGUI::Widget* fnPanel(MyGUI::Widget* p,int x,int y,int w,int h,const std::string& title){MyGUI::Widget* panel=overviewPanel(p,GuildResponsive::Rect(x,y,w,h));fnLabel(panel,10,4,w-20,24,title);registerLine(panel,8,30,w-16);return panel;}
MyGUI::ImageBox* fnIcon(MyGUI::Widget* p,int id,int x,int y,int size,const MyGUI::Colour& colour){MyGUI::ImageBox* icon=registerTextureIcon(p,"MercenarieFinanceIcons.png",x,y,size,colour);icon->setImageCoord(MyGUI::IntCoord(id*96,0,96,96));return icon;}
void resetFinanceView(){
 estateResetView();fn106Stats=0;
 fnDashboard=fnHistory=fnTax=fnPlot=0;fnScroll=0;fnPeriod=fnFilterBox=fnJournalPeriod=0;fnDebt=fnRecentEmpty=0;fnFilterMap.clear();fnFiscalCache.clear();fnOpen=fnBack=fnPrevious=fnNext=fnTaxOpen=fnTaxBack=0;
 fnChartEmpty=fnChartMax=fnChartMin=fnDonutTotal=fnCoverage=fnPageText=fnTableEmpty=0;
 for(int i=0;i<5;++i)fnCards[i]=0;for(int i=0;i<Finance::Count;++i)fnSummary[i]=fnExpenses[i]=fnSummaryNames[i]=0;
 for(int i=0;i<6;++i)for(int j=0;j<4;++j)fnRecent[i][j]=0;for(int i=0;i<16;++i)for(int j=0;j<5;++j)fnRows[i][j]=0;
 for(int i=0;i<2;++i){fnTaxDetail[i]=0;for(int k=0;k<4;++k)fnDebtValues[i][k]=0;for(int j=0;j<90;++j)fnBars[i][j]=0;}
 for(int i=0;i<360;++i)fnLine[i]=0;for(int i=0;i<144;++i)fnRing[i]=0;
 guildFinancesPanel=0;fnDirty=true;fnRevision=0;fnBalanceCache=-2;fnPage=0;
}
void fnClick(MyGUI::Widget* sender){
 if(!fnDashboard||!fnHistory||!fnTax)return;if(fn106Stats)fn106Stats->setVisible(false);
 if(sender==fnOpen){fnDashboard->setVisible(false);fnHistory->setVisible(true);fnTax->setVisible(false);fnPage=0;fnJournalDays=fnDays;fnJournalPeriod->setIndexSelected(fnDays==7?0:fnDays==30?1:2);if(fnDays==90)fnJournalDays=0;}
 else if(sender==fnBack||sender==fnTaxBack){fnDashboard->setVisible(true);fnHistory->setVisible(false);fnTax->setVisible(false);}
 else if(sender==fnTaxOpen){fnDashboard->setVisible(false);fnHistory->setVisible(false);fnTax->setVisible(true);}
 else if(sender==fnPrevious)fnPage=std::max(0,fnPage-1);
 else if(sender==fnNext)++fnPage;
 fnDirty=true;refreshFinanceView();
}
void fnPeriodChange(MyGUI::ComboBox* sender,size_t index){if(!fnPeriod||sender!=fnPeriod||index>2)return;const int periods[]={7,30,90};fnDays=periods[index];fnDirty=true;refreshFinanceView();}
void fnFilterChange(MyGUI::ComboBox* sender,size_t index){if(!fnFilterBox||sender!=fnFilterBox||index>=fnFilterMap.size())return;fnFilter=fnFilterMap[index];fnPage=0;fnDirty=true;refreshFinanceView();}
void fnJournalPeriodChange(MyGUI::ComboBox* sender,size_t index){if(!fnJournalPeriod||sender!=fnJournalPeriod||index>2)return;const int periods[]={7,30,0};fnJournalDays=periods[index];fnPage=0;fnDirty=true;refreshFinanceView();}
MyGUI::Button* fnButton(MyGUI::Widget* p,int x,int y,int w,int h,const std::string& text){MyGUI::Button* b=overviewButton(p,x,y,w,h,"");fnLabel(b,6,2,w-12,h-4,text,registerAmber);b->eventMouseButtonClick+=MyGUI::newDelegate(fnClick);return b;}
void fnRenderRows(){
 const Finance::Journal& j=fiscalLedger.cash;
 std::vector<Finance::Entry> entries=Finance::displayEntries(j,currentGameHours,fnJournalDays);std::sort(entries.begin(),entries.end(),Finance::NewestFirst());
 std::vector<size_t> visible;for(size_t k=0;k<entries.size();++k)if(Finance::matches(entries[k],fnFilter))visible.push_back(k);
 int pages=std::max(1,((int)visible.size()+fnCapacity-1)/fnCapacity);fnPage=std::max(0,std::min(fnPage,pages-1));
 fnSet(fnPageText,fnNumber(fnPage+1)+" / "+fnNumber(pages)+"   |   "+fnNumber(visible.size())+" "+fnText("transactions"),registerIvory);
 fnPrevious->setEnabled(fnPage>0);fnNext->setEnabled(fnPage+1<pages);fnTableEmpty->setVisible(visible.empty());
 for(int row=0;row<fnCapacity;++row){int index=fnPage*fnCapacity+row;bool has=index<(int)visible.size();for(int c=0;c<5;++c)fnRows[row][c]->setVisible(has);if(!has)continue;
  const Finance::Entry& e=entries[visible[index]];std::string date=fnText("day")+" "+fnNumber(Finance::Journal::day(e.hour)+1)+" "+fnNumber((int)e.hour%24)+"h";
  fnSet(fnRows[row][0],date,registerIvory);fnSet(fnRows[row][1],Loc::text(e.description.c_str()),registerIvory);fnSet(fnRows[row][2],fnText(fnCategories[e.category]),fnGrey);fnMoney(fnRows[row][3],fnCats(e.amount),fnColour(e.amount));fnMoney(fnRows[row][4],e.balance<0?fnText("unavailable"):fnCats(e.balance,false),registerIvory);
 }
 std::vector<Finance::DisplayRow> recent=Finance::recent(j,currentGameHours,fnDays);fnRecentEmpty->setVisible(recent.empty());
 for(int row=0;row<6;++row){bool has=recent.size()>(size_t)row;for(int c=0;c<4;++c)fnRecent[row][c]->setVisible(has);if(!has)continue;const Finance::Entry& e=recent[row].entry;
  std::string description=recent[row].movements>1?fnText("grouped")+" ("+fnNumber(recent[row].movements)+" "+fnText("movements")+")":Loc::text(e.description.c_str());
  fnSet(fnRecent[row][0],fnText("day")+" "+fnNumber(Finance::Journal::day(e.hour)+1),fnGrey);fnSet(fnRecent[row][1],description,registerIvory);fnSet(fnRecent[row][2],fnText(fnCategories[e.category]),fnGrey);fnMoney(fnRecent[row][3],fnCats(e.amount),fnColour(e.amount));
 }
}
void fnRenderChart(const Finance::Totals& total){
 Finance::Summary data=Finance::summary(fiscalLedger.cash,currentGameHours,fnDays);const long long* income=data.income;const long long* expense=data.expense;const long long* net=data.net;long long high=1,low=0;
 for(int n=0;n<fnDays;++n){high=std::max(high,std::max(income[n],net[n]));low=std::min(low,std::min(-expense[n],net[n]));}
 bool has=total.income!=0||total.expense!=0;fnChartEmpty->setVisible(!has);
 fnSet(fnChartMax,fnNumber(high),fnGrey);fnSet(fnChartMin,fnNumber(low),fnGrey);
 double range=(double)high-low;int zero=(int)(high/range*(fnPlotH-6))+3;
 for(int i=0;i<90;++i)for(int c=0;c<2;++c){MyGUI::Widget* b=fnBars[c][i];bool active=has&&i<fnDays&&(c?expense[i]:income[i])>0;b->setVisible(active);if(!active)continue;int x=i*fnPlotW/fnDays+1,bw=std::max(1,fnPlotW/fnDays/3),height=std::max(1,(int)((c?expense[i]:income[i])/range*(fnPlotH-6)));b->setCoord(x+c*bw,c?zero:zero-height,bw,height);}
 for(int i=0;i<360;++i){fnLine[i]->setVisible(has);if(!has)continue;double point=i*(fnDays-1)/359.0;int a=(int)point,b=std::min(fnDays-1,a+1);double v=net[a]+(net[b]-net[a])*(point-a);fnLine[i]->setCoord(i*(fnPlotW-3)/359,(int)((high-v)/range*(fnPlotH-6))+2,3,3);}
 // A fixed ring is recoloured only on data changes; no per-frame widget churn.
 const MyGUI::Colour colours[]={MyGUI::Colour(.85f,.30f,.17f),MyGUI::Colour(.94f,.58f,.16f),MyGUI::Colour(.70f,.34f,.28f),MyGUI::Colour(.78f,.39f,.52f),MyGUI::Colour(.30f,.62f,.55f),MyGUI::Colour(.65f,.43f,.33f),MyGUI::Colour(.80f,.27f,.37f),MyGUI::Colour(.44f,.56f,.65f),MyGUI::Colour(.94f,.69f,.23f),MyGUI::Colour(.75f,.75f,.67f),MyGUI::Colour(.43f,.47f,.49f),MyGUI::Colour(.35f,.72f,.61f)};
 for(int i=0;i<144;++i){int c=0;double target=total.expense*(i+.5)/144.0;long long sum=total.out[0];while(c<Finance::Count-1&&sum<=target)sum+=total.out[++c];fnRing[i]->setColour(total.expense?colours[c]:MyGUI::Colour(.20f,.25f,.25f));}
 fnSet(fnDonutTotal,fnText("total")+"\n"+fnNumber(total.expense),registerIvory);
}
void refreshFinanceView(){
 if(!guildFinancesPanel||!guildFinancesPanel->getVisible()||!fnDashboard)return;
 estateRefreshView();
 const Finance::Journal& j=fiscalLedger.cash;int balance=financeBalance();long day=Finance::Journal::day(currentGameHours);std::string language=Loc::engine().language;
 std::ostringstream fiscalKey;for(int i=0;i<2;++i){const FiscalOrganisationState& f=fiscalLedger.organisations[i];fiscalKey<<f.debt<<'/'<<f.paid<<'/'<<f.state<<'/'<<f.nextCollectionHour<<'/'<<f.deadlineHour<<'/';}
 fiscalKey<<estateState.revision<<'/'<<(long long)(currentGameHours*60)<<'/';
 if(!fnDirty&&fnFiscalCache==fiscalKey.str()&&fnRevision==j.revision&&fnBalanceCache==balance&&fnDayCache==day&&fnLanguage==language)return;
 fnFiscalCache=fiscalKey.str();fnDirty=false;fnRevision=j.revision;fnBalanceCache=balance;fnDayCache=day;fnLanguage=language;
 Finance::Totals t=Finance::summary(j,currentGameHours,fnDays).totals;
 fnMoney(fnCards[0],balance>=0?fnCats(balance,false):fnText("unavailable"),registerIvory);
 fnMoney(fnCards[1],j.available?fnCats(t.income):fnText("unavailable"),registerGreen);fnMoney(fnCards[2],j.available?fnCats(-t.expense):fnText("unavailable"),fnRed);fnMoney(fnCards[3],j.available?fnCats(t.income-t.expense):fnText("unavailable"),fnColour(t.income-t.expense));fnMoney(fnCards[4],!estateEnabled()?fnCats(0):estateState.available?fnCats(-estateState.forecast(currentGameHours)):fnText("unavailable"),fnGrey);

 int activeCount=0;for(int c=0;c<Finance::Count;++c)if(c==Finance::Contract||c==Finance::Tax||c==Finance::Investment||c==Finance::Other||t.in[c]||t.out[c])++activeCount;int summaryStep=std::min(32,(fnSummary[0]->getParent()->getHeight()-42)/std::max(1,activeCount));
 int shown=0;for(int c=0;c<Finance::Count;++c){bool active=c==Finance::Contract||c==Finance::Tax||c==Finance::Investment||c==Finance::Other||t.in[c]||t.out[c];fnSummaryNames[c]->setVisible(active);fnSummary[c]->setVisible(active);if(active){fnSummaryNames[c]->setSize(fnSummaryNames[c]->getWidth(),summaryStep);fnSummary[c]->setSize(fnSummary[c]->getWidth(),summaryStep);fnSummaryNames[c]->setPosition(fnSummaryNames[c]->getLeft(),38+shown*summaryStep);fnSummary[c]->setPosition(fnSummary[c]->getLeft(),38+shown*summaryStep);++shown;}fnMoney(fnSummary[c],fnCats(t.in[c]-t.out[c]),fnColour(t.in[c]-t.out[c]));std::ostringstream percent;percent<<std::fixed<<std::setprecision(1)<<(t.expense?100.0*t.out[c]/t.expense:0)<<" %";fnSet(fnExpenses[c],percent.str(),t.out[c]?registerIvory:fnGrey);}
 std::string coverage=fnText(j.available?"coverage":"invalid");if(j.started)coverage+=" "+fnText("day")+" "+fnNumber(Finance::Journal::day(j.startedHour)+1);if(j.truncated)coverage+=" | "+fnText("archive_note");
 fnSet(fnCoverage,coverage,fnGrey);fnRenderRows();fnRenderChart(t);
 long long debt=0;for(int i=0;i<2;++i){const FiscalOrganisationState& state=fiscalLedger.organisations[i];debt+=std::max(0LL,state.debt);
 fnSet(fnDebtValues[i][0],state.debt?fnCats(state.debt,false):fnText("no_debt"),state.debt?registerAmber:fnGrey);fnSet(fnDebtValues[i][1],fnCats(state.paid,false),registerIvory);fnSet(fnDebtValues[i][2],fiscalStateName(state.state),registerIvory);
 double due=state.deadlineHour>0?state.deadlineHour:state.nextCollectionHour;fnSet(fnDebtValues[i][3],state.debt>0&&due>0?fnText("day")+" "+fnNumber(Finance::Journal::day(due)+1)+" / "+fnNumber((int)due%24)+"h":fnText("unavailable"),fnGrey);
 }fnMoney(fnDebt,debt?fnCats(debt,false):fnText("no_debt"),debt?registerAmber:fnGrey);

}
#include "RealEstateView.h"
void buildFinanceView(MyGUI::Widget* c){
 const GuildOverview::Layout& l=overviewLayout;fnFont=std::max(12,std::min(16,l.font-2));fnDays=30;fnFilter=0;fnPage=0;fnDirty=true;
 int x=l.glance.x,y=l.glance.y,w=l.width-x-l.pad,h=l.height-y-l.pad,g=8,canvasH=std::max(870,h-18),cw=w-20;
 guildFinancesPanel=overviewEmptyPanel(c,GuildResponsive::Rect(x,y,w,h));
 fnDashboard=overviewEmptyPanel(guildFinancesPanel,GuildResponsive::Rect(0,0,w,h));
 fnScroll=fnDashboard->createWidget<MyGUI::ScrollView>("Kenshi_ScrollViewEmpty",0,0,w,h,MyGUI::Align::Default);MercenarieNativeInput::bind(fnScroll);fnScroll->setVisibleHScroll(false);fnScroll->setVisibleVScroll(true);fnScroll->setCanvasAlign(MyGUI::Align::Left|MyGUI::Align::Top);fnScroll->setCanvasSize(cw,canvasH);
 MyGUI::Widget* head=overviewPanel(fnScroll,GuildResponsive::Rect(0,0,cw,58));MyGUI::TextBox* title=fnLabel(head,10,2,190,29,fnText("title"));title->setFontHeight(24);fnLabel(head,10,32,std::max(100,cw-490),24,fnText("subtitle"),registerBlue);
 fnPeriod=head->createWidget<MyGUI::ComboBox>("MercenarieContractSort",cw-226,12,216,34,MyGUI::Align::Default);fnPeriod->setComboModeDrop(true);fnPeriod->setFontHeight(fnFont);fnPeriod->setTextColour(registerIvory);const char* periodKeys[]={"period7","period30","period90"};for(int i=0;i<3;++i)fnPeriod->addItem(fnText(periodKeys[i]));fnPeriod->setIndexSelected(1);fnPeriod->eventComboChangePosition+=MyGUI::newDelegate(fnPeriodChange);

 int cardsY=66,cardH=78,cardW=(cw-4*g)/5;const char* cardKeys[]={"funds","income","expenses","net","forecast"};
 for(int i=0;i<5;++i){MyGUI::Widget* p=overviewPanel(fnScroll,GuildResponsive::Rect(i*(cardW+g),cardsY,cardW,cardH));int icon=std::min(38,cardW/4);fnIcon(p,i,8,23,icon,i==1?registerGreen:i==2?fnRed:registerAmber);fnLabel(p,icon+15,6,cardW-icon-21,30,fnText(cardKeys[i]));fnCards[i]=fnLabel(p,icon+15,38,cardW-icon-21,30,"");}
 int row1=152,row1H=std::max(Finance::Count*(fnFont+2)+40,(canvasH-240)*40/100),row2=row1+row1H+g,row2H=std::max(360,canvasH-row2-90),row3=row2+row2H+g,row3H=canvasH-row3;
 int left=(cw-g)*60/100,middle=cw-left-g,mx=left+g;
 MyGUI::Widget* chart=fnPanel(fnScroll,0,row1,left,row1H,fnText("chart"));
 fnLabel(chart,8,32,left-16,20,fnText("legend"),registerAmber);
 fnPlotW=left-65;fnPlotH=row1H-85;fnPlot=overviewEmptyPanel(chart,GuildResponsive::Rect(52,57,fnPlotW,fnPlotH));
 for(int i=0;i<5;++i)registerSolid(fnPlot,0,i*(fnPlotH-1)/4,fnPlotW,1,MyGUI::Colour(.16f,.20f,.20f));for(int i=0;i<7;++i)registerSolid(fnPlot,i*(fnPlotW-1)/6,0,1,fnPlotH,MyGUI::Colour(.16f,.20f,.20f));
 fnChartMax=fnLabel(chart,3,52,47,18,"");fnChartMin=fnLabel(chart,3,row1H-40,47,18,"");
 for(int i=0;i<90;++i)for(int s=0;s<2;++s)fnBars[s][i]=registerSolid(fnPlot,0,0,1,1,s?fnRed:registerGreen);for(int i=0;i<360;++i)fnLine[i]=registerSolid(fnPlot,0,0,3,3,registerAmber);
 fnChartEmpty=fnLabel(fnPlot,3,fnPlotH/2-20,fnPlotW-6,40,fnText("empty"),fnGrey);fnLabel(chart,50,row1H-24,left-58,20,fnText("chart_axis"),fnGrey);
 MyGUI::Widget* pie=fnPanel(fnScroll,mx,row1,middle,row1H,fnText("breakdown"));int diameter=std::max(55,std::min(middle*38/100,row1H-55)),radius=diameter/2-5,centreX=diameter/2+6,centreY=35+(row1H-40)/2;
 for(int i=0;i<144;++i){double angle=i*6.28318530718/144.0-1.57079632679;fnRing[i]=registerSolid(pie,centreX+(int)(radius*std::cos(angle))-3,centreY+(int)(radius*std::sin(angle))-3,7,7,fnGrey);}
 fnDonutTotal=fnLabel(pie,8,centreY-25,diameter-4,50,"");fnDonutTotal->setTextAlign(MyGUI::Align::Center);
 int legendX=diameter+13,rh=(row1H-39)/Finance::Count;for(int i=0;i<Finance::Count;++i){fnLabel(pie,legendX,35+i*rh,middle-legendX-74,rh,fnText(fnCategories[i]),fnGrey);fnExpenses[i]=fnLabel(pie,middle-72,35+i*rh,67,rh,"");}
 MyGUI::Widget* recent=fnPanel(fnScroll,0,row2,left,row2H,fnText("recent"));const char* headers[]={"date","description","category","amount","balance"};int col[]={2,14,58,76,100};
 for(int i=0;i<4;++i){int xx=left*col[i]/100,ww=left*(col[i+1]-col[i])/100-5;fnLabel(recent,xx,33,ww,20,fnText(headers[i]),fnGrey);for(int j=0;j<6;++j)fnRecent[j][i]=fnLabel(recent,xx,58+j*38,ww,37,"");}
 fnRecentEmpty=fnLabel(recent,10,70,left-20,45,fnText("period_empty"),fnGrey);
 fnOpen=fnButton(recent,10,row2H-37,left-20,29,fnText("all"));
 MyGUI::Widget* summary=fnPanel(fnScroll,mx,row2,middle,row2H-90,fnText("summary"));rh=(row2H-39)/Finance::Count;
 for(int i=0;i<Finance::Count;++i){fnSummaryNames[i]=fnLabel(summary,10,38,middle*50/100-12,30,fnText(fnCategories[i]));fnSummary[i]=fnLabel(summary,middle*50/100,38,middle*50/100-8,30,"");}
 MyGUI::Widget* debtPanel=fnPanel(fnScroll,mx,row2+row2H-82,middle,82,fnText("outstanding"));fnDebt=fnLabel(debtPanel,10,34,middle/2-15,28,"");fnTaxOpen=fnButton(debtPanel,middle/2,34,middle/2-10,28,fnText("tax_details"));
 const char* future[]={"recurring","salary","goals"};int futureW=(cw-2*g)/3;for(int i=0;i<3;++i){MyGUI::Widget* block=overviewPanel(fnScroll,GuildResponsive::Rect(i*(futureW+g),row3,futureW,row3H));fnLabel(block,10,4,futureW-20,26,fnText(future[i]),fnGrey);if(i==1){MyGUI::Button* payrollOpen=overviewButton(block,8,32,futureW-16,std::max(26,row3H-38),Loc::text("payroll.page"));payrollOpen->eventMouseButtonClick+=MyGUI::newDelegate(openPayrollFinances);}else fnLabel(block,10,32,futureW-20,24,fnText("future"),registerBlue);}
 // Full history has a fixed reusable row pool, independent of journal length.
 fnHistory=fnPanel(guildFinancesPanel,0,0,w,h,fnText("history"));fnBack=fnButton(fnHistory,w-160,4,150,25,fnText("back"));
 fnFilterBox=fnHistory->createWidget<MyGUI::ComboBox>("MercenarieContractSort",10,42,250,32,MyGUI::Align::Default);fnFilterBox->setComboModeDrop(true);fnFilterBox->setFontHeight(fnFont);fnFilterBox->setTextColour(registerIvory);fnFilterBox->addItem(fnText("filter_all"));fnFilterBox->addItem(fnText("income"));fnFilterBox->addItem(fnText("expenses"));fnFilterMap.clear();fnFilterMap.push_back(0);fnFilterMap.push_back(1);fnFilterMap.push_back(2);for(int i=0;i<Finance::Count;++i){bool active=i==Finance::Contract||i==Finance::Tax||i==Finance::Investment||i==Finance::Other;for(size_t k=0;k<fiscalLedger.cash.entries.size();++k)if(fiscalLedger.cash.entries[k].category==i)active=true;for(size_t d=0;d<fiscalLedger.cash.days.size();++d)if(fiscalLedger.cash.days[d].in[i]||fiscalLedger.cash.days[d].out[i])active=true;if(active){fnFilterBox->addItem(fnText(fnCategories[i]));fnFilterMap.push_back(i+3);}}fnFilterBox->setIndexSelected(0);fnFilterBox->eventComboChangePosition+=MyGUI::newDelegate(fnFilterChange);
 fnJournalPeriod=fnHistory->createWidget<MyGUI::ComboBox>("MercenarieContractSort",275,42,210,32,MyGUI::Align::Default);fnJournalPeriod->setComboModeDrop(true);fnJournalPeriod->setFontHeight(fnFont);fnJournalPeriod->setTextColour(registerIvory);fnJournalPeriod->addItem(fnText("period7"));fnJournalPeriod->addItem(fnText("period30"));fnJournalPeriod->addItem(fnText("period_all"));fnJournalPeriod->setIndexSelected(2);fnJournalDays=0;fnJournalPeriod->eventComboChangePosition+=MyGUI::newDelegate(fnJournalPeriodChange);
 fnCoverage=fnLabel(fnHistory,10,82,w-20,28,"");int hc[]={2,14,41,57,78,99};fnCapacity=std::min(16,std::max(1,(h-190)/30));
 for(int c2=0;c2<5;++c2){int xx=w*hc[c2]/100,ww=w*(hc[c2+1]-hc[c2])/100-6;fnLabel(fnHistory,xx,114,ww,24,fnText(headers[c2]),fnGrey);for(int row=0;row<fnCapacity;++row)fnRows[row][c2]=fnLabel(fnHistory,xx,144+row*30,ww,28,"");}
 fnTableEmpty=fnLabel(fnHistory,20,160,w-40,40,fnText("period_empty"),fnGrey);fnPrevious=fnButton(fnHistory,10,h-39,110,29,fnText("previous"));fnNext=fnButton(fnHistory,w-120,h-39,110,29,fnText("next"));fnPageText=fnLabel(fnHistory,130,h-39,w-260,29,"");
 fnTax=fnPanel(guildFinancesPanel,0,0,w,h,fnText("tax_details"));fnTaxBack=fnButton(fnTax,w-160,4,150,25,fnText("back"));const char* debtKeys[]={"debt","paid","status","due"};for(int i=0;i<2;++i){MyGUI::Widget* card=fnPanel(fnTax,10+i*(w-10)/2,48,(w-30)/2,240,fiscalOrgName((FiscalOrganisation)i));for(int k=0;k<4;++k){fnLabel(card,12,42+k*45,card->getWidth()-24,20,fnText(debtKeys[k]),fnGrey);fnDebtValues[i][k]=fnLabel(card,12,63+k*45,card->getWidth()-24,23,"");}}fnWrap(fnTax,15,308,w-30,90,fnText("tax_note"));
 estateBuildView(guildFinancesPanel);
 fnHistory->setVisible(false);fnTax->setVisible(false);guildFinancesPanel->setVisible(false);
}



