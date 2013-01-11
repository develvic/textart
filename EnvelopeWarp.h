#ifndef _TEXT_ART_ENVELOPE_WARP_H_
#define _TEXT_ART_ENVELOPE_WARP_H_

#include "SkMatrix.h"
#include "SkPath.h"
#include "SkPathMeasure.h"
#include "SkPathCrossing.h"
#include "SkPaint.h"
#include "SkTypeface.h"

#include <string>
#include <vector>

namespace TextArt
{
	enum XWeightingMode
	{
		XWeightingMode_Linearly			= 0x01,
		XWeightingMode_Interpolating	= 0x02
	};

	class EnvelopeWarp
	{
	public:
		EnvelopeWarp(const SkPath& skeleton, const SkMatrix& matrix);

		SkPath warp(const std::string& text, SkTypeface* typeface);
		SkPath warp(const SkPath& path, const SkMatrix& matrix);

		void setTopSkeleton(const SkPath& skeleton);
		void setIsNormalRotated(bool isRotated);
		void setIsSymmetric(bool isSymmetric);
		void setIsTopBased(bool isTopBased);

	public:
		void morphpoints(SkPoint dst[], const SkPoint src[], int count, SkPathMeasure& meas, const SkMatrix& matrix);
		void morphpath(SkPath* dst, const SkPath& src, SkPathMeasure& meas, const SkMatrix& matrix);
		void weight(const SkPoint src[], const SkPoint tSrc[], const SkPoint bSrc[], int count, const SkRect& srcBounds, SkPoint dst[]);
		void weight(const SkPath& path, const SkPath& top, const SkPath& bottom, SkPath* dst);

		void morph(SkPath& bSkeleton, SkPathMeasure& bMeasure, SkPathCrossing& bCrossing,
				   SkPath& tSkeleton, SkPathMeasure& tMeasure, SkPathCrossing& tCrossing,
				   SkPath& glypthPath, SkPathMeasure& lineMeasure, SkMatrix& scaleMatrix,
				   SkScalar xpos, SkScalar hBOffset, SkScalar hTOffset, SkPath& warpedPath);

		bool getK(const SkRect& glypthBound, SkPathCrossing& bCrossing, SkPathCrossing& tCrossing, 
					SkPathMeasure& basePathMeasure, const SkMatrix& compositeMatrix, 
					SkScalar& k1, SkScalar& hBOffset, SkScalar& hTOffset);

		SkPath tSkeleton_;
		SkPath bSkeleton_;
		SkMatrix matrix_;

		SkRect boundsRect_;
		SkPath bWarped_;
		SkPath tWarped_;

		bool isNormalRotated_;
		bool isSymmetric_;
		bool isTopBased_;

		SkScalar k1_;
		bool isTop;
		unsigned int xWeightingMode_;

		SkPath normal;
		SkPath tFlattern;
		std::vector<SkPoint> intersections;
	};
}

#endif
