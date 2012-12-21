#include "SampleTextArt.h"
#include <vector>

class InflateBottom : public TextArtView
{
public:
	InflateBottom()
		: TextArtView("TextArt-InflateBottom")
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
			SkPath tmp;
			tmp.addArc(r, SkIntToScalar(0+25), SkIntToScalar(180-2*25));
			bSkeleton.reverseAddPath(tmp);

			//center Bottom arc relatively to Top line
			bSkeleton.offset(SkScalarHalf(850) - bSkeleton.getBounds().centerX(), SkIntToScalar(-400));

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
    return new InflateBottom;
}

static SkViewRegister reg(MyFactory);
