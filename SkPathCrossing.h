#ifndef __SK_PATH_CROSSING_H__
#define __SK_PATH_CROSSING_H__

#include "SkPath.h"
#include "SkTDArray.h"

class SkPathCrossing
{
public:
	SkPathCrossing(const SkPath& path);

	SkScalar getLength();

	bool rayCrossing(const SkPoint ray[2], SkScalar* crossDistance, SkPoint* crossPoint=NULL);

	SkPath flattern;

private:
    SkPath::Iter    fIter;
    const SkPath*   fPath;
    SkScalar        fLength;            // relative to the current contour
    int             fFirstPtIndex;      // relative to the current contour
    bool            fIsClosed;          // relative to the current contour
    bool            fForceClosed;

    struct Segment {
        SkScalar    fDistance;  // total distance up to this point
        unsigned    fPtIndex : 15; // index into the fPts array
        unsigned    fTValue : 15;
        unsigned    fType : 2;
		SkPoint		fPoints[2];

        SkScalar getScalarT() const;
    };
    SkTDArray<Segment>  fSegments;
    SkTDArray<SkPoint>  fPts; // Points used to define the segments

    static const Segment* NextSegment(const Segment*);

    void     buildSegments();
    SkScalar compute_quad_segs(const SkPoint pts[3], SkScalar distance,
                                int mint, int maxt, int ptIndex);
    SkScalar compute_cubic_segs(const SkPoint pts[3], SkScalar distance,
                                int mint, int maxt, int ptIndex);
    const Segment* distanceToSegment(SkScalar distance, SkScalar* t);

	bool lineToLineIntersection(const SkPoint l1[2], const SkPoint l2[2], SkPoint* crossPoint);
	bool rayToSegmentIntersection(const SkPoint ray[2], const SkPoint l2[2], SkPoint* crossPoint);
};

#endif
