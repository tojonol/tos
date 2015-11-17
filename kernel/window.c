#include <kernel.h>

//poke a character on the screen where the cursor is
void poke_char(WINDOW* wnd, WORD ch)
{
	int xoffset = (wnd->x+wnd->cursor_x)*2;
	int yoffset = (wnd->y+wnd->cursor_y)*160;
	poke_w(xoffset + yoffset + 0xb8000, ch);
}

//move the cursor to position x y
void move_cursor(WINDOW* wnd, int x, int y)
{
	wnd->cursor_x = x;
	wnd->cursor_y = y;
}

//remove the cursor from the screen
void remove_cursor(WINDOW* wnd)
{
	poke_char(wnd,' ');
}

//Show the cursor on the screen
void show_cursor(WINDOW* wnd)
{
	poke_char(wnd, wnd->cursor_char | (0x0f << 8));
}

//clear the window
void clear_window(WINDOW* wnd)
{
	move_cursor(wnd, 0, 0);
	int i, j;
	//iterater through the window and set the characters to 0
	for (i = wnd->x; i< wnd->width; i++)
	{
		for (j = wnd->y; j< wnd->height; j++)
		{
			wnd->cursor_x = i;
			wnd->cursor_y = j;
			poke_char(wnd, 0);
		}
	}
	move_cursor(wnd, 0, 0);
}

//move cursor forward
void advance_cursor(WINDOW* wnd)
{
	wnd->cursor_x++;
	if (wnd->width == wnd->cursor_x)
	{
		wnd->cursor_x = 0;
		wnd->cursor_y++;
	}
}

//write a char to the screen
void output_char(WINDOW* wnd, unsigned char c)
{
    int i = 0, j = 0;
    WORD prevchar;
    remove_cursor(wnd);
    //if the characte is newline
    if (c== '\n')
    {
	wnd->cursor_x = 0;
	wnd->cursor_y++;
    }
    //if the character is a letter
    else
    {
	poke_char(wnd, (short unsigned int) c | (0x0f << 8));
	wnd->cursor_x++;
	if (wnd->cursor_x == wnd->width)
	{
	    wnd->cursor_x = 0;
	    wnd->cursor_y++;
	}
    }

    //if we have reached the bottom line we need to shift
    //everything up
    if (wnd->cursor_y == wnd->height)
    {
	for (i = 0; i< wnd->height; i++)
        {
             for (j = 0; j< wnd->width; j++)
             {
                 wnd->cursor_x = j;
                 wnd->cursor_y = i;
                 //get char from previous line
		 if(i==wnd->height-1)
		    prevchar=' ';
		 else
		    prevchar=peek_w(160*(wnd->y+i+1)+2*(wnd->x+j)+0xb8000);
	         poke_char(wnd, prevchar);
             }
        }
    	wnd->cursor_x = 0;
    	wnd->cursor_y = wnd->height - 1;
    }
    show_cursor(wnd);
}

//While we have not reached the end of string, output each char
void output_string(WINDOW* wnd, const char *str)
{
	while (*str != '\0')
	{
		unsigned char c = (unsigned char) *str;
		output_char(wnd, c);
		str++;
	}
}



/*
 * There is not need to make any changes to the code below,
 * however, you are encouraged to at least look at it!
 */
#define MAXBUF (sizeof(long int) * 8)		 /* enough for binary */

char *printnum(char *b, unsigned int u, int base,
	       BOOL negflag, int length, BOOL ladjust,
	       char padc, BOOL upcase)
{
    char	buf[MAXBUF];	/* build number here */
    char	*p = &buf[MAXBUF-1];
    int		size;
    char	*digs;
    static char up_digs[] = "0123456789ABCDEF";
    static char low_digs[] = "0123456789abcdef";

    digs = upcase ? up_digs : low_digs;
    do {
	*p-- = digs[ u % base ];
	u /= base;
    } while( u != 0 );

    if (negflag)
	*b++ = '-';

    size = &buf [MAXBUF - 1] - p;

    if (size < length && !ladjust) {
	while (length > size) {
	    *b++ = padc;
	    length--;
	}
    }

    while (++p != &buf [MAXBUF])
	*b++ = *p;

    if (size < length) {
	/* must be ladjust */
	while (length > size) {
	    *b++ = padc;
	    length--;
	}
    }
    return b;
}


/*
 *  This version implements therefore following printf features:
 *
 *	%d	decimal conversion
 *	%u	unsigned conversion
 *	%x	hexadecimal conversion
 *	%X	hexadecimal conversion with capital letters
 *	%o	octal conversion
 *	%c	character
 *	%s	string
 *	%m.n	field width, precision
 *	%-m.n	left adjustment
 *	%0m.n	zero-padding
 *	%*.*	width and precision taken from arguments
 *
 *  This version does not implement %f, %e, or %g.  It accepts, but
 *  ignores, an `l' as in %ld, %lo, %lx, and %lu, and therefore will not
 *  work correctly on machines for which sizeof(long) != sizeof(int).
 *  It does not even parse %D, %O, or %U; you should be using %ld, %o and
 *  %lu if you mean long conversion.
 *
 *  This version implements the following nonstandard features:
 *
 *	%b	binary conversion
 *
 */


