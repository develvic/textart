	#include "EnvelopeWarp.h"

#include "SkTextToPathIter.h"
#include "SkTypeface.h"
#include "SkGeometry.h"

#include <vector>

/**
Find dinstance on the path where X equals to given x
*/
SkScalar getLengthTillXImpl(SkPathMeasure& path, const SkScalar& x, const SkScalar& d0, const SkScalar& d1)
{
	SkScalar dMid = d0 + SkScalarHalf(d1 - d0);

	SkPoint midResult;

	path.getPosTan(dMid, &midResult, NULL);

	if ( SkScalarAbs(x-midResult.fX) < 0.1)
		return dMid;

	if (x > midResult.fX)
	{
		return getLengthTillXImpl(path, x, dMid, d1);
	}
	else 
	{
		return getLengthTillXImpl(path, x, d0, dMid);
	}
}

/**
 Calculate path length where specified X coordinate crpssed by the path
*/
SkScalar getLengthTillX(SkPathMeasure& path, const SkScalar& x)
{
	if (path.getLength()==0)
		return SkIntToScalar(0);

	//TODO: validate that X is crossed by the path at all

	return getLengthTillXImpl(path, x, 0, path.getLength());
}

SkScalar getK(SkPathMeasure& tPath, SkPathMeasure& bPath, SkScalar x1, SkScalar x2)
{
	//get X-coordinate of the last point of text(in normal state)
	SkScalar b = x2 - x1;
	SkScalar bl1, tl1, bl2, tl2;
	SkPoint bX1, bX2;
	//get X1-coordinate for line placed on Bottom line
	bPath.getPosTan(x1, &bX1, NULL);
	//get X2-coordinate for line placed on Bottom line
	bPath.getPosTan(x2, &bX2, NULL);
			
	//get length of Top and Bottom lines till the intersection with X-ccordinate calulated above
	bl1 = getLengthTillX(bPath, bX1.fX);
	tl1 = getLengthTillX(tPath, bX1.fX);

	bl2 = getLengthTillX(bPath, bX2.fX);
	tl2 = getLengthTillX(tPath, bX2.fX);

	SkScalar bLen = bl2 - bl1;
	SkScalar tLen = tl2 - tl1;

	SkScalar k = SkScalarDiv(tLen, bLen);

	return k;
}


SkPath flattern(const SkPath& src, int steps)
{
	double step = 1.0 / (double)steps;

	SkPath::Iter    iter(src, false);
    SkPoint         srcP[4], dstP[4];
    SkPath::Verb    verb;
	SkPoint			pt;
	SkPath			rslt;
	SkPath*			dst = &rslt;

    while ((verb = iter.next(srcP)) != SkPath::kDone_Verb)
	{
        switch (verb)
		{
            case SkPath::kMove_Verb:
                dst->moveTo(srcP[0]);
                break;
            case SkPath::kLine_Verb:
                dst->lineTo(srcP[1]);
                break;
            case SkPath::kQuad_Verb:
				pt = srcP[0];
                for(double t=step; t<1.0; t+=step)
				{
					SkEvalQuadAt(srcP, t, &pt);
					dst->lineTo(pt);
				}
				dst->lineTo(srcP[2]);
                break;
            case SkPath::kCubic_Verb:
				pt = srcP[0];
                for(double t=step; t<1.0; t+=step)
				{
					SkEvalCubicAt(srcP, t, &pt, NULL, NULL);
					dst->lineTo(pt);
				}
				dst->lineTo(srcP[3]);
                break;
            case SkPath::kClose_Verb:
                dst->close();
                break;
            default:
                SkDEBUGFAIL("unknown verb");
                break;
        }
    }
	return rslt;
}

bool lineToLine(const SkPoint l1[2], const SkPoint l2[2], SkPoint* cross)
{
	const SkPoint& l1_0 = l1[0];
	const SkPoint& l1_1 = l1[1];

	const SkPoint& l2_0 = l2[0];
	const SkPoint& l2_1 = l2[1];


	double A1 = l1_1.fY - l1_0.fY;
	double B1 = l1_0.fX - l1_1.fX;
	double C1 = A1*l1_0.fX + B1*l1_0.fY;

	double A2 = l2_1.fY - l2_0.fY;
	double B2 = l2_0.fX - l2_1.fX;
	double C2 = A2*l2_0.fX + B2*l2_0.fY;

	double den = A1*B2 - A2*B1;

	if (den == 0)
		return false;

	cross->fX = (B2*C1 - B1*C2)/den;
    cross->fY = (A1*C2 - A2*C1)/den;
	
	return true;
}

