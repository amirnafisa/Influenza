#ifndef __CAIRO_DRAW__
#define __CAIRO_DRAW__

#include <cairo.h>
#include <gtk/gtk.h>
#include "simulation_calls.h"

#define WIDTH   640
#define HEIGHT  480

#define ZOOM_X  100.0
#define ZOOM_Y  100.0

static gboolean on_draw (GtkWidget *widget, cairo_t *cr, gpointer user_data);
void create_drawing_window (Trends *current_trends);
#endif
