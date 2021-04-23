#include <cuda_runtime.h>
#include "CUDADataFormats/Common/interface/Product.h"
#include "CUDADataFormats/Common/interface/HostProduct.h"
#include "CUDADataFormats/Vertex/interface/ZVertexHeterogeneous.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackExtra.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHitCollection.h"
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
#include "HeterogeneousCore/CUDACore/interface/ScopedContext.h"
#include "RecoLocalTracker/SiPixelRecHits/interface/pixelCPEforGPU.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

class SiPixelValidateVerticesGPU : public DQMEDAnalyzer {

public:
  explicit SiPixelValidateVerticesGPU(const edm::ParameterSet& iConfig);
  ~SiPixelValidateVerticesGPU() override = default;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  using HMSstorage = HostProduct<unsigned int[]>;

private:
  void analyze(const edm::Event& iEvent, edm::EventSetup const& iSetup) override;
  void bookHistograms(DQMStore::IBooker&, edm::Run const&, edm::EventSetup const&) override;
  
  edm::EDGetTokenT<cms::cuda::Product<ZVertexHeterogeneous>> tokenverticesCUDA_;
  edm::EDGetTokenT<reco::BeamSpot> tokenbeamSpot_;
  edm::EDGetTokenT<reco::TrackCollection> tokenTracks_;  

  std::string topFolderName_;
  MonitorElement* hnVertices;
  MonitorElement* hvx;
  MonitorElement* hvy;
  MonitorElement* hvz;
};

SiPixelValidateVerticesGPU::SiPixelValidateVerticesGPU(const edm::ParameterSet& iConfig)
    : tokenverticesCUDA_(consumes<cms::cuda::Product<ZVertexHeterogeneous>>(iConfig.getParameter<edm::InputTag>("src"))),
      tokenbeamSpot_(consumes<reco::BeamSpot>(iConfig.getParameter<edm::InputTag>("beamSpotsrc"))),
      tokenTracks_(consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("trackCollectionsrc")))
{
  topFolderName_ = "SiPixelHeterogeneousV/PixelVerticesGPU";
}

void SiPixelValidateVerticesGPU::bookHistograms(DQMStore::IBooker& ibooker, edm::Run const& run , edm::EventSetup const& es) {
  ibooker.cd();
  ibooker.setCurrentFolder(topFolderName_);
  hnVertices = ibooker.book1D("nVerticesgpu", ";#of vertices;", 150, -0.5, 149.5);
  hvx = ibooker.book1D("vertex_xpos", ";xPos;", 500, -0.5, 0.5);
  hvy = ibooker.book1D("vertex_ypos", ";yPos;", 500, -0.5, 0.5);
  hvz = ibooker.book1D("vertex_zpos", ";zPos;", 500, -20., 20.);

}

void SiPixelValidateVerticesGPU::analyze(const edm::Event& iEvent, edm::EventSetup const& es) {
  //gpu vertices part
  auto const& inputDataWrapped = iEvent.get(tokenverticesCUDA_);
  cms::cuda::ScopedContextAnalyze ctx{inputDataWrapped};
  auto const& inputData = ctx.get(inputDataWrapped);
  auto& cudaVertices = *inputData.toHostAsync(ctx.stream());

  auto nv = cudaVertices.nvFinal;
  if(nv > cudaVertices.MAXVTX)   return;
  hnVertices->Fill(nv);

  //beamSpot
  edm::Handle<reco::BeamSpot> beampspotHandle;
  iEvent.getByToken(tokenbeamSpot_, beampspotHandle);
  float x0 = 0., y0 = 0., z0 = 0., dxdz = 0., dydz = 0.;
  if (!beampspotHandle.isValid()) {
    edm::LogWarning("SiPixelValidateVerticesGPU") << "No beamspot found. returning vertexes with (0,0,Z) ";
  } else {
    const reco::BeamSpot &bs = *beampspotHandle;
    x0 = bs.x0();
    y0 = bs.y0();
    z0 = bs.z0();
    dxdz = bs.dxdz();
    dydz = bs.dydz();
  }

  for (unsigned int i = 0; i < nv; i++) {
    //auto i = cudaVertices.sortInd[j];  // on gpu sorted in ascending order....
    //assert(i < nv);
    auto z = cudaVertices.zv[i];
    auto x = x0 + dxdz * z;
    auto y = y0 + dydz * z;
    z += z0;
    hvx->Fill(x);
    hvy->Fill(y);
    hvz->Fill(z);
  }
}

void SiPixelValidateVerticesGPU::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("trackCollectionsrc", edm::InputTag("pixelTracks"));
  desc.add<edm::InputTag>("beamSpotsrc", edm::InputTag("offlineBeamSpot"));
  desc.add<edm::InputTag>("src", edm::InputTag("pixelVertexCUDA"));
  descriptions.add("SiPixelValidateVerticesGPU", desc);
}
DEFINE_FWK_MODULE(SiPixelValidateVerticesGPU);
