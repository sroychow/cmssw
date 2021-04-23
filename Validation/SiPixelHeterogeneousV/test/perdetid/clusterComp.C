//#include "deInfo.h"
#include <TH1D.h>
#include <TH2D.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include<vector>
#include<map>
#include<iostream>
#include "DetInfo.h"
#include <TString.h>
#include<algorithm>
#include<string>
#include<utility>

struct DetHisto {
  TH2D* hclscharge;
  TH2D* hclsSizex;
  TH2D* hclsSizey; 
  TH2D* hncls;
  

  bool isbarrel;
  unsigned int layer;//layer or ring
  int shell;//shell or halfCylinder
  std::string folderName;
  
  void setFoldername(unsigned int rawid) {
    if(isbarrel) {
      folderName = "PXBarrel/Shell_" + std::to_string(shell) + "/PXLayer_" + layer + "/" + std::to_string(rawid) + "/";     
    } else {
      folderName = "PXForward/HalfCylinder_" + std::to_string(shell) + "/PXRing_" +  layer + "/" + std::to_string(rawid) + "/";     
    }
  }
  
  void bookHistos(unsigned int rawid) {
    std::cout << "Booking histos for:" << rawid << std::endl;
    TString tag = TString(std::to_string(rawid));
    //clusters
    hclscharge = new TH2D("compcharge" + tag, "Cluster charge;gpu;cpu", 200, 0, 200e3, 200, 0, 200e3);      
    hclsSizex = new TH2D("compclswx" + tag, "Cluster size X;gpu;cpu", 30, 0, 30, 30, 0, 30);
    hclsSizey = new TH2D("compclswy" + tag, "Cluster size Y;gpu;cpu", 30, 0, 30, 30, 0, 30);
    hncls = new TH2D("compnclusters" + tag, "#clusters;gpu;cpu", 51, -0.5, 50.5, 51, -0.5, 50.5);
  }

};

class DetAnalyzer {
public:
  
  DetAnalyzer(bool plotdethisto) {
    fillHistosFordet_ = plotdethisto;
    if(!fillHistosFordet_)  std::cout << "Per det Histos will not be booked\n";
  }
  ~DetAnalyzer() {}
  
  void getTree(TString fname, bool isgpu) {
    TFile* fin = TFile::Open(fname);
    if(isgpu) {
      gpuTree_ = dynamic_cast<TTree*>(fin->Get("sipixeldigidetId/detinfo"));
      setgpubranches();
    } else {
      cpuTree_ = dynamic_cast<TTree*>(fin->Get("sipixeldigidetId/detinfo"));
      setcpubranches();
    }
  }
  
  void setgpubranches() {
    if(!gpuTree_)   return;
    gpudetmap_ = new std::map<unsigned int, DetInfo>();
    gpuTree_->SetBranchAddress("digiInfo", &gpudetmap_); 
  }
  
  void setcpubranches() {
    if(!cpuTree_)   return;
    cpudetmap_ = new std::map<unsigned int, DetInfo>();
    cpuTree_->SetBranchAddress("digiInfo", &cpudetmap_); 
  }
  
  //Function to compare 2 digis
  bool compareCluster(const DetInfo& ginfo, const DetInfo& cinfo, const unsigned int gi, const unsigned int ci) {
    if(ginfo.cluster_minPixelRow[gi]   == cinfo.cluster_minPixelRow[ci]
       && ginfo.cluster_maxPixelRow[gi] == cinfo.cluster_maxPixelRow[ci]
       && ginfo.cluster_minPixelCol[gi] == cinfo.cluster_minPixelCol[ci]
       && ginfo.cluster_maxPixelCol[gi] == cinfo.cluster_maxPixelCol[ci] 
       )  return true;
    return false;
  }

  void printcluster(const DetInfo& dinfo) {
    std::cout << "#Clusters=" << dinfo.ncluster << std::endl;
    for(unsigned int i = 0; i<dinfo.cluster_charge.size(); i++) {
      std::cout << dinfo.cluster_charge[i] << ":" 
		<< dinfo.cluster_sizeX[i]  << ":" 
		<< dinfo.cluster_sizeY[i]  << ":" 
		<< dinfo.cluster_x[i]  << ":" 
		<< dinfo.cluster_y[i]  << ":" 
		<< dinfo.cluster_minPixelRow[i]  << ":" 
		<< dinfo.cluster_maxPixelRow[i]  << ":" 
		<< dinfo.cluster_minPixelCol[i]  << ":" 
		<< dinfo.cluster_maxPixelCol[i]  << ":" 
		<< std::endl;
    }
  }
  
