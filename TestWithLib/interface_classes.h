#ifndef __INTERFACE_CLASSES__
#define __INTERFACE_CLASSES__

#include <gtk/gtk.h>

enum text_type{BLUE_FOREGROUND=1, PLAIN_OUTPUT=0};
enum {
  DAY, TEST_P, TEST_S, TEST, SUBMIT_P, SUBMIT_S, SUBMIT, SUBMIT_B, UNCOLLECT_P, UNCOLLECT_S, UNCOLLECT, MSSS_P, MSSS_S, MSSS, DISCARD_P, DISCARD_S, DISCARD, CAP_IND, N_COLUMNS
};


class INF_VBOX
{
public:
  GtkWidget* vbox;
private:
  GtkWidget* scrolled_window;
  GtkWidget *result_frame;
public:
  INF_VBOX (GtkWidget* master, gboolean frame, gboolean scrollable, gboolean box)
  {
    if (scrollable) {
      scrolled_window = gtk_scrolled_window_new (NULL, NULL);
      gtk_container_add (GTK_CONTAINER (master), scrolled_window);
    } else {
      scrolled_window = master;
    }

    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_halign (vbox, GTK_ALIGN_START);
    if (frame) {
      result_frame = gtk_frame_new ("");
      gtk_container_add (GTK_CONTAINER (result_frame), vbox);
    } else {
      result_frame = vbox;
    }
    if (box)
      gtk_box_pack_start (GTK_BOX (master), result_frame, TRUE, FALSE, 2);
    else
      gtk_container_add (GTK_CONTAINER(scrolled_window), result_frame);
  }
};

class INF_HBOX
{
public:
  GtkWidget* hbox;
public:
  INF_HBOX (GtkWidget* master, gboolean frame)
  {
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_widget_set_halign (hbox, GTK_ALIGN_START);
    if (frame) {
      GtkWidget *result_frame = gtk_frame_new ("");
      gtk_container_add (GTK_CONTAINER (result_frame), hbox);
      gtk_box_pack_start(GTK_BOX (master), result_frame, TRUE, TRUE, 2);
      //gtk_container_add (GTK_CONTAINER(master), result_frame);
    } else {
      gtk_box_pack_start(GTK_BOX (master), hbox, TRUE, TRUE, 2);
      //gtk_container_add (GTK_CONTAINER(master), hbox);
    }
  }
};

class INF_BUTTON
{
public:
  GtkWidget* button;

  INF_BUTTON (GtkWidget* master, const gchar text[], void (*func_cb)(GtkWidget*, gpointer), gpointer user_data)
  {
    button = gtk_button_new_with_label (text);
    gtk_widget_set_halign (button, GTK_ALIGN_START);
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
    gtk_window_set_default_size (GTK_WINDOW (window), 1000, 600);
  }
};

class INF_INPUT_VIEW {
private:
  GtkSizeGroup *input_group;
  GtkWidget *entry[CONSTANTS_SIZE], *label;
  const gchar* INF_CONSTANTS[CONSTANTS_SIZE] = {
    "Simulation Days",
    "Reproduction Number: Pandemic",
    "Reproduction Number: Seasonal",
    "Scaling",
    "Out of State Travel",
    "PHL Sampling Criteria",
    "Number of PHLs",
    "PHL Capacity",
    "PHL working on Weekend",
    "Travel between cities"
  };
public:
  INF_INPUT_VIEW (GtkWidget* master)
  {

    INF_VBOX enclosing_label_box (master, FALSE, FALSE, TRUE);
    g_object_set (enclosing_label_box.vbox, "margin", 5, NULL);
    INF_VBOX enclosing_entry_box (master, FALSE, FALSE, TRUE);
    g_object_set (enclosing_entry_box.vbox, "margin", 5, NULL);
    input_group	= gtk_size_group_new (GTK_SIZE_GROUP_BOTH);

    for (gint i = 0; i < CONSTANTS_SIZE; i++)
    {
      label = gtk_label_new (INF_CONSTANTS[i]);
      gtk_widget_set_halign (label, GTK_ALIGN_START);
      gtk_size_group_add_widget (input_group,  label);
      gtk_label_set_xalign (GTK_LABEL (label), 0.);
      entry[i] = gtk_entry_new ();
      gtk_widget_set_halign (entry[i], GTK_ALIGN_START);
      gtk_size_group_add_widget (input_group,  entry[i]);
      gtk_box_pack_start(GTK_BOX (enclosing_label_box.vbox), label, FALSE, TRUE, 2);
      gtk_box_pack_start(GTK_BOX (enclosing_entry_box.vbox), entry[i], FALSE, TRUE, 2);
    }
  }

  void populate_entries (gdouble data[])
  {
      gchar buffer[50];
      for (gint i = 0; i < CONSTANTS_SIZE; i++)
      {
        sprintf (buffer, "%f", data[i]);
        gtk_entry_set_text (GTK_ENTRY (entry[i]), buffer);
      }
  }

  void get_data_from_entries (gdouble data[])
  {
    for (gint i = 0; i < CONSTANTS_SIZE; i++)
    {
      data[i] = std::stof (gtk_entry_get_text (GTK_ENTRY (entry[i])));

    }
  }
};

