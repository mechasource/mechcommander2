//===========================================================================//
//ChatWindow.h			: Interface for the ChatWindow component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef CHATWINDOW_H
#define CHATWINDOW_H

//#include <mechgui/logisticsscreen.h>
// #include <mechgui/alistbox.h>

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
ChatWindow:
**************************************************************************************************/

class ChatMessageItem : public aListItem
{
public:

	ChatMessageItem(void);
	void		 setPlayerColor(int32_t color);
	void		 setTextColor( int32_t color );
	void		 setPlayerName( PCSTR name );
	int32_t			 setText( PCSTR text ); // returns number of lines
	int32_t		 getLineCount() { return lineCount; }

private:
	aText		 name;
	aText		 playerText;
	aRect		 playerRect;
	int32_t		 lineCount;
};

class ChatWidget : public LogisticsScreen  // the one that obscures....
{
public:

	ChatWidget(void);
	virtual ~ChatWidget(void);

	void init(void);

private:

	friend class ChatWindow;
	aListBox		listBox;

	ChatMessageItem	listItems[128];
	int32_t			curItem;


};

class ChatWindow: public LogisticsScreen
{
	public:

		ChatWindow(void);
		virtual ~ChatWindow(void);

		static void init(void);
		static void destroy(void);
		static ChatWindow* instance() { return s_instance; }

		int32_t initInstance(void);

		virtual void update(void);
		virtual void render( int32_t xOffset, int32_t yOffset );
		int32_t handleMessage( uint32_t, uint32_t );

		virtual bool pointInside( int32_t xPos, int32_t yPos );
		bool		 isExpanded(void);


	private:

		static ChatWindow*	s_instance;

		aListBox		listBox;

		aEdit			chatEdit;
		ChatWidget		chatWidget;

		ChatMessageItem	listItems[4];
		int32_t			curItem;
		int32_t			maxItems;


		static void refillListBox( aListBox& listBox, PSTR* chatTexts, int32_t* playerIDs,  ChatMessageItem* pItems, 
			int32_t& curItem, int32_t itemCount, int32_t maxCount );





};






//*************************************************************************************************
#endif  // end of file ( ChatWindow.h )
