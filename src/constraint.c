/*
** constraintList.c
*/

//#define DEBUGPRINT 1

# include <ctype.h> /* for isdigit */
# include "lclintMacros.nf"
# include "basic.h"
# include "cgrammar.h"
# include "cgrammar_tokens.h"

# include "exprChecks.h"
# include "aliasChecks.h"
# include "exprNodeSList.h"
//# include "exprData.i"

/*@i33*/
/*@-fcnuse*/
/*@-assignexpose*/

/*@access exprNode @*/

constraint makeConstraintParse (sRef x, lltok relOp, exprNode cconstant)
     
{
  char *t;
  int c;
  constraint ret;
  ret = constraint_makeNew();
  llassert (sRef_isValid(x) );
  if (!sRef_isValid(x))
    return ret;
 
    
  ret->lexpr = constraintExpr_makeTermsRef (x);
  #warning fix abstraction

  if (relOp.tok == GE_OP)
      ret->ar = GTE;
  else if (relOp.tok == LE_OP)
    ret->ar = LTE;
  else if (relOp.tok == EQ_OP)
    ret->ar = EQ;
  else
    llfatalbug(message ("Unsupported relational operator") );


  t =  cstring_toCharsSafe (exprNode_unparse(cconstant));
  c = atoi( t );
  ret->expr = constraintExpr_makeIntLiteral (c);

  ret->post = TRUE;
  //  ret->orig = ret;
  DPRINTF(("GENERATED CONSTRAINT:"));
  DPRINTF( (message ("%s", constraint_print(ret) ) ) );
  return ret;
}

constraint makeConstraintParse2 (constraintExpr l, lltok relOp, exprNode cconstant)
     
{
  char *t;
  int c;
  constraint ret;
  ret = constraint_makeNew();
  llassert (l!=NULL);
  if (!l)
    return ret;
 
    
  ret->lexpr = constraintExpr_copy (l);
  #warning fix abstraction

  if (relOp.tok == GE_OP)
      ret->ar = GTE;
  else if (relOp.tok == LE_OP)
    ret->ar = LTE;
  else if (relOp.tok == EQ_OP)
    ret->ar = EQ;
  else
  llfatalbug(message("Unsupported relational operator") );


  t =  cstring_toCharsSafe (exprNode_unparse(cconstant));
  c = atoi( t );
  ret->expr = constraintExpr_makeIntLiteral (c);

  ret->post = TRUE;
  //  ret->orig = ret;
  DPRINTF(("GENERATED CONSTRAINT:"));
  DPRINTF( (message ("%s", constraint_print(ret) ) ) );
  return ret;
}

bool constraint_same (constraint c1, constraint c2)
{
  
  if (c1->ar != c2->ar)
    return FALSE;

  if (!constraintExpr_similar (c1->lexpr, c2->lexpr) )
    return FALSE;

  if (!constraintExpr_similar (c1->expr, c2->expr) )
    return FALSE;

  return TRUE;
}

constraint makeConstraintParse3 (constraintExpr l, lltok relOp, constraintExpr r)     
{
  constraint ret;
  ret = constraint_makeNew();
  llassert (l !=NULL);
  if (!l)
    return ret;
 
    
  ret->lexpr = constraintExpr_copy (l);
  #warning fix abstraction

  if (relOp.tok == GE_OP)
      ret->ar = GTE;
  else if (relOp.tok == LE_OP)
    ret->ar = LTE;
  else if (relOp.tok == EQ_OP)
    ret->ar = EQ;
  else
  llfatalbug( message("Unsupported relational operator") );

  ret->expr = constraintExpr_copy (r);

  ret->post = TRUE;

  ret->orig = constraint_copy(ret);

  ret = constraint_simplify (ret);
  
  //  ret->orig = ret;
  DPRINTF(("GENERATED CONSTRAINT:"));
  DPRINTF( (message ("%s", constraint_print(ret) ) ) );
  return ret;
}

constraint constraint_copy (constraint c)
{
  constraint ret;

  llassert (constraint_isDefined(c) );
  
  ret = constraint_makeNew();
  ret->lexpr = constraintExpr_copy (c->lexpr);
  ret->ar = c->ar;
  ret->expr =  constraintExpr_copy (c->expr);
  ret->post = c->post;
  ret->generatingExpr = exprNode_fakeCopy(c->generatingExpr);
  
  /*@i33 fix this*/
  if (c->orig != NULL)
    ret->orig = constraint_copy (c->orig);
  else
    ret->orig = NULL;

  if (c->or != NULL)
    ret->or = constraint_copy (c->or);
  else
    ret->or = NULL;
  
  return ret;
}

