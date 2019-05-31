from DataFormats.FWLite import Events,Handle,Runs
source = "externalLHEProducer"

#for filename in ["HIG-RunIIFall18wmLHEGS-00509.root"," HIG-RunIIFall18wmLHEGS-00509_ordered.root","HIG-RunIIFall18wmLHEGS-00509_unordered.root"]:
for filename in ["HIG-RunIIFall18wmLHEGS-00509.root"]:
    runs = Runs(filename)
    run = runs.__iter__().next()
    weightInfoHandle = Handle("LHEWeightInfoProduct")
    run.getByLabel(source, weightInfoHandle)
    weightInfoProd = weightInfoHandle.product()

    events = Events(filename)
    event = events.__iter__().next()
    weightHandle = Handle("LHEWeightProduct")
    event.getByLabel("externalLHEProducer", weightHandle)
    event.getByLabel(source, weightHandle)
    weightInfo = weightHandle.product()
    print "Content of the weights"
    for j, weights in enumerate(weightInfo.weights()):
        print "-"*10, "Looking at entry", j, "length is", len(weights),"-"*10
        matching = weightInfoProd.orderedWeightGroupInfo(j)
        print "Weights length?", len(weights), "Contained ids lenths?", len(matching.containedIds())
        print "-"*80
        for i,weight in enumerate(weights):
            print i, weight
            info = matching.weightMetaInfo(i)
            print "   ID, localIndex, globalIndex, label, set:", info.id, info.localIndex, info.globalIndex, info.label, matching.name()
        print "-"*80



