#include "SampleTextArt.h"

class Wave2 : public TextArtView
{
public:
	Wave2()
		: TextArtView("TextArt-Wave2")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton;

		//wave2
		bSkeleton.cubicTo(SkIntToScalar(160), SkIntToScalar(120),
							SkIntToScalar(540), SkIntToScalar(-120),
							SkIntToScalar(700), SkIntToScalar(0));

		warpFrame.push_back(bSkeleton);
	}

};

static SkView* MyFactory() {
    return new Wave2;
}

static SkViewRegister reg(MyFactory);
