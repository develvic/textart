#include "SampleTextArt.h"
#include <vector>

class TriangleUp : public TextArtView
{
public:
	TriangleUp()
		: TextArtView("TextArt-TriangleUp")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton, tSkeleton;

		//chevron up
		std::vector<SkPoint> tpoints, bpoints;

		//bottom
		bpoints.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(0)));
		bpoints.push_back(SkPoint::Make(SkIntToScalar(475), SkIntToScalar(0)));
		bpoints.push_back(SkPoint::Make(SkIntToScalar(950), SkIntToScalar(0)));
		bSkeleton.addPoly(&(*bpoints.begin()), bpoints.size(), false);

		warpFrame.push_back(bSkeleton);

		//top
		tpoints.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(-30)));
		tpoints.push_back(SkPoint::Make(SkIntToScalar(475), SkIntToScalar(-250)));
		tpoints.push_back(SkPoint::Make(SkIntToScalar(950), SkIntToScalar(-30)));
		tSkeleton.addPoly(&(*tpoints.begin()), tpoints.size(), false);

		warpFrame.push_back(tSkeleton);

	}

};

static SkView* MyFactory() {
    return new TriangleUp;
}

static SkViewRegister reg(MyFactory);
