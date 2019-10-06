import FWCore.ParameterSet.Config as cms

process = cms.Process("test")

process.source = cms.Source("PoolSource",
            # replace 'myfile.root' with the source file you want to use
            fileNames = cms.untracked.vstring("/store/mc/RunIIAutumn18MiniAOD/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/MINIAODSIM/102X_upgrade2018_realistic_v15-v1/80000/E53E0D52-39FF-6F42-A8B2-EEF28EEB4C43.root")
        #fileNames = cms.untracked.vstring("file:CCC44F9F-64EF-E811-8F69-7845C4FBBD07.root"),
)

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(100))

process.out = cms.OutputModule("PoolOutputModule",
            fileName = cms.untracked.string('test.root'),
            outputCommands = cms.untracked.vstring(['keep *'])
)

process.testLHEWeights = cms.EDProducer("LHEWeightProductProducer")
process.testGenWeights = cms.EDProducer("GenWeightProductProducer")

process.p = cms.Path(process.testLHEWeights*process.testGenWeights)
#process.p = cms.Path(process.testLHEWeights)

process.output = cms.EndPath(process.out)
process.schedule = cms.Schedule(process.p,process.output)

