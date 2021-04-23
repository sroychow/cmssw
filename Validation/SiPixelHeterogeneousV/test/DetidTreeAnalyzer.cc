#include <cuda_runtime.h>
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "Geometry/CommonDetUnit/interface/PixelGeomDetUnit.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/CommonTopologies/interface/GeomDetEnumerators.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/SiPixelDigi/interface/PixelDigi.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "DataFormats/TrackerCommon/interface/PixelBarrelName.h"
#include "DataFormats/TrackerCommon/interface/PixelEndcapName.h"
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHitCollection.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"
#include <map>
#include <vector>
#include <TTree.h>
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DetInfo.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

class DetidTreeAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {

public:
  explicit DetidTreeAnalyzer(const edm::ParameterSet& iConfig);
  ~DetidTreeAnalyzer() override = default;
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void analyze(const edm::Event& iEvent, edm::EventSetup const& iSetup) override;
  bool isPixel(const DetId& detid);
  edm::EDGetTokenT<edm::DetSetVector<PixelDigi>> digiToken_;  
  edm::EDGetTokenT<edmNew::DetSetVector<SiPixelCluster>> clustersToken_;
  edm::EDGetTokenT<SiPixelRecHitCollection> recHitToken_;
  const edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> geomToken_;
  const edm::ESGetToken<TrackerTopology, TrackerTopologyRcd> topoToken_;
  TTree* dtree_;
  std::map<unsigned int, DetInfo>* detVec_;
  const TrackerGeometry* tkGeom_ = nullptr;
  const TrackerTopology* tTopo_ = nullptr;
};

DetidTreeAnalyzer::DetidTreeAnalyzer(const edm::ParameterSet& iConfig)
    : digiToken_(consumes<edm::DetSetVector<PixelDigi>>(iConfig.getParameter<edm::InputTag>("src"))),
      clustersToken_(consumes<edmNew::DetSetVector<SiPixelCluster>>(iConfig.getParameter<edm::InputTag>("clustersSrc"))),
      recHitToken_(consumes<SiPixelRecHitCollection>(iConfig.getParameter<edm::InputTag>("recHitSrc"))),
      geomToken_(esConsumes<TrackerGeometry, TrackerDigiGeometryRecord, edm::Transition::BeginRun>()),
      topoToken_(esConsumes<TrackerTopology, TrackerTopologyRcd, edm::Transition::BeginRun>())
{

  usesResource(TFileService::kSharedResource);
  detVec_ = new std::map<unsigned int, DetInfo>();
  edm::Service<TFileService> fs;
  dtree_ = fs->make<TTree>("detinfo", "Tree with local objects per detId");
  dtree_->Branch("digiInfo", &detVec_); 
}

