/***************************************************************************
                          spellcheck.cpp  -  description
                             -------------------
    begin                : Tue May 16 2000
    copyright            : (C) 2000 by Terry Biggs
    email                : tbiggs@infinet.com
    This code is from the gaspell program by Kevin Atkinson (gaspell.cc)-
    I just changed it to a dialog and removed the file and menu stuff.
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
//#ifdef USE_ASPELL

#include <gnome.h>

#include <string>
#include <algo.h>
#include <aspell/manager.hh>
#include <aspell/check.hh>
#include <aspell/config.hh>
#include <aspell/suggest.hh>
#include <aspell/clone_ptr-t.hh>

#include "DICTClient.h"
#include "spellcheck.h"
#include "callback.h"
#include "support.h"
#include "dialogs.h"

using namespace aspell;
using namespace autil;

extern GtkWidget* MainFrm; //-- gnome app
extern GtkWidget* studypad;//-- studypad text widget
extern GtkWidget* notes;   //-- notes text widget
extern bool noteModified;  //-- have notes been modified

bool letscontinue = true;  //-- ????
gint whichtextwidget = 0; //-- which text widget do we check - default - studypad
bool bSpell = true; //-- are we running spellcheck
 //
//
// GtkTextIterator - an almost RandomAccessIterator for the GtkText widget
//
//

class GtkTextIterator {
  GtkText *text;
  guint    pos_;
  typedef  GtkTextIterator self;
public:
  typedef const char           value_type;
  typedef guint                difference_type;
  typedef const char *         pointer;
  typedef const char           reference;
  typedef forward_iterator_tag iterator_category;
  GtkTextIterator() {}
  GtkTextIterator(GtkText *t, guint p = 0) : text(t), pos_(p) {}

  self& set(GtkText *t, guint p = 0) {text = t; pos_=p; return *this;}
  self& go(guint p) {pos_=p; return *this;}
  self& go_beg() {pos_=0; return *this;}
  self& go_end() {pos_=gtk_text_get_length(text); return *this;}
  self& go_cur()
    {pos_=gtk_editable_get_position(GTK_EDITABLE(text));return *this;}
  guint pos() const {return pos_;}

  const char operator* () const {return GTK_TEXT_INDEX(text, pos_);}

  self& operator++ () {++pos_; return *this;}
  self operator++ (int) {self temp = *this; operator++(); return temp;}
  self& operator += (guint i) {pos_+=i; return *this;}
  self& operator -= (guint i) {pos_+=i; return *this;}

  friend bool operator== (const self& rhs, const self& lhs)
    {return rhs.pos_ == lhs.pos_;}
  friend guint operator- (const self& rhs, const self& lhs)
    {return rhs.pos_ - lhs.pos_;}
  friend self& operator- (self rhs, guint lhs) {return rhs-= lhs;}
  friend self& operator+ (self rhs, guint lhs) {return rhs+= lhs;}
};


//
//
// gaspell_options
//
//

typedef Config MyOptions;
MyOptions        myoptions;



class AspellCheck {
      GtkWidget* window;

  GtkWidget* text;
  GtkWidget* correction;
  GtkWidget* suggestions;

  GtkWidget* definition;

  // buttons
  GtkWidget *change_, *change_all_, *ignore_, *ignore_all_;
  GtkWidget *learn_, *revert_, *start_, *stop_;
  GtkWidget *lookup_, *auto_lookup_;

  GtkWidget *main_dlg;

  // toggle menu items
  GtkWidget *savedicts_;
  GtkWidget *urlskip_;

  enum SaveCont {do_nothing, do_load, do_clear, do_exit};
  SaveCont  save_cont;
  string    save_cont_aux;

  //
  GdkColor misspelled_color;
  GdkColor part_of_speech_color;
  GdkColor number_color;

  //
  DICTClient dc;
  Manager    sc;
  CheckState<GtkTextIterator> state;
  bool do_savedicts;
  GtkTextIterator             itr, end;

  guint             mis_begin, mis_end;
  gchar             *word;

  guint             start_pos;
  bool              wrapped;

  bool              changed;
  bool              backup_saved;

//  string            file_name;

public:
  AspellCheck() :  /*file_open(0), file_save(0), */
    save_cont(do_nothing), sc(myoptions), state(sc), do_savedicts(true),
    word(0), mis_end(0), backup_saved(false), changed(false)
  {
    if (myoptions.have("dict-host"))
      if (!myoptions.have("dict-port"))
	dc.setHost(myoptions.retrieve("dict-host").c_str());
      else
	dc.setHost(myoptions.retrieve("dict-host").c_str(),
		   myoptions.retrieve_int("dict-port"));
  }

  ~AspellCheck() {if (do_savedicts) sc.save_all_wls();}

  GtkWidget* win() {return window;}

  enum problem {no_errors, no_file, could_not_open, canceled, exists};

  void init();
  void draw();
  int clear();

  void set_main_dlg(GtkWidget *w) {main_dlg = w;}

  // these members affect which buttons are active
  void edit_mode();
  void check_mode();
  void sug_mode();

  //
  void unhighlight_text();

  //
  void set_correction(const char *);

  void check();      // starts checking at the curser position
  void start();
  void stop() {edit_mode();}
  void change();
  void change_all();
  void def_change() {change();}
  void ignore();
  void ignore_all();
  void learn();
  void revert();
  void lookup();
  void auto_lookup();

  bool auto_lookup_active() {
    return GTK_TOGGLE_BUTTON(auto_lookup_)->active;
  }
  //
  bool exit();
  friend void text_insert(GtkWidget *, const gchar *, gint, gint *, gpointer);
  friend void text_delete(GtkWidget *, gint, gint, gpointer);
};