/*like copy expect it doesn't allocate memory for the constraint*/

void constraint_overWrite (constraint c1, constraint c2)
{
  c1->lexpr = constraintExpr_copy (c2->lexpr);
  c1->ar = c2->ar;
  c1->expr =  constraintExpr_copy (c2->expr);
  c1->post = c2->post;
  /*@i33 fix this*/
  if (c2->orig != NULL)
    c1->orig = constraint_copy (c2->orig);
  else
    c1->orig = NULL;

  if (c2->or != NULL)
    c1->or = constraint_copy (c2->or);
  else
    c1->or = NULL;
  
  c1->generatingExpr = exprNode_fakeCopy (c2->generatingExpr );
}

bool constraint_resolve (/*@unused@*/ constraint c)
{
  return FALSE;
}



/*@notnull@*/ constraint constraint_makeNew (void)
{
  constraint ret;
  ret = dmalloc(sizeof (*ret) );
  ret->lexpr = NULL;
  ret->expr = NULL;
  ret->ar = LT;
  ret->post = FALSE;
  ret->orig = NULL;
  ret->or = NULL;
  ret->generatingExpr = NULL;
  return ret;
}

constraint constraint_addGeneratingExpr (/*@returned@*/ constraint c, exprNode e)
{
    
  if (c->generatingExpr == NULL)
    {
      c->generatingExpr = exprNode_fakeCopy(e);
      DPRINTF ((message ("setting generatingExpr for %s to %s", constraint_print(c), exprNode_unparse(e) )  ));
    }
  else
    {
      DPRINTF ((message ("Not setting generatingExpr for %s to %s", constraint_print(c), exprNode_unparse(e) )  ));
    }
  return c;
}

fileloc constraint_getFileloc (constraint c)
{
  if (exprNode_isDefined(c->generatingExpr) )
    return (exprNode_getfileloc (c->generatingExpr) );
	    
  return (constraintExpr_getFileloc (c->lexpr) );


}

static bool checkForMaxSet (constraint c)
{
  if (constraintExpr_hasMaxSet(c->lexpr) || constraintExpr_hasMaxSet(c->expr) )
    return TRUE;

  return FALSE;
}

bool constraint_hasMaxSet(constraint c)
{
  if (c->orig != NULL)
    {
      if (checkForMaxSet(c->orig) )
	return TRUE;
    }

  return (checkForMaxSet(c) );
}

constraint constraint_makeReadSafeExprNode ( exprNode po, exprNode ind)
{
  constraint ret = constraint_makeNew();
  //  constraintTerm term;
  po = exprNode_fakeCopy(po);
  ind = exprNode_fakeCopy(ind);
  ret->lexpr = constraintExpr_makeMaxReadExpr(po);
  ret->ar    = GTE;
  ret->expr  = constraintExpr_makeValueExpr (ind);
  return ret;
}

constraint constraint_makeWriteSafeInt (exprNode po, int ind)
{
  constraint ret = constraint_makeNew();

 
  ret->lexpr =constraintExpr_makeMaxSetExpr(po);
  ret->ar = GTE;
  ret->expr =  constraintExpr_makeIntLiteral (ind);
  /*@i1*/return ret;
}

constraint constraint_makeSRefSetBufferSize (sRef s, int size)
{
 constraint ret = constraint_makeNew();
 ret->lexpr = constraintExpr_makeSRefMaxset (s);
 ret->ar = EQ;
 ret->expr =  constraintExpr_makeIntLiteral (size);
 ret->post = TRUE;
 /*@i1*/return ret;
}

constraint constraint_makeSRefWriteSafeInt (sRef s, int ind)
{
  constraint ret = constraint_makeNew();

 
  ret->lexpr = constraintExpr_makeSRefMaxset (s);
  ret->ar = GTE;
  ret->expr =  constraintExpr_makeIntLiteral (ind);
  ret->post = TRUE;
  /*@i1*/return ret;
}

/* drl added 01/12/2000
   
  makes the constraint: Ensures index <= MaxRead(buffer) */

