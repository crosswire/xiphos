/*
* this code is taken form gedit-0.7.9
* I only changed a few things to make it work with gnomesword
* Terry
*/

/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * gedit
 *
 * Copyright (C) 2000 Jose Maria Celorio
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 * Authors:
 *   Chema Celorio <chema@celorio.com>
 */ 


#include <config.h>
#include <gnome.h>

#include "printstuff.h"
#include "support.h"
#include "dialogs.h"

#include <libgnomeprint/gnome-printer.h>
#include <libgnomeprint/gnome-print.h>
#include <libgnomeprint/gnome-printer-dialog.h>

#include <libgnomeprint/gnome-print-master.h>
#include <libgnomeprint/gnome-print-master-preview.h>
#include <libgnomeprint/gnome-print-preview.h>

GtkWidget *text1;
gchar *currentfilename;

typedef struct _PrintJobInfo {
	/* gnome print stuff */
  GnomePrintMaster *master;
	GnomePrintContext *pc;
	const GnomePaper *paper;

	/* document stuff */
	gchar *doc;
	guchar *buffer;
//	View *view;
	guint buffer_size;
	guchar *filename;
	
	/* Font stuff */ 
	guchar *font_name;
	gint    font_size;
	float   font_char_width;
	float   font_char_height;

	/* Page stuff */ 
	guint   pages;
	float   page_width, page_height;
	float   margin_top, margin_bottom, margin_left, margin_right, margin_numbers;
	float   printable_width, printable_height;
	float   header_height;
	guint   total_lines, total_lines_real;
	guint   lines_per_page;
	guint   chars_per_line;
	guchar* temp;
	gboolean   orientation;

	/* Range stuff */
	gint range;
	gint print_first;
	gint print_last;
	gint print_this_page : 1;
	gint preview : 1;
	
	/* buffer stuff */
	gint file_offset;
	gint current_line;

	/* Text Wrapping */
	gint wrapping : 1;
	gint tab_size;
} PrintJobInfo;


//        void file_print_cb (GtkWidget *widget, gpointer data, gint file_printpreview);
//        void file_print_preview_cb (GtkWidget *widget, gpointer data);
static  void print_document (gchar *doc, PrintJobInfo *pji, GnomePrinter *printer);
static  void print_line (PrintJobInfo *pji, int line);
static  void print_ps_line(PrintJobInfo * pji, gint line, gint first_line);
static guint print_determine_lines (PrintJobInfo *pji, int real);
static  void print_header (PrintJobInfo *pji, unsigned int page);
static  void print_start_job (PrintJobInfo *pji);
static  void print_set_orientation (PrintJobInfo *pji);
static  void print_header (PrintJobInfo *pji, unsigned int page);
static  void print_setfont (PrintJobInfo *pji);
static  void print_end_page (PrintJobInfo *pji);
static  void print_end_job (GnomePrintContext *pc);
static  void preview_destroy_cb (GtkObject *obj, PrintJobInfo *pji);
static  void print_pji_destroy (PrintJobInfo *pji);
static  void print_set_pji ( PrintJobInfo * pji, gchar *doc);
static  void file_print_preview_cb (GtkWidget *widget, gpointer data);

/**
 * file_print:
 * @widget: 
 * @data: 
 * @file_printpreview: FALSE if print_preview is needed.
 * 
 * Starts the printing process and creates a print dialog box.
 * This should be the only routine global to the world (and Print Preview_cb)
 *
 **/