//
// AspellDlg
//
//

class AspellDlg {


  AspellCheck check;
public:
	GtkWidget* main_dlg;
  AspellDlg() {}
  void draw();
  void start();
};
AspellDlg *spellapp;


//
//
// Signal and Event Hanadlenders
//
//

void destroy_main_dlg (GtkWidget *main_dlg, gpointer data)
{
	bSpell = false;
  gtk_widget_destroy(main_dlg);
  delete spellapp;
}

void suggestions_selection_made(GtkWidget *clist, gint row, gint column,
		    GdkEventButton* event, gpointer data) {
  gchar *sel;
  gtk_clist_get_text(GTK_CLIST(clist), row, column, &sel);
  AspellCheck * d = static_cast<AspellCheck*>(data);
  d->set_correction(sel);
  if (d->auto_lookup_active())
    d->lookup();
  if (event->type == GDK_2BUTTON_PRESS)
    d->def_change();
}

#define item_clkd(item) \
  void item##_clicked(GtkWidget *w, gpointer data) { \
    static_cast<AspellCheck*>(data)->item(); \
  }

item_clkd(start)   item_clkd(stop)
item_clkd(change)  item_clkd(change_all)
item_clkd(ignore)  item_clkd(ignore_all)
item_clkd(revert)  item_clkd(learn)
item_clkd(lookup)  item_clkd(auto_lookup)

#undef item_clkd

void text_insert(GtkWidget *, const gchar *, gint, gint *, gpointer data)
{
  static_cast<AspellCheck*>(data)->edit_mode();
}

void text_delete(GtkWidget *widget, gint, gint, gpointer data)
{
  static_cast<AspellCheck*>(data)->edit_mode();
}

int check_exit(GtkWidget *wid, GdkEvent *, gpointer data)
{
  return static_cast<AspellCheck*>(data)->exit();
}

void about(GtkWidget *, gpointer)
{
/*  const gchar *authors[] = {"Kevin Atkinson",
			    "DICT code: Sudhakar Chandrasekharan", NULL};
  GtkWidget *about = gnome_about_new("Gaspell", VERSION,
				     "Copyright 2000 by Kevin Atkinson",
				     authors,
				     "The Gnome frontend to Aspell.\n"
				     "http://aspell.sourceforge.net/\n",
				     NULL);
  gtk_widget_show(about);
  */
}

