#include "SampleTextArt.h"
#include <vector>

class CanDown : public TextArtView
{
public:
	CanDown()
		: TextArtView("TextArt-CanDown")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton, tSkeleton, tmp;

		//can down
		SkRect r;
		r.set(SkIntToScalar(0), SkIntToScalar(0),
				SkIntToScalar(650), SkIntToScalar(400));
		{	//bottom
			tmp.addArc(r, SkIntToScalar(0), SkIntToScalar(180));
			bSkeleton.reverseAddPath(tmp);
		}

		warpFrame.push_back(bSkeleton);

		{	//top
			bSkeleton.offset(SkIntToScalar(0), SkIntToScalar(-100), &tSkeleton);

			warpFrame.push_back(tSkeleton);
		}
	}

};

static SkView* MyFactory() {
    return new CanDown;
}

static SkViewRegister reg(MyFactory);
