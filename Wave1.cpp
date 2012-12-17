#include "SampleTextArt.h"
#include <vector>

class Wave1 : public TextArtView
{
public:
	Wave1()
		: TextArtView("TextArt-Wave1")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton;

		//wave1
		bSkeleton.cubicTo(SkIntToScalar(160), SkIntToScalar(-120),
							SkIntToScalar(540), SkIntToScalar(120),
							SkIntToScalar(700), SkIntToScalar(0));

		warpFrame.push_back(bSkeleton);
	}

};

static SkView* MyFactory() {
    return new Wave1;
}

static SkViewRegister reg(MyFactory);
