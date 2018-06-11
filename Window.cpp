/*
 * Window.cpp
 *
 *  Created on: Jun 2, 2018
 *      Author: Jasper
 */

#include "Window.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <gtk/gtk.h>
#include "WindowManager.h"

using namespace std;


//static variables
GdkPixbuf* Window::icon;
map<string, GtkWidget*> Window::entries;
Heap* Window::priority_queue;
BST<Customer>* Window::customers;
BST<Product>* Window::products;


Window::Window(string xml) {

    //create the window
    self_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(self_window, "delete-event", G_CALLBACK(delete_event), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(self_window), 10);
    //gtk_window_set_default_size(GTK_WINDOW(self_window), 800, 600);
    
    //create box
    self_box = gtk_vbox_new(FALSE, 2);
    
    //set the box spacing
    gtk_box_set_spacing(GTK_BOX(self_box),3);
    
    //wipe the entries
    map<string, GtkWidget*> m;
    entries = m;
    
    //set up xml stringstream
    stringstream xmlss(xml);
    
    string tag, text, closetag = "";
    
    //create new vector of box pointers
	vector<GtkWidget*> boxes;
	boxes.push_back(self_box);

    
    while(getline(xmlss,tag)) {
        string msg;
        
        //get the tag
        tag = tag.substr(1, tag.length()-2); //get rid of the <>
        size_t pos = tag.find(" ");
        string tagName = tag.substr(0, pos); //get the tag name
        string options = tag.substr(pos+1); //get the options
        
        map<string,string> optionsMap;
        
        string optionpair;
        stringstream optionsSS(options);
        
        while(getline(optionsSS,optionpair,' ')) {//loop through the options, separated by spaces
            string attribute;
            string value;
            
            stringstream optsep(optionpair);
            
            getline(optsep, attribute, '\"'); //skip the options=
            attribute = attribute.substr(0,attribute.length()-1); //get rid of the '='
            getline(optsep,value,'\"'); //get the actual value
            
            optionsMap.insert(make_pair(attribute,value));
        }
        
        if(tagName == "variable") { //skip unassigned variable tag
            
            continue;
            
        } else if(tagName == "hbox") {// skip the line named box and assign inner box == true;
            g_print("creating hbox\n");
            //get homogeneous
            if(optionsMap["homogeneous"] == "true"){
                //create new box
                boxes.push_back(gtk_hbox_new(TRUE, 0));
            } else {
                //create new box
                boxes.push_back(gtk_hbox_new(FALSE, 0));
            }
            
            //set the box spacing
            gtk_box_set_spacing(GTK_BOX(boxes.at(boxes.size()-1)),3);
                       
            //restart the while loop
            continue;
            
        } else if (tagName == "vbox") {
            
            //create new box
            boxes.push_back(gtk_vbox_new(FALSE, 2));
            
            //set the box spacing
            gtk_box_set_spacing(GTK_BOX(boxes.at(boxes.size()-1)),3);
            
            continue;
            
        } else if (tagName == "scroll") {
        	
        	boxes.push_back(gtk_scrolled_window_new(NULL,NULL));
        } 
        else if(tagName == "/hbox" || tagName == "/vbox" || tagName == "/scroll") {
            
            //pack the box into the next higher box
            gtk_box_pack_start(GTK_BOX(boxes.at(boxes.size()-2)),boxes.at(boxes.size()-1),FALSE,FALSE,0);

            //show the box
            gtk_widget_show(boxes.at(boxes.size()-1));

            //remove the box from the box stack
            boxes.pop_back();

            continue;
        }
        
        
        
        getline(xmlss,text); //get the text
        getline(xmlss,closetag); //and the closetag
        
        if(boxes.size() > 1) {
            create_content(tagName,text,optionsMap,boxes.at(boxes.size()-1));
        } else {
            create_content(tagName,text,optionsMap,self_box);
        }
        
        
    }
    //g_print("widgets created\n");
    gtk_widget_show(self_box);
    
    //g_print("showing box\n");
    //g_print("adding box to window\n");
    
    gtk_container_add(GTK_CONTAINER(self_window), self_box);
    //g_print("showing window\n");
    
    //align window in center
    gtk_window_set_position(GTK_WINDOW(self_window), GTK_WIN_POS_CENTER);
    
    //make the window deletable
    gtk_window_set_deletable(GTK_WINDOW(self_window), TRUE);
    
    //set the window icon
    gtk_window_set_icon(GTK_WINDOW(self_window), icon);
    
    //show the window
    gtk_widget_show(self_window);
    
    //make the window active
    //gtk_window_activate_focus(GTK_WINDOW(self_window));
}

