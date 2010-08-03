#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "anicca.h"
#include "error.h"
#include "memory.h"
#include "char.h"
#include "table.h"
#include "noun.h"
#include "verb.h"
#include "adverb.h"
#include "conjunction.h"
#include "primitive.h"
#include "lexer.h"

/*
  parse_literal
  input: Length of string, Pointer to string.
  output: Array of type string with length (n-2).
*/
A parse_literal(I n, C *s) {
    A z = gen_array(CHAR, 1, n-=2, NULL);  C *v = CAV(z);
    if (n > 0) { s++; strncpy(v, s, n); }
    return z;
}

/*
  noun_start
  input:  Length of noun, String of noun.
  output: Array of size 2n, in the form:
    [start index token 1, length token 1, start index token 2,
    length token 2, ..., start index token n, length token n].
*/
A noun_index(I n, C *s) {
    C e, t, st = SS;
    I i, m = 1+n, j = 0, k = 0, *v;
    ST pr;
    A z = gen_array(INT, 1, m, NULL);
    v = IAV(z);

    DO(n,
       t = nountype[s[i]];
       pr = noun[st][t];
       e = pr.effect;
       st = pr.new;

       switch (e) {
       case EO: break;
       case EN: j = i; break;
       case EW: v[k++] = j; v[k++] = i-j; break;
       case ES: goto end_noun; break;
       }
    );
  end_noun:    
    resize_array(z, INT, k);
    AN(z) = k;
    return z;
}

/*
  token_index
  input:  Boxed string to be lexed.
  output: Array of size 2n (n = number of tokens), in the form:
    [start index token 1, length token 1, start index token 2,
    length token 2, ..., start index token n, length token n].
*/
MONAD(token_index) {
    C e, sn, t, s = SS, vec = 0, *str = CAV(y);
    I i, jv, j = 0, k = 0, n = AN(y), *v;
    ST pr;
    A z = gen_array(INT, 1, n+n, NULL);
    v = IAV(z);

    DO(n,
       t = chartype[str[i]];
       pr = dfa[s][t];
       e = pr.effect;
       sn = pr.new;

       switch (e) {
       case EO: break;
       case EN: { j = i; break; }
       case EW: { v[k++] = j; v[k++] = i-j; j = i; break; }
       case EY: { v[k++] = j; v[k++] = i-j; j = -1; break; }
       case EV: {
           if (!vec) { v[k++] = j; v[k] = i-j; jv = j; }
           else      { v[k] = i-jv; }
           j = i;
           vec = 1;
           break;
       }
       case EZ: {
           if (!vec) { v[k++] = j; v[k] = i-j; jv = j; }
           else      { v[k] = i-jv; }
           j = -1;
           vec = 1;
           break;
       }
       case ES: goto end; break;
       }

       if (vec && sn != S9 && sn != SS) { vec = 0; k++; }
       s = sn;
    );
  end:
    resize_array(z, INT, k);
    AN(z) = k;
    println(z);
    return z;
}

/*
  tokens
  input:
    x: Output of token_index.
    y: Boxed string to be tokenized.
  output: Array of boxed tokens.
*/
DYAD(tokens) {
    C c, vn, *s, *str = CAV(y);
    I j, ws, wl, t, n = AN(x)/2, *indx = IAV(x);
    A v, z = gen_array(BOX, 1, n+5, NULL), *av = AAV(z);
    *av++ = mark;

    DO(n,
       j = i+i;
       ws = indx[j];
       wl = indx[j+1];
       s = &str[ws];
       c = *s;
       t = chartype[c];
       vn = verb_name(wl, s);
       v = primitive_lookup(vn);

       if (AT(v)&MARK) {
           switch (t) {
           case CS:
           case C9: {
               *av++ = parse_noun(wl, s);
               break;
           }
           case CQ: {
               *av++ = parse_literal(wl, s);
               break;
           }
           default: break; /* error */
           }
       }
       else { *av++ = v; }
    );

    DO(4, *av++ = mark);
    return z;
}
