#include "SampleTextArt.h"
#include <vector>

class CanUp : public TextArtView
{
public:
	CanUp()
		: TextArtView("TextArt-CanUp")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton, tSkeleton;

		//can up
		SkRect r;
		r.set(SkIntToScalar(0), SkIntToScalar(0),
				SkIntToScalar(650), SkIntToScalar(400));
		bSkeleton.addArc(r, SkIntToScalar(180), SkIntToScalar(180));

		warpFrame.push_back(bSkeleton);

		{	//top
			bSkeleton.offset(SkIntToScalar(0), SkIntToScalar(-100), &tSkeleton);

			warpFrame.push_back(tSkeleton);
		}
	}

};

static SkView* MyFactory() {
    return new CanUp;
}

static SkViewRegister reg(MyFactory);
