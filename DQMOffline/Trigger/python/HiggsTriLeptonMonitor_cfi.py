import FWCore.ParameterSet.Config as cms

#####Same flavour trilepton monitor###########
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

hlthiggsTriLeptonCrossTrigMonitoring.electrons = cms.InputTag("gedGsfElectrons") # while pfIsolatedElectronsEI are reco::PFCandidate !
hlthiggsTriLeptonCrossTrigMonitoring.nelectrons = cms.int32(1)
hlthiggsTriLeptonCrossTrigMonitoring.muons     = cms.InputTag("muons") # while pfIsolatedMuonsEI are reco::PFCandidate !
hlthiggsTriLeptonCrossTrigMonitoring.nmuons = cms.int32(2)

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
