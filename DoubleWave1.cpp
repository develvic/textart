#include "SampleTextArt.h"
#include <vector>

class DoubleWave1 : public TextArtView
{
public:
	DoubleWave1()
		: TextArtView("TextArt-DoubleWave1")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton;

		//double wave1
		bSkeleton.cubicTo(SkIntToScalar(100), SkIntToScalar(-100),
							SkIntToScalar(100), SkIntToScalar(100),
							SkIntToScalar(330), SkIntToScalar(0));
		bSkeleton.cubicTo(SkIntToScalar(330+100), SkIntToScalar(-100),
							SkIntToScalar(330+100), SkIntToScalar(100),
							SkIntToScalar(330+330), SkIntToScalar(0));

		warpFrame.push_back(bSkeleton);
	}

};

static SkView* MyFactory() {
    return new DoubleWave1;
}

static SkViewRegister reg(MyFactory);
