#ifndef DETINFO_h
#define DETINFO_h
#include<vector>

struct DetInfo {
  float x;
  float y;
  float z;
  float eta;

  unsigned int ndigi;
  std::vector<unsigned short> digi_adc;
  std::vector<int> digi_row;
  std::vector<int> digi_col;

  unsigned int ncluster;
  std::vector<int> cluster_charge;
  std::vector<int> cluster_sizeX;
  std::vector<int> cluster_sizeY;
  std::vector<float> cluster_x;
  std::vector<float> cluster_y;
  std::vector<int> cluster_minPixelRow;
  std::vector<int> cluster_maxPixelRow;
  std::vector<int> cluster_minPixelCol;
  std::vector<int> cluster_maxPixelCol;

  unsigned int nrechit;
  std::vector<float> rechit_x;
  std::vector<float> rechit_y;
  std::vector<float> rechit_xerror;
  std::vector<float> rechit_yerror;

  bool isbarrel;
  int layer;//layer or disk
  int shell;
};

#endif
