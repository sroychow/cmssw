import FWCore.ParameterSet.Config as cms

sipixeldetId = cms.EDAnalyzer("DetidTreeAnalyzer",
    src = cms.InputTag("siPixelDigisClustersPreSplitting"),
    clustersSrc = cms.InputTag("siPixelClustersPreSplitting"),
    recHitSrc = cms.InputTag("siPixelRecHitsPreSplitting")
)
