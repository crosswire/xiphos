/*  
 * gtksword_cbs.cpp  - Fri Oct 26 11:15:54 2001
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "gtksword.h"
#include "gtksword_cbs.h"

static GList *pixmaps_directories = NULL;

int
main (int argc, char *argv[])
{
  GtkWidget *mainWindow;

  gtk_set_locale ();
  gtk_init (&argc, &argv);
	add_pixmap_directory("/usr/share/gtksword/pixmaps");
  /*
   * The following code was added by Glade to create one of each component
   * (except popup menus), just so that you see something after building
   * the project. Delete any components that you don't want shown initially.
   */
  mainWindow = create_mainWindow ();
  gtk_widget_show (mainWindow);

  gtk_main ();

  destroy_mainWindow();		
  return 0;
}


void
on_Exit3_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gtk_exit(0);
}


void
on_Help_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


/* This is a dummy pixmap we use when a pixmap can't be found. */
static char *dummy_pixmap_xpm[] = {
/* columns rows colors chars-per-pixel */
"1 1 1 1",
"  c None",
/* pixels */
" "
};

/* This is an internally used function to create pixmaps. */
static GtkWidget*
create_dummy_pixmap                    (GtkWidget       *widget)
{
  GdkColormap *colormap;
  GdkPixmap *gdkpixmap;
  GdkBitmap *mask;
  GtkWidget *pixmap;

  colormap = gtk_widget_get_colormap (widget);
  gdkpixmap = gdk_pixmap_colormap_create_from_xpm_d (NULL, colormap, &mask,
                                                     NULL, dummy_pixmap_xpm);
  if (gdkpixmap == NULL)
    g_error ("Couldn't create replacement pixmap.");
  pixmap = gtk_pixmap_new (gdkpixmap, mask);
//  gdk_pixmap_unref (gdkpixmap);
 // gdk_bitmap_unref (mask);
  return pixmap;
}

/* This is an internally used function to check if a pixmap file exists. */
static gchar*
check_file_exists                      (const gchar     *directory,
                                        const gchar     *filename)
{
  gchar *full_filename;
  struct stat s;
  gint status;

  full_filename = (gchar*) g_malloc (strlen (directory) + 1
                                     + strlen (filename) + 1);
  strcpy (full_filename, directory);
  strcat (full_filename, G_DIR_SEPARATOR_S);
  strcat (full_filename, filename);

  status = stat (full_filename, &s);
  if (status == 0 && S_ISREG (s.st_mode))
    return full_filename;
  g_free (full_filename);
  return NULL;
}



/* Use this function to set the directory containing installed pixmaps. */
void
add_pixmap_directory                   (const gchar     *directory)
{
  pixmaps_directories = g_list_prepend (pixmaps_directories,
                                        g_strdup (directory));
}

/* This is an internally used function to create pixmaps. */
GtkWidget*
create_pixmap                          (GtkWidget       *widget,
                                        const gchar     *filename)
{
  gchar *found_filename = NULL;
  GdkColormap *colormap;
  GdkPixmap *gdkpixmap;
  GdkBitmap *mask;
  GtkWidget *pixmap;
  GList *elem;

  if (!filename || !filename[0])
      return create_dummy_pixmap (widget);

  /* We first try any pixmaps directories set by the application. */
  elem = pixmaps_directories;
  while (elem)
    {
      found_filename = check_file_exists ((gchar*)elem->data, filename);
      if (found_filename)
        break;
      elem = elem->next;
    }

  /* If we haven't found the pixmap, try the source directory. */
  if (!found_filename)
    {
      found_filename = check_file_exists ("../pixmaps", filename);
    }

  if (!found_filename)
    {
      g_warning ("Couldn't find pixmap file: %s", filename);
      return create_dummy_pixmap (widget);
    }

  colormap = gtk_widget_get_colormap (widget);
  gdkpixmap = gdk_pixmap_colormap_create_from_xpm (NULL, colormap, &mask,
                                                   NULL, found_filename);
  if (gdkpixmap == NULL)
    {
      g_warning ("Error loading pixmap file: %s", found_filename);
      g_free (found_filename);
      return create_dummy_pixmap (widget);
    }
  g_free (found_filename);
  pixmap = gtk_pixmap_new (gdkpixmap, mask);
//  gdk_pixmap_unref (gdkpixmap);
  //gdk_bitmap_unref (mask);
  return pixmap;
}


/* 
* This function is used to do a quick and dirty translation of the formating
* commands in the module descriptions into reasonable equivalents in
* plain ascii.  (The descriptions look like .rtf-lite, perhaps?)
* It takes a character pointer to the module description and
* returns a pointer to a new string that's been fixed up.  Ultimately, 
* a better solution should be developed, but this improves the 
* appearance of the descriptions significantly for now.
*
* Perhaps this could serve as the basis for a function that would take
* an input string and a text widget, then insert the string into the 
* widget in a formatted manner (good next step?);
*/

