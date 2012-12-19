#include "SampleTextArt.h"
#include <vector>

class CascadeDown : public TextArtView
{
public:
	CascadeDown()
		: TextArtView("TextArt-CascadeDown")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton, tSkeleton;

		std::vector<SkPoint> tpoints, bpoints;

		//bottom
		bpoints.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(0-75)));
		bpoints.push_back(SkPoint::Make(SkIntToScalar(700), SkIntToScalar(0)));
		bSkeleton.addPoly(&(*bpoints.begin()), bpoints.size(), false);

		warpFrame.push_back(bSkeleton);
		
		//top
		tpoints.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(-150)));
		tpoints.push_back(SkPoint::Make(SkIntToScalar(700), SkIntToScalar(-150+20)));
		tSkeleton.addPoly(&(*tpoints.begin()), tpoints.size(), false);

		warpFrame.push_back(tSkeleton);		
	}

};

static SkView* MyFactory() {
    return new CascadeDown;
}

static SkViewRegister reg(MyFactory);