constraint constraint_makeEnsureLteMaxRead(exprNode index, exprNode buffer)
{
  constraint ret = constraint_makeNew();

  ret->lexpr = constraintExpr_makeValueExpr (index);
  ret->ar = LTE;
  ret->expr = constraintExpr_makeMaxReadExpr(buffer);
  ret->post = TRUE;
  return ret;
}

constraint constraint_makeWriteSafeExprNode (exprNode po, exprNode ind)
{
  constraint ret = constraint_makeNew();

 
  ret->lexpr =constraintExpr_makeMaxSetExpr(po);
  ret->ar = GTE;
  ret->expr =  constraintExpr_makeValueExpr (ind);
  /*@i1*/return ret;
}


constraint constraint_makeReadSafeInt ( exprNode po, int ind)
{
  constraint ret = constraint_makeNew();

  po = exprNode_fakeCopy(po);
  
  ret->lexpr = constraintExpr_makeMaxReadExpr(po);
  ret->ar    = GTE;
  ret->expr  = constraintExpr_makeIntLiteral (ind);
  return ret;
}

constraint constraint_makeSRefReadSafeInt (sRef s, int ind)
{
  constraint ret = constraint_makeNew();

 
  ret->lexpr = constraintExpr_makeSRefMaxRead (s);
  ret->ar = GTE;
  ret->expr =  constraintExpr_makeIntLiteral (ind);
  ret->post = TRUE;
  /*@i1*/return ret;
}

constraint constraint_makeEnsureMaxReadAtLeast (exprNode e1, exprNode t2, fileloc sequencePoint)
{
  constraint ret = constraint_makeNew();


  e1 = exprNode_fakeCopy (e1);
  t2 = exprNode_fakeCopy (t2);
  
  ret = constraint_makeReadSafeExprNode(e1, t2);

  ret->lexpr = constraintExpr_setFileloc (ret->lexpr, sequencePoint);
  
  ret->post = TRUE;  

  //  fileloc_incColumn (ret->lexpr->term->loc);
  return ret;
}

static constraint constraint_makeEnsuresOpConstraintExpr (constraintExpr c1, constraintExpr c2, fileloc sequencePoint,  arithType  ar)
{

  constraint ret;
  
  llassert(c1 && c2);
  //  llassert(sequencePoint);

  ret = constraint_makeNew();
  
  ret->lexpr = c1;
  ret->ar = ar;
  ret->post = TRUE;
  ret->expr =  c2;
  ret->lexpr = constraintExpr_setFileloc (ret->lexpr, sequencePoint);
  return ret;
}

static constraint constraint_makeEnsuresOp (exprNode e1, exprNode e2, fileloc sequencePoint,  arithType  ar)
{
  constraintExpr c1, c2;
  constraint ret;
  exprNode e;

  if (! (e1 && e2) )
    {
      llcontbug((message("null exprNode, Exprnodes are %s and %s",
		       exprNode_unparse(e1), exprNode_unparse(e2) )
	       ));
    }

  //  llassert (sequencePoint);
  
  e  =  exprNode_fakeCopy(e1);
  c1 =  constraintExpr_makeValueExpr (e);
  
  e  =  exprNode_fakeCopy(e2);
  c2 =  constraintExpr_makeValueExpr (e);

  ret = constraint_makeEnsuresOpConstraintExpr (c1, c2, sequencePoint, ar);
  
  return ret;
}


/* make constraint ensures e1 == e2 */

constraint constraint_makeEnsureEqual (exprNode e1, exprNode e2, fileloc sequencePoint)
{
  return ( constraint_makeEnsuresOp (e1, e2, sequencePoint, EQ) );
}

/*make constraint ensures e1 < e2 */
constraint constraint_makeEnsureLessThan (exprNode e1, exprNode e2, fileloc sequencePoint)
{
  constraintExpr t1, t2;

  t1 = constraintExpr_makeValueExpr (e1);
  t2 = constraintExpr_makeValueExpr (e2);

  /*change this to e1 <= (e2 -1) */

  t2 = constraintExpr_makeDecConstraintExpr (t2);
  
 return ( constraint_makeEnsuresOpConstraintExpr (t1, t2, sequencePoint, LTE) );
}

constraint constraint_makeEnsureLessThanEqual (exprNode e1, exprNode e2, fileloc sequencePoint)
{
 return ( constraint_makeEnsuresOp (e1, e2, sequencePoint, LTE) );
}

