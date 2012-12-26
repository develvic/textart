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
		SkPath bSkeleton, tSkeleton;

		//double wave1
		bSkeleton.cubicTo(SkIntToScalar(100), SkIntToScalar(-100),
							SkIntToScalar(100), SkIntToScalar(100),
							SkIntToScalar(330), SkIntToScalar(0));
		bSkeleton.cubicTo(SkIntToScalar(330+100), SkIntToScalar(-100),
							SkIntToScalar(330+100), SkIntToScalar(100),
							SkIntToScalar(330+330), SkIntToScalar(0));

		warpFrame.push_back(bSkeleton);


		{	//top
			bSkeleton.offset(SkIntToScalar(0), SkIntToScalar(-100), &tSkeleton);

			warpFrame.push_back(tSkeleton);
		}
	}

};

static SkView* MyFactory() {
    return new DoubleWave1;
}

static SkViewRegister reg(MyFactory);
