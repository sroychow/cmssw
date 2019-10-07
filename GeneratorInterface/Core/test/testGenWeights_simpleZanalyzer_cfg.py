import FWCore.ParameterSet.Config as cms
import sys
import os
import re
from FWCore.ParameterSet.VarParsing import VarParsing

def commandline():
    options = VarParsing('analysis')

    options.register(
        "fileList",
        "",
        VarParsing.multiplicity.singleton,
        VarParsing.varType.string,
        "List of files to run on."
    )
    options.register(
        "isMiniAOD",
        False,
        VarParsing.multiplicity.singleton,
        VarParsing.varType.bool,
        "Sample is miniAOD"
    )
    options.register(
        "path",
        None,
        VarParsing.multiplicity.singleton,
        VarParsing.varType.string,
        "Path for input files"
    )

    options.parseArguments()


    print "OPTIONS: MiniAOD ---> " + ("Yes" if options.isMiniAOD else "No")
    print "OPTIONS: Tag --->  {TAG}".format(TAG=options.tag)


    if(len(options.inputFiles) and options.fileList != ""):
        print "ERROR: Please provide either fileList or inputFiles but not both."
        sys.exit(1)
    elif(options.fileList != ""):
        options.inputFiles.extend(read_list_from_file(options.fileList))
    elif(options.path):
        options.inputFiles.extend(find_files(options.path,'.*\.root'))
    print "Found {N} files.".format(N=len(options.inputFiles))
    
    return options
# Define the CMSSW process
process = cms.Process("demo")
options = commandline()

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring( options.inputFiles )
)

process.testLheWeights = cms.EDProducer("LHEWeightProductProducer")
process.testGenWeights = cms.EDProducer("GenWeightProductProducer")

process.demo = cms.EDAnalyzer('GenWeightsTestAnalyzer',
    tag = cms.string(options.tag),
    miniaod = cms.bool(options.isMiniAOD),
    genParticleSrc = cms.InputTag("prunedGenParticles" if options.isMiniAOD  else "genParticles"),
    genJetSrc = cms.InputTag( "slimmedGenJets" if options.isMiniAOD  else "ak4GenJetsNoNu"),
    genMETSrc = cms.InputTag( "slimmedMETs" if options.isMiniAOD  else "genMetTrue"),
    LHESrc = cms.InputTag("externalLHEProducer"),
    GenSrc = cms.InputTag("generator")
)

process.TFileService = cms.Service("TFileService", fileName = cms.string("analysis_{TAG}.root".format(TAG=options.tag)) )

process.p = cms.Path(process.testLheWeights*process.testGenWeights*process.demo)