void
file_print(GtkWidget *widget, gchar *current_filename, gint file_printpreview)
{
	GtkWidget *dialog;
	GnomePrinter *printer;
	PrintJobInfo *pji;
	gchar* doc;
		
 	text1 = lookup_widget(widget,"text3");
  	currentfilename = g_strdup(current_filename);

	doc =gtk_editable_get_chars (GTK_EDITABLE (text1), 0, -1);

	
	if ( doc == NULL)
		return;

	pji = g_new0 (PrintJobInfo, 1);
	pji->paper = gnome_paper_with_name ("US-Letter");     //settings->papersize
	g_return_if_fail (pji->paper != NULL);
	/* Do we need to call master new with orientation ???
	   I guess so, but it is printing fine now. Chema*/
	pji->master = gnome_print_master_new();
	gnome_print_master_set_paper( pji->master, pji->paper);
	pji->pc = gnome_print_master_get_context(pji->master);
	g_return_if_fail(pji->pc != NULL);
	
	/* We need to calculate the number of pages before running
	   the dialog, so load pji with info now */
	print_set_pji (pji, doc);

	/* file_print preview is a false when preview is requested */ 
	if (file_printpreview)
	{
		dialog = gnome_print_dialog_new ( (const char *)"Print Document", GNOME_PRINT_DIALOG_RANGE);
		gnome_print_dialog_construct_range_page ( (GnomePrintDialog * )dialog,
							  GNOME_PRINT_RANGE_ALL | GNOME_PRINT_RANGE_RANGE,
							  1, pji->pages, "A",
							  _("Pages")/* Translators: As in [Range] Pages from:[x]  to*/);

		switch (gnome_dialog_run (GNOME_DIALOG (dialog)))
		{
		case GNOME_PRINT_PRINT:
			break;
		case GNOME_PRINT_PREVIEW:
			pji->preview = TRUE;
			break;
		case -1:
			print_pji_destroy (pji);
			return;
		default:
			print_pji_destroy (pji);
			gnome_dialog_close (GNOME_DIALOG (dialog));
			return;
		}

		printer = gnome_print_dialog_get_printer (GNOME_PRINT_DIALOG (dialog));

		pji->print_first =0;
		pji->print_last =0;
		pji->range = gnome_print_dialog_get_range_page ( GNOME_PRINT_DIALOG (dialog), &pji->print_first, &pji->print_last);
		
		gnome_dialog_close (GNOME_DIALOG (dialog));
	}
	else
	{
		pji->print_first = 1;
		pji->print_last = pji->pages;
		pji->preview = TRUE;
		printer = NULL;
	}

	if (pji->preview)
	{   /*
		GnomePrintMasterPreview *preview;
		gchar *title;

		print_document (doc, pji, NULL);
		title = g_strdup_printf (_("GnomeSword (%s): Print Preview"), pji->filename);
		preview = gnome_print_master_preview_new_with_orientation (pji->master, title, !pji->orientation);
		g_free (title);
		gtk_signal_connect (GTK_OBJECT(preview), "destroy",
				    GTK_SIGNAL_FUNC(preview_destroy_cb), pji);
		gtk_widget_show(GTK_WIDGET(preview)); */
	}
	else
	{
		if (printer)
			gnome_print_master_set_printer(pji->master, printer);
		print_document (doc, pji, printer);
		gnome_print_master_print (pji->master);
	}
	
	print_pji_destroy (pji);
	
/*	FIXME : we need to set the parent of the dialog to be the active window,
	because of some window manager issues, that clahey told me about Chema*/
	/* see dialogs/dialog-replace.c for the solution. Chema */ 
#if 0	
//        gnome_dialog_set_parent (GNOME_DIALOG(dialog), GTK_WINDOW(mdi->active_window));
	gtk_widget_show_all (dialog);
#endif
}


/**
 * file_print_preview_cb:
 * @widget: 
 * @data: 
 * 
 * Callback for print previewing. We just call file_prin_cb
 * with the 3rd argument = FALSE;
 *
 **/
void
file_print_preview_cb (GtkWidget *widget, gpointer data)
{
	file_print(NULL, NULL, FALSE);
}


/**
 * print_document:
 * @doc: the document to be printed, we need this for doc->filename
 * @pji: the PrintJobInfo struct
 * @printer: the printer to do the printing to, NULL for printpreview
 * 
 * prints *doc
 *
 **/
