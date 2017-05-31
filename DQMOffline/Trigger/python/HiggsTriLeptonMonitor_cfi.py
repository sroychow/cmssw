import FWCore.ParameterSet.Config as cms

#####Same flavour trilepton monitor###########
from higgsTripleMuonMonitoring_cfi import higgsTripleMuonMonitoring

hlthiggsTripleMuonMonitoring = higgsTripleMuonMonitoring.clone()

hlthiggsTripleMuonMonitoring.FolderName = cms.string('HLT/Higgs/TriLepton/HLT_TripleMu_12_10_5/')
hlthiggsTripleMuonMonitoring.histoPSet.lepPSet = cms.PSet(
  nbins = cms.int32 (  200  ),
  xmin  = cms.double(   -0.5),
  xmax  = cms.double(19999.5),
)
hlthiggsTripleMuonMonitoring.histoPSet.phiPSet = cms.PSet(
  nbins = cms.int32 (  64 ),
  xmin  = cms.double( -3.2),
  xmax  = cms.double( 3.2 ),
)
hlthiggsTripleMuonMonitoring.histoPSet.etaPSet = cms.PSet(
  nbins = cms.int32 (  60 ),
  xmin  = cms.double( -3.),
  xmax  = cms.double(  3. ),
)

hlthiggsTripleMuonMonitoring.vertices     = cms.InputTag("offlinePrimaryVertices")
hlthiggsTripleMuonMonitoring.leptondxycut = cms.double(0.5)
hlthiggsTripleMuonMonitoring.muons     = cms.InputTag("muons") # while pfIsolatedMuonsEI are reco::PFCandidate !
hlthiggsTripleMuonMonitoring.nmuons = cms.int32(3)
hlthiggsTripleMuonMonitoring.leptondzcut = cms.double(1.)

hlthiggsTripleMuonMonitoring.numGenericTriggerEventPSet.andOr         = cms.bool( False )
#hlthiggsTripleMuonMonitoring.numGenericTriggerEventPSet.dbLabel	= cms.string("HiggsDQMTrigger") # it does not exist yet, we should consider the possibility of using the DB, but as it is now$
hlthiggsTripleMuonMonitoring.numGenericTriggerEventPSet.andOrHlt      = cms.bool(True)# True:=OR; False:=AND
hlthiggsTripleMuonMonitoring.numGenericTriggerEventPSet.hltInputTag   = cms.InputTag( "TriggerResults::HLT" )
hlthiggsTripleMuonMonitoring.numGenericTriggerEventPSet.hltPaths      = cms.vstring("HLT_TripleMu_12_10_5_v*") # HLT_ZeroBias_v*
#hlthiggsTripleMuonMonitoring.numGenericTriggerEventPSet.hltDBKey	= cms.string("Higgs_HLT_TriLepton")
hlthiggsTripleMuonMonitoring.numGenericTriggerEventPSet.errorReplyHlt = cms.bool( False )
hlthiggsTripleMuonMonitoring.numGenericTriggerEventPSet.verbosityLevel = cms.uint32(1)

hlthiggsTripleMuonMonitoring.denGenericTriggerEventPSet.andOr         = cms.bool( False )
hlthiggsTripleMuonMonitoring.denGenericTriggerEventPSet.dcsInputTag   = cms.InputTag( "scalersRawToDigi" )
hlthiggsTripleMuonMonitoring.denGenericTriggerEventPSet.dcsPartitions = cms.vint32 ( 24, 25, 26, 27, 28, 29 ) # 24-27: strip, 28-29: pixel, we should add all other detectors !
hlthiggsTripleMuonMonitoring.denGenericTriggerEventPSet.andOrDcs      = cms.bool( False )
hlthiggsTripleMuonMonitoring.denGenericTriggerEventPSet.errorReplyDcs = cms.bool( True )
hlthiggsTripleMuonMonitoring.denGenericTriggerEventPSet.verbosityLevel = cms.uint32(1)
hlthiggsTripleMuonMonitoring.denGenericTriggerEventPSet.hltPaths = cms.vstring("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v*","HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v*")

####Triple Electron monitor######
from higgsTripleElectronMonitoring_cfi import higgsTripleElectronMonitoring