Window::~Window() {
    // TODO Auto-generated destructor stub
}

void Window::create_content(string tagName, string text, map<string,string> optionsMap, GtkWidget* box) {
    string msg;
    if(tagName == "title") {
        
        //make the title lable
        GtkWidget* title_label;
        title_label = gtk_label_new(text.c_str());
        
        //set alignment
        gtk_misc_set_alignment(GTK_MISC(title_label),0,0);
        
        //title markup
        char* markup = g_strconcat("<big>", text.c_str(), "</big>", NULL);
        gtk_label_set_markup(GTK_LABEL(title_label), markup);
        g_free(markup);
        gtk_misc_set_alignment(GTK_MISC(title_label), 0, 0);

        //add title to box
        gtk_box_pack_start(GTK_BOX(box), title_label, FALSE, FALSE, 0);
                
        //show the label
        gtk_widget_show(title_label);
        
    } else if(tagName == "button") {

        GtkWidget* button;
        
        //create new button
        button = gtk_button_new_with_label(text.c_str());
        //set name
        gtk_widget_set_name(button,optionsMap["options"].c_str());
        
        //set signal connect method
        g_signal_connect(button, "clicked", G_CALLBACK(button_pressed), FALSE);
        
        //connect it to the destroy event as well
        g_signal_connect_swapped(button,"clicked",G_CALLBACK(destroy),self_window);
        
        //add it to the box
        gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);
        
        //show the button
        gtk_widget_show(button);
                
    } else if(tagName == "label") {
        GtkWidget* label;
        
        //create new label
        label = gtk_label_new(text.c_str());
        
        //set alignment
        gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
        
        //add to box
        gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
        
        //show the label
        gtk_widget_show(label);
        
    } else if (tagName == "entry") {
        
        //allocate pointer
        GtkWidget* entry;
        
        //create new entry and assign it to the pointer
        entry = gtk_entry_new();
        
        //set the name
        gtk_widget_set_name(entry,text.c_str());
        
        //check if it's a password box
        if (optionsMap["type"] == "psw") {
            gtk_entry_set_visibility(GTK_ENTRY(entry),FALSE);
        }
        
        //add it to the box
        gtk_box_pack_start(GTK_BOX(box),entry,FALSE,FALSE,0);
        
        //show the entry
        gtk_widget_show(entry);
        
        //adding entries to entries map
        entries.insert(make_pair(text,entry));
        
    } else if (tagName == "pbar") {
        
        msg = "creating pbar: " + text + "\n";
        g_print("%s",msg.c_str());
        
        pbar = gtk_progress_bar_new();
        
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(pbar),text.c_str());
        
        gtk_box_pack_start(GTK_BOX(box),pbar,FALSE,FALSE,0);
        
        gtk_widget_show(pbar);
        
    }
}

