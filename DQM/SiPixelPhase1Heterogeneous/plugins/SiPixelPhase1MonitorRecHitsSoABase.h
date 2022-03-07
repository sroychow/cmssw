// -*- C++ -*-
///bookLayer
// Package:    SiPixelPhase1MonitorRecHitsSoA
// Class:      SiPixelPhase1MonitorRecHitsSoA
//
/**\class SiPixelPhase1MonitorRecHitsSoA SiPixelPhase1MonitorRecHitsSoABase 
*/
//
// Author: Suvankar Roy Chowdhury
//
#ifndef DQM_SiPixelPhase1Heterogeneous_SiPixelPhase1MonitorRecHitsSoABase_h
#define DQM_SiPixelPhase1Heterogeneous_SiPixelPhase1MonitorRecHitsSoABase_h
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
// DQM Histograming
#include "DQMServices/Core/interface/MonitorElement.h"
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/DQMStore.h"
//Geometry
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/CommonDetUnit/interface/PixelGeomDetUnit.h"
#include "Geometry/CommonTopologies/interface/PixelTopology.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"

class DetId;

class SiPixelPhase1MonitorRecHitsSoABase : public DQMEDAnalyzer {
public:
  SiPixelPhase1MonitorRecHitsSoABase(const edm::ParameterSet&);
  ~SiPixelPhase1MonitorRecHitsSoABase() override;
  void dqmBeginRun(const edm::Run&, const edm::EventSetup&) override;
  void bookHistograms(DQMStore::IBooker& ibooker, edm::Run const& iRun, edm::EventSetup const& iSetup) override;
  void fillHistosForRecHit(const DetId& id,
                           const float xG,
                           const float yG,
                           const float zG,
                           const float rG,
                           const float fphi,
                           const uint32_t charge = 0,
                           const int16_t sizeX = 0,
                           const int16_t sizeY = 0);
  static void fillPSetDescription(edm::ParameterSetDescription& desc);

protected:
  edm::ParameterSet config_;
  const edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> geomToken_;
  const edm::ESGetToken<TrackerTopology, TrackerTopologyRcd> topoToken_;
  const TrackerGeometry* tkGeom_ = nullptr;
  const TrackerTopology* tTopo_ = nullptr;
  std::string topFolderName_;
  bool onGPU_;
  MonitorElement* hnHits;
  MonitorElement* hBFposZP;
  MonitorElement* hBFposZR;
  MonitorElement* hBposXY;
  MonitorElement* hBposZP;
  MonitorElement* hBcharge;
  MonitorElement* hBsizex;
  MonitorElement* hBsizey;
  MonitorElement* hBposXYL[4];
  MonitorElement* hBposZPL[4];
  MonitorElement* hBchargeL[4];
  MonitorElement* hBsizexL[4];
  MonitorElement* hBsizeyL[4];
  MonitorElement* hFposXY;
  MonitorElement* hFposZP;
  MonitorElement* hFcharge;
  MonitorElement* hFsizex;
  MonitorElement* hFsizey;
  MonitorElement* hFposXYD[2][3];
  MonitorElement* hFposZPD[2][3];
  MonitorElement* hFchargeD[2][3];
  MonitorElement* hFsizexD[2][3];
  MonitorElement* hFsizeyD[2][3];
};
#endif
