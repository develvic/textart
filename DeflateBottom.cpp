#include "SampleTextArt.h"
#include <vector>

class DeflateBottom : public TextArtView
{
public:
	DeflateBottom()
		: TextArtView("TextArt-DeflateBottom")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton, tSkeleton;

		std::vector<SkPoint> tpoints, bpoints;
		
		SkRect r;
		r.set(SkIntToScalar(0), SkIntToScalar(0),
				SkIntToScalar(900), SkIntToScalar(700));

		//bottom
		{
			bSkeleton.addArc(r, SkIntToScalar(180+25), SkIntToScalar(180-2*25));

			bSkeleton.offset(-bSkeleton.getPoint(0).fX, SkIntToScalar(100));

			warpFrame.push_back(bSkeleton);
		}

		//top
		{
			tpoints.push_back(SkPoint::Make(SkIntToScalar(0), SkIntToScalar(0)));
			tpoints.push_back(SkPoint::Make(SkIntToScalar(850), SkIntToScalar(0)));
			tSkeleton.addPoly(&(*tpoints.begin()), tpoints.size(), false);

			warpFrame.push_back(tSkeleton);
		}
	}
};

static SkView* MyFactory() {
    return new DeflateBottom;
}

static SkViewRegister reg(MyFactory);
