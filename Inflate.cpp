#include "SampleTextArt.h"
#include <vector>

class Inflate : public TextArtView
{
public:
	Inflate()
		: TextArtView("TextArt-Inflate")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton, tSkeleton;

		
		SkRect r;
		r.set(SkIntToScalar(0), SkIntToScalar(0),
				SkIntToScalar(800), SkIntToScalar(300));

		//bottom
		{
			SkPath tmp;
			tmp.addArc(r, SkIntToScalar(0+25), SkIntToScalar(180-2*25));
			bSkeleton.reverseAddPath(tmp);

			warpFrame.push_back(bSkeleton);
		}

		//top
		{
			tSkeleton.addArc(r, SkIntToScalar(180+25), SkIntToScalar(180-2*25));

			warpFrame.push_back(tSkeleton);
		}
	}
};

static SkView* MyFactory() {
    return new Inflate;
}

static SkViewRegister reg(MyFactory);
