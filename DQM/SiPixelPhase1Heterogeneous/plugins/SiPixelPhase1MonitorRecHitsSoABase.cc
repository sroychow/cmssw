// -*- C++ -*-
///bookLayer
// Package:    SiPixelPhase1MonitorRecHitsSoA
// Class:      SiPixelPhase1MonitorRecHitsSoA
//
/**\class SiPixelPhase1MonitorRecHitsSoA SiPixelPhase1MonitorRecHitsSoABase 
*/
//
// Author: Alessandro Rossi, Suvankar Roy Chowdhury
//
#include "DataFormats/Math/interface/approx_atan2.h"
#include "CUDADataFormats/Common/interface/Product.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "HeterogeneousCore/CUDACore/interface/ScopedContext.h"
// DQM Histograming
#include "DQMServices/Core/interface/MonitorElement.h"
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "CUDADataFormats/TrackingRecHit/interface/TrackingRecHit2DHeterogeneous.h"
// for string manipulations
#include <fmt/printf.h>
// Geometry
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "Geometry/CommonDetUnit/interface/PixelGeomDetUnit.h"
#include "Geometry/CommonTopologies/interface/PixelTopology.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "SiPixelPhase1MonitorRecHitsSoABase.h"

SiPixelPhase1MonitorRecHitsSoABase::SiPixelPhase1MonitorRecHitsSoABase(const edm::ParameterSet& iConfig) 
  : 
  geomToken_(esConsumes<TrackerGeometry, TrackerDigiGeometryRecord, edm::Transition::BeginRun>()),
  topoToken_(esConsumes<TrackerTopology, TrackerTopologyRcd, edm::Transition::BeginRun>())
{
  topFolderName_ = iConfig.getParameter<std::string>("TopFolderName");  //"SiPixelHeterogeneous/PixelRecHitsSoA";
  onGPU_ = iConfig.getParameter<bool>("onGPU");
}

//
// destructor
//
SiPixelPhase1MonitorRecHitsSoABase::~SiPixelPhase1MonitorRecHitsSoABase() {
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
  edm::LogInfo("SiPixelPhase1MonitorRecHitsSoABase") << ">>> Destroy SiPixelPhase1MonitorRecHitsSoABase ";
}
void SiPixelPhase1MonitorRecHitsSoABase::dqmBeginRun(const edm::Run& iRun, const edm::EventSetup& iSetup) {
  tkGeom_ = &iSetup.getData(geomToken_);
  tTopo_ = &iSetup.getData(topoToken_);
}
//
// -- Book Histograms
//
void SiPixelPhase1MonitorRecHitsSoABase::bookHistograms(DQMStore::IBooker& iBook,
                                                  edm::Run const& iRun,
                                                  edm::EventSetup const& iSetup) {
  iBook.cd();
  iBook.setCurrentFolder(topFolderName_);

  // clang-format off
  //Global
  hnHits = iBook.book1D("nHits", "RecHits per event;RecHits;#events", 200, 0, 5000);
  hBFposZP = iBook.book2D("recHitsGlobalPosZP", "RecHits position Global;Z;#phi", 1000, -50, 50, 200,-3.2,3.2);
  hBFposZR = iBook.book2D("recHitsGlobalPosZR", "RecHits position Global;Z;R", 1000, -50, 50, 200,-20,20);
  //Barrel
  hBposXY = iBook.book2D("recHitsBarrelPosXY", "RecHits position Barrel;X;Y", 200, -20, 20, 200,-20,20);
  hBposZP = iBook.book2D("recHitsBarrelPosZP", "RecHits position Barrel;Z;#phi", 300, -30, 30, 200,-3.2,3.2);
  hBcharge = iBook.book1D("recHitsBarrelCharge", "RecHits Charge Barrel;Charge;#events", 250, 0, 100000);
  hBsizex = iBook.book1D("recHitsBarrelSizex", "RecHits SizeX Barrel;SizeX;#events", 50, 0, 50);
  hBsizey = iBook.book1D("recHitsBarrelSizey", "RecHits SizeY Barrel;SizeY;#events", 50, 0, 50);
  //Barrel Layer
  for(int il=0;il<4;il++){
    hBposXYL[il] = iBook.book2D(Form("recHitsBLay%dPosXY",il+1), Form("RecHits position Barrel Layer%d;X;Y",il+1), 200, -20, 20, 200,-20,20);
    hBposZPL[il] = iBook.book2D(Form("recHitsBLay%dPosZP",il+1), Form("RecHits position Barrel Layer%d;Z;#phi",il+1), 300, -30, 30, 200,-3.2,3.2);
    hBchargeL[il] = iBook.book1D(Form("recHitsBLay%dCharge",il+1), Form("RecHits Charge Barrel Layer%d;Charge;#events",il+1), 250, 0, 100000);
    hBsizexL[il] = iBook.book1D(Form("recHitsBLay%dSizex",il+1), Form("RecHits SizeX Barrel Layer%d;SizeX;#events",il+1), 50, 0, 50);
    hBsizeyL[il] = iBook.book1D(Form("recHitsBLay%dSizey",il+1), Form("RecHits SizeY Barrel Layer%d;SizeY;#events",il+1), 50, 0, 50);
  }
  //Endcaps
  hFposXY = iBook.book2D("recHitsEndcapsPosXY", "RecHits position Endcaps;X;Y", 200, -20, 20, 200,-20,-20);
  hFposZP = iBook.book2D("recHitsEndcapsPosZP", "RecHits position Endcaps;Z;#phi", 500, -50, 50, 200,-3.2,3.2);
  hFcharge = iBook.book1D("recHitsEndcapsCharge", "RecHits Charge Endcaps;Charge;#events", 250, 0, 100000);
  hFsizex = iBook.book1D("recHitsEndcapsSizex", "RecHits SizeX Endcaps;SizeX;#events", 50, 0, 50);
  hFsizey = iBook.book1D("recHitsEndcapsSizey", "RecHits SizeY Endcaps;SizeY;#events", 50, 0, 50);
  //Endcaps Disk
  for(int is=0;is<2;is++){
    int sign=is==0? -1:1;
    for(int id=0;id<3;id++){
      hFposXYD[is][id] = iBook.book2D(Form("recHitsFDisk%+dPosXY",id*sign+sign), Form("RecHits position Endcaps Disk%+d;X;Y",id*sign+sign), 200, -20, 20, 200,-20,20);
      hFposZPD[is][id] = iBook.book2D(Form("recHitsFDisk%+dPosZP",id*sign+sign), Form("RecHits position Endcaps Disk%+d;Z;#phi",id*sign+sign), 500, -50, 50, 200,-3.2,3.2);
      hFchargeD[is][id] = iBook.book1D(Form("recHitsFDisk%+dCharge",id*sign+sign), Form("RecHits Charge Endcaps Disk%+d;Charge;#events",id*sign+sign), 250, 0, 100000);
      hFsizexD[is][id] = iBook.book1D(Form("recHitsFDisk%+dSizex",id*sign+sign), Form("RecHits SizeX Endcaps Disk%+d;SizeX;#events",id*sign+sign), 50, 0, 50);
      hFsizeyD[is][id] = iBook.book1D(Form("recHitsFDisk%+dSizey",id*sign+sign), Form("RecHits SizeY Endcaps Disk%+d;SizeY;#events",id*sign+sign), 50, 0, 50);
    }
  }
}

