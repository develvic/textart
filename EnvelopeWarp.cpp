	#include "EnvelopeWarp.h"

#include "SkTextToPathIter.h"
#include "SkTypeface.h"
#include "SkGeometry.h"

#include <vector>

///****************************************************************************************************

TextArt::EnvelopeWarp::EnvelopeWarp(const SkPath& skeleton, const SkMatrix& matrix)
	: bSkeleton_(skeleton)
	, matrix_(matrix)
	, isNormalRotated_(false)
	, xWeightingMode_(XWeightingMode_Linearly | XWeightingMode_Interpolating)
	, k1_(1.0)
	, isSymmetric_(false)
	, isTopBased_(false)
{
}

void TextArt::EnvelopeWarp::setTopSkeleton(const SkPath& skeleton)
{
	tSkeleton_ = skeleton;

	SkRect bBounds = bSkeleton_.getBounds();
	const SkRect& tBounds = tSkeleton_.getBounds();
}

void TextArt::EnvelopeWarp::setIsNormalRotated(bool isRotated)
{
	isNormalRotated_ = isRotated;
}

void TextArt::EnvelopeWarp::setIsSymmetric(bool isSymmetric)
{
	isSymmetric_ = isSymmetric;
}

void TextArt::EnvelopeWarp::setIsTopBased(bool isTopBased)
{
	isTopBased_ = isTopBased;
}

SkPath TextArt::EnvelopeWarp::warp(const std::string& text, SkTypeface* typeface)
{
	SkPath				warpedPath;

	if (text.empty())
		return warpedPath;

	//prepare paint
	SkPaint paint;
	paint.setTextSize(SkIntToScalar(64));
	paint.setTypeface(typeface);
	paint.setTextAlign(SkPaint::kCenter_Align);

	//measure Bottom path to center text on it
	SkPathMeasure   bMeasure(bSkeleton_, false);
	SkScalar        hBOffset = 0;

    if (paint.getTextAlign() != SkPaint::kLeft_Align)
	{
        SkScalar pathLen = bMeasure.getLength();
        if (paint.getTextAlign() == SkPaint::kCenter_Align)
		{
            pathLen = SkScalarHalf(pathLen);
        }
        hBOffset += pathLen;
    }

	//get text boundaries on normal(non-warped) state
	{
		SkMatrix scaleMartix;
		scaleMartix.setIdentity();

		SkTextToPathIter	iter(text.c_str(), text.size(), paint, true);
		const SkPath*   glypthPath;
		SkScalar        xpos;

		SkScalar        scale = iter.getPathScale();
		scaleMartix.setScale(scale, scale);

		while (iter.next(&glypthPath, &xpos))
		{
			if (glypthPath)
			{
				//prepare resulting transformatiom Matrix
				SkMatrix	compositeMatrix(scaleMartix);
				compositeMatrix.postTranslate(xpos + hBOffset, 0);
				
				SkPath p;
				(*glypthPath).transform(compositeMatrix, &p);
				//get normal(without any warps) text boundaries
				boundsRect_.join( p.getBounds() );
			}
		}
	}

	//center text on Top skeleton
	SkPathMeasure   tMeasure(tSkeleton_, false);
	SkScalar        hTOffset = 0;	
	{
		if (paint.getTextAlign() != SkPaint::kLeft_Align)
		{
			SkScalar pathLen = tMeasure.getLength();
			if (paint.getTextAlign() == SkPaint::kCenter_Align)
			{
				pathLen = SkScalarHalf(pathLen);
			}
			hTOffset += pathLen;
		}
	}		
	
	//warp text on Bottom and Top skeletons
	{
		SkTextToPathIter	iter(text.c_str(), text.size(), paint, true);
		SkScalar        xpos;

		SkMatrix        scaleMartix;
		SkScalar        scale = iter.getPathScale();
		scaleMartix.setScale(scale, scale);

		SkPath line;
		line.lineTo(SkIntToScalar(100), SkIntToScalar(0));
		SkPathMeasure   lineMeasure(line, false);

		SkPathCrossing bCrossing(bSkeleton_);
		SkPathCrossing tCrossing(tSkeleton_);

		const SkPath*   glypthPathOrig;
		while (iter.next(&glypthPathOrig, &xpos))
		{
			if (glypthPathOrig)
			{
				SkPath glypthPath;
				SkRect glypthBound;
				glypthBound = (*glypthPathOrig).getBounds();
				glypthPathOrig->offset(-glypthBound.fLeft, 0, &glypthPath);

				morph(bSkeleton_, bMeasure, bCrossing,
						tSkeleton_, tMeasure, tCrossing,
						glypthPath, lineMeasure, scaleMartix,
						xpos, hBOffset, hTOffset, warpedPath);

			}
		}
	}
	
	return warpedPath;
}

