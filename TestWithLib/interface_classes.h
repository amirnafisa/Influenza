#ifndef __INTERFACE_CLASSES__
#define __INTERFACE_CLASSES__

enum text_type{BLUE_FOREGROUND=1, PLAIN_OUTPUT=0};

class INF_VBOX
{
public:
  GtkWidget* vbox;
public:
  INF_VBOX (GtkWidget* master)
  {
    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
    gtk_container_add (GTK_CONTAINER(master), vbox);
  }
};

class INF_HBOX
{
public:
  GtkWidget* hbox;
public:
  INF_HBOX (GtkWidget* master)
  {
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_container_add (GTK_CONTAINER(master), hbox);
  }
};

class INF_BUTTON
{
public:
  GtkWidget* button;

  INF_BUTTON (GtkWidget* master, const gchar text[], void (*func_cb)(GtkWidget*, gpointer), gpointer user_data)
  {
    button = gtk_button_new_with_label (text);
    g_signal_connect (button, "clicked", G_CALLBACK (func_cb), (gpointer) user_data);
    g_object_set (button, "margin", 5, NULL);
    gtk_box_pack_start(GTK_BOX (master), button, FALSE, TRUE, 2);
  }

};

class INF_MAINWINDOW
{
public:
  GtkWidget* window;

  INF_MAINWINDOW (GtkApplication* app, const gchar title[])
  {
    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), title);
    gtk_window_set_default_size (GTK_WINDOW (window), 600, 600);
  }
};
class INF_TEXT {
public:
  GtkWidget *text_view;
private:
  GtkWidget* label;
  GtkWidget *text_scrolledwindow;
  GtkTextTag *tag_heading, *tag_text;
  GtkTextBuffer *buffer;
  GtkTextIter iter, start, end;

public:
  INF_TEXT (GtkWidget* master)
  {
    INF_VBOX sub_vbox (master);
    label = gtk_label_new ("day\ttest_p\ttest_s\ttest\tsubmit_p\tsubmit_s\tsubmit\tsumit_b\tuncollect_p\tuncollect_s\tuncollect\tMSSS_p\tMSSS_s\tMSSS\tdiscard_p\tdiscard_s\tdiscard\tCAP_ind");
    g_object_set (label, "margin", 5, NULL);
    gtk_box_pack_start(GTK_BOX (sub_vbox.vbox), label, FALSE, TRUE, 2);

    text_scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
    g_object_set (text_scrolledwindow, "margin", 5, NULL);

    buffer = gtk_text_buffer_new (NULL);
    text_view = gtk_text_view_new_with_buffer (buffer);
    gtk_widget_set_hexpand(text_view, TRUE);
    gtk_widget_set_vexpand(text_view, TRUE);

    gtk_text_view_set_left_margin (GTK_TEXT_VIEW (text_view), 30);

    gtk_container_add(GTK_CONTAINER(text_scrolledwindow), text_view);
    gtk_box_pack_start(GTK_BOX (sub_vbox.vbox), text_scrolledwindow, FALSE, TRUE, 2);

    tag_heading = gtk_text_buffer_create_tag (buffer, "blue_foreground", "foreground", "red", "background", "yellow", NULL);
    tag_text = gtk_text_buffer_create_tag (buffer, "plain_output", "foreground", "black", "background", "white", NULL);

  }

  void add_text (const gchar text[], text_type tag)
  {
    gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
    if (tag == BLUE_FOREGROUND)
      gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, text, -1, "blue_foreground", NULL);
    else
      gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, text, -1, "plain_output", NULL);
  }

  void clear_text ()
  {
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    gtk_text_buffer_delete(buffer, &start, &end);
  }
};


#endif
