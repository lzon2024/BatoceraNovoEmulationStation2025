#include "components/ScrollbarComponent.h"

#define VISIBLE_TIME 1500
#define FADEOUT_TIME 350

ScrollbarComponent::ScrollbarComponent(Window* window) : GuiComponent(window),
	mMin(0), mMax(0), mPage(1), mPosition(0), mVisibleTime(0), mFadeOutTime(0), mCornerSize(0.01), mScrollSize(0.008), mColor(0), mVertical(true)
{
	
}

void ScrollbarComponent::setContainerBounds(Vector3f position, Vector2f size, bool vertical)
{
	mVertical = vertical;

	if (mVertical)
	{
		int width = Renderer::getScreenWidth() * mScrollSize;
		setPosition(position.x() + size.x() - width, position.y());
		setSize(width, size.y());
	}
	else
	{
		int height = Renderer::getScreenHeight() * mScrollSize;

		setPosition(position.x(), position.y() + size.y() - height);
		setSize(size.x(), height);
	}
}

void ScrollbarComponent::onCursorChanged()
{
	mVisible = true;	
	mVisibleTime = VISIBLE_TIME;
	mFadeOutTime = 0;
}

void ScrollbarComponent::setScrollPosition(float position)
{
	if (mPosition == position)
		return;

	mPosition = position;
	onCursorChanged();
}

void ScrollbarComponent::update(int deltaTime)
{
	GuiComponent::update(deltaTime);

	if (mFadeOutTime > 0)
	{
		mFadeOutTime -= deltaTime;
		if (mFadeOutTime <= 0)
		{
			mVisible = false;
			mFadeOutTime = 0;
		}
	}

	if (mVisibleTime > 0)
	{
		mVisibleTime -= deltaTime;

		if (mVisibleTime <= 0)
		{
			mFadeOutTime = FADEOUT_TIME;
			mVisibleTime = 0;
		}
	}
}

void ScrollbarComponent::render(const Transform4x4f& parentTrans)
{
	if (!mVisible || mMin == mMax || mColor == 0)
		return;

	if (mPage >= mMax)
		return;

	Transform4x4f trans = parentTrans * getTransform();

	if (!Renderer::isVisibleOnScreen(trans.translation().x(), trans.translation().y(), mSize.x(), mSize.y()))
		return;

	Renderer::setMatrix(trans);

	GuiComponent::renderChildren(trans);

	int minHeight = (mVertical ? Renderer::getScreenWidth() * 0.01 : Renderer::getScreenHeight() * 0.01);


	int reference = mVertical ? getSize().y() : getSize().x();
	int thumbHeight = (reference * mPage) / (mMax + 1);
	if (thumbHeight < minHeight)
		thumbHeight = minHeight;

	float radius = (mVertical ? mSize.x() : mSize.y()) * mCornerSize;
	
	float top = 0;	
	if (((mMax - mPage) + 1) > 0)
		top = ((reference - thumbHeight) * mPosition) / ((mMax - mPage) + 1);
	
	int bottom = Math::min( mVertical ? getSize().y() : getSize().x(), top + thumbHeight);

	float opacity = mFadeOutTime == 0 ? 1.0 : (float)mFadeOutTime / (float)FADEOUT_TIME;
	
	const unsigned int color = (mColor & 0xFFFFFF00) | (unsigned char)((mColor & 0xFF) * opacity);

	// Renderer::drawRect(0, 0, mSize.x(), mSize.y(), 0xFF0000FF);

	if (mVertical)
	{
		if (mCornerSize == 0 || mSize.x() < radius / 2.0)
			Renderer::drawRect(0, top, mSize.x(), bottom - top, color);
		else
			Renderer::drawRoundRect(0, top, mSize.x(), bottom - top, radius, color);
	}
	else
	{
		if (mCornerSize == 0 || mSize.x() < radius / 2.0)
			Renderer::drawRect(top, 0, bottom - top, mSize.y(), color);
		else
			Renderer::drawRoundRect(top, 0, bottom - top, mSize.y(), radius, color);
	}

}