//
//
// AspellCheck & window startup functions
// they place the widgets, draw the screen, etc...
//
//
void AspellDlg::draw()
{
  main_dlg = gnome_dialog_new ("gaspell", NULL);
  check.set_main_dlg(main_dlg);
   gtk_object_set_data (GTK_OBJECT (main_dlg), "main_dlg", main_dlg);
  gtk_widget_set_usize (main_dlg, 500, 200);
  gtk_window_set_policy (GTK_WINDOW (main_dlg), FALSE, FALSE, FALSE);
  //gtk_container_border_width (GTK_CONTAINER (main_dlg), 0);
  GtkWidget *dialog_vbox4 = GNOME_DIALOG (main_dlg)->vbox;
  gtk_object_set_data (GTK_OBJECT (main_dlg), "dialog_vbox4", dialog_vbox4);

  check.draw();
  gtk_box_pack_start(GTK_BOX(dialog_vbox4), check.win(), TRUE, TRUE, 2);
  gtk_widget_show(check.win());

  gtk_widget_show(dialog_vbox4);
  gtk_widget_show(main_dlg);

  gtk_signal_connect (GTK_OBJECT(main_dlg), "destroy",
  		      GTK_SIGNAL_FUNC(destroy_main_dlg), NULL);
}

void AspellDlg::start()
{
  //check.load();
}

