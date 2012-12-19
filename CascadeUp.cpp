#include "SampleTextArt.h"
#include <vector>

class CascadeUp : public TextArtView
{
public:
	CascadeUp()
		: TextArtView("TextArt-CascadeUp")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton, tSkeleton;

		std::vector<SkPoint> tpoints, bpoints;

		//bottom
		bpoints.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(0)));
		bpoints.push_back(SkPoint::Make(SkIntToScalar(700), SkIntToScalar(0-75)));
		bSkeleton.addPoly(&(*bpoints.begin()), bpoints.size(), false);

		warpFrame.push_back(bSkeleton);
		
		//top
		tpoints.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(-150+20)));
		tpoints.push_back(SkPoint::Make(SkIntToScalar(700), SkIntToScalar(-150)));
		tSkeleton.addPoly(&(*tpoints.begin()), tpoints.size(), false);

		warpFrame.push_back(tSkeleton);		
	}

};

static SkView* MyFactory() {
    return new CascadeUp;
}

static SkViewRegister reg(MyFactory);
