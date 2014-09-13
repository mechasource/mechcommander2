/*************************************************************************************************\
LogisticsDialog.h			: Interface for the LogisticsDialog component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef LOGISTICSDIALOG_H
#define LOGISTICSDIALOG_H

//#include <mechgui/logisticsscreen.h>
//#include <mechgui/aanim.h>
//#include <mechgui/alistbox.h>

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
LogisticsDialog:
**************************************************************************************************/
class LogisticsDialog: public LogisticsScreen
{
public:

	virtual void		render(void);
	virtual void		update(void);

	void begin(void);
	void end(void);

	bool				isDone()
	{
		return bDone;
	}

	void	setFont(int32_t newFontResID);

	LogisticsDialog(void);




protected:

	aAnimation	enterAnim;
	aAnimation	exitAnim;
	bool		bDone;
	int32_t								oldFont;

};

class LogisticsOKDialog : public LogisticsDialog
{
public:

	static LogisticsOKDialog* instance()
	{
		return s_instance;
	}


	LogisticsOKDialog(void);


	virtual int32_t			handleMessage(uint32_t, uint32_t);

	static int32_t init(FitIniFile& file);

	void				setText(int32_t textID, int32_t CancelButton, int32_t OKButton);
	void				setText(PCSTR mainText);



private:

	LogisticsOKDialog(const LogisticsOKDialog& src);
	virtual ~LogisticsOKDialog(void);
	LogisticsOKDialog& operator=(const LogisticsOKDialog& ogisticsDialog);


	static LogisticsOKDialog*		s_instance;

	friend void endDialogs(void);
};

class LogisticsOneButtonDialog : public LogisticsDialog
{
public:

	static LogisticsOneButtonDialog* instance()
	{
		return s_instance;
	}


	LogisticsOneButtonDialog(void);


	virtual int32_t			handleMessage(uint32_t, uint32_t);

	static int32_t init(FitIniFile& file);

	void				setText(int32_t textID, int32_t CancelButton, int32_t OKButton);
	void				setText(PCSTR mainText);



protected:

	LogisticsOneButtonDialog(const LogisticsOneButtonDialog& src);
	virtual ~LogisticsOneButtonDialog(void);
	LogisticsOneButtonDialog& operator=(const LogisticsOneButtonDialog& ogisticsDialog);


	static LogisticsOneButtonDialog*		s_instance;

	friend void endDialogs(void);
};

class LogisticsLegalDialog : public LogisticsOneButtonDialog
{
public:

	static LogisticsLegalDialog* instance()
	{
		return s_instance;
	}
	static int32_t init(FitIniFile& file);

	LogisticsLegalDialog() {}

private:

	LogisticsLegalDialog(const LogisticsLegalDialog& src);
	virtual ~LogisticsLegalDialog() {}
	LogisticsLegalDialog& operator=(const LogisticsLegalDialog& ogisticsDialog);

	static LogisticsLegalDialog*		s_instance;

	friend void endDialogs(void);

};



class LogisticsSaveDialog : public LogisticsDialog
{
public:

	static LogisticsSaveDialog* instance()
	{
		return s_instance;
	}


	LogisticsSaveDialog(void);

	virtual void begin(void);
	virtual void beginLoad(void);
	void		 beginCampaign(void);

	virtual void end(void);
	virtual void update(void);
	virtual void render(void);

	static int32_t init(FitIniFile& file);
	virtual int32_t			handleMessage(uint32_t, uint32_t);

	const EString& getFileName()
	{
		return selectedName;
	}



private:

	LogisticsSaveDialog(const LogisticsSaveDialog& src);
	virtual ~LogisticsSaveDialog(void);
	LogisticsSaveDialog& operator=(const LogisticsSaveDialog& ogisticsDialog);

	static LogisticsSaveDialog*		s_instance;

	aListBox	gameListBox;

	EString		selectedName;

	void	initDialog(PCSTR path, bool bCampaign);
	void	updateCampaignMissionInfo(void);
	void	setMission(PCSTR path);
	void	readCampaignNameFromFile(PSTR fileName, PSTR resultName, int32_t len);
	bool	isCorrectVersionSaveGame(PSTR fileName);



	bool	bPromptOverwrite;
	bool	bDeletePrompt;
	bool	bLoad;
	bool	bCampaign;

	void updateMissionInfo(void);
	aLocalizedListItem templateItem;


	friend void endDialogs(void);

};

class LogisticsVariantDialog : public LogisticsDialog
{
public:

	LogisticsVariantDialog(void);
	~LogisticsVariantDialog(void);
	static LogisticsVariantDialog* instance()
	{
		return s_instance;
	}

	virtual void begin(void);
	void	beginTranscript(void);
	virtual void end(void);
	virtual void update(void);
	virtual void render(void);

	int32_t init(FitIniFile& file);
	virtual int32_t			handleMessage(uint32_t, uint32_t);

	const EString& getFileName()
	{
		return selectedName;
	}



protected:

	static LogisticsVariantDialog*		s_instance;
	aListBox	gameListBox;
	EString		selectedName;

	bool	bPromptOverwrite;
	bool	bDeletePrompt;
	bool	bTranscript;


	void	initVariantList(void);
	void	initTranscript(void);


	aAnimTextListItem templateItem;

	friend void endDialogs(void);
};

class LogisticsAcceptVariantDialog : public LogisticsVariantDialog
{
public:
	LogisticsAcceptVariantDialog(void);
	~LogisticsAcceptVariantDialog(void);
	virtual void begin(void);
	virtual void end(void);
	virtual void update(void);
	virtual void render(void);

	int32_t init(FitIniFile& file);
	virtual int32_t			handleMessage(uint32_t, uint32_t);

private:

	EString		selectedName;
	bool		bNameUsedPrompt;

};

class LogisticsMapInfoDialog : public LogisticsDialog
{
public:
	LogisticsMapInfoDialog(void);
	~LogisticsMapInfoDialog(void);
	virtual void end(void);

	void setMap(PCSTR pFileName);

	int32_t init();
	virtual int32_t			handleMessage(uint32_t, uint32_t);

private:



};

//*************************************************************************************************
#endif  // end of file ( LogisticsDialog.h )
