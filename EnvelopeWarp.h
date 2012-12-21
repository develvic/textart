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
	enum XWeightingMode
	{
		XWeightingMode_Linearly,
		XWeightingMode_Interpolating
	};

	class EnvelopeWarp
	{
	public:
		EnvelopeWarp(const SkPath& skeleton, const SkMatrix& matrix);

		SkPath warp(const std::string& text, SkTypeface* typeface);
		SkPath warp(const SkPath& path, const SkMatrix& matrix);

		void setTopSkeleton(const SkPath& skeleton);
		void setIsNormalRotated(bool isRotated);

		const SkRect& getBounds() const;

	public:
		void morphpoints(SkPoint dst[], const SkPoint src[], int count, SkPathMeasure& meas, const SkMatrix& matrix);
		void morphpath(SkPath* dst, const SkPath& src, SkPathMeasure& meas, const SkMatrix& matrix);
		void weight(const SkPoint src[], const SkPoint tSrc[], const SkPoint bSrc[], int count, const SkRect& srcBounds, SkPoint dst[]);
		void weight(const SkPath& path, const SkPath& top, const SkPath& bottom, SkPath* dst);

		SkPath tSkeleton_;
		SkPath bSkeleton_;
		SkMatrix matrix_;

		SkRect boundsRect_;
		SkPath bWarped_;
		SkPath tWarped_;

		bool isNormalRotated_;

		SkScalar k1_;
		bool isTop;
		XWeightingMode xWeightingMode_;
	};
}

#endif