#define isdigit(d) ((d) >= '0' && (d) <= '9')
#define ctod(c) ((c) - '0')


void vsprintf(char *buf, const char *fmt, va_list argp)
{
    char		*p;
    char		*p2;
    int			length;
    int			prec;
    int			ladjust;
    char		padc;
    int			n;
    unsigned int        u;
    int			negflag;
    char		c;

    while (*fmt != '\0') {
	if (*fmt != '%') {
	    *buf++ = *fmt++;
	    continue;
	}
	fmt++;
	if (*fmt == 'l')
	    fmt++;	     /* need to use it if sizeof(int) < sizeof(long) */

	length = 0;
	prec = -1;
	ladjust = FALSE;
	padc = ' ';

	if (*fmt == '-') {
	    ladjust = TRUE;
	    fmt++;
	}

	if (*fmt == '0') {
	    padc = '0';
	    fmt++;
	}

	if (isdigit (*fmt)) {
	    while (isdigit (*fmt))
		length = 10 * length + ctod (*fmt++);
	}
	else if (*fmt == '*') {
	    length = va_arg (argp, int);
	    fmt++;
	    if (length < 0) {
		ladjust = !ladjust;
		length = -length;
	    }
	}

	if (*fmt == '.') {
	    fmt++;
	    if (isdigit (*fmt)) {
		prec = 0;
		while (isdigit (*fmt))
		    prec = 10 * prec + ctod (*fmt++);
	    } else if (*fmt == '*') {
		prec = va_arg (argp, int);
		fmt++;
	    }
	}

	negflag = FALSE;

	switch(*fmt) {
	case 'b':
	case 'B':
	    u = va_arg (argp, unsigned int);
	    buf = printnum (buf, u, 2, FALSE, length, ladjust, padc, 0);
	    break;

	case 'c':
	    c = va_arg (argp, int);
	    *buf++ = c;
	    break;

	case 'd':
	case 'D':
	    n = va_arg (argp, int);
	    if (n >= 0)
		u = n;
	    else {
		u = -n;
		negflag = TRUE;
	    }
	    buf = printnum (buf, u, 10, negflag, length, ladjust, padc, 0);
	    break;

	case 'o':
	case 'O':
	    u = va_arg (argp, unsigned int);
	    buf = printnum (buf, u, 8, FALSE, length, ladjust, padc, 0);
	    break;

	case 's':
	    p = va_arg (argp, char *);
	    if (p == (char *)0)
		p = "(NULL)";
	    if (length > 0 && !ladjust) {
		n = 0;
		p2 = p;
		for (; *p != '\0' && (prec == -1 || n < prec); p++)
		    n++;
		p = p2;
		while (n < length) {
		    *buf++ = ' ';
		    n++;
		}
	    }
	    n = 0;
	    while (*p != '\0') {
		if (++n > prec && prec != -1)
		    break;
		*buf++ = *p++;
	    }
	    if (n < length && ladjust) {
		while (n < length) {
		    *buf++ = ' ';
		    n++;
		}
	    }
	    break;

	case 'u':
	case 'U':
	    u = va_arg (argp, unsigned int);
	    buf = printnum (buf, u, 10, FALSE, length, ladjust, padc, 0);
	    break;

	case 'x':
	    u = va_arg (argp, unsigned int);
	    buf = printnum (buf, u, 16, FALSE, length, ladjust, padc, 0);
	    break;

	case 'X':
	    u = va_arg (argp, unsigned int);
	    buf = printnum (buf, u, 16, FALSE, length, ladjust, padc, 1);
	    break;

	case '\0':
	    fmt--;
	    break;

	default:
	    *buf++ = *fmt;
	}
	fmt++;
    }
    *buf = '\0';
}



void wprintf(WINDOW* wnd, const char *fmt, ...)
{
    va_list	argp;
    char	buf[160];

    va_start(argp, fmt);
    vsprintf(buf, fmt, argp);
    output_string(wnd, buf);
    va_end(argp);
}




static WINDOW kernel_window_def = {0, 0, 80, 25, 0, 0, ' '};
WINDOW* kernel_window = &kernel_window_def;


void kprintf(const char *fmt, ...)
{
    va_list	  argp;
    char	  buf[160];

    va_start(argp, fmt);
    vsprintf(buf, fmt, argp);
    output_string(kernel_window, buf);
    va_end(argp);
}
