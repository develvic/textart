#include "SampleTextArt.h"
#include <vector>

class SlantUp : public TextArtView
{
public:
	SlantUp()
		: TextArtView("TextArt-SlantUp")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton, tSkeleton;

		std::vector<SkPoint> tpoints, bpoints;

		//bottom
		bpoints.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(80)));
		bpoints.push_back(SkPoint::Make(SkIntToScalar(700), SkIntToScalar(-80)));
		bSkeleton.addPoly(&(*bpoints.begin()), bpoints.size(), false);

		warpFrame.push_back(bSkeleton);
		
		//top
		tpoints.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(80-100)));
		tpoints.push_back(SkPoint::Make(SkIntToScalar(700), SkIntToScalar(-80-100)));
		tSkeleton.addPoly(&(*tpoints.begin()), tpoints.size(), false);

		warpFrame.push_back(tSkeleton);		
	}

};

static SkView* MyFactory() {
    return new SlantUp;
}

static SkViewRegister reg(MyFactory);