static void
print_document (gchar *doc, PrintJobInfo *pji, GnomePrinter *printer)
{
	int current_page, current_line;
	
	
	//pji->chars_per_line = pji->chars_per_line + 2;
	pji->temp = (guchar *)g_malloc((guint)pji->chars_per_line + 2);

	current_line = 0;
	
	print_start_job (pji);
	
	for (current_page = 1; current_page <= pji->pages; current_page++)
	{
		if (pji->range != GNOME_PRINT_RANGE_ALL)
			pji->print_this_page = (current_page>=pji->print_first &&
						current_page<=pji->print_last) ? TRUE:FALSE;
		else
			pji->print_this_page = TRUE;

		/* FIXME : This pji->print_this_page is not top hacking. I need
		   to advance the pointer to buffer to the rigth place not to
		   semi-print the non-printable pages. Chema */
		if (pji->print_this_page)
		{
			/* We need to call gnome_print_beginpage so that it adds
			   "%% Page x" comment needed for viewing postcript files (i.e. in gv)*/
			gchar * pagenumbertext;
			pagenumbertext = g_strdup_printf ("%d", current_page);
			gnome_print_beginpage (pji->pc, pagenumbertext);
			g_free (pagenumbertext);

			/* Print the header of the page */
			if (1) //settings->print_header
				print_header(pji, current_page);
			print_setfont (pji);
		}

		/* we do this when the first line in the page is a continuation
		   of the last line in the previous page. This will prevent that
		   the line number in the previous page is repeated in the next*/
		if (pji->buffer [pji->file_offset-1] != '\n' && current_page>1 && pji->wrapping)
			current_line--;

		for ( current_line++; current_line <= pji->total_lines; current_line++)
		{
			print_line (pji, current_line);

			if (pji->current_line % pji->lines_per_page == 0)
				break;
		}

		if (pji->print_this_page)
			print_end_page (pji);
	}
	print_end_job (pji->pc);
	g_free (pji->temp);
		
	gnome_print_context_close (pji->pc);
	gnome_print_master_close (pji->master);

}

static void
print_line (PrintJobInfo *pji, int line)
{
	gint dump_info = FALSE;
	gint chars_in_this_line = 0;
	gint i, temp;
	gint first_line = TRUE;

	while( pji->buffer [pji->file_offset ] != '\n' && pji->file_offset < pji->buffer_size)
	{
		chars_in_this_line++;

		if (pji->buffer [pji->file_offset] == '\t')
		{
			temp = chars_in_this_line;

			chars_in_this_line += pji->tab_size - ( (chars_in_this_line-1) % pji->tab_size) - 1;

			if (chars_in_this_line > pji->chars_per_line + 1)
			    chars_in_this_line = pji->chars_per_line + 1;

			for (i=temp;i<chars_in_this_line+1;i++)
			{
				pji->temp [i-1] = ' ';
			}
		}
		else
		{
			pji->temp  [chars_in_this_line-1] = pji->buffer [pji->file_offset];
		}

		
		if (chars_in_this_line == pji->chars_per_line + 1)
		{
			if (!pji->wrapping)
			{
				/* We need to advance pji->file_offset until the next NL char */
				while( pji->buffer [pji->file_offset ] != '\n')
					pji->file_offset++;
				pji->file_offset--;

				pji->temp [chars_in_this_line] = (guchar) '\0';
				print_ps_line (pji, line, TRUE);
				pji->current_line++;
				chars_in_this_line = 0;
			}
			else
			{
				if (dump_info)
					g_print ("\nThis lines needs breaking\n");

				temp = pji->file_offset; /* We need to save the value of i in case we have to break the line */

				/* We back i till we find a space that we can break the line at */
				while (pji->buffer [pji->file_offset] != ' '  &&
				       pji->buffer [pji->file_offset] != '\t' &&
				       pji->file_offset > temp - pji->chars_per_line - 1 )
				{
					pji->file_offset--;
				}

				if (dump_info)
					g_print("file offset got backed up [%i] times\n", temp - pji->file_offset);

				/* If this line was "unbreakable" break it at chars_per_line width */
				if (pji->file_offset == temp - pji->chars_per_line - 1)
				{
					pji->file_offset = temp;
					if (dump_info)
						g_print ("We are breaking the line\n");
				}

				if (dump_info)
				{
					g_print ("Breaking temp at : %i\n", chars_in_this_line + pji->file_offset - temp - 1);
					g_print ("Chars_in_this_line %i File Offset %i Temp %i\n",
						 chars_in_this_line,
						 pji->file_offset,
						 temp);
				}
				pji->temp [ chars_in_this_line + pji->file_offset - temp - 1] = (guchar) '\0';
				print_ps_line (pji, line, first_line);
				first_line = FALSE;
				pji->current_line++;
				chars_in_this_line = 0;

				/* We need to remove the trailing blanks so that the next line does not start with
				   a space or a tab char */
				while (pji->buffer [pji->file_offset] == ' ' || pji->buffer [pji->file_offset] == '\t')
					pji->file_offset++;

				/* We need to back i 1 time because this is a for loop and we did not processed the
				   last character */
				pji->file_offset--;

				/* If this is the last line of the page return */
				if (pji->current_line%pji->lines_per_page == 0)
				{
					pji->file_offset++;
					return;
				}
			}
		}
	
		pji->file_offset++;
	}

	/* We need to terminate the string and print it */ 
	pji->temp [chars_in_this_line] = (guchar) '\0';
	print_ps_line (pji, line, first_line);
	pji->current_line++;

	/* We need to skip the newline char for the new line character */
	pji->file_offset++;

}

