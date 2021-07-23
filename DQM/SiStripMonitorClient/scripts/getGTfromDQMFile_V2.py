#!/usr/bin/env python3
from __future__ import print_function
from math import *
from ROOT import TFile, TObject, TTree
from array import array
from ROOT import gDirectory
import sys
import os

def getGTfromDQMFile(DQMfile, RunNumber, globalTagVar='Globaltag'):
    if not os.path.isfile(DQMfile):#    print "Error: file", DQMfile, "not found, exit" 
        sys.exit(0)
    thefile = TFile( DQMfile )
    globalTagDir = 'DQMData/Run ' + RunNumber + '/Info/Run summary/ProvInfo/'
    if not gDirectory.GetDirectory( globalTagDir ):
        print("Warning: Info/Run summary/ProvInfo/ directory not found in DQM file")
        sys.exit(0)    

    for key in  gDirectory.GetDirectory( globalTagDir ).GetListOfKeys():
        obj = key.ReadObj()
        if globalTagVar in obj.GetName():
            print(obj.GetName())
            globalTag = obj.GetName()[len("<"+globalTagVar+">s="):-len("</"+globalTagVar+">")]
    if len(globalTag) > 1:
        if globalTag.find('::') >= 0: 
            print(globalTag[0:globalTag.find('::')])
        else:
            print(globalTag)
    return globalTag