void SiPixelPhase1MonitorRecHitsSoABase::fillHistosForRecHit(const DetId& id, const float xG, const float yG, const float zG, const float rG, const float fphi, const uint32_t charge, const int16_t sizeX, const int16_t sizeY) {
  hBFposZP->Fill(zG,fphi);
  int16_t ysign=yG>=0? 1:-1;
  hBFposZR->Fill(zG,rG*ysign);
  switch(id.subdetId()){
  case PixelSubdetector::PixelBarrel:
    hBposXY->Fill(xG,yG);
    hBposZP->Fill(zG,fphi);
    hBcharge->Fill(charge);
    hBsizex->Fill(sizeX);
    hBsizey->Fill(sizeY);
    hBposXYL[tTopo_->pxbLayer(id)-1]->Fill(xG,yG);
    hBposZPL[tTopo_->pxbLayer(id)-1]->Fill(zG,fphi);
    if(!onGPU_) {
      hBchargeL[tTopo_->pxbLayer(id)-1]->Fill(charge);
      hBsizexL[tTopo_->pxbLayer(id)-1]->Fill(sizeX);
      hBsizeyL[tTopo_->pxbLayer(id)-1]->Fill(sizeY);
    }
    break;
  case PixelSubdetector::PixelEndcap:
    hFposXY->Fill(xG,yG);
    hFposZP->Fill(zG,fphi);
    hFcharge->Fill(charge);
    hFsizex->Fill(sizeX);
    hFsizey->Fill(sizeY);
    hFposXYD[tTopo_->pxfSide(id)-1][tTopo_->pxfDisk(id)-1]->Fill(xG,yG);
    hFposZPD[tTopo_->pxfSide(id)-1][tTopo_->pxfDisk(id)-1]->Fill(zG,fphi);
    if(!onGPU_) {
      hFchargeD[tTopo_->pxfSide(id)-1][tTopo_->pxfDisk(id)-1]->Fill(charge);
      hFsizexD[tTopo_->pxfSide(id)-1][tTopo_->pxfDisk(id)-1]->Fill(sizeX);
      hFsizeyD[tTopo_->pxfSide(id)-1][tTopo_->pxfDisk(id)-1]->Fill(sizeY);
    }
    break;
  }
}
void SiPixelPhase1MonitorRecHitsSoABase::fillPSetDescription(edm::ParameterSetDescription& desc) {
  desc.add<std::string>("TopFolderName", "SiPixelHeterogeneous/PixelRecHitsSoA");
}
