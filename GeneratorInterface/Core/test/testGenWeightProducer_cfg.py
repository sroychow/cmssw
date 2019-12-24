import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing('analysis')
options.register(
    "lheSource",
    "externalLHEProducer",
    VarParsing.multiplicity.singleton,
    VarParsing.varType.string,
    "LHE source (default externalLHEProducer)"
)
options.parseArguments()

process = cms.Process("test")

process.source = cms.Source("PoolSource",
            fileNames = cms.untracked.vstring(options.inputFiles)
)

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(options.maxEvents))

process.out = cms.OutputModule("PoolOutputModule",
            fileName = cms.untracked.string('test.root'),
            outputCommands = cms.untracked.vstring(['keep *'])
)

process.testLHEWeights = cms.EDProducer("LHEWeightProductProducer",
                                    lheSource=cms.untracked.InputTag(options.lheSource),
                                    lheSourceLabel=cms.untracked.string(options.lheSource))
process.testGenWeights = cms.EDProducer("GenWeightProductProducer")

process.p = cms.Path(process.testLHEWeights*process.testGenWeights)
#process.p = cms.Path(process.testLHEWeights)

process.output = cms.EndPath(process.out)
process.schedule = cms.Schedule(process.p,process.output)

