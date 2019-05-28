#ifndef SimDataFormats_GeneratorProducts_LHEWeightProduct_h
#define SimDataFormats_GeneratorProducts_LHEWeightProduct_h

#include <memory>
#include <vector>
#include <string>

#include "SimDataFormats/GeneratorProducts/interface/LesHouches.h"
#include "SimDataFormats/GeneratorProducts/interface/WeightsInfo.h"

class LHEWeightProduct {
    public:
    typedef weightsContainer std::vector<std::vector<double>>;

	LHEWeightProduct() {}
	LHEWeightProduct& operator=(LHEWeightProduct&& other) {
	  weights_ = std::move(other.weights_);
	  originalXWGTUP_ = std::move(other.originalXWGTUP_);
	  return *this;
	}
	~LHEWeightProduct() {}

	void addWeight(double weight, size_t setEntry, size_t weightNum) { 	  
        weights_.at(setEntry).insert(weight, weightNum);
	}
	double originalXWGTUP() const { return originalXWGTUP_; }
	const std::vector<WGT>& weights() const { return weights_; }

    private:
	weightsContainer                       weights_;
    std::map<std::string, double> weightsMap_;
	double                          originalXWGTUP_;
};

#endif // GeneratorEvent_LHEInterface_LHEWeightProduct_h

