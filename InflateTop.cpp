#include "SampleTextArt.h"
#include <vector>

class InflateTop : public TextArtView
{
public:
	InflateTop()
		: TextArtView("TextArt-InflateTop")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton, tSkeleton;

		std::vector<SkPoint> tpoints, bpoints;

		SkRect r;
		r.set(SkIntToScalar(0), SkIntToScalar(0),
				SkIntToScalar(800), SkIntToScalar(500));

		//bottom
		{
			bpoints.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(0)));
			bpoints.push_back(SkPoint::Make(SkIntToScalar(800), SkIntToScalar(0)));
			bSkeleton.addPoly(&(*bpoints.begin()), bpoints.size(), false);

			warpFrame.push_back(bSkeleton);
		}

		//top
		{
			tSkeleton.addArc(r, SkIntToScalar(180+25), SkIntToScalar(180-2*25));
			tSkeleton.offset(-bSkeleton.getPoint(0).fX, SkIntToScalar(-200));

			warpFrame.push_back(tSkeleton);
		}
	}
};

static SkView* MyFactory() {
    return new InflateTop;
}

static SkViewRegister reg(MyFactory);