bool rayToLine(const SkPoint ray[2], const SkPoint line[2], SkPoint* cross)
{
	const SkPoint& l1_0 = ray[0];
	const SkPoint& l1_1 = ray[1];

	const SkPoint& l2_0 = line[0];
	const SkPoint& l2_1 = line[1];


	double A1 = l1_1.fY - l1_0.fY;
	double B1 = l1_0.fX - l1_1.fX;
	double C1 = A1*l1_0.fX + B1*l1_0.fY;

	double A2 = l2_1.fY - l2_0.fY;
	double B2 = l2_0.fX - l2_1.fX;
	double C2 = A2*l2_0.fX + B2*l2_0.fY;

	double den = A1*B2 - A2*B1;

	if (den == 0)
		return false;

	cross->fX = (B2*C1 - B1*C2)/den;
    cross->fY = (A1*C2 - A2*C1)/den;

	bool isActualCross = true;
	//ray start at l1_0 and continue till Infinity, cut crosses out of ray begining
	if (l1_0.fX < l1_1.fX)
		isActualCross &= (cross->fX >= l1_0.fX);
	if (l1_0.fX > l1_1.fX)
		isActualCross &= (cross->fX <= l1_0.fX);

	if (l1_0.fY < l1_1.fY)
		isActualCross &= (cross->fY >= l1_0.fY);
	if (l1_0.fY > l1_1.fY)
		isActualCross &= (cross->fY <= l1_0.fY);
	//cut crosses out of line segment
	if ( (SkMinScalar(l2_0.fX, l2_1.fX) <= cross->fX) && (cross->fX <= SkMaxScalar(l2_0.fX, l2_1.fX))  &&
		 (SkMinScalar(l2_0.fY, l2_1.fY) <= cross->fY) && (cross->fY <= SkMaxScalar(l2_0.fY, l2_1.fY))  &&
		 isActualCross
		)
	{	
		return true;
	}

	return false;
}

bool lineToPath(const SkPoint l[2], const SkPath& path, SkPoint* cross)
{
	SkPath flatternPath = flattern(path, 6);

	SkPath::Iter    iter(flatternPath, false);
    SkPoint         srcP[4];
    SkPath::Verb    verb;

    while ((verb = iter.next(srcP)) != SkPath::kDone_Verb)
	{
        switch (verb)
		{
            case SkPath::kLine_Verb:
				if (rayToLine(l, srcP, cross))
					return true;
                break;
/*
            default:
                SkDEBUGFAIL("unknown verb");
                break;
*/
        }
    }
	return false;
}
///****************************************************************************************************

TextArt::EnvelopeWarp::EnvelopeWarp(const SkPath& skeleton, const SkMatrix& matrix)
	: bSkeleton_(skeleton)
	, matrix_(matrix)
	, isNormalRotated_(false)
//	, xWeightingMode_(XWeightingMode_Linearly)
//	, xWeightingMode_(XWeightingMode_Interpolating)
	, xWeightingMode_(XWeightingMode_Linearly | XWeightingMode_Interpolating)
	, k1_(1.0)
	, isSymmetric_(false)
{
}

void TextArt::EnvelopeWarp::setTopSkeleton(const SkPath& skeleton)
{
	tSkeleton_ = skeleton;

	SkRect bBounds = bSkeleton_.getBounds();
	const SkRect& tBounds = tSkeleton_.getBounds();

	bBounds.inset(SkIntToScalar(0), SkFloatToScalar(-0.1));
//	boundsRect_ = tBounds;
//	boundsRect_.growToInclude(bBounds);
}

void TextArt::EnvelopeWarp::setIsNormalRotated(bool isRotated)
{
	isNormalRotated_ = isRotated;
}

void TextArt::EnvelopeWarp::setIsSymmetric(bool isSymmetric)
{
	isSymmetric_ = isSymmetric;
}

const SkRect& TextArt::EnvelopeWarp::getBounds() const
{
	return boundsRect_;
}

