#include "SampleTextArt.h"
#include <vector>

class CurveUp : public TextArtView
{
public:
	CurveUp()
		: TextArtView("TextArt-CurveUp")
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
			tSkeleton.moveTo(SkIntToScalar(0), SkIntToScalar(-70));
			tSkeleton.cubicTo(SkIntToScalar(175), SkIntToScalar(170),
								SkIntToScalar(700), SkIntToScalar(-210),
								SkIntToScalar(730), SkIntToScalar(-240));

			warpFrame.push_back(tSkeleton);
		}
	}

};

static SkView* MyFactory() {
    return new CurveUp;
}

static SkViewRegister reg(MyFactory);
