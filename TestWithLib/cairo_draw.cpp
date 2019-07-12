#include "cairo_draw.h"

static gboolean on_draw (GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    GdkRectangle da;            /* GtkDrawingArea size */
    gdouble dx = 1.0, dy = 1.0; /* Pixels between each point */
    gdouble clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;
    GdkWindow *window = gtk_widget_get_window(widget);

    /* Determine GtkDrawingArea dimensions */
    gdk_window_get_geometry (window, &da.x, &da.y, &da.width, &da.height);

    /* Draw on a white background */
    cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
    cairo_paint (cr);

    /* Change the transformation matrix */
    cairo_translate (cr, da.width / 2, da.height / 2);
    cairo_scale (cr, 10, 10);

    /* Determine the data points to calculate (ie. those in the clipping zone */
    cairo_device_to_user_distance (cr, &dx, &dy);
    cairo_clip_extents (cr, &clip_x1, &clip_y1, &clip_x2, &clip_y2);
    cairo_set_line_width (cr, dx);

    /* Draws x and y axis */
    cairo_set_source_rgb (cr, 0.2, 0.4, 0.6);
    cairo_move_to (cr, clip_x1, 0.0);
    cairo_line_to (cr, clip_x2, 0.0);
    cairo_move_to (cr, 0.0, clip_y1);
    cairo_line_to (cr, 0.0, clip_y2);
    cairo_stroke (cr);

    Trends* current_trends = (Trends*) user_data;
    cairo_set_font_size(cr, 1);
    cairo_set_source_rgba (cr, 1, 0.1, 0.1, 0.1);
    for (gint i = clip_x1; i <= clip_x2; i++)
    {
        cairo_move_to (cr, i, 0);
        cairo_show_text (cr, g_strdup_printf("%i\t", i));

    }
    /* Draw the curve */
    cairo_stroke (cr);

    for (gint i = clip_y1; i <= clip_y2; i++)
    {
        cairo_move_to (cr, 0, i);
        cairo_show_text (cr, g_strdup_printf("%i\t", i));

    }
    /* Draw the curve */
    cairo_stroke (cr);

    /* Link each data point */
    //for (gint i = 0; i < current_trends->idx; i++)
    //    cairo_line_to (cr, i, current_trends->data[i].test_p);

    cairo_set_source_rgba (cr, 1, 0.1, 0.1, 0.1);
    for (gint i = 0; i < 10; i++)
    {
        cairo_move_to (cr, i, i);
        cairo_line_to (cr, i+1, i+1);

    }
    /* Draw the curve */

    cairo_stroke (cr);

    /* Link each data point */
    //for (gint i = 0; i < current_trends->idx; i++)
    //    cairo_line_to (cr, i, current_trends->data[i].submit_p);

    /* Draw the curve */
    //cairo_set_source_rgba (cr, 1, 0.5, 0.2, 0.3);
    //cairo_stroke (cr);

    return FALSE;
}

void create_drawing_window (Trends* current_trends)
{
  GtkWidget *da;
  GtkWidget *window;
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size (GTK_WINDOW (window), WIDTH, HEIGHT);
  gtk_window_set_title (GTK_WINDOW (window), "Trends");

  da = gtk_drawing_area_new ();
  gtk_container_add (GTK_CONTAINER (window), da);

  g_signal_connect (G_OBJECT (da), "draw", G_CALLBACK (on_draw), (gpointer) current_trends);

  gtk_widget_show_all (window);
}
