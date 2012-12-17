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
		SkPath bSkeleton;

		//ring outside
		SkRect r;
		r.set(SkIntToScalar(0), SkIntToScalar(0),
				SkIntToScalar(320), SkIntToScalar(100));
		bSkeleton.addOval(r, SkPath::kCCW_Direction);

		warpFrame.push_back(bSkeleton);
	}

};

static SkView* MyFactory() {
    return new RingOutside;
}

static SkViewRegister reg(MyFactory);
