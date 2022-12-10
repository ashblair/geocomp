#pragma once

//#include "stdafx.h"
//#include <gtk/gtk.h>

//static Gtk::Window* TopWnd = nullptr;

int MessageBox(const std::string, const std::string, Gtk::Window * topWnd = nullptr);

class AllMyChildren
{
public:
    AllMyChildren(void);
    AllMyChildren(const AllMyChildren &);
    AllMyChildren(Gtk::Container &);
    //AllMyChildren(Gtk::Window &);
    //AllMyChildren(Gtk::Widget &);

    Gtk::Widget * get_Child(const std::string &);
    int MessageBox(const std::string &, const std::string &);
    void listChildren(void);
    std::string listChildren2(void);

private:
    std::vector<Gtk::Widget *> all_children;
    Gtk::Widget * parent;

};