static void
print_ps_line (PrintJobInfo * pji, gint line, gint first_line)
{
	gfloat y;



	y = pji->page_height -  pji->margin_top - pji->header_height -
	(pji->font_char_height*( (pji->current_line % pji->lines_per_page)+1 ));

	if (!pji->print_this_page)
		return;
	
	gnome_print_moveto (pji->pc, pji->margin_left, y);
	gnome_print_show (pji->pc, (gchar *)pji->temp);
	/* Why is this here ??? Chema. */
	if ( pji->temp!='\0')
		gnome_print_stroke (pji->pc);
/*
	if (settings->print_lines>0 && line%settings->print_lines==0 && first_line)
	{
		char * number_text = g_strdup_printf ("%i",line);
		GnomeFont *temp_font = gnome_font_new (pji->font_name, 6);
		g_assert (pji->font_name!=NULL);
		
		gnome_print_setfont (pji->pc, temp_font);
		gnome_print_moveto (pji->pc, pji->margin_left - pji->margin_numbers, y);
		gnome_print_show   (pji->pc, number_text);
		g_free (number_text);
		print_setfont (pji);
	}
*/
}

static void
print_set_pji (PrintJobInfo * pji, gchar *doc)
{
	

//	pji->view = gedit_view_current();
	pji->doc = doc;
	pji->buffer_size = gtk_text_get_length(GTK_TEXT(text1));
	pji->buffer = (guchar *)doc;

/*	if (doc->filename == NULL)
		pji->filename = g_strdup (_("Untitled")); 
	else
*/
		pji->filename = (guchar *)g_strdup (currentfilename);

	pji->orientation = PRINT_ORIENT_PORTRAIT;
	if (pji->orientation == PRINT_ORIENT_LANDSCAPE)
	{
		pji->page_width  = gnome_paper_psheight (pji->paper);
		pji->page_height = gnome_paper_pswidth (pji->paper);
	}
	else
	{
		pji->page_width  = gnome_paper_pswidth (pji->paper);
		pji->page_height = gnome_paper_psheight (pji->paper);
	}

	pji->margin_numbers = .25 * 72;
	pji->margin_top = .75 * 72;       /* Printer margins, not page margins */
	pji->margin_bottom = .75 * 72;    /* We should "pull" this from gnome-print when */
	pji->margin_left = .75 * 72;      /* gnome-print implements them */
//	if (settings->print_lines > 0)	pji->margin_left += pji->margin_numbers;
	pji->margin_right = .75 * 72;
	pji->header_height = 1 * 72; //settings->print_header
	pji->printable_width  = pji->page_width -
		                pji->margin_left -
		                pji->margin_right; //-((settings->print_lines>0)?pji->margin_numbers:0)
	pji->printable_height = pji->page_height -
		                pji->margin_top -
		                pji->margin_bottom;
	pji->font_name = (guchar *)"Courier"; /* FIXME: Use courier 10 for now but set to actual font */
	pji->font_size = 10;
	pji->font_char_width = 0.0808 * 72;
	pji->font_char_height = .14 * 72;
	pji->wrapping = TRUE; //settings->print_wrap_lines;
	pji->chars_per_line = (gint)(pji->printable_width / pji->font_char_width);
	pji->lines_per_page = (guint)(pji->printable_height -
			      pji->header_height) /(guint)pji->font_char_height -  1;
	pji->tab_size = 8;
	pji->total_lines = print_determine_lines(pji, FALSE);
	pji->total_lines_real = print_determine_lines(pji, TRUE);
	pji->pages = ((int) (pji->total_lines_real-1)/pji->lines_per_page)+1;
	pji->file_offset = 0;
	pji->current_line = 0;
}