constraint constraint_makeEnsureGreaterThan (exprNode e1, exprNode e2, fileloc sequencePoint)
{
  constraintExpr t1, t2;

  t1 = constraintExpr_makeValueExpr (e1);
  t2 = constraintExpr_makeValueExpr (e2);


  /* change this to e1 >= (e2 + 1) */
  t2 = constraintExpr_makeIncConstraintExpr (t2);
  
  
 return ( constraint_makeEnsuresOpConstraintExpr (t1, t2, sequencePoint, GTE) );
}

constraint constraint_makeEnsureGreaterThanEqual (exprNode e1, exprNode e2, fileloc sequencePoint)
{
 return ( constraint_makeEnsuresOp (e1, e2, sequencePoint, GTE) );
}


exprNode exprNode_copyConstraints (/*@returned@*/ exprNode dst, exprNode src)
{
  dst->ensuresConstraints = constraintList_copy (src->ensuresConstraints );
  dst->requiresConstraints = constraintList_copy (src->requiresConstraints );
  dst->trueEnsuresConstraints = constraintList_copy (src->trueEnsuresConstraints );
  dst->falseEnsuresConstraints = constraintList_copy (src->falseEnsuresConstraints );
  return dst;
}

constraint constraint_makeMaxSetSideEffectPostDecrement (exprNode e, fileloc sequencePoint)
{
  constraint ret = constraint_makeNew();
  //constraintTerm term;

  e = exprNode_fakeCopy(e);
  ret->lexpr = constraintExpr_makeValueExpr (e);
  ret->ar = EQ;
  ret->post = TRUE;
  ret->expr =  constraintExpr_makeValueExpr (e);
  ret->expr =  constraintExpr_makeDecConstraintExpr (ret->expr);

  ret->lexpr = constraintExpr_setFileloc (ret->lexpr, sequencePoint);
//   fileloc_incColumn (  ret->lexpr->term->loc);
//   fileloc_incColumn (  ret->lexpr->term->loc);
  return ret;
}
constraint constraint_makeMaxSetSideEffectPostIncrement (exprNode e, fileloc sequencePoint)
{
  constraint ret = constraint_makeNew();
  //constraintTerm term;

  e = exprNode_fakeCopy(e);
  ret->lexpr = constraintExpr_makeValueExpr (e);
  ret->ar = EQ;
  ret->post = TRUE;
  ret->expr =  constraintExpr_makeValueExpr (e);
  ret->expr =  constraintExpr_makeIncConstraintExpr (ret->expr);

  ret->lexpr = constraintExpr_setFileloc (ret->lexpr, sequencePoint);
//   fileloc_incColumn (  ret->lexpr->term->loc);
//   fileloc_incColumn (  ret->lexpr->term->loc);
  return ret;
}



// constraint constraint_makeMaxReadSideEffectPostIncrement (exprNode e, fileloc sequencePoint)
// {
//   constraint ret = constraint_makeNew();
//   //constraintTerm term;

//   e = exprNode_fakeCopy(e);
//   ret->lexpr = constraintExpr_makeMaxReadExpr(e);
//   ret->ar = EQ;
//   ret->post = TRUE;
//   ret->expr = constraintExpr_makeIncConstraintExpr (e);
//   ret->lexpr = constraintExpr_setFileLoc (ret->lexpr, sequencePoint);
//   return ret;
// }


cstring arithType_print (arithType ar) /*@*/
{
  cstring st = cstring_undefined;
  switch (ar)
    {
    case LT:
      st = cstring_makeLiteral (" < ");
      break;
    case	LTE:
      st = cstring_makeLiteral (" <= ");
      break;
    case 	GT:
      st = cstring_makeLiteral (" > ");
      break;
    case 	GTE:
      st = cstring_makeLiteral (" >= ");
      break;
    case	EQ:
      st = cstring_makeLiteral (" == ");
      break;
    case	NONNEGATIVE:
      st = cstring_makeLiteral (" NONNEGATIVE ");
      break;
    case	POSITIVE:
      st = cstring_makeLiteral (" POSITIVE ");
      break;
    default:
      llassert(FALSE);
      break;
    }
  return st;
}

void constraint_printError (constraint c, fileloc loc)
{
  cstring string;
  fileloc errorLoc;
  
  string = constraint_printDetailed (c);

  errorLoc = loc;

  if (constraint_getFileloc(c) )
    /*@-branchstate@*/
      errorLoc = constraint_getFileloc(c);
  /*@=branchstate@*/
  
  if (c->post)
    {
       voptgenerror (FLG_FUNCTIONPOST, string, errorLoc);
    }
  else
    {
      voptgenerror (FLG_FUNCTIONCONSTRAINT, string, errorLoc);
    }
      
}