void AspellCheck::draw()
{
	GtkWidget* mybox = gtk_vbox_new(false,0);
  window = mybox;
  //
  // set the text window
  //
  switch(whichtextwidget)
  {
  	case 0: text = studypad; //-- check text in studypad window
  		break;
  	case 1:text = notes; //-- check text in notes window
  		break;
  	default:text = studypad; //-- check text in studypad window
  		break;
  }
  itr.set(GTK_TEXT(text));
  end.set(GTK_TEXT(text));
  gtk_text_set_editable (GTK_TEXT (text), true);
  gtk_text_set_word_wrap (GTK_TEXT (text), true);


  GtkWidget* box;
  GtkWidget* vscrollbar;
  GtkWidget* paned2 = gtk_hpaned_new();
  gtk_box_pack_start(GTK_BOX(mybox), paned2, true, true, 0);
//  gtk_paned_add2(mybox, paned2);
  gtk_widget_show(paned2);

  //
  // Create and load the help window.
  //

  definition = gtk_text_new(0,0);
  gtk_text_set_editable (GTK_TEXT (definition), false);
  gtk_text_set_word_wrap(GTK_TEXT (definition), true);
  box = gtk_hbox_new(false, 0);
  gtk_box_pack_start(GTK_BOX(box), definition, true, true, 0);
  vscrollbar = gtk_vscrollbar_new (GTK_TEXT(definition)->vadj);
  gtk_box_pack_start(GTK_BOX(box), vscrollbar, false, false, 0);
  gtk_widget_show(definition);
  gtk_widget_show(vscrollbar);

  string definition_msg = "";
  gtk_text_freeze(GTK_TEXT(definition));
  gtk_text_insert(GTK_TEXT(definition), 0, 0, 0, definition_msg.c_str(), definition_msg.size());
  gtk_text_thaw(GTK_TEXT(definition));

  gtk_container_border_width(GTK_CONTAINER(box), 10);
  gtk_paned_add2(GTK_PANED(paned2), box);
  gtk_widget_show(box);

  //
  // Create and connect the suggestions list and action buttons
  //

  box = gtk_hbox_new(false,0);
  GtkWidget* sug_box = gtk_vbox_new(false,0);
  correction = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(sug_box), correction, false, false, 0);
  gtk_widget_show(correction);
  suggestions = gtk_clist_new(1);
  gtk_clist_set_selection_mode(GTK_CLIST(suggestions),
			       GTK_SELECTION_SINGLE);

  GtkWidget* sug_scrolled = gtk_scrolled_window_new(0,0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sug_scrolled),
				 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add(GTK_CONTAINER(sug_scrolled), suggestions);
  gtk_widget_show(suggestions);

  gtk_box_pack_start(GTK_BOX(sug_box), sug_scrolled, true, true, 0);
  gtk_widget_show(sug_scrolled);

  gtk_box_pack_start(GTK_BOX(box), sug_box, true, true, 4);
  gtk_widget_show(sug_box);

  GtkWidget* table = gtk_table_new(5,2,true);

  #define add_button(but, lab, x, y, sen) \
    but = gtk_button_new_with_label(lab); \
    gtk_table_attach_defaults(GTK_TABLE(table), but, x, x+1, y, y+1); \
    gtk_widget_set_sensitive(but, sen); \
    gtk_widget_show(but)

  add_button(start_, "Start", 0,0, true);
  add_button(stop_, "Stop", 1,0, true);
  add_button(change_, "Change", 0,1, false);
  add_button(change_all_, "Change All", 1,1, false);
  add_button(ignore_, "Ignore", 0,2, false);
  add_button(ignore_all_, "Ignore All", 1,2, false);
  add_button(revert_, "Revert", 0,3, false);
  add_button(learn_, "Learn", 1,3, false);
  add_button(lookup_, "Lookup", 0,4, true);

  #undef add_button

  auto_lookup_  = gtk_toggle_button_new_with_label("Auto");
  gtk_table_attach_defaults(GTK_TABLE(table), auto_lookup_, 1, 2, 4, 5);
  gtk_widget_set_sensitive(auto_lookup_, true);
  gtk_widget_show(auto_lookup_);

  gtk_box_pack_start(GTK_BOX(box), table, false, false, 0);
  gtk_widget_show(table);

  gtk_container_border_width(GTK_CONTAINER(box), 10);
  gtk_paned_add1(GTK_PANED(paned2), box);
  gtk_widget_show(box);

  // Get the system colour map and allocate the colour red
  GdkColormap *cmap = gdk_colormap_get_system();
  misspelled_color.red = 0xffff;
  misspelled_color.green = 0;
  misspelled_color.blue = 0;
  part_of_speech_color.red = 0xffff;
  part_of_speech_color.green = 0;
  part_of_speech_color.blue = 0;
  number_color.red = 0;
  number_color.green = 0;
  number_color.blue = 0xffff;
  if (!gdk_color_alloc(cmap, &misspelled_color)
      || !gdk_color_alloc(cmap, &part_of_speech_color)
      || !gdk_color_alloc(cmap, &number_color) )
    {
      g_error("couldn't allocate required colors");
    }

  gtk_signal_connect(GTK_OBJECT(suggestions), "select_row",
		     GTK_SIGNAL_FUNC(suggestions_selection_made),
		     (gpointer)this);
  gtk_signal_connect(GTK_OBJECT(text), "insert_text",
		     GTK_SIGNAL_FUNC(text_insert), (gpointer)this);
  gtk_signal_connect(GTK_OBJECT(text), "delete_text",
		     GTK_SIGNAL_FUNC(text_delete), (gpointer)this);
  gtk_signal_connect(GTK_OBJECT(main_dlg), "delete_event",
		     GTK_SIGNAL_FUNC(check_exit), (gpointer)this);

  #define con_but(but) \
    gtk_signal_connect(GTK_OBJECT(but##_), "clicked", \
	  	       GTK_SIGNAL_FUNC(but##_clicked), (gpointer)this)

  con_but(start);  con_but(stop);
  con_but(change); con_but(change_all);
  con_but(ignore); con_but(ignore_all);
  con_but(revert); con_but(learn);
  con_but(lookup); con_but(auto_lookup);

  #undef con_but

}

int AspellCheck::clear()
{

  unhighlight_text(); // Fixme: This should NOT be needed

  changed = false;
  return false;
}

//
//
// AspellCheck helper functions
//
//

void AspellCheck::edit_mode()
{
	if(whichtextwidget == 1)noteModified = true; //-- notes have been changed
	if(bSpell)
	{
  	changed = true;
  	unhighlight_text();
  	gtk_widget_set_sensitive(start_, true);
  	gtk_widget_set_sensitive(stop_, false);
  	gtk_widget_set_sensitive(change_, false);
  	gtk_widget_set_sensitive(change_all_, false);
  	gtk_widget_set_sensitive(ignore_, false);
  	gtk_widget_set_sensitive(ignore_all_, false);
  	gtk_widget_set_sensitive(learn_, false);
  	gtk_widget_set_sensitive(revert_, false);
  }
}

void AspellCheck::check_mode()
{
	if(bSpell)
	{
  	g_free(word);
  	word = 0;
  	gtk_clist_clear(GTK_CLIST(suggestions));
  	gtk_widget_set_sensitive(start_, false);
  	gtk_widget_set_sensitive(stop_, false);
  	gtk_widget_set_sensitive(change_, false);
  	gtk_widget_set_sensitive(change_all_, false);
  	gtk_widget_set_sensitive(ignore_, false);
  	gtk_widget_set_sensitive(ignore_all_, false);
  	gtk_widget_set_sensitive(learn_, false);
  	gtk_widget_set_sensitive(revert_, false);
  }
}

void AspellCheck::sug_mode()
{
	if(bSpell)
	{
  	gtk_widget_set_sensitive(start_, false);
  	gtk_widget_set_sensitive(stop_, true);
  	gtk_widget_set_sensitive(change_, true);
  	gtk_widget_set_sensitive(change_all_, false);
  	gtk_widget_set_sensitive(ignore_, true);
  	gtk_widget_set_sensitive(ignore_all_, true);
  	gtk_widget_set_sensitive(learn_, true);
  	gtk_widget_set_sensitive(revert_, true);
  }
}

void AspellCheck::unhighlight_text()
{
  if ((mis_end != 0)&&(bSpell))
  {
    gtk_text_freeze(GTK_TEXT(text));
    guint p = gtk_text_get_point(GTK_TEXT(text));
    guint c = gtk_editable_get_position(GTK_EDITABLE(text));
    gtk_text_set_point(GTK_TEXT(text), mis_begin);
    gtk_text_forward_delete(GTK_TEXT(text), mis_end - mis_begin);
    gtk_text_insert(GTK_TEXT(text), 0, &text->style->black, 0, word, -1);
    mis_end = 0;
    gtk_text_set_point(GTK_TEXT(text), p);
    gtk_text_thaw(GTK_TEXT(text));
    gtk_editable_set_position(GTK_EDITABLE(text), c);
  }
}

void AspellCheck::set_correction(const char *word)
{
  gtk_entry_set_text(GTK_ENTRY(correction), word);
}


//
// AspellCheck options event functions
//
//


//
//
// AspellCheck event functions
//
//
bool AspellCheck::exit()
{
  save_cont = do_exit;
  if (!clear())
    gtk_widget_destroy(main_dlg);
  else
    return true;
  return false;
}

void AspellCheck::start()
{
  gtk_signal_emit_by_name(GTK_OBJECT(text),"move_word",-1);
  start_pos = gtk_editable_get_position(GTK_EDITABLE(text));
  GtkTextIterator temp = itr;
  temp.go_beg(); itr.go_cur(); end.go_end();
  state.reset();
  state.restart(temp,end);
  state.scan(itr);
  state.advance();
  wrapped = false;
  check();
}

void AspellCheck::check()
{
  check_mode();
  aspell::check(state);
  mis_begin = state.word_begin().pos();
  mis_end   = state.word_end().pos();

  if (wrapped && (state.at_end() || mis_begin >= start_pos))
  {
    gnome_dialog_run_and_close(GNOME_DIALOG(gnome_message_box_new(
      "Spell Checker Complete",
      GNOME_MESSAGE_BOX_INFO,
      "Button_Ok", 0)));
    mis_end = 0;
    clear();
    edit_mode();
    letscontinue = false;
  }
  else if (state.at_end())
  {
    gtk_editable_set_position(GTK_EDITABLE(text), 0);
    wrapped = true;
    state.reset();
    itr.go_beg(); end.go_end();
    state.restart(itr, end);
    state.advance();
    check();
  }
  else
  {
    word = gtk_editable_get_chars(GTK_EDITABLE(text), mis_begin, mis_end);
    gtk_text_set_point(GTK_TEXT(text), mis_begin);
    gtk_text_forward_delete(GTK_TEXT(text), mis_end - mis_begin);

    gtk_text_insert(GTK_TEXT(text), 0, &misspelled_color, 0, word, -1);

    SuggestionList & sugs = sc.suggest(word);
    const char * sug;

    SuggestionList::Emul els = sugs.elements();
    while(sug = els.next(), sug) {
      gtk_clist_append(GTK_CLIST(suggestions),
		       const_cast<char * *>(&sug));
    }

    gtk_entry_set_text(GTK_ENTRY(correction), word);

    sug_mode();
  }
}

void AspellCheck::change()
{
  changed = true;
  gtk_text_set_point(GTK_TEXT(text), mis_begin);
  gtk_text_forward_delete(GTK_TEXT(text), mis_end - mis_begin);
  gchar *repl = gtk_entry_get_text(GTK_ENTRY(correction));
  if (mis_begin < start_pos) {
    int s; for (s = 0; repl[s]; ++s);
    start_pos += s - (mis_end - mis_begin);
  }
  sc.store_repl(word,repl);
  state.backup();
  gtk_text_insert(GTK_TEXT(text), 0, &text->style->black, 0, repl, -1);
  gtk_editable_set_position(GTK_EDITABLE(text), mis_begin);
  itr.go_cur();
  end.go_end();
  state.restart(itr, end);
  state.advance();
  check();
}

void AspellCheck::change_all()
{
  change();
}

void AspellCheck::ignore()
{
  gtk_text_set_point(GTK_TEXT(text), mis_end);
  unhighlight_text();
  state.advance();
  check();
}

void AspellCheck::ignore_all()
{
  sc.add_to_session(word);
  ignore();
}

void AspellCheck::revert()
{
  set_correction(word);
}

void AspellCheck::learn()
{
  sc.add_to_personal(word);
  ignore();
}

#define ins(text, color) gtk_text_insert(GTK_TEXT(definition), 0,  \
                                         color, 0, text, -1)
#define inss(text, color) gtk_text_insert(GTK_TEXT(definition), 0, \
                                          color, 0, text.c_str(), -1)

