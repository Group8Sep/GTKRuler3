/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GTK+ Team and others 1997-1999.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/. 
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include "gtkhruler.h"
#include "gtkruler.h"


#define RULER_HEIGHT          14
#define MINIMUM_INCR          5
#define MAXIMUM_SUBDIVIDE     5
#define MAXIMUM_SCALES        10

#define ROUND(x) ((int) ((x) + 0.5))


static void gtk_hruler_class_init    (GtkHRulerClass *klass);
static void gtk_hruler_init          (GtkHRuler      *hruler);
static gint gtk_hruler_motion_notify (GtkWidget      *widget,
				      GdkEventMotion *event);
static void gtk_hruler_draw_ticks    (GtkRuler       *ruler);
static void gtk_hruler_draw_pos      (GtkRuler       *ruler);


G_DEFINE_TYPE(GtkHRuler, gtk_hruler, GTK_TYPE_RULER);

/*
GType
gtk_hruler_get_type (void)
{
  static guint hruler_type = 0;


    if (!hruler_type)
    {
        static GTypeInfo hruler_info = {0,  };
        hruler_info.class_size = sizeof (GtkHRulerClass);
        hruler_info.base_init = NULL;
        hruler_info.base_finalize = NULL;
        hruler_info.class_init = (GClassInitFunc)gtk_hruler_class_init;
        hruler_info.class_finalize = NULL;
        hruler_info.class_data = NULL;
        hruler_info.instance_size = sizeof (GtkHRuler);
        hruler_info.n_preallocs = 0;
        hruler_info.instance_init = (GInstanceInitFunc) gtk_hruler_init;

        hruler_type = g_type_register_static (gtk_ruler_get_type(), "GtkHRuler", &hruler_info, 0);
    }

  return hruler_type;
}
*/
static void
gtk_hruler_class_init (GtkHRulerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    GtkRulerClass *ruler_class = GTK_RULER_CLASS(klass);


  widget_class->motion_notify_event = gtk_hruler_motion_notify;

  ruler_class->draw_ticks = gtk_hruler_draw_ticks;
  ruler_class->draw_pos = gtk_hruler_draw_pos;

    //g_type_class_add_private(object_class, sizeof (GtkHRuler));
}

static void
gtk_hruler_init (GtkHRuler *hruler)
{
  GtkWidget *widget;

  widget = GTK_WIDGET (hruler);
    gtk_widget_set_size_request(widget,
                                gtk_widget_get_style(widget)->xthickness * 2 + 1,
                                gtk_widget_get_style(widget)->ythickness * 2 + RULER_HEIGHT);
}


GtkWidget*
gtk_hruler_new (void)
{
  return g_object_new(GTK_TYPE_HRULER, NULL);
}

