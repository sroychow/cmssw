// -*- C++ -*-
//
// Package:    PhysicsTools/NanoAOD
// Class:      PATObjectCrossLinker
// 
/**\class PATObjectCrossLinker PATObjectCrossLinker.cc PhysicsTools/PATObjectCrossLinker/plugins/PATObjectCrossLinker.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Andrea Rizzi
//         Created:  Mon, 28 Aug 2017 09:26:39 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"

#include "DataFormats/Common/interface/View.h"

#include "PhysicsTools/NanoAOD/interface/MatchingUtils.h"
//
// class declaration
//

class PATObjectCrossLinker : public edm::stream::EDProducer<> {
   public:
      explicit PATObjectCrossLinker(const edm::ParameterSet&);
      ~PATObjectCrossLinker() override;

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      void beginStream(edm::StreamID) override;
      void produce(edm::Event&, const edm::EventSetup&) override;
      void endStream() override;

      template < class C1, class C2,class C3, class C4>
      void matchOneToMany(const C1 & refProdOne,  C2 &  itemsOne, const std::string & nameOne,
                    const C3 & refProdMany, C4& itemsMany, const std::string & nameMany);

      template < class C1, class C2,class C3,class C4>
      void matchElectronToPhoton(const C1 & refProdOne, C2 & itemsOne, const std::string & nameOne,
                    const C3 & refProdMany, C4& itemsMany, const std::string & nameMany);

      template <class C1, class C2, class C3>
      void matchOneToVertices(C1& itemsOne,
                              const C2& refProdVtx,
                              C3& itemsVtx,
                              const std::string& nameVtx);

      template <class C1, class C2, class C3>
      void matchVertexToMany(const C1& refProdVtx,
                            C2& itemsVtx,
                            const std::string& nameVtx,
                            C3& itemsMany);

      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------
      const edm::EDGetTokenT<edm::View<pat::Jet>> jets_;
      const edm::EDGetTokenT<edm::View<pat::Muon>> muons_;
      const edm::EDGetTokenT<edm::View<pat::Electron>> electrons_;
      const edm::EDGetTokenT<edm::View<pat::Tau>> taus_;
      const edm::EDGetTokenT<edm::View<pat::Photon>> photons_;
      const edm::EDGetTokenT<edm::View<reco::VertexCompositePtrCandidate>> vertices_;


};

//
// constructors and destructor
//
PATObjectCrossLinker::PATObjectCrossLinker(const edm::ParameterSet& params):
    jets_(consumes<edm::View<pat::Jet>>( params.getParameter<edm::InputTag>("jets") )),
    muons_(consumes<edm::View<pat::Muon>>( params.getParameter<edm::InputTag>("muons") )),
    electrons_(consumes<edm::View<pat::Electron>>( params.getParameter<edm::InputTag>("electrons") )),
    taus_(consumes<edm::View<pat::Tau>>( params.getParameter<edm::InputTag>("taus") )),
    photons_(consumes<edm::View<pat::Photon>>( params.getParameter<edm::InputTag>("photons") )),
    vertices_(consumes<edm::View<reco::VertexCompositePtrCandidate>>(params.getParameter<edm::InputTag>("vertices")))

{
   produces<std::vector<pat::Jet>>("jets");
   produces<std::vector<pat::Muon>>("muons");
   produces<std::vector<pat::Electron>>("electrons");
   produces<std::vector<pat::Tau>>("taus");
   produces<std::vector<pat::Photon>>("photons");
   produces<std::vector<reco::VertexCompositePtrCandidate>>("vertices");
  
}


PATObjectCrossLinker::~PATObjectCrossLinker()
{
 
   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------

///
template < class C1, class C2,class C3,class C4>
void PATObjectCrossLinker::matchOneToMany(const C1 & refProdOne, C2 & itemsOne, const std::string & nameOne,
		    const C3 & refProdMany, C4& itemsMany, const std::string & nameMany)
{
    size_t ji=0;
    for(auto & j: itemsOne) {
        edm::PtrVector<reco::Candidate> overlaps(refProdMany.id());
        size_t mi=0;
        for(auto & m: itemsMany){
	    if(matchByCommonSourceCandidatePtr(j,m) && (!m.hasUserCand(nameOne))){
                m.addUserCand(nameOne,reco::CandidatePtr(refProdOne.id(), ji, refProdOne.productGetter()));
                overlaps.push_back(reco::CandidatePtr(refProdMany.id(), mi, refProdMany.productGetter()));
            }
            mi++;
        }
    j.setOverlaps(nameMany,overlaps);
    ji++;
   } 
}

template < class C1, class C2,class C3,class C4>
void PATObjectCrossLinker::matchElectronToPhoton(const C1 & refProdOne, C2 & itemsOne, const std::string & nameOne,
		    const C3 & refProdMany, C4& itemsMany, const std::string & nameMany)
{
    size_t ji=0;
    for(auto & j: itemsOne) {
        edm::PtrVector<reco::Candidate> overlaps(refProdMany.id());
        size_t mi=0;
        for(auto & m: itemsMany){
	    if(matchByCommonParentSuperClusterRef(j,m) && (!m.hasUserCand(nameOne))){
                m.addUserCand(nameOne,reco::CandidatePtr(refProdOne.id(), ji, refProdOne.productGetter()));
                overlaps.push_back(reco::CandidatePtr(refProdMany.id(), mi, refProdMany.productGetter()));
            }
            mi++;
        }
    j.setOverlaps(nameMany,overlaps);
    ji++;
   } 
}

// several vertices stored as overlap with a jet
// (reco vertices have no userCands)
template <class C1, class C2, class C3>
void PATObjectCrossLinker::matchOneToVertices(C1& itemsOne,
                                            const C2& refProdVtx,
                                            C3& itemsVtx,
                                            const std::string& nameVtx) {
  for (auto& j : itemsOne) {
    edm::PtrVector<reco::Candidate> overlaps(refProdVtx.id());
    size_t vi = 0;
    for (auto& v : itemsVtx) {
      if (matchByCommonSourceCandidatePtr(j, v)) {
        overlaps.push_back(reco::CandidatePtr(refProdVtx.id(), vi, refProdVtx.productGetter()));
      }
      vi++;
    }
    j.setOverlaps(nameVtx, overlaps);
  }
}

// vertex stored as userCand to matched object
// (reco vertices have no overlaps)
template <class C1, class C2, class C3>
void PATObjectCrossLinker::matchVertexToMany(const C1& refProdVtx,
                                          C2& itemsVtx,
                                          const std::string& nameVtx,
                                          C3& itemsMany) {
  size_t vi = 0;
  for (auto& v : itemsVtx) {
    for (auto& m : itemsMany) {
      if (matchByCommonSourceCandidatePtr(v, m) && (!m.hasUserCand(nameVtx))) {
        m.addUserCand(nameVtx, reco::CandidatePtr(refProdVtx.id(), vi, refProdVtx.productGetter()));
      }
    }
    vi++;
  }
}



void
PATObjectCrossLinker::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;
    edm::Handle<edm::View<pat::Jet>> jetsIn;
    iEvent.getByToken(jets_, jetsIn);
    auto jets = std::make_unique<std::vector<pat::Jet>>();
    for(const auto & j  : *jetsIn) jets->push_back(j);
    auto jetRefProd =  iEvent.getRefBeforePut< std::vector<pat::Jet> >("jets");

    edm::Handle<edm::View<pat::Muon>> muonsIn;
    iEvent.getByToken(muons_, muonsIn);
    auto muons = std::make_unique<std::vector<pat::Muon>>();
    for(const auto & m  : *muonsIn) muons->push_back(m);
    auto muRefProd =  iEvent.getRefBeforePut< std::vector<pat::Muon> >("muons");

    edm::Handle<edm::View<pat::Electron>> electronsIn;
    iEvent.getByToken(electrons_, electronsIn);
    auto electrons = std::make_unique<std::vector<pat::Electron>>();
    for(const auto & e  : *electronsIn) electrons->push_back(e);
    auto eleRefProd =  iEvent.getRefBeforePut< std::vector<pat::Electron> >("electrons");

    edm::Handle<edm::View<pat::Tau>> tausIn;
    iEvent.getByToken(taus_, tausIn);
    auto taus = std::make_unique<std::vector<pat::Tau>>();
    for(const auto & t  : *tausIn) taus->push_back(t);
    auto tauRefProd =  iEvent.getRefBeforePut< std::vector<pat::Tau> >("taus");

    edm::Handle<edm::View<pat::Photon>> photonsIn;
    iEvent.getByToken(photons_, photonsIn);
    auto photons = std::make_unique<std::vector<pat::Photon>>();
    for(const auto & p  : *photonsIn) photons->push_back(p);
    auto phRefProd =  iEvent.getRefBeforePut< std::vector<pat::Photon> >("photons");

    const auto& verticesIn = iEvent.get(vertices_);
    auto vertices = std::make_unique<std::vector<reco::VertexCompositePtrCandidate>>();
    vertices->reserve(verticesIn.size());
    for (const auto& v : verticesIn)
      vertices->push_back(v);
    auto vtxRefProd = iEvent.getRefBeforePut<std::vector<reco::VertexCompositePtrCandidate>>("vertices");

    matchOneToMany(jetRefProd,*jets,"jet",muRefProd,*muons,"muons");
    matchOneToMany(jetRefProd,*jets,"jet",eleRefProd,*electrons,"electrons");
    matchOneToMany(jetRefProd,*jets,"jet",tauRefProd,*taus,"taus");
    matchOneToMany(jetRefProd,*jets,"jet",phRefProd,*photons,"photons");

    matchElectronToPhoton(eleRefProd,*electrons,"electron",phRefProd,*photons,"photons");

    matchOneToVertices(*jets, vtxRefProd, *vertices, "vertices");
    matchOneToVertices(*taus, vtxRefProd, *vertices, "vertices");
    matchVertexToMany(vtxRefProd, *vertices, "vertex", *muons);
    matchVertexToMany(vtxRefProd, *vertices, "vertex", *electrons);

    iEvent.put(std::move(jets),"jets");
    iEvent.put(std::move(muons),"muons");
    iEvent.put(std::move(electrons),"electrons");
    iEvent.put(std::move(taus),"taus");
    iEvent.put(std::move(photons),"photons");
    iEvent.put(std::move(vertices), "vertices");
 
}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
PATObjectCrossLinker::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
PATObjectCrossLinker::endStream() {
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
PATObjectCrossLinker::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(PATObjectCrossLinker);
