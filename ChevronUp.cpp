#include "SampleTextArt.h"
#include <vector>

class ChevronUp : public TextArtView
{
public:
	ChevronUp()
		: TextArtView("TextArt-ChevronUp")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton;

		//chevron up
		std::vector<SkPoint> points, bPoints;
		points.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(0)));
		points.push_back(SkPoint::Make(SkIntToScalar(350), SkIntToScalar(-50)));
		points.push_back(SkPoint::Make(SkIntToScalar(700), SkIntToScalar(0)));
		bSkeleton.addPoly(&(*points.begin()), points.size(), false);

		warpFrame.push_back(bSkeleton);
	}

};

static SkView* MyFactory() {
    return new ChevronUp;
}

static SkViewRegister reg(MyFactory);
