#ifndef _TEXT_ART_ENVELOPE_WARP_H_
#define _TEXT_ART_ENVELOPE_WARP_H_

#include "SkMatrix.h"
#include "SkPath.h"
#include "SkPathMeasure.h"
#include "SkPaint.h"
#include "SkTypeFace.h"

#include <string>

namespace TextArt
{
	class EnvelopeWarp
	{
	public:
		EnvelopeWarp(const SkPath& skeleton, const SkMatrix& matrix);

		SkPath warp(const std::string& text, SkTypeface* typeface);
		SkPath warp(const SkPath& path, const SkMatrix& matrix);

		void setTopSkeleton(const SkPath& skeleton);

		const SkRect& getBounds() const;

	public:
		static void morphpoints(SkPoint dst[], const SkPoint src[], int count, SkPathMeasure& meas, const SkMatrix& matrix);
		static void morphpath(SkPath* dst, const SkPath& src, SkPathMeasure& meas, const SkMatrix& matrix);
		void weight(const SkPoint src[], const SkPoint tSrc[], const SkPoint bSrc[], int count, SkPoint dst[]);
		void weight(const SkPath& path, const SkPath& top, const SkPath& bottom, SkPath* dst);

		SkPath tSkeleton_;
		SkPath bSkeleton_;
		SkMatrix matrix_;

		SkRect boundsRect_;
		SkScalar height_;
		SkPath bWarped_;
		SkPath tWarped_;
	};
}

#endif
