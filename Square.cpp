#include "SampleTextArt.h"
#include <vector>

class Square : public TextArtView
{
public:
	Square()
		: TextArtView("TextArt-Square")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton, tSkeleton;

		//skew transformation
		warpMatrix.setSkew(SkFloatToScalar(0.9), SkFloatToScalar(0.0));

		//line
		std::vector<SkPoint> points, bPoints;
		points.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(0)));
		points.push_back(SkPoint::Make(SkIntToScalar(800), SkIntToScalar(0)));
		bSkeleton.addPoly(&(*points.begin()), points.size(), false);

		warpFrame.push_back(bSkeleton);

		{	//top
			bSkeleton.offset(SkIntToScalar(0), SkIntToScalar(-100), &tSkeleton);

			warpFrame.push_back(tSkeleton);
		}
	}

};

static SkView* MyFactory() {
    return new Square;
}

static SkViewRegister reg(MyFactory);
