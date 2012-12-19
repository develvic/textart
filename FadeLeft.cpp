#include "SampleTextArt.h"
#include <vector>

class FadeLeft : public TextArtView
{
public:
	FadeLeft()
		: TextArtView("TextArt-FadeLeft")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton, tSkeleton;

		std::vector<SkPoint> tpoints, bpoints;

		//bottom
		bpoints.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(0)));
		bpoints.push_back(SkPoint::Make(SkIntToScalar(700), SkIntToScalar(80)));
		bSkeleton.addPoly(&(*bpoints.begin()), bpoints.size(), false);

		warpFrame.push_back(bSkeleton);

		//top
		tpoints.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(-40)));
		tpoints.push_back(SkPoint::Make(SkIntToScalar(700), SkIntToScalar(-120)));
		tSkeleton.addPoly(&(*tpoints.begin()), tpoints.size(), false);

		warpFrame.push_back(tSkeleton);
	}

};

static SkView* MyFactory() {
    return new FadeLeft;
}

static SkViewRegister reg(MyFactory);
