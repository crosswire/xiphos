/* 
 * gtksword.h  - Fri Oct 26 11:15:54 2001
 * copyright (C) 2001 CrossWire Bible Society
 *			P. O. Box 2528
 *			Tempe, AZ  85280-2528
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <swmgr.h>
#include <swdisp.h>


GtkWidget *create_mainWindow(void);
void destroy_mainWindow(void);

class GTKEntryDisp:public SWDisplay {
      protected:
	GtkWidget * gtkText;
      public:
	static GdkColor colourGreen;
	static GdkColor colourBlue;
	static void __initialize() {
		GdkColormap *cmap;

		 cmap = gdk_colormap_get_system();
		 colourGreen.red = 0x0000;
		 colourGreen.green =  0xbbbb;
		 colourGreen.blue = 0x0000;
		 colourBlue.red = 0;
		 colourBlue.green = 0;
		 colourBlue.blue = 0xffff;
		if (!gdk_colormap_alloc_color(cmap, &colourGreen, true, true)) {
			g_error("couldn't allocate colour");
		}
		if (!gdk_colormap_alloc_color(cmap, &colourBlue, true, true)) {
			g_error("couldn't allocate colour");
		}
	}
	GTKEntryDisp(GtkWidget * gtkText) {
		this->gtkText = gtkText;
	}
	virtual char Display(SWModule & imodule);
	gint gettags(gchar * text, gchar *tag, gint pos);
	void applyTags(GtkTextBuffer *buffer, GtkTextIter iter, GString *str, bool bcurrentVerse);

};

class GTKChapDisp:public GTKEntryDisp {
      public:
	GTKChapDisp(GtkWidget * gtkText):GTKEntryDisp(gtkText) {
	} virtual char Display(SWModule & imodule);
};

//----------------------------------------------------------------------------------------------
class HTMLChapDisp:public GTKEntryDisp {	
      public:
	HTMLChapDisp(GtkWidget * gtkText):GTKEntryDisp(gtkText) {
	} virtual char Display(SWModule & imodule);
};

class MainWindow {
	GtkWidget *mainWindow;
	GtkWidget *mainPanel;
	GtkWidget *headerPanel;
	GtkWidget *handlebox3;
	GtkWidget *menubar4;
	GtkWidget *File3;
	GtkWidget *File3_menu;
	GtkAccelGroup *File3_menu_accels;
	GtkWidget *Exit3;
	GtkWidget *options1;
	GtkWidget *options1_menu;
	GtkAccelGroup *options1_menu_accels;
	guint tmp_key;
	GtkWidget *Help;
	GtkWidget *Help_menu;
	GtkAccelGroup *Help_menu_accels;
	GtkWidget *About;
	GtkWidget *handlebox4;
	GtkWidget *toolbar1;
	GtkWidget *hbox6;
	GtkWidget *label2;
	GtkWidget *lookupText;

	GtkWidget *hpaned3;
	GtkWidget *frameSidebar;
	GtkWidget *vbox2;
	GtkWidget *scrolledwindow3;
	GtkWidget *viewport1;
	GtkWidget *toolbarBibleText;
	GtkWidget *button13;
	GtkWidget *label10;
	GtkWidget *scrolledwindow4;
	GtkWidget *viewport2;
	GtkWidget *toolbar3;
	GtkWidget *button14;
	GtkWidget *label11;
	GtkWidget *scrolledwindow5;
	GtkWidget *viewport3;
	GtkWidget *toolbar4;
	GtkWidget *button15;
	GtkWidget *label12;
	GtkWidget *scrolledwindow6;
	GtkWidget *viewport4;
	GtkWidget *ctreeBookmarks;
	GtkWidget *label16;
	GtkWidget *label17;
	GtkWidget *label18;
	GtkWidget *label13;
	GtkWidget *scrolledwindow7;
	GtkWidget *viewport5;
	GtkWidget *searchPanel;
	GtkWidget *hbox4;
	GtkWidget *searchText;
	GtkWidget *searchButton;
	GtkWidget *searchSettingsPanel;
	GtkWidget *searchTypePanel;
	GSList *searchType_group;
	GtkWidget *regexSearch;
	GtkWidget *phraseSearch;
	GtkWidget *multiWordSearch;
	GtkWidget *caseSensitive;
	GtkWidget *searchOptionsPanel;
	GtkWidget *label14;
	GtkWidget *vbox3;
	GtkWidget *labelNumFound;
	GtkWidget *labelSearchText;
	GtkWidget *label22;
	GtkWidget *scrolledwindow9;
	GtkWidget *resultList;
	GtkWidget *label19;
	GtkWidget *scrolledwindow10;
	GtkWidget *textSRPreview;
	GtkWidget *empty_notebook_page;
	GtkWidget *label15;
	GtkWidget *vpaned1;
	GtkWidget *hbox2;
	GtkWidget *hpaned1;
	GtkWidget *notebookText;
	GtkWidget *label3;
	GtkWidget *notebookCommentary;
	GtkWidget *label7;
	GtkWidget *hpaned2;
	GtkWidget *notebookDictionary;
	GtkWidget *label8;
	GtkWidget *vbox1;
	GtkWidget *scrolledwindow2;
	GtkWidget *clistKeys;
	GtkWidget *label9;
	GtkWidget *vbox200;

	GtkWidget *statusbar1;
	GtkTooltips *tooltips;
	GtkWidget *arrow1;
	GtkWidget *arrow2;

	int textcount;
	int commcount;
	int dictcount;
	
	SWDisplay *chapDisplay;
	SWDisplay *entryDisplay;

      public:
	 MainWindow();
	~MainWindow();
	SWModule *curMod;
	SWModule *curcommMod;
	SWModule *curdictMod;
	SWMgr *mainMgr;
	GtkWidget *create();
	GtkWidget *entryDictionaryKey;
	GtkWidget *verse_style1;
	GtkWidget *buttonBT1;
	GtkWidget *buttonCom1;
	GtkWidget *buttonDict1;
	GtkWidget *buttonBM1;
	GtkWidget *buttonSearch1;
	GtkWidget *buttonSR1;
	GtkWidget *notebookSidebar;
	GtkWidget *buttonCom2;
	GtkWidget *buttonDict2;
	GtkWidget *buttonBM2;
	GtkWidget *buttonSearch2;
	GtkWidget *buttonSR2;
	void initSWORD();
	void setGlobalOption(gchar * option, gboolean choice);
	void changeVerse(gint verse);
	void lookupTextChanged();
	void viewModActivate(gchar * modName);
	void changeTextMod(gint modNum);
	void changeCommMod(gint modNum);
	void changeDictMod(gint modNum);
	void searchButtonClicked();
	void navigateButtonClicked(int direction);
	//gint tag_event_handler (GtkTextTag *tag, GtkWidget *widget, GdkEvent *event,
        //          const GtkTextIter *iter, gpointer user_data);
	void setup_tag (GtkTextTag *tag, gpointer user_data);
	void CreateTextTags(GtkTextBuffer *buffer);
	void CreateTextPane(SWModule * mod, GtkWidget * notebook);
	void CreateCommentaryPane(SWModule * mod, GtkWidget * notebook);
	void CreateDictionaryPane(SWModule * mod, GtkWidget * notebook);
	void FillDictKeysSWORD(gchar * modName);
	void dictSearchTextChangedSWORD(char *mytext);
	void changeSidebarPage(int page);
	void resultListSelectionChanged(GtkWidget * clist, gint row, gint column, GdkEventButton * event, gpointer data);
	void SideBarChangeMod(gchar * modName);

	SWMgr *getSwordManager(void) {
		return (mainMgr);
	};
};
extern MainWindow *gtkswordWindow;
