#include "SkPathCrossing.h"
#include "SkGeometry.h"
#include "SkTSearch.h"

// these must be 0,1,2 since they are in our 2-bit field
enum {
    kLine_SegType,
    kQuad_SegType,
    kCubic_SegType
};

#define kMaxTValue  32767

static inline SkScalar tValue2Scalar(int t) {
    SkASSERT((unsigned)t <= kMaxTValue);

#ifdef SK_SCALAR_IS_FLOAT
    return t * 3.05185e-5f; // t / 32767
#else
    return (t + (t >> 14)) << 1;
#endif
}

SkScalar SkPathCrossing::Segment::getScalarT() const {
    return tValue2Scalar(fTValue);
}

static inline int tspan_big_enough(int tspan) {
    SkASSERT((unsigned)tspan <= kMaxTValue);
    return tspan >> 10;
}

// can't use tangents, since we need [0..1..................2] to be seen
// as definitely not a line (it is when drawn, but not parametrically)
// so we compare midpoints
#define CHEAP_DIST_LIMIT    (SK_Scalar1/2)  // just made this value up

static bool quad_too_curvy(const SkPoint pts[3]) {
    // diff = (a/4 + b/2 + c/4) - (a/2 + c/2)
    // diff = -a/4 + b/2 - c/4
    SkScalar dx = SkScalarHalf(pts[1].fX) -
                        SkScalarHalf(SkScalarHalf(pts[0].fX + pts[2].fX));
    SkScalar dy = SkScalarHalf(pts[1].fY) -
                        SkScalarHalf(SkScalarHalf(pts[0].fY + pts[2].fY));

    SkScalar dist = SkMaxScalar(SkScalarAbs(dx), SkScalarAbs(dy));
    return dist > CHEAP_DIST_LIMIT;
}

static bool cheap_dist_exceeds_limit(const SkPoint& pt,
                                     SkScalar x, SkScalar y) {
    SkScalar dist = SkMaxScalar(SkScalarAbs(x - pt.fX), SkScalarAbs(y - pt.fY));
    // just made up the 1/2
    return dist > CHEAP_DIST_LIMIT;
}

static bool cubic_too_curvy(const SkPoint pts[4]) {
    return  cheap_dist_exceeds_limit(pts[1],
                         SkScalarInterp(pts[0].fX, pts[3].fX, SK_Scalar1/3),
                         SkScalarInterp(pts[0].fY, pts[3].fY, SK_Scalar1/3))
                         ||
            cheap_dist_exceeds_limit(pts[2],
                         SkScalarInterp(pts[0].fX, pts[3].fX, SK_Scalar1*2/3),
                         SkScalarInterp(pts[0].fY, pts[3].fY, SK_Scalar1*2/3));
}

SkScalar SkPathCrossing::compute_quad_segs(const SkPoint pts[3],
                          SkScalar distance, int mint, int maxt, int ptIndex) {
    if (tspan_big_enough(maxt - mint) && quad_too_curvy(pts)) {
        SkPoint tmp[5];
        int     halft = (mint + maxt) >> 1;

        SkChopQuadAtHalf(pts, tmp);
        distance = this->compute_quad_segs(tmp, distance, mint, halft, ptIndex);
        distance = this->compute_quad_segs(&tmp[2], distance, halft, maxt, ptIndex);
    } else {
        SkScalar d = SkPoint::Distance(pts[0], pts[2]);
        SkScalar prevD = distance;
        distance += d;

		if (distance > prevD) {
//--------------------------------------
		this->flattern.moveTo(pts[0]);
		this->flattern.lineTo(pts[2]);
//--------------------------------------
            Segment* seg = fSegments.append();
            seg->fDistance = distance;
            seg->fPtIndex = ptIndex;
            seg->fType = kQuad_SegType;
            seg->fTValue = maxt;
			seg->fPoints[0] = pts[0];
			seg->fPoints[1] = pts[2];
        }
    }
    return distance;
}

