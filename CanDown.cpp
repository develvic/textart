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
		SkPath bSkeleton, tmp;

		//can down
		SkRect r;
		r.set(SkIntToScalar(0), SkIntToScalar(0),
				SkIntToScalar(750), SkIntToScalar(120));
		
		tmp.addArc(r, SkIntToScalar(0), SkIntToScalar(180));
		bSkeleton.reverseAddPath(tmp);

		warpFrame.push_back(bSkeleton);
	}

};

static SkView* MyFactory() {
    return new CanDown;
}

static SkViewRegister reg(MyFactory);
