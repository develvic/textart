#include "SampleTextArt.h"
#include <vector>

class RingOutside : public TextArtView
{
public:
	RingOutside()
		: TextArtView("TextArt-RingOutside")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton, tSkeleton;

		//ring outside
		SkRect r;
		r.set(SkIntToScalar(0), SkIntToScalar(0),
				SkIntToScalar(330), SkIntToScalar(80));
		bSkeleton.addOval(r, SkPath::kCCW_Direction);

		warpFrame.push_back(bSkeleton);

		{	//top
			bSkeleton.offset(SkIntToScalar(0), SkIntToScalar(-100), &tSkeleton);

			warpFrame.push_back(tSkeleton);
		}
	}

};

static SkView* MyFactory() {
    return new RingOutside;
}

static SkViewRegister reg(MyFactory);
