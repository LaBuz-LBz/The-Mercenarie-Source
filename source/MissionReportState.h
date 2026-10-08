#pragma once
#include <string>
#include <stdexcept>
// Optional trailing extension keeps pre-report saves readable.
template<class Archive> void archiveReportTime(Archive& a,double& start,double& end){
    if(a.reading&&a.cursor==a.bytes.size()){start=end=-1;return;}
    std::string marker="MISSION-REPORT-1";a.field(marker);
    if(marker!="MISSION-REPORT-1")throw std::runtime_error("invalid mission report version");
    a.field(start);a.field(end);
    if(start!=start||end!=end||start< -1||end< -1||start>1e9||end>1e9)throw std::runtime_error("invalid mission report time");
}
