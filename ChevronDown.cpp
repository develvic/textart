#include "SampleTextArt.h"
#include <vector>

class ChevronDown : public TextArtView
{
public:
	ChevronDown()
		: TextArtView("TextArt-ChevronDown")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton;

		//chevron down
		std::vector<SkPoint> points, bPoints;
		points.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(0)));
		points.push_back(SkPoint::Make(SkIntToScalar(350), SkIntToScalar(50)));
		points.push_back(SkPoint::Make(SkIntToScalar(700), SkIntToScalar(0)));
		bSkeleton.addPoly(&(*points.begin()), points.size(), false);

		warpFrame.push_back(bSkeleton);
	}

};

static SkView* MyFactory() {
    return new ChevronDown;
}

static SkViewRegister reg(MyFactory);