void TextArt::EnvelopeWarp::morph(SkPath& bSkeleton, SkPathMeasure& bMeasure, SkPathCrossing& bCrossing,
								SkPath& tSkeleton, SkPathMeasure& tMeasure, SkPathCrossing& tCrossing,
								SkPath& glypthPath, SkPathMeasure& lineMeasure, SkMatrix& scaleMatrix,
								SkScalar xpos, SkScalar hBOffset, SkScalar hTOffset, SkPath& warpedPath)
{
	SkRect glypthBound;
	glypthBound = glypthPath.getBounds();

	SkScalar k1 = 1.0;

	if (!isSymmetric_)
	{	
		SkScalar hBOffsetTmp = 0.0;
		SkScalar hTOffsetTmp = 0.0;

		if (isTopBased_)
		{
			glypthBound.fTop = 0;
			glypthBound.fBottom = -boundsRect_.height();

			SkMatrix	compositeTMatrix(scaleMatrix);
			compositeTMatrix.postTranslate(xpos + hTOffset, 0);
			compositeTMatrix.postConcat(matrix_);

			if ( getK(glypthBound, bCrossing, tCrossing, tMeasure, compositeTMatrix, k1, hBOffsetTmp, hTOffsetTmp) )
			{
				k1 = 1/k1;

				hBOffset = hBOffsetTmp;
				hTOffset = xpos + hTOffset;
			}
		}
		else
		{
			glypthBound.fTop = -boundsRect_.height();
			glypthBound.fBottom = 0;

			SkMatrix	compositeBMatrix(scaleMatrix);
			compositeBMatrix.postTranslate(xpos + hBOffset, 0);
			compositeBMatrix.postConcat(matrix_);

			if ( getK(glypthBound, bCrossing, tCrossing, bMeasure, compositeBMatrix, k1, hBOffsetTmp, hTOffsetTmp) )
			{
				//use distance to Left on Top Skeleton for positioning Top glypthn
				hTOffset = hTOffsetTmp;
				hBOffset = xpos + hBOffset;
			}
		}
	}
	else
	{
		hBOffset = xpos + hBOffset;
		hTOffset = hBOffset;
	}
				
	SkMatrix	compositeBMatrix(scaleMatrix);
	compositeBMatrix.postTranslate(hBOffset, 0);
	compositeBMatrix.postConcat(matrix_);

	//warp Glypth by bottom line
	k1_ = isTopBased_ ? k1 : SkIntToScalar(1);
	isTop = false;
	SkPath bWarped;
	morphpath(&bWarped, glypthPath, bMeasure, compositeBMatrix);
	bWarped_.addPath(bWarped);

	if (!tSkeleton_.isEmpty())
	{
		SkMatrix	compositeTMatrix(scaleMatrix);
		compositeTMatrix.postTranslate(hTOffset, 0);
		compositeTMatrix.postConcat(matrix_);

		//warp Glypth by top line
		k1_ = !isTopBased_ ? k1 : SkIntToScalar(1);
		isTop = true;
		SkPath tWarped;
		morphpath(&tWarped, glypthPath, tMeasure, compositeTMatrix);
		tWarped_.addPath(tWarped);

		//convert Glypth to Path to allow weighting
		SkPath lineMorphed;
		morphpath(&lineMorphed, glypthPath, lineMeasure, scaleMatrix);

		weight(lineMorphed, tWarped, bWarped, &warpedPath);
	}
	else
		warpedPath.addPath(bWarped_);
}


