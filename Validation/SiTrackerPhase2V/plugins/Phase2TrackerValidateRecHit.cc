// Package:    Phase2TrackerValidateRecHit
// Class:      Phase2TrackerValidateRecHit
//
/**\class Phase2TrackerValidateRecHit Phase2TrackerValidateRecHit.cc 

 Description:  Plugin for Phase2 RecHit validation

*/
//
// Author: Shubhi Parolia, Suvankar Roy Chowdhury
// Date: June 2020
//
// system include files
#include <memory>
#include "Validation/SiTrackerPhase2V/plugins/Phase2TrackerValidateRecHit.h"

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESWatcher.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonDetUnit/interface/TrackerGeomDet.h"
#include "Geometry/CommonDetUnit/interface/PixelGeomDetUnit.h"
#include "Geometry/CommonDetUnit/interface/PixelGeomDetType.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/SiPixelDigi/interface/PixelDigi.h"
#include "DataFormats/SiPixelDigi/interface/PixelDigiCollection.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
#include "SimDataFormats/TrackerDigiSimLink/interface/PixelDigiSimLink.h"
#include "SimTracker/SiPhase2Digitizer/plugins/Phase2TrackerDigitizerFwd.h"

// DQM Histograming
#include "DQMServices/Core/interface/MonitorElement.h"

//
// constructors
//
Phase2TrackerValidateRecHit::Phase2TrackerValidateRecHit(const edm::ParameterSet& iConfig)
    : config_(iConfig),
      tokenRecHits_(consumes<Phase2TrackerRecHit1DCollectionNew>(config_.getParameter<edm::InputTag>("rechits"))),
      tokenClusters_(consumes<Phase2TrackerCluster1DCollectionNew>(config_.getParameter<edm::InputTag>("clusters"))),
      otDigiSimLinkToken_(consumes<edm::DetSetVector<PixelDigiSimLink> >(config_.getParameter<edm::InputTag>("OuterTrackerDigiSimLinkSource"))),
      itPixelDigiSimLinkToken_(consumes<edm::DetSetVector<PixelDigiSimLink> >(config_.getParameter<edm::InputTag>("InnerPixelDigiSimLinkSource"))),
      simTrackToken_(consumes<edm::SimTrackContainer>(config_.getParameter<edm::InputTag>("SimTrackSource"))),
      simVertexToken_(consumes<edm::SimVertexContainer>(config_.getParameter<edm::InputTag>("SimVertexSource")))
{      
  edm::LogInfo("Phase2TrackerValidateRecHit") << ">>> Construct Phase2TrackerValidateRecHit ";
  for (const auto& itag : config_.getParameter<std::vector<edm::InputTag> >("PSimHitSourceBarrel"))
    simHitTokensbarrel_.push_back(consumes<edm::PSimHitContainer>(itag));  
  for (const auto& itag : config_.getParameter<std::vector<edm::InputTag> >("PSimHitSourceEndcap"))
    simHitTokensecap_.push_back(consumes<edm::PSimHitContainer>(itag));  
}

//
// destructor
//
Phase2TrackerValidateRecHit::~Phase2TrackerValidateRecHit() {
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
  edm::LogInfo("Phase2TrackerValidateRecHit") << ">>> Destroy Phase2TrackerValidateRecHit ";
}
//
// -- DQM Begin Run
//
// -- Analyze
//
void Phase2TrackerValidateRecHit::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;

  // Get the RecHits
  edm::Handle<Phase2TrackerRecHit1DCollectionNew> rechits;
  iEvent.getByToken(tokenRecHits_, rechits);
  // Get the geometry
  edm::ESHandle<TrackerGeometry> geomHandle;
  iSetup.get<TrackerDigiGeometryRecord>().get(geomHandle);
  const TrackerGeometry* tkGeom = &(*geomHandle);

  edm::ESHandle<TrackerTopology> tTopoHandle;
  iSetup.get<TrackerTopologyRcd>().get(tTopoHandle);
  const TrackerTopology* tTopo = tTopoHandle.product();
 
    // Loop over modules
  for (Phase2TrackerRecHit1DCollectionNew::const_iterator DSViter = rechits->begin(); DSViter != rechits->end();
       ++DSViter) {
    // Get the detector unit's id
    unsigned int rawid(DSViter->detId());
    DetId detId(rawid);
    // Get the geomdet
    const GeomDetUnit* geomDunit(tkGeom->idToDetUnit(detId));
    if (!geomDunit)
      continue;
    if (geomDunit->type().isInnerTracker()) {
      fillITHistos(*DSViter, tTopo, tkGeom, geomDunit, detId);  
    } else  if (geomDunit->type().isOuterTracker()) {
      fillOTHistos(*DSViter, tTopo, tkGeom, geomDunit, detId);  
    } 
  }

}

