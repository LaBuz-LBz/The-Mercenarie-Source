#pragma once
// Keep indoor floor selection under the existing height-aware arrival rules.
bool missionRoadUsesHorizontalArrival(){
    return escort&&escort->getMovement()&&!escort->getMovement()->isIndoors();
}

bool missionRoadStillMoving(){
    return escort&&escort->getMovement()&&escort->getMovement()->isCurrentlyMoving()&&!escort->getMovement()->pathFailed();
}

// A completed MOVE may leave the leader idle just before a road sample.
// Accept only healthy travel or fully idle native tasks, never foreign work.
bool missionRoadCanHandoff(){
    return escort&&escort->getMovement()&&!escort->getMovement()->pathFailed()
        &&(missionRoadStillMoving()||missionRoadTravelMissing(escort));
}
