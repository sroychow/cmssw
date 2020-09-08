#ifndef Phase2TrackerValidateRecHit_h
#define Phase2TrackerValidateRecHit_h
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "DataFormats/Phase2TrackerCluster/interface/Phase2TrackerCluster1D.h"
#include "DataFormats/TrackerRecHit2D/interface/Phase2TrackerRecHit1D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHitCollection.h"
#include "DataFormats/GeometrySurface/interface/LocalError.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "SimDataFormats/TrackerDigiSimLink/interface/PixelDigiSimLink.h"
#include "SimTracker/TrackerHitAssociation/interface/TrackerHitAssociator.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonDetUnit/interface/PixelGeomDetUnit.h"
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/DQMStore.h"


// DQM Histograming
class PixelDigiSimLink;
class SimTrack;
class SimHit;
class TrackerTopology;
class PixelDigi;
class Phase2TrackerDigi;
class TrackerGeometry;

class Phase2TrackerValidateRecHit : public DQMEDAnalyzer {
public:
  explicit Phase2TrackerValidateRecHit(const edm::ParameterSet&);
  ~Phase2TrackerValidateRecHit() override;
  void bookHistograms(DQMStore::IBooker& ibooker, edm::Run const& iRun, edm::EventSetup const& iSetup) override;
  void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) override;
  
 private:
  void fillITHistos(const edm::Event& iEvent,
		    const TrackerTopology* tTopo,
		    const TrackerGeometry* tkGeom,
		    const TrackerHitAssociator& associateRecHit,
		    const std::vector<edm::Handle<edm::PSimHitContainer>>& simHits);

      
  void fillOTHistos(const edm::Event& iEvent, 
		    const TrackerTopology* tTopo, 
		    const TrackerGeometry* tkGeom, 
		    const TrackerHitAssociator& associateRecHit,
		    const std::vector<edm::Handle<edm::PSimHitContainer>>& simHits);


  void bookLayerHistos(DQMStore::IBooker& ibooker, unsigned int det_id, const TrackerTopology* tTopo, std::string& subdir);
  std::vector<unsigned int> getSimTrackId(const edm::Handle<edm::DetSetVector<PixelDigiSimLink> >& pixelSimLinks, const DetId& detId, unsigned int channel);
  std::string getHistoId(uint32_t det_id, const TrackerTopology* tTopo);

  edm::ParameterSet config_;
  bool pixelFlag_;
  TrackerHitAssociator::Config trackerHitAssociatorConfig_;
  std::string geomType_;
  edm::EDGetTokenT<Phase2TrackerRecHit1DCollectionNew> tokenRecHitsOT_;
  edm::EDGetTokenT<SiPixelRecHitCollection> tokenRecHitsIT_;
  //edm::EDGetTokenT<Phase2TrackerCluster1DCollectionNew> tokenClusters_;
  //const edm::EDGetTokenT<edm::DetSetVector<PixelDigiSimLink> > otDigiSimLinkToken_;
  //const edm::EDGetTokenT<edm::DetSetVector<PixelDigiSimLink> > itPixelDigiSimLinkToken_;
  //const edm::EDGetTokenT<edm::SimTrackContainer> simTrackToken_;
  //const edm::EDGetTokenT<edm::SimVertexContainer> simVertexToken_;
  std::vector<edm::EDGetTokenT<edm::PSimHitContainer> > simHitTokens_;

  MonitorElement* numberRecHits_;
  MonitorElement* globalXY_Pixel_;
  MonitorElement* globalRZ_Pixel_;
  MonitorElement* globalXY_PSP_;
  MonitorElement* globalRZ_PSP_;
  MonitorElement* globalXY_PSS_;
  MonitorElement* globalRZ_PSS_;
  MonitorElement* globalXY_SS_;
  MonitorElement* globalRZ_SS_;
  struct RecHitME {
    // use TH1D instead of TH1F to avoid stauration at 2^31
    // above this increments with +1 don't work for float, need double
    
    MonitorElement* numberRecHits_P = nullptr;
    MonitorElement* numberRecHits_S = nullptr; 
    MonitorElement* clusterSize_P = nullptr;
    MonitorElement* clusterSize_S = nullptr;
    
    MonitorElement* globalPosXY_P = nullptr;
    MonitorElement* globalPosXY_S = nullptr;
    MonitorElement* localPosXY_P = nullptr;
    MonitorElement* localPosXY_S = nullptr;
      
    MonitorElement* deltaX_P = nullptr;
    MonitorElement* deltaX_S = nullptr;
    MonitorElement* deltaY_P = nullptr;
    MonitorElement* deltaY_S = nullptr;
    MonitorElement* pullX_P = nullptr;
    MonitorElement* pullX_S = nullptr;
    MonitorElement* pullY_P = nullptr;
    MonitorElement* pullY_S = nullptr;
    MonitorElement* deltaX_eta_P = nullptr;
    MonitorElement* deltaX_eta_S = nullptr;
    MonitorElement* deltaY_eta_P = nullptr;
    MonitorElement* deltaY_eta_S = nullptr;    
    MonitorElement* pullX_eta_P = nullptr;
    MonitorElement* pullX_eta_S = nullptr;
    MonitorElement* pullY_eta_P = nullptr;
    MonitorElement* pullY_eta_S = nullptr;
    /*
      MonitorElement* pullX_primary;
      MonitorElement* pullY_primary;
    //MonitorElement* deltaX_primary_P;
    //MonitorElement* deltaX_primary_S;
    //MonitorElement* deltaY_primary_P;
    //MonitorElement* deltaY_primary_S;
      MonitorElement* pullX_eta;
      MonitorElement* pullY_eta;
      MonitorElement* primarySimHits;
      MonitorElement* otherSimHits	;
    */
  };
  std::map<std::string, RecHitME>  layerMEs_;
};
#endif