  void bookGlobalHistos() {
    //clusters
    hclscharge = new TH2D("compcharge", "Cluster charge;gpu;cpu", 200, 0, 200e3, 200, 0, 200e3);      
    hclsSizex = new TH2D("compclswx", "Cluster size X;gpu;cpu", 30, 0, 30, 30, 0, 30);
    hclsSizey = new TH2D("compclswy", "Cluster size Y;gpu;cpu", 30, 0, 30, 30, 0, 30);
    hncls = new TH2D("compnclusters", "#clusters;gpu;cpu", 51, -0.5, 50.5, 51, -0.5, 50.5);
    hncpumatched = new TH1D("ncpuclusmatched", "#legacy clusters matched to 1 GPU cluster", 11, -0.5, 10.5);
    hngpumatched = new TH1D("ngpuclusmatched", "#GPU clusters matched to 1 legacy cluster", 11, -0.5, 10.5);
    hxres = new TH1D("xres", "X residual; Cluster_X_{gpu} - Cluster_X_{legacy} [#mu m]", 100, -10., 10.);
    hyres = new TH1D("yres", "Y residual; Cluster_Y_{gpu} - Cluster_Y_{legacy} [#mu m]", 100, -10., 10.);
    hchres = new TH1D("chargeres", "Charge residual; Cluster_charge_{gpu} - Cluster_charge_{legacy}", 21, -10.5, 10.5);
    //hchres = new TH1D("abschargeres", "Absolute charge residual; Abs(Cluster_charge_{gpu} - Cluster_charge_{legacy})", 31, -0.5, 30.5);
  }
   
  void fillDetHistos(DetInfo& gpuinfo, DetInfo& cpuinfo, unsigned int rawid, unsigned int jentry) {
    //Fill number of objects  histos
    hncls->Fill(gpuinfo.ncluster, cpuinfo.ncluster);
    if(fillHistosFordet_) { 
      histomap_[rawid].hncls->Fill(gpuinfo.ncluster, cpuinfo.ncluster);
    }
    /*
    std::cout << "GPU cluster dump\n";
    printcluster(gpuinfo);
    std::cout << "Legacy cluster dump\n";
    printcluster(cpuinfo);
    std::cout << "*******DUM END**********\n";
    */
    std::vector<int> nmatchedgpu(cpuinfo.ncluster,0);
    for(unsigned int gi = 0; gi < gpuinfo.ncluster; gi++) {
      auto g_charge = gpuinfo.cluster_charge[gi];
      auto g_sizeX = gpuinfo.cluster_sizeX[gi];
      auto g_sizeY = gpuinfo.cluster_sizeY[gi];
      auto g_x = gpuinfo.cluster_x[gi];
      auto g_y = gpuinfo.cluster_y[gi];
      bool matched = false;
      int nmatchedcpu = 0;
      for(unsigned int ci = 0; ci < cpuinfo.ncluster; ci++) {
        auto c_charge = cpuinfo.cluster_charge[ci];
 	auto c_sizeX = cpuinfo.cluster_sizeX[ci];
 	auto c_sizeY = cpuinfo.cluster_sizeY[ci];
 	auto c_x = cpuinfo.cluster_x[ci];
 	auto c_y = cpuinfo.cluster_y[ci];
	//define some matching condition
	if(compareCluster(gpuinfo, cpuinfo, gi, ci)) {
	  if(fillHistosFordet_) {
	    histomap_[rawid].hclscharge->Fill(g_charge, c_charge);
	    histomap_[rawid].hclsSizex->Fill(g_sizeX, c_sizeX);
	    histomap_[rawid].hclsSizey->Fill(g_sizeY, c_sizeY);
	  }
	  hclscharge->Fill(g_charge, c_charge);
	  hclsSizex->Fill(g_sizeX, c_sizeX);
	  hclsSizey->Fill(g_sizeY, c_sizeY);
	  //std::cout << "xresU=" << 10000.*(g_x - c_x) << "\t yres:" << 10000.*(g_y - c_y) << std::endl;
	  hxres->Fill(10000.*(g_x - c_x));
	  hyres->Fill(10000.*(g_y - c_y));
	  hchres->Fill(g_charge - c_charge);
	  //if(std::abs(g_charge - c_charge) != 0) 
	  //  std::cout << "Charge from GPU=" << g_charge << "\t from legacy:" << c_charge << std::endl;
          //
          nmatchedcpu++;
	  nmatchedgpu[ci]++;
	}//comp

      }//loop over cpu
      hncpumatched->Fill(nmatchedcpu);
    }
    
    for(auto& c : nmatchedgpu) hngpumatched->Fill(c);
        
  }//end func