void DetidTreeAnalyzer::analyze(const edm::Event& iEvent, edm::EventSetup const& iSetup) {
  detVec_->clear();
  //For Digi
  edm::Handle<edm::DetSetVector<PixelDigi>> input;
  iEvent.getByToken(digiToken_, input);
  const auto& tkGeom_ = &iSetup.getData(geomToken_);
  const auto& tTopo_ = &iSetup.getData(topoToken_);

  if (!input.isValid())
    return;
  edm::DetSetVector<PixelDigi>::const_iterator it;
  //std::cout << "Digi DetSetVectorSize=" << input->size() << std::endl;
  for (it = input->begin(); it != input->end(); ++it) {
    unsigned int detId_raw = it->id;
    if(detVec_->find(detId_raw) == detVec_->end()) {
      std::cout << "Not found:" << it->id << std::endl;
      continue;
    }
    DetInfo dinfo;
    const GeomDetUnit* gDetUnit = tkGeom_->idToDetUnit(DetId(detId_raw));
    const GlobalPoint& pos = gDetUnit->position();
    dinfo.x = pos.x();
    dinfo.y = pos.y();
    dinfo.z = pos.z();
    dinfo.eta = pos.eta();

    dinfo.ndigi = it->size();
    for (edm::DetSet<PixelDigi>::const_iterator di = it->begin(); di != it->end(); di++) {
      dinfo.digi_adc.emplace_back(di->adc());
      dinfo.digi_row.emplace_back(di->row());
      dinfo.digi_col.emplace_back(di->column());
    }
    bool pxb = false;
    if(DetId(detId_raw).subdetId() == PixelSubdetector::PixelBarrel)  pxb = true;
    dinfo.isbarrel = pxb;
    if(pxb) {
      PixelBarrelName pbname(DetId(detId_raw), tTopo_, true);
      dinfo.layer = pbname.layerName();
      dinfo.shell = pbname.shell();
    }
    else { 
      PixelEndcapName pename(DetId(detId_raw), tTopo_, true);
      dinfo.shell =  pename.halfCylinder();
      dinfo.layer =  pename.ringName();
    }
    detVec_->insert({detId_raw, dinfo});
  }//end loop over Digi

  //For cluster
  edm::Handle<edmNew::DetSetVector<SiPixelCluster>> clusterColl;
  iEvent.getByToken(clustersToken_, clusterColl);
  edmNew::DetSetVector<SiPixelCluster>::const_iterator cit;
  //std::cout << "Digi DetSetVectorSize=" << clusterColl->size() << std::endl;
  for (cit = clusterColl->begin(); cit != clusterColl->end(); ++cit) {
    auto detId_raw = cit->id();
    if(detVec_->find(detId_raw) == detVec_->end()) {
      //std::cout << "Not found in cluster detsetV:" << detId_raw << std::endl;
      continue;
    }

    detVec_->at(detId_raw).ncluster = cit->size();

    for (edmNew::DetSet<SiPixelCluster>::const_iterator ci = cit->begin(); ci != cit->end(); ci++) {
      SiPixelCluster const &cluster = *ci;
      if(detVec_->find(detId_raw) != detVec_->end()) {        
        detVec_->at(detId_raw).cluster_charge.emplace_back(cluster.charge());
        detVec_->at(detId_raw).cluster_sizeX.emplace_back(cluster.sizeX());
        detVec_->at(detId_raw).cluster_sizeY.emplace_back(cluster.sizeY());
        detVec_->at(detId_raw).cluster_x.emplace_back(cluster.x());
        detVec_->at(detId_raw).cluster_y.emplace_back(cluster.y());
        detVec_->at(detId_raw).cluster_minPixelRow.emplace_back(cluster.minPixelRow());
        detVec_->at(detId_raw).cluster_maxPixelRow.emplace_back(cluster.maxPixelRow());
        detVec_->at(detId_raw).cluster_minPixelCol.emplace_back(cluster.minPixelCol());
        detVec_->at(detId_raw).cluster_maxPixelCol.emplace_back(cluster.maxPixelCol());
      }
    }
  }//end loop over cluster detset

  //For RecHit
  edm::Handle<SiPixelRecHitCollection> recHitColl;
  iEvent.getByToken(recHitToken_, recHitColl);
  //std::cout << "Digi DetSetVectorSize=" << recHitColl->size() << std::endl;
  for (SiPixelRecHitCollection::const_iterator it = recHitColl->begin(); it != recHitColl->end(); ++it) {
    auto detId_raw = it->detId();
    if(detVec_->find(detId_raw) == detVec_->end()) {
      //std::cout << "Not found in recHit detsetV:" << detId_raw << std::endl;
      continue;
    }
    detVec_->at(detId_raw).nrechit = it->size();
    for (SiPixelRecHit const& rechit : *it) {
      LocalPoint lp = rechit.localPosition();
      LocalError lerr = rechit.localPositionError();

      if(detVec_->find(detId_raw) != detVec_->end()) {
        detVec_->at(detId_raw).rechit_x.emplace_back(lp.x());
        detVec_->at(detId_raw).rechit_y.emplace_back(lp.y());
        detVec_->at(detId_raw).rechit_xerror.emplace_back(lerr.xx());
        detVec_->at(detId_raw).rechit_yerror.emplace_back(lerr.yy());
	//now find cluster ref
	SiPixelRecHit::ClusterRef const& cls = rechit.cluster();
	//now loop over clustermap
	int matclsidx = -1;
	for(unsigned int ci = 0; ci < detVec_->at(detId_raw).ncluster; ci++) {
	  if(detVec_->at(detId_raw).cluster_minPixelRow.at(ci) == cls->minPixelRow()
	     && detVec_->at(detId_raw).cluster_maxPixelRow.at(ci) == cls->maxPixelRow()
	     && detVec_->at(detId_raw).cluster_minPixelCol.at(ci) == cls->minPixelCol()
             && detVec_->at(detId_raw).cluster_maxPixelCol.at(ci) == cls->maxPixelCol()) {
	    matclsidx = ci;
	    break;
	  }
        }
	detVec_->at(detId_raw).rechit_clusteridx.emplace_back(matclsidx);
      }
    }
  }//end rechit detset

  dtree_->Fill();
  
}

bool DetidTreeAnalyzer::isPixel(const DetId& detid) {
   if (detid.det() != DetId::Tracker)
     return false;
   if (detid.subdetId() == PixelSubdetector::PixelBarrel)
     return true;
   if (detid.subdetId() == PixelSubdetector::PixelEndcap)
     return true;
   return false;
}

void DetidTreeAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("src", edm::InputTag("simSiPixelDigis"));
  desc.add<edm::InputTag>("clustersSrc", edm::InputTag("siPixelClusters"));
  desc.add<edm::InputTag>("recHitSrc", edm::InputTag("siPixelRecHitsPreSplitting"));
  descriptions.add("DetidTreeAnalyzer", desc);
}


DEFINE_FWK_MODULE(DetidTreeAnalyzer);