static gint
gtk_hruler_motion_notify (GtkWidget      *widget,
			  GdkEventMotion *event)
{
  GtkRuler *ruler;
  gint x;

  g_return_val_if_fail (widget != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_HRULER (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  ruler = GTK_RULER (widget);

  if (event->is_hint)
    gdk_window_get_pointer (gtk_widget_get_window(widget), &x, NULL, NULL);
  else
    x = event->x;

  ruler->position = ruler->lower + ((ruler->upper - ruler->lower) * x) / gtk_widget_get_allocated_width(widget);

  /*  Make sure the ruler has been allocated already  */
  if (ruler->backing_store != NULL)
    gtk_ruler_draw_pos (ruler);

  return FALSE;
}

static void
gtk_hruler_draw_ticks (GtkRuler *ruler) {
    GtkWidget *widget;
    cairo_t *gc, *bg_gc;
    cairo_font_options_t *font;
    gint i;
    gint width, height;
    gint xthickness;
    gint ythickness;
    gint length, ideal_length;
    gfloat lower, upper;        /* Upper and lower limits, in ruler units */
    gfloat increment;        /* Number of pixels per unit */
    gint scale;            /* Number of units per major unit */
    gfloat subd_incr;
    gfloat start, end, cur;
    gchar unit_str[32];
    gint digit_height;
    gint text_width;
    gint pos;

    g_return_if_fail (ruler != NULL);
    g_return_if_fail (GTK_IS_HRULER(ruler));

    if (!gtk_widget_is_drawable(ruler))
        return;

    widget = GTK_WIDGET (ruler);

    gdk_cairo_set_source_color(gc, &gtk_widget_get_style(widget)->fg[GTK_STATE_NORMAL]);
    gdk_cairo_set_source_color(bg_gc, &gtk_widget_get_style(widget)->bg[GTK_STATE_NORMAL]);
    font = gtk_widget_get_font_options(widget);

    xthickness = gtk_widget_get_style(widget)->xthickness;
    ythickness = gtk_widget_get_style(widget)->ythickness;
//  digit_height = font->ascent; /* assume descent == 0 ? */

    width = gtk_widget_get_allocated_width(widget) - ythickness * 2;
    height = gtk_widget_get_allocated_height(widget); //TODO check if this is correct


    gtk_paint_box(gtk_widget_get_style(widget),
                  (cairo_t *) ruler->backing_store,
                  GTK_STATE_NORMAL,
                  GTK_SHADOW_OUT,
                  widget,
                  "hruler",
                  0, 0,
                  gtk_widget_get_allocated_width(widget), gtk_widget_get_allocated_height(widget));

    cairo_move_to(gc, xthickness, width + ythickness);
    cairo_line_to(gc, gtk_widget_get_allocated_width(widget) - xthickness, width + ythickness);
    cairo_stroke(gc);

    upper = ruler->upper / ruler->metric->pixels_per_unit;
    lower = ruler->lower / ruler->metric->pixels_per_unit;

    if ((upper - lower) == 0)
        return;
    increment = (gfloat) width / (upper - lower);

    /* determine the scale
     *  We calculate the text size as for the vruler instead of using
     *  text_width = gdk_string_width(font, unit_str), so that the result
     *  for the scale looks consistent with an accompanying vruler
     */
    scale = ceil(ruler->max_size / ruler->metric->pixels_per_unit);
    sprintf(unit_str, "%d", scale);
    text_width = strlen(unit_str) * digit_height + 1;

    for (scale = 0; scale < MAXIMUM_SCALES; scale++)
        if (ruler->metric->ruler_scale[scale] * fabs(increment) > 2 * text_width)
            break;

    if (scale == MAXIMUM_SCALES)
        scale = MAXIMUM_SCALES - 1;

    /* drawing starts here */
    length = 0;
    for (i = MAXIMUM_SUBDIVIDE - 1; i >= 0; i--) {
        subd_incr = (gfloat) ruler->metric->ruler_scale[scale] /
                    (gfloat) ruler->metric->subdivide[i];
        if (subd_incr * fabs(increment) <= MINIMUM_INCR)
            continue;

        /* Calculate the length of the tickmarks. Make sure that
         * this length increases for each set of ticks
         */
        ideal_length = height / (i + 1) - 1;
        if (ideal_length > ++length)
            length = ideal_length;

        if (lower < upper) {
            start = floor(lower / subd_incr) * subd_incr;
            end = ceil(upper / subd_incr) * subd_incr;
        } else {
            start = floor(upper / subd_incr) * subd_incr;
            end = ceil(lower / subd_incr) * subd_incr;
        }


        for (cur = start; cur <= end; cur += subd_incr) {
            pos = ROUND ((cur - lower) * increment);

//	  gdk_draw_line (ruler->backing_store, gc,
//			 pos, height + ythickness,
//			 pos, height - length + ythickness);
            cairo_rectangle(gc, pos, height + ythickness - length, length, 1);

            /* draw label */
            if (i == 0) {
                sprintf(unit_str, "%d", (int) cur);
                cairo_move_to(gc, pos + 2, ythickness + digit_height - 1);
                cairo_set_font_options(gc, font);
                cairo_show_text(gc, unit_str);
//	      gdk_draw_string (ruler->backing_store, font, gc,
//			       pos + 2, ythickness + font->ascent - 1,
//			       unit_str);
//	    }
            }
        }
    }
}

static void
gtk_hruler_draw_pos (GtkRuler *ruler)
{
  GtkWidget *widget;
  cairo_t *gc;
  int i;
  gint x, y;
  gint width, height;
  gint bs_width, bs_height;
  gint xthickness;
  gint ythickness;
  gfloat increment;

  g_return_if_fail (ruler != NULL);
  g_return_if_fail (GTK_IS_HRULER (ruler));

  if (gtk_widget_is_drawable(ruler))
    {
      widget = GTK_WIDGET (ruler);

      gc = gdk_cairo_create(gtk_widget_get_window(widget));
      xthickness = gtk_widget_get_style(widget)->xthickness;
      ythickness = gtk_widget_get_style(widget)->ythickness;
      width = gtk_widget_get_allocated_width(widget) - ythickness * 2;
      height = gtk_widget_get_allocated_height(widget); //TODO check this

      bs_width = height / 2;
      bs_width |= 1;  /* make sure it's odd */
      bs_height = bs_width / 2 + 1;

      if ((bs_width > 0) && (bs_height > 0))
	{
	  /*  If a backing store exists, restore the ruler  */
	  if (ruler->backing_store && ruler->non_gr_exp_gc)
//	    gdk_draw_pixmap (ruler->widget.window,
//			     ruler->non_gr_exp_gc,
//			     ruler->backing_store,
//			     ruler->xsrc, ruler->ysrc,
//			     ruler->xsrc, ruler->ysrc,
//			     bs_width, bs_height);
        cairo_surface_create_for_rectangle(gtk_widget_get_window(widget), ruler->xsrc, ruler->ysrc, bs_width, bs_height);

	  increment = (gfloat) width / (ruler->upper - ruler->lower);

	  x = ROUND ((ruler->position - ruler->lower) * increment) + (xthickness - bs_width) / 2 - 1;
	  y = (height + bs_height) / 2 + ythickness;

	  for (i = 0; i < bs_height; i++)
	    cairo_rectangle(gc, x + i, y + i, bs_width - 1 - i, 1);
//	    gdk_draw_line (widget->window, gc,
//			   x + i, y + i,
//			   x + bs_width - 1 - i, y + i);


	  ruler->xsrc = x;
	  ruler->ysrc = y;
	}
    }
}
