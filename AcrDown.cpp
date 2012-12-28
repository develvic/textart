#include "SampleTextArt.h"
#include <vector>

class ArcDown : public TextArtView
{
public:
	ArcDown()
		: TextArtView("TextArt-ArcDown")
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
			SkRect rb(r);
			rb.inset(SkIntToScalar(-240), SkIntToScalar(-140));

			SkPath tmp;
			tmp.addArc(rb, SkIntToScalar(0), SkIntToScalar(180));
			bSkeleton.reverseAddPath(tmp);

			warpFrame.push_back(bSkeleton);
		}

		//top
		{
			

			SkPath tmp;
			tmp.addArc(r, SkIntToScalar(0), SkIntToScalar(180));
			tSkeleton.reverseAddPath(tmp);

			warpFrame.push_back(tSkeleton);
		}
	}

	virtual bool getIsNormalRotated() const 
	{
		return true;
	}
};

static SkView* MyFactory() {
    return new ArcDown;
}

static SkViewRegister reg(MyFactory);
