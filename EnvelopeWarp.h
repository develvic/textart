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

	/**
	Class that perform warping of text regarding to given skeleton paths(Bottom and Top)
	and transformation matrix. Way of transformation is controlled by few boolean parameters:
		- is letter rotated by normal to skeleton(setIsNormalRotated)
		- is Top and Bottom skeletons are symmetric(it use more lightware algorithm of warping)
		- is warping Top skeleton-based(by default it is Bottom-based), see explanation below

	Warping algorithm steps:
		- transform text glypths to SkPaths(using specified SkTypeface)
		- warp text-path by base skeleton, text is placed along the path and morphed.
			If IsNormalRotated=TRUE then text is rotated to be perpendicular to base path. 
			If FALSE then text remains vertical
		- in non-Symmetric mode it omit the normals at letter bounds(left and right) points from base path to opposite one.
			And calculate the raltion between paths lengths, it will be used to preserve vertical length
			In Symmetric mode this part is skipped
		- warp text-path by opposite skeleton path
		- generate warped-path as weighted average between too warped paths(Top and Bottom)

	Warping howto:
		- create skeleton SkPath(Bottom and Top) and traansformation matrix
		- create EnvelopeWarp and pass Bottom skeleton and matrix as parameters to ctor
		- set Top skeleton calling EnvelopeWarp::setTopSkeleton(path)
		- set warp params by calling appropriate setters(by default,NO normal rotation, Bottom-based and not-symmetric) 
		- warp text calling EnvelopeWarp::warp() with text and font passed
	*/
	class EnvelopeWarp
	{
	public:
		/**
			Construct warping object

			@param	bSkeleton	Bottom skeleton path
			@param	matrix		Transformation matrix that will be applied during warping
		*/
		EnvelopeWarp(const SkPath& bSkeleton, const SkMatrix& matrix);

		/**
			Warp given text regarding to constructed EnvelopeWarp object

			@param	text		Text that will be wrapped
			@param	typeface	Typeface(font) that will be used for text displaing
			@return				SkPath object that containg warped text converted to glyphs
		*/
		SkPath warp(const std::string& text, SkTypeface* typeface);

		/**
			Set Top skeleton path

			@param	tSkeleton	Top skeleton path
		*/
		void setTopSkeleton(const SkPath& tSkeleton);

		/**
			Set flag that responsable for rotating text to be perpendicular to base path

			@param	isRotated	If True passed that text will be rotated by normal
		*/
		void setIsNormalRotated(bool isRotated);

		/**
			Switch EnvelopeWarp to symmetric mode. It is more prefferable
			for paths that are symmetric

			@param	isSymmetric	If True passed then Symmetric mmode is on
		*/
		void setIsSymmetric(bool isSymmetric);

		/**
			Specify EnvelopeWarp to use Top skeleton as base path

			@param	isTopBased	If True passed then Top skeleton used as base
		*/
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
