from PhysicsTools.NanoAOD.taus_cff import *
from PhysicsTools.NanoAOD.jets_cff import *
from PhysicsTools.NanoAOD.globals_cff import *
from PhysicsTools.NanoAOD.genparticles_cff import *
from PhysicsTools.NanoAOD.particlelevel_cff import *
from PhysicsTools.NanoAOD.lheInfoTable_cfi import *
from PhysicsTools.NanoAOD.genWeightsTable_cfi import *
import ROOT

genWeights = cms.EDProducer("GenWeightProductProducer")

lheWeightsTable = cms.EDProducer(
    "LHEWeightsTableProducer",
    lheInfo = cms.InputTag("externalLHEProducer"),
    lheWeights = cms.InputTag("externalLHEProducer"),
    genWeights = cms.InputTag("genWeights"),
    #weightgroups = cms.vint32([ROOT.gen.kScaleWeights, ROOT.gen.kMEParamWeights, ROOT.gen.kPdfWeights, ROOT.UnknownWeights]),
    #weightgroups = cms.vint32([0,1,2,3,4]),
    #numWeightgroups = cms.vint([1, -1, 1, 2, 1]),
    #pdfs = cms.vint([91400, 306000, 260000]),
    lheWeightPrecision = cms.int32(14),
)

nanoMetadata = cms.EDProducer("UniqueStringProducer",
    strings = cms.PSet(
        tag = cms.string("untagged"),
    )
)

metGenTable = cms.EDProducer("SimpleCandidateFlatTableProducer",
    src = cms.InputTag("genMetTrue"),
    name = cms.string("GenMET"),
    doc = cms.string("Gen MET"),
    singleton = cms.bool(True),
    extension = cms.bool(False),
    variables = cms.PSet(
       pt  = Var("pt",  float, doc="pt", precision=10),
       phi = Var("phi", float, doc="phi", precision=10),
    ),
)

nanogenSequence = cms.Sequence(
    genWeights+
    nanoMetadata+
    particleLevel+
    genJetTable+
    patJetPartons+
    genJetFlavourAssociation+
    genJetFlavourTable+
    genJetAK8Table+
    genJetAK8FlavourAssociation+
    genJetAK8FlavourTable+
    tauGenJets+
    tauGenJetsSelectorAllHadrons+
    genVisTaus+
    genVisTauTable+
    genTable+
    genWeightsTable+
    lheWeightsTable+
    genParticleTables+
    tautagger+
    rivetProducerHTXS+
    particleLevelTables+
    metGenTable+
    lheInfoTable
)

NANOAODGENoutput = cms.OutputModule("NanoAODOutputModule",
    compressionAlgorithm = cms.untracked.string('LZMA'),
    compressionLevel = cms.untracked.int32(9),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('NANOAODSIM'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('nanogen.root'),
    outputCommands = cms.untracked.vstring(
        'drop *',
        "keep *_lheWeightsTable_*_*",     # event data
        "keep nanoaodFlatTable_*Table_*_*",     # event data
        "keep String_*_genModel_*",  # generator model data
        "keep nanoaodMergeableCounterTable_*Table_*_*", # accumulated per/run or per/lumi data
        "keep nanoaodUniqueString_nanoMetadata_*_*",   # basic metadata
    )
)

def customizeNanoGEN(process):
    process.genParticleTable.src = "genParticles"
    process.patJetPartons.particles = "genParticles"
    process.particleLevel.src = "generatorSmeared"
    process.rivetProducerHTXS.HepMCCollection = "generatorSmeared"

    process.genJetTable.src = "ak4GenJetsNoNu"
    process.genJetFlavourAssociation.jets = process.genJetTable.src
    process.genJetFlavourTable.src = process.genJetTable.src
    process.genJetFlavourTable.jetFlavourInfos = "genJetFlavourAssociation"
    process.genJetAK8Table.src = "ak8GenJetsNoNu"
    process.genJetAK8FlavourAssociation.jets = process.genJetAK8Table.src
    process.genJetAK8FlavourTable.src = process.genJetAK8Table.src
    process.tauGenJets.GenParticles = "genParticles"
    process.genVisTaus.srcGenParticles = "genParticles"
    return process
