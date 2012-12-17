#include "SampleTextArt.h"
#include <vector>

class RingInside : public TextArtView
{
public:
	RingInside()
		: TextArtView("TextArt-RingInside")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton;

		//ring inside
		SkRect r;
		r.set(SkIntToScalar(0), SkIntToScalar(0),
				SkIntToScalar(320), SkIntToScalar(100));
		bSkeleton.addOval(r);

		warpFrame.push_back(bSkeleton);
	}

};

static SkView* MyFactory() {
    return new RingInside;
}

static SkViewRegister reg(MyFactory);
