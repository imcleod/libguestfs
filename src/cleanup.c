/* libguestfs
 * Copyright (C) 2013 Red Hat Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#ifdef HAVE_LIBXML2
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xmlwriter.h>
#endif

#include "hash.h"

#include "guestfs.h"
#include "guestfs-internal-frontend.h"

/* These functions are used internally by the CLEANUP_* macros.
 * Don't call them directly.
 */

void
guestfs___cleanup_free (void *ptr)
{
  free (* (void **) ptr);
}

void
guestfs___cleanup_free_string_list (void *ptr)
{
  guestfs___free_string_list (* (char ***) ptr);
}

void
guestfs___cleanup_hash_free (void *ptr)
{
  Hash_table *h = * (Hash_table **) ptr;

  if (h)
    hash_free (h);
}

void
guestfs___cleanup_unlink_free (void *ptr)
{
  char *filename = * (char **) ptr;

  if (filename) {
    unlink (filename);
    free (filename);
  }
}

void
guestfs___cleanup_xmlBufferFree (void *ptr)
{
#ifdef HAVE_LIBXML2
  xmlBufferPtr xb = * (xmlBufferPtr *) ptr;

  if (xb)
    xmlBufferFree (xb);
#else
  abort ();
#endif
}

void
guestfs___cleanup_xmlFreeDoc (void *ptr)
{
#ifdef HAVE_LIBXML2
  xmlDocPtr doc = * (xmlDocPtr *) ptr;

  if (doc)
    xmlFreeDoc (doc);
#else
  abort ();
#endif
}

void
guestfs___cleanup_xmlFreeTextWriter (void *ptr)
{
#ifdef HAVE_LIBXML2
  xmlTextWriterPtr xo = * (xmlTextWriterPtr *) ptr;

  if (xo)
    xmlFreeTextWriter (xo);
#else
  abort ();
#endif
}

void
guestfs___cleanup_xmlXPathFreeContext (void *ptr)
{
#ifdef HAVE_LIBXML2
  xmlXPathContextPtr ctx = * (xmlXPathContextPtr *) ptr;

  if (ctx)
    xmlXPathFreeContext (ctx);
#else
  abort ();
#endif
}

void
guestfs___cleanup_xmlXPathFreeObject (void *ptr)
{
#ifdef HAVE_LIBXML2
  xmlXPathObjectPtr obj = * (xmlXPathObjectPtr *) ptr;

  if (obj)
    xmlXPathFreeObject (obj);
#else
  abort ();
#endif
}
