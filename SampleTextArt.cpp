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
	if (!initialized)
	{
		initialized = true;

		WarpFrameT	warpFrame;
		SkMatrix	warpMatrix;
		warpMatrix.setIdentity();

		getWarpParams(warpFrame, warpMatrix);

		bSkeleton_ = warpFrame[0];

		SkTypeface* typeface = SkTypeface::CreateFromName("Georgia", SkTypeface::kBold);

		TextArt::EnvelopeWarp textArt(bSkeleton_, warpMatrix);	

		path_ = textArt.warp("Hello TextArt Effects", typeface);
	}

    SkPaint paint;
    paint.setAntiAlias(true);
	paint.setTextSize(SkIntToScalar(64));
    paint.setStyle(SkPaint::kStroke_Style);

	canvas->save();

	//center path on the cnavas
	const SkRect& pathBounds = path_.getBounds();
	SkISize screenSize = canvas->getDeviceSize();

	SkScalar xOffset = SkScalarHalf( SkIntToScalar(screenSize.width()) ) - pathBounds.centerX();
	SkScalar yOffset = SkScalarHalf( SkIntToScalar(screenSize.height()) ) - pathBounds.centerY();

	canvas->translate(xOffset, yOffset);

	paint.setColor(0x33FF3333);
	canvas->drawRect(pathBounds, paint);

	//draw the skeleton path
	paint.setColor(SK_ColorGREEN);
	canvas->drawPath(bSkeleton_, paint);

	//draw the path
	paint.setColor(SK_ColorBLACK);
	paint.setStyle(SkPaint::kFill_Style);
	canvas->drawPath(path_, paint);
	
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