hlthiggsTripleElectronMonitoring = higgsTripleElectronMonitoring.clone()
hlthiggsTripleElectronMonitoring.FolderName = cms.string('HLT/Higgs/TriLepton/HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL/')
hlthiggsTripleElectronMonitoring.histoPSet.lepPSet = cms.PSet(
  nbins = cms.int32 (  200  ),
  xmin  = cms.double(   -0.5),
  xmax  = cms.double(19999.5),
)
hlthiggsTripleElectronMonitoring.histoPSet.phiPSet = cms.PSet(
  nbins = cms.int32 (  64 ),
  xmin  = cms.double( -3.2),
  xmax  = cms.double( 3.2 ),
)
hlthiggsTripleElectronMonitoring.histoPSet.etaPSet = cms.PSet(
  nbins = cms.int32 (  60 ),
  xmin  = cms.double( -3.),
  xmax  = cms.double(  3. ),
)

hlthiggsTripleElectronMonitoring.electrons = cms.InputTag("gedGsfElectrons") # while pfIsolatedElectronsEI are reco::PFCandidate !
hlthiggsTripleElectronMonitoring.nelectrons = cms.int32(3)
hlthiggsTripleElectronMonitoring.vertices     = cms.InputTag("offlinePrimaryVertices")
hlthiggsTripleElectronMonitoring.leptondxycut = cms.double(0.5)
hlthiggsTripleElectronMonitoring.leptondzcut = cms.double(1.)

hlthiggsTripleElectronMonitoring.numGenericTriggerEventPSet.andOr         = cms.bool( False )
#hlthiggsTripleElectronMonitoring.numGenericTriggerEventPSet.dbLabel       = cms.string("HiggsDQMTrigger") # it does not exist yet, we should consider the possibility of using the DB, but as it is now$
hlthiggsTripleElectronMonitoring.numGenericTriggerEventPSet.andOrHlt      = cms.bool(True)# True:=OR; False:=AND
hlthiggsTripleElectronMonitoring.numGenericTriggerEventPSet.hltInputTag   = cms.InputTag( "TriggerResults::HLT" )
hlthiggsTripleElectronMonitoring.numGenericTriggerEventPSet.hltPaths      = cms.vstring("HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL_v*") # HLT_ZeroBias_v*
#hlthiggsTripleElectronMonitoring.numGenericTriggerEventPSet.hltDBKey      = cms.string("Higgs_HLT_TriLepton")
hlthiggsTripleElectronMonitoring.numGenericTriggerEventPSet.errorReplyHlt = cms.bool( False )
hlthiggsTripleElectronMonitoring.numGenericTriggerEventPSet.verbosityLevel = cms.uint32(1)

hlthiggsTripleElectronMonitoring.denGenericTriggerEventPSet.andOr         = cms.bool( False )
hlthiggsTripleElectronMonitoring.denGenericTriggerEventPSet.dcsInputTag   = cms.InputTag( "scalersRawToDigi" )
hlthiggsTripleElectronMonitoring.denGenericTriggerEventPSet.dcsPartitions = cms.vint32 ( 24, 25, 26, 27, 28, 29 ) # 24-27: strip, 28-29: pixel, we should add all other detectors !
hlthiggsTripleElectronMonitoring.denGenericTriggerEventPSet.andOrDcs      = cms.bool( False )
hlthiggsTripleElectronMonitoring.denGenericTriggerEventPSet.errorReplyDcs = cms.bool( True )
hlthiggsTripleElectronMonitoring.denGenericTriggerEventPSet.verbosityLevel = cms.uint32(1)
hlthiggsTripleElectronMonitoring.denGenericTriggerEventPSet.hltPaths = cms.vstring("HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v*")

#####Trilepton cross-trigger monitor###########
from higgsTriLeptonCrossTriggerMonitoring_cfi import higgsTriLeptonCrossTriggerMonitoring