bool TextArt::EnvelopeWarp::getK(const SkRect& glypthBound, SkPathCrossing& bCrossing, SkPathCrossing& tCrossing, 
								SkPathMeasure& basePathMeasure, const SkMatrix& compositeMatrix, 
								SkScalar& k1, SkScalar& hBOffset, SkScalar& hTOffset)
{
		SkScalar bCut = isTopBased_ ? SkIntToScalar(-3) : SkIntToScalar(+3);
		SkPoint centerX[4], tCenterX[4];
		//left normal
		centerX[0].set(glypthBound.fLeft, glypthBound.fBottom + bCut);
		centerX[1].set(glypthBound.fLeft, glypthBound.fTop);
		//right normal
		centerX[2].set(glypthBound.fRight, glypthBound.fBottom + bCut);
		centerX[3].set(glypthBound.fRight, glypthBound.fTop);
						
		//morph normal point by Bottom skeleton
		isTop = isTopBased_;
		k1_ = 1.0;
		morphpoints(tCenterX, centerX, 4, basePathMeasure, compositeMatrix);
		normal.moveTo(tCenterX[0]);
		normal.lineTo(tCenterX[1]);
		normal.moveTo(tCenterX[2]);
		normal.lineTo(tCenterX[3]);

		SkPoint bCrossR, bCrossL, tCrossL, tCrossR;
		SkScalar bCrossDistR, bCrossDistL, tCrossDistL, tCrossDistR;

		//get intersection between Normals and Skeletons
		if (tCrossing.rayCrossing(tCenterX, &tCrossDistL, &tCrossL) &&
			tCrossing.rayCrossing(&tCenterX[2], &tCrossDistR, &tCrossR) &&
			bCrossing.rayCrossing(&tCenterX[0], &bCrossDistL, &bCrossL) &&
			bCrossing.rayCrossing(&tCenterX[2], &bCrossDistR, &bCrossR) )
		{
			intersections.push_back(tCrossL);
			intersections.push_back(tCrossR);
			intersections.push_back(bCrossL);
			intersections.push_back(bCrossR);

			//calulate linear coeffiction for stretching Top glypth
			k1 = (tCrossDistR - tCrossDistL) / (bCrossDistR - bCrossDistL);

			//use distance to Left on Top Skeleton for positioning Top glypthn
			hBOffset = bCrossDistL;
			hTOffset = tCrossDistL;
			return true;
		}

		return false;
}

void TextArt::EnvelopeWarp::morphpoints(SkPoint dst[], const SkPoint src[], int count,
                        SkPathMeasure& meas, const SkMatrix& matrix)
{
    SkMatrix::MapXYProc proc = matrix.getMapXYProc();

    for (int i = 0; i < count; i++)
	{
        SkPoint pos;
        SkVector tangent;

		SkPoint iSrc = src[i];
		iSrc.fX = k1_ * iSrc.fX;

        proc(matrix, iSrc.fX, iSrc.fY, &pos);

		SkScalar sx = pos.fX;
        SkScalar sy = pos.fY;

		if (xWeightingMode_ & XWeightingMode_Linearly)
		{	//in Linearly mode adjust Top text by TopLength/BottomLength relation 
			if (isTop)
			{
				//move text below the Top skeleton
				sy -= boundsRect_.fTop;
			}
		}

        if (!meas.getPosTan(sx, &pos, &tangent))
		{
            // set to 0 if the measure failed, so that we just set dst == pos
            tangent.set(0, 0);
        }

        /*  This is the old way (that explains our approach but is way too slow
            SkMatrix    matrix;
            SkPoint     pt;

            pt.set(sx, sy);
            matrix.setSinCos(tangent.fY, tangent.fX);
            matrix.preTranslate(-sx, 0);
            matrix.postTranslate(pos.fX, pos.fY);
            matrix.mapPoints(&dst[i], &pt, 1);
        */

		if (isNormalRotated_)
		{
			dst[i].set(pos.fX - SkScalarMul(tangent.fY, sy),
                   pos.fY + SkScalarMul(tangent.fX, sy));
		}
		else
		{
			dst[i].set(pos.fX,
				pos.fY + sy);
		}
    }
}

