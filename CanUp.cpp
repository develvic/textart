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
		SkPath bSkeleton;

		//can up
		SkRect r;
		r.set(SkIntToScalar(0), SkIntToScalar(0),
				SkIntToScalar(750), SkIntToScalar(120));
		bSkeleton.addArc(r, SkIntToScalar(180), SkIntToScalar(180));

		warpFrame.push_back(bSkeleton);
	}

};

static SkView* MyFactory() {
    return new CanUp;
}

static SkViewRegister reg(MyFactory);
