#include "SampleTextArt.h"
#include <vector>

class Circle : public TextArtView
{
public:
	Circle()
		: TextArtView("TextArt-Circle")
	{
	}

protected:
	virtual void getWarpParams(WarpFrameT& warpFrame, SkMatrix& warpMatrix) const
	{
		SkPath bSkeleton, tSkeleton;

		SkRect r;
		r.set(SkIntToScalar(0), SkIntToScalar(0),
				SkIntToScalar(300), SkIntToScalar(170));

		//bottom
		{
			bSkeleton.addOval(r);

			warpFrame.push_back(bSkeleton);
		}

		//top
		{
			r.inset(SkIntToScalar(2*-70), SkIntToScalar(2*-40));
			tSkeleton.addOval(r);

			warpFrame.push_back(tSkeleton);
		}
	}

	virtual bool getIsNormalRotated() const 
	{
		return true;
	}
};

static SkView* MyFactory() {
    return new Circle;
}

static SkViewRegister reg(MyFactory);