/**
 * print_determine_lines: 
 * @pji: PrintJobInfo struct
 * @real: this flag determines if we count rows of text or lines
 * of rows splitted by wrapping.
 *
 * Determine the lines in the document so that we can calculate the pages
 * needed to print it. We need this in order for us to do page/pages
 *
 * Return Value: number of lines in the document
 *
 * The code for this function is small, but it is has a lot
 * of debuging code, remove later. Chema
 *
 **/
static guint
print_determine_lines (PrintJobInfo *pji, int real)
{
	gint lines=0;
	gint i, temp_i, j;
	gint chars_in_this_line = 0;

	/* use local variables so that this code can be reused */
	guchar * buffer = pji->buffer;
	gint chars_per_line = pji->chars_per_line;
	gint tab_size = pji->tab_size;
	gint wrapping = pji->wrapping;
	gint buffer_size = pji->buffer_size;
	gint lines_per_page = pji->lines_per_page; /* Needed for dump_text stuff */ 

	int dump_info = FALSE;
	int dump_info_basic = FALSE;
	int dump_text = FALSE;

	

	/* here we modify real if !pji->wrapping */
	if (real && !wrapping)
		real = FALSE;

	if (!real)
	{
		dump_info = FALSE;
		dump_info_basic = FALSE;
		dump_text = FALSE;
	}
		
	if (dump_info_basic)
	{
		if (real)
			g_print ("Determining lines in REAL mode. Lines Per page =%i\n", lines_per_page);
		else
			g_print ("Determining lines in WRAPPING mode. Lines Per page =%i\n", lines_per_page);
	}
	
	if (dump_text && lines%lines_per_page == 0)
		g_print("\n\n-Page %i-\n\n", lines / lines_per_page + 1);
	
	for (i=0; i < buffer_size; i++)
	{
		chars_in_this_line++;

		if (buffer[i] != '\t' && dump_text)
			g_print("%c", buffer[i]);

		if (buffer[i] == '\n')
		{
			lines++;
			if (dump_text && lines%lines_per_page == 0)
				g_print("\n\n-Page %i-\n\n", lines/lines_per_page + 1);
			
			chars_in_this_line=0;
			continue;
		}

		if (buffer[i] == '\t')
		{
			temp_i = chars_in_this_line;

			chars_in_this_line += tab_size - ((chars_in_this_line-1) % tab_size) - 1;

			if (chars_in_this_line > chars_per_line + 1)
			    chars_in_this_line = chars_per_line + 1;

			if (dump_text)
				for (j=temp_i;j<chars_in_this_line+1;j++)
					g_print(".");
			/*
			g_print("\ntabs agregados = %i\n", chars_in_this_line - temp_i);
			*/

		}


		/* Do word wapping here */ 
		if (chars_in_this_line == chars_per_line + 1 && real)
		{
			if (dump_info)
				g_print ("\nThis lines needs breaking\n");

			temp_i = i; /* We need to save the value of i in case we have to break the line */

			/* We back i till we find a space that we can break the line at */
			while (buffer[i] != ' ' && buffer[i] != '\t' && i > temp_i - chars_per_line - 1 )
			{
				i--;
				if (dump_text)
					g_print("\b");
			}

			if (dump_info)
				g_print("i got backed up [%i] times\n", temp_i - i);

			/* If this line was "unbreakable" break it at chars_per_line width */
			if (i == temp_i - chars_per_line - 1)
			{
				i = temp_i;
				if (dump_info)
					g_print ("We are breaking the line\n");
			}

			/* We need to remove the trailing blanks so that the next line does not start with
			   a space or a tab char */
			temp_i = i; /* Need to be able to determine who many spaces/tabs where removed */
			while (buffer[i] == ' ' || buffer[i] == '\t')
				i++;
			if (dump_info && i!=temp_i)
				g_print("We removed %i trailing spaces/tabs", i - temp_i);

			/* We need to back i 1 time because this is a for loop and we did not processed the
			   last character */
			i--;
			lines++;
			if (dump_text && lines%lines_per_page == 0)
				g_print("\n\n-Page %i-\n\n", lines / lines_per_page + 1);

			chars_in_this_line = 0;

			if (dump_text)
				g_print("\n");
		}

	}

	/* If the last line did not finished with a '\n' increment lines */
	if (buffer[i]!='\n')
	{
		lines++;
		if (dump_info_basic)
			g_print("\nAdding one line because it was not terminated with a slash+n\n");
	}

	if (dump_info_basic)
	{
		g_print("determine_lines found %i lines.\n", lines);
	}
	
	temp_i = lines;
	
        /* After counting, scan the doc backwards to determine how many
	   blanks lines there are (at the bottom),substract that from lines */
	for ( i = buffer_size-1; i>0; i--)
	{
		if ( buffer[i] != '\n' && buffer[i] != ' ' && buffer[i] != '\t')
			break;
		else
			if (buffer[i] == '\n')
				lines--;
	}

	if (dump_info_basic && lines != temp_i)
	{
		g_print("We removed %i line(s) because they contained no text\n", temp_i - lines);
	}

	if (dump_text)
		g_print(".\n.\n.\n");

	return lines;
}

