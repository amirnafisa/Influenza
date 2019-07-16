#include "LineGraph.h"

GtkWidget *plotwindow, *da;
static gboolean graph_flag = FALSE;
PlotData new_data;

static gboolean draw_cb (GtkWidget* widget, cairo_t* cr, gpointer user_data)
{

  LineGraph new_graph (cr);
  new_graph.plot(&new_data);

  return TRUE;
}

void close_plotwindow_cb (GtkWidget* widget, gpointer user_data)
{
  gtk_widget_destroy (widget);
  plotwindow = NULL;
}

void create_drawing_window (PlotData* data)
{
  if (plotwindow == NULL) {
    plotwindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (plotwindow), 720, 540);
    gtk_window_set_resizable (GTK_WINDOW (plotwindow), FALSE);
    da = gtk_drawing_area_new ();
    gtk_container_add (GTK_CONTAINER (plotwindow), da);

    g_signal_connect (da, "draw", G_CALLBACK (draw_cb), NULL);
    g_signal_connect (plotwindow, "destroy", G_CALLBACK (close_plotwindow_cb), NULL);
    gtk_widget_show_all(plotwindow);
  }

  new_data = *data;
  gtk_widget_queue_draw(da);

}
