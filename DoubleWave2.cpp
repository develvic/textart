#include "SampleTextArt.h"
#include <vector>

class DoubleWave2 : public TextArtView
{
public:
	DoubleWave2()
		: TextArtView("TextArt-DoubleWave2")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton;

		//double wave2
		bSkeleton.cubicTo(SkIntToScalar(100), SkIntToScalar(100),
							SkIntToScalar(100), SkIntToScalar(-100),
							SkIntToScalar(330), SkIntToScalar(0));
		bSkeleton.cubicTo(SkIntToScalar(330+100), SkIntToScalar(100),
							SkIntToScalar(330+100), SkIntToScalar(-100),
							SkIntToScalar(330+330), SkIntToScalar(0));

		warpFrame.push_back(bSkeleton);
	}

};

static SkView* MyFactory() {
    return new DoubleWave2;
}

static SkViewRegister reg(MyFactory);