static void
print_start_job (PrintJobInfo *pji)
{


	print_set_orientation(pji);
}

static void
print_set_orientation (PrintJobInfo *pji)
{
	double affine [6];

	if (pji->orientation == PRINT_ORIENT_PORTRAIT)
		return;

	art_affine_rotate (affine, 90.0);
	gnome_print_concat (pji->pc, affine);

	art_affine_translate (affine, 0, -pji->page_height);
	gnome_print_concat (pji->pc, affine);

}

static void
print_header (PrintJobInfo *pji, unsigned int page)
{
	gchar* text1 = g_strdup((gchar *)pji->filename);
	gchar* text2 = g_strdup_printf (_("Page: %i/%i"),page,pji->pages);
	GnomeFont *font;
	float x,y,len;
	


	font = gnome_font_new ("Helvetica", 12);
	gnome_print_setfont (pji->pc, font);

	/* Print the file name */
	y = pji->page_height - pji->margin_top - pji->header_height/2;
	len = gnome_font_get_width_string (font, text1);
	x = pji->page_width/2 - len/2;
	gnome_print_moveto(pji->pc, x, y);
	gnome_print_show(pji->pc, text1);
	gnome_print_stroke(pji->pc);

	/* Print the page/pages  */
	y = pji->page_height - pji->margin_top - pji->header_height/4;
	len = gnome_font_get_width_string (font, text2);
	x = pji->page_width - len - 36;
	gnome_print_moveto (pji->pc, x, y);
	gnome_print_show (pji->pc, text2);
	gnome_print_stroke (pji->pc); 


	gtk_object_unref (GTK_OBJECT(font));
	g_free (text1);
	g_free (text2);
}

static void
print_setfont (PrintJobInfo *pji)
{
	GnomeFont *font;	
	
	font = gnome_font_new ((gchar *)pji->font_name, pji->font_size);
	gnome_print_setfont (pji->pc, font);
	gtk_object_unref (GTK_OBJECT(font));
}
	

static void
print_end_page (PrintJobInfo *pji)
{
	gnome_print_showpage (pji->pc);
	print_set_orientation (pji);
}

static void
print_end_job (GnomePrintContext *pc)
{

}

static void
print_pji_destroy (PrintJobInfo *pji)
{


	gtk_object_unref (GTK_OBJECT (pji->master));
	g_free (pji->buffer);
	g_free (pji->filename);
	g_free (pji);
}

static void
preview_destroy_cb (GtkObject *obj, PrintJobInfo *pji)
{

}