char *
str_fixup_format		(const char *string)
{
	char	*strNew;	
	int 	i,j;

	assert(string!=NULL);
	strNew = (char *) malloc(strlen(string));	// First, allocate space for the new string 
	assert(strNew!=NULL);
	if(strNew==NULL) return ((char *)NULL);	// Make sure we actually got it 

	for (i=0,j=0;i<strlen(string);i++) {		// Work through input string char at a time
	    if (string[i]=='\\') {			// Found backslash, probably a command
                           if (string[i+1]=='q') {			// Looks like an alignment command
                               if(string[i+2]=='c') {			// Want to center text 
		strNew[j]='\n';			// - make do with a new line	
		j++;	
		strNew[j]='\t';			//  - and a tab for now
		j++;
	             }
	              i=i+2;				// Assume command and skip past q and ?
	              continue;
	      }
	      else {					// if string[i+1] != 'q'
                             if(string[i+1]=='p' && string[i+2]=='a' && string[i+3]=='r') {	 // "\par" or "\pard"command?
		if(string[i+4]=='d') {		 // "\pard" command - ignore for now
		    i=i+4;				// Skip past the "pard"
		    continue;
                                     }
		else {				// "\par" command
		    strNew[j]='\n';			// - use a new line for now
		    j++;
		    i=i+3;				// Skip past the "par"
		    continue;
		}
	           }
	       }					// End of "else"
	    }					// End of "if(string[i]=='\\')
	    strNew[j]=string[i];			// Don't know what it is, so copy it
	    j++;	
	}					// End of "for"
	strNew[j]='\0';				// Terminate new string

//	fprintf(stderr, "strlen(string) = %i, strlen(strNew) = %i \n", strlen(string), strlen(strNew));	// Used for debugging
//	fprintf(stderr, "string = \"%s\"\n",string);						// Used for debugging
//	fprintf(stderr, "strNew = \"%s\"\n",strNew);						// Used for debugging

	assert(strlen(strNew)<=strlen(string));	// Nothing we have done should have lengthend it
	return(strNew);				// Return fixed-up string		
}

void
on_About_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
 	SWMgr *mainMgr;

	return;
}


void
on_lookupText_changed                  (GtkEditable     *editable,
                                        gpointer         user_data)
{
	gtkswordWindow->lookupTextChanged();
}


void
on_searchButton_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
	gtkswordWindow->searchButtonClicked();
}


void
on_resultList_selection_changed(GtkWidget *clist, gint row, gint column, GdkEventButton *event, gpointer data)
{
	g_warning("selection changed");
	gtkswordWindow->resultListSelectionChanged(clist, row, column, event, data);
}



void
on_verse_style1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

void
on_option_item_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gtkswordWindow->setGlobalOption((gchar *)user_data, GTK_CHECK_MENU_ITEM(menuitem)->active );
}


void
on_buttonPreVerse_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	gtkswordWindow->navigateButtonClicked(0);
}


void
on_buttonNextVerse_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
	gtkswordWindow->navigateButtonClicked(1);
}


void
on_notebookText_switch_page            (GtkNotebook     *notebook,
                                        GtkNotebookPage *page,
                                        gint             page_num,
                                        gpointer         user_data)
{
	gtkswordWindow->changeTextMod(page_num);
}


void
on_notebookCommentary_switch_page      (GtkNotebook     *notebook,
                                        GtkNotebookPage *page,
                                        gint             page_num,
                                        gpointer         user_data)
{	
	gtkswordWindow->changeCommMod(page_num);
}


void
on_notebookDictionary_switch_page      (GtkNotebook     *notebook,
                                        GtkNotebookPage *page,
                                        gint             page_num,
                                        gpointer         user_data)
{
	gtkswordWindow->changeDictMod(page_num);
}


void
on_buttonSearchClose_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_mainWindow_destroy                  (GtkObject       *object,
                                        gpointer         user_data)
{
	gtk_exit(0);
}


void on_arrow1_button_press_event(GtkButton       *button, gpointer         user_data) {
	gtkswordWindow->navigateButtonClicked(0);
}


void on_arrow2_button_press_event(GtkButton       *button, gpointer         user_data) {
	gtkswordWindow->navigateButtonClicked(1);
}


void
on_ctreeBookmarks_tree_select_row      (GtkCTree        *ctree,
                                        GList           *node,
                                        gint             column,
                                        gpointer         user_data)
{

}


void
on_buttonBT1_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	gtkswordWindow->changeSidebarPage(0);
}


void
on_buttonCom1_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	gtkswordWindow->changeSidebarPage(1);
}


void
on_buttonDict1_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
	gtkswordWindow->changeSidebarPage(2);
}


void
on_buttonBM1_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	gtkswordWindow->changeSidebarPage(0);
}


void
on_buttonSearch1_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	gtkswordWindow->changeSidebarPage(1);
}


void
on_buttonSR1_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_buttonCom2_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	gtkswordWindow->changeSidebarPage(1);
}


void
on_buttonDict2_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
	gtkswordWindow->changeSidebarPage(2);
}


void
on_buttonBM2_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	gtkswordWindow->changeSidebarPage(3);
}


void
on_buttonSearch2_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	gtkswordWindow->changeSidebarPage(1);
}


void
on_buttonSR2_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	gtkswordWindow->changeSidebarPage(2);
}


void
on_buttonSBitem_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
	g_warning("number = %d",GPOINTER_TO_INT(user_data));
	gtkswordWindow->changeTextMod(GPOINTER_TO_INT(user_data));
	//gtkswordWindow->SideBarChangeMod((gchar *) user_data);
}


void
on_entryDictionaryKey_changed          (GtkEditable     *editable,
                                        gpointer         user_data)
{
	gchar *entryText;
	
	entryText = (gchar*)gtk_entry_get_text(GTK_ENTRY(gtkswordWindow->entryDictionaryKey));
	gtkswordWindow->dictSearchTextChangedSWORD(entryText);
}

void
on_clistKeys_select_row                (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	gchar *text;
	
	gtk_clist_get_text(clist,
                                        row,
                                        column,
                                        &text);	
	gtk_entry_set_text (GTK_ENTRY(gtkswordWindow->entryDictionaryKey), 
					text);
}

