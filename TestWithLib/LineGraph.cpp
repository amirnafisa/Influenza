#include "LineGraph.h"

//GtkWidget *plotwindow, *da;
static gboolean graph_flag = FALSE;
PlotData new_data;

gboolean draw_cb (GtkWidget* widget, cairo_t* cr, gpointer user_data)
{
  LineGraph new_graph (cr, &new_data);
  return TRUE;
}

void create_drawing_window (GtkWidget* da, PlotData* data)
{
  new_data = *data;
  gtk_widget_queue_draw(da);
}
