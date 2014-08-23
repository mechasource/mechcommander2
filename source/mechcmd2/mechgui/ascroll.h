//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef ASCROLL_H
#define ASCROLL_H

#include <gui/asystem.h>
#include <gui/abutton.h>

namespace mechgui {

	class mcScrollButton : 
		public aAnimButton
	{
	public:
		virtual void render();

		ULONG lightEdge;
		ULONG darkEdge;
		ULONG regularColor;
	};

	class aScrollBar : public aObject
	{
	public:

		aScrollBar();

		virtual int32_t	init(int32_t xPos, int32_t yPos, int32_t w, int32_t h);
		virtual void	destroy();
		virtual void	update();
		virtual void	render();

		virtual int		handleMessage( ULONG message, ULONG fromWho );

		void			SetScrollMax(float newMax);
		void			SetScrollPos(float newPos);
		float			GetScrollMax(void){return scrollMax;};
		float			GetScrollPos(void){return scrollPos;};
		int32_t			SetSrollInc( int32_t newInc ){ scrollInc = newInc; } // amount you move for one arrow click
		int32_t			SetScrollPage(int32_t newInc){ pageInc = newInc;} // amount you move if you click on the bar itself
		void			ScrollUp(void);
		void			ScrollPageUp(void);
		void			ScrollDown(void);
		void			ScrollPageDown(void);
		void			SetScroll( int32_t newScrollPos );	
		void			Enable( bool enable );

	protected:

		float			scrollMax;
		float			scrollPos;
		aAnimButton			topButton;
		aAnimButton			bottomButton;
		mcScrollButton		scrollTab;

		int32_t			lastY;
		int32_t			scrollInc;
		int32_t			pageInc;
		ULONG   color;
		void			ResizeAreas(void);
	};

	class mcScrollBar : public aScrollBar
	{
	public:
		int32_t init ( int32_t xPos, int32_t yPos, int32_t w, int32_t h  );
		virtual void	resize(int32_t w, int32_t h);

		void setOrange();
		void setGreen();

	private:

		aAnimation		orangeInfo[4];
		aAnimation		greenInfo[4];

	};

}

#endif
