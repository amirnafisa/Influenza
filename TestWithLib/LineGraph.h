#ifndef __LINEGRAPH__
#define __LINEGRAPH__

#include <gtk/gtk.h>
#include <plplot/plstream.h>

typedef struct {
  PLFLT x[200];
  PLFLT y[200];
  PLINT n_entries;
}PlotData;

class LineGraph {
public:
  PLFLT xmin = 0., xmax = 10., ymin = 0., ymax = 500.;
    PLINT just=0, axis=0;
    plstream *pls;
  LineGraph (cairo_t *cr)
  {
      pls = new plstream();  // declare plplot object
      pls->sdev("extcairo");
      pls->init();
      pls->cmd(PLESC_DEVINIT, cr);
      pls->env(xmin, xmax, ymin, ymax, just, axis );
      pls->lab( "(x)", "(y)", "PlPlot example title");
  }
  ~LineGraph ()
  {
    delete pls;
  }
  void plot (PlotData *data)
  {
    pls->line( data->n_entries, data->x, data->y );
  }
  void clear ()
  {
    pls->adv(0);
  }
};

static gboolean draw_cb (GtkWidget* widget, cairo_t* cr, gpointer user_data);
void create_drawing_window (PlotData* data);
void close_plotwindow_cb (GtkWidget* widget, gpointer user_data);
#endif