hlthiggsTriLeptonCrossTrigMonitoring = higgsTriLeptonCrossTriggerMonitoring.clone()
hlthiggsTriLeptonCrossTrigMonitoring.FolderName = cms.string('HLT/Higgs/TriLepton/HLT_DiMu9_Ele9_CaloIdL_TrackIdL_DZ/')
hlthiggsTriLeptonCrossTrigMonitoring.histoPSet.lepPSet = cms.PSet(
  nbins = cms.int32 (  200  ),
  xmin  = cms.double(   -0.5),
  xmax  = cms.double(19999.5),
)
hlthiggsTriLeptonCrossTrigMonitoring.histoPSet.phiPSet = cms.PSet(  
  nbins = cms.int32 (  64 ),
  xmin  = cms.double( -3.2),
  xmax  = cms.double( 3.2 ),
)
hlthiggsTriLeptonCrossTrigMonitoring.histoPSet.etaPSet = cms.PSet(
  nbins = cms.int32 (  60 ),
  xmin  = cms.double( -3.),
  xmax  = cms.double(  3. ),
)


hlthiggsTriLeptonCrossTrigMonitoring.electrons = cms.InputTag("gedGsfElectrons") # while pfIsolatedElectronsEI are reco::PFCandidate !
hlthiggsTriLeptonCrossTrigMonitoring.nelectrons = cms.int32(1)
hlthiggsTriLeptonCrossTrigMonitoring.muons     = cms.InputTag("muons") # while pfIsolatedMuonsEI are reco::PFCandidate !
hlthiggsTriLeptonCrossTrigMonitoring.nmuons = cms.int32(2)
hlthiggsTriLeptonCrossTrigMonitoring.vertices     = cms.InputTag("offlinePrimaryVertices") 

hlthiggsTriLeptonCrossTrigMonitoring.leptondxycut = cms.double(0.5)
hlthiggsTriLeptonCrossTrigMonitoring.leptondzcut = cms.double(1.)

hlthiggsTriLeptonCrossTrigMonitoring.numGenericTriggerEventPSet.andOr         = cms.bool( False )
#hlthiggsTriLeptonCrossTrigMonitoring.numGenericTriggerEventPSet.dbLabel       = cms.string("HiggsDQMTrigger") # it does not exist yet, we should consider the possibility of using the DB, but as it is now it will need a label per path !
hlthiggsTriLeptonCrossTrigMonitoring.numGenericTriggerEventPSet.andOrHlt      = cms.bool(True)# True:=OR; False:=AND
hlthiggsTriLeptonCrossTrigMonitoring.numGenericTriggerEventPSet.hltInputTag   = cms.InputTag( "TriggerResults::HLT" )
hlthiggsTriLeptonCrossTrigMonitoring.numGenericTriggerEventPSet.hltPaths      = cms.vstring("HLT_DiMu9_Ele9_CaloIdL_TrackIdL_DZ*") # HLT_ZeroBias_v*
#hlthiggsTriLeptonCrossTrigMonitoring.numGenericTriggerEventPSet.hltDBKey      = cms.string("Higgs_HLT_TriLepton")
hlthiggsTriLeptonCrossTrigMonitoring.numGenericTriggerEventPSet.errorReplyHlt = cms.bool( False )
hlthiggsTriLeptonCrossTrigMonitoring.numGenericTriggerEventPSet.verbosityLevel = cms.uint32(1)

hlthiggsTriLeptonCrossTrigMonitoring.denGenericTriggerEventPSet.andOr         = cms.bool( False )
hlthiggsTriLeptonCrossTrigMonitoring.denGenericTriggerEventPSet.dcsInputTag   = cms.InputTag( "scalersRawToDigi" )
hlthiggsTriLeptonCrossTrigMonitoring.denGenericTriggerEventPSet.dcsPartitions = cms.vint32 ( 24, 25, 26, 27, 28, 29 ) # 24-27: strip, 28-29: pixel, we should add all other detectors !
hlthiggsTriLeptonCrossTrigMonitoring.denGenericTriggerEventPSet.andOrDcs      = cms.bool( False )
hlthiggsTriLeptonCrossTrigMonitoring.denGenericTriggerEventPSet.errorReplyDcs = cms.bool( True )
hlthiggsTriLeptonCrossTrigMonitoring.denGenericTriggerEventPSet.verbosityLevel = cms.uint32(1)
hlthiggsTriLeptonCrossTrigMonitoring.denGenericTriggerEventPSet.hltPaths = cms.vstring("HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v*", "HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v*")
