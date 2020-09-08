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
//--- for SimHit association
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimDataFormats/TrackerDigiSimLink/interface/PixelDigiSimLink.h"
#include "SimTracker/SiPhase2Digitizer/plugins/Phase2TrackerDigitizerFwd.h"

// DQM Histograming
#include "DQMServices/Core/interface/MonitorElement.h"

//
// constructors
//
Phase2TrackerValidateRecHit::Phase2TrackerValidateRecHit(const edm::ParameterSet& iConfig)
  : config_(iConfig),
    pixelFlag_(config_.getParameter<bool>("ITPlotFillingFlag")),
    trackerHitAssociatorConfig_(iConfig, consumesCollector()),
    tokenRecHitsOT_(consumes<Phase2TrackerRecHit1DCollectionNew>(config_.getParameter<edm::InputTag>("rechitsOT"))),
  tokenRecHitsIT_(consumes<SiPixelRecHitCollection>(iConfig.getParameter<edm::InputTag>("rechitsIT"))) {      
  edm::LogInfo("Phase2TrackerValidateRecHit") << ">>> Construct Phase2TrackerValidateRecHit ";
  for (const auto& itag :  config_.getParameter<std::vector<edm::InputTag> >("PSimHitSource"))
    simHitTokens_.push_back(consumes<edm::PSimHitContainer>(itag));
  //tokenClusters_(consumes<Phase2TrackerCluster1DCollectionNew>(config_.getParameter<edm::InputTag>("clusters"))),
  //simTrackToken_(consumes<edm::SimTrackContainer>(config_.getParameter<edm::InputTag>("SimTrackSource"))),
  //simVertexToken_(consumes<edm::SimVertexContainer>(config_.getParameter<edm::InputTag>("SimVertexSource")))
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

  // Get the geometry
  edm::ESHandle<TrackerGeometry> geomHandle;
  iSetup.get<TrackerDigiGeometryRecord>().get(geomHandle);
  const TrackerGeometry* tkGeom = &(*geomHandle);

  edm::ESHandle<TrackerTopology> tTopoHandle;
  iSetup.get<TrackerTopologyRcd>().get(tTopoHandle);
  const TrackerTopology* tTopo = tTopoHandle.product();

  std::vector<edm::Handle<edm::PSimHitContainer>> simHits;
  for (const auto& itoken : simHitTokens_) {
    edm::Handle<edm::PSimHitContainer> simHitHandle;
    iEvent.getByToken(itoken, simHitHandle);
    if (!simHitHandle.isValid())
      continue;
    simHits.emplace_back(simHitHandle);
  }
  //std::cout << "SimHits Vec size=" << simHits.size() << std::endl;

  TrackerHitAssociator associateRecHit(iEvent, trackerHitAssociatorConfig_);

  if (pixelFlag_) 
    fillITHistos(iEvent, tTopo, tkGeom, associateRecHit, simHits);
  else 
    fillOTHistos(iEvent, tTopo, tkGeom, associateRecHit, simHits);


}