void Phase2TrackerValidateRecHit::fillITHistos(const edmNew::DetSet<Phase2TrackerRecHit1D>& rechitvec, 
                                               const TrackerTopology* tTopo, 
                                               const TrackerGeometry* tkGeom, 
                                               const GeomDetUnit* geomDetunit,
                                               const DetId& detId) {
  //loop over rechits for a single detId
  for(edmNew::DetSet<Phase2TrackerRecHit1D>::const_iterator rechitIt = rechitvec.begin(); 
                                                                       rechitIt != rechitvec.end(); ++rechitIt) {
    LocalPoint localPos = rechitIt->localPosition();
    Global3DPoint globalPos = geomDetunit->surface().toGlobal(localPos);
    globalXY_Pixel_->Fill(globalPos.x(), globalPos.y());
    globalRZ_Pixel_->Fill(globalPos.z(), globalPos.perp());
  }


}
void Phase2TrackerValidateRecHit::fillOTHistos(const edmNew::DetSet<Phase2TrackerRecHit1D>& rechitvec, 
                                               const TrackerTopology* tTopo, 
                                               const TrackerGeometry* tkGeom, 
                                               const GeomDetUnit* geomDetunit,
                                               const DetId& detId) {
  // determine the detector we are in
  TrackerGeometry::ModuleType mType = tkGeom->getDetectorType(detId);
  
  //loop over rechits for a single detId
  for(edmNew::DetSet<Phase2TrackerRecHit1D>::const_iterator rechitIt = rechitvec.begin(); 
                                                                       rechitIt != rechitvec.end(); ++rechitIt) {
    LocalPoint localPos = rechitIt->localPosition();
    Global3DPoint globalPos = geomDetunit->surface().toGlobal(localPos);
    if (mType == TrackerGeometry::ModuleType::Ph2PSP) {
      globalXY_PSP_->Fill(globalPos.x(), globalPos.y());
      globalRZ_PSP_->Fill(globalPos.z(), globalPos.perp());
    } else if (mType == TrackerGeometry::ModuleType::Ph2PSS) {
      globalXY_PSS_->Fill(globalPos.x(), globalPos.y());
      globalRZ_PSS_->Fill(globalPos.z(), globalPos.perp());
    } else if (mType == TrackerGeometry::ModuleType::Ph2SS) {
      globalXY_SS_->Fill(globalPos.x(), globalPos.y());
      globalRZ_SS_->Fill(globalPos.z(), globalPos.perp());
    }   
  }

} 


//
// -- Book Histograms
//
void Phase2TrackerValidateRecHit::bookHistograms(DQMStore::IBooker& ibooker,
						 edm::Run const& iRun,
						 edm::EventSetup const& iSetup) {
  
  std::string top_folder = config_.getParameter<std::string>("TopFolderName");
  std::stringstream folder_name;
  
  ibooker.cd();
  folder_name << top_folder;
  ibooker.setCurrentFolder(folder_name.str());
  
  edm::LogInfo("Phase2TrackerValidateRecHit") << " Booking Histograms in : " << folder_name.str();
  std::stringstream HistoName;
  
  HistoName.str("");
  HistoName << "NumberRecHits";
  numberRecHits_ = ibooker.book1D(HistoName.str(), HistoName.str(), 50, 0, 2500);

  //Global histos for IT
  std::string subdir = folder_name.str() + "/Phase2IT";
  ibooker.setCurrentFolder(subdir);
  HistoName.str("");
  HistoName << "Global_Position_XY_Pixel";
  globalXY_Pixel_   = ibooker.book2D(HistoName.str(), HistoName.str(), 500, -120., 120., 500, -120., 120.);
  HistoName.str("");
  HistoName << "Global_Position_RZ_Pixel";
  globalRZ_Pixel_   = ibooker.book2D(HistoName.str(), HistoName.str(), 3000, -3000., 3000., 1250., 0., 1250);

  //Global histos for OT
  subdir = folder_name.str() + "/Phase2OT";
  ibooker.setCurrentFolder(subdir);
  HistoName.str("");
  HistoName << "Global_Position_XY_PSP";
  globalXY_PSP_   = ibooker.book2D(HistoName.str(), HistoName.str(), 500, -120., 120., 500, -120., 120.);
  HistoName.str("");
  HistoName << "Global_Position_RZ_PSP";
  globalRZ_PSP_   = ibooker.book2D(HistoName.str(), HistoName.str(), 3000, -3000., 3000., 1250., 0., 1250);
  HistoName.str("");
  HistoName << "Global_Position_XY_PSS";
  globalXY_PSS_   = ibooker.book2D(HistoName.str(), HistoName.str(), 500, -120., 120., 500, -120., 120.);
  HistoName.str("");
  HistoName << "Global_Position_RZ_PSS";
  globalRZ_PSS_   = ibooker.book2D(HistoName.str(), HistoName.str(), 3000, -3000., 3000., 1250., 0., 1250);
  HistoName.str("");
  HistoName << "Global_Position_XY_SS";
  globalXY_SS_   = ibooker.book2D(HistoName.str(), HistoName.str(), 500, -120., 120., 500, -120., 120.);
  HistoName.str("");
  HistoName << "Global_Position_RZ_SS";
  globalRZ_SS_   = ibooker.book2D(HistoName.str(), HistoName.str(), 3000, -3000., 3000., 1250., 0., 1250);
   
}

//
// -- Book Layer Histograms
//
void Phase2TrackerValidateRecHit::bookLayerHistos(DQMStore::IBooker& ibooker,
						  unsigned int det_id,
						  const TrackerTopology* tTopo,
						  bool flag) {
}

  
 
std::vector<unsigned int> Phase2TrackerValidateRecHit::getSimTrackId(
    const edm::Handle<edm::DetSetVector<PixelDigiSimLink> >& pixelSimLinks, const DetId& detId, unsigned int channel) {
  std::vector<unsigned int> retvec;
  edm::DetSetVector<PixelDigiSimLink>::const_iterator DSViter(pixelSimLinks->find(detId));
  if (DSViter == pixelSimLinks->end())
    return retvec;
  for (edm::DetSet<PixelDigiSimLink>::const_iterator it = DSViter->data.begin(); it != DSViter->data.end(); ++it) {
    if (channel == it->channel()) {
      retvec.push_back(it->SimTrackId());
    }
  }
  return retvec;
}
  //define this as a plug-in
DEFINE_FWK_MODULE(Phase2TrackerValidateRecHit);
 
