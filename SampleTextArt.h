#ifndef _SAMPLE_TEXTART_H_
#define _SAMPLE_TEXTART_H_

#include "SampleCode.h"
#include "SkCanvas.h"
#include "EnvelopeWarp.h"

#include <string>
#include <vector>

class TextArtView : public SampleView 
{

public:
    TextArtView(const std::string& title);


protected:
	typedef std::vector<SkPath> WarpFrameT;

protected:
    // overrides from SkEventSink
    virtual bool onQuery(SkEvent* evt);

    virtual void onDrawContent(SkCanvas* canvas);

    virtual SkView::Click* onFindClickHandler(SkScalar x, SkScalar y);

    virtual bool onClick(Click* click);

	virtual void getWarpParams(WarpFrameT& frame, SkMatrix& matrix) const = 0;

	virtual bool getIsNormalRotated() const {return false;}

	virtual bool getIsSymmetric() const {return false;}

	virtual bool getIsTopBased() const {return false;}

	SkPath		bSkeleton_;
	SkPath		tSkeleton_;
	SkPath      path_;

	SkPath		origPath_;

private:
    int fHints;
    typedef SampleView INHERITED;
	bool initialized;
	std::string title;
};

#endif
