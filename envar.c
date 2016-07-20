/** envar.c
 * Parse a multivalue environment variable (cookies for example)
 *
 * $Author: mxe20 $
 * $Date: 2005/05/11 15:27:02 $
 * $Header: /home/mxe20/src/clog/RCS/envar.c,v 1.1 2005/05/11 15:27:02 mxe20 Exp mxe20 $
 * $Id: envar.c,v 1.1 2005/05/11 15:27:02 mxe20 Exp mxe20 $
 * $Locker: mxe20 $
 * $Log: envar.c,v $
 * Revision 1.1  2005/05/11 15:27:02  mxe20
 * Initial revision
 *
 * Revision 1.2  2005/01/10 18:30:13  mxe20
 * Added RCS macros
 *
 * $Name:  $
 * $RCSfile: envar.c,v $
 * $Revision: 1.1 $
 * $Source: /home/mxe20/src/clog/RCS/envar.c,v $
 * $State: Exp $
 */
#include "envar.h"

/** parseEnVar
 * Input:
 *	char *env -> the environment variable to parse
 * Output:
 *	int *n the number of key/val pairs parsed
 *	Envar * -> structure containing key/val pairs
 */ 
Envar *parseEnVar(char *env, int *n)
{
        Envar *c = NULL;
        int nc = 0;
        char *cookiestr = NULL;

        if (!env)
        {
                *n = 0;
                return NULL;
        }
        /* add ; to end of string for easier parsing */
        cookiestr = malloc(strlen(env) + 2);
        sprintf(cookiestr, "%s;", env);

        env = cookiestr;
        while (*env)
        {
                char *next = strstr(env, ";");
                if (next)
                {
                        /* Get this token */
                        c = realloc(c, sizeof(Envar)*(nc + 1));
                        c[nc].key = copyKey(env);
                        c[nc].val = copyVal(env);
                        nc++;

                        /* find next token */
                        env = next + 1;
                        while (*env && isspace(*env))
                                env++;
                }
        }

        /* Clean up crumbs! */
        free(cookiestr);
        *n = nc;
        return c;
}

/** copyKey
 * copy the key
 */
char *copyKey(char *pair)
{
        char *p = strchr(pair, '=');
        char *ret = NULL;
        if (p)
        {
                int len = p - pair;
                ret = malloc(len + 1);
                memcpy(ret, pair, len);
                ret[len] = '\0';
        }

        return ret;
}

/* copyVal
 * copy the value
 */
char *copyVal(char *pair)
{
        char *ret = NULL;
        int len;
        pair = strchr(pair, '=');
        if (pair)
        {
                pair++;
                len = strchr(pair, ';') - pair;
                ret = malloc(len + 1);
                strncpy(ret, pair, len);
                ret[len] = '\0';
        }

        return ret;
}