/*  TODO

    Need differentially more subdivisions when the follow-path is curvy. Not sure how to
    determine that, but we need it. I guess a cheap answer is let the caller tell us,
    but that seems like a cop-out. Another answer is to get Rob Johnson to figure it out.
*/
void TextArt::EnvelopeWarp::morphpath(SkPath* dst, const SkPath& src, SkPathMeasure& meas,
                      const SkMatrix& matrix)
{
    SkPath::Iter    iter(src, false);
    SkPoint         srcP[4], dstP[3];
    SkPath::Verb    verb;

    while ((verb = iter.next(srcP)) != SkPath::kDone_Verb)
	{
        switch (verb)
		{
            case SkPath::kMove_Verb:
                morphpoints(dstP, srcP, 1, meas, matrix);
                dst->moveTo(dstP[0]);
                break;
            case SkPath::kLine_Verb:
/*
                morphpoints(dstP, &srcP[1], 1, meas, matrix);
                dst->lineTo(dstP[0]);
                break;
*/

                // turn lines into quads to look bendy
                srcP[0].fX = SkScalarAve(srcP[0].fX, srcP[1].fX);
                srcP[0].fY = SkScalarAve(srcP[0].fY, srcP[1].fY);
                morphpoints(dstP, srcP, 2, meas, matrix);
                dst->quadTo(dstP[0], dstP[1]);
                break;
            case SkPath::kQuad_Verb:
                morphpoints(dstP, &srcP[1], 2, meas, matrix);
                dst->quadTo(dstP[0], dstP[1]);
                break;
            case SkPath::kCubic_Verb:
                morphpoints(dstP, &srcP[1], 3, meas, matrix);
                dst->cubicTo(dstP[0], dstP[1], dstP[2]);
                break;
            case SkPath::kClose_Verb:
                dst->close();
                break;
            default:
                SkDEBUGFAIL("unknown verb");
                break;
        }
    }
}

void TextArt::EnvelopeWarp::weight(const SkPoint src[], const SkPoint tSrc[], const SkPoint bSrc[], int count, const SkRect& srcBounds, SkPoint dst[])
{
	for (int i = 0; i < count; i++)
	{
		SkScalar origY = src[i].fY;
		SkScalar origX = src[i].fX;

		dst[i].fX = bSrc[i].fX;
		dst[i].fY = origY;
		
		if (srcBounds.fBottom > 0)
			origY -= srcBounds.fBottom;

		
		SkScalar k1 = SkScalarAbs( SkScalarDiv(origY, srcBounds.height()) );
		dst[i].fY = SkScalarInterp(bSrc[i].fY, tSrc[i].fY, k1);

		if (xWeightingMode_ & XWeightingMode_Interpolating)
		{	//in Interpolating mode calculate X as interpolation beween Top and Bottom
			dst[i].fX = SkScalarInterp(bSrc[i].fX, tSrc[i].fX, k1);
		}
	}
}

void TextArt::EnvelopeWarp::weight(const SkPath& path, const SkPath& top, const SkPath& bottom, SkPath* dst)
{
	SkPath::Iter    iterB(bottom, false);
	SkPath::Iter    iterT(top, false);
	SkPath::Iter    iter(path, false);

    SkPoint         tSrc[4], bSrc[4], src[4], dstP[4];
    SkPath::Verb    verbT, verbB, verb;

	const SkRect&	boundsSrc = path.getBounds();

    while (((verbT = iterT.next(tSrc)) != SkPath::kDone_Verb)	&& 
			((verbB = iterB.next(bSrc)) != SkPath::kDone_Verb)	&&
			((verb = iter.next(src)) != SkPath::kDone_Verb) )
	{
		SkASSERT(verbT==verbB);
		SkASSERT(verbT==verb);

        switch (verbT)
		{
            case SkPath::kMove_Verb:
                weight(&src[0], &tSrc[0], &bSrc[0], 1, boundsSrc, dstP);
                dst->moveTo(dstP[0]);
                break;
            case SkPath::kLine_Verb:
                weight(&src[1], &tSrc[1], &bSrc[1], 1, boundsSrc, dstP);
                dst->lineTo(dstP[0]);
                break;
            case SkPath::kQuad_Verb:
				weight(&src[1], &tSrc[1], &bSrc[1], 2, boundsSrc, dstP);
                dst->quadTo(dstP[0], dstP[1]);
                break;
            case SkPath::kCubic_Verb:
				weight(&src[1], &tSrc[1], &bSrc[1], 3, boundsSrc, dstP);
                dst->cubicTo(dstP[0], dstP[1], dstP[2]);
                break;
            case SkPath::kClose_Verb:
                dst->close();
                break;
            default:
                SkDEBUGFAIL("unknown verb");
                break;
        }
    }
}