class INF_RESULT_VIEW {
public:
  GtkWidget *tree, *da, *combo_box;
private:
  GtkTreeStore *store;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkTreeIter iter;
  gint i;
public:
  INF_RESULT_VIEW (GtkWidget* master, void (*func_cb)(GtkComboBox*, gpointer))
  {


    GtkWidget *scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_halign (scrolled_window, GTK_ALIGN_START);
    /* Create a model.  We are using the store model for now, though we
     * could use any other GtkTreeModel */
    store = gtk_tree_store_new (N_COLUMNS, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT,
                                G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT,
                                G_TYPE_INT, G_TYPE_INT,   G_TYPE_INT, G_TYPE_INT, G_TYPE_INT,
                                G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);

    // Create a view
    tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
    gtk_widget_set_halign (tree, GTK_ALIGN_START);
    g_object_unref (G_OBJECT (store));

    //Add and render columns
    add_column_to_tree ("Day", "text", DAY);
    add_column_to_tree ("test_p", "text", TEST_P);
    add_column_to_tree ("test_s", "text", TEST_S);
    add_column_to_tree ("test", "text", TEST);
    add_column_to_tree ("submit_p", "text", SUBMIT_P);
    add_column_to_tree ("submit_s", "text", SUBMIT_S);
    add_column_to_tree ("submit", "text", SUBMIT);
    add_column_to_tree ("submit_b", "text", SUBMIT_B);
    add_column_to_tree ("uncollect_p", "text", UNCOLLECT_P);
    add_column_to_tree ("uncollect_s", "text", UNCOLLECT_S);
    add_column_to_tree ("uncollect", "text", UNCOLLECT);
    add_column_to_tree ("msss_p", "text", MSSS_P);
    add_column_to_tree ("msss_s", "text", MSSS_S);
    add_column_to_tree ("msss", "text", MSSS);
    add_column_to_tree ("discard_p", "text", DISCARD_P);
    add_column_to_tree ("discard_s", "text", DISCARD_S);
    add_column_to_tree ("discard", "text", DISCARD);
    add_column_to_tree ("cap_ind", "text", CAP_IND);

    gtk_widget_set_size_request (scrolled_window, 700, 100);
    gtk_container_add (GTK_CONTAINER (scrolled_window), tree);
    gtk_box_pack_start(GTK_BOX (master), scrolled_window, TRUE, FALSE, 2);

    combo_box = gtk_combo_box_text_new ();
    gtk_widget_set_halign (combo_box, GTK_ALIGN_START);
    const char *distros[] = {"Select Trend to view Graph", "test_p", "test_s", "test", "submit_p", "submit_s","submit",
    "submit_b", "uncollect_p", "uncollect_s", "uncollect", "msss_p", "msss_s", "msss", "discard_p", "discard_s", "discard", "cap_ind"};
    for (i = 0; i < G_N_ELEMENTS (distros); i++){
  	   gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combo_box), distros[i]);
    }
    gtk_combo_box_set_active (GTK_COMBO_BOX (combo_box), 0);
    gtk_box_pack_start(GTK_BOX (master), combo_box, TRUE, FALSE, 2);

    da = gtk_drawing_area_new ();

    GtkWidget *scrolled_window2 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_halign (scrolled_window2, GTK_ALIGN_START);
    gtk_widget_set_size_request (scrolled_window2, 700, 540);
    gtk_container_add (GTK_CONTAINER (scrolled_window2), da);
    gtk_box_pack_start(GTK_BOX (master), scrolled_window2, TRUE, TRUE, 2);


    g_signal_connect (combo_box, "changed", G_CALLBACK (func_cb), (gpointer) da);
    g_signal_connect (da, "draw", G_CALLBACK (draw_cb), NULL);
  }

  void add_column_to_tree (const gchar title[], const gchar mode[], gint col)
  {
    /* Last column.. whether a book is checked out. */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (title,
                                                       renderer,
                                                       mode, col,
                                                       NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);
  }
  void insert_data (gint data[])
  {
    // Add a new row to the model
    gtk_tree_store_append (store, &iter, NULL);
    gtk_tree_store_set (store, &iter,
                        DAY, data[DAY], TEST_P, data[TEST_P], TEST_S, data[TEST_S], TEST, data[TEST], SUBMIT_P, data[SUBMIT_P], SUBMIT_S, data[SUBMIT_S], SUBMIT, data[SUBMIT], SUBMIT_B, data[SUBMIT_B], UNCOLLECT_P, data[UNCOLLECT_P], UNCOLLECT_S, data[UNCOLLECT_S], UNCOLLECT, data[UNCOLLECT], MSSS_P, data[MSSS_P], MSSS_S, data[MSSS_S], MSSS, data[MSSS], DISCARD_P, data[DISCARD_P], DISCARD_S, data[DISCARD_S], DISCARD, data[DISCARD], CAP_IND, data[CAP_IND],
                        -1);
  }
  void clear_table ()
  {
    gtk_tree_store_clear(store);
  }
};
#endif
