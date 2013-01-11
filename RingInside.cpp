#include "SampleTextArt.h"
#include <vector>

class RingInside : public TextArtView
{
public:
	RingInside()
		: TextArtView("TextArt-RingInside")
	{
	}

	virtual bool getIsSymmetric() const
	{
		return true;
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton, tSkeleton;

		//ring inside
		SkRect r;
		r.set(SkIntToScalar(0), SkIntToScalar(0),
				SkIntToScalar(330), SkIntToScalar(100));
		bSkeleton.addOval(r);

		warpFrame.push_back(bSkeleton);

		{	//top
			bSkeleton.offset(SkIntToScalar(0), SkIntToScalar(-100), &tSkeleton);

			warpFrame.push_back(tSkeleton);
		}
	}
};

static SkView* MyFactory() {
    return new RingInside;
}

static SkViewRegister reg(MyFactory);
