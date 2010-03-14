#!/usr/bin/env python

#Copyright (c) 2009 Matthew Talbert

#Permission is hereby granted, free of charge, to any person
#obtaining a copy of this software and associated documentation
#files (the "Software"), to deal in the Software without
#restriction, including without limitation the rights to use,
#copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the
#Software is furnished to do so, subject to the following
#conditions:

#The above copyright notice and this permission notice shall be
#included in all copies or substantial portions of the Software.

#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
#OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
#WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
#OTHER DEALINGS IN THE SOFTWARE.

import pygtk
import gtk
import dbus
from dbus.mainloop.glib import DBusGMainLoop

class ControlXiphos:

    def receive_search_event(self, hits):
        self.liststore = gtk.ListStore(str)
        s = self.proxy.getNextSearchReference()
        while s != 'XIPHOS_SEARCH_END':
            self.liststore.append([s,])
            s = self.proxy.getNextSearchReference()
        self.results_tree.set_model(self.liststore)

    def receive_navigation(self, reference):
        self.label_ref.set_text(reference)

    def navigate_xiphos(self, widget, data=None):
        self.proxy.setCurrentReference('sword://' + self.entry.get_text())

    def destroy(self, widget, data=None):
        gtk.main_quit()

    def __init__(self):

        DBusGMainLoop(set_as_default=True)

        self.session_bus = dbus.SessionBus()

        self.proxy = self.session_bus.get_object('org.xiphos.remote',
                                                 '/org/xiphos/remote/ipc')
        self.session_bus.add_signal_receiver(self.receive_navigation,
                                       dbus_interface='org.xiphos.remote',
                                       signal_name='navigationSignal')
        self.session_bus.add_signal_receiver(self.receive_search_event,
                                            dbus_interface='org.xiphos.remote',
                                            signal_name='searchPerformedSignal')

        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)

        self.window.connect("destroy", self.destroy)

        self.window.set_border_width(10)

        self.vbox = gtk.VBox()
        self.window.add(self.vbox)
        self.vbox.show()

        self.hbox = gtk.HBox()
        self.vbox.pack_start(self.hbox)
        self.hbox.show()

        self.entry = gtk.Entry()
        self.hbox.pack_start(self.entry)
        self.entry.show()

        self.button = gtk.Button("Navigate Xiphos")
        self.button.connect("clicked", self.navigate_xiphos, None)
        self.hbox.pack_end(self.button)
        self.button.show()

        self.hbox_ref = gtk.HBox()
        self.vbox.pack_end(self.hbox_ref)
        self.hbox_ref.show()

        self.label_ref_static = gtk.Label('Current Reference: ')
        self.hbox_ref.pack_start(self.label_ref_static)
        self.label_ref_static.show()

        self.label_ref = gtk.Label(self.proxy.getCurrentReference())
        self.hbox_ref.pack_end(self.label_ref)
        self.label_ref.show()

        self.hbox_res = gtk.HBox()
        self.vbox.pack_end(self.hbox_res)
        self.hbox_res.show()

        self.scrollbox = gtk.ScrolledWindow()
        self.hbox_res.pack_end(self.scrollbox)
        self.scrollbox.show()

        self.results_tree = gtk.TreeView()
        self.results_tree.set_size_request(-1, 200)

        self.cell = gtk.CellRendererText()
        self.column = gtk.TreeViewColumn('Reference')
        self.column.pack_start(self.cell, True)
        self.column.add_attribute(self.cell, 'text', 0)

        self.results_tree.append_column(self.column)

        self.scrollbox.add(self.results_tree)
        self.results_tree.show()

        self.window.show()

    def main(self):
        gtk.main()

if __name__ == "__main__":
    ipc = ControlXiphos()
    ipc.main()
