//=============================================================================
// TfLiteModelRunner.cpp
//=============================================================================
#include "jxglib/ML/TfLiteModelRunner.h"

namespace jxglib::ML {

size_t CountElements(const TfLiteTensor* tensor)
{
	size_t count = 1;
	for (int i = 0; i < tensor->dims->size; i++) {
		count *= tensor->dims->data[i];
	}
	return count;
}

}