void Phase2TrackerValidateRecHit::fillITHistos(const edm::Event& iEvent, 
                                               const TrackerTopology* tTopo, 
                                               const TrackerGeometry* tkGeom,
					       const TrackerHitAssociator& associateRecHit,
                                               const std::vector<edm::Handle<edm::PSimHitContainer>>& simHits) {
  edm::Handle<SiPixelRecHitCollection> rechits;
  iEvent.getByToken(tokenRecHitsIT_, rechits);
  if (!rechits.isValid())
    return;

  std::map<std::string, unsigned int>  nrechitLayerMapP;

  SiPixelRecHitCollection::const_iterator DSViter;
  for ( DSViter = rechits->begin(); DSViter != rechits->end(); ++DSViter) {
    // Get the detector unit's id
    unsigned int rawid(DSViter->detId());
    DetId detId(rawid);
    // Get the geomdet
    const GeomDetUnit* geomDetunit(tkGeom->idToDetUnit(detId));
    if (!geomDetunit)
      continue;
    std::string key = getHistoId(detId.rawId(), tTopo);
    if(nrechitLayerMapP.find(key) == nrechitLayerMapP.end())
      nrechitLayerMapP.insert(std::make_pair(key, DSViter->size()));
    else nrechitLayerMapP[key] += DSViter->size();
    
    //loop over rechits for a single detId
    for(edmNew::DetSet<SiPixelRecHit>::const_iterator 
	  rechitIt = DSViter->begin(); rechitIt != DSViter->end(); ++rechitIt) {
      
      LocalPoint lp = rechitIt->localPosition();
      Global3DPoint globalPos = geomDetunit->surface().toGlobal(lp);
      //in mm
      double gx = globalPos.x()*10.;
      double gy = globalPos.y()*10.;
      double gz = globalPos.z()*10.;
      double gr = globalPos.perp()*10.;  
      
      globalXY_Pixel_->Fill(gx, gy);
      globalRZ_Pixel_->Fill(gz, gr);
      
      //layer wise histo
      layerMEs_[key].clusterSize_P->Fill(rechitIt->cluster()->size());
      layerMEs_[key].globalPosXY_P->Fill(gx, gy); 
      layerMEs_[key].localPosXY_P->Fill(lp.x(), lp.y()); 
      //GetSimHits
      const std::vector<SimHitIdpr>& matchedId = associateRecHit.associateHitId(*rechitIt);
      //std::cout << "Nmatched SimHits = "  << matchedId.size() << ",";
      const PSimHit* simhitClosest = 0;
      float minx = 10000;
      for (unsigned int si = 0; si < simHits.size(); ++si) {
	for(edm::PSimHitContainer::const_iterator simhitIt = simHits.at(si)->begin();
	    simhitIt != simHits.at(si)->end(); ++simhitIt) {
	  if(detId.rawId() != simhitIt->detUnitId())   continue;
	  for(auto& mId : matchedId) {
	    if(simhitIt->trackId() == mId.first) {
	      if (!simhitClosest || fabs(simhitIt->localPosition().x() - lp.x()) < minx) {
		minx = fabs(simhitIt->localPosition().x() - lp.x());
		simhitClosest = &*simhitIt;
	      }
	    }
	  }
	}//end loop over PSimhitcontainers
      }//end loop over simHits
      if(!simhitClosest)   continue;
      Local3DPoint simlp(simhitClosest->localPosition());
      const LocalError& lperr = rechitIt->localPositionError();
      double dx = lp.x() - simlp.x();
      double dy = lp.y() - simlp.y();
      double pullx = 999.;
      double pully = 999.;
      if (lperr.xx()) 
	pullx = (lp.x() - simlp.x()) / std::sqrt(lperr.xx());
      if (lperr.yy()) 
	pully = (lp.y() - simlp.y()) / std::sqrt(lperr.yy());
      float eta = globalPos.eta();
      layerMEs_[key].deltaX_P->Fill(dx);
      layerMEs_[key].deltaY_P->Fill(dy);
      layerMEs_[key].pullX_P->Fill(pullx);
      layerMEs_[key].pullY_P->Fill(pully);
      layerMEs_[key].deltaX_eta_P->Fill(eta,dx);
      layerMEs_[key].deltaY_eta_P->Fill(eta,dy);
      layerMEs_[key].pullX_eta_P->Fill(eta,pullx);
      layerMEs_[key].pullY_eta_P->Fill(eta,pully);
    }
  }//end loop over PixelRecHit DetSetVector
  //Fill layer counters per event
  for(auto& lme : nrechitLayerMapP) {
    layerMEs_[lme.first].numberRecHits_P->Fill(lme.second);
  }
}
void Phase2TrackerValidateRecHit::fillOTHistos(const edm::Event& iEvent, 
                                               const TrackerTopology* tTopo, 
                                               const TrackerGeometry* tkGeom,
					       const TrackerHitAssociator& associateRecHit,
					       const std::vector<edm::Handle<edm::PSimHitContainer>>& simHits) {

  // Get the RecHits
  edm::Handle<Phase2TrackerRecHit1DCollectionNew> rechits;
  iEvent.getByToken(tokenRecHitsOT_, rechits);
  if(!rechits.isValid())  return;

  std::map<std::string, unsigned int>  nrechitLayerMapP;
  std::map<std::string, unsigned int>  nrechitLayerMapS;
 
  // Loop over modules
  Phase2TrackerRecHit1DCollectionNew::const_iterator DSViter; 
  for (DSViter = rechits->begin(); DSViter != rechits->end(); ++DSViter) {
    // Get the detector unit's id
    unsigned int rawid(DSViter->detId());
    DetId detId(rawid);
    // Get the geomdet
    const GeomDetUnit* geomDetunit(tkGeom->idToDetUnit(detId));
    if (!geomDetunit)
      continue;
    // determine the detector we are in
    TrackerGeometry::ModuleType mType = tkGeom->getDetectorType(detId);
    std::string key = getHistoId(detId.rawId(), tTopo);
    
    if (mType == TrackerGeometry::ModuleType::Ph2PSP) {
      if(nrechitLayerMapP.find(key) == nrechitLayerMapP.end())
	nrechitLayerMapP.insert(std::make_pair(key, DSViter->size()));
      else nrechitLayerMapP[key] += DSViter->size();
    } else if (mType == TrackerGeometry::ModuleType::Ph2PSS || mType == TrackerGeometry::ModuleType::Ph2SS){
      if(nrechitLayerMapS.find(key) == nrechitLayerMapS.end())
	nrechitLayerMapS.insert(std::make_pair(key, DSViter->size()));
      else nrechitLayerMapS[key] += DSViter->size();
    }
    edmNew::DetSet<Phase2TrackerRecHit1D>::const_iterator rechitIt;
    //loop over rechits for a single detId
    for(rechitIt = DSViter->begin(); rechitIt != DSViter->end(); ++rechitIt) {
      LocalPoint lp = rechitIt->localPosition();
      Global3DPoint globalPos = geomDetunit->surface().toGlobal(lp);
      //in mm
      double gx = globalPos.x()*10.;
      double gy = globalPos.y()*10.;
      double gz = globalPos.z()*10.;
      double gr = globalPos.perp()*10.;
      //Fill positions
      if (mType == TrackerGeometry::ModuleType::Ph2PSP) {
	globalXY_PSP_->Fill(gx, gy);
	globalRZ_PSP_->Fill(gz, gr);
	//layer wise histo
	layerMEs_[key].clusterSize_P->Fill(rechitIt->cluster()->size());
	layerMEs_[key].globalPosXY_P->Fill(gx, gy); 
	layerMEs_[key].localPosXY_P->Fill(lp.x(), lp.y()); 
      } else if (mType == TrackerGeometry::ModuleType::Ph2PSS) {
	globalXY_PSS_->Fill(gx, gy);
	globalRZ_PSS_->Fill(gz, gr);
	//layer wise histo
	layerMEs_[key].clusterSize_S->Fill(rechitIt->cluster()->size());
	layerMEs_[key].globalPosXY_S->Fill(gx, gy); 
	layerMEs_[key].localPosXY_S->Fill(lp.x(), lp.y()); 
      } else if (mType == TrackerGeometry::ModuleType::Ph2SS) {
	globalXY_SS_->Fill(gx, gy);
	globalRZ_SS_->Fill(gz, gr);
	//layer wise histo
	layerMEs_[key].clusterSize_S->Fill(rechitIt->cluster()->size());
	layerMEs_[key].globalPosXY_S->Fill(gx, gy); 
	layerMEs_[key].localPosXY_S->Fill(lp.x(), lp.y()); 
      }   
      //GetSimHits
      const std::vector<SimHitIdpr>& matchedId = associateRecHit.associateHitId(*rechitIt);
      //std::cout << "Nmatched SimHits = "  << matchedId.size() << ",";
      const PSimHit* simhitClosest = 0;
      float minx = 10000;
      for (unsigned int si = 0; si < simHits.size(); ++si) {
	for(edm::PSimHitContainer::const_iterator simhitIt = simHits.at(si)->begin();
	    simhitIt != simHits.at(si)->end(); ++simhitIt) {
	  if(detId.rawId() != simhitIt->detUnitId())   continue;
	  for(auto& mId : matchedId) {
	    if(simhitIt->trackId() == mId.first) {
	      if (!simhitClosest || fabs(simhitIt->localPosition().x() - lp.x()) < minx) {
		minx = fabs(simhitIt->localPosition().x() - lp.x());
		simhitClosest = &*simhitIt;
	      }
	    }
	  }
	}//end loop over PSimhitcontainers
      }//end loop over simHits
      if(!simhitClosest)   continue;
      Local3DPoint simlp(simhitClosest->localPosition());
      const LocalError& lperr = rechitIt->localPositionError();
      double dx = lp.x() - simlp.x();
      double dy = lp.y() - simlp.y();
      double pullx = 999.;
      double pully = 999.;
      if (lperr.xx()) 
	pullx = (lp.x() - simlp.x()) / std::sqrt(lperr.xx());
      if (lperr.yy()) 
	pully = (lp.y() - simlp.y()) / std::sqrt(lperr.yy());
      float eta = globalPos.eta();
      if (mType == TrackerGeometry::ModuleType::Ph2PSP) {
	layerMEs_[key].deltaX_P->Fill(dx);
	layerMEs_[key].deltaY_P->Fill(dy);
	layerMEs_[key].pullX_P->Fill(pullx);
	layerMEs_[key].pullY_P->Fill(pully);
	layerMEs_[key].deltaX_eta_P->Fill(eta,dx);
	layerMEs_[key].deltaY_eta_P->Fill(eta,dy);
	layerMEs_[key].pullX_eta_P->Fill(eta,pullx);
	layerMEs_[key].pullY_eta_P->Fill(eta,pully);
      } else if (mType == TrackerGeometry::ModuleType::Ph2PSS || mType == TrackerGeometry::ModuleType::Ph2SS) {
	layerMEs_[key].deltaX_S->Fill(dx);
	layerMEs_[key].deltaY_S->Fill(dy);
	layerMEs_[key].pullX_S->Fill(pullx);
	layerMEs_[key].pullY_S->Fill(pully);
	layerMEs_[key].deltaX_eta_S->Fill(eta,dx);
	layerMEs_[key].deltaY_eta_S->Fill(eta,dy);
	layerMEs_[key].pullX_eta_S->Fill(eta,pullx);
	layerMEs_[key].pullY_eta_S->Fill(eta,pully);
      }
    }//end loop over rechits of a detId
  } //End loop over DetSetVector

  //fill nREcHit counter per layer
  for(auto& lme : nrechitLayerMapP) {
    layerMEs_[lme.first].numberRecHits_P->Fill(lme.second);
  }
  for(auto& lme : nrechitLayerMapS) {
    layerMEs_[lme.first].numberRecHits_S->Fill(lme.second);
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
  edm::LogInfo("Phase2TrackerValidateRecHit") << " Booking Histograms in : " << folder_name.str();
  std::stringstream HistoName;
  
  HistoName.str("");
  HistoName << "NumberRecHits";
  numberRecHits_ = ibooker.book1D(HistoName.str(), HistoName.str(), 50, 0, 2500);
  
  std::string subdir;
  //Global histos for IT
  if(pixelFlag_) {
    folder_name << "/Phase2IT";
    subdir = folder_name.str();
    ibooker.setCurrentFolder(subdir);
    HistoName.str("");
    HistoName << "Global_Position_XY_Pixel";
    globalXY_Pixel_   = ibooker.book2D(HistoName.str(), HistoName.str(), 520, -260., 260., 520, -260., 260.);
    HistoName.str("");
    HistoName << "Global_Position_RZ_Pixel";
    globalRZ_Pixel_   = ibooker.book2D(HistoName.str(), HistoName.str(), 3000, -3000., 3000., 260., 0., 260.);
  } else {
    //Global histos for OT
    folder_name << "/Phase2OT";
    subdir = folder_name.str();
    ibooker.setCurrentFolder(subdir);
    HistoName.str("");
    HistoName << "Global_Position_XY_PSP";
    globalXY_PSP_   = ibooker.book2D(HistoName.str(), HistoName.str(), 2500, -1250., 1250., 2500, -1250., 1250.);
    HistoName.str("");
    HistoName << "Global_Position_RZ_PSP";
    globalRZ_PSP_   = ibooker.book2D(HistoName.str(), HistoName.str(), 3000, -3000., 3000., 1250., 0., 1250);
    HistoName.str("");
    HistoName << "Global_Position_XY_PSS";
    globalXY_PSS_   = ibooker.book2D(HistoName.str(), HistoName.str(), 2500, -1250., 1250., 2500, -1250., 1250.);
    HistoName.str("");
    HistoName << "Global_Position_RZ_PSS";
    globalRZ_PSS_   = ibooker.book2D(HistoName.str(), HistoName.str(), 3000, -3000., 3000., 1250., 0., 1250);
    HistoName.str("");
    HistoName << "Global_Position_XY_SS";
    globalXY_SS_   = ibooker.book2D(HistoName.str(), HistoName.str(), 2500, -1250., 1250., 2500, -1250., 1250.);
    HistoName.str("");
    HistoName << "Global_Position_RZ_SS";
    globalRZ_SS_   = ibooker.book2D(HistoName.str(), HistoName.str(), 3000, -3000., 3000., 1250., 0., 1250);
  }
  //Now book layer wise histos

  edm::ESWatcher<TrackerDigiGeometryRecord> theTkDigiGeomWatcher;
  edm::ESHandle<TrackerTopology> tTopoHandle;
  iSetup.get<TrackerTopologyRcd>().get(tTopoHandle);
  const TrackerTopology* const tTopo = tTopoHandle.product();

  if (theTkDigiGeomWatcher.check(iSetup)) {
    edm::ESHandle<TrackerGeometry> geom_handle;
    iSetup.get<TrackerDigiGeometryRecord>().get(geomType_, geom_handle);
    const TrackerGeometry* tGeom = geom_handle.product();

    for (auto const& det_u : tGeom->detUnits()) {
      unsigned int detId_raw = det_u->geographicalId().rawId();
      bookLayerHistos(ibooker, detId_raw, tTopo, subdir);
     }
   }

 }

 //
 // -- Book Layer Histograms
 //
 void Phase2TrackerValidateRecHit::bookLayerHistos(DQMStore::IBooker& ibooker,
						   unsigned int det_id,
						   const TrackerTopology* tTopo,
						   std::string& subdir) {

   int layer, side;
   int idisc {0};
   std::string subFolder;
   if (pixelFlag_)
     layer = tTopo->getITPixelLayerNumber(det_id);
   else
     layer = tTopo->getOTLayerNumber(det_id);

   if (layer < 0)
     return;
   std::map<std::string, RecHitME>::iterator pos = layerMEs_.find(std::to_string(layer));
   if (pos == layerMEs_.end()) {
     std::string key = getHistoId(det_id, tTopo);
     if (layer > 100) {
       side = layer / 100;
       idisc = layer - side * 100;
       idisc = (idisc < 3) ? 12 : 345;
    }

    bool forDisc12UptoRing10 = (idisc == 12 && tTopo->tidRing(det_id) <= 10) ? true : false;
    bool forDisc345UptoRing7 = (idisc == 345 && tTopo->tidRing(det_id) <= 7) ? true : false;
    bool forS = (pixelFlag_) ? false : true;
    bool forP = (pixelFlag_ || (layer < 4 || (layer > 6 && (forDisc12UptoRing10 || forDisc345UptoRing7)))) ? true : false;

    ibooker.cd();
    ibooker.setCurrentFolder(subdir+"/"+key);
    edm::LogInfo("Phase2TrackerValidateRecHit") << " Booking Histograms in : " << key;

    RecHitME local_histos;
    std::ostringstream histoName;
    
    if(forP) {
      histoName.str("");
      histoName << "Number_RecHits_Pixel";
      local_histos.numberRecHits_P = ibooker.book1D(histoName.str(), histoName.str(), 50, 0., 0.);

      histoName.str("");
      histoName << "Cluster_Size_Pixel";
      local_histos.clusterSize_P = ibooker.book1D(histoName.str(), histoName.str(), 21, -0.5, 20.5);

      histoName.str("");
      histoName << "Global_Position_XY_Pixel"; 
      local_histos.globalPosXY_P = ibooker.book2D(histoName.str(), histoName.str(), 250, -1250., 1250., 250, -1250., 1250.);

      histoName.str("");
      histoName << "Local_Position_XY_Pixel" ;
      local_histos.localPosXY_P = ibooker.book2D(histoName.str(), histoName.str(), 500, 0., 0., 500, 0., 0.);

      histoName.str("");
      histoName << "Delta_X_Pixel";
      local_histos.deltaX_P = ibooker.book1D(histoName.str(), histoName.str(), 100, -0.02, 0.02);

      histoName.str("");
      histoName << "Delta_Y_Pixel";
      local_histos.deltaY_P = ibooker.book1D(histoName.str(), histoName.str(), 100, -0.02, 0.02);

      histoName.str("");
      histoName << "Pull_X_Pixel";
      local_histos.pullX_P = ibooker.book1D(histoName.str(), histoName.str(), 100, -3., 3.);

      histoName.str("");
      histoName << "Pull_Y_Pixel";
      local_histos.pullY_P = ibooker.book1D(histoName.str(), histoName.str(), 100, -3., 3.);
      
      histoName.str("");
      histoName << "Delta_X_vs_Eta_P";
      local_histos.deltaX_eta_P = ibooker.bookProfile(histoName.str(), histoName.str(), 50, -2.5, 2.5, 100, -0.02, 0.02);

      histoName.str("");
      histoName << "Delta_Y_vs_Eta_P";
      local_histos.deltaY_eta_P = ibooker.bookProfile(histoName.str(), histoName.str(), 50, -2.5, 2.5, 100, -0.02, 0.02);

      histoName.str("");
      histoName << "Pull_X_vs_Eta_P";
      local_histos.pullX_eta_P = ibooker.bookProfile(histoName.str(), histoName.str(), 50, -2.5, 2.5, 100, -3., 3.);

      histoName.str("");
      histoName << "Pull_Y_vs_Eta_P";
      local_histos.pullY_eta_P = ibooker.bookProfile(histoName.str(), histoName.str(), 50, -2.5, 2.5, 100, -3., 3.);
      
    }
    if(forS) {
      histoName.str("");
      histoName << "Number_RecHits_Strip";
      local_histos.numberRecHits_S = ibooker.book1D(histoName.str().c_str(), histoName.str().c_str(), 50, 0., 0.);

      histoName.str("");
      histoName << "Cluster_Size_Strip";
      local_histos.clusterSize_S = ibooker.book1D(histoName.str().c_str(), histoName.str().c_str(), 21, -0.5, 20.5);

      histoName.str("");
      histoName << "Global_Position_XY_Strip"; 
      local_histos.globalPosXY_S = ibooker.book2D(histoName.str(), histoName.str(), 250, -1250., 1250., 250, -1250., 1250.);

      histoName.str("");
      histoName << "Local_Position_XY_Strip" ;
      local_histos.localPosXY_S = ibooker.book2D(histoName.str(), histoName.str(), 500, 0., 0., 500, 0., 0.);

      histoName.str("");
      histoName << "Delta_X_Strip";
      local_histos.deltaX_S = ibooker.book1D(histoName.str(), histoName.str(), 100, -0.02, 0.02);

      histoName.str("");
      histoName << "Delta_Y_Strip";
      local_histos.deltaY_S = ibooker.book1D(histoName.str(), histoName.str(), 100, -0.02, 0.02);

      histoName.str("");
      histoName << "Pull_X_Strip";
      local_histos.pullX_S = ibooker.book1D(histoName.str(), histoName.str(), 100, -3., 3.);

      histoName.str("");
      histoName << "Pull_Y_Strip";
      local_histos.pullY_S = ibooker.book1D(histoName.str(), histoName.str(), 100, -3., 3.);
      
      histoName.str("");
      histoName << "Delta_X_vs_Eta_S";
      local_histos.deltaX_eta_S = ibooker.bookProfile(histoName.str(), histoName.str(), 50, -2.5, 2.5, 100, -0.02, 0.02);

      histoName.str("");
      histoName << "Delta_Y_vs_Eta_S";
      local_histos.deltaY_eta_S = ibooker.bookProfile(histoName.str(), histoName.str(), 50, -2.5, 2.5, 100, -0.02, 0.02);

      histoName.str("");
      histoName << "Pull_X_vs_Eta_S";
      local_histos.pullX_eta_S = ibooker.bookProfile(histoName.str(), histoName.str(), 50, -2.5, 2.5, 100, -3., 3.);

      histoName.str("");
      histoName << "Pull_Y_vs_Eta_S";
      local_histos.pullY_eta_S = ibooker.bookProfile(histoName.str(), histoName.str(), 50, -2.5, 2.5, 100, -3., 3.);      
    }
    layerMEs_.insert(std::make_pair(key, local_histos));
  }
}
  
 
std::vector<unsigned int> Phase2TrackerValidateRecHit::getSimTrackId(const edm::Handle<edm::DetSetVector<PixelDigiSimLink> >& pixelSimLinks, 
								     const DetId& detId, unsigned int channel) {
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

std::string Phase2TrackerValidateRecHit::getHistoId(uint32_t det_id, const TrackerTopology* tTopo) {
  int layer;
  std::string Disc;
  std::ostringstream fname1;
  if(pixelFlag_){
    layer = tTopo->getITPixelLayerNumber(det_id);
  }
  else {
    layer = tTopo->getOTLayerNumber(det_id);
  }
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
    if (pixelFlag_) Disc = (disc < 9) ? "FPIX_1" : "FPIX_2";
    else Disc = (disc < 3) ? "TEDD_1" : "TEDD_2";
    fname1 << Disc << "/";
    int ring = tTopo->tidRing(det_id);
    fname1 << "Ring" << ring;
  }
  return fname1.str();
}
  //define this as a plug-in
DEFINE_FWK_MODULE(Phase2TrackerValidateRecHit);
 