cstring  constraint_printDetailed (constraint c)
{
  cstring st = cstring_undefined;


  if (!c->post)
    {
    if (c->orig != NULL)  
      st = message ("Unresolved constraint:\nLclint is unable to resolve %s needed to satisfy %s", constraint_print (c), constraint_print(c->orig) );
    else
      st = message ("Unresolved constraint:\nLclint is unable to resolve %s", constraint_print (c));
      
    }
  else
    {
      if (c->orig != NULL)
	st = message ("Block Post condition:\nThis function block has the post condition %s\n based on %s", constraint_print (c), constraint_print(c->orig) );
      else
	st = message ("Block Post condition:\nThis function block has the post condition %s", constraint_print (c));	
    }

  if (context_getFlag (FLG_CONSTRAINTLOCATION) )
    {
      cstring temp;
      // llassert (c->generatingExpr);
      temp = message ("\nOriginal Generating expression %s: %s\n", fileloc_unparse( exprNode_getfileloc (c->generatingExpr) ),
		      exprNode_unparse(c->generatingExpr) );
      st = cstring_concat (st, temp);

      if (constraint_hasMaxSet(c) )
	{
	  cstring temp2;
	  temp2 = message ("\nHas MaxSet\n");
	  st = cstring_concat (st, temp2);
	}
    }
  return st;
}

cstring  constraint_print (constraint c) /*@*/
{
  cstring st = cstring_undefined;
  cstring type = cstring_undefined;
  llassert (c !=NULL);
  if (c->post)
    {
      type = cstring_makeLiteral ("Ensures: ");
    }
  else
    {
      type = cstring_makeLiteral ("Requires: ");
    }
  st = message ("%s: %s %s %s",
		type,
		constraintExpr_print (c->lexpr),
		arithType_print(c->ar),
		constraintExpr_print(c->expr)
		);
  return st;
}

cstring  constraint_printOr (constraint c) /*@*/
{
  cstring ret;
  constraint temp;

  ret = cstring_undefined;
  temp = c;

  ret = cstring_concat (ret, constraint_print(temp) );

  temp = temp->or;
  
  while ( constraint_isDefined(temp) ) 
    {
      ret = cstring_concat (ret, cstring_makeLiteral (" OR ") );
      ret = cstring_concat (ret, constraint_print(temp) );
      temp = temp->or;
    }

  return ret;

}

/*@only@*/ constraint constraint_doSRefFixBaseParam (/*@returned@*/ /*@only@*/ constraint precondition,
						   exprNodeList arglist)
{
  precondition->lexpr = constraintExpr_doSRefFixBaseParam (precondition->lexpr,
							   arglist);
  precondition->expr = constraintExpr_doSRefFixBaseParam (precondition->expr,
							   arglist);

  return precondition;
}


constraint constraint_doFixResult (constraint postcondition, exprNode fcnCall)
{
  postcondition = constraint_copy (postcondition);
  postcondition->lexpr = constraintExpr_doFixResult (postcondition->lexpr, fcnCall);
  postcondition->expr = constraintExpr_doFixResult (postcondition->expr, fcnCall);

  return postcondition;
}

constraint constraint_doSRefFixConstraintParam (constraint precondition,
						   exprNodeList arglist)
{

  precondition = constraint_copy (precondition);
  precondition->lexpr = constraintExpr_doSRefFixConstraintParam (precondition->lexpr, arglist);
  precondition->expr = constraintExpr_doSRefFixConstraintParam (precondition->expr, arglist);

  return precondition;
}

// bool constraint_hasTerm (constraint c, constraintTerm term)
// {
//   DPRINTF((message ("Constraint %s", constraint_print (c) ) ) );
  
//   if (constraintExpr_includesTerm (c->lexpr, term) )
//     return TRUE;

//   if (constraintExpr_includesTerm (c->expr, term) )
//     return TRUE;

//   return FALSE;
// }

/*@only@*/ constraint constraint_preserveOrig (/*@returned@*/ /*@only@*/ constraint c) /*@modifies c @*/
{
  c->orig = constraint_copy (c);
  return c;
}
/*@=fcnuse*/
/*@=assignexpose*/
/*@=czechfcns@*/
