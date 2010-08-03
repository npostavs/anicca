#include <stdlib.h>

#include "anicca.h"
#include "error.h"
#include "char.h"
#include "function.h"
#include "conjunction.h"

DYAD(amper) {
    V *v; A z;
    I xt = AT(x), yt = AT(y);

    if ((xt&VERB && yt&NOUN)||(xt&NOUN && yt&VERB)) {
        v = VAV(xt&VERB ? x : y);
        z = CDERV(CAMPR, bond, dbond, x, y, VLR(v), VMR(v), VRR(v));
    }
    else if (xt&VERB && yt&&VERB) {
        v = VAV(y);
        z = CDERV(CAMPR, compose, dcompose, x, y, VLR(v), VMR(v), VRR(v));
    }
    return z;
}

DMONAD(bond) { DECL_FG;
    z = AT(f)&NOUN ? df2(f, y, g) : df2(y, g, f);
    return z;
}

DDYAD(dbond) { DECL_FG;
    return z;
}

DMONAD(compose) { DECL_FG;
    z = df1(df1(y, g), f);
    return z;
}

DDYAD(dcompose) { DECL_FG;
    z = df2(df1(x, g), df1(y, g), f);
    return z;
}

DYAD(at) {
    V *v; A z;
    if (AT(x)&VERB && AT(y)&VERB) {
        v = VAV(y);
        z = CDERV(CAT, atop, datop, x, y, VLR(v), VMR(v), VRR(v));
    }
    return z;
}

DMONAD(atop) {
    A z;
    z = compose(y, self);
    return z;
}

DDYAD(datop) { DECL_FG;
    z = df1(df2(x, y, g), f);
    return z;
}

DMONAD(chook) { DECL_FG;
    z = df2(y, df1(y, g), f);
    return z;
}

DDYAD(dchook) { DECL_FG;
    z = df2(x, df1(y, g), f);
    return z;
}

DMONAD(cfork) { DECL_FGH;
    z = (AT(f)&VERB && VID(VAV(f))==CCAP) ? df1(df1(y, h), g) :
        AT(f)&NOUN ? df2(f, df1(y, h), g) :
        df2(df1(y, f), df1(y, h), g);
    return z;
}

DDYAD(dcfork) { DECL_FGH;
    z = (AT(f)&VERB && VID(VAV(f))==CCAP) ? df1(df2(x, y, h), g) :
        AT(f)&NOUN ? df2(f, df2(x, y, h), g) :
        df2(df2(x, y, f), df2(x, y, h), g);
    return z;
}

DYAD(powr) {
    V *v; A z; I xt = AT(x), yt = AT(y);
    ASSERT(xt&VERB && yt&INT, ERDOM);
    v = VAV(xt&VERB ? x : y);
    z = CDERV(CPOWR, cpower, cdpower, x, y, VLR(v), VMR(v), VRR(v));
    return z;
}

DMONAD(cpower) { DECL_FG;
    I n = *IAV(g); A temp;
    DO(n, temp = y; y = df1(temp, f); a_free(temp));
    z = y;
    return z;
}

DDYAD(cdpower) { DECL_FG;
    I n = *IAV(g); A temp;
    DO(n, temp = y; y = df2(x, temp, f); a_free(temp));
    z = y;
    return z;
}
