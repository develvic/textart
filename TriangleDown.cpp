#include "SampleTextArt.h"
#include <vector>

class TriangleDown : public TextArtView
{
public:
	TriangleDown()
		: TextArtView("TextArt-TriangleDown")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton, tSkeleton;

		std::vector<SkPoint> tpoints, bpoints;

		//bottom
		bpoints.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(0)));
		bpoints.push_back(SkPoint::Make(SkIntToScalar(350), SkIntToScalar(90)));
		bpoints.push_back(SkPoint::Make(SkIntToScalar(700), SkIntToScalar(0)));
		bSkeleton.addPoly(&(*bpoints.begin()), bpoints.size(), false);

		warpFrame.push_back(bSkeleton);

		//top
		tpoints.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(-30)));
		tpoints.push_back(SkPoint::Make(SkIntToScalar(350), SkIntToScalar(-10)));
		tpoints.push_back(SkPoint::Make(SkIntToScalar(750), SkIntToScalar(-30)));
		tSkeleton.addPoly(&(*tpoints.begin()), tpoints.size(), false);

		warpFrame.push_back(tSkeleton);
	}

};

static SkView* MyFactory() {
    return new TriangleDown;
}

static SkViewRegister reg(MyFactory);