void Window::button_pressed(GtkWidget* widget, gpointer data) {
    
    string xml = "";
    
    string optionsRaw = gtk_widget_get_name(widget); //the options are stored in the button name
    stringstream optsep(optionsRaw); //now we need a string stream object to extract them
    map<string, string> optionsMap; //and a map to store them
    
    string optvalpair; //this will store the pairs of option:value
    
    while(getline(optsep,optvalpair,',')) { //loop through each comma separated option:value pair
        
        stringstream seperator(optvalpair); //another string stream object to separate the pair
        string option;
        string value;
        getline(seperator,option,':'); //get the option
        getline(seperator,value,':'); //get the value
        optionsMap.insert(make_pair(option,value)); //add them to the map
    }
    
    //gchar* msg = g_strconcat("hacker voice: button \"",optionsMap["name"].c_str(),"\" clicked\n",NULL);
    //g_print("%s",msg);
    
    string name = optionsMap["name"];
    string msg;
    
    if(name == "quit") {
        
        gtk_main_quit();
    } else if(name == "customer_sign_in") {
        
        g_print("in customer_sign_in block\n");
        
        string email = gtk_entry_get_text(GTK_ENTRY(entries["email"]));
        string psw = gtk_entry_get_text(GTK_ENTRY(entries["psw"]));
        
        msg = "email: " + email + "\n";
        g_print("%s",msg.c_str());
        msg = "password: " + psw + "\n";
        g_print("%s",msg.c_str());
        
        
        
    } else if(name == "employee_login") {
        
        unsigned id = atoi(gtk_entry_get_text(GTK_ENTRY(entries["id"])));
        string psw = gtk_entry_get_text(GTK_ENTRY(entries["psw"]));
        
        msg = "id: " + to_string(id) + "\n";
        g_print("%s",msg.c_str());
        msg = "password: " + psw + "\n";
        g_print("%s",msg.c_str());
        
    } else if(name == "customer_create_new_account_1") {
        
        //get parameters
        string email = gtk_entry_get_text(GTK_ENTRY(entries["email"]));
        string conf_email = gtk_entry_get_text(GTK_ENTRY(entries["conf_email"]));
        string psw = gtk_entry_get_text(GTK_ENTRY(entries["psw"]));
        string conf_psw = gtk_entry_get_text(GTK_ENTRY(entries["conf_psw"]));
        
        //time to check
        
        
    } else if(name == "customer_db_search"){
        
        string value = optionsMap["value"];
        
        if(value == "comp_name"){
            
            stringstream productsSS;
            products->printToFile(productsSS);

            xml += "<hbox homogeneous=\"false\">\n";
            xml += create_xml_tag("label","company"); //manf comp
            xml += create_xml_tag("label","model"); //model
            xml += create_xml_tag("label","screen size"); //screen size
            xml += create_xml_tag("label","cpu gen"); //cpu gen
            xml += create_xml_tag("label","year"); //year
            xml += create_xml_tag("label","price"); //price
            xml += create_xml_tag("label"," "); //order column
            xml += "</hbox>\n";            

            xml += "<sroll>\n";

            string attribute;
            
            while(getline(productsSS,attribute)){
                
                xml += "<hbox homogeneous=\"false\">\n";
                
                xml += create_xml_tag("label",attribute); //manf comp
                
                getline(productsSS,attribute);
                xml += create_xml_tag("label",attribute); //model
                
                getline(productsSS,attribute);
                xml += create_xml_tag("label",attribute); //screen size
                
                getline(productsSS,attribute);
                xml += create_xml_tag("label",attribute); //cpu gen
                
                getline(productsSS,attribute);
                xml += create_xml_tag("label",attribute); //year
                
                getline(productsSS,attribute);
                xml += create_xml_tag("label",attribute); //price

                xml += create_xml_tag("button","order!");
                
                xml += "</hbox>\n";

                getline(productsSS,attribute); //skip line in between products
            }
            
            xml += "</scoll>\n";
            
            
        } else if(value == "price"){
            
            
        }
        
    }
    
    /*else if(name == "employee_view_orders") {
     string orders = prioiry_queue.printSorted();
     
     stringstream ordersSep(orders);
     
     string order;
     
     xml += "<hbox homogeneous\"true\">\n";
     
     while(getline(ordersSep,order)){
     
     //get each order in the queue
     
     stringstream orderSep(order);
     
     string item;
     
     while(getline(orderSep,item,',')){
     
     xml += "<label>\n";
     xml += item + "\n";
     xml += "</label>\n";
     
     }
     }
     
     xml += "</hbox>";
     }*/
    
    /**
     @TODO
     - execute options based on if else to detect name and proceed accordingly
     (so we will still need the huge if-elif stack)
     [begun]
     - destroy this window [done]
     */
    
    WindowManager::go_to_window(optionsMap["link"],xml); //go to the new window
}

