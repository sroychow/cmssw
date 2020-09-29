#ifndef _Phase2TrackierValidationUtil_h
#define _Phase2TrackierValidationUtil_h
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimDataFormats/Track/interface/SimTrack.h"
#include<string>
#include<sstream>

namespace Phase2TkUtil {

bool isPrimary(const SimTrack& simTrk, const PSimHit* simHit){
  bool retval = false;
  unsigned int trkId = simTrk.trackId();
  if (trkId != simHit->trackId())
    return retval;
  int vtxIndex = simTrk.vertIndex();
  int ptype = simHit->processType();
  if ((vtxIndex == 0) && (ptype == 0))
    return true;
  return false;
}

std::string getOTHistoId(uint32_t det_id, const TrackerTopology* tTopo) {
  std::string Disc;
  std::ostringstream fname1;
  int layer = tTopo->getOTLayerNumber(det_id);
  
  if(layer < 0)
    return "";
  if (layer < 100) {
    fname1 << "Barrel/";
    fname1 << "Layer" << layer;
    fname1 << "";
  } else {
    int side = layer / 100;
    fname1 << "EndCap_Side" << side << "/";
    int disc = layer - side * 100;
    Disc = (disc < 3) ? "TEDD_1" : "TEDD_2";
    fname1 << Disc << "/";
    int ring = tTopo->tidRing(det_id);
    fname1 << "Ring" << ring;
  }
  return fname1.str();
}
 
}
 
#endif
