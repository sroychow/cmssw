import FWCore.ParameterSet.Config as cms

process = cms.Process("test")

process.source = cms.Source("PoolSource",
            # replace 'myfile.root' with the source file you want to use
            fileNames = cms.untracked.vstring("/store/mc/RunIISummer16MiniAODv3/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/120000/CC675D46-5EDF-E811-B537-3CFDFE63DF40.root")
        #fileNames = cms.untracked.vstring("file:CCC44F9F-64EF-E811-8F69-7845C4FBBD07.root"),
)

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(100))

process.out = cms.OutputModule("PoolOutputModule",
            fileName = cms.untracked.string('test.root'),
            outputCommands = cms.untracked.vstring(['keep *'])
)

process.testWeights = cms.EDProducer("LHEWeightProductProducer")

process.p = cms.Path(process.testWeights)

process.output = cms.EndPath(process.out)
process.schedule = cms.Schedule(process.p,process.output)

