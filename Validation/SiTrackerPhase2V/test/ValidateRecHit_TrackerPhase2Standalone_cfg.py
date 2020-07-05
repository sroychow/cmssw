import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Phase2_cff import Phase2
process = cms.Process('recHitTest',Phase2)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)
#process.MessageLogger = cms.Service("MessageLogger",
#    debugModules = cms.untracked.vstring('siPixelRawData'),
#    destinations = cms.untracked.vstring("cout"),
#    cout = cms.untracked.PSet(
#        threshold = cms.untracked.string('ERROR')
#    )
#)
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.Geometry.GeometryExtended2026D49Reco_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic', '')
# list of files

process.source = cms.Source("PoolSource",
    fileNames =  cms.untracked.vstring(
        'file:/eos/cms/store/relval/CMSSW_11_1_0_pre8/RelValTTbar_14TeV/GEN-SIM-RECO/PU25ns_110X_mcRun4_realistic_v3_2026D49PU200_raw1100-v1/20000/051C2F4A-56AC-3943-BA4D-3DF295E026DF.root'
      )
)
# Production Info
process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.19 $'),
    annotation = cms.untracked.string('step1 nevts:1'),
    name = cms.untracked.string('Applications')
)
# Output definition

process.DQMoutput = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
    outputCommands = process.DQMEventContent.outputCommands,
    fileName = cms.untracked.string('step1_rechitTest.root'),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string(''),
        dataTier = cms.untracked.string('')
    )
)

process.load('RecoLocalTracker.SiPhase2Clusterizer.phase2TrackerClusterizer_cfi')
process.load('RecoLocalTracker.Phase2TrackerRecHits.Phase2StripCPEESProducer_cfi')
process.load('RecoLocalTracker.Phase2TrackerRecHits.Phase2TrackerRecHits_cfi')
process.rechits_step = cms.Path(process.siPhase2RecHits)

process.load('Validation.SiTrackerPhase2V.Phase2TrackerValidateRecHit_cfi')
#process.rechitval_seq = cms.Sequence(process.rechitValid)

process.load('DQMServices.Components.DQMEventInfo_cfi')
#process.dqmEnv.subSystemFolder = cms.untracked.string('Ph2Tk')
process.dqm_comm = cms.Sequence(process.dqmEnv)

# Path and EndPath definitions
process.endjob_step = cms.EndPath(process.endOfProcess)
process.DQMoutput_step = cms.EndPath(process.DQMoutput)

process.p = cms.Path(process.siPhase2RecHits * process.rechitValid * process.dqm_comm )