void AspellCheck::lookup()
{
  gchar * word = gtk_entry_get_text(GTK_ENTRY(correction));
  if (!word[0]) return;
  gtk_text_freeze(GTK_TEXT(definition));
  gtk_editable_delete_text(GTK_EDITABLE(definition), 0, -1);
  try
  {
    ins(word, 0);
    ins("\n", 0);
    if (!dc.isConnected()) dc.dial();
    DICTClient::Definitions defs = dc.lookup(word, "wn");
    if (defs.empty())
    {
      ins("{No Definition found.}", 0);
    }
    else
    {
      vector<WordNetDefinition> wn_defs = wordnet_split(defs[0].definition);
      for(unsigned int i=0; i != wn_defs.size(); ++i)
      {
				if (wn_defs[i].part_of_speech.size())
				{
	  			ins("<", &part_of_speech_color);
	  			inss(wn_defs[i].part_of_speech, &part_of_speech_color);
	  			ins("> ", &part_of_speech_color);
				}
				if (wn_defs[i].number.size()) 	
				{
	  			ins("(", &number_color);
	  			inss(wn_defs[i].number, &number_color);
	  			ins(") ", &number_color);
				}
				inss(wn_defs[i].definition,0);
				ins("  ", 0);
      }
    }
  }
  catch (DICTClient::Exception & excep)
  {
    ins ("{",0);
    inss(excep.message(), 0);
    ins ("}",0);
  }
  gtk_text_thaw(GTK_TEXT(definition));
}

