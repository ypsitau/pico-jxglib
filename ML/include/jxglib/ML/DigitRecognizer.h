//=============================================================================
// DigitRecognizer.h
//=============================================================================
#ifndef JXGLIB_ML_DIGIT_RECOGNIZER_H
#define JXGLIB_ML_DIGIT_RECOGNIZER_H
#include "jxglib/ML/TFLiteRunner.h"

namespace jxglib::ML {

class DigitRecognizer : public TFLiteRunner<9000, 8> {
public:
	DigitRecognizer();
	~DigitRecognizer() {}
public:
	int Recognize(const uint8_t* imageData, float* pConfidence = nullptr);
public:
	// virtual function of TFLiteRunner
	virtual void AddOpResolver(OpResolver& opResolver) override;
};

}

#endif
