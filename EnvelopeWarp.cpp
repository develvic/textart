#include "EnvelopeWarp.h"

#include "SkTextToPathIter.h"
#include "SkTypeface.h"

TextArt::EnvelopeWarp::EnvelopeWarp(const SkPath& skeleton, const SkMatrix& matrix)
	: skeletonPath_(skeleton)
	, matrix_(matrix)
{
}

SkPath TextArt::EnvelopeWarp::warp(const SkPath& path, const SkMatrix& matrix)
{
	SkPath warpedPath;

	SkPathMeasure measure(skeletonPath_, false);

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

	SkTextToPathIter	iter(text.c_str(), text.size(), paint, true);

	SkPathMeasure   measure(skeletonPath_, false);
	SkScalar        hOffset = 0;
	//need to measure first
    if (paint.getTextAlign() != SkPaint::kLeft_Align)
	{
        SkScalar pathLen = measure.getLength();
        if (paint.getTextAlign() == SkPaint::kCenter_Align)
		{
            pathLen = SkScalarHalf(pathLen);
        }
        hOffset += pathLen;
    }

    SkMatrix        scaleMartix;
    SkScalar        scale = iter.getPathScale();

    scaleMartix.setScale(scale, scale);

	const SkPath*   iterPath;
    SkScalar        xpos;

	while (iter.next(&iterPath, &xpos))
	{
		if (iterPath)
		{

			SkMatrix	compositeMatrix(scaleMartix);
			compositeMatrix.postTranslate(xpos + hOffset, 0);
			compositeMatrix.postConcat(matrix_);

			SkPath      tmp;
			morphpath(&tmp, *iterPath, measure, compositeMatrix);

			warpedPath.addPath(tmp);
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
		dst[i].set(pos.fX ,
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
				*/
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