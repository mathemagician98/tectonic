#include <tectonic/tectonic.h>
#include <tectonic/internals.h>
#include <tectonic/xetexd.h>
#include <tectonic/synctex.h>
#include <tectonic/stubs.h>


/* How to output to the GF or DVI file.  */
#define WRITE_OUT(a, b) \
    if (fwrite ((char *) &dvi_buf[a], sizeof (dvi_buf[a]), (b) - (a) + 1, dvi_file) != (b) - (a) + 1) { \
	_tt_abort ("fwrite did not write all data: %s", strerror(errno)); \
    }


integer zlength(str_number s)
{
    register integer Result;
    length_regmem if ((s >= 65536L))
        Result = str_start[(s + 1) - 65536L] - str_start[(s) - 65536L];
    else if ((s >= 32) && (s < 127))
        Result = 1;
    else if ((s <= 127))
        Result = 3;
    else if ((s < 256))
        Result = 4;
    else
        Result = 8;
    return Result;
}

str_number make_string(void)
{
    register str_number Result;
    make_string_regmem if (str_ptr == max_strings)
        overflow(65540L /*"number of strings" */ , max_strings - init_str_ptr);
    str_ptr++;
    str_start[(str_ptr) - 65536L] = pool_ptr;
    Result = str_ptr - 1;
    return Result;
}

void zappend_str(str_number s)
{
    append_str_regmem integer i;
    pool_pointer j;
    i = length(s);
    {
        if (pool_ptr + i > pool_size)
            overflow(65539L /*"pool size" */ , pool_size - init_pool_ptr);
    }
    j = str_start[(s) - 65536L];
    while ((i > 0)) {

        {
            str_pool[pool_ptr] = str_pool[j];
            pool_ptr++;
        }
        j++;
        i--;
    }
}

boolean zstr_eq_buf(str_number s, integer k)
{
    register boolean Result;
    str_eq_buf_regmem pool_pointer j;
    boolean result;
    j = str_start[(s) - 65536L];
    while (j < str_start[(s + 1) - 65536L]) {

        if (buffer[k] >= 65536L) {

            if (str_pool[j] != 55296L + (buffer[k] - 65536L) / 1024) {
                result = false;
                goto lab45;
            } else if (str_pool[j + 1] != 56320L + (buffer[k] - 65536L) % 1024) {
                result = false;
                goto lab45;
            } else
                j++;
        } else if (str_pool[j] != buffer[k]) {
            result = false;
            goto lab45;
        }
        j++;
        k++;
    }
    result = true;
 lab45:                        /*not_found */ Result = result;
    return Result;
}

boolean zstr_eq_str(str_number s, str_number t)
{
    register boolean Result;
    str_eq_str_regmem pool_pointer j, k;
    boolean result;
    result = false;
    if (length(s) != length(t))
        goto lab45;
    if ((length(s) == 1)) {
        if (s < 65536L) {
            if (t < 65536L) {
                if (s != t)
                    goto lab45;
            } else {

                if (s != str_pool[str_start[(t) - 65536L]])
                    goto lab45;
            }
        } else {

            if (t < 65536L) {
                if (str_pool[str_start[(s) - 65536L]] != t)
                    goto lab45;
            } else {

                if (str_pool[str_start[(s) - 65536L]] != str_pool[str_start[(t) - 65536L]])
                    goto lab45;
            }
        }
    } else {

        j = str_start[(s) - 65536L];
        k = str_start[(t) - 65536L];
        while (j < str_start[(s + 1) - 65536L]) {

            if (str_pool[j] != str_pool[k])
                goto lab45;
            j++;
            k++;
        }
    }
    result = true;
 lab45:                        /*not_found */ Result = result;
    return Result;
}

str_number zsearch_string(str_number search)
{
    register str_number Result;
    search_string_regmem str_number result;
    str_number s;
    integer len;
    result = 0;
    len = length(search);
    if (len == 0) {
        result = 65622L /*"" */ ;
        goto lab40;
    } else {

        s = search - 1;
        while (s > 65535L) {

            if (length(s) == len) {

                if (str_eq_str(s, search)) {
                    result = s;
                    goto lab40;
                }
            }
            s--;
        }
    }
 lab40:                        /*found */ Result = result;
    return Result;
}

str_number slow_make_string(void)
{
    register str_number Result;
    slow_make_string_regmem str_number s;
    str_number t;
    t = make_string();
    s = search_string(t);
    if (s > 0) {
        {
            str_ptr--;
            pool_ptr = str_start[(str_ptr) - 65536L];
        }
        Result = s;
        return Result;
    }
    Result = t;
    return Result;
}

void term_input(void)
{
    term_input_regmem integer k;
    fflush(stdout);
    if (!input_line(term_in))
        fatal_error(65543L /*"End of file on the terminal!" */ );
    term_offset = 0;
    selector--;
    if (last != first) {
        register integer for_end;
        k = first;
        for_end = last - 1;
        if (k <= for_end)
            do
                print(buffer[k]);
            while (k++ < for_end);
    }
    print_ln();
    selector++;
}

void zint_error(integer n)
{
    int_error_regmem print(65566L /*" (" */ );
    print_int(n);
    print_char(41 /*")" */ );
    error();
}

void pause_for_instructions(void)
{
    pause_for_instructions_regmem if (OK_to_interrupt) {
        interaction = 3 /*error_stop_mode */ ;
        if ((selector == SELECTOR_LOG_ONLY) || (selector == SELECTOR_NO_PRINT))
            selector++;
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(65576L /*"Interruption" */ );
        }
        {
            help_ptr = 3;
            help_line[2] = 65577L /*"You rang?" */ ;
            help_line[1] = 65578L /*"Try to insert some instructions for me (e.g.,`I\showlists')," */ ;
            help_line[0] = 65579L /*"unless you just want to quit by typing `X'." */ ;
        }
        deletions_allowed = false;
        error();
        deletions_allowed = true;
        interrupt = 0;
    }
}

integer zhalf(integer x)
{
    register integer Result;
    half_regmem if (odd(x))
        Result = (x + 1) / 2;
    else
        Result = x / 2;
    return Result;
}

scaled zround_decimals(small_number k)
{
    register scaled Result;
    round_decimals_regmem integer a;
    a = 0;
    while (k > 0) {

        k--;
        a = (a + dig[k] * 131072L) / 10;
    }
    Result = (a + 1) / 2;
    return Result;
}

scaled zmult_and_add(integer n, scaled x, scaled y, scaled max_answer)
{
    register scaled Result;
    mult_and_add_regmem if (n < 0) {
        x = -(integer) x;
        n = -(integer) n;
    }
    if (n == 0)
        Result = y;
    else if (((x <= (max_answer - y) / n) && (-(integer) x <= (max_answer + y) / n)))
        Result = n * x + y;
    else {

        arith_error = true;
        Result = 0;
    }
    return Result;
}

scaled zx_over_n(scaled x, integer n)
{
    register scaled Result;
    x_over_n_regmem boolean negative;
    negative = false;
    if (n == 0) {
        arith_error = true;
        Result = 0;
        tex_remainder = x;
    } else {

        if (n < 0) {
            x = -(integer) x;
            n = -(integer) n;
            negative = true;
        }
        if (x >= 0) {
            Result = x / n;
            tex_remainder = x % n;
        } else {

            Result = -(integer) ((-(integer) x) / n);
            tex_remainder = -(integer) ((-(integer) x) % n);
        }
    }
    if (negative)
        tex_remainder = -(integer) tex_remainder;
    return Result;
}

scaled zxn_over_d(scaled x, integer n, integer d)
{
    register scaled Result;
    xn_over_d_regmem boolean positive;
    nonnegative_integer t, u, v;
    if (x >= 0)
        positive = true;
    else {

        x = -(integer) x;
        positive = false;
    }
    t = (x % 32768L) * n;
    u = (x / 32768L) * n + (t / 32768L);
    v = (u % d) * 32768L + (t % 32768L);
    if (u / d >= 32768L)
        arith_error = true;
    else
        u = 32768L * (u / d) + (v / d);
    if (positive) {
        Result = u;
        tex_remainder = v % d;
    } else {

        Result = -(integer) u;
        tex_remainder = -(integer) (v % d);
    }
    return Result;
}

halfword zbadness(scaled t, scaled s)
{
    register halfword Result;
    badness_regmem integer r;
    if (t == 0)
        Result = 0;
    else if (s <= 0)
        Result = 10000 /*inf_bad */ ;
    else {

        if (t <= 7230584L)
            r = (t * 297) / s;
        else if (s >= 1663497L)
            r = t / (s / 297);
        else
            r = t;
        if (r > 1290)
            Result = 10000 /*inf_bad */ ;
        else
            Result = (r * r * r + 131072L) / 262144L;
    }
    return Result;
}

        /*:112*//*118: */
void zshow_token_list(integer p, integer q, integer l)
{
    show_token_list_regmem integer m, c;
    integer match_chr;
    UTF16_code n;
    match_chr = 35 /*"#" */ ;
    n = 48 /*"0" */ ;
    tally = 0;
    while ((p != -268435455L) && (tally < l)) {

        if (p == q) {           /*332: */
            first_count = tally;
            trick_count = tally + 1 + error_line - half_error_line;
            if (trick_count < error_line)
                trick_count = error_line;
        }
        if ((p < hi_mem_min) || (p > mem_end)) {
            print_esc(65591L /*"CLOBBERED." */ );
            return;
        }
        if (mem[p].hh.v.LH >= 33554431L /*cs_token_flag */ )
            print_cs(mem[p].hh.v.LH - 33554431L);
        else {

            m = mem[p].hh.v.LH / 2097152L /*max_char_val */ ;
            c = mem[p].hh.v.LH % 2097152L /*max_char_val */ ;
            if (mem[p].hh.v.LH < 0)
                print_esc(65874L /*"BAD." */ );
            else                /*306: */
                switch (m) {
                case 1:
                case 2:
                case 3:
                case 4:
                case 7:
                case 8:
                case 10:
                case 11:
                case 12:
                    print_char(c);
                    break;
                case 6:
                    {
                        print_char(c);
                        print_char(c);
                    }
                    break;
                case 5:
                    {
                        print_char(match_chr);
                        if (c <= 9)
                            print_char(c + 48);
                        else {

                            print_char(33 /*"!" */ );
                            return;
                        }
                    }
                    break;
                case 13:
                    {
                        match_chr = c;
                        print_char(c);
                        n++;
                        print_char(n);
                        if (n > 57 /*"9" */ )
                            return;
                    }
                    break;
                case 14:
                    if (c == 0)
                        print(65875L /*"->" */ );
                    break;
                default:
                    print_esc(65874L /*"BAD." */ );
                    break;
                }
        }
        p = mem[p].hh.v.RH;
    }
    if (p != -268435455L)
        print_esc(65704L /*"ETC." */ );
}

void runaway(void)
{
    runaway_regmem halfword p;
    if (scanner_status > 1 /*skipping */ ) {
        switch (scanner_status) {
        case 2:
            {
                print_nl(65888L /*"Runaway definition" */ );
                p = def_ref;
            }
            break;
        case 3:
            {
                print_nl(65889L /*"Runaway argument" */ );
                p = mem_top - 3;
            }
            break;
        case 4:
            {
                print_nl(65890L /*"Runaway preamble" */ );
                p = mem_top - 4;
            }
            break;
        case 5:
            {
                print_nl(65891L /*"Runaway text" */ );
                p = def_ref;
            }
            break;
        }
        print_char(63 /*"?" */ );
        print_ln();
        show_token_list(mem[p].hh.v.RH, -268435455L, error_line - 10);
    }
}

halfword get_avail(void)
{
    register halfword Result;
    get_avail_regmem halfword p;
    p = avail;
    if (p != -268435455L)
        avail = mem[avail].hh.v.RH;
    else if (mem_end < mem_max) {
        mem_end++;
        p = mem_end;
    } else {

        hi_mem_min--;
        p = hi_mem_min;
        if (hi_mem_min <= lo_mem_max) {
            runaway();
            overflow(65580L /*"main memory size" */ , mem_max + 1 - mem_min);
        }
    }
    mem[p].hh.v.RH = -268435455L;
    Result = p;
    return Result;
}

void zflush_list(halfword p)
{
    flush_list_regmem halfword q, r;
    if (p != -268435455L) {
        r = p;
        do {
            q = r;
            r = mem[r].hh.v.RH;
        } while (!(r == -268435455L));
        mem[q].hh.v.RH = avail;
        avail = p;
    }
}

halfword zget_node(integer s)
{
    register halfword Result;
    get_node_regmem halfword p;
    halfword q;
    integer r;
    integer t;
 lab20:                        /*restart */ p = rover;
    do {
        /*131: */ q = p + mem[p].hh.v.LH;
        while ((mem[q].hh.v.RH == 1073741823L)) {

            t = mem[q + 1].hh.v.RH;
            if (q == rover)
                rover = t;
            mem[t + 1].hh.v.LH = mem[q + 1].hh.v.LH;
            mem[mem[q + 1].hh.v.LH + 1].hh.v.RH = t;
            q = q + mem[q].hh.v.LH;
        }
        r = q - s;
        if (r > p + 1) {        /*132: */
            mem[p].hh.v.LH = r - p;
            rover = p;
            goto lab40;
        }
        if (r == p) {

            if (mem[p + 1].hh.v.RH != p) {      /*133: */
                rover = mem[p + 1].hh.v.RH;
                t = mem[p + 1].hh.v.LH;
                mem[rover + 1].hh.v.LH = t;
                mem[t + 1].hh.v.RH = rover;
                goto lab40;
            }
        }
        mem[p].hh.v.LH = q - /*:131 */ p;
        p = mem[p + 1].hh.v.RH;
    } while (!(p == rover));
    if (s == 1073741824L) {
        Result = 1073741823L;
        return Result;
    }
    if (lo_mem_max + 2 < hi_mem_min) {

        if (lo_mem_max + 2 <= mem_bot + 1073741823L) {  /*130: */
            if (hi_mem_min - lo_mem_max >= 1998)
                t = lo_mem_max + 1000;
            else
                t = lo_mem_max + 1 + (hi_mem_min - lo_mem_max) / 2;
            p = mem[rover + 1].hh.v.LH;
            q = lo_mem_max;
            mem[p + 1].hh.v.RH = q;
            mem[rover + 1].hh.v.LH = q;
            if (t > mem_bot + 1073741823L)
                t = mem_bot + 1073741823L;
            mem[q + 1].hh.v.RH = rover;
            mem[q + 1].hh.v.LH = p;
            mem[q].hh.v.RH = 1073741823L;
            mem[q].hh.v.LH = t - lo_mem_max;
            lo_mem_max = t;
            mem[lo_mem_max].hh.v.RH = -268435455L;
            mem[lo_mem_max].hh.v.LH = -268435455L;
            rover = q;
            goto lab20;
        }
    }
    overflow(65580L /*"main memory size" */ , mem_max + 1 - mem_min);

lab40: /*found */
    mem[r].hh.v.RH = -268435455L;
    if (s >= 3 /*medium_node_size */ ) {
        mem[r + s - 1].hh.v.LH = cur_input.synctex_tag_field;
        mem[r + s - 1].hh.v.RH = line;
    }
    Result = r;
    return Result;
}

void zfree_node(halfword p, halfword s)
{
    free_node_regmem halfword q;
    mem[p].hh.v.LH = s;
    mem[p].hh.v.RH = 1073741823L;
    q = mem[rover + 1].hh.v.LH;
    mem[p + 1].hh.v.LH = q;
    mem[p + 1].hh.v.RH = rover;
    mem[rover + 1].hh.v.LH = p;
    mem[q + 1].hh.v.RH = p;
}

halfword new_null_box(void)
{
    register halfword Result;
    new_null_box_regmem halfword p;
    p = get_node(8 /*box_node_size */ );
    mem[p].hh.u.B0 = 0 /*hlist_node */ ;
    mem[p].hh.u.B1 = 0 /*min_quarterword */ ;
    mem[p + 1].cint = 0;
    mem[p + 2].cint = 0;
    mem[p + 3].cint = 0;
    mem[p + 4].cint = 0;
    mem[p + 5].hh.v.RH = -268435455L;
    mem[p + 5].hh.u.B0 = 0 /*normal */ ;
    mem[p + 5].hh.u.B1 = 0 /*normal */ ;
    mem[p + 6].gr = 0.0;
    Result = p;
    return Result;
}

halfword new_rule(void)
{
    register halfword Result;
    new_rule_regmem halfword p;
    p = get_node(5 /*rule_node_size */ );
    mem[p].hh.u.B0 = 2 /*rule_node */ ;
    mem[p].hh.u.B1 = 0;
    mem[p + 1].cint = -1073741824L;
    mem[p + 2].cint = -1073741824L;
    mem[p + 3].cint = -1073741824L;
    Result = p;
    return Result;
}

halfword znew_ligature(internal_font_number f, quarterword c, halfword q)
{
    register halfword Result;
    new_ligature_regmem halfword p;
    p = get_node(2 /*small_node_size */ );
    mem[p].hh.u.B0 = 6 /*ligature_node */ ;
    mem[p + 1].hh.u.B0 = f;
    mem[p + 1].hh.u.B1 = c;
    mem[p + 1].hh.v.RH = q;
    mem[p].hh.u.B1 = 0;
    Result = p;
    return Result;
}

halfword znew_lig_item(quarterword c)
{
    register halfword Result;
    new_lig_item_regmem halfword p;
    p = get_node(2 /*small_node_size */ );
    mem[p].hh.u.B1 = c;
    mem[p + 1].hh.v.RH = -268435455L;
    Result = p;
    return Result;
}

halfword new_disc(void)
{
    register halfword Result;
    new_disc_regmem halfword p;
    p = get_node(2 /*small_node_size */ );
    mem[p].hh.u.B0 = 7 /*disc_node */ ;
    mem[p].hh.u.B1 = 0;
    mem[p + 1].hh.v.LH = -268435455L;
    mem[p + 1].hh.v.RH = -268435455L;
    Result = p;
    return Result;
}

void zcopy_native_glyph_info(halfword src, halfword dest)
{
    copy_native_glyph_info_regmem integer glyph_count;
    if (mem[src + 5].ptr != NULL) {
        glyph_count = mem[src + 4].qqqq.u.B3;
        mem[dest + 5].ptr = xmalloc_array(char, glyph_count * 10 /*native_glyph_info_size */ );
        memcpy(mem[dest + 5].ptr, mem[src + 5].ptr, glyph_count * 10 /*native_glyph_info_size */ );
        mem[dest + 4].qqqq.u.B3 = glyph_count;
    }
}

halfword znew_math(scaled w, small_number s)
{
    register halfword Result;
    new_math_regmem halfword p;
    p = get_node(3 /*medium_node_size */ );
    mem[p].hh.u.B0 = 9 /*math_node */ ;
    mem[p].hh.u.B1 = s;
    mem[p + 1].cint = w;
    Result = p;
    return Result;
}

halfword znew_spec(halfword p)
{
    register halfword Result;
    new_spec_regmem halfword q;
    q = get_node(4 /*glue_spec_size */ );
    mem[q] = mem[p];
    mem[q].hh.v.RH = -268435455L;
    mem[q + 1].cint = mem[p + 1].cint;
    mem[q + 2].cint = mem[p + 2].cint;
    mem[q + 3].cint = mem[p + 3].cint;
    Result = q;
    return Result;
}

halfword znew_param_glue(small_number n)
{
    register halfword Result;
    new_param_glue_regmem halfword p;
    halfword q;
    p = get_node(3 /*medium_node_size */ );
    mem[p].hh.u.B0 = 10 /*glue_node */ ;
    mem[p].hh.u.B1 = n + 1;
    mem[p + 1].hh.v.RH = -268435455L;
    q = /*232: */ eqtb[2252240L /*glue_base */  + n].hh.v.RH /*:232 */ ;
    mem[p + 1].hh.v.LH = q;
    mem[q].hh.v.RH++;
    Result = p;
    return Result;
}

halfword znew_glue(halfword q)
{
    register halfword Result;
    new_glue_regmem halfword p;
    p = get_node(3 /*medium_node_size */ );
    mem[p].hh.u.B0 = 10 /*glue_node */ ;
    mem[p].hh.u.B1 = 0 /*normal */ ;
    mem[p + 1].hh.v.RH = -268435455L;
    mem[p + 1].hh.v.LH = q;
    mem[q].hh.v.RH++;
    Result = p;
    return Result;
}

halfword znew_skip_param(small_number n)
{
    register halfword Result;
    new_skip_param_regmem halfword p;
    temp_ptr = new_spec( /*232: */ eqtb[2252240L /*glue_base */  + n].hh.v.RH /*:232 */ );
    p = new_glue(temp_ptr);
    mem[temp_ptr].hh.v.RH = -268435455L;
    mem[p].hh.u.B1 = n + 1;
    Result = p;
    return Result;
}

halfword znew_kern(scaled w)
{
    register halfword Result;
    new_kern_regmem halfword p;
    p = get_node(3 /*medium_node_size */ );
    mem[p].hh.u.B0 = 11 /*kern_node */ ;
    mem[p].hh.u.B1 = 0 /*normal */ ;
    mem[p + 1].cint = w;
    Result = p;
    return Result;
}

halfword znew_penalty(integer m)
{
    register halfword Result;
    new_penalty_regmem halfword p;
    p = get_node(3 /*medium_node_size */ );
    mem[p].hh.u.B0 = 12 /*penalty_node */ ;
    mem[p].hh.u.B1 = 0;
    mem[p + 1].cint = m;
    Result = p;
    return Result;
}

/*:165*/

halfword zprev_rightmost(halfword s, halfword e)
{
    register halfword Result;
    prev_rightmost_regmem halfword p;
    Result = -268435455L;
    p = s;
    if (p == -268435455L)
        return Result;
    while (mem[p].hh.v.RH != e) {

        p = mem[p].hh.v.RH;
        if (p == -268435455L)
            return Result;
    }
    Result = p;
    return Result;
}

scaled zround_xn_over_d(scaled x, integer n, integer d)
{
    register scaled Result;
    round_xn_over_d_regmem boolean positive;
    nonnegative_integer t, u, v;
    if (x >= 0)
        positive = true;
    else {

        x = -(integer) x;
        positive = false;
    }
    t = (x % 32768L) * n;
    u = (x / 32768L) * n + (t / 32768L);
    v = (u % d) * 32768L + (t % 32768L);
    if (u / d >= 32768L)
        arith_error = true;
    else
        u = 32768L * (u / d) + (v / d);
    v = v % d;
    if (2 * v >= d)
        u++;
    if (positive)
        Result = u;
    else
        Result = -(integer) u;
    return Result;
}

void zshort_display(integer p)
{
    short_display_regmem integer n;
    while (p > mem_min) {

        if ((p >= hi_mem_min)) {
            if (p <= mem_end) {
                if (mem[p].hh.u.B0 != font_in_short_display) {
                    if ((mem[p].hh.u.B0 > font_max))
                        print_char(42 /*"*" */ );
                    else        /*279: */
                        print_esc(hash[2243238L /*font_id_base */  + mem[p].hh.u.B0].v.RH);
                    print_char(32 /*" " */ );
                    font_in_short_display = mem[p].hh.u.B0;
                }
                print(mem[p].hh.u.B1);
            }
        } else                  /*183: */
            switch (mem[p].hh.u.B0) {
            case 0:
            case 1:
            case 3:
            case 4:
            case 5:
            case 13:
                print(65590L /*"[]" */ );
                break;
            case 8:
                switch (mem[p].hh.u.B1) {
                case 40:
                case 41:
                    {
                        if (mem[p + 4].qqqq.u.B1 != font_in_short_display) {
                            print_esc(hash[2243238L /*font_id_base */  + mem[p + 4].qqqq.u.B1].v.RH);
                            print_char(32 /*" " */ );
                            font_in_short_display = mem[p + 4].qqqq.u.B1;
                        }
                        print_native_word(p);
                    }
                    break;
                default:
                    print(65590L /*"[]" */ );
                    break;
                }
                break;
            case 2:
                print_char(124 /*"|" */ );
                break;
            case 10:
                if (mem[p + 1].hh.v.LH != mem_bot)
                    print_char(32 /*" " */ );
                break;
            case 9:
                if (mem[p].hh.u.B1 >= 4 /*L_code */ )
                    print(65590L /*"[]" */ );
                else
                    print_char(36 /*"$" */ );
                break;
            case 6:
                short_display(mem[p + 1].hh.v.RH);
                break;
            case 7:
                {
                    short_display(mem[p + 1].hh.v.LH);
                    short_display(mem[p + 1].hh.v.RH);
                    n = mem[p].hh.u.B1;
                    while (n > 0) {

                        if (mem[p].hh.v.RH != -268435455L)
                            p = mem[p].hh.v.RH;
                        n--;
                    }
                }
                break;
            default:
                ;
                break;
            }
        p = mem[p].hh.v.RH;
    }
}

void zprint_font_and_char(integer p)
{
    print_font_and_char_regmem if (p > mem_end)
        print_esc(65591L /*"CLOBBERED." */ );
    else {

        if ((mem[p].hh.u.B0 > font_max))
            print_char(42 /*"*" */ );
        else                    /*279: */
            print_esc(hash[2243238L /*font_id_base */  + mem[p].hh.u.B0].v.RH);
        print_char(32 /*" " */ );
        print(mem[p].hh.u.B1);
    }
}

void zprint_mark(integer p)
{
    print_mark_regmem print_char(123 /*"_" */ );
    if ((p < hi_mem_min) || (p > mem_end))
        print_esc(65591L /*"CLOBBERED." */ );
    else
        show_token_list(mem[p].hh.v.RH, -268435455L, max_print_line - 10);
    print_char(125 /*"_" */ );
}

void zprint_rule_dimen(scaled d)
{
    print_rule_dimen_regmem if ((d == -1073741824L))
        print_char(42 /*"*" */ );
    else
        print_scaled(d);
}

void zprint_glue(scaled d, integer order, str_number s)
{
    print_glue_regmem print_scaled(d);
    if ((order < 0 /*normal */ ) || (order > 3 /*filll */ ))
        print(65592L /*"foul" */ );
    else if (order > 0 /*normal */ ) {
        print(65593L /*"fil" */ );
        while (order > 1 /*fil */ ) {

            print_char(108 /*"l" */ );
            order--;
        }
    } else if (s != 0)
        print(s);
}

void zprint_spec(integer p, str_number s)
{
    print_spec_regmem if ((p < mem_min) || (p >= lo_mem_max))
        print_char(42 /*"*" */ );
    else {

        print_scaled(mem[p + 1].cint);
        if (s != 0)
            print(s);
        if (mem[p + 2].cint != 0) {
            print(65594L /*" plus " */ );
            print_glue(mem[p + 2].cint, mem[p].hh.u.B0, s);
        }
        if (mem[p + 3].cint != 0) {
            print(65595L /*" minus " */ );
            print_glue(mem[p + 3].cint, mem[p].hh.u.B1, s);
        }
    }
}

void zprint_fam_and_char(halfword p)
{
    print_fam_and_char_regmem integer c;
    print_esc(65760L /*"fam" */ );
    print_int((mem[p].hh.u.B0 % 256) % 256);
    print_char(32 /*" " */ );
    c = ((unsigned short) mem[p].hh.u.B1 + ((mem[p].hh.u.B0 / 256) * 65536L));
    if (c < 65536L)
        print(c);
    else
        print_char(c);
}

void zprint_delimiter(halfword p)
{
    print_delimiter_regmem integer a;
    a = (mem[p].qqqq.u.B0 % 256) * 256 + (mem[p].qqqq.u.B1 + (mem[p].qqqq.u.B0 / 256) * 65536L);
    a = a * 4096 + (mem[p].qqqq.u.B2 % 256) * 256 + (mem[p].qqqq.u.B3 + (mem[p].qqqq.u.B2 / 256) * 65536L);
    if (a < 0)
        print_int(a);
    else
        print_hex(a);
}

void zprint_subsidiary_data(halfword p, UTF16_code c)
{
    print_subsidiary_data_regmem if ((pool_ptr - str_start[(str_ptr) - 65536L]) >= depth_threshold) {
        if (mem[p].hh.v.RH != 0 /*empty */ )
            print(65596L /*" []" */ );
    } else {

        {
            str_pool[pool_ptr] = c;
            pool_ptr++;
        }
        temp_ptr = p;
        switch (mem[p].hh.v.RH) {
        case 1:
            {
                print_ln();
                print_current_string();
                print_fam_and_char(p);
            }
            break;
        case 2:
            show_info();
            break;
        case 3:
            if (mem[p].hh.v.LH == -268435455L) {
                print_ln();
                print_current_string();
                print(66232L /*"__" */ );
            } else
                show_info();
            break;
        default:
            ;
            break;
        }
        pool_ptr--;
    }
}

void zprint_style(integer c)
{
    print_style_regmem switch (c / 2) {
    case 0:
        print_esc(66233L /*"displaystyle" */ );
        break;
    case 1:
        print_esc(66234L /*"textstyle" */ );
        break;
    case 2:
        print_esc(66235L /*"scriptstyle" */ );
        break;
    case 3:
        print_esc(66236L /*"scriptscriptstyle" */ );
        break;
    default:
        print(66237L /*"Unknown style!" */ );
        break;
    }
}

void zprint_skip_param(integer n)
{
    print_skip_param_regmem switch (n) {
    case 0:
        print_esc(65671L /*"lineskip" */ );
        break;
    case 1:
        print_esc(65672L /*"baselineskip" */ );
        break;
    case 2:
        print_esc(65673L /*"parskip" */ );
        break;
    case 3:
        print_esc(65674L /*"abovedisplayskip" */ );
        break;
    case 4:
        print_esc(65675L /*"belowdisplayskip" */ );
        break;
    case 5:
        print_esc(65676L /*"abovedisplayshortskip" */ );
        break;
    case 6:
        print_esc(65677L /*"belowdisplayshortskip" */ );
        break;
    case 7:
        print_esc(65678L /*"leftskip" */ );
        break;
    case 8:
        print_esc(65679L /*"rightskip" */ );
        break;
    case 9:
        print_esc(65680L /*"topskip" */ );
        break;
    case 10:
        print_esc(65681L /*"splittopskip" */ );
        break;
    case 11:
        print_esc(65682L /*"tabskip" */ );
        break;
    case 12:
        print_esc(65683L /*"spaceskip" */ );
        break;
    case 13:
        print_esc(65684L /*"xspaceskip" */ );
        break;
    case 14:
        print_esc(65685L /*"parfillskip" */ );
        break;
    case 15:
        print_esc(65686L /*"XeTeXlinebreakskip" */ );
        break;
    case 16:
        print_esc(65687L /*"thinmuskip" */ );
        break;
    case 17:
        print_esc(65688L /*"medmuskip" */ );
        break;
    case 18:
        print_esc(65689L /*"thickmuskip" */ );
        break;
    default:
        print(65690L /*"[unknown glue parameter!]" */ );
        break;
    }
}

void zshow_node_list(integer p)
{
    show_node_list_regmem integer n;
    integer i;
    double g;
    if ((pool_ptr - str_start[(str_ptr) - 65536L]) > depth_threshold) {
        if (p > -268435455L)
            print(65596L /*" []" */ );
        return;
    }
    n = 0;
    while (p > mem_min) {

        print_ln();
        print_current_string();
        if (p > mem_end) {
            print(65597L /*"Bad link, display aborted." */ );
            return;
        }
        n++;
        if (n > breadth_max) {
            print(65598L /*"etc." */ );
            return;
        }
        if ((p >= hi_mem_min))
            print_font_and_char(p);
        else
            switch (mem[p].hh.u.B0) {
            case 0:
            case 1:
            case 13:
                {
                    if (mem[p].hh.u.B0 == 0 /*hlist_node */ )
                        print_esc(104 /*"h" */ );
                    else if (mem[p].hh.u.B0 == 1 /*vlist_node */ )
                        print_esc(118 /*"v" */ );
                    else
                        print_esc(65603L /*"unset" */ );
                    print(65604L /*"box(" */ );
                    print_scaled(mem[p + 3].cint);
                    print_char(43 /*"+" */ );
                    print_scaled(mem[p + 2].cint);
                    print(65605L /*")x" */ );
                    print_scaled(mem[p + 1].cint);
                    if (mem[p].hh.u.B0 == 13 /*unset_node */ ) {  /*193: */
                        if (mem[p].hh.u.B1 != 0 /*min_quarterword */ ) {
                            print(65566L /*" (" */ );
                            print_int(mem[p].hh.u.B1 + 1);
                            print(65607L /*" columns)" */ );
                        }
                        if (mem[p + 6].cint != 0) {
                            print(65608L /*", stretch " */ );
                            print_glue(mem[p + 6].cint, mem[p + 5].hh.u.B1, 0);
                        }
                        if (mem[p + 4].cint != 0) {
                            print(65609L /*", shrink " */ );
                            print_glue(mem[p + 4].cint, mem[p + 5].hh.u.B0, 0);
                        }
                    } else {

                        g = mem[p + 6].gr;
                        if ((g != 0.0) && (mem[p + 5].hh.u.B0 != 0 /*normal */ )) {
                            print(65610L /*", glue set " */ );
                            if (mem[p + 5].hh.u.B0 == 2 /*shrinking */ )
                                print(65611L /*"- " */ );
                            if (fabs(g) > 20000.0) {
                                if (g > 0.0)
                                    print_char(62 /*">" */ );
                                else
                                    print(65612L /*"< -" */ );
                                print_glue(20000 * 65536L, mem[p + 5].hh.u.B1, 0);
                            } else
                                print_glue(round(65536L * g), mem[p + 5].hh.u.B1, 0);
                        }
                        if (mem[p + 4].cint != 0) {
                            print(65606L /*", shifted " */ );
                            print_scaled(mem[p + 4].cint);
                        }
                        if ((eTeX_mode == 1)) { /*1491: */

                            if ((mem[p].hh.u.B0 == 0 /*hlist_node */ ) && ((mem[p].hh.u.B1) == 2 /*dlist */ ))
                                print(66893L /*", display" */ );
                        }
                    }
                    {
                        {
                            str_pool[pool_ptr] = 46 /*"." */ ;
                            pool_ptr++;
                        }
                        show_node_list(mem[p + 5].hh.v.RH);
                        pool_ptr--;
                    }
                }
                break;
            case 2:
                {
                    print_esc(65613L /*"rule(" */ );
                    print_rule_dimen(mem[p + 3].cint);
                    print_char(43 /*"+" */ );
                    print_rule_dimen(mem[p + 2].cint);
                    print(65605L /*")x" */ );
                    print_rule_dimen(mem[p + 1].cint);
                }
                break;
            case 3:
                {
                    print_esc(65614L /*"insert" */ );
                    print_int(mem[p].hh.u.B1);
                    print(65615L /*", natural size " */ );
                    print_scaled(mem[p + 3].cint);
                    print(65616L /*"; split(" */ );
                    print_spec(mem[p + 4].hh.v.RH, 0);
                    print_char(44 /*"," */ );
                    print_scaled(mem[p + 2].cint);
                    print(65617L /*"); float cost " */ );
                    print_int(mem[p + 1].cint);
                    {
                        {
                            str_pool[pool_ptr] = 46 /*"." */ ;
                            pool_ptr++;
                        }
                        show_node_list(mem[p + 4].hh.v.LH);
                        pool_ptr--;
                    }
                }
                break;
            case 8:
                switch (mem[p].hh.u.B1) {
                case 0:
                    {
                        print_write_whatsit(66711L /*"openout" */ , p);
                        print_char(61 /*"=" */ );
                        print_file_name(mem[p + 1].hh.v.RH, mem[p + 2].hh.v.LH, mem[p + 2].hh.v.RH);
                    }
                    break;
                case 1:
                    {
                        print_write_whatsit(65914L /*"write" */ , p);
                        print_mark(mem[p + 1].hh.v.RH);
                    }
                    break;
                case 2:
                    print_write_whatsit(66712L /*"closeout" */ , p);
                    break;
                case 3:
                    {
                        print_esc(66713L /*"special" */ );
                        print_mark(mem[p + 1].hh.v.RH);
                    }
                    break;
                case 4:
                    {
                        print_esc(66715L /*"setlanguage" */ );
                        print_int(mem[p + 1].hh.v.RH);
                        print(66728L /*" (hyphenmin " */ );
                        print_int(mem[p + 1].hh.u.B0);
                        print_char(44 /*"," */ );
                        print_int(mem[p + 1].hh.u.B1);
                        print_char(41 /*")" */ );
                    }
                    break;
                case 40:
                case 41:
                    {
                        print_esc(hash[2243238L /*font_id_base */  + mem[p + 4].qqqq.u.B1].v.RH);
                        print_char(32 /*" " */ );
                        print_native_word(p);
                    }
                    break;
                case 42:
                    {
                        print_esc(hash[2243238L /*font_id_base */  + mem[p + 4].qqqq.u.B1].v.RH);
                        print(66729L /*" glyph#" */ );
                        print_int(mem[p + 4].qqqq.u.B2);
                    }
                    break;
                case 43:
                case 44:
                    {
                        if (mem[p].hh.u.B1 == 43 /*pic_node */ )
                            print_esc(66716L /*"XeTeXpicfile" */ );
                        else
                            print_esc(66717L /*"XeTeXpdffile" */ );
                        print(66730L /*" "" */ );
                        {
                            register integer for_end;
                            i = 0;
                            for_end = mem[p + 4].hh.u.B0 - 1;
                            if (i <= for_end)
                                do
                                    print_raw_char(pic_path_byte(p, i), true);
                                while (i++ < for_end);
                        }
                        print(34 /*""" */ );
                    }
                    break;
                case 6:
                    print_esc(66721L /*"pdfsavepos" */ );
                    break;
                default:
                    print(66731L /*"whatsit?" */ );
                    break;
                }
                break;
            case 10:
                if (mem[p].hh.u.B1 >= 100 /*a_leaders */ ) {      /*198: */
                    print_esc(65622L /*"" */ );
                    if (mem[p].hh.u.B1 == 101 /*c_leaders */ )
                        print_char(99 /*"c" */ );
                    else if (mem[p].hh.u.B1 == 102 /*x_leaders */ )
                        print_char(120 /*"x" */ );
                    print(65623L /*"leaders " */ );
                    print_spec(mem[p + 1].hh.v.LH, 0);
                    {
                        {
                            str_pool[pool_ptr] = 46 /*"." */ ;
                            pool_ptr++;
                        }
                        show_node_list(mem[p + 1].hh.v.RH);
                        pool_ptr--;
                    }
                } else {

                    print_esc(65618L /*"glue" */ );
                    if (mem[p].hh.u.B1 != 0 /*normal */ ) {
                        print_char(40 /*"(" */ );
                        if (mem[p].hh.u.B1 < 98 /*cond_math_glue */ )
                            print_skip_param(mem[p].hh.u.B1 - 1);
                        else if (mem[p].hh.u.B1 == 98 /*cond_math_glue */ )
                            print_esc(65619L /*"nonscript" */ );
                        else
                            print_esc(65620L /*"mskip" */ );
                        print_char(41 /*")" */ );
                    }
                    if (mem[p].hh.u.B1 != 98 /*cond_math_glue */ ) {
                        print_char(32 /*" " */ );
                        if (mem[p].hh.u.B1 < 98 /*cond_math_glue */ )
                            print_spec(mem[p + 1].hh.v.LH, 0);
                        else
                            print_spec(mem[p + 1].hh.v.LH, 65621L /*"mu" */ );
                    }
                }
                break;
            case 11:
                if (mem[p].hh.u.B1 != 99 /*mu_glue */ ) {
                    print_esc(65599L /*"kern" */ );
                    if (mem[p].hh.u.B1 != 0 /*normal */ )
                        print_char(32 /*" " */ );
                    print_scaled(mem[p + 1].cint);
                    if (mem[p].hh.u.B1 == 2 /*acc_kern */ )
                        print(65624L /*" (for accent)" */ );
                    else if (mem[p].hh.u.B1 == 3 /*space_adjustment */ )
                        print(65625L /*" (space adjustment)" */ );
                } else {

                    print_esc(65626L /*"mkern" */ );
                    print_scaled(mem[p + 1].cint);
                    print(65621L /*"mu" */ );
                }
                break;
            case 40:
                {
                    print_esc(65599L /*"kern" */ );
                    print_scaled(mem[p + 1].cint);
                    if (mem[p].hh.u.B1 == 0)
                        print(65600L /*" (left margin)" */ );
                    else
                        print(65601L /*" (right margin)" */ );
                }
                break;
            case 9:
                if (mem[p].hh.u.B1 > 1 /*after */ ) {
                    if (odd(mem[p].hh.u.B1))
                        print_esc(65627L /*"end" */ );
                    else
                        print_esc(65628L /*"begin" */ );
                    if (mem[p].hh.u.B1 > 8 /*R_code */ )
                        print_char(82 /*"R" */ );
                    else if (mem[p].hh.u.B1 > 4 /*L_code */ )
                        print_char(76 /*"L" */ );
                    else
                        print_char(77 /*"M" */ );
                } else {

                    print_esc(65629L /*"math" */ );
                    if (mem[p].hh.u.B1 == 0 /*before */ )
                        print(65630L /*"on" */ );
                    else
                        print(65631L /*"off" */ );
                    if (mem[p + 1].cint != 0) {
                        print(65632L /*", surrounded " */ );
                        print_scaled(mem[p + 1].cint);
                    }
                }
                break;
            case 6:
                {
                    print_font_and_char(p + 1);
                    print(65633L /*" (ligature " */ );
                    if (mem[p].hh.u.B1 > 1)
                        print_char(124 /*"|" */ );
                    font_in_short_display = mem[p + 1].hh.u.B0;
                    short_display(mem[p + 1].hh.v.RH);
                    if (odd(mem[p].hh.u.B1))
                        print_char(124 /*"|" */ );
                    print_char(41 /*")" */ );
                }
                break;
            case 12:
                {
                    print_esc(65634L /*"penalty " */ );
                    print_int(mem[p + 1].cint);
                }
                break;
            case 7:
                {
                    print_esc(65635L /*"discretionary" */ );
                    if (mem[p].hh.u.B1 > 0) {
                        print(65636L /*" replacing " */ );
                        print_int(mem[p].hh.u.B1);
                    }
                    {
                        {
                            str_pool[pool_ptr] = 46 /*"." */ ;
                            pool_ptr++;
                        }
                        show_node_list(mem[p + 1].hh.v.LH);
                        pool_ptr--;
                    }
                    {
                        str_pool[pool_ptr] = 124 /*"|" */ ;
                        pool_ptr++;
                    }
                    show_node_list(mem[p + 1].hh.v.RH);
                    pool_ptr--;
                }
                break;
            case 4:
                {
                    print_esc(65637L /*"mark" */ );
                    if (mem[p + 1].hh.v.LH != 0) {
                        print_char(115 /*"s" */ );
                        print_int(mem[p + 1].hh.v.LH);
                    }
                    print_mark(mem[p + 1].hh.v.RH);
                }
                break;
            case 5:
                {
                    print_esc(65638L /*"vadjust" */ );
                    if (mem[p].hh.u.B1 != 0)
                        print(65639L /*" pre " */ );
                    {
                        {
                            str_pool[pool_ptr] = 46 /*"." */ ;
                            pool_ptr++;
                        }
                        show_node_list(mem[p + 1].cint);
                        pool_ptr--;
                    }
                }
                break;
            case 14:
                print_style(mem[p].hh.u.B1);
                break;
            case 15:
                {
                    print_esc(65838L /*"mathchoice" */ );
                    {
                        str_pool[pool_ptr] = 68 /*"D" */ ;
                        pool_ptr++;
                    }
                    show_node_list(mem[p + 1].hh.v.LH);
                    pool_ptr--;
                    {
                        str_pool[pool_ptr] = 84 /*"T" */ ;
                        pool_ptr++;
                    }
                    show_node_list(mem[p + 1].hh.v.RH);
                    pool_ptr--;
                    {
                        str_pool[pool_ptr] = 83 /*"S" */ ;
                        pool_ptr++;
                    }
                    show_node_list(mem[p + 2].hh.v.LH);
                    pool_ptr--;
                    {
                        str_pool[pool_ptr] = 115 /*"s" */ ;
                        pool_ptr++;
                    }
                    show_node_list(mem[p + 2].hh.v.RH);
                    pool_ptr--;
                }
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 27:
            case 26:
            case 29:
            case 28:
            case 30:
            case 31:
                {
                    switch (mem[p].hh.u.B0) {
                    case 16:
                        print_esc(66238L /*"mathord" */ );
                        break;
                    case 17:
                        print_esc(66239L /*"mathop" */ );
                        break;
                    case 18:
                        print_esc(66240L /*"mathbin" */ );
                        break;
                    case 19:
                        print_esc(66241L /*"mathrel" */ );
                        break;
                    case 20:
                        print_esc(66242L /*"mathopen" */ );
                        break;
                    case 21:
                        print_esc(66243L /*"mathclose" */ );
                        break;
                    case 22:
                        print_esc(66244L /*"mathpunct" */ );
                        break;
                    case 23:
                        print_esc(66245L /*"mathinner" */ );
                        break;
                    case 27:
                        print_esc(66246L /*"overline" */ );
                        break;
                    case 26:
                        print_esc(66247L /*"underline" */ );
                        break;
                    case 29:
                        print_esc(65855L /*"vcenter" */ );
                        break;
                    case 24:
                        {
                            print_esc(65847L /*"radical" */ );
                            print_delimiter(p + 4);
                        }
                        break;
                    case 28:
                        {
                            print_esc(65813L /*"accent" */ );
                            print_fam_and_char(p + 4);
                        }
                        break;
                    case 30:
                        {
                            print_esc(66248L /*"left" */ );
                            print_delimiter(p + 1);
                        }
                        break;
                    case 31:
                        {
                            if (mem[p].hh.u.B1 == 0 /*normal */ )
                                print_esc(66249L /*"right" */ );
                            else
                                print_esc(66250L /*"middle" */ );
                            print_delimiter(p + 1);
                        }
                        break;
                    }
                    if (mem[p].hh.u.B0 < 30 /*left_noad */ ) {
                        if (mem[p].hh.u.B1 != 0 /*normal */ ) {

                            if (mem[p].hh.u.B1 == 1 /*limits */ )
                                print_esc(66251L /*"limits" */ );
                            else
                                print_esc(66252L /*"nolimits" */ );
                        }
                        print_subsidiary_data(p + 1, 46 /*"." */ );
                    }
                    print_subsidiary_data(p + 2, 94 /*"^" */ );
                    print_subsidiary_data(p + 3, 95 /*"_" */ );
                }
                break;
            case 25:
                {
                    print_esc(66253L /*"fraction, thickness " */ );
                    if (mem[p + 1].cint == 1073741824L)
                        print(66254L /*"= default" */ );
                    else
                        print_scaled(mem[p + 1].cint);
                    if (((mem[p + 4].qqqq.u.B0 % 256) != 0)
                        || ((mem[p + 4].qqqq.u.B1 + (mem[p + 4].qqqq.u.B0 / 256) * 65536L) != 0 /*min_quarterword */ )
                        || ((mem[p + 4].qqqq.u.B2 % 256) != 0)
                        || ((mem[p + 4].qqqq.u.B3 + (mem[p + 4].qqqq.u.B2 / 256) * 65536L) != 0 /*min_quarterword */ )) {
                        print(66255L /*", left-delimiter " */ );
                        print_delimiter(p + 4);
                    }
                    if (((mem[p + 5].qqqq.u.B0 % 256) != 0)
                        || ((mem[p + 5].qqqq.u.B1 + (mem[p + 5].qqqq.u.B0 / 256) * 65536L) != 0 /*min_quarterword */ )
                        || ((mem[p + 5].qqqq.u.B2 % 256) != 0)
                        || ((mem[p + 5].qqqq.u.B3 + (mem[p + 5].qqqq.u.B2 / 256) * 65536L) != 0 /*min_quarterword */ )) {
                        print(66256L /*", right-delimiter " */ );
                        print_delimiter(p + 5);
                    }
                    print_subsidiary_data(p + 2, 92 /*"\" */ );
                    print_subsidiary_data(p + 3, 47 /*"/" */ );
                }
                break;
            default:
                print(65602L /*"Unknown node type!" */ );
                break;
            }
        p = mem[p].hh.v.RH;
    }
}

void zshow_box(halfword p)
{
    show_box_regmem depth_threshold = eqtb[8938765L /*int_base 25 */ ].cint;
    breadth_max = eqtb[8938764L /*int_base 24 */ ].cint /*:244 */ ;
    if (breadth_max <= 0)
        breadth_max = 5;
    if (pool_ptr + depth_threshold >= pool_size)
        depth_threshold = pool_size - pool_ptr - 1;
    show_node_list(p);
    print_ln();
}

void zshort_display_n(integer p, integer m)
{
    short_display_n_regmem breadth_max = m;
    depth_threshold = pool_size - pool_ptr - 1;
    show_node_list(p);
}

void zdelete_token_ref(halfword p)
{
    delete_token_ref_regmem if (mem[p].hh.v.LH == -268435455L)
        flush_list(p);
    else
        mem[p].hh.v.LH--;
}

void zdelete_glue_ref(halfword p)
{
    delete_glue_ref_regmem if (mem[p].hh.v.RH == -268435455L)
        free_node(p, 4 /*glue_spec_size */ );
    else
        mem[p].hh.v.RH--;
}

void zflush_node_list(halfword p)
{
    flush_node_list_regmem halfword q;
    while (p != -268435455L) {

        q = mem[p].hh.v.RH;
        if ((p >= hi_mem_min)) {
            mem[p].hh.v.RH = avail;
            avail = p;
        } else {
            switch (mem[p].hh.u.B0) {
            case 0:
            case 1:
            case 13:
                {
                    flush_node_list(mem[p + 5].hh.v.RH);
                    free_node(p, 8 /*box_node_size */ );
                    goto lab30;
                }
                break;
            case 2:
                {
                    free_node(p, 5 /*rule_node_size */ );
                    goto lab30;
                }
                break;
            case 3:
                {
                    flush_node_list(mem[p + 4].hh.v.LH);
                    delete_glue_ref(mem[p + 4].hh.v.RH);
                    free_node(p, 5 /*ins_node_size */ );
                    goto lab30;
                }
                break;
            case 8:
                {
                    switch (mem[p].hh.u.B1) {
                    case 0:
                        free_node(p, 3 /*open_node_size */ );
                        break;
                    case 1:
                    case 3:
                        {
                            delete_token_ref(mem[p + 1].hh.v.RH);
                            free_node(p, 2 /*write_node_size */ );
                            goto lab30;
                        }
                        break;
                    case 2:
                    case 4:
                        free_node(p, 2 /*small_node_size */ );
                        break;
                    case 40:
                    case 41:
                        {
                            {
                                if (mem[p + 5].ptr != NULL) {
                                    free(mem[p + 5].ptr);
                                    mem[p + 5].ptr = NULL;
                                    mem[p + 4].qqqq.u.B3 = 0;
                                }
                            }
                            free_node(p, mem[p + 4].qqqq.u.B0);
                        }
                        break;
                    case 42:
                        free_node(p, 5 /*glyph_node_size */ );
                        break;
                    case 43:
                    case 44:
                        free_node(p,
                                  (9 /*pic_node_size */  +
                                   (mem[p + 4].hh.u.B0 + sizeof(memory_word) - 1) / sizeof(memory_word)));
                        break;
                    case 6:
                        free_node(p, 2 /*small_node_size */ );
                        break;
                    default:
                        confusion(66733L /*"ext3" */ );
                        break;
                    }
                    goto lab30;
                }
                break;
            case 10:
                {
                    {
                        if (mem[mem[p + 1].hh.v.LH].hh.v.RH == -268435455L)
                            free_node(mem[p + 1].hh.v.LH, 4 /*glue_spec_size */ );
                        else
                            mem[mem[p + 1].hh.v.LH].hh.v.RH--;
                    }
                    if (mem[p + 1].hh.v.RH != -268435455L)
                        flush_node_list(mem[p + 1].hh.v.RH);
                    free_node(p, 3 /*medium_node_size */ );
                    goto lab30;
                }
                break;
            case 11:
            case 9:
            case 12:
                {
                    free_node(p, 3 /*medium_node_size */ );
                    goto lab30;
                }
                break;
            case 40:
                {
                    free_node(p, 3 /*margin_kern_node_size */ );
                    goto lab30;
                }
                break;
            case 6:
                flush_node_list(mem[p + 1].hh.v.RH);
                break;
            case 4:
                delete_token_ref(mem[p + 1].hh.v.RH);
                break;
            case 7:
                {
                    flush_node_list(mem[p + 1].hh.v.LH);
                    flush_node_list(mem[p + 1].hh.v.RH);
                }
                break;
            case 5:
                flush_node_list(mem[p + 1].cint);
                break;
            case 14:
                {
                    free_node(p, 3 /*style_node_size */ );
                    goto lab30;
                }
                break;
            case 15:
                {
                    flush_node_list(mem[p + 1].hh.v.LH);
                    flush_node_list(mem[p + 1].hh.v.RH);
                    flush_node_list(mem[p + 2].hh.v.LH);
                    flush_node_list(mem[p + 2].hh.v.RH);
                    free_node(p, 3 /*style_node_size */ );
                    goto lab30;
                }
                break;
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 27:
            case 26:
            case 29:
            case 28:
                {
                    if (mem[p + 1].hh.v.RH >= 2 /*sub_box */ )
                        flush_node_list(mem[p + 1].hh.v.LH);
                    if (mem[p + 2].hh.v.RH >= 2 /*sub_box */ )
                        flush_node_list(mem[p + 2].hh.v.LH);
                    if (mem[p + 3].hh.v.RH >= 2 /*sub_box */ )
                        flush_node_list(mem[p + 3].hh.v.LH);
                    if (mem[p].hh.u.B0 == 24 /*radical_noad */ )
                        free_node(p, 5 /*radical_noad_size */ );
                    else if (mem[p].hh.u.B0 == 28 /*accent_noad */ )
                        free_node(p, 5 /*accent_noad_size */ );
                    else
                        free_node(p, 4 /*noad_size */ );
                    goto lab30;
                }
                break;
            case 30:
            case 31:
                {
                    free_node(p, 4 /*noad_size */ );
                    goto lab30;
                }
                break;
            case 25:
                {
                    flush_node_list(mem[p + 2].hh.v.LH);
                    flush_node_list(mem[p + 3].hh.v.LH);
                    free_node(p, 6 /*fraction_noad_size */ );
                    goto lab30;
                }
                break;
            default:
                confusion(65640L /*"flushing" */ );
                break;
            }
            free_node(p, 2 /*small_node_size */ );
 lab30:                        /*done */ ;
        }
        p = q;
    }
}

halfword zcopy_node_list(halfword p)
{
    register halfword Result;
    copy_node_list_regmem halfword h;
    halfword q;
    halfword r;
    unsigned char words;
    h = get_avail();
    q = h;
    while (p != -268435455L) {

        words = 1;
        if ((p >= hi_mem_min))
            r = get_avail();
        else                    /*214: */
            switch (mem[p].hh.u.B0) {
            case 0:
            case 1:
            case 13:
                {
                    r = get_node(8 /*box_node_size */ );
                    mem[r + 7].hh.v.LH = mem[p + 7].hh.v.LH;
                    mem[r + 7].hh.v.RH = mem[p + 7].hh.v.RH;
                    mem[r + 6] = mem[p + 6];
                    mem[r + 5] = mem[p + 5];
                    mem[r + 5].hh.v.RH = copy_node_list(mem[p + 5].hh.v.RH);
                    words = 5;
                }
                break;
            case 2:
                {
                    r = get_node(5 /*rule_node_size */ );
                    words = 4 /*rule_node_size -1 */ ;
                }
                break;
            case 3:
                {
                    r = get_node(5 /*ins_node_size */ );
                    mem[r + 4] = mem[p + 4];
                    mem[mem[p + 4].hh.v.RH].hh.v.RH++;
                    mem[r + 4].hh.v.LH = copy_node_list(mem[p + 4].hh.v.LH);
                    words = 4 /*ins_node_size -1 */ ;
                }
                break;
            case 8:
                switch (mem[p].hh.u.B1) {
                case 0:
                    {
                        r = get_node(3 /*open_node_size */ );
                        words = 3 /*open_node_size */ ;
                    }
                    break;
                case 1:
                case 3:
                    {
                        r = get_node(2 /*write_node_size */ );
                        mem[mem[p + 1].hh.v.RH].hh.v.LH++;
                        words = 2 /*write_node_size */ ;
                    }
                    break;
                case 2:
                case 4:
                    {
                        r = get_node(2 /*small_node_size */ );
                        words = 2 /*small_node_size */ ;
                    }
                    break;
                case 40:
                case 41:
                    {
                        words = mem[p + 4].qqqq.u.B0;
                        r = get_node(words);
                        while (words > 0) {

                            words--;
                            mem[r + words] = mem[p + words];
                        }
                        mem[r + 5].ptr = NULL;
                        mem[r + 4].qqqq.u.B3 = 0;
                        copy_native_glyph_info(p, r);
                    }
                    break;
                case 42:
                    {
                        r = get_node(5 /*glyph_node_size */ );
                        words = 5 /*glyph_node_size */ ;
                    }
                    break;
                case 43:
                case 44:
                    {
                        words =
                            (9 /*pic_node_size */  +
                             (mem[p + 4].hh.u.B0 + sizeof(memory_word) - 1) / sizeof(memory_word));
                        r = get_node(words);
                    }
                    break;
                case 6:
                    r = get_node(2 /*small_node_size */ );
                    break;
                default:
                    confusion(66732L /*"ext2" */ );
                    break;
                }
                break;
            case 10:
                {
                    r = get_node(3 /*medium_node_size */ );
                    mem[mem[p + 1].hh.v.LH].hh.v.RH++;
                    mem[r + 2].hh.v.LH = mem[p + 2].hh.v.LH;
                    mem[r + 2].hh.v.RH = mem[p + 2].hh.v.RH;
                    mem[r + 1].hh.v.LH = mem[p + 1].hh.v.LH;
                    mem[r + 1].hh.v.RH = copy_node_list(mem[p + 1].hh.v.RH);
                }
                break;
            case 11:
            case 9:
            case 12:
                {
                    r = get_node(3 /*medium_node_size */ );
                    words = 3 /*medium_node_size */ ;
                }
                break;
            case 40:
                {
                    r = get_node(3 /*margin_kern_node_size */ );
                    words = 3 /*margin_kern_node_size */ ;
                }
                break;
            case 6:
                {
                    r = get_node(2 /*small_node_size */ );
                    mem[r + 1] = mem[p + 1];
                    mem[r + 1].hh.v.RH = copy_node_list(mem[p + 1].hh.v.RH);
                }
                break;
            case 7:
                {
                    r = get_node(2 /*small_node_size */ );
                    mem[r + 1].hh.v.LH = copy_node_list(mem[p + 1].hh.v.LH);
                    mem[r + 1].hh.v.RH = copy_node_list(mem[p + 1].hh.v.RH);
                }
                break;
            case 4:
                {
                    r = get_node(2 /*small_node_size */ );
                    mem[mem[p + 1].hh.v.RH].hh.v.LH++;
                    words = 2 /*small_node_size */ ;
                }
                break;
            case 5:
                {
                    r = get_node(2 /*small_node_size */ );
                    mem[r + 1].cint = copy_node_list(mem[p + 1].cint);
                }
                break;
            default:
                confusion(65641L /*"copying" */ );
                break;
            }
        while (words > 0) {

            words--;
            mem[r + words] = mem[p + words];
        }
        mem[q].hh.v.RH = r;
        q = r;
        p = mem[p].hh.v.RH;
    }
    mem[q].hh.v.RH = -268435455L;
    q = mem[h].hh.v.RH;
    {
        mem[h].hh.v.RH = avail;
        avail = h;
    }
    Result = q;
    return Result;
}

void zprint_mode(integer m)
{
    print_mode_regmem if (m > 0)
        switch (m / (103 /*max_command 1 */ )) {
        case 0:
            print(65642L /*"vertical mode" */ );
            break;
        case 1:
            print(65643L /*"horizontal mode" */ );
            break;
        case 2:
            print(65644L /*"display math mode" */ );
            break;
    } else if (m == 0)
        print(65645L /*"no mode" */ );
    else
        switch ((-(integer) m) / (103 /*max_command 1 */ )) {
        case 0:
            print(65646L /*"internal vertical mode" */ );
            break;
        case 1:
            print(65647L /*"restricted horizontal mode" */ );
            break;
        case 2:
            print(65648L /*"math mode" */ );
            break;
        }
}

void zprint_in_mode(integer m)
{
    print_in_mode_regmem if (m > 0)
        switch (m / (103 /*max_command 1 */ )) {
        case 0:
            print(65649L /*"' in vertical mode" */ );
            break;
        case 1:
            print(65650L /*"' in horizontal mode" */ );
            break;
        case 2:
            print(65651L /*"' in display math mode" */ );
            break;
    } else if (m == 0)
        print(65652L /*"' in no mode" */ );
    else
        switch ((-(integer) m) / (103 /*max_command 1 */ )) {
        case 0:
            print(65653L /*"' in internal vertical mode" */ );
            break;
        case 1:
            print(65654L /*"' in restricted horizontal mode" */ );
            break;
        case 2:
            print(65655L /*"' in math mode" */ );
            break;
        }
}

void push_nest(void)
{
    push_nest_regmem if (nest_ptr > max_nest_stack) {
        max_nest_stack = nest_ptr;
        if (nest_ptr == nest_size)
            overflow(65656L /*"semantic nest size" */ , nest_size);
    }
    nest[nest_ptr] = cur_list;
    nest_ptr++;
    cur_list.head_field = get_avail();
    cur_list.tail_field = cur_list.head_field;
    cur_list.pg_field = 0;
    cur_list.ml_field = line;
    cur_list.eTeX_aux_field = -268435455L;
}

void pop_nest(void)
{
    pop_nest_regmem {
        mem[cur_list.head_field].hh.v.RH = avail;
        avail = cur_list.head_field;
    }
    nest_ptr--;
    cur_list = nest[nest_ptr];
}

void show_activities(void)
{
    show_activities_regmem integer p;
    short /*mmode */ m;
    memory_word a;
    halfword q, r;
    integer t;
    nest[nest_ptr] = cur_list;
    print_nl(65622L /*"" */ );
    print_ln();
    {
        register integer for_end;
        p = nest_ptr;
        for_end = 0;
        if (p >= for_end)
            do {
                m = nest[p].mode_field;
                a = nest[p].aux_field;
                print_nl(65657L /*"### " */ );
                print_mode(m);
                print(65658L /*" entered at line " */ );
                print_int(abs(nest[p].ml_field));
                if (m == 104 /*hmode */ ) {

                    if (nest[p].pg_field != 8585216L) {
                        print(65659L /*" (language" */ );
                        print_int(nest[p].pg_field % 65536L);
                        print(65660L /*":hyphenmin" */ );
                        print_int(nest[p].pg_field / 4194304L);
                        print_char(44 /*"," */ );
                        print_int((nest[p].pg_field / 65536L) % 64);
                        print_char(41 /*")" */ );
                    }
                }
                if (nest[p].ml_field < 0)
                    print(65661L /*" (\output routine)" */ );
                if (p == 0) {
                    if (mem_top - 2 != page_tail) {
                        print_nl(66378L /*"### current page:" */ );
                        if (output_active)
                            print(66379L /*" (held over for next output)" */ );
                        show_box(mem[mem_top - 2].hh.v.RH);
                        if (page_contents > 0 /*empty */ ) {
                            print_nl(66380L /*"total height " */ );
                            print_totals();
                            print_nl(66381L /*" goal height " */ );
                            print_scaled(page_so_far[0]);
                            r = mem[mem_top].hh.v.RH;
                            while (r != mem_top) {

                                print_ln();
                                print_esc(65614L /*"insert" */ );
                                t = mem[r].hh.u.B1;
                                print_int(t);
                                print(66382L /*" adds " */ );
                                if (eqtb[8938824L /*count_base */  + t].cint == 1000)
                                    t = mem[r + 3].cint;
                                else
                                    t = x_over_n(mem[r + 3].cint, 1000) * eqtb[8938824L /*count_base */  + t].cint;
                                print_scaled(t);
                                if (mem[r].hh.u.B0 == 1 /*split_up */ ) {
                                    q = mem_top - 2;
                                    t = 0;
                                    do {
                                        q = mem[q].hh.v.RH;
                                        if ((mem[q].hh.u.B0 == 3 /*ins_node */ ) && (mem[q].hh.u.B1 == mem[r].hh.u.B1))
                                            t++;
                                    } while (!(q == mem[r + 1].hh.v.LH));
                                    print(66383L /*", #" */ );
                                    print_int(t);
                                    print(66384L /*" might split" */ );
                                }
                                r = mem[r].hh.v.RH;
                            }
                        }
                    }
                    if (mem[mem_top - 1].hh.v.RH != -268435455L)
                        print_nl(65662L /*"### recent contributions:" */ );
                }
                show_box(mem[nest[p].head_field].hh.v.RH);
                switch (abs(m) / (103 /*max_command 1 */ )) {
                case 0:
                    {
                        print_nl(65663L /*"prevdepth " */ );
                        if (a.cint <= -65536000L)
                            print(65664L /*"ignored" */ );
                        else
                            print_scaled(a.cint);
                        if (nest[p].pg_field != 0) {
                            print(65665L /*", prevgraf " */ );
                            print_int(nest[p].pg_field);
                            if (nest[p].pg_field != 1)
                                print(65666L /*" lines" */ );
                            else
                                print(65667L /*" line" */ );
                        }
                    }
                    break;
                case 1:
                    {
                        print_nl(65668L /*"spacefactor " */ );
                        print_int(a.hh.v.LH);
                        if (m > 0) {

                            if (a.hh.v.RH > 0) {
                                print(65669L /*", current language " */ );
                                print_int(a.hh.v.RH);
                            }
                        }
                    }
                    break;
                case 2:
                    if (a.cint != -268435455L) {
                        print(65670L /*"this will be denominator of:" */ );
                        show_box(a.cint);
                    }
                    break;
                }
            }
            while (p-- > for_end);
    }
}

void zprint_param(integer n)
{
    print_param_regmem switch (n) {
    case 0:
        print_esc(65716L /*"pretolerance" */ );
        break;
    case 1:
        print_esc(65717L /*"tolerance" */ );
        break;
    case 2:
        print_esc(65718L /*"linepenalty" */ );
        break;
    case 3:
        print_esc(65719L /*"hyphenpenalty" */ );
        break;
    case 4:
        print_esc(65720L /*"exhyphenpenalty" */ );
        break;
    case 5:
        print_esc(65721L /*"clubpenalty" */ );
        break;
    case 6:
        print_esc(65722L /*"widowpenalty" */ );
        break;
    case 7:
        print_esc(65723L /*"displaywidowpenalty" */ );
        break;
    case 8:
        print_esc(65724L /*"brokenpenalty" */ );
        break;
    case 9:
        print_esc(65725L /*"binoppenalty" */ );
        break;
    case 10:
        print_esc(65726L /*"relpenalty" */ );
        break;
    case 11:
        print_esc(65727L /*"predisplaypenalty" */ );
        break;
    case 12:
        print_esc(65728L /*"postdisplaypenalty" */ );
        break;
    case 13:
        print_esc(65729L /*"interlinepenalty" */ );
        break;
    case 14:
        print_esc(65730L /*"doublehyphendemerits" */ );
        break;
    case 15:
        print_esc(65731L /*"finalhyphendemerits" */ );
        break;
    case 16:
        print_esc(65732L /*"adjdemerits" */ );
        break;
    case 17:
        print_esc(65733L /*"mag" */ );
        break;
    case 18:
        print_esc(65734L /*"delimiterfactor" */ );
        break;
    case 19:
        print_esc(65735L /*"looseness" */ );
        break;
    case 20:
        print_esc(65736L /*"time" */ );
        break;
    case 21:
        print_esc(65737L /*"day" */ );
        break;
    case 22:
        print_esc(65738L /*"month" */ );
        break;
    case 23:
        print_esc(65739L /*"year" */ );
        break;
    case 24:
        print_esc(65740L /*"showboxbreadth" */ );
        break;
    case 25:
        print_esc(65741L /*"showboxdepth" */ );
        break;
    case 26:
        print_esc(65742L /*"hbadness" */ );
        break;
    case 27:
        print_esc(65743L /*"vbadness" */ );
        break;
    case 28:
        print_esc(65744L /*"pausing" */ );
        break;
    case 29:
        print_esc(65745L /*"tracingonline" */ );
        break;
    case 30:
        print_esc(65746L /*"tracingmacros" */ );
        break;
    case 31:
        print_esc(65747L /*"tracingstats" */ );
        break;
    case 32:
        print_esc(65748L /*"tracingparagraphs" */ );
        break;
    case 33:
        print_esc(65749L /*"tracingpages" */ );
        break;
    case 34:
        print_esc(65750L /*"tracingoutput" */ );
        break;
    case 35:
        print_esc(65751L /*"tracinglostchars" */ );
        break;
    case 36:
        print_esc(65752L /*"tracingcommands" */ );
        break;
    case 37:
        print_esc(65753L /*"tracingrestores" */ );
        break;
    case 38:
        print_esc(65754L /*"uchyph" */ );
        break;
    case 39:
        print_esc(65755L /*"outputpenalty" */ );
        break;
    case 40:
        print_esc(65756L /*"maxdeadcycles" */ );
        break;
    case 41:
        print_esc(65757L /*"hangafter" */ );
        break;
    case 42:
        print_esc(65758L /*"floatingpenalty" */ );
        break;
    case 43:
        print_esc(65759L /*"globaldefs" */ );
        break;
    case 44:
        print_esc(65760L /*"fam" */ );
        break;
    case 45:
        print_esc(65761L /*"escapechar" */ );
        break;
    case 46:
        print_esc(65762L /*"defaulthyphenchar" */ );
        break;
    case 47:
        print_esc(65763L /*"defaultskewchar" */ );
        break;
    case 48:
        print_esc(65764L /*"endlinechar" */ );
        break;
    case 49:
        print_esc(65765L /*"newlinechar" */ );
        break;
    case 50:
        print_esc(65766L /*"language" */ );
        break;
    case 51:
        print_esc(65767L /*"lefthyphenmin" */ );
        break;
    case 52:
        print_esc(65768L /*"righthyphenmin" */ );
        break;
    case 53:
        print_esc(65769L /*"holdinginserts" */ );
        break;
    case 54:
        print_esc(65770L /*"errorcontextlines" */ );
        break;
    case 55:
        print_esc(65771L /*"charsubdefmin" */ );
        break;
    case 56:
        print_esc(65772L /*"charsubdefmax" */ );
        break;
    case 57:
        print_esc(65773L /*"tracingcharsubdef" */ );
        break;
    case 69:
        print_esc(65774L /*"XeTeXlinebreakpenalty" */ );
        break;
    case 70:
        print_esc(65775L /*"XeTeXprotrudechars" */ );
        break;
    case 83:
        print_esc(66945L /*"synctex" */ );
        break;
    case 58:
        print_esc(66831L /*"tracingassigns" */ );
        break;
    case 59:
        print_esc(66832L /*"tracinggroups" */ );
        break;
    case 60:
        print_esc(66833L /*"tracingifs" */ );
        break;
    case 61:
        print_esc(66834L /*"tracingscantokens" */ );
        break;
    case 62:
        print_esc(66835L /*"tracingnesting" */ );
        break;
    case 63:
        print_esc(66836L /*"predisplaydirection" */ );
        break;
    case 64:
        print_esc(66837L /*"lastlinefit" */ );
        break;
    case 65:
        print_esc(66838L /*"savingvdiscards" */ );
        break;
    case 66:
        print_esc(66839L /*"savinghyphcodes" */ );
        break;
    case 67:
        print_esc(66878L /*"suppressfontnotfounderror" */ );
        break;
    case 71:
        print_esc(66879L /*"TeXXeTstate" */ );
        break;
    case 73:
        print_esc(66880L /*"XeTeXupwardsmode" */ );
        break;
    case 74:
        print_esc(66881L /*"XeTeXuseglyphmetrics" */ );
        break;
    case 75:
        print_esc(66882L /*"XeTeXinterchartokenstate" */ );
        break;
    case 72:
        print_esc(66883L /*"XeTeXdashbreakstate" */ );
        break;
    case 76:
        print_esc(66884L /*"XeTeXinputnormalization" */ );
        break;
    case 79:
        print_esc(66885L /*"XeTeXtracingfonts" */ );
        break;
    case 80:
        print_esc(66886L /*"XeTeXinterwordspaceshaping" */ );
        break;
    case 81:
        print_esc(66887L /*"XeTeXgenerateactualtext" */ );
        break;
    case 82:
        print_esc(66888L /*"XeTeXhyphenatablelength" */ );
        break;
    default:
        print(65776L /*"[unknown integer parameter!]" */ );
        break;
    }
}

void begin_diagnostic(void)
{
    begin_diagnostic_regmem old_setting = selector;
    if ((eqtb[8938769L /*int_base 29 */ ].cint <= 0) && (selector == SELECTOR_TERM_AND_LOG)) {
        selector--;
        if (history == HISTORY_SPOTLESS)
            history = HISTORY_WARNING_ISSUED;
    }
}

void zend_diagnostic(boolean blank_line)
{
    end_diagnostic_regmem print_nl(65622L /*"" */ );
    if (blank_line)
        print_ln();
    selector = old_setting;
}

void zprint_length_param(integer n)
{
    print_length_param_regmem switch (n) {
    case 0:
        print_esc(65779L /*"parindent" */ );
        break;
    case 1:
        print_esc(65780L /*"mathsurround" */ );
        break;
    case 2:
        print_esc(65781L /*"lineskiplimit" */ );
        break;
    case 3:
        print_esc(65782L /*"hsize" */ );
        break;
    case 4:
        print_esc(65783L /*"vsize" */ );
        break;
    case 5:
        print_esc(65784L /*"maxdepth" */ );
        break;
    case 6:
        print_esc(65785L /*"splitmaxdepth" */ );
        break;
    case 7:
        print_esc(65786L /*"boxmaxdepth" */ );
        break;
    case 8:
        print_esc(65787L /*"hfuzz" */ );
        break;
    case 9:
        print_esc(65788L /*"vfuzz" */ );
        break;
    case 10:
        print_esc(65789L /*"delimitershortfall" */ );
        break;
    case 11:
        print_esc(65790L /*"nulldelimiterspace" */ );
        break;
    case 12:
        print_esc(65791L /*"scriptspace" */ );
        break;
    case 13:
        print_esc(65792L /*"predisplaysize" */ );
        break;
    case 14:
        print_esc(65793L /*"displaywidth" */ );
        break;
    case 15:
        print_esc(65794L /*"displayindent" */ );
        break;
    case 16:
        print_esc(65795L /*"overfullrule" */ );
        break;
    case 17:
        print_esc(65796L /*"hangindent" */ );
        break;
    case 18:
        print_esc(65797L /*"hoffset" */ );
        break;
    case 19:
        print_esc(65798L /*"voffset" */ );
        break;
    case 20:
        print_esc(65799L /*"emergencystretch" */ );
        break;
    case 21:
        print_esc(65800L /*"pdfpagewidth" */ );
        break;
    case 22:
        print_esc(65801L /*"pdfpageheight" */ );
        break;
    default:
        print(65802L /*"[unknown dimen parameter!]" */ );
        break;
    }
}

void zprint_cmd_chr(quarterword cmd, halfword chr_code)
{
    print_cmd_chr_regmem integer n;
    str_number font_name_str;
    UTF16_code quote_char;
    switch (cmd) {
    case 1:
        {
            print(65876L /*"begin-group character " */ );
            if (chr_code < 65536L)
                print(chr_code);
            else
                print_char(chr_code);
        }
        break;
    case 2:
        {
            print(65877L /*"end-group character " */ );
            if (chr_code < 65536L)
                print(chr_code);
            else
                print_char(chr_code);
        }
        break;
    case 3:
        {
            print(65878L /*"math shift character " */ );
            if (chr_code < 65536L)
                print(chr_code);
            else
                print_char(chr_code);
        }
        break;
    case 6:
        {
            print(65879L /*"macro parameter character " */ );
            if (chr_code < 65536L)
                print(chr_code);
            else
                print_char(chr_code);
        }
        break;
    case 7:
        {
            print(65880L /*"superscript character " */ );
            if (chr_code < 65536L)
                print(chr_code);
            else
                print_char(chr_code);
        }
        break;
    case 8:
        {
            print(65881L /*"subscript character " */ );
            if (chr_code < 65536L)
                print(chr_code);
            else
                print_char(chr_code);
        }
        break;
    case 9:
        print(65882L /*"end of alignment template" */ );
        break;
    case 10:
        {
            print(65883L /*"blank space " */ );
            if (chr_code < 65536L)
                print(chr_code);
            else
                print_char(chr_code);
        }
        break;
    case 11:
        {
            print(65884L /*"the letter " */ );
            if (chr_code < 65536L)
                print(chr_code);
            else
                print_char(chr_code);
        }
        break;
    case 12:
        {
            print(65885L /*"the character " */ );
            if (chr_code < 65536L)
                print(chr_code);
            else
                print_char(chr_code);
        }
        break;
    case 76:
    case 77:
        if (chr_code < 2252259L /*skip_base */ )
            print_skip_param(chr_code - 2252240L);
        else if (chr_code < 2252515L /*mu_skip_base */ ) {
            print_esc(65691L /*"skip" */ );
            print_int(chr_code - 2252259L);
        } else {

            print_esc(65692L /*"muskip" */ );
            print_int(chr_code - 2252515L);
        }
        break;
    case 73:
        if (chr_code >= 2252783L /*toks_base */ ) {
            print_esc(65703L /*"toks" */ );
            print_int(chr_code - 2252783L);
        } else
            switch (chr_code) {
            case 2252772:
                print_esc(65694L /*"output" */ );
                break;
            case 2252773:
                print_esc(65695L /*"everypar" */ );
                break;
            case 2252774:
                print_esc(65696L /*"everymath" */ );
                break;
            case 2252775:
                print_esc(65697L /*"everydisplay" */ );
                break;
            case 2252776:
                print_esc(65698L /*"everyhbox" */ );
                break;
            case 2252777:
                print_esc(65699L /*"everyvbox" */ );
                break;
            case 2252778:
                print_esc(65700L /*"everyjob" */ );
                break;
            case 2252779:
                print_esc(65701L /*"everycr" */ );
                break;
            case 2252781:
                print_esc(66830L /*"everyeof" */ );
                break;
            case 2252782:
                print_esc(66720L /*"XeTeXinterchartoks" */ );
                break;
            default:
                print_esc(65702L /*"errhelp" */ );
                break;
            }
        break;
    case 74:
        if (chr_code < 8938824L /*count_base */ )
            print_param(chr_code - 8938740L);
        else {

            print_esc(65777L /*"count" */ );
            print_int(chr_code - 8938824L);
        }
        break;
    case 75:
        if (chr_code < 10053215L /*scaled_base */ )
            print_length_param(chr_code - 10053192L);
        else {

            print_esc(65803L /*"dimen" */ );
            print_int(chr_code - 10053215L);
        }
        break;
    case 45:
        print_esc(65813L /*"accent" */ );
        break;
    case 92:
        print_esc(65814L /*"advance" */ );
        break;
    case 40:
        print_esc(65815L /*"afterassignment" */ );
        break;
    case 41:
        print_esc(65816L /*"aftergroup" */ );
        break;
    case 78:
        print_esc(65826L /*"fontdimen" */ );
        break;
    case 61:
        print_esc(65817L /*"begingroup" */ );
        break;
    case 42:
        print_esc(65845L /*"penalty" */ );
        break;
    case 16:
        print_esc(65818L /*"char" */ );
        break;
    case 109:
        print_esc(65809L /*"csname" */ );
        break;
    case 90:
        print_esc(65825L /*"font" */ );
        break;
    case 15:
        if (chr_code == 1)
            print_esc(65821L /*"Udelimiter" */ );
        else
            print_esc(65819L /*"delimiter" */ );
        break;
    case 94:
        print_esc(65822L /*"divide" */ );
        break;
    case 67:
        print_esc(65810L /*"endcsname" */ );
        break;
    case 62:
        print_esc(65823L /*"endgroup" */ );
        break;
    case 64:
        print_esc(32 /*" " */ );
        break;
    case 104:
        if (chr_code == 0)
            print_esc(65824L /*"expandafter" */ );
        else
            print_esc(66130L /*"unless" */ );
        break;
    case 32:
        print_esc(65827L /*"halign" */ );
        break;
    case 36:
        print_esc(65828L /*"hrule" */ );
        break;
    case 39:
        if (chr_code == 0)
            print_esc(65829L /*"ignorespaces" */ );
        else
            print_esc(65806L /*"primitive" */ );
        break;
    case 37:
        print_esc(65614L /*"insert" */ );
        break;
    case 44:
        print_esc(47 /*"/" */ );
        break;
    case 18:
        {
            print_esc(65637L /*"mark" */ );
            if (chr_code > 0)
                print_char(115 /*"s" */ );
        }
        break;
    case 46:
        if (chr_code == 1)
            print_esc(65832L /*"Umathaccent" */ );
        else
            print_esc(65830L /*"mathaccent" */ );
        break;
    case 17:
        if (chr_code == 2)
            print_esc(65837L /*"Umathchar" */ );
        else if (chr_code == 1)
            print_esc(65835L /*"Umathcharnum" */ );
        else
            print_esc(65833L /*"mathchar" */ );
        break;
    case 54:
        print_esc(65838L /*"mathchoice" */ );
        break;
    case 93:
        print_esc(65839L /*"multiply" */ );
        break;
    case 34:
        print_esc(65840L /*"noalign" */ );
        break;
    case 65:
        print_esc(65841L /*"noboundary" */ );
        break;
    case 105:
        if (chr_code == 0)
            print_esc(65842L /*"noexpand" */ );
        else
            print_esc(65806L /*"primitive" */ );
        break;
    case 55:
        print_esc(65619L /*"nonscript" */ );
        break;
    case 63:
        print_esc(65843L /*"omit" */ );
        break;
    case 66:
        if (chr_code == 1)
            print_esc(65849L /*"Uradical" */ );
        else
            print_esc(65847L /*"radical" */ );
        break;
    case 98:
        if (chr_code == 0)
            print_esc(65850L /*"read" */ );
        else
            print_esc(66903L /*"readline" */ );
        break;
    case 0:
        print_esc(65851L /*"relax" */ );
        break;
    case 100:
        print_esc(65852L /*"setbox" */ );
        break;
    case 81:
        print_esc(65846L /*"prevgraf" */ );
        break;
    case 85:
        switch (chr_code) {
        case 2252771:
            print_esc(65844L /*"parshape" */ );
            break;
        case 2253039:
            print_esc(66938L /*"interlinepenalties" */ );
            break;
        case 2253040:
            print_esc(66939L /*"clubpenalties" */ );
            break;
        case 2253041:
            print_esc(66940L /*"widowpenalties" */ );
            break;
        case 2253042:
            print_esc(66941L /*"displaywidowpenalties" */ );
            break;
        }
        break;
    case 111:
        if (chr_code == 0)
            print_esc(65853L /*"the" */ );
        else if (chr_code == 1)
            print_esc(66868L /*"unexpanded" */ );
        else
            print_esc(66869L /*"detokenize" */ );
        break;
    case 72:
        {
            print_esc(65703L /*"toks" */ );
            if (chr_code != mem_bot)
                print_sa_num(chr_code);
        }
        break;
    case 38:
        print_esc(65638L /*"vadjust" */ );
        break;
    case 33:
        if (chr_code == 0)
            print_esc(65854L /*"valign" */ );
        else
            switch (chr_code) {
            case 6:
                print_esc(66889L /*"beginL" */ );
                break;
            case 7:
                print_esc(66890L /*"endL" */ );
                break;
            case 10:
                print_esc(66891L /*"beginR" */ );
                break;
            default:
                print_esc(66892L /*"endR" */ );
                break;
            }
        break;
    case 56:
        print_esc(65855L /*"vcenter" */ );
        break;
    case 35:
        print_esc(65856L /*"vrule" */ );
        break;
    case 13:
        print_esc(65917L /*"par" */ );
        break;
    case 106:
        if (chr_code == 0)
            print_esc(65952L /*"input" */ );
        else if (chr_code == 2)
            print_esc(66901L /*"scantokens" */ );
        else
            print_esc(65953L /*"endinput" */ );
        break;
    case 112:
        {
            switch ((chr_code % 5)) {
            case 1:
                print_esc(65955L /*"firstmark" */ );
                break;
            case 2:
                print_esc(65956L /*"botmark" */ );
                break;
            case 3:
                print_esc(65957L /*"splitfirstmark" */ );
                break;
            case 4:
                print_esc(65958L /*"splitbotmark" */ );
                break;
            default:
                print_esc(65954L /*"topmark" */ );
                break;
            }
            if (chr_code >= 5)
                print_char(115 /*"s" */ );
        }
        break;
    case 91:
        {
            if ((chr_code < mem_bot) || (chr_code > mem_bot + 19))
                cmd = (mem[chr_code].hh.u.B0 / 64);
            else {

                cmd = chr_code - mem_bot;
                chr_code = -268435455L;
            }
            if (cmd == 0 /*int_val */ )
                print_esc(65777L /*"count" */ );
            else if (cmd == 1 /*dimen_val */ )
                print_esc(65803L /*"dimen" */ );
            else if (cmd == 2 /*glue_val */ )
                print_esc(65691L /*"skip" */ );
            else
                print_esc(65692L /*"muskip" */ );
            if (chr_code != -268435455L)
                print_sa_num(chr_code);
        }
        break;
    case 80:
        if (chr_code == 1 /*vmode */ )
            print_esc(66003L /*"prevdepth" */ );
        else
            print_esc(66002L /*"spacefactor" */ );
        break;
    case 83:
        if (chr_code == 0)
            print_esc(66004L /*"deadcycles" */ );
        else if (chr_code == 2)
            print_esc(66874L /*"interactionmode" */ );
        else
            print_esc(66005L /*"insertpenalties" */ );
        break;
    case 84:
        if (chr_code == 1 /*width_offset */ )
            print_esc(66006L /*"wd" */ );
        else if (chr_code == 3 /*height_offset */ )
            print_esc(66007L /*"ht" */ );
        else
            print_esc(66008L /*"dp" */ );
        break;
    case 71:
        switch (chr_code) {
        case 0:
            print_esc(66009L /*"lastpenalty" */ );
            break;
        case 1:
            print_esc(66010L /*"lastkern" */ );
            break;
        case 2:
            print_esc(66011L /*"lastskip" */ );
            break;
        case 4:
            print_esc(66012L /*"inputlineno" */ );
            break;
        case 45:
            print_esc(66014L /*"shellescape" */ );
            break;
        case 3:
            print_esc(66779L /*"lastnodetype" */ );
            break;
        case 6:
            print_esc(66780L /*"eTeXversion" */ );
            break;
        case 14:
            print_esc(66782L /*"XeTeXversion" */ );
            break;
        case 15:
            print_esc(66784L /*"XeTeXcountglyphs" */ );
            break;
        case 16:
            print_esc(66785L /*"XeTeXcountvariations" */ );
            break;
        case 17:
            print_esc(66786L /*"XeTeXvariation" */ );
            break;
        case 18:
            print_esc(66787L /*"XeTeXfindvariationbyname" */ );
            break;
        case 19:
            print_esc(66788L /*"XeTeXvariationmin" */ );
            break;
        case 20:
            print_esc(66789L /*"XeTeXvariationmax" */ );
            break;
        case 21:
            print_esc(66790L /*"XeTeXvariationdefault" */ );
            break;
        case 22:
            print_esc(66791L /*"XeTeXcountfeatures" */ );
            break;
        case 23:
            print_esc(66792L /*"XeTeXfeaturecode" */ );
            break;
        case 24:
            print_esc(66793L /*"XeTeXfindfeaturebyname" */ );
            break;
        case 25:
            print_esc(66794L /*"XeTeXisexclusivefeature" */ );
            break;
        case 26:
            print_esc(66795L /*"XeTeXcountselectors" */ );
            break;
        case 27:
            print_esc(66796L /*"XeTeXselectorcode" */ );
            break;
        case 28:
            print_esc(66797L /*"XeTeXfindselectorbyname" */ );
            break;
        case 29:
            print_esc(66798L /*"XeTeXisdefaultselector" */ );
            break;
        case 30:
            print_esc(66802L /*"XeTeXOTcountscripts" */ );
            break;
        case 31:
            print_esc(66803L /*"XeTeXOTcountlanguages" */ );
            break;
        case 32:
            print_esc(66804L /*"XeTeXOTcountfeatures" */ );
            break;
        case 33:
            print_esc(66805L /*"XeTeXOTscripttag" */ );
            break;
        case 34:
            print_esc(66806L /*"XeTeXOTlanguagetag" */ );
            break;
        case 35:
            print_esc(66807L /*"XeTeXOTfeaturetag" */ );
            break;
        case 36:
            print_esc(66808L /*"XeTeXcharglyph" */ );
            break;
        case 37:
            print_esc(66809L /*"XeTeXglyphindex" */ );
            break;
        case 47:
            print_esc(66810L /*"XeTeXglyphbounds" */ );
            break;
        case 38:
            print_esc(66812L /*"XeTeXfonttype" */ );
            break;
        case 39:
            print_esc(66813L /*"XeTeXfirstfontchar" */ );
            break;
        case 40:
            print_esc(66814L /*"XeTeXlastfontchar" */ );
            break;
        case 41:
            print_esc(66815L /*"pdflastxpos" */ );
            break;
        case 42:
            print_esc(66816L /*"pdflastypos" */ );
            break;
        case 46:
            print_esc(66817L /*"XeTeXpdfpagecount" */ );
            break;
        case 7:
            print_esc(66853L /*"currentgrouplevel" */ );
            break;
        case 8:
            print_esc(66854L /*"currentgrouptype" */ );
            break;
        case 9:
            print_esc(66855L /*"currentiflevel" */ );
            break;
        case 10:
            print_esc(66856L /*"currentiftype" */ );
            break;
        case 11:
            print_esc(66857L /*"currentifbranch" */ );
            break;
        case 48:
            print_esc(66858L /*"fontcharwd" */ );
            break;
        case 49:
            print_esc(66859L /*"fontcharht" */ );
            break;
        case 50:
            print_esc(66860L /*"fontchardp" */ );
            break;
        case 51:
            print_esc(66861L /*"fontcharic" */ );
            break;
        case 52:
            print_esc(66862L /*"parshapelength" */ );
            break;
        case 53:
            print_esc(66863L /*"parshapeindent" */ );
            break;
        case 54:
            print_esc(66864L /*"parshapedimen" */ );
            break;
        case 59:
            print_esc(66913L /*"numexpr" */ );
            break;
        case 60:
            print_esc(66914L /*"dimexpr" */ );
            break;
        case 61:
            print_esc(66915L /*"glueexpr" */ );
            break;
        case 62:
            print_esc(66916L /*"muexpr" */ );
            break;
        case 12:
            print_esc(66920L /*"gluestretchorder" */ );
            break;
        case 13:
            print_esc(66921L /*"glueshrinkorder" */ );
            break;
        case 55:
            print_esc(66922L /*"gluestretch" */ );
            break;
        case 56:
            print_esc(66923L /*"glueshrink" */ );
            break;
        case 57:
            print_esc(66924L /*"mutoglue" */ );
            break;
        case 58:
            print_esc(66925L /*"gluetomu" */ );
            break;
        default:
            print_esc(66013L /*"badness" */ );
            break;
        }
        break;
    case 110:
        switch (chr_code) {
        case 0:
            print_esc(66082L /*"number" */ );
            break;
        case 1:
            print_esc(66083L /*"romannumeral" */ );
            break;
        case 2:
            print_esc(66084L /*"string" */ );
            break;
        case 3:
            print_esc(66085L /*"meaning" */ );
            break;
        case 4:
            print_esc(66086L /*"fontname" */ );
            break;
        case 43:
            print_esc(66092L /*"strcmp" */ );
            break;
        case 44:
            print_esc(66093L /*"mdfivesum" */ );
            break;
        case 11:
            print_esc(66088L /*"leftmarginkern" */ );
            break;
        case 12:
            print_esc(66089L /*"rightmarginkern" */ );
            break;
        case 5:
            print_esc(66781L /*"eTeXrevision" */ );
            break;
        case 6:
            print_esc(66783L /*"XeTeXrevision" */ );
            break;
        case 7:
            print_esc(66799L /*"XeTeXvariationname" */ );
            break;
        case 8:
            print_esc(66800L /*"XeTeXfeaturename" */ );
            break;
        case 9:
            print_esc(66801L /*"XeTeXselectorname" */ );
            break;
        case 10:
            print_esc(66811L /*"XeTeXglyphname" */ );
            break;
        case 13:
            print_esc(66090L /*"Uchar" */ );
            break;
        case 14:
            print_esc(66091L /*"Ucharcat" */ );
            break;
        default:
            print_esc(66087L /*"jobname" */ );
            break;
        }
        break;
    case 107:
        {
            if (chr_code >= 32 /*unless_code */ )
                print_esc(66130L /*"unless" */ );
            switch (chr_code % 32 /*unless_code */ ) {
            case 1:
                print_esc(66113L /*"ifcat" */ );
                break;
            case 2:
                print_esc(66114L /*"ifnum" */ );
                break;
            case 3:
                print_esc(66115L /*"ifdim" */ );
                break;
            case 4:
                print_esc(66116L /*"ifodd" */ );
                break;
            case 5:
                print_esc(66117L /*"ifvmode" */ );
                break;
            case 6:
                print_esc(66118L /*"ifhmode" */ );
                break;
            case 7:
                print_esc(66119L /*"ifmmode" */ );
                break;
            case 8:
                print_esc(66120L /*"ifinner" */ );
                break;
            case 9:
                print_esc(66121L /*"ifvoid" */ );
                break;
            case 10:
                print_esc(66122L /*"ifhbox" */ );
                break;
            case 11:
                print_esc(66123L /*"ifvbox" */ );
                break;
            case 12:
                print_esc(66124L /*"ifx" */ );
                break;
            case 13:
                print_esc(66125L /*"ifeof" */ );
                break;
            case 14:
                print_esc(66126L /*"iftrue" */ );
                break;
            case 15:
                print_esc(66127L /*"iffalse" */ );
                break;
            case 16:
                print_esc(66128L /*"ifcase" */ );
                break;
            case 21:
                print_esc(66129L /*"ifprimitive" */ );
                break;
            case 17:
                print_esc(66904L /*"ifdefined" */ );
                break;
            case 18:
                print_esc(66905L /*"ifcsname" */ );
                break;
            case 19:
                print_esc(66906L /*"iffontchar" */ );
                break;
            case 20:
                print_esc(66907L /*"ifincsname" */ );
                break;
            default:
                print_esc(66112L /*"if" */ );
                break;
            }
        }
        break;
    case 108:
        if (chr_code == 2 /*fi_code */ )
            print_esc(66131L /*"fi" */ );
        else if (chr_code == 4 /*or_code */ )
            print_esc(66132L /*"or" */ );
        else
            print_esc(66133L /*"else" */ );
        break;
    case 4:
        if (chr_code == 65537L /*span_code */ )
            print_esc(66288L /*"span" */ );
        else {

            print(66292L /*"alignment tab character " */ );
            if (chr_code < 65536L)
                print(chr_code);
            else
                print_char(chr_code);
        }
        break;
    case 5:
        if (chr_code == 65538L /*cr_code */ )
            print_esc(66289L /*"cr" */ );
        else
            print_esc(66290L /*"crcr" */ );
        break;
    case 82:
        switch (chr_code) {
        case 0:
            print_esc(66368L /*"pagegoal" */ );
            break;
        case 1:
            print_esc(66369L /*"pagetotal" */ );
            break;
        case 2:
            print_esc(66370L /*"pagestretch" */ );
            break;
        case 3:
            print_esc(66371L /*"pagefilstretch" */ );
            break;
        case 4:
            print_esc(66372L /*"pagefillstretch" */ );
            break;
        case 5:
            print_esc(66373L /*"pagefilllstretch" */ );
            break;
        case 6:
            print_esc(66374L /*"pageshrink" */ );
            break;
        default:
            print_esc(66375L /*"pagedepth" */ );
            break;
        }
        break;
    case 14:
        if (chr_code == 1)
            print_esc(66422L /*"dump" */ );
        else
            print_esc(65627L /*"end" */ );
        break;
    case 26:
        switch (chr_code) {
        case 4:
            print_esc(66423L /*"hskip" */ );
            break;
        case 0:
            print_esc(66424L /*"hfil" */ );
            break;
        case 1:
            print_esc(66425L /*"hfill" */ );
            break;
        case 2:
            print_esc(66426L /*"hss" */ );
            break;
        default:
            print_esc(66427L /*"hfilneg" */ );
            break;
        }
        break;
    case 27:
        switch (chr_code) {
        case 4:
            print_esc(66428L /*"vskip" */ );
            break;
        case 0:
            print_esc(66429L /*"vfil" */ );
            break;
        case 1:
            print_esc(66430L /*"vfill" */ );
            break;
        case 2:
            print_esc(66431L /*"vss" */ );
            break;
        default:
            print_esc(66432L /*"vfilneg" */ );
            break;
        }
        break;
    case 28:
        print_esc(65620L /*"mskip" */ );
        break;
    case 29:
        print_esc(65599L /*"kern" */ );
        break;
    case 30:
        print_esc(65626L /*"mkern" */ );
        break;
    case 21:
        if (chr_code == 1)
            print_esc(66450L /*"moveleft" */ );
        else
            print_esc(66451L /*"moveright" */ );
        break;
    case 22:
        if (chr_code == 1)
            print_esc(66452L /*"raise" */ );
        else
            print_esc(66453L /*"lower" */ );
        break;
    case 20:
        switch (chr_code) {
        case 0:
            print_esc(65705L /*"box" */ );
            break;
        case 1:
            print_esc(66454L /*"copy" */ );
            break;
        case 2:
            print_esc(66455L /*"lastbox" */ );
            break;
        case 3:
            print_esc(66363L /*"vsplit" */ );
            break;
        case 4:
            print_esc(66456L /*"vtop" */ );
            break;
        case 5:
            print_esc(66365L /*"vbox" */ );
            break;
        default:
            print_esc(66457L /*"hbox" */ );
            break;
        }
        break;
    case 31:
        if (chr_code == 100 /*a_leaders */ )
            print_esc(66459L /*"leaders" */ );
        else if (chr_code == 101 /*c_leaders */ )
            print_esc(66460L /*"cleaders" */ );
        else if (chr_code == 102 /*x_leaders */ )
            print_esc(66461L /*"xleaders" */ );
        else
            print_esc(66458L /*"shipout" */ );
        break;
    case 43:
        if (chr_code == 0)
            print_esc(66478L /*"noindent" */ );
        else
            print_esc(66477L /*"indent" */ );
        break;
    case 25:
        if (chr_code == 10 /*glue_node */ )
            print_esc(66490L /*"unskip" */ );
        else if (chr_code == 11 /*kern_node */ )
            print_esc(66489L /*"unkern" */ );
        else
            print_esc(66488L /*"unpenalty" */ );
        break;
    case 23:
        if (chr_code == 1 /*copy_code */ )
            print_esc(66492L /*"unhcopy" */ );
        else
            print_esc(66491L /*"unhbox" */ );
        break;
    case 24:
        if (chr_code == 1 /*copy_code */ )
            print_esc(66494L /*"unvcopy" */ );
        else if (chr_code == 2 /*last_box_code */ )
            print_esc(66936L /*"pagediscards" */ );
        else if (chr_code == 3 /*vsplit_code */ )
            print_esc(66937L /*"splitdiscards" */ );
/*:
1650*/
        else
            print_esc(66493L /*"unvbox" */ );
        break;
    case 47:
        if (chr_code == 1)
            print_esc(45 /*"-" */ );
        else
            print_esc(65635L /*"discretionary" */ );
        break;
    case 48:
        if (chr_code == 1)
            print_esc(66526L /*"leqno" */ );
        else
            print_esc(66525L /*"eqno" */ );
        break;
    case 50:
        switch (chr_code) {
        case 16:
            print_esc(66238L /*"mathord" */ );
            break;
        case 17:
            print_esc(66239L /*"mathop" */ );
            break;
        case 18:
            print_esc(66240L /*"mathbin" */ );
            break;
        case 19:
            print_esc(66241L /*"mathrel" */ );
            break;
        case 20:
            print_esc(66242L /*"mathopen" */ );
            break;
        case 21:
            print_esc(66243L /*"mathclose" */ );
            break;
        case 22:
            print_esc(66244L /*"mathpunct" */ );
            break;
        case 23:
            print_esc(66245L /*"mathinner" */ );
            break;
        case 26:
            print_esc(66247L /*"underline" */ );
            break;
        default:
            print_esc(66246L /*"overline" */ );
            break;
        }
        break;
    case 51:
        if (chr_code == 1 /*limits */ )
            print_esc(66251L /*"limits" */ );
        else if (chr_code == 2 /*no_limits */ )
            print_esc(66252L /*"nolimits" */ );
        else
            print_esc(66527L /*"displaylimits" */ );
        break;
    case 53:
        print_style(chr_code);
        break;
    case 52:
        switch (chr_code) {
        case 1:
            print_esc(66548L /*"over" */ );
            break;
        case 2:
            print_esc(66549L /*"atop" */ );
            break;
        case 3:
            print_esc(66550L /*"abovewithdelims" */ );
            break;
        case 4:
            print_esc(66551L /*"overwithdelims" */ );
            break;
        case 5:
            print_esc(66552L /*"atopwithdelims" */ );
            break;
        default:
            print_esc(66547L /*"above" */ );
            break;
        }
        break;
    case 49:
        if (chr_code == 30 /*left_noad */ )
            print_esc(66248L /*"left" */ );
        else if (chr_code == 1)
            print_esc(66250L /*"middle" */ );
        else
            print_esc(66249L /*"right" */ );
        break;
    case 95:
        if (chr_code == 1)
            print_esc(66572L /*"long" */ );
        else if (chr_code == 2)
            print_esc(66573L /*"outer" */ );
        else if (chr_code == 8)
            print_esc(66586L /*"protected" */ );
        else
            print_esc(66574L /*"global" */ );
        break;
    case 99:
        if (chr_code == 0)
            print_esc(66575L /*"def" */ );
        else if (chr_code == 1)
            print_esc(66576L /*"gdef" */ );
        else if (chr_code == 2)
            print_esc(66577L /*"edef" */ );
        else
            print_esc(66578L /*"xdef" */ );
        break;
    case 96:
        if (chr_code != 0 /*normal */ )
            print_esc(66596L /*"futurelet" */ );
        else
            print_esc(66595L /*"let" */ );
        break;
    case 97:
        switch (chr_code) {
        case 0:
            print_esc(66597L /*"chardef" */ );
            break;
        case 1:
            print_esc(66598L /*"mathchardef" */ );
            break;
        case 9:
            print_esc(66602L /*"Umathchardef" */ );
            break;
        case 8:
            print_esc(66600L /*"Umathcharnumdef" */ );
            break;
        case 2:
            print_esc(66603L /*"countdef" */ );
            break;
        case 3:
            print_esc(66604L /*"dimendef" */ );
            break;
        case 4:
            print_esc(66605L /*"skipdef" */ );
            break;
        case 5:
            print_esc(66606L /*"muskipdef" */ );
            break;
        case 7:
            print_esc(66608L /*"charsubdef" */ );
            break;
        default:
            print_esc(66607L /*"toksdef" */ );
            break;
        }
        break;
    case 68:
        {
            print_esc(65818L /*"char" */ );
            print_hex(chr_code);
        }
        break;
    case 69:
        {
            print_esc(65833L /*"mathchar" */ );
            print_hex(chr_code);
        }
        break;
    case 70:
        {
            print_esc(65837L /*"Umathchar" */ );
            print_hex(math_class_field(chr_code));
            print_hex(math_fam_field(chr_code));
            print_hex(math_char_field(chr_code));
        }
        break;
    case 86:
        if (chr_code == 2254068L /*cat_code_base */ )
            print_esc(65711L /*"catcode" */ );
        else if (chr_code == 6710516L /*math_code_base */ )
            print_esc(65715L /*"mathcode" */ );
        else if (chr_code == 3368180L /*lc_code_base */ )
            print_esc(65712L /*"lccode" */ );
        else if (chr_code == 4482292L /*uc_code_base */ )
            print_esc(65713L /*"uccode" */ );
        else if (chr_code == 5596404L /*sf_code_base */ )
            print_esc(65714L /*"sfcode" */ );
        else
            print_esc(65778L /*"delcode" */ );
        break;
    case 87:
        if (chr_code == 5596404L /*sf_code_base */ )
            print_esc(66617L /*"XeTeXcharclass" */ );
        else if (chr_code == 6710516L /*math_code_base */ )
            print_esc(66614L /*"Umathcodenum" */ );
        else if (chr_code == 6710517L /*math_code_base 1 */ )
            print_esc(66616L /*"Umathcode" */ );
        else if (chr_code == 8939080L /*del_code_base */ )
            print_esc(66619L /*"Udelcodenum" */ );
        else
            print_esc(66621L /*"Udelcode" */ );
        break;
    case 88:
        print_size(chr_code - 2253300L);
        break;
    case 101:
        if (chr_code == 1)
            print_esc(66351L /*"patterns" */ );
        else
            print_esc(66339L /*"hyphenation" */ );
        break;
    case 79:
        switch (chr_code) {
        case 0:
            print_esc(66636L /*"hyphenchar" */ );
            break;
        case 1:
            print_esc(66637L /*"skewchar" */ );
            break;
        case 2:
            print_esc(66638L /*"lpcode" */ );
            break;
        case 3:
            print_esc(66639L /*"rpcode" */ );
            break;
        }
        break;
    case 89:
        {
            print(66647L /*"select font " */ );
            font_name_str = font_name[chr_code];
            if (((font_area[chr_code] == 65535L /*aat_font_flag */ )
                 || (font_area[chr_code] == 65534L /*otgr_font_flag */ ))) {
                quote_char = 34 /*""" */ ;
                {
                    register integer for_end;
                    n = 0;
                    for_end = length(font_name_str) - 1;
                    if (n <= for_end)
                        do
                            if (str_pool[str_start[(font_name_str) - 65536L] + n] == 34 /*""" */ )
                                quote_char = 39 /*"'" */ ;
                        while (n++ < for_end) ;
                }
                print_char(quote_char);
                print(font_name_str);
                print_char(quote_char);
            } else
                print(font_name_str);
            if (font_size[chr_code] != font_dsize[chr_code]) {
                print(66097L /*" at " */ );
                print_scaled(font_size[chr_code]);
                print(65693L /*"pt" */ );
            }
        }
        break;
    case 102:
        switch (chr_code) {
        case 0:
            print_esc(65554L /*"batchmode" */ );
            break;
        case 1:
            print_esc(65555L /*"nonstopmode" */ );
            break;
        case 2:
            print_esc(65556L /*"scrollmode" */ );
            break;
        default:
            print_esc(66648L /*"errorstopmode" */ );
            break;
        }
        break;
    case 60:
        if (chr_code == 0)
            print_esc(66650L /*"closein" */ );
        else
            print_esc(66649L /*"openin" */ );
        break;
    case 58:
        if (chr_code == 0)
            print_esc(66651L /*"message" */ );
        else
            print_esc(66652L /*"errmessage" */ );
        break;
    case 57:
        if (chr_code == 3368180L /*lc_code_base */ )
            print_esc(66658L /*"lowercase" */ );
        else
            print_esc(66659L /*"uppercase" */ );
        break;
    case 19:
        switch (chr_code) {
        case 1:
            print_esc(66661L /*"showbox" */ );
            break;
        case 2:
            print_esc(66662L /*"showthe" */ );
            break;
        case 3:
            print_esc(66663L /*"showlists" */ );
            break;
        case 4:
            print_esc(66865L /*"showgroups" */ );
            break;
        case 5:
            print_esc(66867L /*"showtokens" */ );
            break;
        case 6:
            print_esc(66870L /*"showifs" */ );
            break;
        default:
            print_esc(66660L /*"show" */ );
            break;
        }
        break;
    case 103:
        print(66670L /*"undefined" */ );
        break;
    case 113:
    case 114:
    case 115:
    case 116:
        {
            n = cmd - 113;
            if (mem[mem[chr_code].hh.v.RH].hh.v.LH == 29360129L /*protected_token */ )
                n = n + 4;
            if (odd(n / 4))
                print_esc(66586L /*"protected" */ );
            if (odd(n))
                print_esc(66572L /*"long" */ );
            if (odd(n / 2))
                print_esc(66573L /*"outer" */ );
            if (n > 0)
                print_char(32 /*" " */ );
            print(66671L /*"macro" */ );
        }
        break;
    case 117:
        print_esc(66672L /*"outer endtemplate" */ );
        break;
    case 59:
        switch (chr_code) {
        case 0:
            print_esc(66711L /*"openout" */ );
            break;
        case 1:
            print_esc(65914L /*"write" */ );
            break;
        case 2:
            print_esc(66712L /*"closeout" */ );
            break;
        case 3:
            print_esc(66713L /*"special" */ );
            break;
        case 4:
            print_esc(66714L /*"immediate" */ );
            break;
        case 5:
            print_esc(66715L /*"setlanguage" */ );
            break;
        case 41:
            print_esc(66716L /*"XeTeXpicfile" */ );
            break;
        case 42:
            print_esc(66717L /*"XeTeXpdffile" */ );
            break;
        case 43:
            print_esc(66718L /*"XeTeXglyph" */ );
            break;
        case 46:
            print_esc(66719L /*"XeTeXlinebreaklocale" */ );
            break;
        case 44:
            print_esc(66722L /*"XeTeXinputencoding" */ );
            break;
        case 45:
            print_esc(66723L /*"XeTeXdefaultencoding" */ );
            break;
        case 6:
            print_esc(66721L /*"pdfsavepos" */ );
            break;
        default:
            print(66724L /*"[unknown extension!]" */ );
            break;
        }
        break;
    default:
        print(65886L /*"[unknown command code!]" */ );
        break;
    }
}

void znot_aat_font_error(integer cmd, integer c, integer f)
{
    not_aat_font_error_regmem {
        if (interaction == 3 /*error_stop_mode */ ) ;
        if (file_line_error_style_p)
            print_file_line();
        else
            print_nl(65544L /*"! " */ );
        print(66818L /*"Cannot use " */ );
    }
    print_cmd_chr(cmd, c);
    print(66819L /*" with " */ );
    print(font_name[f]);
    print(66820L /*"; not an AAT font" */ );
    error();
}

void znot_aat_gr_font_error(integer cmd, integer c, integer f)
{
    not_aat_gr_font_error_regmem {
        if (interaction == 3 /*error_stop_mode */ ) ;
        if (file_line_error_style_p)
            print_file_line();
        else
            print_nl(65544L /*"! " */ );
        print(66818L /*"Cannot use " */ );
    }
    print_cmd_chr(cmd, c);
    print(66819L /*" with " */ );
    print(font_name[f]);
    print(66821L /*"; not an AAT or Graphite font" */ );
    error();
}

void znot_ot_font_error(integer cmd, integer c, integer f)
{
    not_ot_font_error_regmem {
        if (interaction == 3 /*error_stop_mode */ ) ;
        if (file_line_error_style_p)
            print_file_line();
        else
            print_nl(65544L /*"! " */ );
        print(66818L /*"Cannot use " */ );
    }
    print_cmd_chr(cmd, c);
    print(66819L /*" with " */ );
    print(font_name[f]);
    print(66822L /*"; not an OpenType Layout font" */ );
    error();
}

void znot_native_font_error(integer cmd, integer c, integer f)
{
    not_native_font_error_regmem {
        if (interaction == 3 /*error_stop_mode */ ) ;
        if (file_line_error_style_p)
            print_file_line();
        else
            print_nl(65544L /*"! " */ );
        print(66818L /*"Cannot use " */ );
    }
    print_cmd_chr(cmd, c);
    print(66819L /*" with " */ );
    print(font_name[f]);
    print(66823L /*"; not a native platform font" */ );
    error();
}

/*:1434*/

halfword zid_lookup(integer j, integer l)
{
    register halfword Result;
    id_lookup_regmem integer h;
    integer d;
    halfword p;
    halfword k;
    integer ll;
    h = 0;
    {
        register integer for_end;
        k = j;
        for_end = j + l - 1;
        if (k <= for_end)
            do {
                h = h + h + buffer[k];
                while (h >= 8501 /*hash_prime */ )
                    h = h - 8501;
            }
            while (k++ < for_end);
    }
    p = h + 2228226L;
    ll = l;
    {
        register integer for_end;
        d = 0;
        for_end = l - 1;
        if (d <= for_end)
            do
                if (buffer[j + d] >= 65536L)
                    ll++;
            while (d++ < for_end) ;
    }
    while (true) {

        if (hash[p].v.RH > 0) {

            if (length(hash[p].v.RH) == ll) {

                if (str_eq_buf(hash[p].v.RH, j))
                    goto lab40;
            }
        }
        if (hash[p].v.LH == 0) {
            if (no_new_control_sequence)
                p = 2252239L /*undefined_control_sequence */ ;
            else {              /*269: */

                if (hash[p].v.RH > 0) {
                    if (hash_high < hash_extra) {
                        hash_high++;
                        hash[p].v.LH = hash_high + 10053470L;
                        p = hash_high + 10053470L;
                    } else {

                        do {
                            if ((hash_used == 2228226L /*hash_base */ ))
                                overflow(65807L /*"hash size" */ , 15000 /*hash_size */  + hash_extra);
                            hash_used--;
                        } while (!(hash[hash_used].v.RH == 0));
                        hash[p].v.LH = hash_used;
                        p = hash_used;
                    }
                }
                {
                    if (pool_ptr + ll > pool_size)
                        overflow(65539L /*"pool size" */ , pool_size - init_pool_ptr);
                }
                d = (pool_ptr - str_start[(str_ptr) - 65536L]);
                while (pool_ptr > str_start[(str_ptr) - 65536L]) {

                    pool_ptr--;
                    str_pool[pool_ptr + l] = str_pool[pool_ptr];
                }
                {
                    register integer for_end;
                    k = j;
                    for_end = j + l - 1;
                    if (k <= for_end)
                        do {
                            if (buffer[k] < 65536L) {
                                str_pool[pool_ptr] = buffer[k];
                                pool_ptr++;
                            } else {

                                {
                                    str_pool[pool_ptr] = 55296L + (buffer[k] - 65536L) / 1024;
                                    pool_ptr++;
                                }
                                {
                                    str_pool[pool_ptr] = 56320L + (buffer[k] - 65536L) % 1024;
                                    pool_ptr++;
                                }
                            }
                        }
                        while (k++ < for_end);
                }
                hash[p].v.RH = make_string();
                pool_ptr = pool_ptr + d;
            }
            goto lab40;
        }
        p = hash[p].v.LH;
    }
 lab40:                        /*found */ Result = p;
    return Result;
}

halfword zprim_lookup(str_number s)
{
    register halfword Result;
    prim_lookup_regmem integer h;
    halfword p;
    halfword k;
    integer j, l;
    if (s < 256) {
        p = s;
        if ((p < 0) || (prim_eqtb[p].hh.u.B1 != 1 /*level_one */ ))
            p = 0 /*undefined_primitive */ ;
    } else {

        j = str_start[(s) - 65536L];
        if (s == str_ptr)
            l = (pool_ptr - str_start[(str_ptr) - 65536L]);
        else
            l = length(s);
        h = str_pool[j];
        {
            register integer for_end;
            k = j + 1;
            for_end = j + l - 1;
            if (k <= for_end)
                do {
                    h = h + h + str_pool[k];
                    while (h >= 431 /*prim_prime */ )
                        h = h - 431;
                }
                while (k++ < for_end);
        }
        p = h + 1;
        while (true) {

            if (prim[p].v.RH > 0) {

                if (length(prim[p].v.RH) == l) {

                    if (str_eq_str(prim[p].v.RH, s))
                        goto lab40;
                }
            }
            if (prim[p].v.LH == 0) {
                if (no_new_control_sequence)
                    p = 0 /*undefined_primitive */ ;
                else {          /*272: */

                    if (prim[p].v.RH > 0) {
                        do {
                            if ((prim_used == 1 /*prim_base */ ))
                                overflow(65808L /*"primitive size" */ , 500 /*prim_size */ );
                            prim_used--;
                        } while (!(prim[prim_used].v.RH == 0));
                        prim[p].v.LH = prim_used;
                        p = prim_used;
                    }
                    prim[p].v.RH = s;
                }
                goto lab40;
            }
            p = prim[p].v.LH;
        }
    }

lab40: /*found */
    Result = p;
    return Result;
}

/*:276*//*280: *//*296: */

void zprint_group(boolean e)
{
    print_group_regmem switch (cur_group) {
    case 0:
        {
            print(66840L /*"bottom level" */ );
            return;
        }
        break;
    case 1:
    case 14:
        {
            if (cur_group == 14 /*semi_simple_group */ )
                print(66841L /*"semi " */ );
            print(66842L /*"simple" */ );
        }
        break;
    case 2:
    case 3:
        {
            if (cur_group == 3 /*adjusted_hbox_group */ )
                print(66843L /*"adjusted " */ );
            print(66457L /*"hbox" */ );
        }
        break;
    case 4:
        print(66365L /*"vbox" */ );
        break;
    case 5:
        print(66456L /*"vtop" */ );
        break;
    case 6:
    case 7:
        {
            if (cur_group == 7 /*no_align_group */ )
                print(66844L /*"no " */ );
            print(66845L /*"align" */ );
        }
        break;
    case 8:
        print(65694L /*"output" */ );
        break;
    case 10:
        print(66846L /*"disc" */ );
        break;
    case 11:
        print(65614L /*"insert" */ );
        break;
    case 12:
        print(65855L /*"vcenter" */ );
        break;
    case 9:
    case 13:
    case 15:
    case 16:
        {
            print(65629L /*"math" */ );
            if (cur_group == 13 /*math_choice_group */ )
                print(66847L /*" choice" */ );
            else if (cur_group == 15 /*math_shift_group */ )
                print(66848L /*" shift" */ );
            else if (cur_group == 16 /*math_left_group */ )
                print(66849L /*" left" */ );
        }
        break;
    }
    print(66850L /*" group (level " */ );
    print_int(cur_level);
    print_char(41 /*")" */ );
    if (save_stack[save_ptr - 1].cint != 0) {
        if (e)
            print(65658L /*" entered at line " */ );
        else
            print(66272L /*" at line " */ );
        print_int(save_stack[save_ptr - 1].cint);
    }
}

/*:1448*//*1449: */

boolean pseudo_input(void)
{
    register boolean Result;
    pseudo_input_regmem halfword p;
    integer sz;
    four_quarters w;
    halfword r;
    last = first;
    p = mem[pseudo_files].hh.v.LH;
    if (p == -268435455L)
        Result = false;
    else {

        mem[pseudo_files].hh.v.LH = mem[p].hh.v.RH;
        sz = mem[p].hh.v.LH;
        if (4 * sz - 3 >= buf_size - last) {    /*35: */
            cur_input.loc_field = first;
            cur_input.limit_field = last - 1;
            overflow(65538L /*"buffer size" */ , buf_size);
        }
        last = first;
        {
            register integer for_end;
            r = p + 1;
            for_end = p + sz - 1;
            if (r <= for_end)
                do {
                    w = mem[r].qqqq;
                    buffer[last] = w.u.B0;
                    buffer[last + 1] = w.u.B1;
                    buffer[last + 2] = w.u.B2;
                    buffer[last + 3] = w.u.B3;
                    last = last + 4;
                }
                while (r++ < for_end);
        }
        if (last >= max_buf_stack)
            max_buf_stack = last + 1;
        while ((last > first) && (buffer[last - 1] == 32 /*" " */ ))
            last--;
        free_node(p, sz);
        Result = true;
    }
    return Result;
}

void pseudo_close(void)
{
    pseudo_close_regmem halfword p, q;
    p = mem[pseudo_files].hh.v.RH;
    q = mem[pseudo_files].hh.v.LH;
    {
        mem[pseudo_files].hh.v.RH = avail;
        avail = pseudo_files;
    }
    pseudo_files = p;
    while (q != -268435455L) {

        p = q;
        q = mem[p].hh.v.RH;
        free_node(p, mem[p].hh.v.LH);
    }
}

void group_warning(void)
{
    group_warning_regmem integer i;
    boolean w;
    base_ptr = input_ptr;
    input_stack[base_ptr] = cur_input;
    i = in_open;
    w = false;
    while ((grp_stack[i] == cur_boundary) && (i > 0)) {

        if (eqtb[8938802L /*int_base 62 */ ].cint > 0) {
            while ((input_stack[base_ptr].state_field == 0 /*token_list */ ) || (input_stack[base_ptr].index_field > i))
                base_ptr--;
            if (input_stack[base_ptr].name_field > 17)
                w = true;
        }
        grp_stack[i] = save_stack[save_ptr].hh.v.RH;
        i--;
    }
    if (w) {
        print_nl(66909L /*"Warning: end of " */ );
        print_group(true);
        print(66910L /*" of a different file" */ );
        print_ln();
        if (eqtb[8938802L /*int_base 62 */ ].cint > 1)
            show_context();
        if (history == HISTORY_SPOTLESS)
            history = HISTORY_WARNING_ISSUED;
    }
}

void if_warning(void)
{
    if_warning_regmem integer i;
    boolean w;
    base_ptr = input_ptr;
    input_stack[base_ptr] = cur_input;
    i = in_open;
    w = false;
    while (if_stack[i] == cond_ptr) {

        if (eqtb[8938802L /*int_base 62 */ ].cint > 0) {
            while ((input_stack[base_ptr].state_field == 0 /*token_list */ ) || (input_stack[base_ptr].index_field > i))
                base_ptr--;
            if (input_stack[base_ptr].name_field > 17)
                w = true;
        }
        if_stack[i] = mem[cond_ptr].hh.v.RH;
        i--;
    }
    if (w) {
        print_nl(66909L /*"Warning: end of " */ );
        print_cmd_chr(107 /*if_test */ , cur_if);
        if (if_line != 0) {
            print(66871L /*" entered on line " */ );
            print_int(if_line);
        }
        print(66910L /*" of a different file" */ );
        print_ln();
        if (eqtb[8938802L /*int_base 62 */ ].cint > 1)
            show_context();
        if (history == HISTORY_SPOTLESS)
            history = HISTORY_WARNING_ISSUED;
    }
}

void file_warning(void)
{
    file_warning_regmem halfword p;
    quarterword l;
    quarterword c;
    integer i;
    p = save_ptr;
    l = cur_level;
    c = cur_group;
    save_ptr = cur_boundary;
    while (grp_stack[in_open] != save_ptr) {

        cur_level--;
        print_nl(66911L /*"Warning: end of file when " */ );
        print_group(true);
        print(66912L /*" is incomplete" */ );
        cur_group = save_stack[save_ptr].hh.u.B1;
        save_ptr = save_stack[save_ptr].hh.v.RH;
    }
    save_ptr = p;
    cur_level = l;
    cur_group = c;
    p = cond_ptr;
    l = if_limit;
    c = cur_if;
    i = if_line;
    while (if_stack[in_open] != cond_ptr) {

        print_nl(66911L /*"Warning: end of file when " */ );
        print_cmd_chr(107 /*if_test */ , cur_if);
        if (if_limit == 2 /*fi_code */ )
            print_esc(66133L /*"else" */ );
        if (if_line != 0) {
            print(66871L /*" entered on line " */ );
            print_int(if_line);
        }
        print(66912L /*" is incomplete" */ );
        if_line = mem[cond_ptr + 1].cint;
        cur_if = mem[cond_ptr].hh.u.B1;
        if_limit = mem[cond_ptr].hh.u.B0;
        cond_ptr = mem[cond_ptr].hh.v.RH;
    }
    cond_ptr = p;
    if_limit = l;
    cur_if = c;
    if_line = i;
    print_ln();
    if (eqtb[8938802L /*int_base 62 */ ].cint > 1)
        show_context();
    if (history == HISTORY_SPOTLESS)
        history = HISTORY_WARNING_ISSUED;
}

void zdelete_sa_ref(halfword q)
{
    delete_sa_ref_regmem halfword p;
    small_number i;
    small_number s;
    mem[q + 1].hh.v.LH--;
    if (mem[q + 1].hh.v.LH != -268435455L)
        return;
    if (mem[q].hh.u.B0 < 128 /*dimen_val_limit */ ) {

        if (mem[q + 2].cint == 0)
            s = 3 /*word_node_size */ ;
        else
            return;
    } else {

        if (mem[q].hh.u.B0 < 256 /*mu_val_limit */ ) {

            if (mem[q + 1].hh.v.RH == mem_bot)
                delete_glue_ref(mem_bot);
            else
                return;
        } else if (mem[q + 1].hh.v.RH != -268435455L)
            return;
        s = 2 /*pointer_node_size */ ;
    }
    do {
        i = mem[q].hh.u.B0 % 64;
        p = q;
        q = mem[p].hh.v.RH;
        free_node(p, s);
        if (q == -268435455L) {
            sa_root[i] = -268435455L;
            return;
        }
        {
            if (odd(i))
                mem[q + (i / 2) + 1].hh.v.RH = -268435455L;
            else
                mem[q + (i / 2) + 1].hh.v.LH = -268435455L;
            mem[q].hh.u.B1--;
        }
        s = 33 /*index_node_size */ ;
    } while (!(mem[q].hh.u.B1 > 0));
}

/*:1609*//*1611: */

void zsa_save(halfword p)
{
    sa_save_regmem halfword q;
    quarterword i;
    if (cur_level != sa_level) {
        if (save_ptr > max_save_stack) {
            max_save_stack = save_ptr;
            if (max_save_stack > save_size - 7)
                overflow(65857L /*"save size" */ , save_size);
        }
        save_stack[save_ptr].hh.u.B0 = 4 /*restore_sa */ ;
        save_stack[save_ptr].hh.u.B1 = sa_level;
        save_stack[save_ptr].hh.v.RH = sa_chain;
        save_ptr++;
        sa_chain = -268435455L;
        sa_level = cur_level;
    }
    i = mem[p].hh.u.B0;
    if (i < 128 /*dimen_val_limit */ ) {
        if (mem[p + 2].cint == 0) {
            q = get_node(2 /*pointer_node_size */ );
            i = 384 /*tok_val_limit */ ;
        } else {

            q = get_node(3 /*word_node_size */ );
            mem[q + 2].cint = mem[p + 2].cint;
        }
        mem[q + 1].hh.v.RH = -268435455L;
    } else {

        q = get_node(2 /*pointer_node_size */ );
        mem[q + 1].hh.v.RH = mem[p + 1].hh.v.RH;
    }
    mem[q + 1].hh.v.LH = p;
    mem[q].hh.u.B0 = i;
    mem[q].hh.u.B1 = mem[p].hh.u.B1;
    mem[q].hh.v.RH = sa_chain;
    sa_chain = q;
    mem[p + 1].hh.v.LH++;
}

void zsa_destroy(halfword p)
{
    sa_destroy_regmem if (mem[p].hh.u.B0 < 256 /*mu_val_limit */ )
        delete_glue_ref(mem[p + 1].hh.v.RH);
    else if (mem[p + 1].hh.v.RH != -268435455L) {

        if (mem[p].hh.u.B0 < 320 /*box_val_limit */ )
            flush_node_list(mem[p + 1].hh.v.RH);
        else
            delete_token_ref(mem[p + 1].hh.v.RH);
    }
}

void zsa_def(halfword p, halfword e)
{
    memory_word *mem = zmem;

    mem[p + 1].hh.v.LH++;
    if (mem[p + 1].hh.v.RH == e) {
        sa_destroy(p);
    } else {
        if (mem[p].hh.u.B1 == cur_level)
            sa_destroy(p);
        else
            sa_save(p);
        mem[p].hh.u.B1 = cur_level;
        mem[p + 1].hh.v.RH = e;
    }
    delete_sa_ref(p);
}

void zsa_w_def(halfword p, integer w)
{
    memory_word *mem = zmem;

    mem[p + 1].hh.v.LH++;

    if (mem[p + 2].cint == w) {
    } else {
        if (mem[p].hh.u.B1 != cur_level)
            sa_save(p);
        mem[p].hh.u.B1 = cur_level;
        mem[p + 2].cint = w;
    }
    delete_sa_ref(p);
}

void zgsa_def(halfword p, halfword e)
{
    memory_word *mem = zmem;

    mem[p + 1].hh.v.LH++;
    sa_destroy(p);
    mem[p].hh.u.B1 = 1 /*level_one */ ;
    mem[p + 1].hh.v.RH = e;
    delete_sa_ref(p);
}

void zgsa_w_def(halfword p, integer w)
{
    memory_word *mem = zmem;

    mem[p + 1].hh.v.LH++;
    mem[p].hh.u.B1 = 1 /*level_one */ ;
    mem[p + 2].cint = w;
    delete_sa_ref(p);
}

void sa_restore(void)
{
    memory_word *mem = zmem;
    halfword p;

    do {
        p = mem[sa_chain + 1].hh.v.LH;
        if (mem[p].hh.u.B1 == 1 /*level_one */ ) {
            if (mem[p].hh.u.B0 >= 128 /*dimen_val_limit */ )
                sa_destroy(sa_chain);
        } else {

            if (mem[p].hh.u.B0 < 128 /*dimen_val_limit */ ) {

                if (mem[sa_chain].hh.u.B0 < 128 /*dimen_val_limit */ )
                    mem[p + 2].cint = mem[sa_chain + 2].cint;
                else
                    mem[p + 2].cint = 0;
            } else {

                sa_destroy(p);
                mem[p + 1].hh.v.RH = mem[sa_chain + 1].hh.v.RH;
            }
            mem[p].hh.u.B1 = mem[sa_chain].hh.u.B1;
        }
        delete_sa_ref(p);
        p = sa_chain;
        sa_chain = mem[p].hh.v.RH;
        if (mem[p].hh.u.B0 < 128 /*dimen_val_limit */ )
            free_node(p, 3 /*word_node_size */ );
        else
            free_node(p, 2 /*pointer_node_size */ );
    } while (!(sa_chain == -268435455L));
}

void znew_save_level(group_code c)
{
    if (save_ptr > max_save_stack) {
        max_save_stack = save_ptr;
        if (max_save_stack > save_size - 7)
            overflow(65857L /*"save size" */ , save_size);
    }
    if ((eTeX_mode == 1)) {
        save_stack[save_ptr + 0].cint = line;
        save_ptr++;
    }
    save_stack[save_ptr].hh.u.B0 = 3 /*level_boundary */ ;
    save_stack[save_ptr].hh.u.B1 = cur_group;
    save_stack[save_ptr].hh.v.RH = cur_boundary;
    if (cur_level == 65535L /*max_quarterword */ )
        overflow(65858L /*"grouping levels" */ , 65535L /*max_quarterword -0 */ );
    cur_boundary = save_ptr;
    cur_group = c;
    cur_level++;
    save_ptr++;
}

void zeq_destroy(memory_word w)
{
    eq_destroy_regmem halfword q;
    switch (w.hh.u.B0) {
    case 113:
    case 114:
    case 115:
    case 116:
        delete_token_ref(w.hh.v.RH);
        break;
    case 119:
        delete_glue_ref(w.hh.v.RH);
        break;
    case 120:
        {
            q = w.hh.v.RH;
            if (q != -268435455L)
                free_node(q, mem[q].hh.v.LH + mem[q].hh.v.LH + 1);
        }
        break;
    case 121:
        flush_node_list(w.hh.v.RH);
        break;
    case 72:
    case 91:
        if ((w.hh.v.RH < mem_bot) || (w.hh.v.RH > mem_bot + 19))
            delete_sa_ref(w.hh.v.RH);
        break;
    default:
        ;
        break;
    }
}

void zeq_save(halfword p, quarterword l)
{
    eq_save_regmem if (save_ptr > max_save_stack) {
        max_save_stack = save_ptr;
        if (max_save_stack > save_size - 7)
            overflow(65857L /*"save size" */ , save_size);
    }
    if (l == 0 /*level_zero */ )
        save_stack[save_ptr].hh.u.B0 = 1 /*restore_zero */ ;
    else {

        save_stack[save_ptr] = eqtb[p];
        save_ptr++;
        save_stack[save_ptr].hh.u.B0 = 0 /*restore_old_value */ ;
    }
    save_stack[save_ptr].hh.u.B1 = l;
    save_stack[save_ptr].hh.v.RH = p;
    save_ptr++;
}

void zeq_define(halfword p, quarterword t, halfword e)
{
    eq_define_regmem

    if ((eTeX_mode == 1) && (eqtb[p].hh.u.B0 == t) && (eqtb[p].hh.v.RH == e)) {
        eq_destroy(eqtb[p]);
        return;
    }

    if (eqtb[p].hh.u.B1 == cur_level)
        eq_destroy(eqtb[p]);
    else if (cur_level > 1 /*level_one */ )
        eq_save(p, eqtb[p].hh.u.B1);
    eqtb[p].hh.u.B1 = cur_level;
    eqtb[p].hh.u.B0 = t;
    eqtb[p].hh.v.RH = e;
}

void zeq_word_define(halfword p, integer w)
{
    eq_word_define_regmem

    if ((eTeX_mode == 1) && (eqtb[p].cint == w)) {
        return;
    }

    if (xeq_level[p] != cur_level) {
        eq_save(p, xeq_level[p]);
        xeq_level[p] = cur_level;
    }
    eqtb[p].cint = w;
}

void zgeq_define(halfword p, quarterword t, halfword e)
{
    geq_define_regmem;

    {
        eq_destroy(eqtb[p]);
        eqtb[p].hh.u.B1 = 1 /*level_one */ ;
        eqtb[p].hh.u.B0 = t;
        eqtb[p].hh.v.RH = e;
    }
}

void zgeq_word_define(halfword p, integer w)
{
    geq_word_define_regmem;
    {
        eqtb[p].cint = w;
        xeq_level[p] = 1 /*level_one */ ;
    }
}

void zsave_for_after(halfword t)
{
    save_for_after_regmem if (cur_level > 1 /*level_one */ ) {
        if (save_ptr > max_save_stack) {
            max_save_stack = save_ptr;
            if (max_save_stack > save_size - 7)
                overflow(65857L /*"save size" */ , save_size);
        }
        save_stack[save_ptr].hh.u.B0 = 2 /*insert_token */ ;
        save_stack[save_ptr].hh.u.B1 = 0 /*level_zero */ ;
        save_stack[save_ptr].hh.v.RH = t;
        save_ptr++;
    }
}

void unsave(void)
{
    unsave_regmem halfword p;
    quarterword l;
    halfword t;
    boolean a;
    a = false;
    if (cur_level > 1 /*level_one */ ) {
        cur_level--;
        while (true) {

            save_ptr--;
            if (save_stack[save_ptr].hh.u.B0 == 3 /*level_boundary */ )
                goto lab30;
            p = save_stack[save_ptr].hh.v.RH;
            if (save_stack[save_ptr].hh.u.B0 == 2 /*insert_token */ ) {   /*338: */
                t = cur_tok;
                cur_tok = p;
                if (a) {
                    p = get_avail();
                    mem[p].hh.v.LH = cur_tok;
                    mem[p].hh.v.RH = cur_input.loc_field;
                    cur_input.loc_field = p;
                    cur_input.start_field = p;
                    if (cur_tok < 6291456L /*right_brace_limit */ ) {

                        if (cur_tok < 4194304L /*left_brace_limit */ )
                            align_state--;
                        else
                            align_state++;
                    }
                } else {

                    back_input();
                    a = (eTeX_mode == 1);
                }
                cur_tok = t;
            } else if (save_stack[save_ptr].hh.u.B0 == 4 /*restore_sa */ ) {
                sa_restore();
                sa_chain = p;
                sa_level = save_stack[save_ptr].hh.u.B1;
            } else {

                if (save_stack[save_ptr].hh.u.B0 == 0 /*restore_old_value */ ) {
                    l = save_stack[save_ptr].hh.u.B1;
                    save_ptr--;
                } else
                    save_stack[save_ptr] = eqtb[2252239L /*undefined_control_sequence */ ];
                if ((p < 8938740L /*int_base */ ) || (p > 10053470L /*eqtb_size */ )) {

                    if (eqtb[p].hh.u.B1 == 1 /*level_one */ ) {
                        eq_destroy(save_stack[save_ptr]);
                    } else {
                        eq_destroy(eqtb[p]);
                        eqtb[p] = save_stack[save_ptr];
                    }
                } else if (xeq_level[p] != 1 /*level_one */ ) {
                    eqtb[p] = save_stack[save_ptr];
                    xeq_level[p] = l;
                }
            }
        }

    lab30:/* done */
        if (grp_stack[in_open] == cur_boundary)
            group_warning();
        cur_group = save_stack[save_ptr].hh.u.B1;
        cur_boundary = save_stack[save_ptr].hh.v.RH;
        if ((eTeX_mode == 1))
            save_ptr--;
    } else
        confusion(65863L /*"curlevel" */ );
}

void prepare_mag(void)
{
    prepare_mag_regmem if ((mag_set > 0) && (eqtb[8938757L /*int_base 17 */ ].cint != mag_set)) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(65867L /*"Incompatible magnification (" */ );
        }
        print_int(eqtb[8938757L /*int_base 17 */ ].cint);
        print(65868L /*");" */ );
        print_nl(65869L /*" the previous value will be retained" */ );
        {
            help_ptr = 2;
            help_line[1] = 65870L /*"I can handle only one magnification ratio per job. So I've" */ ;
            help_line[0] = 65871L /*"reverted to the magnification you used earlier on this run." */ ;
        }
        int_error(mag_set);
        geq_word_define(8938757L /*int_base 17 */ , mag_set);
    }
    if ((eqtb[8938757L /*int_base 17 */ ].cint <= 0) || (eqtb[8938757L /*int_base 17 */ ].cint > 32768L)) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(65872L /*"Illegal magnification has been changed to 1000" */ );
        }
        {
            help_ptr = 1;
            help_line[0] = 65873L /*"The magnification ratio must be between 1 and 32768." */ ;
        }
        int_error(eqtb[8938757L /*int_base 17 */ ].cint);
        geq_word_define(8938757L /*int_base 17 */ , 1000);
    }
    mag_set = eqtb[8938757L /*int_base 17 */ ].cint;
}

void ztoken_show(halfword p)
{
    token_show_regmem if (p != -268435455L)
        show_token_list(mem[p].hh.v.RH, -268435455L, 10000000L);
}

void print_meaning(void)
{
    print_meaning_regmem print_cmd_chr(cur_cmd, cur_chr);
    if (cur_cmd >= 113 /*call */ ) {
        print_char(58 /*":" */ );
        print_ln();
        token_show(cur_chr);
    } else if ((cur_cmd == 112 /*top_bot_mark */ ) && (cur_chr < 5)) {
        print_char(58 /*":" */ );
        print_ln();
        token_show(cur_mark[cur_chr]);
    }
}

void show_cur_cmd_chr(void)
{
    show_cur_cmd_chr_regmem integer n;
    integer l;
    halfword p;
    begin_diagnostic();
    print_nl(123 /*"_" */ );
    if (cur_list.mode_field != shown_mode) {
        print_mode(cur_list.mode_field);
        print(65589L /*": " */ );
        shown_mode = cur_list.mode_field;
    }
    print_cmd_chr(cur_cmd, cur_chr);
    if (eqtb[8938800L /*int_base 60 */ ].cint > 0) {

        if (cur_cmd >= 107 /*if_test */ ) {

            if (cur_cmd <= 108 /*fi_or_else */ ) {
                print(65589L /*": " */ );
                if (cur_cmd == 108 /*fi_or_else */ ) {
                    print_cmd_chr(107 /*if_test */ , cur_if);
                    print_char(32 /*" " */ );
                    n = 0;
                    l = if_line;
                } else {

                    n = 1;
                    l = line;
                }
                p = cond_ptr;
                while (p != -268435455L) {

                    n++;
                    p = mem[p].hh.v.RH;
                }
                print(65887L /*"(level " */ );
                print_int(n);
                print_char(41 /*")" */ );
                if (l != 0) {
                    print(66871L /*" entered on line " */ );
                    print_int(l);
                }
            }
        }
    }
    print_char(125 /*"_" */ );
    end_diagnostic(false);
}

void show_context(void)
{
    show_context_regmem unsigned char /*max_selector */ old_setting;
    integer nn;
    boolean bottom_line;
    integer i;
    integer j;
    integer l;
    integer m;
    integer n;
    integer p;
    integer q;
    base_ptr = input_ptr;
    input_stack[base_ptr] = cur_input;
    nn = -1;
    bottom_line = false;
    while (true) {

        cur_input = input_stack[base_ptr];
        if ((cur_input.state_field != 0 /*token_list */ )) {

            if ((cur_input.name_field > 19) || (base_ptr == 0))
                bottom_line = true;
        }
        if ((base_ptr == input_ptr) || bottom_line || (nn < eqtb[8938794L /*int_base 54 */ ].cint)) {   /*324: */
            if ((base_ptr == input_ptr) || (cur_input.state_field != 0 /*token_list */ )
                || (cur_input.index_field != 3 /*backed_up */ ) || (cur_input.loc_field != -268435455L)) {
                tally = 0;
                old_setting = selector;
                if (cur_input.state_field != 0 /*token_list */ ) {
                    if (cur_input.name_field <= 17) {

                        if ((cur_input.name_field == 0)) {

                            if (base_ptr == 0)
                                print_nl(65892L /*"<*>" */ );
                            else
                                print_nl(65893L /*"<insert> " */ );
                        } else {

                            print_nl(65894L /*"<read " */ );
                            if (cur_input.name_field == 17)
                                print_char(42 /*"*" */ );
                            else
                                print_int(cur_input.name_field - 1);
                            print_char(62 /*">" */ );
                        }
                    } else {

                        print_nl(65895L /*"l." */ );
                        if (cur_input.index_field == in_open)
                            print_int(line);
                        else
                            print_int(line_stack[cur_input.index_field + 1]);
                    }
                    print_char(32 /*" " */ );
                    {
                        l = tally;
                        tally = 0;
                        selector = SELECTOR_PSEUDO;
                        trick_count = 1000000L;
                    }
                    if (buffer[cur_input.limit_field] == eqtb[8938788L /*int_base 48 */ ].cint)
                        j = cur_input.limit_field;
                    else
                        j = cur_input.limit_field + 1;
                    if (j > 0) {
                        register integer for_end;
                        i = cur_input.start_field;
                        for_end = j - 1;
                        if (i <= for_end)
                            do {
                                if (i == cur_input.loc_field) {
                                    first_count = tally;
                                    trick_count = tally + 1 + error_line - half_error_line;
                                    if (trick_count < error_line)
                                        trick_count = error_line;
                                }
                                print_char(buffer[i]);
                            }
                            while (i++ < for_end);
                    }
                } else {

                    switch (cur_input.index_field) {
                    case 0:
                        print_nl(65896L /*"<argument> " */ );
                        break;
                    case 1:
                    case 2:
                        print_nl(65897L /*"<template> " */ );
                        break;
                    case 3:
                    case 4:
                        if (cur_input.loc_field == -268435455L)
                            print_nl(65898L /*"<recently read> " */ );
                        else
                            print_nl(65899L /*"<to be read again> " */ );
                        break;
                    case 5:
                        print_nl(65900L /*"<inserted text> " */ );
                        break;
                    case 6:
                        {
                            print_ln();
                            print_cs(cur_input.name_field);
                        }
                        break;
                    case 7:
                        print_nl(65901L /*"<output> " */ );
                        break;
                    case 8:
                        print_nl(65902L /*"<everypar> " */ );
                        break;
                    case 9:
                        print_nl(65903L /*"<everymath> " */ );
                        break;
                    case 10:
                        print_nl(65904L /*"<everydisplay> " */ );
                        break;
                    case 11:
                        print_nl(65905L /*"<everyhbox> " */ );
                        break;
                    case 12:
                        print_nl(65906L /*"<everyvbox> " */ );
                        break;
                    case 13:
                        print_nl(65907L /*"<everyjob> " */ );
                        break;
                    case 14:
                        print_nl(65908L /*"<everycr> " */ );
                        break;
                    case 15:
                        print_nl(65909L /*"<mark> " */ );
                        break;
                    case 16:
                        print_nl(65910L /*"<everyeof> " */ );
                        break;
                    case 17:
                        print_nl(65911L /*"<XeTeXinterchartoks> " */ );
                        break;
                    case 18:
                        print_nl(65912L /*"<write> " */ );
                        break;
                    default:
                        print_nl(63 /*"?" */ );
                        break;
                    }
                    {
                        l = tally;
                        tally = 0;
                        selector = SELECTOR_PSEUDO;
                        trick_count = 1000000L;
                    }
                    if (cur_input.index_field < 6 /*macro */ )
                        show_token_list(cur_input.start_field, cur_input.loc_field, 100000L);
                    else
                        show_token_list(mem[cur_input.start_field].hh.v.RH, cur_input.loc_field, 100000L);
                }
                selector = old_setting;
                if (trick_count == 1000000L) {
                    first_count = tally;
                    trick_count = tally + 1 + error_line - half_error_line;
                    if (trick_count < error_line)
                        trick_count = error_line;
                }
                if (tally < trick_count)
                    m = tally - first_count;
                else
                    m = trick_count - first_count;
                if (l + first_count <= half_error_line) {
                    p = 0;
                    n = l + first_count;
                } else {

                    print(65557L /*"..." */ );
                    p = l + first_count - half_error_line + 3;
                    n = half_error_line;
                }
                {
                    register integer for_end;
                    q = p;
                    for_end = first_count - 1;
                    if (q <= for_end)
                        do
                            print_raw_char(trick_buf[q % error_line], true);
                        while (q++ < for_end);
                }
                print_ln();
                {
                    register integer for_end;
                    q = 1;
                    for_end = n;
                    if (q <= for_end)
                        do
                            print_raw_char(32 /*" " */ , true);
                        while (q++ < for_end);
                }
                if (m + n <= error_line)
                    p = first_count + m;
                else
                    p = first_count + (error_line - n - 3);
                {
                    register integer for_end;
                    q = first_count;
                    for_end = p - 1;
                    if (q <= for_end)
                        do
                            print_raw_char(trick_buf[q % error_line], true);
                        while (q++ < for_end);
                }
                if (m + n > error_line)
                    print(65557L /*"..." */ );
                nn++;
            }
        } else if (nn == eqtb[8938794L /*int_base 54 */ ].cint) {
            print_nl(65557L /*"..." */ );
            nn++;
        }
        if (bottom_line)
            goto lab30;
        base_ptr--;
    }
 lab30:                        /*done */ cur_input = input_stack[input_ptr];
}

void zbegin_token_list(halfword p, quarterword t)
{
    begin_token_list_regmem {
        if (input_ptr > max_in_stack) {
            max_in_stack = input_ptr;
            if (input_ptr == stack_size)
                overflow(65913L /*"input stack size" */ , stack_size);
        }
        input_stack[input_ptr] = cur_input;
        input_ptr++;
    }
    cur_input.state_field = 0 /*token_list */ ;
    cur_input.start_field = p;
    cur_input.index_field = t;
    if (t >= 6 /*macro */ ) {
        mem[p].hh.v.LH++;
        if (t == 6 /*macro */ )
            cur_input.limit_field = param_ptr;
        else {

            cur_input.loc_field = mem[p].hh.v.RH;
            if (eqtb[8938770L /*int_base 30 */ ].cint > 1) {
                begin_diagnostic();
                print_nl(65622L /*"" */ );
                switch (t) {
                case 15:
                    print_esc(65637L /*"mark" */ );
                    break;
                case 18:
                    print_esc(65914L /*"write" */ );
                    break;
                default:
                    print_cmd_chr(73 /*assign_toks */ , t + 2252765L);
                    break;
                }
                print(65875L /*"->" */ );
                token_show(p);
                end_diagnostic(false);
            }
        }
    } else
        cur_input.loc_field = p;
}

void end_token_list(void)
{
    end_token_list_regmem if (cur_input.index_field >= 3 /*backed_up */ ) {
        if (cur_input.index_field <= 5 /*inserted */ )
            flush_list(cur_input.start_field);
        else {

            delete_token_ref(cur_input.start_field);
            if (cur_input.index_field == 6 /*macro */ )
                while (param_ptr > cur_input.limit_field) {

                    param_ptr--;
                    flush_list(param_stack[param_ptr]);
                }
        }
    } else if (cur_input.index_field == 1 /*u_template */ ) {

        if (align_state > 500000L)
            align_state = 0;
        else
            fatal_error(65915L /*"(interwoven alignment preambles are not allowed)" */ );
    }
    {
        input_ptr--;
        cur_input = input_stack[input_ptr];
    }
    {
        if (interrupt != 0)
            pause_for_instructions();
    }
}

void back_input(void)
{
    back_input_regmem halfword p;
    while ((cur_input.state_field == 0 /*token_list */ ) && (cur_input.loc_field == -268435455L)
           && (cur_input.index_field != 2 /*v_template */ ))
        end_token_list();
    p = get_avail();
    mem[p].hh.v.LH = cur_tok;
    if (cur_tok < 6291456L /*right_brace_limit */ ) {

        if (cur_tok < 4194304L /*left_brace_limit */ )
            align_state--;
        else
            align_state++;
    }
    {
        if (input_ptr > max_in_stack) {
            max_in_stack = input_ptr;
            if (input_ptr == stack_size)
                overflow(65913L /*"input stack size" */ , stack_size);
        }
        input_stack[input_ptr] = cur_input;
        input_ptr++;
    }
    cur_input.state_field = 0 /*token_list */ ;
    cur_input.start_field = p;
    cur_input.index_field = 3 /*backed_up */ ;
    cur_input.loc_field = p;
}

void back_error(void)
{
    back_error_regmem OK_to_interrupt = false;
    back_input();
    OK_to_interrupt = true;
    error();
}

void ins_error(void)
{
    ins_error_regmem OK_to_interrupt = false;
    back_input();
    cur_input.index_field = 5 /*inserted */ ;
    OK_to_interrupt = true;
    error();
}

void begin_file_reading(void)
{
    begin_file_reading_regmem if (in_open == max_in_open)
        overflow(65916L /*"text input levels" */ , max_in_open);
    if (first == buf_size)
        overflow(65538L /*"buffer size" */ , buf_size);
    in_open++;
    {
        if (input_ptr > max_in_stack) {
            max_in_stack = input_ptr;
            if (input_ptr == stack_size)
                overflow(65913L /*"input stack size" */ , stack_size);
        }
        input_stack[input_ptr] = cur_input;
        input_ptr++;
    }
    cur_input.index_field = in_open;
    source_filename_stack[cur_input.index_field] = 0;
    full_source_filename_stack[cur_input.index_field] = 0;
    eof_seen[cur_input.index_field] = false;
    grp_stack[cur_input.index_field] = cur_boundary;
    if_stack[cur_input.index_field] = cond_ptr;
    line_stack[cur_input.index_field] = line;
    cur_input.start_field = first;
    cur_input.state_field = 1 /*mid_line */ ;
    cur_input.name_field = 0;
    cur_input.synctex_tag_field = 0;
}

void end_file_reading(void)
{
    end_file_reading_regmem first = cur_input.start_field;
    line = line_stack[cur_input.index_field];
    if ((cur_input.name_field == 18) || (cur_input.name_field == 19))
        pseudo_close();
    else if (cur_input.name_field > 17)
        u_close(input_file[cur_input.index_field]);
    {
        input_ptr--;
        cur_input = input_stack[input_ptr];
    }
    in_open--;
}

void clear_for_error_prompt(void)
{
    clear_for_error_prompt_regmem
        while ((cur_input.state_field != 0 /*token_list */ ) && (cur_input.name_field == 0) && (input_ptr > 0)
               && (cur_input.loc_field > cur_input.limit_field))
        end_file_reading();
    print_ln();
}

void check_outer_validity(void)
{
    check_outer_validity_regmem halfword p;
    halfword q;
    if (scanner_status != 0 /*normal */ ) {
        deletions_allowed = false;
        if (cur_cs != 0) {
            if ((cur_input.state_field == 0 /*token_list */ ) || (cur_input.name_field < 1)
                || (cur_input.name_field > 17)) {
                p = get_avail();
                mem[p].hh.v.LH = 33554431L /*cs_token_flag */  + cur_cs;
                begin_token_list(p, 3 /*backed_up */ );
            }
            cur_cmd = 10 /*spacer */ ;
            cur_chr = 32 /*" " */ ;
        }
        if (scanner_status > 1 /*skipping */ ) {        /*350: */
            runaway();
            if (cur_cs == 0) {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(65924L /*"File ended" */ );
            } else {

                cur_cs = 0;
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(65925L /*"Forbidden control sequence found" */ );
                }
            }
            p = get_avail();
            switch (scanner_status) {
            case 2:
                {
                    print(65931L /*" while scanning definition" */ );
                    mem[p].hh.v.LH = 4194429L /*right_brace_token 125 */ ;
                }
                break;
            case 3:
                {
                    print(65932L /*" while scanning use" */ );
                    mem[p].hh.v.LH = par_token;
                    long_state = 115 /*outer_call */ ;
                }
                break;
            case 4:
                {
                    print(65933L /*" while scanning preamble" */ );
                    mem[p].hh.v.LH = 4194429L /*right_brace_token 125 */ ;
                    q = p;
                    p = get_avail();
                    mem[p].hh.v.RH = q;
                    mem[p].hh.v.LH = 35797658L /*cs_token_flag 2243227 */ ;
                    align_state = -1000000L;
                }
                break;
            case 5:
                {
                    print(65934L /*" while scanning text" */ );
                    mem[p].hh.v.LH = 4194429L /*right_brace_token 125 */ ;
                }
                break;
            }
            begin_token_list(p, 5 /*inserted */ );
            print(65926L /*" of " */ );
            sprint_cs(warning_index);
            {
                help_ptr = 4;
                help_line[3] = 65927L /*"I suspect you have forgotten a `_', causing me" */ ;
                help_line[2] = 65928L /*"to read past where you wanted me to stop." */ ;
                help_line[1] = 65929L /*"I'll try to recover; but if the error is serious," */ ;
                help_line[0] = 65930L /*"you'd better type `E' or `X' now and fix your file." */ ;
            }
            error();
        } else {

            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(65918L /*"Incomplete " */ );
            }
            print_cmd_chr(107 /*if_test */ , cur_if);
            print(65919L /*"; all text was ignored after line " */ );
            print_int(skip_line);
            {
                help_ptr = 3;
                help_line[2] = 65920L /*"A forbidden control sequence occurred in skipped text." */ ;
                help_line[1] = 65921L /*"This kind of error happens when you say `\if...' and forget" */ ;
                help_line[0] = 65922L /*"the matching `\fi'. I've inserted a `\fi'; this might work." */ ;
            }
            if (cur_cs != 0)
                cur_cs = 0;
            else
                help_line[2] = 65923L /*"The file ended while I was skipping conditional text." */ ;
            cur_tok = 35797661L /*cs_token_flag 2243230 */ ;
            ins_error();
        }
        deletions_allowed = true;
    }
}

void get_next(void)
{
    get_next_regmem integer k;
    halfword t;
    unsigned char /*max_char_code */ cat;
    UnicodeScalar c;
    UTF16_code lower;
    small_number d;
    small_number sup_count;
 lab20:                        /*restart */ cur_cs = 0;
    if (cur_input.state_field != 0 /*token_list */ ) {  /*355: */
 lab25:                        /*switch */ if (cur_input.loc_field <= cur_input.limit_field) {
            cur_chr = buffer[cur_input.loc_field];
            cur_input.loc_field++;
            if ((cur_chr >= 55296L) && (cur_chr < 56320L) && (cur_input.loc_field <= cur_input.limit_field)
                && (buffer[cur_input.loc_field] >= 56320L) && (buffer[cur_input.loc_field] < 57344L)) {
                lower = buffer[cur_input.loc_field] - 56320L;
                cur_input.loc_field++;
                cur_chr = 65536L + (cur_chr - 55296L) * 1024 + lower;
            }
 lab21:    /*reswitch */ cur_cmd = eqtb[2254068L /*cat_code_base */  + cur_chr].hh.v.RH;
            switch (cur_input.state_field + cur_cmd) {  /*357: */
            case 10:
            case 26:
            case 42:
            case 27:
            case 43:
                goto lab25;
                break;
            case 1:
            case 17:
            case 33:
                {
                    if (cur_input.loc_field > cur_input.limit_field)
                        cur_cs = 2228225L /*null_cs */ ;
                    else {

 lab26:                        /*start_cs */ k = cur_input.loc_field;
                        cur_chr = buffer[k];
                        cat = eqtb[2254068L /*cat_code_base */  + cur_chr].hh.v.RH;
                        k++;
                        if (cat == 11 /*letter */ )
                            cur_input.state_field = 17 /*skip_blanks */ ;
                        else if (cat == 10 /*spacer */ )
                            cur_input.state_field = 17 /*skip_blanks */ ;
                        else
                            cur_input.state_field = 1 /*mid_line */ ;
                        if ((cat == 11 /*letter */ ) && (k <= cur_input.limit_field)) { /*368: */
                            do {
                                cur_chr = buffer[k];
                                cat = eqtb[2254068L /*cat_code_base */  + cur_chr].hh.v.RH;
                                k++;
                            } while (!((cat != 11 /*letter */ ) || (k > cur_input.limit_field)));
                            {
                                if ((cat == 7 /*sup_mark */ ) && (buffer[k] == cur_chr) && (k < cur_input.limit_field)) {
                                    sup_count = 2;
                                    while ((sup_count < 6) && (k + 2 * sup_count - 2 <= cur_input.limit_field)
                                           && (buffer[k + sup_count - 1] == cur_chr))
                                        sup_count++;
                                    {
                                        register integer for_end;
                                        d = 1;
                                        for_end = sup_count;
                                        if (d <= for_end)
                                            do
                                                if (!
                                                    (((buffer[k + sup_count - 2 + d] >= 48 /*"0" */ )
                                                      && (buffer[k + sup_count - 2 + d] <= 57 /*"9" */ ))
                                                     || ((buffer[k + sup_count - 2 + d] >= 97 /*"a" */ )
                                                         && (buffer[k + sup_count - 2 + d] <= 102 /*"f" */ )))) {
                                                    c = buffer[k + 1];
                                                    if (c < 128) {
                                                        if (c < 64)
                                                            buffer[k - 1] = c + 64;
                                                        else
                                                            buffer[k - 1] = c - 64;
                                                        d = 2;
                                                        cur_input.limit_field = cur_input.limit_field - d;
                                                        while (k <= cur_input.limit_field) {

                                                            buffer[k] = buffer[k + d];
                                                            k++;
                                                        }
                                                        goto lab26;
                                                    } else
                                                        sup_count = 0;
                                                }
                                            while (d++ < for_end) ;
                                    }
                                    if (sup_count > 0) {
                                        cur_chr = 0;
                                        {
                                            register integer for_end;
                                            d = 1;
                                            for_end = sup_count;
                                            if (d <= for_end)
                                                do {
                                                    c = buffer[k + sup_count - 2 + d];
                                                    if (c <= 57 /*"9" */ )
                                                        cur_chr = 16 * cur_chr + c - 48;
                                                    else
                                                        cur_chr = 16 * cur_chr + c - 87;
                                                }
                                                while (d++ < for_end);
                                        }
                                        if (cur_chr > 1114111L /*biggest_usv */ )
                                            cur_chr = buffer[k];
                                        else {

                                            buffer[k - 1] = cur_chr;
                                            d = 2 * sup_count - 1;
                                            cur_input.limit_field = cur_input.limit_field - d;
                                            while (k <= cur_input.limit_field) {

                                                buffer[k] = buffer[k + d];
                                                k++;
                                            }
                                            goto lab26;
                                        }
                                    }
                                }
                            }
                            if (cat != 11 /*letter */ )
                                k--;
                            if (k > cur_input.loc_field + 1) {
                                cur_cs = id_lookup(cur_input.loc_field, k - cur_input.loc_field);
                                cur_input.loc_field = k;
                                goto lab40;
                            }
                        } else {        /*367: */

                            if ((cat == 7 /*sup_mark */ ) && (buffer[k] == cur_chr) && (k < cur_input.limit_field)) {
                                sup_count = 2;
                                while ((sup_count < 6) && (k + 2 * sup_count - 2 <= cur_input.limit_field)
                                       && (buffer[k + sup_count - 1] == cur_chr))
                                    sup_count++;
                                {
                                    register integer for_end;
                                    d = 1;
                                    for_end = sup_count;
                                    if (d <= for_end)
                                        do
                                            if (!
                                                (((buffer[k + sup_count - 2 + d] >= 48 /*"0" */ )
                                                  && (buffer[k + sup_count - 2 + d] <= 57 /*"9" */ ))
                                                 || ((buffer[k + sup_count - 2 + d] >= 97 /*"a" */ )
                                                     && (buffer[k + sup_count - 2 + d] <= 102 /*"f" */ )))) {
                                                c = buffer[k + 1];
                                                if (c < 128) {
                                                    if (c < 64)
                                                        buffer[k - 1] = c + 64;
                                                    else
                                                        buffer[k - 1] = c - 64;
                                                    d = 2;
                                                    cur_input.limit_field = cur_input.limit_field - d;
                                                    while (k <= cur_input.limit_field) {

                                                        buffer[k] = buffer[k + d];
                                                        k++;
                                                    }
                                                    goto lab26;
                                                } else
                                                    sup_count = 0;
                                            }
                                        while (d++ < for_end) ;
                                }
                                if (sup_count > 0) {
                                    cur_chr = 0;
                                    {
                                        register integer for_end;
                                        d = 1;
                                        for_end = sup_count;
                                        if (d <= for_end)
                                            do {
                                                c = buffer[k + sup_count - 2 + d];
                                                if (c <= 57 /*"9" */ )
                                                    cur_chr = 16 * cur_chr + c - 48;
                                                else
                                                    cur_chr = 16 * cur_chr + c - 87;
                                            }
                                            while (d++ < for_end);
                                    }
                                    if (cur_chr > 1114111L /*biggest_usv */ )
                                        cur_chr = buffer[k];
                                    else {

                                        buffer[k - 1] = cur_chr;
                                        d = 2 * sup_count - 1;
                                        cur_input.limit_field = cur_input.limit_field - d;
                                        while (k <= cur_input.limit_field) {

                                            buffer[k] = buffer[k + d];
                                            k++;
                                        }
                                        goto lab26;
                                    }
                                }
                            }
                        }
                        if (buffer[cur_input.loc_field] > 65535L) {
                            cur_cs = id_lookup(cur_input.loc_field, 1);
                            cur_input.loc_field++;
                            goto lab40;
                        }
                        cur_cs = 1114113L /*single_base */  + buffer[cur_input.loc_field];
                        cur_input.loc_field++;
                    }
 lab40:                        /*found */ cur_cmd = eqtb[cur_cs].hh.u.B0;
                    cur_chr = eqtb[cur_cs].hh.v.RH;
                    if (cur_cmd >= 115 /*outer_call */ )
                        check_outer_validity();
                }
                break;
            case 14:
            case 30:
            case 46:
                {
                    cur_cs = cur_chr + 1;
                    cur_cmd = eqtb[cur_cs].hh.u.B0;
                    cur_chr = eqtb[cur_cs].hh.v.RH;
                    cur_input.state_field = 1 /*mid_line */ ;
                    if (cur_cmd >= 115 /*outer_call */ )
                        check_outer_validity();
                }
                break;
            case 8:
            case 24:
            case 40:
                {
                    if (cur_chr == buffer[cur_input.loc_field]) {

                        if (cur_input.loc_field < cur_input.limit_field) {
                            sup_count = 2;
                            while ((sup_count < 6) && (cur_input.loc_field + 2 * sup_count - 2 <= cur_input.limit_field)
                                   && (cur_chr == buffer[cur_input.loc_field + sup_count - 1]))
                                sup_count++;
                            {
                                register integer for_end;
                                d = 1;
                                for_end = sup_count;
                                if (d <= for_end)
                                    do
                                        if (!
                                            (((buffer[cur_input.loc_field + sup_count - 2 + d] >= 48 /*"0" */ )
                                              && (buffer[cur_input.loc_field + sup_count - 2 + d] <= 57 /*"9" */ ))
                                             || ((buffer[cur_input.loc_field + sup_count - 2 + d] >= 97 /*"a" */ )
                                                 && (buffer[cur_input.loc_field + sup_count - 2 + d] <=
                                                     102 /*"f" */ )))) {
                                            c = buffer[cur_input.loc_field + 1];
                                            if (c < 128) {
                                                cur_input.loc_field = cur_input.loc_field + 2;
                                                if (c < 64)
                                                    cur_chr = c + 64;
                                                else
                                                    cur_chr = c - 64;
                                                goto lab21;
                                            }
                                            goto lab27;
                                        }
                                    while (d++ < for_end) ;
                            }
                            cur_chr = 0;
                            {
                                register integer for_end;
                                d = 1;
                                for_end = sup_count;
                                if (d <= for_end)
                                    do {
                                        c = buffer[cur_input.loc_field + sup_count - 2 + d];
                                        if (c <= 57 /*"9" */ )
                                            cur_chr = 16 * cur_chr + c - 48;
                                        else
                                            cur_chr = 16 * cur_chr + c - 87;
                                    }
                                    while (d++ < for_end);
                            }
                            if (cur_chr > 1114111L /*biggest_usv */ ) {
                                cur_chr = buffer[cur_input.loc_field];
                                goto lab27;
                            }
                            cur_input.loc_field = cur_input.loc_field + 2 * sup_count - 1;
                            goto lab21;
                        }
                    }
 lab27:            /*not_exp */ cur_input.state_field = 1 /*mid_line */ ;
                }
                break;
            case 16:
            case 32:
            case 48:
                {
                    {
                        if (interaction == 3 /*error_stop_mode */ ) ;
                        if (file_line_error_style_p)
                            print_file_line();
                        else
                            print_nl(65544L /*"! " */ );
                        print(65935L /*"Text line contains an invalid character" */ );
                    }
                    {
                        help_ptr = 2;
                        help_line[1] = 65936L /*"A funny symbol that I can't read has just been input." */ ;
                        help_line[0] = 65937L /*"Continue, and I'll forget that it ever happened." */ ;
                    }
                    deletions_allowed = false;
                    error();
                    deletions_allowed = true;
                    goto lab20;
                }
                break;
            case 11:
                {
                    cur_input.state_field = 17 /*skip_blanks */ ;
                    cur_chr = 32 /*" " */ ;
                }
                break;
            case 6:
                {
                    cur_input.loc_field = cur_input.limit_field + 1;
                    cur_cmd = 10 /*spacer */ ;
                    cur_chr = 32 /*" " */ ;
                }
                break;
            case 22:
            case 15:
            case 31:
            case 47:
                {
                    cur_input.loc_field = cur_input.limit_field + 1;
                    goto lab25;
                }
                break;
            case 38:
                {
                    cur_input.loc_field = cur_input.limit_field + 1;
                    cur_cs = par_loc;
                    cur_cmd = eqtb[cur_cs].hh.u.B0;
                    cur_chr = eqtb[cur_cs].hh.v.RH;
                    if (cur_cmd >= 115 /*outer_call */ )
                        check_outer_validity();
                }
                break;
            case 2:
                align_state++;
                break;
            case 18:
            case 34:
                {
                    cur_input.state_field = 1 /*mid_line */ ;
                    align_state++;
                }
                break;
            case 3:
                align_state--;
                break;
            case 19:
            case 35:
                {
                    cur_input.state_field = 1 /*mid_line */ ;
                    align_state--;
                }
                break;
            case 20:
            case 21:
            case 23:
            case 25:
            case 28:
            case 29:
            case 36:
            case 37:
            case 39:
            case 41:
            case 44:
            case 45:
                cur_input.state_field = 1 /*mid_line */ ;
                break;
            default:
                ;
                break;
            }
        } else {

            cur_input.state_field = 33 /*new_line */ ;
            if (cur_input.name_field > 17) {    /*374: */
                line++;
                first = cur_input.start_field;
                if (!force_eof) {

                    if (cur_input.name_field <= 19) {
                        if (pseudo_input())
                            firm_up_the_line();
                        else if ((eqtb[2252781L /*every_eof_loc */ ].hh.v.RH != -268435455L)
                                 && !eof_seen[cur_input.index_field]) {
                            cur_input.limit_field = first - 1;
                            eof_seen[cur_input.index_field] = true;
                            begin_token_list(eqtb[2252781L /*every_eof_loc */ ].hh.v.RH, 16 /*every_eof_text */ );
                            goto lab20;
                        } else
                            force_eof = true;
                    } else {

                        if (input_line(input_file[cur_input.index_field]))
                            firm_up_the_line();
                        else if ((eqtb[2252781L /*every_eof_loc */ ].hh.v.RH != -268435455L)
                                 && !eof_seen[cur_input.index_field]) {
                            cur_input.limit_field = first - 1;
                            eof_seen[cur_input.index_field] = true;
                            begin_token_list(eqtb[2252781L /*every_eof_loc */ ].hh.v.RH, 16 /*every_eof_text */ );
                            goto lab20;
                        } else
                            force_eof = true;
                    }
                }
                if (force_eof) {
                    if (eqtb[8938802L /*int_base 62 */ ].cint > 0) {

                        if ((grp_stack[in_open] != cur_boundary) || (if_stack[in_open] != cond_ptr))
                            file_warning();
                    }
                    if (cur_input.name_field >= 19) {
                        print_char(41 /*")" */ );
                        open_parens--;
                        fflush(stdout);
                    }
                    force_eof = false;
                    end_file_reading();
                    check_outer_validity();
                    goto lab20;
                }
                if ((eqtb[8938788L /*int_base 48 */ ].cint < 0) || (eqtb[8938788L /*int_base 48 */ ].cint > 255))
                    cur_input.limit_field--;
                else
                    buffer[cur_input.limit_field] = eqtb[8938788L /*int_base 48 */ ].cint;
                first = cur_input.limit_field + 1;
                cur_input.loc_field = cur_input.start_field;
            } else {

                if (!(cur_input.name_field == 0)) {
                    cur_cmd = 0;
                    cur_chr = 0;
                    return;
                }
                if (input_ptr > 0) {
                    end_file_reading();
                    goto lab20;
                }
                if (selector < SELECTOR_LOG_ONLY)
                    open_log_file();
                if (interaction > 1 /*nonstop_mode */ ) {
                    if ((eqtb[8938788L /*int_base 48 */ ].cint < 0) || (eqtb[8938788L /*int_base 48 */ ].cint > 255))
                        cur_input.limit_field++;
                    if (cur_input.limit_field == cur_input.start_field)
                        print_nl(65938L /*"(Please type a command or say `\end')" */ );
                    print_ln();
                    first = cur_input.start_field;
                    {
                        ;
                        print(42 /*"*" */ );
                        term_input();
                    }
                    cur_input.limit_field = last;
                    if ((eqtb[8938788L /*int_base 48 */ ].cint < 0) || (eqtb[8938788L /*int_base 48 */ ].cint > 255))
                        cur_input.limit_field--;
                    else
                        buffer[cur_input.limit_field] = eqtb[8938788L /*int_base 48 */ ].cint;
                    first = cur_input.limit_field + 1;
                    cur_input.loc_field = cur_input.start_field;
                } else
                    fatal_error(65939L /*"*** (job aborted, no legal \end found)" */ );
            }
            {
                if (interrupt != 0)
                    pause_for_instructions();
            }
            goto lab25;
        }
    } else /*369: */ if (cur_input.loc_field != -268435455L) {
        t = mem[cur_input.loc_field].hh.v.LH;
        cur_input.loc_field = mem[cur_input.loc_field].hh.v.RH;
        if (t >= 33554431L /*cs_token_flag */ ) {
            cur_cs = t - 33554431L;
            cur_cmd = eqtb[cur_cs].hh.u.B0;
            cur_chr = eqtb[cur_cs].hh.v.RH;
            if (cur_cmd >= 115 /*outer_call */ ) {

                if (cur_cmd == 118 /*dont_expand */ ) { /*370: */
                    cur_cs = mem[cur_input.loc_field].hh.v.LH - 33554431L;
                    cur_input.loc_field = -268435455L;
                    cur_cmd = eqtb[cur_cs].hh.u.B0;
                    cur_chr = eqtb[cur_cs].hh.v.RH;
                    if (cur_cmd > 102 /*max_command */ ) {
                        cur_cmd = 0 /*relax */ ;
                        cur_chr = 65537L /*no_expand_flag */ ;
                    }
                } else
                    check_outer_validity();
            }
        } else {

            cur_cmd = t / 2097152L /*max_char_val */ ;
            cur_chr = t % 2097152L /*max_char_val */ ;
            switch (cur_cmd) {
            case 1:
                align_state++;
                break;
            case 2:
                align_state--;
                break;
            case 5:
                {
                    begin_token_list(param_stack[cur_input.limit_field + cur_chr - 1], 0 /*parameter */ );
                    goto lab20;
                }
                break;
            default:
                ;
                break;
            }
        }
    } else {

        end_token_list();
        goto lab20;
    }
    if (cur_cmd <= 5 /*car_ret */ ) {

        if (cur_cmd >= 4 /*tab_mark */ ) {

            if (align_state == 0) {     /*818: */
                if ((scanner_status == 4 /*aligning */ ) || (cur_align == -268435455L))
                    fatal_error(65915L /*"(interwoven alignment preambles are not allowed)" */ );
                cur_cmd = mem[cur_align + 5].hh.v.LH;
                mem[cur_align + 5].hh.v.LH = cur_chr;
                if (cur_cmd == 63 /*omit */ )
                    begin_token_list(mem_top - 10, 2 /*v_template */ );
                else
                    begin_token_list(mem[cur_align + 2].cint, 2 /*v_template */ );
                align_state = 1000000L;
                goto lab20;
            }
        }
    }
}

void firm_up_the_line(void)
{
    firm_up_the_line_regmem integer k;
    cur_input.limit_field = last;
    if (eqtb[8938768L /*int_base 28 */ ].cint > 0) {

        if (interaction > 1 /*nonstop_mode */ ) {
            ;
            print_ln();
            if (cur_input.start_field < cur_input.limit_field) {
                register integer for_end;
                k = cur_input.start_field;
                for_end = cur_input.limit_field - 1;
                if (k <= for_end)
                    do
                        print(buffer[k]);
                    while (k++ < for_end);
            }
            first = cur_input.limit_field;
            {
                ;
                print(65940L /*"=>" */ );
                term_input();
            }
            if (last > first) {
                {
                    register integer for_end;
                    k = first;
                    for_end = last - 1;
                    if (k <= for_end)
                        do
                            buffer[k + cur_input.start_field - first] = buffer[k];
                        while (k++ < for_end);
                }
                cur_input.limit_field = cur_input.start_field + last - first;
            }
        }
    }
}

void get_token(void)
{
    get_token_regmem no_new_control_sequence = false;
    get_next();
    no_new_control_sequence = true;
    if (cur_cs == 0)
        cur_tok = (cur_cmd * 2097152L /*max_char_val */ ) + cur_chr;
    else
        cur_tok = 33554431L /*cs_token_flag */  + cur_cs;
}

void macro_call(void)
{
    macro_call_regmem halfword r;
    halfword p;
    halfword q;
    halfword s;
    halfword t;
    halfword u, v;
    halfword rbrace_ptr;
    small_number n;
    halfword unbalance;
    halfword m;
    halfword ref_count;
    small_number save_scanner_status;
    halfword save_warning_index;
    UTF16_code match_chr;
    save_scanner_status = scanner_status;
    save_warning_index = warning_index;
    warning_index = cur_cs;
    ref_count = cur_chr;
    r = mem[ref_count].hh.v.RH;
    n = 0;
    if (eqtb[8938770L /*int_base 30 */ ].cint > 0) {    /*419: */
        begin_diagnostic();
        print_ln();
        print_cs(warning_index);
        token_show(ref_count);
        end_diagnostic(false);
    }
    if (mem[r].hh.v.LH == 29360129L /*protected_token */ )
        r = mem[r].hh.v.RH;
    if (mem[r].hh.v.LH != 29360128L /*end_match_token */ ) {    /*409: */
        scanner_status = 3 /*matching */ ;
        unbalance = 0;
        long_state = eqtb[cur_cs].hh.u.B0;
        if (long_state >= 115 /*outer_call */ )
            long_state = long_state - 2;
        do {
            mem[mem_top - 3].hh.v.RH = -268435455L;
            if ((mem[r].hh.v.LH >= 29360128L /*end_match_token */ ) || (mem[r].hh.v.LH < 27262976L /*match_token */ ))
                s = -268435455L;
            else {

                match_chr = mem[r].hh.v.LH - 27262976L;
                s = mem[r].hh.v.RH;
                r = s;
                p = mem_top - 3;
                m = 0;
            }
 lab22:                        /*continue */ get_token();
            if (cur_tok == mem[r].hh.v.LH) {    /*412: */
                r = mem[r].hh.v.RH;
                if ((mem[r].hh.v.LH >= 27262976L /*match_token */ )
                    && (mem[r].hh.v.LH <= 29360128L /*end_match_token */ )) {
                    if (cur_tok < 4194304L /*left_brace_limit */ )
                        align_state--;
                    goto lab40;
                } else
                    goto lab22;
            }
            if (s != r) {

                if (s == -268435455L) { /*416: */
                    {
                        if (interaction == 3 /*error_stop_mode */ ) ;
                        if (file_line_error_style_p)
                            print_file_line();
                        else
                            print_nl(65544L /*"! " */ );
                        print(65973L /*"Use of " */ );
                    }
                    sprint_cs(warning_index);
                    print(65974L /*" doesn't match its definition" */ );
                    {
                        help_ptr = 4;
                        help_line[3] = 65975L /*"If you say, e.g., `\def\a1_..._', then you must always" */ ;
                        help_line[2] = 65976L /*"put `1' after `\a', since control sequence names are" */ ;
                        help_line[1] = 65977L /*"made up of letters only. The macro here has not been" */ ;
                        help_line[0] = 65978L /*"followed by the required stuff, so I'm ignoring it." */ ;
                    }
                    error();
                    goto exit;
                } else {

                    t = s;
                    do {
                        {
                            q = get_avail();
                            mem[p].hh.v.RH = q;
                            mem[q].hh.v.LH = mem[t].hh.v.LH;
                            p = q;
                        }
                        m++;
                        u = mem[t].hh.v.RH;
                        v = s;
                        while (true) {

                            if (u == r) {

                                if (cur_tok != mem[v].hh.v.LH)
                                    goto lab30;
                                else {

                                    r = mem[v].hh.v.RH;
                                    goto lab22;
                                }
                            }
                            if (mem[u].hh.v.LH != mem[v].hh.v.LH)
                                goto lab30;
                            u = mem[u].hh.v.RH;
                            v = mem[v].hh.v.RH;
                        }
 lab30:                        /*done */ t = mem[t].hh.v.RH;
                    } while (!(t == r));
                    r = s;
                }
            }
            if (cur_tok == par_token) {

                if (long_state != 114 /*long_call */ ) {        /*414: */
                    if (long_state == 113 /*call */ ) {
                        runaway();
                        {
                            if (interaction == 3 /*error_stop_mode */ ) ;
                            if (file_line_error_style_p)
                                print_file_line();
                            else
                                print_nl(65544L /*"! " */ );
                            print(65968L /*"Paragraph ended before " */ );
                        }
                        sprint_cs(warning_index);
                        print(65969L /*" was complete" */ );
                        {
                            help_ptr = 3;
                            help_line[2] = 65970L /*"I suspect you've forgotten a `_', causing me to apply this" */ ;
                            help_line[1] = 65971L /*"control sequence to too much text. How can we recover?" */ ;
                            help_line[0] = 65972L /*"My plan is to forget the whole thing and hope for the best." */ ;
                        }
                        back_error();
                    }
                    pstack[n] = mem[mem_top - 3].hh.v.RH;
                    align_state = align_state - unbalance;
                    {
                        register integer for_end;
                        m = 0;
                        for_end = n;
                        if (m <= for_end)
                            do
                                flush_list(pstack[m]);
                            while (m++ < for_end);
                    }
                    goto exit;
                }
            }
            if (cur_tok < 6291456L /*right_brace_limit */ ) {

                if (cur_tok < 4194304L /*left_brace_limit */ ) {        /*417: */
                    unbalance = 1;
                    while (true) {

                        {
                            {
                                q = avail;
                                if (q == -268435455L)
                                    q = get_avail();
                                else {

                                    avail = mem[q].hh.v.RH;
                                    mem[q].hh.v.RH = -268435455L;
                                }
                            }
                            mem[p].hh.v.RH = q;
                            mem[q].hh.v.LH = cur_tok;
                            p = q;
                        }
                        get_token();
                        if (cur_tok == par_token) {

                            if (long_state != 114 /*long_call */ ) {    /*414: */
                                if (long_state == 113 /*call */ ) {
                                    runaway();
                                    {
                                        if (interaction == 3 /*error_stop_mode */ ) ;
                                        if (file_line_error_style_p)
                                            print_file_line();
                                        else
                                            print_nl(65544L /*"! " */ );
                                        print(65968L /*"Paragraph ended before " */ );
                                    }
                                    sprint_cs(warning_index);
                                    print(65969L /*" was complete" */ );
                                    {
                                        help_ptr = 3;
                                        help_line[2] =
                                            65970L /*"I suspect you've forgotten a `_', causing me to apply this" */ ;
                                        help_line[1] =
                                            65971L /*"control sequence to too much text. How can we recover?" */ ;
                                        help_line[0] =
                                            65972L /*"My plan is to forget the whole thing and hope for the best." */ ;
                                    }
                                    back_error();
                                }
                                pstack[n] = mem[mem_top - 3].hh.v.RH;
                                align_state = align_state - unbalance;
                                {
                                    register integer for_end;
                                    m = 0;
                                    for_end = n;
                                    if (m <= for_end)
                                        do
                                            flush_list(pstack[m]);
                                        while (m++ < for_end);
                                }
                                goto exit;
                            }
                        }
                        if (cur_tok < 6291456L /*right_brace_limit */ ) {

                            if (cur_tok < 4194304L /*left_brace_limit */ )
                                unbalance++;
                            else {

                                unbalance--;
                                if (unbalance == 0)
                                    goto lab31;
                            }
                        }
                    }
 lab31:                        /*done1 */ rbrace_ptr = p;
                    {
                        q = get_avail();
                        mem[p].hh.v.RH = q;
                        mem[q].hh.v.LH = cur_tok;
                        p = q;
                    }
                } else {        /*413: */

                    back_input();
                    {
                        if (interaction == 3 /*error_stop_mode */ ) ;
                        if (file_line_error_style_p)
                            print_file_line();
                        else
                            print_nl(65544L /*"! " */ );
                        print(65960L /*"Argument of " */ );
                    }
                    sprint_cs(warning_index);
                    print(65961L /*" has an extra _" */ );
                    {
                        help_ptr = 6;
                        help_line[5] = 65962L /*"I've run across a `_' that doesn't seem to match anything." */ ;
                        help_line[4] = 65963L /*"For example, `\def\a#1_..._' and `\a_' would produce" */ ;
                        help_line[3] = 65964L /*"this error. If you simply proceed now, the `\par' that" */ ;
                        help_line[2] = 65965L /*"I've just inserted will cause me to report a runaway" */ ;
                        help_line[1] = 65966L /*"argument that might be the root of the problem. But if" */ ;
                        help_line[0] = 65967L /*"your `_' was spurious, just type `2' and it will go away." */ ;
                    }
                    align_state++;
                    long_state = 113 /*call */ ;
                    cur_tok = par_token;
                    ins_error();
                    goto lab22;
                }
            } else {            /*411: */

                if (cur_tok == 20971552L /*space_token */ ) {

                    if (mem[r].hh.v.LH <= 29360128L /*end_match_token */ ) {

                        if (mem[r].hh.v.LH >= 27262976L /*match_token */ )
                            goto lab22;
                    }
                }
                {
                    q = get_avail();
                    mem[p].hh.v.RH = q;
                    mem[q].hh.v.LH = cur_tok;
                    p = q;
                }
            }
            m++;
            if (mem[r].hh.v.LH > 29360128L /*end_match_token */ )
                goto lab22;
            if (mem[r].hh.v.LH < 27262976L /*match_token */ )
                goto lab22;
 lab40:                                        /*found */ if (s != -268435455L) {
                                                /*418: */
                if ((m == 1) && (mem[p].hh.v.LH < 6291456L /*right_brace_limit */ ) && (p != mem_top - 3)) {
                    mem[rbrace_ptr].hh.v.RH = -268435455L;
                    {
                        mem[p].hh.v.RH = avail;
                        avail = p;
                    }
                    p = mem[mem_top - 3].hh.v.RH;
                    pstack[n] = mem[p].hh.v.RH;
                    {
                        mem[p].hh.v.RH = avail;
                        avail = p;
                    }
                } else
                    pstack[n] = mem[mem_top - 3].hh.v.RH;
                n++;
                if (eqtb[8938770L /*int_base 30 */ ].cint > 0) {
                    begin_diagnostic();
                    print_nl(match_chr);
                    print_int(n);
                    print(65979L /*"<-" */ );
                    show_token_list(pstack[n - 1], -268435455L, 1000);
                    end_diagnostic(false);
                }
            }
        } while (!(mem[r].hh.v.LH == 29360128L /*end_match_token */ ));
    }
    while ((cur_input.state_field == 0 /*token_list */ ) && (cur_input.loc_field == -268435455L)
           && (cur_input.index_field != 2 /*v_template */ ))
        end_token_list();
    begin_token_list(ref_count, 6 /*macro */ );
    cur_input.name_field = warning_index;
    cur_input.loc_field = mem[r].hh.v.RH;
    if (n > 0) {
        if (param_ptr + n > max_param_stack) {
            max_param_stack = param_ptr + n;
            if (max_param_stack > param_size)
                overflow(65959L /*"parameter stack size" */ , param_size);
        }
        {
            register integer for_end;
            m = 0;
            for_end = n - 1;
            if (m <= for_end)
                do
                    param_stack[param_ptr + m] = pstack[m];
                while (m++ < for_end);
        }
        param_ptr = param_ptr + n;
    }

exit:
    scanner_status = save_scanner_status;
    warning_index = save_warning_index;
}

void insert_relax(void)
{
    insert_relax_regmem cur_tok = 33554431L /*cs_token_flag */  + cur_cs;
    back_input();
    cur_tok = 35797664L /*cs_token_flag 2243233 */ ;
    back_input();
    cur_input.index_field = 5 /*inserted */ ;
}

void znew_index(quarterword i, halfword q)
{
    new_index_regmem small_number k;
    cur_ptr = get_node(33 /*index_node_size */ );
    mem[cur_ptr].hh.u.B0 = i;
    mem[cur_ptr].hh.u.B1 = 0;
    mem[cur_ptr].hh.v.RH = q;
    {
        register integer for_end;
        k = 1;
        for_end = 32 /*index_node_size -1 */ ;
        if (k <= for_end)
            do
                mem[cur_ptr + k] = sa_null;
            while (k++ < for_end);
    }
}

void zfind_sa_element(small_number t, halfword n, boolean w)
{
    find_sa_element_regmem halfword q;
    small_number i;
    cur_ptr = sa_root[t];
    {
        if (cur_ptr == -268435455L) {

            if (w)
                goto lab45;
            else
                return;
        }
    }
    q = cur_ptr;
    i = n / 262144L;
    if (odd(i))
        cur_ptr = mem[q + (i / 2) + 1].hh.v.RH;
    else
        cur_ptr = mem[q + (i / 2) + 1].hh.v.LH;
    {
        if (cur_ptr == -268435455L) {

            if (w)
                goto lab46;
            else
                return;
        }
    }
    q = cur_ptr;
    i = (n / 4096) % 64;
    if (odd(i))
        cur_ptr = mem[q + (i / 2) + 1].hh.v.RH;
    else
        cur_ptr = mem[q + (i / 2) + 1].hh.v.LH;
    {
        if (cur_ptr == -268435455L) {

            if (w)
                goto lab47;
            else
                return;
        }
    }
    q = cur_ptr;
    i = (n / 64) % 64;
    if (odd(i))
        cur_ptr = mem[q + (i / 2) + 1].hh.v.RH;
    else
        cur_ptr = mem[q + (i / 2) + 1].hh.v.LH;
    {
        if (cur_ptr == -268435455L) {

            if (w)
                goto lab48;
            else
                return;
        }
    }
    q = cur_ptr;
    i = n % 64;
    if (odd(i))
        cur_ptr = mem[q + (i / 2) + 1].hh.v.RH;
    else
        cur_ptr = mem[q + (i / 2) + 1].hh.v.LH;
    if ((cur_ptr == -268435455L) && w)
        goto lab49;
    return;
 lab45:                        /*not_found */ new_index(t, -268435455L);
    sa_root[t] = cur_ptr;
    q = cur_ptr;
    i = n / 262144L;
 lab46:                        /*not_found1 */ new_index(i, q);
    {
        if (odd(i))
            mem[q + (i / 2) + 1].hh.v.RH = cur_ptr;
        else
            mem[q + (i / 2) + 1].hh.v.LH = cur_ptr;
        mem[q].hh.u.B1++;
    }
    q = cur_ptr;
    i = (n / 4096) % 64;
 lab47:                        /*not_found2 */ new_index(i, q);
    {
        if (odd(i))
            mem[q + (i / 2) + 1].hh.v.RH = cur_ptr;
        else
            mem[q + (i / 2) + 1].hh.v.LH = cur_ptr;
        mem[q].hh.u.B1++;
    }
    q = cur_ptr;
    i = (n / 64) % 64;
 lab48:                        /*not_found3 */ new_index(i, q);
    {
        if (odd(i))
            mem[q + (i / 2) + 1].hh.v.RH = cur_ptr;
        else
            mem[q + (i / 2) + 1].hh.v.LH = cur_ptr;
        mem[q].hh.u.B1++;
    }
    q = cur_ptr;
    i = n % 64;
 lab49:/*not_found4 *//*1608: */ if (t == 7 /*mark_val */ ) {
        cur_ptr = get_node(4 /*mark_class_node_size */ );
        mem[cur_ptr + 1] = sa_null;
        mem[cur_ptr + 2] = sa_null;
        mem[cur_ptr + 3] = sa_null;
    } else {

        if (t <= 1 /*dimen_val */ ) {
            cur_ptr = get_node(3 /*word_node_size */ );
            mem[cur_ptr + 2].cint = 0;
            mem[cur_ptr + 1].hh.v.RH = n;
        } else {

            cur_ptr = get_node(2 /*pointer_node_size */ );
            if (t <= 3 /*mu_val */ ) {
                mem[cur_ptr + 1].hh.v.RH = mem_bot;
                mem[mem_bot].hh.v.RH++;
            } else
                mem[cur_ptr + 1].hh.v.RH = -268435455L;
        }
        mem[cur_ptr + 1].hh.v.LH = -268435455L;
    }
    mem[cur_ptr].hh.u.B0 = 64 * t + i;
    mem[cur_ptr].hh.u.B1 = 1 /*level_one *//*:1608 */ ;
    mem[cur_ptr].hh.v.RH = q;
    {
        if (odd(i))
            mem[q + (i / 2) + 1].hh.v.RH = cur_ptr;
        else
            mem[q + (i / 2) + 1].hh.v.LH = cur_ptr;
        mem[q].hh.u.B1++;
    }
}

void expand(void)
{
    expand_regmem halfword t;
    boolean b;
    halfword p, q, r;
    integer j;
    integer cv_backup;
    small_number cvl_backup, radix_backup, co_backup;
    halfword backup_backup;
    small_number save_scanner_status;
    expand_depth_count++;
    if (expand_depth_count >= expand_depth)
        overflow(65941L /*"expansion depth" */ , expand_depth);
    cv_backup = cur_val;
    cvl_backup = cur_val_level;
    radix_backup = radix;
    co_backup = cur_order;
    backup_backup = mem[mem_top - 13].hh.v.RH;
 lab21:/*reswitch */ if (cur_cmd < 113 /*call */ ) {   /*384: */
        if (eqtb[8938776L /*int_base 36 */ ].cint > 1)
            show_cur_cmd_chr();
        switch (cur_cmd) {
        case 112:
            {
                t = cur_chr % 5;
                if (cur_chr >= 5)
                    scan_register_num();
                else
                    cur_val = 0;
                if (cur_val == 0)
                    cur_ptr = cur_mark[t];
                else {          /*1612: */

                    find_sa_element(7 /*mark_val */ , cur_val, false);
                    if (cur_ptr != -268435455L) {

                        if (odd(t))
                            cur_ptr = mem[cur_ptr + (t / 2) + 1].hh.v.RH;
                        else
                            cur_ptr = mem[cur_ptr + (t / 2) + 1].hh.v.LH;
                    }
                }
                if (cur_ptr != -268435455L)
                    begin_token_list(cur_ptr, 15 /*mark_text */ );
            }
            break;
        case 104:
            if (cur_chr == 0) { /*385: */
                get_token();
                t = cur_tok;
                get_token();
                if (cur_cmd > 102 /*max_command */ )
                    expand();
                else
                    back_input();
                cur_tok = t;
                back_input();
            } else {            /*1553: */

                get_token();
                if ((cur_cmd == 107 /*if_test */ ) && (cur_chr != 16 /*if_case_code */ )) {
                    cur_chr = cur_chr + 32;
                    goto lab21;
                }
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66020L /*"You can't use `" */ );
                }
                print_esc(66130L /*"unless" */ );
                print(66908L /*"' before `" */ );
                print_cmd_chr(cur_cmd, cur_chr);
                print_char(39 /*"'" */ );
                {
                    help_ptr = 1;
                    help_line[0] = 65937L /*"Continue, and I'll forget that it ever happened." */ ;
                }
                back_error();
            }
            break;
        case 105:
            if (cur_chr == 0) { /*386: */
                save_scanner_status = scanner_status;
                scanner_status = 0 /*normal */ ;
                get_token();
                scanner_status = save_scanner_status;
                t = cur_tok;
                back_input();
                if (t >= 33554431L /*cs_token_flag */ ) {
                    p = get_avail();
                    mem[p].hh.v.LH = 35797666L /*cs_token_flag 2243235 */ ;
                    mem[p].hh.v.RH = cur_input.loc_field;
                    cur_input.start_field = p;
                    cur_input.loc_field = p;
                }
            } else {            /*387: */

                save_scanner_status = scanner_status;
                scanner_status = 0 /*normal */ ;
                get_token();
                scanner_status = save_scanner_status;
                if (cur_cs < 2228226L /*hash_base */ )
                    cur_cs = prim_lookup(cur_cs - 257);
                else
                    cur_cs = prim_lookup(hash[cur_cs].v.RH);
                if (cur_cs != 0 /*undefined_primitive */ ) {
                    t = prim_eqtb[cur_cs].hh.u.B0;
                    if (t > 102 /*max_command */ ) {
                        cur_cmd = t;
                        cur_chr = prim_eqtb[cur_cs].hh.v.RH;
                        cur_tok = (cur_cmd * 2097152L /*max_char_val */ ) + cur_chr;
                        cur_cs = 0;
                        goto lab21;
                    } else {

                        back_input();
                        p = get_avail();
                        mem[p].hh.v.LH = 35797668L /*cs_token_flag 2243237 */ ;
                        mem[p].hh.v.RH = cur_input.loc_field;
                        cur_input.loc_field = p;
                        cur_input.start_field = p;
                    }
                }
            }
            break;
        case 109:
            {
                r = get_avail();
                p = r;
                b = is_in_csname;
                is_in_csname = true;
                do {
                    get_x_token();
                    if (cur_cs == 0) {
                        q = get_avail();
                        mem[p].hh.v.RH = q;
                        mem[q].hh.v.LH = cur_tok;
                        p = q;
                    }
                } while (!(cur_cs != 0));
                if (cur_cmd != 67 /*end_cs_name */ ) {  /*391: */
                    {
                        if (interaction == 3 /*error_stop_mode */ ) ;
                        if (file_line_error_style_p)
                            print_file_line();
                        else
                            print_nl(65544L /*"! " */ );
                        print(65948L /*"Missing " */ );
                    }
                    print_esc(65810L /*"endcsname" */ );
                    print(65949L /*" inserted" */ );
                    {
                        help_ptr = 2;
                        help_line[1] = 65950L /*"The control sequence marked <to be read again> should" */ ;
                        help_line[0] = 65951L /*"not appear between \csname and \endcsname." */ ;
                    }
                    back_error();
                }
                is_in_csname = b;
                j = first;
                p = mem[r].hh.v.RH;
                while (p != -268435455L) {

                    if (j >= max_buf_stack) {
                        max_buf_stack = j + 1;
                        if (max_buf_stack == buf_size)
                            overflow(65538L /*"buffer size" */ , buf_size);
                    }
                    buffer[j] = mem[p].hh.v.LH % 2097152L /*max_char_val */ ;
                    j++;
                    p = mem[p].hh.v.RH;
                }
                if ((j > first + 1) || (buffer[first] > 65535L)) {
                    no_new_control_sequence = false;
                    cur_cs = id_lookup(first, j - first);
                    no_new_control_sequence = true;
                } else if (j == first)
                    cur_cs = 2228225L /*null_cs */ ;
                else
                    cur_cs = 1114113L /*single_base */  + buffer[first] /*:392 */ ;
                flush_list(r);
                if (eqtb[cur_cs].hh.u.B0 == 103 /*undefined_cs */ ) {
                    eq_define(cur_cs, 0 /*relax */ , 1114112L /*too_big_usv */ );
                }
                cur_tok = cur_cs + 33554431L;
                back_input();
            }
            break;
        case 110:
            conv_toks();
            break;
        case 111:
            ins_the_toks();
            break;
        case 107:
            conditional();
            break;
        case 108:
            {
                if (eqtb[8938800L /*int_base 60 */ ].cint > 0) {

                    if (eqtb[8938776L /*int_base 36 */ ].cint <= 1)
                        show_cur_cmd_chr();
                }
                if (cur_chr > if_limit) {

                    if (if_limit == 1 /*if_code */ )
                        insert_relax();
                    else {

                        {
                            if (interaction == 3 /*error_stop_mode */ ) ;
                            if (file_line_error_style_p)
                                print_file_line();
                            else
                                print_nl(65544L /*"! " */ );
                            print(66134L /*"Extra " */ );
                        }
                        print_cmd_chr(108 /*fi_or_else */ , cur_chr);
                        {
                            help_ptr = 1;
                            help_line[0] = 66135L /*"I'm ignoring this; it doesn't match any \if." */ ;
                        }
                        error();
                    }
                } else {

                    while (cur_chr != 2 /*fi_code */ )
                        pass_text();
                    {
                        if (if_stack[in_open] == cond_ptr)
                            if_warning();
                        p = cond_ptr;
                        if_line = mem[p + 1].cint;
                        cur_if = mem[p].hh.u.B1;
                        if_limit = mem[p].hh.u.B0;
                        cond_ptr = mem[p].hh.v.RH;
                        free_node(p, 2 /*if_node_size */ );
                    }
                }
            }
            break;
        case 106:
            if (cur_chr == 1)
                force_eof = true /*1537: */ ;
            else if (cur_chr == 2)
                pseudo_start();
            else if (name_in_progress)
                insert_relax();
            else
                start_input();
            break;
        default:
            {
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(65942L /*"Undefined control sequence" */ );
                }
                {
                    help_ptr = 5;
                    help_line[4] = 65943L /*"The control sequence at the end of the top line" */ ;
                    help_line[3] = 65944L /*"of your error message was never \def'ed. If you have" */ ;
                    help_line[2] = 65945L /*"misspelled it (e.g., `\hobx'), type `I' and the correct" */ ;
                    help_line[1] = 65946L /*"spelling (e.g., `I\hbox'). Otherwise just continue," */ ;
                    help_line[0] = 65947L /*"and I'll forget about whatever was undefined." */ ;
                }
                error();
            }
            break;
        }
    } else if (cur_cmd < 117 /*end_template */ )
        macro_call();
    else {                      /*393: */

        cur_tok = 35797663L /*cs_token_flag 2243232 */ ;
        back_input();
    }
    cur_val = cv_backup;
    cur_val_level = cvl_backup;
    radix = radix_backup;
    cur_order = co_backup;
    mem[mem_top - 13].hh.v.RH = backup_backup;
    expand_depth_count--;
}

void get_x_token(void)
{
 get_x_token_regmem lab20:     /*restart */ get_next();
    if (cur_cmd <= 102 /*max_command */ )
        goto lab30;
    if (cur_cmd >= 113 /*call */ ) {

        if (cur_cmd < 117 /*end_template */ )
            macro_call();
        else {

            cur_cs = 2243232L /*frozen_endv */ ;
            cur_cmd = 9 /*endv */ ;
            goto lab30;
        }
    } else
        expand();
    goto lab20;
 lab30:                        /*done */ if (cur_cs == 0)
        cur_tok = (cur_cmd * 2097152L /*max_char_val */ ) + cur_chr;
    else
        cur_tok = 33554431L /*cs_token_flag */  + cur_cs;
}

void x_token(void)
{
    x_token_regmem while (cur_cmd > 102 /*max_command */ ) {

        expand();
        get_next();
    }
    if (cur_cs == 0)
        cur_tok = (cur_cmd * 2097152L /*max_char_val */ ) + cur_chr;
    else
        cur_tok = 33554431L /*cs_token_flag */  + cur_cs;
}

void scan_left_brace(void)
{
    scan_left_brace_regmem
    do {
        get_x_token();
    } while (!((cur_cmd != 10 /*spacer */ ) && (cur_cmd != 0 /*relax */ ) /*:422 */ ));
    if (cur_cmd != 1 /*left_brace */ ) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(65980L /*"Missing _ inserted" */ );
        }
        {
            help_ptr = 4;
            help_line[3] = 65981L /*"A left brace was mandatory here, so I've put one in." */ ;
            help_line[2] = 65982L /*"You might want to delete and/or insert some corrections" */ ;
            help_line[1] = 65983L /*"so that I will find a matching right brace soon." */ ;
            help_line[0] = 65984L /*"(If you're confused by all this, try typing `I_' now.)" */ ;
        }
        back_error();
        cur_tok = 2097275L /*left_brace_token 123 */ ;
        cur_cmd = 1 /*left_brace */ ;
        cur_chr = 123 /*"_" */ ;
        align_state++;
    }
}

void scan_optional_equals(void)
{
    scan_optional_equals_regmem
    do {
        get_x_token();
    } while (!(cur_cmd != 10 /*spacer *//*:424 */ ));
    if (cur_tok != 25165885L /*other_token 61 */ )
        back_input();
}

boolean zscan_keyword(str_number s)
{
    register boolean Result;
    scan_keyword_regmem halfword p;
    halfword q;
    pool_pointer k;
    p = mem_top - 13;
    mem[p].hh.v.RH = -268435455L;
    if (s < 65536L /*too_big_char */ ) {
        while (true) {

            get_x_token();
            if ((cur_cs == 0) && ((cur_chr == s) || (cur_chr == s - 32))) {
                {
                    q = get_avail();
                    mem[p].hh.v.RH = q;
                    mem[q].hh.v.LH = cur_tok;
                    p = q;
                }
                flush_list(mem[mem_top - 13].hh.v.RH);
                Result = true;
                return Result;
            } else if ((cur_cmd != 10 /*spacer */ ) || (p != mem_top - 13)) {
                back_input();
                if (p != mem_top - 13)
                    begin_token_list(mem[mem_top - 13].hh.v.RH, 3 /*backed_up */ );
                Result = false;
                return Result;
            }
        }
    }
    k = str_start[(s) - 65536L];
    while (k < str_start[(s + 1) - 65536L]) {

        get_x_token();
        if ((cur_cs == 0) && ((cur_chr == str_pool[k]) || (cur_chr == str_pool[k] - 32))) {
            {
                q = get_avail();
                mem[p].hh.v.RH = q;
                mem[q].hh.v.LH = cur_tok;
                p = q;
            }
            k++;
        } else if ((cur_cmd != 10 /*spacer */ ) || (p != mem_top - 13)) {
            back_input();
            if (p != mem_top - 13)
                begin_token_list(mem[mem_top - 13].hh.v.RH, 3 /*backed_up */ );
            Result = false;
            return Result;
        }
    }
    flush_list(mem[mem_top - 13].hh.v.RH);
    Result = true;
    return Result;
}

void mu_error(void)
{
    mu_error_regmem {
        if (interaction == 3 /*error_stop_mode */ ) ;
        if (file_line_error_style_p)
            print_file_line();
        else
            print_nl(65544L /*"! " */ );
        print(65985L /*"Incompatible glue units" */ );
    }
    {
        help_ptr = 1;
        help_line[0] = 65986L /*"I'm going to assume that 1mu=1pt when they're mixed." */ ;
    }
    error();
}

void zscan_glyph_number(internal_font_number f)
{
    scan_glyph_number_regmem if (scan_keyword(47 /*"/" */ )) {
        scan_and_pack_name();
        {
            cur_val = map_glyph_to_index(f);
            cur_val_level = 0 /*int_val */ ;
        }
    } else if (scan_keyword(117 /*"u" */ )) {
        scan_char_num();
        {
            cur_val = map_char_to_glyph(f, cur_val);
            cur_val_level = 0 /*int_val */ ;
        }
    } else
        scan_int();
}

void scan_char_class(void)
{
    scan_char_class_regmem scan_int();
    if ((cur_val < 0) || (cur_val > 4096 /*char_class_limit */ )) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66022L /*"Bad character class" */ );
        }
        {
            help_ptr = 2;
            help_line[1] = 66023L /*"A character class must be between 0 and 4096." */ ;
            help_line[0] = 65995L /*"I changed this one to zero." */ ;
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_char_class_not_ignored(void)
{
    scan_char_class_not_ignored_regmem scan_int();
    if ((cur_val < 0) || (cur_val > 4096 /*char_class_limit */ )) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66022L /*"Bad character class" */ );
        }
        {
            help_ptr = 2;
            help_line[1] = 66024L /*"A class for inter-character transitions must be between 0 and 4095." */ ;
            help_line[0] = 65995L /*"I changed this one to zero." */ ;
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_eight_bit_int(void)
{
    scan_eight_bit_int_regmem scan_int();
    if ((cur_val < 0) || (cur_val > 255)) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66025L /*"Bad register code" */ );
        }
        {
            help_ptr = 2;
            help_line[1] = 66026L /*"A register code or char class must be between 0 and 255." */ ;
            help_line[0] = 65995L /*"I changed this one to zero." */ ;
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_usv_num(void)
{
    scan_usv_num_regmem scan_int();
    if ((cur_val < 0) || (cur_val > 1114111L /*biggest_usv */ )) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66027L /*"Bad character code" */ );
        }
        {
            help_ptr = 2;
            help_line[1] = 66028L /*"A Unicode scalar value must be between 0 and "10FFFF." */ ;
            help_line[0] = 65995L /*"I changed this one to zero." */ ;
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_char_num(void)
{
    scan_char_num_regmem scan_int();
    if ((cur_val < 0) || (cur_val > 65535L /*biggest_char */ )) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66027L /*"Bad character code" */ );
        }
        {
            help_ptr = 2;
            help_line[1] = 66029L /*"A character number must be between 0 and 65535." */ ;
            help_line[0] = 65995L /*"I changed this one to zero." */ ;
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_xetex_math_char_int(void)
{
    scan_xetex_math_char_int_regmem scan_int();
    if (math_char_field(cur_val) == 2097151L /*active_math_char */ ) {
        if (cur_val != 2097151L /*active_math_char */ ) {
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66030L /*"Bad active XeTeX math code" */ );
            }
            {
                help_ptr = 2;
                help_line[1] = 66031L /*"Since I ignore class and family for active math chars," */ ;
                help_line[0] = 66032L /*"I changed this one to "1FFFFF." */ ;
            }
            int_error(cur_val);
            cur_val = 2097151L /*active_math_char */ ;
        }
    } else if (math_char_field(cur_val) > 1114111L /*biggest_usv */ ) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66033L /*"Bad XeTeX math character code" */ );
        }
        {
            help_ptr = 2;
            help_line[1] = 66034L /*"Since I expected a character number between 0 and "10FFFF," */ ;
            help_line[0] = 65995L /*"I changed this one to zero." */ ;
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_math_class_int(void)
{
    scan_math_class_int_regmem scan_int();
    if ((cur_val < 0) || (cur_val > 7)) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66035L /*"Bad math class" */ );
        }
        {
            help_ptr = 2;
            help_line[1] = 66036L /*"Since I expected to read a number between 0 and 7," */ ;
            help_line[0] = 65995L /*"I changed this one to zero." */ ;
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_math_fam_int(void)
{
    scan_math_fam_int_regmem scan_int();
    if ((cur_val < 0) || (cur_val > 255 /*number_math_families -1 */ )) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66037L /*"Bad math family" */ );
        }
        {
            help_ptr = 2;
            help_line[1] = 66038L /*"Since I expected to read a number between 0 and 255," */ ;
            help_line[0] = 65995L /*"I changed this one to zero." */ ;
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_four_bit_int(void)
{
    scan_four_bit_int_regmem scan_int();
    if ((cur_val < 0) || (cur_val > 15)) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66039L /*"Bad number" */ );
        }
        {
            help_ptr = 2;
            help_line[1] = 66040L /*"Since I expected to read a number between 0 and 15," */ ;
            help_line[0] = 65995L /*"I changed this one to zero." */ ;
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_fifteen_bit_int(void)
{
    scan_fifteen_bit_int_regmem scan_int();
    if ((cur_val < 0) || (cur_val > 32767)) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66041L /*"Bad mathchar" */ );
        }
        {
            help_ptr = 2;
            help_line[1] = 66042L /*"A mathchar number must be between 0 and 32767." */ ;
            help_line[0] = 65995L /*"I changed this one to zero." */ ;
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_delimiter_int(void)
{
    scan_delimiter_int_regmem scan_int();
    if ((cur_val < 0) || (cur_val > 134217727L)) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66043L /*"Bad delimiter code" */ );
        }
        {
            help_ptr = 2;
            help_line[1] = 66044L /*"A numeric delimiter code must be between 0 and 2^_27_-1." */ ;
            help_line[0] = 65995L /*"I changed this one to zero." */ ;
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_register_num(void)
{
    scan_register_num_regmem scan_int();
    if ((cur_val < 0) || (cur_val > max_reg_num)) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66025L /*"Bad register code" */ );
        }
        {
            help_ptr = 2;
            help_line[1] = max_reg_help_line;
            help_line[0] = 65995L /*"I changed this one to zero." */ ;
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_four_bit_int_or_18(void)
{
    scan_four_bit_int_or_18_regmem scan_int();
    if ((cur_val < 0) || ((cur_val > 15) && (cur_val != 18))) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66039L /*"Bad number" */ );
        }
        {
            help_ptr = 2;
            help_line[1] = 66040L /*"Since I expected to read a number between 0 and 15," */ ;
            help_line[0] = 65995L /*"I changed this one to zero." */ ;
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void get_x_or_protected(void)
{
    get_x_or_protected_regmem while (true) {

        get_token();
        if (cur_cmd <= 102 /*max_command */ )
            return;
        if ((cur_cmd >= 113 /*call */ ) && (cur_cmd < 117 /*end_template */ )) {

            if (mem[mem[cur_chr].hh.v.RH].hh.v.LH == 29360129L /*protected_token */ )
                return;
        }
        expand();
    }
}

integer zeffective_char(boolean err_p, internal_font_number f, quarterword c)
{
    register integer Result;
    effective_char_regmem integer base_c;
    integer result;
    if ((!xtx_ligature_present) && (font_mapping[f] != NULL))
        c = apply_tfm_font_mapping(font_mapping[f], c);
    xtx_ligature_present = false;
    result = c;
    if (!mltex_enabled_p)
        goto lab40;
    if (font_ec[f] >= c) {

        if (font_bc[f] <= c) {

            if ((font_info[char_base[f] + c].qqqq.u.B0 > 0 /*min_quarterword */ ))
                goto lab40;
        }
    }
    if (c >= eqtb[8938795L /*int_base 55 */ ].cint) {

        if (c <= eqtb[8938796L /*int_base 56 */ ].cint) {

            if ((eqtb[7824628L /*char_sub_code_base */  + c].hh.v.RH > 0)) {
                base_c = (eqtb[7824628L /*char_sub_code_base */  + c].hh.v.RH % 256);
                result = base_c;
                if (!err_p)
                    goto lab40;
                if (font_ec[f] >= base_c) {

                    if (font_bc[f] <= base_c) {

                        if ((font_info[char_base[f] + base_c].qqqq.u.B0 > 0 /*min_quarterword */ ))
                            goto lab40;
                    }
                }
            }
        }
    }
    if (err_p) {
        begin_diagnostic();
        print_nl(66186L /*"Missing character: There is no " */ );
        print(66942L /*"substitution for " */ );
        print(c);
        print(66187L /*" in font " */ );
        print(font_name[f]);
        print_char(33 /*"!" */ );
        end_diagnostic(false);
        result = font_bc[f];
    }
 lab40:                        /*found */ Result = result;
    return Result;
}

void scan_font_ident(void)
{
    scan_font_ident_regmem internal_font_number f;
    halfword m;
    do {
        get_x_token();
    } while (!(cur_cmd != 10 /*spacer *//*:424 */ ));
    if (cur_cmd == 90 /*def_font */ )
        f = eqtb[2253299L /*cur_font_loc */ ].hh.v.RH;
    else if (cur_cmd == 89 /*set_font */ )
        f = cur_chr;
    else if (cur_cmd == 88 /*def_family */ ) {
        m = cur_chr;
        scan_math_fam_int();
        f = eqtb[m + cur_val].hh.v.RH;
    } else {

        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66178L /*"Missing font identifier" */ );
        }
        {
            help_ptr = 2;
            help_line[1] = 66179L /*"I was looking for a control sequence whose" */ ;
            help_line[0] = 66180L /*"current meaning has been defined by \font." */ ;
        }
        back_error();
        f = 0 /*font_base */ ;
    }
    cur_val = f;
}

void zfind_font_dimen(boolean writing)
{
    find_font_dimen_regmem internal_font_number f;
    integer n;
    scan_int();
    n = cur_val;
    scan_font_ident();
    f = cur_val;
    if (n <= 0)
        cur_val = fmem_ptr;
    else {

        if (writing && (n <= 4 /*space_shrink_code */ ) && (n >= 2 /*space_code */ ) && (font_glue[f] != -268435455L)) {
            delete_glue_ref(font_glue[f]);
            font_glue[f] = -268435455L;
        }
        if (n > font_params[f]) {

            if (f < font_ptr)
                cur_val = fmem_ptr;
            else {              /*599: */

                do {
                    if (fmem_ptr == font_mem_size)
                        overflow(66185L /*"font memory" */ , font_mem_size);
                    font_info[fmem_ptr].cint = 0;
                    fmem_ptr++;
                    font_params[f]++;
                } while (!(n == font_params[f]));
                cur_val = fmem_ptr - 1;
            }
        } else
            cur_val = n + param_base[f];
    }
    if (cur_val == fmem_ptr) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66164L /*"Font " */ );
        }
        print_esc(hash[2243238L /*font_id_base */  + f].v.RH);
        print(66181L /*" has only " */ );
        print_int(font_params[f]);
        print(66182L /*" fontdimen parameters" */ );
        {
            help_ptr = 2;
            help_line[1] = 66183L /*"To increase the number of font parameters, you must" */ ;
            help_line[0] = 66184L /*"use \fontdimen immediately after the \font is loaded." */ ;
        }
        error();
    }
}

void zscan_something_internal(small_number level, boolean negative)
{
    scan_something_internal_regmem halfword m;
    integer n, k, kk;
    halfword q, r;
    halfword tx;
    four_quarters i;
    integer p;
    m = cur_chr;
    switch (cur_cmd) {
    case 86:
        {
            scan_usv_num();
            if (m == 6710516L /*math_code_base */ ) {
                cur_val1 = eqtb[6710516L /*math_code_base */  + cur_val].hh.v.RH;
                if (math_char_field(cur_val1) == 2097151L /*active_math_char */ )
                    cur_val1 = 32768L;
                else if ((math_class_field(cur_val1) > 7) || (math_fam_field(cur_val1) > 15)
                         || (math_char_field(cur_val1) > 255)) {
                    {
                        if (interaction == 3 /*error_stop_mode */ ) ;
                        if (file_line_error_style_p)
                            print_file_line();
                        else
                            print_nl(65544L /*"! " */ );
                        print(65993L /*"Extended mathchar used as mathchar" */ );
                    }
                    {
                        help_ptr = 2;
                        help_line[1] = 65994L /*"A mathchar number must be between 0 and "7FFF." */ ;
                        help_line[0] = 65995L /*"I changed this one to zero." */ ;
                    }
                    int_error(cur_val1);
                    cur_val1 = 0;
                }
                cur_val1 =
                    (math_class_field(cur_val1) * 4096) + (math_fam_field(cur_val1) * 256) + math_char_field(cur_val1);
                {
                    cur_val = cur_val1;
                    cur_val_level = 0 /*int_val */ ;
                }
            } else if (m == 8939080L /*del_code_base */ ) {
                cur_val1 = eqtb[8939080L /*del_code_base */  + cur_val].cint;
                if (cur_val1 >= 1073741824L) {
                    {
                        if (interaction == 3 /*error_stop_mode */ ) ;
                        if (file_line_error_style_p)
                            print_file_line();
                        else
                            print_nl(65544L /*"! " */ );
                        print(65996L /*"Extended delcode used as delcode" */ );
                    }
                    {
                        help_ptr = 2;
                        help_line[1] = 65997L /*"A delimiter code must be between 0 and "7FFFFFF." */ ;
                        help_line[0] = 65995L /*"I changed this one to zero." */ ;
                    }
                    error();
                    {
                        cur_val = 0;
                        cur_val_level = 0 /*int_val */ ;
                    }
                } else {

                    {
                        cur_val = cur_val1;
                        cur_val_level = 0 /*int_val */ ;
                    }
                }
            } else if (m < 5596404L /*sf_code_base */ ) {
                cur_val = eqtb[m + cur_val].hh.v.RH;
                cur_val_level = 0 /*int_val */ ;
            } else if (m < 6710516L /*math_code_base */ ) {
                cur_val = eqtb[m + cur_val].hh.v.RH % 65536L;
                cur_val_level = 0 /*int_val */ ;
            } else {

                cur_val = eqtb[m + cur_val].cint;
                cur_val_level = 0 /*int_val */ ;
            }
        }
        break;
    case 87:
        {
            scan_usv_num();
            if (m == 5596404L /*sf_code_base */ ) {
                {
                    cur_val = eqtb[5596404L /*sf_code_base */  + cur_val].hh.v.RH / 65536L;
                    cur_val_level = 0 /*int_val */ ;
                }
            } else if (m == 6710516L /*math_code_base */ ) {
                {
                    cur_val = eqtb[6710516L /*math_code_base */  + cur_val].hh.v.RH;
                    cur_val_level = 0 /*int_val */ ;
                }
            } else if (m == 6710517L /*math_code_base 1 */ ) {
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(65987L /*"Can't use \Umathcode as a number (try \Umathcodenum)" */ );
                }
                {
                    help_ptr = 2;
                    help_line[1] = 65988L /*"\Umathcode is for setting a mathcode from separate values;" */ ;
                    help_line[0] = 65989L /*"use \Umathcodenum to access them as single values." */ ;
                }
                error();
                {
                    cur_val = 0;
                    cur_val_level = 0 /*int_val */ ;
                }
            } else if (m == 8939080L /*del_code_base */ ) {
                {
                    cur_val = eqtb[8939080L /*del_code_base */  + cur_val].cint;
                    cur_val_level = 0 /*int_val */ ;
                }
            } else {

                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(65990L /*"Can't use \Udelcode as a number (try \Udelcodenum)" */ );
                }
                {
                    help_ptr = 2;
                    help_line[1] = 65991L /*"\Udelcode is for setting a delcode from separate values;" */ ;
                    help_line[0] = 65992L /*"use \Udelcodenum to access them as single values." */ ;
                }
                error();
                {
                    cur_val = 0;
                    cur_val_level = 0 /*int_val */ ;
                }
            }
        }
        break;
    case 72:
    case 73:
    case 88:
    case 89:
    case 90:
        if (level != 5 /*tok_val */ ) {
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(65998L /*"Missing number, treated as zero" */ );
            }
            {
                help_ptr = 3;
                help_line[2] = 65999L /*"A number should have been here; I inserted `0'." */ ;
                help_line[1] = 66000L /*"(If you can't figure out why I needed to see a number," */ ;
                help_line[0] = 66001L /*"look up `weird error' in the index to The TeXbook.)" */ ;
            }
            back_error();
            {
                cur_val = 0;
                cur_val_level = 1 /*dimen_val */ ;
            }
        } else if (cur_cmd <= 73 /*assign_toks */ ) {
            if (cur_cmd < 73 /*assign_toks */ ) {

                if (m == mem_bot) {
                    scan_register_num();
                    if (cur_val < 256)
                        cur_val = eqtb[2252783L /*toks_base */  + cur_val].hh.v.RH;
                    else {

                        find_sa_element(5 /*tok_val */ , cur_val, false);
                        if (cur_ptr == -268435455L)
                            cur_val = -268435455L;
                        else
                            cur_val = mem[cur_ptr + 1].hh.v.RH;
                    }
                } else
                    cur_val = mem[m + 1].hh.v.RH;
            } else if (cur_chr == 2252782L /*XeTeX_inter_char_loc */ ) {
                scan_char_class_not_ignored();
                cur_ptr = cur_val;
                scan_char_class_not_ignored();
                find_sa_element(6 /*inter_char_val */ , cur_ptr * 4096 /*char_class_limit */  + cur_val, false);
                if (cur_ptr == -268435455L)
                    cur_val = -268435455L;
                else
                    cur_val = mem[cur_ptr + 1].hh.v.RH;
            } else
                cur_val = eqtb[m].hh.v.RH;
            cur_val_level = 5 /*tok_val */ ;
        } else {

            back_input();
            scan_font_ident();
            {
                cur_val = 2243238L /*font_id_base */  + cur_val;
                cur_val_level = 4 /*ident_val */ ;
            }
        }
        break;
    case 74:
        {
            cur_val = eqtb[m].cint;
            cur_val_level = 0 /*int_val */ ;
        }
        break;
    case 75:
        {
            cur_val = eqtb[m].cint;
            cur_val_level = 1 /*dimen_val */ ;
        }
        break;
    case 76:
        {
            cur_val = eqtb[m].hh.v.RH;
            cur_val_level = 2 /*glue_val */ ;
        }
        break;
    case 77:
        {
            cur_val = eqtb[m].hh.v.RH;
            cur_val_level = 3 /*mu_val */ ;
        }
        break;
    case 80:
        if (abs(cur_list.mode_field) != m) {
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66015L /*"Improper " */ );
            }
            print_cmd_chr(80 /*set_aux */ , m);
            {
                help_ptr = 4;
                help_line[3] = 66016L /*"You can refer to \spacefactor only in horizontal mode;" */ ;
                help_line[2] = 66017L /*"you can refer to \prevdepth only in vertical mode; and" */ ;
                help_line[1] = 66018L /*"neither of these is meaningful inside \write. So" */ ;
                help_line[0] = 66019L /*"I'm forgetting what you said and using zero instead." */ ;
            }
            error();
            if (level != 5 /*tok_val */ ) {
                cur_val = 0;
                cur_val_level = 1 /*dimen_val */ ;
            } else {

                cur_val = 0;
                cur_val_level = 0 /*int_val */ ;
            }
        } else if (m == 1 /*vmode */ ) {
            cur_val = cur_list.aux_field.cint;
            cur_val_level = 1 /*dimen_val */ ;
        } else {

            cur_val = cur_list.aux_field.hh.v.LH;
            cur_val_level = 0 /*int_val */ ;
        }
        break;
    case 81:
        if (cur_list.mode_field == 0) {
            cur_val = 0;
            cur_val_level = 0 /*int_val */ ;
        } else {

            nest[nest_ptr] = cur_list;
            p = nest_ptr;
            while (abs(nest[p].mode_field) != 1 /*vmode */ )
                p--;
            {
                cur_val = nest[p].pg_field;
                cur_val_level = 0 /*int_val */ ;
            }
        }
        break;
    case 83:
        {
            if (m == 0)
                cur_val = /*1481: */ dead_cycles;
            else if (m == 2)
                cur_val = /*:1481 */ interaction;
            else
                cur_val = insert_penalties;
            cur_val_level = 0 /*int_val */ ;
        }
        break;
    case 82:
        {
            if ((page_contents == 0 /*empty */ ) && (!output_active)) {

                if (m == 0)
                    cur_val = 1073741823L;
                else
                    cur_val = 0;
            } else
                cur_val = page_so_far[m];
            cur_val_level = 1 /*dimen_val */ ;
        }
        break;
    case 85:
        {
            if (m > 2252771L /*par_shape_loc */ ) {     /*1654: */
                scan_int();
                if ((eqtb[m].hh.v.RH == -268435455L) || (cur_val < 0))
                    cur_val = 0;
                else {

                    if (cur_val > mem[eqtb[m].hh.v.RH + 1].cint)
                        cur_val = mem[eqtb[m].hh.v.RH + 1].cint;
                    cur_val = mem[eqtb[m].hh.v.RH + cur_val + 1].cint;
                }
            } else if (eqtb[2252771L /*par_shape_loc */ ].hh.v.RH == -268435455L)
                cur_val = 0;
            else
                cur_val = mem[eqtb[2252771L /*par_shape_loc */ ].hh.v.RH].hh.v.LH;
            cur_val_level = 0 /*int_val */ ;
        }
        break;
    case 84:
        {
            scan_register_num();
            if (cur_val < 256)
                q = eqtb[2253043L /*box_base */  + cur_val].hh.v.RH;
            else {

                find_sa_element(4, cur_val, false);
                if (cur_ptr == -268435455L)
                    q = -268435455L;
                else
                    q = mem[cur_ptr + 1].hh.v.RH;
            }
            if (q == -268435455L)
                cur_val = 0;
            else
                cur_val = mem[q + m].cint;
            cur_val_level = 1 /*dimen_val */ ;
        }
        break;
    case 68:
    case 69:
        {
            cur_val = cur_chr;
            cur_val_level = 0 /*int_val */ ;
        }
        break;
    case 78:
        {
            find_font_dimen(false);
            font_info[fmem_ptr].cint = 0;
            {
                cur_val = font_info[cur_val].cint;
                cur_val_level = 1 /*dimen_val */ ;
            }
        }
        break;
    case 79:
        {
            scan_font_ident();
            if (m == 0) {
                cur_val = hyphen_char[cur_val];
                cur_val_level = 0 /*int_val */ ;
            } else if (m == 1) {
                cur_val = skew_char[cur_val];
                cur_val_level = 0 /*int_val */ ;
            } else {

                n = cur_val;
                if (((font_area[n] == 65535L /*aat_font_flag */ ) || (font_area[n] == 65534L /*otgr_font_flag */ )))
                    scan_glyph_number(n);
                else
                    scan_char_num();
                k = cur_val;
                switch (m) {
                case 2:
                    {
                        cur_val = get_cp_code(n, k, 0);
                        cur_val_level = 0 /*int_val */ ;
                    }
                    break;
                case 3:
                    {
                        cur_val = get_cp_code(n, k, 1);
                        cur_val_level = 0 /*int_val */ ;
                    }
                    break;
                }
            }
        }
        break;
    case 91:
        {
            if ((m < mem_bot) || (m > mem_bot + 19)) {
                cur_val_level = (mem[m].hh.u.B0 / 64);
                if (cur_val_level < 2 /*glue_val */ )
                    cur_val = mem[m + 2].cint;
                else
                    cur_val = mem[m + 1].hh.v.RH;
            } else {

                scan_register_num();
                cur_val_level = m - mem_bot;
                if (cur_val > 255) {
                    find_sa_element(cur_val_level, cur_val, false);
                    if (cur_ptr == -268435455L) {

                        if (cur_val_level < 2 /*glue_val */ )
                            cur_val = 0;
                        else
                            cur_val = mem_bot;
                    } else if (cur_val_level < 2 /*glue_val */ )
                        cur_val = mem[cur_ptr + 2].cint;
                    else
                        cur_val = mem[cur_ptr + 1].hh.v.RH;
                } else
                    switch (cur_val_level) {
                    case 0:
                        cur_val = eqtb[8938824L /*count_base */  + cur_val].cint;
                        break;
                    case 1:
                        cur_val = eqtb[10053215L /*scaled_base */  + cur_val].cint;
                        break;
                    case 2:
                        cur_val = eqtb[2252259L /*skip_base */  + cur_val].hh.v.RH;
                        break;
                    case 3:
                        cur_val = eqtb[2252515L /*mu_skip_base */  + cur_val].hh.v.RH;
                        break;
                    }
            }
        }
        break;
    case 71:
        if (m >= 4 /*input_line_no_code */ ) {

            if (m >= 57 /*eTeX_glue */ ) {      /*1568: */
                if (m < 58 /*eTeX_mu */ ) {
                    switch (m) {        /*1595: */
                    case 57:
                        scan_mu_glue();
                        break;
                    }
                    cur_val_level = 2 /*glue_val */ ;
                } else if (m < 59 /*eTeX_expr */ ) {
                    switch (m) {        /*1596: */
                    case 58:
                        scan_normal_glue();
                        break;
                    }
                    cur_val_level = 3 /*mu_val */ ;
                } else {

                    cur_val_level = m - 59;
                    scan_expr();
                }
                while (cur_val_level > level) {

                    if (cur_val_level == 2 /*glue_val */ ) {
                        m = cur_val;
                        cur_val = mem[m + 1].cint;
                        delete_glue_ref(m);
                    } else if (cur_val_level == 3 /*mu_val */ )
                        mu_error();
                    cur_val_level--;
                }
                if (negative) {

                    if (cur_val_level >= 2 /*glue_val */ ) {
                        m = cur_val;
                        cur_val = new_spec(m);
                        delete_glue_ref(m);
                        {
                            mem[cur_val + 1].cint = -(integer) mem[cur_val + 1].cint;
                            mem[cur_val + 2].cint = -(integer) mem[cur_val + 2].cint;
                            mem[cur_val + 3].cint = -(integer) mem[cur_val + 3].cint;
                        }
                    } else
                        cur_val = -(integer) cur_val;
                }
                return;
            } else if (m >= 47 /*XeTeX_dim */ ) {
                switch (m) {    /*1435: */
                case 47:
                    {
                        if (((font_area[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH] == 65535L /*aat_font_flag */ )
                             || (font_area[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH] ==
                                 65534L /*otgr_font_flag */ ))) {
                            scan_int();
                            n = cur_val;
                            if ((n < 1) || (n > 4)) {
                                {
                                    if (interaction == 3 /*error_stop_mode */ ) ;
                                    if (file_line_error_style_p)
                                        print_file_line();
                                    else
                                        print_nl(65544L /*"! " */ );
                                    print(66824L /*"\\XeTeXglyphbounds requires an edge index from 1 to 4;" */ );
                                }
                                print_nl(66825L /*"I don't know anything about edge " */ );
                                print_int(n);
                                error();
                                cur_val = 0;
                            } else {

                                scan_int();
                                cur_val = get_glyph_bounds(eqtb[2253299L /*cur_font_loc */ ].hh.v.RH, n, cur_val);
                            }
                        } else {

                            not_native_font_error(71 /*last_item */ , m, eqtb[2253299L /*cur_font_loc */ ].hh.v.RH);
                            cur_val = 0;
                        }
                    }
                    break;
                case 48:
                case 49:
                case 50:
                case 51:
                    {
                        scan_font_ident();
                        q = cur_val;
                        scan_usv_num();
                        if (((font_area[q] == 65535L /*aat_font_flag */ )
                             || (font_area[q] == 65534L /*otgr_font_flag */ ))) {
                            switch (m) {
                            case 48:
                                cur_val = getnativecharwd(q, cur_val);
                                break;
                            case 49:
                                cur_val = getnativecharht(q, cur_val);
                                break;
                            case 50:
                                cur_val = getnativechardp(q, cur_val);
                                break;
                            case 51:
                                cur_val = getnativecharic(q, cur_val);
                                break;
                            }
                        } else {

                            if ((font_bc[q] <= cur_val) && (font_ec[q] >= cur_val)) {
                                i = font_info[char_base[q] + effective_char(true, q, cur_val)].qqqq;
                                switch (m) {
                                case 48:
                                    cur_val = font_info[width_base[q] + i.u.B0].cint;
                                    break;
                                case 49:
                                    cur_val = font_info[height_base[q] + (i.u.B1) / 16].cint;
                                    break;
                                case 50:
                                    cur_val = font_info[depth_base[q] + (i.u.B1) % 16].cint;
                                    break;
                                case 51:
                                    cur_val = font_info[italic_base[q] + (i.u.B2) / 4].cint;
                                    break;
                                }
                            } else
                                cur_val = 0;
                        }
                    }
                    break;
                case 52:
                case 53:
                case 54:
                    {
                        q = cur_chr - 52;
                        scan_int();
                        if ((eqtb[2252771L /*par_shape_loc */ ].hh.v.RH == -268435455L) || (cur_val <= 0))
                            cur_val = 0;
                        else {

                            if (q == 2) {
                                q = cur_val % 2;
                                cur_val = (cur_val + q) / 2;
                            }
                            if (cur_val > mem[eqtb[2252771L /*par_shape_loc */ ].hh.v.RH].hh.v.LH)
                                cur_val = mem[eqtb[2252771L /*par_shape_loc */ ].hh.v.RH].hh.v.LH;
                            cur_val = mem[eqtb[2252771L /*par_shape_loc */ ].hh.v.RH + 2 * cur_val - q].cint;
                        }
                        cur_val_level = 1 /*dimen_val */ ;
                    }
                    break;
                case 55:
                case 56:
                    {
                        scan_normal_glue();
                        q = cur_val;
                        if (m == 55 /*glue_stretch_code */ )
                            cur_val = mem[q + 2].cint;
                        else
                            cur_val = mem[q + 3].cint;
                        delete_glue_ref(q);
                    }
                    break;
                }
                cur_val_level = 1 /*dimen_val */ ;
            } else {

                switch (m) {
                case 4:
                    cur_val = line;
                    break;
                case 5:
                    cur_val = last_badness;
                    break;
                case 45:
		    cur_val = 0; /* shellenabledp */
                    break;
                case 6:
                    cur_val = 2 /*eTeX_version */ ;
                    break;
                case 14:
                    cur_val = 0 /*XeTeX_version */ ;
                    break;
                case 15:
                    {
                        scan_font_ident();
                        n = cur_val;
                        if ((font_area[n] == 65535L /*aat_font_flag */ ))
                            cur_val = aat_font_get(m - 14, font_layout_engine[n]);
                        else if ((font_area[n] == 65534L /*otgr_font_flag */ ))
                            cur_val = ot_font_get(m - 14, font_layout_engine[n]);
                        else
                            cur_val = 0;
                    }
                    break;
                case 22:
                    {
                        scan_font_ident();
                        n = cur_val;
                        if ((font_area[n] == 65535L /*aat_font_flag */ ))
                            cur_val = aat_font_get(m - 14, font_layout_engine[n]);
                        else if (((font_area[n] == 65534L /*otgr_font_flag */ )
                                  && (usingGraphite(font_layout_engine[n]))))
                            cur_val = ot_font_get(m - 14, font_layout_engine[n]);
                        else
                            cur_val = 0;
                    }
                    break;
                case 17:
                case 19:
                case 20:
                case 21:
                case 16:
                    {
                        scan_font_ident();
                        n = cur_val;
                        cur_val = 0;
                    }
                    break;
                case 23:
                case 25:
                case 26:
                    {
                        scan_font_ident();
                        n = cur_val;
                        if ((font_area[n] == 65535L /*aat_font_flag */ )) {
                            scan_int();
                            k = cur_val;
                            cur_val = aat_font_get_1(m - 14, font_layout_engine[n], k);
                        } else
                            if (((font_area[n] == 65534L /*otgr_font_flag */ )
                                 && (usingGraphite(font_layout_engine[n])))) {
                            scan_int();
                            k = cur_val;
                            cur_val = ot_font_get_1(m - 14, font_layout_engine[n], k);
                        } else {

                            not_aat_gr_font_error(71 /*last_item */ , m, n);
                            cur_val = -1;
                        }
                    }
                    break;
                case 27:
                case 29:
                    {
                        scan_font_ident();
                        n = cur_val;
                        if ((font_area[n] == 65535L /*aat_font_flag */ )) {
                            scan_int();
                            k = cur_val;
                            scan_int();
                            cur_val = aat_font_get_2(m - 14, font_layout_engine[n], k, cur_val);
                        } else
                            if (((font_area[n] == 65534L /*otgr_font_flag */ )
                                 && (usingGraphite(font_layout_engine[n])))) {
                            scan_int();
                            k = cur_val;
                            scan_int();
                            cur_val = ot_font_get_2(m - 14, font_layout_engine[n], k, cur_val);
                        } else {

                            not_aat_gr_font_error(71 /*last_item */ , m, n);
                            cur_val = -1;
                        }
                    }
                    break;
                case 18:
                    {
                        scan_font_ident();
                        n = cur_val;
                        if ((font_area[n] == 65535L /*aat_font_flag */ )) {
                            scan_and_pack_name();
                            cur_val = aat_font_get_named(m - 14, font_layout_engine[n]);
                        } else {

                            not_aat_font_error(71 /*last_item */ , m, n);
                            cur_val = -1;
                        }
                    }
                    break;
                case 24:
                    {
                        scan_font_ident();
                        n = cur_val;
                        if ((font_area[n] == 65535L /*aat_font_flag */ )) {
                            scan_and_pack_name();
                            cur_val = aat_font_get_named(m - 14, font_layout_engine[n]);
                        } else
                            if (((font_area[n] == 65534L /*otgr_font_flag */ )
                                 && (usingGraphite(font_layout_engine[n])))) {
                            scan_and_pack_name();
                            cur_val = gr_font_get_named(m - 14, font_layout_engine[n]);
                        } else {

                            not_aat_gr_font_error(71 /*last_item */ , m, n);
                            cur_val = -1;
                        }
                    }
                    break;
                case 28:
                    {
                        scan_font_ident();
                        n = cur_val;
                        if ((font_area[n] == 65535L /*aat_font_flag */ )) {
                            scan_int();
                            k = cur_val;
                            scan_and_pack_name();
                            cur_val = aat_font_get_named_1(m - 14, font_layout_engine[n], k);
                        } else
                            if (((font_area[n] == 65534L /*otgr_font_flag */ )
                                 && (usingGraphite(font_layout_engine[n])))) {
                            scan_int();
                            k = cur_val;
                            scan_and_pack_name();
                            cur_val = gr_font_get_named_1(m - 14, font_layout_engine[n], k);
                        } else {

                            not_aat_gr_font_error(71 /*last_item */ , m, n);
                            cur_val = -1;
                        }
                    }
                    break;
                case 30:
                    {
                        scan_font_ident();
                        n = cur_val;
                        if (((font_area[n] == 65534L /*otgr_font_flag */ ) && (usingOpenType(font_layout_engine[n]))))
                            cur_val = ot_font_get(m - 14, font_layout_engine[n]);
                        else {

                            cur_val = 0;
                        }
                    }
                    break;
                case 31:
                case 33:
                    {
                        scan_font_ident();
                        n = cur_val;
                        if (((font_area[n] == 65534L /*otgr_font_flag */ ) && (usingOpenType(font_layout_engine[n])))) {
                            scan_int();
                            cur_val = ot_font_get_1(m - 14, font_layout_engine[n], cur_val);
                        } else {

                            not_ot_font_error(71 /*last_item */ , m, n);
                            cur_val = -1;
                        }
                    }
                    break;
                case 32:
                case 34:
                    {
                        scan_font_ident();
                        n = cur_val;
                        if (((font_area[n] == 65534L /*otgr_font_flag */ ) && (usingOpenType(font_layout_engine[n])))) {
                            scan_int();
                            k = cur_val;
                            scan_int();
                            cur_val = ot_font_get_2(m - 14, font_layout_engine[n], k, cur_val);
                        } else {

                            not_ot_font_error(71 /*last_item */ , m, n);
                            cur_val = -1;
                        }
                    }
                    break;
                case 35:
                    {
                        scan_font_ident();
                        n = cur_val;
                        if (((font_area[n] == 65534L /*otgr_font_flag */ ) && (usingOpenType(font_layout_engine[n])))) {
                            scan_int();
                            k = cur_val;
                            scan_int();
                            kk = cur_val;
                            scan_int();
                            cur_val = ot_font_get_3(m - 14, font_layout_engine[n], k, kk, cur_val);
                        } else {

                            not_ot_font_error(71 /*last_item */ , m, n);
                            cur_val = -1;
                        }
                    }
                    break;
                case 36:
                    {
                        if (((font_area[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH] == 65535L /*aat_font_flag */ )
                             || (font_area[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH] ==
                                 65534L /*otgr_font_flag */ ))) {
                            scan_int();
                            n = cur_val;
                            cur_val = map_char_to_glyph(eqtb[2253299L /*cur_font_loc */ ].hh.v.RH, n);
                        } else {

                            not_native_font_error(71 /*last_item */ , m, eqtb[2253299L /*cur_font_loc */ ].hh.v.RH);
                            cur_val = 0;
                        }
                    }
                    break;
                case 37:
                    {
                        if (((font_area[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH] == 65535L /*aat_font_flag */ )
                             || (font_area[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH] ==
                                 65534L /*otgr_font_flag */ ))) {
                            scan_and_pack_name();
                            cur_val = map_glyph_to_index(eqtb[2253299L /*cur_font_loc */ ].hh.v.RH);
                        } else {

                            not_native_font_error(71 /*last_item */ , m, eqtb[2253299L /*cur_font_loc */ ].hh.v.RH);
                            cur_val = 0;
                        }
                    }
                    break;
                case 38:
                    {
                        scan_font_ident();
                        n = cur_val;
                        if ((font_area[n] == 65535L /*aat_font_flag */ ))
                            cur_val = 1;
                        else if (((font_area[n] == 65534L /*otgr_font_flag */ )
                                  && (usingOpenType(font_layout_engine[n]))))
                            cur_val = 2;
                        else if (((font_area[n] == 65534L /*otgr_font_flag */ )
                                  && (usingGraphite(font_layout_engine[n]))))
                            cur_val = 3;
                        else
                            cur_val = 0;
                    }
                    break;
                case 39:
                case 40:
                    {
                        scan_font_ident();
                        n = cur_val;
                        if (((font_area[n] == 65535L /*aat_font_flag */ )
                             || (font_area[n] == 65534L /*otgr_font_flag */ )))
                            cur_val = get_font_char_range(n, m == 39 /*XeTeX_first_char_code */ );
                        else {

                            if (m == 39 /*XeTeX_first_char_code */ )
                                cur_val = font_bc[n];
                            else
                                cur_val = font_ec[n];
                        }
                    }
                    break;
                case 41:
                    cur_val = pdf_last_x_pos;
                    break;
                case 42:
                    cur_val = pdf_last_y_pos;
                    break;
                case 46:
                    {
                        scan_and_pack_name();
                        cur_val = count_pdf_file_pages();
                    }
                    break;
                case 7:
                    cur_val = cur_level - 1;
                    break;
                case 8:
                    cur_val = cur_group;
                    break;
                case 9:
                    {
                        q = cond_ptr;
                        cur_val = 0;
                        while (q != -268435455L) {

                            cur_val++;
                            q = mem[q].hh.v.RH;
                        }
                    }
                    break;
                case 10:
                    if (cond_ptr == -268435455L)
                        cur_val = 0;
                    else if (cur_if < 32 /*unless_code */ )
                        cur_val = cur_if + 1;
                    else
                        cur_val = -(integer) (cur_if - 31);
                    break;
                case 11:
                    if ((if_limit == 4 /*or_code */ ) || (if_limit == 3 /*else_code */ ))
                        cur_val = 1;
                    else if (if_limit == 2 /*fi_code */ )
                        cur_val = -1;
                    else
                        cur_val = 0;
                    break;
                case 12:
                case 13:
                    {
                        scan_normal_glue();
                        q = cur_val;
                        if (m == 12 /*glue_stretch_order_code */ )
                            cur_val = mem[q].hh.u.B0;
                        else
                            cur_val = mem[q].hh.u.B1;
                        delete_glue_ref(q);
                    }
                    break;
                }
                cur_val_level = 0 /*int_val */ ;
            }
        } else {

            if (cur_chr == 2 /*glue_val */ )
                cur_val = mem_bot;
            else
                cur_val = 0;
            tx = cur_list.tail_field;
            if (!(tx >= hi_mem_min)) {

                if ((mem[tx].hh.u.B0 == 9 /*math_node */ ) && (mem[tx].hh.u.B1 == 3 /*end_M_code */ )) {
                    r = cur_list.head_field;
                    do {
                        q = r;
                        r = mem[q].hh.v.RH;
                    } while (!(r == tx));
                    tx = q;
                }
            }
            if (cur_chr == 3 /*last_node_type_code */ ) {
                cur_val_level = 0 /*int_val */ ;
                if ((tx == cur_list.head_field) || (cur_list.mode_field == 0))
                    cur_val = -1;
            } else
                cur_val_level = cur_chr;
            if (!(tx >= hi_mem_min) && (cur_list.mode_field != 0))
                switch (cur_chr) {
                case 0:
                    if (mem[tx].hh.u.B0 == 12 /*penalty_node */ )
                        cur_val = mem[tx + 1].cint;
                    break;
                case 1:
                    if (mem[tx].hh.u.B0 == 11 /*kern_node */ )
                        cur_val = mem[tx + 1].cint;
                    break;
                case 2:
                    if (mem[tx].hh.u.B0 == 10 /*glue_node */ ) {
                        cur_val = mem[tx + 1].hh.v.LH;
                        if (mem[tx].hh.u.B1 == 99 /*mu_glue */ )
                            cur_val_level = 3 /*mu_val */ ;
                    }
                    break;
                case 3:
                    if (mem[tx].hh.u.B0 <= 13 /*unset_node */ )
                        cur_val = mem[tx].hh.u.B0 + 1;
                    else
                        cur_val = 15 /*unset_node 2 */ ;
                    break;
            } else if ((cur_list.mode_field == 1 /*vmode */ ) && (tx == cur_list.head_field))
                switch (cur_chr) {
                case 0:
                    cur_val = last_penalty;
                    break;
                case 1:
                    cur_val = last_kern;
                    break;
                case 2:
                    if (last_glue != 1073741823L)
                        cur_val = last_glue;
                    break;
                case 3:
                    cur_val = last_node_type;
                    break;
                }
        }
        break;
    default:
        {
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66020L /*"You can't use `" */ );
            }
            print_cmd_chr(cur_cmd, cur_chr);
            print(66021L /*"' after " */ );
            print_esc(65853L /*"the" */ );
            {
                help_ptr = 1;
                help_line[0] = 66019L /*"I'm forgetting what you said and using zero instead." */ ;
            }
            error();
            if (level != 5 /*tok_val */ ) {
                cur_val = 0;
                cur_val_level = 1 /*dimen_val */ ;
            } else {

                cur_val = 0;
                cur_val_level = 0 /*int_val */ ;
            }
        }
        break;
    }
    while (cur_val_level > level) {     /*447: */

        if (cur_val_level == 2 /*glue_val */ )
            cur_val = mem[cur_val + 1].cint;
        else if (cur_val_level == 3 /*mu_val */ )
            mu_error();
        cur_val_level--;
    }
    if (negative) {

        if (cur_val_level >= 2 /*glue_val */ ) {
            cur_val = new_spec(cur_val);
            {
                mem[cur_val + 1].cint = -(integer) mem[cur_val + 1].cint;
                mem[cur_val + 2].cint = -(integer) mem[cur_val + 2].cint;
                mem[cur_val + 3].cint = -(integer) mem[cur_val + 3].cint;
            }
        } else
            cur_val = -(integer) cur_val;
    } else if ((cur_val_level >= 2 /*glue_val */ ) && (cur_val_level <= 3 /*mu_val */ ))
        mem[cur_val].hh.v.RH++;
}

void scan_int(void)
{
    scan_int_regmem boolean negative;
    integer m;
    small_number d;
    boolean vacuous;
    boolean OK_so_far;
    radix = 0;
    OK_so_far = true;
    negative = false;
    do {
        /*424: */
        do {
            get_x_token();
        } while (!(cur_cmd != 10 /*spacer *//*:424 */ ));
        if (cur_tok == 25165869L /*other_token 45 */ ) {
            negative = !negative;
            cur_tok = 25165867L /*other_token 43 */ ;
        }
    } while (!(cur_tok != 25165867L /*other_token 43 *//*:459 */ ));
    if (cur_tok == 25165920L /*alpha_token */ ) {       /*460: */
        get_token();
        if (cur_tok < 33554431L /*cs_token_flag */ ) {
            cur_val = cur_chr;
            if (cur_cmd <= 2 /*right_brace */ ) {

                if (cur_cmd == 2 /*right_brace */ )
                    align_state++;
                else
                    align_state--;
            }
        } else if (cur_tok < 34668544L /*cs_token_flag 1114113 */ )
            cur_val = cur_tok - 33554432L;
        else
            cur_val = cur_tok - 34668544L;
        if (cur_val > 1114111L /*biggest_usv */ ) {
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66045L /*"Improper alphabetic constant" */ );
            }
            {
                help_ptr = 2;
                help_line[1] = 66046L /*"A one-character control sequence belongs after a ` mark." */ ;
                help_line[0] = 66047L /*"So I'm essentially inserting \0 here." */ ;
            }
            cur_val = 48 /*"0" */ ;
            back_error();
        } else {                /*461: */

            get_x_token();
            if (cur_cmd != 10 /*spacer */ )
                back_input();
        }
    } else if ((cur_cmd >= 68 /*min_internal */ ) && (cur_cmd <= 91 /*max_internal */ ))
        scan_something_internal(0 /*int_val */ , false);
    else {                      /*462: */

        radix = 10;
        m = 214748364L;
        if (cur_tok == 25165863L /*octal_token */ ) {
            radix = 8;
            m = 268435456L;
            get_x_token();
        } else if (cur_tok == 25165858L /*hex_token */ ) {
            radix = 16;
            m = 134217728L;
            get_x_token();
        }
        vacuous = true;
        cur_val = 0;
        while (true) {

            if ((cur_tok < 25165872L /*zero_token */  + radix) && (cur_tok >= 25165872L /*zero_token */ )
                && (cur_tok <= 25165881L /*zero_token 9 */ ))
                d = cur_tok - 25165872L;
            else if (radix == 16) {

                if ((cur_tok <= 23068742L /*A_token 5 */ ) && (cur_tok >= 23068737L /*A_token */ ))
                    d = cur_tok - 23068727L;
                else if ((cur_tok <= 25165894L /*other_A_token 5 */ ) && (cur_tok >= 25165889L /*other_A_token */ ))
                    d = cur_tok - 25165879L;
                else
                    goto lab30;
            } else
                goto lab30;
            vacuous = false;
            if ((cur_val >= m) && ((cur_val > m) || (d > 7) || (radix != 10))) {
                if (OK_so_far) {
                    {
                        if (interaction == 3 /*error_stop_mode */ ) ;
                        if (file_line_error_style_p)
                            print_file_line();
                        else
                            print_nl(65544L /*"! " */ );
                        print(66048L /*"Number too big" */ );
                    }
                    {
                        help_ptr = 2;
                        help_line[1] = 66049L /*"I can only go up to 2147483647='17777777777="7FFFFFFF," */ ;
                        help_line[0] = 66050L /*"so I'm using that number instead of yours." */ ;
                    }
                    error();
                    cur_val = 2147483647L;
                    OK_so_far = false;
                }
            } else
                cur_val = cur_val * radix + d;
            get_x_token();
        }
 lab30:                        /*done *//*:463 */ ;
        if (vacuous) {          /*464: */
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(65998L /*"Missing number, treated as zero" */ );
            }
            {
                help_ptr = 3;
                help_line[2] = 65999L /*"A number should have been here; I inserted `0'." */ ;
                help_line[1] = 66000L /*"(If you can't figure out why I needed to see a number," */ ;
                help_line[0] = 66001L /*"look up `weird error' in the index to The TeXbook.)" */ ;
            }
            back_error();
        } else if (cur_cmd != 10 /*spacer */ )
            back_input();
    }
    if (negative)
        cur_val = -(integer) cur_val;
}

void zxetex_scan_dimen(boolean mu, boolean inf, boolean shortcut, boolean requires_units)
{
    xetex_scan_dimen_regmem boolean negative;
    integer f;
    integer num, denom;
    small_number k, kk;
    halfword p, q;
    scaled v;
    integer save_cur_val;
    f = 0;
    arith_error = false;
    cur_order = 0 /*normal */ ;
    negative = false;
    if (!shortcut) {
        negative = false;
        do {
            /*424: */
            do {
                get_x_token();
            } while (!(cur_cmd != 10 /*spacer *//*:424 */ ));
            if (cur_tok == 25165869L /*other_token 45 */ ) {
                negative = !negative;
                cur_tok = 25165867L /*other_token 43 */ ;
            }
        } while (!(cur_tok != 25165867L /*other_token 43 *//*:459 */ ));
        if ((cur_cmd >= 68 /*min_internal */ ) && (cur_cmd <= 91 /*max_internal */ )) { /*468: */

            if (mu) {
                scan_something_internal(3 /*mu_val */ , false);
                if (cur_val_level >= 2 /*glue_val */ ) {
                    v = mem[cur_val + 1].cint;
                    delete_glue_ref(cur_val);
                    cur_val = v;
                }
                if (cur_val_level == 3 /*mu_val */ )
                    goto lab89;
                if (cur_val_level != 0 /*int_val */ )
                    mu_error();
            } else {

                scan_something_internal(1 /*dimen_val */ , false);
                if (cur_val_level == 1 /*dimen_val */ )
                    goto lab89;
            }
        } else {

            back_input();
            if (cur_tok == 25165868L /*continental_point_token */ )
                cur_tok = 25165870L /*point_token */ ;
            if (cur_tok != 25165870L /*point_token */ )
                scan_int();
            else {

                radix = 10;
                cur_val = 0;
            }
            if (cur_tok == 25165868L /*continental_point_token */ )
                cur_tok = 25165870L /*point_token */ ;
            if ((radix == 10) && (cur_tok == 25165870L /*point_token */ )) {    /*471: */
                k = 0;
                p = -268435455L;
                get_token();
                while (true) {

                    get_x_token();
                    if ((cur_tok > 25165881L /*zero_token 9 */ ) || (cur_tok < 25165872L /*zero_token */ ))
                        goto lab31;
                    if (k < 17) {
                        q = get_avail();
                        mem[q].hh.v.RH = p;
                        mem[q].hh.v.LH = cur_tok - 25165872L;
                        p = q;
                        k++;
                    }
                }
 lab31:                        /*done1 */  {
                    register integer for_end;
                    kk = k;
                    for_end = 1;
                    if (kk >= for_end)
                        do {
                            dig[kk - 1] = mem[p].hh.v.LH;
                            q = p;
                            p = mem[p].hh.v.RH;
                            {
                                mem[q].hh.v.RH = avail;
                                avail = q;
                            }
                        }
                        while (kk-- > for_end);
                }
                f = round_decimals(k);
                if (cur_cmd != 10 /*spacer */ )
                    back_input();
            }
        }
    }
    if (cur_val < 0) {
        negative = !negative;
        cur_val = -(integer) cur_val;
    }
    if (requires_units) {
        if (inf) {              /*473: */

            if (scan_keyword(65593L /*"fil" */ )) {
                cur_order = 1 /*fil */ ;
                while (scan_keyword(108 /*"l" */ )) {

                    if (cur_order == 3 /*filll */ ) {
                        {
                            if (interaction == 3 /*error_stop_mode */ ) ;
                            if (file_line_error_style_p)
                                print_file_line();
                            else
                                print_nl(65544L /*"! " */ );
                            print(66052L /*"Illegal unit of measure (" */ );
                        }
                        print(66053L /*"replaced by filll)" */ );
                        {
                            help_ptr = 1;
                            help_line[0] = 66054L /*"I dddon't go any higher than filll." */ ;
                        }
                        error();
                    } else
                        cur_order++;
                }
                goto lab88;
            }
        }
        save_cur_val = cur_val;
        do {
            get_x_token();
        } while (!(cur_cmd != 10 /*spacer *//*:424 */ ));
        if ((cur_cmd < 68 /*min_internal */ ) || (cur_cmd > 91 /*max_internal */ ))
            back_input();
        else {

            if (mu) {
                scan_something_internal(3 /*mu_val */ , false);
                if (cur_val_level >= 2 /*glue_val */ ) {
                    v = mem[cur_val + 1].cint;
                    delete_glue_ref(cur_val);
                    cur_val = v;
                }
                if (cur_val_level != 3 /*mu_val */ )
                    mu_error();
            } else
                scan_something_internal(1 /*dimen_val */ , false);
            v = cur_val;
            goto lab40;
        }
        if (mu)
            goto lab45;
        if (scan_keyword(66055L /*"em" */ ))
            v = ( /*577: */ font_info[6 /*quad_code */  + param_base[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH]].
                 cint /*:577 */ );
        else if (scan_keyword(66056L /*"ex" */ ))
            v = ( /*578: */ font_info[5 /*x_height_code */  + param_base[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH]].
                 cint /*:578 */ );
        else
            goto lab45;
        {
            get_x_token();
            if (cur_cmd != 10 /*spacer */ )
                back_input();
        }
 lab40:                        /*found */ cur_val = mult_and_add(save_cur_val, v, xn_over_d(v, f, 65536L), 1073741823L);
        goto lab89;
 lab45:                        /*not_found *//*:474 */ ;
        if (mu) {               /*475: */

            if (scan_keyword(65621L /*"mu" */ ))
                goto lab88;
            else {

                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66052L /*"Illegal unit of measure (" */ );
                }
                print(66057L /*"mu inserted)" */ );
                {
                    help_ptr = 4;
                    help_line[3] = 66058L /*"The unit of measurement in math glue must be mu." */ ;
                    help_line[2] = 66059L /*"To recover gracefully from this error, it's best to" */ ;
                    help_line[1] = 66060L /*"delete the erroneous units; e.g., type `2' to delete" */ ;
                    help_line[0] = 66061L /*"two letters. (See Chapter 27 of The TeXbook.)" */ ;
                }
                error();
                goto lab88;
            }
        }
        if (scan_keyword(66051L /*"true" */ )) {        /*476: */
            prepare_mag();
            if (eqtb[8938757L /*int_base 17 */ ].cint != 1000) {
                cur_val = xn_over_d(cur_val, 1000, eqtb[8938757L /*int_base 17 */ ].cint);
                f = (1000 * f + 65536L * tex_remainder) / eqtb[8938757L /*int_base 17 */ ].cint;
                cur_val = cur_val + (f / 65536L);
                f = f % 65536L;
            }
        }
        if (scan_keyword(65693L /*"pt" */ ))
            goto lab88;
        if (scan_keyword(66062L /*"in" */ )) {
            num = 7227;
            denom = 100;
        } else if (scan_keyword(66063L /*"pc" */ )) {
            num = 12;
            denom = 1;
        } else if (scan_keyword(66064L /*"cm" */ )) {
            num = 7227;
            denom = 254;
        } else if (scan_keyword(66065L /*"mm" */ )) {
            num = 7227;
            denom = 2540;
        } else if (scan_keyword(66066L /*"bp" */ )) {
            num = 7227;
            denom = 7200;
        } else if (scan_keyword(66067L /*"dd" */ )) {
            num = 1238;
            denom = 1157;
        } else if (scan_keyword(66068L /*"cc" */ )) {
            num = 14856;
            denom = 1157;
        } else if (scan_keyword(66069L /*"sp" */ ))
            goto lab30;
        else {                  /*478: */

            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66052L /*"Illegal unit of measure (" */ );
            }
            print(66070L /*"pt inserted)" */ );
            {
                help_ptr = 6;
                help_line[5] = 66071L /*"Dimensions can be in units of em, ex, in, pt, pc," */ ;
                help_line[4] = 66072L /*"cm, mm, dd, cc, bp, or sp; but yours is a new one!" */ ;
                help_line[3] = 66073L /*"I'll assume that you meant to say pt, for printer's points." */ ;
                help_line[2] = 66059L /*"To recover gracefully from this error, it's best to" */ ;
                help_line[1] = 66060L /*"delete the erroneous units; e.g., type `2' to delete" */ ;
                help_line[0] = 66061L /*"two letters. (See Chapter 27 of The TeXbook.)" */ ;
            }
            error();
            goto lab32;
        }
        cur_val = xn_over_d(cur_val, num, denom);
        f = (num * f + 65536L * tex_remainder) / denom;
        cur_val = cur_val + (f / 65536L);
        f = f % 65536L;
 lab32:                        /*done2 *//*:477 */ ;
 lab88:                        /*attach_fraction */ if (cur_val >= 16384)
            arith_error = true;
        else
            cur_val = cur_val * 65536L + f;
 lab30:                        /*done *//*:472 */ ;
        {
            get_x_token();
            if (cur_cmd != 10 /*spacer */ )
                back_input();
        }
    } else {

        if (cur_val >= 16384)
            arith_error = true;
        else
            cur_val = cur_val * 65536L + f;
    }
 lab89:                                                                        /*attach_sign */ if (arith_error || (abs(cur_val) >= 1073741824L)) {
                                                                                /*479: */
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66074L /*"Dimension too large" */ );
        }
        {
            help_ptr = 2;
            help_line[1] = 66075L /*"I can't work with sizes bigger than about 19 feet." */ ;
            help_line[0] = 66076L /*"Continue and I'll use the largest value I can." */ ;
        }
        error();
        cur_val = 1073741823L;
        arith_error = false;
    }
    if (negative)
        cur_val = -(integer) cur_val;
}

void zscan_dimen(boolean mu, boolean inf, boolean shortcut)
{
    scan_dimen_regmem xetex_scan_dimen(mu, inf, shortcut, true);
}

void scan_decimal(void)
{
    scan_decimal_regmem xetex_scan_dimen(false, false, false, false);
}

void zscan_glue(small_number level)
{
    scan_glue_regmem boolean negative;
    halfword q;
    boolean mu;
    mu = (level == 3 /*mu_val */ );
    negative = false;
    do {
        /*424: */
        do {
            get_x_token();
        } while (!(cur_cmd != 10 /*spacer *//*:424 */ ));
        if (cur_tok == 25165869L /*other_token 45 */ ) {
            negative = !negative;
            cur_tok = 25165867L /*other_token 43 */ ;
        }
    } while (!(cur_tok != 25165867L /*other_token 43 *//*:459 */ ));
    if ((cur_cmd >= 68 /*min_internal */ ) && (cur_cmd <= 91 /*max_internal */ )) {
        scan_something_internal(level, negative);
        if (cur_val_level >= 2 /*glue_val */ ) {
            if (cur_val_level != level)
                mu_error();
            return;
        }
        if (cur_val_level == 0 /*int_val */ )
            scan_dimen(mu, false, true);
        else if (level == 3 /*mu_val */ )
            mu_error();
    } else {

        back_input();
        scan_dimen(mu, false, false);
        if (negative)
            cur_val = -(integer) cur_val;
    }
    q = new_spec(mem_bot);
    mem[q + 1].cint = cur_val;
    if (scan_keyword(66077L /*"plus" */ )) {
        scan_dimen(mu, true, false);
        mem[q + 2].cint = cur_val;
        mem[q].hh.u.B0 = cur_order;
    }
    if (scan_keyword(66078L /*"minus" */ )) {
        scan_dimen(mu, true, false);
        mem[q + 3].cint = cur_val;
        mem[q].hh.u.B1 = cur_order;
    }
    cur_val = /*:481 */ q;
}

integer zadd_or_sub(integer x, integer y, integer max_answer, boolean negative)
{
    register integer Result;
    add_or_sub_regmem integer a;
    if (negative)
        y = -(integer) y;
    if (x >= 0) {

        if (y <= max_answer - x)
            a = x + y;
        else {

            arith_error = true;
            a = 0;
        }
    } else if (y >= -(integer) max_answer - x)
        a = x + y;
    else {

        arith_error = true;
        a = 0;
    }
    Result = a;
    return Result;
}

integer zquotient(integer n, integer d)
{
    register integer Result;
    quotient_regmem boolean negative;
    integer a;
    if (d == 0) {
        arith_error = true;
        a = 0;
    } else {

        if (d > 0)
            negative = false;
        else {

            d = -(integer) d;
            negative = true;
        }
        if (n < 0) {
            n = -(integer) n;
            negative = !negative;
        }
        a = n / d;
        n = n - a * d;
        d = n - d;
        if (d + n >= 0)
            a++;
        if (negative)
            a = -(integer) a;
    }
    Result = a;
    return Result;
}

integer zfract(integer x, integer n, integer d, integer max_answer)
{
    register integer Result;
    fract_regmem boolean negative;
    integer a;
    integer f;
    integer h;
    integer r;
    integer t;
    if (d == 0)
        goto lab88;
    a = 0;
    if (d > 0)
        negative = false;
    else {

        d = -(integer) d;
        negative = true;
    }
    if (x < 0) {
        x = -(integer) x;
        negative = !negative;
    } else if (x == 0)
        goto lab30;
    if (n < 0) {
        n = -(integer) n;
        negative = !negative;
    }
    t = n / d;
    if (t > max_answer / x)
        goto lab88;
    a = t * x;
    n = n - t * d;
    if (n == 0)
        goto lab40;
    t = x / d;
    if (t > (max_answer - a) / n)
        goto lab88;
    a = a + t * n;
    x = x - t * d;
    if (x == 0)
        goto lab40;
    if (x < n) {
        t = x;
        x = n;
        n = t;
    }
    f = 0;
    r = (d / 2) - d;
    h = -(integer) r;
    while (true) {

        if (odd(n)) {
            r = r + x;
            if (r >= 0) {
                r = r - d;
                f++;
            }
        }
        n = n / 2;
        if (n == 0)
            goto lab41;
        if (x < h)
            x = x + x;
        else {

            t = x - d;
            x = t + x;
            f = f + n;
            if (x < n) {
                if (x == 0)
                    goto lab41;
                t = x;
                x = n;
                n = t;
            }
        }
    }
 lab41:                        /*found1 *//*:1588 */ if (f > (max_answer - a))
        goto lab88;
    a = a + f;
 lab40:                        /*found */ if (negative)
        a = -(integer) a;
    goto lab30;
 lab88:                        /*too_big */  {

        arith_error = true;
        a = 0;
    }
 lab30:                        /*done */ Result = a;
    return Result;
}

void scan_expr(void)
{
    scan_expr_regmem boolean a, b;
    small_number l;
    small_number r;
    small_number s;
    small_number o;
    integer e;
    integer t;
    integer f;
    integer n;
    halfword p;
    halfword q;
    l = cur_val_level;
    a = arith_error;
    b = false;
    p = -268435455L;
 lab20:/*restart */ r = 0 /*expr_none */ ;
    e = 0;
    s = 0 /*expr_none */ ;
    t = 0;
    n = 0;
 lab22:/*continue */ if (s == 0 /*expr_none */ )
        o = l;
    else
        o = 0 /*int_val */ ;
    do {
        get_x_token();
    } while (!(cur_cmd != 10 /*spacer *//*:424 */ ));
    if (cur_tok == 25165864L /*other_token 40 */ ) {    /*1576: */
        q = get_node(4 /*expr_node_size */ );
        mem[q].hh.v.RH = p;
        mem[q].hh.u.B0 = l;
        mem[q].hh.u.B1 = 4 * s + r;
        mem[q + 1].cint = e;
        mem[q + 2].cint = t;
        mem[q + 3].cint = n;
        p = q;
        l = o;
        goto lab20;
    }
    back_input();
    if (o == 0 /*int_val */ )
        scan_int();
    else if (o == 1 /*dimen_val */ )
        scan_dimen(false, false, false);
    else if (o == 2 /*glue_val */ )
        scan_normal_glue();
    else
        scan_mu_glue();
    f = /*:1573 */ cur_val;
 lab40:                        /*found *//*1572: *//*424: */
    do {
        get_x_token();
    } while (!(cur_cmd != 10 /*spacer *//*:424 */ ));
    if (cur_tok == 25165867L /*other_token 43 */ )
        o = 1 /*expr_add */ ;
    else if (cur_tok == 25165869L /*other_token 45 */ )
        o = 2 /*expr_sub */ ;
    else if (cur_tok == 25165866L /*other_token 42 */ )
        o = 3 /*expr_mult */ ;
    else if (cur_tok == 25165871L /*other_token 47 */ )
        o = 4 /*expr_div */ ;
    else {

        o = 0 /*expr_none */ ;
        if (p == -268435455L) {
            if (cur_cmd != 0 /*relax */ )
                back_input();
        } else if (cur_tok != 25165865L /*other_token 41 */ ) {
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66918L /*"Missing ) inserted for expression" */ );
            }
            {
                help_ptr = 1;
                help_line[0] = 66919L /*"I was expecting to see `+', `-', `*', `/', or `)'. Didn't." */ ;
            }
            back_error();
        }
    }
    arith_error = b;
    if ((l == 0 /*int_val */ ) || (s > 2 /*expr_sub */ )) {
        if ((f > 2147483647L) || (f < -2147483647L)) {
            arith_error = true;
            f = 0;
        }
    } else if (l == 1 /*dimen_val */ ) {
        if (abs(f) > 1073741823L) {
            arith_error = true;
            f = 0;
        }
    } else {

        if ((abs(mem[f + 1].cint) > 1073741823L) || (abs(mem[f + 2].cint) > 1073741823L)
            || (abs(mem[f + 3].cint) > 1073741823L)) {
            arith_error = true;
            delete_glue_ref(f);
            f = new_spec(mem_bot);
        }
    }
    switch (s) {                /*1579: */
    case 0:
        if ((l >= 2 /*glue_val */ ) && (o != 0 /*expr_none */ )) {
            t = new_spec(f);
            delete_glue_ref(f);
            if (mem[t + 2].cint == 0)
                mem[t].hh.u.B0 = 0 /*normal */ ;
            if (mem[t + 3].cint == 0)
                mem[t].hh.u.B1 = 0 /*normal */ ;
        } else
            t = f;
        break;
    case 3:
        if (o == 4 /*expr_div */ ) {
            n = f;
            o = 5 /*expr_scale */ ;
        } else if (l == 0 /*int_val */ )
            t = mult_and_add(t, f, 0, 2147483647L);
        else if (l == 1 /*dimen_val */ )
            t = mult_and_add(t, f, 0, 1073741823L);
        else {

            mem[t + 1].cint = mult_and_add(mem[t + 1].cint, f, 0, 1073741823L);
            mem[t + 2].cint = mult_and_add(mem[t + 2].cint, f, 0, 1073741823L);
            mem[t + 3].cint = mult_and_add(mem[t + 3].cint, f, 0, 1073741823L);
        }
        break;
    case 4:
        if (l < 2 /*glue_val */ )
            t = quotient(t, f);
        else {

            mem[t + 1].cint = quotient(mem[t + 1].cint, f);
            mem[t + 2].cint = quotient(mem[t + 2].cint, f);
            mem[t + 3].cint = quotient(mem[t + 3].cint, f);
        }
        break;
    case 5:
        if (l == 0 /*int_val */ )
            t = fract(t, n, f, 2147483647L);
        else if (l == 1 /*dimen_val */ )
            t = fract(t, n, f, 1073741823L);
        else {

            mem[t + 1].cint = fract(mem[t + 1].cint, n, f, 1073741823L);
            mem[t + 2].cint = fract(mem[t + 2].cint, n, f, 1073741823L);
            mem[t + 3].cint = fract(mem[t + 3].cint, n, f, 1073741823L);
        }
        break;
    }
    if (o > 2 /*expr_sub */ )
        s = o;
    else {                      /*1580: */

        s = 0 /*expr_none */ ;
        if (r == 0 /*expr_none */ )
            e = t;
        else if (l == 0 /*int_val */ )
            e = add_or_sub(e, t, 2147483647L, r == 2 /*expr_sub */ );
        else if (l == 1 /*dimen_val */ )
            e = add_or_sub(e, t, 1073741823L, r == 2 /*expr_sub */ );
        else {                  /*1582: */

            mem[e + 1].cint = add_or_sub(mem[e + 1].cint, mem[t + 1].cint, 1073741823L, r == 2 /*expr_sub */ );
            if (mem[e].hh.u.B0 == mem[t].hh.u.B0)
                mem[e + 2].cint = add_or_sub(mem[e + 2].cint, mem[t + 2].cint, 1073741823L, r == 2 /*expr_sub */ );
            else if ((mem[e].hh.u.B0 < mem[t].hh.u.B0) && (mem[t + 2].cint != 0)) {
                mem[e + 2].cint = mem[t + 2].cint;
                mem[e].hh.u.B0 = mem[t].hh.u.B0;
            }
            if (mem[e].hh.u.B1 == mem[t].hh.u.B1)
                mem[e + 3].cint = add_or_sub(mem[e + 3].cint, mem[t + 3].cint, 1073741823L, r == 2 /*expr_sub */ );
            else if ((mem[e].hh.u.B1 < mem[t].hh.u.B1) && (mem[t + 3].cint != 0)) {
                mem[e + 3].cint = mem[t + 3].cint;
                mem[e].hh.u.B1 = mem[t].hh.u.B1;
            }
            delete_glue_ref(t);
            if (mem[e + 2].cint == 0)
                mem[e].hh.u.B0 = 0 /*normal */ ;
            if (mem[e + 3].cint == 0)
                mem[e].hh.u.B1 = 0 /*normal */ ;
        }
        r = o;
    }
    b = arith_error;
    if (o != 0 /*expr_none */ )
        goto lab22;
    if (p != -268435455L) {     /*1577: */
        f = e;
        q = p;
        e = mem[q + 1].cint;
        t = mem[q + 2].cint;
        n = mem[q + 3].cint;
        s = mem[q].hh.u.B1 / 4;
        r = mem[q].hh.u.B1 % 4;
        l = mem[q].hh.u.B0;
        p = mem[q].hh.v.RH;
        free_node(q, 4 /*expr_node_size */ );
        goto lab40;
    }
    if (b) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66626L /*"Arithmetic overflow" */ );
        }
        {
            help_ptr = 2;
            help_line[1] = 66917L /*"I can't evaluate this expression," */ ;
            help_line[0] = 66628L /*"since the result is out of range." */ ;
        }
        error();
        if (l >= 2 /*glue_val */ ) {
            delete_glue_ref(e);
            e = mem_bot;
            mem[e].hh.v.RH++;
        } else
            e = 0;
    }
    arith_error = a;
    cur_val = e;
    cur_val_level = l;
}

void scan_normal_glue(void)
{
    scan_normal_glue_regmem scan_glue(2 /*glue_val */ );
}

void scan_mu_glue(void)
{
    scan_mu_glue_regmem scan_glue(3 /*mu_val */ );
}

halfword scan_rule_spec(void)
{
    register halfword Result;
    scan_rule_spec_regmem halfword q;
    q = new_rule();
    if (cur_cmd == 35 /*vrule */ )
        mem[q + 1].cint = 26214 /*default_rule */ ;
    else {

        mem[q + 3].cint = 26214 /*default_rule */ ;
        mem[q + 2].cint = 0;
    }
 lab21:/*reswitch */ if (scan_keyword(66079L /*"width" */ )) {
        scan_dimen(false, false, false);
        mem[q + 1].cint = cur_val;
        goto lab21;
    }
    if (scan_keyword(66080L /*"height" */ )) {
        scan_dimen(false, false, false);
        mem[q + 3].cint = cur_val;
        goto lab21;
    }
    if (scan_keyword(66081L /*"depth" */ )) {
        scan_dimen(false, false, false);
        mem[q + 2].cint = cur_val;
        goto lab21;
    }
    Result = q;
    return Result;
}

void scan_general_text(void)
{
    scan_general_text_regmem unsigned char /*absorbing */ s;
    halfword w;
    halfword d;
    halfword p;
    halfword q;
    halfword unbalance;
    s = scanner_status;
    w = warning_index;
    d = def_ref;
    scanner_status = 5 /*absorbing */ ;
    warning_index = cur_cs;
    def_ref = get_avail();
    mem[def_ref].hh.v.LH = -268435455L;
    p = def_ref;
    scan_left_brace();
    unbalance = 1;
    while (true) {

        get_token();
        if (cur_tok < 6291456L /*right_brace_limit */ ) {

            if (cur_cmd < 2 /*right_brace */ )
                unbalance++;
            else {

                unbalance--;
                if (unbalance == 0)
                    goto lab40;
            }
        }
        {
            q = get_avail();
            mem[p].hh.v.RH = q;
            mem[q].hh.v.LH = cur_tok;
            p = q;
        }
    }
 lab40:                        /*found */ q = mem[def_ref].hh.v.RH;
    {
        mem[def_ref].hh.v.RH = avail;
        avail = def_ref;
    }
    if (q == -268435455L)
        cur_val = mem_top - 3;
    else
        cur_val = p;
    mem[mem_top - 3].hh.v.RH = q;
    scanner_status = s;
    warning_index = w;
    def_ref = d;
}

void pseudo_start(void)
{
    pseudo_start_regmem unsigned char /*max_selector */ old_setting;
    str_number s;
    pool_pointer l, m;
    halfword p, q, r;
    four_quarters w;
    integer nl, sz;
    scan_general_text();
    old_setting = selector;
    selector = SELECTOR_NEW_STRING ;
    token_show(mem_top - 3);
    selector = old_setting;
    flush_list(mem[mem_top - 3].hh.v.RH);
    {
        if (pool_ptr + 1 > pool_size)
            overflow(65539L /*"pool size" */ , pool_size - init_pool_ptr);
    }
    s = make_string();
    str_pool[pool_ptr] = 32 /*" " */ ;
    l = str_start[(s) - 65536L];
    nl = eqtb[8938789L /*int_base 49 */ ].cint;
    p = get_avail();
    q = p;
    while (l < pool_ptr) {

        m = l;
        while ((l < pool_ptr) && (str_pool[l] != nl))
            l++;
        sz = (l - m + 7) / 4;
        if (sz == 1)
            sz = 2;
        r = get_node(sz);
        mem[q].hh.v.RH = r;
        q = r;
        mem[q].hh.v.LH = sz;
        while (sz > 2) {

            sz--;
            r++;
            w.u.B0 = str_pool[m];
            w.u.B1 = str_pool[m + 1];
            w.u.B2 = str_pool[m + 2];
            w.u.B3 = str_pool[m + 3];
            mem[r].qqqq = w;
            m = m + 4;
        }
        w.u.B0 = 32 /*" " */ ;
        w.u.B1 = 32 /*" " */ ;
        w.u.B2 = 32 /*" " */ ;
        w.u.B3 = 32 /*" " */ ;
        if (l > m) {
            w.u.B0 = str_pool[m];
            if (l > m + 1) {
                w.u.B1 = str_pool[m + 1];
                if (l > m + 2) {
                    w.u.B2 = str_pool[m + 2];
                    if (l > m + 3)
                        w.u.B3 = str_pool[m + 3];
                }
            }
        }
        mem[r + 1].qqqq = w;
        if (str_pool[l] == nl)
            l++;
    }
    mem[p].hh.v.LH = mem[p].hh.v.RH;
    mem[p].hh.v.RH = pseudo_files;
    pseudo_files = /*:1542 */ p;
    {
        str_ptr--;
        pool_ptr = str_start[(str_ptr) - 65536L];
    }
    begin_file_reading();
    line = 0;
    cur_input.limit_field = cur_input.start_field;
    cur_input.loc_field = cur_input.limit_field + 1;
    if (eqtb[8938801L /*int_base 61 */ ].cint > 0) {
        if (term_offset > max_print_line - 3)
            print_ln();
        else if ((term_offset > 0) || (file_offset > 0))
            print_char(32 /*" " */ );
        cur_input.name_field = 19;
        print(66902L /*"( " */ );
        open_parens++;
        fflush(stdout);
    } else {

        cur_input.name_field = 18;
        cur_input.synctex_tag_field = 0;
    }
}

halfword zstr_toks_cat(pool_pointer b, small_number cat)
{
    register halfword Result;
    str_toks_cat_regmem halfword p;
    halfword q;
    halfword t;
    pool_pointer k;
    {
        if (pool_ptr + 1 > pool_size)
            overflow(65539L /*"pool size" */ , pool_size - init_pool_ptr);
    }
    p = mem_top - 3;
    mem[p].hh.v.RH = -268435455L;
    k = b;
    while (k < pool_ptr) {

        t = str_pool[k];
        if ((t == 32 /*" " */ ) && (cat == 0))
            t = 20971552L /*space_token */ ;
        else {

            if ((t >= 55296L) && (t <= 56319L) && (k + 1 < pool_ptr) && (str_pool[k + 1] >= 56320L)
                && (str_pool[k + 1] <= 57343L)) {
                k++;
                t = 65536L + (t - 55296L) * 1024 + (str_pool[k] - 56320L);
            }
            if (cat == 0)
                t = 25165824L /*other_token */  + t;
            else
                t = 2097152L /*max_char_val */  * cat + t;
        }
        {
            {
                q = avail;
                if (q == -268435455L)
                    q = get_avail();
                else {

                    avail = mem[q].hh.v.RH;
                    mem[q].hh.v.RH = -268435455L;
                }
            }
            mem[p].hh.v.RH = q;
            mem[q].hh.v.LH = t;
            p = q;
        }
        k++;
    }
    pool_ptr = b;
    Result = p;
    return Result;
}

halfword zstr_toks(pool_pointer b)
{
    register halfword Result;
    str_toks_regmem Result = str_toks_cat(b, 0);
    return Result;
}

halfword the_toks(void)
{
    register halfword Result;
    the_toks_regmem unsigned char /*max_selector */ old_setting;
    halfword p, q, r;
    pool_pointer b;
    small_number c;
    if (odd(cur_chr)) {
        c = cur_chr;
        scan_general_text();
        if (c == 1)
            Result = cur_val;
        else {

            old_setting = selector;
            selector = SELECTOR_NEW_STRING ;
            b = pool_ptr;
            p = get_avail();
            mem[p].hh.v.RH = mem[mem_top - 3].hh.v.RH;
            token_show(p);
            flush_list(p);
            selector = old_setting;
            Result = str_toks(b);
        }
        return Result;
    }
    get_x_token();
    scan_something_internal(5 /*tok_val */ , false);
    if (cur_val_level >= 4 /*ident_val */ ) {   /*485: */
        p = mem_top - 3;
        mem[p].hh.v.RH = -268435455L;
        if (cur_val_level == 4 /*ident_val */ ) {
            q = get_avail();
            mem[p].hh.v.RH = q;
            mem[q].hh.v.LH = 33554431L /*cs_token_flag */  + cur_val;
            p = q;
        } else if (cur_val != -268435455L) {
            r = mem[cur_val].hh.v.RH;
            while (r != -268435455L) {

                {
                    {
                        q = avail;
                        if (q == -268435455L)
                            q = get_avail();
                        else {

                            avail = mem[q].hh.v.RH;
                            mem[q].hh.v.RH = -268435455L;
                        }
                    }
                    mem[p].hh.v.RH = q;
                    mem[q].hh.v.LH = mem[r].hh.v.LH;
                    p = q;
                }
                r = mem[r].hh.v.RH;
            }
        }
        Result = p;
    } else {

        old_setting = selector;
        selector = SELECTOR_NEW_STRING ;
        b = pool_ptr;
        switch (cur_val_level) {
        case 0:
            print_int(cur_val);
            break;
        case 1:
            {
                print_scaled(cur_val);
                print(65693L /*"pt" */ );
            }
            break;
        case 2:
            {
                print_spec(cur_val, 65693L /*"pt" */ );
                delete_glue_ref(cur_val);
            }
            break;
        case 3:
            {
                print_spec(cur_val, 65621L /*"mu" */ );
                delete_glue_ref(cur_val);
            }
            break;
        }
        selector = old_setting;
        Result = str_toks(b);
    }
    return Result;
}

void ins_the_toks(void)
{
    ins_the_toks_regmem mem[mem_top - 12].hh.v.RH = the_toks();
    begin_token_list(mem[mem_top - 3].hh.v.RH, 5 /*inserted */ );
}

void conv_toks(void)
{
    conv_toks_regmem unsigned char /*max_selector */ old_setting;
    halfword save_warning_index, save_def_ref;
    boolean boolvar;
    str_number s;
    str_number u;
    small_number c;
    small_number save_scanner_status;
    pool_pointer b;
    integer fnt, arg1, arg2;
    str_number font_name_str;
    small_number i;
    UTF16_code quote_char;
    small_number cat;
    UnicodeScalar saved_chr;
    halfword p, q;
    cat = 0;
    c = cur_chr;
    switch (c) {
    case 0:
    case 1:
        scan_int();
        break;
    case 2:
    case 3:
        {
            save_scanner_status = scanner_status;
            scanner_status = 0 /*normal */ ;
            get_token();
            scanner_status = save_scanner_status;
        }
        break;
    case 4:
        scan_font_ident();
        break;
    case 13:
        scan_usv_num();
        break;
    case 14:
        {
            scan_usv_num();
            saved_chr = cur_val;
            scan_int();
            if ((cur_val < 1 /*left_brace */ ) || (cur_val > 12 /*other_char */ ) || (cur_val == 5 /*out_param */ )
                || (cur_val == 9 /*ignore */ )) {
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66094L /*"Invalid code (" */ );
                }
                print_int(cur_val);
                print(66095L /*"), should be in the ranges 1..4, 6..8, 10..12" */ );
                {
                    help_ptr = 1;
                    help_line[0] = 66096L /*"I'm going to use 12 instead of that illegal code value." */ ;
                }
                error();
                cat = 12;
            } else
                cat = cur_val;
            cur_val = saved_chr;
        }
        break;
    case 5:
        ;
        break;
    case 43:
        {
            save_scanner_status = scanner_status;
            save_warning_index = warning_index;
            save_def_ref = def_ref;
            if (str_start[(str_ptr) - 65536L] < pool_ptr)
                u = make_string();
            else
                u = 0;
            compare_strings();
            def_ref = save_def_ref;
            warning_index = save_warning_index;
            scanner_status = save_scanner_status;
            if (u != 0)
                str_ptr--;
        }
        break;
    case 44:
        {
            save_scanner_status = scanner_status;
            save_warning_index = warning_index;
            save_def_ref = def_ref;
            if (str_start[(str_ptr) - 65536L] < pool_ptr)
                u = make_string();
            else
                u = 0;
            boolvar = scan_keyword(66826L /*"file" */ );
            scan_pdf_ext_toks();
            if (selector == SELECTOR_NEW_STRING )
                pdf_error(66726L /*"tokens" */ , 66727L /*"tokens_to_string() called while selector = new_string" */ );
            old_setting = selector;
            selector = SELECTOR_NEW_STRING ;
            show_token_list(mem[def_ref].hh.v.RH, -268435455L, pool_size - pool_ptr);
            selector = old_setting;
            s = make_string();
            delete_token_ref(def_ref);
            def_ref = save_def_ref;
            warning_index = save_warning_index;
            scanner_status = save_scanner_status;
            b = pool_ptr;
            getmd5sum(s, boolvar);
            mem[mem_top - 12].hh.v.RH = str_toks(b);
            if ((s == str_ptr - 1)) {
                str_ptr--;
                pool_ptr = str_start[(str_ptr) - 65536L];
            }
            begin_token_list(mem[mem_top - 3].hh.v.RH, 5 /*inserted */ );
            if (u != 0)
                str_ptr--;
            return;
        }
        break;
    case 6:
        ;
        break;
    case 7:
        {
            scan_font_ident();
            fnt = cur_val;
            if ((font_area[fnt] == 65535L /*aat_font_flag */ )) {
                scan_int();
                arg1 = cur_val;
                arg2 = 0;
            } else
                not_aat_font_error(110 /*convert */ , c, fnt);
        }
        break;
    case 8:
        {
            scan_font_ident();
            fnt = cur_val;
            if ((font_area[fnt] == 65535L /*aat_font_flag */ )
                || ((font_area[fnt] == 65534L /*otgr_font_flag */ ) && (usingGraphite(font_layout_engine[fnt])))) {
                scan_int();
                arg1 = cur_val;
                arg2 = 0;
            } else
                not_aat_gr_font_error(110 /*convert */ , c, fnt);
        }
        break;
    case 9:
        {
            scan_font_ident();
            fnt = cur_val;
            if ((font_area[fnt] == 65535L /*aat_font_flag */ )
                || ((font_area[fnt] == 65534L /*otgr_font_flag */ ) && (usingGraphite(font_layout_engine[fnt])))) {
                scan_int();
                arg1 = cur_val;
                scan_int();
                arg2 = cur_val;
            } else
                not_aat_gr_font_error(110 /*convert */ , c, fnt);
        }
        break;
    case 10:
        {
            scan_font_ident();
            fnt = cur_val;
            if (((font_area[fnt] == 65535L /*aat_font_flag */ ) || (font_area[fnt] == 65534L /*otgr_font_flag */ ))) {
                scan_int();
                arg1 = cur_val;
            } else
                not_native_font_error(110 /*convert */ , c, fnt);
        }
        break;
    case 11:
    case 12:
        {
            scan_register_num();
            if (cur_val < 256)
                p = eqtb[2253043L /*box_base */  + cur_val].hh.v.RH;
            else {

                find_sa_element(4, cur_val, false);
                if (cur_ptr == -268435455L)
                    p = -268435455L;
                else
                    p = mem[cur_ptr + 1].hh.v.RH;
            }
            if ((p == -268435455L) || (mem[p].hh.u.B0 != 0 /*hlist_node */ ))
                pdf_error(66827L /*"marginkern" */ , 66828L /*"a non-empty hbox expected" */ );
        }
        break;
    case 15:
        if (job_name == 0)
            open_log_file();
        break;
    }
    old_setting = selector;
    selector = SELECTOR_NEW_STRING ;
    b = pool_ptr;
    switch (c) {
    case 0:
        print_int(cur_val);
        break;
    case 1:
        print_roman_int(cur_val);
        break;
    case 2:
        if (cur_cs != 0)
            sprint_cs(cur_cs);
        else
            print_char(cur_chr);
        break;
    case 3:
        print_meaning();
        break;
    case 4:
        {
            font_name_str = font_name[cur_val];
            if (((font_area[cur_val] == 65535L /*aat_font_flag */ )
                 || (font_area[cur_val] == 65534L /*otgr_font_flag */ ))) {
                quote_char = 34 /*""" */ ;
                {
                    register integer for_end;
                    i = 0;
                    for_end = length(font_name_str) - 1;
                    if (i <= for_end)
                        do
                            if (str_pool[str_start[(font_name_str) - 65536L] + i] == 34 /*""" */ )
                                quote_char = 39 /*"'" */ ;
                        while (i++ < for_end) ;
                }
                print_char(quote_char);
                print(font_name_str);
                print_char(quote_char);
            } else
                print(font_name_str);
            if (font_size[cur_val] != font_dsize[cur_val]) {
                print(66097L /*" at " */ );
                print_scaled(font_size[cur_val]);
                print(65693L /*"pt" */ );
            }
        }
        break;
    case 13:
    case 14:
        print_char(cur_val);
        break;
    case 5:
        print(65536L /*".6" */ );
        break;
    case 43:
        print_int(cur_val);
        break;
    case 6:
        print(65537L /*".99996" */ );
        break;
    case 7:
        if ((font_area[fnt] == 65535L /*aat_font_flag */ ))
            aat_print_font_name(c, font_layout_engine[fnt], arg1, arg2);
        break;
    case 8:
    case 9:
        if ((font_area[fnt] == 65535L /*aat_font_flag */ ))
            aat_print_font_name(c, font_layout_engine[fnt], arg1, arg2);
        else if (((font_area[fnt] == 65534L /*otgr_font_flag */ ) && (usingGraphite(font_layout_engine[fnt]))))
            gr_print_font_name(c, font_layout_engine[fnt], arg1, arg2);
        break;
    case 10:
        if (((font_area[fnt] == 65535L /*aat_font_flag */ ) || (font_area[fnt] == 65534L /*otgr_font_flag */ )))
            print_glyph_name(fnt, arg1);
        break;
    case 11:
        {
            p = mem[p + 5].hh.v.RH;
            while ((p != -268435455L)
                   &&
                   ((!(p >= hi_mem_min)
                     && ((mem[p].hh.u.B0 == 3 /*ins_node */ ) || (mem[p].hh.u.B0 == 4 /*mark_node */ )
                         || (mem[p].hh.u.B0 == 5 /*adjust_node */ ) || (mem[p].hh.u.B0 == 12 /*penalty_node */ )
                         || ((mem[p].hh.u.B0 == 7 /*disc_node */ ) && (mem[p + 1].hh.v.LH == -268435455L)
                             && (mem[p + 1].hh.v.RH == -268435455L) && (mem[p].hh.u.B1 == 0))
                         || ((mem[p].hh.u.B0 == 9 /*math_node */ ) && (mem[p + 1].cint == 0))
                         || ((mem[p].hh.u.B0 == 11 /*kern_node */ )
                             && ((mem[p + 1].cint == 0) || (mem[p].hh.u.B1 == 0 /*normal */ )))
                         || ((mem[p].hh.u.B0 == 10 /*glue_node */ ) && (mem[p + 1].hh.v.LH == mem_bot))
                         || ((mem[p].hh.u.B0 == 0 /*hlist_node */ ) && (mem[p + 1].cint == 0) && (mem[p + 3].cint == 0)
                             && (mem[p + 2].cint == 0) && (mem[p + 5].hh.v.RH == -268435455L))))
                    || ((!(p >= hi_mem_min)) && (mem[p].hh.u.B0 == 10 /*glue_node */ )
                        && (mem[p].hh.u.B1 == 8 /*left_skip_code 1 */ ))))
                p = mem[p].hh.v.RH;
            if ((p != -268435455L) && (!(p >= hi_mem_min)) && (mem[p].hh.u.B0 == 40 /*margin_kern_node */ )
                && (mem[p].hh.u.B1 == 0))
                print_scaled(mem[p + 1].cint);
            else
                print(48 /*"0" */ );
            print(65693L /*"pt" */ );
        }
        break;
    case 12:
        {
            q = mem[p + 5].hh.v.RH;
            p = prev_rightmost(q, -268435455L);
            while ((p != -268435455L)
                   &&
                   ((!(p >= hi_mem_min)
                     && ((mem[p].hh.u.B0 == 3 /*ins_node */ ) || (mem[p].hh.u.B0 == 4 /*mark_node */ )
                         || (mem[p].hh.u.B0 == 5 /*adjust_node */ ) || (mem[p].hh.u.B0 == 12 /*penalty_node */ )
                         || ((mem[p].hh.u.B0 == 7 /*disc_node */ ) && (mem[p + 1].hh.v.LH == -268435455L)
                             && (mem[p + 1].hh.v.RH == -268435455L) && (mem[p].hh.u.B1 == 0))
                         || ((mem[p].hh.u.B0 == 9 /*math_node */ ) && (mem[p + 1].cint == 0))
                         || ((mem[p].hh.u.B0 == 11 /*kern_node */ )
                             && ((mem[p + 1].cint == 0) || (mem[p].hh.u.B1 == 0 /*normal */ )))
                         || ((mem[p].hh.u.B0 == 10 /*glue_node */ ) && (mem[p + 1].hh.v.LH == mem_bot))
                         || ((mem[p].hh.u.B0 == 0 /*hlist_node */ ) && (mem[p + 1].cint == 0) && (mem[p + 3].cint == 0)
                             && (mem[p + 2].cint == 0) && (mem[p + 5].hh.v.RH == -268435455L))))
                    || ((!(p >= hi_mem_min)) && (mem[p].hh.u.B0 == 10 /*glue_node */ )
                        && (mem[p].hh.u.B1 == 9 /*right_skip_code 1 */ ))))
                p = prev_rightmost(q, p);
            if ((p != -268435455L) && (!(p >= hi_mem_min)) && (mem[p].hh.u.B0 == 40 /*margin_kern_node */ )
                && (mem[p].hh.u.B1 == 1))
                print_scaled(mem[p + 1].cint);
            else
                print(48 /*"0" */ );
            print(65693L /*"pt" */ );
        }
        break;
    case 15:
        print_file_name(job_name, 0, 0);
        break;
    }
    selector = old_setting;
    mem[mem_top - 12].hh.v.RH = str_toks_cat(b, cat);
    begin_token_list(mem[mem_top - 3].hh.v.RH, 5 /*inserted */ );
}

halfword zscan_toks(boolean macro_def, boolean xpand)
{
    register halfword Result;
    scan_toks_regmem halfword t;
    halfword s;
    halfword p;
    halfword q;
    halfword unbalance;
    halfword hash_brace;
    if (macro_def)
        scanner_status = 2 /*defining */ ;
    else
        scanner_status = 5 /*absorbing */ ;
    warning_index = cur_cs;
    def_ref = get_avail();
    mem[def_ref].hh.v.LH = -268435455L;
    p = def_ref;
    hash_brace = 0;
    t = 25165872L /*zero_token */ ;
    if (macro_def) {            /*493: */
        while (true) {

            get_token();
            if (cur_tok < 6291456L /*right_brace_limit */ )
                goto lab31;
            if (cur_cmd == 6 /*mac_param */ ) { /*495: */
                s = 27262976L /*match_token */  + cur_chr;
                get_token();
                if (cur_cmd == 1 /*left_brace */ ) {
                    hash_brace = cur_tok;
                    {
                        q = get_avail();
                        mem[p].hh.v.RH = q;
                        mem[q].hh.v.LH = cur_tok;
                        p = q;
                    }
                    {
                        q = get_avail();
                        mem[p].hh.v.RH = q;
                        mem[q].hh.v.LH = 29360128L /*end_match_token */ ;
                        p = q;
                    }
                    goto lab30;
                }
                if (t == 25165881L /*zero_token 9 */ ) {
                    {
                        if (interaction == 3 /*error_stop_mode */ ) ;
                        if (file_line_error_style_p)
                            print_file_line();
                        else
                            print_nl(65544L /*"! " */ );
                        print(66100L /*"You already have nine parameters" */ );
                    }
                    {
                        help_ptr = 1;
                        help_line[0] = 66101L /*"I'm going to ignore the # sign you just used." */ ;
                    }
                    error();
                } else {

                    t++;
                    if (cur_tok != t) {
                        {
                            if (interaction == 3 /*error_stop_mode */ ) ;
                            if (file_line_error_style_p)
                                print_file_line();
                            else
                                print_nl(65544L /*"! " */ );
                            print(66102L /*"Parameters must be numbered consecutively" */ );
                        }
                        {
                            help_ptr = 2;
                            help_line[1] = 66103L /*"I've inserted the digit you should have used after the #." */ ;
                            help_line[0] = 66104L /*"Type `1' to delete what you did use." */ ;
                        }
                        back_error();
                    }
                    cur_tok = s;
                }
            }
            {
                q = get_avail();
                mem[p].hh.v.RH = q;
                mem[q].hh.v.LH = cur_tok;
                p = q;
            }
        }
 lab31:                        /*done1 */  {

            q = get_avail();
            mem[p].hh.v.RH = q;
            mem[q].hh.v.LH = 29360128L /*end_match_token */ ;
            p = q;
        }
        if (cur_cmd == 2 /*right_brace */ ) {   /*494: */
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(65980L /*"Missing _ inserted" */ );
            }
            align_state++;
            {
                help_ptr = 2;
                help_line[1] = 66098L /*"Where was the left brace? You said something like `\def\a_'," */ ;
                help_line[0] = 66099L /*"which I'm going to interpret as `\def\a__'." */ ;
            }
            error();
            goto lab40;
        }
 lab30:                        /*done */ ;
    } else
        scan_left_brace();
    unbalance = 1;
    while (true) {

        if (xpand) {            /*497: */
            while (true) {

                get_next();
                if (cur_cmd >= 113 /*call */ ) {

                    if (mem[mem[cur_chr].hh.v.RH].hh.v.LH == 29360129L /*protected_token */ ) {
                        cur_cmd = 0 /*relax */ ;
                        cur_chr = 65537L /*no_expand_flag */ ;
                    }
                }
                if (cur_cmd <= 102 /*max_command */ )
                    goto lab32;
                if (cur_cmd != 111 /*the */ )
                    expand();
                else {

                    q = the_toks();
                    if (mem[mem_top - 3].hh.v.RH != -268435455L) {
                        mem[p].hh.v.RH = mem[mem_top - 3].hh.v.RH;
                        p = q;
                    }
                }
            }
 lab32:                        /*done2 */ x_token();
        } else
            get_token();
        if (cur_tok < 6291456L /*right_brace_limit */ ) {

            if (cur_cmd < 2 /*right_brace */ )
                unbalance++;
            else {

                unbalance--;
                if (unbalance == 0)
                    goto lab40;
            }
        } else if (cur_cmd == 6 /*mac_param */ ) {

            if (macro_def) {    /*498: */
                s = cur_tok;
                if (xpand)
                    get_x_token();
                else
                    get_token();
                if (cur_cmd != 6 /*mac_param */ ) {

                    if ((cur_tok <= 25165872L /*zero_token */ ) || (cur_tok > t)) {
                        {
                            if (interaction == 3 /*error_stop_mode */ ) ;
                            if (file_line_error_style_p)
                                print_file_line();
                            else
                                print_nl(65544L /*"! " */ );
                            print(66105L /*"Illegal parameter number in definition of " */ );
                        }
                        sprint_cs(warning_index);
                        {
                            help_ptr = 3;
                            help_line[2] = 66106L /*"You meant to type ## instead of #, right?" */ ;
                            help_line[1] = 66107L /*"Or maybe a _ was forgotten somewhere earlier, and things" */ ;
                            help_line[0] = 66108L /*"are all screwed up? I'm going to assume that you meant ##." */ ;
                        }
                        back_error();
                        cur_tok = s;
                    } else
                        cur_tok = 10485712L /*out_param_token -48 */  + cur_chr;
                }
            }
        }
        {
            q = get_avail();
            mem[p].hh.v.RH = q;
            mem[q].hh.v.LH = cur_tok;
            p = q;
        }
    }
 lab40:/*found */ scanner_status = 0 /*normal */ ;
    if (hash_brace != 0) {
        q = get_avail();
        mem[p].hh.v.RH = q;
        mem[q].hh.v.LH = hash_brace;
        p = q;
    }
    Result = p;
    return Result;
}

void zread_toks(integer n, halfword r, halfword j)
{
    read_toks_regmem halfword p;
    halfword q;
    integer s;
    small_number m;
    scanner_status = 2 /*defining */ ;
    warning_index = r;
    def_ref = get_avail();
    mem[def_ref].hh.v.LH = -268435455L;
    p = def_ref;
    {
        q = get_avail();
        mem[p].hh.v.RH = q;
        mem[q].hh.v.LH = 29360128L /*end_match_token */ ;
        p = q;
    }
    if ((n < 0) || (n > 15))
        m = 16;
    else
        m = n;
    s = align_state;
    align_state = 1000000L;
    do {
        /*502: */ begin_file_reading();
        cur_input.name_field = m + 1;
        if (read_open[m] == 2 /*closed */ ) {   /*503: */

            if (interaction > 1 /*nonstop_mode */ ) {

                if (n < 0) {
                    ;
                    print(65622L /*"" */ );
                    term_input();
                } else {

                    ;
                    print_ln();
                    sprint_cs(r);
                    {
                        ;
                        print(61 /*"=" */ );
                        term_input();
                    }
                    n = -1;
                }
            } else
                fatal_error(66109L /*"*** (cannot \read from terminal in nonstop modes)" */ );
        } else if (read_open[m] == 1 /*just_open */ ) { /*504: */

            if (input_line(read_file[m]))
                read_open[m] = 0 /*normal */ ;
            else {

                u_close(read_file[m]);
                read_open[m] = 2 /*closed */ ;
            }
        } else {                /*505: */

            if (!input_line(read_file[m])) {
                u_close(read_file[m]);
                read_open[m] = 2 /*closed */ ;
                if (align_state != 1000000L) {
                    runaway();
                    {
                        if (interaction == 3 /*error_stop_mode */ ) ;
                        if (file_line_error_style_p)
                            print_file_line();
                        else
                            print_nl(65544L /*"! " */ );
                        print(66110L /*"File ended within " */ );
                    }
                    print_esc(65850L /*"read" */ );
                    {
                        help_ptr = 1;
                        help_line[0] = 66111L /*"This \read has unbalanced braces." */ ;
                    }
                    align_state = 1000000L;
                    error();
                }
            }
        }
        cur_input.limit_field = last;
        if ((eqtb[8938788L /*int_base 48 */ ].cint < 0) || (eqtb[8938788L /*int_base 48 */ ].cint > 255))
            cur_input.limit_field--;
        else
            buffer[cur_input.limit_field] = eqtb[8938788L /*int_base 48 */ ].cint;
        first = cur_input.limit_field + 1;
        cur_input.loc_field = cur_input.start_field;
        cur_input.state_field = 33 /*new_line */ ;
        if (j == 1) {
            while (cur_input.loc_field <= cur_input.limit_field) {

                cur_chr = buffer[cur_input.loc_field];
                cur_input.loc_field++;
                if (cur_chr == 32 /*" " */ )
                    cur_tok = 20971552L /*space_token */ ;
                else
                    cur_tok = cur_chr + 25165824L;
                {
                    q = get_avail();
                    mem[p].hh.v.RH = q;
                    mem[q].hh.v.LH = cur_tok;
                    p = q;
                }
            }
            goto lab30;
        }
        while (true) {

            get_token();
            if (cur_tok == 0)
                goto lab30;
            if (align_state < 1000000L) {
                do {
                    get_token();
                } while (!(cur_tok == 0));
                align_state = 1000000L;
                goto lab30;
            }
            {
                q = get_avail();
                mem[p].hh.v.RH = q;
                mem[q].hh.v.LH = cur_tok;
                p = q;
            }
        }
 lab30:                        /*done */ end_file_reading();
    } while (!(align_state == 1000000L));
    cur_val = def_ref;
    scanner_status = 0 /*normal */ ;
    align_state = s;
}

void pass_text(void)
{
    pass_text_regmem integer l;
    small_number save_scanner_status;
    save_scanner_status = scanner_status;
    scanner_status = 1 /*skipping */ ;
    l = 0;
    skip_line = line;
    while (true) {

        get_next();
        if (cur_cmd == 108 /*fi_or_else */ ) {
            if (l == 0)
                goto lab30;
            if (cur_chr == 2 /*fi_code */ )
                l--;
        } else if (cur_cmd == 107 /*if_test */ )
            l++;
    }
 lab30:                        /*done */ scanner_status = save_scanner_status;
    if (eqtb[8938800L /*int_base 60 */ ].cint > 0)
        show_cur_cmd_chr();
}

void zchange_if_limit(small_number l, halfword p)
{
    change_if_limit_regmem halfword q;
    if (p == cond_ptr)
        if_limit = l;
    else {

        q = cond_ptr;
        while (true) {

            if (q == -268435455L)
                confusion(66112L /*"if" */ );
            if (mem[q].hh.v.RH == p) {
                mem[q].hh.u.B0 = l;
                return;
            }
            q = mem[q].hh.v.RH;
        }
    }
}

void conditional(void)
{
    conditional_regmem boolean b;
    boolean e;
    unsigned char /*">" */ r;
    integer m, n;
    halfword p, q;
    small_number save_scanner_status;
    halfword save_cond_ptr;
    small_number this_if;
    boolean is_unless;
    if (eqtb[8938800L /*int_base 60 */ ].cint > 0) {

        if (eqtb[8938776L /*int_base 36 */ ].cint <= 1)
            show_cur_cmd_chr();
    }
    {
        p = get_node(2 /*if_node_size */ );
        mem[p].hh.v.RH = cond_ptr;
        mem[p].hh.u.B0 = if_limit;
        mem[p].hh.u.B1 = cur_if;
        mem[p + 1].cint = if_line;
        cond_ptr = p;
        cur_if = cur_chr;
        if_limit = 1 /*if_code */ ;
        if_line = line;
    }
    save_cond_ptr = cond_ptr;
    is_unless = (cur_chr >= 32 /*unless_code */ );
    this_if = cur_chr % 32 /*unless_code */ ;
    switch (this_if) {
    case 0:
    case 1:
        {
            {
                get_x_token();
                if (cur_cmd == 0 /*relax */ ) {

                    if (cur_chr == 65537L /*no_expand_flag */ ) {
                        cur_cmd = 13 /*active_char */ ;
                        cur_chr = cur_tok - 33554432L;
                    }
                }
            }
            if ((cur_cmd > 13 /*active_char */ ) || (cur_chr > 1114111L /*biggest_usv */ )) {
                m = 0 /*relax */ ;
                n = 1114112L /*too_big_usv */ ;
            } else {

                m = cur_cmd;
                n = cur_chr;
            }
            {
                get_x_token();
                if (cur_cmd == 0 /*relax */ ) {

                    if (cur_chr == 65537L /*no_expand_flag */ ) {
                        cur_cmd = 13 /*active_char */ ;
                        cur_chr = cur_tok - 33554432L;
                    }
                }
            }
            if ((cur_cmd > 13 /*active_char */ ) || (cur_chr > 1114111L /*biggest_usv */ )) {
                cur_cmd = 0 /*relax */ ;
                cur_chr = 1114112L /*too_big_usv */ ;
            }
            if (this_if == 0 /*if_char_code */ )
                b = (n == cur_chr);
            else
                b = (m == cur_cmd);
        }
        break;
    case 2:
    case 3:
        {
            if (this_if == 2 /*if_int_code */ )
                scan_int();
            else
                scan_dimen(false, false, false);
            n = cur_val;
            do {
                get_x_token();
            } while (!(cur_cmd != 10 /*spacer *//*:424 */ ));
            if ((cur_tok >= 25165884L /*other_token 60 */ ) && (cur_tok <= 25165886L /*other_token 62 */ ))
                r = cur_tok - 25165824L;
            else {

                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66138L /*"Missing = inserted for " */ );
                }
                print_cmd_chr(107 /*if_test */ , this_if);
                {
                    help_ptr = 1;
                    help_line[0] = 66139L /*"I was expecting to see `<', `=', or `>'. Didn't." */ ;
                }
                back_error();
                r = 61 /*"=" */ ;
            }
            if (this_if == 2 /*if_int_code */ )
                scan_int();
            else
                scan_dimen(false, false, false);
            switch (r) {
            case 60:
                b = (n < cur_val);
                break;
            case 61:
                b = (n == cur_val);
                break;
            case 62:
                b = (n > cur_val);
                break;
            }
        }
        break;
    case 4:
        {
            scan_int();
            b = odd(cur_val);
        }
        break;
    case 5:
        b = (abs(cur_list.mode_field) == 1 /*vmode */ );
        break;
    case 6:
        b = (abs(cur_list.mode_field) == 104 /*hmode */ );
        break;
    case 7:
        b = (abs(cur_list.mode_field) == 207 /*mmode */ );
        break;
    case 8:
        b = (cur_list.mode_field < 0);
        break;
    case 9:
    case 10:
    case 11:
        {
            scan_register_num();
            if (cur_val < 256)
                p = eqtb[2253043L /*box_base */  + cur_val].hh.v.RH;
            else {

                find_sa_element(4, cur_val, false);
                if (cur_ptr == -268435455L)
                    p = -268435455L;
                else
                    p = mem[cur_ptr + 1].hh.v.RH;
            }
            if (this_if == 9 /*if_void_code */ )
                b = (p == -268435455L);
            else if (p == -268435455L)
                b = false;
            else if (this_if == 10 /*if_hbox_code */ )
                b = (mem[p].hh.u.B0 == 0 /*hlist_node */ );
            else
                b = (mem[p].hh.u.B0 == 1 /*vlist_node */ );
        }
        break;
    case 12:
        {
            save_scanner_status = scanner_status;
            scanner_status = 0 /*normal */ ;
            get_next();
            n = cur_cs;
            p = cur_cmd;
            q = cur_chr;
            get_next();
            if (cur_cmd != p)
                b = false;
            else if (cur_cmd < 113 /*call */ )
                b = (cur_chr == q);
            else {              /*527: */

                p = mem[cur_chr].hh.v.RH;
                q = mem[eqtb[n].hh.v.RH].hh.v.RH;
                if (p == q)
                    b = true;
                else {

                    while ((p != -268435455L) && (q != -268435455L))
                        if (mem[p].hh.v.LH != mem[q].hh.v.LH)
                            p = -268435455L;
                        else {

                            p = mem[p].hh.v.RH;
                            q = mem[q].hh.v.RH;
                        }
                    b = ((p == -268435455L) && (q == -268435455L));
                }
            }
            scanner_status = save_scanner_status;
        }
        break;
    case 13:
        {
            scan_four_bit_int_or_18();
            if (cur_val == 18)
                b = 1; /* !shellenabledp */
            else
                b = (read_open[cur_val] == 2 /*closed */ );
        }
        break;
    case 14:
        b = true;
        break;
    case 15:
        b = false;
        break;
    case 17:
        {
            save_scanner_status = scanner_status;
            scanner_status = 0 /*normal */ ;
            get_next();
            b = (cur_cmd != 103 /*undefined_cs */ );
            scanner_status = save_scanner_status;
        }
        break;
    case 18:
        {
            n = get_avail();
            p = n;
            e = is_in_csname;
            is_in_csname = true;
            do {
                get_x_token();
                if (cur_cs == 0) {
                    q = get_avail();
                    mem[p].hh.v.RH = q;
                    mem[q].hh.v.LH = cur_tok;
                    p = q;
                }
            } while (!(cur_cs != 0));
            if (cur_cmd != 67 /*end_cs_name */ ) {      /*391: */
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(65948L /*"Missing " */ );
                }
                print_esc(65810L /*"endcsname" */ );
                print(65949L /*" inserted" */ );
                {
                    help_ptr = 2;
                    help_line[1] = 65950L /*"The control sequence marked <to be read again> should" */ ;
                    help_line[0] = 65951L /*"not appear between \csname and \endcsname." */ ;
                }
                back_error();
            }
            m = first;
            p = mem[n].hh.v.RH;
            while (p != -268435455L) {

                if (m >= max_buf_stack) {
                    max_buf_stack = m + 1;
                    if (max_buf_stack == buf_size)
                        overflow(65538L /*"buffer size" */ , buf_size);
                }
                buffer[m] = mem[p].hh.v.LH % 2097152L /*max_char_val */ ;
                m++;
                p = mem[p].hh.v.RH;
            }
            if (m > first + 1)
                cur_cs = id_lookup(first, m - first);
            else if (m == first)
                cur_cs = 2228225L /*null_cs */ ;
            else
                cur_cs = 1114113L /*single_base */  + buffer[first]     /*
                                                                           :1556 */ ;
            flush_list(n);
            b = (eqtb[cur_cs].hh.u.B0 != 103 /*undefined_cs */ );
            is_in_csname = e;
        }
        break;
    case 20:
        b = is_in_csname;
        break;
    case 19:
        {
            scan_font_ident();
            n = cur_val;
            scan_usv_num();
            if (((font_area[n] == 65535L /*aat_font_flag */ ) || (font_area[n] == 65534L /*otgr_font_flag */ )))
                b = (map_char_to_glyph(n, cur_val) > 0);
            else {

                if ((font_bc[n] <= cur_val) && (font_ec[n] >= cur_val))
                    b = (font_info[char_base[n] + effective_char(true, n, cur_val)].qqqq.u.B0 > 0 /*min_quarterword */ );
                else
                    b = false;
            }
        }
        break;
    case 16:
        {
            scan_int();
            n = cur_val;
            if (eqtb[8938776L /*int_base 36 */ ].cint > 1) {
                begin_diagnostic();
                print(66140L /*"_case " */ );
                print_int(n);
                print_char(125 /*"_" */ );
                end_diagnostic(false);
            }
            while (n != 0) {

                pass_text();
                if (cond_ptr == save_cond_ptr) {

                    if (cur_chr == 4 /*or_code */ )
                        n--;
                    else
                        goto lab50;
                } else if (cur_chr == 2 /*fi_code */ ) {        /*515: */
                    if (if_stack[in_open] == cond_ptr)
                        if_warning();
                    p = cond_ptr;
                    if_line = mem[p + 1].cint;
                    cur_if = mem[p].hh.u.B1;
                    if_limit = mem[p].hh.u.B0;
                    cond_ptr = mem[p].hh.v.RH;
                    free_node(p, 2 /*if_node_size */ );
                }
            }
            change_if_limit(4 /*or_code */ , save_cond_ptr);
            return;
        }
        break;
    case 21:
        {
            save_scanner_status = scanner_status;
            scanner_status = 0 /*normal */ ;
            get_next();
            scanner_status = save_scanner_status;
            if (cur_cs < 2228226L /*hash_base */ )
                m = prim_lookup(cur_cs - 257);
            else
                m = prim_lookup(hash[cur_cs].v.RH);
            b = ((cur_cmd != 103 /*undefined_cs */ ) && (m != 0 /*undefined_primitive */ )
                 && (cur_cmd == prim_eqtb[m].hh.u.B0) && (cur_chr == prim_eqtb[m].hh.v.RH));
        }
        break;
    }
    if (is_unless)
        b = !b;
    if (eqtb[8938776L /*int_base 36 */ ].cint > 1) {    /*521: */
        begin_diagnostic();
        if (b)
            print(66136L /*"_true_" */ );
        else
            print(66137L /*"_false_" */ );
        end_diagnostic(false);
    }
    if (b) {
        change_if_limit(3 /*else_code */ , save_cond_ptr);
        return;
    }
    while (true) {

        pass_text();
        if (cond_ptr == save_cond_ptr) {
            if (cur_chr != 4 /*or_code */ )
                goto lab50;
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66134L /*"Extra " */ );
            }
            print_esc(66132L /*"or" */ );
            {
                help_ptr = 1;
                help_line[0] = 66135L /*"I'm ignoring this; it doesn't match any \if." */ ;
            }
            error();
        } else if (cur_chr == 2 /*fi_code */ ) {        /*515: */
            if (if_stack[in_open] == cond_ptr)
                if_warning();
            p = cond_ptr;
            if_line = mem[p + 1].cint;
            cur_if = mem[p].hh.u.B1;
            if_limit = mem[p].hh.u.B0;
            cond_ptr = mem[p].hh.v.RH;
            free_node(p, 2 /*if_node_size */ );
        }
    }
 lab50:/*common_ending */ if (cur_chr == 2 /*fi_code */ ) {    /*515: */
        if (if_stack[in_open] == cond_ptr)
            if_warning();
        p = cond_ptr;
        if_line = mem[p + 1].cint;
        cur_if = mem[p].hh.u.B1;
        if_limit = mem[p].hh.u.B0;
        cond_ptr = mem[p].hh.v.RH;
        free_node(p, 2 /*if_node_size */ );
    } else
        if_limit = 2 /*fi_code */ ;
}

void begin_name(void)
{
    begin_name_regmem area_delimiter = 0;
    ext_delimiter = 0;
    quoted_filename = false;
    file_name_quote_char = 0;
}

boolean zmore_name(UTF16_code c)
{
    register boolean Result;
    more_name_regmem if (stop_at_space && (c == 32 /*" " */ ) && (file_name_quote_char == 0))
        Result = false;
    else if (stop_at_space && (file_name_quote_char != 0) && (c == file_name_quote_char)) {
        file_name_quote_char = 0;
        Result = true;
    } else if (stop_at_space && (file_name_quote_char == 0) && ((c == 34 /*""" */ ) || (c == 39 /*"'" */ ))) {
        file_name_quote_char = c;
        quoted_filename = true;
        Result = true;
    } else {

        {
            if (pool_ptr + 1 > pool_size)
                overflow(65539L /*"pool size" */ , pool_size - init_pool_ptr);
        }
        {
            str_pool[pool_ptr] = c;
            pool_ptr++;
        }
        if (IS_DIR_SEP(c)) {
            area_delimiter = (pool_ptr - str_start[(str_ptr) - 65536L]);
            ext_delimiter = 0;
        } else if (c == 46 /*"." */ )
            ext_delimiter = (pool_ptr - str_start[(str_ptr) - 65536L]);
        Result = true;
    }
    return Result;
}

void end_name(void)
{
    end_name_regmem str_number temp_str;
    pool_pointer j;
    if (str_ptr + 3 > max_strings)
        overflow(65540L /*"number of strings" */ , max_strings - init_str_ptr);
    if (area_delimiter == 0)
        cur_area = 65622L /*"" */ ;
    else {

        cur_area = str_ptr;
        str_start[(str_ptr + 1) - 65536L] = str_start[(str_ptr) - 65536L] + area_delimiter;
        str_ptr++;
        temp_str = search_string(cur_area);
        if (temp_str > 0) {
            cur_area = temp_str;
            str_ptr--;
            {
                register integer for_end;
                j = str_start[(str_ptr + 1) - 65536L];
                for_end = pool_ptr - 1;
                if (j <= for_end)
                    do {
                        str_pool[j - area_delimiter] = str_pool[j];
                    }
                    while (j++ < for_end);
            }
            pool_ptr = pool_ptr - area_delimiter;
        }
    }
    if (ext_delimiter == 0) {
        cur_ext = 65622L /*"" */ ;
        cur_name = slow_make_string();
    } else {

        cur_name = str_ptr;
        str_start[(str_ptr + 1) - 65536L] = str_start[(str_ptr) - 65536L] + ext_delimiter - area_delimiter - 1;
        str_ptr++;
        cur_ext = make_string();
        str_ptr--;
        temp_str = search_string(cur_name);
        if (temp_str > 0) {
            cur_name = temp_str;
            str_ptr--;
            {
                register integer for_end;
                j = str_start[(str_ptr + 1) - 65536L];
                for_end = pool_ptr - 1;
                if (j <= for_end)
                    do {
                        str_pool[j - ext_delimiter + area_delimiter + 1] = str_pool[j];
                    }
                    while (j++ < for_end);
            }
            pool_ptr = pool_ptr - ext_delimiter + area_delimiter + 1;
        }
        cur_ext = slow_make_string();
    }
}

void zpack_file_name(str_number n, str_number a, str_number e)
{
    pack_file_name_regmem integer k;
    UTF16_code c;
    pool_pointer j;
    k = 0;
    if (name_of_file)
        free(name_of_file);
    name_of_file = xmalloc_array(UTF8_code, (length(a) + length(n) + length(e)) * 3 + 1);
    {
        register integer for_end;
        j = str_start[(a) - 65536L];
        for_end = str_start[(a + 1) - 65536L] - 1;
        if (j <= for_end)
            do {
                c = str_pool[j];
                k++;
                if (k <= INTEGER_MAX) {
                    if ((c < 128))
                        name_of_file[k] = c;
                    else if ((c < 2048)) {
                        name_of_file[k] = 192 + c / 64;
                        k++;
                        name_of_file[k] = 128 + c % 64;
                    } else {

                        name_of_file[k] = 224 + c / 4096;
                        k++;
                        name_of_file[k] = 128 + (c % 4096) / 64;
                        k++;
                        name_of_file[k] = 128 + (c % 4096) % 64;
                    }
                }
            }
            while (j++ < for_end);
    }
    {
        register integer for_end;
        j = str_start[(n) - 65536L];
        for_end = str_start[(n + 1) - 65536L] - 1;
        if (j <= for_end)
            do {
                c = str_pool[j];
                k++;
                if (k <= INTEGER_MAX) {
                    if ((c < 128))
                        name_of_file[k] = c;
                    else if ((c < 2048)) {
                        name_of_file[k] = 192 + c / 64;
                        k++;
                        name_of_file[k] = 128 + c % 64;
                    } else {

                        name_of_file[k] = 224 + c / 4096;
                        k++;
                        name_of_file[k] = 128 + (c % 4096) / 64;
                        k++;
                        name_of_file[k] = 128 + (c % 4096) % 64;
                    }
                }
            }
            while (j++ < for_end);
    }
    {
        register integer for_end;
        j = str_start[(e) - 65536L];
        for_end = str_start[(e + 1) - 65536L] - 1;
        if (j <= for_end)
            do {
                c = str_pool[j];
                k++;
                if (k <= INTEGER_MAX) {
                    if ((c < 128))
                        name_of_file[k] = c;
                    else if ((c < 2048)) {
                        name_of_file[k] = 192 + c / 64;
                        k++;
                        name_of_file[k] = 128 + c % 64;
                    } else {

                        name_of_file[k] = 224 + c / 4096;
                        k++;
                        name_of_file[k] = 128 + (c % 4096) / 64;
                        k++;
                        name_of_file[k] = 128 + (c % 4096) % 64;
                    }
                }
            }
            while (j++ < for_end);
    }
    if (k <= INTEGER_MAX)
        name_length = k;
    else
        name_length = INTEGER_MAX;
    name_of_file[name_length + 1] = 0;
}

void zpack_buffered_name(small_number n, integer a, integer b)
{
    pack_buffered_name_regmem integer k;
    UTF16_code c;
    integer j;
    if (n + b - a + 5 > INTEGER_MAX)
        b = a + INTEGER_MAX - n - 5;
    k = 0;
    if (name_of_file)
        free(name_of_file);
    name_of_file = xmalloc_array(UTF8_code, n + (b - a + 1) + 5);
    {
        register integer for_end;
        j = 1;
        for_end = n;
        if (j <= for_end)
            do {
                c = TEX_format_default[j];
                k++;
                if (k <= INTEGER_MAX) {
                    if ((c < 128))
                        name_of_file[k] = c;
                    else if ((c < 2048)) {
                        name_of_file[k] = 192 + c / 64;
                        k++;
                        name_of_file[k] = 128 + c % 64;
                    } else {

                        name_of_file[k] = 224 + c / 4096;
                        k++;
                        name_of_file[k] = 128 + (c % 4096) / 64;
                        k++;
                        name_of_file[k] = 128 + (c % 4096) % 64;
                    }
                }
            }
            while (j++ < for_end);
    }
    {
        register integer for_end;
        j = a;
        for_end = b;
        if (j <= for_end)
            do {
                c = buffer[j];
                k++;
                if (k <= INTEGER_MAX) {
                    if ((c < 128))
                        name_of_file[k] = c;
                    else if ((c < 2048)) {
                        name_of_file[k] = 192 + c / 64;
                        k++;
                        name_of_file[k] = 128 + c % 64;
                    } else {

                        name_of_file[k] = 224 + c / 4096;
                        k++;
                        name_of_file[k] = 128 + (c % 4096) / 64;
                        k++;
                        name_of_file[k] = 128 + (c % 4096) % 64;
                    }
                }
            }
            while (j++ < for_end);
    }
    {
        register integer for_end;
        j = format_default_length - 3;
        for_end = format_default_length;
        if (j <= for_end)
            do {
                c = TEX_format_default[j];
                k++;
                if (k <= INTEGER_MAX) {
                    if ((c < 128))
                        name_of_file[k] = c;
                    else if ((c < 2048)) {
                        name_of_file[k] = 192 + c / 64;
                        k++;
                        name_of_file[k] = 128 + c % 64;
                    } else {

                        name_of_file[k] = 224 + c / 4096;
                        k++;
                        name_of_file[k] = 128 + (c % 4096) / 64;
                        k++;
                        name_of_file[k] = 128 + (c % 4096) % 64;
                    }
                }
            }
            while (j++ < for_end);
    }
    if (k <= INTEGER_MAX)
        name_length = k;
    else
        name_length = INTEGER_MAX;
    name_of_file[name_length + 1] = 0;
}

str_number
make_name_string(void)
{
    str_number Result;
    integer k;
    pool_pointer save_area_delimiter, save_ext_delimiter;
    boolean save_name_in_progress, save_stop_at_space;

    if ((pool_ptr + name_length > pool_size) || (str_ptr == max_strings)
        || ((pool_ptr - str_start[(str_ptr) - 65536L]) > 0))
        Result = 63 /*"?" */ ;
    else {

        make_utf16_name();
        {
            register integer for_end;
            k = 0;
            for_end = name_length16 - 1;
            if (k <= for_end)
                do {
                    str_pool[pool_ptr] = name_of_file16[k];
                    pool_ptr++;
                }
                while (k++ < for_end);
        }
        Result = make_string();
        save_area_delimiter = area_delimiter;
        save_ext_delimiter = ext_delimiter;
        save_name_in_progress = name_in_progress;
        save_stop_at_space = stop_at_space;
        name_in_progress = true;
        begin_name();
        stop_at_space = false;
        k = 0;
        while ((k < name_length16) && (more_name(name_of_file16[k])))
            k++;
        stop_at_space = save_stop_at_space;
        end_name();
        name_in_progress = save_name_in_progress;
        area_delimiter = save_area_delimiter;
        ext_delimiter = save_ext_delimiter;
    }
    return Result;
}


void scan_file_name(void)
{
    scan_file_name_regmem name_in_progress = true;
    begin_name();
    do {
        get_x_token();
    } while (!(cur_cmd != 10 /*spacer *//*:424 */ ));
    while (true) {

        if ((cur_cmd > 12 /*other_char */ ) || (cur_chr > 65535L /*biggest_char */ )) {
            back_input();
            goto lab30;
        }
        if (!more_name(cur_chr))
            goto lab30;
        get_x_token();
    }
 lab30:                        /*done */ end_name();
    name_in_progress = false;
}

void zpack_job_name(str_number s)
{
    pack_job_name_regmem cur_area = 65622L /*"" */ ;
    cur_ext = s;
    cur_name = job_name;
    pack_file_name(cur_name, cur_area, cur_ext);
}

void zprompt_file_name(str_number s, str_number e)
{
    prompt_file_name_regmem integer k;
    str_number saved_cur_name;
    str_number saved_cur_ext;
    str_number saved_cur_area;
    if (interaction == 2 /*scroll_mode */ ) ;
    if (s == 66142L /*"input file name" */ ) {
        if (interaction == 3 /*error_stop_mode */ ) ;
        if (file_line_error_style_p)
            print_file_line();
        else
            print_nl(65544L /*"! " */ );
        print(66143L /*"I can't find file `" */ );
    } else {

        if (interaction == 3 /*error_stop_mode */ ) ;
        if (file_line_error_style_p)
            print_file_line();
        else
            print_nl(65544L /*"! " */ );
        print(66144L /*"I can't write on file `" */ );
    }
    print_file_name(cur_name, cur_area, cur_ext);
    print(66145L /*"'." */ );
    if ((e == 66146L /*".tex" */ ) || (e == 65622L /*"" */ ))
        show_context();
    print_ln();
    print_c_string("(Press Enter to retry, or Control-D to exit");
    if ((e != 65622L /*"" */ )) {
        print(66147L /*"; default file extension is `" */ );
        print(e);
        print(39 /*"'" */ );
    }
    print(41 /*")" */ );
    print_ln();
    print_nl(66148L /*"Please type another " */ );
    print(s);
    if (interaction < 2 /*scroll_mode */ )
        fatal_error(66149L /*"*** (job aborted, file error in nonstop mode)" */ );
    saved_cur_name = cur_name;
    saved_cur_ext = cur_ext;
    saved_cur_area = cur_area;
    {
        ;
        print(65589L /*": " */ );
        term_input();
    }
    {
        begin_name();
        k = first;
        while ((buffer[k] == 32 /*" " */ ) && (k < last))
            k++;
        while (true) {

            if (k == last)
                goto lab30;
            if (!more_name(buffer[k]))
                goto lab30;
            k++;
        }
 lab30:                        /*done */ end_name();
    }
    if ((length(cur_name) == 0) && (cur_ext == 65622L /*"" */ ) && (cur_area == 65622L /*"" */ )) {
        cur_name = saved_cur_name;
        cur_ext = saved_cur_ext;
        cur_area = saved_cur_area;
    } else if (cur_ext == 65622L /*"" */ )
        cur_ext = e;
    pack_file_name(cur_name, cur_area, cur_ext);
}

void open_log_file(void)
{
    memory_word *eqtb = zeqtb;
    unsigned char /*max_selector */ old_setting;
    integer k;
    integer l;

    old_setting = selector;
    if (job_name == 0)
        job_name = get_job_name(66153L /*"texput" */ );
    pack_job_name(66155L /*".log" */ );
    while (!open_output(&log_file, "w")) {     /*554: */
        selector = SELECTOR_TERM_ONLY;
        prompt_file_name(66157L /*"transcript file name" */ , 66155L /*".log" */ );
    }
    texmf_log_name = make_name_string();
    selector = SELECTOR_LOG_ONLY;
    log_opened = true;

    input_stack[input_ptr] = cur_input;
    print_nl(66156L /*"**" */ );
    l = input_stack[0].limit_field;
    if (buffer[l] == eqtb[8938788L /*int_base 48 */ ].cint)
        l--;
    {
        register integer for_end;
        k = 1;
        for_end = l;
        if (k <= for_end)
            do
                print(buffer[k]);
            while (k++ < for_end);
    }
    print_ln();
    selector = old_setting + 2;
}

void start_input(void)
{
    start_input_regmem str_number temp_str;
    integer k;
    scan_file_name();
    pack_file_name(cur_name, cur_area, cur_ext);
    while (true) {

        begin_file_reading();
        tex_input_type = 1;
        if (u_open_in(&input_file[cur_input.index_field], kpse_tex_format, "rb",
		      eqtb[8938817L /*eTeX_state_base 6 */ ].cint, eqtb[8938818L /*eTeX_state_base 7 */ ].cint)) {
            make_utf16_name();
            name_in_progress = true;
            begin_name();
            stop_at_space = false;
            k = 0;
            while ((k < name_length16) && (more_name(name_of_file16[k])))
                k++;
            stop_at_space = true;
            end_name();
            name_in_progress = false;
            goto lab30;
        }
        end_file_reading();
        prompt_file_name(66142L /*"input file name" */ , 65622L /*"" */ );
    }
 lab30: /*done */
    cur_input.name_field = make_name_string();
    source_filename_stack[in_open] = cur_input.name_field;
    full_source_filename_stack[in_open] = make_full_name_string();
    if (cur_input.name_field == str_ptr - 1) {
        temp_str = search_string(cur_input.name_field);
        if (temp_str > 0) {
            cur_input.name_field = temp_str;
            {
                str_ptr--;
                pool_ptr = str_start[(str_ptr) - 65536L];
            }
        }
    }
    if (job_name == 0) {
        job_name = get_job_name(cur_name);
        open_log_file();
    }
    if (term_offset + length(full_source_filename_stack[in_open]) > max_print_line - 2)
        print_ln();
    else if ((term_offset > 0) || (file_offset > 0))
        print_char(32 /*" " */ );
    print_char(40 /*"(" */ );
    open_parens++;
    print(full_source_filename_stack[in_open]);
    fflush(stdout);
    cur_input.state_field = 33 /*new_line */ ;
    synctex_start_input();
    {
        line = 1;
        input_line(input_file[cur_input.index_field]);
        firm_up_the_line();
        if ((eqtb[8938788L /*int_base 48 */ ].cint < 0) || (eqtb[8938788L /*int_base 48 */ ].cint > 255))
            cur_input.limit_field--;
        else
            buffer[cur_input.limit_field] = eqtb[8938788L /*int_base 48 */ ].cint;
        first = cur_input.limit_field + 1;
        cur_input.loc_field = cur_input.start_field;
    }
}

four_quarters zeffective_char_info(internal_font_number f, quarterword c)
{
    register four_quarters Result;
    effective_char_info_regmem four_quarters ci;
    integer base_c;
    if ((!xtx_ligature_present) && (font_mapping[f] != NULL))
        c = apply_tfm_font_mapping(font_mapping[f], c);
    xtx_ligature_present = false;
    if (!mltex_enabled_p) {
        Result = font_info[char_base[f] + c].qqqq;
        return Result;
    }
    if (font_ec[f] >= c) {

        if (font_bc[f] <= c) {
            ci = font_info[char_base[f] + c].qqqq;
            if ((ci.u.B0 > 0 /*min_quarterword */ )) {
                Result = ci;
                return Result;
            }
        }
    }
    if (c >= eqtb[8938795L /*int_base 55 */ ].cint) {

        if (c <= eqtb[8938796L /*int_base 56 */ ].cint) {

            if ((eqtb[7824628L /*char_sub_code_base */  + c].hh.v.RH > 0)) {
                base_c = (eqtb[7824628L /*char_sub_code_base */  + c].hh.v.RH % 256);
                if (font_ec[f] >= base_c) {

                    if (font_bc[f] <= base_c) {
                        ci = font_info[char_base[f] + base_c].qqqq;
                        if ((ci.u.B0 > 0 /*min_quarterword */ )) {
                            Result = ci;
                            return Result;
                        }
                    }
                }
            }
        }
    }
    Result = null_character;
    return Result;
}

void zchar_warning(internal_font_number f, integer c)
{
    char_warning_regmem integer old_setting;
    if (eqtb[8938775L /*int_base 35 */ ].cint > 0) {
        old_setting = eqtb[8938769L /*int_base 29 */ ].cint;
        if ((eTeX_mode == 1) && (eqtb[8938775L /*int_base 35 */ ].cint > 1))
            eqtb[8938769L /*int_base 29 */ ].cint = 1;
        {
            begin_diagnostic();
            print_nl(66186L /*"Missing character: There is no " */ );
            if (c < 65536L)
                print(c);
            else
                print_char(c);
            print(66187L /*" in font " */ );
            print(font_name[f]);
            print_char(33 /*"!" */ );
            end_diagnostic(false);
        }
        eqtb[8938769L /*int_base 29 */ ].cint = old_setting;
    }
}

halfword znew_native_word_node(internal_font_number f, integer n)
{
    register halfword Result;
    new_native_word_node_regmem integer l;
    halfword q;
    l = 6 /*native_node_size */  + (n * sizeof(UTF16_code) + sizeof(memory_word) - 1) / sizeof(memory_word);
    q = get_node(l);
    mem[q].hh.u.B0 = 8 /*whatsit_node */ ;
    if ((eqtb[8938821L /*eTeX_state_base 10 */ ].cint > 0))
        mem[q].hh.u.B1 = 41 /*native_word_node_AT */ ;
    else
        mem[q].hh.u.B1 = 40 /*native_word_node */ ;
    mem[q + 4].qqqq.u.B0 = l;
    mem[q + 4].qqqq.u.B1 = f;
    mem[q + 4].qqqq.u.B2 = n;
    mem[q + 4].qqqq.u.B3 = 0;
    mem[q + 5].ptr = NULL;
    Result = q;
    return Result;
}

halfword znew_native_character(internal_font_number f, UnicodeScalar c)
{
    register halfword Result;
    new_native_character_regmem halfword p;
    integer i, len;
    if (font_mapping[f] != 0) {
        if (c > 65535L) {
            {
                if (pool_ptr + 2 > pool_size)
                    overflow(65539L /*"pool size" */ , pool_size - init_pool_ptr);
            }
            {
                str_pool[pool_ptr] = (c - 65536L) / 1024 + 55296L;
                pool_ptr++;
            }
            {
                str_pool[pool_ptr] = (c - 65536L) % 1024 + 56320L;
                pool_ptr++;
            }
        } else {

            {
                if (pool_ptr + 1 > pool_size)
                    overflow(65539L /*"pool size" */ , pool_size - init_pool_ptr);
            }
            {
                str_pool[pool_ptr] = c;
                pool_ptr++;
            }
        }
        len =
            apply_mapping(font_mapping[f], &(str_pool[str_start[(str_ptr) - 65536L]]),
                          (pool_ptr - str_start[(str_ptr) - 65536L]));
        pool_ptr = str_start[(str_ptr) - 65536L];
        i = 0;
        while (i < len) {

            if ((mapped_text[i] >= 55296L) && (mapped_text[i] < 56320L)) {
                c = (mapped_text[i] - 55296L) * 1024 + mapped_text[i + 1] + 9216;
                if (map_char_to_glyph(f, c) == 0) {
                    char_warning(f, c);
                }
                i = i + 2;
            } else {

                if (map_char_to_glyph(f, mapped_text[i]) == 0) {
                    char_warning(f, mapped_text[i]);
                }
                i = i + 1;
            }
        }
        p = new_native_word_node(f, len);
        {
            register integer for_end;
            i = 0;
            for_end = len - 1;
            if (i <= for_end)
                do {
                    set_native_char(p, i, mapped_text[i]);
                }
                while (i++ < for_end);
        }
    } else {

        if (eqtb[8938775L /*int_base 35 */ ].cint > 0) {

            if (map_char_to_glyph(f, c) == 0) {
                char_warning(f, c);
            }
        }
        p = get_node(7 /*native_node_size 1 */ );
        mem[p].hh.u.B0 = 8 /*whatsit_node */ ;
        mem[p].hh.u.B1 = 40 /*native_word_node */ ;
        mem[p + 4].qqqq.u.B0 = 7 /*native_node_size 1 */ ;
        mem[p + 4].qqqq.u.B3 = 0;
        mem[p + 5].ptr = NULL;
        mem[p + 4].qqqq.u.B1 = f;
        if (c > 65535L) {
            mem[p + 4].qqqq.u.B2 = 2;
            set_native_char(p, 0, (c - 65536L) / 1024 + 55296L);
            set_native_char(p, 1, (c - 65536L) % 1024 + 56320L);
        } else {

            mem[p + 4].qqqq.u.B2 = 1;
            set_native_char(p, 0, c);
        }
    }
    set_native_metrics(p, (eqtb[8938814L /*eTeX_state_base 3 */ ].cint > 0));
    Result = p;
    return Result;
}

void zfont_feature_warning(void *featureNameP, integer featLen, void *settingNameP, integer setLen)
{
    font_feature_warning_regmem;
    integer i;

    begin_diagnostic();
    print_nl(66257L /*"Unknown " */ );
    if (setLen > 0) {
        print(66258L /*"selector `" */ );
        print_utf8_str(settingNameP, setLen);
        print(66259L /*"' for " */ );
    }
    print(66260L /*"feature `" */ );
    print_utf8_str(featureNameP, featLen);
    print(66261L /*"' in font `" */ );
    i = 1;
    while (name_of_file[i] != 0) {
        print_raw_char(name_of_file[i], true);
        i++;
    }
    print(66145L /*"'." */ );
    end_diagnostic(false);
}

void zfont_mapping_warning(void *mappingNameP, integer mappingNameLen, integer warningType)
{
    font_mapping_warning_regmem;
    integer i;

    begin_diagnostic();
    if (warningType == 0)
        print_nl(66262L /*"Loaded mapping `" */ );
    else
        print_nl(66263L /*"Font mapping `" */ );
    print_utf8_str(mappingNameP, mappingNameLen);
    print(66264L /*"' for font `" */ );
    i = 1;
    while (name_of_file[i] != 0) {
        print_raw_char(name_of_file[i], true);
        i++;
    }
    switch (warningType) {
    case 1:
        print(66265L /*"' not found." */ );
        break;
    case 2:
        {
            print(66266L /*"' not usable;" */ );
            print_nl(66267L /*"bad mapping file or incorrect mapping type." */ );
        }
        break;
    default:
        print(66145L /*"'." */ );
        break;
    }
    end_diagnostic(false);
}

void graphite_warning(void)
{
    graphite_warning_regmem;
    integer i;

    begin_diagnostic();
    print_nl(66268L /*"Font `" */ );
    i = 1;
    while (name_of_file[i] != 0) {
        print_raw_char(name_of_file[i], true);
        i++;
    }
    print(66269L /*"' does not support Graphite. Trying OpenType layout instead." */ );
    end_diagnostic(false);
}

internal_font_number zload_native_font(halfword u, str_number nom, str_number aire, scaled s)
{
    /*done */
#define first_math_fontdimen ( 10 )
    register internal_font_number Result;
    load_native_font_regmem integer k, num_font_dimens;
    void *font_engine;
    scaled actual_size;
    halfword p;
    scaled ascent, descent, font_slant, x_ht, cap_ht;
    internal_font_number f;
    str_number full_name;
    Result = 0 /*font_base */ ;
    font_engine = find_native_font(name_of_file + 1, s);
    if (font_engine == 0)
        goto lab30;
    if (s >= 0)
        actual_size = s;
    else {

        if ((s != -1000))
            actual_size = xn_over_d(loaded_font_design_size, -(integer) s, 1000);
        else
            actual_size = loaded_font_design_size;
    }
    {
        if (pool_ptr + name_length > pool_size)
            overflow(65539L /*"pool size" */ , pool_size - init_pool_ptr);
    }
    {
        register integer for_end;
        k = 1;
        for_end = name_length;
        if (k <= for_end)
            do {
                str_pool[pool_ptr] = name_of_file[k];
                pool_ptr++;
            }
            while (k++ < for_end);
    }
    full_name = make_string();
    {
        register integer for_end;
        f = 1 /*font_base 1 */ ;
        for_end = font_ptr;
        if (f <= for_end)
            do
                if ((font_area[f] == native_font_type_flag) && str_eq_str(font_name[f], full_name)
                    && (font_size[f] == actual_size)) {
                    release_font_engine(font_engine, native_font_type_flag);
                    {
                        str_ptr--;
                        pool_ptr = str_start[(str_ptr) - 65536L];
                    }
                    Result = f;
                    goto lab30;
                }
            while (f++ < for_end) ;
    }
    if ((native_font_type_flag == 65534L /*otgr_font_flag */ ) && isOpenTypeMathFont(font_engine))
        num_font_dimens = first_math_fontdimen + 55;
    else
        num_font_dimens = 8;
    if ((font_ptr == font_max) || (fmem_ptr + num_font_dimens > font_mem_size)) {
        {
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66164L /*"Font " */ );
            }
            sprint_cs(u);
            print_char(61 /*"=" */ );
            if (file_name_quote_char != 0)
                print_char(file_name_quote_char);
            print_file_name(nom, aire, cur_ext);
            if (file_name_quote_char != 0)
                print_char(file_name_quote_char);
            if (s >= 0) {
                print(66097L /*" at " */ );
                print_scaled(s);
                print(65693L /*"pt" */ );
            } else if (s != -1000) {
                print(66161L /*" scaled " */ );
                print_int(-(integer) s);
            }
            print(66173L /*" not loaded: Not enough room left" */ );
            {
                help_ptr = 4;
                help_line[3] = 66174L /*"I'm afraid I won't be able to make use of this font," */ ;
                help_line[2] = 66175L /*"because my memory for character-size data is too small." */ ;
                help_line[1] = 66176L /*"If you're really stuck, ask a wizard to enlarge me." */ ;
                help_line[0] = 66177L /*"Or maybe try `I\font<same font id>=<name of loaded font>'." */ ;
            }
            error();
            goto lab30;
        }
    }
    font_ptr++;
    font_area[font_ptr] = native_font_type_flag;
    font_name[font_ptr] = full_name;
    font_check[font_ptr].u.B0 = 0;
    font_check[font_ptr].u.B1 = 0;
    font_check[font_ptr].u.B2 = 0;
    font_check[font_ptr].u.B3 = 0;
    font_glue[font_ptr] = -268435455L;
    font_dsize[font_ptr] = loaded_font_design_size;
    font_size[font_ptr] = actual_size;
    if ((native_font_type_flag == 65535L /*aat_font_flag */ )) {
        aat_get_font_metrics(font_engine, &ascent, &descent, &x_ht, &cap_ht,
                             &font_slant);
    } else {

        ot_get_font_metrics(font_engine, &ascent, &descent, &x_ht, &cap_ht,
                            &font_slant);
    }
    height_base[font_ptr] = ascent;
    depth_base[font_ptr] = -(integer) descent;
    font_params[font_ptr] = num_font_dimens;
    font_bc[font_ptr] = 0;
    font_ec[font_ptr] = 65535L;
    font_used[font_ptr] = false;
    hyphen_char[font_ptr] = eqtb[8938786L /*int_base 46 */ ].cint;
    skew_char[font_ptr] = eqtb[8938787L /*int_base 47 */ ].cint;
    param_base[font_ptr] = fmem_ptr - 1;
    font_layout_engine[font_ptr] = font_engine;
    font_mapping[font_ptr] = 0;
    font_letter_space[font_ptr] = loaded_font_letter_space;
    p = new_native_character(font_ptr, 32 /*" " */ );
    s = mem[p + 1].cint + loaded_font_letter_space;
    free_node(p, mem[p + 4].qqqq.u.B0);
    font_info[fmem_ptr].cint = font_slant;
    fmem_ptr++;
    font_info[fmem_ptr].cint = s;
    fmem_ptr++;
    font_info[fmem_ptr].cint = s / 2;
    fmem_ptr++;
    font_info[fmem_ptr].cint = s / 3;
    fmem_ptr++;
    font_info[fmem_ptr].cint = x_ht;
    fmem_ptr++;
    font_info[fmem_ptr].cint = font_size[font_ptr];
    fmem_ptr++;
    font_info[fmem_ptr].cint = s / 3;
    fmem_ptr++;
    font_info[fmem_ptr].cint = cap_ht;
    fmem_ptr++;
    if (num_font_dimens == first_math_fontdimen + 55) {
        font_info[fmem_ptr].cint = num_font_dimens;
        fmem_ptr++;
        {
            register integer for_end;
            k = 0;
            for_end = 55 /*lastMathConstant */ ;
            if (k <= for_end)
                do {
                    font_info[fmem_ptr].cint = get_ot_math_constant(font_ptr, k);
                    fmem_ptr++;
                }
                while (k++ < for_end);
        }
    }
    font_mapping[font_ptr] = loaded_font_mapping;
    font_flags[font_ptr] = loaded_font_flags;
    Result = font_ptr;
 lab30:                        /*done */ ;
    return Result;
}

void zdo_locale_linebreaks(integer s, integer len)
{
    do_locale_linebreaks_regmem integer offs, prevOffs, i;
    boolean use_penalty, use_skip;
    if ((eqtb[8938808L /*int_base 68 */ ].cint == 0) || (len == 1)) {
        mem[cur_list.tail_field].hh.v.RH = new_native_word_node(main_f, len);
        cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
        {
            register integer for_end;
            i = 0;
            for_end = len - 1;
            if (i <= for_end)
                do
                    set_native_char(cur_list.tail_field, i, native_text[s + i]);
                while (i++ < for_end);
        }
        set_native_metrics(cur_list.tail_field, (eqtb[8938814L /*eTeX_state_base 3 */ ].cint > 0));
    } else {

        use_skip = eqtb[2252255L /*glue_base 15 */ ].hh.v.RH != mem_bot;
        use_penalty = eqtb[8938809L /*int_base 69 */ ].cint != 0 || !use_skip;
        linebreak_start(main_f, eqtb[8938808L /*int_base 68 */ ].cint, native_text + s, len);
        offs = 0;
        do {
            prevOffs = offs;
            offs = linebreak_next();
            if (offs > 0) {
                if (prevOffs != 0) {
                    if (use_penalty) {
                        mem[cur_list.tail_field].hh.v.RH = new_penalty(eqtb[8938809L /*int_base 69 */ ].cint);
                        cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
                    }
                    if (use_skip) {
                        mem[cur_list.tail_field].hh.v.RH = new_param_glue(15 /*XeTeX_linebreak_skip_code */ );
                        cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
                    }
                }
                mem[cur_list.tail_field].hh.v.RH = new_native_word_node(main_f, offs - prevOffs);
                cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
                {
                    register integer for_end;
                    i = prevOffs;
                    for_end = offs - 1;
                    if (i <= for_end)
                        do
                            set_native_char(cur_list.tail_field, i - prevOffs, native_text[s + i]);
                        while (i++ < for_end);
                }
                set_native_metrics(cur_list.tail_field, (eqtb[8938814L /*eTeX_state_base 3 */ ].cint > 0));
            }
        } while (!(offs < 0));
    }
}

void bad_utf8_warning(void)
{
    bad_utf8_warning_regmem begin_diagnostic();
    print_nl(66270L /*"Invalid UTF-8 byte or sequence" */ );
    if ((cur_input.name_field == 0))
        print(66271L /*" in terminal input" */ );
    else {

        print(66272L /*" at line " */ );
        print_int(line);
    }
    print(66273L /*" replaced by U+FFFD." */ );
    end_diagnostic(false);
}

integer get_input_normalization_state(void)
{
    register integer Result;
    get_input_normalization_state_regmem if (eqtb == NULL)
        Result = 0;
    else
        Result = eqtb[8938816L /*eTeX_state_base 5 */ ].cint;
    return Result;
}

integer get_tracing_fonts_state(void)
{
    register integer Result;
    get_tracing_fonts_state_regmem Result = eqtb[8938819L /*eTeX_state_base 8 */ ].cint;
    return Result;
}

internal_font_number zread_font_info(halfword u, str_number nom, str_number aire, scaled s)
{
    register internal_font_number Result;
    read_font_info_regmem font_index k;
    boolean name_too_long;
    boolean file_opened;
    halfword lf, lh, bc, ec, nw, nh, nd, ni, nl, nk, ne, np;
    internal_font_number f;
    internal_font_number g;
    eight_bits a, b, c, d;
    four_quarters qw;
    scaled sw;
    integer bch_label;
    short bchar;
    scaled z;
    integer alpha;
    unsigned char beta;
    g = 0 /*font_base */ ;
    file_opened = false;
    pack_file_name(nom, aire, cur_ext);
    if (eqtb[8938819L /*eTeX_state_base 8 */ ].cint > 0) {
        begin_diagnostic();
        print_nl(66160L /*"Requested font "" */ );
        print_c_string((string) (name_of_file + 1));
        print('"');
        if (s < 0) {
            print(66161L /*" scaled " */ );
            print_int(-(integer) s);
        } else {

            print(66097L /*" at " */ );
            print_scaled(s);
            print(65693L /*"pt" */ );
        }
        end_diagnostic(false);
    }
    if (quoted_filename) {
        g = load_native_font(u, nom, aire, s);
        if (g != 0 /*font_base */ )
            goto lab30;
    }
    name_too_long = (length(nom) > 255) || (length(aire) > 255);
    if (name_too_long)
        goto lab11;
    pack_file_name(nom, aire, 65622L /*"" */ );
    check_for_tfm_font_mapping();
    if (open_input(&tfm_file, kpse_tfm_format, "rb")) {
        file_opened = true /*:582 */ ;
        {
            {
                lf = tfm_temp;
                if (lf > 127)
                    goto lab11;
                tfm_temp = getc(tfm_file);
                lf = lf * 256 + tfm_temp;
            }
            tfm_temp = getc(tfm_file);
            {
                lh = tfm_temp;
                if (lh > 127)
                    goto lab11;
                tfm_temp = getc(tfm_file);
                lh = lh * 256 + tfm_temp;
            }
            tfm_temp = getc(tfm_file);
            {
                bc = tfm_temp;
                if (bc > 127)
                    goto lab11;
                tfm_temp = getc(tfm_file);
                bc = bc * 256 + tfm_temp;
            }
            tfm_temp = getc(tfm_file);
            {
                ec = tfm_temp;
                if (ec > 127)
                    goto lab11;
                tfm_temp = getc(tfm_file);
                ec = ec * 256 + tfm_temp;
            }
            if ((bc > ec + 1) || (ec > 255))
                goto lab11;
            if (bc > 255) {
                bc = 1;
                ec = 0;
            }
            tfm_temp = getc(tfm_file);
            {
                nw = tfm_temp;
                if (nw > 127)
                    goto lab11;
                tfm_temp = getc(tfm_file);
                nw = nw * 256 + tfm_temp;
            }
            tfm_temp = getc(tfm_file);
            {
                nh = tfm_temp;
                if (nh > 127)
                    goto lab11;
                tfm_temp = getc(tfm_file);
                nh = nh * 256 + tfm_temp;
            }
            tfm_temp = getc(tfm_file);
            {
                nd = tfm_temp;
                if (nd > 127)
                    goto lab11;
                tfm_temp = getc(tfm_file);
                nd = nd * 256 + tfm_temp;
            }
            tfm_temp = getc(tfm_file);
            {
                ni = tfm_temp;
                if (ni > 127)
                    goto lab11;
                tfm_temp = getc(tfm_file);
                ni = ni * 256 + tfm_temp;
            }
            tfm_temp = getc(tfm_file);
            {
                nl = tfm_temp;
                if (nl > 127)
                    goto lab11;
                tfm_temp = getc(tfm_file);
                nl = nl * 256 + tfm_temp;
            }
            tfm_temp = getc(tfm_file);
            {
                nk = tfm_temp;
                if (nk > 127)
                    goto lab11;
                tfm_temp = getc(tfm_file);
                nk = nk * 256 + tfm_temp;
            }
            tfm_temp = getc(tfm_file);
            {
                ne = tfm_temp;
                if (ne > 127)
                    goto lab11;
                tfm_temp = getc(tfm_file);
                ne = ne * 256 + tfm_temp;
            }
            tfm_temp = getc(tfm_file);
            {
                np = tfm_temp;
                if (np > 127)
                    goto lab11;
                tfm_temp = getc(tfm_file);
                np = np * 256 + tfm_temp;
            }
            if (lf != 6 + lh + (ec - bc + 1) + nw + nh + nd + ni + nl + nk + ne + np)
                goto lab11;
            if ((nw == 0) || (nh == 0) || (nd == 0) || (ni == 0))
                goto lab11;
        }
        lf = lf - 6 - lh;
        if (np < 7)
            lf = lf + 7 - np;
        if ((font_ptr == font_max) || (fmem_ptr + lf > font_mem_size)) {        /*586: */
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66164L /*"Font " */ );
            }
            sprint_cs(u);
            print_char(61 /*"=" */ );
            if (file_name_quote_char != 0)
                print_char(file_name_quote_char);
            print_file_name(nom, aire, cur_ext);
            if (file_name_quote_char != 0)
                print_char(file_name_quote_char);
            if (s >= 0) {
                print(66097L /*" at " */ );
                print_scaled(s);
                print(65693L /*"pt" */ );
            } else if (s != -1000) {
                print(66161L /*" scaled " */ );
                print_int(-(integer) s);
            }
            print(66173L /*" not loaded: Not enough room left" */ );
            {
                help_ptr = 4;
                help_line[3] = 66174L /*"I'm afraid I won't be able to make use of this font," */ ;
                help_line[2] = 66175L /*"because my memory for character-size data is too small." */ ;
                help_line[1] = 66176L /*"If you're really stuck, ask a wizard to enlarge me." */ ;
                help_line[0] = 66177L /*"Or maybe try `I\font<same font id>=<name of loaded font>'." */ ;
            }
            error();
            goto lab30;
        }
        f = font_ptr + 1;
        char_base[f] = fmem_ptr - bc;
        width_base[f] = char_base[f] + ec + 1;
        height_base[f] = width_base[f] + nw;
        depth_base[f] = height_base[f] + nh;
        italic_base[f] = depth_base[f] + nd;
        lig_kern_base[f] = italic_base[f] + ni;
        kern_base[f] = lig_kern_base[f] + nl - 256 * (128);
        exten_base[f] = kern_base[f] + 256 * (128) + nk;
        param_base[f] = exten_base[f] + /*:585 */ ne;
        {
            if (lh < 2)
                goto lab11;
            {
                tfm_temp = getc(tfm_file);
                a = tfm_temp;
                qw.u.B0 = a;
                tfm_temp = getc(tfm_file);
                b = tfm_temp;
                qw.u.B1 = b;
                tfm_temp = getc(tfm_file);
                c = tfm_temp;
                qw.u.B2 = c;
                tfm_temp = getc(tfm_file);
                d = tfm_temp;
                qw.u.B3 = d;
                font_check[f] = qw;
            }
            tfm_temp = getc(tfm_file);
            {
                z = tfm_temp;
                if (z > 127)
                    goto lab11;
                tfm_temp = getc(tfm_file);
                z = z * 256 + tfm_temp;
            }
            tfm_temp = getc(tfm_file);
            z = z * 256 + tfm_temp;
            tfm_temp = getc(tfm_file);
            z = (z * 16) + (tfm_temp / 16);
            if (z < 65536L)
                goto lab11;
            while (lh > 2) {

                tfm_temp = getc(tfm_file);
                tfm_temp = getc(tfm_file);
                tfm_temp = getc(tfm_file);
                tfm_temp = getc(tfm_file);
                lh--;
            }
            font_dsize[f] = z;
            if (s != -1000) {

                if (s >= 0)
                    z = s;
                else
                    z = xn_over_d(z, -(integer) s, 1000);
            }
            font_size[f] = z;
        }
        {
            register integer for_end;
            k = fmem_ptr;
            for_end = width_base[f] - 1;
            if (k <= for_end)
                do {
                    {
                        tfm_temp = getc(tfm_file);
                        a = tfm_temp;
                        qw.u.B0 = a;
                        tfm_temp = getc(tfm_file);
                        b = tfm_temp;
                        qw.u.B1 = b;
                        tfm_temp = getc(tfm_file);
                        c = tfm_temp;
                        qw.u.B2 = c;
                        tfm_temp = getc(tfm_file);
                        d = tfm_temp;
                        qw.u.B3 = d;
                        font_info[k].qqqq = qw;
                    }
                    if ((a >= nw) || (b / 16 >= nh) || (b % 16 >= nd) || (c / 4 >= ni))
                        goto lab11;
                    switch (c % 4) {
                    case 1:
                        if (d >= nl)
                            goto lab11;
                        break;
                    case 3:
                        if (d >= ne)
                            goto lab11;
                        break;
                    case 2:
                        {
                            {
                                if ((d < bc) || (d > ec))
                                    goto lab11;
                            }
                            while (d < k + bc - fmem_ptr) {

                                qw = font_info[char_base[f] + d].qqqq;
                                if (((qw.u.B2) % 4) != 2 /*list_tag */ )
                                    goto lab45;
                                d = qw.u.B3;
                            }
                            if (d == k + bc - fmem_ptr)
                                goto lab11;
 lab45:                        /*not_found */ ;
                        }
                        break;
                    default:
                        ;
                        break;
                    }
                }
                while (k++ < for_end);
        }
        {
            {
                alpha = 16;
                while (z >= 8388608L) {

                    z = z / 2;
                    alpha = alpha + alpha;
                }
                beta = 256 / alpha;
                alpha = alpha * z;
            }
            {
                register integer for_end;
                k = width_base[f];
                for_end = lig_kern_base[f] - 1;
                if (k <= for_end)
                    do {
                        tfm_temp = getc(tfm_file);
                        a = tfm_temp;
                        tfm_temp = getc(tfm_file);
                        b = tfm_temp;
                        tfm_temp = getc(tfm_file);
                        c = tfm_temp;
                        tfm_temp = getc(tfm_file);
                        d = tfm_temp;
                        sw = (((((d * z) / 256) + (c * z)) / 256) + (b * z)) / beta;
                        if (a == 0)
                            font_info[k].cint = sw;
                        else if (a == 255)
                            font_info[k].cint = sw - alpha;
                        else
                            goto lab11;
                    }
                    while (k++ < for_end);
            }
            if (font_info[width_base[f]].cint != 0)
                goto lab11;
            if (font_info[height_base[f]].cint != 0)
                goto lab11;
            if (font_info[depth_base[f]].cint != 0)
                goto lab11;
            if (font_info[italic_base[f]].cint != 0)
                goto lab11;
        }
        bch_label = 32767;
        bchar = 256;
        if (nl > 0) {
            {
                register integer for_end;
                k = lig_kern_base[f];
                for_end = kern_base[f] + 256 * (128) - 1;
                if (k <= for_end)
                    do {
                        {
                            tfm_temp = getc(tfm_file);
                            a = tfm_temp;
                            qw.u.B0 = a;
                            tfm_temp = getc(tfm_file);
                            b = tfm_temp;
                            qw.u.B1 = b;
                            tfm_temp = getc(tfm_file);
                            c = tfm_temp;
                            qw.u.B2 = c;
                            tfm_temp = getc(tfm_file);
                            d = tfm_temp;
                            qw.u.B3 = d;
                            font_info[k].qqqq = qw;
                        }
                        if (a > 128) {
                            if (256 * c + d >= nl)
                                goto lab11;
                            if (a == 255) {

                                if (k == lig_kern_base[f])
                                    bchar = b;
                            }
                        } else {

                            if (b != bchar) {
                                {
                                    if ((b < bc) || (b > ec))
                                        goto lab11;
                                }
                                qw = font_info[char_base[f] + b].qqqq;
                                if (!(qw.u.B0 > 0 /*min_quarterword */ ))
                                    goto lab11;
                            }
                            if (c < 128) {
                                {
                                    if ((d < bc) || (d > ec))
                                        goto lab11;
                                }
                                qw = font_info[char_base[f] + d].qqqq;
                                if (!(qw.u.B0 > 0 /*min_quarterword */ ))
                                    goto lab11;
                            } else if (256 * (c - 128) + d >= nk)
                                goto lab11;
                            if (a < 128) {

                                if (k - lig_kern_base[f] + a + 1 >= nl)
                                    goto lab11;
                            }
                        }
                    }
                    while (k++ < for_end);
            }
            if (a == 255)
                bch_label = 256 * c + d;
        }
        {
            register integer for_end;
            k = kern_base[f] + 256 * (128);
            for_end = exten_base[f] - 1;
            if (k <= for_end)
                do {
                    tfm_temp = getc(tfm_file);
                    a = tfm_temp;
                    tfm_temp = getc(tfm_file);
                    b = tfm_temp;
                    tfm_temp = getc(tfm_file);
                    c = tfm_temp;
                    tfm_temp = getc(tfm_file);
                    d = tfm_temp;
                    sw = (((((d * z) / 256) + (c * z)) / 256) + (b * z)) / beta;
                    if (a == 0)
                        font_info[k].cint = sw;
                    else if (a == 255)
                        font_info[k].cint = sw - alpha;
                    else
                        goto lab11;
                }
                while (k++ < for_end);
        }
        {
            register integer for_end;
            k = exten_base[f];
            for_end = param_base[f] - 1;
            if (k <= for_end)
                do {
                    {
                        tfm_temp = getc(tfm_file);
                        a = tfm_temp;
                        qw.u.B0 = a;
                        tfm_temp = getc(tfm_file);
                        b = tfm_temp;
                        qw.u.B1 = b;
                        tfm_temp = getc(tfm_file);
                        c = tfm_temp;
                        qw.u.B2 = c;
                        tfm_temp = getc(tfm_file);
                        d = tfm_temp;
                        qw.u.B3 = d;
                        font_info[k].qqqq = qw;
                    }
                    if (a != 0) {
                        {
                            if ((a < bc) || (a > ec))
                                goto lab11;
                        }
                        qw = font_info[char_base[f] + a].qqqq;
                        if (!(qw.u.B0 > 0 /*min_quarterword */ ))
                            goto lab11;
                    }
                    if (b != 0) {
                        {
                            if ((b < bc) || (b > ec))
                                goto lab11;
                        }
                        qw = font_info[char_base[f] + b].qqqq;
                        if (!(qw.u.B0 > 0 /*min_quarterword */ ))
                            goto lab11;
                    }
                    if (c != 0) {
                        {
                            if ((c < bc) || (c > ec))
                                goto lab11;
                        }
                        qw = font_info[char_base[f] + c].qqqq;
                        if (!(qw.u.B0 > 0 /*min_quarterword */ ))
                            goto lab11;
                    }
                    {
                        {
                            if ((d < bc) || (d > ec))
                                goto lab11;
                        }
                        qw = font_info[char_base[f] + d].qqqq;
                        if (!(qw.u.B0 > 0 /*min_quarterword */ ))
                            goto lab11;
                    }
                }
                while (k++ < for_end);
        }
        {
            {
                register integer for_end;
                k = 1;
                for_end = np;
                if (k <= for_end)
                    do
                        if (k == 1) {
                            tfm_temp = getc(tfm_file);
                            sw = tfm_temp;
                            if (sw > 127)
                                sw = sw - 256;
                            tfm_temp = getc(tfm_file);
                            sw = sw * 256 + tfm_temp;
                            tfm_temp = getc(tfm_file);
                            sw = sw * 256 + tfm_temp;
                            tfm_temp = getc(tfm_file);
                            font_info[param_base[f]].cint = (sw * 16) + (tfm_temp / 16);
                        } else {

                            tfm_temp = getc(tfm_file);
                            a = tfm_temp;
                            tfm_temp = getc(tfm_file);
                            b = tfm_temp;
                            tfm_temp = getc(tfm_file);
                            c = tfm_temp;
                            tfm_temp = getc(tfm_file);
                            d = tfm_temp;
                            sw = (((((d * z) / 256) + (c * z)) / 256) + (b * z)) / beta;
                            if (a == 0)
                                font_info[param_base[f] + k - 1].cint = sw;
                            else if (a == 255)
                                font_info[param_base[f] + k - 1].cint = sw - alpha;
                            else
                                goto lab11;
                        }
                    while (k++ < for_end);
            }
            if (feof(tfm_file))
                goto lab11;
            {
                register integer for_end;
                k = np + 1;
                for_end = 7;
                if (k <= for_end)
                    do
                        font_info[param_base[f] + k - 1].cint = 0;
                    while (k++ < for_end);
            }
        }
        if (np >= 7)
            font_params[f] = np;
        else
            font_params[f] = 7;
        hyphen_char[f] = eqtb[8938786L /*int_base 46 */ ].cint;
        skew_char[f] = eqtb[8938787L /*int_base 47 */ ].cint;
        if (bch_label < nl)
            bchar_label[f] = bch_label + lig_kern_base[f];
        else
            bchar_label[f] = 0 /*non_address */ ;
        font_bchar[f] = bchar;
        font_false_bchar[f] = bchar;
        if (bchar <= ec) {

            if (bchar >= bc) {
                qw = font_info[char_base[f] + bchar].qqqq;
                if ((qw.u.B0 > 0 /*min_quarterword */ ))
                    font_false_bchar[f] = 65536L /*too_big_char */ ;
            }
        }
        font_name[f] = nom;
        font_area[f] = aire;
        font_bc[f] = bc;
        font_ec[f] = ec;
        font_glue[f] = -268435455L;
        char_base[f] = char_base[f];
        width_base[f] = width_base[f];
        lig_kern_base[f] = lig_kern_base[f];
        kern_base[f] = kern_base[f];
        exten_base[f] = exten_base[f];
        param_base[f]--;
        fmem_ptr = fmem_ptr + lf;
        font_ptr = f;
        g = f;
        font_mapping[f] = load_tfm_font_mapping();
        goto lab30;
    }
    if (g != 0 /*font_base */ )
        goto lab30;
    if (!quoted_filename) {
        g = load_native_font(u, nom, aire, s);
        if (g != 0 /*font_base */ )
            goto lab30;
    }
 lab11:/*bad_tfm */ if (eqtb[8938807L /*int_base 67 */ ].cint == 0) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66164L /*"Font " */ );
        }
        sprint_cs(u);
        print_char(61 /*"=" */ );
        if (file_name_quote_char != 0)
            print_char(file_name_quote_char);
        print_file_name(nom, aire, cur_ext);
        if (file_name_quote_char != 0)
            print_char(file_name_quote_char);
        if (s >= 0) {
            print(66097L /*" at " */ );
            print_scaled(s);
            print(65693L /*"pt" */ );
        } else if (s != -1000) {
            print(66161L /*" scaled " */ );
            print_int(-(integer) s);
        }
        if (file_opened)
            print(66165L /*" not loadable: Bad metric (TFM) file" */ );
        else if (name_too_long)
            print(66166L /*" not loadable: Metric (TFM) file name too long" */ );
        else
            print(66167L /*" not loadable: Metric (TFM) file or installed font not found" */ );
        {
            help_ptr = 5;
            help_line[4] = 66168L /*"I wasn't able to read the size data for this font," */ ;
            help_line[3] = 66169L /*"so I will ignore the font specification." */ ;
            help_line[2] = 66170L /*"[Wizards can fix TFM files using TFtoPL/PLtoTF.]" */ ;
            help_line[1] = 66171L /*"You might try inserting a different font spec;" */ ;
            help_line[0] = 66172L /*"e.g., type `I\font<same font id>=<substitute font name>'." */ ;
        }
        error();
    }
lab30: /* done */
    if (file_opened)
        close_file(tfm_file);
    if (eqtb[8938819L /*eTeX_state_base 8 */ ].cint > 0) {
        if (g == 0 /*font_base */ ) {
            begin_diagnostic();
            print_nl(66162L /*" -> font not found, using "nullfont"" */ );
            end_diagnostic(false);
        } else if (file_opened) {
            begin_diagnostic();
            print_nl(66163L /*" -> " */ );
            print_c_string((string) (name_of_file + 1));
            end_diagnostic(false);
        }
    }
    Result = g;
    return Result;
}

halfword znew_character(internal_font_number f, UTF16_code c)
{
    register halfword Result;
    new_character_regmem halfword p;
    quarterword ec;
    if (((font_area[f] == 65535L /*aat_font_flag */ ) || (font_area[f] == 65534L /*otgr_font_flag */ ))) {
        Result = new_native_character(f, c);
        return Result;
    }
    ec = effective_char(false, f, c);
    if (font_bc[f] <= ec) {

        if (font_ec[f] >= ec) {

            if ((font_info[char_base[f] + ec].qqqq.u.B0 > 0 /*min_quarterword */ )) {
                p = get_avail();
                mem[p].hh.u.B0 = f;
                mem[p].hh.u.B1 = c;
                Result = p;
                return Result;
            }
        }
    }
    char_warning(f, c);
    Result = -268435455L;
    return Result;
}

void dvi_swap(void)
{
    dvi_swap_regmem if (dvi_ptr > (2147483647L - dvi_offset)) {
        cur_s = -2;
        fatal_error(66188L /*"dvi length exceeds "7FFFFFFF" */ );
    }
    if (dvi_limit == dvi_buf_size) {
        WRITE_OUT(0, half_buf - 1);
        dvi_limit = half_buf;
        dvi_offset = dvi_offset + dvi_buf_size;
        dvi_ptr = 0;
    } else {

        WRITE_OUT(half_buf, dvi_buf_size - 1);
        dvi_limit = dvi_buf_size;
    }
    dvi_gone = dvi_gone + half_buf;
}

void zdvi_four(integer x)
{
    dvi_four_regmem if (x >= 0) {
        dvi_buf[dvi_ptr] = x / 16777216L;
        dvi_ptr++;
        if (dvi_ptr == dvi_limit)
            dvi_swap();
    } else {

        x = x + 1073741824L;
        x = x + 1073741824L;
        {
            dvi_buf[dvi_ptr] = (x / 16777216L) + 128;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
    }
    x = x % 16777216L;
    {
        dvi_buf[dvi_ptr] = x / 65536L;
        dvi_ptr++;
        if (dvi_ptr == dvi_limit)
            dvi_swap();
    }
    x = x % 65536L;
    {
        dvi_buf[dvi_ptr] = x / 256;
        dvi_ptr++;
        if (dvi_ptr == dvi_limit)
            dvi_swap();
    }
    {
        dvi_buf[dvi_ptr] = x % 256;
        dvi_ptr++;
        if (dvi_ptr == dvi_limit)
            dvi_swap();
    }
}

void zdvi_two(UTF16_code s)
{
    dvi_two_regmem {
        dvi_buf[dvi_ptr] = s / 256;
        dvi_ptr++;
        if (dvi_ptr == dvi_limit)
            dvi_swap();
    }
    {
        dvi_buf[dvi_ptr] = s % 256;
        dvi_ptr++;
        if (dvi_ptr == dvi_limit)
            dvi_swap();
    }
}

void zdvi_pop(integer l)
{
    dvi_pop_regmem if ((l == dvi_offset + dvi_ptr) && (dvi_ptr > 0))
        dvi_ptr--;
    else {

        dvi_buf[dvi_ptr] = 142 /*pop */ ;
        dvi_ptr++;
        if (dvi_ptr == dvi_limit)
            dvi_swap();
    }
}

void zdvi_native_font_def(internal_font_number f)
{
    dvi_native_font_def_regmem integer font_def_length, i;
    {
        dvi_buf[dvi_ptr] = 252 /*define_native_font */ ;
        dvi_ptr++;
        if (dvi_ptr == dvi_limit)
            dvi_swap();
    }
    dvi_four(f - 1);
    font_def_length = make_font_def(f);
    {
        register integer for_end;
        i = 0;
        for_end = font_def_length - 1;
        if (i <= for_end)
            do {
                dvi_buf[dvi_ptr] = xdv_buffer[i];
                dvi_ptr++;
                if (dvi_ptr == dvi_limit)
                    dvi_swap();
            }
            while (i++ < for_end);
    }
}

void zdvi_font_def(internal_font_number f)
{
    dvi_font_def_regmem pool_pointer k;
    integer l;
    if (((font_area[f] == 65535L /*aat_font_flag */ ) || (font_area[f] == 65534L /*otgr_font_flag */ )))
        dvi_native_font_def(f);
    else {

        if (f <= 256) {
            {
                dvi_buf[dvi_ptr] = 243 /*fnt_def1 */ ;
                dvi_ptr++;
                if (dvi_ptr == dvi_limit)
                    dvi_swap();
            }
            {
                dvi_buf[dvi_ptr] = f - 1;
                dvi_ptr++;
                if (dvi_ptr == dvi_limit)
                    dvi_swap();
            }
        } else {

            {
                dvi_buf[dvi_ptr] = 244 /*fnt_def1 1 */ ;
                dvi_ptr++;
                if (dvi_ptr == dvi_limit)
                    dvi_swap();
            }
            {
                dvi_buf[dvi_ptr] = (f - 1) / 256;
                dvi_ptr++;
                if (dvi_ptr == dvi_limit)
                    dvi_swap();
            }
            {
                dvi_buf[dvi_ptr] = (f - 1) % 256;
                dvi_ptr++;
                if (dvi_ptr == dvi_limit)
                    dvi_swap();
            }
        }
        {
            dvi_buf[dvi_ptr] = font_check[f].u.B0;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        {
            dvi_buf[dvi_ptr] = font_check[f].u.B1;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        {
            dvi_buf[dvi_ptr] = font_check[f].u.B2;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        {
            dvi_buf[dvi_ptr] = font_check[f].u.B3;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        dvi_four(font_size[f]);
        dvi_four(font_dsize[f]);
        {
            dvi_buf[dvi_ptr] = length(font_area[f]);
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        l = 0;
        k = str_start[(font_name[f]) - 65536L];
        while ((l == 0) && (k < str_start[(font_name[f] + 1) - 65536L])) {

            if (str_pool[k] == 58 /*":" */ )
                l = k - str_start[(font_name[f]) - 65536L];
            k++;
        }
        if (l == 0)
            l = length(font_name[f]);
        {
            dvi_buf[dvi_ptr] = l;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        {
            register integer for_end;
            k = str_start[(font_area[f]) - 65536L];
            for_end = str_start[(font_area[f] + 1) - 65536L] - 1;
            if (k <= for_end)
                do {
                    dvi_buf[dvi_ptr] = str_pool[k];
                    dvi_ptr++;
                    if (dvi_ptr == dvi_limit)
                        dvi_swap();
                }
                while (k++ < for_end);
        }
        {
            register integer for_end;
            k = str_start[(font_name[f]) - 65536L];
            for_end = str_start[(font_name[f]) - 65536L] + l - 1;
            if (k <= for_end)
                do {
                    dvi_buf[dvi_ptr] = str_pool[k];
                    dvi_ptr++;
                    if (dvi_ptr == dvi_limit)
                        dvi_swap();
                }
                while (k++ < for_end);
        }
    }
}

void zmovement(scaled w, eight_bits o)
{
    movement_regmem small_number mstate;
    halfword p, q;
    integer k;
    q = get_node(3 /*movement_node_size */ );
    mem[q + 1].cint = w;
    mem[q + 2].cint = dvi_offset + dvi_ptr;
    if (o == 157 /*down1 */ ) {
        mem[q].hh.v.RH = down_ptr;
        down_ptr = q;
    } else {

        mem[q].hh.v.RH = right_ptr;
        right_ptr = q;
    }
    p = mem[q].hh.v.RH;
    mstate = 0 /*none_seen */ ;
    while (p != -268435455L) {

        if (mem[p + 1].cint == w)       /*632: */
            switch (mstate + mem[p].hh.v.LH) {
            case 3:
            case 4:
            case 15:
            case 16:
                if (mem[p + 2].cint < dvi_gone)
                    goto lab45;
                else {          /*633: */

                    k = mem[p + 2].cint - dvi_offset;
                    if (k < 0)
                        k = k + dvi_buf_size;
                    dvi_buf[k] = dvi_buf[k] + 5;
                    mem[p].hh.v.LH = 1 /*y_here */ ;
                    goto lab40;
                }
                break;
            case 5:
            case 9:
            case 11:
                if (mem[p + 2].cint < dvi_gone)
                    goto lab45;
                else {          /*634: */

                    k = mem[p + 2].cint - dvi_offset;
                    if (k < 0)
                        k = k + dvi_buf_size;
                    dvi_buf[k] = dvi_buf[k] + 10;
                    mem[p].hh.v.LH = 2 /*z_here */ ;
                    goto lab40;
                }
                break;
            case 1:
            case 2:
            case 8:
            case 13:
                goto lab40;
                break;
            default:
                ;
                break;
        } else
            switch (mstate + mem[p].hh.v.LH) {
            case 1:
                mstate = 6 /*y_seen */ ;
                break;
            case 2:
                mstate = 12 /*z_seen */ ;
                break;
            case 8:
            case 13:
                goto lab45;
                break;
            default:
                ;
                break;
            }
        p = mem[p].hh.v.RH;
    }
 lab45:                        /*not_found *//*:631 */ ;
    mem[q].hh.v.LH = 3 /*yz_OK */ ;
    if (abs(w) >= 8388608L) {
        {
            dvi_buf[dvi_ptr] = o + 3;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        dvi_four(w);
        return;
    }
    if (abs(w) >= 32768L) {
        {
            dvi_buf[dvi_ptr] = o + 2;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        if (w < 0)
            w = w + 16777216L;
        {
            dvi_buf[dvi_ptr] = w / 65536L;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        w = w % 65536L;
        goto lab2;
    }
    if (abs(w) >= 128) {
        {
            dvi_buf[dvi_ptr] = o + 1;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        if (w < 0)
            w = w + 65536L;
        goto lab2;
    }
    {
        dvi_buf[dvi_ptr] = o;
        dvi_ptr++;
        if (dvi_ptr == dvi_limit)
            dvi_swap();
    }
    if (w < 0)
        w = w + 256;
    goto lab1;
 lab2:{

        dvi_buf[dvi_ptr] = w / 256;
        dvi_ptr++;
        if (dvi_ptr == dvi_limit)
            dvi_swap();
    }
 lab1:{

        dvi_buf[dvi_ptr] = w % 256;
        dvi_ptr++;
        if (dvi_ptr == dvi_limit)
            dvi_swap();
    }
    return;
 lab40:                        /*found *//*629: */ mem[q].hh.v.LH = mem[p].hh.v.LH;
    if (mem[q].hh.v.LH == 1 /*y_here */ ) {
        {
            dvi_buf[dvi_ptr] = o + 4;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        while (mem[q].hh.v.RH != p) {

            q = mem[q].hh.v.RH;
            switch (mem[q].hh.v.LH) {
            case 3:
                mem[q].hh.v.LH = 5 /*z_OK */ ;
                break;
            case 4:
                mem[q].hh.v.LH = 6 /*d_fixed */ ;
                break;
            default:
                ;
                break;
            }
        }
    } else {

        {
            dvi_buf[dvi_ptr] = o + 9;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        while (mem[q].hh.v.RH != p) {

            q = mem[q].hh.v.RH;
            switch (mem[q].hh.v.LH) {
            case 3:
                mem[q].hh.v.LH = 4 /*y_OK */ ;
                break;
            case 5:
                mem[q].hh.v.LH = 6 /*d_fixed */ ;
                break;
            default:
                ;
                break;
            }
        }
    }
}

void zprune_movements(integer l)
{
    prune_movements_regmem halfword p;
    while (down_ptr != -268435455L) {

        if (mem[down_ptr + 2].cint < l)
            goto lab30;
        p = down_ptr;
        down_ptr = mem[p].hh.v.RH;
        free_node(p, 3 /*movement_node_size */ );
    }
 lab30:                        /*done */ while (right_ptr != -268435455L) {

        if (mem[right_ptr + 2].cint < l)
            return;
        p = right_ptr;
        right_ptr = mem[p].hh.v.RH;
        free_node(p, 3 /*movement_node_size */ );
    }
}

void zspecial_out(halfword p)
{
    special_out_regmem unsigned char /*max_selector */ old_setting;
    pool_pointer k;
    if (cur_h != dvi_h) {
        movement(cur_h - dvi_h, 143 /*right1 */ );
        dvi_h = cur_h;
    }
    if (cur_v != dvi_v) {
        movement(cur_v - dvi_v, 157 /*down1 */ );
        dvi_v = cur_v;
    }
    doing_special = true;
    old_setting = selector;
    selector = SELECTOR_NEW_STRING ;
    show_token_list(mem[mem[p + 1].hh.v.RH].hh.v.RH, -268435455L, pool_size - pool_ptr);
    selector = old_setting;
    {
        if (pool_ptr + 1 > pool_size)
            overflow(65539L /*"pool size" */ , pool_size - init_pool_ptr);
    }
    if ((pool_ptr - str_start[(str_ptr) - 65536L]) < 256) {
        {
            dvi_buf[dvi_ptr] = 239 /*xxx1 */ ;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        {
            dvi_buf[dvi_ptr] = (pool_ptr - str_start[(str_ptr) - 65536L]);
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
    } else {

        {
            dvi_buf[dvi_ptr] = 242 /*xxx4 */ ;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        dvi_four((pool_ptr - str_start[(str_ptr) - 65536L]));
    }
    {
        register integer for_end;
        k = str_start[(str_ptr) - 65536L];
        for_end = pool_ptr - 1;
        if (k <= for_end)
            do {
                dvi_buf[dvi_ptr] = str_pool[k];
                dvi_ptr++;
                if (dvi_ptr == dvi_limit)
                    dvi_swap();
            }
            while (k++ < for_end);
    }
    pool_ptr = str_start[(str_ptr) - 65536L];
    doing_special = false;
}

void zwrite_out(halfword p)
{
    write_out_regmem;

    unsigned char /*max_selector */ old_setting;
    integer old_mode;
    small_number j;
    halfword q, r;
    integer d;

    q = get_avail();
    mem[q].hh.v.LH = 4194429L /*right_brace_token 125 */ ;
    r = get_avail();
    mem[q].hh.v.RH = r;
    mem[r].hh.v.LH = 35797665L /*cs_token_flag 2243234 */ ;
    begin_token_list(q, 5 /*inserted */ );
    begin_token_list(mem[p + 1].hh.v.RH, 18 /*write_text */ );
    q = get_avail();
    mem[q].hh.v.LH = 2097275L /*left_brace_token 123 */ ;
    begin_token_list(q, 5 /*inserted */ );
    old_mode = cur_list.mode_field;
    cur_list.mode_field = 0;
    cur_cs = write_loc;
    q = scan_toks(false, true);
    get_token();
    if (cur_tok != 35797665L /*cs_token_flag 2243234 */ ) {     /*1412: */
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66743L /*"Unbalanced write command" */ );
        }
        {
            help_ptr = 2;
            help_line[1] = 66744L /*"On this page there's a \write with fewer real _'s than _'s." */ ;
            help_line[0] = 66410L /*"I can't handle that very well; good luck." */ ;
        }
        error();
        do {
            get_token();
        } while (!(cur_tok == 35797665L /*cs_token_flag 2243234 */ ));
    }
    cur_list.mode_field = old_mode;
    end_token_list();
    old_setting = selector;
    j = mem[p + 1].hh.v.LH;
    if (j == 18)
        selector = SELECTOR_NEW_STRING ;
    else if (write_open[j])
        selector = j;
    else {

        if ((j == 17) && (selector == SELECTOR_TERM_AND_LOG))
            selector = SELECTOR_LOG_ONLY;
        print_nl(65622L /*"" */ );
    }
    token_show(def_ref);
    print_ln();
    flush_list(def_ref);
    if (j == 18) {
        if ((eqtb[8938769L /*int_base 29 */ ].cint <= 0))
            selector = SELECTOR_LOG_ONLY;
        else
            selector = SELECTOR_TERM_AND_LOG;
        if (!log_opened)
            selector = SELECTOR_TERM_ONLY;
        print_nl(66735L /*"runsystem(" */ );
        {
            register integer for_end;
            d = 0;
            for_end = (pool_ptr - str_start[(str_ptr) - 65536L]) - 1;
            if (d <= for_end)
                do {
                    print(str_pool[str_start[(str_ptr) - 65536L] + d]);
                }
                while (d++ < for_end);
        }
        print(66736L /*")..." */ );
        print(66742L /*"disabled" */ );
        print_char(46 /*"." */ );
        print_nl(65622L /*"" */ );
        print_ln();
        pool_ptr = str_start[(str_ptr) - 65536L];
    }
    selector = old_setting;
}

void zpic_out(halfword p)
{
    pic_out_regmem unsigned char /*max_selector */ old_setting;
    integer i;
    pool_pointer k;
    if (cur_h != dvi_h) {
        movement(cur_h - dvi_h, 143 /*right1 */ );
        dvi_h = cur_h;
    }
    if (cur_v != dvi_v) {
        movement(cur_v - dvi_v, 157 /*down1 */ );
        dvi_v = cur_v;
    }
    old_setting = selector;
    selector = SELECTOR_NEW_STRING ;
    print(66745L /*"pdf:image " */ );
    print(66746L /*"matrix " */ );
    print_scaled(mem[p + 5].hh.v.LH);
    print(32 /*" " */ );
    print_scaled(mem[p + 5].hh.v.RH);
    print(32 /*" " */ );
    print_scaled(mem[p + 6].hh.v.LH);
    print(32 /*" " */ );
    print_scaled(mem[p + 6].hh.v.RH);
    print(32 /*" " */ );
    print_scaled(mem[p + 7].hh.v.LH);
    print(32 /*" " */ );
    print_scaled(mem[p + 7].hh.v.RH);
    print(32 /*" " */ );
    print(66747L /*"page " */ );
    print_int(mem[p + 4].hh.u.B1);
    print(32 /*" " */ );
    switch (mem[p + 8].hh.u.B0) {
    case 1:
        print(66748L /*"pagebox cropbox " */ );
        break;
    case 2:
        print(66749L /*"pagebox mediabox " */ );
        break;
    case 3:
        print(66750L /*"pagebox bleedbox " */ );
        break;
    case 5:
        print(66751L /*"pagebox artbox " */ );
        break;
    case 4:
        print(66752L /*"pagebox trimbox " */ );
        break;
    default:
        ;
        break;
    }
    print(40 /*"(" */ );
    {
        register integer for_end;
        i = 0;
        for_end = mem[p + 4].hh.u.B0 - 1;
        if (i <= for_end)
            do
                print_raw_char(pic_path_byte(p, i), true);
            while (i++ < for_end);
    }
    print(41 /*")" */ );
    selector = old_setting;
    if ((pool_ptr - str_start[(str_ptr) - 65536L]) < 256) {
        {
            dvi_buf[dvi_ptr] = 239 /*xxx1 */ ;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        {
            dvi_buf[dvi_ptr] = (pool_ptr - str_start[(str_ptr) - 65536L]);
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
    } else {

        {
            dvi_buf[dvi_ptr] = 242 /*xxx4 */ ;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        dvi_four((pool_ptr - str_start[(str_ptr) - 65536L]));
    }
    {
        register integer for_end;
        k = str_start[(str_ptr) - 65536L];
        for_end = pool_ptr - 1;
        if (k <= for_end)
            do {
                dvi_buf[dvi_ptr] = str_pool[k];
                dvi_ptr++;
                if (dvi_ptr == dvi_limit)
                    dvi_swap();
            }
            while (k++ < for_end);
    }
    pool_ptr = str_start[(str_ptr) - 65536L];
}

void zout_what(halfword p)
{
    out_what_regmem small_number j;
    unsigned char /*max_selector */ old_setting;
    switch (mem[p].hh.u.B1) {
    case 0:
    case 1:
    case 2:
        if (!doing_leaders) {
            j = mem[p + 1].hh.v.LH;
            if (mem[p].hh.u.B1 == 1 /*write_node */ )
                write_out(p);
            else {

                if (write_open[j])
                    close_file(write_file[j]);
                if (mem[p].hh.u.B1 == 2 /*close_node */ )
                    write_open[j] = false;
                else if (j < 16) {
                    cur_name = mem[p + 1].hh.v.RH;
                    cur_area = mem[p + 2].hh.v.LH;
                    cur_ext = mem[p + 2].hh.v.RH;
                    if (cur_ext == 65622L /*"" */ )
                        cur_ext = 66146L /*".tex" */ ;
                    pack_file_name(cur_name, cur_area, cur_ext);
                    while (!open_output(&write_file[j], "w"))
                        prompt_file_name(66754L /*"output file name" */ , 66146L /*".tex" */ );
                    write_open[j] = true;
                    if (log_opened) {
                        old_setting = selector;
                        if ((eqtb[8938769L /*int_base 29 */ ].cint <= 0))
                            selector = SELECTOR_LOG_ONLY;
                        else
                            selector = SELECTOR_TERM_AND_LOG;
                        print_nl(66755L /*"\openout" */ );
                        print_int(j);
                        print(66756L /*" = `" */ );
                        print_file_name(cur_name, cur_area, cur_ext);
                        print(66145L /*"'." */ );
                        print_nl(65622L /*"" */ );
                        print_ln();
                        selector = old_setting;
                    }
                }
            }
        }
        break;
    case 3:
        special_out(p);
        break;
    case 4:
        ;
        break;
    default:
        confusion(66753L /*"ext4" */ );
        break;
    }
}

halfword znew_edge(small_number s, scaled w)
{
    register halfword Result;
    new_edge_regmem halfword p;
    p = get_node(3 /*edge_node_size */ );
    mem[p].hh.u.B0 = 14 /*edge_node */ ;
    mem[p].hh.u.B1 = s;
    mem[p + 1].cint = w;
    mem[p + 2].cint = 0;
    Result = p;
    return Result;
}

halfword zzreverse(halfword this_box, halfword t, scaled * cur_g, double * cur_glue)
{
    register halfword Result;
    reverse_regmem halfword l;
    halfword p;
    halfword q;
    glue_ord g_order;
    unsigned char /*shrinking */ g_sign;
    double glue_temp;
    halfword m, n;
    g_order = mem[this_box + 5].hh.u.B1;
    g_sign = mem[this_box + 5].hh.u.B0;
    l = t;
    p = temp_ptr;
    m = -268435455L;
    n = -268435455L;
    while (true) {

        while (p != -268435455L)        /*1511: */
 lab21:                        /*reswitch */ if ((p >= hi_mem_min))
                do {
                    f = mem[p].hh.u.B0;
                    c = mem[p].hh.u.B1;
                    cur_h =
                        cur_h + font_info[width_base[f] +
                                          font_info[char_base[f] + effective_char(true, f, c)].qqqq.u.B0].cint;
                    q = mem[p].hh.v.RH;
                    mem[p].hh.v.RH = l;
                    l = p;
                    p = q;
                } while (!(!(p >= hi_mem_min)));
            else {              /*1512: */

                q = mem[p].hh.v.RH;
                switch (mem[p].hh.u.B0) {
                case 0:
                case 1:
                case 2:
                case 11:
                    rule_wd = mem[p + 1].cint;
                    break;
                case 8:
                    if ((mem[p].hh.u.B1 == 40 /*native_word_node */ ) || (mem[p].hh.u.B1 == 41 /*native_word_node_AT */ )
                        || (mem[p].hh.u.B1 == 42 /*glyph_node */ ) || (mem[p].hh.u.B1 == 43 /*pic_node */ )
                        || (mem[p].hh.u.B1 == 44 /*pdf_node */ ))
                        rule_wd = mem[p + 1].cint;
                    else
                        goto lab15;
                    break;
                case 10:
                    {
                        g = mem[p + 1].hh.v.LH;
                        rule_wd = mem[g + 1].cint - *cur_g;
                        if (g_sign != 0 /*normal */ ) {
                            if (g_sign == 1 /*stretching */ ) {
                                if (mem[g].hh.u.B0 == g_order) {
                                    *cur_glue = *cur_glue + mem[g + 2].cint;
                                    glue_temp = mem[this_box + 6].gr * *cur_glue;
                                    if (glue_temp > 1000000000.0)
                                        glue_temp = 1000000000.0;
                                    else if (glue_temp < -1000000000.0)
                                        glue_temp = -1000000000.0;
                                    *cur_g = round(glue_temp);
                                }
                            } else if (mem[g].hh.u.B1 == g_order) {
                                *cur_glue = *cur_glue - mem[g + 3].cint;
                                glue_temp = mem[this_box + 6].gr * *cur_glue;
                                if (glue_temp > 1000000000.0)
                                    glue_temp = 1000000000.0;
                                else if (glue_temp < -1000000000.0)
                                    glue_temp = -1000000000.0;
                                *cur_g = round(glue_temp);
                            }
                        }
                        rule_wd = rule_wd + *cur_g;
                        if ((((g_sign == 1 /*stretching */ ) && (mem[g].hh.u.B0 == g_order))
                             || ((g_sign == 2 /*shrinking */ ) && (mem[g].hh.u.B1 == g_order)))) {
                            {
                                if (mem[g].hh.v.RH == -268435455L)
                                    free_node(g, 4 /*glue_spec_size */ );
                                else
                                    mem[g].hh.v.RH--;
                            }
                            if (mem[p].hh.u.B1 < 100 /*a_leaders */ ) {
                                mem[p].hh.u.B0 = 11 /*kern_node */ ;
                                mem[p + 1].cint = rule_wd;
                            } else {

                                g = get_node(4 /*glue_spec_size */ );
                                mem[g].hh.u.B0 = 4 /*filll 1 */ ;
                                mem[g].hh.u.B1 = 4 /*filll 1 */ ;
                                mem[g + 1].cint = rule_wd;
                                mem[g + 2].cint = 0;
                                mem[g + 3].cint = 0;
                                mem[p + 1].hh.v.LH = g;
                            }
                        }
                    }
                    break;
                case 6:
                    {
                        flush_node_list(mem[p + 1].hh.v.RH);
                        temp_ptr = p;
                        p = get_avail();
                        mem[p] = mem[temp_ptr + 1];
                        mem[p].hh.v.RH = q;
                        free_node(temp_ptr, 2 /*small_node_size */ );
                        goto lab21;
                    }
                    break;
                case 9:
                    {
                        rule_wd = mem[p + 1].cint;
                        if (odd(mem[p].hh.u.B1)) {

                            if (mem[LR_ptr].hh.v.LH != (4 /*L_code */  * (mem[p].hh.u.B1 / 4 /*L_code */ ) + 3)) {
                                mem[p].hh.u.B0 = 11 /*kern_node */ ;
                                LR_problems++;
                            } else {

                                {
                                    temp_ptr = LR_ptr;
                                    LR_ptr = mem[temp_ptr].hh.v.RH;
                                    {
                                        mem[temp_ptr].hh.v.RH = avail;
                                        avail = temp_ptr;
                                    }
                                }
                                if (n > -268435455L) {
                                    n--;
                                    mem[p].hh.u.B1--;
                                } else {

                                    mem[p].hh.u.B0 = 11 /*kern_node */ ;
                                    if (m > -268435455L)
                                        m--;
                                    else {      /*1517: */

                                        free_node(p, 3 /*medium_node_size */ );
                                        mem[t].hh.v.RH = q;
                                        mem[t + 1].cint = rule_wd;
                                        mem[t + 2].cint = -(integer) cur_h - rule_wd;
                                        goto lab30;
                                    }
                                }
                            }
                        } else {

                            {
                                temp_ptr = get_avail();
                                mem[temp_ptr].hh.v.LH = (4 /*L_code */  * (mem[p].hh.u.B1 / 4 /*L_code */ ) + 3);
                                mem[temp_ptr].hh.v.RH = LR_ptr;
                                LR_ptr = temp_ptr;
                            }
                            if ((n > -268435455L) || ((mem[p].hh.u.B1 / 8 /*R_code */ ) != cur_dir)) {
                                n++;
                                mem[p].hh.u.B1++;
                            } else {

                                mem[p].hh.u.B0 = 11 /*kern_node */ ;
                                m++;
                            }
                        }
                    }
                    break;
                case 14:
                    confusion(66898L /*"LR2" */ );
                    break;
                default:
                    goto lab15;
                    break;
                }
                cur_h = cur_h + rule_wd;
 lab15:                        /*next_p */ mem[p].hh.v.RH = l;
                if (mem[p].hh.u.B0 == 11 /*kern_node */ ) {

                    if ((rule_wd == 0) || (l == -268435455L)) {
                        free_node(p, 3 /*medium_node_size */ );
                        p = l;
                    }
                }
                l = p;
                p = q;
            }
        if ((t == -268435455L) && (m == -268435455L) && (n == -268435455L))
            goto lab30;
        p = new_math(0, mem[LR_ptr].hh.v.LH);
        LR_problems = LR_problems + 10000;
    }
 lab30:                        /*done */ Result = l;
    return Result;
}

void hlist_out(void)
{
    hlist_out_regmem scaled base_line;
    scaled left_edge;
    scaled save_h, save_v;
    halfword this_box;
    glue_ord g_order;
    unsigned char /*shrinking */ g_sign;
    halfword p;
    integer save_loc;
    halfword leader_box;
    scaled leader_wd;
    scaled lx;
    boolean outer_doing_leaders;
    scaled edge;
    halfword prev_p;
    integer len;
    halfword q, r;
    integer k, j;
    double glue_temp;
    double cur_glue;
    scaled cur_g;
    cur_g = 0;
    cur_glue = 0.0;
    this_box = temp_ptr;
    g_order = mem[this_box + 5].hh.u.B1;
    g_sign = mem[this_box + 5].hh.u.B0;
    if (eqtb[8938820L /*eTeX_state_base 9 */ ].cint > 1) {
        p = mem[this_box + 5].hh.v.RH;
        prev_p = this_box + 5;
        while (p != -268435455L) {

            if (mem[p].hh.v.RH != -268435455L) {
                if ((((p) != -268435455L && (!(p >= hi_mem_min)) && (mem[p].hh.u.B0 == 8 /*whatsit_node */ )
                      && ((mem[p].hh.u.B1 == 40 /*native_word_node */ )
                          || (mem[p].hh.u.B1 == 41 /*native_word_node_AT */ ))))
                    && (font_letter_space[mem[p + 4].qqqq.u.B1] == 0)) {
                    r = p;
                    k = mem[r + 4].qqqq.u.B2;
                    q = mem[p].hh.v.RH;
 lab1236:          /*check_next *//*641: */ while ((q != -268435455L) && !(q >= hi_mem_min)
                                                    && ((mem[q].hh.u.B0 == 12 /*penalty_node */ )
                                                        || (mem[q].hh.u.B0 == 3 /*ins_node */ )
                                                        || (mem[q].hh.u.B0 == 4 /*mark_node */ )
                                                        || (mem[q].hh.u.B0 == 5 /*adjust_node */ )
                                                        || ((mem[q].hh.u.B0 == 8 /*whatsit_node */ )
                                                            && (mem[q].hh.u.B1 <= 4))))
                        q = mem[q].hh.v.RH /*:641 */ ;
                    if ((q != -268435455L) && !(q >= hi_mem_min)) {
                        if ((mem[q].hh.u.B0 == 10 /*glue_node */ ) && (mem[q].hh.u.B1 == 0 /*normal */ )) {
                            if ((mem[q + 1].hh.v.LH == font_glue[mem[r + 4].qqqq.u.B1])) {
                                q = mem[q].hh.v.RH;
                                while ((q != -268435455L) && !(q >= hi_mem_min)
                                       && ((mem[q].hh.u.B0 == 12 /*penalty_node */ ) || (mem[q].hh.u.B0 == 3 /*ins_node */ )
                                           || (mem[q].hh.u.B0 == 4 /*mark_node */ )
                                           || (mem[q].hh.u.B0 == 5 /*adjust_node */ )
                                           || ((mem[q].hh.u.B0 == 8 /*whatsit_node */ ) && (mem[q].hh.u.B1 <= 4))))
                                    q = mem[q].hh.v.RH /*:641 */ ;
                                if ((((q) != -268435455L && (!(q >= hi_mem_min))
                                      && (mem[q].hh.u.B0 == 8 /*whatsit_node */ )
                                      && ((mem[q].hh.u.B1 == 40 /*native_word_node */ )
                                          || (mem[q].hh.u.B1 == 41 /*native_word_node_AT */ ))))
                                    && (mem[q + 4].qqqq.u.B1 == mem[r + 4].qqqq.u.B1)) {
                                    p = q;
                                    k = k + 1 + mem[q + 4].qqqq.u.B2;
                                    q = mem[q].hh.v.RH;
                                    goto lab1236;
                                }
                            } else
                                q = mem[q].hh.v.RH;
                            if ((q != -268435455L) && !(q >= hi_mem_min) && (mem[q].hh.u.B0 == 11 /*kern_node */ )
                                && (mem[q].hh.u.B1 == 3 /*space_adjustment */ )) {
                                q = mem[q].hh.v.RH;
                                while ((q != -268435455L) && !(q >= hi_mem_min)
                                       && ((mem[q].hh.u.B0 == 12 /*penalty_node */ ) || (mem[q].hh.u.B0 == 3 /*ins_node */ )
                                           || (mem[q].hh.u.B0 == 4 /*mark_node */ )
                                           || (mem[q].hh.u.B0 == 5 /*adjust_node */ )
                                           || ((mem[q].hh.u.B0 == 8 /*whatsit_node */ ) && (mem[q].hh.u.B1 <= 4))))
                                    q = mem[q].hh.v.RH /*:641 */ ;
                                if ((((q) != -268435455L && (!(q >= hi_mem_min))
                                      && (mem[q].hh.u.B0 == 8 /*whatsit_node */ )
                                      && ((mem[q].hh.u.B1 == 40 /*native_word_node */ )
                                          || (mem[q].hh.u.B1 == 41 /*native_word_node_AT */ ))))
                                    && (mem[q + 4].qqqq.u.B1 == mem[r + 4].qqqq.u.B1)) {
                                    p = q;
                                    k = k + 1 + mem[q + 4].qqqq.u.B2;
                                    q = mem[q].hh.v.RH;
                                    goto lab1236;
                                }
                            }
                            goto lab1237;
                        }
                        if ((((q) != -268435455L && (!(q >= hi_mem_min)) && (mem[q].hh.u.B0 == 8 /*whatsit_node */ )
                              && ((mem[q].hh.u.B1 == 40 /*native_word_node */ )
                                  || (mem[q].hh.u.B1 == 41 /*native_word_node_AT */ ))))
                            && (mem[q + 4].qqqq.u.B1 == mem[r + 4].qqqq.u.B1)) {
                            p = q;
                            q = mem[q].hh.v.RH;
                            goto lab1236;
                        }
                    }
 lab1237:                      /*end_node_run */ if (p != r) {
                        {
                            if (pool_ptr + k > pool_size)
                                overflow(65539L /*"pool size" */ , pool_size - init_pool_ptr);
                        }
                        k = 0;
                        q = r;
                        while (true) {

                            if (mem[q].hh.u.B0 == 8 /*whatsit_node */ ) {
                                if ((mem[q].hh.u.B1 == 40 /*native_word_node */ )
                                    || (mem[q].hh.u.B1 == 41 /*native_word_node_AT */ )) {
                                    {
                                        register integer for_end;
                                        j = 0;
                                        for_end = mem[q + 4].qqqq.u.B2 - 1;
                                        if (j <= for_end)
                                            do {
                                                str_pool[pool_ptr] = get_native_char(q, j);
                                                pool_ptr++;
                                            }
                                            while (j++ < for_end);
                                    }
                                    k = k + mem[q + 1].cint;
                                }
                            } else if (mem[q].hh.u.B0 == 10 /*glue_node */ ) {
                                {
                                    str_pool[pool_ptr] = 32 /*" " */ ;
                                    pool_ptr++;
                                }
                                g = mem[q + 1].hh.v.LH;
                                k = k + mem[g + 1].cint;
                                if (g_sign != 0 /*normal */ ) {
                                    if (g_sign == 1 /*stretching */ ) {
                                        if (mem[g].hh.u.B0 == g_order) {
                                            k = k + round(mem[this_box + 6].gr * mem[g + 2].cint);
                                        }
                                    } else {

                                        if (mem[g].hh.u.B1 == g_order) {
                                            k = k - round(mem[this_box + 6].gr * mem[g + 3].cint);
                                        }
                                    }
                                }
                            } else if (mem[q].hh.u.B0 == 11 /*kern_node */ ) {
                                k = k + mem[q + 1].cint;
                            }
                            if (q == p)
                                break;
                            else
                                q = mem[q].hh.v.RH;
                        }
                        q = new_native_word_node(mem[r + 4].qqqq.u.B1, (pool_ptr - str_start[(str_ptr) - 65536L]));
                        mem[q].hh.u.B1 = mem[r].hh.u.B1;
                        {
                            register integer for_end;
                            j = 0;
                            for_end = (pool_ptr - str_start[(str_ptr) - 65536L]) - 1;
                            if (j <= for_end)
                                do
                                    set_native_char(q, j, str_pool[str_start[(str_ptr) - 65536L] + j]);
                                while (j++ < for_end);
                        }
                        mem[q + 1].cint = k;
                        set_justified_native_glyphs(q);
                        mem[prev_p].hh.v.RH = q;
                        mem[q].hh.v.RH = mem[p].hh.v.RH;
                        mem[p].hh.v.RH = -268435455L;
                        prev_p = r;
                        p = mem[r].hh.v.RH;
                        while (p != -268435455L) {

                            if (!(p >= hi_mem_min)
                                && ((mem[p].hh.u.B0 == 12 /*penalty_node */ ) || (mem[p].hh.u.B0 == 3 /*ins_node */ )
                                    || (mem[p].hh.u.B0 == 4 /*mark_node */ ) || (mem[p].hh.u.B0 == 5 /*adjust_node */ )
                                    || ((mem[p].hh.u.B0 == 8 /*whatsit_node */ ) && (mem[p].hh.u.B1 <= 4)))) {
                                mem[prev_p].hh.v.RH = mem[p].hh.v.RH;
                                mem[p].hh.v.RH = mem[q].hh.v.RH;
                                mem[q].hh.v.RH = p;
                                q = p;
                            }
                            prev_p = p;
                            p = mem[p].hh.v.RH;
                        }
                        flush_node_list(r);
                        pool_ptr = str_start[(str_ptr) - 65536L];
                        p = q;
                    }
                }
                prev_p = p;
            }
            p = mem[p].hh.v.RH;
        }
    }
    p = mem[this_box + 5].hh.v.RH;
    cur_s++;
    if (cur_s > 0) {
        dvi_buf[dvi_ptr] = 141 /*push */ ;
        dvi_ptr++;
        if (dvi_ptr == dvi_limit)
            dvi_swap();
    }
    if (cur_s > max_push)
        max_push = cur_s;
    save_loc = dvi_offset + dvi_ptr;
    base_line = cur_v;
    prev_p = this_box + 5;
    if ((eTeX_mode == 1)) {
        {
            temp_ptr = get_avail();
            mem[temp_ptr].hh.v.LH = 0 /*before */ ;
            mem[temp_ptr].hh.v.RH = LR_ptr;
            LR_ptr = temp_ptr;
        }
        if ((mem[this_box].hh.u.B1) == 2 /*dlist */ ) {

            if (cur_dir == 1 /*right_to_left */ ) {
                cur_dir = 0 /*left_to_right */ ;
                cur_h = cur_h - mem[this_box + 1].cint;
            } else
                mem[this_box].hh.u.B1 = 0;
        }
        if ((cur_dir == 1 /*right_to_left */ ) && ((mem[this_box].hh.u.B1) != 1 /*reversed */ )) {        /*1508: */
            save_h = cur_h;
            temp_ptr = p;
            p = new_kern(0);
            mem[p + 2].hh.v.LH = 0;
            mem[prev_p].hh.v.RH = p;
            cur_h = 0;
            mem[p].hh.v.RH = reverse(this_box, -268435455L, cur_g, cur_glue);
            mem[p + 1].cint = -(integer) cur_h;
            cur_h = save_h;
            mem[this_box].hh.u.B1 = 1 /*reversed */ ;
        }
    }
    left_edge = cur_h;
    synctex_hlist(this_box);
    while (p != -268435455L)    /*642: */
 lab21:                        /*reswitch */ if ((p >= hi_mem_min)) {
            if (cur_h != dvi_h) {
                movement(cur_h - dvi_h, 143 /*right1 */ );
                dvi_h = cur_h;
            }
            if (cur_v != dvi_v) {
                movement(cur_v - dvi_v, 157 /*down1 */ );
                dvi_v = cur_v;
            }
            do {
                f = mem[p].hh.u.B0;
                c = mem[p].hh.u.B1;
                if ((p != mem_top - 12) && (font_mapping[f] != NULL))
                    c = apply_tfm_font_mapping(font_mapping[f], c);
                if (f != dvi_f) {       /*643: */
                    if (!font_used[f]) {
                        dvi_font_def(f);
                        font_used[f] = true;
                    }
                    if (f <= 64) {
                        dvi_buf[dvi_ptr] = f + 170;
                        dvi_ptr++;
                        if (dvi_ptr == dvi_limit)
                            dvi_swap();
                    } else if (f <= 256) {
                        {
                            dvi_buf[dvi_ptr] = 235 /*fnt1 */ ;
                            dvi_ptr++;
                            if (dvi_ptr == dvi_limit)
                                dvi_swap();
                        }
                        {
                            dvi_buf[dvi_ptr] = f - 1;
                            dvi_ptr++;
                            if (dvi_ptr == dvi_limit)
                                dvi_swap();
                        }
                    } else {

                        {
                            dvi_buf[dvi_ptr] = 236 /*fnt1 1 */ ;
                            dvi_ptr++;
                            if (dvi_ptr == dvi_limit)
                                dvi_swap();
                        }
                        {
                            dvi_buf[dvi_ptr] = (f - 1) / 256;
                            dvi_ptr++;
                            if (dvi_ptr == dvi_limit)
                                dvi_swap();
                        }
                        {
                            dvi_buf[dvi_ptr] = (f - 1) % 256;
                            dvi_ptr++;
                            if (dvi_ptr == dvi_limit)
                                dvi_swap();
                        }
                    }
                    dvi_f = f;
                }
                if (font_ec[f] >= c) {

                    if (font_bc[f] <= c) {

                        if ((font_info[char_base[f] + c].qqqq.u.B0 > 0 /*min_quarterword */ )) {
                            if (c >= 128) {
                                dvi_buf[dvi_ptr] = 128 /*set1 */ ;
                                dvi_ptr++;
                                if (dvi_ptr == dvi_limit)
                                    dvi_swap();
                            }
                            {
                                dvi_buf[dvi_ptr] = c;
                                dvi_ptr++;
                                if (dvi_ptr == dvi_limit)
                                    dvi_swap();
                            }
                            cur_h = cur_h + font_info[width_base[f] + font_info[char_base[f] + c].qqqq.u.B0].cint;
                            goto lab22;
                        }
                    }
                }
                if (mltex_enabled_p) {  /*1669: */
                    if (c >= eqtb[8938795L /*int_base 55 */ ].cint) {

                        if (c <= eqtb[8938796L /*int_base 56 */ ].cint) {

                            if ((eqtb[7824628L /*char_sub_code_base */  + c].hh.v.RH > 0)) {
                                base_c = (eqtb[7824628L /*char_sub_code_base */  + c].hh.v.RH % 256);
                                accent_c = (eqtb[7824628L /*char_sub_code_base */  + c].hh.v.RH / 256);
                                if ((font_ec[f] >= base_c)) {

                                    if ((font_bc[f] <= base_c)) {

                                        if ((font_ec[f] >= accent_c)) {

                                            if ((font_bc[f] <= accent_c)) {
                                                ia_c = font_info[char_base[f] + effective_char(true, f, accent_c)].qqqq;
                                                ib_c = font_info[char_base[f] + effective_char(true, f, base_c)].qqqq;
                                                if ((ib_c.u.B0 > 0 /*min_quarterword */ )) {

                                                    if ((ia_c.u.B0 > 0 /*min_quarterword */ ))
                                                        goto lab40;
                                                }
                                            }
                                        }
                                    }
                                }
                                begin_diagnostic();
                                print_nl(66943L /*"Missing character: Incomplete substitution " */ );
                                print(c);
                                print(66610L /*" = " */ );
                                print(accent_c);
                                print(32 /*" " */ );
                                print(base_c);
                                print(66187L /*" in font " */ );
                                print(font_name[f]);
                                print_char(33 /*"!" */ );
                                end_diagnostic(false);
                                goto lab22;
                            }
                        }
                    }
                    begin_diagnostic();
                    print_nl(66186L /*"Missing character: There is no " */ );
                    print(66942L /*"substitution for " */ );
                    print(c);
                    print(66187L /*" in font " */ );
                    print(font_name[f]);
                    print_char(33 /*"!" */ );
                    end_diagnostic(false);
                    goto lab22;
 lab40:            /*found *//*1672: */ if (eqtb[8938775L /*int_base 35 */ ].cint > 99) {
                        begin_diagnostic();
                        print_nl(66944L /*"Using character substitution: " */ );
                        print(c);
                        print(66610L /*" = " */ );
                        print(accent_c);
                        print(32 /*" " */ );
                        print(base_c);
                        print(66187L /*" in font " */ );
                        print(font_name[f]);
                        print_char(46 /*"." */ );
                        end_diagnostic(false);
                    }
                    base_x_height = font_info[5 /*x_height_code */  + param_base[f]].cint;
                    base_slant = font_info[1 /*slant_code */  + param_base[f]].cint / ((double)65536.0);
                    accent_slant = base_slant;
                    base_width = font_info[width_base[f] + ib_c.u.B0].cint;
                    base_height = font_info[height_base[f] + (ib_c.u.B1) / 16].cint;
                    accent_width = font_info[width_base[f] + ia_c.u.B0].cint;
                    accent_height = font_info[height_base[f] + (ia_c.u.B1) / 16].cint;
                    delta =
                        round((base_width - accent_width) / ((double)2.0) + base_height * base_slant -
                              base_x_height * accent_slant);
                    dvi_h = cur_h;
                    cur_h = cur_h + delta;
                    if (cur_h != dvi_h) {
                        movement(cur_h - dvi_h, 143 /*right1 */ );
                        dvi_h = cur_h;
                    }
                    if (((base_height != base_x_height) && (accent_height > 0))) {
                        cur_v = base_line + (base_x_height - base_height);
                        if (cur_v != dvi_v) {
                            movement(cur_v - dvi_v, 157 /*down1 */ );
                            dvi_v = cur_v;
                        }
                        if (accent_c >= 128) {
                            dvi_buf[dvi_ptr] = 128 /*set1 */ ;
                            dvi_ptr++;
                            if (dvi_ptr == dvi_limit)
                                dvi_swap();
                        }
                        {
                            dvi_buf[dvi_ptr] = accent_c;
                            dvi_ptr++;
                            if (dvi_ptr == dvi_limit)
                                dvi_swap();
                        }
                        cur_v = base_line;
                    } else {

                        if (cur_v != dvi_v) {
                            movement(cur_v - dvi_v, 157 /*down1 */ );
                            dvi_v = cur_v;
                        }
                        if (accent_c >= 128) {
                            dvi_buf[dvi_ptr] = 128 /*set1 */ ;
                            dvi_ptr++;
                            if (dvi_ptr == dvi_limit)
                                dvi_swap();
                        }
                        {
                            dvi_buf[dvi_ptr] = accent_c;
                            dvi_ptr++;
                            if (dvi_ptr == dvi_limit)
                                dvi_swap();
                        }
                    }
                    cur_h = cur_h + accent_width;
                    dvi_h = cur_h;
                    cur_h = cur_h + (-(integer) accent_width - delta);
                    if (cur_h != dvi_h) {
                        movement(cur_h - dvi_h, 143 /*right1 */ );
                        dvi_h = cur_h;
                    }
                    if (cur_v != dvi_v) {
                        movement(cur_v - dvi_v, 157 /*down1 */ );
                        dvi_v = cur_v;
                    }
                    if (base_c >= 128) {
                        dvi_buf[dvi_ptr] = 128 /*set1 */ ;
                        dvi_ptr++;
                        if (dvi_ptr == dvi_limit)
                            dvi_swap();
                    }
                    {
                        dvi_buf[dvi_ptr] = base_c;
                        dvi_ptr++;
                        if (dvi_ptr == dvi_limit)
                            dvi_swap();
                    }
                    cur_h = cur_h + base_width;
                    dvi_h = /*:1673 */ cur_h;
                }
 lab22:                        /*continue */ prev_p = mem[prev_p].hh.v.RH;
                p = mem[p].hh.v.RH;
            } while (!(!(p >= hi_mem_min)));
            synctex_current();
            dvi_h = cur_h;
        } else {                /*644: */

            switch (mem[p].hh.u.B0) {
            case 0:
            case 1:
                if (mem[p + 5].hh.v.RH == -268435455L) {
                    if (mem[p].hh.u.B0 == 1 /*vlist_node */ ) {
                        synctex_void_vlist(p, this_box);
                    } else {

                        synctex_void_hlist(p, this_box);
                    }
                    cur_h = cur_h + mem[p + 1].cint;
                } else {

                    save_h = dvi_h;
                    save_v = dvi_v;
                    cur_v = base_line + mem[p + 4].cint;
                    temp_ptr = p;
                    edge = cur_h + mem[p + 1].cint;
                    if (cur_dir == 1 /*right_to_left */ )
                        cur_h = edge;
                    if (mem[p].hh.u.B0 == 1 /*vlist_node */ )
                        vlist_out();
                    else
                        hlist_out();
                    dvi_h = save_h;
                    dvi_v = save_v;
                    cur_h = edge;
                    cur_v = base_line;
                }
                break;
            case 2:
                {
                    rule_ht = mem[p + 3].cint;
                    rule_dp = mem[p + 2].cint;
                    rule_wd = mem[p + 1].cint;
                    goto lab14;
                }
                break;
            case 8:
                {
                    switch (mem[p].hh.u.B1) {
                    case 40:
                    case 41:
                    case 42:
                        {
                            if (cur_h != dvi_h) {
                                movement(cur_h - dvi_h, 143 /*right1 */ );
                                dvi_h = cur_h;
                            }
                            if (cur_v != dvi_v) {
                                movement(cur_v - dvi_v, 157 /*down1 */ );
                                dvi_v = cur_v;
                            }
                            f = mem[p + 4].qqqq.u.B1;
                            if (f != dvi_f) {   /*643: */
                                if (!font_used[f]) {
                                    dvi_font_def(f);
                                    font_used[f] = true;
                                }
                                if (f <= 64) {
                                    dvi_buf[dvi_ptr] = f + 170;
                                    dvi_ptr++;
                                    if (dvi_ptr == dvi_limit)
                                        dvi_swap();
                                } else if (f <= 256) {
                                    {
                                        dvi_buf[dvi_ptr] = 235 /*fnt1 */ ;
                                        dvi_ptr++;
                                        if (dvi_ptr == dvi_limit)
                                            dvi_swap();
                                    }
                                    {
                                        dvi_buf[dvi_ptr] = f - 1;
                                        dvi_ptr++;
                                        if (dvi_ptr == dvi_limit)
                                            dvi_swap();
                                    }
                                } else {

                                    {
                                        dvi_buf[dvi_ptr] = 236 /*fnt1 1 */ ;
                                        dvi_ptr++;
                                        if (dvi_ptr == dvi_limit)
                                            dvi_swap();
                                    }
                                    {
                                        dvi_buf[dvi_ptr] = (f - 1) / 256;
                                        dvi_ptr++;
                                        if (dvi_ptr == dvi_limit)
                                            dvi_swap();
                                    }
                                    {
                                        dvi_buf[dvi_ptr] = (f - 1) % 256;
                                        dvi_ptr++;
                                        if (dvi_ptr == dvi_limit)
                                            dvi_swap();
                                    }
                                }
                                dvi_f = f;
                            }
                            if (mem[p].hh.u.B1 == 42 /*glyph_node */ ) {
                                {
                                    dvi_buf[dvi_ptr] = 253 /*set_glyphs */ ;
                                    dvi_ptr++;
                                    if (dvi_ptr == dvi_limit)
                                        dvi_swap();
                                }
                                dvi_four(mem[p + 1].cint);
                                dvi_two(1);
                                dvi_four(0);
                                dvi_four(0);
                                dvi_two(mem[p + 4].qqqq.u.B2);
                                cur_h = cur_h + mem[p + 1].cint;
                            } else {

                                if (mem[p].hh.u.B1 == 41 /*native_word_node_AT */ ) {
                                    if ((mem[p + 4].qqqq.u.B2 > 0) || (mem[p + 5].ptr != NULL)) {
                                        {
                                            dvi_buf[dvi_ptr] = 254 /*set_text_and_glyphs */ ;
                                            dvi_ptr++;
                                            if (dvi_ptr == dvi_limit)
                                                dvi_swap();
                                        }
                                        len = mem[p + 4].qqqq.u.B2;
                                        dvi_two(len);
                                        {
                                            register integer for_end;
                                            k = 0;
                                            for_end = len - 1;
                                            if (k <= for_end)
                                                do {
                                                    dvi_two(get_native_char(p, k));
                                                }
                                                while (k++ < for_end);
                                        }
                                        len = make_xdv_glyph_array_data(p);
                                        {
                                            register integer for_end;
                                            k = 0;
                                            for_end = len - 1;
                                            if (k <= for_end)
                                                do {
                                                    dvi_buf[dvi_ptr] = xdv_buffer[k];
                                                    dvi_ptr++;
                                                    if (dvi_ptr == dvi_limit)
                                                        dvi_swap();
                                                }
                                                while (k++ < for_end);
                                        }
                                    }
                                } else {

                                    if (mem[p + 5].ptr != NULL) {
                                        {
                                            dvi_buf[dvi_ptr] = 253 /*set_glyphs */ ;
                                            dvi_ptr++;
                                            if (dvi_ptr == dvi_limit)
                                                dvi_swap();
                                        }
                                        len = make_xdv_glyph_array_data(p);
                                        {
                                            register integer for_end;
                                            k = 0;
                                            for_end = len - 1;
                                            if (k <= for_end)
                                                do {
                                                    dvi_buf[dvi_ptr] = xdv_buffer[k];
                                                    dvi_ptr++;
                                                    if (dvi_ptr == dvi_limit)
                                                        dvi_swap();
                                                }
                                                while (k++ < for_end);
                                        }
                                    }
                                }
                                cur_h = cur_h + mem[p + 1].cint;
                            }
                            dvi_h = cur_h;
                        }
                        break;
                    case 43:
                    case 44:
                        {
                            save_h = dvi_h;
                            save_v = dvi_v;
                            cur_v = base_line;
                            edge = cur_h + mem[p + 1].cint;
                            pic_out(p);
                            dvi_h = save_h;
                            dvi_v = save_v;
                            cur_h = edge;
                            cur_v = base_line;
                        }
                        break;
                    case 6:
                        {
                            pdf_last_x_pos = cur_h + cur_h_offset;
                            pdf_last_y_pos = cur_page_height - cur_v - cur_v_offset;
                        }
                        break;
                    default:
                        out_what(p);
                        break;
                    }
                }
                break;
            case 10:
                {
                    g = mem[p + 1].hh.v.LH;
                    rule_wd = mem[g + 1].cint - cur_g;
                    if (g_sign != 0 /*normal */ ) {
                        if (g_sign == 1 /*stretching */ ) {
                            if (mem[g].hh.u.B0 == g_order) {
                                cur_glue = cur_glue + mem[g + 2].cint;
                                glue_temp = mem[this_box + 6].gr * cur_glue;
                                if (glue_temp > 1000000000.0)
                                    glue_temp = 1000000000.0;
                                else if (glue_temp < -1000000000.0)
                                    glue_temp = -1000000000.0;
                                cur_g = round(glue_temp);
                            }
                        } else if (mem[g].hh.u.B1 == g_order) {
                            cur_glue = cur_glue - mem[g + 3].cint;
                            glue_temp = mem[this_box + 6].gr * cur_glue;
                            if (glue_temp > 1000000000.0)
                                glue_temp = 1000000000.0;
                            else if (glue_temp < -1000000000.0)
                                glue_temp = -1000000000.0;
                            cur_g = round(glue_temp);
                        }
                    }
                    rule_wd = rule_wd + cur_g;
                    if ((eTeX_mode == 1)) {     /*1486: */

                        if ((((g_sign == 1 /*stretching */ ) && (mem[g].hh.u.B0 == g_order))
                             || ((g_sign == 2 /*shrinking */ ) && (mem[g].hh.u.B1 == g_order)))) {
                            {
                                if (mem[g].hh.v.RH == -268435455L)
                                    free_node(g, 4 /*glue_spec_size */ );
                                else
                                    mem[g].hh.v.RH--;
                            }
                            if (mem[p].hh.u.B1 < 100 /*a_leaders */ ) {
                                mem[p].hh.u.B0 = 11 /*kern_node */ ;
                                mem[p + 1].cint = rule_wd;
                            } else {

                                g = get_node(4 /*glue_spec_size */ );
                                mem[g].hh.u.B0 = 4 /*filll 1 */ ;
                                mem[g].hh.u.B1 = 4 /*filll 1 */ ;
                                mem[g + 1].cint = rule_wd;
                                mem[g + 2].cint = 0;
                                mem[g + 3].cint = 0;
                                mem[p + 1].hh.v.LH = g;
                            }
                        }
                    }
                    if (mem[p].hh.u.B1 >= 100 /*a_leaders */ ) {  /*648: */
                        leader_box = mem[p + 1].hh.v.RH;
                        if (mem[leader_box].hh.u.B0 == 2 /*rule_node */ ) {
                            rule_ht = mem[leader_box + 3].cint;
                            rule_dp = mem[leader_box + 2].cint;
                            goto lab14;
                        }
                        leader_wd = mem[leader_box + 1].cint;
                        if ((leader_wd > 0) && (rule_wd > 0)) {
                            rule_wd = rule_wd + 10;
                            if (cur_dir == 1 /*right_to_left */ )
                                cur_h = cur_h - 10;
                            edge = cur_h + rule_wd;
                            lx = 0;
                            if (mem[p].hh.u.B1 == 100 /*a_leaders */ ) {
                                save_h = cur_h;
                                cur_h = left_edge + leader_wd * ((cur_h - left_edge) / leader_wd);
                                if (cur_h < save_h)
                                    cur_h = cur_h + leader_wd;
                            } else {

                                lq = rule_wd / leader_wd;
                                lr = rule_wd % leader_wd;
                                if (mem[p].hh.u.B1 == 101 /*c_leaders */ )
                                    cur_h = cur_h + (lr / 2);
                                else {

                                    lx = lr / (lq + 1);
                                    cur_h = cur_h + ((lr - (lq - 1) * lx) / 2);
                                }
                            }
                            while (cur_h + leader_wd <= edge) { /*650: */

                                cur_v = base_line + mem[leader_box + 4].cint;
                                if (cur_v != dvi_v) {
                                    movement(cur_v - dvi_v, 157 /*down1 */ );
                                    dvi_v = cur_v;
                                }
                                save_v = dvi_v;
                                if (cur_h != dvi_h) {
                                    movement(cur_h - dvi_h, 143 /*right1 */ );
                                    dvi_h = cur_h;
                                }
                                save_h = dvi_h;
                                temp_ptr = leader_box;
                                if (cur_dir == 1 /*right_to_left */ )
                                    cur_h = cur_h + leader_wd;
                                outer_doing_leaders = doing_leaders;
                                doing_leaders = true;
                                if (mem[leader_box].hh.u.B0 == 1 /*vlist_node */ )
                                    vlist_out();
                                else
                                    hlist_out();
                                doing_leaders = outer_doing_leaders;
                                dvi_v = save_v;
                                dvi_h = save_h;
                                cur_v = base_line;
                                cur_h = save_h + leader_wd + lx;
                            }
                            if (cur_dir == 1 /*right_to_left */ )
                                cur_h = edge;
                            else
                                cur_h = edge - 10;
                            goto lab15;
                        }
                    }
                    goto lab13;
                }
                break;
            case 40:
                {
                    cur_h = cur_h + mem[p + 1].cint;
                }
                break;
            case 11:
                {
                    synctex_kern(p, this_box);
                    cur_h = cur_h + mem[p + 1].cint;
                }
                break;
            case 9:
                {
                    synctex_math(p, this_box);
                    {
                        if ((eTeX_mode == 1)) { /*1504: */
                            if (odd(mem[p].hh.u.B1)) {

                                if (mem[LR_ptr].hh.v.LH == (4 /*L_code */  * (mem[p].hh.u.B1 / 4 /*L_code */ ) + 3)) {
                                    temp_ptr = LR_ptr;
                                    LR_ptr = mem[temp_ptr].hh.v.RH;
                                    {
                                        mem[temp_ptr].hh.v.RH = avail;
                                        avail = temp_ptr;
                                    }
                                } else {

                                    if (mem[p].hh.u.B1 > 4 /*L_code */ )
                                        LR_problems++;
                                }
                            } else {

                                {
                                    temp_ptr = get_avail();
                                    mem[temp_ptr].hh.v.LH = (4 /*L_code */  * (mem[p].hh.u.B1 / 4 /*L_code */ ) + 3);
                                    mem[temp_ptr].hh.v.RH = LR_ptr;
                                    LR_ptr = temp_ptr;
                                }
                                if ((mem[p].hh.u.B1 / 8 /*R_code */ ) != cur_dir) {       /*1509: */
                                    save_h = cur_h;
                                    temp_ptr = mem[p].hh.v.RH;
                                    rule_wd = mem[p + 1].cint;
                                    free_node(p, 3 /*medium_node_size */ );
                                    cur_dir = 1 - cur_dir;
                                    p = new_edge(cur_dir, rule_wd);
                                    mem[prev_p].hh.v.RH = p;
                                    cur_h = cur_h - left_edge + rule_wd;
                                    mem[p].hh.v.RH = reverse(this_box, new_edge(1 - cur_dir, 0), cur_g, cur_glue);
                                    mem[p + 2].cint = cur_h;
                                    cur_dir = 1 - cur_dir;
                                    cur_h = save_h;
                                    goto lab21;
                                }
                            }
                            mem[p].hh.u.B0 = 11 /*kern_node */ ;
                        }
                        cur_h = cur_h + mem[p + 1].cint;
                    }
                }
                break;
            case 6:
                {
                    mem[mem_top - 12] = mem[p + 1];
                    mem[mem_top - 12].hh.v.RH = mem[p].hh.v.RH;
                    p = mem_top - 12;
                    xtx_ligature_present = true;
                    goto lab21;
                }
                break;
            case 14:
                {
                    cur_h = cur_h + mem[p + 1].cint;
                    left_edge = cur_h + mem[p + 2].cint;
                    cur_dir = mem[p].hh.u.B1;
                }
                break;
            default:
                ;
                break;
            }
            goto lab15;
 lab14:                        /*fin_rule *//*646: */ if ((rule_ht == -1073741824L))
                rule_ht = mem[this_box + 3].cint;
            if ((rule_dp == -1073741824L))
                rule_dp = mem[this_box + 2].cint;
            rule_ht = rule_ht + rule_dp;
            if ((rule_ht > 0) && (rule_wd > 0)) {
                if (cur_h != dvi_h) {
                    movement(cur_h - dvi_h, 143 /*right1 */ );
                    dvi_h = cur_h;
                }
                cur_v = base_line + rule_dp;
                if (cur_v != dvi_v) {
                    movement(cur_v - dvi_v, 157 /*down1 */ );
                    dvi_v = cur_v;
                }
                {
                    dvi_buf[dvi_ptr] = 132 /*set_rule */ ;
                    dvi_ptr++;
                    if (dvi_ptr == dvi_limit)
                        dvi_swap();
                }
                dvi_four(rule_ht);
                dvi_four(rule_wd);
                cur_v = base_line;
                dvi_h = dvi_h + rule_wd;
            }
 lab13:                        /*move_past */  {

                cur_h = cur_h + rule_wd;
                synctex_horizontal_rule_or_glue(p, this_box);
            }
 lab15:                        /*next_p */ prev_p = p;
            p = mem[p].hh.v.RH;
        }
    synctex_tsilh(this_box);
    if ((eTeX_mode == 1)) {
        {
            while (mem[LR_ptr].hh.v.LH != 0 /*before */ ) {

                if (mem[LR_ptr].hh.v.LH > 4 /*L_code */ )
                    LR_problems = LR_problems + 10000;
                {
                    temp_ptr = LR_ptr;
                    LR_ptr = mem[temp_ptr].hh.v.RH;
                    {
                        mem[temp_ptr].hh.v.RH = avail;
                        avail = temp_ptr;
                    }
                }
            }
            {
                temp_ptr = LR_ptr;
                LR_ptr = mem[temp_ptr].hh.v.RH;
                {
                    mem[temp_ptr].hh.v.RH = avail;
                    avail = temp_ptr;
                }
            }
        }
        if ((mem[this_box].hh.u.B1) == 2 /*dlist */ )
            cur_dir = 1 /*right_to_left */ ;
    }
    prune_movements(save_loc);
    if (cur_s > 0)
        dvi_pop(save_loc);
    cur_s--;
}

void vlist_out(void)
{
    vlist_out_regmem scaled left_edge;
    scaled top_edge;
    scaled save_h, save_v;
    halfword this_box;
    glue_ord g_order;
    unsigned char /*shrinking */ g_sign;
    halfword p;
    integer save_loc;
    halfword leader_box;
    scaled leader_ht;
    scaled lx;
    boolean outer_doing_leaders;
    scaled edge;
    double glue_temp;
    double cur_glue;
    scaled cur_g;
    boolean upwards;
    cur_g = 0;
    cur_glue = 0.0;
    this_box = temp_ptr;
    g_order = mem[this_box + 5].hh.u.B1;
    g_sign = mem[this_box + 5].hh.u.B0;
    p = mem[this_box + 5].hh.v.RH;
    upwards = (mem[this_box].hh.u.B1 == 1 /*min_quarterword 1 */ );
    cur_s++;
    if (cur_s > 0) {
        dvi_buf[dvi_ptr] = 141 /*push */ ;
        dvi_ptr++;
        if (dvi_ptr == dvi_limit)
            dvi_swap();
    }
    if (cur_s > max_push)
        max_push = cur_s;
    save_loc = dvi_offset + dvi_ptr;
    left_edge = cur_h;
    synctex_vlist(this_box);
    if (upwards)
        cur_v = cur_v + mem[this_box + 2].cint;
    else
        cur_v = cur_v - mem[this_box + 3].cint;
    top_edge = cur_v;
    while (p != -268435455L) {  /*652: */

        if ((p >= hi_mem_min))
            confusion(66190L /*"vlistout" */ );
        else {                  /*653: */

            switch (mem[p].hh.u.B0) {
            case 0:
            case 1:
                if (mem[p + 5].hh.v.RH == -268435455L) {
                    if (upwards)
                        cur_v = cur_v - mem[p + 2].cint;
                    else
                        cur_v = cur_v + mem[p + 3].cint;
                    if (mem[p].hh.u.B0 == 1 /*vlist_node */ ) {
                        synctex_void_vlist(p, this_box);
                    } else {

                        synctex_void_hlist(p, this_box);
                    }
                    if (upwards)
                        cur_v = cur_v - mem[p + 3].cint;
                    else
                        cur_v = cur_v + mem[p + 2].cint;
                } else {

                    if (upwards)
                        cur_v = cur_v - mem[p + 2].cint;
                    else
                        cur_v = cur_v + mem[p + 3].cint;
                    if (cur_v != dvi_v) {
                        movement(cur_v - dvi_v, 157 /*down1 */ );
                        dvi_v = cur_v;
                    }
                    save_h = dvi_h;
                    save_v = dvi_v;
                    if (cur_dir == 1 /*right_to_left */ )
                        cur_h = left_edge - mem[p + 4].cint;
                    else
                        cur_h = left_edge + mem[p + 4].cint;
                    temp_ptr = p;
                    if (mem[p].hh.u.B0 == 1 /*vlist_node */ )
                        vlist_out();
                    else
                        hlist_out();
                    dvi_h = save_h;
                    dvi_v = save_v;
                    if (upwards)
                        cur_v = save_v - mem[p + 3].cint;
                    else
                        cur_v = save_v + mem[p + 2].cint;
                    cur_h = left_edge;
                }
                break;
            case 2:
                {
                    rule_ht = mem[p + 3].cint;
                    rule_dp = mem[p + 2].cint;
                    rule_wd = mem[p + 1].cint;
                    goto lab14;
                }
                break;
            case 8:
                {
                    switch (mem[p].hh.u.B1) {
                    case 42:
                        {
                            cur_v = cur_v + mem[p + 3].cint;
                            cur_h = left_edge;
                            if (cur_h != dvi_h) {
                                movement(cur_h - dvi_h, 143 /*right1 */ );
                                dvi_h = cur_h;
                            }
                            if (cur_v != dvi_v) {
                                movement(cur_v - dvi_v, 157 /*down1 */ );
                                dvi_v = cur_v;
                            }
                            f = mem[p + 4].qqqq.u.B1;
                            if (f != dvi_f) {   /*643: */
                                if (!font_used[f]) {
                                    dvi_font_def(f);
                                    font_used[f] = true;
                                }
                                if (f <= 64) {
                                    dvi_buf[dvi_ptr] = f + 170;
                                    dvi_ptr++;
                                    if (dvi_ptr == dvi_limit)
                                        dvi_swap();
                                } else if (f <= 256) {
                                    {
                                        dvi_buf[dvi_ptr] = 235 /*fnt1 */ ;
                                        dvi_ptr++;
                                        if (dvi_ptr == dvi_limit)
                                            dvi_swap();
                                    }
                                    {
                                        dvi_buf[dvi_ptr] = f - 1;
                                        dvi_ptr++;
                                        if (dvi_ptr == dvi_limit)
                                            dvi_swap();
                                    }
                                } else {

                                    {
                                        dvi_buf[dvi_ptr] = 236 /*fnt1 1 */ ;
                                        dvi_ptr++;
                                        if (dvi_ptr == dvi_limit)
                                            dvi_swap();
                                    }
                                    {
                                        dvi_buf[dvi_ptr] = (f - 1) / 256;
                                        dvi_ptr++;
                                        if (dvi_ptr == dvi_limit)
                                            dvi_swap();
                                    }
                                    {
                                        dvi_buf[dvi_ptr] = (f - 1) % 256;
                                        dvi_ptr++;
                                        if (dvi_ptr == dvi_limit)
                                            dvi_swap();
                                    }
                                }
                                dvi_f = f;
                            }
                            {
                                dvi_buf[dvi_ptr] = 253 /*set_glyphs */ ;
                                dvi_ptr++;
                                if (dvi_ptr == dvi_limit)
                                    dvi_swap();
                            }
                            dvi_four(0);
                            dvi_two(1);
                            dvi_four(0);
                            dvi_four(0);
                            dvi_two(mem[p + 4].qqqq.u.B2);
                            cur_v = cur_v + mem[p + 2].cint;
                            cur_h = left_edge;
                        }
                        break;
                    case 43:
                    case 44:
                        {
                            save_h = dvi_h;
                            save_v = dvi_v;
                            cur_v = cur_v + mem[p + 3].cint;
                            pic_out(p);
                            dvi_h = save_h;
                            dvi_v = save_v;
                            cur_v = save_v + mem[p + 2].cint;
                            cur_h = left_edge;
                        }
                        break;
                    case 6:
                        {
                            pdf_last_x_pos = cur_h + cur_h_offset;
                            pdf_last_y_pos = cur_page_height - cur_v - cur_v_offset;
                        }
                        break;
                    default:
                        out_what(p);
                        break;
                    }
                }
                break;
            case 10:
                {
                    g = mem[p + 1].hh.v.LH;
                    rule_ht = mem[g + 1].cint - cur_g;
                    if (g_sign != 0 /*normal */ ) {
                        if (g_sign == 1 /*stretching */ ) {
                            if (mem[g].hh.u.B0 == g_order) {
                                cur_glue = cur_glue + mem[g + 2].cint;
                                glue_temp = mem[this_box + 6].gr * cur_glue;
                                if (glue_temp > 1000000000.0)
                                    glue_temp = 1000000000.0;
                                else if (glue_temp < -1000000000.0)
                                    glue_temp = -1000000000.0;
                                cur_g = round(glue_temp);
                            }
                        } else if (mem[g].hh.u.B1 == g_order) {
                            cur_glue = cur_glue - mem[g + 3].cint;
                            glue_temp = mem[this_box + 6].gr * cur_glue;
                            if (glue_temp > 1000000000.0)
                                glue_temp = 1000000000.0;
                            else if (glue_temp < -1000000000.0)
                                glue_temp = -1000000000.0;
                            cur_g = round(glue_temp);
                        }
                    }
                    rule_ht = rule_ht + cur_g;
                    if (mem[p].hh.u.B1 >= 100 /*a_leaders */ ) {  /*657: */
                        leader_box = mem[p + 1].hh.v.RH;
                        if (mem[leader_box].hh.u.B0 == 2 /*rule_node */ ) {
                            rule_wd = mem[leader_box + 1].cint;
                            rule_dp = 0;
                            goto lab14;
                        }
                        leader_ht = mem[leader_box + 3].cint + mem[leader_box + 2].cint;
                        if ((leader_ht > 0) && (rule_ht > 0)) {
                            rule_ht = rule_ht + 10;
                            edge = cur_v + rule_ht;
                            lx = 0;
                            if (mem[p].hh.u.B1 == 100 /*a_leaders */ ) {
                                save_v = cur_v;
                                cur_v = top_edge + leader_ht * ((cur_v - top_edge) / leader_ht);
                                if (cur_v < save_v)
                                    cur_v = cur_v + leader_ht;
                            } else {

                                lq = rule_ht / leader_ht;
                                lr = rule_ht % leader_ht;
                                if (mem[p].hh.u.B1 == 101 /*c_leaders */ )
                                    cur_v = cur_v + (lr / 2);
                                else {

                                    lx = lr / (lq + 1);
                                    cur_v = cur_v + ((lr - (lq - 1) * lx) / 2);
                                }
                            }
                            while (cur_v + leader_ht <= edge) { /*659: */

                                if (cur_dir == 1 /*right_to_left */ )
                                    cur_h = left_edge - mem[leader_box + 4].cint;
                                else
                                    cur_h = left_edge + mem[leader_box + 4].cint;
                                if (cur_h != dvi_h) {
                                    movement(cur_h - dvi_h, 143 /*right1 */ );
                                    dvi_h = cur_h;
                                }
                                save_h = dvi_h;
                                cur_v = cur_v + mem[leader_box + 3].cint;
                                if (cur_v != dvi_v) {
                                    movement(cur_v - dvi_v, 157 /*down1 */ );
                                    dvi_v = cur_v;
                                }
                                save_v = dvi_v;
                                temp_ptr = leader_box;
                                outer_doing_leaders = doing_leaders;
                                doing_leaders = true;
                                if (mem[leader_box].hh.u.B0 == 1 /*vlist_node */ )
                                    vlist_out();
                                else
                                    hlist_out();
                                doing_leaders = outer_doing_leaders;
                                dvi_v = save_v;
                                dvi_h = save_h;
                                cur_h = left_edge;
                                cur_v = save_v - mem[leader_box + 3].cint + leader_ht + lx;
                            }
                            cur_v = edge - 10;
                            goto lab15;
                        }
                    }
                    goto lab13;
                }
                break;
            case 11:
                if (upwards)
                    cur_v = cur_v - mem[p + 1].cint;
                else
                    cur_v = cur_v + mem[p + 1].cint;
                break;
            default:
                ;
                break;
            }
            goto lab15;
 lab14:                        /*fin_rule *//*655: */ if ((rule_wd == -1073741824L))
                rule_wd = mem[this_box + 1].cint;
            rule_ht = rule_ht + rule_dp;
            if (upwards)
                cur_v = cur_v - rule_ht;
            else
                cur_v = cur_v + rule_ht;
            if ((rule_ht > 0) && (rule_wd > 0)) {
                if (cur_dir == 1 /*right_to_left */ )
                    cur_h = cur_h - rule_wd;
                if (cur_h != dvi_h) {
                    movement(cur_h - dvi_h, 143 /*right1 */ );
                    dvi_h = cur_h;
                }
                if (cur_v != dvi_v) {
                    movement(cur_v - dvi_v, 157 /*down1 */ );
                    dvi_v = cur_v;
                }
                {
                    dvi_buf[dvi_ptr] = 137 /*put_rule */ ;
                    dvi_ptr++;
                    if (dvi_ptr == dvi_limit)
                        dvi_swap();
                }
                dvi_four(rule_ht);
                dvi_four(rule_wd);
                cur_h = left_edge;
            }
            goto lab15;
 lab13:                        /*move_past */ if (upwards)
                cur_v = cur_v - rule_ht;
            else
                cur_v = cur_v + rule_ht;
        }
 lab15:                        /*next_p */ p = mem[p].hh.v.RH;
    }
    synctex_tsilv(this_box);
    prune_movements(save_loc);
    if (cur_s > 0)
        dvi_pop(save_loc);
    cur_s--;
}

void zship_out(halfword p)
{
    ship_out_regmem integer page_loc;
    unsigned char j, k;
    pool_pointer s;
    unsigned char /*max_selector */ old_setting;
    synctex_sheet(eqtb[8938757L /*int_base 17 */ ].cint);
    {
        if (job_name == 0)
            open_log_file();
        if (eqtb[8938774L /*int_base 34 */ ].cint > 0) {
            print_nl(65622L /*"" */ );
            print_ln();
            print(66191L /*"Completed box being shipped out" */ );
        }
        if (term_offset > max_print_line - 9)
            print_ln();
        else if ((term_offset > 0) || (file_offset > 0))
            print_char(32 /*" " */ );
        print_char(91 /*"[" */ );
        j = 9;
        while ((eqtb[8938824L /*count_base */  + j].cint == 0) && (j > 0))
            j--;
        {
            register integer for_end;
            k = 0;
            for_end = j;
            if (k <= for_end)
                do {
                    print_int(eqtb[8938824L /*count_base */  + k].cint);
                    if (k < j)
                        print_char(46 /*"." */ );
                }
                while (k++ < for_end);
        }
        fflush(stdout);
        if (eqtb[8938774L /*int_base 34 */ ].cint > 0) {
            print_char(93 /*"]" */ );
            begin_diagnostic();
            show_box(p);
            end_diagnostic(true);
        }
        if ((mem[p + 3].cint > 1073741823L) || (mem[p + 2].cint > 1073741823L)
            || (mem[p + 3].cint + mem[p + 2].cint + eqtb[10053211L /*dimen_base 19 */ ].cint > 1073741823L)
            || (mem[p + 1].cint + eqtb[10053210L /*dimen_base 18 */ ].cint > 1073741823L)) {
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66197L /*"Huge page cannot be shipped out" */ );
            }
            {
                help_ptr = 2;
                help_line[1] = 66198L /*"The page just created is more than 18 feet tall or" */ ;
                help_line[0] = 66199L /*"more than 18 feet wide, so I suspect something went wrong." */ ;
            }
            error();
            if (eqtb[8938774L /*int_base 34 */ ].cint <= 0) {
                begin_diagnostic();
                print_nl(66200L /*"The following box has been deleted:" */ );
                show_box(p);
                end_diagnostic(true);
            }
            goto lab30;
        }
        if (mem[p + 3].cint + mem[p + 2].cint + eqtb[10053211L /*dimen_base 19 */ ].cint > max_v)
            max_v = mem[p + 3].cint + mem[p + 2].cint + eqtb[10053211L /*dimen_base 19 */ ].cint;
        if (mem[p + 1].cint + eqtb[10053210L /*dimen_base 18 */ ].cint > max_h)
            max_h = mem[p + 1].cint + eqtb[10053210L /*dimen_base 18 */ ].cint /*:663 */ ;
        dvi_h = 0;
        dvi_v = 0;
        cur_h = eqtb[10053210L /*dimen_base 18 */ ].cint;
        dvi_f = 0 /*font_base */ ;
        cur_h_offset = eqtb[10053210L /*dimen_base 18 */ ].cint + (65536L * 7227) / ((double)100);
        cur_v_offset = eqtb[10053211L /*dimen_base 19 */ ].cint + (65536L * 7227) / ((double)100);
        if (eqtb[10053213L /*dimen_base 21 */ ].cint != 0)
            cur_page_width = eqtb[10053213L /*dimen_base 21 */ ].cint;
        else
            cur_page_width = mem[p + 1].cint + 2 * cur_h_offset;
        if (eqtb[10053214L /*dimen_base 22 */ ].cint != 0)
            cur_page_height = eqtb[10053214L /*dimen_base 22 */ ].cint;
        else
            cur_page_height = mem[p + 3].cint + mem[p + 2].cint + 2 * /*:1405 */ cur_v_offset;
        if (output_file_name == 0) {
            if (job_name == 0)
                open_log_file();
            pack_job_name(output_file_extension);
            while (!open_dvi_output(&dvi_file))
                prompt_file_name(66150L /*"file name for output" */ , output_file_extension);
            output_file_name = make_name_string();
        }
        if (total_pages == 0) {
            {
                dvi_buf[dvi_ptr] = 247 /*pre */ ;
                dvi_ptr++;
                if (dvi_ptr == dvi_limit)
                    dvi_swap();
            }
            {
                dvi_buf[dvi_ptr] = 7 /*id_byte */ ;
                dvi_ptr++;
                if (dvi_ptr == dvi_limit)
                    dvi_swap();
            }
            dvi_four(25400000L);
            dvi_four(473628672L);
            prepare_mag();
            dvi_four(eqtb[8938757L /*int_base 17 */ ].cint);
            if (output_comment) {
                l = strlen(output_comment);
                {
                    dvi_buf[dvi_ptr] = l;
                    dvi_ptr++;
                    if (dvi_ptr == dvi_limit)
                        dvi_swap();
                }
                {
                    register integer for_end;
                    s = 0;
                    for_end = l - 1;
                    if (s <= for_end)
                        do {
                            dvi_buf[dvi_ptr] = output_comment[s];
                            dvi_ptr++;
                            if (dvi_ptr == dvi_limit)
                                dvi_swap();
                        }
                        while (s++ < for_end);
                }
            } else {

                old_setting = selector;
                selector = SELECTOR_NEW_STRING ;
                print(66189L /*" XeTeX output " */ );
                print_int(eqtb[8938763L /*int_base 23 */ ].cint);
                print_char(46 /*"." */ );
                print_two(eqtb[8938762L /*int_base 22 */ ].cint);
                print_char(46 /*"." */ );
                print_two(eqtb[8938761L /*int_base 21 */ ].cint);
                print_char(58 /*":" */ );
                print_two(eqtb[8938760L /*int_base 20 */ ].cint / 60);
                print_two(eqtb[8938760L /*int_base 20 */ ].cint % 60);
                selector = old_setting;
                {
                    dvi_buf[dvi_ptr] = (pool_ptr - str_start[(str_ptr) - 65536L]);
                    dvi_ptr++;
                    if (dvi_ptr == dvi_limit)
                        dvi_swap();
                }
                {
                    register integer for_end;
                    s = str_start[(str_ptr) - 65536L];
                    for_end = pool_ptr - 1;
                    if (s <= for_end)
                        do {
                            dvi_buf[dvi_ptr] = str_pool[s];
                            dvi_ptr++;
                            if (dvi_ptr == dvi_limit)
                                dvi_swap();
                        }
                        while (s++ < for_end);
                }
                pool_ptr = str_start[(str_ptr) - 65536L];
            }
        }
        page_loc = dvi_offset + dvi_ptr;
        {
            dvi_buf[dvi_ptr] = 139 /*bop */ ;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        {
            register integer for_end;
            k = 0;
            for_end = 9;
            if (k <= for_end)
                do
                    dvi_four(eqtb[8938824L /*count_base */  + k].cint);
                while (k++ < for_end);
        }
        dvi_four(last_bop);
        last_bop = page_loc;
        old_setting = selector;
        selector = SELECTOR_NEW_STRING ;
        print(66195L /*"pdf:pagesize " */ );
        if ((eqtb[10053213L /*dimen_base 21 */ ].cint > 0) && (eqtb[10053214L /*dimen_base 22 */ ].cint > 0)) {
            print(66079L /*"width" */ );
            print(32 /*" " */ );
            print_scaled(eqtb[10053213L /*dimen_base 21 */ ].cint);
            print(65693L /*"pt" */ );
            print(32 /*" " */ );
            print(66080L /*"height" */ );
            print(32 /*" " */ );
            print_scaled(eqtb[10053214L /*dimen_base 22 */ ].cint);
            print(65693L /*"pt" */ );
        } else
            print(66196L /*"default" */ );
        selector = old_setting;
        {
            dvi_buf[dvi_ptr] = 239 /*xxx1 */ ;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        {
            dvi_buf[dvi_ptr] = (pool_ptr - str_start[(str_ptr) - 65536L]);
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        {
            register integer for_end;
            s = str_start[(str_ptr) - 65536L];
            for_end = pool_ptr - 1;
            if (s <= for_end)
                do {
                    dvi_buf[dvi_ptr] = str_pool[s];
                    dvi_ptr++;
                    if (dvi_ptr == dvi_limit)
                        dvi_swap();
                }
                while (s++ < for_end);
        }
        pool_ptr = str_start[(str_ptr) - 65536L];
        cur_v = mem[p + 3].cint + eqtb[10053211L /*dimen_base 19 */ ].cint;
        temp_ptr = p;
        if (mem[p].hh.u.B0 == 1 /*vlist_node */ )
            vlist_out();
        else
            hlist_out();
        {
            dvi_buf[dvi_ptr] = 140 /*eop */ ;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        total_pages++;
        cur_s = -1;
        if (!no_pdf_output)
            fflush(dvi_file);

 lab30:                        /*done *//*:662 */ ;
        if ((eTeX_mode == 1)) { /*1518: */
            if (LR_problems > 0) {
                {
                    print_ln();
                    print_nl(66895L /*"\endL or \endR problem (" */ );
                    print_int(LR_problems / 10000);
                    print(66896L /*" missing, " */ );
                    print_int(LR_problems % 10000);
                    print(66897L /*" extra" */ );
                    LR_problems = 0;
                }
                print_char(41 /*")" */ );
                print_ln();
            }
            if ((LR_ptr != -268435455L) || (cur_dir != 0 /*left_to_right */ ))
                confusion(66899L /*"LR3" */ );
        }
        if (eqtb[8938774L /*int_base 34 */ ].cint <= 0)
            print_char(93 /*"]" */ );
        dead_cycles = 0;
        fflush(stdout);
        flush_node_list(p);
    }
    synctex_teehs();
}

void zscan_spec(group_code c, boolean three_codes)
{
    scan_spec_regmem integer s;
    unsigned char /*additional */ spec_code;
    if (three_codes)
        s = save_stack[save_ptr + 0].cint;
    if (scan_keyword(66213L /*"to" */ ))
        spec_code = 0 /*exactly */ ;
    else if (scan_keyword(66214L /*"spread" */ ))
        spec_code = 1 /*additional */ ;
    else {

        spec_code = 1 /*additional */ ;
        cur_val = 0;
        goto lab40;
    }
    scan_dimen(false, false, false);
 lab40:                        /*found */ if (three_codes) {
        save_stack[save_ptr + 0].cint = s;
        save_ptr++;
    }
    save_stack[save_ptr + 0].cint = spec_code;
    save_stack[save_ptr + 1].cint = cur_val;
    save_ptr = save_ptr + 2;
    new_save_level(c);
    scan_left_brace();
}

scaled zchar_pw(halfword p, small_number side)
{
    register scaled Result;
    char_pw_regmem internal_font_number f;
    integer c;
    Result = 0;
    if (side == 0)
        last_leftmost_char = -268435455L;
    else
        last_rightmost_char = -268435455L;
    if (p == -268435455L)
        return Result;
    if ((((p) != -268435455L && (!(p >= hi_mem_min)) && (mem[p].hh.u.B0 == 8 /*whatsit_node */ )
          && ((mem[p].hh.u.B1 == 40 /*native_word_node */ ) || (mem[p].hh.u.B1 == 41 /*native_word_node_AT */ ))))) {
        if (mem[p + 5].ptr != NULL) {
            f = mem[p + 4].qqqq.u.B1;
            Result =
                round_xn_over_d(font_info[6 /*quad_code */  + param_base[f]].cint, get_native_word_cp(p, side), 1000);
        }
        return Result;
    }
    if ((((p) != -268435455L && (!(p >= hi_mem_min)) && (mem[p].hh.u.B0 == 8 /*whatsit_node */ )
          && (mem[p].hh.u.B1 == 42 /*glyph_node */ )))) {
        f = mem[p + 4].qqqq.u.B1;
        Result =
            round_xn_over_d(font_info[6 /*quad_code */  + param_base[f]].cint, get_cp_code(f, mem[p + 4].qqqq.u.B2, side),
                            1000);
        return Result;
    }
    if (!(p >= hi_mem_min)) {
        if (mem[p].hh.u.B0 == 6 /*ligature_node */ )
            p = p + 1;
        else
            return Result;
    }
    f = mem[p].hh.u.B0;
    c = get_cp_code(f, mem[p].hh.u.B1, side);
    switch (side) {
    case 0:
        last_leftmost_char = p;
        break;
    case 1:
        last_rightmost_char = p;
        break;
    }
    if (c == 0)
        return Result;
    Result = round_xn_over_d(font_info[6 /*quad_code */  + param_base[f]].cint, c, 1000);
    return Result;
}

halfword znew_margin_kern(scaled w, halfword p, small_number side)
{
    register halfword Result;
    new_margin_kern_regmem halfword k;
    k = get_node(3 /*margin_kern_node_size */ );
    mem[k].hh.u.B0 = 40 /*margin_kern_node */ ;
    mem[k].hh.u.B1 = side;
    mem[k + 1].cint = w;
    Result = k;
    return Result;
}

halfword zhpack(halfword p, scaled w, small_number m)
{
    register halfword Result;
    hpack_regmem halfword r;
    halfword q;
    scaled h, d, x;
    scaled s;
    halfword g;
    glue_ord o;
    internal_font_number f;
    four_quarters i;
    eight_bits hd;
    halfword pp, ppp;
    integer total_chars, k;
    last_badness = 0;
    r = get_node(8 /*box_node_size */ );
    mem[r].hh.u.B0 = 0 /*hlist_node */ ;
    mem[r].hh.u.B1 = 0 /*min_quarterword */ ;
    mem[r + 4].cint = 0;
    q = r + 5;
    mem[q].hh.v.RH = p;
    h = 0;
    d = 0;
    x = 0;
    total_stretch[0 /*normal */ ] = 0;
    total_shrink[0 /*normal */ ] = 0;
    total_stretch[1 /*fil */ ] = 0;
    total_shrink[1 /*fil */ ] = 0;
    total_stretch[2 /*fill */ ] = 0;
    total_shrink[2 /*fill */ ] = 0;
    total_stretch[3 /*filll */ ] = 0;
    total_shrink[3 /*filll */ ] = 0 /*:673 */ ;
    if ((eqtb[8938811L /*eTeX_state_base 0 */ ].cint > 0)) {    /*1497: */
        temp_ptr = get_avail();
        mem[temp_ptr].hh.v.LH = 0 /*before */ ;
        mem[temp_ptr].hh.v.RH = LR_ptr;
        LR_ptr = temp_ptr;
    }
    while (p != -268435455L) {  /*674: */

 lab21:                                                /*reswitch */ while ((p >= hi_mem_min)) {
                                                        /*677: */

            f = mem[p].hh.u.B0;
            i = font_info[char_base[f] + effective_char(true, f, mem[p].hh.u.B1)].qqqq;
            hd = i.u.B1;
            x = x + font_info[width_base[f] + i.u.B0].cint;
            s = font_info[height_base[f] + (hd) / 16].cint;
            if (s > h)
                h = s;
            s = font_info[depth_base[f] + (hd) % 16].cint;
            if (s > d)
                d = s;
            p = mem[p].hh.v.RH;
        }
        if (p != -268435455L) {
            switch (mem[p].hh.u.B0) {
            case 0:
            case 1:
            case 2:
            case 13:
                {
                    x = x + mem[p + 1].cint;
                    if (mem[p].hh.u.B0 >= 2 /*rule_node */ )
                        s = 0;
                    else
                        s = mem[p + 4].cint;
                    if (mem[p + 3].cint - s > h)
                        h = mem[p + 3].cint - s;
                    if (mem[p + 2].cint + s > d)
                        d = mem[p + 2].cint + s;
                }
                break;
            case 3:
            case 4:
            case 5:
                if ((adjust_tail != -268435455L) || (pre_adjust_tail != -268435455L)) { /*680: */
                    while (mem[q].hh.v.RH != p)
                        q = mem[q].hh.v.RH;
                    if (mem[p].hh.u.B0 == 5 /*adjust_node */ ) {
                        if (mem[p].hh.u.B1 != 0) {
                            if (pre_adjust_tail == -268435455L)
                                confusion(66215L /*"pre vadjust" */ );
                            mem[pre_adjust_tail].hh.v.RH = mem[p + 1].cint;
                            while (mem[pre_adjust_tail].hh.v.RH != -268435455L)
                                pre_adjust_tail = mem[pre_adjust_tail].hh.v.RH;
                        } else {

                            if (adjust_tail == -268435455L)
                                confusion(66215L /*"pre vadjust" */ );
                            mem[adjust_tail].hh.v.RH = mem[p + 1].cint;
                            while (mem[adjust_tail].hh.v.RH != -268435455L)
                                adjust_tail = mem[adjust_tail].hh.v.RH;
                        }
                        p = mem[p].hh.v.RH;
                        free_node(mem[q].hh.v.RH, 2 /*small_node_size */ );
                    } else {

                        mem[adjust_tail].hh.v.RH = p;
                        adjust_tail = p;
                        p = mem[p].hh.v.RH;
                    }
                    mem[q].hh.v.RH = p;
                    p = q;
                }
                break;
            case 8:
                {
                    switch (mem[p].hh.u.B1) {
                    case 40:
                    case 41:
                        {
                            if ((q != r + 5) && (mem[q].hh.u.B0 == 7 /*disc_node */ ))
                                k = mem[q].hh.u.B1;
                            else
                                k = 0;
                            while ((mem[q].hh.v.RH != p)) {

                                k--;
                                q = mem[q].hh.v.RH;
                                if (mem[q].hh.u.B0 == 7 /*disc_node */ )
                                    k = mem[q].hh.u.B1;
                            }
                            pp = mem[p].hh.v.RH;
 lab20:                        /*restart */ if ((k <= 0) && (pp != -268435455L) && (!(pp >= hi_mem_min))) {
                                if ((mem[pp].hh.u.B0 == 8 /*whatsit_node */ )
                                    && ((mem[pp].hh.u.B1 == 40 /*native_word_node */ )
                                        || (mem[pp].hh.u.B1 == 41 /*native_word_node_AT */ ))
                                    && (mem[pp + 4].qqqq.u.B1 == mem[p + 4].qqqq.u.B1)) {
                                    pp = mem[pp].hh.v.RH;
                                    goto lab20;
                                } else if ((mem[pp].hh.u.B0 == 7 /*disc_node */ )) {
                                    ppp = mem[pp].hh.v.RH;
                                    if ((((ppp) != -268435455L && (!(ppp >= hi_mem_min))
                                          && (mem[ppp].hh.u.B0 == 8 /*whatsit_node */ )
                                          && ((mem[ppp].hh.u.B1 == 40 /*native_word_node */ )
                                              || (mem[ppp].hh.u.B1 == 41 /*native_word_node_AT */ ))))
                                        && (mem[ppp + 4].qqqq.u.B1 == mem[p + 4].qqqq.u.B1)) {
                                        pp = mem[ppp].hh.v.RH;
                                        goto lab20;
                                    }
                                }
                            }
                            if ((pp != mem[p].hh.v.RH)) {
                                total_chars = 0;
                                p = mem[q].hh.v.RH;
                                while ((p != pp)) {

                                    if ((mem[p].hh.u.B0 == 8 /*whatsit_node */ ))
                                        total_chars = total_chars + mem[p + 4].qqqq.u.B2;
                                    ppp = p;
                                    p = mem[p].hh.v.RH;
                                }
                                p = mem[q].hh.v.RH;
                                pp = new_native_word_node(mem[p + 4].qqqq.u.B1, total_chars);
                                mem[pp].hh.u.B1 = mem[p].hh.u.B1;
                                mem[q].hh.v.RH = pp;
                                mem[pp].hh.v.RH = mem[ppp].hh.v.RH;
                                mem[ppp].hh.v.RH = -268435455L;
                                total_chars = 0;
                                ppp = p;
                                do {
                                    if ((mem[ppp].hh.u.B0 == 8 /*whatsit_node */ )) {
                                        register integer for_end;
                                        k = 0;
                                        for_end = mem[ppp + 4].qqqq.u.B2 - 1;
                                        if (k <= for_end)
                                            do {
                                                set_native_char(pp, total_chars, get_native_char(ppp, k));
                                                total_chars++;
                                            }
                                            while (k++ < for_end);
                                    }
                                    ppp = mem[ppp].hh.v.RH;
                                } while (!((ppp == -268435455L)));
                                flush_node_list(p);
                                p = mem[q].hh.v.RH;
                                set_native_metrics(p, (eqtb[8938814L /*eTeX_state_base 3 */ ].cint > 0));
                            }
                            if (mem[p + 3].cint > h)
                                h = mem[p + 3].cint;
                            if (mem[p + 2].cint > d)
                                d = mem[p + 2].cint;
                            x = x + mem[p + 1].cint;
                        }
                        break;
                    case 42:
                    case 43:
                    case 44:
                        {
                            if (mem[p + 3].cint > h)
                                h = mem[p + 3].cint;
                            if (mem[p + 2].cint > d)
                                d = mem[p + 2].cint;
                            x = x + mem[p + 1].cint;
                        }
                        break;
                    default:
                        ;
                        break;
                    }
                }
                break;
            case 10:
                {
                    g = mem[p + 1].hh.v.LH;
                    x = x + mem[g + 1].cint;
                    o = mem[g].hh.u.B0;
                    total_stretch[o] = total_stretch[o] + mem[g + 2].cint;
                    o = mem[g].hh.u.B1;
                    total_shrink[o] = total_shrink[o] + mem[g + 3].cint;
                    if (mem[p].hh.u.B1 >= 100 /*a_leaders */ ) {
                        g = mem[p + 1].hh.v.RH;
                        if (mem[g + 3].cint > h)
                            h = mem[g + 3].cint;
                        if (mem[g + 2].cint > d)
                            d = mem[g + 2].cint;
                    }
                }
                break;
            case 11:
                x = x + mem[p + 1].cint;
                break;
            case 40:
                x = x + mem[p + 1].cint;
                break;
            case 9:
                {
                    x = x + mem[p + 1].cint;
                    if ((eqtb[8938811L /*eTeX_state_base 0 */ ].cint > 0)) {    /*1498: */

                        if (odd(mem[p].hh.u.B1)) {

                            if (mem[LR_ptr].hh.v.LH == (4 /*L_code */  * (mem[p].hh.u.B1 / 4 /*L_code */ ) + 3)) {
                                temp_ptr = LR_ptr;
                                LR_ptr = mem[temp_ptr].hh.v.RH;
                                {
                                    mem[temp_ptr].hh.v.RH = avail;
                                    avail = temp_ptr;
                                }
                            } else {

                                LR_problems++;
                                mem[p].hh.u.B0 = 11 /*kern_node */ ;
                                mem[p].hh.u.B1 = 1 /*explicit */ ;
                            }
                        } else {

                            temp_ptr = get_avail();
                            mem[temp_ptr].hh.v.LH = (4 /*L_code */  * (mem[p].hh.u.B1 / 4 /*L_code */ ) + 3);
                            mem[temp_ptr].hh.v.RH = LR_ptr;
                            LR_ptr = temp_ptr;
                        }
                    }
                }
                break;
            case 6:
                {
                    mem[mem_top - 12] = mem[p + 1];
                    mem[mem_top - 12].hh.v.RH = mem[p].hh.v.RH;
                    p = mem_top - 12;
                    xtx_ligature_present = true;
                    goto lab21;
                }
                break;
            default:
                ;
                break;
            }
            p = mem[p].hh.v.RH;
        }
    }
    if (adjust_tail != -268435455L)
        mem[adjust_tail].hh.v.RH = -268435455L;
    if (pre_adjust_tail != -268435455L)
        mem[pre_adjust_tail].hh.v.RH = -268435455L;
    mem[r + 3].cint = h;
    mem[r + 2].cint = d;
    if (m == 1 /*additional */ )
        w = x + w;
    mem[r + 1].cint = w;
    x = w - x;
    if (x == 0) {
        mem[r + 5].hh.u.B0 = 0 /*normal */ ;
        mem[r + 5].hh.u.B1 = 0 /*normal */ ;
        mem[r + 6].gr = 0.0;
        goto exit;
    } else if (x > 0) {         /*683: */
        if (total_stretch[3 /*filll */ ] != 0)
            o = 3 /*filll */ ;
        else if (total_stretch[2 /*fill */ ] != 0)
            o = 2 /*fill */ ;
        else if (total_stretch[1 /*fil */ ] != 0)
            o = 1 /*fil */ ;
        else
            o = 0 /*normal *//*:684 */ ;
        mem[r + 5].hh.u.B1 = o;
        mem[r + 5].hh.u.B0 = 1 /*stretching */ ;
        if (total_stretch[o] != 0)
            mem[r + 6].gr = x / ((double)total_stretch[o]);
        else {

            mem[r + 5].hh.u.B0 = 0 /*normal */ ;
            mem[r + 6].gr = 0.0;
        }
        if (o == 0 /*normal */ ) {

            if (mem[r + 5].hh.v.RH != -268435455L) {    /*685: */
                last_badness = badness(x, total_stretch[0 /*normal */ ]);
                if (last_badness > eqtb[8938766L /*int_base 26 */ ].cint) {
                    print_ln();
                    if (last_badness > 100)
                        print_nl(66216L /*"Underfull" */ );
                    else
                        print_nl(66217L /*"Loose" */ );
                    print(66218L /*" \hbox (badness " */ );
                    print_int(last_badness);
                    goto lab50;
                }
            }
        }
        goto exit;
    } else {                    /*689: */

        if (total_shrink[3 /*filll */ ] != 0)
            o = 3 /*filll */ ;
        else if (total_shrink[2 /*fill */ ] != 0)
            o = 2 /*fill */ ;
        else if (total_shrink[1 /*fil */ ] != 0)
            o = 1 /*fil */ ;
        else
            o = 0 /*normal *//*:690 */ ;
        mem[r + 5].hh.u.B1 = o;
        mem[r + 5].hh.u.B0 = 2 /*shrinking */ ;
        if (total_shrink[o] != 0)
            mem[r + 6].gr = (-(integer) x) / ((double)total_shrink[o]);
        else {

            mem[r + 5].hh.u.B0 = 0 /*normal */ ;
            mem[r + 6].gr = 0.0;
        }
        if ((total_shrink[o] < -(integer) x) && (o == 0 /*normal */ ) && (mem[r + 5].hh.v.RH != -268435455L)) {
            last_badness = 1000000L;
            mem[r + 6].gr = 1.0;
            if ((-(integer) x - total_shrink[0 /*normal */ ] > eqtb[10053200L /*dimen_base 8 */ ].cint)
                || (eqtb[8938766L /*int_base 26 */ ].cint < 100)) {
                if ((eqtb[10053208L /*dimen_base 16 */ ].cint > 0)
                    && (-(integer) x - total_shrink[0 /*normal */ ] > eqtb[10053200L /*dimen_base 8 */ ].cint)) {
                    while (mem[q].hh.v.RH != -268435455L)
                        q = mem[q].hh.v.RH;
                    mem[q].hh.v.RH = new_rule();
                    mem[mem[q].hh.v.RH + 1].cint = eqtb[10053208L /*dimen_base 16 */ ].cint;
                }
                print_ln();
                print_nl(66224L /*"Overfull \hbox (" */ );
                print_scaled(-(integer) x - total_shrink[0 /*normal */ ]);
                print(66225L /*"pt too wide" */ );
                goto lab50;
            }
        } else if (o == 0 /*normal */ ) {

            if (mem[r + 5].hh.v.RH != -268435455L) {    /*692: */
                last_badness = badness(-(integer) x, total_shrink[0 /*normal */ ]);
                if (last_badness > eqtb[8938766L /*int_base 26 */ ].cint) {
                    print_ln();
                    print_nl(66226L /*"Tight \hbox (badness " */ );
                    print_int(last_badness);
                    goto lab50;
                }
            }
        }
        goto exit;
    }
 lab50:                        /*common_ending *//*688: */ if (output_active)
        print(66219L /*") has occurred while \output is active" */ );
    else {

        if (pack_begin_line != 0) {
            if (pack_begin_line > 0)
                print(66220L /*") in paragraph at lines " */ );
            else
                print(66221L /*") in alignment at lines " */ );
            print_int(abs(pack_begin_line));
            print(66222L /*"--" */ );
        } else
            print(66223L /*") detected at line " */ );
        print_int(line);
    }
    print_ln();
    font_in_short_display = 0 /*font_base */ ;
    short_display(mem[r + 5].hh.v.RH);
    print_ln();
    begin_diagnostic();
    show_box(r);
    end_diagnostic(true);

exit:
    if ((eqtb[8938811L /*eTeX_state_base 0 */ ].cint > 0)) {
	/*1499: */
        if (mem[LR_ptr].hh.v.LH != 0 /*before */ ) {
            while (mem[q].hh.v.RH != -268435455L)
                q = mem[q].hh.v.RH;
            do {
                temp_ptr = q;
                q = new_math(0, mem[LR_ptr].hh.v.LH);
                mem[temp_ptr].hh.v.RH = q;
                LR_problems = LR_problems + 10000;
                {
                    temp_ptr = LR_ptr;
                    LR_ptr = mem[temp_ptr].hh.v.RH;
                    {
                        mem[temp_ptr].hh.v.RH = avail;
                        avail = temp_ptr;
                    }
                }
            } while (!(mem[LR_ptr].hh.v.LH == 0 /*before */ ));
        }
        if (LR_problems > 0) {
            {
                print_ln();
                print_nl(66895L /*"\endL or \endR problem (" */ );
                print_int(LR_problems / 10000);
                print(66896L /*" missing, " */ );
                print_int(LR_problems % 10000);
                print(66897L /*" extra" */ );
                LR_problems = 0;
            }
            goto lab50;
        }
        {
            temp_ptr = LR_ptr;
            LR_ptr = mem[temp_ptr].hh.v.RH;
            {
                mem[temp_ptr].hh.v.RH = avail;
                avail = temp_ptr;
            }
        }
        if (LR_ptr != -268435455L)
            confusion(66894L /*"LR1" */ );
    }
    Result = r;
    return Result;
}

halfword zvpackage(halfword p, scaled h, small_number m, scaled l)
{
    register halfword Result;
    vpackage_regmem halfword r;
    scaled w, d, x;
    scaled s;
    halfword g;
    glue_ord o;
    last_badness = 0;
    r = get_node(8 /*box_node_size */ );
    mem[r].hh.u.B0 = 1 /*vlist_node */ ;
    if ((eqtb[8938813L /*eTeX_state_base 2 */ ].cint > 0))
        mem[r].hh.u.B1 = 1 /*min_quarterword 1 */ ;
    else
        mem[r].hh.u.B1 = 0 /*min_quarterword */ ;
    mem[r + 4].cint = 0;
    mem[r + 5].hh.v.RH = p;
    w = 0;
    d = 0;
    x = 0;
    total_stretch[0 /*normal */ ] = 0;
    total_shrink[0 /*normal */ ] = 0;
    total_stretch[1 /*fil */ ] = 0;
    total_shrink[1 /*fil */ ] = 0;
    total_stretch[2 /*fill */ ] = 0;
    total_shrink[2 /*fill */ ] = 0;
    total_stretch[3 /*filll */ ] = 0;
    total_shrink[3 /*filll */ ] = 0 /*:673 */ ;
    while (p != -268435455L) {  /*694: */

        if ((p >= hi_mem_min))
            confusion(66227L /*"vpack" */ );
        else
            switch (mem[p].hh.u.B0) {
            case 0:
            case 1:
            case 2:
            case 13:
                {
                    x = x + d + mem[p + 3].cint;
                    d = mem[p + 2].cint;
                    if (mem[p].hh.u.B0 >= 2 /*rule_node */ )
                        s = 0;
                    else
                        s = mem[p + 4].cint;
                    if (mem[p + 1].cint + s > w)
                        w = mem[p + 1].cint + s;
                }
                break;
            case 8:
                {
                    if ((mem[p].hh.u.B1 == 43 /*pic_node */ ) || (mem[p].hh.u.B1 == 44 /*pdf_node */ )) {
                        x = x + d + mem[p + 3].cint;
                        d = mem[p + 2].cint;
                        if (mem[p + 1].cint > w)
                            w = mem[p + 1].cint;
                    }
                }
                break;
            case 10:
                {
                    x = x + d;
                    d = 0;
                    g = mem[p + 1].hh.v.LH;
                    x = x + mem[g + 1].cint;
                    o = mem[g].hh.u.B0;
                    total_stretch[o] = total_stretch[o] + mem[g + 2].cint;
                    o = mem[g].hh.u.B1;
                    total_shrink[o] = total_shrink[o] + mem[g + 3].cint;
                    if (mem[p].hh.u.B1 >= 100 /*a_leaders */ ) {
                        g = mem[p + 1].hh.v.RH;
                        if (mem[g + 1].cint > w)
                            w = mem[g + 1].cint;
                    }
                }
                break;
            case 11:
                {
                    x = x + d + mem[p + 1].cint;
                    d = 0;
                }
                break;
            default:
                ;
                break;
            }
        p = mem[p].hh.v.RH;
    }
    mem[r + 1].cint = w;
    if (d > l) {
        x = x + d - l;
        mem[r + 2].cint = l;
    } else
        mem[r + 2].cint = d;
    if (m == 1 /*additional */ )
        h = x + h;
    mem[r + 3].cint = h;
    x = h - x;
    if (x == 0) {
        mem[r + 5].hh.u.B0 = 0 /*normal */ ;
        mem[r + 5].hh.u.B1 = 0 /*normal */ ;
        mem[r + 6].gr = 0.0;
        goto exit;
    } else if (x > 0) {         /*698: */
        if (total_stretch[3 /*filll */ ] != 0)
            o = 3 /*filll */ ;
        else if (total_stretch[2 /*fill */ ] != 0)
            o = 2 /*fill */ ;
        else if (total_stretch[1 /*fil */ ] != 0)
            o = 1 /*fil */ ;
        else
            o = 0 /*normal *//*:684 */ ;
        mem[r + 5].hh.u.B1 = o;
        mem[r + 5].hh.u.B0 = 1 /*stretching */ ;
        if (total_stretch[o] != 0)
            mem[r + 6].gr = x / ((double)total_stretch[o]);
        else {

            mem[r + 5].hh.u.B0 = 0 /*normal */ ;
            mem[r + 6].gr = 0.0;
        }
        if (o == 0 /*normal */ ) {

            if (mem[r + 5].hh.v.RH != -268435455L) {    /*699: */
                last_badness = badness(x, total_stretch[0 /*normal */ ]);
                if (last_badness > eqtb[8938767L /*int_base 27 */ ].cint) {
                    print_ln();
                    if (last_badness > 100)
                        print_nl(66216L /*"Underfull" */ );
                    else
                        print_nl(66217L /*"Loose" */ );
                    print(66228L /*" \vbox (badness " */ );
                    print_int(last_badness);
                    goto lab50;
                }
            }
        }
        goto exit;
    } else {                    /*701: */

        if (total_shrink[3 /*filll */ ] != 0)
            o = 3 /*filll */ ;
        else if (total_shrink[2 /*fill */ ] != 0)
            o = 2 /*fill */ ;
        else if (total_shrink[1 /*fil */ ] != 0)
            o = 1 /*fil */ ;
        else
            o = 0 /*normal *//*:690 */ ;
        mem[r + 5].hh.u.B1 = o;
        mem[r + 5].hh.u.B0 = 2 /*shrinking */ ;
        if (total_shrink[o] != 0)
            mem[r + 6].gr = (-(integer) x) / ((double)total_shrink[o]);
        else {

            mem[r + 5].hh.u.B0 = 0 /*normal */ ;
            mem[r + 6].gr = 0.0;
        }
        if ((total_shrink[o] < -(integer) x) && (o == 0 /*normal */ ) && (mem[r + 5].hh.v.RH != -268435455L)) {
            last_badness = 1000000L;
            mem[r + 6].gr = 1.0;
            if ((-(integer) x - total_shrink[0 /*normal */ ] > eqtb[10053201L /*dimen_base 9 */ ].cint)
                || (eqtb[8938767L /*int_base 27 */ ].cint < 100)) {
                print_ln();
                print_nl(66229L /*"Overfull \vbox (" */ );
                print_scaled(-(integer) x - total_shrink[0 /*normal */ ]);
                print(66230L /*"pt too high" */ );
                goto lab50;
            }
        } else if (o == 0 /*normal */ ) {

            if (mem[r + 5].hh.v.RH != -268435455L) {    /*703: */
                last_badness = badness(-(integer) x, total_shrink[0 /*normal */ ]);
                if (last_badness > eqtb[8938767L /*int_base 27 */ ].cint) {
                    print_ln();
                    print_nl(66231L /*"Tight \vbox (badness " */ );
                    print_int(last_badness);
                    goto lab50;
                }
            }
        }
        goto exit;
    }
 lab50:                        /*common_ending *//*700: */ if (output_active)
        print(66219L /*") has occurred while \output is active" */ );
    else {

        if (pack_begin_line != 0) {
            print(66221L /*") in alignment at lines " */ );
            print_int(abs(pack_begin_line));
            print(66222L /*"--" */ );
        } else
            print(66223L /*") detected at line " */ );
        print_int(line);
        print_ln();
    }
    begin_diagnostic();
    show_box(r);
    end_diagnostic(true);

exit:
    Result = r;
    return Result;
}

void zappend_to_vlist(halfword b)
{
    append_to_vlist_regmem scaled d;
    halfword p;
    boolean upwards;
    upwards = (eqtb[8938813L /*eTeX_state_base 2 */ ].cint > 0);
    if (cur_list.aux_field.cint > -65536000L) {
        if (upwards)
            d = mem[eqtb[2252241L /*glue_base 1 */ ].hh.v.RH + 1].cint - cur_list.aux_field.cint - mem[b + 2].cint;
        else
            d = mem[eqtb[2252241L /*glue_base 1 */ ].hh.v.RH + 1].cint - cur_list.aux_field.cint - mem[b + 3].cint;
        if (d < eqtb[10053194L /*dimen_base 2 */ ].cint)
            p = new_param_glue(0 /*line_skip_code */ );
        else {

            p = new_skip_param(1 /*baseline_skip_code */ );
            mem[temp_ptr + 1].cint = d;
        }
        mem[cur_list.tail_field].hh.v.RH = p;
        cur_list.tail_field = p;
    }
    mem[cur_list.tail_field].hh.v.RH = b;
    cur_list.tail_field = b;
    if (upwards)
        cur_list.aux_field.cint = mem[b + 3].cint;
    else
        cur_list.aux_field.cint = mem[b + 2].cint;
}

halfword new_noad(void)
{
    register halfword Result;
    new_noad_regmem halfword p;
    p = get_node(4 /*noad_size */ );
    mem[p].hh.u.B0 = 16 /*ord_noad */ ;
    mem[p].hh.u.B1 = 0 /*normal */ ;
    mem[p + 1].hh = empty_field;
    mem[p + 3].hh = empty_field;
    mem[p + 2].hh = empty_field;
    Result = p;
    return Result;
}

halfword znew_style(small_number s)
{
    register halfword Result;
    new_style_regmem halfword p;
    p = get_node(3 /*style_node_size */ );
    mem[p].hh.u.B0 = 14 /*style_node */ ;
    mem[p].hh.u.B1 = s;
    mem[p + 1].cint = 0;
    mem[p + 2].cint = 0;
    Result = p;
    return Result;
}

halfword new_choice(void)
{
    register halfword Result;
    new_choice_regmem halfword p;
    p = get_node(3 /*style_node_size */ );
    mem[p].hh.u.B0 = 15 /*choice_node */ ;
    mem[p].hh.u.B1 = 0;
    mem[p + 1].hh.v.LH = -268435455L;
    mem[p + 1].hh.v.RH = -268435455L;
    mem[p + 2].hh.v.LH = -268435455L;
    mem[p + 2].hh.v.RH = -268435455L;
    Result = p;
    return Result;
}

void show_info(void)
{
    show_info_regmem show_node_list(mem[temp_ptr].hh.v.LH);
}

scaled zmath_x_height(integer size_code)
{
    register scaled Result;
    math_x_height_regmem integer f;
    scaled rval;
    f = eqtb[2253302L /*math_font_base 2 */  + size_code].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathsy_param(f, 5);
    else
        rval = font_info[5 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled zmath_quad(integer size_code)
{
    register scaled Result;
    math_quad_regmem integer f;
    scaled rval;
    f = eqtb[2253302L /*math_font_base 2 */  + size_code].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathsy_param(f, 6);
    else
        rval = font_info[6 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled znum1(integer size_code)
{
    register scaled Result;
    num1_regmem integer f;
    scaled rval;
    f = eqtb[2253302L /*math_font_base 2 */  + size_code].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathsy_param(f, 8);
    else
        rval = font_info[8 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled znum2(integer size_code)
{
    register scaled Result;
    num2_regmem integer f;
    scaled rval;
    f = eqtb[2253302L /*math_font_base 2 */  + size_code].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathsy_param(f, 9);
    else
        rval = font_info[9 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled znum3(integer size_code)
{
    register scaled Result;
    num3_regmem integer f;
    scaled rval;
    f = eqtb[2253302L /*math_font_base 2 */  + size_code].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathsy_param(f, 10);
    else
        rval = font_info[10 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled zdenom1(integer size_code)
{
    register scaled Result;
    denom1_regmem integer f;
    scaled rval;
    f = eqtb[2253302L /*math_font_base 2 */  + size_code].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathsy_param(f, 11);
    else
        rval = font_info[11 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled zdenom2(integer size_code)
{
    register scaled Result;
    denom2_regmem integer f;
    scaled rval;
    f = eqtb[2253302L /*math_font_base 2 */  + size_code].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathsy_param(f, 12);
    else
        rval = font_info[12 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled zsup1(integer size_code)
{
    register scaled Result;
    sup1_regmem integer f;
    scaled rval;
    f = eqtb[2253302L /*math_font_base 2 */  + size_code].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathsy_param(f, 13);
    else
        rval = font_info[13 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled zsup2(integer size_code)
{
    register scaled Result;
    sup2_regmem integer f;
    scaled rval;
    f = eqtb[2253302L /*math_font_base 2 */  + size_code].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathsy_param(f, 14);
    else
        rval = font_info[14 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled zsup3(integer size_code)
{
    register scaled Result;
    sup3_regmem integer f;
    scaled rval;
    f = eqtb[2253302L /*math_font_base 2 */  + size_code].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathsy_param(f, 15);
    else
        rval = font_info[15 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled zsub1(integer size_code)
{
    register scaled Result;
    sub1_regmem integer f;
    scaled rval;
    f = eqtb[2253302L /*math_font_base 2 */  + size_code].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathsy_param(f, 16);
    else
        rval = font_info[16 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled zsub2(integer size_code)
{
    register scaled Result;
    sub2_regmem integer f;
    scaled rval;
    f = eqtb[2253302L /*math_font_base 2 */  + size_code].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathsy_param(f, 17);
    else
        rval = font_info[17 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled zsup_drop(integer size_code)
{
    register scaled Result;
    sup_drop_regmem integer f;
    scaled rval;
    f = eqtb[2253302L /*math_font_base 2 */  + size_code].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathsy_param(f, 18);
    else
        rval = font_info[18 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled zsub_drop(integer size_code)
{
    register scaled Result;
    sub_drop_regmem integer f;
    scaled rval;
    f = eqtb[2253302L /*math_font_base 2 */  + size_code].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathsy_param(f, 19);
    else
        rval = font_info[19 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled zdelim1(integer size_code)
{
    register scaled Result;
    delim1_regmem integer f;
    scaled rval;
    f = eqtb[2253302L /*math_font_base 2 */  + size_code].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathsy_param(f, 20);
    else
        rval = font_info[20 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled zdelim2(integer size_code)
{
    register scaled Result;
    delim2_regmem integer f;
    scaled rval;
    f = eqtb[2253302L /*math_font_base 2 */  + size_code].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathsy_param(f, 21);
    else
        rval = font_info[21 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled zaxis_height(integer size_code)
{
    register scaled Result;
    axis_height_regmem integer f;
    scaled rval;
    f = eqtb[2253302L /*math_font_base 2 */  + size_code].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathsy_param(f, 22);
    else
        rval = font_info[22 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled default_rule_thickness(void)
{
    register scaled Result;
    default_rule_thickness_regmem integer f;
    scaled rval;
    f = eqtb[2253303L /*math_font_base 3 */  + cur_size].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathex_param(f, 8);
    else
        rval = font_info[8 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled big_op_spacing1(void)
{
    register scaled Result;
    big_op_spacing1_regmem integer f;
    scaled rval;
    f = eqtb[2253303L /*math_font_base 3 */  + cur_size].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathex_param(f, 9);
    else
        rval = font_info[9 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled big_op_spacing2(void)
{
    register scaled Result;
    big_op_spacing2_regmem integer f;
    scaled rval;
    f = eqtb[2253303L /*math_font_base 3 */  + cur_size].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathex_param(f, 10);
    else
        rval = font_info[10 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled big_op_spacing3(void)
{
    register scaled Result;
    big_op_spacing3_regmem integer f;
    scaled rval;
    f = eqtb[2253303L /*math_font_base 3 */  + cur_size].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathex_param(f, 11);
    else
        rval = font_info[11 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled big_op_spacing4(void)
{
    register scaled Result;
    big_op_spacing4_regmem integer f;
    scaled rval;
    f = eqtb[2253303L /*math_font_base 3 */  + cur_size].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathex_param(f, 12);
    else
        rval = font_info[12 + param_base[f]].cint;
    Result = rval;
    return Result;
}

scaled big_op_spacing5(void)
{
    register scaled Result;
    big_op_spacing5_regmem integer f;
    scaled rval;
    f = eqtb[2253303L /*math_font_base 3 */  + cur_size].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rval = get_native_mathex_param(f, 13);
    else
        rval = font_info[13 + param_base[f]].cint;
    Result = rval;
    return Result;
}

halfword zfraction_rule(scaled t)
{
    register halfword Result;
    fraction_rule_regmem halfword p;
    p = new_rule();
    mem[p + 3].cint = t;
    mem[p + 2].cint = 0;
    Result = p;
    return Result;
}

halfword zoverbar(halfword b, scaled k, scaled t)
{
    register halfword Result;
    overbar_regmem halfword p, q;
    p = new_kern(k);
    mem[p].hh.v.RH = b;
    q = fraction_rule(t);
    mem[q].hh.v.RH = p;
    p = new_kern(t);
    mem[p].hh.v.RH = q;
    Result = vpackage(p, 0, 1 /*additional */ , 1073741823L);
    return Result;
}

halfword zchar_box(internal_font_number f, integer c)
{
    register halfword Result;
    char_box_regmem four_quarters q;
    eight_bits hd;
    halfword b, p;
    if (((font_area[f] == 65535L /*aat_font_flag */ ) || (font_area[f] == 65534L /*otgr_font_flag */ ))) {
        b = new_null_box();
        p = new_native_character(f, c);
        mem[b + 5].hh.v.RH = p;
        mem[b + 3].cint = mem[p + 3].cint;
        mem[b + 1].cint = mem[p + 1].cint;
        if (mem[p + 2].cint < 0)
            mem[b + 2].cint = 0;
        else
            mem[b + 2].cint = mem[p + 2].cint;
    } else {

        q = font_info[char_base[f] + effective_char(true, f, c)].qqqq;
        hd = q.u.B1;
        b = new_null_box();
        mem[b + 1].cint = font_info[width_base[f] + q.u.B0].cint + font_info[italic_base[f] + (q.u.B2) / 4].cint;
        mem[b + 3].cint = font_info[height_base[f] + (hd) / 16].cint;
        mem[b + 2].cint = font_info[depth_base[f] + (hd) % 16].cint;
        p = get_avail();
        mem[p].hh.u.B1 = c;
        mem[p].hh.u.B0 = f;
    }
    mem[b + 5].hh.v.RH = p;
    Result = b;
    return Result;
}

void zstack_into_box(halfword b, internal_font_number f, quarterword c)
{
    stack_into_box_regmem halfword p;
    p = char_box(f, c);
    mem[p].hh.v.RH = mem[b + 5].hh.v.RH;
    mem[b + 5].hh.v.RH = p;
    mem[b + 3].cint = mem[p + 3].cint;
}

scaled zheight_plus_depth(internal_font_number f, quarterword c)
{
    register scaled Result;
    height_plus_depth_regmem four_quarters q;
    eight_bits hd;
    q = font_info[char_base[f] + effective_char(true, f, c)].qqqq;
    hd = q.u.B1;
    Result = font_info[height_base[f] + (hd) / 16].cint + font_info[depth_base[f] + (hd) % 16].cint;
    return Result;
}

void zstack_glyph_into_box(halfword b, internal_font_number f, integer g)
{
    stack_glyph_into_box_regmem halfword p, q;
    p = get_node(5 /*glyph_node_size */ );
    mem[p].hh.u.B0 = 8 /*whatsit_node */ ;
    mem[p].hh.u.B1 = 42 /*glyph_node */ ;
    mem[p + 4].qqqq.u.B1 = f;
    mem[p + 4].qqqq.u.B2 = g;
    set_native_glyph_metrics(p, 1);
    if (mem[b].hh.u.B0 == 0 /*hlist_node */ ) {
        q = mem[b + 5].hh.v.RH;
        if (q == -268435455L)
            mem[b + 5].hh.v.RH = p;
        else {

            while (mem[q].hh.v.RH != -268435455L)
                q = mem[q].hh.v.RH;
            mem[q].hh.v.RH = p;
            if ((mem[b + 3].cint < mem[p + 3].cint))
                mem[b + 3].cint = mem[p + 3].cint;
            if ((mem[b + 2].cint < mem[p + 2].cint))
                mem[b + 2].cint = mem[p + 2].cint;
        }
    } else {

        mem[p].hh.v.RH = mem[b + 5].hh.v.RH;
        mem[b + 5].hh.v.RH = p;
        mem[b + 3].cint = mem[p + 3].cint;
        if ((mem[b + 1].cint < mem[p + 1].cint))
            mem[b + 1].cint = mem[p + 1].cint;
    }
}

void zstack_glue_into_box(halfword b, scaled min, scaled max)
{
    stack_glue_into_box_regmem halfword p, q;
    q = new_spec(mem_bot);
    mem[q + 1].cint = min;
    mem[q + 2].cint = max - min;
    p = new_glue(q);
    if (mem[b].hh.u.B0 == 0 /*hlist_node */ ) {
        q = mem[b + 5].hh.v.RH;
        if (q == -268435455L)
            mem[b + 5].hh.v.RH = p;
        else {

            while (mem[q].hh.v.RH != -268435455L)
                q = mem[q].hh.v.RH;
            mem[q].hh.v.RH = p;
        }
    } else {

        mem[p].hh.v.RH = mem[b + 5].hh.v.RH;
        mem[b + 5].hh.v.RH = p;
        mem[b + 3].cint = mem[p + 3].cint;
        mem[b + 1].cint = mem[p + 1].cint;
    }
}

halfword zbuild_opentype_assembly(internal_font_number f, void *a, scaled s, boolean horiz)
{
    register halfword Result;
    build_opentype_assembly_regmem halfword b;
    integer n;
    integer i, j;
    integer g;
    halfword p;
    scaled s_max, o, oo, prev_o, min_o;
    boolean no_extenders;
    scaled nat, str;
    b = new_null_box();
    if (horiz)
        mem[b].hh.u.B0 = 0 /*hlist_node */ ;
    else
        mem[b].hh.u.B0 = 1 /*vlist_node */ ;
    n = -1;
    no_extenders = true;
    min_o = ot_min_connector_overlap(f);
    do {
        n = n + 1;
        s_max = 0;
        prev_o = 0;
        {
            register integer for_end;
            i = 0;
            for_end = ot_part_count(a) - 1;
            if (i <= for_end)
                do {
                    if (ot_part_is_extender(a, i)) {
                        no_extenders = false;
                        {
                            register integer for_end;
                            j = 1;
                            for_end = n;
                            if (j <= for_end)
                                do {
                                    o = ot_part_start_connector(f, a, i);
                                    if (min_o < o)
                                        o = min_o;
                                    if (prev_o < o)
                                        o = prev_o;
                                    s_max = s_max - o + ot_part_full_advance(f, a, i);
                                    prev_o = ot_part_end_connector(f, a, i);
                                }
                                while (j++ < for_end);
                        }
                    } else {

                        o = ot_part_start_connector(f, a, i);
                        if (min_o < o)
                            o = min_o;
                        if (prev_o < o)
                            o = prev_o;
                        s_max = s_max - o + ot_part_full_advance(f, a, i);
                        prev_o = ot_part_end_connector(f, a, i);
                    }
                }
                while (i++ < for_end);
        }
    } while (!((s_max >= s) || no_extenders));
    prev_o = 0;
    {
        register integer for_end;
        i = 0;
        for_end = ot_part_count(a) - 1;
        if (i <= for_end)
            do {
                if (ot_part_is_extender(a, i)) {
                    {
                        register integer for_end;
                        j = 1;
                        for_end = n;
                        if (j <= for_end)
                            do {
                                o = ot_part_start_connector(f, a, i);
                                if (prev_o < o)
                                    o = prev_o;
                                oo = o;
                                if (min_o < o)
                                    o = min_o;
                                if (oo > 0)
                                    stack_glue_into_box(b, -(integer) oo, -(integer) o);
                                g = ot_part_glyph(a, i);
                                stack_glyph_into_box(b, f, g);
                                prev_o = ot_part_end_connector(f, a, i);
                            }
                            while (j++ < for_end);
                    }
                } else {

                    o = ot_part_start_connector(f, a, i);
                    if (prev_o < o)
                        o = prev_o;
                    oo = o;
                    if (min_o < o)
                        o = min_o;
                    if (oo > 0)
                        stack_glue_into_box(b, -(integer) oo, -(integer) o);
                    g = ot_part_glyph(a, i);
                    stack_glyph_into_box(b, f, g);
                    prev_o = ot_part_end_connector(f, a, i);
                }
            }
            while (i++ < for_end);
    }
    p = mem[b + 5].hh.v.RH;
    nat = 0;
    str = 0;
    while (p != -268435455L) {

        if (mem[p].hh.u.B0 == 8 /*whatsit_node */ ) {
            if (horiz)
                nat = nat + mem[p + 1].cint;
            else
                nat = nat + mem[p + 3].cint + mem[p + 2].cint;
        } else if (mem[p].hh.u.B0 == 10 /*glue_node */ ) {
            nat = nat + mem[mem[p + 1].hh.v.LH + 1].cint;
            str = str + mem[mem[p + 1].hh.v.LH + 2].cint;
        }
        p = mem[p].hh.v.RH;
    }
    o = 0;
    if ((s > nat) && (str > 0)) {
        o = (s - nat);
        if ((o > str))
            o = str;
        mem[b + 5].hh.u.B1 = 0 /*normal */ ;
        mem[b + 5].hh.u.B0 = 1 /*stretching */ ;
        mem[b + 6].gr = o / ((double)str);
        if (horiz)
            mem[b + 1].cint = nat + round(str * mem[b + 6].gr);
        else
            mem[b + 3].cint = nat + round(str * mem[b + 6].gr);
    } else if (horiz)
        mem[b + 1].cint = nat;
    else
        mem[b + 3].cint = nat;
    Result = b;
    return Result;
}

halfword zvar_delimiter(halfword d, integer s, scaled v)
{
    register halfword Result;
    var_delimiter_regmem halfword b;
    void *ot_assembly_ptr;
    internal_font_number f, g;
    quarterword c, x, y;
    integer m, n;
    scaled u;
    scaled w;
    four_quarters q;
    eight_bits hd;
    four_quarters r;
    integer z;
    boolean large_attempt;
    f = 0 /*font_base */ ;
    w = 0;
    large_attempt = false;
    z = (mem[d].qqqq.u.B0 % 256);
    x = (mem[d].qqqq.u.B1 + (mem[d].qqqq.u.B0 / 256) * 65536L);
    ot_assembly_ptr = NULL;
    while (true) {

        if ((z != 0) || (x != 0 /*min_quarterword */ )) {
            z = z + s + 256;
            do {
                z = z - 256;
                g = eqtb[2253300L /*math_font_base */  + z].hh.v.RH;
                if (g != 0 /*font_base */ ) {   /*734: */

                    if (((font_area[g] == 65534L /*otgr_font_flag */ ) && (usingOpenType(font_layout_engine[g])))) {
                        x = map_char_to_glyph(g, x);
                        f = g;
                        c = x;
                        w = 0;
                        n = 0;
                        do {
                            y = get_ot_math_variant(g, x, n, &u, 0);
                            if (u > w) {
                                c = y;
                                w = u;
                                if (u >= v)
                                    goto lab40;
                            }
                            n = n + 1;
                        } while (!(u < 0));
                        ot_assembly_ptr = get_ot_assembly_ptr(g, x, 0);
                        if (ot_assembly_ptr != NULL)
                            goto lab40;
                    } else {

                        y = x;
                        if ((y >= font_bc[g]) && (y <= font_ec[g])) {
 lab22:                        /*continue */ q = font_info[char_base[g] + y].qqqq;
                            if ((q.u.B0 > 0 /*min_quarterword */ )) {
                                if (((q.u.B2) % 4) == 3 /*ext_tag */ ) {
                                    f = g;
                                    c = y;
                                    goto lab40;
                                }
                                hd = q.u.B1;
                                u = font_info[height_base[g] + (hd) / 16].cint + font_info[depth_base[g] +
                                                                                           (hd) % 16].cint;
                                if (u > w) {
                                    f = g;
                                    c = y;
                                    w = u;
                                    if (u >= v)
                                        goto lab40;
                                }
                                if (((q.u.B2) % 4) == 2 /*list_tag */ ) {
                                    y = q.u.B3;
                                    goto lab22;
                                }
                            }
                        }
                    }
                }
            } while (!(z < 256 /*script_size */ ));
        }
        if (large_attempt)
            goto lab40;
        large_attempt = true;
        z = (mem[d].qqqq.u.B2 % 256);
        x = (mem[d].qqqq.u.B3 + (mem[d].qqqq.u.B2 / 256) * 65536L);
    }
 lab40:/*found */ if (f != 0 /*font_base */ ) {
        if (!((font_area[f] == 65534L /*otgr_font_flag */ ) && (usingOpenType(font_layout_engine[f])))) {       /*736: */

            if (((q.u.B2) % 4) == 3 /*ext_tag */ ) {      /*739: */
                b = new_null_box();
                mem[b].hh.u.B0 = 1 /*vlist_node */ ;
                r = font_info[exten_base[f] + q.u.B3].qqqq;
                c = r.u.B3;
                u = height_plus_depth(f, c);
                w = 0;
                q = font_info[char_base[f] + effective_char(true, f, c)].qqqq;
                mem[b + 1].cint = font_info[width_base[f] + q.u.B0].cint + font_info[italic_base[f] + (q.u.B2) / 4].cint;
                c = r.u.B2;
                if (c != 0 /*min_quarterword */ )
                    w = w + height_plus_depth(f, c);
                c = r.u.B1;
                if (c != 0 /*min_quarterword */ )
                    w = w + height_plus_depth(f, c);
                c = r.u.B0;
                if (c != 0 /*min_quarterword */ )
                    w = w + height_plus_depth(f, c);
                n = 0;
                if (u > 0)
                    while (w < v) {

                        w = w + u;
                        n++;
                        if (r.u.B1 != 0 /*min_quarterword */ )
                            w = w + u;
                    }
                c = r.u.B2;
                if (c != 0 /*min_quarterword */ )
                    stack_into_box(b, f, c);
                c = r.u.B3;
                {
                    register integer for_end;
                    m = 1;
                    for_end = n;
                    if (m <= for_end)
                        do
                            stack_into_box(b, f, c);
                        while (m++ < for_end);
                }
                c = r.u.B1;
                if (c != 0 /*min_quarterword */ ) {
                    stack_into_box(b, f, c);
                    c = r.u.B3;
                    {
                        register integer for_end;
                        m = 1;
                        for_end = n;
                        if (m <= for_end)
                            do
                                stack_into_box(b, f, c);
                            while (m++ < for_end);
                    }
                }
                c = r.u.B0;
                if (c != 0 /*min_quarterword */ )
                    stack_into_box(b, f, c);
                mem[b + 2].cint = w - mem[b + 3].cint;
            } else
                b = char_box(f, c) /*:736 */ ;
        } else {

            if (ot_assembly_ptr != NULL)
                b = build_opentype_assembly(f, ot_assembly_ptr, v, 0);
            else {

                b = new_null_box();
                mem[b].hh.u.B0 = 1 /*vlist_node */ ;
                mem[b + 5].hh.v.RH = get_node(5 /*glyph_node_size */ );
                mem[mem[b + 5].hh.v.RH].hh.u.B0 = 8 /*whatsit_node */ ;
                mem[mem[b + 5].hh.v.RH].hh.u.B1 = 42 /*glyph_node */ ;
                mem[mem[b + 5].hh.v.RH + 4].qqqq.u.B1 = f;
                mem[mem[b + 5].hh.v.RH + 4].qqqq.u.B2 = c;
                set_native_glyph_metrics(mem[b + 5].hh.v.RH, 1);
                mem[b + 1].cint = mem[mem[b + 5].hh.v.RH + 1].cint;
                mem[b + 3].cint = mem[mem[b + 5].hh.v.RH + 3].cint;
                mem[b + 2].cint = mem[mem[b + 5].hh.v.RH + 2].cint;
            }
        }
    } else {

        b = new_null_box();
        mem[b + 1].cint = eqtb[10053203L /*dimen_base 11 */ ].cint;
    }
    mem[b + 4].cint = half(mem[b + 3].cint - mem[b + 2].cint) - axis_height(s);
    Result = b;
    return Result;
}

halfword zrebox(halfword b, scaled w)
{
    register halfword Result;
    rebox_regmem halfword p;
    internal_font_number f;
    scaled v;
    if ((mem[b + 1].cint != w) && (mem[b + 5].hh.v.RH != -268435455L)) {
        if (mem[b].hh.u.B0 == 1 /*vlist_node */ )
            b = hpack(b, 0, 1 /*additional */ );
        p = mem[b + 5].hh.v.RH;
        if (((p >= hi_mem_min)) && (mem[p].hh.v.RH == -268435455L)) {
            f = mem[p].hh.u.B0;
            v = font_info[width_base[f] + font_info[char_base[f] + effective_char(true, f, mem[p].hh.u.B1)].qqqq.u.B0].cint;
            if (v != mem[b + 1].cint)
                mem[p].hh.v.RH = new_kern(mem[b + 1].cint - v);
        }
        free_node(b, 8 /*box_node_size */ );
        b = new_glue(mem_bot + 12);
        mem[b].hh.v.RH = p;
        while (mem[p].hh.v.RH != -268435455L)
            p = mem[p].hh.v.RH;
        mem[p].hh.v.RH = new_glue(mem_bot + 12);
        Result = hpack(b, w, 0 /*exactly */ );
    } else {

        mem[b + 1].cint = w;
        Result = b;
    }
    return Result;
}

halfword zmath_glue(halfword g, scaled m)
{
    register halfword Result;
    math_glue_regmem halfword p;
    integer n;
    scaled f;
    n = x_over_n(m, 65536L);
    f = tex_remainder;
    if (f < 0) {
        n--;
        f = f + 65536L;
    }
    p = get_node(4 /*glue_spec_size */ );
    mem[p + 1].cint = mult_and_add(n, mem[g + 1].cint, xn_over_d(mem[g + 1].cint, f, 65536L), 1073741823L);
    mem[p].hh.u.B0 = mem[g].hh.u.B0;
    if (mem[p].hh.u.B0 == 0 /*normal */ )
        mem[p + 2].cint = mult_and_add(n, mem[g + 2].cint, xn_over_d(mem[g + 2].cint, f, 65536L), 1073741823L);
    else
        mem[p + 2].cint = mem[g + 2].cint;
    mem[p].hh.u.B1 = mem[g].hh.u.B1;
    if (mem[p].hh.u.B1 == 0 /*normal */ )
        mem[p + 3].cint = mult_and_add(n, mem[g + 3].cint, xn_over_d(mem[g + 3].cint, f, 65536L), 1073741823L);
    else
        mem[p + 3].cint = mem[g + 3].cint;
    Result = p;
    return Result;
}

void zmath_kern(halfword p, scaled m)
{
    math_kern_regmem integer n;
    scaled f;
    if (mem[p].hh.u.B1 == 99 /*mu_glue */ ) {
        n = x_over_n(m, 65536L);
        f = tex_remainder;
        if (f < 0) {
            n--;
            f = f + 65536L;
        }
        mem[p + 1].cint = mult_and_add(n, mem[p + 1].cint, xn_over_d(mem[p + 1].cint, f, 65536L), 1073741823L);
        mem[p].hh.u.B1 = 1 /*explicit */ ;
    }
}

void flush_math(void)
{
    flush_math_regmem flush_node_list(mem[cur_list.head_field].hh.v.RH);
    flush_node_list(cur_list.aux_field.cint);
    mem[cur_list.head_field].hh.v.RH = -268435455L;
    cur_list.tail_field = cur_list.head_field;
    cur_list.aux_field.cint = -268435455L;
}

halfword zclean_box(halfword p, small_number s)
{
    register halfword Result;
    clean_box_regmem halfword q;
    small_number save_style;
    halfword x;
    halfword r;
    switch (mem[p].hh.v.RH) {
    case 1:
        {
            cur_mlist = new_noad();
            mem[cur_mlist + 1] = mem[p];
        }
        break;
    case 2:
        {
            q = mem[p].hh.v.LH;
            goto lab40;
        }
        break;
    case 3:
        cur_mlist = mem[p].hh.v.LH;
        break;
    default:
        {
            q = new_null_box();
            goto lab40;
        }
        break;
    }
    save_style = cur_style;
    cur_style = s;
    mlist_penalties = false;
    mlist_to_hlist();
    q = mem[mem_top - 3].hh.v.RH;
    cur_style = save_style;
    {
        if (cur_style < 4 /*script_style */ )
            cur_size = 0 /*text_size */ ;
        else
            cur_size = 256 /*script_size */  * ((cur_style - 2) / 2);
        cur_mu = x_over_n(math_quad(cur_size), 18);
    }
 lab40:                        /*found */ if ((q >= hi_mem_min) || (q == -268435455L))
        x = hpack(q, 0, 1 /*additional */ );
    else if ((mem[q].hh.v.RH == -268435455L) && (mem[q].hh.u.B0 <= 1 /*vlist_node */ ) && (mem[q + 4].cint == 0))
        x = q;
    else
        x = hpack(q, 0, 1 /*additional */ );
    q = mem[x + 5].hh.v.RH;
    if ((q >= hi_mem_min)) {
        r = mem[q].hh.v.RH;
        if (r != -268435455L) {

            if (mem[r].hh.v.RH == -268435455L) {

                if (!(r >= hi_mem_min)) {

                    if (mem[r].hh.u.B0 == 11 /*kern_node */ ) {
                        free_node(r, 3 /*medium_node_size */ );
                        mem[q].hh.v.RH = -268435455L;
                    }
                }
            }
        }
    }
    Result = x;
    return Result;
}

void zfetch(halfword a)
{
    fetch_regmem cur_c = (unsigned short) mem[a].hh.u.B1;
    cur_f = eqtb[2253300L /*math_font_base */  + (mem[a].hh.u.B0 % 256) + cur_size].hh.v.RH;
    cur_c = cur_c + (mem[a].hh.u.B0 / 256) * 65536L;
    if (cur_f == 0 /*font_base */ ) {   /*749: */
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(65622L /*"" */ );
        }
        print_size(cur_size);
        print_char(32 /*" " */ );
        print_int((mem[a].hh.u.B0 % 256));
        print(66274L /*" is undefined (character " */ );
        print(cur_c);
        print_char(41 /*")" */ );
        {
            help_ptr = 4;
            help_line[3] = 66275L /*"Somewhere in the math formula just ended, you used the" */ ;
            help_line[2] = 66276L /*"stated character from an undefined font family. For example," */ ;
            help_line[1] = 66277L /*"plain TeX doesn't allow \it or \sl in subscripts. Proceed," */ ;
            help_line[0] = 66278L /*"and I'll try to forget that I needed that character." */ ;
        }
        error();
        cur_i = null_character;
        mem[a].hh.v.RH = 0 /*empty */ ;
    } else if (((font_area[cur_f] == 65535L /*aat_font_flag */ ) || (font_area[cur_f] == 65534L /*otgr_font_flag */ ))) {
        cur_i = null_character;
    } else {

        if ((cur_c >= font_bc[cur_f]) && (cur_c <= font_ec[cur_f]))
            cur_i = font_info[char_base[cur_f] + cur_c].qqqq;
        else
            cur_i = null_character;
        if (!((cur_i.u.B0 > 0 /*min_quarterword */ ))) {
            char_warning(cur_f, cur_c);
            mem[a].hh.v.RH = 0 /*empty */ ;
        }
    }
}

void zmake_over(halfword q)
{
    make_over_regmem
        mem[q + 1].hh.v.LH =
        overbar(clean_box(q + 1, 2 * (cur_style / 2) + 1), 3 * default_rule_thickness(), default_rule_thickness());
    mem[q + 1].hh.v.RH = 2 /*sub_box */ ;
}

void zmake_under(halfword q)
{
    make_under_regmem halfword p, x, y;
    scaled delta;
    x = clean_box(q + 1, cur_style);
    p = new_kern(3 * default_rule_thickness());
    mem[x].hh.v.RH = p;
    mem[p].hh.v.RH = fraction_rule(default_rule_thickness());
    y = vpackage(x, 0, 1 /*additional */ , 1073741823L);
    delta = mem[y + 3].cint + mem[y + 2].cint + default_rule_thickness();
    mem[y + 3].cint = mem[x + 3].cint;
    mem[y + 2].cint = delta - mem[y + 3].cint;
    mem[q + 1].hh.v.LH = y;
    mem[q + 1].hh.v.RH = 2 /*sub_box */ ;
}

void zmake_vcenter(halfword q)
{
    make_vcenter_regmem halfword v;
    scaled delta;
    v = mem[q + 1].hh.v.LH;
    if (mem[v].hh.u.B0 != 1 /*vlist_node */ )
        confusion(65855L /*"vcenter" */ );
    delta = mem[v + 3].cint + mem[v + 2].cint;
    mem[v + 3].cint = axis_height(cur_size) + half(delta);
    mem[v + 2].cint = delta - mem[v + 3].cint;
}

void zmake_radical(halfword q)
{
    make_radical_regmem halfword x, y;
    internal_font_number f;
    scaled rule_thickness;
    scaled delta, clr;
    f = eqtb[2253300L /*math_font_base */  + (mem[q + 4].qqqq.u.B0 % 256) + cur_size].hh.v.RH;
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f]))))
        rule_thickness = get_ot_math_constant(f, 51 /*radicalRuleThickness */ );
    else
        rule_thickness = default_rule_thickness();
    x = clean_box(q + 1, 2 * (cur_style / 2) + 1);
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f])))) {
        if (cur_style < 2 /*text_style */ )
            clr = get_ot_math_constant(f, 50 /*radicalDisplayStyleVerticalGap */ );
        else
            clr = get_ot_math_constant(f, 49 /*radicalVerticalGap */ );
    } else {

        if (cur_style < 2 /*text_style */ )
            clr = rule_thickness + (abs(math_x_height(cur_size)) / 4);
        else {

            clr = rule_thickness;
            clr = clr + (abs(clr) / 4);
        }
    }
    y = var_delimiter(q + 4, cur_size, mem[x + 3].cint + mem[x + 2].cint + clr + rule_thickness);
    if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f])))) {
        mem[y + 2].cint = mem[y + 3].cint + mem[y + 2].cint - rule_thickness;
        mem[y + 3].cint = rule_thickness;
    }
    delta = mem[y + 2].cint - (mem[x + 3].cint + mem[x + 2].cint + clr);
    if (delta > 0)
        clr = clr + half(delta);
    mem[y + 4].cint = -(integer) (mem[x + 3].cint + clr);
    mem[y].hh.v.RH = overbar(x, clr, mem[y + 3].cint);
    mem[q + 1].hh.v.LH = hpack(y, 0, 1 /*additional */ );
    mem[q + 1].hh.v.RH = 2 /*sub_box */ ;
}

scaled zcompute_ot_math_accent_pos(halfword p)
{
    register scaled Result;
    compute_ot_math_accent_pos_regmem halfword q, r;
    scaled s, g;
    if ((mem[p + 1].hh.v.RH == 1 /*math_char */ )) {
        fetch(p + 1);
        q = new_native_character(cur_f, cur_c);
        g = get_native_glyph(q, 0);
        s = get_ot_math_accent_pos(cur_f, g);
    } else {

        if ((mem[p + 1].hh.v.RH == 3 /*sub_mlist */ )) {
            r = mem[p + 1].hh.v.LH;
            if ((r != -268435455L) && (mem[r].hh.u.B0 == 28 /*accent_noad */ ))
                s = compute_ot_math_accent_pos(r);
            else
                s = 2147483647L;
        } else
            s = 2147483647L;
    }
    Result = s;
    return Result;
}

void zmake_math_accent(halfword q)
{
    make_math_accent_regmem halfword p, x, y;
    integer a;
    integer c, g;
    internal_font_number f;
    four_quarters i;
    scaled s, sa;
    scaled h;
    scaled delta;
    scaled w, w2;
    void *ot_assembly_ptr;
    fetch(q + 4);
    x = -268435455L;
    if (((font_area[cur_f] == 65535L /*aat_font_flag */ ) || (font_area[cur_f] == 65534L /*otgr_font_flag */ ))) {
        c = cur_c;
        f = cur_f;
        if (!((mem[q].hh.u.B1 == 2 /*bottom_acc */ ) || (mem[q].hh.u.B1 == 3 /*bottom_acc 1 */ )))
            s = compute_ot_math_accent_pos(q);
        else
            s = 0;
        x = clean_box(q + 1, 2 * (cur_style / 2) + 1);
        w = mem[x + 1].cint;
        h = mem[x + 3].cint;
    } else if ((cur_i.u.B0 > 0 /*min_quarterword */ )) {
        i = cur_i;
        c = cur_c;
        f = cur_f;
        s = 0;
        if (mem[q + 1].hh.v.RH == 1 /*math_char */ ) {
            fetch(q + 1);
            if (((cur_i.u.B2) % 4) == 1 /*lig_tag */ ) {
                a = lig_kern_base[cur_f] + cur_i.u.B3;
                cur_i = font_info[a].qqqq;
                if (cur_i.u.B0 > 128) {
                    a = lig_kern_base[cur_f] + 256 * cur_i.u.B2 + cur_i.u.B3 + 32768L - 256 * (128);
                    cur_i = font_info[a].qqqq;
                }
                while (true) {

                    if (cur_i.u.B1 == skew_char[cur_f]) {
                        if (cur_i.u.B2 >= 128) {

                            if (cur_i.u.B0 <= 128)
                                s = font_info[kern_base[cur_f] + 256 * cur_i.u.B2 + cur_i.u.B3].cint;
                        }
                        goto lab31;
                    }
                    if (cur_i.u.B0 >= 128)
                        goto lab31;
                    a = a + cur_i.u.B0 + 1;
                    cur_i = font_info[a].qqqq;
                }
            }
        }
 lab31:                        /*done1 *//*:768 */ ;
        x = clean_box(q + 1, 2 * (cur_style / 2) + 1);
        w = mem[x + 1].cint;
        h = mem[x + 3].cint;
        while (true) {

            if (((i.u.B2) % 4) != 2 /*list_tag */ )
                goto lab30;
            y = i.u.B3;
            i = font_info[char_base[f] + y].qqqq;
            if (!(i.u.B0 > 0 /*min_quarterword */ ))
                goto lab30;
            if (font_info[width_base[f] + i.u.B0].cint > w)
                goto lab30;
            c = y;
        }
 lab30:                        /*done *//*:767 */ ;
    }
    if (x != -268435455L) {
        if (((font_area[f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[f])))) {

            if (((mem[q].hh.u.B1 == 2 /*bottom_acc */ ) || (mem[q].hh.u.B1 == 3 /*bottom_acc 1 */ )))
                delta = 0;
            else if (h < get_ot_math_constant(f, 6 /*accentBaseHeight */ ))
                delta = h;
            else
                delta = get_ot_math_constant(f, 6 /*accentBaseHeight */ );
        } else if (h < font_info[5 /*x_height_code */  + param_base[f]].cint)
            delta = h;
        else
            delta = font_info[5 /*x_height_code */  + param_base[f]].cint;
        if ((mem[q + 2].hh.v.RH != 0 /*empty */ ) || (mem[q + 3].hh.v.RH != 0 /*empty */ )) {

            if (mem[q + 1].hh.v.RH == 1 /*math_char */ ) {      /*769: */
                flush_node_list(x);
                x = new_noad();
                mem[x + 1] = mem[q + 1];
                mem[x + 2] = mem[q + 2];
                mem[x + 3] = mem[q + 3];
                mem[q + 2].hh = empty_field;
                mem[q + 3].hh = empty_field;
                mem[q + 1].hh.v.RH = 3 /*sub_mlist */ ;
                mem[q + 1].hh.v.LH = x;
                x = clean_box(q + 1, cur_style);
                delta = delta + mem[x + 3].cint - h;
                h = mem[x + 3].cint;
            }
        }
        y = char_box(f, c);
        if (((font_area[f] == 65535L /*aat_font_flag */ ) || (font_area[f] == 65534L /*otgr_font_flag */ ))) {
            p = get_node(5 /*glyph_node_size */ );
            mem[p].hh.u.B0 = 8 /*whatsit_node */ ;
            mem[p].hh.u.B1 = 42 /*glyph_node */ ;
            mem[p + 4].qqqq.u.B1 = f;
            mem[p + 4].qqqq.u.B2 = get_native_glyph(mem[y + 5].hh.v.RH, 0);
            set_native_glyph_metrics(p, 1);
            free_node(mem[y + 5].hh.v.RH, mem[mem[y + 5].hh.v.RH + 4].qqqq.u.B0);
            mem[y + 5].hh.v.RH = p;
            if (odd(mem[q].hh.u.B1))
                set_native_glyph_metrics(p, 1);
            else {

                c = mem[p + 4].qqqq.u.B2;
                a = 0;
                do {
                    g = get_ot_math_variant(f, c, a, &w2, 1);
                    if ((w2 > 0) && (w2 <= w)) {
                        mem[p + 4].qqqq.u.B2 = g;
                        set_native_glyph_metrics(p, 1);
                        a++;
                    }
                } while (!((w2 < 0) || (w2 >= w)));
                if ((w2 < 0)) {
                    ot_assembly_ptr = get_ot_assembly_ptr(f, c, 1);
                    if (ot_assembly_ptr != NULL) {
                        free_node(p, 5 /*glyph_node_size */ );
                        p = build_opentype_assembly(f, ot_assembly_ptr, w, 1);
                        mem[y + 5].hh.v.RH = p;
                        goto lab40;
                    }
                } else
                    set_native_glyph_metrics(p, 1);
            }
 lab40:                        /*found */ mem[y + 1].cint = mem[p + 1].cint;
            mem[y + 3].cint = mem[p + 3].cint;
            mem[y + 2].cint = mem[p + 2].cint;
            if (((mem[q].hh.u.B1 == 2 /*bottom_acc */ ) || (mem[q].hh.u.B1 == 3 /*bottom_acc 1 */ ))) {
                if (mem[y + 3].cint < 0)
                    mem[y + 3].cint = 0;
            } else if (mem[y + 2].cint < 0)
                mem[y + 2].cint = 0;
            if ((((p) != -268435455L && (!(p >= hi_mem_min)) && (mem[p].hh.u.B0 == 8 /*whatsit_node */ )
                  && (mem[p].hh.u.B1 == 42 /*glyph_node */ )))) {
                sa = get_ot_math_accent_pos(f, mem[p + 4].qqqq.u.B2);
                if (sa == 2147483647L)
                    sa = half(mem[y + 1].cint);
            } else
                sa = half(mem[y + 1].cint);
            if (((mem[q].hh.u.B1 == 2 /*bottom_acc */ ) || (mem[q].hh.u.B1 == 3 /*bottom_acc 1 */ )) || (s == 2147483647L))
                s = half(w);
            mem[y + 4].cint = s - sa;
        } else
            mem[y + 4].cint = s + half(w - mem[y + 1].cint);
        mem[y + 1].cint = 0;
        if (((mem[q].hh.u.B1 == 2 /*bottom_acc */ ) || (mem[q].hh.u.B1 == 3 /*bottom_acc 1 */ ))) {
            mem[x].hh.v.RH = y;
            y = vpackage(x, 0, 1 /*additional */ , 1073741823L);
            mem[y + 4].cint = -(integer) (h - mem[y + 3].cint);
        } else {

            p = new_kern(-(integer) delta);
            mem[p].hh.v.RH = x;
            mem[y].hh.v.RH = p;
            y = vpackage(y, 0, 1 /*additional */ , 1073741823L);
            if (mem[y + 3].cint < h) {  /*765: */
                p = new_kern(h - mem[y + 3].cint);
                mem[p].hh.v.RH = mem[y + 5].hh.v.RH;
                mem[y + 5].hh.v.RH = p;
                mem[y + 3].cint = h;
            }
        }
        mem[y + 1].cint = mem[x + 1].cint;
        mem[q + 1].hh.v.LH = y;
        mem[q + 1].hh.v.RH = 2 /*sub_box */ ;
    }
}

void zmake_fraction(halfword q)
{
    make_fraction_regmem halfword p, v, x, y, z;
    scaled delta, delta1, delta2, shift_up, shift_down, clr;
    if (mem[q + 1].cint == 1073741824L)
        mem[q + 1].cint = default_rule_thickness();
    x = clean_box(q + 2, cur_style + 2 - 2 * (cur_style / 6));
    z = clean_box(q + 3, 2 * (cur_style / 2) + 3 - 2 * (cur_style / 6));
    if (mem[x + 1].cint < mem[z + 1].cint)
        x = rebox(x, mem[z + 1].cint);
    else
        z = rebox(z, mem[x + 1].cint);
    if (cur_style < 2 /*text_style */ ) {
        shift_up = num1(cur_size);
        shift_down = denom1(cur_size);
    } else {

        shift_down = denom2(cur_size);
        if (mem[q + 1].cint != 0)
            shift_up = num2(cur_size);
        else
            shift_up = num3(cur_size);
    }
    if (mem[q + 1].cint == 0) { /*772: */
        if (((font_area[cur_f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[cur_f])))) {
            if (cur_style < 2 /*text_style */ )
                clr = get_ot_math_constant(cur_f, 27 /*stackDisplayStyleGapMin */ );
            else
                clr = get_ot_math_constant(cur_f, 26 /*stackGapMin */ );
        } else {

            if (cur_style < 2 /*text_style */ )
                clr = 7 * default_rule_thickness();
            else
                clr = 3 * default_rule_thickness();
        }
        delta = half(clr - ((shift_up - mem[x + 2].cint) - (mem[z + 3].cint - shift_down)));
        if (delta > 0) {
            shift_up = shift_up + delta;
            shift_down = shift_down + delta;
        }
    } else {                    /*773: */

        if (((font_area[cur_f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[cur_f])))) {
            delta = half(mem[q + 1].cint);
            if (cur_style < 2 /*text_style */ )
                clr = get_ot_math_constant(cur_f, 37 /*fractionNumDisplayStyleGapMin */ );
            else
                clr = get_ot_math_constant(cur_f, 36 /*fractionNumeratorGapMin */ );
            delta1 = clr - ((shift_up - mem[x + 2].cint) - (axis_height(cur_size) + delta));
            if (cur_style < 2 /*text_style */ )
                clr = get_ot_math_constant(cur_f, 40 /*fractionDenomDisplayStyleGapMin */ );
            else
                clr = get_ot_math_constant(cur_f, 39 /*fractionDenominatorGapMin */ );
            delta2 = clr - ((axis_height(cur_size) - delta) - (mem[z + 3].cint - shift_down));
        } else {

            if (cur_style < 2 /*text_style */ )
                clr = 3 * mem[q + 1].cint;
            else
                clr = mem[q + 1].cint;
            delta = half(mem[q + 1].cint);
            delta1 = clr - ((shift_up - mem[x + 2].cint) - (axis_height(cur_size) + delta));
            delta2 = clr - ((axis_height(cur_size) - delta) - (mem[z + 3].cint - shift_down));
        }
        if (delta1 > 0)
            shift_up = shift_up + delta1;
        if (delta2 > 0)
            shift_down = shift_down + delta2;
    }
    v = new_null_box();
    mem[v].hh.u.B0 = 1 /*vlist_node */ ;
    mem[v + 3].cint = shift_up + mem[x + 3].cint;
    mem[v + 2].cint = mem[z + 2].cint + shift_down;
    mem[v + 1].cint = mem[x + 1].cint;
    if (mem[q + 1].cint == 0) {
        p = new_kern((shift_up - mem[x + 2].cint) - (mem[z + 3].cint - shift_down));
        mem[p].hh.v.RH = z;
    } else {

        y = fraction_rule(mem[q + 1].cint);
        p = new_kern((axis_height(cur_size) - delta) - (mem[z + 3].cint - shift_down));
        mem[y].hh.v.RH = p;
        mem[p].hh.v.RH = z;
        p = new_kern((shift_up - mem[x + 2].cint) - (axis_height(cur_size) + delta));
        mem[p].hh.v.RH = y;
    }
    mem[x].hh.v.RH = p;
    mem[v + 5].hh.v.RH = /*:774 */ x;
    if (cur_style < 2 /*text_style */ )
        delta = delim1(cur_size);
    else
        delta = delim2(cur_size);
    x = var_delimiter(q + 4, cur_size, delta);
    mem[x].hh.v.RH = v;
    z = var_delimiter(q + 5, cur_size, delta);
    mem[v].hh.v.RH = z;
    mem[q + 1].cint = hpack(x, 0, 1 /*additional */ ) /*:775 */ ;
}

scaled zmake_op(halfword q)
{
    register scaled Result;
    make_op_regmem scaled delta;
    halfword p, v, x, y, z;
    quarterword c;
    four_quarters i;
    scaled shift_up, shift_down;
    scaled h1, h2;
    integer n, g;
    void *ot_assembly_ptr;
    internal_font_number save_f;
    if ((mem[q].hh.u.B1 == 0 /*normal */ ) && (cur_style < 2 /*text_style */ ))
        mem[q].hh.u.B1 = 1 /*limits */ ;
    delta = 0;
    if (mem[q + 1].hh.v.RH == 1 /*math_char */ ) {
        fetch(q + 1);
        if (!((font_area[cur_f] == 65534L /*otgr_font_flag */ ) && (usingOpenType(font_layout_engine[cur_f])))) {
            if ((cur_style < 2 /*text_style */ ) && (((cur_i.u.B2) % 4) == 2 /*list_tag */ )) {
                c = cur_i.u.B3;
                i = font_info[char_base[cur_f] + c].qqqq;
                if ((i.u.B0 > 0 /*min_quarterword */ )) {
                    cur_c = c;
                    cur_i = i;
                    mem[q + 1].hh.u.B1 = c;
                }
            }
            delta = font_info[italic_base[cur_f] + (cur_i.u.B2) / 4].cint;
        }
        x = clean_box(q + 1, cur_style);
        if (((font_area[cur_f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[cur_f])))) {
            p = mem[x + 5].hh.v.RH;
            if ((((p) != -268435455L && (!(p >= hi_mem_min)) && (mem[p].hh.u.B0 == 8 /*whatsit_node */ )
                  && (mem[p].hh.u.B1 == 42 /*glyph_node */ )))) {
                if (cur_style < 2 /*text_style */ ) {
                    h1 = get_ot_math_constant(cur_f, 3 /*displayOperatorMinHeight */ );
                    if (h1 < (mem[p + 3].cint + mem[p + 2].cint) * 5 / ((double)4))
                        h1 = (mem[p + 3].cint + mem[p + 2].cint) * 5 / ((double)4);
                    c = mem[p + 4].qqqq.u.B2;
                    n = 0;
                    do {
                        g = get_ot_math_variant(cur_f, c, n, &h2, 0);
                        if (h2 > 0) {
                            mem[p + 4].qqqq.u.B2 = g;
                            set_native_glyph_metrics(p, 1);
                        }
                        n++;
                    } while (!((h2 < 0) || (h2 >= h1)));
                    if ((h2 < 0)) {
                        ot_assembly_ptr = get_ot_assembly_ptr(cur_f, c, 0);
                        if (ot_assembly_ptr != NULL) {
                            free_node(p, 5 /*glyph_node_size */ );
                            p = build_opentype_assembly(cur_f, ot_assembly_ptr, h1, 0);
                            mem[x + 5].hh.v.RH = p;
                            delta = 0;
                            goto lab40;
                        }
                    } else
                        set_native_glyph_metrics(p, 1);
                }
                delta = get_ot_math_ital_corr(cur_f, mem[p + 4].qqqq.u.B2);
 lab40:                        /*found */ mem[x + 1].cint = mem[p + 1].cint;
                mem[x + 3].cint = mem[p + 3].cint;
                mem[x + 2].cint = mem[p + 2].cint;
            }
        }
        if ((mem[q + 3].hh.v.RH != 0 /*empty */ ) && (mem[q].hh.u.B1 != 1 /*limits */ ))
            mem[x + 1].cint = mem[x + 1].cint - delta;
        mem[x + 4].cint = half(mem[x + 3].cint - mem[x + 2].cint) - axis_height(cur_size);
        mem[q + 1].hh.v.RH = 2 /*sub_box */ ;
        mem[q + 1].hh.v.LH = x;
    }
    save_f = cur_f;
    if (mem[q].hh.u.B1 == 1 /*limits */ ) {       /*777: */
        x = clean_box(q + 2, 2 * (cur_style / 4) + 4 + (cur_style % 2));
        y = clean_box(q + 1, cur_style);
        z = clean_box(q + 3, 2 * (cur_style / 4) + 5);
        v = new_null_box();
        mem[v].hh.u.B0 = 1 /*vlist_node */ ;
        mem[v + 1].cint = mem[y + 1].cint;
        if (mem[x + 1].cint > mem[v + 1].cint)
            mem[v + 1].cint = mem[x + 1].cint;
        if (mem[z + 1].cint > mem[v + 1].cint)
            mem[v + 1].cint = mem[z + 1].cint;
        x = rebox(x, mem[v + 1].cint);
        y = rebox(y, mem[v + 1].cint);
        z = rebox(z, mem[v + 1].cint);
        mem[x + 4].cint = half(delta);
        mem[z + 4].cint = -(integer) mem[x + 4].cint;
        mem[v + 3].cint = mem[y + 3].cint;
        mem[v + 2].cint = mem[y + 2].cint;
        cur_f = save_f;
        if (mem[q + 2].hh.v.RH == 0 /*empty */ ) {
            free_node(x, 8 /*box_node_size */ );
            mem[v + 5].hh.v.RH = y;
        } else {

            shift_up = big_op_spacing3() - mem[x + 2].cint;
            if (shift_up < big_op_spacing1())
                shift_up = big_op_spacing1();
            p = new_kern(shift_up);
            mem[p].hh.v.RH = y;
            mem[x].hh.v.RH = p;
            p = new_kern(big_op_spacing5());
            mem[p].hh.v.RH = x;
            mem[v + 5].hh.v.RH = p;
            mem[v + 3].cint = mem[v + 3].cint + big_op_spacing5() + mem[x + 3].cint + mem[x + 2].cint + shift_up;
        }
        if (mem[q + 3].hh.v.RH == 0 /*empty */ )
            free_node(z, 8 /*box_node_size */ );
        else {

            shift_down = big_op_spacing4() - mem[z + 3].cint;
            if (shift_down < big_op_spacing2())
                shift_down = big_op_spacing2();
            p = new_kern(shift_down);
            mem[y].hh.v.RH = p;
            mem[p].hh.v.RH = z;
            p = new_kern(big_op_spacing5());
            mem[z].hh.v.RH = p;
            mem[v + 2].cint = mem[v + 2].cint + big_op_spacing5() + mem[z + 3].cint + mem[z + 2].cint + shift_down;
        }
        mem[q + 1].cint = v;
    }
    Result = delta;
    return Result;
}

void zmake_ord(halfword q)
{
    make_ord_regmem integer a;
    halfword p, r;
 lab20:/*restart */ if (mem[q + 3].hh.v.RH == 0 /*empty */ ) {

        if (mem[q + 2].hh.v.RH == 0 /*empty */ ) {

            if (mem[q + 1].hh.v.RH == 1 /*math_char */ ) {
                p = mem[q].hh.v.RH;
                if (p != -268435455L) {

                    if ((mem[p].hh.u.B0 >= 16 /*ord_noad */ ) && (mem[p].hh.u.B0 <= 22 /*punct_noad */ )) {

                        if (mem[p + 1].hh.v.RH == 1 /*math_char */ ) {

                            if ((mem[p + 1].hh.u.B0 % 256) == (mem[q + 1].hh.u.B0 % 256)) {
                                mem[q + 1].hh.v.RH = 4 /*math_text_char */ ;
                                fetch(q + 1);
                                if (((cur_i.u.B2) % 4) == 1 /*lig_tag */ ) {
                                    a = lig_kern_base[cur_f] + cur_i.u.B3;
                                    cur_c = mem[p + 1].hh.u.B1;
                                    cur_i = font_info[a].qqqq;
                                    if (cur_i.u.B0 > 128) {
                                        a = lig_kern_base[cur_f] + 256 * cur_i.u.B2 + cur_i.u.B3 + 32768L - 256 * (128);
                                        cur_i = font_info[a].qqqq;
                                    }
                                    while (true) {

                                        if (cur_i.u.B1 == cur_c) {

                                            if (cur_i.u.B0 <= 128) {

                                                if (cur_i.u.B2 >= 128) {
                                                    p = new_kern(font_info
                                                                 [kern_base[cur_f] + 256 * cur_i.u.B2 + cur_i.u.B3].cint);
                                                    mem[p].hh.v.RH = mem[q].hh.v.RH;
                                                    mem[q].hh.v.RH = p;
                                                    return;
                                                } else {

                                                    {
                                                        if (interrupt != 0)
                                                            pause_for_instructions();
                                                    }
                                                    switch (cur_i.u.B2) {
                                                    case 1:
                                                    case 5:
                                                        mem[q + 1].hh.u.B1 = cur_i.u.B3;
                                                        break;
                                                    case 2:
                                                    case 6:
                                                        mem[p + 1].hh.u.B1 = cur_i.u.B3;
                                                        break;
                                                    case 3:
                                                    case 7:
                                                    case 11:
                                                        {
                                                            r = new_noad();
                                                            mem[r + 1].hh.u.B1 = cur_i.u.B3;
                                                            mem[r + 1].hh.u.B0 = (mem[q + 1].hh.u.B0 % 256);
                                                            mem[q].hh.v.RH = r;
                                                            mem[r].hh.v.RH = p;
                                                            if (cur_i.u.B2 < 11)
                                                                mem[r + 1].hh.v.RH = 1 /*math_char */ ;
                                                            else
                                                                mem[r + 1].hh.v.RH = 4 /*math_text_char */ ;
                                                        }
                                                        break;
                                                    default:
                                                        {
                                                            mem[q].hh.v.RH = mem[p].hh.v.RH;
                                                            mem[q + 1].hh.u.B1 = cur_i.u.B3;
                                                            mem[q + 3] = mem[p + 3];
                                                            mem[q + 2] = mem[p + 2];
                                                            free_node(p, 4 /*noad_size */ );
                                                        }
                                                        break;
                                                    }
                                                    if (cur_i.u.B2 > 3)
                                                        return;
                                                    mem[q + 1].hh.v.RH = 1 /*math_char */ ;
                                                    goto lab20;
                                                }
                                            }
                                        }
                                        if (cur_i.u.B0 >= 128)
                                            return;
                                        a = a + cur_i.u.B0 + 1;
                                        cur_i = font_info[a].qqqq;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

halfword zattach_hkern_to_new_hlist(halfword q, scaled delta)
{
    register halfword Result;
    attach_hkern_to_new_hlist_regmem halfword y, z;
    z = new_kern(delta);
    if (mem[q + 1].cint == -268435455L)
        mem[q + 1].cint = z;
    else {

        y = mem[q + 1].cint;
        while (mem[y].hh.v.RH != -268435455L)
            y = mem[y].hh.v.RH;
        mem[y].hh.v.RH = z;
    }
    Result = mem[q + 1].cint;
    return Result;
}

void zmake_scripts(halfword q, scaled delta)
{
    make_scripts_regmem halfword p, x, y, z;
    scaled shift_up, shift_down, clr, sub_kern, sup_kern;
    halfword script_c;
    quarterword script_g;
    internal_font_number script_f;
    integer t;
    internal_font_number save_f;
    p = mem[q + 1].cint;
    script_c = -268435455L;
    script_g = 0;
    script_f = 0;
    sup_kern = 0;
    sub_kern = 0;
    if ((p >= hi_mem_min)
        ||
        (((p) != -268435455L && (!(p >= hi_mem_min)) && (mem[p].hh.u.B0 == 8 /*whatsit_node */ )
          && (mem[p].hh.u.B1 == 42 /*glyph_node */ )))) {
        shift_up = 0;
        shift_down = 0;
    } else {

        z = hpack(p, 0, 1 /*additional */ );
        if (cur_style < 4 /*script_style */ )
            t = 256 /*script_size */ ;
        else
            t = 512 /*script_script_size */ ;
        shift_up = mem[z + 3].cint - sup_drop(t);
        shift_down = mem[z + 2].cint + sub_drop(t);
        free_node(z, 8 /*box_node_size */ );
    }
    if (mem[q + 2].hh.v.RH == 0 /*empty */ ) {  /*784: */
        save_f = cur_f;
        x = clean_box(q + 3, 2 * (cur_style / 4) + 5);
        cur_f = save_f;
        mem[x + 1].cint = mem[x + 1].cint + eqtb[10053204L /*dimen_base 12 */ ].cint;
        if (shift_down < sub1(cur_size))
            shift_down = sub1(cur_size);
        if (((font_area[cur_f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[cur_f]))))
            clr = mem[x + 3].cint - get_ot_math_constant(cur_f, 9 /*subscriptTopMax */ );
        else
            clr = mem[x + 3].cint - (abs(math_x_height(cur_size) * 4) / 5);
        if (shift_down < clr)
            shift_down = clr;
        mem[x + 4].cint = shift_down;
        if (((font_area[cur_f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[cur_f])))) {   /*787: */
            if (mem[q + 3].hh.v.RH == 1 /*math_char */ ) {
                save_f = cur_f;
                fetch(q + 3);
                if (((font_area[cur_f] == 65534L /*otgr_font_flag */ )
                     && (isOpenTypeMathFont(font_layout_engine[cur_f])))) {
                    script_c = new_native_character(cur_f, cur_c);
                    script_g = get_native_glyph(script_c, 0);
                    script_f = cur_f;
                } else {

                    script_g = 0;
                    script_f = 0;
                }
                cur_f = save_f;
            }
            if ((((p) != -268435455L && (!(p >= hi_mem_min)) && (mem[p].hh.u.B0 == 8 /*whatsit_node */ )
                  && (mem[p].hh.u.B1 == 42 /*glyph_node */ ))))
                sub_kern =
                    get_ot_math_kern(mem[p + 4].qqqq.u.B1, mem[p + 4].qqqq.u.B2, script_f, script_g, 1 /*sub_cmd */ ,
                                     shift_down);
            if (sub_kern != 0)
                p = attach_hkern_to_new_hlist(q, sub_kern);
        }
    } else {

        {
            save_f = cur_f;
            x = clean_box(q + 2, 2 * (cur_style / 4) + 4 + (cur_style % 2));
            cur_f = save_f;
            mem[x + 1].cint = mem[x + 1].cint + eqtb[10053204L /*dimen_base 12 */ ].cint;
            if (odd(cur_style))
                clr = sup3(cur_size);
            else if (cur_style < 2 /*text_style */ )
                clr = sup1(cur_size);
            else
                clr = sup2(cur_size);
            if (shift_up < clr)
                shift_up = clr;
            if (((font_area[cur_f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[cur_f]))))
                clr = mem[x + 2].cint + get_ot_math_constant(cur_f, 13 /*superscriptBottomMin */ );
            else
                clr = mem[x + 2].cint + (abs(math_x_height(cur_size)) / 4);
            if (shift_up < clr)
                shift_up = clr;
            if (((font_area[cur_f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[cur_f])))) {       /*788: */
                if (mem[q + 2].hh.v.RH == 1 /*math_char */ ) {
                    save_f = cur_f;
                    fetch(q + 2);
                    if (((font_area[cur_f] == 65534L /*otgr_font_flag */ )
                         && (isOpenTypeMathFont(font_layout_engine[cur_f])))) {
                        script_c = new_native_character(cur_f, cur_c);
                        script_g = get_native_glyph(script_c, 0);
                        script_f = cur_f;
                    } else {

                        script_g = 0;
                        script_f = 0;
                    }
                    cur_f = save_f;
                }
                if ((((p) != -268435455L && (!(p >= hi_mem_min)) && (mem[p].hh.u.B0 == 8 /*whatsit_node */ )
                      && (mem[p].hh.u.B1 == 42 /*glyph_node */ ))))
                    sup_kern =
                        get_ot_math_kern(mem[p + 4].qqqq.u.B1, mem[p + 4].qqqq.u.B2, script_f, script_g, 0 /*sup_cmd */ ,
                                         shift_up);
                if ((sup_kern != 0) && (mem[q + 3].hh.v.RH == 0 /*empty */ ))
                    p = attach_hkern_to_new_hlist(q, sup_kern);
            }
        }
        if (mem[q + 3].hh.v.RH == 0 /*empty */ )
            mem[x + 4].cint = -(integer) shift_up;
        else {                  /*786: */

            save_f = cur_f;
            y = clean_box(q + 3, 2 * (cur_style / 4) + 5);
            cur_f = save_f;
            mem[y + 1].cint = mem[y + 1].cint + eqtb[10053204L /*dimen_base 12 */ ].cint;
            if (shift_down < sub2(cur_size))
                shift_down = sub2(cur_size);
            if (((font_area[cur_f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[cur_f]))))
                clr =
                    get_ot_math_constant(cur_f,
                                         15 /*subSuperscriptGapMin */ ) - ((shift_up - mem[x + 2].cint) -
                                                                           (mem[y + 3].cint - shift_down));
            else
                clr = 4 * default_rule_thickness() - ((shift_up - mem[x + 2].cint) - (mem[y + 3].cint - shift_down));
            if (clr > 0) {
                shift_down = shift_down + clr;
                if (((font_area[cur_f] == 65534L /*otgr_font_flag */ )
                     && (isOpenTypeMathFont(font_layout_engine[cur_f]))))
                    clr =
                        get_ot_math_constant(cur_f,
                                             16 /*superscriptBottomMaxWithSubscript */ ) - (shift_up - mem[x + 2].cint);
                else
                    clr = (abs(math_x_height(cur_size) * 4) / 5) - (shift_up - mem[x + 2].cint);
                if (clr > 0) {
                    shift_up = shift_up + clr;
                    shift_down = shift_down - clr;
                }
            }
            if (((font_area[cur_f] == 65534L /*otgr_font_flag */ ) && (isOpenTypeMathFont(font_layout_engine[cur_f])))) {
                {
                    if (mem[q + 3].hh.v.RH == 1 /*math_char */ ) {
                        save_f = cur_f;
                        fetch(q + 3);
                        if (((font_area[cur_f] == 65534L /*otgr_font_flag */ )
                             && (isOpenTypeMathFont(font_layout_engine[cur_f])))) {
                            script_c = new_native_character(cur_f, cur_c);
                            script_g = get_native_glyph(script_c, 0);
                            script_f = cur_f;
                        } else {

                            script_g = 0;
                            script_f = 0;
                        }
                        cur_f = save_f;
                    }
                    if ((((p) != -268435455L && (!(p >= hi_mem_min)) && (mem[p].hh.u.B0 == 8 /*whatsit_node */ )
                          && (mem[p].hh.u.B1 == 42 /*glyph_node */ ))))
                        sub_kern =
                            get_ot_math_kern(mem[p + 4].qqqq.u.B1, mem[p + 4].qqqq.u.B2, script_f, script_g,
                                             1 /*sub_cmd */ , shift_down);
                    if (sub_kern != 0)
                        p = attach_hkern_to_new_hlist(q, sub_kern);
                }
                {
                    if (mem[q + 2].hh.v.RH == 1 /*math_char */ ) {
                        save_f = cur_f;
                        fetch(q + 2);
                        if (((font_area[cur_f] == 65534L /*otgr_font_flag */ )
                             && (isOpenTypeMathFont(font_layout_engine[cur_f])))) {
                            script_c = new_native_character(cur_f, cur_c);
                            script_g = get_native_glyph(script_c, 0);
                            script_f = cur_f;
                        } else {

                            script_g = 0;
                            script_f = 0;
                        }
                        cur_f = save_f;
                    }
                    if ((((p) != -268435455L && (!(p >= hi_mem_min)) && (mem[p].hh.u.B0 == 8 /*whatsit_node */ )
                          && (mem[p].hh.u.B1 == 42 /*glyph_node */ ))))
                        sup_kern =
                            get_ot_math_kern(mem[p + 4].qqqq.u.B1, mem[p + 4].qqqq.u.B2, script_f, script_g,
                                             0 /*sup_cmd */ , shift_up);
                    if ((sup_kern != 0) && (mem[q + 3].hh.v.RH == 0 /*empty */ ))
                        p = attach_hkern_to_new_hlist(q, sup_kern);
                }
            }
            mem[x + 4].cint = sup_kern + delta - sub_kern;
            p = new_kern((shift_up - mem[x + 2].cint) - (mem[y + 3].cint - shift_down));
            mem[x].hh.v.RH = p;
            mem[p].hh.v.RH = y;
            x = vpackage(x, 0, 1 /*additional */ , 1073741823L);
            mem[x + 4].cint = shift_down;
        }
    }
    if (mem[q + 1].cint == -268435455L)
        mem[q + 1].cint = x;
    else {

        p = mem[q + 1].cint;
        while (mem[p].hh.v.RH != -268435455L)
            p = mem[p].hh.v.RH;
        mem[p].hh.v.RH = x;
    }
}

small_number zmake_left_right(halfword q, small_number style, scaled max_d, scaled max_h)
{
    register small_number Result;
    make_left_right_regmem scaled delta, delta1, delta2;
    cur_style = style;
    {
        if (cur_style < 4 /*script_style */ )
            cur_size = 0 /*text_size */ ;
        else
            cur_size = 256 /*script_size */  * ((cur_style - 2) / 2);
        cur_mu = x_over_n(math_quad(cur_size), 18);
    }
    delta2 = max_d + axis_height(cur_size);
    delta1 = max_h + max_d - delta2;
    if (delta2 > delta1)
        delta1 = delta2;
    delta = (delta1 / 500) * eqtb[8938758L /*int_base 18 */ ].cint;
    delta2 = delta1 + delta1 - eqtb[10053202L /*dimen_base 10 */ ].cint;
    if (delta < delta2)
        delta = delta2;
    mem[q + 1].cint = var_delimiter(q + 1, cur_size, delta);
    Result = mem[q].hh.u.B0 - (10 /*left_noad -20 */ );
    return Result;
}

void mlist_to_hlist(void)
{
    mlist_to_hlist_regmem halfword mlist;
    boolean penalties;
    small_number style;
    small_number save_style;
    halfword q;
    halfword r;
    small_number r_type;
    small_number t;
    halfword p, x, y, z;
    integer pen;
    small_number s;
    scaled max_h, max_d;
    scaled delta;
    mlist = cur_mlist;
    penalties = mlist_penalties;
    style = cur_style;
    q = mlist;
    r = -268435455L;
    r_type = 17 /*op_noad */ ;
    max_h = 0;
    max_d = 0;
    {
        if (cur_style < 4 /*script_style */ )
            cur_size = 0 /*text_size */ ;
        else
            cur_size = 256 /*script_size */  * ((cur_style - 2) / 2);
        cur_mu = x_over_n(math_quad(cur_size), 18);
    }
    while (q != -268435455L) {  /*753: */

 lab21:                        /*reswitch */ delta = 0;
        switch (mem[q].hh.u.B0) {
        case 18:
            switch (r_type) {
            case 18:
            case 17:
            case 19:
            case 20:
            case 22:
            case 30:
                {
                    mem[q].hh.u.B0 = 16 /*ord_noad */ ;
                    goto lab21;
                }
                break;
            default:
                ;
                break;
            }
            break;
        case 19:
        case 21:
        case 22:
        case 31:
            {
                if (r_type == 18 /*bin_noad */ )
                    mem[r].hh.u.B0 = 16 /*ord_noad *//*:755 */ ;
                if (mem[q].hh.u.B0 == 31 /*right_noad */ )
                    goto lab80;
            }
            break;
        case 30:
            goto lab80;
            break;
        case 25:
            {
                make_fraction(q);
                goto lab82;
            }
            break;
        case 17:
            {
                delta = make_op(q);
                if (mem[q].hh.u.B1 == 1 /*limits */ )
                    goto lab82;
            }
            break;
        case 16:
            make_ord(q);
            break;
        case 20:
        case 23:
            ;
            break;
        case 24:
            make_radical(q);
            break;
        case 27:
            make_over(q);
            break;
        case 26:
            make_under(q);
            break;
        case 28:
            make_math_accent(q);
            break;
        case 29:
            make_vcenter(q);
            break;
        case 14:
            {
                cur_style = mem[q].hh.u.B1;
                {
                    if (cur_style < 4 /*script_style */ )
                        cur_size = 0 /*text_size */ ;
                    else
                        cur_size = 256 /*script_size */  * ((cur_style - 2) / 2);
                    cur_mu = x_over_n(math_quad(cur_size), 18);
                }
                goto lab81;
            }
            break;
        case 15:
            {
                switch (cur_style / 2) {
                case 0:
                    {
                        p = mem[q + 1].hh.v.LH;
                        mem[q + 1].hh.v.LH = -268435455L;
                    }
                    break;
                case 1:
                    {
                        p = mem[q + 1].hh.v.RH;
                        mem[q + 1].hh.v.RH = -268435455L;
                    }
                    break;
                case 2:
                    {
                        p = mem[q + 2].hh.v.LH;
                        mem[q + 2].hh.v.LH = -268435455L;
                    }
                    break;
                case 3:
                    {
                        p = mem[q + 2].hh.v.RH;
                        mem[q + 2].hh.v.RH = -268435455L;
                    }
                    break;
                }
                flush_node_list(mem[q + 1].hh.v.LH);
                flush_node_list(mem[q + 1].hh.v.RH);
                flush_node_list(mem[q + 2].hh.v.LH);
                flush_node_list(mem[q + 2].hh.v.RH);
                mem[q].hh.u.B0 = 14 /*style_node */ ;
                mem[q].hh.u.B1 = cur_style;
                mem[q + 1].cint = 0;
                mem[q + 2].cint = 0;
                if (p != -268435455L) {
                    z = mem[q].hh.v.RH;
                    mem[q].hh.v.RH = p;
                    while (mem[p].hh.v.RH != -268435455L)
                        p = mem[p].hh.v.RH;
                    mem[p].hh.v.RH = z;
                }
                goto lab81;
            }
            break;
        case 3:
        case 4:
        case 5:
        case 8:
        case 12:
        case 7:
            goto lab81;
            break;
        case 2:
            {
                if (mem[q + 3].cint > max_h)
                    max_h = mem[q + 3].cint;
                if (mem[q + 2].cint > max_d)
                    max_d = mem[q + 2].cint;
                goto lab81;
            }
            break;
        case 10:
            {
                if (mem[q].hh.u.B1 == 99 /*mu_glue */ ) {
                    x = mem[q + 1].hh.v.LH;
                    y = math_glue(x, cur_mu);
                    delete_glue_ref(x);
                    mem[q + 1].hh.v.LH = y;
                    mem[q].hh.u.B1 = 0 /*normal */ ;
                } else if ((cur_size != 0 /*text_size */ ) && (mem[q].hh.u.B1 == 98 /*cond_math_glue */ )) {
                    p = mem[q].hh.v.RH;
                    if (p != -268435455L) {

                        if ((mem[p].hh.u.B0 == 10 /*glue_node */ ) || (mem[p].hh.u.B0 == 11 /*kern_node */ )) {
                            mem[q].hh.v.RH = mem[p].hh.v.RH;
                            mem[p].hh.v.RH = -268435455L;
                            flush_node_list(p);
                        }
                    }
                }
                goto lab81;
            }
            break;
        case 11:
            {
                math_kern(q, cur_mu);
                goto lab81;
            }
            break;
        default:
            confusion(66279L /*"mlist1" */ );
            break;
        }
        switch (mem[q + 1].hh.v.RH) {
        case 1:
        case 4:
            {
                fetch(q + 1);
                if (((font_area[cur_f] == 65535L /*aat_font_flag */ )
                     || (font_area[cur_f] == 65534L /*otgr_font_flag */ ))) {
                    z = new_native_character(cur_f, cur_c);
                    p = get_node(5 /*glyph_node_size */ );
                    mem[p].hh.u.B0 = 8 /*whatsit_node */ ;
                    mem[p].hh.u.B1 = 42 /*glyph_node */ ;
                    mem[p + 4].qqqq.u.B1 = cur_f;
                    mem[p + 4].qqqq.u.B2 = get_native_glyph(z, 0);
                    set_native_glyph_metrics(p, 1);
                    free_node(z, mem[z + 4].qqqq.u.B0);
                    delta = get_ot_math_ital_corr(cur_f, mem[p + 4].qqqq.u.B2);
                    if ((mem[q + 1].hh.v.RH == 4 /*math_text_char */ )
                        &&
                        (!((font_area[cur_f] == 65534L /*otgr_font_flag */ )
                           && (isOpenTypeMathFont(font_layout_engine[cur_f]))) != 0))
                        delta = 0;
                    if ((mem[q + 3].hh.v.RH == 0 /*empty */ ) && (delta != 0)) {
                        mem[p].hh.v.RH = new_kern(delta);
                        delta = 0;
                    }
                } else if ((cur_i.u.B0 > 0 /*min_quarterword */ )) {
                    delta = font_info[italic_base[cur_f] + (cur_i.u.B2) / 4].cint;
                    p = new_character(cur_f, cur_c);
                    if ((mem[q + 1].hh.v.RH == 4 /*math_text_char */ )
                        && (font_info[2 /*space_code */  + param_base[cur_f]].cint != 0))
                        delta = 0;
                    if ((mem[q + 3].hh.v.RH == 0 /*empty */ ) && (delta != 0)) {
                        mem[p].hh.v.RH = new_kern(delta);
                        delta = 0;
                    }
                } else
                    p = -268435455L;
            }
            break;
        case 0:
            p = -268435455L;
            break;
        case 2:
            p = mem[q + 1].hh.v.LH;
            break;
        case 3:
            {
                cur_mlist = mem[q + 1].hh.v.LH;
                save_style = cur_style;
                mlist_penalties = false;
                mlist_to_hlist();
                cur_style = save_style;
                {
                    if (cur_style < 4 /*script_style */ )
                        cur_size = 0 /*text_size */ ;
                    else
                        cur_size = 256 /*script_size */  * ((cur_style - 2) / 2);
                    cur_mu = x_over_n(math_quad(cur_size), 18);
                }
                p = hpack(mem[mem_top - 3].hh.v.RH, 0, 1 /*additional */ );
            }
            break;
        default:
            confusion(66280L /*"mlist2" */ );
            break;
        }
        mem[q + 1].cint = p;
        if ((mem[q + 3].hh.v.RH == 0 /*empty */ ) && (mem[q + 2].hh.v.RH == 0 /*empty */ ))
            goto lab82;
        make_scripts(q, delta);
 lab82:/*check_dimensions */ z = hpack(mem[q + 1].cint, 0, 1 /*additional */ );
        if (mem[z + 3].cint > max_h)
            max_h = mem[z + 3].cint;
        if (mem[z + 2].cint > max_d)
            max_d = mem[z + 2].cint;
        free_node(z, 8 /*box_node_size */ );
 lab80:                        /*done_with_noad */ r = q;
        r_type = mem[r].hh.u.B0;
        if (r_type == 31 /*right_noad */ ) {
            r_type = 30 /*left_noad */ ;
            cur_style = style;
            {
                if (cur_style < 4 /*script_style */ )
                    cur_size = 0 /*text_size */ ;
                else
                    cur_size = 256 /*script_size */  * ((cur_style - 2) / 2);
                cur_mu = x_over_n(math_quad(cur_size), 18);
            }
        }
 lab81:                        /*done_with_node */ q = mem[q].hh.v.RH;
    }
    if (r_type == 18 /*bin_noad */ )
        mem[r].hh.u.B0 = 16 /*ord_noad *//*:755 */ ;
    p = mem_top - 3;
    mem[p].hh.v.RH = -268435455L;
    q = mlist;
    r_type = 0;
    cur_style = style;
    {
        if (cur_style < 4 /*script_style */ )
            cur_size = 0 /*text_size */ ;
        else
            cur_size = 256 /*script_size */  * ((cur_style - 2) / 2);
        cur_mu = x_over_n(math_quad(cur_size), 18);
    }
    while (q != -268435455L) {

        t = 16 /*ord_noad */ ;
        s = 4 /*noad_size */ ;
        pen = 10000 /*inf_penalty */ ;
        switch (mem[q].hh.u.B0) {
        case 17:
        case 20:
        case 21:
        case 22:
        case 23:
            t = mem[q].hh.u.B0;
            break;
        case 18:
            {
                t = 18 /*bin_noad */ ;
                pen = eqtb[8938749L /*int_base 9 */ ].cint;
            }
            break;
        case 19:
            {
                t = 19 /*rel_noad */ ;
                pen = eqtb[8938750L /*int_base 10 */ ].cint;
            }
            break;
        case 16:
        case 29:
        case 27:
        case 26:
            ;
            break;
        case 24:
            s = 5 /*radical_noad_size */ ;
            break;
        case 28:
            s = 5 /*accent_noad_size */ ;
            break;
        case 25:
            {
                t = 23 /*inner_noad */ ;
                s = 6 /*fraction_noad_size */ ;
            }
            break;
        case 30:
        case 31:
            t = make_left_right(q, style, max_d, max_h);
            break;
        case 14:
            {
                cur_style = mem[q].hh.u.B1;
                s = 3 /*style_node_size */ ;
                {
                    if (cur_style < 4 /*script_style */ )
                        cur_size = 0 /*text_size */ ;
                    else
                        cur_size = 256 /*script_size */  * ((cur_style - 2) / 2);
                    cur_mu = x_over_n(math_quad(cur_size), 18);
                }
                goto lab83;
            }
            break;
        case 8:
        case 12:
        case 2:
        case 7:
        case 5:
        case 3:
        case 4:
        case 10:
        case 11:
            {
                mem[p].hh.v.RH = q;
                p = q;
                q = mem[q].hh.v.RH;
                mem[p].hh.v.RH = -268435455L;
                goto lab30;
            }
            break;
        default:
            confusion(66281L /*"mlist3" */ );
            break;
        }
        if (r_type > 0) {
            switch (str_pool[r_type * 8 + t + magic_offset]) {
            case 48:
                x = 0;
                break;
            case 49:
                if (cur_style < 4 /*script_style */ )
                    x = 16 /*thin_mu_skip_code */ ;
                else
                    x = 0;
                break;
            case 50:
                x = 16 /*thin_mu_skip_code */ ;
                break;
            case 51:
                if (cur_style < 4 /*script_style */ )
                    x = 17 /*med_mu_skip_code */ ;
                else
                    x = 0;
                break;
            case 52:
                if (cur_style < 4 /*script_style */ )
                    x = 18 /*thick_mu_skip_code */ ;
                else
                    x = 0;
                break;
            default:
                confusion(66283L /*"mlist4" */ );
                break;
            }
            if (x != 0) {
                y = math_glue(eqtb[2252240L /*glue_base */  + x].hh.v.RH, cur_mu);
                z = new_glue(y);
                mem[y].hh.v.RH = -268435455L;
                mem[p].hh.v.RH = z;
                p = z;
                mem[z].hh.u.B1 = x + 1;
            }
        }
        if (mem[q + 1].cint != -268435455L) {
            mem[p].hh.v.RH = mem[q + 1].cint;
            do {
                p = mem[p].hh.v.RH;
            } while (!(mem[p].hh.v.RH == -268435455L));
        }
        if (penalties) {

            if (mem[q].hh.v.RH != -268435455L) {

                if (pen < 10000 /*inf_penalty */ ) {
                    r_type = mem[mem[q].hh.v.RH].hh.u.B0;
                    if (r_type != 12 /*penalty_node */ ) {

                        if (r_type != 19 /*rel_noad */ ) {
                            z = new_penalty(pen);
                            mem[p].hh.v.RH = z;
                            p = z;
                        }
                    }
                }
            }
        }
        if (mem[q].hh.u.B0 == 31 /*right_noad */ )
            t = 20 /*open_noad */ ;
        r_type = t;
 lab83:                        /*delete_q */ r = q;
        q = mem[q].hh.v.RH;
        free_node(r, s);
 lab30:                        /*done */ ;
    }
}

void push_alignment(void)
{
    push_alignment_regmem halfword p;
    p = get_node(6 /*align_stack_node_size */ );
    mem[p].hh.v.RH = align_ptr;
    mem[p].hh.v.LH = cur_align;
    mem[p + 1].hh.v.LH = mem[mem_top - 8].hh.v.RH;
    mem[p + 1].hh.v.RH = cur_span;
    mem[p + 2].cint = cur_loop;
    mem[p + 3].cint = align_state;
    mem[p + 4].hh.v.LH = cur_head;
    mem[p + 4].hh.v.RH = cur_tail;
    mem[p + 5].hh.v.LH = cur_pre_head;
    mem[p + 5].hh.v.RH = cur_pre_tail;
    align_ptr = p;
    cur_head = get_avail();
    cur_pre_head = get_avail();
}

void pop_alignment(void)
{
    pop_alignment_regmem halfword p;
    {
        mem[cur_head].hh.v.RH = avail;
        avail = cur_head;
    }
    {
        mem[cur_pre_head].hh.v.RH = avail;
        avail = cur_pre_head;
    }
    p = align_ptr;
    cur_tail = mem[p + 4].hh.v.RH;
    cur_head = mem[p + 4].hh.v.LH;
    cur_pre_tail = mem[p + 5].hh.v.RH;
    cur_pre_head = mem[p + 5].hh.v.LH;
    align_state = mem[p + 3].cint;
    cur_loop = mem[p + 2].cint;
    cur_span = mem[p + 1].hh.v.RH;
    mem[mem_top - 8].hh.v.RH = mem[p + 1].hh.v.LH;
    cur_align = mem[p].hh.v.LH;
    align_ptr = mem[p].hh.v.RH;
    free_node(p, 6 /*align_stack_node_size */ );
}

void get_preamble_token(void)
{
 get_preamble_token_regmem lab20:      /*restart */ get_token();
    while ((cur_chr == 65537L /*span_code */ ) && (cur_cmd == 4 /*tab_mark */ )) {

        get_token();
        if (cur_cmd > 102 /*max_command */ ) {
            expand();
            get_token();
        }
    }
    if (cur_cmd == 9 /*endv */ )
        fatal_error(65915L /*"(interwoven alignment preambles are not allowed)" */ );
    if ((cur_cmd == 76 /*assign_glue */ ) && (cur_chr == 2252251L /*glue_base 11 */ )) {
        scan_optional_equals();
        scan_glue(2 /*glue_val */ );
        if (eqtb[8938783L /*int_base 43 */ ].cint > 0)
            geq_define(2252251L /*glue_base 11 */ , 119 /*glue_ref */ , cur_val);
        else
            eq_define(2252251L /*glue_base 11 */ , 119 /*glue_ref */ , cur_val);
        goto lab20;
    }
}

void init_align(void)
{
    init_align_regmem halfword save_cs_ptr;
    halfword p;
    save_cs_ptr = cur_cs;
    push_alignment();
    align_state = -1000000L;
    if ((cur_list.mode_field == 207 /*mmode */ )
        && ((cur_list.tail_field != cur_list.head_field) || (cur_list.aux_field.cint != -268435455L))) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66015L /*"Improper " */ );
        }
        print_esc(65827L /*"halign" */ );
        print(66284L /*" inside $$'s" */ );
        {
            help_ptr = 3;
            help_line[2] = 66285L /*"Displays can use special alignments (like \eqalignno)" */ ;
            help_line[1] = 66286L /*"only if nothing but the alignment itself is between $$'s." */ ;
            help_line[0] = 66287L /*"So I've deleted the formulas that preceded this alignment." */ ;
        }
        error();
        flush_math();
    }
    push_nest();
    if (cur_list.mode_field == 207 /*mmode */ ) {
        cur_list.mode_field = -1;
        cur_list.aux_field.cint = nest[nest_ptr - 2].aux_field.cint;
    } else if (cur_list.mode_field > 0)
        cur_list.mode_field = -(integer) cur_list.mode_field /*:804 */ ;
    scan_spec(6 /*align_group */ , false);
    mem[mem_top - 8].hh.v.RH = -268435455L;
    cur_align = mem_top - 8;
    cur_loop = -268435455L;
    scanner_status = 4 /*aligning */ ;
    warning_index = save_cs_ptr;
    align_state = -1000000L;
    while (true) {

        mem[cur_align].hh.v.RH = new_param_glue(11 /*tab_skip_code */ );
        cur_align = mem[cur_align].hh.v.RH /*:807 */ ;
        if (cur_cmd == 5 /*car_ret */ )
            goto lab30;
        p = mem_top - 4;
        mem[p].hh.v.RH = -268435455L;
        while (true) {

            get_preamble_token();
            if (cur_cmd == 6 /*mac_param */ )
                goto lab31;
            if ((cur_cmd <= 5 /*car_ret */ ) && (cur_cmd >= 4 /*tab_mark */ ) && (align_state == -1000000L)) {

                if ((p == mem_top - 4) && (cur_loop == -268435455L) && (cur_cmd == 4 /*tab_mark */ ))
                    cur_loop = cur_align;
                else {

                    {
                        if (interaction == 3 /*error_stop_mode */ ) ;
                        if (file_line_error_style_p)
                            print_file_line();
                        else
                            print_nl(65544L /*"! " */ );
                        print(66293L /*"Missing # inserted in alignment preamble" */ );
                    }
                    {
                        help_ptr = 3;
                        help_line[2] = 66294L /*"There should be exactly one # between &'s, when an" */ ;
                        help_line[1] = 66295L /*"\halign or \valign is being set up. In this case you had" */ ;
                        help_line[0] = 66296L /*"none, so I've put one in; maybe that will work." */ ;
                    }
                    back_error();
                    goto lab31;
                }
            } else if ((cur_cmd != 10 /*spacer */ ) || (p != mem_top - 4)) {
                mem[p].hh.v.RH = get_avail();
                p = mem[p].hh.v.RH;
                mem[p].hh.v.LH = cur_tok;
            }
        }
 lab31:                        /*done1 *//*:812 */ ;
        mem[cur_align].hh.v.RH = new_null_box();
        cur_align = mem[cur_align].hh.v.RH;
        mem[cur_align].hh.v.LH = mem_top - 9;
        mem[cur_align + 1].cint = -1073741824L;
        mem[cur_align + 3].cint = mem[mem_top - 4].hh.v.RH;
        p = mem_top - 4;
        mem[p].hh.v.RH = -268435455L;
        while (true) {

 lab22:                        /*continue */ get_preamble_token();
            if ((cur_cmd <= 5 /*car_ret */ ) && (cur_cmd >= 4 /*tab_mark */ ) && (align_state == -1000000L))
                goto lab32;
            if (cur_cmd == 6 /*mac_param */ ) {
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66297L /*"Only one # is allowed per tab" */ );
                }
                {
                    help_ptr = 3;
                    help_line[2] = 66294L /*"There should be exactly one # between &'s, when an" */ ;
                    help_line[1] = 66295L /*"\halign or \valign is being set up. In this case you had" */ ;
                    help_line[0] = 66298L /*"more than one, so I'm ignoring all but the first." */ ;
                }
                error();
                goto lab22;
            }
            mem[p].hh.v.RH = get_avail();
            p = mem[p].hh.v.RH;
            mem[p].hh.v.LH = cur_tok;
        }
 lab32:                        /*done2 */ mem[p].hh.v.RH = get_avail();
        p = mem[p].hh.v.RH;
        mem[p].hh.v.LH = 35797662L /*cs_token_flag 2243231 *//*:813 */ ;
        mem[cur_align + 2].cint = mem[mem_top - 4].hh.v.RH /*:808 */ ;
    }
 lab30:/*done */ scanner_status = 0 /*normal *//*:806 */ ;
    new_save_level(6 /*align_group */ );
    if (eqtb[2252779L /*every_cr_loc */ ].hh.v.RH != -268435455L)
        begin_token_list(eqtb[2252779L /*every_cr_loc */ ].hh.v.RH, 14 /*every_cr_text */ );
    align_peek();
}

void zinit_span(halfword p)
{
    init_span_regmem push_nest();
    if (cur_list.mode_field == -104)
        cur_list.aux_field.hh.v.LH = 1000;
    else {

        cur_list.aux_field.cint = -65536000L;
        normal_paragraph();
    }
    cur_span = p;
}

void init_row(void)
{
    init_row_regmem push_nest();
    cur_list.mode_field = (-105) - cur_list.mode_field;
    if (cur_list.mode_field == -104)
        cur_list.aux_field.hh.v.LH = 0;
    else
        cur_list.aux_field.cint = 0;
    {
        mem[cur_list.tail_field].hh.v.RH = new_glue(mem[mem[mem_top - 8].hh.v.RH + 1].hh.v.LH);
        cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
    }
    mem[cur_list.tail_field].hh.u.B1 = 12 /*tab_skip_code 1 */ ;
    cur_align = mem[mem[mem_top - 8].hh.v.RH].hh.v.RH;
    cur_tail = cur_head;
    cur_pre_tail = cur_pre_head;
    init_span(cur_align);
}

void init_col(void)
{
    init_col_regmem mem[cur_align + 5].hh.v.LH = cur_cmd;
    if (cur_cmd == 63 /*omit */ )
        align_state = 0;
    else {

        back_input();
        begin_token_list(mem[cur_align + 3].cint, 1 /*u_template */ );
    }
}

boolean fin_col(void)
{
    register boolean Result;
    fin_col_regmem halfword p;
    halfword q, r;
    halfword s;
    halfword u;
    scaled w;
    glue_ord o;
    halfword n;
    if (cur_align == -268435455L)
        confusion(66299L /*"endv" */ );
    q = mem[cur_align].hh.v.RH;
    if (q == -268435455L)
        confusion(66299L /*"endv" */ );
    if (align_state < 500000L)
        fatal_error(65915L /*"(interwoven alignment preambles are not allowed)" */ );
    p = mem[q].hh.v.RH;
    if ((p == -268435455L) && (mem[cur_align + 5].hh.v.LH < 65538L /*cr_code */ )) {

        if (cur_loop != -268435455L) {  /*822: */
            mem[q].hh.v.RH = new_null_box();
            p = mem[q].hh.v.RH;
            mem[p].hh.v.LH = mem_top - 9;
            mem[p + 1].cint = -1073741824L;
            cur_loop = mem[cur_loop].hh.v.RH;
            q = mem_top - 4;
            r = mem[cur_loop + 3].cint;
            while (r != -268435455L) {

                mem[q].hh.v.RH = get_avail();
                q = mem[q].hh.v.RH;
                mem[q].hh.v.LH = mem[r].hh.v.LH;
                r = mem[r].hh.v.RH;
            }
            mem[q].hh.v.RH = -268435455L;
            mem[p + 3].cint = mem[mem_top - 4].hh.v.RH;
            q = mem_top - 4;
            r = mem[cur_loop + 2].cint;
            while (r != -268435455L) {

                mem[q].hh.v.RH = get_avail();
                q = mem[q].hh.v.RH;
                mem[q].hh.v.LH = mem[r].hh.v.LH;
                r = mem[r].hh.v.RH;
            }
            mem[q].hh.v.RH = -268435455L;
            mem[p + 2].cint = mem[mem_top - 4].hh.v.RH /*:823 */ ;
            cur_loop = mem[cur_loop].hh.v.RH;
            mem[p].hh.v.RH = new_glue(mem[cur_loop + 1].hh.v.LH);
        } else {

            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66300L /*"Extra alignment tab has been changed to " */ );
            }
            print_esc(66289L /*"cr" */ );
            {
                help_ptr = 3;
                help_line[2] = 66301L /*"You have given more \span or & marks than there were" */ ;
                help_line[1] = 66302L /*"in the preamble to the \halign or \valign now in progress." */ ;
                help_line[0] = 66303L /*"So I'll assume that you meant to type \cr instead." */ ;
            }
            mem[cur_align + 5].hh.v.LH = 65538L /*cr_code */ ;
            error();
        }
    }
    if (mem[cur_align + 5].hh.v.LH != 65537L /*span_code */ ) {
        unsave();
        new_save_level(6 /*align_group */ );
        {
            if (cur_list.mode_field == -104) {
                adjust_tail = cur_tail;
                pre_adjust_tail = cur_pre_tail;
                u = hpack(mem[cur_list.head_field].hh.v.RH, 0, 1 /*additional */ );
                w = mem[u + 1].cint;
                cur_tail = adjust_tail;
                adjust_tail = -268435455L;
                cur_pre_tail = pre_adjust_tail;
                pre_adjust_tail = -268435455L;
            } else {

                u = vpackage(mem[cur_list.head_field].hh.v.RH, 0, 1 /*additional */ , 0);
                w = mem[u + 3].cint;
            }
            n = 0 /*min_quarterword */ ;
            if (cur_span != cur_align) {        /*827: */
                q = cur_span;
                do {
                    n++;
                    q = mem[mem[q].hh.v.RH].hh.v.RH;
                } while (!(q == cur_align));
                if (n > 65535L /*max_quarterword */ )
                    confusion(66304L /*"too many spans" */ );
                q = cur_span;
                while (mem[mem[q].hh.v.LH].hh.v.RH < n)
                    q = mem[q].hh.v.LH;
                if (mem[mem[q].hh.v.LH].hh.v.RH > n) {
                    s = get_node(2 /*span_node_size */ );
                    mem[s].hh.v.LH = mem[q].hh.v.LH;
                    mem[s].hh.v.RH = n;
                    mem[q].hh.v.LH = s;
                    mem[s + 1].cint = w;
                } else if (mem[mem[q].hh.v.LH + 1].cint < w)
                    mem[mem[q].hh.v.LH + 1].cint = w;
            } else if (w > mem[cur_align + 1].cint)
                mem[cur_align + 1].cint = w;
            mem[u].hh.u.B0 = 13 /*unset_node */ ;
            mem[u].hh.u.B1 = n;
            if (total_stretch[3 /*filll */ ] != 0)
                o = 3 /*filll */ ;
            else if (total_stretch[2 /*fill */ ] != 0)
                o = 2 /*fill */ ;
            else if (total_stretch[1 /*fil */ ] != 0)
                o = 1 /*fil */ ;
            else
                o = 0 /*normal *//*:684 */ ;
            mem[u + 5].hh.u.B1 = o;
            mem[u + 6].cint = total_stretch[o];
            if (total_shrink[3 /*filll */ ] != 0)
                o = 3 /*filll */ ;
            else if (total_shrink[2 /*fill */ ] != 0)
                o = 2 /*fill */ ;
            else if (total_shrink[1 /*fil */ ] != 0)
                o = 1 /*fil */ ;
            else
                o = 0 /*normal *//*:690 */ ;
            mem[u + 5].hh.u.B0 = o;
            mem[u + 4].cint = total_shrink[o];
            pop_nest();
            mem[cur_list.tail_field].hh.v.RH = u;
            cur_list.tail_field = u;
        }
        {
            mem[cur_list.tail_field].hh.v.RH = new_glue(mem[mem[cur_align].hh.v.RH + 1].hh.v.LH);
            cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
        }
        mem[cur_list.tail_field].hh.u.B1 = 12 /*tab_skip_code 1 *//*:824 */ ;
        if (mem[cur_align + 5].hh.v.LH >= 65538L /*cr_code */ ) {
            Result = true;
            return Result;
        }
        init_span(p);
    }
    align_state = 1000000L;
    do {
        get_x_or_protected();
    } while (!(cur_cmd != 10 /*spacer */ ));
    cur_align = p;
    init_col();
    Result = false;
    return Result;
}

void fin_row(void)
{
    fin_row_regmem halfword p;
    if (cur_list.mode_field == -104) {
        p = hpack(mem[cur_list.head_field].hh.v.RH, 0, 1 /*additional */ );
        pop_nest();
        if (cur_pre_head != cur_pre_tail) {
            mem[cur_list.tail_field].hh.v.RH = mem[cur_pre_head].hh.v.RH;
            cur_list.tail_field = cur_pre_tail;
        }
        append_to_vlist(p);
        if (cur_head != cur_tail) {
            mem[cur_list.tail_field].hh.v.RH = mem[cur_head].hh.v.RH;
            cur_list.tail_field = cur_tail;
        }
    } else {

        p = vpackage(mem[cur_list.head_field].hh.v.RH, 0, 1 /*additional */ , 1073741823L);
        pop_nest();
        mem[cur_list.tail_field].hh.v.RH = p;
        cur_list.tail_field = p;
        cur_list.aux_field.hh.v.LH = 1000;
    }
    mem[p].hh.u.B0 = 13 /*unset_node */ ;
    mem[p + 6].cint = 0;
    if (eqtb[2252779L /*every_cr_loc */ ].hh.v.RH != -268435455L)
        begin_token_list(eqtb[2252779L /*every_cr_loc */ ].hh.v.RH, 14 /*every_cr_text */ );
    align_peek();
}

void fin_align(void)
{
    fin_align_regmem halfword p, q, r, s, u, v;
    scaled t, w;
    scaled o;
    halfword n;
    scaled rule_save;
    memory_word aux_save;
    if (cur_group != 6 /*align_group */ )
        confusion(66305L /*"align1" */ );
    unsave();
    if (cur_group != 6 /*align_group */ )
        confusion(66306L /*"align0" */ );
    unsave();
    if (nest[nest_ptr - 1].mode_field == 207 /*mmode */ )
        o = eqtb[10053207L /*dimen_base 15 */ ].cint;
    else
        o = 0;
    q = mem[mem[mem_top - 8].hh.v.RH].hh.v.RH;
    do {
        flush_list(mem[q + 3].cint);
        flush_list(mem[q + 2].cint);
        p = mem[mem[q].hh.v.RH].hh.v.RH;
        if (mem[q + 1].cint == -1073741824L) {  /*831: */
            mem[q + 1].cint = 0;
            r = mem[q].hh.v.RH;
            s = mem[r + 1].hh.v.LH;
            if (s != mem_bot) {
                mem[mem_bot].hh.v.RH++;
                delete_glue_ref(s);
                mem[r + 1].hh.v.LH = mem_bot;
            }
        }
        if (mem[q].hh.v.LH != mem_top - 9) {    /*832: */
            t = mem[q + 1].cint + mem[mem[mem[q].hh.v.RH + 1].hh.v.LH + 1].cint;
            r = mem[q].hh.v.LH;
            s = mem_top - 9;
            mem[s].hh.v.LH = p;
            n = 1 /*min_quarterword 1 */ ;
            do {
                mem[r + 1].cint = mem[r + 1].cint - t;
                u = mem[r].hh.v.LH;
                while (mem[r].hh.v.RH > n) {

                    s = mem[s].hh.v.LH;
                    n = mem[mem[s].hh.v.LH].hh.v.RH + 1;
                }
                if (mem[r].hh.v.RH < n) {
                    mem[r].hh.v.LH = mem[s].hh.v.LH;
                    mem[s].hh.v.LH = r;
                    mem[r].hh.v.RH--;
                    s = r;
                } else {

                    if (mem[r + 1].cint > mem[mem[s].hh.v.LH + 1].cint)
                        mem[mem[s].hh.v.LH + 1].cint = mem[r + 1].cint;
                    free_node(r, 2 /*span_node_size */ );
                }
                r = u;
            } while (!(r == mem_top - 9));
        }
        mem[q].hh.u.B0 = 13 /*unset_node */ ;
        mem[q].hh.u.B1 = 0 /*min_quarterword */ ;
        mem[q + 3].cint = 0;
        mem[q + 2].cint = 0;
        mem[q + 5].hh.u.B1 = 0 /*normal */ ;
        mem[q + 5].hh.u.B0 = 0 /*normal */ ;
        mem[q + 6].cint = 0;
        mem[q + 4].cint = 0;
        q = p;
    } while (!(q == -268435455L /*:830 */ ));
    save_ptr = save_ptr - 2;
    pack_begin_line = -(integer) cur_list.ml_field;
    if (cur_list.mode_field == -1) {
        rule_save = eqtb[10053208L /*dimen_base 16 */ ].cint;
        eqtb[10053208L /*dimen_base 16 */ ].cint = 0;
        p = hpack(mem[mem_top - 8].hh.v.RH, save_stack[save_ptr + 1].cint, save_stack[save_ptr + 0].cint);
        eqtb[10053208L /*dimen_base 16 */ ].cint = rule_save;
    } else {

        q = mem[mem[mem_top - 8].hh.v.RH].hh.v.RH;
        do {
            mem[q + 3].cint = mem[q + 1].cint;
            mem[q + 1].cint = 0;
            q = mem[mem[q].hh.v.RH].hh.v.RH;
        } while (!(q == -268435455L));
        p = vpackage(mem[mem_top - 8].hh.v.RH, save_stack[save_ptr + 1].cint, save_stack[save_ptr + 0].cint,
                     1073741823L);
        q = mem[mem[mem_top - 8].hh.v.RH].hh.v.RH;
        do {
            mem[q + 1].cint = mem[q + 3].cint;
            mem[q + 3].cint = 0;
            q = mem[mem[q].hh.v.RH].hh.v.RH;
        } while (!(q == -268435455L));
    }
    pack_begin_line = 0 /*:833 */ ;
    q = mem[cur_list.head_field].hh.v.RH;
    s = cur_list.head_field;
    while (q != -268435455L) {

        if (!(q >= hi_mem_min)) {

            if (mem[q].hh.u.B0 == 13 /*unset_node */ ) {  /*836: */
                if (cur_list.mode_field == -1) {
                    mem[q].hh.u.B0 = 0 /*hlist_node */ ;
                    mem[q + 1].cint = mem[p + 1].cint;
                    if (nest[nest_ptr - 1].mode_field == 207 /*mmode */ )
                        mem[q].hh.u.B1 = 2 /*dlist */ ;
                } else {

                    mem[q].hh.u.B0 = 1 /*vlist_node */ ;
                    mem[q + 3].cint = mem[p + 3].cint;
                }
                mem[q + 5].hh.u.B1 = mem[p + 5].hh.u.B1;
                mem[q + 5].hh.u.B0 = mem[p + 5].hh.u.B0;
                mem[q + 6].gr = mem[p + 6].gr;
                mem[q + 4].cint = o;
                r = mem[mem[q + 5].hh.v.RH].hh.v.RH;
                s = mem[mem[p + 5].hh.v.RH].hh.v.RH;
                do {
                    /*837: */ n = mem[r].hh.u.B1;
                    t = mem[s + 1].cint;
                    w = t;
                    u = mem_top - 4;
                    mem[r].hh.u.B1 = 0;
                    while (n > 0 /*min_quarterword */ ) {

                        n--;
                        s = mem[s].hh.v.RH;
                        v = mem[s + 1].hh.v.LH;
                        mem[u].hh.v.RH = new_glue(v);
                        u = mem[u].hh.v.RH;
                        mem[u].hh.u.B1 = 12 /*tab_skip_code 1 */ ;
                        t = t + mem[v + 1].cint;
                        if (mem[p + 5].hh.u.B0 == 1 /*stretching */ ) {
                            if (mem[v].hh.u.B0 == mem[p + 5].hh.u.B1)
                                t = t + round(mem[p + 6].gr * mem[v + 2].cint);
                        } else if (mem[p + 5].hh.u.B0 == 2 /*shrinking */ ) {
                            if (mem[v].hh.u.B1 == mem[p + 5].hh.u.B1)
                                t = t - round(mem[p + 6].gr * mem[v + 3].cint);
                        }
                        s = mem[s].hh.v.RH;
                        mem[u].hh.v.RH = new_null_box();
                        u = mem[u].hh.v.RH;
                        t = t + mem[s + 1].cint;
                        if (cur_list.mode_field == -1)
                            mem[u + 1].cint = mem[s + 1].cint;
                        else {

                            mem[u].hh.u.B0 = 1 /*vlist_node */ ;
                            mem[u + 3].cint = mem[s + 1].cint;
                        }
                    }
                    if (cur_list.mode_field == -1) {    /*839: */
                        mem[r + 3].cint = mem[q + 3].cint;
                        mem[r + 2].cint = mem[q + 2].cint;
                        if (t == mem[r + 1].cint) {
                            mem[r + 5].hh.u.B0 = 0 /*normal */ ;
                            mem[r + 5].hh.u.B1 = 0 /*normal */ ;
                            mem[r + 6].gr = 0.0;
                        } else if (t > mem[r + 1].cint) {
                            mem[r + 5].hh.u.B0 = 1 /*stretching */ ;
                            if (mem[r + 6].cint == 0)
                                mem[r + 6].gr = 0.0;
                            else
                                mem[r + 6].gr = (t - mem[r + 1].cint) / ((double)mem[r + 6].cint);
                        } else {

                            mem[r + 5].hh.u.B1 = mem[r + 5].hh.u.B0;
                            mem[r + 5].hh.u.B0 = 2 /*shrinking */ ;
                            if (mem[r + 4].cint == 0)
                                mem[r + 6].gr = 0.0;
                            else if ((mem[r + 5].hh.u.B1 == 0 /*normal */ ) && (mem[r + 1].cint - t > mem[r + 4].cint))
                                mem[r + 6].gr = 1.0;
                            else
                                mem[r + 6].gr = (mem[r + 1].cint - t) / ((double)mem[r + 4].cint);
                        }
                        mem[r + 1].cint = w;
                        mem[r].hh.u.B0 = 0 /*hlist_node */ ;
                    } else {    /*840: */

                        mem[r + 1].cint = mem[q + 1].cint;
                        if (t == mem[r + 3].cint) {
                            mem[r + 5].hh.u.B0 = 0 /*normal */ ;
                            mem[r + 5].hh.u.B1 = 0 /*normal */ ;
                            mem[r + 6].gr = 0.0;
                        } else if (t > mem[r + 3].cint) {
                            mem[r + 5].hh.u.B0 = 1 /*stretching */ ;
                            if (mem[r + 6].cint == 0)
                                mem[r + 6].gr = 0.0;
                            else
                                mem[r + 6].gr = (t - mem[r + 3].cint) / ((double)mem[r + 6].cint);
                        } else {

                            mem[r + 5].hh.u.B1 = mem[r + 5].hh.u.B0;
                            mem[r + 5].hh.u.B0 = 2 /*shrinking */ ;
                            if (mem[r + 4].cint == 0)
                                mem[r + 6].gr = 0.0;
                            else if ((mem[r + 5].hh.u.B1 == 0 /*normal */ ) && (mem[r + 3].cint - t > mem[r + 4].cint))
                                mem[r + 6].gr = 1.0;
                            else
                                mem[r + 6].gr = (mem[r + 3].cint - t) / ((double)mem[r + 4].cint);
                        }
                        mem[r + 3].cint = w;
                        mem[r].hh.u.B0 = 1 /*vlist_node */ ;
                    }
                    mem[r + 4].cint = 0;
                    if (u != mem_top - 4) {
                        mem[u].hh.v.RH = mem[r].hh.v.RH;
                        mem[r].hh.v.RH = mem[mem_top - 4].hh.v.RH;
                        r = u;
                    }
                    r = mem[mem[r].hh.v.RH].hh.v.RH;
                    s = mem[mem[s].hh.v.RH].hh.v.RH;
                } while (!(r == -268435455L));
            } else if (mem[q].hh.u.B0 == 2 /*rule_node */ ) {     /*835: */
                if ((mem[q + 1].cint == -1073741824L))
                    mem[q + 1].cint = mem[p + 1].cint;
                if ((mem[q + 3].cint == -1073741824L))
                    mem[q + 3].cint = mem[p + 3].cint;
                if ((mem[q + 2].cint == -1073741824L))
                    mem[q + 2].cint = mem[p + 2].cint;
                if (o != 0) {
                    r = mem[q].hh.v.RH;
                    mem[q].hh.v.RH = -268435455L;
                    q = hpack(q, 0, 1 /*additional */ );
                    mem[q + 4].cint = o;
                    mem[q].hh.v.RH = r;
                    mem[s].hh.v.RH = q;
                }
            }
        }
        s = q;
        q = mem[q].hh.v.RH;
    }
    flush_node_list(p);
    pop_alignment();
    aux_save = cur_list.aux_field;
    p = mem[cur_list.head_field].hh.v.RH;
    q = cur_list.tail_field;
    pop_nest();
    if (cur_list.mode_field == 207 /*mmode */ ) {       /*1241: */
        do_assignments();
        if (cur_cmd != 3 /*math_shift */ ) {    /*1242: */
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66571L /*"Missing $$ inserted" */ );
            }
            {
                help_ptr = 2;
                help_line[1] = 66285L /*"Displays can use special alignments (like \eqalignno)" */ ;
                help_line[0] = 66286L /*"only if nothing but the alignment itself is between $$'s." */ ;
            }
            back_error();
        } else {                /*1232: */

            get_x_token();
            if (cur_cmd != 3 /*math_shift */ ) {
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66567L /*"Display math should end with $$" */ );
                }
                {
                    help_ptr = 2;
                    help_line[1] = 66568L /*"The `$' that I just saw supposedly matches a previous `$$'." */ ;
                    help_line[0] = 66569L /*"So I shall assume that you typed `$$' both times." */ ;
                }
                back_error();
            }
        }
        flush_node_list(cur_list.eTeX_aux_field);
        pop_nest();
        {
            mem[cur_list.tail_field].hh.v.RH = new_penalty(eqtb[8938751L /*int_base 11 */ ].cint);
            cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
        }
        {
            mem[cur_list.tail_field].hh.v.RH = new_param_glue(3 /*above_display_skip_code */ );
            cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
        }
        mem[cur_list.tail_field].hh.v.RH = p;
        if (p != -268435455L)
            cur_list.tail_field = q;
        {
            mem[cur_list.tail_field].hh.v.RH = new_penalty(eqtb[8938752L /*int_base 12 */ ].cint);
            cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
        }
        {
            mem[cur_list.tail_field].hh.v.RH = new_param_glue(4 /*below_display_skip_code */ );
            cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
        }
        cur_list.aux_field.cint = aux_save.cint;
        resume_after_display();
    } else {

        cur_list.aux_field = aux_save;
        mem[cur_list.tail_field].hh.v.RH = p;
        if (p != -268435455L)
            cur_list.tail_field = q;
        if (cur_list.mode_field == 1 /*vmode */ )
            build_page();
    }
}

void align_peek(void)
{
 align_peek_regmem lab20:      /*restart */ align_state = 1000000L;
    do {
        get_x_or_protected();
    } while (!(cur_cmd != 10 /*spacer */ ));
    if (cur_cmd == 34 /*no_align */ ) {
        scan_left_brace();
        new_save_level(7 /*no_align_group */ );
        if (cur_list.mode_field == -1)
            normal_paragraph();
    } else if (cur_cmd == 2 /*right_brace */ )
        fin_align();
    else if ((cur_cmd == 5 /*car_ret */ ) && (cur_chr == 65539L /*cr_cr_code */ ))
        goto lab20;
    else {

        init_row();
        init_col();
    }
}

halfword zfinite_shrink(halfword p)
{
    register halfword Result;
    finite_shrink_regmem halfword q;
    if (no_shrink_error_yet) {
        no_shrink_error_yet = false;
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66307L /*"Infinite glue shrinkage found in a paragraph" */ );
        }
        {
            help_ptr = 5;
            help_line[4] = 66308L /*"The paragraph just ended includes some glue that has" */ ;
            help_line[3] = 66309L /*"infinite shrinkability, e.g., `\hskip 0pt minus 1fil'." */ ;
            help_line[2] = 66310L /*"Such glue doesn't belong there---it allows a paragraph" */ ;
            help_line[1] = 66311L /*"of any length to fit on one line. But it's safe to proceed," */ ;
            help_line[0] = 66312L /*"since the offensive shrinkability has been made finite." */ ;
        }
        error();
    }
    q = new_spec(p);
    mem[q].hh.u.B1 = 0 /*normal */ ;
    delete_glue_ref(p);
    Result = q;
    return Result;
}

void zpush_node(halfword p)
{
    push_node_regmem if (hlist_stack_level > 512 /*max_hlist_stack */ )
        pdf_error(66313L /*"push_node" */ , 66314L /*"stack overflow" */ );
    hlist_stack[hlist_stack_level] = p;
    hlist_stack_level = hlist_stack_level + 1;
}

halfword pop_node(void)
{
    register halfword Result;
    pop_node_regmem hlist_stack_level = hlist_stack_level - 1;
    if (hlist_stack_level < 0)
        pdf_error(66315L /*"pop_node" */ , 66316L /*"stack underflow (internal error)" */ );
    Result = hlist_stack[hlist_stack_level];
    return Result;
}

halfword zfind_protchar_left(halfword l, boolean d)
{
    register halfword Result;
    find_protchar_left_regmem halfword t;
    boolean run;
    if ((mem[l].hh.v.RH != -268435455L) && (mem[l].hh.u.B0 == 0 /*hlist_node */ ) && (mem[l + 1].cint == 0)
        && (mem[l + 3].cint == 0) && (mem[l + 2].cint == 0) && (mem[l + 5].hh.v.RH == -268435455L))
        l = mem[l].hh.v.RH;
    else if (d)
        while ((mem[l].hh.v.RH != -268435455L) && (!((l >= hi_mem_min) || (mem[l].hh.u.B0 < 9 /*math_node */ ))))
            l = mem[l].hh.v.RH;
    hlist_stack_level = 0;
    run = true;
    do {
        t = l;
        while (run && (mem[l].hh.u.B0 == 0 /*hlist_node */ ) && (mem[l + 5].hh.v.RH != -268435455L)) {

            push_node(l);
            l = mem[l + 5].hh.v.RH;
        }
        while (run
               && (!(l >= hi_mem_min)
                   && ((mem[l].hh.u.B0 == 3 /*ins_node */ ) || (mem[l].hh.u.B0 == 4 /*mark_node */ )
                       || (mem[l].hh.u.B0 == 5 /*adjust_node */ ) || (mem[l].hh.u.B0 == 12 /*penalty_node */ )
                       || ((mem[l].hh.u.B0 == 7 /*disc_node */ ) && (mem[l + 1].hh.v.LH == -268435455L)
                           && (mem[l + 1].hh.v.RH == -268435455L) && (mem[l].hh.u.B1 == 0))
                       || ((mem[l].hh.u.B0 == 9 /*math_node */ ) && (mem[l + 1].cint == 0))
                       || ((mem[l].hh.u.B0 == 11 /*kern_node */ )
                           && ((mem[l + 1].cint == 0) || (mem[l].hh.u.B1 == 0 /*normal */ )))
                       || ((mem[l].hh.u.B0 == 10 /*glue_node */ ) && (mem[l + 1].hh.v.LH == mem_bot))
                       || ((mem[l].hh.u.B0 == 0 /*hlist_node */ ) && (mem[l + 1].cint == 0) && (mem[l + 3].cint == 0)
                           && (mem[l + 2].cint == 0) && (mem[l + 5].hh.v.RH == -268435455L))))) {

            while ((mem[l].hh.v.RH == -268435455L) && (hlist_stack_level > 0)) {

                l = pop_node();
            }
            if (mem[l].hh.v.RH != -268435455L)
                l = mem[l].hh.v.RH;
            else if (hlist_stack_level == 0)
                run = false;
        }
    } while (!(t == l));
    Result = l;
    return Result;
}

halfword zfind_protchar_right(halfword l, halfword r)
{
    register halfword Result;
    find_protchar_right_regmem halfword t;
    boolean run;
    Result = -268435455L;
    if (r == -268435455L)
        return Result;
    hlist_stack_level = 0;
    run = true;
    do {
        t = r;
        while (run && (mem[r].hh.u.B0 == 0 /*hlist_node */ ) && (mem[r + 5].hh.v.RH != -268435455L)) {

            push_node(l);
            push_node(r);
            l = mem[r + 5].hh.v.RH;
            r = l;
            while (mem[r].hh.v.RH != -268435455L)
                r = mem[r].hh.v.RH;
        }
        while (run
               && (!(r >= hi_mem_min)
                   && ((mem[r].hh.u.B0 == 3 /*ins_node */ ) || (mem[r].hh.u.B0 == 4 /*mark_node */ )
                       || (mem[r].hh.u.B0 == 5 /*adjust_node */ ) || (mem[r].hh.u.B0 == 12 /*penalty_node */ )
                       || ((mem[r].hh.u.B0 == 7 /*disc_node */ ) && (mem[r + 1].hh.v.LH == -268435455L)
                           && (mem[r + 1].hh.v.RH == -268435455L) && (mem[r].hh.u.B1 == 0))
                       || ((mem[r].hh.u.B0 == 9 /*math_node */ ) && (mem[r + 1].cint == 0))
                       || ((mem[r].hh.u.B0 == 11 /*kern_node */ )
                           && ((mem[r + 1].cint == 0) || (mem[r].hh.u.B1 == 0 /*normal */ )))
                       || ((mem[r].hh.u.B0 == 10 /*glue_node */ ) && (mem[r + 1].hh.v.LH == mem_bot))
                       || ((mem[r].hh.u.B0 == 0 /*hlist_node */ ) && (mem[r + 1].cint == 0) && (mem[r + 3].cint == 0)
                           && (mem[r + 2].cint == 0) && (mem[r + 5].hh.v.RH == -268435455L))))) {

            while ((r == l) && (hlist_stack_level > 0)) {

                r = pop_node();
                l = pop_node();
            }
            if ((r != l) && (r != -268435455L))
                r = prev_rightmost(l, r);
            else if ((r == l) && (hlist_stack_level == 0))
                run = false;
        }
    } while (!(t == r));
    Result = r;
    return Result;
}

scaled ztotal_pw(halfword q, halfword p)
{
    register scaled Result;
    total_pw_regmem halfword l, r;
    integer n;
    if (mem[q + 1].hh.v.RH == -268435455L)
        l = first_p;
    else
        l = mem[mem[q + 1].hh.v.RH + 1].hh.v.RH;
    r = prev_rightmost(global_prev_p, p);
    if ((p != -268435455L) && (mem[p].hh.u.B0 == 7 /*disc_node */ ) && (mem[p + 1].hh.v.LH != -268435455L)) {
        r = mem[p + 1].hh.v.LH;
        while (mem[r].hh.v.RH != -268435455L)
            r = mem[r].hh.v.RH;
    } else
        r = find_protchar_right(l, r);
    if ((l != -268435455L) && (mem[l].hh.u.B0 == 7 /*disc_node */ )) {
        if (mem[l + 1].hh.v.RH != -268435455L) {
            l = mem[l + 1].hh.v.RH;
            goto lab30;
        } else {

            n = mem[l].hh.u.B1;
            l = mem[l].hh.v.RH;
            while (n > 0) {

                if (mem[l].hh.v.RH != -268435455L)
                    l = mem[l].hh.v.RH;
                n--;
            }
        }
    }
    l = find_protchar_left(l, true);
 lab30:                        /*done */ Result = char_pw(l, 0) + char_pw(r, 1);
    return Result;
}

void ztry_break(integer pi, small_number break_type)
{
    try_break_regmem halfword r;
    halfword prev_r;
    halfword old_l;
    boolean no_break_yet;
    halfword prev_prev_r;
    halfword s;
    halfword q;
    halfword v;
    integer t;
    internal_font_number f;
    halfword l;
    boolean node_r_stays_active;
    scaled line_width;
    unsigned char /*tight_fit */ fit_class;
    halfword b;
    integer d;
    boolean artificial_demerits;
    scaled shortfall;
    scaled g;

    if (abs(pi) >= 10000 /*inf_penalty */ ) {
        if (pi > 0)
            goto exit;
        else
            pi = -10000 /*eject_penalty *//*:860 */ ;
    }

    no_break_yet = true;
    prev_r = mem_top - 7;
    old_l = 0;
    cur_active_width[1] = active_width[1];
    cur_active_width[2] = active_width[2];
    cur_active_width[3] = active_width[3];
    cur_active_width[4] = active_width[4];
    cur_active_width[5] = active_width[5];
    cur_active_width[6] = active_width[6];
    while (true) {

 lab22:                        /*continue */ r = mem[prev_r].hh.v.RH;
        if (mem[r].hh.u.B0 == 2 /*delta_node */ ) {
            cur_active_width[1] = cur_active_width[1] + mem[r + 1].cint;
            cur_active_width[2] = cur_active_width[2] + mem[r + 2].cint;
            cur_active_width[3] = cur_active_width[3] + mem[r + 3].cint;
            cur_active_width[4] = cur_active_width[4] + mem[r + 4].cint;
            cur_active_width[5] = cur_active_width[5] + mem[r + 5].cint;
            cur_active_width[6] = cur_active_width[6] + mem[r + 6].cint;
            prev_prev_r = prev_r;
            prev_r = r;
            goto lab22;
        }
        {
            l = mem[r + 1].hh.v.LH;
            if (l > old_l) {
                if ((minimum_demerits < 1073741823L) && ((old_l != easy_line) || (r == mem_top - 7))) { /*865: */
                    if (no_break_yet) { /*866: */
                        no_break_yet = false;
                        break_width[1] = background[1];
                        break_width[2] = background[2];
                        break_width[3] = background[3];
                        break_width[4] = background[4];
                        break_width[5] = background[5];
                        break_width[6] = background[6];
                        s = cur_p;
                        if (break_type > 0 /*unhyphenated */ ) {

                            if (cur_p != -268435455L) { /*869: */
                                t = mem[cur_p].hh.u.B1;
                                v = cur_p;
                                s = mem[cur_p + 1].hh.v.RH;
                                while (t > 0) {

                                    t--;
                                    v = mem[v].hh.v.RH;
                                    if ((v >= hi_mem_min)) {
                                        f = mem[v].hh.u.B0;
                                        break_width[1] =
                                            break_width[1] - font_info[width_base[f] +
                                                                       font_info[char_base[f] +
                                                                                 effective_char(true, f,
                                                                                                mem[v].hh.u.B1)].qqqq.u.B0].
                                            cint;
                                    } else
                                        switch (mem[v].hh.u.B0) {
                                        case 6:
                                            {
                                                f = mem[v + 1].hh.u.B0;
                                                xtx_ligature_present = true;
                                                break_width[1] =
                                                    break_width[1] - font_info[width_base[f] +
                                                                               font_info[char_base[f] +
                                                                                         effective_char(true, f,
                                                                                                        mem[v +
                                                                                                            1].hh.u.B1)].
                                                                               qqqq.u.B0].cint;
                                            }
                                            break;
                                        case 0:
                                        case 1:
                                        case 2:
                                        case 11:
                                            break_width[1] = break_width[1] - mem[v + 1].cint;
                                            break;
                                        case 8:
                                            if ((mem[v].hh.u.B1 == 40 /*native_word_node */ )
                                                || (mem[v].hh.u.B1 == 41 /*native_word_node_AT */ )
                                                || (mem[v].hh.u.B1 == 42 /*glyph_node */ )
                                                || (mem[v].hh.u.B1 == 43 /*pic_node */ )
                                                || (mem[v].hh.u.B1 == 44 /*pdf_node */ ))
                                                break_width[1] = break_width[1] - mem[v + 1].cint;
                                            else
                                                confusion(66317L /*"disc1a" */ );
                                            break;
                                        default:
                                            confusion(66318L /*"disc1" */ );
                                            break;
                                        }
                                }
                                while (s != -268435455L) {

                                    if ((s >= hi_mem_min)) {
                                        f = mem[s].hh.u.B0;
                                        break_width[1] =
                                            break_width[1] + font_info[width_base[f] +
                                                                       font_info[char_base[f] +
                                                                                 effective_char(true, f,
                                                                                                mem[s].hh.u.B1)].qqqq.u.B0].
                                            cint;
                                    } else
                                        switch (mem[s].hh.u.B0) {
                                        case 6:
                                            {
                                                f = mem[s + 1].hh.u.B0;
                                                xtx_ligature_present = true;
                                                break_width[1] =
                                                    break_width[1] + font_info[width_base[f] +
                                                                               font_info[char_base[f] +
                                                                                         effective_char(true, f,
                                                                                                        mem[s +
                                                                                                            1].hh.u.B1)].
                                                                               qqqq.u.B0].cint;
                                            }
                                            break;
                                        case 0:
                                        case 1:
                                        case 2:
                                        case 11:
                                            break_width[1] = break_width[1] + mem[s + 1].cint;
                                            break;
                                        case 8:
                                            if ((mem[s].hh.u.B1 == 40 /*native_word_node */ )
                                                || (mem[s].hh.u.B1 == 41 /*native_word_node_AT */ )
                                                || (mem[s].hh.u.B1 == 42 /*glyph_node */ )
                                                || (mem[s].hh.u.B1 == 43 /*pic_node */ )
                                                || (mem[s].hh.u.B1 == 44 /*pdf_node */ ))
                                                break_width[1] = break_width[1] + mem[s + 1].cint;
                                            else
                                                confusion(66319L /*"disc2a" */ );
                                            break;
                                        default:
                                            confusion(66320L /*"disc2" */ );
                                            break;
                                        }
                                    s = mem[s].hh.v.RH;
                                }
                                break_width[1] = break_width[1] + disc_width;
                                if (mem[cur_p + 1].hh.v.RH == -268435455L)
                                    s = mem[v].hh.v.RH;
                            }
                        }
                        while (s != -268435455L) {

                            if ((s >= hi_mem_min))
                                goto lab30;
                            switch (mem[s].hh.u.B0) {
                            case 10:
                                {
                                    v = mem[s + 1].hh.v.LH;
                                    break_width[1] = break_width[1] - mem[v + 1].cint;
                                    break_width[2 + mem[v].hh.u.B0] = break_width[2 + mem[v].hh.u.B0] - mem[v + 2].cint;
                                    break_width[6] = break_width[6] - mem[v + 3].cint;
                                }
                                break;
                            case 12:
                                ;
                                break;
                            case 9:
                                break_width[1] = break_width[1] - mem[s + 1].cint;
                                break;
                            case 11:
                                if (mem[s].hh.u.B1 != 1 /*explicit */ )
                                    goto lab30;
                                else
                                    break_width[1] = break_width[1] - mem[s + 1].cint;
                                break;
                            default:
                                goto lab30;
                                break;
                            }
                            s = mem[s].hh.v.RH;
                        }
 lab30:                        /*done */ ;
                    }
                    if (mem[prev_r].hh.u.B0 == 2 /*delta_node */ ) {
                        mem[prev_r + 1].cint = mem[prev_r + 1].cint - cur_active_width[1] + break_width[1];
                        mem[prev_r + 2].cint = mem[prev_r + 2].cint - cur_active_width[2] + break_width[2];
                        mem[prev_r + 3].cint = mem[prev_r + 3].cint - cur_active_width[3] + break_width[3];
                        mem[prev_r + 4].cint = mem[prev_r + 4].cint - cur_active_width[4] + break_width[4];
                        mem[prev_r + 5].cint = mem[prev_r + 5].cint - cur_active_width[5] + break_width[5];
                        mem[prev_r + 6].cint = mem[prev_r + 6].cint - cur_active_width[6] + break_width[6];
                    } else if (prev_r == mem_top - 7) {
                        active_width[1] = break_width[1];
                        active_width[2] = break_width[2];
                        active_width[3] = break_width[3];
                        active_width[4] = break_width[4];
                        active_width[5] = break_width[5];
                        active_width[6] = break_width[6];
                    } else {

                        q = get_node(7 /*delta_node_size */ );
                        mem[q].hh.v.RH = r;
                        mem[q].hh.u.B0 = 2 /*delta_node */ ;
                        mem[q].hh.u.B1 = 0;
                        mem[q + 1].cint = break_width[1] - cur_active_width[1];
                        mem[q + 2].cint = break_width[2] - cur_active_width[2];
                        mem[q + 3].cint = break_width[3] - cur_active_width[3];
                        mem[q + 4].cint = break_width[4] - cur_active_width[4];
                        mem[q + 5].cint = break_width[5] - cur_active_width[5];
                        mem[q + 6].cint = break_width[6] - cur_active_width[6];
                        mem[prev_r].hh.v.RH = q;
                        prev_prev_r = prev_r;
                        prev_r = q;
                    }
                    if (abs(eqtb[8938756L /*int_base 16 */ ].cint) >= 1073741823L - minimum_demerits)
                        minimum_demerits = 1073741822L;
                    else
                        minimum_demerits = minimum_demerits + abs(eqtb[8938756L /*int_base 16 */ ].cint);
                    {
                        register integer for_end;
                        fit_class = 0 /*very_loose_fit */ ;
                        for_end = 3 /*tight_fit */ ;
                        if (fit_class <= for_end)
                            do {
                                if (minimal_demerits[fit_class] <= minimum_demerits) {  /*874: */
                                    q = get_node(2 /*passive_node_size */ );
                                    mem[q].hh.v.RH = passive;
                                    passive = q;
                                    mem[q + 1].hh.v.RH = cur_p;
                                    mem[q + 1].hh.v.LH = best_place[fit_class];
                                    q = get_node(active_node_size);
                                    mem[q + 1].hh.v.RH = passive;
                                    mem[q + 1].hh.v.LH = best_pl_line[fit_class] + 1;
                                    mem[q].hh.u.B1 = fit_class;
                                    mem[q].hh.u.B0 = break_type;
                                    mem[q + 2].cint = minimal_demerits[fit_class];
                                    if (do_last_line_fit) {     /*1639: */
                                        mem[q + 3].cint = best_pl_short[fit_class];
                                        mem[q + 4].cint = best_pl_glue[fit_class];
                                    }
                                    mem[q].hh.v.RH = r;
                                    mem[prev_r].hh.v.RH = q;
                                    prev_r = q;
                                }
                                minimal_demerits[fit_class] = 1073741823L;
                            }
                            while (fit_class++ < for_end);
                    }
                    minimum_demerits = 1073741823L;
                    if (r != mem_top - 7) {
                        q = get_node(7 /*delta_node_size */ );
                        mem[q].hh.v.RH = r;
                        mem[q].hh.u.B0 = 2 /*delta_node */ ;
                        mem[q].hh.u.B1 = 0;
                        mem[q + 1].cint = cur_active_width[1] - break_width[1];
                        mem[q + 2].cint = cur_active_width[2] - break_width[2];
                        mem[q + 3].cint = cur_active_width[3] - break_width[3];
                        mem[q + 4].cint = cur_active_width[4] - break_width[4];
                        mem[q + 5].cint = cur_active_width[5] - break_width[5];
                        mem[q + 6].cint = cur_active_width[6] - break_width[6];
                        mem[prev_r].hh.v.RH = q;
                        prev_prev_r = prev_r;
                        prev_r = q;
                    }
                }
                if (r == mem_top - 7)
                    goto exit;
                if (l > easy_line) {
                    line_width = second_width;
                    old_l = 1073741822L;
                } else {

                    old_l = l;
                    if (l > last_special_line)
                        line_width = second_width;
                    else if (eqtb[2252771L /*par_shape_loc */ ].hh.v.RH == -268435455L)
                        line_width = first_width;
                    else
                        line_width = mem[eqtb[2252771L /*par_shape_loc */ ].hh.v.RH + 2 * l].cint;
                }
            }
        }
        {
            artificial_demerits = false;
            shortfall = line_width - cur_active_width[1];
            if (eqtb[8938810L /*int_base 70 */ ].cint > 1)
                shortfall = shortfall + total_pw(r, cur_p);
            if (shortfall > 0) {        /*881: */

                if ((cur_active_width[3] != 0) || (cur_active_width[4] != 0) || (cur_active_width[5] != 0)) {
                    if (do_last_line_fit) {
                        if (cur_p == -268435455L) {     /*1634: */
                            if ((mem[r + 3].cint == 0) || (mem[r + 4].cint <= 0))
                                goto lab45;
                            if ((cur_active_width[3] != fill_width[0]) || (cur_active_width[4] != fill_width[1])
                                || (cur_active_width[5] != fill_width[2]))
                                goto lab45;
                            if (mem[r + 3].cint > 0)
                                g = cur_active_width[2];
                            else
                                g = cur_active_width[6];
                            if (g <= 0)
                                goto lab45;
                            arith_error = false;
                            g = fract(g, mem[r + 3].cint, mem[r + 4].cint, 1073741823L);
                            if (eqtb[8938804L /*int_base 64 */ ].cint < 1000)
                                g = fract(g, eqtb[8938804L /*int_base 64 */ ].cint, 1000, 1073741823L);
                            if (arith_error) {

                                if (mem[r + 3].cint > 0)
                                    g = 1073741823L;
                                else
                                    g = -1073741823L;
                            }
                            if (g > 0) {        /*1635: */
                                if (g > shortfall)
                                    g = shortfall;
                                if (g > 7230584L) {

                                    if (cur_active_width[2] < 1663497L) {
                                        b = 10000 /*inf_bad */ ;
                                        fit_class = 0 /*very_loose_fit */ ;
                                        goto lab40;
                                    }
                                }
                                b = badness(g, cur_active_width[2]);
                                if (b > 12) {

                                    if (b > 99)
                                        fit_class = 0 /*very_loose_fit */ ;
                                    else
                                        fit_class = 1 /*loose_fit */ ;
                                } else
                                    fit_class = 2 /*decent_fit */ ;
                                goto lab40;
                            } else if (g < 0) { /*1636: */
                                if (-(integer) g > cur_active_width[6])
                                    g = -(integer) cur_active_width[6];
                                b = badness(-(integer) g, cur_active_width[6]);
                                if (b > 12)
                                    fit_class = 3 /*tight_fit */ ;
                                else
                                    fit_class = 2 /*decent_fit */ ;
                                goto lab40;
                            }
 lab45:                        /*not_found */ ;
                        }
                        shortfall = 0;
                    }
                    b = 0;
                    fit_class = 2 /*decent_fit */ ;
                } else {

                    if (shortfall > 7230584L) {

                        if (cur_active_width[2] < 1663497L) {
                            b = 10000 /*inf_bad */ ;
                            fit_class = 0 /*very_loose_fit */ ;
                            goto lab31;
                        }
                    }
                    b = badness(shortfall, cur_active_width[2]);
                    if (b > 12) {

                        if (b > 99)
                            fit_class = 0 /*very_loose_fit */ ;
                        else
                            fit_class = 1 /*loose_fit */ ;
                    } else
                        fit_class = 2 /*decent_fit */ ;
 lab31:                        /*done1 */ ;
                }
            } else {            /*882: */

                if (-(integer) shortfall > cur_active_width[6])
                    b = 10001 /*inf_bad 1 */ ;
                else
                    b = badness(-(integer) shortfall, cur_active_width[6]);
                if (b > 12)
                    fit_class = 3 /*tight_fit */ ;
                else
                    fit_class = 2 /*decent_fit */ ;
            }
            if (do_last_line_fit) {     /*1637: */
                if (cur_p == -268435455L)
                    shortfall = 0;
                if (shortfall > 0)
                    g = cur_active_width[2];
                else if (shortfall < 0)
                    g = cur_active_width[6];
                else
                    g = 0;
            }
 lab40:    /*found */ if ((b > 10000 /*inf_bad */ ) || (pi == -10000 /*eject_penalty */ )) {   /*883: */
                if (final_pass && (minimum_demerits == 1073741823L) && (mem[r].hh.v.RH == mem_top - 7)
                    && (prev_r == mem_top - 7))
                    artificial_demerits = true;
                else if (b > threshold)
                    goto lab60;
                node_r_stays_active = false;
            } else {

                prev_r = r;
                if (b > threshold)
                    goto lab22;
                node_r_stays_active = true;
            }
            if (artificial_demerits)
                d = 0;
            else {              /*888: */

                d = eqtb[8938742L /*int_base 2 */ ].cint + b;
                if (abs(d) >= 10000)
                    d = 100000000L;
                else
                    d = d * d;
                if (pi != 0) {

                    if (pi > 0)
                        d = d + pi * pi;
                    else if (pi > -10000 /*eject_penalty */ )
                        d = d - pi * pi;
                }
                if ((break_type == 1 /*hyphenated */ ) && (mem[r].hh.u.B0 == 1 /*hyphenated */ )) {

                    if (cur_p != -268435455L)
                        d = d + eqtb[8938754L /*int_base 14 */ ].cint;
                    else
                        d = d + eqtb[8938755L /*int_base 15 */ ].cint;
                }
                if (abs(fit_class - mem[r].hh.u.B1) > 1)
                    d = d + eqtb[8938756L /*int_base 16 */ ].cint;
            }
            d = d + mem[r + 2].cint;
            if (d <= minimal_demerits[fit_class]) {
                minimal_demerits[fit_class] = d;
                best_place[fit_class] = mem[r + 1].hh.v.RH;
                best_pl_line[fit_class] = l;
                if (do_last_line_fit) { /*1638: */
                    best_pl_short[fit_class] = shortfall;
                    best_pl_glue[fit_class] = g;
                }
                if (d < minimum_demerits)
                    minimum_demerits = d;
            }
            if (node_r_stays_active)
                goto lab22;

	lab60: /* deactivate *//*889: */
	    mem[prev_r].hh.v.RH = mem[r].hh.v.RH;
            free_node(r, active_node_size);
            if (prev_r == mem_top - 7) {        /*890: */
                r = mem[mem_top - 7].hh.v.RH;
                if (mem[r].hh.u.B0 == 2 /*delta_node */ ) {
                    active_width[1] = active_width[1] + mem[r + 1].cint;
                    active_width[2] = active_width[2] + mem[r + 2].cint;
                    active_width[3] = active_width[3] + mem[r + 3].cint;
                    active_width[4] = active_width[4] + mem[r + 4].cint;
                    active_width[5] = active_width[5] + mem[r + 5].cint;
                    active_width[6] = active_width[6] + mem[r + 6].cint;
                    cur_active_width[1] = active_width[1];
                    cur_active_width[2] = active_width[2];
                    cur_active_width[3] = active_width[3];
                    cur_active_width[4] = active_width[4];
                    cur_active_width[5] = active_width[5];
                    cur_active_width[6] = active_width[6];
                    mem[mem_top - 7].hh.v.RH = mem[r].hh.v.RH;
                    free_node(r, 7 /*delta_node_size */ );
                }
            } else if (mem[prev_r].hh.u.B0 == 2 /*delta_node */ ) {
                r = mem[prev_r].hh.v.RH;
                if (r == mem_top - 7) {
                    cur_active_width[1] = cur_active_width[1] - mem[prev_r + 1].cint;
                    cur_active_width[2] = cur_active_width[2] - mem[prev_r + 2].cint;
                    cur_active_width[3] = cur_active_width[3] - mem[prev_r + 3].cint;
                    cur_active_width[4] = cur_active_width[4] - mem[prev_r + 4].cint;
                    cur_active_width[5] = cur_active_width[5] - mem[prev_r + 5].cint;
                    cur_active_width[6] = cur_active_width[6] - mem[prev_r + 6].cint;
                    mem[prev_prev_r].hh.v.RH = mem_top - 7;
                    free_node(prev_r, 7 /*delta_node_size */ );
                    prev_r = prev_prev_r;
                } else if (mem[r].hh.u.B0 == 2 /*delta_node */ ) {
                    cur_active_width[1] = cur_active_width[1] + mem[r + 1].cint;
                    cur_active_width[2] = cur_active_width[2] + mem[r + 2].cint;
                    cur_active_width[3] = cur_active_width[3] + mem[r + 3].cint;
                    cur_active_width[4] = cur_active_width[4] + mem[r + 4].cint;
                    cur_active_width[5] = cur_active_width[5] + mem[r + 5].cint;
                    cur_active_width[6] = cur_active_width[6] + mem[r + 6].cint;
                    mem[prev_r + 1].cint = mem[prev_r + 1].cint + mem[r + 1].cint;
                    mem[prev_r + 2].cint = mem[prev_r + 2].cint + mem[r + 2].cint;
                    mem[prev_r + 3].cint = mem[prev_r + 3].cint + mem[r + 3].cint;
                    mem[prev_r + 4].cint = mem[prev_r + 4].cint + mem[r + 4].cint;
                    mem[prev_r + 5].cint = mem[prev_r + 5].cint + mem[r + 5].cint;
                    mem[prev_r + 6].cint = mem[prev_r + 6].cint + mem[r + 6].cint;
                    mem[prev_r].hh.v.RH = mem[r].hh.v.RH;
                    free_node(r, 7 /*delta_node_size */ );
                }
            }
        }
    }

exit:
    ;
}

void zpost_line_break(boolean d)
{
    post_line_break_regmem halfword q, r, s;
    halfword p, k;
    scaled w;
    boolean glue_break;
    halfword ptmp;
    boolean disc_break;
    boolean post_disc_break;
    scaled cur_width;
    scaled cur_indent;
    quarterword t;
    integer pen;
    halfword cur_line;
    halfword LR_ptr;
    LR_ptr = cur_list.eTeX_aux_field;
    q = mem[best_bet + 1].hh.v.RH;
    cur_p = -268435455L;
    do {
        r = q;
        q = mem[q + 1].hh.v.LH;
        mem[r + 1].hh.v.LH = cur_p;
        cur_p = r;
    } while (!(q == -268435455L /*:907 */ ));
    cur_line = cur_list.pg_field + 1;
    do {
        /*909: */ if ((eqtb[8938811L /*eTeX_state_base 0 */ ].cint > 0)) {      /*1494: */
            q = mem[mem_top - 3].hh.v.RH;
            if (LR_ptr != -268435455L) {
                temp_ptr = LR_ptr;
                r = q;
                do {
                    s = new_math(0, (mem[temp_ptr].hh.v.LH - 1));
                    mem[s].hh.v.RH = r;
                    r = s;
                    temp_ptr = mem[temp_ptr].hh.v.RH;
                } while (!(temp_ptr == -268435455L));
                mem[mem_top - 3].hh.v.RH = r;
            }
            while (q != mem[cur_p + 1].hh.v.RH) {

                if (!(q >= hi_mem_min)) {

                    if (mem[q].hh.u.B0 == 9 /*math_node */ ) {    /*1495: */

                        if (odd(mem[q].hh.u.B1)) {
                            if (LR_ptr != -268435455L) {

                                if (mem[LR_ptr].hh.v.LH == (4 /*L_code */  * (mem[q].hh.u.B1 / 4 /*L_code */ ) + 3)) {
                                    temp_ptr = LR_ptr;
                                    LR_ptr = mem[temp_ptr].hh.v.RH;
                                    {
                                        mem[temp_ptr].hh.v.RH = avail;
                                        avail = temp_ptr;
                                    }
                                }
                            }
                        } else {

                            temp_ptr = get_avail();
                            mem[temp_ptr].hh.v.LH = (4 /*L_code */  * (mem[q].hh.u.B1 / 4 /*L_code */ ) + 3);
                            mem[temp_ptr].hh.v.RH = LR_ptr;
                            LR_ptr = temp_ptr;
                        }
                    }
                }
                q = mem[q].hh.v.RH;
            }
        }
        q = mem[cur_p + 1].hh.v.RH;
        disc_break = false;
        post_disc_break = false;
        glue_break = false;
        if (q != -268435455L) {

            if (mem[q].hh.u.B0 == 10 /*glue_node */ ) {
                delete_glue_ref(mem[q + 1].hh.v.LH);
                mem[q + 1].hh.v.LH = eqtb[2252248L /*glue_base 8 */ ].hh.v.RH;
                mem[q].hh.u.B1 = 9 /*right_skip_code 1 */ ;
                mem[eqtb[2252248L /*glue_base 8 */ ].hh.v.RH].hh.v.RH++;
                glue_break = true;
                goto lab30;
            } else {

                if (mem[q].hh.u.B0 == 7 /*disc_node */ ) {        /*911: */
                    t = mem[q].hh.u.B1;
                    if (t == 0)
                        r = mem[q].hh.v.RH;
                    else {

                        r = q;
                        while (t > 1) {

                            r = mem[r].hh.v.RH;
                            t--;
                        }
                        s = mem[r].hh.v.RH;
                        r = mem[s].hh.v.RH;
                        mem[s].hh.v.RH = -268435455L;
                        flush_node_list(mem[q].hh.v.RH);
                        mem[q].hh.u.B1 = 0;
                    }
                    if (mem[q + 1].hh.v.RH != -268435455L) {    /*913: */
                        s = mem[q + 1].hh.v.RH;
                        while (mem[s].hh.v.RH != -268435455L)
                            s = mem[s].hh.v.RH;
                        mem[s].hh.v.RH = r;
                        r = mem[q + 1].hh.v.RH;
                        mem[q + 1].hh.v.RH = -268435455L;
                        post_disc_break = true;
                    }
                    if (mem[q + 1].hh.v.LH != -268435455L) {    /*914: */
                        s = mem[q + 1].hh.v.LH;
                        mem[q].hh.v.RH = s;
                        while (mem[s].hh.v.RH != -268435455L)
                            s = mem[s].hh.v.RH;
                        mem[q + 1].hh.v.LH = -268435455L;
                        q = s;
                    }
                    mem[q].hh.v.RH = r;
                    disc_break = true;
                } else if (mem[q].hh.u.B0 == 11 /*kern_node */ )
                    mem[q + 1].cint = 0;
                else if (mem[q].hh.u.B0 == 9 /*math_node */ ) {
                    mem[q + 1].cint = 0;
                    if ((eqtb[8938811L /*eTeX_state_base 0 */ ].cint > 0)) {    /*1495: */

                        if (odd(mem[q].hh.u.B1)) {
                            if (LR_ptr != -268435455L) {

                                if (mem[LR_ptr].hh.v.LH == (4 /*L_code */  * (mem[q].hh.u.B1 / 4 /*L_code */ ) + 3)) {
                                    temp_ptr = LR_ptr;
                                    LR_ptr = mem[temp_ptr].hh.v.RH;
                                    {
                                        mem[temp_ptr].hh.v.RH = avail;
                                        avail = temp_ptr;
                                    }
                                }
                            }
                        } else {

                            temp_ptr = get_avail();
                            mem[temp_ptr].hh.v.LH = (4 /*L_code */  * (mem[q].hh.u.B1 / 4 /*L_code */ ) + 3);
                            mem[temp_ptr].hh.v.RH = LR_ptr;
                            LR_ptr = temp_ptr;
                        }
                    }
                }
            }
        } else {

            q = mem_top - 3;
            while (mem[q].hh.v.RH != -268435455L)
                q = mem[q].hh.v.RH;
        }
 lab30:/*done */ if (eqtb[8938810L /*int_base 70 */ ].cint > 0) {
            if (disc_break && ((q >= hi_mem_min) || (mem[q].hh.u.B0 != 7 /*disc_node */ ))) {
                p = q;
                ptmp = p;
            } else {

                p = prev_rightmost(mem[mem_top - 3].hh.v.RH, q);
                ptmp = p;
                p = find_protchar_right(mem[mem_top - 3].hh.v.RH, p);
            }
            w = char_pw(p, 1);
            if (w != 0) {
                k = new_margin_kern(-(integer) w, last_rightmost_char, 1);
                mem[k].hh.v.RH = mem[ptmp].hh.v.RH;
                mem[ptmp].hh.v.RH = k;
                if ((ptmp == q))
                    q = mem[q].hh.v.RH;
            }
        }
        if (!glue_break) {
            r = new_param_glue(8 /*right_skip_code */ );
            mem[r].hh.v.RH = mem[q].hh.v.RH;
            mem[q].hh.v.RH = r;
            q = /*:915 */ r;
        }
        if ((eqtb[8938811L /*eTeX_state_base 0 */ ].cint > 0)) {        /*1496: */

            if (LR_ptr != -268435455L) {
                s = mem_top - 3;
                r = mem[s].hh.v.RH;
                while (r != q) {

                    s = r;
                    r = mem[s].hh.v.RH;
                }
                r = LR_ptr;
                while (r != -268435455L) {

                    temp_ptr = new_math(0, mem[r].hh.v.LH);
                    mem[s].hh.v.RH = temp_ptr;
                    s = temp_ptr;
                    r = mem[r].hh.v.RH;
                }
                mem[s].hh.v.RH = q;
            }
        }
        r = mem[q].hh.v.RH;
        mem[q].hh.v.RH = -268435455L;
        q = mem[mem_top - 3].hh.v.RH;
        mem[mem_top - 3].hh.v.RH = r;
        if (eqtb[8938810L /*int_base 70 */ ].cint > 0) {
            p = q;
            p = find_protchar_left(p, false);
            w = char_pw(p, 0);
            if (w != 0) {
                k = new_margin_kern(-(integer) w, last_leftmost_char, 0);
                mem[k].hh.v.RH = q;
                q = k;
            }
        }
        if (eqtb[2252247L /*glue_base 7 */ ].hh.v.RH != mem_bot) {
            r = new_param_glue(7 /*left_skip_code */ );
            mem[r].hh.v.RH = q;
            q = r;
        }
        if (cur_line > last_special_line) {
            cur_width = second_width;
            cur_indent = second_indent;
        } else if (eqtb[2252771L /*par_shape_loc */ ].hh.v.RH == -268435455L) {
            cur_width = first_width;
            cur_indent = first_indent;
        } else {

            cur_width = mem[eqtb[2252771L /*par_shape_loc */ ].hh.v.RH + 2 * cur_line].cint;
            cur_indent = mem[eqtb[2252771L /*par_shape_loc */ ].hh.v.RH + 2 * cur_line - 1].cint;
        }
        adjust_tail = mem_top - 5;
        pre_adjust_tail = mem_top - 14;
        just_box = hpack(q, cur_width, 0 /*exactly */ );
        mem[just_box + 4].cint = /*:918 */ cur_indent;
        if (mem_top - 14 != pre_adjust_tail) {
            mem[cur_list.tail_field].hh.v.RH = mem[mem_top - 14].hh.v.RH;
            cur_list.tail_field = pre_adjust_tail;
        }
        pre_adjust_tail = -268435455L;
        append_to_vlist(just_box);
        if (mem_top - 5 != adjust_tail) {
            mem[cur_list.tail_field].hh.v.RH = mem[mem_top - 5].hh.v.RH;
            cur_list.tail_field = adjust_tail;
        }
        adjust_tail = -268435455L /*:917 */ ;
        if (cur_line + 1 != best_line) {
            q = eqtb[2253039L /*inter_line_penalties_loc */ ].hh.v.RH;
            if (q != -268435455L) {
                r = cur_line;
                if (r > mem[q + 1].cint)
                    r = mem[q + 1].cint;
                pen = mem[q + r + 1].cint;
            } else
                pen = eqtb[8938753L /*int_base 13 */ ].cint;
            q = eqtb[2253040L /*club_penalties_loc */ ].hh.v.RH;
            if (q != -268435455L) {
                r = cur_line - cur_list.pg_field;
                if (r > mem[q + 1].cint)
                    r = mem[q + 1].cint;
                pen = pen + mem[q + r + 1].cint;
            } else if (cur_line == cur_list.pg_field + 1)
                pen = pen + eqtb[8938745L /*int_base 5 */ ].cint;
            if (d)
                q = eqtb[2253042L /*display_widow_penalties_loc */ ].hh.v.RH;
            else
                q = eqtb[2253041L /*widow_penalties_loc */ ].hh.v.RH;
            if (q != -268435455L) {
                r = best_line - cur_line - 1;
                if (r > mem[q + 1].cint)
                    r = mem[q + 1].cint;
                pen = pen + mem[q + r + 1].cint;
            } else if (cur_line + 2 == best_line) {

                if (d)
                    pen = pen + eqtb[8938747L /*int_base 7 */ ].cint;
                else
                    pen = pen + eqtb[8938746L /*int_base 6 */ ].cint;
            }
            if (disc_break)
                pen = pen + eqtb[8938748L /*int_base 8 */ ].cint;
            if (pen != 0) {
                r = new_penalty(pen);
                mem[cur_list.tail_field].hh.v.RH = r;
                cur_list.tail_field = r;
            }
        }
        cur_line++;
        cur_p = mem[cur_p + 1].hh.v.LH;
        if (cur_p != -268435455L) {

            if (!post_disc_break) {     /*908: */
                r = mem_top - 3;
                while (true) {

                    q = mem[r].hh.v.RH;
                    if (q == mem[cur_p + 1].hh.v.RH)
                        goto lab31;
                    if ((q >= hi_mem_min))
                        goto lab31;
                    if ((mem[q].hh.u.B0 < 9 /*math_node */ ))
                        goto lab31;
                    if (mem[q].hh.u.B0 == 11 /*kern_node */ ) {

                        if ((mem[q].hh.u.B1 != 1 /*explicit */ ) && (mem[q].hh.u.B1 != 3 /*space_adjustment */ ))
                            goto lab31;
                    }
                    r = q;
                    if (mem[q].hh.u.B0 == 9 /*math_node */ ) {

                        if ((eqtb[8938811L /*eTeX_state_base 0 */ ].cint > 0)) {        /*1495: */

                            if (odd(mem[q].hh.u.B1)) {
                                if (LR_ptr != -268435455L) {

                                    if (mem[LR_ptr].hh.v.LH == (4 /*L_code */  * (mem[q].hh.u.B1 / 4 /*L_code */ ) + 3)) {
                                        temp_ptr = LR_ptr;
                                        LR_ptr = mem[temp_ptr].hh.v.RH;
                                        {
                                            mem[temp_ptr].hh.v.RH = avail;
                                            avail = temp_ptr;
                                        }
                                    }
                                }
                            } else {

                                temp_ptr = get_avail();
                                mem[temp_ptr].hh.v.LH = (4 /*L_code */  * (mem[q].hh.u.B1 / 4 /*L_code */ ) + 3);
                                mem[temp_ptr].hh.v.RH = LR_ptr;
                                LR_ptr = temp_ptr;
                            }
                        }
                    }
                }
 lab31:                        /*done1 */ if (r != mem_top - 3) {
                    mem[r].hh.v.RH = -268435455L;
                    flush_node_list(mem[mem_top - 3].hh.v.RH);
                    mem[mem_top - 3].hh.v.RH = q;
                }
            }
        }
    } while (!(cur_p == -268435455L));
    if ((cur_line != best_line) || (mem[mem_top - 3].hh.v.RH != -268435455L))
        confusion(66337L /*"line breaking" */ );
    cur_list.pg_field = best_line - 1;
    cur_list.eTeX_aux_field = LR_ptr;
}

small_number zreconstitute(small_number j, small_number n, halfword bchar, halfword hchar)
{
    register small_number Result;
    reconstitute_regmem halfword p;
    halfword t;
    four_quarters q;
    halfword cur_rh;
    halfword test_char;
    scaled w;
    font_index k;
    hyphen_passed = 0;
    t = mem_top - 4;
    w = 0;
    mem[mem_top - 4].hh.v.RH = -268435455L;
    cur_l = hu[j];
    cur_q = t;
    if (j == 0) {
        ligature_present = init_lig;
        p = init_list;
        if (ligature_present)
            lft_hit = init_lft;
        while (p > -268435455L) {

            {
                mem[t].hh.v.RH = get_avail();
                t = mem[t].hh.v.RH;
                mem[t].hh.u.B0 = hf;
                mem[t].hh.u.B1 = mem[p].hh.u.B1;
            }
            p = mem[p].hh.v.RH;
        }
    } else if (cur_l < 65536L /*too_big_char */ ) {
        mem[t].hh.v.RH = get_avail();
        t = mem[t].hh.v.RH;
        mem[t].hh.u.B0 = hf;
        mem[t].hh.u.B1 = cur_l;
    }
    lig_stack = -268435455L;
    {
        if (j < n)
            cur_r = hu[j + 1];
        else
            cur_r = bchar;
        if (odd(hyf[j]))
            cur_rh = hchar;
        else
            cur_rh = 65536L /*too_big_char */ ;
    }
 lab22:/*continue *//*944: */ if (cur_l == 65536L /*too_big_char */ ) {
        k = bchar_label[hf];
        if (k == 0 /*non_address */ )
            goto lab30;
        else
            q = font_info[k].qqqq;
    } else {

        q = font_info[char_base[hf] + effective_char(true, hf, cur_l)].qqqq;
        if (((q.u.B2) % 4) != 1 /*lig_tag */ )
            goto lab30;
        k = lig_kern_base[hf] + q.u.B3;
        q = font_info[k].qqqq;
        if (q.u.B0 > 128) {
            k = lig_kern_base[hf] + 256 * q.u.B2 + q.u.B3 + 32768L - 256 * (128);
            q = font_info[k].qqqq;
        }
    }
    if (cur_rh < 65536L /*too_big_char */ )
        test_char = cur_rh;
    else
        test_char = cur_r;
    while (true) {

        if (q.u.B1 == test_char) {

            if (q.u.B0 <= 128) {

                if (cur_rh < 65536L /*too_big_char */ ) {
                    hyphen_passed = j;
                    hchar = 65536L /*too_big_char */ ;
                    cur_rh = 65536L /*too_big_char */ ;
                    goto lab22;
                } else {

                    if (hchar < 65536L /*too_big_char */ ) {

                        if (odd(hyf[j])) {
                            hyphen_passed = j;
                            hchar = 65536L /*too_big_char */ ;
                        }
                    }
                    if (q.u.B2 < 128) {   /*946: */
                        if (cur_l == 65536L /*too_big_char */ )
                            lft_hit = true;
                        if (j == n) {

                            if (lig_stack == -268435455L)
                                rt_hit = true;
                        }
                        {
                            if (interrupt != 0)
                                pause_for_instructions();
                        }
                        switch (q.u.B2) {
                        case 1:
                        case 5:
                            {
                                cur_l = q.u.B3;
                                ligature_present = true;
                            }
                            break;
                        case 2:
                        case 6:
                            {
                                cur_r = q.u.B3;
                                if (lig_stack > -268435455L)
                                    mem[lig_stack].hh.u.B1 = cur_r;
                                else {

                                    lig_stack = new_lig_item(cur_r);
                                    if (j == n)
                                        bchar = 65536L /*too_big_char */ ;
                                    else {

                                        p = get_avail();
                                        mem[lig_stack + 1].hh.v.RH = p;
                                        mem[p].hh.u.B1 = hu[j + 1];
                                        mem[p].hh.u.B0 = hf;
                                    }
                                }
                            }
                            break;
                        case 3:
                            {
                                cur_r = q.u.B3;
                                p = lig_stack;
                                lig_stack = new_lig_item(cur_r);
                                mem[lig_stack].hh.v.RH = p;
                            }
                            break;
                        case 7:
                        case 11:
                            {
                                if (ligature_present) {
                                    p = new_ligature(hf, cur_l, mem[cur_q].hh.v.RH);
                                    if (lft_hit) {
                                        mem[p].hh.u.B1 = 2;
                                        lft_hit = false;
                                    }
                                    if (false) {

                                        if (lig_stack == -268435455L) {
                                            mem[p].hh.u.B1++;
                                            rt_hit = false;
                                        }
                                    }
                                    mem[cur_q].hh.v.RH = p;
                                    t = p;
                                    ligature_present = false;
                                }
                                cur_q = t;
                                cur_l = q.u.B3;
                                ligature_present = true;
                            }
                            break;
                        default:
                            {
                                cur_l = q.u.B3;
                                ligature_present = true;
                                if (lig_stack > -268435455L) {
                                    if (mem[lig_stack + 1].hh.v.RH > -268435455L) {
                                        mem[t].hh.v.RH = mem[lig_stack + 1].hh.v.RH;
                                        t = mem[t].hh.v.RH;
                                        j++;
                                    }
                                    p = lig_stack;
                                    lig_stack = mem[p].hh.v.RH;
                                    free_node(p, 2 /*small_node_size */ );
                                    if (lig_stack == -268435455L) {
                                        if (j < n)
                                            cur_r = hu[j + 1];
                                        else
                                            cur_r = bchar;
                                        if (odd(hyf[j]))
                                            cur_rh = hchar;
                                        else
                                            cur_rh = 65536L /*too_big_char */ ;
                                    } else
                                        cur_r = mem[lig_stack].hh.u.B1;
                                } else if (j == n)
                                    goto lab30;
                                else {

                                    {
                                        mem[t].hh.v.RH = get_avail();
                                        t = mem[t].hh.v.RH;
                                        mem[t].hh.u.B0 = hf;
                                        mem[t].hh.u.B1 = cur_r;
                                    }
                                    j++;
                                    {
                                        if (j < n)
                                            cur_r = hu[j + 1];
                                        else
                                            cur_r = bchar;
                                        if (odd(hyf[j]))
                                            cur_rh = hchar;
                                        else
                                            cur_rh = 65536L /*too_big_char */ ;
                                    }
                                }
                            }
                            break;
                        }
                        if (q.u.B2 > 4) {

                            if (q.u.B2 != 7)
                                goto lab30;
                        }
                        goto lab22;
                    }
                    w = font_info[kern_base[hf] + 256 * q.u.B2 + q.u.B3].cint;
                    goto lab30;
                }
            }
        }
        if (q.u.B0 >= 128) {

            if (cur_rh == 65536L /*too_big_char */ )
                goto lab30;
            else {

                cur_rh = 65536L /*too_big_char */ ;
                goto lab22;
            }
        }
        k = k + q.u.B0 + 1;
        q = font_info[k].qqqq;
    }
 lab30:                        /*done *//*:944 */ ;
    if (ligature_present) {
        p = new_ligature(hf, cur_l, mem[cur_q].hh.v.RH);
        if (lft_hit) {
            mem[p].hh.u.B1 = 2;
            lft_hit = false;
        }
        if (rt_hit) {

            if (lig_stack == -268435455L) {
                mem[p].hh.u.B1++;
                rt_hit = false;
            }
        }
        mem[cur_q].hh.v.RH = p;
        t = p;
        ligature_present = false;
    }
    if (w != 0) {
        mem[t].hh.v.RH = new_kern(w);
        t = mem[t].hh.v.RH;
        w = 0;
        mem[t + 2].hh.v.LH = 0;
    }
    if (lig_stack > -268435455L) {
        cur_q = t;
        cur_l = mem[lig_stack].hh.u.B1;
        ligature_present = true;
        {
            if (mem[lig_stack + 1].hh.v.RH > -268435455L) {
                mem[t].hh.v.RH = mem[lig_stack + 1].hh.v.RH;
                t = mem[t].hh.v.RH;
                j++;
            }
            p = lig_stack;
            lig_stack = mem[p].hh.v.RH;
            free_node(p, 2 /*small_node_size */ );
            if (lig_stack == -268435455L) {
                if (j < n)
                    cur_r = hu[j + 1];
                else
                    cur_r = bchar;
                if (odd(hyf[j]))
                    cur_rh = hchar;
                else
                    cur_rh = 65536L /*too_big_char */ ;
            } else
                cur_r = mem[lig_stack].hh.u.B1;
        }
        goto lab22;
    }
    Result = j;
    return Result;
}

void hyphenate(void)
{
    hyphenate_regmem short /*hyphenatable_length_limit 2 */ i, j, l;
    halfword q, r, s;
    halfword bchar;
    halfword major_tail, minor_tail;
    UnicodeScalar c;
    short /*hyphenatable_length_limit */ c_loc;
    integer r_count;
    halfword hyf_node;
    trie_pointer z;
    integer v;
    hyph_pointer h;
    str_number k;
    pool_pointer u;
    {
        register integer for_end;
        j = 0;
        for_end = hn;
        if (j <= for_end)
            do
                hyf[j] = 0;
            while (j++ < for_end);
    }
    h = hc[1];
    hn++;
    hc[hn] = cur_lang;
    {
        register integer for_end;
        j = 2;
        for_end = hn;
        if (j <= for_end)
            do
                h = (h + h + hc[j]) % 607 /*hyph_prime */ ;
            while (j++ < for_end);
    }
    while (true) {

        k = hyph_word[h];
        if (k == 0)
            goto lab45;
        if (length(k) == hn) {
            j = 1;
            u = str_start[(k) - 65536L];
            do {
                if (str_pool[u] != hc[j])
                    goto lab30;
                j++;
                u++;
            } while (!(j > hn));
            s = hyph_list[h];
            while (s != -268435455L) {

                hyf[mem[s].hh.v.LH] = 1;
                s = mem[s].hh.v.RH;
            }
            hn--;
            goto lab40;
        }
 lab30:                        /*done *//*:966 */ ;
        h = hyph_link[h];
        if (h == 0)
            goto lab45;
        h--;
    }
 lab45:                        /*not_found */ hn--;
    if (trie_trc[cur_lang + 1] != cur_lang)
        return;
    hc[0] = 0;
    hc[hn + 1] = 0;
    hc[hn + 2] = max_hyph_char;
    {
        register integer for_end;
        j = 0;
        for_end = hn - r_hyf + 1;
        if (j <= for_end)
            do {
                z = trie_trl[cur_lang + 1] + hc[j];
                l = j;
                while (hc[l] == trie_trc[z]) {

                    if (trie_tro[z] != min_trie_op) {   /*959: */
                        v = trie_tro[z];
                        do {
                            v = v + op_start[cur_lang];
                            i = l - hyf_distance[v];
                            if (hyf_num[v] > hyf[i])
                                hyf[i] = hyf_num[v];
                            v = hyf_next[v];
                        } while (!(v == min_trie_op));
                    }
                    l++;
                    z = trie_trl[z] + hc[l];
                }
            }
            while (j++ < for_end);
    }
 lab40:                        /*found */  {
        register integer for_end;
        j = 0;
        for_end = l_hyf - 1;
        if (j <= for_end)
            do
                hyf[j] = 0;
            while (j++ < for_end);
    }
    {
        register integer for_end;
        j = 0;
        for_end = r_hyf - 1;
        if (j <= for_end)
            do
                hyf[hn - j] = 0 /*:958 */ ;
            while (j++ < for_end);
    }
    {
        register integer for_end;
        j = l_hyf;
        for_end = hn - r_hyf;
        if (j <= for_end)
            do
                if (odd(hyf[j]))
                    goto lab41;
            while (j++ < for_end) ;
    }
    return;
 lab41:                        /*found1 *//*:936 */ ;
    if ((((ha) != -268435455L && (!(ha >= hi_mem_min)) && (mem[ha].hh.u.B0 == 8 /*whatsit_node */ )
          && ((mem[ha].hh.u.B1 == 40 /*native_word_node */ ) || (mem[ha].hh.u.B1 == 41 /*native_word_node_AT */ ))))) {
        s = cur_p;
        while (mem[s].hh.v.RH != ha)
            s = mem[s].hh.v.RH;
        hyphen_passed = 0;
        {
            register integer for_end;
            j = l_hyf;
            for_end = hn - r_hyf;
            if (j <= for_end)
                do {
                    if (odd(hyf[j])) {
                        q = new_native_word_node(hf, j - hyphen_passed);
                        mem[q].hh.u.B1 = mem[ha].hh.u.B1;
                        {
                            register integer for_end;
                            i = 0;
                            for_end = j - hyphen_passed - 1;
                            if (i <= for_end)
                                do
                                    set_native_char(q, i, get_native_char(ha, i + hyphen_passed));
                                while (i++ < for_end);
                        }
                        set_native_metrics(q, (eqtb[8938814L /*eTeX_state_base 3 */ ].cint > 0));
                        mem[s].hh.v.RH = q;
                        s = q;
                        q = new_disc();
                        mem[q + 1].hh.v.LH = new_native_character(hf, hyf_char);
                        mem[s].hh.v.RH = q;
                        s = q;
                        hyphen_passed = j;
                    }
                }
                while (j++ < for_end);
        }
        hn = mem[ha + 4].qqqq.u.B2;
        q = new_native_word_node(hf, hn - hyphen_passed);
        mem[q].hh.u.B1 = mem[ha].hh.u.B1;
        {
            register integer for_end;
            i = 0;
            for_end = hn - hyphen_passed - 1;
            if (i <= for_end)
                do
                    set_native_char(q, i, get_native_char(ha, i + hyphen_passed));
                while (i++ < for_end);
        }
        set_native_metrics(q, (eqtb[8938814L /*eTeX_state_base 3 */ ].cint > 0));
        mem[s].hh.v.RH = q;
        s = q;
        q = mem[ha].hh.v.RH;
        mem[s].hh.v.RH = q;
        mem[ha].hh.v.RH = -268435455L;
        flush_node_list(ha);
    } else {

        q = mem[hb].hh.v.RH;
        mem[hb].hh.v.RH = -268435455L;
        r = mem[ha].hh.v.RH;
        mem[ha].hh.v.RH = -268435455L;
        bchar = hyf_bchar;
        if ((ha >= hi_mem_min)) {

            if (mem[ha].hh.u.B0 != hf)
                goto lab42;
            else {

                init_list = ha;
                init_lig = false;
                hu[0] = mem[ha].hh.u.B1;
            }
        } else if (mem[ha].hh.u.B0 == 6 /*ligature_node */ ) {

            if (mem[ha + 1].hh.u.B0 != hf)
                goto lab42;
            else {

                init_list = mem[ha + 1].hh.v.RH;
                init_lig = true;
                init_lft = (mem[ha].hh.u.B1 > 1);
                hu[0] = mem[ha + 1].hh.u.B1;
                if (init_list == -268435455L) {

                    if (init_lft) {
                        hu[0] = max_hyph_char;
                        init_lig = false;
                    }
                }
                free_node(ha, 2 /*small_node_size */ );
            }
        } else {

            if (!(r >= hi_mem_min)) {

                if (mem[r].hh.u.B0 == 6 /*ligature_node */ ) {

                    if (mem[r].hh.u.B1 > 1)
                        goto lab42;
                }
            }
            j = 1;
            s = ha;
            init_list = -268435455L;
            goto lab50;
        }
        s = cur_p;
        while (mem[s].hh.v.RH != ha)
            s = mem[s].hh.v.RH;
        j = 0;
        goto lab50;
 lab42:                        /*found2 */ s = ha;
        j = 0;
        hu[0] = max_hyph_char;
        init_lig = false;
        init_list = -268435455L;
 lab50:                        /*common_ending */ flush_node_list(r);
        do {
            l = j;
            j = reconstitute(j, hn, bchar, hyf_char) + 1;
            if (hyphen_passed == 0) {
                mem[s].hh.v.RH = mem[mem_top - 4].hh.v.RH;
                while (mem[s].hh.v.RH > -268435455L)
                    s = mem[s].hh.v.RH;
                if (odd(hyf[j - 1])) {
                    l = j;
                    hyphen_passed = j - 1;
                    mem[mem_top - 4].hh.v.RH = -268435455L;
                }
            }
            if (hyphen_passed > 0)      /*949: */
                do {
                    r = get_node(2 /*small_node_size */ );
                    mem[r].hh.v.RH = mem[mem_top - 4].hh.v.RH;
                    mem[r].hh.u.B0 = 7 /*disc_node */ ;
                    major_tail = r;
                    r_count = 0;
                    while (mem[major_tail].hh.v.RH > -268435455L) {

                        major_tail = mem[major_tail].hh.v.RH;
                        r_count++;
                    }
                    i = hyphen_passed;
                    hyf[i] = 0;
                    minor_tail = -268435455L;
                    mem[r + 1].hh.v.LH = -268435455L;
                    hyf_node = new_character(hf, hyf_char);
                    if (hyf_node != -268435455L) {
                        i++;
                        c = hu[i];
                        hu[i] = hyf_char;
                        {
                            mem[hyf_node].hh.v.RH = avail;
                            avail = hyf_node;
                        }
                    }
                    while (l <= i) {

                        l = reconstitute(l, i, font_bchar[hf], 65536L /*too_big_char */ ) + 1;
                        if (mem[mem_top - 4].hh.v.RH > -268435455L) {
                            if (minor_tail == -268435455L)
                                mem[r + 1].hh.v.LH = mem[mem_top - 4].hh.v.RH;
                            else
                                mem[minor_tail].hh.v.RH = mem[mem_top - 4].hh.v.RH;
                            minor_tail = mem[mem_top - 4].hh.v.RH;
                            while (mem[minor_tail].hh.v.RH > -268435455L)
                                minor_tail = mem[minor_tail].hh.v.RH;
                        }
                    }
                    if (hyf_node != -268435455L) {
                        hu[i] = c;
                        l = i;
                        i--;
                    }
                    minor_tail = -268435455L;
                    mem[r + 1].hh.v.RH = -268435455L;
                    c_loc = 0;
                    if (bchar_label[hf] != 0 /*non_address */ ) {
                        l--;
                        c = hu[l];
                        c_loc = l;
                        hu[l] = max_hyph_char;
                    }
                    while (l < j) {

                        do {
                            l = reconstitute(l, hn, bchar, 65536L /*too_big_char */ ) + 1;
                            if (c_loc > 0) {
                                hu[c_loc] = c;
                                c_loc = 0;
                            }
                            if (mem[mem_top - 4].hh.v.RH > -268435455L) {
                                if (minor_tail == -268435455L)
                                    mem[r + 1].hh.v.RH = mem[mem_top - 4].hh.v.RH;
                                else
                                    mem[minor_tail].hh.v.RH = mem[mem_top - 4].hh.v.RH;
                                minor_tail = mem[mem_top - 4].hh.v.RH;
                                while (mem[minor_tail].hh.v.RH > -268435455L)
                                    minor_tail = mem[minor_tail].hh.v.RH;
                            }
                        } while (!(l >= j));
                        while (l > j) { /*952: */

                            j = reconstitute(j, hn, bchar, 65536L /*too_big_char */ ) + 1;
                            mem[major_tail].hh.v.RH = mem[mem_top - 4].hh.v.RH;
                            while (mem[major_tail].hh.v.RH > -268435455L) {

                                major_tail = mem[major_tail].hh.v.RH;
                                r_count++;
                            }
                        }
                    }
                    if (r_count > 127) {
                        mem[s].hh.v.RH = mem[r].hh.v.RH;
                        mem[r].hh.v.RH = -268435455L;
                        flush_node_list(r);
                    } else {

                        mem[s].hh.v.RH = r;
                        mem[r].hh.u.B1 = r_count;
                    }
                    s = /*:953 */ major_tail;
                    hyphen_passed = j - 1;
                    mem[mem_top - 4].hh.v.RH = -268435455L;
                } while (!(!odd(hyf[j - 1]) /*:949 */ ));
        } while (!(j > hn));
        mem[s].hh.v.RH = /*:948 */ q;
        flush_list(init_list);
    }
}

integer max_hyphenatable_length(void)
{
    register integer Result;
    max_hyphenatable_length_regmem
        if (eqtb[8938822L /*eTeX_state_base 11 */ ].cint > 4095 /*hyphenatable_length_limit */ )
        Result = 4095 /*hyphenatable_length_limit */ ;
    else
        Result = eqtb[8938822L /*eTeX_state_base 11 */ ].cint;
    return Result;
}

boolean zeTeX_enabled(boolean b, quarterword j, halfword k)
{
    register boolean Result;
    eTeX_enabled_regmem if (!b) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66015L /*"Improper " */ );
        }
        print_cmd_chr(j, k);
        {
            help_ptr = 1;
            help_line[0] = 66829L /*"Sorry, this optional e-TeX feature has been disabled." */ ;
        }
        error();
    }
    Result = b;
    return Result;
}

void show_save_groups(void)
{
    show_save_groups_regmem integer p;
    short /*mmode */ m;
    save_pointer v;
    quarterword l;
    group_code c;
    signed char a;
    integer i;
    quarterword j;
    str_number s;
    p = nest_ptr;
    nest[p] = cur_list;
    v = save_ptr;
    l = cur_level;
    c = cur_group;
    save_ptr = cur_boundary;
    cur_level--;
    a = 1;
    print_nl(65622L /*"" */ );
    print_ln();
    while (true) {

        print_nl(65657L /*"### " */ );
        print_group(true);
        if (cur_group == 0 /*bottom_level */ )
            goto lab30;
        do {
            m = nest[p].mode_field;
            if (p > 0)
                p--;
            else
                m = 1 /*vmode */ ;
        } while (!(m != 104 /*hmode */ ));
        print(65566L /*" (" */ );
        switch (cur_group) {
        case 1:
            {
                p++;
                goto lab42;
            }
            break;
        case 2:
        case 3:
            s = 66457L /*"hbox" */ ;
            break;
        case 4:
            s = 66365L /*"vbox" */ ;
            break;
        case 5:
            s = 66456L /*"vtop" */ ;
            break;
        case 6:
            if (a == 0) {
                if (m == -1)
                    s = 65827L /*"halign" */ ;
                else
                    s = 65854L /*"valign" */ ;
                a = 1;
                goto lab41;
            } else {

                if (a == 1)
                    print(66866L /*"align entry" */ );
                else
                    print_esc(66289L /*"cr" */ );
                if (p >= a)
                    p = p - a;
                a = 0;
                goto lab40;
            }
            break;
        case 7:
            {
                p++;
                a = -1;
                print_esc(65840L /*"noalign" */ );
                goto lab42;
            }
            break;
        case 8:
            {
                print_esc(65694L /*"output" */ );
                goto lab40;
            }
            break;
        case 9:
            goto lab42;
            break;
        case 10:
        case 13:
            {
                if (cur_group == 10 /*disc_group */ )
                    print_esc(65635L /*"discretionary" */ );
                else
                    print_esc(65838L /*"mathchoice" */ );
                {
                    register integer for_end;
                    i = 1;
                    for_end = 3;
                    if (i <= for_end)
                        do
                            if (i <= save_stack[save_ptr - 2].cint)
                                print(66232L /*"__" */ );
                        while (i++ < for_end) ;
                }
                goto lab42;
            }
            break;
        case 11:
            {
                if (save_stack[save_ptr - 2].cint == 255)
                    print_esc(65638L /*"vadjust" */ );
                else {

                    print_esc(65614L /*"insert" */ );
                    print_int(save_stack[save_ptr - 2].cint);
                }
                goto lab42;
            }
            break;
        case 12:
            {
                s = 65855L /*"vcenter" */ ;
                goto lab41;
            }
            break;
        case 14:
            {
                p++;
                print_esc(65817L /*"begingroup" */ );
                goto lab40;
            }
            break;
        case 15:
            {
                if (m == 207 /*mmode */ )
                    print_char(36 /*"$" */ );
                else if (nest[p].mode_field == 207 /*mmode */ ) {
                    print_cmd_chr(48 /*eq_no */ , save_stack[save_ptr - 2].cint);
                    goto lab40;
                }
                print_char(36 /*"$" */ );
                goto lab40;
            }
            break;
        case 16:
            {
                if (mem[nest[p + 1].eTeX_aux_field].hh.u.B0 == 30 /*left_noad */ )
                    print_esc(66248L /*"left" */ );
                else
                    print_esc(66250L /*"middle" */ );
                goto lab40;
            }
            break;
        }
        i = save_stack[save_ptr - 4].cint;
        if (i != 0) {

            if (i < 1073741824L) {
                if (abs(nest[p].mode_field) == 1 /*vmode */ )
                    j = 21 /*hmove */ ;
                else
                    j = 22 /*vmove */ ;
                if (i > 0)
                    print_cmd_chr(j, 0);
                else
                    print_cmd_chr(j, 1);
                print_scaled(abs(i));
                print(65693L /*"pt" */ );
            } else if (i < 1073807360L) {
                if (i >= 1073774592L) {
                    print_esc(66574L /*"global" */ );
                    i = i - (32768L);
                }
                print_esc(65852L /*"setbox" */ );
                print_int(i - 1073741824L);
                print_char(61 /*"=" */ );
            } else
                print_cmd_chr(31 /*leader_ship */ , i - (1073807261L));
        }
 lab41:                        /*found1 */ print_esc(s);
        if (save_stack[save_ptr - 2].cint != 0) {
            print_char(32 /*" " */ );
            if (save_stack[save_ptr - 3].cint == 0 /*exactly */ )
                print(66213L /*"to" */ );
            else
                print(66214L /*"spread" */ );
            print_scaled(save_stack[save_ptr - 2].cint);
            print(65693L /*"pt" */ );
        }
 lab42:/*found2 */ print_char(123 /*"_" */ );
 lab40:/*found */ print_char(41 /*")" */ );
        cur_level--;
        cur_group = save_stack[save_ptr].hh.u.B1;
        save_ptr = save_stack[save_ptr].hh.v.RH;
    }
 lab30:                        /*done */ save_ptr = v;
    cur_level = l;
    cur_group = c;
}

halfword zvert_break(halfword p, scaled h, scaled d)
{
    register halfword Result;
    vert_break_regmem halfword prev_p;
    halfword q, r;
    integer pi;
    integer b;
    integer least_cost;
    halfword best_place;
    scaled prev_dp;
    small_number t;
    prev_p = p;
    least_cost = 1073741823L;
    active_width[1] = 0;
    active_width[2] = 0;
    active_width[3] = 0;
    active_width[4] = 0;
    active_width[5] = 0;
    active_width[6] = 0;
    prev_dp = 0;
    while (true) {

        if (p == -268435455L)
            pi = -10000 /*eject_penalty */ ;
        else                    /*1008: */
            switch (mem[p].hh.u.B0) {
            case 0:
            case 1:
            case 2:
                {
                    active_width[1] = active_width[1] + prev_dp + mem[p + 3].cint;
                    prev_dp = mem[p + 2].cint;
                    goto lab45;
                }
                break;
            case 8:
                {
                    if ((mem[p].hh.u.B1 == 43 /*pic_node */ ) || (mem[p].hh.u.B1 == 44 /*pdf_node */ )) {
                        active_width[1] = active_width[1] + prev_dp + mem[p + 3].cint;
                        prev_dp = mem[p + 2].cint;
                    }
                    goto lab45;
                }
                break;
            case 10:
                if ((mem[prev_p].hh.u.B0 < 9 /*math_node */ ))
                    pi = 0;
                else
                    goto lab90;
                break;
            case 11:
                {
                    if (mem[p].hh.v.RH == -268435455L)
                        t = 12 /*penalty_node */ ;
                    else
                        t = mem[mem[p].hh.v.RH].hh.u.B0;
                    if (t == 10 /*glue_node */ )
                        pi = 0;
                    else
                        goto lab90;
                }
                break;
            case 12:
                pi = mem[p + 1].cint;
                break;
            case 4:
            case 3:
                goto lab45;
                break;
            default:
                confusion(66358L /*"vertbreak" */ );
                break;
            }
        if (pi < 10000 /*inf_penalty */ ) {
            if (active_width[1] < h) {

                if ((active_width[3] != 0) || (active_width[4] != 0) || (active_width[5] != 0))
                    b = 0;
                else
                    b = badness(h - active_width[1], active_width[2]);
            } else if (active_width[1] - h > active_width[6])
                b = 1073741823L;
            else
                b = badness(active_width[1] - h, active_width[6]) /*:1010 */ ;
            if (b < 1073741823L) {

                if (pi <= -10000 /*eject_penalty */ )
                    b = pi;
                else if (b < 10000 /*inf_bad */ )
                    b = b + pi;
                else
                    b = 100000L;
            }
            if (b <= least_cost) {
                best_place = p;
                least_cost = b;
                best_height_plus_depth = active_width[1] + prev_dp;
            }
            if ((b == 1073741823L) || (pi <= -10000 /*eject_penalty */ ))
                goto lab30;
        }
        if ((mem[p].hh.u.B0 < 10 /*glue_node */ ) || (mem[p].hh.u.B0 > 11 /*kern_node */ ))
            goto lab45;
 lab90:/*update_heights *//*1011: */ if (mem[p].hh.u.B0 == 11 /*kern_node */ )
            q = p;
        else {

            q = mem[p + 1].hh.v.LH;
            active_width[2 + mem[q].hh.u.B0] = active_width[2 + mem[q].hh.u.B0] + mem[q + 2].cint;
            active_width[6] = active_width[6] + mem[q + 3].cint;
            if ((mem[q].hh.u.B1 != 0 /*normal */ ) && (mem[q + 3].cint != 0)) {
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66359L /*"Infinite glue shrinkage found in box being split" */ );
                }
                {
                    help_ptr = 4;
                    help_line[3] = 66360L /*"The box you are \vsplitting contains some infinitely" */ ;
                    help_line[2] = 66361L /*"shrinkable glue, e.g., `\vss' or `\vskip 0pt minus 1fil'." */ ;
                    help_line[1] = 66362L /*"Such glue doesn't belong there; but you can safely proceed," */ ;
                    help_line[0] = 66312L /*"since the offensive shrinkability has been made finite." */ ;
                }
                error();
                r = new_spec(q);
                mem[r].hh.u.B1 = 0 /*normal */ ;
                delete_glue_ref(q);
                mem[p + 1].hh.v.LH = r;
                q = r;
            }
        }
        active_width[1] = active_width[1] + prev_dp + mem[q + 1].cint;
        prev_dp = 0 /*:1011 */ ;
 lab45:                        /*not_found */ if (prev_dp > d) {
            active_width[1] = active_width[1] + prev_dp - d;
            prev_dp = d;
        }
        prev_p = p;
        p = mem[prev_p].hh.v.RH;
    }
 lab30:                        /*done */ Result = best_place;
    return Result;
}

halfword zvsplit(halfword n, scaled h)
{
    register halfword Result;
    vsplit_regmem halfword v;
    halfword p;
    halfword q;
    cur_val = n;
    if (cur_val < 256)
        v = eqtb[2253043L /*box_base */  + cur_val].hh.v.RH;
    else {

        find_sa_element(4, cur_val, false);
        if (cur_ptr == -268435455L)
            v = -268435455L;
        else
            v = mem[cur_ptr + 1].hh.v.RH;
    }
    flush_node_list(disc_ptr[3 /*vsplit_code */ ]);
    disc_ptr[3 /*vsplit_code */ ] = -268435455L;
    if (sa_root[7 /*mark_val */ ] != -268435455L) {

        if (do_marks(0, 0, sa_root[7 /*mark_val */ ]))
            sa_root[7 /*mark_val */ ] = -268435455L;
    }
    if (cur_mark[3 /*split_first_mark_code */ ] != -268435455L) {
        delete_token_ref(cur_mark[3 /*split_first_mark_code */ ]);
        cur_mark[3 /*split_first_mark_code */ ] = -268435455L;
        delete_token_ref(cur_mark[4 /*split_bot_mark_code */ ]);
        cur_mark[4 /*split_bot_mark_code */ ] = -268435455L;
    }
    if (v == -268435455L) {
        Result = -268435455L;
        return Result;
    }
    if (mem[v].hh.u.B0 != 1 /*vlist_node */ ) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(65622L /*"" */ );
        }
        print_esc(66363L /*"vsplit" */ );
        print(66364L /*" needs a " */ );
        print_esc(66365L /*"vbox" */ );
        {
            help_ptr = 2;
            help_line[1] = 66366L /*"The box you are trying to split is an \hbox." */ ;
            help_line[0] = 66367L /*"I can't split such a box, so I'll leave it alone." */ ;
        }
        error();
        Result = -268435455L;
        return Result;
    }
    q = vert_break(mem[v + 5].hh.v.RH, h, eqtb[10053198L /*dimen_base 6 */ ].cint);
    p = mem[v + 5].hh.v.RH;
    if (p == q)
        mem[v + 5].hh.v.RH = -268435455L;
    else
        while (true) {

            if (mem[p].hh.u.B0 == 4 /*mark_node */ ) {

                if (mem[p + 1].hh.v.LH != 0) {  /*1615: */
                    find_sa_element(7 /*mark_val */ , mem[p + 1].hh.v.LH, true);
                    if (mem[cur_ptr + 2].hh.v.RH == -268435455L) {
                        mem[cur_ptr + 2].hh.v.RH = mem[p + 1].hh.v.RH;
                        mem[mem[p + 1].hh.v.RH].hh.v.LH++;
                    } else
                        delete_token_ref(mem[cur_ptr + 3].hh.v.LH);
                    mem[cur_ptr + 3].hh.v.LH = mem[p + 1].hh.v.RH;
                    mem[mem[p + 1].hh.v.RH].hh.v.LH++;
                } else if (cur_mark[3 /*split_first_mark_code */ ] == -268435455L) {
                    cur_mark[3 /*split_first_mark_code */ ] = mem[p + 1].hh.v.RH;
                    cur_mark[4 /*split_bot_mark_code */ ] = cur_mark[3 /*split_first_mark_code */ ];
                    mem[cur_mark[3 /*split_first_mark_code */ ]].hh.v.LH =
                        mem[cur_mark[3 /*split_first_mark_code */ ]].hh.v.LH + 2;
                } else {

                    delete_token_ref(cur_mark[4 /*split_bot_mark_code */ ]);
                    cur_mark[4 /*split_bot_mark_code */ ] = mem[p + 1].hh.v.RH;
                    mem[cur_mark[4 /*split_bot_mark_code */ ]].hh.v.LH++;
                }
            }
            if (mem[p].hh.v.RH == q) {
                mem[p].hh.v.RH = -268435455L;
                goto lab30;
            }
            p = mem[p].hh.v.RH;
        }
 lab30:                        /*done *//*:1014 */ ;
    q = prune_page_top(q, eqtb[8938805L /*int_base 65 */ ].cint > 0);
    p = mem[v + 5].hh.v.RH;
    free_node(v, 8 /*box_node_size */ );
    if (q != -268435455L)
        q = vpackage(q, 0, 1 /*additional */ , 1073741823L);
    if (cur_val < 256)
        eqtb[2253043L /*box_base */  + cur_val].hh.v.RH = q;
    else {

        find_sa_element(4, cur_val, false);
        if (cur_ptr != -268435455L) {
            mem[cur_ptr + 1].hh.v.RH = q;
            mem[cur_ptr + 1].hh.v.LH++;
            delete_sa_ref(cur_ptr);
        }
    }
    Result = vpackage(p, h, 0 /*exactly */ , eqtb[10053198L /*dimen_base 6 */ ].cint);
    return Result;
}

void print_totals(void)
{
    print_totals_regmem print_scaled(page_so_far[1]);
    if (page_so_far[2] != 0) {
        print(65594L /*" plus " */ );
        print_scaled(page_so_far[2]);
        print(65622L /*"" */ );
    }
    if (page_so_far[3] != 0) {
        print(65594L /*" plus " */ );
        print_scaled(page_so_far[3]);
        print(65593L /*"fil" */ );
    }
    if (page_so_far[4] != 0) {
        print(65594L /*" plus " */ );
        print_scaled(page_so_far[4]);
        print(66376L /*"fill" */ );
    }
    if (page_so_far[5] != 0) {
        print(65594L /*" plus " */ );
        print_scaled(page_so_far[5]);
        print(66377L /*"filll" */ );
    }
    if (page_so_far[6] != 0) {
        print(65595L /*" minus " */ );
        print_scaled(page_so_far[6]);
    }
}

void zfreeze_page_specs(small_number s)
{
    freeze_page_specs_regmem page_contents = s;
    page_so_far[0] = eqtb[10053196L /*dimen_base 4 */ ].cint;
    page_max_depth = eqtb[10053197L /*dimen_base 5 */ ].cint;
    page_so_far[7] = 0;
    page_so_far[1] = 0;
    page_so_far[2] = 0;
    page_so_far[3] = 0;
    page_so_far[4] = 0;
    page_so_far[5] = 0;
    page_so_far[6] = 0;
    least_page_cost = 1073741823L;
}

void zbox_error(eight_bits n)
{
    box_error_regmem error();
    begin_diagnostic();
    print_nl(66200L /*"The following box has been deleted:" */ );
    show_box(eqtb[2253043L /*box_base */  + n].hh.v.RH);
    end_diagnostic(true);
    flush_node_list(eqtb[2253043L /*box_base */  + n].hh.v.RH);
    eqtb[2253043L /*box_base */  + n].hh.v.RH = -268435455L;
}

void zensure_vbox(eight_bits n)
{
    ensure_vbox_regmem halfword p;
    p = eqtb[2253043L /*box_base */  + n].hh.v.RH;
    if (p != -268435455L) {

        if (mem[p].hh.u.B0 == 0 /*hlist_node */ ) {
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66387L /*"Insertions can only be added to a vbox" */ );
            }
            {
                help_ptr = 3;
                help_line[2] = 66388L /*"Tut tut: You're trying to \insert into a" */ ;
                help_line[1] = 66389L /*"\box register that now contains an \hbox." */ ;
                help_line[0] = 66390L /*"Proceed, and I'll discard its present contents." */ ;
            }
            box_error(n);
        }
    }
}

void zfire_up(halfword c)
{
    fire_up_regmem halfword p, q, r, s;
    halfword prev_p;
    unsigned char /*biggest_reg */ n;
    boolean wait;
    integer save_vbadness;
    scaled save_vfuzz;
    halfword save_split_top_skip;
    if (mem[best_page_break].hh.u.B0 == 12 /*penalty_node */ ) {
        geq_word_define(8938779L /*int_base 39 */ , mem[best_page_break + 1].cint);
        mem[best_page_break + 1].cint = 10000 /*inf_penalty */ ;
    } else
        geq_word_define(8938779L /*int_base 39 */ , 10000 /*inf_penalty */ );
    if (sa_root[7 /*mark_val */ ] != -268435455L) {

        if (do_marks(1, 0, sa_root[7 /*mark_val */ ]))
            sa_root[7 /*mark_val */ ] = -268435455L;
    }
    if (cur_mark[2 /*bot_mark_code */ ] != -268435455L) {
        if (cur_mark[0 /*top_mark_code */ ] != -268435455L)
            delete_token_ref(cur_mark[0 /*top_mark_code */ ]);
        cur_mark[0 /*top_mark_code */ ] = cur_mark[2 /*bot_mark_code */ ];
        mem[cur_mark[0 /*top_mark_code */ ]].hh.v.LH++;
        delete_token_ref(cur_mark[1 /*first_mark_code */ ]);
        cur_mark[1 /*first_mark_code */ ] = -268435455L;
    }
    if (c == best_page_break)
        best_page_break = -268435455L;
    if (eqtb[2253298L /*box_base 255 */ ].hh.v.RH != -268435455L) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(65622L /*"" */ );
        }
        print_esc(65705L /*"box" */ );
        print(66401L /*"255 is not void" */ );
        {
            help_ptr = 2;
            help_line[1] = 66402L /*"You shouldn't use \box255 except in \output routines." */ ;
            help_line[0] = 66390L /*"Proceed, and I'll discard its present contents." */ ;
        }
        box_error(255);
    }
    insert_penalties = 0;
    save_split_top_skip = eqtb[2252250L /*glue_base 10 */ ].hh.v.RH;
    if (eqtb[8938793L /*int_base 53 */ ].cint <= 0) {   /*1053: */
        r = mem[mem_top].hh.v.RH;
        while (r != mem_top) {

            if (mem[r + 2].hh.v.LH != -268435455L) {
                n = mem[r].hh.u.B1;
                ensure_vbox(n);
                if (eqtb[2253043L /*box_base */  + n].hh.v.RH == -268435455L)
                    eqtb[2253043L /*box_base */  + n].hh.v.RH = new_null_box();
                p = eqtb[2253043L /*box_base */  + n].hh.v.RH + 5;
                while (mem[p].hh.v.RH != -268435455L)
                    p = mem[p].hh.v.RH;
                mem[r + 2].hh.v.RH = p;
            }
            r = mem[r].hh.v.RH;
        }
    }
    q = mem_top - 4;
    mem[q].hh.v.RH = -268435455L;
    prev_p = mem_top - 2;
    p = mem[prev_p].hh.v.RH;
    while (p != best_page_break) {

        if (mem[p].hh.u.B0 == 3 /*ins_node */ ) {
            if (eqtb[8938793L /*int_base 53 */ ].cint <= 0) {   /*1055: */
                r = mem[mem_top].hh.v.RH;
                while (mem[r].hh.u.B1 != mem[p].hh.u.B1)
                    r = mem[r].hh.v.RH;
                if (mem[r + 2].hh.v.LH == -268435455L)
                    wait = true;
                else {

                    wait = false;
                    s = mem[r + 2].hh.v.RH;
                    mem[s].hh.v.RH = mem[p + 4].hh.v.LH;
                    if (mem[r + 2].hh.v.LH == p) {      /*1056: */
                        if (mem[r].hh.u.B0 == 1 /*split_up */ ) {

                            if ((mem[r + 1].hh.v.LH == p) && (mem[r + 1].hh.v.RH != -268435455L)) {
                                while (mem[s].hh.v.RH != mem[r + 1].hh.v.RH)
                                    s = mem[s].hh.v.RH;
                                mem[s].hh.v.RH = -268435455L;
                                eqtb[2252250L /*glue_base 10 */ ].hh.v.RH = mem[p + 4].hh.v.RH;
                                mem[p + 4].hh.v.LH = prune_page_top(mem[r + 1].hh.v.RH, false);
                                if (mem[p + 4].hh.v.LH != -268435455L) {
                                    temp_ptr = vpackage(mem[p + 4].hh.v.LH, 0, 1 /*additional */ , 1073741823L);
                                    mem[p + 3].cint = mem[temp_ptr + 3].cint + mem[temp_ptr + 2].cint;
                                    free_node(temp_ptr, 8 /*box_node_size */ );
                                    wait = true;
                                }
                            }
                        }
                        mem[r + 2].hh.v.LH = -268435455L;
                        n = mem[r].hh.u.B1;
                        temp_ptr = mem[eqtb[2253043L /*box_base */  + n].hh.v.RH + 5].hh.v.RH;
                        free_node(eqtb[2253043L /*box_base */  + n].hh.v.RH, 8 /*box_node_size */ );
                        eqtb[2253043L /*box_base */  + n].hh.v.RH =
                            vpackage(temp_ptr, 0, 1 /*additional */ , 1073741823L);
                    } else {

                        while (mem[s].hh.v.RH != -268435455L)
                            s = mem[s].hh.v.RH;
                        mem[r + 2].hh.v.RH = s;
                    }
                }
                mem[prev_p].hh.v.RH = mem[p].hh.v.RH;
                mem[p].hh.v.RH = -268435455L;
                if (wait) {
                    mem[q].hh.v.RH = p;
                    q = p;
                    insert_penalties++;
                } else {

                    delete_glue_ref(mem[p + 4].hh.v.RH);
                    free_node(p, 5 /*ins_node_size */ );
                }
                p = /*:1057 */ prev_p;
            }
        } else if (mem[p].hh.u.B0 == 4 /*mark_node */ ) {

            if (mem[p + 1].hh.v.LH != 0) {      /*1618: */
                find_sa_element(7 /*mark_val */ , mem[p + 1].hh.v.LH, true);
                if (mem[cur_ptr + 1].hh.v.RH == -268435455L) {
                    mem[cur_ptr + 1].hh.v.RH = mem[p + 1].hh.v.RH;
                    mem[mem[p + 1].hh.v.RH].hh.v.LH++;
                }
                if (mem[cur_ptr + 2].hh.v.LH != -268435455L)
                    delete_token_ref(mem[cur_ptr + 2].hh.v.LH);
                mem[cur_ptr + 2].hh.v.LH = mem[p + 1].hh.v.RH;
                mem[mem[p + 1].hh.v.RH].hh.v.LH++;
            } else {            /*1051: */

                if (cur_mark[1 /*first_mark_code */ ] == -268435455L) {
                    cur_mark[1 /*first_mark_code */ ] = mem[p + 1].hh.v.RH;
                    mem[cur_mark[1 /*first_mark_code */ ]].hh.v.LH++;
                }
                if (cur_mark[2 /*bot_mark_code */ ] != -268435455L)
                    delete_token_ref(cur_mark[2 /*bot_mark_code */ ]);
                cur_mark[2 /*bot_mark_code */ ] = mem[p + 1].hh.v.RH;
                mem[cur_mark[2 /*bot_mark_code */ ]].hh.v.LH++;
            }
        }
        prev_p = p;
        p = mem[prev_p].hh.v.RH;
    }
    eqtb[2252250L /*glue_base 10 */ ].hh.v.RH = save_split_top_skip;
    if (p != -268435455L) {
        if (mem[mem_top - 1].hh.v.RH == -268435455L) {

            if (nest_ptr == 0)
                cur_list.tail_field = page_tail;
            else
                nest[0].tail_field = page_tail;
        }
        mem[page_tail].hh.v.RH = mem[mem_top - 1].hh.v.RH;
        mem[mem_top - 1].hh.v.RH = p;
        mem[prev_p].hh.v.RH = -268435455L;
    }
    save_vbadness = eqtb[8938767L /*int_base 27 */ ].cint;
    eqtb[8938767L /*int_base 27 */ ].cint = 10000 /*inf_bad */ ;
    save_vfuzz = eqtb[10053201L /*dimen_base 9 */ ].cint;
    eqtb[10053201L /*dimen_base 9 */ ].cint = 1073741823L;
    eqtb[2253298L /*box_base 255 */ ].hh.v.RH =
        vpackage(mem[mem_top - 2].hh.v.RH, best_size, 0 /*exactly */ , page_max_depth);
    eqtb[8938767L /*int_base 27 */ ].cint = save_vbadness;
    eqtb[10053201L /*dimen_base 9 */ ].cint = save_vfuzz;
    if (last_glue != 1073741823L)
        delete_glue_ref(last_glue);
    page_contents = 0 /*empty */ ;
    page_tail = mem_top - 2;
    mem[mem_top - 2].hh.v.RH = -268435455L;
    last_glue = 1073741823L;
    last_penalty = 0;
    last_kern = 0;
    last_node_type = -1;
    page_so_far[7] = 0;
    page_max_depth = 0 /*:1026 */ ;
    if (q != mem_top - 4) {
        mem[mem_top - 2].hh.v.RH = mem[mem_top - 4].hh.v.RH;
        page_tail = q;
    }
    r = mem[mem_top].hh.v.RH;
    while (r != mem_top) {

        q = mem[r].hh.v.RH;
        free_node(r, 4 /*page_ins_node_size */ );
        r = q;
    }
    mem[mem_top].hh.v.RH = /*:1054 *//*:1049 */ mem_top;
    if (sa_root[7 /*mark_val */ ] != -268435455L) {

        if (do_marks(2, 0, sa_root[7 /*mark_val */ ]))
            sa_root[7 /*mark_val */ ] = -268435455L;
    }
    if ((cur_mark[0 /*top_mark_code */ ] != -268435455L) && (cur_mark[1 /*first_mark_code */ ] == -268435455L)) {
        cur_mark[1 /*first_mark_code */ ] = cur_mark[0 /*top_mark_code */ ];
        mem[cur_mark[0 /*top_mark_code */ ]].hh.v.LH++;
    }
    if (eqtb[2252772L /*output_routine_loc */ ].hh.v.RH != -268435455L) {

        if (dead_cycles >= eqtb[8938780L /*int_base 40 */ ].cint) {     /*1059: */
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66403L /*"Output loop---" */ );
            }
            print_int(dead_cycles);
            print(66404L /*" consecutive dead cycles" */ );
            {
                help_ptr = 3;
                help_line[2] = 66405L /*"I've concluded that your \output is awry; it never does a" */ ;
                help_line[1] = 66406L /*"\shipout, so I'm shipping \box255 out myself. Next time" */ ;
                help_line[0] = 66407L /*"increase \maxdeadcycles if you want me to be more patient!" */ ;
            }
            error();
        } else {                /*1060: */

            output_active = true;
            dead_cycles++;
            push_nest();
            cur_list.mode_field = -1;
            cur_list.aux_field.cint = -65536000L;
            cur_list.ml_field = -(integer) line;
            begin_token_list(eqtb[2252772L /*output_routine_loc */ ].hh.v.RH, 7 /*output_text */ );
            new_save_level(8 /*output_group */ );
            normal_paragraph();
            scan_left_brace();
            return;
        }
    }
    {
        if (mem[mem_top - 2].hh.v.RH != -268435455L) {
            if (mem[mem_top - 1].hh.v.RH == -268435455L) {

                if (nest_ptr == 0)
                    cur_list.tail_field = page_tail;
                else
                    nest[0].tail_field = page_tail;
            } else
                mem[page_tail].hh.v.RH = mem[mem_top - 1].hh.v.RH;
            mem[mem_top - 1].hh.v.RH = mem[mem_top - 2].hh.v.RH;
            mem[mem_top - 2].hh.v.RH = -268435455L;
            page_tail = mem_top - 2;
        }
        flush_node_list(disc_ptr[2 /*last_box_code */ ]);
        disc_ptr[2 /*last_box_code */ ] = -268435455L;
        ship_out(eqtb[2253298L /*box_base 255 */ ].hh.v.RH);
        eqtb[2253298L /*box_base 255 */ ].hh.v.RH = -268435455L;
    }
}

void build_page(void)
{
    build_page_regmem halfword p;
    halfword q, r;
    integer b, c;
    integer pi;
    unsigned char /*biggest_reg */ n;
    scaled delta, h, w;
    if ((mem[mem_top - 1].hh.v.RH == -268435455L) || output_active)
        return;
    do {
 lab22:                        /*continue */ p = mem[mem_top - 1].hh.v.RH;
        if (last_glue != 1073741823L)
            delete_glue_ref(last_glue);
        last_penalty = 0;
        last_kern = 0;
        last_node_type = mem[p].hh.u.B0 + 1;
        if (mem[p].hh.u.B0 == 10 /*glue_node */ ) {
            last_glue = mem[p + 1].hh.v.LH;
            mem[last_glue].hh.v.RH++;
        } else {

            last_glue = 1073741823L;
            if (mem[p].hh.u.B0 == 12 /*penalty_node */ )
                last_penalty = mem[p + 1].cint;
            else if (mem[p].hh.u.B0 == 11 /*kern_node */ )
                last_kern = mem[p + 1].cint;
        }
        switch (mem[p].hh.u.B0) {
        case 0:
        case 1:
        case 2:
            if (page_contents < 2 /*box_there */ ) {    /*1036: */
                if (page_contents == 0 /*empty */ )
                    freeze_page_specs(2 /*box_there */ );
                else
                    page_contents = 2 /*box_there */ ;
                q = new_skip_param(9 /*top_skip_code */ );
                if (mem[temp_ptr + 1].cint > mem[p + 3].cint)
                    mem[temp_ptr + 1].cint = mem[temp_ptr + 1].cint - mem[p + 3].cint;
                else
                    mem[temp_ptr + 1].cint = 0;
                mem[q].hh.v.RH = p;
                mem[mem_top - 1].hh.v.RH = q;
                goto lab22;
            } else {            /*1037: */

                page_so_far[1] = page_so_far[1] + page_so_far[7] + mem[p + 3].cint;
                page_so_far[7] = mem[p + 2].cint;
                goto lab80;
            }
            break;
        case 8:
            {
                if ((mem[p].hh.u.B1 == 43 /*pic_node */ ) || (mem[p].hh.u.B1 == 44 /*pdf_node */ )) {
                    page_so_far[1] = page_so_far[1] + page_so_far[7] + mem[p + 3].cint;
                    page_so_far[7] = mem[p + 2].cint;
                }
                goto lab80;
            }
            break;
        case 10:
            if (page_contents < 2 /*box_there */ )
                goto lab31;
            else if ((mem[page_tail].hh.u.B0 < 9 /*math_node */ ))
                pi = 0;
            else
                goto lab90;
            break;
        case 11:
            if (page_contents < 2 /*box_there */ )
                goto lab31;
            else if (mem[p].hh.v.RH == -268435455L)
                return;
            else if (mem[mem[p].hh.v.RH].hh.u.B0 == 10 /*glue_node */ )
                pi = 0;
            else
                goto lab90;
            break;
        case 12:
            if (page_contents < 2 /*box_there */ )
                goto lab31;
            else
                pi = mem[p + 1].cint;
            break;
        case 4:
            goto lab80;
            break;
        case 3:
            {
                if (page_contents == 0 /*empty */ )
                    freeze_page_specs(1 /*inserts_only */ );
                n = mem[p].hh.u.B1;
                r = mem_top;
                while (n >= mem[mem[r].hh.v.RH].hh.u.B1)
                    r = mem[r].hh.v.RH;
                n = n;
                if (mem[r].hh.u.B1 != n) {        /*1044: */
                    q = get_node(4 /*page_ins_node_size */ );
                    mem[q].hh.v.RH = mem[r].hh.v.RH;
                    mem[r].hh.v.RH = q;
                    r = q;
                    mem[r].hh.u.B1 = n;
                    mem[r].hh.u.B0 = 0 /*inserting */ ;
                    ensure_vbox(n);
                    if (eqtb[2253043L /*box_base */  + n].hh.v.RH == -268435455L)
                        mem[r + 3].cint = 0;
                    else
                        mem[r + 3].cint =
                            mem[eqtb[2253043L /*box_base */  + n].hh.v.RH + 3].cint +
                            mem[eqtb[2253043L /*box_base */  + n].hh.v.RH + 2].cint;
                    mem[r + 2].hh.v.LH = -268435455L;
                    q = eqtb[2252259L /*skip_base */  + n].hh.v.RH;
                    if (eqtb[8938824L /*count_base */  + n].cint == 1000)
                        h = mem[r + 3].cint;
                    else
                        h = x_over_n(mem[r + 3].cint, 1000) * eqtb[8938824L /*count_base */  + n].cint;
                    page_so_far[0] = page_so_far[0] - h - mem[q + 1].cint;
                    page_so_far[2 + mem[q].hh.u.B0] = page_so_far[2 + mem[q].hh.u.B0] + mem[q + 2].cint;
                    page_so_far[6] = page_so_far[6] + mem[q + 3].cint;
                    if ((mem[q].hh.u.B1 != 0 /*normal */ ) && (mem[q + 3].cint != 0)) {
                        {
                            if (interaction == 3 /*error_stop_mode */ ) ;
                            if (file_line_error_style_p)
                                print_file_line();
                            else
                                print_nl(65544L /*"! " */ );
                            print(66396L /*"Infinite glue shrinkage inserted from " */ );
                        }
                        print_esc(65691L /*"skip" */ );
                        print_int(n);
                        {
                            help_ptr = 3;
                            help_line[2] = 66397L /*"The correction glue for page breaking with insertions" */ ;
                            help_line[1] = 66398L /*"must have finite shrinkability. But you may proceed," */ ;
                            help_line[0] = 66312L /*"since the offensive shrinkability has been made finite." */ ;
                        }
                        error();
                    }
                }
                if (mem[r].hh.u.B0 == 1 /*split_up */ )
                    insert_penalties = insert_penalties + mem[p + 1].cint;
                else {

                    mem[r + 2].hh.v.RH = p;
                    delta = page_so_far[0] - page_so_far[1] - page_so_far[7] + page_so_far[6];
                    if (eqtb[8938824L /*count_base */  + n].cint == 1000)
                        h = mem[p + 3].cint;
                    else
                        h = x_over_n(mem[p + 3].cint, 1000) * eqtb[8938824L /*count_base */  + n].cint;
                    if (((h <= 0) || (h <= delta))
                        && (mem[p + 3].cint + mem[r + 3].cint <= eqtb[10053215L /*scaled_base */  + n].cint)) {
                        page_so_far[0] = page_so_far[0] - h;
                        mem[r + 3].cint = mem[r + 3].cint + mem[p + 3].cint;
                    } else {    /*1045: */

                        if (eqtb[8938824L /*count_base */  + n].cint <= 0)
                            w = 1073741823L;
                        else {

                            w = page_so_far[0] - page_so_far[1] - page_so_far[7];
                            if (eqtb[8938824L /*count_base */  + n].cint != 1000)
                                w = x_over_n(w, eqtb[8938824L /*count_base */  + n].cint) * 1000;
                        }
                        if (w > eqtb[10053215L /*scaled_base */  + n].cint - mem[r + 3].cint)
                            w = eqtb[10053215L /*scaled_base */  + n].cint - mem[r + 3].cint;
                        q = vert_break(mem[p + 4].hh.v.LH, w, mem[p + 2].cint);
                        mem[r + 3].cint = mem[r + 3].cint + best_height_plus_depth;
                        if (eqtb[8938824L /*count_base */  + n].cint != 1000)
                            best_height_plus_depth =
                                x_over_n(best_height_plus_depth, 1000) * eqtb[8938824L /*count_base */  + n].cint;
                        page_so_far[0] = page_so_far[0] - best_height_plus_depth;
                        mem[r].hh.u.B0 = 1 /*split_up */ ;
                        mem[r + 1].hh.v.RH = q;
                        mem[r + 1].hh.v.LH = p;
                        if (q == -268435455L)
                            insert_penalties = insert_penalties - 10000;
                        else if (mem[q].hh.u.B0 == 12 /*penalty_node */ )
                            insert_penalties = insert_penalties + mem[q + 1].cint;
                    }
                }
                goto lab80;
            }
            break;
        default:
            confusion(66391L /*"page" */ );
            break;
        }
        if (pi < 10000 /*inf_penalty */ ) {
            if (page_so_far[1] < page_so_far[0]) {

                if ((page_so_far[3] != 0) || (page_so_far[4] != 0) || (page_so_far[5] != 0))
                    b = 0;
                else
                    b = badness(page_so_far[0] - page_so_far[1], page_so_far[2]);
            } else if (page_so_far[1] - page_so_far[0] > page_so_far[6])
                b = 1073741823L;
            else
                b = badness(page_so_far[1] - page_so_far[0], page_so_far[6]) /*:1042 */ ;
            if (b < 1073741823L) {

                if (pi <= -10000 /*eject_penalty */ )
                    c = pi;
                else if (b < 10000 /*inf_bad */ )
                    c = b + pi + insert_penalties;
                else
                    c = 100000L;
            } else
                c = b;
            if (insert_penalties >= 10000)
                c = 1073741823L;
            if (c <= least_page_cost) {
                best_page_break = p;
                best_size = page_so_far[0];
                least_page_cost = c;
                r = mem[mem_top].hh.v.RH;
                while (r != mem_top) {

                    mem[r + 2].hh.v.LH = mem[r + 2].hh.v.RH;
                    r = mem[r].hh.v.RH;
                }
            }
            if ((c == 1073741823L) || (pi <= -10000 /*eject_penalty */ )) {
                fire_up(p);
                if (output_active)
                    return;
                goto lab30;
            }
        }
        if ((mem[p].hh.u.B0 < 10 /*glue_node */ ) || (mem[p].hh.u.B0 > 11 /*kern_node */ ))
            goto lab80;
 lab90:/*update_heights *//*1039: */ if (mem[p].hh.u.B0 == 11 /*kern_node */ )
            q = p;
        else {

            q = mem[p + 1].hh.v.LH;
            page_so_far[2 + mem[q].hh.u.B0] = page_so_far[2 + mem[q].hh.u.B0] + mem[q + 2].cint;
            page_so_far[6] = page_so_far[6] + mem[q + 3].cint;
            if ((mem[q].hh.u.B1 != 0 /*normal */ ) && (mem[q + 3].cint != 0)) {
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66392L /*"Infinite glue shrinkage found on current page" */ );
                }
                {
                    help_ptr = 4;
                    help_line[3] = 66393L /*"The page about to be output contains some infinitely" */ ;
                    help_line[2] = 66361L /*"shrinkable glue, e.g., `\vss' or `\vskip 0pt minus 1fil'." */ ;
                    help_line[1] = 66362L /*"Such glue doesn't belong there; but you can safely proceed," */ ;
                    help_line[0] = 66312L /*"since the offensive shrinkability has been made finite." */ ;
                }
                error();
                r = new_spec(q);
                mem[r].hh.u.B1 = 0 /*normal */ ;
                delete_glue_ref(q);
                mem[p + 1].hh.v.LH = r;
                q = r;
            }
        }
        page_so_far[1] = page_so_far[1] + page_so_far[7] + mem[q + 1].cint;
        page_so_far[7] = 0 /*:1039 */ ;
 lab80:                        /*contribute *//*1038: */ if (page_so_far[7] > page_max_depth) {
            page_so_far[1] = page_so_far[1] + page_so_far[7] - page_max_depth;
            page_so_far[7] = page_max_depth;
        }
        mem[page_tail].hh.v.RH = p;
        page_tail = p;
        mem[mem_top - 1].hh.v.RH = mem[p].hh.v.RH;
        mem[p].hh.v.RH = -268435455L;
        goto lab30;
 lab31:                        /*done1 *//*1034: */ mem[mem_top - 1].hh.v.RH = mem[p].hh.v.RH;
        mem[p].hh.v.RH = -268435455L;
        if (eqtb[8938805L /*int_base 65 */ ].cint > 0) {
            if (disc_ptr[2 /*last_box_code */ ] == -268435455L)
                disc_ptr[2 /*last_box_code */ ] = p;
            else
                mem[disc_ptr[1 /*copy_code */ ]].hh.v.RH = p;
            disc_ptr[1 /*copy_code */ ] = p;
        } else
            flush_node_list(p);
 lab30:                        /*done *//*:1032 */ ;
    } while (!(mem[mem_top - 1].hh.v.RH == -268435455L));
    if (nest_ptr == 0)
        cur_list.tail_field = mem_top - 1;
    else
        nest[0].tail_field = mem_top - 1 /*:1030 */ ;
}

void app_space(void)
{
    app_space_regmem halfword q;
    if ((cur_list.aux_field.hh.v.LH >= 2000) && (eqtb[2252253L /*glue_base 13 */ ].hh.v.RH != mem_bot))
        q = new_param_glue(13 /*xspace_skip_code */ );
    else {

        if (eqtb[2252252L /*glue_base 12 */ ].hh.v.RH != mem_bot)
            main_p = eqtb[2252252L /*glue_base 12 */ ].hh.v.RH;
        else {                  /*1077: */

            main_p = font_glue[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH];
            if (main_p == -268435455L) {
                main_p = new_spec(mem_bot);
                main_k = param_base[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH] + 2;
                mem[main_p + 1].cint = font_info[main_k].cint;
                mem[main_p + 2].cint = font_info[main_k + 1].cint;
                mem[main_p + 3].cint = font_info[main_k + 2].cint;
                font_glue[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH] = main_p;
            }
        }
        main_p = new_spec(main_p);
        if (cur_list.aux_field.hh.v.LH >= 2000)
            mem[main_p + 1].cint =
                mem[main_p + 1].cint + font_info[7 /*extra_space_code */  +
                                                 param_base[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH]].cint;
        mem[main_p + 2].cint = xn_over_d(mem[main_p + 2].cint, cur_list.aux_field.hh.v.LH, 1000);
        mem[main_p + 3].cint = xn_over_d(mem[main_p + 3].cint, 1000, cur_list.aux_field.hh.v.LH) /*:1079 */ ;
        q = new_glue(main_p);
        mem[main_p].hh.v.RH = -268435455L;
    }
    mem[cur_list.tail_field].hh.v.RH = q;
    cur_list.tail_field = q;
}

void insert_dollar_sign(void)
{
    insert_dollar_sign_regmem back_input();
    cur_tok = 6291492L /*math_shift_token 36 */ ;
    {
        if (interaction == 3 /*error_stop_mode */ ) ;
        if (file_line_error_style_p)
            print_file_line();
        else
            print_nl(65544L /*"! " */ );
        print(66415L /*"Missing $ inserted" */ );
    }
    {
        help_ptr = 2;
        help_line[1] = 66416L /*"I've inserted a begin-math/end-math symbol since I think" */ ;
        help_line[0] = 66417L /*"you left one out. Proceed, with fingers crossed." */ ;
    }
    ins_error();
}

void you_cant(void)
{
    you_cant_regmem {
        if (interaction == 3 /*error_stop_mode */ ) ;
        if (file_line_error_style_p)
            print_file_line();
        else
            print_nl(65544L /*"! " */ );
        print(66020L /*"You can't use `" */ );
    }
    print_cmd_chr(cur_cmd, cur_chr);
    print_in_mode(cur_list.mode_field);
}

void report_illegal_case(void)
{
    report_illegal_case_regmem you_cant();
    {
        help_ptr = 4;
        help_line[3] = 66418L /*"Sorry, but I'm not programmed to handle this case;" */ ;
        help_line[2] = 66419L /*"I'll just pretend that you didn't ask for it." */ ;
        help_line[1] = 66420L /*"If you're in the wrong mode, you might be able to" */ ;
        help_line[0] = 66421L /*"return to the right one by typing `I_' or `I$' or `I\par'." */ ;
    }
    error();
}

boolean privileged(void)
{
    register boolean Result;
    privileged_regmem if (cur_list.mode_field > 0)
        Result = true;
    else {

        report_illegal_case();
        Result = false;
    }
    return Result;
}

boolean its_all_over(void)
{
    register boolean Result;
    its_all_over_regmem if (privileged()) {
        if ((mem_top - 2 == page_tail) && (cur_list.head_field == cur_list.tail_field) && (dead_cycles == 0)) {
            Result = true;
            return Result;
        }
        back_input();
        {
            mem[cur_list.tail_field].hh.v.RH = new_null_box();
            cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
        }
        mem[cur_list.tail_field + 1].cint = eqtb[10053195L /*dimen_base 3 */ ].cint;
        {
            mem[cur_list.tail_field].hh.v.RH = new_glue(mem_bot + 8);
            cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
        }
        {
            mem[cur_list.tail_field].hh.v.RH = new_penalty(-1073741824L);
            cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
        }
        build_page();
    }
    Result = false;
    return Result;
}

void append_glue(void)
{
    append_glue_regmem small_number s;
    s = cur_chr;
    switch (s) {
    case 0:
        cur_val = mem_bot + 4;
        break;
    case 1:
        cur_val = mem_bot + 8;
        break;
    case 2:
        cur_val = mem_bot + 12;
        break;
    case 3:
        cur_val = mem_bot + 16;
        break;
    case 4:
        scan_glue(2 /*glue_val */ );
        break;
    case 5:
        scan_glue(3 /*mu_val */ );
        break;
    }
    {
        mem[cur_list.tail_field].hh.v.RH = new_glue(cur_val);
        cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
    }
    if (s >= 4 /*skip_code */ ) {
        mem[cur_val].hh.v.RH--;
        if (s > 4 /*skip_code */ )
            mem[cur_list.tail_field].hh.u.B1 = 99 /*mu_glue */ ;
    }
}

void append_kern(void)
{
    append_kern_regmem quarterword s;
    s = cur_chr;
    scan_dimen(s == 99 /*mu_glue */ , false, false);
    {
        mem[cur_list.tail_field].hh.v.RH = new_kern(cur_val);
        cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
    }
    mem[cur_list.tail_field].hh.u.B1 = s;
}

void off_save(void)
{
    off_save_regmem halfword p;
    if (cur_group == 0 /*bottom_level */ ) {    /*1101: */
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66134L /*"Extra " */ );
        }
        print_cmd_chr(cur_cmd, cur_chr);
        {
            help_ptr = 1;
            help_line[0] = 66439L /*"Things are pretty mixed up, but I think the worst is over." */ ;
        }
        error();
    } else {

        back_input();
        p = get_avail();
        mem[mem_top - 3].hh.v.RH = p;
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(65948L /*"Missing " */ );
        }
        switch (cur_group) {
        case 14:
            {
                mem[p].hh.v.LH = 35797659L /*cs_token_flag 2243228 */ ;
                print_esc(65823L /*"endgroup" */ );
            }
            break;
        case 15:
            {
                mem[p].hh.v.LH = 6291492L /*math_shift_token 36 */ ;
                print_char(36 /*"$" */ );
            }
            break;
        case 16:
            {
                mem[p].hh.v.LH = 35797660L /*cs_token_flag 2243229 */ ;
                mem[p].hh.v.RH = get_avail();
                p = mem[p].hh.v.RH;
                mem[p].hh.v.LH = 25165870L /*other_token 46 */ ;
                print_esc(66438L /*"right." */ );
            }
            break;
        default:
            {
                mem[p].hh.v.LH = 4194429L /*right_brace_token 125 */ ;
                print_char(125 /*"_" */ );
            }
            break;
        }
        print(65949L /*" inserted" */ );
        begin_token_list(mem[mem_top - 3].hh.v.RH, 5 /*inserted */ );
        {
            help_ptr = 5;
            help_line[4] = 66433L /*"I've inserted something that you may have forgotten." */ ;
            help_line[3] = 66434L /*"(See the <inserted text> above.)" */ ;
            help_line[2] = 66435L /*"With luck, this will get me unwedged. But if you" */ ;
            help_line[1] = 66436L /*"really didn't forget anything, try typing `2' now; then" */ ;
            help_line[0] = 66437L /*"my insertion and my current dilemma will both disappear." */ ;
        }
        error();
    }
}

void extra_right_brace(void)
{
    extra_right_brace_regmem {
        if (interaction == 3 /*error_stop_mode */ ) ;
        if (file_line_error_style_p)
            print_file_line();
        else
            print_nl(65544L /*"! " */ );
        print(66444L /*"Extra _, or forgotten " */ );
    }
    switch (cur_group) {
    case 14:
        print_esc(65823L /*"endgroup" */ );
        break;
    case 15:
        print_char(36 /*"$" */ );
        break;
    case 16:
        print_esc(66249L /*"right" */ );
        break;
    }
    {
        help_ptr = 5;
        help_line[4] = 66445L /*"I've deleted a group-closing symbol because it seems to be" */ ;
        help_line[3] = 66446L /*"spurious, as in `$x_$'. But perhaps the _ is legitimate and" */ ;
        help_line[2] = 66447L /*"you forgot something else, as in `\hbox_$x_'. In such cases" */ ;
        help_line[1] = 66448L /*"the way to recover is to insert both the forgotten and the" */ ;
        help_line[0] = 66449L /*"deleted material, e.g., by typing `I$_'." */ ;
    }
    error();
    align_state++;
}

void normal_paragraph(void)
{
    normal_paragraph_regmem if (eqtb[8938759L /*int_base 19 */ ].cint != 0)
        eq_word_define(8938759L /*int_base 19 */ , 0);
    if (eqtb[10053209L /*dimen_base 17 */ ].cint != 0)
        eq_word_define(10053209L /*dimen_base 17 */ , 0);
    if (eqtb[8938781L /*int_base 41 */ ].cint != 1)
        eq_word_define(8938781L /*int_base 41 */ , 1);
    if (eqtb[2252771L /*par_shape_loc */ ].hh.v.RH != -268435455L)
        eq_define(2252771L /*par_shape_loc */ , 120 /*shape_ref */ , -268435455L);
    if (eqtb[2253039L /*inter_line_penalties_loc */ ].hh.v.RH != -268435455L)
        eq_define(2253039L /*inter_line_penalties_loc */ , 120 /*shape_ref */ , -268435455L);
}

void zbox_end(integer box_context)
{
    box_end_regmem halfword p;
    small_number a;
    if (box_context < 1073741824L) {    /*1111: */
        if (cur_box != -268435455L) {
            mem[cur_box + 4].cint = box_context;
            if (abs(cur_list.mode_field) == 1 /*vmode */ ) {
                if (pre_adjust_tail != -268435455L) {
                    if (mem_top - 14 != pre_adjust_tail) {
                        mem[cur_list.tail_field].hh.v.RH = mem[mem_top - 14].hh.v.RH;
                        cur_list.tail_field = pre_adjust_tail;
                    }
                    pre_adjust_tail = -268435455L;
                }
                append_to_vlist(cur_box);
                if (adjust_tail != -268435455L) {
                    if (mem_top - 5 != adjust_tail) {
                        mem[cur_list.tail_field].hh.v.RH = mem[mem_top - 5].hh.v.RH;
                        cur_list.tail_field = adjust_tail;
                    }
                    adjust_tail = -268435455L;
                }
                if (cur_list.mode_field > 0)
                    build_page();
            } else {

                if (abs(cur_list.mode_field) == 104 /*hmode */ )
                    cur_list.aux_field.hh.v.LH = 1000;
                else {

                    p = new_noad();
                    mem[p + 1].hh.v.RH = 2 /*sub_box */ ;
                    mem[p + 1].hh.v.LH = cur_box;
                    cur_box = p;
                }
                mem[cur_list.tail_field].hh.v.RH = cur_box;
                cur_list.tail_field = cur_box;
            }
        }
    } else if (box_context < 1073807360L) {     /*1112: */
        if (box_context < 1073774592L) {
            cur_val = box_context - 1073741824L;
            a = 0;
        } else {

            cur_val = box_context - 1073774592L;
            a = 4;
        }
        if (cur_val < 256) {

            if ((a >= 4))
                geq_define(2253043L /*box_base */  + cur_val, 121 /*box_ref */ , cur_box);
            else
                eq_define(2253043L /*box_base */  + cur_val, 121 /*box_ref */ , cur_box);
        } else {

            find_sa_element(4, cur_val, true);
            if ((a >= 4))
                gsa_def(cur_ptr, cur_box);
            else
                sa_def(cur_ptr, cur_box);
        }
    } else if (cur_box != -268435455L) {

        if (box_context > 1073807360L) {        /*1113: */
            do {
                get_x_token();
            } while (!((cur_cmd != 10 /*spacer */ ) && (cur_cmd != 0 /*relax */ ) /*:422 */ ));
            if (((cur_cmd == 26 /*hskip */ ) && (abs(cur_list.mode_field) != 1 /*vmode */ ))
                || ((cur_cmd == 27 /*vskip */ ) && (abs(cur_list.mode_field) == 1 /*vmode */ ))) {
                append_glue();
                mem[cur_list.tail_field].hh.u.B1 = box_context - (1073807261L);
                mem[cur_list.tail_field + 1].hh.v.RH = cur_box;
            } else {

                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66462L /*"Leaders not followed by proper glue" */ );
                }
                {
                    help_ptr = 3;
                    help_line[2] = 66463L /*"You should say `\leaders <box or rule><hskip or vskip>'." */ ;
                    help_line[1] = 66464L /*"I found the <box or rule>, but there's no suitable" */ ;
                    help_line[0] = 66465L /*"<hskip or vskip>, so I'm ignoring these leaders." */ ;
                }
                back_error();
                flush_node_list(cur_box);
            }
        } else
            ship_out(cur_box);
    }
}

void zbegin_box(integer box_context)
{
    begin_box_regmem halfword p, q;
    halfword r;
    boolean fm;
    halfword tx;
    quarterword m;
    halfword k;
    halfword n;
    switch (cur_chr) {
    case 0:
        {
            scan_register_num();
            if (cur_val < 256)
                cur_box = eqtb[2253043L /*box_base */  + cur_val].hh.v.RH;
            else {

                find_sa_element(4, cur_val, false);
                if (cur_ptr == -268435455L)
                    cur_box = -268435455L;
                else
                    cur_box = mem[cur_ptr + 1].hh.v.RH;
            }
            if (cur_val < 256)
                eqtb[2253043L /*box_base */  + cur_val].hh.v.RH = -268435455L;
            else {

                find_sa_element(4, cur_val, false);
                if (cur_ptr != -268435455L) {
                    mem[cur_ptr + 1].hh.v.RH = -268435455L;
                    mem[cur_ptr + 1].hh.v.LH++;
                    delete_sa_ref(cur_ptr);
                }
            }
        }
        break;
    case 1:
        {
            scan_register_num();
            if (cur_val < 256)
                q = eqtb[2253043L /*box_base */  + cur_val].hh.v.RH;
            else {

                find_sa_element(4, cur_val, false);
                if (cur_ptr == -268435455L)
                    q = -268435455L;
                else
                    q = mem[cur_ptr + 1].hh.v.RH;
            }
            cur_box = copy_node_list(q);
        }
        break;
    case 2:
        {
            cur_box = -268435455L;
            if (abs(cur_list.mode_field) == 207 /*mmode */ ) {
                you_cant();
                {
                    help_ptr = 1;
                    help_line[0] = 66467L /*"Sorry; this \lastbox will be void." */ ;
                }
                error();
            } else if ((cur_list.mode_field == 1 /*vmode */ ) && (cur_list.head_field == cur_list.tail_field)) {
                you_cant();
                {
                    help_ptr = 2;
                    help_line[1] = 66468L /*"Sorry...I usually can't take things from the current page." */ ;
                    help_line[0] = 66469L /*"This \lastbox will therefore be void." */ ;
                }
                error();
            } else {

                tx = cur_list.tail_field;
                if (!(tx >= hi_mem_min)) {

                    if ((mem[tx].hh.u.B0 == 9 /*math_node */ ) && (mem[tx].hh.u.B1 == 3 /*end_M_code */ )) {
                        r = cur_list.head_field;
                        do {
                            q = r;
                            r = mem[q].hh.v.RH;
                        } while (!(r == tx));
                        tx = q;
                    }
                }
                if (!(tx >= hi_mem_min)) {

                    if ((mem[tx].hh.u.B0 == 0 /*hlist_node */ ) || (mem[tx].hh.u.B0 == 1 /*vlist_node */ )) {       /*1116: */
                        q = cur_list.head_field;
                        p = -268435455L;
                        do {
                            r = p;
                            p = q;
                            fm = false;
                            if (!(q >= hi_mem_min)) {

                                if (mem[q].hh.u.B0 == 7 /*disc_node */ ) {
                                    {
                                        register integer for_end;
                                        m = 1;
                                        for_end = mem[q].hh.u.B1;
                                        if (m <= for_end)
                                            do
                                                p = mem[p].hh.v.RH;
                                            while (m++ < for_end);
                                    }
                                    if (p == tx)
                                        goto lab30;
                                } else if ((mem[q].hh.u.B0 == 9 /*math_node */ )
                                           && (mem[q].hh.u.B1 == 2 /*begin_M_code */ ))
                                    fm = true;
                            }
                            q = mem[p].hh.v.RH;
                        } while (!(q == tx));
                        q = mem[tx].hh.v.RH;
                        mem[p].hh.v.RH = q;
                        mem[tx].hh.v.RH = -268435455L;
                        if (q == -268435455L) {

                            if (fm)
                                confusion(66466L /*"tail1" */ );
                            else
                                cur_list.tail_field = p;
                        } else if (fm) {
                            cur_list.tail_field = r;
                            mem[r].hh.v.RH = -268435455L;
                            flush_node_list(p);
                        }
                        cur_box = tx;
                        mem[cur_box + 4].cint = 0;
                    }
                }
 lab30:                        /*done */ ;
            }
        }
        break;
    case 3:
        {
            scan_register_num();
            n = cur_val;
            if (!scan_keyword(66213L /*"to" */ )) {
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66470L /*"Missing `to' inserted" */ );
                }
                {
                    help_ptr = 2;
                    help_line[1] = 66471L /*"I'm working on `\vsplit<box number> to <dimen>';" */ ;
                    help_line[0] = 66472L /*"will look for the <dimen> next." */ ;
                }
                error();
            }
            scan_dimen(false, false, false);
            cur_box = vsplit(n, cur_val);
        }
        break;
    default:
        {
            k = cur_chr - 4;
            save_stack[save_ptr + 0].cint = box_context;
            if (k == 104 /*hmode */ ) {

                if ((box_context < 1073741824L) && (abs(cur_list.mode_field) == 1 /*vmode */ ))
                    scan_spec(3 /*adjusted_hbox_group */ , true);
                else
                    scan_spec(2 /*hbox_group */ , true);
            } else {

                if (k == 1 /*vmode */ )
                    scan_spec(4 /*vbox_group */ , true);
                else {

                    scan_spec(5 /*vtop_group */ , true);
                    k = 1 /*vmode */ ;
                }
                normal_paragraph();
            }
            push_nest();
            cur_list.mode_field = -(integer) k;
            if (k == 1 /*vmode */ ) {
                cur_list.aux_field.cint = -65536000L;
                if (eqtb[2252777L /*every_vbox_loc */ ].hh.v.RH != -268435455L)
                    begin_token_list(eqtb[2252777L /*every_vbox_loc */ ].hh.v.RH, 12 /*every_vbox_text */ );
            } else {

                cur_list.aux_field.hh.v.LH = 1000;
                if (eqtb[2252776L /*every_hbox_loc */ ].hh.v.RH != -268435455L)
                    begin_token_list(eqtb[2252776L /*every_hbox_loc */ ].hh.v.RH, 11 /*every_hbox_text */ );
            }
            return;
        }
        break;
    }
    box_end(box_context);
}

void zscan_box(integer box_context)
{
    scan_box_regmem
    do {
        get_x_token();
    } while (!((cur_cmd != 10 /*spacer */ ) && (cur_cmd != 0 /*relax */ ) /*:422 */ ));
    if (cur_cmd == 20 /*make_box */ )
        begin_box(box_context);
    else if ((box_context >= 1073807361L) && ((cur_cmd == 36 /*hrule */ ) || (cur_cmd == 35 /*vrule */ ))) {
        cur_box = scan_rule_spec();
        box_end(box_context);
    } else {

        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66473L /*"A <box> was supposed to be here" */ );
        }
        {
            help_ptr = 3;
            help_line[2] = 66474L /*"I was expecting to see \hbox or \vbox or \copy or \box or" */ ;
            help_line[1] = 66475L /*"something like that. So you might find something missing in" */ ;
            help_line[0] = 66476L /*"your output. But keep trying; you can fix this later." */ ;
        }
        back_error();
    }
}

void zpackage(small_number c)
{
    package_regmem scaled h;
    halfword p;
    scaled d;
    integer u, v;
    d = eqtb[10053199L /*dimen_base 7 */ ].cint;
    u = eqtb[8938813L /*eTeX_state_base 2 */ ].cint;
    unsave();
    save_ptr = save_ptr - 3;
    v = eqtb[8938813L /*eTeX_state_base 2 */ ].cint;
    eqtb[8938813L /*eTeX_state_base 2 */ ].cint = u;
    if (cur_list.mode_field == -104)
        cur_box = hpack(mem[cur_list.head_field].hh.v.RH, save_stack[save_ptr + 2].cint, save_stack[save_ptr + 1].cint);
    else {

        cur_box =
            vpackage(mem[cur_list.head_field].hh.v.RH, save_stack[save_ptr + 2].cint, save_stack[save_ptr + 1].cint, d);
        if (c == 4 /*vtop_code */ ) {   /*1122: */
            h = 0;
            p = mem[cur_box + 5].hh.v.RH;
            if (p != -268435455L) {

                if (mem[p].hh.u.B0 <= 2 /*rule_node */ )
                    h = mem[p + 3].cint;
            }
            mem[cur_box + 2].cint = mem[cur_box + 2].cint - h + mem[cur_box + 3].cint;
            mem[cur_box + 3].cint = h;
        }
    }
    eqtb[8938813L /*eTeX_state_base 2 */ ].cint = v;
    pop_nest();
    box_end(save_stack[save_ptr + 0].cint);
}

small_number znorm_min(integer h)
{
    register small_number Result;
    norm_min_regmem if (h <= 0)
        Result = 1;
    else if (h >= 63)
        Result = 63;
    else
        Result = h;
    return Result;
}

void znew_graf(boolean indented)
{
    new_graf_regmem cur_list.pg_field = 0;
    if ((cur_list.mode_field == 1 /*vmode */ ) || (cur_list.head_field != cur_list.tail_field)) {
        mem[cur_list.tail_field].hh.v.RH = new_param_glue(2 /*par_skip_code */ );
        cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
    }
    push_nest();
    cur_list.mode_field = 104 /*hmode */ ;
    cur_list.aux_field.hh.v.LH = 1000;
    if (eqtb[8938790L /*int_base 50 */ ].cint <= 0)
        cur_lang = 0;
    else if (eqtb[8938790L /*int_base 50 */ ].cint > 255 /*biggest_lang */ )
        cur_lang = 0;
    else
        cur_lang = eqtb[8938790L /*int_base 50 */ ].cint;
    cur_list.aux_field.hh.v.RH = cur_lang;
    cur_list.pg_field =
        (norm_min(eqtb[8938791L /*int_base 51 */ ].cint) * 64 +
         norm_min(eqtb[8938792L /*int_base 52 */ ].cint)) * 65536L + cur_lang;
    if (indented) {
        cur_list.tail_field = new_null_box();
        mem[cur_list.head_field].hh.v.RH = cur_list.tail_field;
        mem[cur_list.tail_field + 1].cint = eqtb[10053192L /*dimen_base 0 */ ].cint;
        if ((insert_src_special_every_par))
            insert_src_special();
    }
    if (eqtb[2252773L /*every_par_loc */ ].hh.v.RH != -268435455L)
        begin_token_list(eqtb[2252773L /*every_par_loc */ ].hh.v.RH, 8 /*every_par_text */ );
    if (nest_ptr == 1)
        build_page();
}

void indent_in_hmode(void)
{
    indent_in_hmode_regmem halfword p, q;
    if (cur_chr > 0) {
        p = new_null_box();
        mem[p + 1].cint = eqtb[10053192L /*dimen_base 0 */ ].cint;
        if (abs(cur_list.mode_field) == 104 /*hmode */ )
            cur_list.aux_field.hh.v.LH = 1000;
        else {

            q = new_noad();
            mem[q + 1].hh.v.RH = 2 /*sub_box */ ;
            mem[q + 1].hh.v.LH = p;
            p = q;
        }
        {
            mem[cur_list.tail_field].hh.v.RH = p;
            cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
        }
    }
}

void head_for_vmode(void)
{
    head_for_vmode_regmem if (cur_list.mode_field < 0) {

        if (cur_cmd != 36 /*hrule */ )
            off_save();
        else {

            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66020L /*"You can't use `" */ );
            }
            print_esc(65828L /*"hrule" */ );
            print(66479L /*"' here except with leaders" */ );
            {
                help_ptr = 2;
                help_line[1] = 66480L /*"To put a horizontal rule in an hbox or an alignment," */ ;
                help_line[0] = 66481L /*"you should use \leaders or \hrulefill (see The TeXbook)." */ ;
            }
            error();
        }
    } else {

        back_input();
        cur_tok = par_token;
        back_input();
        cur_input.index_field = 5 /*inserted */ ;
    }
}

void end_graf(void)
{
    end_graf_regmem if (cur_list.mode_field == 104 /*hmode */ ) {
        if (cur_list.head_field == cur_list.tail_field)
            pop_nest();
        else
            line_break(false);
        if (cur_list.eTeX_aux_field != -268435455L) {
            flush_list(cur_list.eTeX_aux_field);
            cur_list.eTeX_aux_field = -268435455L;
        }
        normal_paragraph();
        error_count = 0;
    }
}

void begin_insert_or_adjust(void)
{
    begin_insert_or_adjust_regmem if (cur_cmd == 38 /*vadjust */ )
        cur_val = 255;
    else {

        scan_eight_bit_int();
        if (cur_val == 255) {
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66482L /*"You can't " */ );
            }
            print_esc(65614L /*"insert" */ );
            print_int(255);
            {
                help_ptr = 1;
                help_line[0] = 66483L /*"I'm changing to \insert0; box 255 is special." */ ;
            }
            error();
            cur_val = 0;
        }
    }
    save_stack[save_ptr + 0].cint = cur_val;
    if ((cur_cmd == 38 /*vadjust */ ) && scan_keyword(66484L /*"pre" */ ))
        save_stack[save_ptr + 1].cint = 1;
    else
        save_stack[save_ptr + 1].cint = 0;
    save_ptr = save_ptr + 2;
    new_save_level(11 /*insert_group */ );
    scan_left_brace();
    normal_paragraph();
    push_nest();
    cur_list.mode_field = -1;
    cur_list.aux_field.cint = -65536000L;
}

void make_mark(void)
{
    make_mark_regmem halfword p;
    halfword c;
    if (cur_chr == 0)
        c = 0;
    else {

        scan_register_num();
        c = cur_val;
    }
    p = scan_toks(false, true);
    p = get_node(2 /*small_node_size */ );
    mem[p + 1].hh.v.LH = c;
    mem[p].hh.u.B0 = 4 /*mark_node */ ;
    mem[p].hh.u.B1 = 0;
    mem[p + 1].hh.v.RH = def_ref;
    mem[cur_list.tail_field].hh.v.RH = p;
    cur_list.tail_field = p;
}

void append_penalty(void)
{
    append_penalty_regmem scan_int();
    {
        mem[cur_list.tail_field].hh.v.RH = new_penalty(cur_val);
        cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
    }
    if (cur_list.mode_field == 1 /*vmode */ )
        build_page();
}

void delete_last(void)
{
    delete_last_regmem halfword p, q;
    halfword r;
    boolean fm;
    halfword tx;
    quarterword m;
    if ((cur_list.mode_field == 1 /*vmode */ ) && (cur_list.tail_field == cur_list.head_field)) {       /*1141: */
        if ((cur_chr != 10 /*glue_node */ ) || (last_glue != 1073741823L)) {
            you_cant();
            {
                help_ptr = 2;
                help_line[1] = 66468L /*"Sorry...I usually can't take things from the current page." */ ;
                help_line[0] = 66485L /*"Try `I\vskip-\lastskip' instead." */ ;
            }
            if (cur_chr == 11 /*kern_node */ )
                help_line[0] = (66486L /*"Try `I\kern-\lastkern' instead." */ );
            else if (cur_chr != 10 /*glue_node */ )
                help_line[0] = (66487L /*"Perhaps you can make the output routine do it." */ );
            error();
        }
    } else {

        tx = cur_list.tail_field;
        if (!(tx >= hi_mem_min)) {

            if ((mem[tx].hh.u.B0 == 9 /*math_node */ ) && (mem[tx].hh.u.B1 == 3 /*end_M_code */ )) {
                r = cur_list.head_field;
                do {
                    q = r;
                    r = mem[q].hh.v.RH;
                } while (!(r == tx));
                tx = q;
            }
        }
        if (!(tx >= hi_mem_min)) {

            if (mem[tx].hh.u.B0 == cur_chr) {
                q = cur_list.head_field;
                p = -268435455L;
                do {
                    r = p;
                    p = q;
                    fm = false;
                    if (!(q >= hi_mem_min)) {

                        if (mem[q].hh.u.B0 == 7 /*disc_node */ ) {
                            {
                                register integer for_end;
                                m = 1;
                                for_end = mem[q].hh.u.B1;
                                if (m <= for_end)
                                    do
                                        p = mem[p].hh.v.RH;
                                    while (m++ < for_end);
                            }
                            if (p == tx)
                                return;
                        } else if ((mem[q].hh.u.B0 == 9 /*math_node */ ) && (mem[q].hh.u.B1 == 2 /*begin_M_code */ ))
                            fm = true;
                    }
                    q = mem[p].hh.v.RH;
                } while (!(q == tx));
                q = mem[tx].hh.v.RH;
                mem[p].hh.v.RH = q;
                mem[tx].hh.v.RH = -268435455L;
                if (q == -268435455L) {

                    if (fm)
                        confusion(66466L /*"tail1" */ );
                    else
                        cur_list.tail_field = p;
                } else if (fm) {
                    cur_list.tail_field = r;
                    mem[r].hh.v.RH = -268435455L;
                    flush_node_list(p);
                }
                flush_node_list(tx);
            }
        }
    }
}

void unpackage(void)
{
    unpackage_regmem halfword p;
    halfword r;
    unsigned char /*copy_code */ c;
    if (cur_chr > 1 /*copy_code */ ) {  /*1651: */
        mem[cur_list.tail_field].hh.v.RH = disc_ptr[cur_chr];
        disc_ptr[cur_chr] = -268435455L;
        goto lab30;
    }
    c = cur_chr;
    scan_register_num();
    if (cur_val < 256)
        p = eqtb[2253043L /*box_base */  + cur_val].hh.v.RH;
    else {

        find_sa_element(4, cur_val, false);
        if (cur_ptr == -268435455L)
            p = -268435455L;
        else
            p = mem[cur_ptr + 1].hh.v.RH;
    }
    if (p == -268435455L)
        return;
    if ((abs(cur_list.mode_field) == 207 /*mmode */ )
        || ((abs(cur_list.mode_field) == 1 /*vmode */ ) && (mem[p].hh.u.B0 != 1 /*vlist_node */ ))
        || ((abs(cur_list.mode_field) == 104 /*hmode */ ) && (mem[p].hh.u.B0 != 0 /*hlist_node */ ))) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66495L /*"Incompatible list can't be unboxed" */ );
        }
        {
            help_ptr = 3;
            help_line[2] = 66496L /*"Sorry, Pandora. (You sneaky devil.)" */ ;
            help_line[1] = 66497L /*"I refuse to unbox an \hbox in vertical mode or vice versa." */ ;
            help_line[0] = 66498L /*"And I can't open any boxes in math mode." */ ;
        }
        error();
        return;
    }
    if (c == 1 /*copy_code */ )
        mem[cur_list.tail_field].hh.v.RH = copy_node_list(mem[p + 5].hh.v.RH);
    else {

        mem[cur_list.tail_field].hh.v.RH = mem[p + 5].hh.v.RH;
        if (cur_val < 256)
            eqtb[2253043L /*box_base */  + cur_val].hh.v.RH = -268435455L;
        else {

            find_sa_element(4, cur_val, false);
            if (cur_ptr != -268435455L) {
                mem[cur_ptr + 1].hh.v.RH = -268435455L;
                mem[cur_ptr + 1].hh.v.LH++;
                delete_sa_ref(cur_ptr);
            }
        }
        free_node(p, 8 /*box_node_size */ );
    }
 lab30:                        /*done */ while (mem[cur_list.tail_field].hh.v.RH != -268435455L) {

        r = mem[cur_list.tail_field].hh.v.RH;
        if (!(r >= hi_mem_min) && (mem[r].hh.u.B0 == 40 /*margin_kern_node */ )) {
            mem[cur_list.tail_field].hh.v.RH = mem[r].hh.v.RH;
            free_node(r, 3 /*margin_kern_node_size */ );
        }
        cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
    }
}

void append_italic_correction(void)
{
    append_italic_correction_regmem halfword p;
    internal_font_number f;
    if (cur_list.tail_field != cur_list.head_field) {
        if ((cur_list.tail_field >= hi_mem_min))
            p = cur_list.tail_field;
        else if (mem[cur_list.tail_field].hh.u.B0 == 6 /*ligature_node */ )
            p = cur_list.tail_field + 1;
        else if ((mem[cur_list.tail_field].hh.u.B0 == 8 /*whatsit_node */ )) {
            if ((mem[cur_list.tail_field].hh.u.B1 == 40 /*native_word_node */ )
                || (mem[cur_list.tail_field].hh.u.B1 == 41 /*native_word_node_AT */ )) {
                {
                    mem[cur_list.tail_field].hh.v.RH = new_kern(get_native_italic_correction(cur_list.tail_field));
                    cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
                }
                mem[cur_list.tail_field].hh.u.B1 = 1 /*explicit */ ;
            } else if ((mem[cur_list.tail_field].hh.u.B1 == 42 /*glyph_node */ )) {
                {
                    mem[cur_list.tail_field].hh.v.RH =
                        new_kern(get_native_glyph_italic_correction(cur_list.tail_field));
                    cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
                }
                mem[cur_list.tail_field].hh.u.B1 = 1 /*explicit */ ;
            }
            return;
        } else
            return;
        f = mem[p].hh.u.B0;
        {
            mem[cur_list.tail_field].hh.v.RH =
                new_kern(font_info
                         [italic_base[f] +
                          (font_info[char_base[f] + effective_char(true, f, mem[p].hh.u.B1)].qqqq.u.B2) / 4].cint);
            cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
        }
        mem[cur_list.tail_field].hh.u.B1 = 1 /*explicit */ ;
    }
}

void append_discretionary(void)
{
    append_discretionary_regmem integer c;
    {
        mem[cur_list.tail_field].hh.v.RH = new_disc();
        cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
    }
    if (cur_chr == 1) {
        c = hyphen_char[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH];
        if (c >= 0) {

            if (c <= 65535L /*biggest_char */ )
                mem[cur_list.tail_field + 1].hh.v.LH = new_character(eqtb[2253299L /*cur_font_loc */ ].hh.v.RH, c);
        }
    } else {

        save_ptr++;
        save_stack[save_ptr - 1].cint = 0;
        new_save_level(10 /*disc_group */ );
        scan_left_brace();
        push_nest();
        cur_list.mode_field = -104;
        cur_list.aux_field.hh.v.LH = 1000;
    }
}

void build_discretionary(void)
{
    build_discretionary_regmem halfword p, q;
    integer n;
    unsave();
    q = cur_list.head_field;
    p = mem[q].hh.v.RH;
    n = 0;
    while (p != -268435455L) {

        if (!(p >= hi_mem_min)) {

            if (mem[p].hh.u.B0 > 2 /*rule_node */ ) {

                if (mem[p].hh.u.B0 != 11 /*kern_node */ ) {

                    if (mem[p].hh.u.B0 != 6 /*ligature_node */ ) {

                        if ((mem[p].hh.u.B0 != 8 /*whatsit_node */ )
                            || ((mem[p].hh.u.B1 != 40 /*native_word_node */ )
                                && (mem[p].hh.u.B1 != 41 /*native_word_node_AT */ )
                                && (mem[p].hh.u.B1 != 42 /*glyph_node */ ))) {
                            {
                                if (interaction == 3 /*error_stop_mode */ ) ;
                                if (file_line_error_style_p)
                                    print_file_line();
                                else
                                    print_nl(65544L /*"! " */ );
                                print(66505L /*"Improper discretionary list" */ );
                            }
                            {
                                help_ptr = 1;
                                help_line[0] = 66506L /*"Discretionary lists must contain only boxes and kerns." */ ;
                            }
                            error();
                            begin_diagnostic();
                            print_nl(66507L /*"The following discretionary sublist has been deleted:" */ );
                            show_box(p);
                            end_diagnostic(true);
                            flush_node_list(p);
                            mem[q].hh.v.RH = -268435455L;
                            goto lab30;
                        }
                    }
                }
            }
        }
        q = p;
        p = mem[q].hh.v.RH;
        n++;
    }
 lab30:                        /*done *//*:1156 */ ;
    p = mem[cur_list.head_field].hh.v.RH;
    pop_nest();
    switch (save_stack[save_ptr - 1].cint) {
    case 0:
        mem[cur_list.tail_field + 1].hh.v.LH = p;
        break;
    case 1:
        mem[cur_list.tail_field + 1].hh.v.RH = p;
        break;
    case 2:
        {
            if ((n > 0) && (abs(cur_list.mode_field) == 207 /*mmode */ )) {
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66499L /*"Illegal math " */ );
                }
                print_esc(65635L /*"discretionary" */ );
                {
                    help_ptr = 2;
                    help_line[1] = 66500L /*"Sorry: The third part of a discretionary break must be" */ ;
                    help_line[0] = 66501L /*"empty, in math formulas. I had to delete your third part." */ ;
                }
                flush_node_list(p);
                n = 0;
                error();
            } else
                mem[cur_list.tail_field].hh.v.RH = p;
            if (n <= 65535L /*max_quarterword */ )
                mem[cur_list.tail_field].hh.u.B1 = n;
            else {

                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66502L /*"Discretionary list is too long" */ );
                }
                {
                    help_ptr = 2;
                    help_line[1] = 66503L /*"Wow---I never thought anybody would tweak me here." */ ;
                    help_line[0] = 66504L /*"You can't seriously need such a huge discretionary list?" */ ;
                }
                error();
            }
            if (n > 0)
                cur_list.tail_field = q;
            save_ptr--;
            return;
        }
        break;
    }
    save_stack[save_ptr - 1].cint++;
    new_save_level(10 /*disc_group */ );
    scan_left_brace();
    push_nest();
    cur_list.mode_field = -104;
    cur_list.aux_field.hh.v.LH = 1000;
}

void make_accent(void)
{
    make_accent_regmem double s, t;
    halfword p, q, r;
    internal_font_number f;
    scaled a, h, x, w, delta, lsb, rsb;
    four_quarters i;
    scan_char_num();
    f = eqtb[2253299L /*cur_font_loc */ ].hh.v.RH;
    p = new_character(f, cur_val);
    if (p != -268435455L) {
        x = font_info[5 /*x_height_code */  + param_base[f]].cint;
        s = font_info[1 /*slant_code */  + param_base[f]].cint / ((double)65536.0);
        if (((font_area[f] == 65535L /*aat_font_flag */ ) || (font_area[f] == 65534L /*otgr_font_flag */ ))) {
            a = mem[p + 1].cint;
            if (a == 0)
                get_native_char_sidebearings(f, cur_val, &lsb, &rsb);
        } else
            a = font_info[width_base[f] + font_info[char_base[f] + effective_char(true, f, mem[p].hh.u.B1)].qqqq.u.B0].cint;
        do_assignments();
        q = -268435455L;
        f = eqtb[2253299L /*cur_font_loc */ ].hh.v.RH;
        if ((cur_cmd == 11 /*letter */ ) || (cur_cmd == 12 /*other_char */ ) || (cur_cmd == 68 /*char_given */ )) {
            q = new_character(f, cur_chr);
            cur_val = cur_chr;
        } else if (cur_cmd == 16 /*char_num */ ) {
            scan_char_num();
            q = new_character(f, cur_val);
        } else
            back_input();
        if (q != -268435455L) { /*1160: */
            t = font_info[1 /*slant_code */  + param_base[f]].cint / ((double)65536.0);
            if (((font_area[f] == 65535L /*aat_font_flag */ ) || (font_area[f] == 65534L /*otgr_font_flag */ ))) {
                w = mem[q + 1].cint;
                get_native_char_height_depth(f, cur_val, &h, &delta);
            } else {

                i = font_info[char_base[f] + effective_char(true, f, mem[q].hh.u.B1)].qqqq;
                w = font_info[width_base[f] + i.u.B0].cint;
                h = font_info[height_base[f] + (i.u.B1) / 16].cint;
            }
            if (h != x) {
                p = hpack(p, 0, 1 /*additional */ );
                mem[p + 4].cint = x - h;
            }
            if (((font_area[f] == 65535L /*aat_font_flag */ ) || (font_area[f] == 65534L /*otgr_font_flag */ ))
                && (a == 0))
                delta = round((w - lsb + rsb) / ((double)2.0) + h * t - x * s);
            else
                delta = round((w - a) / ((double)2.0) + h * t - x * s);
            r = new_kern(delta);
            mem[r].hh.u.B1 = 2 /*acc_kern */ ;
            mem[cur_list.tail_field].hh.v.RH = r;
            mem[r].hh.v.RH = p;
            cur_list.tail_field = new_kern(-(integer) a - delta);
            mem[cur_list.tail_field].hh.u.B1 = 2 /*acc_kern */ ;
            mem[p].hh.v.RH = cur_list.tail_field;
            p = q;
        }
        mem[cur_list.tail_field].hh.v.RH = p;
        cur_list.tail_field = p;
        cur_list.aux_field.hh.v.LH = 1000;
    }
}

void align_error(void)
{
    align_error_regmem if (abs(align_state) > 2) {      /*1163: */
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66512L /*"Misplaced " */ );
        }
        print_cmd_chr(cur_cmd, cur_chr);
        if (cur_tok == 8388646L /*tab_token 38 */ ) {
            {
                help_ptr = 6;
                help_line[5] = 66513L /*"I can't figure out why you would want to use a tab mark" */ ;
                help_line[4] = 66514L /*"here. If you just want an ampersand, the remedy is" */ ;
                help_line[3] = 66515L /*"simple: Just type `I\&' now. But if some right brace" */ ;
                help_line[2] = 66516L /*"up above has ended a previous alignment prematurely," */ ;
                help_line[1] = 66517L /*"you're probably due for more error messages, and you" */ ;
                help_line[0] = 66518L /*"might try typing `S' now just to see what is salvageable." */ ;
            }
        } else {

            {
                help_ptr = 5;
                help_line[4] = 66513L /*"I can't figure out why you would want to use a tab mark" */ ;
                help_line[3] = 66519L /*"or \cr or \span just now. If something like a right brace" */ ;
                help_line[2] = 66516L /*"up above has ended a previous alignment prematurely," */ ;
                help_line[1] = 66517L /*"you're probably due for more error messages, and you" */ ;
                help_line[0] = 66518L /*"might try typing `S' now just to see what is salvageable." */ ;
            }
        }
        error();
    } else {

        back_input();
        if (align_state < 0) {
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(65980L /*"Missing _ inserted" */ );
            }
            align_state++;
            cur_tok = 2097275L /*left_brace_token 123 */ ;
        } else {

            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66508L /*"Missing _ inserted" */ );
            }
            align_state--;
            cur_tok = 4194429L /*right_brace_token 125 */ ;
        }
        {
            help_ptr = 3;
            help_line[2] = 66509L /*"I've put in what seems to be necessary to fix" */ ;
            help_line[1] = 66510L /*"the current column of the current alignment." */ ;
            help_line[0] = 66511L /*"Try to go on, since this might almost work." */ ;
        }
        ins_error();
    }
}

void no_align_error(void)
{
    no_align_error_regmem {
        if (interaction == 3 /*error_stop_mode */ ) ;
        if (file_line_error_style_p)
            print_file_line();
        else
            print_nl(65544L /*"! " */ );
        print(66512L /*"Misplaced " */ );
    }
    print_esc(65840L /*"noalign" */ );
    {
        help_ptr = 2;
        help_line[1] = 66520L /*"I expect to see \noalign only after the \cr of" */ ;
        help_line[0] = 66521L /*"an alignment. Proceed, and I'll ignore this case." */ ;
    }
    error();
}

void omit_error(void)
{
    omit_error_regmem {
        if (interaction == 3 /*error_stop_mode */ ) ;
        if (file_line_error_style_p)
            print_file_line();
        else
            print_nl(65544L /*"! " */ );
        print(66512L /*"Misplaced " */ );
    }
    print_esc(65843L /*"omit" */ );
    {
        help_ptr = 2;
        help_line[1] = 66522L /*"I expect to see \omit only after tab marks or the \cr of" */ ;
        help_line[0] = 66521L /*"an alignment. Proceed, and I'll ignore this case." */ ;
    }
    error();
}

void do_endv(void)
{
    do_endv_regmem base_ptr = input_ptr;
    input_stack[base_ptr] = cur_input;
    while ((input_stack[base_ptr].index_field != 2 /*v_template */ ) && (input_stack[base_ptr].loc_field == -268435455L)
           && (input_stack[base_ptr].state_field == 0 /*token_list */ ))
        base_ptr--;
    if ((input_stack[base_ptr].index_field != 2 /*v_template */ ) || (input_stack[base_ptr].loc_field != -268435455L)
        || (input_stack[base_ptr].state_field != 0 /*token_list */ ))
        fatal_error(65915L /*"(interwoven alignment preambles are not allowed)" */ );
    if (cur_group == 6 /*align_group */ ) {
        end_graf();
        if (fin_col())
            fin_row();
    } else
        off_save();
}

void cs_error(void)
{
    cs_error_regmem {
        if (interaction == 3 /*error_stop_mode */ ) ;
        if (file_line_error_style_p)
            print_file_line();
        else
            print_nl(65544L /*"! " */ );
        print(66134L /*"Extra " */ );
    }
    print_esc(65810L /*"endcsname" */ );
    {
        help_ptr = 1;
        help_line[0] = 66524L /*"I'm ignoring this, since I wasn't doing a \csname." */ ;
    }
    error();
}

void zpush_math(group_code c)
{
    push_math_regmem push_nest();
    cur_list.mode_field = -207;
    cur_list.aux_field.cint = -268435455L;
    new_save_level(c);
}

void zjust_copy(halfword p, halfword h, halfword t)
{
    just_copy_regmem halfword r;
    unsigned char words;
    while (p != -268435455L) {

        words = 1;
        if ((p >= hi_mem_min))
            r = get_avail();
        else
            switch (mem[p].hh.u.B0) {
            case 0:
            case 1:
                {
                    r = get_node(8 /*box_node_size */ );
                    mem[r + 7].hh.v.LH = mem[p + 7].hh.v.LH;
                    mem[r + 7].hh.v.RH = mem[p + 7].hh.v.RH;
                    mem[r + 6] = mem[p + 6];
                    mem[r + 5] = mem[p + 5];
                    words = 5;
                    mem[r + 5].hh.v.RH = -268435455L;
                }
                break;
            case 2:
                {
                    r = get_node(5 /*rule_node_size */ );
                    words = 5 /*rule_node_size */ ;
                }
                break;
            case 6:
                {
                    r = get_avail();
                    mem[r] = mem[p + 1];
                    goto lab40;
                }
                break;
            case 11:
            case 9:
                {
                    words = 3 /*medium_node_size */ ;
                    r = get_node(words);
                }
                break;
            case 10:
                {
                    r = get_node(3 /*medium_node_size */ );
                    mem[mem[p + 1].hh.v.LH].hh.v.RH++;
                    mem[r + 2].hh.v.LH = mem[p + 2].hh.v.LH;
                    mem[r + 2].hh.v.RH = mem[p + 2].hh.v.RH;
                    mem[r + 1].hh.v.LH = mem[p + 1].hh.v.LH;
                    mem[r + 1].hh.v.RH = -268435455L;
                }
                break;
            case 8:
                switch (mem[p].hh.u.B1) {
                case 0:
                    {
                        r = get_node(3 /*open_node_size */ );
                        words = 3 /*open_node_size */ ;
                    }
                    break;
                case 1:
                case 3:
                    {
                        r = get_node(2 /*write_node_size */ );
                        mem[mem[p + 1].hh.v.RH].hh.v.LH++;
                        words = 2 /*write_node_size */ ;
                    }
                    break;
                case 2:
                case 4:
                    {
                        r = get_node(2 /*small_node_size */ );
                        words = 2 /*small_node_size */ ;
                    }
                    break;
                case 40:
                case 41:
                    {
                        words = mem[p + 4].qqqq.u.B0;
                        r = get_node(words);
                        while (words > 0) {

                            words--;
                            mem[r + words] = mem[p + words];
                        }
                        mem[r + 5].ptr = NULL;
                        mem[r + 4].qqqq.u.B3 = 0;
                        copy_native_glyph_info(p, r);
                    }
                    break;
                case 42:
                    {
                        r = get_node(5 /*glyph_node_size */ );
                        words = 5 /*glyph_node_size */ ;
                    }
                    break;
                case 43:
                case 44:
                    {
                        words =
                            (9 /*pic_node_size */  +
                             (mem[p + 4].hh.u.B0 + sizeof(memory_word) - 1) / sizeof(memory_word));
                        r = get_node(words);
                    }
                    break;
                case 6:
                    r = get_node(2 /*small_node_size */ );
                    break;
                default:
                    confusion(66732L /*"ext2" */ );
                    break;
                }
                break;
            default:
                goto lab45;
                break;
            }
        while (words > 0) {

            words--;
            mem[r + words] = mem[p + words];
        }
 lab40:                        /*found */ mem[h].hh.v.RH = r;
        h = r;
 lab45:                        /*not_found */ p = mem[p].hh.v.RH;
    }
    mem[h].hh.v.RH = t;
}

void zjust_reverse(halfword p)
{
    just_reverse_regmem halfword l;
    halfword t;
    halfword q;
    halfword m, n;
    m = -268435455L;
    n = -268435455L;
    if (mem[mem_top - 3].hh.v.RH == -268435455L) {
        just_copy(mem[p].hh.v.RH, mem_top - 3, -268435455L);
        q = mem[mem_top - 3].hh.v.RH;
    } else {

        q = mem[p].hh.v.RH;
        mem[p].hh.v.RH = -268435455L;
        flush_node_list(mem[mem_top - 3].hh.v.RH);
    }
    t = new_edge(cur_dir, 0);
    l = t;
    cur_dir = 1 - cur_dir;
    while (q != -268435455L)
        if ((q >= hi_mem_min))
            do {
                p = q;
                q = mem[p].hh.v.RH;
                mem[p].hh.v.RH = l;
                l = p;
            } while (!(!(q >= hi_mem_min)));
        else {

            p = q;
            q = mem[p].hh.v.RH;
            if (mem[p].hh.u.B0 == 9 /*math_node */ ) {    /*1527: */

                if (odd(mem[p].hh.u.B1)) {

                    if (mem[LR_ptr].hh.v.LH != (4 /*L_code */  * (mem[p].hh.u.B1 / 4 /*L_code */ ) + 3)) {
                        mem[p].hh.u.B0 = 11 /*kern_node */ ;
                        LR_problems++;
                    } else {

                        {
                            temp_ptr = LR_ptr;
                            LR_ptr = mem[temp_ptr].hh.v.RH;
                            {
                                mem[temp_ptr].hh.v.RH = avail;
                                avail = temp_ptr;
                            }
                        }
                        if (n > -268435455L) {
                            n--;
                            mem[p].hh.u.B1--;
                        } else {

                            if (m > -268435455L)
                                m--;
                            else {

                                mem[t + 1].cint = mem[p + 1].cint;
                                mem[t].hh.v.RH = q;
                                free_node(p, 3 /*medium_node_size */ );
                                goto lab30;
                            }
                            mem[p].hh.u.B0 = 11 /*kern_node */ ;
                        }
                    }
                } else {

                    {
                        temp_ptr = get_avail();
                        mem[temp_ptr].hh.v.LH = (4 /*L_code */  * (mem[p].hh.u.B1 / 4 /*L_code */ ) + 3);
                        mem[temp_ptr].hh.v.RH = LR_ptr;
                        LR_ptr = temp_ptr;
                    }
                    if ((n > -268435455L) || ((mem[p].hh.u.B1 / 8 /*R_code */ ) != cur_dir)) {
                        n++;
                        mem[p].hh.u.B1++;
                    } else {

                        mem[p].hh.u.B0 = 11 /*kern_node */ ;
                        m++;
                    }
                }
            }
            mem[p].hh.v.RH = l;
            l = p;
        }
    goto lab30;
    mem[t + 1].cint = mem[p + 1].cint;
    mem[t].hh.v.RH = q;
    free_node(p, 2 /*small_node_size */ );
 lab30:                        /*done */ mem[mem_top - 3].hh.v.RH = l;
}

void init_math(void)
{
    init_math_regmem scaled w;
    halfword j;
    integer x;
    scaled l;
    scaled s;
    halfword p;
    halfword q;
    internal_font_number f;
    integer n;
    scaled v;
    scaled d;
    get_token();
    if ((cur_cmd == 3 /*math_shift */ ) && (cur_list.mode_field > 0)) { /*1180: */
        j = -268435455L;
        w = -1073741823L;
        if (cur_list.head_field == cur_list.tail_field) {       /*1520: */
            pop_nest();
            if (cur_list.eTeX_aux_field == -268435455L)
                x = 0;
            else if (mem[cur_list.eTeX_aux_field].hh.v.LH >= 8 /*R_code */ )
                x = -1;
            else
                x = 1 /*:1519 */ ;
        } else {

            line_break(true);
            if ((eTeX_mode == 1)) {     /*1528: */
                if (eqtb[2252248L /*glue_base 8 */ ].hh.v.RH == mem_bot)
                    j = new_kern(0);
                else
                    j = new_param_glue(8 /*right_skip_code */ );
                if (eqtb[2252247L /*glue_base 7 */ ].hh.v.RH == mem_bot)
                    p = new_kern(0);
                else
                    p = new_param_glue(7 /*left_skip_code */ );
                mem[p].hh.v.RH = j;
                j = new_null_box();
                mem[j + 1].cint = mem[just_box + 1].cint;
                mem[j + 4].cint = mem[just_box + 4].cint;
                mem[j + 5].hh.v.RH = p;
                mem[j + 5].hh.u.B1 = mem[just_box + 5].hh.u.B1;
                mem[j + 5].hh.u.B0 = mem[just_box + 5].hh.u.B0;
                mem[j + 6].gr = mem[just_box + 6].gr;
            }
            v = mem[just_box + 4].cint;
            if (cur_list.eTeX_aux_field == -268435455L)
                x = 0;
            else if (mem[cur_list.eTeX_aux_field].hh.v.LH >= 8 /*R_code */ )
                x = -1;
            else
                x = 1 /*:1519 */ ;
            if (x >= 0) {
                p = mem[just_box + 5].hh.v.RH;
                mem[mem_top - 3].hh.v.RH = -268435455L;
            } else {

                v = -(integer) v - mem[just_box + 1].cint;
                p = new_math(0, 6 /*begin_L_code */ );
                mem[mem_top - 3].hh.v.RH = p;
                just_copy(mem[just_box + 5].hh.v.RH, p, new_math(0, 7 /*end_L_code */ ));
                cur_dir = 1 /*right_to_left */ ;
            }
            v = v + 2 * font_info[6 /*quad_code */  + param_base[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH]].cint;
            if ((eqtb[8938811L /*eTeX_state_base 0 */ ].cint > 0)) {    /*1497: */
                temp_ptr = get_avail();
                mem[temp_ptr].hh.v.LH = 0 /*before */ ;
                mem[temp_ptr].hh.v.RH = LR_ptr;
                LR_ptr = temp_ptr;
            }
            while (p != -268435455L) {

 lab21:                        /*reswitch */ if ((p >= hi_mem_min)) {
                    f = mem[p].hh.u.B0;
                    d = font_info[width_base[f] +
                                  font_info[char_base[f] + effective_char(true, f, mem[p].hh.u.B1)].qqqq.u.B0].cint;
                    goto lab40;
                }
                switch (mem[p].hh.u.B0) {
                case 0:
                case 1:
                case 2:
                    {
                        d = mem[p + 1].cint;
                        goto lab40;
                    }
                    break;
                case 6:
                    {
                        mem[mem_top - 12] = mem[p + 1];
                        mem[mem_top - 12].hh.v.RH = mem[p].hh.v.RH;
                        p = mem_top - 12;
                        xtx_ligature_present = true;
                        goto lab21;
                    }
                    break;
                case 11:
                    d = mem[p + 1].cint;
                    break;
                case 40:
                    d = mem[p + 1].cint;
                    break;
                case 9:
                    {
                        d = mem[p + 1].cint;
                        if ((eqtb[8938811L /*eTeX_state_base 0 */ ].cint > 0)) {        /*1525: */

                            if (odd(mem[p].hh.u.B1)) {
                                if (mem[LR_ptr].hh.v.LH == (4 /*L_code */  * (mem[p].hh.u.B1 / 4 /*L_code */ ) + 3)) {
                                    temp_ptr = LR_ptr;
                                    LR_ptr = mem[temp_ptr].hh.v.RH;
                                    {
                                        mem[temp_ptr].hh.v.RH = avail;
                                        avail = temp_ptr;
                                    }
                                } else if (mem[p].hh.u.B1 > 4 /*L_code */ ) {
                                    w = 1073741823L;
                                    goto lab30;
                                }
                            } else {

                                {
                                    temp_ptr = get_avail();
                                    mem[temp_ptr].hh.v.LH = (4 /*L_code */  * (mem[p].hh.u.B1 / 4 /*L_code */ ) + 3);
                                    mem[temp_ptr].hh.v.RH = LR_ptr;
                                    LR_ptr = temp_ptr;
                                }
                                if ((mem[p].hh.u.B1 / 8 /*R_code */ ) != cur_dir) {
                                    just_reverse(p);
                                    p = mem_top - 3;
                                }
                            }
                        } else if (mem[p].hh.u.B1 >= 4 /*L_code */ ) {
                            w = 1073741823L;
                            goto lab30;
                        }
                    }
                    break;
                case 14:
                    {
                        d = mem[p + 1].cint;
                        cur_dir = mem[p].hh.u.B1;
                    }
                    break;
                case 10:
                    {
                        q = mem[p + 1].hh.v.LH;
                        d = mem[q + 1].cint;
                        if (mem[just_box + 5].hh.u.B0 == 1 /*stretching */ ) {
                            if ((mem[just_box + 5].hh.u.B1 == mem[q].hh.u.B0) && (mem[q + 2].cint != 0))
                                v = 1073741823L;
                        } else if (mem[just_box + 5].hh.u.B0 == 2 /*shrinking */ ) {
                            if ((mem[just_box + 5].hh.u.B1 == mem[q].hh.u.B1) && (mem[q + 3].cint != 0))
                                v = 1073741823L;
                        }
                        if (mem[p].hh.u.B1 >= 100 /*a_leaders */ )
                            goto lab40;
                    }
                    break;
                case 8:
                    if ((mem[p].hh.u.B1 == 40 /*native_word_node */ ) || (mem[p].hh.u.B1 == 41 /*native_word_node_AT */ )
                        || (mem[p].hh.u.B1 == 42 /*glyph_node */ ) || (mem[p].hh.u.B1 == 43 /*pic_node */ )
                        || (mem[p].hh.u.B1 == 44 /*pdf_node */ )) {
                        d = mem[p + 1].cint;
                        goto lab40;
                    } else
                        d = 0 /*:1398 */ ;
                    break;
                default:
                    d = 0;
                    break;
                }
                if (v < 1073741823L)
                    v = v + d;
                goto lab45;
 lab40:                        /*found */ if (v < 1073741823L) {
                    v = v + d;
                    w = v;
                } else {

                    w = 1073741823L;
                    goto lab30;
                }
 lab45:                        /*not_found */ p = mem[p].hh.v.RH;
            }
 lab30:    /*done *//*1523: */ if ((eqtb[8938811L /*eTeX_state_base 0 */ ].cint > 0)) {
                while (LR_ptr != -268435455L) {

                    temp_ptr = LR_ptr;
                    LR_ptr = mem[temp_ptr].hh.v.RH;
                    {
                        mem[temp_ptr].hh.v.RH = avail;
                        avail = temp_ptr;
                    }
                }
                if (LR_problems != 0) {
                    w = 1073741823L;
                    LR_problems = 0;
                }
            }
            cur_dir = 0 /*left_to_right */ ;
            flush_node_list(mem[mem_top - 3].hh.v.RH);
        }
        if (eqtb[2252771L /*par_shape_loc */ ].hh.v.RH == -268435455L) {

            if ((eqtb[10053209L /*dimen_base 17 */ ].cint != 0)
                &&
                (((eqtb[8938781L /*int_base 41 */ ].cint >= 0)
                  && (cur_list.pg_field + 2 > eqtb[8938781L /*int_base 41 */ ].cint))
                 || (cur_list.pg_field + 1 < -(integer) eqtb[8938781L /*int_base 41 */ ].cint))) {
                l = eqtb[10053195L /*dimen_base 3 */ ].cint - abs(eqtb[10053209L /*dimen_base 17 */ ].cint);
                if (eqtb[10053209L /*dimen_base 17 */ ].cint > 0)
                    s = eqtb[10053209L /*dimen_base 17 */ ].cint;
                else
                    s = 0;
            } else {

                l = eqtb[10053195L /*dimen_base 3 */ ].cint;
                s = 0;
            }
        } else {

            n = mem[eqtb[2252771L /*par_shape_loc */ ].hh.v.RH].hh.v.LH;
            if (cur_list.pg_field + 2 >= n)
                p = eqtb[2252771L /*par_shape_loc */ ].hh.v.RH + 2 * n;
            else
                p = eqtb[2252771L /*par_shape_loc */ ].hh.v.RH + 2 * (cur_list.pg_field + 2);
            s = mem[p - 1].cint;
            l = mem[p].cint;
        }
        push_math(15 /*math_shift_group */ );
        cur_list.mode_field = 207 /*mmode */ ;
        eq_word_define(8938784L /*int_base 44 */ , -1);
        eq_word_define(10053205L /*dimen_base 13 */ , w);
        cur_list.eTeX_aux_field = j;
        if ((eTeX_mode == 1))
            eq_word_define(8938803L /*int_base 63 */ , x);
        eq_word_define(10053206L /*dimen_base 14 */ , l);
        eq_word_define(10053207L /*dimen_base 15 */ , s);
        if (eqtb[2252775L /*every_display_loc */ ].hh.v.RH != -268435455L)
            begin_token_list(eqtb[2252775L /*every_display_loc */ ].hh.v.RH, 10 /*every_display_text */ );
        if (nest_ptr == 1)
            build_page();
    } else {

        back_input();
        {
            push_math(15 /*math_shift_group */ );
            eq_word_define(8938784L /*int_base 44 */ , -1);
            if ((insert_src_special_every_math))
                insert_src_special();
            if (eqtb[2252774L /*every_math_loc */ ].hh.v.RH != -268435455L)
                begin_token_list(eqtb[2252774L /*every_math_loc */ ].hh.v.RH, 9 /*every_math_text */ );
        }
    }
}

void start_eq_no(void)
{
    start_eq_no_regmem save_stack[save_ptr + 0].cint = cur_chr;
    save_ptr++;
    {
        push_math(15 /*math_shift_group */ );
        eq_word_define(8938784L /*int_base 44 */ , -1);
        if ((insert_src_special_every_math))
            insert_src_special();
        if (eqtb[2252774L /*every_math_loc */ ].hh.v.RH != -268435455L)
            begin_token_list(eqtb[2252774L /*every_math_loc */ ].hh.v.RH, 9 /*every_math_text */ );
    }
}

void zscan_math(halfword p)
{
    scan_math_regmem integer c;
 lab20:                        /*restart *//*422: */
    do {
        get_x_token();
    } while (!((cur_cmd != 10 /*spacer */ ) && (cur_cmd != 0 /*relax */ ) /*:422 */ ));
 lab21:                        /*reswitch */ switch (cur_cmd) {
    case 11:
    case 12:
    case 68:
        {
            c = eqtb[6710516L /*math_code_base */  + cur_chr].hh.v.RH;
            if (math_char_field(c) == 2097151L /*active_math_char */ ) {
                {
                    cur_cs = cur_chr + 1;
                    cur_cmd = eqtb[cur_cs].hh.u.B0;
                    cur_chr = eqtb[cur_cs].hh.v.RH;
                    x_token();
                    back_input();
                }
                goto lab20;
            }
        }
        break;
    case 16:
        {
            scan_char_num();
            cur_chr = cur_val;
            cur_cmd = 68 /*char_given */ ;
            goto lab21;
        }
        break;
    case 17:
        if (cur_chr == 2) {
            scan_math_class_int();
            c = set_class_field(cur_val);
            scan_math_fam_int();
            c = c + set_family_field(cur_val);
            scan_usv_num();
            c = c + cur_val;
        } else if (cur_chr == 1) {
            scan_xetex_math_char_int();
            c = cur_val;
        } else {

            scan_fifteen_bit_int();
            c = set_class_field(cur_val / 4096) + set_family_field((cur_val % 4096) / 256) + (cur_val % 256);
        }
        break;
    case 69:
        {
            c = set_class_field(cur_chr / 4096) + set_family_field((cur_chr % 4096) / 256) + (cur_chr % 256);
        }
        break;
    case 70:
        c = cur_chr;
        break;
    case 15:
        {
            if (cur_chr == 1) {
                scan_math_class_int();
                c = set_class_field(cur_val);
                scan_math_fam_int();
                c = c + set_family_field(cur_val);
                scan_usv_num();
                c = c + cur_val;
            } else {

                scan_delimiter_int();
                c = cur_val / 4096;
                c = set_class_field(c / 4096) + set_family_field((c % 4096) / 256) + (c % 256);
            }
        }
        break;
    default:
        {
            back_input();
            scan_left_brace();
            save_stack[save_ptr + 0].cint = p;
            save_ptr++;
            push_math(9 /*math_group */ );
            return;
        }
        break;
    }
    mem[p].hh.v.RH = 1 /*math_char */ ;
    mem[p].hh.u.B1 = c % 65536L;
    if ((math_class_field(c) == 7)
        && ((eqtb[8938784L /*int_base 44 */ ].cint >= 0)
            && (eqtb[8938784L /*int_base 44 */ ].cint < 256 /*number_math_families */ )))
        mem[p].hh.u.B0 = eqtb[8938784L /*int_base 44 */ ].cint;
    else
        mem[p].hh.u.B0 = (math_fam_field(c));
    mem[p].hh.u.B0 = mem[p].hh.u.B0 + (math_char_field(c) / 65536L) * 256;
}

void zset_math_char(integer c)
{
    set_math_char_regmem halfword p;
    UnicodeScalar ch;
    if (math_char_field(c) == 2097151L /*active_math_char */ ) {        /*1187: */
        cur_cs = cur_chr + 1;
        cur_cmd = eqtb[cur_cs].hh.u.B0;
        cur_chr = eqtb[cur_cs].hh.v.RH;
        x_token();
        back_input();
    } else {

        p = new_noad();
        mem[p + 1].hh.v.RH = 1 /*math_char */ ;
        ch = math_char_field(c);
        mem[p + 1].hh.u.B1 = ch % 65536L;
        mem[p + 1].hh.u.B0 = math_fam_field(c);
        if (math_class_field(c) == 7) {
            if (((eqtb[8938784L /*int_base 44 */ ].cint >= 0)
                 && (eqtb[8938784L /*int_base 44 */ ].cint < 256 /*number_math_families */ )))
                mem[p + 1].hh.u.B0 = eqtb[8938784L /*int_base 44 */ ].cint;
            mem[p].hh.u.B0 = 16 /*ord_noad */ ;
        } else
            mem[p].hh.u.B0 = 16 /*ord_noad */  + math_class_field(c);
        mem[p + 1].hh.u.B0 = mem[p + 1].hh.u.B0 + (ch / 65536L) * 256;
        mem[cur_list.tail_field].hh.v.RH = p;
        cur_list.tail_field = p;
    }
}

void math_limit_switch(void)
{
    math_limit_switch_regmem if (cur_list.head_field != cur_list.tail_field) {

        if (mem[cur_list.tail_field].hh.u.B0 == 17 /*op_noad */ ) {
            mem[cur_list.tail_field].hh.u.B1 = cur_chr;
            return;
        }
    }
    {
        if (interaction == 3 /*error_stop_mode */ ) ;
        if (file_line_error_style_p)
            print_file_line();
        else
            print_nl(65544L /*"! " */ );
        print(66528L /*"Limit controls must follow a math operator" */ );
    }
    {
        help_ptr = 1;
        help_line[0] = 66529L /*"I'm ignoring this misplaced \limits or \nolimits command." */ ;
    }
    error();
}

void zscan_delimiter(halfword p, boolean r)
{
    scan_delimiter_regmem if (r) {
        if (cur_chr == 1) {
            cur_val1 = 1073741824L;
            scan_math_fam_int();
            cur_val1 = cur_val1 + cur_val * 2097152L;
            scan_usv_num();
            cur_val = cur_val1 + cur_val;
        } else
            scan_delimiter_int();
    } else {

        do {
            get_x_token();
        } while (!((cur_cmd != 10 /*spacer */ ) && (cur_cmd != 0 /*relax */ ) /*:422 */ ));
        switch (cur_cmd) {
        case 11:
        case 12:
            {
                cur_val = eqtb[8939080L /*del_code_base */  + cur_chr].cint;
            }
            break;
        case 15:
            if (cur_chr == 1) {
                cur_val1 = 1073741824L;
                scan_math_class_int();
                scan_math_fam_int();
                cur_val1 = cur_val1 + cur_val * 2097152L;
                scan_usv_num();
                cur_val = cur_val1 + cur_val;
            } else
                scan_delimiter_int();
            break;
        default:
            {
                cur_val = -1;
            }
            break;
        }
    }
    if (cur_val < 0) {
        {
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66530L /*"Missing delimiter (. inserted)" */ );
            }
            {
                help_ptr = 6;
                help_line[5] = 66531L /*"I was expecting to see something like `(' or `\_' or" */ ;
                help_line[4] = 66532L /*"`\_' here. If you typed, e.g., `_' instead of `\_', you" */ ;
                help_line[3] = 66533L /*"should probably delete the `_' by typing `1' now, so that" */ ;
                help_line[2] = 66534L /*"braces don't get unbalanced. Otherwise just proceed." */ ;
                help_line[1] = 66535L /*"Acceptable delimiters are characters whose \delcode is" */ ;
                help_line[0] = 66536L /*"nonnegative, or you can use `\delimiter <delimiter code>'." */ ;
            }
            back_error();
            cur_val = 0;
        }
    }
    if (cur_val >= 1073741824L) {
        mem[p].qqqq.u.B0 = ((cur_val % 2097152L) / 65536L) * 256 + (cur_val / 2097152L) % 256;
        mem[p].qqqq.u.B1 = cur_val % 65536L;
        mem[p].qqqq.u.B2 = 0;
        mem[p].qqqq.u.B3 = 0;
    } else {

        mem[p].qqqq.u.B0 = (cur_val / 1048576L) % 16;
        mem[p].qqqq.u.B1 = (cur_val / 4096) % 256;
        mem[p].qqqq.u.B2 = (cur_val / 256) % 16;
        mem[p].qqqq.u.B3 = cur_val % 256;
    }
}

void math_radical(void)
{
    math_radical_regmem {
        mem[cur_list.tail_field].hh.v.RH = get_node(5 /*radical_noad_size */ );
        cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
    }
    mem[cur_list.tail_field].hh.u.B0 = 24 /*radical_noad */ ;
    mem[cur_list.tail_field].hh.u.B1 = 0 /*normal */ ;
    mem[cur_list.tail_field + 1].hh = empty_field;
    mem[cur_list.tail_field + 3].hh = empty_field;
    mem[cur_list.tail_field + 2].hh = empty_field;
    scan_delimiter(cur_list.tail_field + 4, true);
    scan_math(cur_list.tail_field + 1);
}

void math_ac(void)
{
    math_ac_regmem integer c;
    if (cur_cmd == 45 /*accent */ ) {   /*1201: */
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66539L /*"Please use " */ );
        }
        print_esc(65830L /*"mathaccent" */ );
        print(66540L /*" for accents in math mode" */ );
        {
            help_ptr = 2;
            help_line[1] = 66541L /*"I'm changing \accent to \mathaccent here; wish me luck." */ ;
            help_line[0] = 66542L /*"(Accents are not the same in formulas as they are in text.)" */ ;
        }
        error();
    }
    {
        mem[cur_list.tail_field].hh.v.RH = get_node(5 /*accent_noad_size */ );
        cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
    }
    mem[cur_list.tail_field].hh.u.B0 = 28 /*accent_noad */ ;
    mem[cur_list.tail_field].hh.u.B1 = 0 /*normal */ ;
    mem[cur_list.tail_field + 1].hh = empty_field;
    mem[cur_list.tail_field + 3].hh = empty_field;
    mem[cur_list.tail_field + 2].hh = empty_field;
    mem[cur_list.tail_field + 4].hh.v.RH = 1 /*math_char */ ;
    if (cur_chr == 1) {
        if (scan_keyword(66537L /*"fixed" */ ))
            mem[cur_list.tail_field].hh.u.B1 = 1 /*fixed_acc */ ;
        else if (scan_keyword(66538L /*"bottom" */ )) {
            if (scan_keyword(66537L /*"fixed" */ ))
                mem[cur_list.tail_field].hh.u.B1 = 3 /*bottom_acc 1 */ ;
            else
                mem[cur_list.tail_field].hh.u.B1 = 2 /*bottom_acc */ ;
        }
        scan_math_class_int();
        c = set_class_field(cur_val);
        scan_math_fam_int();
        c = c + set_family_field(cur_val);
        scan_usv_num();
        cur_val = cur_val + c;
    } else {

        scan_fifteen_bit_int();
        cur_val = set_class_field(cur_val / 4096) + set_family_field((cur_val % 4096) / 256) + (cur_val % 256);
    }
    mem[cur_list.tail_field + 4].hh.u.B1 = cur_val % 65536L;
    if ((math_class_field(cur_val) == 7)
        && ((eqtb[8938784L /*int_base 44 */ ].cint >= 0)
            && (eqtb[8938784L /*int_base 44 */ ].cint < 256 /*number_math_families */ )))
        mem[cur_list.tail_field + 4].hh.u.B0 = eqtb[8938784L /*int_base 44 */ ].cint;
    else
        mem[cur_list.tail_field + 4].hh.u.B0 = math_fam_field(cur_val);
    mem[cur_list.tail_field + 4].hh.u.B0 = mem[cur_list.tail_field + 4].hh.u.B0 + (math_char_field(cur_val) / 65536L) * 256;
    scan_math(cur_list.tail_field + 1);
}

void append_choices(void)
{
    append_choices_regmem {
        mem[cur_list.tail_field].hh.v.RH = new_choice();
        cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
    }
    save_ptr++;
    save_stack[save_ptr - 1].cint = 0;
    push_math(13 /*math_choice_group */ );
    scan_left_brace();
}

halfword zfin_mlist(halfword p)
{
    register halfword Result;
    fin_mlist_regmem halfword q;
    if (cur_list.aux_field.cint != -268435455L) {       /*1220: */
        mem[cur_list.aux_field.cint + 3].hh.v.RH = 3 /*sub_mlist */ ;
        mem[cur_list.aux_field.cint + 3].hh.v.LH = mem[cur_list.head_field].hh.v.RH;
        if (p == -268435455L)
            q = cur_list.aux_field.cint;
        else {

            q = mem[cur_list.aux_field.cint + 2].hh.v.LH;
            if ((mem[q].hh.u.B0 != 30 /*left_noad */ ) || (cur_list.eTeX_aux_field == -268435455L))
                confusion(66249L /*"right" */ );
            mem[cur_list.aux_field.cint + 2].hh.v.LH = mem[cur_list.eTeX_aux_field].hh.v.RH;
            mem[cur_list.eTeX_aux_field].hh.v.RH = cur_list.aux_field.cint;
            mem[cur_list.aux_field.cint].hh.v.RH = p;
        }
    } else {

        mem[cur_list.tail_field].hh.v.RH = p;
        q = mem[cur_list.head_field].hh.v.RH;
    }
    pop_nest();
    Result = q;
    return Result;
}

void build_choices(void)
{
    build_choices_regmem halfword p;
    unsave();
    p = fin_mlist(-268435455L);
    switch (save_stack[save_ptr - 1].cint) {
    case 0:
        mem[cur_list.tail_field + 1].hh.v.LH = p;
        break;
    case 1:
        mem[cur_list.tail_field + 1].hh.v.RH = p;
        break;
    case 2:
        mem[cur_list.tail_field + 2].hh.v.LH = p;
        break;
    case 3:
        {
            mem[cur_list.tail_field + 2].hh.v.RH = p;
            save_ptr--;
            return;
        }
        break;
    }
    save_stack[save_ptr - 1].cint++;
    push_math(13 /*math_choice_group */ );
    scan_left_brace();
}

void sub_sup(void)
{
    sub_sup_regmem small_number t;
    halfword p;
    t = 0 /*empty */ ;
    p = -268435455L;
    if (cur_list.tail_field != cur_list.head_field) {

        if ((mem[cur_list.tail_field].hh.u.B0 >= 16 /*ord_noad */ )
            && (mem[cur_list.tail_field].hh.u.B0 < 30 /*left_noad */ )) {
            p = cur_list.tail_field + 2 + cur_cmd - 7;
            t = mem[p].hh.v.RH;
        }
    }
    if ((p == -268435455L) || (t != 0 /*empty */ )) {   /*1212: */
        {
            mem[cur_list.tail_field].hh.v.RH = new_noad();
            cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
        }
        p = cur_list.tail_field + 2 + cur_cmd - 7;
        if (t != 0 /*empty */ ) {
            if (cur_cmd == 7 /*sup_mark */ ) {
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66543L /*"Double superscript" */ );
                }
                {
                    help_ptr = 1;
                    help_line[0] = 66544L /*"I treat `x^1^2' essentially like `x^1__^2'." */ ;
                }
            } else {

                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66545L /*"Double subscript" */ );
                }
                {
                    help_ptr = 1;
                    help_line[0] = 66546L /*"I treat `x_1_2' essentially like `x_1___2'." */ ;
                }
            }
            error();
        }
    }
    scan_math(p);
}

void math_fraction(void)
{
    math_fraction_regmem small_number c;
    c = cur_chr;
    if (cur_list.aux_field.cint != -268435455L) {       /*1218: */
        if (c >= 3 /*delimited_code */ ) {
            scan_delimiter(mem_top - 12, false);
            scan_delimiter(mem_top - 12, false);
        }
        if (c % 3 /*delimited_code */  == 0 /*above_code */ )
            scan_dimen(false, false, false);
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66553L /*"Ambiguous; you need another _ and _" */ );
        }
        {
            help_ptr = 3;
            help_line[2] = 66554L /*"I'm ignoring this fraction specification, since I don't" */ ;
            help_line[1] = 66555L /*"know whether a construction like `x \over y \over z'" */ ;
            help_line[0] = 66556L /*"means `_x \over y_ \over z' or `x \over _y \over z_'." */ ;
        }
        error();
    } else {

        cur_list.aux_field.cint = get_node(6 /*fraction_noad_size */ );
        mem[cur_list.aux_field.cint].hh.u.B0 = 25 /*fraction_noad */ ;
        mem[cur_list.aux_field.cint].hh.u.B1 = 0 /*normal */ ;
        mem[cur_list.aux_field.cint + 2].hh.v.RH = 3 /*sub_mlist */ ;
        mem[cur_list.aux_field.cint + 2].hh.v.LH = mem[cur_list.head_field].hh.v.RH;
        mem[cur_list.aux_field.cint + 3].hh = empty_field;
        mem[cur_list.aux_field.cint + 4].qqqq = null_delimiter;
        mem[cur_list.aux_field.cint + 5].qqqq = null_delimiter;
        mem[cur_list.head_field].hh.v.RH = -268435455L;
        cur_list.tail_field = cur_list.head_field;
        if (c >= 3 /*delimited_code */ ) {
            scan_delimiter(cur_list.aux_field.cint + 4, false);
            scan_delimiter(cur_list.aux_field.cint + 5, false);
        }
        switch (c % 3 /*delimited_code */ ) {
        case 0:
            {
                scan_dimen(false, false, false);
                mem[cur_list.aux_field.cint + 1].cint = cur_val;
            }
            break;
        case 1:
            mem[cur_list.aux_field.cint + 1].cint = 1073741824L;
            break;
        case 2:
            mem[cur_list.aux_field.cint + 1].cint = 0;
            break;
        }
    }
}

void math_left_right(void)
{
    math_left_right_regmem small_number t;
    halfword p;
    halfword q;
    t = cur_chr;
    if ((t != 30 /*left_noad */ ) && (cur_group != 16 /*math_left_group */ )) { /*1227: */
        if (cur_group == 15 /*math_shift_group */ ) {
            scan_delimiter(mem_top - 12, false);
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66134L /*"Extra " */ );
            }
            if (t == 1) {
                print_esc(66250L /*"middle" */ );
                {
                    help_ptr = 1;
                    help_line[0] = 66557L /*"I'm ignoring a \middle that had no matching \left." */ ;
                }
            } else {

                print_esc(66249L /*"right" */ );
                {
                    help_ptr = 1;
                    help_line[0] = 66558L /*"I'm ignoring a \right that had no matching \left." */ ;
                }
            }
            error();
        } else
            off_save();
    } else {

        p = new_noad();
        mem[p].hh.u.B0 = t;
        scan_delimiter(p + 1, false);
        if (t == 1) {
            mem[p].hh.u.B0 = 31 /*right_noad */ ;
            mem[p].hh.u.B1 = 1;
        }
        if (t == 30 /*left_noad */ )
            q = p;
        else {

            q = fin_mlist(p);
            unsave();
        }
        if (t != 31 /*right_noad */ ) {
            push_math(16 /*math_left_group */ );
            mem[cur_list.head_field].hh.v.RH = q;
            cur_list.tail_field = p;
            cur_list.eTeX_aux_field = p;
        } else {

            {
                mem[cur_list.tail_field].hh.v.RH = new_noad();
                cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
            }
            mem[cur_list.tail_field].hh.u.B0 = 23 /*inner_noad */ ;
            mem[cur_list.tail_field + 1].hh.v.RH = 3 /*sub_mlist */ ;
            mem[cur_list.tail_field + 1].hh.v.LH = q;
        }
    }
}

void zapp_display(halfword j, halfword b, scaled d)
{
    app_display_regmem scaled z;
    scaled s;
    scaled e;
    integer x;
    halfword p, q, r, t, u;
    s = eqtb[10053207L /*dimen_base 15 */ ].cint;
    x = eqtb[8938803L /*int_base 63 */ ].cint;
    if (x == 0)
        mem[b + 4].cint = s + d;
    else {

        z = eqtb[10053206L /*dimen_base 14 */ ].cint;
        p = b;
        if (x > 0)
            e = z - d - mem[p + 1].cint;
        else {

            e = d;
            d = z - e - mem[p + 1].cint;
        }
        if (j != -268435455L) {
            b = copy_node_list(j);
            mem[b + 3].cint = mem[p + 3].cint;
            mem[b + 2].cint = mem[p + 2].cint;
            s = s - mem[b + 4].cint;
            d = d + s;
            e = e + mem[b + 1].cint - z - s;
        }
        if ((mem[p].hh.u.B1) == 2 /*dlist */ )
            q = p;
        else {

            r = mem[p + 5].hh.v.RH;
            free_node(p, 8 /*box_node_size */ );
            if (r == -268435455L)
                confusion(66900L /*"LR4" */ );
            if (x > 0) {
                p = r;
                do {
                    q = r;
                    r = mem[r].hh.v.RH;
                } while (!(r == -268435455L));
            } else {

                p = -268435455L;
                q = r;
                do {
                    t = mem[r].hh.v.RH;
                    mem[r].hh.v.RH = p;
                    p = r;
                    r = t;
                } while (!(r == -268435455L));
            }
        }
        if (j == -268435455L) {
            r = new_kern(0);
            t = new_kern(0);
        } else {

            r = mem[b + 5].hh.v.RH;
            t = mem[r].hh.v.RH;
        }
        u = new_math(0, 3 /*end_M_code */ );
        if (mem[t].hh.u.B0 == 10 /*glue_node */ ) {
            j = new_skip_param(8 /*right_skip_code */ );
            mem[q].hh.v.RH = j;
            mem[j].hh.v.RH = u;
            j = mem[t + 1].hh.v.LH;
            mem[temp_ptr].hh.u.B0 = mem[j].hh.u.B0;
            mem[temp_ptr].hh.u.B1 = mem[j].hh.u.B1;
            mem[temp_ptr + 1].cint = e - mem[j + 1].cint;
            mem[temp_ptr + 2].cint = -(integer) mem[j + 2].cint;
            mem[temp_ptr + 3].cint = -(integer) mem[j + 3].cint;
            mem[u].hh.v.RH = t;
        } else {

            mem[t + 1].cint = e;
            mem[t].hh.v.RH = u;
            mem[q].hh.v.RH = t;
        }
        u = new_math(0, 2 /*begin_M_code */ );
        if (mem[r].hh.u.B0 == 10 /*glue_node */ ) {
            j = new_skip_param(7 /*left_skip_code */ );
            mem[u].hh.v.RH = j;
            mem[j].hh.v.RH = p;
            j = mem[r + 1].hh.v.LH;
            mem[temp_ptr].hh.u.B0 = mem[j].hh.u.B0;
            mem[temp_ptr].hh.u.B1 = mem[j].hh.u.B1;
            mem[temp_ptr + 1].cint = d - mem[j + 1].cint;
            mem[temp_ptr + 2].cint = -(integer) mem[j + 2].cint;
            mem[temp_ptr + 3].cint = -(integer) mem[j + 3].cint;
            mem[r].hh.v.RH = u;
        } else {

            mem[r + 1].cint = d;
            mem[r].hh.v.RH = p;
            mem[u].hh.v.RH = r;
            if (j == -268435455L) {
                b = hpack(u, 0, 1 /*additional */ );
                mem[b + 4].cint = s;
            } else
                mem[b + 5].hh.v.RH = u;
        }
    }
    append_to_vlist(b);
}

void after_math(void)
{
    after_math_regmem boolean l;
    boolean danger;
    integer m;
    halfword p;
    halfword a;
    halfword b;
    scaled w;
    scaled z;
    scaled e;
    scaled q;
    scaled d;
    scaled s;
    small_number g1, g2;
    halfword r;
    halfword t;
    halfword pre_t;
    halfword j;
    danger = false;
    if (cur_list.mode_field == 207 /*mmode */ )
        j = cur_list.eTeX_aux_field /*:1530 */ ;
    if (((font_params[eqtb[2253302L /*math_font_base 2 0 */ ].hh.v.RH] < 22 /*total_mathsy_params */ )
         &&
         (!((font_area[eqtb[2253302L /*math_font_base 2 0 */ ].hh.v.RH] == 65534L /*otgr_font_flag */ )
            && (isOpenTypeMathFont(font_layout_engine[eqtb[2253302L /*math_font_base 2 0 */ ].hh.v.RH])))))
        || ((font_params[eqtb[2253558L /*math_font_base 2 256 */ ].hh.v.RH] < 22 /*total_mathsy_params */ )
            &&
            (!((font_area[eqtb[2253558L /*math_font_base 2 256 */ ].hh.v.RH] == 65534L /*otgr_font_flag */ )
               && (isOpenTypeMathFont(font_layout_engine[eqtb[2253558L /*math_font_base 2 256 */ ].hh.v.RH])))))
        || ((font_params[eqtb[2253814L /*math_font_base 2 512 */ ].hh.v.RH] < 22 /*total_mathsy_params */ )
            &&
            (!((font_area[eqtb[2253814L /*math_font_base 2 512 */ ].hh.v.RH] == 65534L /*otgr_font_flag */ )
               && (isOpenTypeMathFont(font_layout_engine[eqtb[2253814L /*math_font_base 2 512 */ ].hh.v.RH])))))) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66559L /*"Math formula deleted: Insufficient symbol fonts" */ );
        }
        {
            help_ptr = 3;
            help_line[2] = 66560L /*"Sorry, but I can't typeset math unless \textfont 2" */ ;
            help_line[1] = 66561L /*"and \scriptfont 2 and \scriptscriptfont 2 have all" */ ;
            help_line[0] = 66562L /*"the \fontdimen values needed in math symbol fonts." */ ;
        }
        error();
        flush_math();
        danger = true;
    } else
        if (((font_params[eqtb[2253303L /*math_font_base 3 0 */ ].hh.v.RH] < 13 /*total_mathex_params */ )
             &&
             (!((font_area[eqtb[2253303L /*math_font_base 3 0 */ ].hh.v.RH] == 65534L /*otgr_font_flag */ )
                && (isOpenTypeMathFont(font_layout_engine[eqtb[2253303L /*math_font_base 3 0 */ ].hh.v.RH])))))
            || ((font_params[eqtb[2253559L /*math_font_base 3 256 */ ].hh.v.RH] < 13 /*total_mathex_params */ )
                &&
                (!((font_area[eqtb[2253559L /*math_font_base 3 256 */ ].hh.v.RH] == 65534L /*otgr_font_flag */ )
                   && (isOpenTypeMathFont(font_layout_engine[eqtb[2253559L /*math_font_base 3 256 */ ].hh.v.RH])))))
            || ((font_params[eqtb[2253815L /*math_font_base 3 512 */ ].hh.v.RH] < 13 /*total_mathex_params */ )
                &&
                (!((font_area[eqtb[2253815L /*math_font_base 3 512 */ ].hh.v.RH] == 65534L /*otgr_font_flag */ )
                   && (isOpenTypeMathFont(font_layout_engine[eqtb[2253815L /*math_font_base 3 512 */ ].hh.v.RH])))))) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66563L /*"Math formula deleted: Insufficient extension fonts" */ );
        }
        {
            help_ptr = 3;
            help_line[2] = 66564L /*"Sorry, but I can't typeset math unless \textfont 3" */ ;
            help_line[1] = 66565L /*"and \scriptfont 3 and \scriptscriptfont 3 have all" */ ;
            help_line[0] = 66566L /*"the \fontdimen values needed in math extension fonts." */ ;
        }
        error();
        flush_math();
        danger = true;
    }
    m = cur_list.mode_field;
    l = false;
    p = fin_mlist(-268435455L);
    if (cur_list.mode_field == -(integer) m) {
        {
            get_x_token();
            if (cur_cmd != 3 /*math_shift */ ) {
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66567L /*"Display math should end with $$" */ );
                }
                {
                    help_ptr = 2;
                    help_line[1] = 66568L /*"The `$' that I just saw supposedly matches a previous `$$'." */ ;
                    help_line[0] = 66569L /*"So I shall assume that you typed `$$' both times." */ ;
                }
                back_error();
            }
        }
        cur_mlist = p;
        cur_style = 2 /*text_style */ ;
        mlist_penalties = false;
        mlist_to_hlist();
        a = hpack(mem[mem_top - 3].hh.v.RH, 0, 1 /*additional */ );
        mem[a].hh.u.B1 = 2 /*dlist */ ;
        unsave();
        save_ptr--;
        if (save_stack[save_ptr + 0].cint == 1)
            l = true;
        danger = false;
        if (cur_list.mode_field == 207 /*mmode */ )
            j = cur_list.eTeX_aux_field /*:1530 */ ;
        if (((font_params[eqtb[2253302L /*math_font_base 2 0 */ ].hh.v.RH] < 22 /*total_mathsy_params */ )
             &&
             (!((font_area[eqtb[2253302L /*math_font_base 2 0 */ ].hh.v.RH] == 65534L /*otgr_font_flag */ )
                && (isOpenTypeMathFont(font_layout_engine[eqtb[2253302L /*math_font_base 2 0 */ ].hh.v.RH])))))
            || ((font_params[eqtb[2253558L /*math_font_base 2 256 */ ].hh.v.RH] < 22 /*total_mathsy_params */ )
                &&
                (!((font_area[eqtb[2253558L /*math_font_base 2 256 */ ].hh.v.RH] == 65534L /*otgr_font_flag */ )
                   && (isOpenTypeMathFont(font_layout_engine[eqtb[2253558L /*math_font_base 2 256 */ ].hh.v.RH])))))
            || ((font_params[eqtb[2253814L /*math_font_base 2 512 */ ].hh.v.RH] < 22 /*total_mathsy_params */ )
                &&
                (!((font_area[eqtb[2253814L /*math_font_base 2 512 */ ].hh.v.RH] == 65534L /*otgr_font_flag */ )
                   && (isOpenTypeMathFont(font_layout_engine[eqtb[2253814L /*math_font_base 2 512 */ ].hh.v.RH])))))) {
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66559L /*"Math formula deleted: Insufficient symbol fonts" */ );
            }
            {
                help_ptr = 3;
                help_line[2] = 66560L /*"Sorry, but I can't typeset math unless \textfont 2" */ ;
                help_line[1] = 66561L /*"and \scriptfont 2 and \scriptscriptfont 2 have all" */ ;
                help_line[0] = 66562L /*"the \fontdimen values needed in math symbol fonts." */ ;
            }
            error();
            flush_math();
            danger = true;
        } else
            if (((font_params[eqtb[2253303L /*math_font_base 3 0 */ ].hh.v.RH] < 13 /*total_mathex_params */ )
                 &&
                 (!((font_area[eqtb[2253303L /*math_font_base 3 0 */ ].hh.v.RH] == 65534L /*otgr_font_flag */ )
                    && (isOpenTypeMathFont(font_layout_engine[eqtb[2253303L /*math_font_base 3 0 */ ].hh.v.RH])))))
                || ((font_params[eqtb[2253559L /*math_font_base 3 256 */ ].hh.v.RH] < 13 /*total_mathex_params */ )
                    &&
                    (!((font_area[eqtb[2253559L /*math_font_base 3 256 */ ].hh.v.RH] == 65534L /*otgr_font_flag */ )
                       && (isOpenTypeMathFont(font_layout_engine[eqtb[2253559L /*math_font_base 3 256 */ ].hh.v.RH])))))
                || ((font_params[eqtb[2253815L /*math_font_base 3 512 */ ].hh.v.RH] < 13 /*total_mathex_params */ )
                    &&
                    (!((font_area[eqtb[2253815L /*math_font_base 3 512 */ ].hh.v.RH] == 65534L /*otgr_font_flag */ )
                       &&
                       (isOpenTypeMathFont(font_layout_engine[eqtb[2253815L /*math_font_base 3 512 */ ].hh.v.RH])))))) {
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66563L /*"Math formula deleted: Insufficient extension fonts" */ );
            }
            {
                help_ptr = 3;
                help_line[2] = 66564L /*"Sorry, but I can't typeset math unless \textfont 3" */ ;
                help_line[1] = 66565L /*"and \scriptfont 3 and \scriptscriptfont 3 have all" */ ;
                help_line[0] = 66566L /*"the \fontdimen values needed in math extension fonts." */ ;
            }
            error();
            flush_math();
            danger = true;
        }
        m = cur_list.mode_field;
        p = fin_mlist(-268435455L);
    } else
        a = -268435455L;
    if (m < 0) {                /*1231: */
        {
            mem[cur_list.tail_field].hh.v.RH = new_math(eqtb[10053193L /*dimen_base 1 */ ].cint, 0 /*before */ );
            cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
        }
        cur_mlist = p;
        cur_style = 2 /*text_style */ ;
        mlist_penalties = (cur_list.mode_field > 0);
        mlist_to_hlist();
        mem[cur_list.tail_field].hh.v.RH = mem[mem_top - 3].hh.v.RH;
        while (mem[cur_list.tail_field].hh.v.RH != -268435455L)
            cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
        {
            mem[cur_list.tail_field].hh.v.RH = new_math(eqtb[10053193L /*dimen_base 1 */ ].cint, 1 /*after */ );
            cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
        }
        cur_list.aux_field.hh.v.LH = 1000;
        unsave();
    } else {

        if (a == -268435455L) { /*1232: */
            get_x_token();
            if (cur_cmd != 3 /*math_shift */ ) {
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66567L /*"Display math should end with $$" */ );
                }
                {
                    help_ptr = 2;
                    help_line[1] = 66568L /*"The `$' that I just saw supposedly matches a previous `$$'." */ ;
                    help_line[0] = 66569L /*"So I shall assume that you typed `$$' both times." */ ;
                }
                back_error();
            }
        }
        cur_mlist = p;
        cur_style = 0 /*display_style */ ;
        mlist_penalties = false;
        mlist_to_hlist();
        p = mem[mem_top - 3].hh.v.RH;
        adjust_tail = mem_top - 5;
        pre_adjust_tail = mem_top - 14;
        b = hpack(p, 0, 1 /*additional */ );
        p = mem[b + 5].hh.v.RH;
        t = adjust_tail;
        adjust_tail = -268435455L;
        pre_t = pre_adjust_tail;
        pre_adjust_tail = -268435455L;
        w = mem[b + 1].cint;
        z = eqtb[10053206L /*dimen_base 14 */ ].cint;
        s = eqtb[10053207L /*dimen_base 15 */ ].cint;
        if (eqtb[8938803L /*int_base 63 */ ].cint < 0)
            s = -(integer) s - z;
        if ((a == -268435455L) || danger) {
            e = 0;
            q = 0;
        } else {

            e = mem[a + 1].cint;
            q = e + math_quad(0 /*text_size */ );
        }
        if (w + q > z) {        /*1236: */
            if ((e != 0)
                && ((w - total_shrink[0 /*normal */ ] + q <= z) || (total_shrink[1 /*fil */ ] != 0)
                    || (total_shrink[2 /*fill */ ] != 0) || (total_shrink[3 /*filll */ ] != 0))) {
                free_node(b, 8 /*box_node_size */ );
                b = hpack(p, z - q, 0 /*exactly */ );
            } else {

                e = 0;
                if (w > z) {
                    free_node(b, 8 /*box_node_size */ );
                    b = hpack(p, z, 0 /*exactly */ );
                }
            }
            w = mem[b + 1].cint;
        }
        mem[b].hh.u.B1 = 2 /*dlist */ ;
        d = half(z - w);
        if ((e > 0) && (d < 2 * e)) {
            d = half(z - w - e);
            if (p != -268435455L) {

                if (!(p >= hi_mem_min)) {

                    if (mem[p].hh.u.B0 == 10 /*glue_node */ )
                        d = 0;
                }
            }
        }
        {
            mem[cur_list.tail_field].hh.v.RH = new_penalty(eqtb[8938751L /*int_base 11 */ ].cint);
            cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
        }
        if ((d + s <= eqtb[10053205L /*dimen_base 13 */ ].cint) || l) {
            g1 = 3 /*above_display_skip_code */ ;
            g2 = 4 /*below_display_skip_code */ ;
        } else {

            g1 = 5 /*above_display_short_skip_code */ ;
            g2 = 6 /*below_display_short_skip_code */ ;
        }
        if (l && (e == 0)) {
            app_display(j, a, 0);
            {
                mem[cur_list.tail_field].hh.v.RH = new_penalty(10000 /*inf_penalty */ );
                cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
            }
        } else {

            mem[cur_list.tail_field].hh.v.RH = new_param_glue(g1);
            cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
        }
        if (e != 0) {
            r = new_kern(z - w - e - d);
            if (l) {
                mem[a].hh.v.RH = r;
                mem[r].hh.v.RH = b;
                b = a;
                d = 0;
            } else {

                mem[b].hh.v.RH = r;
                mem[r].hh.v.RH = a;
            }
            b = hpack(b, 0, 1 /*additional */ );
        }
        app_display(j, b, d);
        if ((a != -268435455L) && (e == 0) && !l) {
            {
                mem[cur_list.tail_field].hh.v.RH = new_penalty(10000 /*inf_penalty */ );
                cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
            }
            app_display(j, a, z - mem[a + 1].cint);
            g2 = 0;
        }
        if (t != mem_top - 5) {
            mem[cur_list.tail_field].hh.v.RH = mem[mem_top - 5].hh.v.RH;
            cur_list.tail_field = t;
        }
        if (pre_t != mem_top - 14) {
            mem[cur_list.tail_field].hh.v.RH = mem[mem_top - 14].hh.v.RH;
            cur_list.tail_field = pre_t;
        }
        {
            mem[cur_list.tail_field].hh.v.RH = new_penalty(eqtb[8938752L /*int_base 12 */ ].cint);
            cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
        }
        if (g2 > 0) {
            mem[cur_list.tail_field].hh.v.RH = new_param_glue(g2);
            cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
        }
        flush_node_list(j);
        resume_after_display();
    }
}

void resume_after_display(void)
{
    resume_after_display_regmem if (cur_group != 15 /*math_shift_group */ )
        confusion(66570L /*"display" */ );
    unsave();
    cur_list.pg_field = cur_list.pg_field + 3;
    push_nest();
    cur_list.mode_field = 104 /*hmode */ ;
    cur_list.aux_field.hh.v.LH = 1000;
    if (eqtb[8938790L /*int_base 50 */ ].cint <= 0)
        cur_lang = 0;
    else if (eqtb[8938790L /*int_base 50 */ ].cint > 255 /*biggest_lang */ )
        cur_lang = 0;
    else
        cur_lang = eqtb[8938790L /*int_base 50 */ ].cint;
    cur_list.aux_field.hh.v.RH = cur_lang;
    cur_list.pg_field =
        (norm_min(eqtb[8938791L /*int_base 51 */ ].cint) * 64 +
         norm_min(eqtb[8938792L /*int_base 52 */ ].cint)) * 65536L + cur_lang;
    {
        get_x_token();
        if (cur_cmd != 10 /*spacer */ )
            back_input();
    }
    if (nest_ptr == 1)
        build_page();
}

void get_r_token(void)
{
 get_r_token_regmem lab20:     /*restart */
    do {
        get_token();
    } while (!(cur_tok != 20971552L /*space_token */ ));
    if ((cur_cs == 0) || (cur_cs > eqtb_top)
        || ((cur_cs > 2243226L /*frozen_control_sequence */ ) && (cur_cs <= 10053470L /*eqtb_size */ ))) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66588L /*"Missing control sequence inserted" */ );
        }
        {
            help_ptr = 5;
            help_line[4] = 66589L /*"Please don't say `\def cs_..._', say `\def\cs_..._'." */ ;
            help_line[3] = 66590L /*"I've inserted an inaccessible control sequence so that your" */ ;
            help_line[2] = 66591L /*"definition will be completed without mixing me up too badly." */ ;
            help_line[1] = 66592L /*"You can recover graciously from this error, if you're" */ ;
            help_line[0] = 66593L /*"careful; see exercise 27.2 in The TeXbook." */ ;
        }
        if (cur_cs == 0)
            back_input();
        cur_tok = 35797657L /*cs_token_flag 2243226 */ ;
        ins_error();
        goto lab20;
    }
}

void trap_zero_glue(void)
{
    trap_zero_glue_regmem
        if ((mem[cur_val + 1].cint == 0) && (mem[cur_val + 2].cint == 0) && (mem[cur_val + 3].cint == 0)) {
        mem[mem_bot].hh.v.RH++;
        delete_glue_ref(cur_val);
        cur_val = mem_bot;
    }
}

void zdo_register_command(small_number a)
{
    do_register_command_regmem halfword l, q, r, s;
    unsigned char /*mu_val */ p;
    boolean e;
    integer w;
    q = cur_cmd;
    e = false;
    {
        if (q != 91 /*register */ ) {
            get_x_token();
            if ((cur_cmd >= 74 /*assign_int */ ) && (cur_cmd <= 77 /*assign_mu_glue */ )) {
                l = cur_chr;
                p = cur_cmd - 74;
                goto lab40;
            }
            if (cur_cmd != 91 /*register */ ) {
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66020L /*"You can't use `" */ );
                }
                print_cmd_chr(cur_cmd, cur_chr);
                print(66021L /*"' after " */ );
                print_cmd_chr(q, 0);
                {
                    help_ptr = 1;
                    help_line[0] = 66629L /*"I'm forgetting what you said and not changing anything." */ ;
                }
                error();
                return;
            }
        }
        if ((cur_chr < mem_bot) || (cur_chr > mem_bot + 19)) {
            l = cur_chr;
            p = (mem[l].hh.u.B0 / 64);
            e = true;
        } else {

            p = cur_chr - mem_bot;
            scan_register_num();
            if (cur_val > 255) {
                find_sa_element(p, cur_val, true);
                l = cur_ptr;
                e = true;
            } else
                switch (p) {
                case 0:
                    l = cur_val + 8938824L;
                    break;
                case 1:
                    l = cur_val + 10053215L;
                    break;
                case 2:
                    l = cur_val + 2252259L;
                    break;
                case 3:
                    l = cur_val + 2252515L;
                    break;
                }
        }
    }
 lab40:/*found */ if (p < 2 /*glue_val */ ) {

        if (e)
            w = mem[l + 2].cint;
        else
            w = eqtb[l].cint;
    } else if (e)
        s = mem[l + 1].hh.v.RH;
    else
        s = eqtb[l].hh.v.RH /*:1272 */ ;
    if (q == 91 /*register */ )
        scan_optional_equals();
    else if (scan_keyword(66625L /*"by" */ )) ;
    arith_error = false;
    if (q < 93 /*multiply */ ) {        /*1273: */

        if (p < 2 /*glue_val */ ) {
            if (p == 0 /*int_val */ )
                scan_int();
            else
                scan_dimen(false, false, false);
            if (q == 92 /*advance */ )
                cur_val = cur_val + w;
        } else {

            scan_glue(p);
            if (q == 92 /*advance */ ) {        /*1274: */
                q = new_spec(cur_val);
                r = s;
                delete_glue_ref(cur_val);
                mem[q + 1].cint = mem[q + 1].cint + mem[r + 1].cint;
                if (mem[q + 2].cint == 0)
                    mem[q].hh.u.B0 = 0 /*normal */ ;
                if (mem[q].hh.u.B0 == mem[r].hh.u.B0)
                    mem[q + 2].cint = mem[q + 2].cint + mem[r + 2].cint;
                else if ((mem[q].hh.u.B0 < mem[r].hh.u.B0) && (mem[r + 2].cint != 0)) {
                    mem[q + 2].cint = mem[r + 2].cint;
                    mem[q].hh.u.B0 = mem[r].hh.u.B0;
                }
                if (mem[q + 3].cint == 0)
                    mem[q].hh.u.B1 = 0 /*normal */ ;
                if (mem[q].hh.u.B1 == mem[r].hh.u.B1)
                    mem[q + 3].cint = mem[q + 3].cint + mem[r + 3].cint;
                else if ((mem[q].hh.u.B1 < mem[r].hh.u.B1) && (mem[r + 3].cint != 0)) {
                    mem[q + 3].cint = mem[r + 3].cint;
                    mem[q].hh.u.B1 = mem[r].hh.u.B1;
                }
                cur_val = q;
            }
        }
    } else {                    /*1275: */

        scan_int();
        if (p < 2 /*glue_val */ ) {

            if (q == 93 /*multiply */ ) {

                if (p == 0 /*int_val */ )
                    cur_val = mult_and_add(w, cur_val, 0, 2147483647L);
                else
                    cur_val = mult_and_add(w, cur_val, 0, 1073741823L);
            } else
                cur_val = x_over_n(w, cur_val);
        } else {

            r = new_spec(s);
            if (q == 93 /*multiply */ ) {
                mem[r + 1].cint = mult_and_add(mem[s + 1].cint, cur_val, 0, 1073741823L);
                mem[r + 2].cint = mult_and_add(mem[s + 2].cint, cur_val, 0, 1073741823L);
                mem[r + 3].cint = mult_and_add(mem[s + 3].cint, cur_val, 0, 1073741823L);
            } else {

                mem[r + 1].cint = x_over_n(mem[s + 1].cint, cur_val);
                mem[r + 2].cint = x_over_n(mem[s + 2].cint, cur_val);
                mem[r + 3].cint = x_over_n(mem[s + 3].cint, cur_val);
            }
            cur_val = r;
        }
    }
    if (arith_error) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66626L /*"Arithmetic overflow" */ );
        }
        {
            help_ptr = 2;
            help_line[1] = 66627L /*"I can't carry out that multiplication or division," */ ;
            help_line[0] = 66628L /*"since the result is out of range." */ ;
        }
        if (p >= 2 /*glue_val */ )
            delete_glue_ref(cur_val);
        error();
        return;
    }
    if (p < 2 /*glue_val */ ) {

        if (e) {

            if ((a >= 4))
                gsa_w_def(l, cur_val);
            else
                sa_w_def(l, cur_val);
        } else if ((a >= 4))
            geq_word_define(l, cur_val);
        else
            eq_word_define(l, cur_val);
    } else {

        trap_zero_glue();
        if (e) {

            if ((a >= 4))
                gsa_def(l, cur_val);
            else
                sa_def(l, cur_val);
        } else if ((a >= 4))
            geq_define(l, 119 /*glue_ref */ , cur_val);
        else
            eq_define(l, 119 /*glue_ref */ , cur_val);
    }
}

void alter_aux(void)
{
    alter_aux_regmem halfword c;
    if (cur_chr != abs(cur_list.mode_field))
        report_illegal_case();
    else {

        c = cur_chr;
        scan_optional_equals();
        if (c == 1 /*vmode */ ) {
            scan_dimen(false, false, false);
            cur_list.aux_field.cint = cur_val;
        } else {

            scan_int();
            if ((cur_val <= 0) || (cur_val > 32767)) {
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66632L /*"Bad space factor" */ );
                }
                {
                    help_ptr = 1;
                    help_line[0] = 66633L /*"I allow only values in the range 1..32767 here." */ ;
                }
                int_error(cur_val);
            } else
                cur_list.aux_field.hh.v.LH = cur_val;
        }
    }
}

void alter_prev_graf(void)
{
    alter_prev_graf_regmem integer p;
    nest[nest_ptr] = cur_list;
    p = nest_ptr;
    while (abs(nest[p].mode_field) != 1 /*vmode */ )
        p--;
    scan_optional_equals();
    scan_int();
    if (cur_val < 0) {
        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66353L /*"Bad " */ );
        }
        print_esc(65846L /*"prevgraf" */ );
        {
            help_ptr = 1;
            help_line[0] = 66634L /*"I allow only nonnegative values here." */ ;
        }
        int_error(cur_val);
    } else {

        nest[p].pg_field = cur_val;
        cur_list = nest[nest_ptr];
    }
}

void alter_page_so_far(void)
{
    alter_page_so_far_regmem unsigned char c;
    c = cur_chr;
    scan_optional_equals();
    scan_dimen(false, false, false);
    page_so_far[c] = cur_val;
}

void alter_integer(void)
{
    alter_integer_regmem small_number c;
    c = cur_chr;
    scan_optional_equals();
    scan_int();
    if (c == 0)
        dead_cycles = /*1483: */ cur_val;
    else if (c == 2) {
        if ((cur_val < 0 /*batch_mode */ ) || (cur_val > 3 /*error_stop_mode */ )) {
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66875L /*"Bad interaction mode" */ );
            }
            {
                help_ptr = 2;
                help_line[1] = 66876L /*"Modes are 0=batch, 1=nonstop, 2=scroll, and" */ ;
                help_line[0] = 66877L /*"3=errorstop. Proceed, and I'll ignore this case." */ ;
            }
            int_error(cur_val);
        } else {

            cur_chr = cur_val;
            new_interaction();
        }
    } else
        insert_penalties = cur_val;
}

void alter_box_dimen(void)
{
    alter_box_dimen_regmem small_number c;
    halfword b;
    c = cur_chr;
    scan_register_num();
    if (cur_val < 256)
        b = eqtb[2253043L /*box_base */  + cur_val].hh.v.RH;
    else {

        find_sa_element(4, cur_val, false);
        if (cur_ptr == -268435455L)
            b = -268435455L;
        else
            b = mem[cur_ptr + 1].hh.v.RH;
    }
    scan_optional_equals();
    scan_dimen(false, false, false);
    if (b != -268435455L)
        mem[b + c].cint = cur_val;
}

void znew_font(small_number a)
{
    new_font_regmem halfword u;
    scaled s;
    internal_font_number f;
    str_number t;
    unsigned char /*max_selector */ old_setting;
    if (job_name == 0)
        open_log_file();
    get_r_token();
    u = cur_cs;
    if (u >= 2228226L /*hash_base */ )
        t = hash[u].v.RH;
    else if (u >= 1114113L /*single_base */ ) {

        if (u == 2228225L /*null_cs */ )
            t = 66640L /*"FONT" */ ;
        else
            t = u - 1114113L;
    } else {

        old_setting = selector;
        selector = SELECTOR_NEW_STRING ;
        print(66640L /*"FONT" */ );
        print(u - 1);
        selector = old_setting;
        {
            if (pool_ptr + 1 > pool_size)
                overflow(65539L /*"pool size" */ , pool_size - init_pool_ptr);
        }
        t = make_string();
    }
    if ((a >= 4))
        geq_define(u, 89 /*set_font */ , 0 /*font_base */ );
    else
        eq_define(u, 89 /*set_font */ , 0 /*font_base */ );
    scan_optional_equals();
    scan_file_name();
    name_in_progress = true;
    if (scan_keyword(66641L /*"at" */ )) {      /*1294: */
        scan_dimen(false, false, false);
        s = cur_val;
        if ((s <= 0) || (s >= 134217728L)) {
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66643L /*"Improper `at' size (" */ );
            }
            print_scaled(s);
            print(66644L /*"pt), replaced by 10pt" */ );
            {
                help_ptr = 2;
                help_line[1] = 66645L /*"I can only handle fonts at positive sizes that are" */ ;
                help_line[0] = 66646L /*"less than 2048pt, so I've changed what you said to 10pt." */ ;
            }
            error();
            s = 10 * 65536L;
        }
    } else if (scan_keyword(66642L /*"scaled" */ )) {
        scan_int();
        s = -(integer) cur_val;
        if ((cur_val <= 0) || (cur_val > 32768L)) {
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(65872L /*"Illegal magnification has been changed to 1000" */ );
            }
            {
                help_ptr = 1;
                help_line[0] = 65873L /*"The magnification ratio must be between 1 and 32768." */ ;
            }
            int_error(cur_val);
            s = -1000;
        }
    } else
        s = -1000;
    name_in_progress = false /*:1293 */ ;
    {
        register integer for_end;
        f = 1 /*font_base 1 */ ;
        for_end = font_ptr;
        if (f <= for_end)
            do {
                if (str_eq_str(font_name[f], cur_name)
                    &&
                    (((cur_area == 65622L /*"" */ )
                      && ((font_area[f] == 65535L /*aat_font_flag */ )
                          || (font_area[f] == 65534L /*otgr_font_flag */ ))) || str_eq_str(font_area[f], cur_area))) {
                    if (s > 0) {
                        if (s == font_size[f])
                            goto lab50;
                    } else if (font_size[f] == xn_over_d(font_dsize[f], -(integer) s, 1000))
                        goto lab50;
                }
                append_str(cur_area);
                append_str(cur_name);
                append_str(cur_ext);
                if (str_eq_str(font_name[f], make_string())) {
                    {
                        str_ptr--;
                        pool_ptr = str_start[(str_ptr) - 65536L];
                    }
                    if (((font_area[f] == 65535L /*aat_font_flag */ ) || (font_area[f] == 65534L /*otgr_font_flag */ ))) {
                        if (s > 0) {
                            if (s == font_size[f])
                                goto lab50;
                        } else if (font_size[f] == xn_over_d(font_dsize[f], -(integer) s, 1000))
                            goto lab50;
                    }
                } else {

                    str_ptr--;
                    pool_ptr = str_start[(str_ptr) - 65536L];
                }
            }
            while (f++ < for_end);
    }
    f = read_font_info(u, cur_name, cur_area, s);
 lab50:                        /*common_ending */ if ((a >= 4))
        geq_define(u, 89 /*set_font */ , f);
    else
        eq_define(u, 89 /*set_font */ , f);
    eqtb[2243238L /*font_id_base */  + f] = eqtb[u];
    hash[2243238L /*font_id_base */  + f].v.RH = t;
}

void new_interaction(void)
{
    new_interaction_regmem print_ln();
    interaction = cur_chr;
    if (interaction == 0 /*batch_mode */ )
        selector = SELECTOR_NO_PRINT;
    else
        selector = SELECTOR_TERM_ONLY/*:79 */ ;
    if (log_opened)
        selector = selector + 2;
}

void issue_message(void)
{
    issue_message_regmem unsigned char /*max_selector */ old_setting;
    unsigned char c;
    str_number s;
    c = cur_chr;
    mem[mem_top - 12].hh.v.RH = scan_toks(false, true);
    old_setting = selector;
    selector = SELECTOR_NEW_STRING ;
    token_show(def_ref);
    selector = old_setting;
    flush_list(def_ref);
    {
        if (pool_ptr + 1 > pool_size)
            overflow(65539L /*"pool size" */ , pool_size - init_pool_ptr);
    }
    s = make_string();
    if (c == 0) {               /*1315: */
        if (term_offset + length(s) > max_print_line - 2)
            print_ln();
        else if ((term_offset > 0) || (file_offset > 0))
            print_char(32 /*" " */ );
        print(s);
        fflush(stdout);
    } else {                    /*1318: */

        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(65622L /*"" */ );
        }
        print(s);
        if (eqtb[2252780L /*err_help_loc */ ].hh.v.RH != -268435455L)
            use_err_help = true;
        else if (long_help_seen) {
            help_ptr = 1;
            help_line[0] = 66653L /*"(That was another \errmessage.)" */ ;
        } else {

            if (interaction < 3 /*error_stop_mode */ )
                long_help_seen = true;
            {
                help_ptr = 4;
                help_line[3] = 66654L /*"This error message was generated by an \errmessage" */ ;
                help_line[2] = 66655L /*"command, so I can't give any explicit help." */ ;
                help_line[1] = 66656L /*"Pretend that you're Hercule Poirot: Examine all clues," */ ;
                help_line[0] = 66657L /*"and deduce the truth by order and method." */ ;
            }
        }
        error();
        use_err_help = false;
    }
    {
        str_ptr--;
        pool_ptr = str_start[(str_ptr) - 65536L];
    }
}

void shift_case(void)
{
    shift_case_regmem halfword b;
    halfword p;
    halfword t;
    integer c;
    b = cur_chr;
    p = scan_toks(false, false);
    p = mem[def_ref].hh.v.RH;
    while (p != -268435455L) {

        t = mem[p].hh.v.LH;
        if (t < 34668544L /*cs_token_flag 1114113 */ ) {
            c = t % 2097152L /*max_char_val */ ;
            if (eqtb[b + c].hh.v.RH != 0)
                mem[p].hh.v.LH = t - c + eqtb[b + c].hh.v.RH;
        }
        p = mem[p].hh.v.RH;
    }
    begin_token_list(mem[def_ref].hh.v.RH, 3 /*backed_up */ );
    {
        mem[def_ref].hh.v.RH = avail;
        avail = def_ref;
    }
}

void show_whatever(void)
{
    show_whatever_regmem halfword p;
    small_number t;
    unsigned char /*or_code */ m;
    integer l;
    integer n;
    switch (cur_chr) {
    case 3:
        {
            begin_diagnostic();
            show_activities();
        }
        break;
    case 1:
        {
            scan_register_num();
            if (cur_val < 256)
                p = eqtb[2253043L /*box_base */  + cur_val].hh.v.RH;
            else {

                find_sa_element(4, cur_val, false);
                if (cur_ptr == -268435455L)
                    p = -268435455L;
                else
                    p = mem[cur_ptr + 1].hh.v.RH;
            }
            begin_diagnostic();
            print_nl(66673L /*"> \box" */ );
            print_int(cur_val);
            print_char(61 /*"=" */ );
            if (p == -268435455L)
                print(65706L /*"void" */ );
            else
                show_box(p);
        }
        break;
    case 0:
        {
            get_token();
            if (interaction == 3 /*error_stop_mode */ ) ;
            print_nl(66669L /*"> " */ );
            if (cur_cs != 0) {
                sprint_cs(cur_cs);
                print_char(61 /*"=" */ );
            }
            print_meaning();
            goto lab50;
        }
        break;
    case 4:
        {
            begin_diagnostic();
            show_save_groups();
        }
        break;
    case 6:
        {
            begin_diagnostic();
            print_nl(65622L /*"" */ );
            print_ln();
            if (cond_ptr == -268435455L) {
                print_nl(65657L /*"### " */ );
                print(66872L /*"no active conditionals" */ );
            } else {

                p = cond_ptr;
                n = 0;
                do {
                    n++;
                    p = mem[p].hh.v.RH;
                } while (!(p == -268435455L));
                p = cond_ptr;
                t = cur_if;
                l = if_line;
                m = if_limit;
                do {
                    print_nl(66873L /*"### level " */ );
                    print_int(n);
                    print(65589L /*": " */ );
                    print_cmd_chr(107 /*if_test */ , t);
                    if (m == 2 /*fi_code */ )
                        print_esc(66133L /*"else" */ );
                    if (l != 0) {
                        print(66871L /*" entered on line " */ );
                        print_int(l);
                    }
                    n--;
                    t = mem[p].hh.u.B1;
                    l = mem[p + 1].cint;
                    m = mem[p].hh.u.B0;
                    p = mem[p].hh.v.RH;
                } while (!(p == -268435455L));
            }
        }
        break;
    default:
        {
            p = the_toks();
            if (interaction == 3 /*error_stop_mode */ ) ;
            print_nl(66669L /*"> " */ );
            token_show(mem_top - 3);
            flush_list(mem[mem_top - 3].hh.v.RH);
            goto lab50;
        }
        break;
    }
    end_diagnostic(true);
    {
        if (interaction == 3 /*error_stop_mode */ ) ;
        if (file_line_error_style_p)
            print_file_line();
        else
            print_nl(65544L /*"! " */ );
        print(66674L /*"OK" */ );
    }
    if (selector == SELECTOR_TERM_AND_LOG) {

        if (eqtb[8938769L /*int_base 29 */ ].cint <= 0) {
            selector = SELECTOR_TERM_ONLY;
            print(66675L /*" (see the transcript file)" */ );
            selector = SELECTOR_TERM_AND_LOG;
        }
    }
 lab50:/*common_ending */ if (interaction < 3 /*error_stop_mode */ ) {
        help_ptr = 0;
        error_count--;
    } else if (eqtb[8938769L /*int_base 29 */ ].cint > 0) {
        {
            help_ptr = 3;
            help_line[2] = 66664L /*"This isn't an error message; I'm just \showing something." */ ;
            help_line[1] = 66665L /*"Type `I\show...' to show more (e.g., \show\cs," */ ;
            help_line[0] = 66666L /*"\showthe\count10, \showbox255, \showlists)." */ ;
        }
    } else {

        {
            help_ptr = 5;
            help_line[4] = 66664L /*"This isn't an error message; I'm just \showing something." */ ;
            help_line[3] = 66665L /*"Type `I\show...' to show more (e.g., \show\cs," */ ;
            help_line[2] = 66666L /*"\showthe\count10, \showbox255, \showlists)." */ ;
            help_line[1] = 66667L /*"And type `I\tracingonline=1\show...' to show boxes and" */ ;
            help_line[0] = 66668L /*"lists on your terminal as well as in the transcript file." */ ;
        }
    }
    error();
}

void znew_write_whatsit(small_number w)
{
    new_write_whatsit_regmem new_whatsit(cur_chr, w);
    if (w != 2 /*write_node_size */ )
        scan_four_bit_int();
    else {

        scan_int();
        if (cur_val < 0)
            cur_val = 17;
        else if ((cur_val > 15) && (cur_val != 18))
            cur_val = 16;
    }
    mem[cur_list.tail_field + 1].hh.v.LH = cur_val;
}

void zload_picture(boolean is_pdf)
{
    load_picture_regmem char *pic_path;
    real_rect bounds;
    transform t, t2;
    real_point corners[4];
    double x_size_req, y_size_req;
    boolean check_keywords;
    double xmin, xmax, ymin, ymax;
    small_number i;
    integer page;
    integer pdf_box_type;
    integer result;
    scan_file_name();
    pack_file_name(cur_name, cur_area, cur_ext);
    pdf_box_type = 0;
    page = 0;
    if (is_pdf) {
        if (scan_keyword(66391L /*"page" */ )) {
            scan_int();
            page = cur_val;
        }
        pdf_box_type = pdfbox_none;
        if (scan_keyword(66759L /*"crop" */ ))
            pdf_box_type = pdfbox_crop;
        else if (scan_keyword(66760L /*"media" */ ))
            pdf_box_type = pdfbox_media;
        else if (scan_keyword(66761L /*"bleed" */ ))
            pdf_box_type = pdfbox_bleed;
        else if (scan_keyword(66762L /*"trim" */ ))
            pdf_box_type = pdfbox_trim;
        else if (scan_keyword(66763L /*"art" */ ))
            pdf_box_type = pdfbox_art;
    }
    if (pdf_box_type == pdfbox_none)
        result = find_pic_file(&pic_path, &bounds, pdfbox_crop, page);
    else
        result = find_pic_file(&pic_path, &bounds, pdf_box_type, page);
    SET_POINT(corners[0], bounds.x, bounds.y);
    SET_POINT(corners[1], corners[0].x, bounds.y + bounds.ht);
    SET_POINT(corners[2], bounds.x + bounds.wd, corners[1].y);
    SET_POINT(corners[3], corners[2].x, corners[0].y);
    x_size_req = 0.0;
    y_size_req = 0.0;
    make_identity(&t);
    check_keywords = true;
    while (check_keywords) {

        if (scan_keyword(66642L /*"scaled" */ )) {
            scan_int();
            if ((x_size_req == 0.0) && (y_size_req == 0.0)) {
                make_scale(&t2, cur_val / ((double)1000.0), cur_val / ((double)1000.0));
                {
                    register integer for_end;
                    i = 0;
                    for_end = 3;
                    if (i <= for_end)
                        do
                            transform_point(&corners[i], &t2);
                        while (i++ < for_end);
                }
                transform_concat(&t, &t2);
            }
        } else if (scan_keyword(66764L /*"xscaled" */ )) {
            scan_int();
            if ((x_size_req == 0.0) && (y_size_req == 0.0)) {
                make_scale(&t2, cur_val / ((double)1000.0), 1.0);
                {
                    register integer for_end;
                    i = 0;
                    for_end = 3;
                    if (i <= for_end)
                        do
                            transform_point(&corners[i], &t2);
                        while (i++ < for_end);
                }
                transform_concat(&t, &t2);
            }
        } else if (scan_keyword(66765L /*"yscaled" */ )) {
            scan_int();
            if ((x_size_req == 0.0) && (y_size_req == 0.0)) {
                make_scale(&t2, 1.0, cur_val / ((double)1000.0));
                {
                    register integer for_end;
                    i = 0;
                    for_end = 3;
                    if (i <= for_end)
                        do
                            transform_point(&corners[i], &t2);
                        while (i++ < for_end);
                }
                transform_concat(&t, &t2);
            }
        } else if (scan_keyword(66079L /*"width" */ )) {
            scan_dimen(false, false, false);
            if (cur_val <= 0) {
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66766L /*"Improper image " */ );
                }
                print(66767L /*"size (" */ );
                print_scaled(cur_val);
                print(66768L /*"pt) will be ignored" */ );
                {
                    help_ptr = 2;
                    help_line[1] = 66769L /*"I can't scale images to zero or negative sizes," */ ;
                    help_line[0] = 66770L /*"so I'm ignoring this." */ ;
                }
                error();
            } else
                x_size_req = Fix2D(cur_val);
        } else if (scan_keyword(66080L /*"height" */ )) {
            scan_dimen(false, false, false);
            if (cur_val <= 0) {
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66766L /*"Improper image " */ );
                }
                print(66767L /*"size (" */ );
                print_scaled(cur_val);
                print(66768L /*"pt) will be ignored" */ );
                {
                    help_ptr = 2;
                    help_line[1] = 66769L /*"I can't scale images to zero or negative sizes," */ ;
                    help_line[0] = 66770L /*"so I'm ignoring this." */ ;
                }
                error();
            } else
                y_size_req = Fix2D(cur_val);
        } else if (scan_keyword(66771L /*"rotated" */ )) {
            scan_decimal();
            if ((x_size_req != 0.0) || (y_size_req != 0.0)) {
                {
                    xmin = 1000000.0;
                    xmax = -(integer) xmin;
                    ymin = xmin;
                    ymax = xmax;
                    {
                        register integer for_end;
                        i = 0;
                        for_end = 3;
                        if (i <= for_end)
                            do {
                                if (corners[i].x < xmin)
                                    xmin = corners[i].x;
                                if (corners[i].x > xmax)
                                    xmax = corners[i].x;
                                if (corners[i].y < ymin)
                                    ymin = corners[i].y;
                                if (corners[i].y > ymax)
                                    ymax = corners[i].y;
                            }
                            while (i++ < for_end);
                    }
                }
                if (x_size_req == 0.0) {
                    make_scale(&t2, y_size_req / ((double)(ymax - ymin)),
                               y_size_req / ((double)(ymax - ymin)));
                } else if (y_size_req == 0.0) {
                    make_scale(&t2, x_size_req / ((double)(xmax - xmin)),
                               x_size_req / ((double)(xmax - xmin)));
                } else {

                    make_scale(&t2, x_size_req / ((double)(xmax - xmin)),
                               y_size_req / ((double)(ymax - ymin)));
                }
                {
                    register integer for_end;
                    i = 0;
                    for_end = 3;
                    if (i <= for_end)
                        do
                            transform_point(&corners[i], &t2);
                        while (i++ < for_end);
                }
                x_size_req = 0.0;
                y_size_req = 0.0;
                transform_concat(&t, &t2);
            }
            make_rotation(&t2, Fix2D(cur_val) * 3.141592653589793 / ((double)180.0));
            {
                register integer for_end;
                i = 0;
                for_end = 3;
                if (i <= for_end)
                    do
                        transform_point(&corners[i], &t2);
                    while (i++ < for_end);
            }
            {
                xmin = 1000000.0;
                xmax = -(integer) xmin;
                ymin = xmin;
                ymax = xmax;
                {
                    register integer for_end;
                    i = 0;
                    for_end = 3;
                    if (i <= for_end)
                        do {
                            if (corners[i].x < xmin)
                                xmin = corners[i].x;
                            if (corners[i].x > xmax)
                                xmax = corners[i].x;
                            if (corners[i].y < ymin)
                                ymin = corners[i].y;
                            if (corners[i].y > ymax)
                                ymax = corners[i].y;
                        }
                        while (i++ < for_end);
                }
            }
            SET_POINT(corners[0], xmin, ymin);
            SET_POINT(corners[1], xmin, ymax);
            SET_POINT(corners[2], xmax, ymax);
            SET_POINT(corners[3], xmax, ymin);
            transform_concat(&t, &t2);
        } else
            check_keywords = false;
    }
    if ((x_size_req != 0.0) || (y_size_req != 0.0)) {
        {
            xmin = 1000000.0;
            xmax = -(integer) xmin;
            ymin = xmin;
            ymax = xmax;
            {
                register integer for_end;
                i = 0;
                for_end = 3;
                if (i <= for_end)
                    do {
                        if (corners[i].x < xmin)
                            xmin = corners[i].x;
                        if (corners[i].x > xmax)
                            xmax = corners[i].x;
                        if (corners[i].y < ymin)
                            ymin = corners[i].y;
                        if (corners[i].y > ymax)
                            ymax = corners[i].y;
                    }
                    while (i++ < for_end);
            }
        }
        if (x_size_req == 0.0) {
            make_scale(&t2, y_size_req / ((double)(ymax - ymin)), y_size_req / ((double)(ymax - ymin)));
        } else if (y_size_req == 0.0) {
            make_scale(&t2, x_size_req / ((double)(xmax - xmin)), x_size_req / ((double)(xmax - xmin)));
        } else {

            make_scale(&t2, x_size_req / ((double)(xmax - xmin)), y_size_req / ((double)(ymax - ymin)));
        }
        {
            register integer for_end;
            i = 0;
            for_end = 3;
            if (i <= for_end)
                do
                    transform_point(&corners[i], &t2);
                while (i++ < for_end);
        }
        x_size_req = 0.0;
        y_size_req = 0.0;
        transform_concat(&t, &t2);
    }
    {
        xmin = 1000000.0;
        xmax = -(integer) xmin;
        ymin = xmin;
        ymax = xmax;
        {
            register integer for_end;
            i = 0;
            for_end = 3;
            if (i <= for_end)
                do {
                    if (corners[i].x < xmin)
                        xmin = corners[i].x;
                    if (corners[i].x > xmax)
                        xmax = corners[i].x;
                    if (corners[i].y < ymin)
                        ymin = corners[i].y;
                    if (corners[i].y > ymax)
                        ymax = corners[i].y;
                }
                while (i++ < for_end);
        }
    }
    make_translation(&t2, -(integer) xmin * 72 / ((double)72.27), -(integer) ymin * 72 / ((double)72.27));
    transform_concat(&t, &t2);
    if (result == 0) {
        new_whatsit(43 /*pic_node */ ,
                    9 /*pic_node_size */  + (strlen(pic_path) + sizeof(memory_word) - 1) / sizeof(memory_word));
        if (is_pdf) {
            mem[cur_list.tail_field].hh.u.B1 = 44 /*pdf_node */ ;
        }
        mem[cur_list.tail_field + 4].hh.u.B0 = strlen(pic_path);
        mem[cur_list.tail_field + 4].hh.u.B1 = page;
        mem[cur_list.tail_field + 8].hh.u.B0 = pdf_box_type;
        mem[cur_list.tail_field + 1].cint = D2Fix(xmax - xmin);
        mem[cur_list.tail_field + 3].cint = D2Fix(ymax - ymin);
        mem[cur_list.tail_field + 2].cint = 0;
        mem[cur_list.tail_field + 5].hh.v.LH = D2Fix(t.a);
        mem[cur_list.tail_field + 5].hh.v.RH = D2Fix(t.b);
        mem[cur_list.tail_field + 6].hh.v.LH = D2Fix(t.c);
        mem[cur_list.tail_field + 6].hh.v.RH = D2Fix(t.d);
        mem[cur_list.tail_field + 7].hh.v.LH = D2Fix(t.x);
        mem[cur_list.tail_field + 7].hh.v.RH = D2Fix(t.y);
        memcpy(&mem[cur_list.tail_field + 9], pic_path, strlen(pic_path));
        free(pic_path);
    } else {

        {
            if (interaction == 3 /*error_stop_mode */ ) ;
            if (file_line_error_style_p)
                print_file_line();
            else
                print_nl(65544L /*"! " */ );
            print(66772L /*"Unable to load picture or PDF file '" */ );
        }
        print_file_name(cur_name, cur_area, cur_ext);
        print(39 /*"'" */ );
        if (result == -43) {
            {
                help_ptr = 2;
                help_line[1] = 66773L /*"The requested image couldn't be read because" */ ;
                help_line[0] = 66774L /*"the file was not found." */ ;
            }
        } else {

            {
                help_ptr = 2;
                help_line[1] = 66773L /*"The requested image couldn't be read because" */ ;
                help_line[0] = 66775L /*"it was not a recognized image format." */ ;
            }
        }
        error();
    }
}

void scan_and_pack_name(void)
{
    scan_and_pack_name_regmem scan_file_name();
    pack_file_name(cur_name, cur_area, cur_ext);
}

void do_extension(void)
{
    do_extension_regmem integer i, j, k;
    halfword p;
    switch (cur_chr) {
    case 0:
        {
            new_write_whatsit(3 /*open_node_size */ );
            scan_optional_equals();
            scan_file_name();
            mem[cur_list.tail_field + 1].hh.v.RH = cur_name;
            mem[cur_list.tail_field + 2].hh.v.LH = cur_area;
            mem[cur_list.tail_field + 2].hh.v.RH = cur_ext;
        }
        break;
    case 1:
        {
            k = cur_cs;
            new_write_whatsit(2 /*write_node_size */ );
            cur_cs = k;
            p = scan_toks(false, false);
            mem[cur_list.tail_field + 1].hh.v.RH = def_ref;
        }
        break;
    case 2:
        {
            new_write_whatsit(2 /*write_node_size */ );
            mem[cur_list.tail_field + 1].hh.v.RH = -268435455L;
        }
        break;
    case 3:
        {
            new_whatsit(3 /*special_node */ , 2 /*write_node_size */ );
            mem[cur_list.tail_field + 1].hh.v.LH = -268435455L;
            p = scan_toks(false, true);
            mem[cur_list.tail_field + 1].hh.v.RH = def_ref;
        }
        break;
    case 4:
        {
            get_x_token();
            if ((cur_cmd == 59 /*extension */ ) && (cur_chr <= 2 /*close_node */ )) {
                p = cur_list.tail_field;
                do_extension();
                out_what(cur_list.tail_field);
                flush_node_list(cur_list.tail_field);
                cur_list.tail_field = p;
                mem[p].hh.v.RH = -268435455L;
            } else
                back_input();
        }
        break;
    case 5:
        if (abs(cur_list.mode_field) != 104 /*hmode */ )
            report_illegal_case();
        else {

            new_whatsit(4 /*language_node */ , 2 /*small_node_size */ );
            scan_int();
            if (cur_val <= 0)
                cur_list.aux_field.hh.v.RH = 0;
            else if (cur_val > 255)
                cur_list.aux_field.hh.v.RH = 0;
            else
                cur_list.aux_field.hh.v.RH = cur_val;
            mem[cur_list.tail_field + 1].hh.v.RH = cur_list.aux_field.hh.v.RH;
            mem[cur_list.tail_field + 1].hh.u.B0 = norm_min(eqtb[8938791L /*int_base 51 */ ].cint);
            mem[cur_list.tail_field + 1].hh.u.B1 = norm_min(eqtb[8938792L /*int_base 52 */ ].cint);
        }
        break;
    case 41:
        if (abs(cur_list.mode_field) == 207 /*mmode */ )
            report_illegal_case();
        else
            load_picture(false);
        break;
    case 42:
        if (abs(cur_list.mode_field) == 207 /*mmode */ )
            report_illegal_case();
        else
            load_picture(true);
        break;
    case 43:
        {
            if (abs(cur_list.mode_field) == 1 /*vmode */ ) {
                back_input();
                new_graf(true);
            } else if (abs(cur_list.mode_field) == 207 /*mmode */ )
                report_illegal_case();
            else {

                if (((font_area[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH] == 65535L /*aat_font_flag */ )
                     || (font_area[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH] == 65534L /*otgr_font_flag */ ))) {
                    new_whatsit(42 /*glyph_node */ , 5 /*glyph_node_size */ );
                    scan_int();
                    if ((cur_val < 0) || (cur_val > 65535L)) {
                        {
                            if (interaction == 3 /*error_stop_mode */ ) ;
                            if (file_line_error_style_p)
                                print_file_line();
                            else
                                print_nl(65544L /*"! " */ );
                            print(66757L /*"Bad glyph number" */ );
                        }
                        {
                            help_ptr = 2;
                            help_line[1] = 66758L /*"A glyph number must be between 0 and 65535." */ ;
                            help_line[0] = 65995L /*"I changed this one to zero." */ ;
                        }
                        int_error(cur_val);
                        cur_val = 0;
                    }
                    mem[cur_list.tail_field + 4].qqqq.u.B1 = eqtb[2253299L /*cur_font_loc */ ].hh.v.RH;
                    mem[cur_list.tail_field + 4].qqqq.u.B2 = cur_val;
                    set_native_glyph_metrics(cur_list.tail_field, (eqtb[8938814L /*eTeX_state_base 3 */ ].cint > 0));
                } else
                    not_native_font_error(59 /*extension */ , 43 /*glyph_code */ ,
                                          eqtb[2253299L /*cur_font_loc */ ].hh.v.RH);
            }
        }
        break;
    case 44:
        {
            scan_and_pack_name();
            i = get_encoding_mode_and_info(&j);
            if (i == 0 /*XeTeX_input_mode_auto */ ) {
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66776L /*"Encoding mode `auto' is not valid for \XeTeXinputencoding" */ );
                }
                {
                    help_ptr = 2;
                    help_line[1] = 66777L /*"You can't use `auto' encoding here, only for \XeTeXdefaultencoding." */ ;
                    help_line[0] = 66778L /*"I'll ignore this and leave the current encoding unchanged." */ ;
                }
                error();
            } else
                set_input_file_encoding(input_file[in_open], i, j);
        }
        break;
    case 45:
        {
            scan_and_pack_name();
            i = get_encoding_mode_and_info(&j);
            eqtb[8938817L /*eTeX_state_base 6 */ ].cint = i;
            eqtb[8938818L /*eTeX_state_base 7 */ ].cint = j;
        }
        break;
    case 46:
        {
            scan_file_name();
            if (length(cur_name) == 0)
                eqtb[8938808L /*int_base 68 */ ].cint = 0;
            else
                eqtb[8938808L /*int_base 68 */ ].cint = cur_name;
        }
        break;
    case 6:
        {
            new_whatsit(6 /*pdftex_first_extension_code 0 */ , 2 /*small_node_size */ );
        }
        break;
    default:
        confusion(66725L /*"ext1" */ );
        break;
    }
}

void fix_language(void)
{
    fix_language_regmem UTF16_code l;
    if (eqtb[8938790L /*int_base 50 */ ].cint <= 0)
        l = 0;
    else if (eqtb[8938790L /*int_base 50 */ ].cint > 255)
        l = 0;
    else
        l = eqtb[8938790L /*int_base 50 */ ].cint;
    if (l != cur_list.aux_field.hh.v.RH) {
        new_whatsit(4 /*language_node */ , 2 /*small_node_size */ );
        mem[cur_list.tail_field + 1].hh.v.RH = l;
        cur_list.aux_field.hh.v.RH = l;
        mem[cur_list.tail_field + 1].hh.u.B0 = norm_min(eqtb[8938791L /*int_base 51 */ ].cint);
        mem[cur_list.tail_field + 1].hh.u.B1 = norm_min(eqtb[8938792L /*int_base 52 */ ].cint);
    }
}

void insert_src_special(void)
{
    insert_src_special_regmem halfword toklist, p, q;
    if ((source_filename_stack[in_open] > 0 && is_new_source(source_filename_stack[in_open], line))) {
        toklist = get_avail();
        p = toklist;
        mem[p].hh.v.LH = 35797667L /*cs_token_flag 2243236 */ ;
        mem[p].hh.v.RH = get_avail();
        p = mem[p].hh.v.RH;
        mem[p].hh.v.LH = 2097275L /*left_brace_token 123 */ ;
        q = str_toks(make_src_special(source_filename_stack[in_open], line));
        mem[p].hh.v.RH = mem[mem_top - 3].hh.v.RH;
        p = q;
        mem[p].hh.v.RH = get_avail();
        p = mem[p].hh.v.RH;
        mem[p].hh.v.LH = 4194429L /*right_brace_token 125 */ ;
        begin_token_list(toklist, 5 /*inserted */ );
        remember_source_info(source_filename_stack[in_open], line);
    }
}

void append_src_special(void)
{
    register memory_word *mem = zmem;

    if ((source_filename_stack[in_open] > 0 && is_new_source(source_filename_stack[in_open], line))) {
        new_whatsit(3 /*special_node */ , 2 /*write_node_size */ );
        mem[cur_list.tail_field + 1].hh.v.LH = 0;
        def_ref = get_avail();
        mem[def_ref].hh.v.LH = -268435455L;
        str_toks(make_src_special(source_filename_stack[in_open], line));
        mem[def_ref].hh.v.RH = mem[mem_top - 3].hh.v.RH;
        mem[cur_list.tail_field + 1].hh.v.RH = def_ref;
        remember_source_info(source_filename_stack[in_open], line);
    }
}

void handle_right_brace(void)
{
    handle_right_brace_regmem halfword p, q;
    scaled d;
    integer f;
    switch (cur_group) {
    case 1:
        unsave();
        break;
    case 0:
        {
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(66440L /*"Too many _'s" */ );
            }
            {
                help_ptr = 2;
                help_line[1] = 66441L /*"You've closed more groups than you opened." */ ;
                help_line[0] = 66442L /*"Such booboos are generally harmless, so keep going." */ ;
            }
            error();
        }
        break;
    case 14:
    case 15:
    case 16:
        extra_right_brace();
        break;
    case 2:
        package(0);
        break;
    case 3:
        {
            adjust_tail = mem_top - 5;
            pre_adjust_tail = mem_top - 14;
            package(0);
        }
        break;
    case 4:
        {
            end_graf();
            package(0);
        }
        break;
    case 5:
        {
            end_graf();
            package(4 /*vtop_code */ );
        }
        break;
    case 11:
        {
            end_graf();
            q = eqtb[2252250L /*glue_base 10 */ ].hh.v.RH;
            mem[q].hh.v.RH++;
            d = eqtb[10053198L /*dimen_base 6 */ ].cint;
            f = eqtb[8938782L /*int_base 42 */ ].cint;
            unsave();
            save_ptr = save_ptr - 2;
            p = vpackage(mem[cur_list.head_field].hh.v.RH, 0, 1 /*additional */ , 1073741823L);
            pop_nest();
            if (save_stack[save_ptr + 0].cint < 255) {
                {
                    mem[cur_list.tail_field].hh.v.RH = get_node(5 /*ins_node_size */ );
                    cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
                }
                mem[cur_list.tail_field].hh.u.B0 = 3 /*ins_node */ ;
                mem[cur_list.tail_field].hh.u.B1 = save_stack[save_ptr + 0].cint;
                mem[cur_list.tail_field + 3].cint = mem[p + 3].cint + mem[p + 2].cint;
                mem[cur_list.tail_field + 4].hh.v.LH = mem[p + 5].hh.v.RH;
                mem[cur_list.tail_field + 4].hh.v.RH = q;
                mem[cur_list.tail_field + 2].cint = d;
                mem[cur_list.tail_field + 1].cint = f;
            } else {

                {
                    mem[cur_list.tail_field].hh.v.RH = get_node(2 /*small_node_size */ );
                    cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
                }
                mem[cur_list.tail_field].hh.u.B0 = 5 /*adjust_node */ ;
                mem[cur_list.tail_field].hh.u.B1 = save_stack[save_ptr + 1].cint;
                mem[cur_list.tail_field + 1].cint = mem[p + 5].hh.v.RH;
                delete_glue_ref(q);
            }
            free_node(p, 8 /*box_node_size */ );
            if (nest_ptr == 0)
                build_page();
        }
        break;
    case 8:
        {
            if ((cur_input.loc_field != -268435455L) || ((cur_input.index_field != 7 /*output_text */ ) && (cur_input.index_field != 3 /*backed_up */ ))) {     /*1062: */
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66408L /*"Unbalanced output routine" */ );
                }
                {
                    help_ptr = 2;
                    help_line[1] = 66409L /*"Your sneaky output routine has problematic _'s and/or _'s." */ ;
                    help_line[0] = 66410L /*"I can't handle that very well; good luck." */ ;
                }
                error();
                do {
                    get_token();
                } while (!(cur_input.loc_field == -268435455L));
            }
            end_token_list();
            end_graf();
            unsave();
            output_active = false;
            insert_penalties = 0;
            if (eqtb[2253298L /*box_base 255 */ ].hh.v.RH != -268435455L) {
                {
                    if (interaction == 3 /*error_stop_mode */ ) ;
                    if (file_line_error_style_p)
                        print_file_line();
                    else
                        print_nl(65544L /*"! " */ );
                    print(66411L /*"Output routine didn't use all of " */ );
                }
                print_esc(65705L /*"box" */ );
                print_int(255);
                {
                    help_ptr = 3;
                    help_line[2] = 66412L /*"Your \output commands should empty \box255," */ ;
                    help_line[1] = 66413L /*"e.g., by saying `\shipout\box255'." */ ;
                    help_line[0] = 66414L /*"Proceed; I'll discard its present contents." */ ;
                }
                box_error(255);
            }
            if (cur_list.tail_field != cur_list.head_field) {
                mem[page_tail].hh.v.RH = mem[cur_list.head_field].hh.v.RH;
                page_tail = cur_list.tail_field;
            }
            if (mem[mem_top - 2].hh.v.RH != -268435455L) {
                if (mem[mem_top - 1].hh.v.RH == -268435455L)
                    nest[0].tail_field = page_tail;
                mem[page_tail].hh.v.RH = mem[mem_top - 1].hh.v.RH;
                mem[mem_top - 1].hh.v.RH = mem[mem_top - 2].hh.v.RH;
                mem[mem_top - 2].hh.v.RH = -268435455L;
                page_tail = mem_top - 2;
            }
            flush_node_list(disc_ptr[2 /*last_box_code */ ]);
            disc_ptr[2 /*last_box_code */ ] = -268435455L;
            pop_nest();
            build_page();
        }
        break;
    case 10:
        build_discretionary();
        break;
    case 6:
        {
            back_input();
            cur_tok = 35797658L /*cs_token_flag 2243227 */ ;
            {
                if (interaction == 3 /*error_stop_mode */ ) ;
                if (file_line_error_style_p)
                    print_file_line();
                else
                    print_nl(65544L /*"! " */ );
                print(65948L /*"Missing " */ );
            }
            print_esc(66289L /*"cr" */ );
            print(65949L /*" inserted" */ );
            {
                help_ptr = 1;
                help_line[0] = 66523L /*"I'm guessing that you meant to end an alignment here." */ ;
            }
            ins_error();
        }
        break;
    case 7:
        {
            end_graf();
            unsave();
            align_peek();
        }
        break;
    case 12:
        {
            end_graf();
            unsave();
            save_ptr = save_ptr - 2;
            p = vpackage(mem[cur_list.head_field].hh.v.RH, save_stack[save_ptr + 1].cint, save_stack[save_ptr + 0].cint,
                         1073741823L);
            pop_nest();
            {
                mem[cur_list.tail_field].hh.v.RH = new_noad();
                cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
            }
            mem[cur_list.tail_field].hh.u.B0 = 29 /*vcenter_noad */ ;
            mem[cur_list.tail_field + 1].hh.v.RH = 2 /*sub_box */ ;
            mem[cur_list.tail_field + 1].hh.v.LH = p;
        }
        break;
    case 13:
        build_choices();
        break;
    case 9:
        {
            unsave();
            save_ptr--;
            mem[save_stack[save_ptr + 0].cint].hh.v.RH = 3 /*sub_mlist */ ;
            p = fin_mlist(-268435455L);
            mem[save_stack[save_ptr + 0].cint].hh.v.LH = p;
            if (p != -268435455L) {

                if (mem[p].hh.v.RH == -268435455L) {

                    if (mem[p].hh.u.B0 == 16 /*ord_noad */ ) {
                        if (mem[p + 3].hh.v.RH == 0 /*empty */ ) {

                            if (mem[p + 2].hh.v.RH == 0 /*empty */ ) {
                                mem[save_stack[save_ptr + 0].cint].hh = mem[p + 1].hh;
                                free_node(p, 4 /*noad_size */ );
                            }
                        }
                    } else if (mem[p].hh.u.B0 == 28 /*accent_noad */ ) {

                        if (save_stack[save_ptr + 0].cint == cur_list.tail_field + 1) {

                            if (mem[cur_list.tail_field].hh.u.B0 == 16 /*ord_noad */ ) {  /*1222: */
                                q = cur_list.head_field;
                                while (mem[q].hh.v.RH != cur_list.tail_field)
                                    q = mem[q].hh.v.RH;
                                mem[q].hh.v.RH = p;
                                free_node(cur_list.tail_field, 4 /*noad_size */ );
                                cur_list.tail_field = p;
                            }
                        }
                    }
                }
            }
        }
        break;
    default:
        confusion(66443L /*"rightbrace" */ );
        break;
    }
}

void main_control(void)
{
    main_control_regmem integer t;
    if (eqtb[2252778L /*every_job_loc */ ].hh.v.RH != -268435455L)
        begin_token_list(eqtb[2252778L /*every_job_loc */ ].hh.v.RH, 13 /*every_job_text */ );
 lab60:                        /*big_switch */ get_x_token();
 lab21:                        /*reswitch *//*1066: */ if (interrupt != 0) {

        if (OK_to_interrupt) {
            back_input();
            {
                if (interrupt != 0)
                    pause_for_instructions();
            }
            goto lab60;
        }
    };

    if (eqtb[8938776L /*int_base 36 */ ].cint > 0)
        show_cur_cmd_chr();
    switch (abs(cur_list.mode_field) + cur_cmd) {
    case 115:
    case 116:
    case 172:
        goto lab70;
        break;
    case 120:
        {
            scan_usv_num();
            cur_chr = cur_val;
            goto lab70;
        }
        break;
    case 169:
        {
            get_x_token();
            if ((cur_cmd == 11 /*letter */ ) || (cur_cmd == 12 /*other_char */ ) || (cur_cmd == 68 /*char_given */ )
                || (cur_cmd == 16 /*char_num */ ))
                cancel_boundary = true;
            goto lab21;
        }
        break;
    default:
        {
            if (abs(cur_list.mode_field) == 104 /*hmode */ ) {

                if ((eqtb[8938815L /*eTeX_state_base 4 */ ].cint > 0) && (space_class != 4096 /*char_class_limit */ )
                    && (prev_class != (4095 /*char_class_limit -1 */ ))) {
                    prev_class = (4095 /*char_class_limit -1 */ );
                    find_sa_element(6 /*inter_char_val */ ,
                                    space_class * 4096 /*char_class_limit */  + (4095 /*char_class_limit -1 */ ),
                                    false);
                    if (cur_ptr != -268435455L) {
                        if (cur_cs == 0) {
                            if (cur_cmd == 16 /*char_num */ )
                                cur_cmd = 12 /*other_char */ ;
                            cur_tok = (cur_cmd * 2097152L /*max_char_val */ ) + cur_chr;
                        } else
                            cur_tok = 33554431L /*cs_token_flag */  + cur_cs;
                        back_input();
                        begin_token_list(mem[cur_ptr + 1].hh.v.RH, 17 /*inter_char_text */ );
                        goto lab60;
                    }
                }
            }
            switch (abs(cur_list.mode_field) + cur_cmd) {
            case 114:
                if (cur_list.aux_field.hh.v.LH == 1000)
                    goto lab120;
                else
                    app_space();
                break;
            case 168:
            case 271:
                goto lab120;
                break;
            case 1:
            case 104:
            case 207:
            case 11:
            case 217:
            case 272:
                ;
                break;
            case 40:
            case 143:
            case 246:
                {
                    if (cur_chr == 0) {
                        do {
                            get_x_token();
                        } while (!(cur_cmd != 10 /*spacer *//*:424 */ ));
                        goto lab21;
                    } else {

                        t = scanner_status;
                        scanner_status = 0 /*normal */ ;
                        get_next();
                        scanner_status = t;
                        if (cur_cs < 2228226L /*hash_base */ )
                            cur_cs = prim_lookup(cur_cs - 257);
                        else
                            cur_cs = prim_lookup(hash[cur_cs].v.RH);
                        if (cur_cs != 0 /*undefined_primitive */ ) {
                            cur_cmd = prim_eqtb[cur_cs].hh.u.B0;
                            cur_chr = prim_eqtb[cur_cs].hh.v.RH;
                            goto lab21;
                        }
                    }
                }
                break;
            case 15:
                if (its_all_over())
                    return;
                break;
            case 23:
            case 125:
            case 228:
            case 72:
            case 175:
            case 278:
            case 39:
            case 45:
            case 49:
            case 152:
            case 7:
            case 110:
            case 213:
                report_illegal_case();
                break;
            case 8:
            case 111:
            case 9:
            case 112:
            case 18:
            case 121:
            case 70:
            case 173:
            case 71:
            case 174:
            case 51:
            case 154:
            case 16:
            case 119:
            case 50:
            case 153:
            case 53:
            case 156:
            case 67:
            case 170:
            case 54:
            case 157:
            case 55:
            case 158:
            case 57:
            case 160:
            case 56:
            case 159:
            case 31:
            case 134:
            case 52:
            case 155:
            case 29:
            case 132:
            case 47:
            case 150:
            case 216:
            case 220:
            case 221:
            case 234:
            case 231:
            case 240:
            case 243:
                insert_dollar_sign();
                break;
            case 37:
            case 139:
            case 242:
                {
                    {
                        mem[cur_list.tail_field].hh.v.RH = scan_rule_spec();
                        cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
                    }
                    if (abs(cur_list.mode_field) == 1 /*vmode */ )
                        cur_list.aux_field.cint = -65536000L;
                    else if (abs(cur_list.mode_field) == 104 /*hmode */ )
                        cur_list.aux_field.hh.v.LH = 1000;
                }
                break;
            case 28:
            case 130:
            case 233:
            case 235:
                append_glue();
                break;
            case 30:
            case 133:
            case 236:
            case 237:
                append_kern();
                break;
            case 2:
            case 105:
                new_save_level(1 /*simple_group */ );
                break;
            case 62:
            case 165:
            case 268:
                new_save_level(14 /*semi_simple_group */ );
                break;
            case 63:
            case 166:
            case 269:
                if (cur_group == 14 /*semi_simple_group */ )
                    unsave();
                else
                    off_save();
                break;
            case 3:
            case 106:
            case 209:
                handle_right_brace();
                break;
            case 22:
            case 126:
            case 229:
                {
                    t = cur_chr;
                    scan_dimen(false, false, false);
                    if (t == 0)
                        scan_box(cur_val);
                    else
                        scan_box(-(integer) cur_val);
                }
                break;
            case 32:
            case 135:
            case 238:
                scan_box(1073807261L + cur_chr);
                break;
            case 21:
            case 124:
            case 227:
                begin_box(0);
                break;
            case 44:
                new_graf(cur_chr > 0);
                break;
            case 12:
            case 13:
            case 17:
            case 69:
            case 4:
            case 24:
            case 36:
            case 46:
            case 48:
            case 27:
            case 34:
            case 65:
            case 66:
                {
                    back_input();
                    new_graf(true);
                }
                break;
            case 147:
            case 250:
                indent_in_hmode();
                break;
            case 14:
                {
                    normal_paragraph();
                    if (cur_list.mode_field > 0)
                        build_page();
                }
                break;
            case 117:
                {
                    if (align_state < 0)
                        off_save();
                    end_graf();
                    if (cur_list.mode_field == 1 /*vmode */ )
                        build_page();
                }
                break;
            case 118:
            case 131:
            case 140:
            case 128:
            case 136:
                head_for_vmode();
                break;
            case 38:
            case 141:
            case 244:
            case 142:
            case 245:
                begin_insert_or_adjust();
                break;
            case 19:
            case 122:
            case 225:
                make_mark();
                break;
            case 43:
            case 146:
            case 249:
                append_penalty();
                break;
            case 26:
            case 129:
            case 232:
                delete_last();
                break;
            case 25:
            case 127:
            case 230:
                unpackage();
                break;
            case 148:
                append_italic_correction();
                break;
            case 251:
                {
                    mem[cur_list.tail_field].hh.v.RH = new_kern(0);
                    cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
                }
                break;
            case 151:
            case 254:
                append_discretionary();
                break;
            case 149:
                make_accent();
                break;
            case 6:
            case 109:
            case 212:
            case 5:
            case 108:
            case 211:
                align_error();
                break;
            case 35:
            case 138:
            case 241:
                no_align_error();
                break;
            case 64:
            case 167:
            case 270:
                omit_error();
                break;
            case 33:
                init_align();
                break;
            case 137:
                if (cur_chr > 0) {
                    if (eTeX_enabled((eqtb[8938811L /*eTeX_state_base 0 */ ].cint > 0), cur_cmd, cur_chr)) {
                        mem[cur_list.tail_field].hh.v.RH = new_math(0, cur_chr);
                        cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
                    }
                } else          /*:1490 */
                    init_align();
                break;
            case 239:
                if (privileged()) {

                    if (cur_group == 15 /*math_shift_group */ )
                        init_align();
                    else
                        off_save();
                }
                break;
            case 10:
            case 113:
                do_endv();
                break;
            case 68:
            case 171:
            case 274:
                cs_error();
                break;
            case 107:
                init_math();
                break;
            case 255:
                if (privileged()) {

                    if (cur_group == 15 /*math_shift_group */ )
                        start_eq_no();
                    else
                        off_save();
                }
                break;
            case 208:
                {
                    {
                        mem[cur_list.tail_field].hh.v.RH = new_noad();
                        cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
                    }
                    back_input();
                    scan_math(cur_list.tail_field + 1);
                }
                break;
            case 218:
            case 219:
            case 275:
                set_math_char(eqtb[6710516L /*math_code_base */  + cur_chr].hh.v.RH);
                break;
            case 223:
                {
                    scan_char_num();
                    cur_chr = cur_val;
                    set_math_char(eqtb[6710516L /*math_code_base */  + cur_chr].hh.v.RH);
                }
                break;
            case 224:
                if (cur_chr == 2) {
                    scan_math_class_int();
                    t = set_class_field(cur_val);
                    scan_math_fam_int();
                    t = t + set_family_field(cur_val);
                    scan_usv_num();
                    t = t + cur_val;
                    set_math_char(t);
                } else if (cur_chr == 1) {
                    scan_xetex_math_char_int();
                    set_math_char(cur_val);
                } else {

                    scan_fifteen_bit_int();
                    set_math_char(set_class_field(cur_val / 4096) + set_family_field((cur_val % 4096) / 256) +
                                  (cur_val % 256));
                }
                break;
            case 276:
                {
                    set_math_char(set_class_field(cur_chr / 4096) + set_family_field((cur_chr % 4096) / 256) +
                                  (cur_chr % 256));
                }
                break;
            case 277:
                set_math_char(cur_chr);
                break;
            case 222:
                {
                    if (cur_chr == 1) {
                        scan_math_class_int();
                        t = set_class_field(cur_val);
                        scan_math_fam_int();
                        t = t + set_family_field(cur_val);
                        scan_usv_num();
                        t = t + cur_val;
                        set_math_char(t);
                    } else {

                        scan_delimiter_int();
                        cur_val = cur_val / 4096;
                        set_math_char(set_class_field(cur_val / 4096) + set_family_field((cur_val % 4096) / 256) +
                                      (cur_val % 256));
                    }
                }
                break;
            case 257:
                {
                    {
                        mem[cur_list.tail_field].hh.v.RH = new_noad();
                        cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
                    }
                    mem[cur_list.tail_field].hh.u.B0 = cur_chr;
                    scan_math(cur_list.tail_field + 1);
                }
                break;
            case 258:
                math_limit_switch();
                break;
            case 273:
                math_radical();
                break;
            case 252:
            case 253:
                math_ac();
                break;
            case 263:
                {
                    scan_spec(12 /*vcenter_group */ , false);
                    normal_paragraph();
                    push_nest();
                    cur_list.mode_field = -1;
                    cur_list.aux_field.cint = -65536000L;
                    if ((insert_src_special_every_vbox))
                        insert_src_special();
                    if (eqtb[2252777L /*every_vbox_loc */ ].hh.v.RH != -268435455L)
                        begin_token_list(eqtb[2252777L /*every_vbox_loc */ ].hh.v.RH, 12 /*every_vbox_text */ );
                }
                break;
            case 260:
                {
                    mem[cur_list.tail_field].hh.v.RH = new_style(cur_chr);
                    cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
                }
                break;
            case 262:
                {
                    {
                        mem[cur_list.tail_field].hh.v.RH = new_glue(mem_bot);
                        cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
                    }
                    mem[cur_list.tail_field].hh.u.B1 = 98 /*cond_math_glue */ ;
                }
                break;
            case 261:
                append_choices();
                break;
            case 215:
            case 214:
                sub_sup();
                break;
            case 259:
                math_fraction();
                break;
            case 256:
                math_left_right();
                break;
            case 210:
                if (cur_group == 15 /*math_shift_group */ )
                    after_math();
                else
                    off_save();
                break;
            case 73:
            case 176:
            case 279:
            case 74:
            case 177:
            case 280:
            case 75:
            case 178:
            case 281:
            case 76:
            case 179:
            case 282:
            case 77:
            case 180:
            case 283:
            case 78:
            case 181:
            case 284:
            case 79:
            case 182:
            case 285:
            case 80:
            case 183:
            case 286:
            case 81:
            case 184:
            case 287:
            case 82:
            case 185:
            case 288:
            case 83:
            case 186:
            case 289:
            case 84:
            case 187:
            case 290:
            case 85:
            case 188:
            case 291:
            case 86:
            case 189:
            case 292:
            case 87:
            case 190:
            case 293:
            case 88:
            case 191:
            case 294:
            case 89:
            case 192:
            case 295:
            case 90:
            case 193:
            case 296:
            case 91:
            case 194:
            case 297:
            case 92:
            case 195:
            case 298:
            case 93:
            case 196:
            case 299:
            case 94:
            case 197:
            case 300:
            case 95:
            case 198:
            case 301:
            case 96:
            case 199:
            case 302:
            case 97:
            case 200:
            case 303:
            case 98:
            case 201:
            case 304:
            case 99:
            case 202:
            case 305:
            case 100:
            case 203:
            case 306:
            case 101:
            case 204:
            case 307:
            case 102:
            case 205:
            case 308:
            case 103:
            case 206:
            case 309:
                prefixed_command();
                break;
            case 41:
            case 144:
            case 247:
                {
                    get_token();
                    after_token = cur_tok;
                }
                break;
            case 42:
            case 145:
            case 248:
                {
                    get_token();
                    save_for_after(cur_tok);
                }
                break;
            case 61:
            case 164:
            case 267:
                open_or_close_in();
                break;
            case 59:
            case 162:
            case 265:
                issue_message();
                break;
            case 58:
            case 161:
            case 264:
                shift_case();
                break;
            case 20:
            case 123:
            case 226:
                show_whatever();
                break;
            case 60:
            case 163:
            case 266:
                do_extension();
                break;
            }
        }
        break;
    }
    goto lab60;
 lab70:                        /*main_loop *//*1069: */ if (((cur_list.head_field == cur_list.tail_field) && (cur_list.mode_field > 0))) {
        if ((insert_src_special_auto))
            append_src_special();
    }
    prev_class = (4095 /*char_class_limit -1 */ );
    if (((font_area[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH] == 65535L /*aat_font_flag */ )
         || (font_area[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH] == 65534L /*otgr_font_flag */ ))) {
        if (cur_list.mode_field > 0) {

            if (eqtb[8938790L /*int_base 50 */ ].cint != cur_list.aux_field.hh.v.RH)
                fix_language();
        }
        main_h = 0;
        main_f = eqtb[2253299L /*cur_font_loc */ ].hh.v.RH;
        native_len = 0;
 lab71:/*collect_native */ main_s = eqtb[5596404L /*sf_code_base */  + cur_chr].hh.v.RH % 65536L;
        if (main_s == 1000)
            cur_list.aux_field.hh.v.LH = 1000;
        else if (main_s < 1000) {
            if (main_s > 0)
                cur_list.aux_field.hh.v.LH = main_s;
        } else if (cur_list.aux_field.hh.v.LH < 1000)
            cur_list.aux_field.hh.v.LH = 1000;
        else
            cur_list.aux_field.hh.v.LH = main_s;
        cur_ptr = -268435455L;
        space_class = eqtb[5596404L /*sf_code_base */  + cur_chr].hh.v.RH / 65536L;
        if ((eqtb[8938815L /*eTeX_state_base 4 */ ].cint > 0) && space_class != 4096 /*char_class_limit */ ) {
            if (prev_class == (4095 /*char_class_limit -1 */ )) {
                if ((cur_input.state_field != 0 /*token_list */ ) || (cur_input.index_field != 4 /*backed_up_char */ )) {
                    find_sa_element(6 /*inter_char_val */ ,
                                    (4095 /*char_class_limit -1 */ ) * 4096 /*char_class_limit */  + space_class,
                                    false);
                    if (cur_ptr != -268435455L) {
                        if (cur_cmd != 11 /*letter */ )
                            cur_cmd = 12 /*other_char */ ;
                        cur_tok = (cur_cmd * 2097152L /*max_char_val */ ) + cur_chr;
                        back_input();
                        cur_input.index_field = 4 /*backed_up_char */ ;
                        begin_token_list(mem[cur_ptr + 1].hh.v.RH, 17 /*inter_char_text */ );
                        goto lab60;
                    }
                }
            } else {

                find_sa_element(6 /*inter_char_val */ , prev_class * 4096 /*char_class_limit */  + space_class, false);
                if (cur_ptr != -268435455L) {
                    if (cur_cmd != 11 /*letter */ )
                        cur_cmd = 12 /*other_char */ ;
                    cur_tok = (cur_cmd * 2097152L /*max_char_val */ ) + cur_chr;
                    back_input();
                    cur_input.index_field = 4 /*backed_up_char */ ;
                    begin_token_list(mem[cur_ptr + 1].hh.v.RH, 17 /*inter_char_text */ );
                    prev_class = (4095 /*char_class_limit -1 */ );
                    goto lab72;
                }
            }
            prev_class = space_class;
        }
        if ((cur_chr > 65535L)) {
            while (native_text_size <= native_len + 2) {

                native_text_size = native_text_size + 128;
                native_text = xrealloc(native_text, native_text_size * sizeof(UTF16_code));
            }
            {
                native_text[native_len] = (cur_chr - 65536L) / 1024 + 55296L;
                native_len++;
            }
            {
                native_text[native_len] = (cur_chr - 65536L) % 1024 + 56320L;
                native_len++;
            }
        } else {

            while (native_text_size <= native_len + 1) {

                native_text_size = native_text_size + 128;
                native_text = xrealloc(native_text, native_text_size * sizeof(UTF16_code));
            }
            {
                native_text[native_len] = cur_chr;
                native_len++;
            }
        }
        is_hyph = (cur_chr == hyphen_char[main_f]) || ((eqtb[8938812L /*eTeX_state_base 1 */ ].cint > 0)
                                                       && ((cur_chr == 8212) || (cur_chr == 8211)));
        if ((main_h == 0) && is_hyph)
            main_h = native_len;
        get_next();
        if ((cur_cmd == 11 /*letter */ ) || (cur_cmd == 12 /*other_char */ ) || (cur_cmd == 68 /*char_given */ ))
            goto lab71;
        x_token();
        if ((cur_cmd == 11 /*letter */ ) || (cur_cmd == 12 /*other_char */ ) || (cur_cmd == 68 /*char_given */ ))
            goto lab71;
        if (cur_cmd == 16 /*char_num */ ) {
            scan_usv_num();
            cur_chr = cur_val;
            goto lab71;
        }
        if ((eqtb[8938815L /*eTeX_state_base 4 */ ].cint > 0) && (space_class != 4096 /*char_class_limit */ )
            && (prev_class != (4095 /*char_class_limit -1 */ ))) {
            prev_class = (4095 /*char_class_limit -1 */ );
            find_sa_element(6 /*inter_char_val */ ,
                            space_class * 4096 /*char_class_limit */  + (4095 /*char_class_limit -1 */ ), false);
            if (cur_ptr != -268435455L) {
                if (cur_cs == 0) {
                    if (cur_cmd == 16 /*char_num */ )
                        cur_cmd = 12 /*other_char */ ;
                    cur_tok = (cur_cmd * 2097152L /*max_char_val */ ) + cur_chr;
                } else
                    cur_tok = 33554431L /*cs_token_flag */  + cur_cs;
                back_input();
                begin_token_list(mem[cur_ptr + 1].hh.v.RH, 17 /*inter_char_text */ );
                goto lab72;
            }
        }
 lab72:                        /*collected */ if ((font_mapping[main_f] != 0)) {
            main_k = apply_mapping(font_mapping[main_f], native_text, native_len);
            native_len = 0;
            while (native_text_size <= native_len + main_k) {

                native_text_size = native_text_size + 128;
                native_text = xrealloc(native_text, native_text_size * sizeof(UTF16_code));
            }
            main_h = 0;
            {
                register integer for_end;
                main_p = 0;
                for_end = main_k - 1;
                if (main_p <= for_end)
                    do {
                        {
                            native_text[native_len] = mapped_text[main_p];
                            native_len++;
                        }
                        if ((main_h == 0)
                            && ((mapped_text[main_p] == hyphen_char[main_f])
                                || ((eqtb[8938812L /*eTeX_state_base 1 */ ].cint > 0)
                                    && ((mapped_text[main_p] == 8212) || (mapped_text[main_p] == 8211)))))
                            main_h = native_len;
                    }
                    while (main_p++ < for_end);
            }
        }
        if (eqtb[8938775L /*int_base 35 */ ].cint > 0) {
            temp_ptr = 0;
            while ((temp_ptr < native_len)) {

                main_k = native_text[temp_ptr];
                temp_ptr++;
                if ((main_k >= 55296L) && (main_k < 56320L)) {
                    main_k = 65536L + (main_k - 55296L) * 1024;
                    main_k = main_k + native_text[temp_ptr] - 56320L;
                    temp_ptr++;
                }
                if (map_char_to_glyph(main_f, main_k) == 0)
                    char_warning(main_f, main_k);
            }
        }
        main_k = native_len;
        main_pp = cur_list.tail_field;
        if (cur_list.mode_field == 104 /*hmode */ ) {
            main_ppp = cur_list.head_field;
            if (main_ppp != main_pp)
                while ((mem[main_ppp].hh.v.RH != main_pp)) {

                    if ((!(main_ppp >= hi_mem_min)) && (mem[main_ppp == 7 /*disc_node */ ].hh.u.B0)) {
                        temp_ptr = main_ppp;
                        {
                            register integer for_end;
                            main_p = 1;
                            for_end = mem[temp_ptr].hh.u.B1;
                            if (main_p <= for_end)
                                do
                                    main_ppp = mem[main_ppp].hh.v.RH;
                                while (main_p++ < for_end);
                        }
                    }
                    if (main_ppp != main_pp)
                        main_ppp = mem[main_ppp].hh.v.RH;
                }
            temp_ptr = 0;
            do {
                if (main_h == 0)
                    main_h = main_k;
                if ((((main_pp) != -268435455L && (!(main_pp >= hi_mem_min))
                      && (mem[main_pp].hh.u.B0 == 8 /*whatsit_node */ )
                      && ((mem[main_pp].hh.u.B1 == 40 /*native_word_node */ )
                          || (mem[main_pp].hh.u.B1 == 41 /*native_word_node_AT */ ))))
                    && (mem[main_pp + 4].qqqq.u.B1 == main_f) && (main_ppp != main_pp) && (!(main_ppp >= hi_mem_min))
                    && (mem[main_ppp].hh.u.B0 != 7 /*disc_node */ )) {
                    main_k = main_h + mem[main_pp + 4].qqqq.u.B2;
                    while (native_text_size <= native_len + main_k) {

                        native_text_size = native_text_size + 128;
                        native_text = xrealloc(native_text, native_text_size * sizeof(UTF16_code));
                    }
                    save_native_len = native_len;
                    {
                        register integer for_end;
                        main_p = 0;
                        for_end = mem[main_pp + 4].qqqq.u.B2 - 1;
                        if (main_p <= for_end)
                            do {
                                native_text[native_len] = get_native_char(main_pp, main_p);
                                native_len++;
                            }
                            while (main_p++ < for_end);
                    }
                    {
                        register integer for_end;
                        main_p = 0;
                        for_end = main_h - 1;
                        if (main_p <= for_end)
                            do {
                                native_text[native_len] = native_text[temp_ptr + main_p];
                                native_len++;
                            }
                            while (main_p++ < for_end);
                    }
                    do_locale_linebreaks(save_native_len, main_k);
                    native_len = save_native_len;
                    main_k = native_len - main_h - temp_ptr;
                    temp_ptr = main_h;
                    main_h = 0;
                    while ((main_h < main_k) && (native_text[temp_ptr + main_h] != hyphen_char[main_f])
                           && ((!(eqtb[8938812L /*eTeX_state_base 1 */ ].cint > 0))
                               || ((native_text[temp_ptr + main_h] != 8212)
                                   && (native_text[temp_ptr + main_h] != 8211))))
                        main_h++;
                    if ((main_h < main_k))
                        main_h++;
                    mem[main_ppp].hh.v.RH = mem[main_pp].hh.v.RH;
                    mem[main_pp].hh.v.RH = -268435455L;
                    flush_node_list(main_pp);
                    main_pp = cur_list.tail_field;
                    while ((mem[main_ppp].hh.v.RH != main_pp))
                        main_ppp = mem[main_ppp].hh.v.RH;
                } else {

                    do_locale_linebreaks(temp_ptr, main_h);
                    temp_ptr = temp_ptr + main_h;
                    main_k = main_k - main_h;
                    main_h = 0;
                    while ((main_h < main_k) && (native_text[temp_ptr + main_h] != hyphen_char[main_f])
                           && ((!(eqtb[8938812L /*eTeX_state_base 1 */ ].cint > 0))
                               || ((native_text[temp_ptr + main_h] != 8212)
                                   && (native_text[temp_ptr + main_h] != 8211))))
                        main_h++;
                    if ((main_h < main_k))
                        main_h++;
                }
                if ((main_k > 0) || is_hyph) {
                    {
                        mem[cur_list.tail_field].hh.v.RH = new_disc();
                        cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
                    }
                    main_pp = cur_list.tail_field;
                }
            } while (!(main_k == 0));
        } else {

            main_ppp = cur_list.head_field;
            if (main_ppp != main_pp)
                while ((mem[main_ppp].hh.v.RH != main_pp)) {

                    if ((!(main_ppp >= hi_mem_min)) && (mem[main_ppp == 7 /*disc_node */ ].hh.u.B0)) {
                        temp_ptr = main_ppp;
                        {
                            register integer for_end;
                            main_p = 1;
                            for_end = mem[temp_ptr].hh.u.B1;
                            if (main_p <= for_end)
                                do
                                    main_ppp = mem[main_ppp].hh.v.RH;
                                while (main_p++ < for_end);
                        }
                    }
                    if (main_ppp != main_pp)
                        main_ppp = mem[main_ppp].hh.v.RH;
                }
            if ((((main_pp) != -268435455L && (!(main_pp >= hi_mem_min)) && (mem[main_pp].hh.u.B0 == 8 /*whatsit_node */ )
                  && ((mem[main_pp].hh.u.B1 == 40 /*native_word_node */ )
                      || (mem[main_pp].hh.u.B1 == 41 /*native_word_node_AT */ )))) && (mem[main_pp + 4].qqqq.u.B1 == main_f)
                && (main_ppp != main_pp) && (!(main_ppp >= hi_mem_min)) && (mem[main_ppp].hh.u.B0 != 7 /*disc_node */ )) {
                mem[main_pp].hh.v.RH = new_native_word_node(main_f, main_k + mem[main_pp + 4].qqqq.u.B2);
                cur_list.tail_field = mem[main_pp].hh.v.RH;
                {
                    register integer for_end;
                    main_p = 0;
                    for_end = mem[main_pp + 4].qqqq.u.B2 - 1;
                    if (main_p <= for_end)
                        do
                            set_native_char(cur_list.tail_field, main_p, get_native_char(main_pp, main_p));
                        while (main_p++ < for_end);
                }
                {
                    register integer for_end;
                    main_p = 0;
                    for_end = main_k - 1;
                    if (main_p <= for_end)
                        do
                            set_native_char(cur_list.tail_field, main_p + mem[main_pp + 4].qqqq.u.B2,
                                            native_text[main_p]);
                        while (main_p++ < for_end);
                }
                set_native_metrics(cur_list.tail_field, (eqtb[8938814L /*eTeX_state_base 3 */ ].cint > 0));
                main_p = cur_list.head_field;
                if (main_p != main_pp)
                    while (mem[main_p].hh.v.RH != main_pp)
                        main_p = mem[main_p].hh.v.RH;
                mem[main_p].hh.v.RH = mem[main_pp].hh.v.RH;
                mem[main_pp].hh.v.RH = -268435455L;
                flush_node_list(main_pp);
            } else {

                mem[main_pp].hh.v.RH = new_native_word_node(main_f, main_k);
                cur_list.tail_field = mem[main_pp].hh.v.RH;
                {
                    register integer for_end;
                    main_p = 0;
                    for_end = main_k - 1;
                    if (main_p <= for_end)
                        do
                            set_native_char(cur_list.tail_field, main_p, native_text[main_p]);
                        while (main_p++ < for_end);
                }
                set_native_metrics(cur_list.tail_field, (eqtb[8938814L /*eTeX_state_base 3 */ ].cint > 0));
            }
        }
        if (eqtb[8938820L /*eTeX_state_base 9 */ ].cint > 0) {
            main_p = cur_list.head_field;
            main_pp = -268435455L;
            while (main_p != cur_list.tail_field) {

                if ((((main_p) != -268435455L && (!(main_p >= hi_mem_min))
                      && (mem[main_p].hh.u.B0 == 8 /*whatsit_node */ )
                      && ((mem[main_p].hh.u.B1 == 40 /*native_word_node */ )
                          || (mem[main_p].hh.u.B1 == 41 /*native_word_node_AT */ )))))
                    main_pp = main_p;
                main_p = mem[main_p].hh.v.RH;
            }
            if ((main_pp != -268435455L)) {
                if ((mem[main_pp + 4].qqqq.u.B1 == main_f)) {
                    main_p = mem[main_pp].hh.v.RH;
                    while (!(main_p >= hi_mem_min)
                           && ((mem[main_p].hh.u.B0 == 12 /*penalty_node */ ) || (mem[main_p].hh.u.B0 == 3 /*ins_node */ )
                               || (mem[main_p].hh.u.B0 == 4 /*mark_node */ ) || (mem[main_p].hh.u.B0 == 5 /*adjust_node */ )
                               || ((mem[main_p].hh.u.B0 == 8 /*whatsit_node */ ) && (mem[main_p].hh.u.B1 <= 4))))
                        main_p = mem[main_p].hh.v.RH;
                    if (!(main_p >= hi_mem_min) && (mem[main_p].hh.u.B0 == 10 /*glue_node */ )) {
                        main_ppp = mem[main_p].hh.v.RH;
                        while (!(main_ppp >= hi_mem_min)
                               && ((mem[main_ppp].hh.u.B0 == 12 /*penalty_node */ )
                                   || (mem[main_ppp].hh.u.B0 == 3 /*ins_node */ )
                                   || (mem[main_ppp].hh.u.B0 == 4 /*mark_node */ )
                                   || (mem[main_ppp].hh.u.B0 == 5 /*adjust_node */ )
                                   || ((mem[main_ppp].hh.u.B0 == 8 /*whatsit_node */ ) && (mem[main_ppp].hh.u.B1 <= 4))))
                            main_ppp = mem[main_ppp].hh.v.RH;
                        if (main_ppp == cur_list.tail_field) {
                            temp_ptr =
                                new_native_word_node(main_f,
                                                     mem[main_pp + 4].qqqq.u.B2 + 1 + mem[cur_list.tail_field +
                                                                                        4].qqqq.u.B2);
                            main_k = 0;
                            {
                                register integer for_end;
                                t = 0;
                                for_end = mem[main_pp + 4].qqqq.u.B2 - 1;
                                if (t <= for_end)
                                    do {
                                        set_native_char(temp_ptr, main_k, get_native_char(main_pp, t));
                                        main_k++;
                                    }
                                    while (t++ < for_end);
                            }
                            set_native_char(temp_ptr, main_k, 32 /*" " */ );
                            main_k++;
                            {
                                register integer for_end;
                                t = 0;
                                for_end = mem[cur_list.tail_field + 4].qqqq.u.B2 - 1;
                                if (t <= for_end)
                                    do {
                                        set_native_char(temp_ptr, main_k, get_native_char(cur_list.tail_field, t));
                                        main_k++;
                                    }
                                    while (t++ < for_end);
                            }
                            set_native_metrics(temp_ptr, (eqtb[8938814L /*eTeX_state_base 3 */ ].cint > 0));
                            t = mem[temp_ptr + 1].cint - mem[main_pp + 1].cint - mem[cur_list.tail_field + 1].cint;
                            free_node(temp_ptr, mem[temp_ptr + 4].qqqq.u.B0);
                            if (t != mem[font_glue[main_f] + 1].cint) {
                                temp_ptr = new_kern(t - mem[font_glue[main_f] + 1].cint);
                                mem[temp_ptr].hh.u.B1 = 3 /*space_adjustment */ ;
                                mem[temp_ptr].hh.v.RH = mem[main_p].hh.v.RH;
                                mem[main_p].hh.v.RH = temp_ptr;
                            }
                        }
                    }
                }
            }
        }
        if (cur_ptr != -268435455L)
            goto lab60;
        else
            goto lab21;
    }
    main_s = eqtb[5596404L /*sf_code_base */  + cur_chr].hh.v.RH % 65536L;
    if (main_s == 1000)
        cur_list.aux_field.hh.v.LH = 1000;
    else if (main_s < 1000) {
        if (main_s > 0)
            cur_list.aux_field.hh.v.LH = main_s;
    } else if (cur_list.aux_field.hh.v.LH < 1000)
        cur_list.aux_field.hh.v.LH = 1000;
    else
        cur_list.aux_field.hh.v.LH = main_s;
    cur_ptr = -268435455L;
    space_class = eqtb[5596404L /*sf_code_base */  + cur_chr].hh.v.RH / 65536L;
    if ((eqtb[8938815L /*eTeX_state_base 4 */ ].cint > 0) && space_class != 4096 /*char_class_limit */ ) {
        if (prev_class == (4095 /*char_class_limit -1 */ )) {
            if ((cur_input.state_field != 0 /*token_list */ ) || (cur_input.index_field != 4 /*backed_up_char */ )) {
                find_sa_element(6 /*inter_char_val */ ,
                                (4095 /*char_class_limit -1 */ ) * 4096 /*char_class_limit */  + space_class, false);
                if (cur_ptr != -268435455L) {
                    if (cur_cmd != 11 /*letter */ )
                        cur_cmd = 12 /*other_char */ ;
                    cur_tok = (cur_cmd * 2097152L /*max_char_val */ ) + cur_chr;
                    back_input();
                    cur_input.index_field = 4 /*backed_up_char */ ;
                    begin_token_list(mem[cur_ptr + 1].hh.v.RH, 17 /*inter_char_text */ );
                    goto lab60;
                }
            }
        } else {

            find_sa_element(6 /*inter_char_val */ , prev_class * 4096 /*char_class_limit */  + space_class, false);
            if (cur_ptr != -268435455L) {
                if (cur_cmd != 11 /*letter */ )
                    cur_cmd = 12 /*other_char */ ;
                cur_tok = (cur_cmd * 2097152L /*max_char_val */ ) + cur_chr;
                back_input();
                cur_input.index_field = 4 /*backed_up_char */ ;
                begin_token_list(mem[cur_ptr + 1].hh.v.RH, 17 /*inter_char_text */ );
                prev_class = (4095 /*char_class_limit -1 */ );
                goto lab60;
            }
        }
        prev_class = space_class;
    }
    main_f = eqtb[2253299L /*cur_font_loc */ ].hh.v.RH;
    bchar = font_bchar[main_f];
    false_bchar = font_false_bchar[main_f];
    if (cur_list.mode_field > 0) {

        if (eqtb[8938790L /*int_base 50 */ ].cint != cur_list.aux_field.hh.v.RH)
            fix_language();
    }
    {
        lig_stack = avail;
        if (lig_stack == -268435455L)
            lig_stack = get_avail();
        else {

            avail = mem[lig_stack].hh.v.RH;
            mem[lig_stack].hh.v.RH = -268435455L;
        }
    }
    mem[lig_stack].hh.u.B0 = main_f;
    cur_l = cur_chr;
    mem[lig_stack].hh.u.B1 = cur_l;
    cur_q = cur_list.tail_field;
    if (cancel_boundary) {
        cancel_boundary = false;
        main_k = 0 /*non_address */ ;
    } else
        main_k = bchar_label[main_f];
    if (main_k == 0 /*non_address */ )
        goto lab92;
    cur_r = cur_l;
    cur_l = 65536L /*too_big_char */ ;
    goto lab111;
 lab80:/*main_loop_wrapup *//*1070: */ if (cur_l < 65536L /*too_big_char */ ) {
        if (mem[cur_q].hh.v.RH > -268435455L) {

            if (mem[cur_list.tail_field].hh.u.B1 == hyphen_char[main_f])
                ins_disc = true;
        }
        if (ligature_present) {
            main_p = new_ligature(main_f, cur_l, mem[cur_q].hh.v.RH);
            if (lft_hit) {
                mem[main_p].hh.u.B1 = 2;
                lft_hit = false;
            }
            if (rt_hit) {

                if (lig_stack == -268435455L) {
                    mem[main_p].hh.u.B1++;
                    rt_hit = false;
                }
            }
            mem[cur_q].hh.v.RH = main_p;
            cur_list.tail_field = main_p;
            ligature_present = false;
        }
        if (ins_disc) {
            ins_disc = false;
            if (cur_list.mode_field > 0) {
                mem[cur_list.tail_field].hh.v.RH = new_disc();
                cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
            }
        }
    }
 lab90:                        /*main_loop_move *//*1071: */ if (lig_stack == -268435455L)
        goto lab21;
    cur_q = cur_list.tail_field;
    cur_l = mem[lig_stack].hh.u.B1;
 lab91:                        /*main_loop_move 1 */ if (!(lig_stack >= hi_mem_min))
        goto lab95;
 lab92:                        /*main_loop_move 2 */ if ((effective_char(false, main_f, cur_chr) > font_ec[main_f])
                              || (effective_char(false, main_f, cur_chr) < font_bc[main_f])) {
        char_warning(main_f, cur_chr);
        {
            mem[lig_stack].hh.v.RH = avail;
            avail = lig_stack;
        }
        goto lab60;
    }
    main_i = effective_char_info(main_f, cur_l);
    if (!(main_i.u.B0 > 0 /*min_quarterword */ )) {
        char_warning(main_f, cur_chr);
        {
            mem[lig_stack].hh.v.RH = avail;
            avail = lig_stack;
        }
        goto lab60;
    }
    mem[cur_list.tail_field].hh.v.RH = lig_stack;
    cur_list.tail_field = /*:1071 */ lig_stack;
 lab100:                       /*main_loop_lookahead *//*1073: */ get_next();
    if (cur_cmd == 11 /*letter */ )
        goto lab101;
    if (cur_cmd == 12 /*other_char */ )
        goto lab101;
    if (cur_cmd == 68 /*char_given */ )
        goto lab101;
    x_token();
    if (cur_cmd == 11 /*letter */ )
        goto lab101;
    if (cur_cmd == 12 /*other_char */ )
        goto lab101;
    if (cur_cmd == 68 /*char_given */ )
        goto lab101;
    if (cur_cmd == 16 /*char_num */ ) {
        scan_char_num();
        cur_chr = cur_val;
        goto lab101;
    }
    if (cur_cmd == 65 /*no_boundary */ )
        bchar = 65536L /*too_big_char */ ;
    cur_r = bchar;
    lig_stack = -268435455L;
    goto lab110;
 lab101:/*main_loop_lookahead 1 */ main_s = eqtb[5596404L /*sf_code_base */  + cur_chr].hh.v.RH % 65536L;
    if (main_s == 1000)
        cur_list.aux_field.hh.v.LH = 1000;
    else if (main_s < 1000) {
        if (main_s > 0)
            cur_list.aux_field.hh.v.LH = main_s;
    } else if (cur_list.aux_field.hh.v.LH < 1000)
        cur_list.aux_field.hh.v.LH = 1000;
    else
        cur_list.aux_field.hh.v.LH = main_s;
    cur_ptr = -268435455L;
    space_class = eqtb[5596404L /*sf_code_base */  + cur_chr].hh.v.RH / 65536L;
    if ((eqtb[8938815L /*eTeX_state_base 4 */ ].cint > 0) && space_class != 4096 /*char_class_limit */ ) {
        if (prev_class == (4095 /*char_class_limit -1 */ )) {
            if ((cur_input.state_field != 0 /*token_list */ ) || (cur_input.index_field != 4 /*backed_up_char */ )) {
                find_sa_element(6 /*inter_char_val */ ,
                                (4095 /*char_class_limit -1 */ ) * 4096 /*char_class_limit */  + space_class, false);
                if (cur_ptr != -268435455L) {
                    if (cur_cmd != 11 /*letter */ )
                        cur_cmd = 12 /*other_char */ ;
                    cur_tok = (cur_cmd * 2097152L /*max_char_val */ ) + cur_chr;
                    back_input();
                    cur_input.index_field = 4 /*backed_up_char */ ;
                    begin_token_list(mem[cur_ptr + 1].hh.v.RH, 17 /*inter_char_text */ );
                    goto lab60;
                }
            }
        } else {

            find_sa_element(6 /*inter_char_val */ , prev_class * 4096 /*char_class_limit */  + space_class, false);
            if (cur_ptr != -268435455L) {
                if (cur_cmd != 11 /*letter */ )
                    cur_cmd = 12 /*other_char */ ;
                cur_tok = (cur_cmd * 2097152L /*max_char_val */ ) + cur_chr;
                back_input();
                cur_input.index_field = 4 /*backed_up_char */ ;
                begin_token_list(mem[cur_ptr + 1].hh.v.RH, 17 /*inter_char_text */ );
                prev_class = (4095 /*char_class_limit -1 */ );
                goto lab60;
            }
        }
        prev_class = space_class;
    }
    {
        lig_stack = avail;
        if (lig_stack == -268435455L)
            lig_stack = get_avail();
        else {

            avail = mem[lig_stack].hh.v.RH;
            mem[lig_stack].hh.v.RH = -268435455L;
        }
    }
    mem[lig_stack].hh.u.B0 = main_f;
    cur_r = cur_chr;
    mem[lig_stack].hh.u.B1 = cur_r;
    if (cur_r == false_bchar)
        cur_r = 65536L /*too_big_char *//*:1073 */ ;
 lab110:/*main_lig_loop *//*1074: */ if (((main_i.u.B2) % 4) != 1 /*lig_tag */ )
        goto lab80;
    if (cur_r == 65536L /*too_big_char */ )
        goto lab80;
    main_k = lig_kern_base[main_f] + main_i.u.B3;
    main_j = font_info[main_k].qqqq;
    if (main_j.u.B0 <= 128)
        goto lab112;
    main_k = lig_kern_base[main_f] + 256 * main_j.u.B2 + main_j.u.B3 + 32768L - 256 * (128);
 lab111:                       /*main_lig_loop 1 */ main_j = font_info[main_k].qqqq;
 lab112:                       /*main_lig_loop 2 */ if (main_j.u.B1 == cur_r) {

        if (main_j.u.B0 <= 128) { /*1075: */
            if (main_j.u.B2 >= 128) {
                if (cur_l < 65536L /*too_big_char */ ) {
                    if (mem[cur_q].hh.v.RH > -268435455L) {

                        if (mem[cur_list.tail_field].hh.u.B1 == hyphen_char[main_f])
                            ins_disc = true;
                    }
                    if (ligature_present) {
                        main_p = new_ligature(main_f, cur_l, mem[cur_q].hh.v.RH);
                        if (lft_hit) {
                            mem[main_p].hh.u.B1 = 2;
                            lft_hit = false;
                        }
                        if (rt_hit) {

                            if (lig_stack == -268435455L) {
                                mem[main_p].hh.u.B1++;
                                rt_hit = false;
                            }
                        }
                        mem[cur_q].hh.v.RH = main_p;
                        cur_list.tail_field = main_p;
                        ligature_present = false;
                    }
                    if (ins_disc) {
                        ins_disc = false;
                        if (cur_list.mode_field > 0) {
                            mem[cur_list.tail_field].hh.v.RH = new_disc();
                            cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
                        }
                    }
                }
                {
                    mem[cur_list.tail_field].hh.v.RH =
                        new_kern(font_info[kern_base[main_f] + 256 * main_j.u.B2 + main_j.u.B3].cint);
                    cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
                }
                goto lab90;
            }
            if (cur_l == 65536L /*too_big_char */ )
                lft_hit = true;
            else if (lig_stack == -268435455L)
                rt_hit = true;
            {
                if (interrupt != 0)
                    pause_for_instructions();
            }
            switch (main_j.u.B2) {
            case 1:
            case 5:
                {
                    cur_l = main_j.u.B3;
                    main_i = font_info[char_base[main_f] + effective_char(true, main_f, cur_l)].qqqq;
                    ligature_present = true;
                }
                break;
            case 2:
            case 6:
                {
                    cur_r = main_j.u.B3;
                    if (lig_stack == -268435455L) {
                        lig_stack = new_lig_item(cur_r);
                        bchar = 65536L /*too_big_char */ ;
                    } else if ((lig_stack >= hi_mem_min)) {
                        main_p = lig_stack;
                        lig_stack = new_lig_item(cur_r);
                        mem[lig_stack + 1].hh.v.RH = main_p;
                    } else
                        mem[lig_stack].hh.u.B1 = cur_r;
                }
                break;
            case 3:
                {
                    cur_r = main_j.u.B3;
                    main_p = lig_stack;
                    lig_stack = new_lig_item(cur_r);
                    mem[lig_stack].hh.v.RH = main_p;
                }
                break;
            case 7:
            case 11:
                {
                    if (cur_l < 65536L /*too_big_char */ ) {
                        if (mem[cur_q].hh.v.RH > -268435455L) {

                            if (mem[cur_list.tail_field].hh.u.B1 == hyphen_char[main_f])
                                ins_disc = true;
                        }
                        if (ligature_present) {
                            main_p = new_ligature(main_f, cur_l, mem[cur_q].hh.v.RH);
                            if (lft_hit) {
                                mem[main_p].hh.u.B1 = 2;
                                lft_hit = false;
                            }
                            if (false) {

                                if (lig_stack == -268435455L) {
                                    mem[main_p].hh.u.B1++;
                                    rt_hit = false;
                                }
                            }
                            mem[cur_q].hh.v.RH = main_p;
                            cur_list.tail_field = main_p;
                            ligature_present = false;
                        }
                        if (ins_disc) {
                            ins_disc = false;
                            if (cur_list.mode_field > 0) {
                                mem[cur_list.tail_field].hh.v.RH = new_disc();
                                cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
                            }
                        }
                    }
                    cur_q = cur_list.tail_field;
                    cur_l = main_j.u.B3;
                    main_i = font_info[char_base[main_f] + effective_char(true, main_f, cur_l)].qqqq;
                    ligature_present = true;
                }
                break;
            default:
                {
                    cur_l = main_j.u.B3;
                    ligature_present = true;
                    if (lig_stack == -268435455L)
                        goto lab80;
                    else
                        goto lab91;
                }
                break;
            }
            if (main_j.u.B2 > 4) {

                if (main_j.u.B2 != 7)
                    goto lab80;
            }
            if (cur_l < 65536L /*too_big_char */ )
                goto lab110;
            main_k = bchar_label[main_f];
            goto lab111;
        }
    }
    if (main_j.u.B0 == 0)
        main_k++;
    else {

        if (main_j.u.B0 >= 128)
            goto lab80;
        main_k = main_k + main_j.u.B0 + 1;
    }
    goto lab111;
 lab95:                        /*main_loop_move_lig *//*1072: */ main_p = mem[lig_stack + 1].hh.v.RH;
    if (main_p > -268435455L) {
        mem[cur_list.tail_field].hh.v.RH = main_p;
        cur_list.tail_field = mem[cur_list.tail_field].hh.v.RH;
    }
    temp_ptr = lig_stack;
    lig_stack = mem[temp_ptr].hh.v.RH;
    free_node(temp_ptr, 2 /*small_node_size */ );
    main_i = font_info[char_base[main_f] + effective_char(true, main_f, cur_l)].qqqq;
    ligature_present = true;
    if (lig_stack == -268435455L) {

        if (main_p > -268435455L)
            goto lab100;
        else
            cur_r = bchar;
    } else
        cur_r = mem[lig_stack].hh.u.B1;
    goto lab110;
 lab120:/*append_normal_space */ if ((eqtb[8938815L /*eTeX_state_base 4 */ ].cint > 0)
                                 && (space_class != 4096 /*char_class_limit */ )
                                 && (prev_class != (4095 /*char_class_limit -1 */ ))) {
        prev_class = (4095 /*char_class_limit -1 */ );
        find_sa_element(6 /*inter_char_val */ ,
                        space_class * 4096 /*char_class_limit */  + (4095 /*char_class_limit -1 */ ), false);
        if (cur_ptr != -268435455L) {
            if (cur_cs == 0) {
                if (cur_cmd == 16 /*char_num */ )
                    cur_cmd = 12 /*other_char */ ;
                cur_tok = (cur_cmd * 2097152L /*max_char_val */ ) + cur_chr;
            } else
                cur_tok = 33554431L /*cs_token_flag */  + cur_cs;
            back_input();
            begin_token_list(mem[cur_ptr + 1].hh.v.RH, 17 /*inter_char_text */ );
            goto lab60;
        }
    }
    if (eqtb[2252252L /*glue_base 12 */ ].hh.v.RH == mem_bot) {
        {
            main_p = font_glue[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH];
            if (main_p == -268435455L) {
                main_p = new_spec(mem_bot);
                main_k = param_base[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH] + 2;
                mem[main_p + 1].cint = font_info[main_k].cint;
                mem[main_p + 2].cint = font_info[main_k + 1].cint;
                mem[main_p + 3].cint = font_info[main_k + 2].cint;
                font_glue[eqtb[2253299L /*cur_font_loc */ ].hh.v.RH] = main_p;
            }
        }
        temp_ptr = new_glue(main_p);
    } else
        temp_ptr = new_param_glue(12 /*space_skip_code */ );
    mem[cur_list.tail_field].hh.v.RH = temp_ptr;
    cur_list.tail_field = temp_ptr;
    goto lab60;
}

void give_err_help(void)
{
    give_err_help_regmem token_show(eqtb[2252780L /*err_help_loc */ ].hh.v.RH);
}

boolean open_fmt_file(void)
{
    integer j;
    FILE *tmp;

    j = cur_input.loc_field;

    if (buffer[cur_input.loc_field] == 38 /*"&" */ ) {
        cur_input.loc_field++;
        j = cur_input.loc_field;
        buffer[last] = 32 /*" " */ ;
        while (buffer[j] != 32 /*" " */ )
            j++;
        pack_buffered_name(0, cur_input.loc_field, j - 1);
	if (open_input (&tmp, kpse_fmt_format, "rb")
	    && (fmt_file = gzdopen(fileno(tmp), "rb")))
            goto lab40;
        fputs("Sorry, I can't find the format `", stdout);
        fputs((string) (name_of_file + 1), stdout);
        fputs("'; will try `", stdout);
        fputs(TEX_format_default + 1, stdout);
        fprintf(stdout, "%s\n", "'.");
        fflush(stdout);
    }

    pack_buffered_name(format_default_length - 4, 1, 0);

    if (!(open_input (&tmp, kpse_fmt_format, "rb")
	  && (fmt_file = gzdopen(fileno(tmp), "rb")))) {
        fputs("I can't find the format file `", stdout);
        fputs(TEX_format_default + 1, stdout);
        fprintf(stdout, "%s\n", "'!");
        return false;
    }

lab40: /* found */
    cur_input.loc_field = j;
    return true;
}

void close_files_and_terminate(void)
{
    close_files_and_terminate_regmem;

    integer k;

    terminate_font_manager();
    {
        register integer for_end;
        k = 0;
        for_end = 15;
        if (k <= for_end)
            do
                if (write_open[k])
                    close_file(write_file[k]);
            while (k++ < for_end) ;
    }

    while (cur_s > -1) {
        if (cur_s > 0) {
            dvi_buf[dvi_ptr] = 142 /*pop */ ;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        } else {
	    dvi_buf[dvi_ptr] = 140 /*eop */ ;
	    dvi_ptr++;
	    if (dvi_ptr == dvi_limit)
		dvi_swap();
            total_pages++;
        }
        cur_s--;
    }

    if (total_pages == 0)
        print_nl(66201L /*"No pages of output." */ );
    else if (cur_s != -2) {
	dvi_buf[dvi_ptr] = 248 /*post */ ;
	dvi_ptr++;
	if (dvi_ptr == dvi_limit)
	    dvi_swap();
        dvi_four(last_bop);
        last_bop = dvi_offset + dvi_ptr - 5;
        dvi_four(25400000L);
        dvi_four(473628672L);
        prepare_mag();
        dvi_four(eqtb[8938757L /*int_base 17 */ ].cint);
        dvi_four(max_v);
        dvi_four(max_h);
        {
            dvi_buf[dvi_ptr] = max_push / 256;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        {
            dvi_buf[dvi_ptr] = max_push % 256;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        {
            dvi_buf[dvi_ptr] = (total_pages / 256) % 256;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        {
            dvi_buf[dvi_ptr] = total_pages % 256;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        while (font_ptr > 0 /*font_base */ ) {

            if (font_used[font_ptr])
                dvi_font_def(font_ptr);
            font_ptr--;
        }
        {
            dvi_buf[dvi_ptr] = 249 /*post_post */ ;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        dvi_four(last_bop);
        {
            dvi_buf[dvi_ptr] = 7 /*id_byte */ ;
            dvi_ptr++;
            if (dvi_ptr == dvi_limit)
                dvi_swap();
        }
        ;

        k = 4 + ((dvi_buf_size - dvi_ptr) % 4);

        while (k > 0) {
	    dvi_buf[dvi_ptr] = 223;
	    dvi_ptr++;
	    if (dvi_ptr == dvi_limit)
		dvi_swap();
            k--;
        }

        if (dvi_limit == half_buf)
            WRITE_OUT(half_buf, dvi_buf_size - 1);

        if (dvi_ptr > (2147483647L - dvi_offset)) {
            cur_s = -2;
            fatal_error(66188L /*"dvi length exceeds "7FFFFFFF" */ );
        }

        if (dvi_ptr > 0)
            WRITE_OUT(0, dvi_ptr - 1);

        k = dvi_close(dvi_file);

        if (k == 0) {
            print_nl(66202L /*"Output written on " */ );
            print(output_file_name);
            print(65566L /*" (" */ );
            print_int(total_pages);
            if (total_pages != 1)
                print(66203L /*" pages" */ );
            else
                print(66204L /*" page" */ );
            if (no_pdf_output) {
                print(66205L /*", " */ );
                print_int(dvi_offset + dvi_ptr);
                print(66206L /*" bytes)." */ );
            } else
                print(66207L /*")." */ );
        } else {
            print_nl(66208L /*"Error " */ );
            print_int(k);
            print(65566L /*" (" */ );
            if (no_pdf_output)
                print_c_string(strerror(k));
            else
                print(66209L /*"driver return code" */ );
            print(66210L /*") generating output;" */ );
            print_nl(66211L /*"file " */ );
            print(output_file_name);
            print(66212L /*" may not be valid." */ );
        }
    }

    synctex_terminate(log_opened);
    if (log_opened) {
        putc('\n', log_file);
        close_file(log_file);
        selector = selector - 2;
        if (selector == SELECTOR_TERM_ONLY) {
            print_nl(66701L /*"Transcript written on " */ );
            print(texmf_log_name);
            print_char(46 /*"." */ );
        }
    }

    print_ln();
}

void zflush_str(str_number s)
{
    flush_str_regmem if ((s == str_ptr - 1)) {
        str_ptr--;
        pool_ptr = str_start[(str_ptr) - 65536L];
    }
}

str_number ztokens_to_string(halfword p)
{
    register str_number Result;
    tokens_to_string_regmem if (selector == SELECTOR_NEW_STRING )
        pdf_error(66726L /*"tokens" */ , 66727L /*"tokens_to_string() called while selector = new_string" */ );
    old_setting = selector;
    selector = SELECTOR_NEW_STRING ;
    show_token_list(mem[p].hh.v.RH, -268435455L, pool_size - pool_ptr);
    selector = old_setting;
    Result = make_string();
    return Result;
}

void scan_pdf_ext_toks(void)
{
    scan_pdf_ext_toks_regmem {
        if (scan_toks(false, true) != 0) ;
    }
}

void compare_strings(void)
{
    compare_strings_regmem str_number s1, s2;
    pool_pointer i1, i2, j1, j2;
    {
        if (scan_toks(false, true) != 0) ;
    }
    s1 = tokens_to_string(def_ref);
    delete_token_ref(def_ref);
    {
        if (scan_toks(false, true) != 0) ;
    }
    s2 = tokens_to_string(def_ref);
    delete_token_ref(def_ref);
    i1 = str_start[(s1) - 65536L];
    j1 = str_start[(s1 + 1) - 65536L];
    i2 = str_start[(s2) - 65536L];
    j2 = str_start[(s2 + 1) - 65536L];
    while ((i1 < j1) && (i2 < j2)) {

        if (str_pool[i1] < str_pool[i2]) {
            cur_val = -1;
            goto lab30;
        }
        if (str_pool[i1] > str_pool[i2]) {
            cur_val = 1;
            goto lab30;
        }
        i1++;
        i2++;
    }
    if ((i1 == j1) && (i2 == j2))
        cur_val = 0;
    else if (i1 < j1)
        cur_val = 1;
    else
        cur_val = -1;
 lab30:                        /*done */ flush_str(s2);
    flush_str(s1);
    cur_val_level = 0 /*int_val */ ;
}

str_number get_nullstr(void)
{
    register str_number Result;
    get_nullstr_regmem Result = 65622L /*"" */ ;
    return Result;
}

/*:1713*/