SkPath TextArt::EnvelopeWarp::warp(const SkPath& path, const SkMatrix& matrix)
{
	SkPath warpedPath;

	SkPathMeasure measure(bSkeleton_, false);

	morphpath(&warpedPath, path, measure, matrix);

	return warpedPath;
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
/*
		//calculate TopLength/BottomLength relation for Linearly mode
		if (xWeightingMode_ & XWeightingMode_Linearly)
		{
			//get X-coordinate of the last point of text(in normal state)
			SkScalar b = boundsRect_.width() + hBOffset;
			SkScalar bl, tl;
			SkPoint bp;
			//get X-coordinate of the last point of text placed on Bottom line
			bMeasure.getPosTan(b, &bp, NULL);
			
			//get length of Top and Bottom lines till the intersection with X-ccordinate calulated above
			bl = getLengthTillX(bMeasure, bp.fX);
			tl = getLengthTillX(tMeasure, bp.fX);
			//scaling coefficient
			k1_ = SkScalarDiv(tl, bl);
			//adjust Top centering offset
			hTOffset /= k1_;
		}
*/
		tFlattern = flattern(tSkeleton_, 3);

		const SkPath*   glypthPathOrig;
		while (iter.next(&glypthPathOrig, &xpos))
		{
			if (glypthPathOrig)
			{
				SkPath glypthPath;
				//prepare resulting transformatiom Matrix
				SkMatrix	compositeBMatrix(scaleMartix);
				compositeBMatrix.postTranslate(xpos + hBOffset, 0);
				compositeBMatrix.postConcat(matrix_);

				SkRect glypthBound;
				glypthBound = (*glypthPathOrig).getBounds();
				glypthPathOrig->offset(-glypthBound.fLeft, 0, &glypthPath);
				glypthBound = glypthPath.getBounds();


				if (!isSymmetric_)
				{	//compuite normal at CenterX to Bottom path
					const SkRect& tBounds = tSkeleton_.getBounds();

					SkPoint centerX[4], tCenterX[4];
					//left
					centerX[0].set(glypthBound.fLeft, glypthBound.fBottom);
					centerX[1].set(glypthBound.fLeft, -100);
					//right
					centerX[2].set(glypthBound.fRight, glypthBound.fBottom);
					centerX[3].set(glypthBound.fRight, -100);
							
					isTop = false;
					morphpoints(tCenterX, centerX, 4, bMeasure, compositeBMatrix);
					normal.moveTo(tCenterX[0]);
					normal.lineTo(tCenterX[1]);
					normal.moveTo(tCenterX[2]);
					normal.lineTo(tCenterX[3]);

					SkPoint tCrossL;
					SkScalar tTL;
					if (lineToPath(tCenterX, tSkeleton_, &tCrossL))
					{
						intersections.push_back(tCrossL);
						tTL = getLengthTillX(tMeasure, tCrossL.fX);
						hTOffset = tTL;
						xpos = 0;
					}

					SkPoint tCrossR;
					if (lineToPath(&tCenterX[2], tSkeleton_, &tCrossR))
					{
						intersections.push_back(tCrossR);
						SkScalar tTR = getLengthTillX(tMeasure, tCrossR.fX);

						SkScalar bTL = getLengthTillX(bMeasure, tCenterX[0].fX);
						SkScalar bTR = getLengthTillX(bMeasure, tCenterX[2].fX);
						k1_ = (tTR - tTL) / (bTR - bTL);
					}
				}
				
				isTop = false;
				//warp Glypth by bottom line
				SkPath bWarped;
				morphpath(&bWarped, glypthPath, bMeasure, compositeBMatrix);
				bWarped_.addPath(bWarped);

				if (!tSkeleton_.isEmpty())
				{
					SkMatrix	compositeTMatrix(scaleMartix);
					compositeTMatrix.postTranslate(xpos + hTOffset, 0);
					compositeTMatrix.postConcat(matrix_);

					//transform Glypth, 
					SkPath lineMorphed;
					morphpath(&lineMorphed, glypthPath, lineMeasure, scaleMartix);

					isTop = true;
					SkPath tWarped;
					morphpath(&tWarped, glypthPath, tMeasure, compositeTMatrix);
					tWarped_.addPath(tWarped);

					weight(lineMorphed, tWarped, bWarped, &warpedPath);
				}
				else
					warpedPath.addPath(bWarped_);
			}

		}
	}
	
	return warpedPath;
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
		if (isTop)
		{
			iSrc.fX = k1_ * iSrc.fX;
		}

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