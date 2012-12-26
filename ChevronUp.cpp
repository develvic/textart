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
		SkPath bSkeleton, tSkeleton;

		//chevron up
		std::vector<SkPoint> points, bPoints;
		points.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(0)));
		points.push_back(SkPoint::Make(SkIntToScalar(350), SkIntToScalar(-50)));
		points.push_back(SkPoint::Make(SkIntToScalar(700), SkIntToScalar(0)));
		bSkeleton.addPoly(&(*points.begin()), points.size(), false);

		warpFrame.push_back(bSkeleton);

		{	//top
			bSkeleton.offset(SkIntToScalar(0), SkIntToScalar(-100), &tSkeleton);

			warpFrame.push_back(tSkeleton);
		}
	}

};

static SkView* MyFactory() {
    return new ChevronUp;
}

static SkViewRegister reg(MyFactory);
