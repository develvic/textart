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
		SkPath bSkeleton, tSkeleton;

		std::vector<SkPoint> tPoints, bPoints;
	
		{	//bottom
			bPoints.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(0)));
			bPoints.push_back(SkPoint::Make(SkIntToScalar(350), SkIntToScalar(50)));
			bPoints.push_back(SkPoint::Make(SkIntToScalar(700), SkIntToScalar(0)));
			bSkeleton.addPoly(&(*bPoints.begin()), bPoints.size(), false);

			warpFrame.push_back(bSkeleton);
		}

		{	//top
			bSkeleton.offset(SkIntToScalar(0), SkIntToScalar(-100), &tSkeleton);

			warpFrame.push_back(tSkeleton);
		}
	}

};

static SkView* MyFactory() {
    return new ChevronDown;
}

static SkViewRegister reg(MyFactory);
