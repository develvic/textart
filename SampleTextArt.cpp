#include "SampleTextArt.h"
#include "SkCanvas.h"
#include "SkTypeface.h"
#include "SkScalar.h"

#include "EnvelopeWarp.h"

#include <vector>

TextArtView::TextArtView(const std::string& ititle)
	: initialized(false)
	, title(ititle)
{
//			m.setPerspX(SkScalarToPersp(SK_Scalar1/200));
//			m.setPerspY(SkScalarToPersp(SK_Scalar1/500));
}

// overrides from SkEventSink
bool TextArtView::onQuery(SkEvent* evt)
{
    if (SampleCode::TitleQ(*evt)) {
        SampleCode::TitleR(evt, title.c_str());
        return true;
    }
    return this->INHERITED::onQuery(evt);
}

void TextArtView::onDrawContent(SkCanvas* canvas)
{
	const std::string TEXT = "Hello TextArt Effects";

	if (!initialized)
	{
		initialized = true;

		WarpFrameT	warpFrame;
		SkMatrix	warpMatrix;
		warpMatrix.setIdentity();

		getWarpParams(warpFrame, warpMatrix);

		bSkeleton_ = warpFrame[0];
		if (warpFrame.size()>1)
			tSkeleton_ = warpFrame[1];

		SkTypeface* typeface = SkTypeface::CreateFromName("Georgia", SkTypeface::kBold);

		TextArt::EnvelopeWarp textArt(bSkeleton_, warpMatrix);	
		textArt.setTopSkeleton(tSkeleton_);
		
		path_ = textArt.warp(TEXT, typeface);
		textBounds_ = textArt.getBounds();
	}

    SkPaint paint;
    paint.setAntiAlias(true);
	paint.setTextSize(SkIntToScalar(64));
    paint.setStyle(SkPaint::kStroke_Style);
//	paint.setTypeface(typeface);

	canvas->save();

	//center path on the cnavas
	const SkRect& pathBounds = path_.getBounds();
	SkISize screenSize = canvas->getDeviceSize();

	SkScalar xOffset = SkScalarHalf( SkIntToScalar(screenSize.width()) ) - pathBounds.centerX();
	SkScalar yOffset = SkScalarHalf( SkIntToScalar(screenSize.height()) ) - pathBounds.centerY();

	canvas->translate(xOffset, yOffset);

	paint.setColor(0x33FF3333);
//	canvas->drawRect(pathBounds, paint);

//	canvas->drawRect(bBounds, paint);
//	canvas->drawRect(tBounds, paint);


/*
	canvas->drawRect(textArt.getBounds(), paint);

	paint.setColor(SK_ColorGREEN);
	canvas->drawPath(textArt.bSkeleton_, paint);
	canvas->drawPath(textArt.bWarped_, paint);

	paint.setColor(SK_ColorCYAN);
	canvas->drawPath(textArt.tSkeleton_, paint);
	canvas->drawPath(textArt.tWarped_, paint);
*/
	//draw the skeleton path
	paint.setColor(SK_ColorBLUE);
	canvas->drawPath(bSkeleton_, paint);
	canvas->drawPath(tSkeleton_, paint);

	//draw the path
	paint.setColor(SK_ColorBLACK);
	paint.setStyle(SkPaint::kFill_Style);
	canvas->drawPath(path_, paint);

/*
	SkRect b;
	paint.setColor(SK_ColorRED);
	canvas->drawText(TEXT.c_str(), TEXT.size(), 0, 0, paint);
	paint.measureText(TEXT.c_str(), TEXT.size(), &b);
	canvas->drawRect(b, paint);
*/

	canvas->restore();
}

SkView::Click* TextArtView::onFindClickHandler(SkScalar x, SkScalar y) {
    fHints += 1;
    this->inval(NULL);
    return this->INHERITED::onFindClickHandler(x, y);
}

bool TextArtView::onClick(Click* click) {
    return this->INHERITED::onClick(click);
}
