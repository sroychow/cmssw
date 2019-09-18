import FWCore.ParameterSet.Config as cms

process = cms.Process("test")

process.source = cms.Source("PoolSource",
            # replace 'myfile.root' with the source file you want to use
            fileNames = cms.untracked.vstring("file:HIG-RunIIFall18wmLHEGS-00509.root")
                )

process.TFileService = cms.Service("TFileService",
            fileName = cms.string("test.root")
)

process.testWeights = cms.EDAnalyzer("TestAnalyzer")

process.p = cms.Path(process.testWeights)