SkScalar SkPathCrossing::compute_cubic_segs(const SkPoint pts[4],
                           SkScalar distance, int mint, int maxt, int ptIndex) {
    if (tspan_big_enough(maxt - mint) && cubic_too_curvy(pts)) {
        SkPoint tmp[7];
        int     halft = (mint + maxt) >> 1;

        SkChopCubicAtHalf(pts, tmp);
        distance = this->compute_cubic_segs(tmp, distance, mint, halft, ptIndex);
        distance = this->compute_cubic_segs(&tmp[3], distance, halft, maxt, ptIndex);
    } else {
        SkScalar d = SkPoint::Distance(pts[0], pts[3]);
        SkScalar prevD = distance;
        distance += d;

		if (distance > prevD) {
//--------------------------------------
		this->flattern.moveTo(pts[0]);
		this->flattern.lineTo(pts[3]);
//--------------------------------------
            Segment* seg = fSegments.append();
            seg->fDistance = distance;
            seg->fPtIndex = ptIndex;
            seg->fType = kCubic_SegType;
            seg->fTValue = maxt;
			seg->fPoints[0] = pts[0];
			seg->fPoints[1] = pts[3];
        }
    }
    return distance;
}

void SkPathCrossing::buildSegments() {
    SkPoint         pts[4];
    int             ptIndex = fFirstPtIndex;
    SkScalar        distance = 0;
    bool            isClosed = fForceClosed;
    bool            firstMoveTo = ptIndex < 0;
    Segment*        seg;

    /*  Note:
     *  as we accumulate distance, we have to check that the result of +=
     *  actually made it larger, since a very small delta might be > 0, but
     *  still have no effect on distance (if distance >>> delta).
     *
     *  We do this check below, and in compute_quad_segs and compute_cubic_segs
     */
    fSegments.reset();
    bool done = false;
    do {
        switch (fIter.next(pts)) {
            case SkPath::kMove_Verb:
                ptIndex += 1;
                fPts.append(1, pts);
                if (!firstMoveTo) {
                    done = true;
                    break;
                }
                firstMoveTo = false;
//--------------------------------------
		this->flattern.moveTo(pts[0]);
//--------------------------------------
                break;

            case SkPath::kLine_Verb: {
                SkScalar d = SkPoint::Distance(pts[0], pts[1]);
                SkASSERT(d >= 0);
                SkScalar prevD = distance;
                distance += d;

                if (distance > prevD) {
//--------------------------------------
		this->flattern.moveTo(pts[0]);
		this->flattern.lineTo(pts[1]);
//--------------------------------------
                    seg = fSegments.append();
                    seg->fDistance = distance;
                    seg->fPtIndex = ptIndex;
                    seg->fType = kLine_SegType;
                    seg->fTValue = kMaxTValue;
					seg->fPoints[0] = pts[0];
					seg->fPoints[1] = pts[1];
                    fPts.append(1, pts + 1);
                    ptIndex++;
                }
            } break;

            case SkPath::kQuad_Verb: {
                SkScalar prevD = distance;
                distance = this->compute_quad_segs(pts, distance, 0,
                                                   kMaxTValue, ptIndex);
                if (distance > prevD) {
                    fPts.append(2, pts + 1);
                    ptIndex += 2;
                }
            } break;

            case SkPath::kCubic_Verb: {
                SkScalar prevD = distance;
                distance = this->compute_cubic_segs(pts, distance, 0,
                                                    kMaxTValue, ptIndex);
                if (distance > prevD) {
                    fPts.append(3, pts + 1);
                    ptIndex += 3;
                }
            } break;

            case SkPath::kClose_Verb:
                isClosed = true;
                break;

            case SkPath::kDone_Verb:
                done = true;
                break;
        }
    } while (!done);

    fLength = distance;
    fIsClosed = isClosed;
    fFirstPtIndex = ptIndex;

#ifdef SK_DEBUG
    {
        const Segment* seg = fSegments.begin();
        const Segment* stop = fSegments.end();
        unsigned        ptIndex = 0;
        SkScalar        distance = 0;

        while (seg < stop) {
            SkASSERT(seg->fDistance > distance);
            SkASSERT(seg->fPtIndex >= ptIndex);
            SkASSERT(seg->fTValue > 0);

            const Segment* s = seg;
            while (s < stop - 1 && s[0].fPtIndex == s[1].fPtIndex) {
                SkASSERT(s[0].fType == s[1].fType);
                SkASSERT(s[0].fTValue < s[1].fTValue);
                s += 1;
            }

            distance = seg->fDistance;
            ptIndex = seg->fPtIndex;
            seg += 1;
        }
    //  SkDebugf("\n");
    }
#endif
}