  void Loop() {
    bookGlobalHistos();
    Long64_t cpuentries = cpuTree_->GetEntries();
    Long64_t gpuentries = gpuTree_->GetEntries();
    Long64_t toprocess = cpuentries; 
    if (cpuentries != gpuentries) {
      std::cout << "#Entries in the 2 trees are not same\n";
      toprocess = std::min(cpuentries, gpuentries);
    }
    Long64_t nb;
    std::cout << "#Entries to process:" << toprocess << std::endl;

    for(Long64_t jentry=0; jentry<toprocess;jentry++) {
    //for(Long64_t jentry=0; jentry<10;jentry++) {
      nb = cpuTree_->GetEntry(jentry);
      nb = gpuTree_->GetEntry(jentry);
      std::cout << "Processed entries:" << jentry << std::endl;
      //Clusters
      for(auto& detcls : *gpudetmap_) {
        auto rawid = detcls.first;
        if(cpudetmap_->find(rawid) == cpudetmap_->end())  {
          std::cout << "Detid " << rawid << " not found in CPU Cluster map!!\n";
          continue;
        }  

        auto gpuinfo = detcls.second;
        auto cpuinfo = cpudetmap_->at(rawid);

        //clusters
        if(fillHistosFordet_ && histomap_.find(rawid) == histomap_.end()) {
          DetHisto hd;
          hd.isbarrel = detcls.second.isbarrel;
          hd.layer = detcls.second.layer;//layer or ring
          hd.shell = detcls.second.shell;
          hd.setFoldername(rawid);
          hd.bookHistos(rawid);
          histomap_.insert({rawid, hd});
        }
        fillDetHistos(gpuinfo, cpuinfo, rawid, jentry);

      }//end loop over gpu clusters

 
    }//end event loop
    std::cout << "Loop end\n";
  }


  void writeHistos() {
    TFile* fout = TFile::Open("clusterComp.root", "recreate");
    fout->mkdir("cluster");
    fout->cd("cluster");
    hclscharge->Write();
    hclsSizex->Write();
    hclsSizey->Write();
    hncls->Write();
    hxres->Write();
    hyres->Write();
    hncpumatched->Write();
    hngpumatched->Write();
    hchres->Write();
    fout->cd();
    if(fillHistosFordet_) {
      for(auto& hd : histomap_) {
	TString fn(hd.second.folderName.c_str());
	fout->mkdir(fn);
	fout->cd(fn);
	hd.second.hclscharge->Write();
	hd.second.hclsSizex->Write();
	hd.second.hclsSizey->Write();
	hd.second.hncls->Write();
      }
    }
    fout->Save();
    fout->Close();
  }

private:
  TTree* gpuTree_;
  TTree* cpuTree_;
  std::map<unsigned int, DetInfo>* gpudetmap_;
  std::map<unsigned int, DetInfo>* cpudetmap_;
  
  bool fillHistosFordet_;

  TH2D* hclscharge;
  TH2D* hclsSizex;
  TH2D* hclsSizey; 
  TH2D* hncls;
  TH1D* hxres;
  TH1D* hyres;
  TH1D* hncpumatched;
  TH1D* hngpumatched;
  TH1D* hchres; 

  std::map<unsigned int, DetHisto> histomap_;
};

int main() {
  DetAnalyzer dt(0);
  dt.getTree("input/gpu_input.root", 1);
  dt.getTree("input/legacy_input.root", 0);
  dt.Loop();
  dt.writeHistos();
}
