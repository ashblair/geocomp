#include "first.hpp"
#include "widgets.hpp"
#include "winredefs.hpp"

using namespace std;

//static Gtk::Window* TopWnd = nullptr;

int MessageBox(const string title, const string content, Gtk::Window * topWnd)
{
    
    if (topWnd == nullptr) 
    {
        Gtk::MessageDialog dialog(title.c_str());
        dialog.set_secondary_text(content.c_str());
        return dialog.run();
    }
    else 
    {
        Gtk::MessageDialog dialog(*topWnd, title.c_str(), false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_YES_NO, false);
        dialog.set_secondary_text(content.c_str());
        return dialog.run();
    }
    //Gtk::MessageDialog dialog(*TopWnd, title.c_str());
}




AllMyChildren::AllMyChildren(void)
{
    all_children.clear();
    parent = NULL;
}

AllMyChildren::AllMyChildren(const AllMyChildren & a)
{
    all_children = a.all_children;
    parent = a.parent;
}

AllMyChildren::AllMyChildren(Gtk::Container & a)
{
    parent = &a;
    all_children = a.get_children();

}


Gtk::Widget * AllMyChildren::get_Child(const string & childName)
{
    for (vector<Gtk::Widget *>::iterator it = all_children.begin(); 
        it != all_children.end(); 
        ++it)
    {
        Gtk::Widget * child = *it;
        string elementName = child->get_name();
        if (elementName.compare(childName) == 0) return child;
    }
    return NULL;

}

int AllMyChildren::MessageBox(const string & title, const string & primary_msg)
{
    //Gtk::Window pWin = parent->get_toplevel();//(Gtk::Window) *parent;//*(parent->get_window().get());    
    //Gtk::MessageDialog dialog(*(parent->get_window().get()), title.c_str());
    pGEXT pG = GEXT::getInst();
    Gtk::MessageDialog * dialog = NULL;
    //MsgBox, a GtkMessageDialog
    pG->get("MsgBox", dialog);
    dialog->set_title(title.c_str());
    dialog->set_message(primary_msg);
    //Gtk::MessageDialog dialog(title.c_str());
    dialog->set_secondary_text(listChildren2().c_str());
    return dialog->run();
}

void AllMyChildren::listChildren(void)
{
    //string sBuff = G_OBJECT_TYPE_NAME(parent);
    cout << parent->get_name() << "(" << parent->get_type() << ")" << 
        " has " <<  to_string(all_children.size()) << " children:\n\t";
    int i = 0;
    for (vector<Gtk::Widget *>::iterator it = all_children.begin(); 
        it != all_children.end(); 
        ++it)
    {
        Gtk::Widget * child = *it;
        string elementName = child->get_name();
        cout << to_string(++i) << ") " << elementName << " ";
    }
    cout << "\n";

}

string AllMyChildren::listChildren2(void)
{
    string r = "", p_n = parent->get_name(), p_c = G_OBJECT_CLASS_NAME(G_OBJECT_GET_CLASS(parent->gobj_copy()));

    r = p_n;
    r += "(";
    r += p_c;
    r += (string)") has " +  to_string(all_children.size()) + " children:\n\t";
    int i = 0;
    for (vector<Gtk::Widget *>::iterator it = all_children.begin(); 
        it != all_children.end(); 
        ++it)
    {
        Gtk::Widget * child = *it;
        string elementName = child->get_name();
        r += to_string(++i) + ") " + elementName + " ";
    }
    r += "\n";
    return r;
}
