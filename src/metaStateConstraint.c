/*
** LCLint - annotation-assisted static program checker
** Copyright (C) 1994-2001 University of Virginia,
**         Massachusetts Institute of Technology
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2 of the License, or (at your
** option) any later version.
** 
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** General Public License for more details.
** 
** The GNU General Public License is available from http://www.gnu.org/ or
** the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
** MA 02111-1307, USA.
**
** For information on lclint: lclint-request@cs.virginia.edu
** To report a bug: lclint-bug@cs.virginia.edu
** For more information: http://lclint.cs.virginia.edu
*/
/*
** metaStateConstraint.c
*/

# include "lclintMacros.nf"
# include "basic.h"
# include "mtincludes.h"

metaStateConstraint 
metaStateConstraint_create (/*@only@*/ metaStateSpecifier lspec, /*@only@*/ metaStateExpression rspec)
{
  metaStateConstraint res = (metaStateConstraint) dmalloc (sizeof (*res));
  res->lspec = lspec;
  res->rspec = rspec;
  return res;
}

cstring metaStateConstraint_unparse (metaStateConstraint m) 
{
  return message ("%q = %q", 
		  metaStateSpecifier_unparse (m->lspec),
		  metaStateExpression_unparse (m->rspec));
}

void metaStateConstraint_free (/*@only@*/ metaStateConstraint m) 
{
  metaStateSpecifier_free (m->lspec);
  metaStateExpression_free (m->rspec);
  sfree (m);
}

