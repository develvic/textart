#include "SampleTextArt.h"
#include <vector>

class Stop : public TextArtView
{
public:
	Stop()
		: TextArtView("TextArt-Stop")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton, tSkeleton;

		std::vector<SkPoint> tpoints, bpoints;

		//bottom
		bpoints.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(20)));
		bpoints.push_back(SkPoint::Make(SkIntToScalar(235), SkIntToScalar(100)));
		bpoints.push_back(SkPoint::Make(SkIntToScalar(465), SkIntToScalar(100)));
		bpoints.push_back(SkPoint::Make(SkIntToScalar(700), SkIntToScalar(20)));
		bSkeleton.addPoly(&(*bpoints.begin()), bpoints.size(), false);

		warpFrame.push_back(bSkeleton);

		//top
		tpoints.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(-20)));
		tpoints.push_back(SkPoint::Make(SkIntToScalar(235), SkIntToScalar(-100)));
		tpoints.push_back(SkPoint::Make(SkIntToScalar(465), SkIntToScalar(-100)));
		tpoints.push_back(SkPoint::Make(SkIntToScalar(700), SkIntToScalar(-20)));
		tSkeleton.addPoly(&(*tpoints.begin()), tpoints.size(), false);

		warpFrame.push_back(tSkeleton);
	}

};

static SkView* MyFactory() {
    return new Stop;
}

static SkViewRegister reg(MyFactory);
