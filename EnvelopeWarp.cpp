#include "EnvelopeWarp.h"

#include "SkTextToPathIter.h"
#include "SkTypeface.h"

TextArt::EnvelopeWarp::EnvelopeWarp(const SkPath& skeleton, const SkMatrix& matrix)
	: bSkeleton_(skeleton)
	, matrix_(matrix)
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

	SkPaint paint;
	paint.setTextSize(SkIntToScalar(64));
	paint.setTypeface(typeface);
	paint.setTextAlign(SkPaint::kCenter_Align);


	SkRect tb = tSkeleton_.getBounds();
	paint.measureText(text.c_str(), text.size(), &boundsRect_);
	boundsRect_.inset(0, SkFloatToScalar(2));
	tSkeleton_.offset(0-tb.fLeft, boundsRect_.height());
	height_ = boundsRect_.height();

	SkPathMeasure   tMeasure(tSkeleton_, false);
	SkPathMeasure   bMeasure(bSkeleton_, false);
	SkScalar        hOffset = 0;
	//need to measure first
    if (paint.getTextAlign() != SkPaint::kLeft_Align)
	{
        SkScalar pathLen = bMeasure.getLength();
        if (paint.getTextAlign() == SkPaint::kCenter_Align)
		{
            pathLen = SkScalarHalf(pathLen);
        }
        hOffset += pathLen;
    }

	SkTextToPathIter	iter(text.c_str(), text.size(), paint, true);
	const SkPath*   iterPath;
    SkScalar        xpos;

    SkMatrix        scaleMartix;
    SkScalar        scale = iter.getPathScale();
    scaleMartix.setScale(scale, scale);
/*
	SkPath origPath;
	while (iter.next(&iterPath, &xpos))
	{
		if (iterPath)
		{
			SkMatrix	compositeMatrix(scaleMartix);
			compositeMatrix.postTranslate(xpos + hOffset, 0);

			SkPath tmp;
			(*iterPath).transform(compositeMatrix, &tmp);
			origPath.addPath(tmp);
		}
	}
	boundsRect_ = origPath.getBounds();
	SkRect tb = tSkeleton_.getBounds();
	tSkeleton_.offset(0-tb.fLeft, boundsRect_.height());
	height_ = boundsRect_.height();
*/

