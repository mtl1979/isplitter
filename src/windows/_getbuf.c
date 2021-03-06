#if _MSC_VER < 1900
#include <crtdbg.h>
#include <malloc.h>
#include <stdio.h>

#define _INTERNAL_BUFSIZ	4096
#define REG1 register


/***
*_getbuf() - Allocate a buffer and init stream data bases
*
*Purpose:
*       Allocates a buffer for a stream and inits the stream data bases.
*
*       [NOTE  1: This routine assumes the caller has already checked to make
*       sure the stream needs a buffer.
*
*       [NOTE 2: Multi-thread - Assumes caller has aquired stream lock, if
*       needed.]
*
*Entry:
*       FILE *stream = stream to allocate a buffer for
*
*Exit:
*       void
*
*Exceptions:
*
*******************************************************************************/

void __cdecl _getbuf (
        FILE *str
        )
{
        REG1 FILE *stream;

        _ASSERTE(str != NULL);

        /* Init pointers */
        stream = str;


        /* Try to get a big buffer */
#ifdef _WIN32
        if (stream->_base = malloc(_INTERNAL_BUFSIZ))
#else  /* _WIN32 */
#if defined (_M_MPPC) || defined (_M_M68K)
        if (stream->_base = malloc(BUFSIZ))
#endif  /* defined (_M_MPPC) || defined (_M_M68K) */
#endif  /* _WIN32 */
        {
                /* Got a big buffer */
                stream->_flag |= _IOMYBUF;
#ifdef _WIN32
                stream->_bufsiz = _INTERNAL_BUFSIZ;
#else  /* _WIN32 */
#if defined (_M_MPPC) || defined (_M_M68K)
                stream->_bufsiz = BUFSIZ;
#endif  /* defined (_M_MPPC) || defined (_M_M68K) */
#endif  /* _WIN32 */
        }

        else {


                /* Did NOT get a buffer - use single char buffering. */
                stream->_flag |= _IONBF;
                stream->_base = (char *)&(stream->_charbuf);
#ifdef _WIN32
                stream->_bufsiz = 2;
#else  /* _WIN32 */
#if defined (_M_MPPC) || defined (_M_M68K)
                stream->_bufsiz = 1;
#endif  /* defined (_M_MPPC) || defined (_M_M68K) */
#endif  /* _WIN32 */

        }

        stream->_ptr = stream->_base;
        stream->_cnt = 0;

        return;
}
#endif
