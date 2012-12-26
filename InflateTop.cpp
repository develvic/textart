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
				SkIntToScalar(900), SkIntToScalar(800));

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
			
			//center Top arc relatively to Bottom line
			//tSkeleton.offset(SkScalarHalf(800) - tSkeleton.getBounds().centerX(), SkIntToScalar(-200));

			//alignt Top to the Left of Bottom
			tSkeleton.offset(0 - tSkeleton.getBounds().left(), SkIntToScalar(-270));

			warpFrame.push_back(tSkeleton);
		}
	}
};

static SkView* MyFactory() {
    return new InflateTop;
}

static SkViewRegister reg(MyFactory);
