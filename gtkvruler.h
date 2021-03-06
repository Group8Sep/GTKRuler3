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

#ifndef __GTK_VRULER_H__
#define __GTK_VRULER_H__


#include <gdk/gdk.h>
#include "gtkruler.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define GTK_VRULER(obj)          G_TYPE_CHECK_INSTANCE_CAST (obj, gtk_vruler_get_type (), GtkVRuler)
#define GTK_VRULER_CLASS(klass)  G_TYPE_CHECK_CLASS_CAST (klass, gtk_vruler_get_type (), GtkVRulerClass)
#define GTK_IS_VRULER(obj)       G_TYPE_CHECK_INSTANCE_TYPE (obj, gtk_vruler_get_type ())


typedef struct _GtkVRuler       GtkVRuler;
typedef struct _GtkVRulerClass  GtkVRulerClass;

struct _GtkVRuler
{
  GtkRuler ruler;
};

struct _GtkVRulerClass
{
  GtkRulerClass parent_class;
};


GType      gtk_vruler_get_type (void);
GtkWidget* gtk_vruler_new      (void);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_VRULER_H__ */