void AspellCheck::auto_lookup()
{
}

//
//
// open spellcheck
//
//

void options_act(poptContext, poptCallbackReason,
		 const poptOption *, const char *, void *);

static const KeyInfo extra[] = {
  {"dict-host", KeyInfoString, "", "Host name of DICT server"},
  {"dict-port", KeyInfoInt   , "", "Port number of DICT server"},
  {"infile",    KeyInfoString, "", "File name to check."}
};

GtkWidget*
spellcheck(gint itext)
{
  myoptions.set_extra(extra, extra+3);
  bSpell = true;
  poptOption  * popt_opts;
  unsigned int popt_opts_size = 2;
  unsigned int popt_strs_size = 0;
  whichtextwidget = itext;
  ConfigData::PossibleElementsEmul els = myoptions.possible_elements();
  const KeyInfo * k;
  while (k = els.next(), k) {
    if (k->desc == 0) continue;
    if (k->type == KeyInfoBool) {
      popt_opts_size += 2;
      popt_strs_size += 6 + strlen(k->name);
    } else if (k->type == KeyInfoList) {
      popt_opts_size += 2;
      popt_strs_size += 2*(5 + strlen(k->name));
    } else {
      popt_opts_size += 1;
    }
  }
  popt_opts = new poptOption[popt_opts_size];
  memset((void *)popt_opts, 0, popt_opts_size * sizeof(poptOption));
  poptOption * i = popt_opts;
  char * popt_strs = new char[popt_strs_size];
  char * j = popt_strs;
  i->argInfo = POPT_ARG_CALLBACK;
  i->arg     = options_act;
  ++i;
  els = myoptions.possible_elements();
  while (k = els.next(), k) {
    if (k->desc == 0) continue;
    if (k->type == KeyInfoBool) {
      i->argInfo  = POPT_ARG_NONE;
      i->longName = k->name;
      i->descrip  = k->desc;
      ++i;
      i->argInfo = POPT_ARG_NONE;
      i->longName = j;
      strcpy(j, "dont-"); j += 5;
      strcpy(j, k->name); j += strlen(j) + 1;
      i->descrip  = NULL;
      ++i;
    } else if (k->type == KeyInfoList) {
      i->argInfo = POPT_ARG_NONE;
      i->longName = j;
      strcpy(j, "add-");  j += 4;
      strcpy(j, k->name); j += strlen(j) + 1;
      i->descrip  = k->desc;
      ++i;
      i->argInfo = POPT_ARG_NONE;
      i->longName = j;
      strcpy(j, "rem-");  j += 4;
      strcpy(j, k->name); j += strlen(j) + 1;
      i->descrip  = NULL;
      ++i;
    } else {
      i->longName =  k->name;
      i->argInfo  =  POPT_ARG_STRING;
      i->descrip  =  k->desc;
      i->argDescrip = k->type == KeyInfoInt  ? "<int>" : "<str>";
      ++i;
    }
  }
  try
  {
    spellapp = new AspellDlg();
  }
  catch (exception & e)
  {
    GtkWidget * err = gnome_error_dialog(e.what());
    gnome_dialog_run_and_close(GNOME_DIALOG(err));
    return(NULL);
  }

  spellapp->draw();
  spellapp->start();
  return(spellapp->main_dlg);


/*
  delete[] popt_opts;
  delete[] popt_strs;
  delete app;
*/

}

void options_act(poptContext con, poptCallbackReason, 
		 const struct poptOption * opt,
		 const char * arg, void *) 
{
  if (!arg) arg = "";
  try {
    myoptions.replace(opt->longName, arg);
  } catch (exception & e) {
    cerr << e.what() << endl;
    exit(-1);
  }
}
//#endif /* USE_ASPELL */