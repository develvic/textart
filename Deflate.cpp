#include "SampleTextArt.h"
#include <vector>

class Deflate : public TextArtView
{
public:
	Deflate()
		: TextArtView("TextArt-Deflate")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton, tSkeleton;

		
		SkRect r;
		r.set(SkIntToScalar(0), SkIntToScalar(0),
				SkIntToScalar(800), SkIntToScalar(400));

		//bottom
		{
			bSkeleton.addArc(r, SkIntToScalar(180+25), SkIntToScalar(180-2*25));

			warpFrame.push_back(bSkeleton);
		}
		
		//top
		{
			r.offset(SkIntToScalar(0), SkIntToScalar(-480));

			SkPath tmp;
			tmp.addArc(r, SkIntToScalar(0+25), SkIntToScalar(180-2*25));
			tSkeleton.reverseAddPath(tmp);

			warpFrame.push_back(tSkeleton);
		}
	}
};

static SkView* MyFactory() {
    return new Deflate;
}

static SkViewRegister reg(MyFactory);