string Window::create_xml_tag(string tag, string text){
    
    string xml;
    xml += "<" + tag + ">\n";
    xml += text + "\n";
    xml += "</" + tag + ">\n";
    return xml;
}

string Window::create_xml_tag_with_options(string tag, string text){
    
    string xml;
    xml += "<" + tag + ">\n";
    xml += text + "\n";
    xml += "</" + tag + ">\n";
    return xml;
}

gboolean Window::delete_event(GtkWidget *widget, GdkEvent *event, gpointer data) { //called when close window red button is pressed at app level
    
    string msg = "current window id: " + WindowManager::get_current_window_id() + "\n";
    g_print("%s",msg.c_str());
    
    
    if(WindowManager::get_current_window_id() == "end_screen") {
        gtk_main_quit();
        return TRUE;
    } else {
        
        WindowManager::go_to_window("end_screen","");
        //WindowManager::run_pbar();
        return FALSE;
    }
    
}

void Window::destroy(GtkWidget* widget, gpointer data) { //called from regular buttons
    
    gtk_widget_destroy(widget);
    
}

void Window::increase_pbar() {
    
    gdouble frac = gtk_progress_bar_get_fraction(GTK_PROGRESS_BAR(pbar));
    frac += 0.1;
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pbar),frac);
    
}

void Window::set_icon(string path) {
    
    GdkPixbuf *pixbuf;
    GError *error = NULL;
    pixbuf = gdk_pixbuf_new_from_file(path.c_str(), &error);
    
    if (!pixbuf) {
        
        fprintf(stderr, "%s\n", error->message);
        g_error_free(error);
    }
    
    icon = pixbuf;
    
}

void Window::assign_pointers(Heap* heap, BST<Customer>* _customers, BST<Product>* _products) {
    
    //priority_queue = &heap;
    //customers = &_customers;
	products = _products;

    g_print("pointers assigned\n");    
}


/*
 Deprecated consturctor
 Window::Window(string title, string labels_text[], string buttons_text[], string layout) {
 
 //create the window
 self_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
 g_signal_connect(self_window, "delete-event", G_CALLBACK(delete_event), NULL);
 gtk_container_set_border_width(GTK_CONTAINER(self_window), 10);
 
 //create box
 self_box = gtk_vbox_new(FALSE, 2);
 
 //make the title lable
 GtkWidget* label;
 label = gtk_label_new(title.c_str());
 
 //title markup
 char* markup = g_strconcat("<big>", title.c_str(), "</big>", NULL);
 gtk_label_set_markup(GTK_LABEL(label), markup);
 g_free(markup);
 gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
 
 //show the label
 gtk_widget_show(label);
 
 //add title to box
 gtk_box_pack_start(GTK_BOX(self_box), label, FALSE, FALSE, 0);
 
 //create secondary labels
 for (int i = 0; i < sizeof(labels_text) / sizeof(string); i++) {
 
 //create new label
 label = gtk_label_new(labels_text[i].c_str());
 //set alignment
 gtk_misc_set_alignment(GTK_MISC(label), 30, 0);
 //add to box
 gtk_box_pack_start(GTK_BOX(self_box), label, FALSE, FALSE, 0);
 
 //show the label
 gtk_widget_show(label);
 }
 
 GtkWidget* button;
 
 for(int i = 0; i < sizeof(buttons_text) / sizeof(string); i++) {
 
 //create new button
 button = gtk_button_new_with_label(buttons_text[i].c_str());
 //set name
 gtk_widget_set_name(button,buttons_text[i].c_str());
 
 //set alignment
 gtk_misc_set_alignment(GTK_MISC(button), 30, 0);
 
 //set signal connect method
 g_signal_connect(button, "clicked", G_CALLBACK(button_pressed), FALSE);
 
 //show the button
 gtk_widget_show(button);
 
 //add it to the box
 gtk_box_pack_start(GTK_BOX(self_box), button, FALSE, FALSE, 0);
 
 }
 
 gtk_container_add(GTK_CONTAINER(self_window), self_box);
 
 gtk_widget_show(self_window);
 }*/
