using Gtk;
using WebKit;

	public void help () {
	}


private class EditKit : Window {

	private const string MAIN_UI  = "main.ui";
        private WebView web_view;

	private const ActionEntry[] entries = {
		{"menuFile", null, "_File"},
		{"new", Gtk.Stock.NEW, "_New", null, null, on_new},
		{"open", Gtk.Stock.OPEN, "_Open", null, null, on_open},
		{"save", Gtk.Stock.SAVE, "_Save", null, null, on_save},
		{"print", Gtk.Stock.PRINT, "_Print", null, null, on_action},
		{"menuEdit", null, "_Edit"},
		{"undo", Gtk.Stock.UNDO, "_Undo", null, null, on_action},
		{"redo", Gtk.Stock.REDO, "_Redo", null, null, on_action},
		{"cut", Gtk.Stock.CUT, "_Cut", null, null, on_action},
		{"copy", Gtk.Stock.COPY, "_Copy", null, null, on_action},
		{"paste", Gtk.Stock.PASTE, "_Paste", null, null, on_paste},
		{"menuInsert", null, "_Insert"},
		{"insertimage", "insert-image", "Insert _Image", null, null, on_insert_image},
		{"insertlink", "insert-link", "Insert _Link", null, null, on_insert_link},
		{"menuFormat", null, "_Format"},
		{"bold", Gtk.Stock.BOLD, "_Bold", """<ctrl>B""", null, on_action},
		{"italic", Gtk.Stock.ITALIC, "_Italic", """<ctrl>I""", null, on_action},
		{"underline", Gtk.Stock.UNDERLINE, "_Underline", """<ctrl>U""", null, on_action},
		{"strikethrough", Gtk.Stock.STRIKETHROUGH, "_Strike", """<ctrl>T""", null, on_action},
		{"font", Gtk.Stock.SELECT_FONT, "Select _Font", """<ctrl>F""", null, on_select_font},
		{"color", Gtk.Stock.SELECT_COLOR, "Select _Color", null, null, on_select_color},
		{"justifyleft", Gtk.Stock.JUSTIFY_LEFT, "Justify _Left", null, null, on_action},
		{"justifyright", Gtk.Stock.JUSTIFY_RIGHT, "Justify _Right", null, null, on_action},
		{"justifycenter", Gtk.Stock.JUSTIFY_CENTER, "Justify _Center", null, null, on_action},
		{"justifyfull", Gtk.Stock.JUSTIFY_FILL, "Justify _Full", null, null, on_action},
		{"indent", Gtk.Stock.INDENT, "_Icrease Indent", "<Control>bracketright", "Increase Indent", on_action},
		{"outdent", Gtk.Stock.UNINDENT, "_Decrease Indent", "<Control>bracketleft", "Decrease Indent", on_action}
	};

    private EditKit () {
	this.title = title;
	resize (800, 600);
	create_widgets ();
	connect_signals ();
    }

	private void create_widgets () {

		var actions = new Gtk.ActionGroup("Actions");
		actions.set_translation_domain("xiphos");
		actions.add_actions(entries, this);

		actions.get_action("insertimage").set_property("icon-name", "insert-image");
		actions.get_action("insertlink").set_property("icon-name", "insert-link");

		var ui = new UIManager();
		ui.insert_action_group(actions, 0);

		try {
			ui.add_ui_from_file(MAIN_UI);
		} catch (Error e) {
			stdout.printf("Error: %s\n", e.message);
		}

		this.add_accel_group(ui.get_accel_group());

		var toolbar1 = ui.get_widget("/toolbar_main");
		var toolbar2 = ui.get_widget("/toolbar_format");
		var menubar = ui.get_widget("/menubar_main");

		this.web_view = new WebView ();
		web_view.set_editable(true);
		web_view.load_html_string("", "file:///");
		var scrolled_window = new ScrolledWindow (null, null);
		scrolled_window.set_policy (PolicyType.AUTOMATIC, PolicyType.AUTOMATIC);
		scrolled_window.add (this.web_view);
		var vbox = new VBox (false, 0);
		vbox.pack_start (menubar, false, true, 0);
		vbox.pack_start (toolbar1, false, true, 0);
		vbox.pack_start (toolbar2, false, true, 0);
		vbox.pack_start (scrolled_window, true, true, 0);
		add (vbox);

	}

    private void connect_signals () {
	this.destroy.connect (Gtk.main_quit);
    }

	private void on_action(Gtk.Action action) {
		this.web_view.execute_script("document.execCommand('" + action.get_name() + "', false, false);");
	}

	private void on_open() {

		string filename;
		string content;

		FileChooserDialog dialog = new FileChooserDialog(
			"Select an HTML file",
			this,
			Gtk.FileChooserAction.OPEN,
			Gtk.Stock.CANCEL, Gtk.ResponseType.CANCEL,
			Gtk.Stock.OPEN, Gtk.ResponseType.ACCEPT,
			null);

		if (dialog.run() == Gtk.ResponseType.ACCEPT) {
			filename = dialog.get_filename();
			try {
				FileUtils.get_contents(filename, out content);
			} catch (FileError e) {
				stdout.printf("Error: %s\n", e.message);
			}
				this.web_view.load_html_string(content, "file:///");
		}

		dialog.destroy();
	}

	private void on_new() {
		this.web_view.load_html_string("", "file:///");
	}

	private void on_save() {

		string filename;
		string content;

		FileChooserDialog dialog = new FileChooserDialog(
			"Select an HTML file",
			this,
			Gtk.FileChooserAction.SAVE,
			Gtk.Stock.CANCEL, Gtk.ResponseType.CANCEL,
			Gtk.Stock.OPEN, Gtk.ResponseType.ACCEPT,
			null);

		dialog.set_do_overwrite_confirmation(true);

		if (dialog.run() == Gtk.ResponseType.ACCEPT) {
			filename = dialog.get_filename();
			this.web_view.execute_script ("document.title=document.documentElement.innerHTML;");
			content = this.web_view.get_main_frame().get_title();
			try {
				FileUtils.set_contents(filename, content);
			} catch (FileError e) {
				stdout.printf("Error: %s\n", e.message);
			}
		}

		dialog.destroy();
	}

	private void on_paste() {
		this.web_view.paste_clipboard();
	}

	private void on_select_font() {

	}

	private void on_select_color() {

		Gdk.Color color;
		ColorSelectionDialog dialog = new ColorSelectionDialog("Select Color");

		if (dialog.run() == Gtk.ResponseType.OK) {
			var colsel = (ColorSelection) dialog.get_color_selection();
			colsel.get_current_color(out color);

			this.web_view.execute_script (
				"document.execCommand('forecolor', null, '" +
				color.to_string().substring(0,3) +
				color.to_string().substring(5,2) +
				color.to_string().substring(9,2) + "');");
		}

		dialog.destroy();
	}

	private void on_insert_image() {

	}

	private void on_insert_link() {

	}

    private static int main (string[] args) {

	Gtk.init (ref args);
	var editor = new EditKit ();
		editor.show_all();
	Gtk.main ();
	return 0;
    }
}