/*
	{
		SkPathMeasure   tMeasure(tSkeleton_, false);
		SkPathMeasure   bMeasure(bSkeleton_, false);


		SkPath tWarped, bWarped;

		morphpath(&bWarped, origPath, bMeasure, matrix_);
		morphpath(&tWarped, origPath, tMeasure, matrix_);

		weight(origPath, tWarped, bWarped, &warpedPath);

		bWarped_.addPath(bWarped); tWarped_.addPath(tWarped);
	//	warpedPath.addPath(tWarped);
	}
*/

	SkPath tWarped, bWarped;
	xpos = 0;
	while (iter.next(&iterPath, &xpos))
	{
		if (iterPath)
		{

			SkMatrix	compositeMatrix(scaleMartix);
			compositeMatrix.postTranslate(xpos + hOffset, 0);
			compositeMatrix.postConcat(matrix_);

			SkPath tWarped, bWarped;

			morphpath(&bWarped, *iterPath, bMeasure, compositeMatrix);

			if (!tSkeleton_.isEmpty())
			{
				morphpath(&tWarped, *iterPath, tMeasure, compositeMatrix);

				weight(*iterPath, tWarped, bWarped, &warpedPath);

				bWarped_.addPath(bWarped); tWarped_.addPath(tWarped);
			}
			else
				warpedPath.addPath(bWarped);

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

        proc(matrix, src[i].fX, src[i].fY, &pos);
        SkScalar sx = pos.fX;
        SkScalar sy = pos.fY;

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
/*
        dst[i].set(pos.fX - SkScalarMul(tangent.fY, sy),
                   pos.fY + SkScalarMul(tangent.fX, sy));
*/
		dst[i].set(pos.fX,
            pos.fY +  sy);
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
                morphpoints(dstP, &srcP[1], 1, meas, matrix);
                dst->lineTo(dstP[0]);
                break;

/*
                // turn lines into quads to look bendy
                srcP[0].fX = SkScalarAve(srcP[0].fX, srcP[1].fX);
                srcP[0].fY = SkScalarAve(srcP[0].fY, srcP[1].fY);
                morphpoints(dstP, srcP, 2, meas, matrix);
                dst->quadTo(dstP[0], dstP[1]);
                break;
*/
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

void TextArt::EnvelopeWarp::weight(const SkPoint src[], const SkPoint tSrc[], const SkPoint bSrc[], int count, SkPoint dst[])
{
	for (int i = 0; i < count; i++)
	{
		SkScalar origY = src[i].fY;

		dst[i].fX = bSrc[i].fX;
		dst[i].fY = origY;
		
		if (boundsRect_.fBottom > 0)
			origY -= boundsRect_.fBottom;
		
		SkScalar k1 = SkScalarAbs( SkScalarDiv(origY, height_) );
		dst[i].fY = SkScalarInterp(bSrc[i].fY, tSrc[i].fY, k1);

/*
		SkScalar h = boundsRect_.centerY();

		if (origY <= h)
		{
			origY = origY + 17;
			SkScalar k = SkScalarAbs( SkScalarDiv(origY, 18) );

			dst[i].fY = SkScalarInterp(bSrc[i].fY, tSrc[i].fY, k);
		}
		else
		{
			if (origY > 0)
			{
				origY = origY + 17;

				SkScalar k = SkScalarAbs( SkScalarDiv(origY, 18) );

				dst[i].fY = SkScalarInterp(tSrc[i].fY, bSrc[i].fY, k);

			}
			else
			{
				origY = origY + 17;
				SkScalar k = SkScalarAbs( SkScalarDiv(origY, 18) );

				dst[i].fY = SkScalarInterp(tSrc[i].fY, bSrc[i].fY, k);
			}
		}
*/
/*
		if (origY <= 0)
		{
			SkScalar k1 = SkScalarDiv(origY, -35.0);

			dst[i].fY = SkScalarInterp(bSrc[i].fY, tSrc[i].fY, k1);
		}
		else
		{
			SkScalar k1 = SkScalarDiv(origY, 1);

			dst[i].fY = SkScalarInterp(0, bSrc[i].fY*2, k1);
		}
*/
	}
}

void TextArt::EnvelopeWarp::weight(const SkPath& path, const SkPath& top, const SkPath& bottom, SkPath* dst)
{
	SkPath::Iter    iterB(bottom, false);
	SkPath::Iter    iterT(top, false);
	SkPath::Iter    iter(path, false);

    SkPoint         tSrc[4], bSrc[4], src[4], dstP[4];
    SkPath::Verb    verbT, verbB, verb;

    while (((verbT = iterT.next(tSrc)) != SkPath::kDone_Verb)	&& 
			((verbB = iterB.next(bSrc)) != SkPath::kDone_Verb)	&&
			((verb = iter.next(src)) != SkPath::kDone_Verb) )
	{
		SkASSERT(verbT==verbB);
		SkASSERT(verbT==verb);

        switch (verbT)
		{
            case SkPath::kMove_Verb:
                weight(&src[0], &tSrc[0], &bSrc[0], 1, dstP);
                dst->moveTo(dstP[0]);
                break;
            case SkPath::kLine_Verb:
                weight(&src[1], &tSrc[1], &bSrc[1], 1, dstP);
                dst->lineTo(dstP[0]);
                break;
            case SkPath::kQuad_Verb:
				weight(&src[1], &tSrc[1], &bSrc[1], 2, dstP);
                dst->quadTo(dstP[0], dstP[1]);
                break;
            case SkPath::kCubic_Verb:
				weight(&src[1], &tSrc[1], &bSrc[1], 3, dstP);
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