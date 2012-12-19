#include "SampleTextArt.h"
#include <vector>

class CurveDown : public TextArtView
{
public:
	CurveDown()
		: TextArtView("TextArt-CurveDown")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton, tSkeleton;

		

		//bottom
		{
		bSkeleton.cubicTo(SkIntToScalar(240), SkIntToScalar(175),
							SkIntToScalar(700), SkIntToScalar(70),
							SkIntToScalar(730), SkIntToScalar(0));

		warpFrame.push_back(bSkeleton);
		}

		//top
		{
			tSkeleton.moveTo(SkIntToScalar(0), SkIntToScalar(-240));
			tSkeleton.cubicTo(SkIntToScalar(30), SkIntToScalar(-210),
								SkIntToScalar(575), SkIntToScalar(170),
								SkIntToScalar(730), SkIntToScalar(-100));

			warpFrame.push_back(tSkeleton);
		}
	}

};

static SkView* MyFactory() {
    return new CurveDown;
}

static SkViewRegister reg(MyFactory);
