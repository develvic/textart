#include "SampleTextArt.h"
#include <vector>

class DeflateTop : public TextArtView
{
public:
	DeflateTop()
		: TextArtView("TextArt-DeflateTop")
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
			SkPath tmp;
			tmp.addArc(r, SkIntToScalar(0+25), SkIntToScalar(180-2*25));
			tSkeleton.reverseAddPath(tmp);

			tSkeleton.offset(-bSkeleton.getPoint(0).fX, SkIntToScalar(-600));

			warpFrame.push_back(tSkeleton);
		}
	}
};

static SkView* MyFactory() {
    return new DeflateTop;
}

static SkViewRegister reg(MyFactory);
