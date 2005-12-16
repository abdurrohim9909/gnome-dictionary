/* gdict-context.c - Abstract class for dictionary contexts
 *
 * Copyright (C) 2005  Emmanuele Bassi <ebassi@gmail.com>
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
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib/gi18n.h>

#include "gdict-context.h"
#include "gdict-enum-types.h"
#include "gdict-utils.h"
#include "gdict-marshal.h"
#include "gdict-context-private.h"

static void gdict_context_class_init (gpointer g_iface);

GType
gdict_context_get_type (void)
{
  static GType context_type = 0;
  
  if (G_UNLIKELY (context_type == 0))
    {
      static GTypeInfo context_info =
      {
        sizeof (GdictContextIface),
        NULL,                       /* base_init */
        NULL,                       /* base_finalize */
        (GClassInitFunc) gdict_context_class_init,
      };
      
      context_type = g_type_register_static (G_TYPE_INTERFACE,
      					     "GdictContext",
      					     &context_info, 0);
      g_type_interface_add_prerequisite (context_type, G_TYPE_OBJECT);
    }
  
  return context_type;
}


static void
gdict_context_class_init (gpointer g_iface)
{
  GType iface_type = G_TYPE_FROM_INTERFACE (g_iface);

  /**
   * GdictContext::lookup-start
   * @context: the object which received the signal
   *
   * This signal is emitted when a look up operation has been issued using
   * a #GdictContext.  Since every operation using a context is 
   * asynchronous, you can use this signal to know if the request has been
   * issued or not.
   *
   * Since: 1.0
   */
  g_signal_new ("lookup-start",
                iface_type,
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET (GdictContextIface, lookup_start),
                NULL, NULL,
                gdict_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
  /**
   * GdictContext::lookup-end
   * @context: the object which received the signal
   *
   * This signal is emitted when a look up operation that has been issued
   * using a #GdictContext has been completed.  Since every operation using a
   * context is asynchronous, you can use this signal to know if the request
   * has been completed or not.
   *
   * Since: 1.0
   */
  g_signal_new ("lookup-end",
                iface_type,
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET (GdictContextIface, lookup_end),
                NULL, NULL,
                gdict_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
 /**
   * GdictContext::error
   * @context: the object which received the signal
   * @error: a #GError
   *
   * This signal is emitted when an error happened during an asynchronous
   * request.
   *
   * Since: 1.0
   */
  g_signal_new ("error",
                iface_type,
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET (GdictContextIface, error),
                NULL, NULL,
                gdict_marshal_VOID__POINTER,
                G_TYPE_NONE, 1,
                G_TYPE_POINTER);
  /**
   * GdictContext::database-found
   * @context: the object which received the signal
   * @database: a #GdictDatabase
   *
   * This signal is emitted when a database request has found a database.
   *
   * Since: 1.0
   */
  g_signal_new ("database-found",
                iface_type,
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET (GdictContextIface, database_found),
                NULL, NULL,
                gdict_marshal_VOID__BOXED,
                G_TYPE_NONE, 1,
                GDICT_TYPE_DATABASE);
  /**
   * GdictContext::strategy-found
   * @context: the object which received the signal
   * @strategy: a #GdictStrategy
   *
   * This signal is emitted when a strategy request has found a strategy.
   *
   * Since: 1.0
   */
  g_signal_new ("strategy-found",
                iface_type,
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET (GdictContextIface, strategy_found),
                NULL, NULL,
                gdict_marshal_VOID__BOXED,
                G_TYPE_NONE, 1,
                GDICT_TYPE_STRATEGY);
  /**
   * GdictContext::match-found
   * @context: the object which received the signal
   * @match: a #GdictMatch
   *
   * This signal is emitted when a match request has found a match.
   *
   * Since: 1.0
   */
  g_signal_new ("match-found",
                iface_type,
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET (GdictContextIface, match_found),
                NULL, NULL,
                gdict_marshal_VOID__BOXED,
                G_TYPE_NONE, 1,
                GDICT_TYPE_MATCH);
  /**
   * GdictContext::definition-found
   * @context: the object which received the signal
   * @definition: a #GdictDefinition
   *
   * This signal is emitted when a definition request has found a definition.
   *
   * Since: 1.0
   */
  g_signal_new ("definition-found",
                iface_type,
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET (GdictContextIface, definition_found),
                NULL, NULL,
                gdict_marshal_VOID__BOXED,
                G_TYPE_NONE, 1,
                GDICT_TYPE_DEFINITION);
  
  /**
   * GdictContext:local-only
   * 
   * Whether the context uses only local dictionaries or not.
   *
   * Since: 1.0
   */
  g_object_interface_install_property (g_iface,
  				       g_param_spec_boolean ("local-only",
  				       			     _("Local Only"),
  				       			     _("Whether the context uses only local dictionaries or not"),
  				       			     FALSE,
  				       			     (G_PARAM_READABLE | G_PARAM_WRITABLE)));
}

GQuark
gdict_context_error_quark (void)
{
  static GQuark quark = 0;
  
  if (G_UNLIKELY (quark == 0))
    quark = g_quark_from_static_string ("gdict-context-error-quark");
  
  return quark;
}

/**
 * gdict_context_set_local_only:
 * @context: a #GdictContext
 * @local_only: %TRUE if only local resources will be used
 *
 * Sets whether only local resources will be used when querying for databases,
 * strategies, matches or definitions.
 *
 * Since: 1.0
 */
void
gdict_context_set_local_only (GdictContext *context,
			      gboolean      local_only)
{
  g_return_if_fail (GDICT_IS_CONTEXT (context));
  
  g_object_set (context, "local-only", &local_only, NULL);
}

/**
 * gdict_context_get_local_only:
 * @context: a #GdictContext
 *
 * Gets whether only local resources will be used when querying.
 *
 * Return value: %TRUE if only local resources will be used.
 *
 * Since: 1.0
 */
gboolean
gdict_context_get_local_only (GdictContext *context)
{
  gboolean local_only;
  
  g_return_val_if_fail (GDICT_IS_CONTEXT (context), FALSE);
  
  g_object_get (context, "local-only", &local_only, NULL);
  
  return local_only;
}

/**
 * gdict_context_lookup_databases:
 * @context: a #GdictContext
 * @error: return location for a #GError, or %NULL
 *
 * Query @context for the list of databases available.  Each time a
 * new database is found, the "database-found" signal is fired.
 *
 * Return value: %TRUE if the query was successfully started.
 *
 * Since: 1.0
 */
gboolean
gdict_context_lookup_databases (GdictContext  *context,
				GError       **error)
{
  g_return_val_if_fail (GDICT_IS_CONTEXT (context), FALSE);
  
  return GDICT_CONTEXT_GET_IFACE (context)->get_databases (context, error);
}

/**
 * gdict_context_lookup_strategies:
 * @context: a #GdictContext
 * @error: return location for a #GError, or %NULL
 *
 * Query @context for the list of matching strategies available.  Each
 * time a new strategy is found, the "strategy-found" signal is fired.
 *
 * Return value: %TRUE if the query was successfully started.
 *
 * Since: 1.0
 */
gboolean
gdict_context_lookup_strategies (GdictContext  *context,
				 GError       **error)
{
  g_return_val_if_fail (GDICT_IS_CONTEXT (context), FALSE);
  
  return GDICT_CONTEXT_GET_IFACE (context)->get_strategies (context, error);
}

/**
 * gdict_context_match_word:
 * @context: a #GdictContext
 * @database: a database name to search into, or %NULL for the
 *    default database
 * @strategy: a strategy name to use for matching, or %NULL for
 *    the default strategy
 * @word: the word to match
 * @error: return location for a #GError, or %NULL
 *
 * Query @context for a list of word matching @word inside @database,
 * using @strategy as a matching strategy.  Each time a matching word
 * is found, the "match-found" signal is fired.
 *
 * Return value: %TRUE if the query was successfully started.
 *
 * Since: 1.0
 */
gboolean
gdict_context_match_word (GdictContext  *context,
			  const gchar   *database,
			  const gchar   *strategy,
			  const gchar   *word,
			  GError       **error)
{
  g_return_val_if_fail (GDICT_IS_CONTEXT (context), FALSE);
  g_return_val_if_fail (word != NULL, FALSE);
  
  return GDICT_CONTEXT_GET_IFACE (context)->match_word (context,
  							database,
  							strategy,
  							word,
  							error);
}

/**
 * gdict_context_define_word:
 * @context: a #GdictContext
 * @database: a database name to search into, or %NULL for the
 *    default database
 * @word: the word to search
 * @error: return location for a #GError, or %NULL
 *
 * Query @context for a list of definitions of @word inside @database.  Each
 * time a new definition is found, the "definition-found" signal is fired.
 *
 * Return value: %TRUE if the query was successfully sent.
 *
 * Since: 1.0
 */
gboolean
gdict_context_define_word (GdictContext  *context,
			   const gchar   *database,
			   const gchar   *word,
			   GError       **error)
{
  g_return_val_if_fail (GDICT_IS_CONTEXT (context), FALSE);
  g_return_val_if_fail (word != NULL, FALSE);
  
  return GDICT_CONTEXT_GET_IFACE (context)->define_word (context,
  							 database,
  							 word,
  							 error);
}



/*****************
 * GdictDatabase *
 *****************/

GDICT_DEFINE_BOXED_TYPE (GdictDatabase, gdict_database);

GdictDatabase *
_gdict_database_new (const gchar *name)
{
  GdictDatabase *retval;
  
  g_return_val_if_fail (name != NULL, NULL);
  
  retval = g_new0 (GdictDatabase, 1);
  retval->name = g_strdup (name);
  retval->full_name = NULL;
  retval->ref_count = 1;
  
  return retval;
}

/**
 * gdict_database_ref:
 * @db: a #GdictDatabase
 *
 * Increases the reference count of @db by one.
 *
 * Return value: @db with its reference count increased
 *
 * Since: 1.0
 */
GdictDatabase *
gdict_database_ref (GdictDatabase *db)
{
  g_return_val_if_fail (db != NULL, NULL);
  
  g_assert (db->ref_count != 0);
  
  db->ref_count += 1;
  
  return db;
}

/**
 * gdict_database_unref:
 * @db: a #GdictDatabase
 *
 * Decreases the reference count of @db by one.  If the reference count reaches
 * zero, @db is destroyed.
 *
 * Since: 1.0
 */
void
gdict_database_unref (GdictDatabase *db)
{
  g_return_if_fail (db != NULL);
  
  g_assert (db->ref_count != 0);
  
  db->ref_count -= 1;
  if (db->ref_count == 0)
    {
      g_free (db->name);
      g_free (db->full_name);
      
      g_free (db);
    }
}

/**
 * gdict_database_get_name:
 * @db: a #GdictDatabase
 *
 * Gets the short name of the database, to be used with functions like
 * gdict_context_match_word() or gdict_context_define_word().
 *
 * Return value: the short name of the database.  The string is owned by
 *   the #GdictDatabase object, and should never be modified or freed.
 *
 * Since: 1.0
 */
G_CONST_RETURN gchar *
gdict_database_get_name (GdictDatabase *db)
{
  g_return_val_if_fail (db != NULL, NULL);
  
  return db->name;
}

/**
 * gdict_database_get_full_name:
 * @db: a #GdictDatabase
 *
 * Gets the full name of the database, suitable for display.
 *
 * Return value: the full name of the database.  The string is owned by
 *   the #GdictDatabase object, and should never be modified or freed.
 *
 * Since: 1.0
 */
G_CONST_RETURN gchar *
gdict_database_get_full_name (GdictDatabase *db)
{
  g_return_val_if_fail (db != NULL, NULL);
  
  return db->full_name;
}



/*****************
 * GdictStrategy *
 *****************/

GDICT_DEFINE_BOXED_TYPE (GdictStrategy, gdict_strategy);

GdictStrategy *
_gdict_strategy_new (const gchar *name)
{
  GdictStrategy *strat;
  
  g_return_val_if_fail (name != NULL, NULL);
  
  strat = g_new0 (GdictStrategy, 1);
  strat->name = g_strdup (name);
  strat->description = NULL;
  strat->ref_count = 1;
  
  return strat;
}

/**
 * gdict_strategy_ref:
 * @strat: a #GdictStrategy
 *
 * Increases the reference count of @strat by one.
 *
 * Return value: @strat with its reference count increased
 *
 * Since: 1.0
 */
GdictStrategy *
gdict_strategy_ref (GdictStrategy   *strat)
{
  g_return_val_if_fail (strat != NULL, NULL);
  
  g_assert (strat->ref_count != 0);
  
  strat->ref_count += 1;
  
  return strat;
}

void
gdict_strategy_unref (GdictStrategy *strat)
{
  g_return_if_fail (strat != NULL);
  
  g_assert (strat->ref_count != 0);
  
  strat->ref_count -= 1;
  if (strat->ref_count == 0)
    {
      g_free (strat->name);
      g_free (strat->description);
      
      g_free (strat);
    }
}

G_CONST_RETURN gchar *
gdict_strategy_get_name (GdictStrategy *strat)
{
  g_return_val_if_fail (strat != NULL, NULL);
  
  return strat->name;
}

G_CONST_RETURN gchar *
gdict_strategy_get_description (GdictStrategy *strat)
{
  g_return_val_if_fail (strat != NULL, NULL);
  
  return strat->description;
}



/**************
 * GdictMatch *
 **************/

GDICT_DEFINE_BOXED_TYPE (GdictMatch, gdict_match);

GdictMatch *
_gdict_match_new (const gchar *word)
{
  GdictMatch *match;
  
  g_return_val_if_fail (word != NULL, NULL);
  
  match = g_new0 (GdictMatch, 1);
  match->word = g_strdup (word);
  match->database = NULL;
  match->ref_count = 1;
  
  return match;
}

GdictMatch *
gdict_match_ref (GdictMatch *match)
{
  g_return_val_if_fail (match != NULL, NULL);

  g_assert (match->ref_count != 0);
  
  match->ref_count += 1;
  
  return match;
}

void
gdict_match_unref (GdictMatch *match)
{
  g_return_if_fail (match != NULL);
  
  g_assert (match->ref_count != 0);
  
  match->ref_count -= 1;
  
  if (match->ref_count == 0)
    {
      g_free (match->word);
      g_free (match->database);
      
      g_free (match);
    }
}

G_CONST_RETURN gchar *
gdict_match_get_word (GdictMatch *match)
{
  g_return_val_if_fail (match != NULL, NULL);
  
  return match->word;
}

G_CONST_RETURN gchar *
gdict_match_get_database (GdictMatch *match)
{
  g_return_val_if_fail (match != NULL, NULL);
  
  return match->database;
}



/*******************
 * GdictDefinition *
 *******************/

GDICT_DEFINE_BOXED_TYPE (GdictDefinition, gdict_definition);

GdictDefinition *
_gdict_definition_new (gint total)
{
  GdictDefinition *def;
  
  def = g_new0 (GdictDefinition, 1);
  
  def->total = total;
  def->word = NULL;
  def->database_name = NULL;
  def->database_full = NULL;
  def->ref_count = 1;
  
  return def;
}

GdictDefinition *
gdict_definition_ref (GdictDefinition *def)
{
  g_return_val_if_fail (def != NULL, NULL);
  
  g_assert (def->ref_count != 0);
  
  def->ref_count += 1;
  
  return def;
}

void
gdict_definition_unref (GdictDefinition *def)
{
  g_return_if_fail (def != NULL);
  
  g_assert (def->ref_count != 0);
  
  def->ref_count -= 1;
  if (def->ref_count == 0)
    {
      g_free (def->word);
      g_free (def->database_name);
      g_free (def->database_full);
      
      g_free (def);
    }
}

gint
gdict_definition_get_total (GdictDefinition *def)
{
  g_return_val_if_fail (def != NULL, -1);

  return def->total;
}

G_CONST_RETURN gchar *
gdict_definition_get_word (GdictDefinition *def)
{
  g_return_val_if_fail (def != NULL, NULL);
  
  return def->word;
}

G_CONST_RETURN gchar *
gdict_definition_get_database (GdictDefinition *def)
{
  g_return_val_if_fail (def != NULL, NULL);
  
  return def->database_full;
}

G_CONST_RETURN gchar *
gdict_definition_get_text (GdictDefinition *def)
{
  g_return_val_if_fail (def != NULL, NULL);
  
  return def->definition;
}