SkPathCrossing::SkPathCrossing(const SkPath& path)
{
    fPath = &path;
    fLength = -1;   // signal we need to compute it
    fForceClosed = false;
    fFirstPtIndex = -1;

    fIter.setPath(path, fForceClosed);
}

SkScalar SkPathCrossing::getLength() {
    if (fPath == NULL) {
        return 0;
    }
    if (fLength < 0) {
        this->buildSegments();
    }
    SkASSERT(fLength >= 0);
    return fLength;
}

bool SkPathCrossing::rayCrossing(const SkPoint ray[2], SkScalar* crossDistance, SkPoint* crossPoint/*= NULL*/)
{
	if (fLength < 0)
        this->buildSegments();

	SkASSERT(fLength >= 0);

	SkPoint crossPointImpl;
	SkPoint* crossPointImplPtr = &crossPointImpl;
	if (crossPoint)
		crossPointImplPtr = crossPoint;

	const Segment* segmentItr = fSegments.begin();
	const Segment* segmentEnd = fSegments.end();
	for( ; segmentItr!=segmentEnd; ++segmentItr)
	{
		if (rayToSegmentIntersection(ray, segmentItr->fPoints, crossPointImplPtr))
		{
			SkScalar distance = SkPoint::Distance(*crossPointImplPtr, segmentItr->fPoints[1]);

			*crossDistance = segmentItr->fDistance - distance;
			return true;
		}
	}

	return false;
}

const SkPathCrossing::Segment* SkPathCrossing::distanceToSegment(
                                            SkScalar distance, SkScalar* t) {
    SkDEBUGCODE(SkScalar length = ) this->getLength();
    SkASSERT(distance >= 0 && distance <= length);

    const Segment*  seg = fSegments.begin();
    int             count = fSegments.count();

    int index = SkTSearch<SkScalar>(&seg->fDistance, count, distance,
                                    sizeof(Segment));
    // don't care if we hit an exact match or not, so we xor index if it is negative
    index ^= (index >> 31);
    seg = &seg[index];

    // now interpolate t-values with the prev segment (if possible)
    SkScalar    startT = 0, startD = 0;
    // check if the prev segment is legal, and references the same set of points
    if (index > 0) {
        startD = seg[-1].fDistance;
        if (seg[-1].fPtIndex == seg->fPtIndex) {
            SkASSERT(seg[-1].fType == seg->fType);
            startT = seg[-1].getScalarT();
        }
    }

    SkASSERT(seg->getScalarT() > startT);
    SkASSERT(distance >= startD);
    SkASSERT(seg->fDistance > startD);

    *t = startT + SkScalarMulDiv(seg->getScalarT() - startT,
                                 distance - startD,
                                 seg->fDistance - startD);
    return seg;
}

bool SkPathCrossing::lineToLineIntersection(const SkPoint l1[2], const SkPoint l2[2], SkPoint* cross)
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

	cross->fX = SkScalarDiv((B2*C1 - B1*C2), den);
    cross->fY = SkScalarDiv((A1*C2 - A2*C1), den);
	
	return true;
}

bool SkPathCrossing::rayToSegmentIntersection(const SkPoint ray[2], const SkPoint segm[2], SkPoint* crossPoint)
{
	if (lineToLineIntersection(ray, segm, crossPoint))
	{
		bool isActualCross = true;
		//ray start at l1_0 and continue till Infinity, cut crosses out of ray begining
		if (ray[0].fX < ray[1].fX)
			isActualCross &= (crossPoint->fX >= ray[0].fX);
		if (ray[0].fX > ray[1].fX)
			isActualCross &= (crossPoint->fX <= ray[0].fX);

		if (ray[0].fY < ray[1].fY)
			isActualCross &= (crossPoint->fY >= ray[0].fY);
		if (ray[0].fY > ray[1].fY)
			isActualCross &= (crossPoint->fY <= ray[0].fY);

		//cutoff coress out of the segment
		isActualCross &= (SkMinScalar(segm[0].fX, segm[1].fX) <= crossPoint->fX) && (crossPoint->fX <= SkMaxScalar(segm[0].fX, segm[1].fX));
		isActualCross &= (SkMinScalar(segm[0].fY, segm[1].fY) <= crossPoint->fY) && (crossPoint->fY <= SkMaxScalar(segm[0].fY, segm[1].fY));

		return isActualCross;
	}
	else
		return false;

}