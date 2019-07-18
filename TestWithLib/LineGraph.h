#ifndef __LINEGRAPH__
#define __LINEGRAPH__

#include <gtk/gtk.h>
#include <plplot/plstream.h>

#define SIMULATION_DAYS_LIMIT 200
#define N_TRENDS 18

typedef struct {
  PLFLT y[SIMULATION_DAYS_LIMIT];
  PLINT n_entries;
  const gchar *title;
  const gchar *y_label;
}PlotData;

class LineGraph {
public:
  PLFLT xmin = 0., xmax = 0., ymin = 0., ymax = 0.;
    PLINT just=0, axis=0;
    plstream *pls;
  LineGraph (cairo_t *cr, PlotData* data)
  {
      pls = new plstream();  // declare plplot object
      pls->sdev("extcairo");
      pls->init();
      pls->cmd(PLESC_DEVINIT, cr);

      if (data->n_entries > 0) {
        xmax = data->n_entries*1.1;
        for (gint i: data->y)
          if (i > ymax)
            ymax = i;

        ymax *= 1.1;
        pls->env(xmin, xmax, ymin, ymax, just, axis );
        pls->lab( "Days", data->y_label, data->title);
        plot(data);
      }
  }
  ~LineGraph ()
  {
    delete pls;
  }
  void plot (PlotData *data)
  {
    PLFLT x[data->n_entries];
    for (gint i = 0; i < data->n_entries; i++)
      x[i] = (PLFLT) i+1;
    pls->line( data->n_entries, x, data->y );
  }
  void clear ()
  {
    pls->adv(0);
  }
};

gboolean draw_cb (GtkWidget* widget, cairo_t* cr, gpointer user_data);
void create_drawing_window (GtkWidget* da, PlotData* data);
void close_plotwindow_cb (GtkWidget* widget, gpointer user_data);
#endif
