#ifndef _TEXT_ART_ENVELOPE_WARP_H_
#define _TEXT_ART_ENVELOPE_WARP_H_

#include "SkMatrix.h"
#include "SkPath.h"
#include "SkPathMeasure.h"
#include "SkPaint.h"
#include "SkTypeface.h"

#include <string>

namespace TextArt
{
	class EnvelopeWarp
	{
	public:
		EnvelopeWarp(const SkPath& skeleton, const SkMatrix& matrix);

		SkPath warp(const std::string& text, SkTypeface* typeface);
		SkPath warp(const SkPath& path, const SkMatrix& matrix);

	private:
		static void morphpoints(SkPoint dst[], const SkPoint src[], int count, SkPathMeasure& meas, const SkMatrix& matrix);
		static void morphpath(SkPath* dst, const SkPath& src, SkPathMeasure& meas, const SkMatrix& matrix);

		SkPath skeletonPath_;
		SkMatrix matrix_;
	};
}

#endif
