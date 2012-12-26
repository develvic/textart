#include "SampleTextArt.h"
#include <vector>

class ArcUp : public TextArtView
{
public:
	ArcUp()
		: TextArtView("TextArt-ArcUp")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton, tSkeleton;

		SkRect r;
		r.set(SkIntToScalar(0), SkIntToScalar(0),
				SkIntToScalar(600), SkIntToScalar(340));

		//bottom
		{
			bSkeleton.addArc(r, SkIntToScalar(180), SkIntToScalar(180));

			warpFrame.push_back(bSkeleton);
		}

		//top
		{
			r.inset(SkIntToScalar(-120), SkIntToScalar(-70));
			tSkeleton.addArc(r, SkIntToScalar(180), SkIntToScalar(180));

			warpFrame.push_back(tSkeleton);
		}
	}

	virtual bool getIsNormalRotated() const 
	{
		return true;
	}
};

static SkView* MyFactory() {
    return new ArcUp;
}

static SkViewRegister reg(MyFactory);
