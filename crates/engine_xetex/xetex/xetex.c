/* xetex.c: bulk of the WEB code translated to C
   Copyright 2016-2018 The Tectonic Project
   Licensed under the MIT License.
*/

#include "core.h"
#include "xetex.h"
#include "synctex.h"

#include <stdio.h> /* for EOF */

#define IS_LC_HEX(c) (((c) >= '0' && (c) <= '9' ) || ((c) >= 'a' && (c) <= 'f' ))

static void
int_error(int32_t n)
{
    print_cstr(" (");
    print_int(n);
    print_char(')');
    error();
}


int32_t
badness(scaled_t t, scaled_t s)
{
    int32_t r;

    if (t == 0)
        return 0;

    if (s <= 0)
        return INF_BAD;

    if (t <= 7230584L) /* magic constant */
        r = (t * 297) / s;
    else if (s >= 1663497L) /* magic constant */
        r = t / (s / 297);
    else
        r = t;

    if (r > 1290) /* magic constant */
        return INF_BAD;

    return (r * r * r + 0x20000) / 0x40000;
}


/*:112*/
/*118:*/
void
show_token_list(int32_t p, int32_t q, int32_t l)
{
    int32_t m, c;
    int32_t match_chr;
    UTF16_code n;

    match_chr = '#' ;
    n = '0' ;
    tally = 0;

    while (p != TEX_NULL && tally < l) {
        /*332:*/
        if (p == q) {
            first_count = tally;
            trick_count = tally + 1 + error_line - half_error_line;
            if (trick_count < error_line)
                trick_count = error_line;
        }

        if (p < hi_mem_min || p > mem_end) {
            print_esc_cstr("CLOBBERED.");
            return;
        }

        if (mem[p].b32.s0 >= CS_TOKEN_FLAG) {
            print_cs(mem[p].b32.s0 - CS_TOKEN_FLAG);
        } else {
            m = mem[p].b32.s0 / MAX_CHAR_VAL;
            c = mem[p].b32.s0 % MAX_CHAR_VAL;

            if (mem[p].b32.s0 < 0) {
                print_esc_cstr("BAD.");
            } else {
                /*306:*/
                switch (m) {
                case LEFT_BRACE:
                case RIGHT_BRACE:
                case MATH_SHIFT:
                case TAB_MARK:
                case SUP_MARK:
                case SUB_MARK:
                case SPACER:
                case LETTER:
                case OTHER_CHAR:
                    print_char(c);
                    break;
                case MAC_PARAM:
                    print_char(c);
                    print_char(c);
                    break;
                case OUT_PARAM:
                    print_char(match_chr);
                    if (c <= 9) {
                        print_char(c + 48);
                    } else {
                        print_char('!');
                        return;
                    }
                    break;
                case MATCH:
                    match_chr = c;
                    print_char(c);
                    n++;
                    print_char(n);
                    if (n > '9' )
                        return;
                    break;
                case END_MATCH:
                    if (c == 0)
                        print_cstr("->");
                    break;
                default:
                    print_esc_cstr("BAD.");
                    break;
                }
            }
        }

        p = LLIST_link(p);
    }

    if (p != TEX_NULL)
        print_esc_cstr("ETC.");
}


void
runaway(void)
{
    int32_t p = TEX_NULL;

    if (scanner_status > SKIPPING) {
        switch (scanner_status) {
        case DEFINING:
            print_nl_cstr("Runaway definition");
            p = def_ref;
            break;
        case MATCHING:
            print_nl_cstr("Runaway argument");
            p = TEMP_HEAD;
            break;
        case ALIGNING:
            print_nl_cstr("Runaway preamble");
            p = HOLD_HEAD;
            break;
        case ABSORBING:
            print_nl_cstr("Runaway text");
            p = def_ref;
            break;
        }

        print_char('?');
        print_ln();
        show_token_list(mem[p].b32.s1, TEX_NULL, error_line - 10);
    }
}


int32_t get_avail(void)
{
    int32_t p;
    p = avail;
    if (p != TEX_NULL)
        avail = LLIST_link(avail);
    else if (mem_end < MEM_TOP) {
        mem_end++;
        p = mem_end;
    } else {

        hi_mem_min--;
        p = hi_mem_min;
        if (is_char_node(lo_mem_max)) {
            runaway();
            overflow("main memory size", MEM_TOP + 1);
        }
    }
    mem[p].b32.s1 = TEX_NULL;
    return p;
}

void flush_list(int32_t p)
{
    int32_t q, r;
    if (p != TEX_NULL) {
        r = p;
        do {
            q = r;
            r = LLIST_link(r);
        } while (!(r == TEX_NULL));
        mem[q].b32.s1 = avail;
        avail = p;
    }
}

int32_t get_node(int32_t s)
{
    int32_t p;
    int32_t q;
    int32_t r;
    int32_t t;

restart:
    p = rover;

    do {
        /*131: */ q = p + mem[p].b32.s0;
        while (mem[q].b32.s1 == MAX_HALFWORD) {

            t = mem[q + 1].b32.s1;
            if (q == rover)
                rover = t;
            mem[t + 1].b32.s0 = mem[q + 1].b32.s0;
            mem[mem[q + 1].b32.s0 + 1].b32.s1 = t;
            q = q + mem[q].b32.s0;
        }
        r = q - s;
        if (r > p + 1) {        /*132: */
            mem[p].b32.s0 = r - p;
            rover = p;
            goto found;
        }
        if (r == p) {

            if (mem[p + 1].b32.s1 != p) {      /*133: */
                rover = mem[p + 1].b32.s1;
                t = mem[p + 1].b32.s0;
                mem[rover + 1].b32.s0 = t;
                mem[t + 1].b32.s1 = rover;
                goto found;
            }
        }
        mem[p].b32.s0 = q - /*:131 */ p;
        p = mem[p + 1].b32.s1;
    } while (!(p == rover));
    if (s == 0x40000000) {
        return MAX_HALFWORD;
    }
    if (lo_mem_max + 2 < hi_mem_min) {

        if (lo_mem_max + 2 <= MAX_HALFWORD) {  /*130: */
            if (hi_mem_min - lo_mem_max >= 1998)
                t = lo_mem_max + 1000;
            else
                t = lo_mem_max + 1 + (hi_mem_min - lo_mem_max) / 2;
            p = mem[rover + 1].b32.s0;
            q = lo_mem_max;
            mem[p + 1].b32.s1 = q;
            mem[rover + 1].b32.s0 = q;
            if (t > MAX_HALFWORD)
                t = MAX_HALFWORD;
            mem[q + 1].b32.s1 = rover;
            mem[q + 1].b32.s0 = p;
            mem[q].b32.s1 = MAX_HALFWORD;
            mem[q].b32.s0 = t - lo_mem_max;
            lo_mem_max = t;
            mem[lo_mem_max].b32.s1 = TEX_NULL;
            mem[lo_mem_max].b32.s0 = TEX_NULL;
            rover = q;
            goto restart;
        }
    }
    overflow("main memory size", MEM_TOP + 1);

found:
    mem[r].b32.s1 = TEX_NULL;
    if (s >= MEDIUM_NODE_SIZE) {
        mem[r + s - 1].b32.s0 = cur_input.synctex_tag;
        mem[r + s - 1].b32.s1 = line;
    }
    return r;
}

void free_node(int32_t p, int32_t s)
{
    int32_t q;
    mem[p].b32.s0 = s;
    mem[p].b32.s1 = MAX_HALFWORD;
    q = mem[rover + 1].b32.s0;
    mem[p + 1].b32.s0 = q;
    mem[p + 1].b32.s1 = rover;
    mem[rover + 1].b32.s0 = p;
    mem[q + 1].b32.s1 = p;
}

int32_t new_null_box(void)
{
    int32_t p;
    p = get_node(BOX_NODE_SIZE);
    NODE_type(p) = HLIST_NODE;
    mem[p].b16.s0 = 0;
    mem[p + 1].b32.s1 = 0;
    mem[p + 2].b32.s1 = 0;
    mem[p + 3].b32.s1 = 0;
    mem[p + 4].b32.s1 = 0;
    mem[p + 5].b32.s1 = TEX_NULL;
    mem[p + 5].b16.s1 = NORMAL;
    mem[p + 5].b16.s0 = NORMAL;
    BOX_glue_set(p) = 0.0;
    return p;
}

int32_t new_rule(void)
{
    int32_t p;
    p = get_node(RULE_NODE_SIZE);
    NODE_type(p) = RULE_NODE;
    mem[p].b16.s0 = 0;
    mem[p + 1].b32.s1 = NULL_FLAG;
    mem[p + 2].b32.s1 = NULL_FLAG;
    mem[p + 3].b32.s1 = NULL_FLAG;
    return p;
}

int32_t new_ligature(internal_font_number f, uint16_t c, int32_t q)
{
    int32_t p;
    p = get_node(SMALL_NODE_SIZE);
    NODE_type(p) = LIGATURE_NODE;
    mem[p + 1].b16.s1 = f;
    mem[p + 1].b16.s0 = c;
    mem[p + 1].b32.s1 = q;
    mem[p].b16.s0 = 0;
    return p;
}

int32_t new_lig_item(uint16_t c)
{
    int32_t p;
    p = get_node(SMALL_NODE_SIZE);
    mem[p].b16.s0 = c;
    mem[p + 1].b32.s1 = TEX_NULL;
    return p;
}

int32_t new_disc(void)
{
    int32_t p;
    p = get_node(SMALL_NODE_SIZE);
    NODE_type(p) = DISC_NODE;
    mem[p].b16.s0 = 0;
    mem[p + 1].b32.s0 = TEX_NULL;
    mem[p + 1].b32.s1 = TEX_NULL;
    return p;
}


void
copy_native_glyph_info(int32_t src, int32_t dest)
{
    int32_t glyph_count;

    if (NATIVE_NODE_glyph_info_ptr(src) != NULL) {
        glyph_count = NATIVE_NODE_glyph_count(src);
        NATIVE_NODE_glyph_info_ptr(dest) = xmalloc_array(char, glyph_count * NATIVE_GLYPH_INFO_SIZE);
        memcpy(NATIVE_NODE_glyph_info_ptr(dest), NATIVE_NODE_glyph_info_ptr(src), glyph_count * NATIVE_GLYPH_INFO_SIZE);
        NATIVE_NODE_glyph_count(dest) = glyph_count;
    }
}


int32_t new_math(scaled_t w, small_number s)
{
    int32_t p;
    p = get_node(MEDIUM_NODE_SIZE);
    NODE_type(p) = MATH_NODE;
    mem[p].b16.s0 = s;
    mem[p + 1].b32.s1 = w;
    return p;
}

int32_t new_spec(int32_t p)
{
    int32_t q;
    q = get_node(GLUE_SPEC_SIZE);
    mem[q] = mem[p];
    mem[q].b32.s1 = TEX_NULL;
    mem[q + 1].b32.s1 = mem[p + 1].b32.s1;
    mem[q + 2].b32.s1 = mem[p + 2].b32.s1;
    mem[q + 3].b32.s1 = mem[p + 3].b32.s1;
    return q;
}

int32_t new_param_glue(small_number n)
{
    int32_t p;
    int32_t q;

    p = get_node(MEDIUM_NODE_SIZE);
    NODE_type(p) = GLUE_NODE;
    mem[p].b16.s0 = n + 1;
    mem[p + 1].b32.s1 = TEX_NULL;
    q = /*232: */ eqtb[GLUE_BASE + n].b32.s1 /*:232 */ ;
    mem[p + 1].b32.s0 = q;
    GLUE_SPEC_ref_count(q)++;
    return p;
}

int32_t new_glue(int32_t q)
{
    int32_t p;
    p = get_node(MEDIUM_NODE_SIZE);
    NODE_type(p) = GLUE_NODE;
    GLUE_SPEC_shrink_order(p) = NORMAL;
    mem[p + 1].b32.s1 = TEX_NULL;
    mem[p + 1].b32.s0 = q;
    GLUE_SPEC_ref_count(q)++;
    return p;
}

int32_t new_skip_param(small_number n)
{
    int32_t p;

    temp_ptr = new_spec( /*232: */ eqtb[GLUE_BASE + n].b32.s1 /*:232 */ );
    p = new_glue(temp_ptr);
    mem[temp_ptr].b32.s1 = TEX_NULL;
    mem[p].b16.s0 = n + 1;
    return p;
}

int32_t new_kern(scaled_t w)
{
    int32_t p;
    p = get_node(MEDIUM_NODE_SIZE);
    NODE_type(p) = KERN_NODE;
    mem[p].b16.s0 = NORMAL;
    mem[p + 1].b32.s1 = w;
    return p;
}

int32_t new_penalty(int32_t m)
{
    int32_t p;
    p = get_node(MEDIUM_NODE_SIZE);
    NODE_type(p) = PENALTY_NODE;
    mem[p].b16.s0 = 0;
    mem[p + 1].b32.s1 = m;
    return p;
}

/*:165*/

int32_t prev_rightmost(int32_t s, int32_t e)
{
    int32_t p;
    p = s;
    if (p == TEX_NULL)
        return TEX_NULL;
    while (mem[p].b32.s1 != e) {

        p = LLIST_link(p);
        if (p == TEX_NULL)
            return TEX_NULL;
    }
    return p;
}

int32_t
get_microinterval(void)
{
    int32_t s, m;

    get_seconds_and_micros(&s, &m);

    if ((s - epochseconds) > 0x7FFF)
        return -1;
    else if (microseconds > m)
        return ((s - 1 - epochseconds) * 65536) + (((m + 1000000L - microseconds) / 100.0) * 65536) / 10000.0;
    else
        return ((s - epochseconds) * 65536) + (((m - microseconds) / 100.0) * 65536) / 10000.0;
}

void
short_display(int32_t p)
{
    int32_t n;

    while (p > 0) {
        if (is_char_node(p)) {
            if (p <= mem_end) {
                if (mem[p].b16.s1 != font_in_short_display) {
                    if (mem[p].b16.s1 > font_max)
                        print_char('*');
                    else /*279:*/
                        print_esc(hash[FONT_ID_BASE + mem[p].b16.s1].s1);
                    print_char(' ');
                    font_in_short_display = mem[p].b16.s1;
                }
                print(mem[p].b16.s0);
            }
        } else {
            /*183:*/
            switch (mem[p].b16.s1) {
            case HLIST_NODE:
            case VLIST_NODE:
            case INS_NODE:
            case MARK_NODE:
            case ADJUST_NODE:
            case UNSET_NODE:
                print_cstr("[]");
                break;
            case WHATSIT_NODE:
                switch (mem[p].b16.s0) {
                case NATIVE_WORD_NODE:
                case NATIVE_WORD_NODE_AT:
                    if (mem[p + 4].b16.s2 != font_in_short_display) {
                        print_esc(hash[FONT_ID_BASE + mem[p + 4].b16.s2].s1);
                        print_char(' ');
                        font_in_short_display = mem[p + 4].b16.s2;
                    }
                    print_native_word(p);
                    break;
                default:
                    print_cstr("[]");
                    break;
                }
                break;
            case RULE_NODE:
                print_char('|');
                break;
            case GLUE_NODE:
                if (mem[p + 1].b32.s0 != 0)
                    print_char(' ');
                break;
            case MATH_NODE:
                if (mem[p].b16.s0 >= L_CODE)
                    print_cstr("[]");
                else
                    print_char('$');
                break;
            case LIGATURE_NODE:
                short_display(mem[p + 1].b32.s1);
                break;
            case DISC_NODE:
                short_display(mem[p + 1].b32.s0);
                short_display(mem[p + 1].b32.s1);
                n = mem[p].b16.s0;

                while (n > 0) {
                    if (mem[p].b32.s1 != TEX_NULL)
                        p = LLIST_link(p);
                    n--;
                }
                break;
            default:
                break;
            }
        }

        p = LLIST_link(p);
    }
}


void print_font_and_char(int32_t p)
{
    if (p > mem_end)
        print_esc_cstr("CLOBBERED.");
    else {

        if ((mem[p].b16.s1 > font_max))
            print_char('*');
        else /*279: */
            print_esc(hash[FONT_ID_BASE + mem[p].b16.s1].s1);
        print_char(' ');
        print(mem[p].b16.s0);
    }
}

void print_mark(int32_t p)
{
    print_char('{');
    if ((p < hi_mem_min) || (p > mem_end))
        print_esc_cstr("CLOBBERED.");
    else
        show_token_list(mem[p].b32.s1, TEX_NULL, max_print_line - 10);
    print_char('}');
}

void print_rule_dimen(scaled_t d)
{
    if (d == NULL_FLAG)
        print_char('*');
    else
        print_scaled(d);
}

void print_glue(scaled_t d, int32_t order, const char* s)
{
    print_scaled(d);
    if ((order < NORMAL) || (order > FILLL))
        print_cstr("foul");
    else if (order > NORMAL) {
        print_cstr("fil");
        while (order > FIL) {

            print_char('l');
            order--;
        }
    } else if (s != 0)
        print_cstr(s);
}

void print_spec(int32_t p, const char* s)
{

    if (p < 0 || p >= lo_mem_max)
        print_char('*');
    else {
        print_scaled(mem[p + 1].b32.s1);
        if (s != NULL)
            print_cstr(s);
        if (mem[p + 2].b32.s1 != 0) {
            print_cstr(" plus ");
            print_glue(mem[p + 2].b32.s1, mem[p].b16.s1, s);
        }
        if (mem[p + 3].b32.s1 != 0) {
            print_cstr(" minus ");
            print_glue(mem[p + 3].b32.s1, mem[p].b16.s0, s);
        }
    }
}

void print_fam_and_char(int32_t p)
{
    int32_t c;
    print_esc_cstr("fam");
    print_int((mem[p].b16.s1 % 256) % 256);
    print_char(' ');
    c = ((unsigned short) mem[p].b16.s0 + ((mem[p].b16.s1 / 256) * 65536L));
    if (c < 65536L)
        print(c);
    else
        print_char(c);
}

void print_delimiter(int32_t p)
{
    int32_t a;
    a = (mem[p].b16.s3 % 256) * 256 + (mem[p].b16.s2 + (mem[p].b16.s3 / 256) * 65536L);
    a = a * 4096 + (mem[p].b16.s1 % 256) * 256 + (mem[p].b16.s0 + (mem[p].b16.s1 / 256) * 65536L);
    if (a < 0)
        print_int(a);
    else
        print_hex(a);
}


void
print_subsidiary_data(int32_t p, UTF16_code c)
{

    if (cur_length() >= depth_threshold) {
        if (mem[p].b32.s1 != EMPTY)
            print_cstr(" []");
    } else {
        str_pool[pool_ptr] = c;
        pool_ptr++;
        temp_ptr = p;

        switch (mem[p].b32.s1) {
        case MATH_CHAR:
            print_ln();
            print_current_string();
            print_fam_and_char(p);
            break;
        case SUB_BOX:
            show_info();
            break;
        case SUB_MLIST:
            if (mem[p].b32.s0 == TEX_NULL) {
                print_ln();
                print_current_string();
                print_cstr("{}");
            } else {
                show_info();
            }
            break;
        default:
            break;
        }

        pool_ptr--;
    }
}


void print_style(int32_t c)
{
    switch (c / 2) {
    case DISPLAY_STYLE / 2:
        print_esc_cstr("displaystyle");
        break;
    case TEXT_STYLE / 2:
        print_esc_cstr("textstyle");
        break;
    case SCRIPT_STYLE / 2:
        print_esc_cstr("scriptstyle");
        break;
    case SCRIPT_SCRIPT_STYLE / 2:
        print_esc_cstr("scriptscriptstyle");
        break;
    default:
        print_cstr("Unknown style!");
        break;
    }
}

void print_skip_param(int32_t n)
{
    switch (n) {
    case GLUE_PAR__line_skip:
        print_esc_cstr("lineskip");
        break;
    case GLUE_PAR__baseline_skip:
        print_esc_cstr("baselineskip");
        break;
    case GLUE_PAR__par_skip:
        print_esc_cstr("parskip");
        break;
    case GLUE_PAR__above_display_skip:
        print_esc_cstr("abovedisplayskip");
        break;
    case GLUE_PAR__below_display_skip:
        print_esc_cstr("belowdisplayskip");
        break;
    case GLUE_PAR__above_display_short_skip:
        print_esc_cstr("abovedisplayshortskip");
        break;
    case GLUE_PAR__below_display_short_skip:
        print_esc_cstr("belowdisplayshortskip");
        break;
    case GLUE_PAR__left_skip:
        print_esc_cstr("leftskip");
        break;
    case GLUE_PAR__right_skip:
        print_esc_cstr("rightskip");
        break;
    case GLUE_PAR__top_skip:
        print_esc_cstr("topskip");
        break;
    case GLUE_PAR__split_top_skip:
        print_esc_cstr("splittopskip");
        break;
    case GLUE_PAR__tab_skip:
        print_esc_cstr("tabskip");
        break;
    case GLUE_PAR__space_skip:
        print_esc_cstr("spaceskip");
        break;
    case GLUE_PAR__xspace_skip:
        print_esc_cstr("xspaceskip");
        break;
    case GLUE_PAR__par_fill_skip:
        print_esc_cstr("parfillskip");
        break;
    case GLUE_PAR__xetex_linebreak_skip:
        print_esc_cstr("XeTeXlinebreakskip");
        break;
    case GLUE_PAR__thin_mu_skip:
        print_esc_cstr("thinmuskip");
        break;
    case GLUE_PAR__med_mu_skip:
        print_esc_cstr("medmuskip");
        break;
    case GLUE_PAR__thick_mu_skip:
        print_esc_cstr("thickmuskip");
        break;
    default:
        print_cstr("[unknown glue parameter!]");
        break;
    }
}


void
show_node_list(int32_t p)
{
    int32_t n;
    int32_t i;
    double g;

    if (cur_length() > depth_threshold) {
        if (p > TEX_NULL)
            print_cstr(" []");
        return;
    }

    n = 0;

    while (p > 0) {
        print_ln();
        print_current_string();

        if (p > mem_end) {
            print_cstr("Bad link, display aborted.");
            return;
        }

        n++;

        if (n > breadth_max) {
            print_cstr("etc.");
            return;
        }

        if (is_char_node(p)) {
            print_font_and_char(p);
        } else {
            switch (mem[p].b16.s1) {
            case HLIST_NODE:
            case VLIST_NODE:
            case UNSET_NODE:
                if (NODE_type(p) == HLIST_NODE)
                    print_esc('h' );
                else if (NODE_type(p) == VLIST_NODE)
                    print_esc('v' );
                else
                    print_esc_cstr("unset");

                print_cstr("box(");
                print_scaled(mem[p + 3].b32.s1);
                print_char('+');
                print_scaled(mem[p + 2].b32.s1);
                print_cstr(")x");
                print_scaled(mem[p + 1].b32.s1);

                if (NODE_type(p) == UNSET_NODE) { /*193:*/
                    if (mem[p].b16.s0 != 0) {
                        print_cstr(" (");
                        print_int(mem[p].b16.s0 + 1);
                        print_cstr(" columns)");
                    }
                    if (mem[p + 6].b32.s1 != 0) {
                        print_cstr(", stretch ");
                        print_glue(mem[p + 6].b32.s1, mem[p + 5].b16.s0, NULL);
                    }
                    if (mem[p + 4].b32.s1 != 0) {
                        print_cstr(", shrink ");
                        print_glue(mem[p + 4].b32.s1, mem[p + 5].b16.s1, NULL);
                    }
                } else {
                    g = BOX_glue_set(p);

                    if (g != 0.0 && mem[p + 5].b16.s1 != NORMAL) {
                        print_cstr(", glue set ");
                        if (mem[p + 5].b16.s1 == SHRINKING)
                            print_cstr("- ");

                        if (fabs(g) > 20000.0) {
                            if (g > 0.0)
                                print_char('>');
                            else
                                print_cstr("< -");
                            print_glue(20000 * 65536L, mem[p + 5].b16.s0, NULL);
                        } else {
                            print_glue(tex_round(65536L * g), mem[p + 5].b16.s0, NULL);
                        }
                    }

                    if (mem[p + 4].b32.s1 != 0) {
                        print_cstr(", shifted ");
                        print_scaled(mem[p + 4].b32.s1);
                    }

                    /*1491:*/
                    if (NODE_type(p) == HLIST_NODE && mem[p].b16.s0 == DLIST)
                        print_cstr(", display");
                }

                str_pool[pool_ptr] = '.' ;
                pool_ptr++;
                show_node_list(mem[p + 5].b32.s1);
                pool_ptr--;
                break;

            case RULE_NODE:
                print_esc_cstr("rule(");
                print_rule_dimen(mem[p + 3].b32.s1);
                print_char('+');
                print_rule_dimen(mem[p + 2].b32.s1);
                print_cstr(")x");
                print_rule_dimen(mem[p + 1].b32.s1);
                break;

            case INS_NODE:
                print_esc_cstr("insert");
                print_int(mem[p].b16.s0);
                print_cstr(", natural size ");
                print_scaled(mem[p + 3].b32.s1);
                print_cstr("; split(");
                print_spec(mem[p + 4].b32.s1, NULL);
                print_char(',');
                print_scaled(mem[p + 2].b32.s1);
                print_cstr("); float cost ");
                print_int(mem[p + 1].b32.s1);
                str_pool[pool_ptr] = '.' ;
                pool_ptr++;
                show_node_list(mem[p + 4].b32.s0);
                pool_ptr--;
                break;

            case WHATSIT_NODE:
                switch (mem[p].b16.s0) {
                case OPEN_NODE:
                    print_write_whatsit("openout", p);
                    print_char('=');
                    print_file_name(mem[p + 1].b32.s1, mem[p + 2].b32.s0, mem[p + 2].b32.s1);
                    break;
                case WRITE_NODE:
                    print_write_whatsit("write", p);
                    print_mark(mem[p + 1].b32.s1);
                    break;
                case CLOSE_NODE:
                    print_write_whatsit("closeout", p);
                    break;
                case SPECIAL_NODE:
                    print_esc_cstr("special");
                    print_mark(mem[p + 1].b32.s1);
                    break;
                case LANGUAGE_NODE:
                    print_esc_cstr("setlanguage");
                    print_int(mem[p + 1].b32.s1);
                    print_cstr(" (hyphenmin ");
                    print_int(mem[p + 1].b16.s1);
                    print_char(',');
                    print_int(mem[p + 1].b16.s0);
                    print_char(')');
                    break;
                case PDF_SAVE_POS_NODE:
                    print_esc_cstr("pdfsavepos");
                    break;
                case NATIVE_WORD_NODE:
                case NATIVE_WORD_NODE_AT:
                    print_esc(hash[FONT_ID_BASE + mem[p + 4].b16.s2].s1);
                    print_char(' ');
                    print_native_word(p);
                    break;
                case GLYPH_NODE:
                    print_esc(hash[FONT_ID_BASE + mem[p + 4].b16.s2].s1);
                    print_cstr(" glyph#");
                    print_int(mem[p + 4].b16.s1);
                    break;
                case PIC_NODE:
                case PDF_NODE:
                    if (mem[p].b16.s0 == PIC_NODE)
                        print_esc_cstr("XeTeXpicfile");
                    else
                        print_esc_cstr("XeTeXpdffile");

                    print_cstr("( ");
                    for (i = 0; i < PIC_NODE_path_len(p); i++)
                        print_raw_char(PIC_NODE_path(p)[i], true);
                    print('"');
                    break;
                default:
                    print_cstr("whatsit?");
                    break;
                }
                break; /* WHATSIT_NODE */

            case GLUE_NODE:
                if (mem[p].b16.s0 >= A_LEADERS) {      /*198: */
                    print_esc_cstr("");
                    if (mem[p].b16.s0 == C_LEADERS)
                        print_char('c');
                    else if (mem[p].b16.s0 == X_LEADERS)
                        print_char('x');
                    print_cstr("leaders ");
                    print_spec(mem[p + 1].b32.s0, NULL);
                    str_pool[pool_ptr] = '.' ;
                    pool_ptr++;
                    show_node_list(mem[p + 1].b32.s1);
                    pool_ptr--;
                } else {
                    print_esc_cstr("glue");

                    if (GLUE_SPEC_shrink_order(p) != NORMAL) {
                        print_char('(');
                        if (mem[p].b16.s0 < COND_MATH_GLUE)
                            print_skip_param(mem[p].b16.s0 - 1);
                        else if (mem[p].b16.s0 == COND_MATH_GLUE)
                            print_esc_cstr("nonscript");
                        else
                            print_esc_cstr("mskip");
                        print_char(')');
                    }

                    if (mem[p].b16.s0 != COND_MATH_GLUE) {
                        print_char(' ');
                        if (mem[p].b16.s0 < COND_MATH_GLUE)
                            print_spec(mem[p + 1].b32.s0, NULL);
                        else
                            print_spec(mem[p + 1].b32.s0, "mu");
                    }
                }
                break;

            case KERN_NODE:
                if (mem[p].b16.s0 != MU_GLUE) {
                    print_esc_cstr("kern");
                    if (mem[p].b16.s0 != NORMAL)
                        print_char(' ');
                    print_scaled(mem[p + 1].b32.s1);
                    if (NODE_subtype(p) == ACC_KERN)
                        print_cstr(" (for accent)");
                    else if (NODE_subtype(p) == SPACE_ADJUSTMENT)
                        print_cstr(" (space adjustment)");
                } else {
                    print_esc_cstr("mkern");
                    print_scaled(mem[p + 1].b32.s1);
                    print_cstr("mu");
                }
                break;

            case MARGIN_KERN_NODE:
                print_esc_cstr("kern");
                print_scaled(mem[p + 1].b32.s1);
                if (mem[p].b16.s0 == 0)
                    print_cstr(" (left margin)");
                else
                    print_cstr(" (right margin)");
                break;

            case MATH_NODE:
                if (mem[p].b16.s0 > AFTER) {
                    if (odd(mem[p].b16.s0))
                        print_esc_cstr("end");
                    else
                        print_esc_cstr("begin");
                    if (mem[p].b16.s0 > R_CODE)
                        print_char('R');
                    else if (mem[p].b16.s0 > L_CODE)
                        print_char('L');
                    else
                        print_char('M');
                } else {
                    print_esc_cstr("math");
                    if (mem[p].b16.s0 == BEFORE)
                        print_cstr("on");
                    else
                        print_cstr("off");
                    if (mem[p + 1].b32.s1 != 0) {
                        print_cstr(", surrounded ");
                        print_scaled(mem[p + 1].b32.s1);
                    }
                }
                break;

            case LIGATURE_NODE:
                print_font_and_char(p + 1);
                print_cstr(" (ligature ");
                if (mem[p].b16.s0 > 1)
                    print_char('|');
                font_in_short_display = mem[p + 1].b16.s1;
                short_display(mem[p + 1].b32.s1);
                if (odd(mem[p].b16.s0))
                    print_char('|');
                print_char(')');
                break;

            case PENALTY_NODE:
                print_esc_cstr("penalty ");
                print_int(mem[p + 1].b32.s1);
                break;

            case DISC_NODE:
                print_esc_cstr("discretionary");
                if (mem[p].b16.s0 > 0) {
                    print_cstr(" replacing ");
                    print_int(mem[p].b16.s0);
                }

                str_pool[pool_ptr] = '.' ;
                pool_ptr++;
                show_node_list(mem[p + 1].b32.s0);
                pool_ptr--;
                str_pool[pool_ptr] = '|' ;
                pool_ptr++;
                show_node_list(mem[p + 1].b32.s1);
                pool_ptr--;
                break;

            case MARK_NODE:
                print_esc_cstr("mark");
                if (mem[p + 1].b32.s0 != 0) {
                    print_char('s');
                    print_int(mem[p + 1].b32.s0);
                }
                print_mark(mem[p + 1].b32.s1);
                break;

            case ADJUST_NODE:
                print_esc_cstr("vadjust");
                if (mem[p].b16.s0 != 0)
                    print_cstr(" pre ");

                str_pool[pool_ptr] = '.' ;
                pool_ptr++;
                show_node_list(mem[p + 1].b32.s1);
                pool_ptr--;
                break;

            case STYLE_NODE:
                print_style(mem[p].b16.s0);
                break;

            case CHOICE_NODE:
                print_esc_cstr("mathchoice");
                str_pool[pool_ptr] = 'D' ;
                pool_ptr++;
                show_node_list(mem[p + 1].b32.s0);
                pool_ptr--;
                str_pool[pool_ptr] = 'T' ;
                pool_ptr++;
                show_node_list(mem[p + 1].b32.s1);
                pool_ptr--;
                str_pool[pool_ptr] = 'S' ;
                pool_ptr++;
                show_node_list(mem[p + 2].b32.s0);
                pool_ptr--;
                str_pool[pool_ptr] = 's' ;
                pool_ptr++;
                show_node_list(mem[p + 2].b32.s1);
                pool_ptr--;
                break;

            case ORD_NOAD:
            case OP_NOAD:
            case BIN_NOAD:
            case REL_NOAD:
            case OPEN_NOAD:
            case CLOSE_NOAD:
            case PUNCT_NOAD:
            case INNER_NOAD:
            case RADICAL_NOAD:
            case OVER_NOAD:
            case UNDER_NOAD:
            case VCENTER_NOAD:
            case ACCENT_NOAD:
            case LEFT_NOAD:
            case RIGHT_NOAD:
                {
                    switch (mem[p].b16.s1) {
                    case ORD_NOAD:
                        print_esc_cstr("mathord");
                        break;
                    case OP_NOAD:
                        print_esc_cstr("mathop");
                        break;
                    case BIN_NOAD:
                        print_esc_cstr("mathbin");
                        break;
                    case REL_NOAD:
                        print_esc_cstr("mathrel");
                        break;
                    case OPEN_NOAD:
                        print_esc_cstr("mathopen");
                        break;
                    case CLOSE_NOAD:
                        print_esc_cstr("mathclose");
                        break;
                    case PUNCT_NOAD:
                        print_esc_cstr("mathpunct");
                        break;
                    case INNER_NOAD:
                        print_esc_cstr("mathinner");
                        break;
                    case OVER_NOAD:
                        print_esc_cstr("overline");
                        break;
                    case UNDER_NOAD:
                        print_esc_cstr("underline");
                        break;
                    case VCENTER_NOAD:
                        print_esc_cstr("vcenter");
                        break;
                    case RADICAL_NOAD:
                        print_esc_cstr("radical");
                        print_delimiter(p + 4);
                        break;
                    case ACCENT_NOAD:
                        print_esc_cstr("accent");
                        print_fam_and_char(p + 4);
                        break;
                    case LEFT_NOAD:
                        print_esc_cstr("left");
                        print_delimiter(p + 1);
                        break;
                    case RIGHT_NOAD:
                        if (mem[p].b16.s0 == NORMAL)
                            print_esc_cstr("right");
                        else
                            print_esc_cstr("middle");
                        print_delimiter(p + 1);
                        break;
                    }

                    if (mem[p].b16.s1 < LEFT_NOAD) {
                        if (mem[p].b16.s0 != NORMAL) {
                            if (mem[p].b16.s0 == LIMITS)
                                print_esc_cstr("limits");
                            else
                                print_esc_cstr("nolimits");
                        }
                        print_subsidiary_data(p + 1, '.' );
                    }

                    print_subsidiary_data(p + 2, '^' );
                    print_subsidiary_data(p + 3, '_' );
                }
                break; /* many math noads */

            case FRACTION_NOAD:
                print_esc_cstr("fraction, thickness ");
                if (mem[p + 1].b32.s1 == DEFAULT_CODE)
                    print_cstr("= default");
                else
                    print_scaled(mem[p + 1].b32.s1);

                if (mem[p + 4].b16.s3 % 256 != 0 ||
                    (mem[p + 4].b16.s2 + (mem[p + 4].b16.s3 / 256) * 65536L) != 0 ||
                    mem[p + 4].b16.s1 % 256 != 0 ||
                    (mem[p + 4].b16.s0 + (mem[p + 4].b16.s1 / 256) * 65536L) != 0) {
                    print_cstr(", left-delimiter ");
                    print_delimiter(p + 4);
                }

                if (mem[p + 5].b16.s3 % 256 != 0 ||
                    (mem[p + 5].b16.s2 + (mem[p + 5].b16.s3 / 256) * 65536L) != 0 ||
                    mem[p + 5].b16.s1 % 256 != 0 ||
                    (mem[p + 5].b16.s0 + (mem[p + 5].b16.s1 / 256) * 65536L) != 0) {
                    print_cstr(", right-delimiter ");
                    print_delimiter(p + 5);
                }

                print_subsidiary_data(p + 2, '\\' );
                print_subsidiary_data(p + 3, '/' );
                break;

            default:
                print_cstr("Unknown node type!");
                break;
            }
        }

        p = LLIST_link(p);
    }
}


void show_box(int32_t p)
{

    depth_threshold = INTPAR(show_box_depth);
    breadth_max = INTPAR(show_box_breadth) /*:244 */ ;
    if (breadth_max <= 0)
        breadth_max = 5;
    if (pool_ptr + depth_threshold >= pool_size)
        depth_threshold = pool_size - pool_ptr - 1;
    show_node_list(p);
    print_ln();
}

void short_display_n(int32_t p, int32_t m)
{
    breadth_max = m;
    depth_threshold = pool_size - pool_ptr - 1;
    show_node_list(p);
}

void delete_token_ref(int32_t p)
{
    if (mem[p].b32.s0 == TEX_NULL)
        flush_list(p);
    else
        mem[p].b32.s0--;
}

void delete_glue_ref(int32_t p)
{
    if (mem[p].b32.s1 == TEX_NULL)
        free_node(p, GLUE_SPEC_SIZE);
    else
        mem[p].b32.s1--;
}


void
flush_node_list(int32_t p)
{
    int32_t q;

    while (p != TEX_NULL) {
        q = mem[p].b32.s1;

        if (is_char_node(p)) {
            mem[p].b32.s1 = avail;
            avail = p;
        } else {
            switch (mem[p].b16.s1) {
            case HLIST_NODE:
            case VLIST_NODE:
            case UNSET_NODE:
                flush_node_list(mem[p + 5].b32.s1);
                free_node(p, BOX_NODE_SIZE);
                goto done;
                break;

            case RULE_NODE:
                free_node(p, RULE_NODE_SIZE);
                goto done;
                break;

            case INS_NODE:
                flush_node_list(mem[p + 4].b32.s0);
                delete_glue_ref(INSERTION_NODE_split_top_ptr(p));
                free_node(p, INS_NODE_SIZE);
                goto done;
                break;

            case WHATSIT_NODE:
                switch (mem[p].b16.s0) {
                case OPEN_NODE:
                    free_node(p, OPEN_NODE_SIZE);
                    break;
                case WRITE_NODE:
                case SPECIAL_NODE:
                    delete_token_ref(mem[p + 1].b32.s1);
                    free_node(p, WRITE_NODE_SIZE);
                    goto done;
                    break;
                case CLOSE_NODE:
                case LANGUAGE_NODE:
                    free_node(p, SMALL_NODE_SIZE);
                    break;
                case NATIVE_WORD_NODE:
                case NATIVE_WORD_NODE_AT:
                    if (NATIVE_NODE_glyph_info_ptr(p) != NULL) {
                        NATIVE_NODE_glyph_info_ptr(p) = mfree(NATIVE_NODE_glyph_info_ptr(p));
                        NATIVE_NODE_glyph_count(p) = 0;
                    }
                    free_node(p, NATIVE_NODE_size(p));
                    break;
                case GLYPH_NODE:
                    free_node(p, GLYPH_NODE_SIZE);
                    break;
                case PIC_NODE:
                case PDF_NODE:
                    free_node(p,
                              (PIC_NODE_SIZE +
                               (mem[p + 4].b16.s1 + sizeof(memory_word) - 1) / sizeof(memory_word)));
                    break;
                case PDF_SAVE_POS_NODE:
                    free_node(p, SMALL_NODE_SIZE);
                    break;
                default:
                    confusion("ext3");
                    break;
                }
                goto done;
                break;

            case GLUE_NODE:
                if (mem[mem[p + 1].b32.s0].b32.s1 == TEX_NULL)
                    free_node(mem[p + 1].b32.s0, GLUE_SPEC_SIZE);
                else
                    mem[mem[p + 1].b32.s0].b32.s1--;

                if (mem[p + 1].b32.s1 != TEX_NULL)
                    flush_node_list(mem[p + 1].b32.s1);
                free_node(p, MEDIUM_NODE_SIZE);
                goto done;
                break;

            case KERN_NODE:
            case MATH_NODE:
            case PENALTY_NODE:
                free_node(p, MEDIUM_NODE_SIZE);
                goto done;
                break;

            case MARGIN_KERN_NODE:
                free_node(p, MARGIN_KERN_NODE_SIZE);
                goto done;
                break;

            case LIGATURE_NODE:
                flush_node_list(mem[p + 1].b32.s1);
                break;

            case MARK_NODE:
                delete_token_ref(mem[p + 1].b32.s1);
                break;

            case DISC_NODE:
                flush_node_list(mem[p + 1].b32.s0);
                flush_node_list(mem[p + 1].b32.s1);
                break;

            case ADJUST_NODE:
                flush_node_list(mem[p + 1].b32.s1);
                break;

            case STYLE_NODE:
                free_node(p, STYLE_NODE_SIZE);
                goto done;
                break;

            case CHOICE_NODE:
                flush_node_list(mem[p + 1].b32.s0);
                flush_node_list(mem[p + 1].b32.s1);
                flush_node_list(mem[p + 2].b32.s0);
                flush_node_list(mem[p + 2].b32.s1);
                free_node(p, STYLE_NODE_SIZE);
                goto done;
                break;

            case ORD_NOAD:
            case OP_NOAD:
            case BIN_NOAD:
            case REL_NOAD:
            case OPEN_NOAD:
            case CLOSE_NOAD:
            case PUNCT_NOAD:
            case INNER_NOAD:
            case RADICAL_NOAD:
            case OVER_NOAD:
            case UNDER_NOAD:
            case VCENTER_NOAD:
            case ACCENT_NOAD:
                if (mem[p + 1].b32.s1 >= SUB_BOX)
                    flush_node_list(mem[p + 1].b32.s0);
                if (mem[p + 2].b32.s1 >= SUB_BOX)
                    flush_node_list(mem[p + 2].b32.s0);
                if (mem[p + 3].b32.s1 >= SUB_BOX)
                    flush_node_list(mem[p + 3].b32.s0);
                if (mem[p].b16.s1 == RADICAL_NOAD)
                    free_node(p, RADICAL_NOAD_SIZE);
                else if (mem[p].b16.s1 == ACCENT_NOAD)
                    free_node(p, ACCENT_NOAD_SIZE);
                else
                    free_node(p, NOAD_SIZE);
                goto done;
                break;

            case LEFT_NOAD:
            case RIGHT_NOAD:
                free_node(p, NOAD_SIZE);
                goto done;
                break;

            case FRACTION_NOAD:
                flush_node_list(mem[p + 2].b32.s0);
                flush_node_list(mem[p + 3].b32.s0);
                free_node(p, FRACTION_NOAD_SIZE);
                goto done;
                break;

            default:
                confusion("flushing");
                break;
            }

            free_node(p, SMALL_NODE_SIZE);
        done:
            ;
        }

        p = q;
    }
}


int32_t
copy_node_list(int32_t p)
{
    int32_t h;
    int32_t q;
    int32_t r;
    unsigned char words;

    h = get_avail();
    q = h;

    while (p != TEX_NULL) {
        words = 1;
        if (is_char_node(p)) {
            r = get_avail();
        } else { /*214:*/
            switch (mem[p].b16.s1) {
            case HLIST_NODE:
            case VLIST_NODE:
            case UNSET_NODE:
                r = get_node(BOX_NODE_SIZE);
                SYNCTEX_tag(r, BOX_NODE_SIZE) = SYNCTEX_tag(p, BOX_NODE_SIZE);
                SYNCTEX_line(r, BOX_NODE_SIZE) = SYNCTEX_line(p, BOX_NODE_SIZE);
                mem[r + 6] = mem[p + 6];
                mem[r + 5] = mem[p + 5];
                mem[r + 5].b32.s1 = copy_node_list(mem[p + 5].b32.s1);
                words = 5;
                break;

            case RULE_NODE:
                r = get_node(RULE_NODE_SIZE);
                words = (RULE_NODE_SIZE - 1);
                break;

            case INS_NODE:
                r = get_node(INS_NODE_SIZE);
                mem[r + 4] = mem[p + 4];
                GLUE_SPEC_ref_count(mem[p + 4].b32.s1)++;
                mem[r + 4].b32.s0 = copy_node_list(mem[p + 4].b32.s0);
                words = (INS_NODE_SIZE - 1);
                break;

            case WHATSIT_NODE:
                switch (mem[p].b16.s0) {
                case OPEN_NODE:
                    r = get_node(OPEN_NODE_SIZE);
                    words = OPEN_NODE_SIZE;
                    break;
                case WRITE_NODE:
                case SPECIAL_NODE:
                    r = get_node(WRITE_NODE_SIZE);
                    mem[mem[p + 1].b32.s1].b32.s0++;
                    words = WRITE_NODE_SIZE;
                    break;
                case CLOSE_NODE:
                case LANGUAGE_NODE:
                    r = get_node(SMALL_NODE_SIZE);
                    words = SMALL_NODE_SIZE;
                    break;
                case NATIVE_WORD_NODE:
                case NATIVE_WORD_NODE_AT:
                    words = NATIVE_NODE_size(p);
                    r = get_node(words);

                    while (words > 0) {
                        words--;
                        mem[r + words] = mem[p + words];
                    }

                    NATIVE_NODE_glyph_info_ptr(r) = NULL;
                    NATIVE_NODE_glyph_count(r) = 0;
                    copy_native_glyph_info(p, r);
                    break;
                case GLYPH_NODE:
                    r = get_node(GLYPH_NODE_SIZE);
                    words = GLYPH_NODE_SIZE;
                    break;
                case PIC_NODE:
                case PDF_NODE:
                    words =
                        (PIC_NODE_SIZE +
                         (mem[p + 4].b16.s1 + sizeof(memory_word) - 1) / sizeof(memory_word));
                    r = get_node(words);
                    break;
                case PDF_SAVE_POS_NODE:
                    r = get_node(SMALL_NODE_SIZE);
                    break;
                default:
                    confusion("ext2");
                    break;
                }
                break;

            case GLUE_NODE:
                r = get_node(MEDIUM_NODE_SIZE);
                GLUE_SPEC_ref_count(mem[p + 1].b32.s0)++;
                mem[r + 2].b32.s0 = mem[p + 2].b32.s0;
                mem[r + 2].b32.s1 = mem[p + 2].b32.s1;
                mem[r + 1].b32.s0 = mem[p + 1].b32.s0;
                mem[r + 1].b32.s1 = copy_node_list(mem[p + 1].b32.s1);
                break;

            case KERN_NODE:
            case MATH_NODE:
            case PENALTY_NODE:
                r = get_node(MEDIUM_NODE_SIZE);
                words = MEDIUM_NODE_SIZE;
                break;

            case MARGIN_KERN_NODE:
                r = get_node(MARGIN_KERN_NODE_SIZE);
                words = MARGIN_KERN_NODE_SIZE;
                break;

            case LIGATURE_NODE:
                r = get_node(SMALL_NODE_SIZE);
                mem[r + 1] = mem[p + 1];
                mem[r + 1].b32.s1 = copy_node_list(mem[p + 1].b32.s1);
                break;

            case DISC_NODE:
                r = get_node(SMALL_NODE_SIZE);
                mem[r + 1].b32.s0 = copy_node_list(mem[p + 1].b32.s0);
                mem[r + 1].b32.s1 = copy_node_list(mem[p + 1].b32.s1);
                break;

            case MARK_NODE:
                r = get_node(SMALL_NODE_SIZE);
                mem[mem[p + 1].b32.s1].b32.s0++;
                words = SMALL_NODE_SIZE;
                break;

            case ADJUST_NODE:
                r = get_node(SMALL_NODE_SIZE);
                mem[r + 1].b32.s1 = copy_node_list(mem[p + 1].b32.s1);
                break;

            default:
                confusion("copying");
                break;
            }
        }

        while (words > 0) {
            words--;
            mem[r + words] = mem[p + words];
        }

        mem[q].b32.s1 = r;
        q = r;
        p = LLIST_link(p);
    }

    mem[q].b32.s1 = TEX_NULL;
    q = mem[h].b32.s1;
    mem[h].b32.s1 = avail;
    avail = h;
    return q;
}


void print_mode(int32_t m)
{
    if (m > 0)
        switch (m / ((MAX_COMMAND + 1))) {
        case 0:
            print_cstr("vertical mode");
            break;
        case 1:
            print_cstr("horizontal mode");
            break;
        case 2:
            print_cstr("display math mode");
            break;
    } else if (m == 0)
        print_cstr("no mode");
    else
        switch ((-(int32_t) m) / ((MAX_COMMAND + 1))) {
        case 0:
            print_cstr("internal vertical mode");
            break;
        case 1:
            print_cstr("restricted horizontal mode");
            break;
        case 2:
            print_cstr("math mode");
            break;
        }
}

void print_in_mode(int32_t m)
{
    if (m > 0)
        switch (m / ((MAX_COMMAND + 1))) {
        case 0:
            print_cstr("' in vertical mode");
            break;
        case 1:
            print_cstr("' in horizontal mode");
            break;
        case 2:
            print_cstr("' in display math mode");
            break;
    } else if (m == 0)
        print_cstr("' in no mode");
    else
        switch ((-(int32_t) m) / ((MAX_COMMAND + 1))) {
        case 0:
            print_cstr("' in internal vertical mode");
            break;
        case 1:
            print_cstr("' in restricted horizontal mode");
            break;
        case 2:
            print_cstr("' in math mode");
            break;
        }
}

void push_nest(void)
{
    if (nest_ptr > max_nest_stack) {
        max_nest_stack = nest_ptr;
        if (nest_ptr == nest_size)
            overflow("semantic nest size", nest_size);
    }
    nest[nest_ptr] = cur_list;
    nest_ptr++;
    cur_list.head = get_avail();
    cur_list.tail = cur_list.head;
    cur_list.prev_graf = 0;
    cur_list.mode_line = line;
    cur_list.eTeX_aux = TEX_NULL;
}

void pop_nest(void)
{
    {
        mem[cur_list.head].b32.s1 = avail;
        avail = cur_list.head;
    }
    nest_ptr--;
    cur_list = nest[nest_ptr];
}

void show_activities(void)
{
    int32_t p;
    short /*mmode */ m;
    memory_word a;
    int32_t q, r;
    int32_t t;

    nest[nest_ptr] = cur_list;
    print_nl_cstr("");
    print_ln();
    {
        register int32_t for_end;
        p = nest_ptr;
        for_end = 0;
        if (p >= for_end)
            do {
                m = nest[p].mode;
                a = nest[p].aux;
                print_nl_cstr("### ");
                print_mode(m);
                print_cstr(" entered at line ");
                print_int(abs(nest[p].mode_line));
                if (m == HMODE) {

                    if (nest[p].prev_graf != 0x830000) {
                        print_cstr(" (language");
                        print_int(nest[p].prev_graf % 65536L);
                        print_cstr(":hyphenmin");
                        print_int(nest[p].prev_graf / 0x400000);
                        print_char(',');
                        print_int((nest[p].prev_graf / 65536L) % 64);
                        print_char(')');
                    }
                }
                if (nest[p].mode_line < 0)
                    print_cstr(" (\\output routine)");
                if (p == 0) {
                    if (PAGE_HEAD != page_tail) {
                        print_nl_cstr("### current page:");
                        if (output_active)
                            print_cstr(" (held over for next output)");
                        show_box(mem[PAGE_HEAD].b32.s1);
                        if (page_contents > EMPTY) {
                            print_nl_cstr("total height ");
                            print_totals();
                            print_nl_cstr(" goal height ");
                            print_scaled(page_so_far[0]);
                            r = mem[PAGE_INS_HEAD].b32.s1;
                            while (r != PAGE_INS_HEAD) {

                                print_ln();
                                print_esc_cstr("insert");
                                t = mem[r].b16.s0;
                                print_int(t);
                                print_cstr(" adds ");
                                if (COUNT_REG(t) == 1000)
                                    t = mem[r + 3].b32.s1;
                                else
                                    t = x_over_n(mem[r + 3].b32.s1, 1000) * COUNT_REG(t);
                                print_scaled(t);
                                if (mem[r].b16.s1 == SPLIT_UP) {
                                    q = PAGE_HEAD;
                                    t = 0;
                                    do {
                                        q = LLIST_link(q);
                                        if ((NODE_type(q) == INS_NODE) && (mem[q].b16.s0 == mem[r].b16.s0))
                                            t++;
                                    } while (!(q == mem[r + 1].b32.s0));
                                    print_cstr(", #");
                                    print_int(t);
                                    print_cstr(" might split");
                                }
                                r = LLIST_link(r);
                            }
                        }
                    }
                    if (mem[CONTRIB_HEAD].b32.s1 != TEX_NULL)
                        print_nl_cstr("### recent contributions:");
                }
                show_box(mem[nest[p].head].b32.s1);
                switch (abs(m) / ((MAX_COMMAND + 1))) {
                case 0:
                    {
                        print_nl_cstr("prevdepth ");
                        if (a.b32.s1 <= IGNORE_DEPTH)
                            print_cstr("ignored");
                        else
                            print_scaled(a.b32.s1);
                        if (nest[p].prev_graf != 0) {
                            print_cstr(", prevgraf ");
                            print_int(nest[p].prev_graf);
                            if (nest[p].prev_graf != 1)
                                print_cstr(" lines");
                            else
                                print_cstr(" line");
                        }
                    }
                    break;
                case 1:
                    {
                        print_nl_cstr("spacefactor ");
                        print_int(a.b32.s0);
                        if (m > 0) {

                            if (a.b32.s1 > 0) {
                                print_cstr(", current language ");
                                print_int(a.b32.s1);
                            }
                        }
                    }
                    break;
                case 2:
                    if (a.b32.s1 != TEX_NULL) {
                        print_cstr("this will be denominator of:");
                        show_box(a.b32.s1);
                    }
                    break;
                }
            }
            while (p-- > for_end);
    }
}

void print_param(int32_t n)
{
    switch (n) {
    case INT_PAR__pretolerance:
        print_esc_cstr("pretolerance");
        break;
    case INT_PAR__tolerance:
        print_esc_cstr("tolerance");
        break;
    case INT_PAR__line_penalty:
        print_esc_cstr("linepenalty");
        break;
    case INT_PAR__hyphen_penalty:
        print_esc_cstr("hyphenpenalty");
        break;
    case INT_PAR__ex_hyphen_penalty:
        print_esc_cstr("exhyphenpenalty");
        break;
    case INT_PAR__club_penalty:
        print_esc_cstr("clubpenalty");
        break;
    case INT_PAR__widow_penalty:
        print_esc_cstr("widowpenalty");
        break;
    case INT_PAR__display_widow_penalty:
        print_esc_cstr("displaywidowpenalty");
        break;
    case INT_PAR__broken_penalty:
        print_esc_cstr("brokenpenalty");
        break;
    case INT_PAR__bin_op_penalty:
        print_esc_cstr("binoppenalty");
        break;
    case INT_PAR__rel_penalty:
        print_esc_cstr("relpenalty");
        break;
    case INT_PAR__pre_display_penalty:
        print_esc_cstr("predisplaypenalty");
        break;
    case INT_PAR__post_display_penalty:
        print_esc_cstr("postdisplaypenalty");
        break;
    case INT_PAR__inter_line_penalty:
        print_esc_cstr("interlinepenalty");
        break;
    case INT_PAR__double_hyphen_demerits:
        print_esc_cstr("doublehyphendemerits");
        break;
    case INT_PAR__final_hyphen_demerits:
        print_esc_cstr("finalhyphendemerits");
        break;
    case INT_PAR__adj_demerits:
        print_esc_cstr("adjdemerits");
        break;
    case INT_PAR__mag:
        print_esc_cstr("mag");
        break;
    case INT_PAR__delimiter_factor:
        print_esc_cstr("delimiterfactor");
        break;
    case INT_PAR__looseness:
        print_esc_cstr("looseness");
        break;
    case INT_PAR__time:
        print_esc_cstr("time");
        break;
    case INT_PAR__day:
        print_esc_cstr("day");
        break;
    case INT_PAR__month:
        print_esc_cstr("month");
        break;
    case INT_PAR__year:
        print_esc_cstr("year");
        break;
    case INT_PAR__show_box_breadth:
        print_esc_cstr("showboxbreadth");
        break;
    case INT_PAR__show_box_depth:
        print_esc_cstr("showboxdepth");
        break;
    case INT_PAR__hbadness:
        print_esc_cstr("hbadness");
        break;
    case INT_PAR__vbadness:
        print_esc_cstr("vbadness");
        break;
    case INT_PAR__pausing:
        print_esc_cstr("pausing");
        break;
    case INT_PAR__tracing_online:
        print_esc_cstr("tracingonline");
        break;
    case INT_PAR__tracing_macros:
        print_esc_cstr("tracingmacros");
        break;
    case INT_PAR__tracing_stats:
        print_esc_cstr("tracingstats");
        break;
    case INT_PAR__tracing_paragraphs:
        print_esc_cstr("tracingparagraphs");
        break;
    case INT_PAR__tracing_pages:
        print_esc_cstr("tracingpages");
        break;
    case INT_PAR__tracing_output:
        print_esc_cstr("tracingoutput");
        break;
    case INT_PAR__tracing_lost_chars:
        print_esc_cstr("tracinglostchars");
        break;
    case INT_PAR__tracing_commands:
        print_esc_cstr("tracingcommands");
        break;
    case INT_PAR__tracing_restores:
        print_esc_cstr("tracingrestores");
        break;
    case INT_PAR__uc_hyph:
        print_esc_cstr("uchyph");
        break;
    case INT_PAR__output_penalty:
        print_esc_cstr("outputpenalty");
        break;
    case INT_PAR__max_dead_cycles:
        print_esc_cstr("maxdeadcycles");
        break;
    case INT_PAR__hang_after:
        print_esc_cstr("hangafter");
        break;
    case INT_PAR__floating_penalty:
        print_esc_cstr("floatingpenalty");
        break;
    case INT_PAR__global_defs:
        print_esc_cstr("globaldefs");
        break;
    case INT_PAR__cur_fam:
        print_esc_cstr("fam");
        break;
    case INT_PAR__escape_char:
        print_esc_cstr("escapechar");
        break;
    case INT_PAR__default_hyphen_char:
        print_esc_cstr("defaulthyphenchar");
        break;
    case INT_PAR__default_skew_char:
        print_esc_cstr("defaultskewchar");
        break;
    case INT_PAR__end_line_char:
        print_esc_cstr("endlinechar");
        break;
    case INT_PAR__new_line_char:
        print_esc_cstr("newlinechar");
        break;
    case INT_PAR__language:
        print_esc_cstr("language");
        break;
    case INT_PAR__left_hyphen_min:
        print_esc_cstr("lefthyphenmin");
        break;
    case INT_PAR__right_hyphen_min:
        print_esc_cstr("righthyphenmin");
        break;
    case INT_PAR__holding_inserts:
        print_esc_cstr("holdinginserts");
        break;
    case INT_PAR__error_context_lines:
        print_esc_cstr("errorcontextlines");
        break;
    case INT_PAR__char_sub_def_min:
        print_esc_cstr("charsubdefmin");
        break;
    case INT_PAR__char_sub_def_max:
        print_esc_cstr("charsubdefmax");
        break;
    case INT_PAR__tracing_char_sub_def:
        print_esc_cstr("tracingcharsubdef");
        break;
    case INT_PAR__xetex_linebreak_penalty:
        print_esc_cstr("XeTeXlinebreakpenalty");
        break;
    case INT_PAR__xetex_protrude_chars:
        print_esc_cstr("XeTeXprotrudechars");
        break;
    case INT_PAR__synctex:
        print_esc_cstr("synctex");
        break;
    case INT_PAR__tracing_assigns:
        print_esc_cstr("tracingassigns");
        break;
    case INT_PAR__tracing_groups:
        print_esc_cstr("tracinggroups");
        break;
    case INT_PAR__tracing_ifs:
        print_esc_cstr("tracingifs");
        break;
    case INT_PAR__tracing_scan_tokens:
        print_esc_cstr("tracingscantokens");
        break;
    case INT_PAR__tracing_nesting:
        print_esc_cstr("tracingnesting");
        break;
    case INT_PAR__pre_display_correction:
        print_esc_cstr("predisplaydirection");
        break;
    case INT_PAR__last_line_fit:
        print_esc_cstr("lastlinefit");
        break;
    case INT_PAR__saving_vdiscards:
        print_esc_cstr("savingvdiscards");
        break;
    case INT_PAR__saving_hyphs:
        print_esc_cstr("savinghyphcodes");
        break;
    case INT_PAR__suppress_fontnotfound_error:
        print_esc_cstr("suppressfontnotfounderror");
        break;
    case INT_PAR__texxet:
        print_esc_cstr("TeXXeTstate");
        break;
    case INT_PAR__xetex_upwards:
        print_esc_cstr("XeTeXupwardsmode");
        break;
    case INT_PAR__xetex_use_glyph_metrics:
        print_esc_cstr("XeTeXuseglyphmetrics");
        break;
    case INT_PAR__xetex_inter_char_tokens:
        print_esc_cstr("XeTeXinterchartokenstate");
        break;
    case INT_PAR__xetex_dash_break:
        print_esc_cstr("XeTeXdashbreakstate");
        break;
    case INT_PAR__xetex_input_normalization:
        print_esc_cstr("XeTeXinputnormalization");
        break;
    case INT_PAR__xetex_tracing_fonts:
        print_esc_cstr("XeTeXtracingfonts");
        break;
    case INT_PAR__xetex_interword_space_shaping:
        print_esc_cstr("XeTeXinterwordspaceshaping");
        break;
    case INT_PAR__xetex_generate_actual_text:
        print_esc_cstr("XeTeXgenerateactualtext");
        break;
    case INT_PAR__xetex_hyphenatable_length:
        print_esc_cstr("XeTeXhyphenatablelength");
        break;
    case INT_PAR__pdfoutput:
        print_esc_cstr("pdfoutput");
        break;
    default:
        print_cstr("[unknown int32_t parameter!]");
        break;
    }
}

void begin_diagnostic(void)
{

    old_setting = selector;

    if (INTPAR(tracing_online) <= 0 && selector == SELECTOR_TERM_AND_LOG) {
        selector--;
        if (history == HISTORY_SPOTLESS)
            history = HISTORY_WARNING_ISSUED;
    }
}

void end_diagnostic(bool blank_line)
{
    print_nl_cstr("");
    if (blank_line)
        print_ln();
    selector = old_setting;
}

void print_length_param(int32_t n)
{
    switch (n) {
    case DIMEN_PAR__par_indent:
        print_esc_cstr("parindent");
        break;
    case DIMEN_PAR__math_surround:
        print_esc_cstr("mathsurround");
        break;
    case DIMEN_PAR__line_skip_limit:
        print_esc_cstr("lineskiplimit");
        break;
    case DIMEN_PAR__hsize:
        print_esc_cstr("hsize");
        break;
    case DIMEN_PAR__vsize:
        print_esc_cstr("vsize");
        break;
    case DIMEN_PAR__max_depth:
        print_esc_cstr("maxdepth");
        break;
    case DIMEN_PAR__split_max_depth:
        print_esc_cstr("splitmaxdepth");
        break;
    case DIMEN_PAR__box_max_depth:
        print_esc_cstr("boxmaxdepth");
        break;
    case DIMEN_PAR__hfuzz:
        print_esc_cstr("hfuzz");
        break;
    case DIMEN_PAR__vfuzz:
        print_esc_cstr("vfuzz");
        break;
    case DIMEN_PAR__delimiter_shortfall:
        print_esc_cstr("delimitershortfall");
        break;
    case DIMEN_PAR__null_delimiter_space:
        print_esc_cstr("nulldelimiterspace");
        break;
    case DIMEN_PAR__script_space:
        print_esc_cstr("scriptspace");
        break;
    case DIMEN_PAR__pre_display_size:
        print_esc_cstr("predisplaysize");
        break;
    case DIMEN_PAR__display_width:
        print_esc_cstr("displaywidth");
        break;
    case DIMEN_PAR__display_indent:
        print_esc_cstr("displayindent");
        break;
    case DIMEN_PAR__overfull_rule:
        print_esc_cstr("overfullrule");
        break;
    case DIMEN_PAR__hang_indent:
        print_esc_cstr("hangindent");
        break;
    case DIMEN_PAR__h_offset:
        print_esc_cstr("hoffset");
        break;
    case DIMEN_PAR__v_offset:
        print_esc_cstr("voffset");
        break;
    case DIMEN_PAR__emergency_stretch:
        print_esc_cstr("emergencystretch");
        break;
    case DIMEN_PAR__pdf_page_width:
        print_esc_cstr("pdfpagewidth");
        break;
    case DIMEN_PAR__pdf_page_height:
        print_esc_cstr("pdfpageheight");
        break;
    default:
        print_cstr("[unknown dimen parameter!]");
        break;
    }
}


void
print_cmd_chr(uint16_t cmd, int32_t chr_code)
{
    int32_t n;
    str_number font_name_str;
    UTF16_code quote_char;

    switch (cmd) {
    case LEFT_BRACE:
        print_cstr("begin-group character ");
        if (chr_code < 65536L)
            print(chr_code);
        else
            print_char(chr_code);
        break;

    case RIGHT_BRACE:
        print_cstr("end-group character ");
        if (chr_code < 65536L)
            print(chr_code);
        else
            print_char(chr_code);
        break;

    case MATH_SHIFT:
        print_cstr("math shift character ");
        if (chr_code < 65536L)
            print(chr_code);
        else
            print_char(chr_code);
        break;

    case MAC_PARAM:
        print_cstr("macro parameter character ");
        if (chr_code < 65536L)
            print(chr_code);
        else
            print_char(chr_code);
        break;

    case SUP_MARK:
        print_cstr("superscript character ");
        if (chr_code < 65536L)
            print(chr_code);
        else
            print_char(chr_code);
        break;

    case SUB_MARK:
        print_cstr("subscript character ");
        if (chr_code < 65536L)
            print(chr_code);
        else
            print_char(chr_code);
        break;

    case ENDV:
        print_cstr("end of alignment template");
        break;

    case SPACER:
        print_cstr("blank space ");
        if (chr_code < 65536L)
            print(chr_code);
        else
            print_char(chr_code);
        break;

    case LETTER:
        print_cstr("the letter ");
        if (chr_code < 65536L)
            print(chr_code);
        else
            print_char(chr_code);
        break;

    case OTHER_CHAR:
        print_cstr("the character ");
        if (chr_code < 65536L)
            print(chr_code);
        else
            print_char(chr_code);
        break;

    case ASSIGN_GLUE:
    case ASSIGN_MU_GLUE:
        if (chr_code < SKIP_BASE) {
            print_skip_param(chr_code - GLUE_BASE);
        } else if (chr_code < MU_SKIP_BASE) {
            print_esc_cstr("skip");
            print_int(chr_code - SKIP_BASE);
        } else {
            print_esc_cstr("muskip");
            print_int(chr_code - MU_SKIP_BASE);
        }
        break;

    case ASSIGN_TOKS:
        if (chr_code >= TOKS_BASE) {
            print_esc_cstr("toks");
            print_int(chr_code - TOKS_BASE);
        } else {
            switch (chr_code) {
            case LOCAL_BASE + LOCAL__output_routine:
                print_esc_cstr("output");
                break;
            case LOCAL_BASE + LOCAL__every_par:
                print_esc_cstr("everypar");
                break;
            case LOCAL_BASE + LOCAL__every_math:
                print_esc_cstr("everymath");
                break;
            case LOCAL_BASE + LOCAL__every_display:
                print_esc_cstr("everydisplay");
                break;
            case LOCAL_BASE + LOCAL__every_hbox:
                print_esc_cstr("everyhbox");
                break;
            case LOCAL_BASE + LOCAL__every_vbox:
                print_esc_cstr("everyvbox");
                break;
            case LOCAL_BASE + LOCAL__every_job:
                print_esc_cstr("everyjob");
                break;
            case LOCAL_BASE + LOCAL__every_cr:
                print_esc_cstr("everycr");
                break;
            case LOCAL_BASE + LOCAL__every_eof:
                print_esc_cstr("everyeof");
                break;
            case LOCAL_BASE + LOCAL__xetex_inter_char:
                print_esc_cstr("XeTeXinterchartoks");
                break;
            case LOCAL_BASE + LOCAL__TectonicCodaTokens:
                print_esc_cstr("TectonicCodaTokens");
                break;
            default:
                print_esc_cstr("errhelp");
                break;
            }
        }
        break;

    case ASSIGN_INT:
        if (chr_code < COUNT_BASE) {
            print_param(chr_code - INT_BASE);
        } else {
            print_esc_cstr("count");
            print_int(chr_code - COUNT_BASE);
        }
        break;

    case ASSIGN_DIMEN:
        if (chr_code < SCALED_BASE) {
            print_length_param(chr_code - DIMEN_BASE);
        } else {
            print_esc_cstr("dimen");
            print_int(chr_code - SCALED_BASE);
        }
        break;

    case ACCENT:
        print_esc_cstr("accent");
        break;

    case ADVANCE:
        print_esc_cstr("advance");
        break;

    case AFTER_ASSIGNMENT:
        print_esc_cstr("afterassignment");
        break;

    case AFTER_GROUP:
        print_esc_cstr("aftergroup");
        break;

    case ASSIGN_FONT_DIMEN:
        print_esc_cstr("fontdimen");
        break;

    case BEGIN_GROUP:
        print_esc_cstr("begingroup");
        break;

    case BREAK_PENALTY:
        print_esc_cstr("penalty");
        break;

    case CHAR_NUM:
        print_esc_cstr("char");
        break;

    case CS_NAME:
        print_esc_cstr("csname");
        break;

    case DEF_FONT:
        print_esc_cstr("font");
        break;

    case DELIM_NUM:
        if (chr_code == 1)
            print_esc_cstr("Udelimiter");
        else
            print_esc_cstr("delimiter");
        break;

    case DIVIDE:
        print_esc_cstr("divide");
        break;

    case END_CS_NAME:
        print_esc_cstr("endcsname");
        break;

    case END_GROUP:
        print_esc_cstr("endgroup");
        break;

    case EX_SPACE:
        print_esc(' ' );
        break;

    case EXPAND_AFTER:
        if (chr_code == 0)
            print_esc_cstr("expandafter");
        else
            print_esc_cstr("unless");
        break;

    case HALIGN:
        print_esc_cstr("halign");
        break;

    case HRULE:
        print_esc_cstr("hrule");
        break;

    case IGNORE_SPACES:
        if (chr_code == 0)
            print_esc_cstr("ignorespaces");
        else
            print_esc_cstr("primitive");
        break;

    case INSERT:
        print_esc_cstr("insert");
        break;

    case ITAL_CORR:
        print_esc('/' );
        break;

    case MARK:
        print_esc_cstr("mark");
        if (chr_code > 0)
            print_char('s');
        break;

    case MATH_ACCENT:
        if (chr_code == 1)
            print_esc_cstr("Umathaccent");
        else
            print_esc_cstr("mathaccent");
        break;

    case MATH_CHAR_NUM:
        if (chr_code == 2)
            print_esc_cstr("Umathchar");
        else if (chr_code == 1)
            print_esc_cstr("Umathcharnum");
        else
            print_esc_cstr("mathchar");
        break;

    case MATH_CHOICE:
        print_esc_cstr("mathchoice");
        break;

    case MULTIPLY:
        print_esc_cstr("multiply");
        break;

    case NO_ALIGN:
        print_esc_cstr("noalign");
        break;

    case NO_BOUNDARY:
        print_esc_cstr("noboundary");
        break;

    case NO_EXPAND:
        if (chr_code == 0)
            print_esc_cstr("noexpand");
        else
            print_esc_cstr("primitive");
        break;

    case NON_SCRIPT:
        print_esc_cstr("nonscript");
        break;

    case OMIT:
        print_esc_cstr("omit");
        break;

    case RADICAL:
        if (chr_code == 1)
            print_esc_cstr("Uradical");
        else
            print_esc_cstr("radical");
        break;

    case READ_TO_CS:
        if (chr_code == 0)
            print_esc_cstr("read");
        else
            print_esc_cstr("readline");
        break;

    case RELAX:
        print_esc_cstr("relax");
        break;

    case SET_BOX:
        print_esc_cstr("setbox");
        break;

    case SET_PREV_GRAF:
        print_esc_cstr("prevgraf");
        break;

    case SET_SHAPE:
        switch (chr_code) {
        case LOCAL_BASE + LOCAL__par_shape:
            print_esc_cstr("parshape");
            break;
        case INTER_LINE_PENALTIES_LOC:
            print_esc_cstr("interlinepenalties");
            break;
        case CLUB_PENALTIES_LOC:
            print_esc_cstr("clubpenalties");
            break;
        case WIDOW_PENALTIES_LOC:
            print_esc_cstr("widowpenalties");
            break;
        case DISPLAY_WIDOW_PENALTIES_LOC:
            print_esc_cstr("displaywidowpenalties");
            break;
        }
        break;

    case THE:
        if (chr_code == 0)
            print_esc_cstr("the");
        else if (chr_code == 1)
            print_esc_cstr("unexpanded");
        else
            print_esc_cstr("detokenize");
        break;

    case TOKS_REGISTER:
        print_esc_cstr("toks");
        if (chr_code != 0)
            print_sa_num(chr_code);
        break;

    case VADJUST:
        print_esc_cstr("vadjust");
        break;

    case VALIGN:
        if (chr_code == 0) {
            print_esc_cstr("valign");
        } else {
            switch (chr_code) {
            case BEGIN_L_CODE:
                print_esc_cstr("beginL");
                break;
            case END_L_CODE:
                print_esc_cstr("endL");
                break;
            case BEGIN_R_CODE:
                print_esc_cstr("beginR");
                break;
            default:
                print_esc_cstr("endR");
                break;
            }
        }
        break;

    case VCENTER:
        print_esc_cstr("vcenter");
        break;

    case VRULE:
        print_esc_cstr("vrule");
        break;

    case PAR_END:
        print_esc_cstr("par");
        break;

    case INPUT:
        if (chr_code == 0)
            print_esc_cstr("input");
        else if (chr_code == 2)
            print_esc_cstr("scantokens");
        else
            print_esc_cstr("endinput");
        break;

    case TOP_BOT_MARK:
        switch (chr_code % MARKS_CODE) {
        case FIRST_MARK_CODE:
            print_esc_cstr("firstmark");
            break;
        case BOT_MARK_CODE:
            print_esc_cstr("botmark");
            break;
        case SPLIT_FIRST_MARK_CODE:
            print_esc_cstr("splitfirstmark");
            break;
        case SPLIT_BOT_MARK_CODE:
            print_esc_cstr("splitbotmark");
            break;
        default:
            print_esc_cstr("topmark");
            break;
        }
        if (chr_code >= MARKS_CODE)
            print_char('s');
        break;

    case REGISTER:
        if (chr_code < 0 || chr_code > 19 /*lo_mem_stat_max*/) {
            cmd = (mem[chr_code].b16.s1 / 64);
        } else {
            cmd = chr_code;
            chr_code = TEX_NULL;
        }

        if (cmd == INT_VAL)
            print_esc_cstr("count");
        else if (cmd == DIMEN_VAL)
            print_esc_cstr("dimen");
        else if (cmd == GLUE_VAL)
            print_esc_cstr("skip");
        else
            print_esc_cstr("muskip");

        if (chr_code != TEX_NULL)
            print_sa_num(chr_code);
        break;

    case SET_AUX:
        if (chr_code == VMODE)
            print_esc_cstr("prevdepth");
        else
            print_esc_cstr("spacefactor");
        break;

    case SET_PAGE_INT:
        if (chr_code == 0)
            print_esc_cstr("deadcycles");
        else if (chr_code == 2)
            print_esc_cstr("interactionmode");
        else
            print_esc_cstr("insertpenalties");
        break;

    case SET_BOX_DIMEN:
        if (chr_code == WIDTH_OFFSET)
            print_esc_cstr("wd");
        else if (chr_code == HEIGHT_OFFSET)
            print_esc_cstr("ht");
        else
            print_esc_cstr("dp");
        break;

    case LAST_ITEM:
        switch (chr_code) {
        case INT_VAL:
            print_esc_cstr("lastpenalty");
            break;
        case DIMEN_VAL:
            print_esc_cstr("lastkern");
            break;
        case GLUE_VAL:
            print_esc_cstr("lastskip");
            break;
        case INPUT_LINE_NO_CODE:
            print_esc_cstr("inputlineno");
            break;
        case LAST_NODE_TYPE_CODE:
            print_esc_cstr("lastnodetype");
            break;
        case ETEX_VERSION_CODE:
            print_esc_cstr("eTeXversion");
            break;
        case XETEX_VERSION_CODE:
            print_esc_cstr("XeTeXversion");
            break;
        case XETEX_COUNT_GLYPHS_CODE:
            print_esc_cstr("XeTeXcountglyphs");
            break;
        case XETEX_COUNT_VARIATIONS_CODE:
            print_esc_cstr("XeTeXcountvariations");
            break;
        case XETEX_VARIATION_CODE:
            print_esc_cstr("XeTeXvariation");
            break;
        case XETEX_FIND_VARIATION_BY_NAME_CODE:
            print_esc_cstr("XeTeXfindvariationbyname");
            break;
        case XETEX_VARIATION_MIN_CODE:
            print_esc_cstr("XeTeXvariationmin");
            break;
        case XETEX_VARIATION_MAX_CODE:
            print_esc_cstr("XeTeXvariationmax");
            break;
        case XETEX_VARIATION_DEFAULT_CODE:
            print_esc_cstr("XeTeXvariationdefault");
            break;
        case XETEX_COUNT_FEATURES_CODE:
            print_esc_cstr("XeTeXcountfeatures");
            break;
        case XETEX_FEATURE_CODE_CODE:
            print_esc_cstr("XeTeXfeaturecode");
            break;
        case XETEX_FIND_FEATURE_BY_NAME_CODE:
            print_esc_cstr("XeTeXfindfeaturebyname");
            break;
        case XETEX_IS_EXCLUSIVE_FEATURE_CODE:
            print_esc_cstr("XeTeXisexclusivefeature");
            break;
        case XETEX_COUNT_SELECTORS_CODE:
            print_esc_cstr("XeTeXcountselectors");
            break;
        case XETEX_SELECTOR_CODE_CODE:
            print_esc_cstr("XeTeXselectorcode");
            break;
        case XETEX_FIND_SELECTOR_BY_NAME_CODE:
            print_esc_cstr("XeTeXfindselectorbyname");
            break;
        case XETEX_IS_DEFAULT_SELECTOR_CODE:
            print_esc_cstr("XeTeXisdefaultselector");
            break;
        case XETEX_OT_COUNT_SCRIPTS_CODE:
            print_esc_cstr("XeTeXOTcountscripts");
            break;
        case XETEX_OT_COUNT_LANGUAGES_CODE:
            print_esc_cstr("XeTeXOTcountlanguages");
            break;
        case XETEX_OT_COUNT_FEATURES_CODE:
            print_esc_cstr("XeTeXOTcountfeatures");
            break;
        case XETEX_OT_SCRIPT_CODE:
            print_esc_cstr("XeTeXOTscripttag");
            break;
        case XETEX_OT_LANGUAGE_CODE:
            print_esc_cstr("XeTeXOTlanguagetag");
            break;
        case XETEX_OT_FEATURE_CODE:
            print_esc_cstr("XeTeXOTfeaturetag");
            break;
        case XETEX_MAP_CHAR_TO_GLYPH_CODE:
            print_esc_cstr("XeTeXcharglyph");
            break;
        case XETEX_GLYPH_INDEX_CODE:
            print_esc_cstr("XeTeXglyphindex");
            break;
        case XETEX_GLYPH_BOUNDS_CODE:
            print_esc_cstr("XeTeXglyphbounds");
            break;
        case XETEX_FONT_TYPE_CODE:
            print_esc_cstr("XeTeXfonttype");
            break;
        case XETEX_FIRST_CHAR_CODE:
            print_esc_cstr("XeTeXfirstfontchar");
            break;
        case XETEX_LAST_CHAR_CODE:
            print_esc_cstr("XeTeXlastfontchar");
            break;
        case XETEX_PDF_PAGE_COUNT_CODE:
            print_esc_cstr("XeTeXpdfpagecount");
            break;
        case CURRENT_GROUP_LEVEL_CODE:
            print_esc_cstr("currentgrouplevel");
            break;
        case CURRENT_GROUP_TYPE_CODE:
            print_esc_cstr("currentgrouptype");
            break;
        case CURRENT_IF_LEVEL_CODE:
            print_esc_cstr("currentiflevel");
            break;
        case CURRENT_IF_TYPE_CODE:
            print_esc_cstr("currentiftype");
            break;
        case CURRENT_IF_BRANCH_CODE:
            print_esc_cstr("currentifbranch");
            break;
        case FONT_CHAR_WD_CODE:
            print_esc_cstr("fontcharwd");
            break;
        case FONT_CHAR_HT_CODE:
            print_esc_cstr("fontcharht");
            break;
        case FONT_CHAR_DP_CODE:
            print_esc_cstr("fontchardp");
            break;
        case FONT_CHAR_IC_CODE:
            print_esc_cstr("fontcharic");
            break;
        case PAR_SHAPE_LENGTH_CODE:
            print_esc_cstr("parshapelength");
            break;
        case PAR_SHAPE_INDENT_CODE:
            print_esc_cstr("parshapeindent");
            break;
        case PAR_SHAPE_DIMEN_CODE:
            print_esc_cstr("parshapedimen");
            break;
        case (ETEX_EXPR - INT_VAL + INT_VAL):
            print_esc_cstr("numexpr");
            break;
        case (ETEX_EXPR - INT_VAL + DIMEN_VAL):
            print_esc_cstr("dimexpr");
            break;
        case (ETEX_EXPR - INT_VAL + GLUE_VAL):
            print_esc_cstr("glueexpr");
            break;
        case (ETEX_EXPR - INT_VAL + MU_VAL):
            print_esc_cstr("muexpr");
            break;
        case GLUE_STRETCH_ORDER_CODE:
            print_esc_cstr("gluestretchorder");
            break;
        case GLUE_SHRINK_ORDER_CODE:
            print_esc_cstr("glueshrinkorder");
            break;
        case GLUE_STRETCH_CODE:
            print_esc_cstr("gluestretch");
            break;
        case GLUE_SHRINK_CODE:
            print_esc_cstr("glueshrink");
            break;
        case MU_TO_GLUE_CODE:
            print_esc_cstr("mutoglue");
            break;
        case GLUE_TO_MU_CODE:
            print_esc_cstr("gluetomu");
            break;
        case PDF_LAST_X_POS_CODE:
            print_esc_cstr("pdflastxpos");
            break;
        case PDF_LAST_Y_POS_CODE:
            print_esc_cstr("pdflastypos");
            break;
        case ELAPSED_TIME_CODE:
            print_esc_cstr("elapsedtime");
            break;
        case PDF_SHELL_ESCAPE_CODE:
            print_esc_cstr("shellescape");
            break;
        case RANDOM_SEED_CODE:
            print_esc_cstr("randomseed");
            break;
        default:
            print_esc_cstr("badness");
            break;
        }
        break;

    case CONVERT:
        switch (chr_code) {
        case NUMBER_CODE:
            print_esc_cstr("number");
            break;
        case ROMAN_NUMERAL_CODE:
            print_esc_cstr("romannumeral");
            break;
        case STRING_CODE:
            print_esc_cstr("string");
            break;
        case MEANING_CODE:
            print_esc_cstr("meaning");
            break;
        case FONT_NAME_CODE:
            print_esc_cstr("fontname");
            break;
        case ETEX_REVISION_CODE:
            print_esc_cstr("eTeXrevision");
            break;
        case EXPANDED_CODE:
            print_esc_cstr("expanded");
            break;
        case LEFT_MARGIN_KERN_CODE:
            print_esc_cstr("leftmarginkern");
            break;
        case RIGHT_MARGIN_KERN_CODE:
            print_esc_cstr("rightmarginkern");
            break;
        case PDF_CREATION_DATE_CODE:
            print_esc_cstr("creationdate");
            break;
        case PDF_FILE_MOD_DATE_CODE:
            print_esc_cstr("filemoddate");
            break;
        case PDF_FILE_SIZE_CODE:
            print_esc_cstr("filesize");
            break;
        case PDF_MDFIVE_SUM_CODE:
            print_esc_cstr("mdfivesum");
            break;
        case PDF_FILE_DUMP_CODE:
            print_esc_cstr("filedump");
            break;
        case PDF_STRCMP_CODE:
            print_esc_cstr("strcmp");
            break;
        case UNIFORM_DEVIATE_CODE:
            print_esc_cstr("uniformdeviate");
            break;
        case NORMAL_DEVIATE_CODE:
            print_esc_cstr("normaldeviate");
            break;
        case XETEX_REVISION_CODE:
            print_esc_cstr("XeTeXrevision");
            break;
        case XETEX_VARIATION_NAME_CODE:
            print_esc_cstr("XeTeXvariationname");
            break;
        case XETEX_FEATURE_NAME_CODE:
            print_esc_cstr("XeTeXfeaturename");
            break;
        case XETEX_SELECTOR_NAME_CODE:
            print_esc_cstr("XeTeXselectorname");
            break;
        case XETEX_GLYPH_NAME_CODE:
            print_esc_cstr("XeTeXglyphname");
            break;
        case XETEX_UCHAR_CODE:
            print_esc_cstr("Uchar");
            break;
        case XETEX_UCHARCAT_CODE:
            print_esc_cstr("Ucharcat");
            break;
        default:
            print_esc_cstr("jobname");
            break;
        }
        break;

    case IF_TEST:
        if (chr_code >= UNLESS_CODE)
            print_esc_cstr("unless");

        switch (chr_code % UNLESS_CODE) {
        case IF_CAT_CODE:
            print_esc_cstr("ifcat");
            break;
        case IF_INT_CODE:
            print_esc_cstr("ifnum");
            break;
        case IF_DIM_CODE:
            print_esc_cstr("ifdim");
            break;
        case IF_ODD_CODE:
            print_esc_cstr("ifodd");
            break;
        case IF_VMODE_CODE:
            print_esc_cstr("ifvmode");
            break;
        case IF_HMODE_CODE:
            print_esc_cstr("ifhmode");
            break;
        case IF_MMODE_CODE:
            print_esc_cstr("ifmmode");
            break;
        case IF_INNER_CODE:
            print_esc_cstr("ifinner");
            break;
        case IF_VOID_CODE:
            print_esc_cstr("ifvoid");
            break;
        case IF_HBOX_CODE:
            print_esc_cstr("ifhbox");
            break;
        case IF_VBOX_CODE:
            print_esc_cstr("ifvbox");
            break;
        case IFX_CODE:
            print_esc_cstr("ifx");
            break;
        case IF_EOF_CODE:
            print_esc_cstr("ifeof");
            break;
        case IF_TRUE_CODE:
            print_esc_cstr("iftrue");
            break;
        case IF_FALSE_CODE:
            print_esc_cstr("iffalse");
            break;
        case IF_CASE_CODE:
            print_esc_cstr("ifcase");
            break;
        case IF_PRIMITIVE_CODE:
            print_esc_cstr("ifprimitive");
            break;
        case IF_DEF_CODE:
            print_esc_cstr("ifdefined");
            break;
        case IF_CS_CODE:
            print_esc_cstr("ifcsname");
            break;
        case IF_FONT_CHAR_CODE:
            print_esc_cstr("iffontchar");
            break;
        case IF_IN_CSNAME_CODE:
            print_esc_cstr("ifincsname");
            break;
        default:
            print_esc_cstr("if");
            break;
        }
        break;

    case FI_OR_ELSE:
        if (chr_code == FI_CODE)
            print_esc_cstr("fi");
        else if (chr_code == OR_CODE)
            print_esc_cstr("or");
        else
            print_esc_cstr("else");
        break;

    case TAB_MARK:
        if (chr_code == SPAN_CODE) {
            print_esc_cstr("span");
        } else {
            print_cstr("alignment tab character ");
            if (chr_code < 65536L)
                print(chr_code);
            else
                print_char(chr_code);
        }
        break;

    case CAR_RET:
        if (chr_code == CR_CODE)
            print_esc_cstr("cr");
        else
            print_esc_cstr("crcr");
        break;

    case SET_PAGE_DIMEN:
        switch (chr_code) {
        case 0: /* genuine literal in WEB */
            print_esc_cstr("pagegoal");
            break;
        case 1: /* genuine literal in WEB */
            print_esc_cstr("pagetotal");
            break;
        case 2: /* genuine literal in WEB */
            print_esc_cstr("pagestretch");
            break;
        case 3: /* genuine literal in WEB */
            print_esc_cstr("pagefilstretch");
            break;
        case 4: /* genuine literal in WEB */
            print_esc_cstr("pagefillstretch");
            break;
        case 5: /* genuine literal in WEB */
            print_esc_cstr("pagefilllstretch");
            break;
        case 6: /* genuine literal in WEB */
            print_esc_cstr("pageshrink");
            break;
        default:
            print_esc_cstr("pagedepth");
            break;
        }
        break;

    case STOP:
        if (chr_code == 1)
            print_esc_cstr("dump");
        else
            print_esc_cstr("end");
        break;

    case HSKIP:
        switch (chr_code) {
        case SKIP_CODE:
            print_esc_cstr("hskip");
            break;
        case FIL_CODE:
            print_esc_cstr("hfil");
            break;
        case FILL_CODE:
            print_esc_cstr("hfill");
            break;
        case SS_CODE:
            print_esc_cstr("hss");
            break;
        default:
            print_esc_cstr("hfilneg");
            break;
        }
        break;

    case VSKIP:
        switch (chr_code) {
        case SKIP_CODE:
            print_esc_cstr("vskip");
            break;
        case FIL_CODE:
            print_esc_cstr("vfil");
            break;
        case FILL_CODE:
            print_esc_cstr("vfill");
            break;
        case SS_CODE:
            print_esc_cstr("vss");
            break;
        default:
            print_esc_cstr("vfilneg");
            break;
        }
        break;

    case MSKIP:
        print_esc_cstr("mskip");
        break;

    case KERN:
        print_esc_cstr("kern");
        break;

    case MKERN:
        print_esc_cstr("mkern");
        break;

    case HMOVE:
        if (chr_code == 1)
            print_esc_cstr("moveleft");
        else
            print_esc_cstr("moveright");
        break;

    case VMOVE:
        if (chr_code == 1)
            print_esc_cstr("raise");
        else
            print_esc_cstr("lower");
        break;

    case MAKE_BOX:
        switch (chr_code) {
        case BOX_CODE:
            print_esc_cstr("box");
            break;
        case COPY_CODE:
            print_esc_cstr("copy");
            break;
        case LAST_BOX_CODE:
            print_esc_cstr("lastbox");
            break;
        case VSPLIT_CODE:
            print_esc_cstr("vsplit");
            break;
        case VTOP_CODE:
            print_esc_cstr("vtop");
            break;
        case (VTOP_CODE + VMODE):
            print_esc_cstr("vbox");
            break;
        default:
            print_esc_cstr("hbox");
            break;
        }
        break;

    case LEADER_SHIP:
        if (chr_code == A_LEADERS)
            print_esc_cstr("leaders");
        else if (chr_code == C_LEADERS)
            print_esc_cstr("cleaders");
        else if (chr_code == X_LEADERS)
            print_esc_cstr("xleaders");
        else
            print_esc_cstr("shipout");
        break;

    case START_PAR:
        if (chr_code == 0)
            print_esc_cstr("noindent");
        else
            print_esc_cstr("indent");
        break;

    case REMOVE_ITEM:
        if (chr_code == GLUE_NODE)
            print_esc_cstr("unskip");
        else if (chr_code == KERN_NODE)
            print_esc_cstr("unkern");
        else
            print_esc_cstr("unpenalty");
        break;

    case UN_HBOX:
        if (chr_code == COPY_CODE)
            print_esc_cstr("unhcopy");
        else
            print_esc_cstr("unhbox");
        break;

    case UN_VBOX:
        if (chr_code == COPY_CODE)
            print_esc_cstr("unvcopy");
        else if (chr_code == LAST_BOX_CODE)
            print_esc_cstr("pagediscards");
        else if (chr_code == VSPLIT_CODE)
            print_esc_cstr("splitdiscards");
        else
            print_esc_cstr("unvbox");
        break;

    case DISCRETIONARY:
        if (chr_code == 1)
            print_esc('-' );
        else
            print_esc_cstr("discretionary");
        break;

    case EQ_NO:
        if (chr_code == 1)
            print_esc_cstr("leqno");
        else
            print_esc_cstr("eqno");
        break;

    case MATH_COMP:
        switch (chr_code) {
        case ORD_NOAD:
            print_esc_cstr("mathord");
            break;
        case OP_NOAD:
            print_esc_cstr("mathop");
            break;
        case BIN_NOAD:
            print_esc_cstr("mathbin");
            break;
        case REL_NOAD:
            print_esc_cstr("mathrel");
            break;
        case OPEN_NOAD:
            print_esc_cstr("mathopen");
            break;
        case CLOSE_NOAD:
            print_esc_cstr("mathclose");
            break;
        case PUNCT_NOAD:
            print_esc_cstr("mathpunct");
            break;
        case INNER_NOAD:
            print_esc_cstr("mathinner");
            break;
        case UNDER_NOAD:
            print_esc_cstr("underline");
            break;
        default:
            print_esc_cstr("overline");
            break;
        }
        break;

    case LIMIT_SWITCH:
        if (chr_code == LIMITS)
            print_esc_cstr("limits");
        else if (chr_code == NO_LIMITS)
            print_esc_cstr("nolimits");
        else
            print_esc_cstr("displaylimits");
        break;

    case MATH_STYLE:
        print_style(chr_code);
        break;

    case ABOVE:
        switch (chr_code) {
        case OVER_CODE:
            print_esc_cstr("over");
            break;
        case ATOP_CODE:
            print_esc_cstr("atop");
            break;
        case DELIMITED_CODE + ABOVE_CODE:
            print_esc_cstr("abovewithdelims");
            break;
        case DELIMITED_CODE + OVER_CODE:
            print_esc_cstr("overwithdelims");
            break;
        case DELIMITED_CODE + ATOP_CODE:
            print_esc_cstr("atopwithdelims");
            break;
        default:
            print_esc_cstr("above");
            break;
        }
        break;

    case LEFT_RIGHT:
        if (chr_code == LEFT_NOAD)
            print_esc_cstr("left");
        else if (chr_code == MIDDLE_NOAD)
            print_esc_cstr("middle");
        else
            print_esc_cstr("right");
        break;

    case PREFIX:
        if (chr_code == 1)
            print_esc_cstr("long");
        else if (chr_code == 2)
            print_esc_cstr("outer");
        else if (chr_code == 8)
            print_esc_cstr("protected");
        else
            print_esc_cstr("global");
        break;

    case DEF:
        if (chr_code == 0)
            print_esc_cstr("def");
        else if (chr_code == 1)
            print_esc_cstr("gdef");
        else if (chr_code == 2)
            print_esc_cstr("edef");
        else
            print_esc_cstr("xdef");
        break;

    case LET:
        if (chr_code != NORMAL)
            print_esc_cstr("futurelet");
        else
            print_esc_cstr("let");
        break;

    case SHORTHAND_DEF:
        switch (chr_code) {
        case CHAR_DEF_CODE:
            print_esc_cstr("chardef");
            break;
        case MATH_CHAR_DEF_CODE:
            print_esc_cstr("mathchardef");
            break;
        case XETEX_MATH_CHAR_DEF_CODE:
            print_esc_cstr("Umathchardef");
            break;
        case XETEX_MATH_CHAR_NUM_DEF_CODE:
            print_esc_cstr("Umathcharnumdef");
            break;
        case COUNT_DEF_CODE:
            print_esc_cstr("countdef");
            break;
        case DIMEN_DEF_CODE:
            print_esc_cstr("dimendef");
            break;
        case SKIP_DEF_CODE:
            print_esc_cstr("skipdef");
            break;
        case MU_SKIP_DEF_CODE:
            print_esc_cstr("muskipdef");
            break;
        case CHAR_SUB_DEF_CODE:
            print_esc_cstr("charsubdef");
            break;
        default:
            print_esc_cstr("toksdef");
            break;
        }
        break;

    case CHAR_GIVEN:
        print_esc_cstr("char");
        print_hex(chr_code);
        break;

    case MATH_GIVEN:
        print_esc_cstr("mathchar");
        print_hex(chr_code);
        break;

    case XETEX_MATH_GIVEN:
        print_esc_cstr("Umathchar");
        print_hex(math_class(chr_code));
        print_hex(math_fam(chr_code));
        print_hex(math_char(chr_code));
        break;

    case DEF_CODE:
        if (chr_code == CAT_CODE_BASE)
            print_esc_cstr("catcode");
        else if (chr_code == MATH_CODE_BASE)
            print_esc_cstr("mathcode");
        else if (chr_code == LC_CODE_BASE)
            print_esc_cstr("lccode");
        else if (chr_code == UC_CODE_BASE)
            print_esc_cstr("uccode");
        else if (chr_code == SF_CODE_BASE)
            print_esc_cstr("sfcode");
        else
            print_esc_cstr("delcode");
        break;

    case XETEX_DEF_CODE:
        if (chr_code == SF_CODE_BASE)
            print_esc_cstr("XeTeXcharclass");
        else if (chr_code == MATH_CODE_BASE)
            print_esc_cstr("Umathcodenum");
        else if (chr_code == (MATH_CODE_BASE + 1))
            print_esc_cstr("Umathcode");
        else if (chr_code == DEL_CODE_BASE)
            print_esc_cstr("Udelcodenum");
        else
            print_esc_cstr("Udelcode");
        break;

    case DEF_FAMILY:
        print_size(chr_code - MATH_FONT_BASE);
        break;

    case HYPH_DATA:
        if (chr_code == 1)
            print_esc_cstr("patterns");
        else
            print_esc_cstr("hyphenation");
        break;

    case ASSIGN_FONT_INT:
        switch (chr_code) {
        case 0:
            print_esc_cstr("hyphenchar");
            break;
        case 1:
            print_esc_cstr("skewchar");
            break;
        case LP_CODE_BASE:
            print_esc_cstr("lpcode");
            break;
        case RP_CODE_BASE:
            print_esc_cstr("rpcode");
            break;
        }
        break;

    case SET_FONT:
        print_cstr("select font ");

        font_name_str = font_name[chr_code];
        if (font_area[chr_code] == AAT_FONT_FLAG || font_area[chr_code] == OTGR_FONT_FLAG) {
            int32_t for_end = length(font_name_str) - 1;
            quote_char = '"' ;

            for (n = 0; n <= for_end; n++) {
                if (str_pool[str_start[(font_name_str) - 65536L] + n] == '"' )
                    quote_char = '\'' ;
            }

            print_char(quote_char);
            print(font_name_str);
            print_char(quote_char);
        } else {
            print(font_name_str);
        }

        if (font_size[chr_code] != font_dsize[chr_code]) {
            print_cstr(" at ");
            print_scaled(font_size[chr_code]);
            print_cstr("pt");
        }
        break;

    case SET_INTERACTION:
        switch (chr_code) {
        case BATCH_MODE:
            print_esc_cstr("batchmode");
            break;
        case NONSTOP_MODE:
            print_esc_cstr("nonstopmode");
            break;
        case SCROLL_MODE:
            print_esc_cstr("scrollmode");
            break;
        default:
            print_esc_cstr("errorstopmode");
            break;
        }
        break;

    case IN_STREAM:
        if (chr_code == 0)
            print_esc_cstr("closein");
        else
            print_esc_cstr("openin");
        break;

    case MESSAGE:
        if (chr_code == 0)
            print_esc_cstr("message");
        else
            print_esc_cstr("errmessage");
        break;

    case CASE_SHIFT:
        if (chr_code == LC_CODE_BASE)
            print_esc_cstr("lowercase");
        else
            print_esc_cstr("uppercase");
        break;

    case XRAY:
        switch (chr_code) {
        case SHOW_BOX_CODE:
            print_esc_cstr("showbox");
            break;
        case SHOW_THE_CODE:
            print_esc_cstr("showthe");
            break;
        case SHOW_LISTS:
            print_esc_cstr("showlists");
            break;
        case SHOW_GROUPS:
            print_esc_cstr("showgroups");
            break;
        case SHOW_TOKENS:
            print_esc_cstr("showtokens");
            break;
        case SHOW_IFS:
            print_esc_cstr("showifs");
            break;
        default:
            print_esc_cstr("show");
            break;
        }
        break;

    case UNDEFINED_CS:
        print_cstr("undefined");
        break;

    case CALL:
    case LONG_CALL:
    case OUTER_CALL:
    case LONG_OUTER_CALL:
        n = cmd - CALL;
        if (mem[mem[chr_code].b32.s1].b32.s0 == PROTECTED_TOKEN)
            n = n + 4;
        if (odd(n / 4))
            print_esc_cstr("protected");
        if (odd(n))
            print_esc_cstr("long");
        if (odd(n / 2))
            print_esc_cstr("outer");
        if (n > 0)
            print_char(' ');
        print_cstr("macro");
        break;

    case END_TEMPLATE:
        print_esc_cstr("outer endtemplate");
        break;

    case EXTENSION:
        switch (chr_code) {
        case OPEN_NODE:
            print_esc_cstr("openout");
            break;
        case WRITE_NODE:
            print_esc_cstr("write");
            break;
        case CLOSE_NODE:
            print_esc_cstr("closeout");
            break;
        case SPECIAL_NODE:
            print_esc_cstr("special");
            break;
        case IMMEDIATE_CODE:
            print_esc_cstr("immediate");
            break;
        case SET_LANGUAGE_CODE:
            print_esc_cstr("setlanguage");
            break;
        case PDF_SAVE_POS_NODE:
            print_esc_cstr("pdfsavepos");
            break;
        case RESET_TIMER_CODE:
            print_esc_cstr("resettimer");
            break;
        case SET_RANDOM_SEED_CODE:
            print_esc_cstr("setrandomseed");
            break;
        case PIC_FILE_CODE:
            print_esc_cstr("XeTeXpicfile");
            break;
        case PDF_FILE_CODE:
            print_esc_cstr("XeTeXpdffile");
            break;
        case GLYPH_CODE:
            print_esc_cstr("XeTeXglyph");
            break;
        case XETEX_LINEBREAK_LOCALE_EXTENSION_CODE:
            print_esc_cstr("XeTeXlinebreaklocale");
            break;
        case XETEX_INPUT_ENCODING_EXTENSION_CODE:
            print_esc_cstr("XeTeXinputencoding");
            break;
        case XETEX_DEFAULT_ENCODING_EXTENSION_CODE:
            print_esc_cstr("XeTeXdefaultencoding");
            break;
        default:
            print_cstr("[unknown extension!]");
            break;
        }
        break;

    default:
        print_cstr("[unknown command code!]");
        break;
    }
}


void not_aat_font_error(int32_t cmd, int32_t c, int32_t f)
{
    error_here_with_diagnostic("Cannot use ");
    print_cmd_chr(cmd, c);
    print_cstr(" with ");
    print(font_name[f]);
    print_cstr("; not an AAT font");
    capture_to_diagnostic(NULL);

    error();
}

void not_aat_gr_font_error(int32_t cmd, int32_t c, int32_t f)
{
    error_here_with_diagnostic("Cannot use ");
    print_cmd_chr(cmd, c);
    print_cstr(" with ");
    print(font_name[f]);
    print_cstr("; not an AAT or Graphite font");
    capture_to_diagnostic(NULL);

    error();
}

void not_ot_font_error(int32_t cmd, int32_t c, int32_t f)
{
    error_here_with_diagnostic("Cannot use ");
    print_cmd_chr(cmd, c);
    print_cstr(" with ");
    print(font_name[f]);
    print_cstr("; not an OpenType Layout font");
    capture_to_diagnostic(NULL);

    error();
}

void not_native_font_error(int32_t cmd, int32_t c, int32_t f)
{
    error_here_with_diagnostic("Cannot use ");
    print_cmd_chr(cmd, c);
    print_cstr(" with ");
    print(font_name[f]);
    print_cstr("; not a native platform font");
    capture_to_diagnostic(NULL);

    error();
}

/*:1434*/


int32_t
id_lookup(int32_t j, int32_t l)
{
    int32_t h;
    int32_t d;
    int32_t p;
    int32_t k;
    int32_t ll;

    h = 0;

    for (k = j; k <= j + l - 1; k++) {
        h = h + h + buffer[k];
        while (h >= HASH_PRIME)
            h = h - 8501;
    }

    p = h + HASH_BASE;
    ll = l;

    for (d = 0; d <= l - 1; d++) {
        if (buffer[j + d] >= 65536L)
            ll++;
    }

    while (true) {
        if (hash[p].s1 > 0) {
            if (length(hash[p].s1) == ll) {
                if (str_eq_buf(hash[p].s1, j))
                    goto found;
            }
        }

        if (hash[p].s0 == 0) {
            if (no_new_control_sequence) {
                p = UNDEFINED_CONTROL_SEQUENCE;
            } else { /*269:*/
                if (hash[p].s1 > 0) {
                    if (hash_high < hash_extra) {
                        hash_high++;
                        hash[p].s0 = hash_high + EQTB_SIZE;
                        p = hash_high + EQTB_SIZE;
                    } else {
                        do {
                            if (hash_used == HASH_BASE)
                                overflow("hash size", HASH_SIZE + hash_extra);
                            hash_used--;
                        } while (hash[hash_used].s1 != 0);

                        hash[p].s0 = hash_used;
                        p = hash_used;
                    }
                }

                if (pool_ptr + ll > pool_size)
                    overflow("pool size", pool_size - init_pool_ptr);

                d = cur_length();

                while (pool_ptr > str_start[str_ptr - TOO_BIG_CHAR]) {
                    pool_ptr--;
                    str_pool[pool_ptr + l] = str_pool[pool_ptr];
                }

                for (k = j; k <= j + l - 1; k++) {
                    if (buffer[k] < 65536L) {
                        str_pool[pool_ptr] = buffer[k];
                        pool_ptr++;
                    } else {
                        str_pool[pool_ptr] = 0xD800 + (buffer[k] - 65536L) / 1024;
                        pool_ptr++;
                        str_pool[pool_ptr] = 0xDC00 + (buffer[k] - 65536L) % 1024;
                        pool_ptr++;
                    }
                }

                hash[p].s1 = make_string();
                pool_ptr += d;
            }
            goto found;

        }

        p = hash[p].s0;
    }

found:
    return p;
}


int32_t prim_lookup(str_number s)
{
    int32_t h;
    int32_t p;
    int32_t k;
    int32_t j, l = 0;

    if (s <= BIGGEST_CHAR) {
        if (s < 0) {
            p = UNDEFINED_PRIMITIVE;
            goto found;
        } else
            p = (s % PRIM_PRIME) + 1;
    } else {

        j = str_start[(s) - 65536L];
        if (s == str_ptr)
            l = (cur_length());
        else
            l = length(s);
        h = str_pool[j];
        {
            register int32_t for_end;
            k = j + 1;
            for_end = j + l - 1;
            if (k <= for_end)
                do {
                    h = h + h + str_pool[k];
                    while (h >= PRIM_PRIME)
                        h = h - 431;
                }
                while (k++ < for_end);
        }
        p = h + 1;
    }

    while (true) {
        if (prim[p].s1 > 65536L) {
            if (length(prim[p].s1) - 1 == l) {
                    if (str_eq_str(prim[p].s1 - 1, s))
                        goto found;
            }
        } else if (prim[p].s1 == 1 + s)
            goto found;

        if (prim[p].s0 == 0) {
            if (no_new_control_sequence)
                p = UNDEFINED_PRIMITIVE;
            else { /*272:*/
                if (prim[p].s1 > 0) {
                    do {
                        if (prim_used == PRIM_BASE)
                            overflow("primitive size", PRIM_SIZE);
                        prim_used--;
                    } while (!(prim[prim_used].s1 == 0));
                    prim[p].s0 = prim_used;
                    p = prim_used;
                }
                prim[p].s1 = s + 1;
            }
            goto found;
        }
        p = prim[p].s0;
    }

found:
    return p;
}

/*:276*//*280: *//*296: */

void print_group(bool e)
{
    switch (cur_group) {
    case BOTTOM_LEVEL:
        print_cstr("bottom level");
        return;
    case SIMPLE_GROUP:
    case SEMI_SIMPLE_GROUP:
        {
            if (cur_group == SEMI_SIMPLE_GROUP)
                print_cstr("semi ");
            print_cstr("simple");
        }
        break;
    case HBOX_GROUP:
    case ADJUSTED_HBOX_GROUP:
        {
            if (cur_group == ADJUSTED_HBOX_GROUP)
                print_cstr("adjusted ");
            print_cstr("hbox");
        }
        break;
    case VBOX_GROUP:
        print_cstr("vbox");
        break;
    case VTOP_GROUP:
        print_cstr("vtop");
        break;
    case ALIGN_GROUP:
    case NO_ALIGN_GROUP:
        {
            if (cur_group == NO_ALIGN_GROUP)
                print_cstr("no ");
            print_cstr("align");
        }
        break;
    case OUTPUT_GROUP:
        print_cstr("output");
        break;
    case DISC_GROUP:
        print_cstr("disc");
        break;
    case INSERT_GROUP:
        print_cstr("insert");
        break;
    case VCENTER_GROUP:
        print_cstr("vcenter");
        break;
    case MATH_GROUP:
    case MATH_CHOICE_GROUP:
    case MATH_SHIFT_GROUP:
    case MATH_LEFT_GROUP:
        {
            print_cstr("math");
            if (cur_group == MATH_CHOICE_GROUP)
                print_cstr(" choice");
            else if (cur_group == MATH_SHIFT_GROUP)
                print_cstr(" shift");
            else if (cur_group == MATH_LEFT_GROUP)
                print_cstr(" left");
        }
        break;
    }
    print_cstr(" group (level ");
    print_int(cur_level);
    print_char(')');
    if (save_stack[save_ptr - 1].b32.s1 != 0) {
        if (e)
            print_cstr(" entered at line ");
        else
            print_cstr(" at line ");
        print_int(save_stack[save_ptr - 1].b32.s1);
    }
}

/*:1448*//*1449: */

bool pseudo_input(void)
{
    int32_t p;
    int32_t sz;
    b16x4 w;
    int32_t r;
    last = first;
    p = mem[pseudo_files].b32.s0;
    if (p == TEX_NULL)
        return false;
    else {

        mem[pseudo_files].b32.s0 = mem[p].b32.s1;
        sz = mem[p].b32.s0;
        if (4 * sz - 3 >= buf_size - last) {    /*35: */
            cur_input.loc = first;
            cur_input.limit = last - 1;
            overflow("buffer size", buf_size);
        }
        last = first;
        {
            register int32_t for_end;
            r = p + 1;
            for_end = p + sz - 1;
            if (r <= for_end)
                do {
                    w = mem[r].b16;
                    buffer[last] = w.s3;
                    buffer[last + 1] = w.s2;
                    buffer[last + 2] = w.s1;
                    buffer[last + 3] = w.s0;
                    last = last + 4;
                }
                while (r++ < for_end);
        }
        if (last >= max_buf_stack)
            max_buf_stack = last + 1;
        while ((last > first) && (buffer[last - 1] == ' ' ))
            last--;
        free_node(p, sz);
        return true;
    }
}

void pseudo_close(void)
{
    int32_t p, q;
    p = mem[pseudo_files].b32.s1;
    q = mem[pseudo_files].b32.s0;
    {
        mem[pseudo_files].b32.s1 = avail;
        avail = pseudo_files;
    }
    pseudo_files = p;
    while (q != TEX_NULL) {

        p = q;
        q = mem[p].b32.s1;
        free_node(p, mem[p].b32.s0);
    }
}

void group_warning(void)
{
    int32_t i;
    bool w;

    base_ptr = input_ptr;
    input_stack[base_ptr] = cur_input;
    i = in_open;
    w = false;
    while ((grp_stack[i] == cur_boundary) && (i > 0)) {

        if (INTPAR(tracing_nesting) > 0) {
            while ((input_stack[base_ptr].state == TOKEN_LIST) || (input_stack[base_ptr].index > i))
                base_ptr--;
            if (input_stack[base_ptr].name > 17)
                w = true;
        }
        grp_stack[i] = save_stack[save_ptr].b32.s1;
        i--;
    }
    if (w) {
        print_nl_cstr("Warning: ");
        diagnostic_begin_capture_warning_here();
        print_cstr("end of ");
        print_group(true);
        print_cstr(" of a different file");
        print_ln();

        if (INTPAR(tracing_nesting) > 1)
            show_context();
        if (history == HISTORY_SPOTLESS)
            history = HISTORY_WARNING_ISSUED;

        capture_to_diagnostic(NULL);
    }
}

void if_warning(void)
{
    int32_t i;
    bool w;

    base_ptr = input_ptr;
    input_stack[base_ptr] = cur_input;
    i = in_open;
    w = false;
    while (if_stack[i] == cond_ptr) {

        if (INTPAR(tracing_nesting) > 0) {
            while ((input_stack[base_ptr].state == TOKEN_LIST) || (input_stack[base_ptr].index > i))
                base_ptr--;
            if (input_stack[base_ptr].name > 17)
                w = true;
        }
        if_stack[i] = mem[cond_ptr].b32.s1;
        i--;
    }
    if (w) {
        print_nl_cstr("Warning: ");
        diagnostic_begin_capture_warning_here();
        print_cstr("end of ");

        print_cmd_chr(IF_TEST, cur_if);
        if (if_line != 0) {
            print_cstr(" entered on line ");
            print_int(if_line);
        }
        print_cstr(" of a different file");
        print_ln();
        if (INTPAR(tracing_nesting) > 1)
            show_context();

        capture_to_diagnostic(NULL);
        if (history == HISTORY_SPOTLESS)
            history = HISTORY_WARNING_ISSUED;
    }
}

void file_warning(void)
{
    int32_t p;
    uint16_t l;
    uint16_t c;
    int32_t i;

    p = save_ptr;
    l = cur_level;
    c = cur_group;
    save_ptr = cur_boundary;
    while (grp_stack[in_open] != save_ptr) {
        cur_level--;

        print_nl_cstr("Warning: ");
        diagnostic_begin_capture_warning_here();
        print_cstr("end of file when ");
        print_group(true);
        print_cstr(" is incomplete");
        capture_to_diagnostic(NULL);

        cur_group = save_stack[save_ptr].b16.s0;
        save_ptr = save_stack[save_ptr].b32.s1;
    }
    save_ptr = p;
    cur_level = l;
    cur_group = c;
    p = cond_ptr;
    l = if_limit;
    c = cur_if;
    i = if_line;
    while (if_stack[in_open] != cond_ptr) {
        print_nl_cstr("Warning: ");
        diagnostic_begin_capture_warning_here();
        print_cstr("end of file when ");
        print_cmd_chr(IF_TEST, cur_if);
        if (if_limit == FI_CODE)
            print_esc_cstr("else");
        if (if_line != 0) {
            print_cstr(" entered on line ");
            print_int(if_line);
        }
        print_cstr(" is incomplete");
        capture_to_diagnostic(NULL);

        if_line = mem[cond_ptr + 1].b32.s1;
        cur_if = mem[cond_ptr].b16.s0;
        if_limit = mem[cond_ptr].b16.s1;
        cond_ptr = LLIST_link(cond_ptr);
    }
    cond_ptr = p;
    if_limit = l;
    cur_if = c;
    if_line = i;
    print_ln();
    if (INTPAR(tracing_nesting) > 1) {
        diagnostic_begin_capture_warning_here();
        show_context();
        capture_to_diagnostic(NULL);
    }
    if (history == HISTORY_SPOTLESS)
        history = HISTORY_WARNING_ISSUED;
}

void delete_sa_ref(int32_t q)
{
    int32_t p;
    small_number i;
    small_number s;
    mem[q + 1].b32.s0--;
    if (mem[q + 1].b32.s0 != TEX_NULL)
        return;
    if (mem[q].b16.s1 < DIMEN_VAL_LIMIT) {

        if (mem[q + 2].b32.s1 == 0)
            s = WORD_NODE_SIZE;
        else
            return;
    } else {

        if (mem[q].b16.s1 < MU_VAL_LIMIT) {

            if (mem[q + 1].b32.s1 == 0)
                delete_glue_ref(0);
            else
                return;
        } else if (mem[q + 1].b32.s1 != TEX_NULL)
            return;
        s = POINTER_NODE_SIZE;
    }
    do {
        i = mem[q].b16.s1 % 64;
        p = q;
        q = mem[p].b32.s1;
        free_node(p, s);
        if (q == TEX_NULL) {
            sa_root[i] = TEX_NULL;
            return;
        }
        {
            if (odd(i))
                mem[q + (i / 2) + 1].b32.s1 = TEX_NULL;
            else
                mem[q + (i / 2) + 1].b32.s0 = TEX_NULL;
            mem[q].b16.s0--;
        }
        s = INDEX_NODE_SIZE;
    } while (!(mem[q].b16.s0 > 0));
}

/*:1609*//*1611: */

void sa_save(int32_t p)
{
    int32_t q;
    uint16_t i;
    if (cur_level != sa_level) {
        if (save_ptr > max_save_stack) {
            max_save_stack = save_ptr;
            if (max_save_stack > save_size - 7)
                overflow("save size", save_size);
        }
        save_stack[save_ptr].b16.s1 = RESTORE_SA;
        save_stack[save_ptr].b16.s0 = sa_level;
        save_stack[save_ptr].b32.s1 = sa_chain;
        save_ptr++;
        sa_chain = TEX_NULL;
        sa_level = cur_level;
    }
    i = mem[p].b16.s1;
    if (i < DIMEN_VAL_LIMIT) {
        if (mem[p + 2].b32.s1 == 0) {
            q = get_node(POINTER_NODE_SIZE);
            i = TOK_VAL_LIMIT;
        } else {

            q = get_node(WORD_NODE_SIZE);
            mem[q + 2].b32.s1 = mem[p + 2].b32.s1;
        }
        mem[q + 1].b32.s1 = TEX_NULL;
    } else {

        q = get_node(POINTER_NODE_SIZE);
        mem[q + 1].b32.s1 = mem[p + 1].b32.s1;
    }
    mem[q + 1].b32.s0 = p;
    mem[q].b16.s1 = i;
    mem[q].b16.s0 = mem[p].b16.s0;
    mem[q].b32.s1 = sa_chain;
    sa_chain = q;
    mem[p + 1].b32.s0++;
}

void sa_destroy(int32_t p)
{
    if (mem[p].b16.s1 < MU_VAL_LIMIT)
        delete_glue_ref(mem[p + 1].b32.s1);
    else if (mem[p + 1].b32.s1 != TEX_NULL) {

        if (mem[p].b16.s1 < BOX_VAL_LIMIT)
            flush_node_list(mem[p + 1].b32.s1);
        else
            delete_token_ref(mem[p + 1].b32.s1);
    }
}

void sa_def(int32_t p, int32_t e)
{

    mem[p + 1].b32.s0++;
    if (mem[p + 1].b32.s1 == e) {
        sa_destroy(p);
    } else {
        if (mem[p].b16.s0 == cur_level)
            sa_destroy(p);
        else
            sa_save(p);
        mem[p].b16.s0 = cur_level;
        mem[p + 1].b32.s1 = e;
    }
    delete_sa_ref(p);
}

void sa_w_def(int32_t p, int32_t w)
{

    mem[p + 1].b32.s0++;

    if (mem[p + 2].b32.s1 == w) {
    } else {
        if (mem[p].b16.s0 != cur_level)
            sa_save(p);
        mem[p].b16.s0 = cur_level;
        mem[p + 2].b32.s1 = w;
    }
    delete_sa_ref(p);
}

void gsa_def(int32_t p, int32_t e)
{

    mem[p + 1].b32.s0++;
    sa_destroy(p);
    mem[p].b16.s0 = LEVEL_ONE;
    mem[p + 1].b32.s1 = e;
    delete_sa_ref(p);
}

void gsa_w_def(int32_t p, int32_t w)
{

    mem[p + 1].b32.s0++;
    mem[p].b16.s0 = LEVEL_ONE;
    mem[p + 2].b32.s1 = w;
    delete_sa_ref(p);
}

void sa_restore(void)
{
    int32_t p;

    do {
        p = mem[sa_chain + 1].b32.s0;
        if (mem[p].b16.s0 == LEVEL_ONE) {
            if (mem[p].b16.s1 >= DIMEN_VAL_LIMIT)
                sa_destroy(sa_chain);
        } else {

            if (mem[p].b16.s1 < DIMEN_VAL_LIMIT) {

                if (mem[sa_chain].b16.s1 < DIMEN_VAL_LIMIT)
                    mem[p + 2].b32.s1 = mem[sa_chain + 2].b32.s1;
                else
                    mem[p + 2].b32.s1 = 0;
            } else {

                sa_destroy(p);
                mem[p + 1].b32.s1 = mem[sa_chain + 1].b32.s1;
            }
            mem[p].b16.s0 = mem[sa_chain].b16.s0;
        }
        delete_sa_ref(p);
        p = sa_chain;
        sa_chain = mem[p].b32.s1;
        if (mem[p].b16.s1 < DIMEN_VAL_LIMIT)
            free_node(p, WORD_NODE_SIZE);
        else
            free_node(p, POINTER_NODE_SIZE);
    } while (!(sa_chain == TEX_NULL));
}

void new_save_level(group_code c)
{
    if (save_ptr > max_save_stack) {
        max_save_stack = save_ptr;
        if (max_save_stack > save_size - 7)
            overflow("save size", save_size);
    }

    save_stack[save_ptr + 0].b32.s1 = line;
    save_ptr++;
    save_stack[save_ptr].b16.s1 = LEVEL_BOUNDARY;
    save_stack[save_ptr].b16.s0 = cur_group;
    save_stack[save_ptr].b32.s1 = cur_boundary;
    if (cur_level == UINT16_MAX)
        overflow("grouping levels", UINT16_MAX);
    cur_boundary = save_ptr;
    cur_group = c;
    cur_level++;
    save_ptr++;
}

void eq_destroy(memory_word w)
{
    int32_t q;
    switch (w.b16.s1) {
    case 113:
    case 114:
    case 115:
    case 116:
        delete_token_ref(w.b32.s1);
        break;
    case 119:
        delete_glue_ref(w.b32.s1);
        break;
    case 120:
        {
            q = w.b32.s1;
            if (q != TEX_NULL)
                free_node(q, mem[q].b32.s0 + mem[q].b32.s0 + 1);
        }
        break;
    case 121:
        flush_node_list(w.b32.s1);
        break;
    case 72:
    case 91:
        if (w.b32.s1 < 0 || w.b32.s1 > 19)
            delete_sa_ref(w.b32.s1);
        break;
    default:
        ;
        break;
    }
}

void eq_save(int32_t p, uint16_t l)
{

    if (save_ptr > max_save_stack) {
        max_save_stack = save_ptr;
        if (max_save_stack > save_size - 7)
            overflow("save size", save_size);
    }
    if (l == LEVEL_ZERO)
        save_stack[save_ptr].b16.s1 = RESTORE_ZERO;
    else {

        save_stack[save_ptr] = eqtb[p];
        save_ptr++;
        save_stack[save_ptr].b16.s1 = RESTORE_OLD_VALUE;
    }
    save_stack[save_ptr].b16.s0 = l;
    save_stack[save_ptr].b32.s1 = p;
    save_ptr++;
}

void
eq_define(int32_t p, uint16_t t, int32_t e)
{

    if (eqtb[p].b16.s1 == t && eqtb[p].b32.s1 == e) {
        eq_destroy(eqtb[p]);
        return;
    }

    if (eqtb[p].b16.s0 == cur_level)
        eq_destroy(eqtb[p]);
    else if (cur_level > LEVEL_ONE)
        eq_save(p, eqtb[p].b16.s0);

    eqtb[p].b16.s0 = cur_level;
    eqtb[p].b16.s1 = t;
    eqtb[p].b32.s1 = e;
}

void
eq_word_define(int32_t p, int32_t w)
{

    if (eqtb[p].b32.s1 == w)
        return;

    if (XEQ_LEVEL(p) != cur_level) {
        eq_save(p, XEQ_LEVEL(p));
        XEQ_LEVEL(p) = cur_level;
    }
    eqtb[p].b32.s1 = w;
}

void geq_define(int32_t p, uint16_t t, int32_t e)
{

    eq_destroy(eqtb[p]);
    eqtb[p].b16.s0 = LEVEL_ONE;
    eqtb[p].b16.s1 = t;
    eqtb[p].b32.s1 = e;
}

void geq_word_define(int32_t p, int32_t w)
{

    eqtb[p].b32.s1 = w;
    XEQ_LEVEL(p) = LEVEL_ONE;
}

void save_for_after(int32_t t)
{
    if (cur_level > LEVEL_ONE) {
        if (save_ptr > max_save_stack) {
            max_save_stack = save_ptr;
            if (max_save_stack > save_size - 7)
                overflow("save size", save_size);
        }
        save_stack[save_ptr].b16.s1 = INSERT_TOKEN;
        save_stack[save_ptr].b16.s0 = LEVEL_ZERO;
        save_stack[save_ptr].b32.s1 = t;
        save_ptr++;
    }
}

void unsave(void)
{
    int32_t p;
    uint16_t l = 0;
    int32_t t;
    bool a;

    a = false;
    if (cur_level > LEVEL_ONE) {
        cur_level--;
        while (true) {

            save_ptr--;
            if (save_stack[save_ptr].b16.s1 == LEVEL_BOUNDARY)
                goto done;
            p = save_stack[save_ptr].b32.s1;
            if (save_stack[save_ptr].b16.s1 == INSERT_TOKEN) {   /*338: */
                t = cur_tok;
                cur_tok = p;
                if (a) {
                    p = get_avail();
                    mem[p].b32.s0 = cur_tok;
                    mem[p].b32.s1 = cur_input.loc;
                    cur_input.loc = p;
                    cur_input.start = p;
                    if (cur_tok < RIGHT_BRACE_LIMIT) {

                        if (cur_tok < LEFT_BRACE_LIMIT)
                            align_state--;
                        else
                            align_state++;
                    }
                } else {
                    back_input();
                    a = true;
                }
                cur_tok = t;
            } else if (save_stack[save_ptr].b16.s1 == RESTORE_SA) {
                sa_restore();
                sa_chain = p;
                sa_level = save_stack[save_ptr].b16.s0;
            } else {

                if (save_stack[save_ptr].b16.s1 == RESTORE_OLD_VALUE) {
                    l = save_stack[save_ptr].b16.s0;
                    save_ptr--;
                } else
                    save_stack[save_ptr] = eqtb[UNDEFINED_CONTROL_SEQUENCE];
                if ((p < INT_BASE) || (p > EQTB_SIZE)) {

                    if (eqtb[p].b16.s0 == LEVEL_ONE) {
                        eq_destroy(save_stack[save_ptr]);
                    } else {
                        eq_destroy(eqtb[p]);
                        eqtb[p] = save_stack[save_ptr];
                    }
                } else if (XEQ_LEVEL(p) != LEVEL_ONE) {
                    eqtb[p] = save_stack[save_ptr];
                    XEQ_LEVEL(p) = l;
                }
            }
        }

    done:
        if (grp_stack[in_open] == cur_boundary)
            group_warning();
        cur_group = save_stack[save_ptr].b16.s0;
        cur_boundary = save_stack[save_ptr].b32.s1;
        save_ptr--;
    } else
        confusion("curlevel");
}

void prepare_mag(void)
{

    if (mag_set > 0 && INTPAR(mag) != mag_set) {
        ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Incompatible magnification (");
        print_int(INTPAR(mag));
        print_cstr(");");
        print_nl_cstr(" the previous value will be retained");

        ttstub_diag_printf(errmsg, " (%d)", mag_set);
        capture_to_diagnostic(NULL);

        {
            help_ptr = 2;
            help_line[1] = "I can handle only one magnification ratio per job. So I've";
            help_line[0] = "reverted to the magnification you used earlier on this run.";
        }
        int_error(mag_set);
        geq_word_define(INT_BASE + INT_PAR__mag, mag_set);
    }
    if ((INTPAR(mag) <= 0) || (INTPAR(mag) > 32768L)) {
        ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Illegal magnification has been changed to 1000");
        ttstub_diag_printf(errmsg, " (%d)", INTPAR(mag));
        capture_to_diagnostic(NULL);

        {
            help_ptr = 1;
            help_line[0] = "The magnification ratio must be between 1 and 32768.";
        }
        int_error(INTPAR(mag));
        geq_word_define(INT_BASE + INT_PAR__mag, 1000);
    }
    mag_set = INTPAR(mag);
}

void token_show(int32_t p)
{

    if (p != TEX_NULL)
        show_token_list(mem[p].b32.s1, TEX_NULL, 10000000L);
}

void print_meaning(void)
{
    print_cmd_chr(cur_cmd, cur_chr);
    if (cur_cmd >= CALL) {
        print_char(':');
        print_ln();
        token_show(cur_chr);
    } else if ((cur_cmd == TOP_BOT_MARK) && (cur_chr < 5)) {
        print_char(':');
        print_ln();
        token_show(cur_mark[cur_chr]);
    }
}

// Used just to trace commands - all gated behind INTPAR(tracing_commands)
void show_cur_cmd_chr(void)
{
    int32_t n;
    int32_t l;
    int32_t p;

    begin_diagnostic();

    diagnostic_begin_capture_warning_here();

    print_nl('{');
    if (cur_list.mode != shown_mode) {
        print_mode(cur_list.mode);
        print_cstr(": ");
        shown_mode = cur_list.mode;
    }
    print_cmd_chr(cur_cmd, cur_chr);
    if (INTPAR(tracing_ifs) > 0) {

        if (cur_cmd >= IF_TEST) {

            if (cur_cmd <= FI_OR_ELSE) {
                print_cstr(": ");
                if (cur_cmd == FI_OR_ELSE) {
                    print_cmd_chr(IF_TEST, cur_if);
                    print_char(' ');
                    n = 0;
                    l = if_line;
                } else {

                    n = 1;
                    l = line;
                }
                p = cond_ptr;
                while (p != TEX_NULL) {

                    n++;
                    p = LLIST_link(p);
                }
                print_cstr("(level ");
                print_int(n);
                print_char(')');
                if (l != 0) {
                    print_cstr(" entered on line ");
                    print_int(l);
                }
            }
        }
    }
    print_char('}');

    capture_to_diagnostic(NULL);

    end_diagnostic(false);
}

void show_context(void)
{
    unsigned char /*max_selector */ old_setting;
    int32_t nn;
    bool bottom_line;
    int32_t i;
    int32_t j;
    int32_t l;
    int32_t m;
    int32_t n;
    int32_t p;
    int32_t q;

    base_ptr = input_ptr;
    input_stack[base_ptr] = cur_input;
    nn = -1;
    bottom_line = false;

    while (true) {

        cur_input = input_stack[base_ptr];
        if ((cur_input.state != TOKEN_LIST)) {

            if ((cur_input.name > 19) || (base_ptr == 0))
                bottom_line = true;
        }
        if ((base_ptr == input_ptr) || bottom_line || (nn < INTPAR(error_context_lines))) {   /*324: */
            if ((base_ptr == input_ptr) || (cur_input.state != TOKEN_LIST)
                || (cur_input.index != BACKED_UP) || (cur_input.loc != TEX_NULL)) {
                tally = 0;
                old_setting = selector;
                if (cur_input.state != TOKEN_LIST) {
                    if (cur_input.name <= 17) {

                        if (cur_input.name == 0) {

                            if (base_ptr == 0)
                                print_nl_cstr("<*>");
                            else
                                print_nl_cstr("<insert> ");
                        } else {

                            print_nl_cstr("<read ");
                            if (cur_input.name == 17)
                                print_char('*');
                            else
                                print_int(cur_input.name - 1);
                            print_char('>');
                        }
                    } else {

                        print_nl_cstr("l.");
                        if (cur_input.index == in_open)
                            print_int(line);
                        else
                            print_int(line_stack[cur_input.index + 1]);
                    }
                    print_char(' ');
                    {
                        l = tally;
                        tally = 0;
                        selector = SELECTOR_PSEUDO;
                        trick_count = 1000000L;
                    }
                    if (buffer[cur_input.limit] == INTPAR(end_line_char))
                        j = cur_input.limit;
                    else
                        j = cur_input.limit + 1;
                    if (j > 0) {
                        register int32_t for_end;
                        i = cur_input.start;
                        for_end = j - 1;
                        if (i <= for_end)
                            do {
                                if (i == cur_input.loc) {
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

                    switch (cur_input.index) {
                    case PARAMETER:
                        print_nl_cstr("<argument> ");
                        break;
                    case U_TEMPLATE:
                    case V_TEMPLATE:
                        print_nl_cstr("<template> ");
                        break;
                    case BACKED_UP:
                    case BACKED_UP_CHAR:
                        if (cur_input.loc == TEX_NULL)
                            print_nl_cstr("<recently read> ");
                        else
                            print_nl_cstr("<to be read again> ");
                        break;
                    case INSERTED:
                        print_nl_cstr("<inserted text> ");
                        break;
                    case MACRO:
                        print_ln();
                        print_cs(cur_input.name);
                        break;
                    case OUTPUT_TEXT:
                        print_nl_cstr("<output> ");
                        break;
                    case EVERY_PAR_TEXT:
                        print_nl_cstr("<everypar> ");
                        break;
                    case EVERY_MATH_TEXT:
                        print_nl_cstr("<everymath> ");
                        break;
                    case EVERY_DISPLAY_TEXT:
                        print_nl_cstr("<everydisplay> ");
                        break;
                    case EVERY_HBOX_TEXT:
                        print_nl_cstr("<everyhbox> ");
                        break;
                    case EVERY_VBOX_TEXT:
                        print_nl_cstr("<everyvbox> ");
                        break;
                    case EVERY_JOB_TEXT:
                        print_nl_cstr("<everyjob> ");
                        break;
                    case EVERY_CR_TEXT:
                        print_nl_cstr("<everycr> ");
                        break;
                    case MARK_TEXT:
                        print_nl_cstr("<mark> ");
                        break;
                    case EVERY_EOF_TEXT:
                        print_nl_cstr("<everyeof> ");
                        break;
                    case INTER_CHAR_TEXT:
                        print_nl_cstr("<XeTeXinterchartoks> ");
                        break;
                    case WRITE_TEXT:
                        print_nl_cstr("<write> ");
                        break;
                    case TECTONIC_CODA_TEXT:
                        print_nl_cstr("<TectonicCodaTokens> ");
                        break;
                    default:
                        print_nl('?' );
                        break;
                    }
                    {
                        l = tally;
                        tally = 0;
                        selector = SELECTOR_PSEUDO;
                        trick_count = 1000000L;
                    }
                    if (cur_input.index < MACRO)
                        show_token_list(cur_input.start, cur_input.loc, 100000L);
                    else
                        show_token_list(mem[cur_input.start].b32.s1, cur_input.loc, 100000L);
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

                    print_cstr("...");
                    p = l + first_count - half_error_line + 3;
                    n = half_error_line;
                }
                {
                    register int32_t for_end;
                    q = p;
                    for_end = first_count - 1;
                    if (q <= for_end)
                        do
                            print_char(trick_buf[q % error_line]);
                        while (q++ < for_end);
                }
                print_ln();
                {
                    register int32_t for_end;
                    q = 1;
                    for_end = n;
                    if (q <= for_end)
                        do
                            print_raw_char(' ', true);
                        while (q++ < for_end);
                }
                if (m + n <= error_line)
                    p = first_count + m;
                else
                    p = first_count + (error_line - n - 3);
                {
                    register int32_t for_end;
                    q = first_count;
                    for_end = p - 1;
                    if (q <= for_end)
                        do
                            print_char(trick_buf[q % error_line]);
                        while (q++ < for_end);
                }
                if (m + n > error_line)
                    print_cstr("...");
                nn++;
            }
        } else if (nn == INTPAR(error_context_lines)) {
            print_nl_cstr("...");
            nn++;
        }
        if (bottom_line)
            goto done;
        base_ptr--;
    }
done:
    cur_input = input_stack[input_ptr];
}


void
begin_token_list(int32_t p, uint16_t t)
{

    if (input_ptr > max_in_stack) {
        max_in_stack = input_ptr;
        if (input_ptr == stack_size)
            overflow("input stack size", stack_size);
    }

    input_stack[input_ptr] = cur_input;
    input_ptr++;

    cur_input.state = TOKEN_LIST;
    cur_input.start = p;
    cur_input.index = t;

    if (t >= MACRO) {
        mem[p].b32.s0++;

        if (t == MACRO) {
            cur_input.limit = param_ptr;
        } else {
            cur_input.loc = mem[p].b32.s1;

            if (INTPAR(tracing_macros) > 1) {
                begin_diagnostic();
                diagnostic_begin_capture_warning_here();
                print_nl_cstr("");
                switch (t) {
                case MARK_TEXT:
                    print_esc_cstr("mark");
                    break;
                case WRITE_TEXT:
                    print_esc_cstr("write");
                    break;
                default:
                    print_cmd_chr(ASSIGN_TOKS, t + LOCAL_BASE + LOCAL__output_routine - OUTPUT_TEXT);
                    break;
                }
                print_cstr("->");
                token_show(p);
                capture_to_diagnostic(NULL);
                end_diagnostic(false);
            }
        }
    } else {
        cur_input.loc = p;
    }
}


void end_token_list(void)
{
    if (cur_input.index >= BACKED_UP) {
        if (cur_input.index <= INSERTED)
            flush_list(cur_input.start);
        else {

            delete_token_ref(cur_input.start);
            if (cur_input.index == MACRO)
                while (param_ptr > cur_input.limit) {

                    param_ptr--;
                    flush_list(param_stack[param_ptr]);
                }
        }
    } else if (cur_input.index == U_TEMPLATE) {

        if (align_state > 500000L)
            align_state = 0;
        else
            fatal_error("(interwoven alignment preambles are not allowed)");
    }
    {
        input_ptr--;
        cur_input = input_stack[input_ptr];
    }
}

void back_input(void)
{
    int32_t p;
    while ((cur_input.state == TOKEN_LIST) && (cur_input.loc == TEX_NULL)
           && (cur_input.index != V_TEMPLATE))
        end_token_list();
    p = get_avail();
    mem[p].b32.s0 = cur_tok;
    if (cur_tok < RIGHT_BRACE_LIMIT) {

        if (cur_tok < LEFT_BRACE_LIMIT)
            align_state--;
        else
            align_state++;
    }
    {
        if (input_ptr > max_in_stack) {
            max_in_stack = input_ptr;
            if (input_ptr == stack_size)
                overflow("input stack size", stack_size);
        }
        input_stack[input_ptr] = cur_input;
        input_ptr++;
    }
    cur_input.state = TOKEN_LIST;
    cur_input.start = p;
    cur_input.index = BACKED_UP;
    cur_input.loc = p;
}

void
back_error(void)
{
    back_input();
    error();
}

void
ins_error(void)
{
    back_input();
    cur_input.index = INSERTED;
    error();
}

void begin_file_reading(void)
{
    if (in_open == max_in_open)
        overflow("text input levels", max_in_open);
    if (first == buf_size)
        overflow("buffer size", buf_size);
    in_open++;
    {
        if (input_ptr > max_in_stack) {
            max_in_stack = input_ptr;
            if (input_ptr == stack_size)
                overflow("input stack size", stack_size);
        }
        input_stack[input_ptr] = cur_input;
        input_ptr++;
    }
    cur_input.index = in_open;
    source_filename_stack[cur_input.index] = 0;
    full_source_filename_stack[cur_input.index] = 0;
    eof_seen[cur_input.index] = false;
    grp_stack[cur_input.index] = cur_boundary;
    if_stack[cur_input.index] = cond_ptr;
    line_stack[cur_input.index] = line;
    cur_input.start = first;
    cur_input.state = MID_LINE;
    cur_input.name = 0;
    cur_input.synctex_tag = 0;
}

void end_file_reading(void)
{
    first = cur_input.start;
    line = line_stack[cur_input.index];
    if ((cur_input.name == 18) || (cur_input.name == 19))
        pseudo_close();
    else if (cur_input.name > 17)
        u_close(input_file[cur_input.index]);
    {
        input_ptr--;
        cur_input = input_stack[input_ptr];
    }
    in_open--;
}


void
check_outer_validity(void)
{
    int32_t p;
    int32_t q;

    if (scanner_status != NORMAL) {
        deletions_allowed = false;

        if (cur_cs != 0) {
            if (cur_input.state == TOKEN_LIST || cur_input.name < 1 || cur_input.name > 17) {
                p = get_avail();
                mem[p].b32.s0 = CS_TOKEN_FLAG + cur_cs;
                begin_token_list(p, BACKED_UP);
            }

            cur_cmd = SPACER;
            cur_chr = ' ' ;
        }

        if (scanner_status > SKIPPING) { /*350:*/
            runaway();

            if (cur_cs == 0) {
                error_here_with_diagnostic("File ended");
            } else {
                cur_cs = 0;
                error_here_with_diagnostic("Forbidden control sequence found");
            }

            p = get_avail();

            switch (scanner_status) {
            case DEFINING:
                print_cstr(" while scanning definition");
                mem[p].b32.s0 = (RIGHT_BRACE_TOKEN + '}' );
                break;
            case MATCHING:
                print_cstr(" while scanning use");
                mem[p].b32.s0 = par_token;
                long_state = OUTER_CALL;
                break;

            case ALIGNING:
                print_cstr(" while scanning preamble");
                mem[p].b32.s0 = (RIGHT_BRACE_TOKEN + '}' );
                q = p;
                p = get_avail();
                mem[p].b32.s1 = q;
                mem[p].b32.s0 = CS_TOKEN_FLAG + FROZEN_CR;
                align_state = -1000000L;
                break;

            case ABSORBING:
                print_cstr(" while scanning text");
                mem[p].b32.s0 = (RIGHT_BRACE_TOKEN + '}' );
                break;
            }

            begin_token_list(p, INSERTED);
            print_cstr(" of ");
            sprint_cs(warning_index);
            capture_to_diagnostic(NULL);
            help_ptr = 4;
            help_line[3] = "I suspect you have forgotten a `}', causing me";
            help_line[2] = "to read past where you wanted me to stop.";
            help_line[1] = "I'll try to recover; but if the error is serious,";
            help_line[0] = "you'd better type `E' or `X' now and fix your file.";
            error();
        } else {
            error_here_with_diagnostic("Incomplete ");
            print_cmd_chr(IF_TEST, cur_if);
            print_cstr("; all text was ignored after line ");
            print_int(skip_line);
            capture_to_diagnostic(NULL);
            help_ptr = 3;
            help_line[2] = "A forbidden control sequence occurred in skipped text.";
            help_line[1] = "This kind of error happens when you say `\\if...' and forget";
            help_line[0] = "the matching `\\fi'. I've inserted a `\\fi'; this might work.";

            if (cur_cs != 0)
                cur_cs = 0;
            else
                help_line[2] = "The file ended while I was skipping conditional text.";

            cur_tok = CS_TOKEN_FLAG + FROZEN_FI;
            ins_error();
        }

        deletions_allowed = true;
    }
}


/* These macros are kinda scary, but convenient */
#define ANY_STATE_PLUS(c) case (MID_LINE + c): case (SKIP_BLANKS + c): case (NEW_LINE + c)
#define ADD_DELIMS_TO(s) \
    case (s + MATH_SHIFT): case (s + TAB_MARK): case (s + MAC_PARAM): \
    case (s + SUB_MARK): case (s + LETTER): case (s + OTHER_CHAR)


void
get_next(void)
{
    int32_t k;
    int32_t t;
    unsigned char /*max_char_code */ cat;
    UnicodeScalar c;
    UTF16_code lower;
    small_number d;
    small_number sup_count;

restart:
    cur_cs = 0;

    if (cur_input.state != TOKEN_LIST) { /*355:*/
    texswitch:
        if (cur_input.loc <= cur_input.limit) {
            cur_chr = buffer[cur_input.loc];
            cur_input.loc++;

            if (cur_chr >= 0xD800 && cur_chr < 0xDC00 && cur_input.loc <= cur_input.limit &&
                buffer[cur_input.loc] >= 0xDC00 && buffer[cur_input.loc] < 0xE000) {
                lower = buffer[cur_input.loc] - 0xDC00;
                cur_input.loc++;
                cur_chr = 65536L + (cur_chr - 0xD800) * 1024 + lower;
            }

        reswitch:
            cur_cmd = CAT_CODE(cur_chr);

            switch (cur_input.state + cur_cmd) { /*357:*/
            ANY_STATE_PLUS(IGNORE):
            case SKIP_BLANKS + SPACER:
            case NEW_LINE + SPACER:
                goto texswitch;
                break;

            ANY_STATE_PLUS(ESCAPE):
                if (cur_input.loc > cur_input.limit) {
                    cur_cs = NULL_CS;
                } else {
                start_cs:
                    k = cur_input.loc;
                    cur_chr = buffer[k];
                    cat = CAT_CODE(cur_chr);
                    k++;

                    if (cat == LETTER)
                        cur_input.state = SKIP_BLANKS;
                    else if (cat == SPACER)
                        cur_input.state = SKIP_BLANKS;
                    else
                        cur_input.state = MID_LINE;

                    if (cat == LETTER && k <= cur_input.limit) { /*368:*/
                        do {
                            cur_chr = buffer[k];
                            cat = CAT_CODE(cur_chr);
                            k++;
                        } while (cat == LETTER && k <= cur_input.limit);

                        if (cat == SUP_MARK && buffer[k] == cur_chr && k < cur_input.limit) {
                            /* Special characters: either ^^X, or up to six
                             * ^'s followed by one hex character for each
                             * ^. */

                            int32_t sup_count_save;

                            /* How many ^'s are there? */

                            sup_count = 2;

                            while (sup_count < 6 && k + 2 * sup_count - 2 <= cur_input.limit &&
                                   buffer[k + sup_count - 1] == cur_chr)
                                sup_count++;

                            /* If they are followed by a sufficient number of
                             * hex characters, treat it as an extended ^^^
                             * sequence. If not, treat it as original-style
                             * ^^X. */

                            sup_count_save = sup_count;

                            for (d = 1; d <= sup_count_save; d++) {
                                if (!IS_LC_HEX(buffer[k + sup_count - 2 + d])) {
                                    /* Non-hex: do it old style */
                                    c = buffer[k + 1];

                                    if (c < 128) {
                                        if (c < 64)
                                            buffer[k - 1] = c + 64;
                                        else
                                            buffer[k - 1] = c - 64;
                                        d = 2;
                                        cur_input.limit = cur_input.limit - d;
                                        while (k <= cur_input.limit) {
                                            buffer[k] = buffer[k + d];
                                            k++;
                                        }
                                        goto start_cs;
                                    } else {
                                        sup_count = 0;
                                    }
                                }
                            }

                            if (sup_count > 0) {
                                cur_chr = 0;

                                for (d = 1; d <= sup_count; d++) {
                                    c = buffer[k + sup_count - 2 + d];
                                    if (c <= '9' )
                                        cur_chr = 16 * cur_chr + c - '0';
                                    else
                                        cur_chr = 16 * cur_chr + c - 'a' + 10;
                                }

                                if (cur_chr > BIGGEST_USV) {
                                    cur_chr = buffer[k];
                                } else {
                                    buffer[k - 1] = cur_chr;
                                    d = 2 * sup_count - 1;
                                    cur_input.limit = cur_input.limit - d;

                                    while (k <= cur_input.limit) {
                                        buffer[k] = buffer[k + d];
                                        k++;
                                    }
                                    goto start_cs;
                                }
                            }
                        }

                        if (cat != LETTER)
                            k--;

                        if (k > cur_input.loc + 1) {
                            cur_cs = id_lookup(cur_input.loc, k - cur_input.loc);
                            cur_input.loc = k;
                            goto found;
                        }
                    } else { /*367:*/
                        if (cat == SUP_MARK && buffer[k] == cur_chr && k < cur_input.limit) {
                            int32_t sup_count_save;

                            sup_count = 2;

                            while (sup_count < 6 && k + 2 * sup_count - 2 <= cur_input.limit &&
                                   buffer[k + sup_count - 1] == cur_chr)
                                sup_count++;

                            sup_count_save = sup_count;

                            for (d = 1; d <= sup_count_save; d++) {
                                if (!IS_LC_HEX(buffer[k + sup_count - 2 + d])) {
                                    c = buffer[k + 1];
                                    if (c < 128) {
                                        if (c < 64)
                                            buffer[k - 1] = c + 64;
                                        else
                                            buffer[k - 1] = c - 64;
                                        d = 2;
                                        cur_input.limit = cur_input.limit - d;
                                        while (k <= cur_input.limit) {
                                            buffer[k] = buffer[k + d];
                                            k++;
                                        }
                                        goto start_cs;
                                    } else {
                                        sup_count = 0;
                                    }
                                }
                            }

                            if (sup_count > 0) {
                                cur_chr = 0;

                                for (d = 1; d <= sup_count; d++) {
                                    c = buffer[k + sup_count - 2 + d];
                                    if (c <= '9' )
                                        cur_chr = 16 * cur_chr + c - '0';
                                    else
                                        cur_chr = 16 * cur_chr + c - 'a' + 10;
                                }

                                if (cur_chr > BIGGEST_USV) {
                                    cur_chr = buffer[k];
                                } else {
                                    buffer[k - 1] = cur_chr;
                                    d = 2 * sup_count - 1;
                                    cur_input.limit = cur_input.limit - d;
                                    while (k <= cur_input.limit) {
                                        buffer[k] = buffer[k + d];
                                        k++;
                                    }
                                    goto start_cs;
                                }
                            }
                        }
                    }

                    if (buffer[cur_input.loc] > 65535L) {
                        cur_cs = id_lookup(cur_input.loc, 1);
                        cur_input.loc++;
                        goto found;
                    }

                    cur_cs = SINGLE_BASE + buffer[cur_input.loc];
                    cur_input.loc++;
                }

            found:
                cur_cmd = eqtb[cur_cs].b16.s1;
                cur_chr = eqtb[cur_cs].b32.s1;
                if (cur_cmd >= OUTER_CALL)
                    check_outer_validity();
                break;

            ANY_STATE_PLUS(ACTIVE_CHAR):
                cur_cs = cur_chr + 1;
                cur_cmd = eqtb[cur_cs].b16.s1;
                cur_chr = eqtb[cur_cs].b32.s1;
                cur_input.state = MID_LINE;
                if (cur_cmd >= OUTER_CALL)
                    check_outer_validity();
                break;

            ANY_STATE_PLUS(SUP_MARK):
                if (cur_chr == buffer[cur_input.loc]) {
                    if (cur_input.loc < cur_input.limit) {
                        sup_count = 2;

                        while (sup_count < 6 && cur_input.loc + 2 * sup_count - 2 <= cur_input.limit &&
                               cur_chr == buffer[cur_input.loc + sup_count - 1])
                            sup_count++;

                        for (d = 1; d <= sup_count; d++) {
                            if (!IS_LC_HEX(buffer[cur_input.loc + sup_count - 2 + d])) {
                                c = buffer[cur_input.loc + 1];
                                if (c < 128) {
                                    cur_input.loc = cur_input.loc + 2;
                                    if (c < 64)
                                        cur_chr = c + 64;
                                    else
                                        cur_chr = c - 64;
                                    goto reswitch;
                                }
                                goto not_exp;
                            }
                        }

                        cur_chr = 0;

                        for (d = 1; d <= sup_count; d++) {
                            c = buffer[cur_input.loc + sup_count - 2 + d];
                            if (c <= '9' )
                                cur_chr = 16 * cur_chr + c - '0';
                            else
                                cur_chr = 16 * cur_chr + c - 'a' + 10;
                        }

                        if (cur_chr > BIGGEST_USV) {
                            cur_chr = buffer[cur_input.loc];
                            goto not_exp;
                        }

                        cur_input.loc = cur_input.loc + 2 * sup_count - 1;
                        goto reswitch;
                    }
                }

            not_exp:
                cur_input.state = MID_LINE;
                break;

            ANY_STATE_PLUS(INVALID_CHAR):
                error_here_with_diagnostic("Text line contains an invalid character");
                capture_to_diagnostic(NULL);

                help_ptr = 2;
                help_line[1] = "A funny symbol that I can't read has just been input.";
                help_line[0] = "Continue, and I'll forget that it ever happened.";
                deletions_allowed = false;
                error();
                deletions_allowed = true;
                goto restart;
                break;

            case MID_LINE + SPACER:
                cur_input.state = SKIP_BLANKS;
                cur_chr = ' ' ;
                break;

            case MID_LINE + CAR_RET:
                cur_input.loc = cur_input.limit + 1;
                cur_cmd = SPACER;
                cur_chr = ' ' ;
                break;

            ANY_STATE_PLUS(COMMENT):
            case SKIP_BLANKS + CAR_RET:
                cur_input.loc = cur_input.limit + 1;
                goto texswitch;
                break;

            case NEW_LINE + CAR_RET:
                cur_input.loc = cur_input.limit + 1;
                cur_cs = par_loc;
                cur_cmd = eqtb[cur_cs].b16.s1;
                cur_chr = eqtb[cur_cs].b32.s1;
                if (cur_cmd >= OUTER_CALL)
                    check_outer_validity();
                break;

            case MID_LINE + LEFT_BRACE:
                align_state++;
                break;

            case SKIP_BLANKS + LEFT_BRACE:
            case NEW_LINE + LEFT_BRACE:
                cur_input.state = MID_LINE;
                align_state++;
                break;

            case MID_LINE + RIGHT_BRACE:
                align_state--;
                break;

            case SKIP_BLANKS + RIGHT_BRACE:
            case NEW_LINE + RIGHT_BRACE:
                cur_input.state = MID_LINE;
                align_state--;
                break;

            ADD_DELIMS_TO(SKIP_BLANKS):
            ADD_DELIMS_TO(NEW_LINE):
                cur_input.state = MID_LINE;
                break;

            default:
                break;
            }
        } else {
            cur_input.state = NEW_LINE;

            if (cur_input.name > 17) { /*374:*/
                line++;
                first = cur_input.start;

                if (!force_eof) {
                    if (cur_input.name <= 19) {
                        if (pseudo_input()) {
                            cur_input.limit = last;
                        } else if (LOCAL(every_eof) != TEX_NULL && !eof_seen[cur_input.index]) {
                            cur_input.limit = first - 1;
                            eof_seen[cur_input.index] = true;
                            begin_token_list(LOCAL(every_eof), EVERY_EOF_TEXT);
                            goto restart;
                        } else {
                            force_eof = true;
                        }
                    } else {
                        if (input_line(input_file[cur_input.index])) {
                            cur_input.limit = last;
                        } else if (LOCAL(every_eof) != TEX_NULL && !eof_seen[cur_input.index]) {
                            cur_input.limit = first - 1;
                            eof_seen[cur_input.index] = true;
                            begin_token_list(LOCAL(every_eof), EVERY_EOF_TEXT);
                            goto restart;
                        } else {
                            force_eof = true;
                        }
                    }
                }

                if (force_eof) {
                    if (INTPAR(tracing_nesting) > 0) {
                        if (grp_stack[in_open] != cur_boundary || if_stack[in_open] != cond_ptr)
                            file_warning();
                    }

                    if (cur_input.name >= 19) {
                        print_char(')');
                        open_parens--;
                        ttstub_output_flush(rust_stdout);
                    }

                    force_eof = false;
                    end_file_reading();
                    check_outer_validity();
                    goto restart;
                }

                if (INTPAR(end_line_char) < 0 || INTPAR(end_line_char) > 255)
                    cur_input.limit--;
                else
                    buffer[cur_input.limit] = INTPAR(end_line_char);

                first = cur_input.limit + 1;
                cur_input.loc = cur_input.start;
            } else {
                if (cur_input.name != 0) {
                    cur_cmd = 0;
                    cur_chr = 0;
                    return;
                }

                if (input_ptr > 0) {
                    end_file_reading();
                    goto restart;
                }

                /* Tectonic extension: we add a \TectonicCodaTokens toklist
                 * that gets inserted at the very very end of processing if no
                 * \end or \dump has been seen. We just use a global state
                 * variable to make sure it only gets inserted once. */

                if (!used_tectonic_coda_tokens && LOCAL(TectonicCodaTokens) != TEX_NULL) {
                    used_tectonic_coda_tokens = true;
                    begin_token_list(LOCAL(TectonicCodaTokens), TECTONIC_CODA_TEXT);
                    goto restart;
                }

                if (selector < SELECTOR_LOG_ONLY)
                    open_log_file();

                fatal_error("*** (job aborted, no legal \\end found)");
            }
            goto texswitch;
        }
    } else if (cur_input.loc != TEX_NULL) { /* if we're inputting from a non-null token list: */
        t = mem[cur_input.loc].b32.s0;
        cur_input.loc = LLIST_link(cur_input.loc);

        if (t >= CS_TOKEN_FLAG) {
            cur_cs = t - CS_TOKEN_FLAG;
            cur_cmd = eqtb[cur_cs].b16.s1;
            cur_chr = eqtb[cur_cs].b32.s1;

            if (cur_cmd >= OUTER_CALL) {
                if (cur_cmd == DONT_EXPAND) { /*370:*/
                    cur_cs = mem[cur_input.loc].b32.s0 - CS_TOKEN_FLAG;
                    cur_input.loc = TEX_NULL;
                    cur_cmd = eqtb[cur_cs].b16.s1;
                    cur_chr = eqtb[cur_cs].b32.s1;
                    if (cur_cmd > MAX_COMMAND) {
                        cur_cmd = RELAX;
                        cur_chr = NO_EXPAND_FLAG;
                    }
                } else {
                    check_outer_validity();
                }
            }
        } else {
            cur_cmd = t / MAX_CHAR_VAL;
            cur_chr = t % MAX_CHAR_VAL;

            switch (cur_cmd) {
            case LEFT_BRACE:
                align_state++;
                break;
            case RIGHT_BRACE:
                align_state--;
                break;
            case OUT_PARAM:
                begin_token_list(param_stack[cur_input.limit + cur_chr - 1], PARAMETER);
                goto restart;
                break;
            default:
                break;
            }
        }
    } else { /* token list but no tokens left */
        end_token_list();
        goto restart;
    }

    if (cur_cmd <= CAR_RET && cur_cmd >= TAB_MARK && align_state == 0) { /*818:*/
        if (scanner_status == ALIGNING || cur_align == TEX_NULL)
            fatal_error("(interwoven alignment preambles are not allowed)");

        cur_cmd = mem[cur_align + 5].b32.s0;
        mem[cur_align + 5].b32.s0 = cur_chr;
        if (cur_cmd == OMIT)
            begin_token_list(OMIT_TEMPLATE, V_TEMPLATE);
        else
            begin_token_list(mem[cur_align + 2].b32.s1, V_TEMPLATE);
        align_state = 1000000L;
        goto restart;
    }
}


void get_token(void)
{
    no_new_control_sequence = false;
    get_next();
    no_new_control_sequence = true;
    if (cur_cs == 0)
        cur_tok = (cur_cmd * MAX_CHAR_VAL) + cur_chr;
    else
        cur_tok = CS_TOKEN_FLAG + cur_cs;
}


void
macro_call(void)
{
    int32_t r;
    int32_t p = TEX_NULL;
    int32_t q;
    int32_t s;
    int32_t t;
    int32_t u, v;
    int32_t rbrace_ptr = TEX_NULL;
    small_number n;
    int32_t unbalance;
    int32_t m = 0;
    int32_t ref_count;
    small_number save_scanner_status;
    int32_t save_warning_index;
    UTF16_code match_chr;

    save_scanner_status = scanner_status;
    save_warning_index = warning_index;
    warning_index = cur_cs;
    ref_count = cur_chr;
    r = mem[ref_count].b32.s1;
    n = 0;

    if (INTPAR(tracing_macros) > 0) { /*419:*/
        begin_diagnostic();
        print_ln();

        diagnostic_begin_capture_warning_here();

        print_cs(warning_index);
        token_show(ref_count);

        capture_to_diagnostic(NULL);

        end_diagnostic(false);
    }

    if (mem[r].b32.s0 == PROTECTED_TOKEN)
        r = LLIST_link(r);

    if (mem[r].b32.s0 != END_MATCH_TOKEN) { /*409:*/
        scanner_status = MATCHING;
        unbalance = 0;
        long_state = eqtb[cur_cs].b16.s1;

        if (long_state >= OUTER_CALL)
            long_state = long_state - 2;

        do {
            mem[TEMP_HEAD].b32.s1 = TEX_NULL;
            if (mem[r].b32.s0 >= END_MATCH_TOKEN || mem[r].b32.s0 < MATCH_TOKEN) {
                s = TEX_NULL;
            } else {
                match_chr = mem[r].b32.s0 - MATCH_TOKEN;
                s = mem[r].b32.s1;
                r = s;
                p = TEMP_HEAD;
                m = 0;
            }

        continue_:
            get_token();

            if (cur_tok == mem[r].b32.s0) { /*412:*/
                r = LLIST_link(r);
                if (mem[r].b32.s0 >= MATCH_TOKEN && mem[r].b32.s0 <= END_MATCH_TOKEN) {
                    if (cur_tok < LEFT_BRACE_LIMIT)
                        align_state--;
                    goto found;
                } else {
                    goto continue_;
                }
            }

            if (s != r) {
                if (s == TEX_NULL) { /*416:*/
                    error_here_with_diagnostic("Use of ");
                    sprint_cs(warning_index);
                    print_cstr(" doesn't match its definition");
                    capture_to_diagnostic(NULL);
                    help_ptr = 4;
                    help_line[3] = "If you say, e.g., `\\def\\a1{...}', then you must always";
                    help_line[2] = "put `1' after `\\a', since control sequence names are";
                    help_line[1] = "made up of letters only. The macro here has not been";
                    help_line[0] = "followed by the required stuff, so I'm ignoring it.";
                    error();
                    goto exit;
                } else {
                    t = s;

                    do {
                        q = get_avail();
                        mem[p].b32.s1 = q;
                        mem[q].b32.s0 = mem[t].b32.s0;
                        p = q;

                        m++;
                        u = mem[t].b32.s1;
                        v = s;

                        while (true) {
                            if (u == r) {
                                if (cur_tok != mem[v].b32.s0) {
                                    goto done;
                                } else {
                                    r = mem[v].b32.s1;
                                    goto continue_;
                                }
                            }

                            if (mem[u].b32.s0 != mem[v].b32.s0)
                                goto done;

                            u = LLIST_link(u);
                            v = LLIST_link(v);
                        }

                    done:
                        t = LLIST_link(t);
                    } while (t != r);

                    r = s;
                }
            }

            if (cur_tok == par_token) {
                if (long_state != LONG_CALL) { /*414:*/
                    if (long_state == CALL) {
                        runaway();
                        error_here_with_diagnostic("Paragraph ended before ");
                        sprint_cs(warning_index);
                        print_cstr(" was complete");
                        capture_to_diagnostic(NULL);
                        help_ptr = 3;
                        help_line[2] = "I suspect you've forgotten a `}', causing me to apply this";
                        help_line[1] = "control sequence to too much text. How can we recover?";
                        help_line[0] = "My plan is to forget the whole thing and hope for the best.";
                        back_error();
                    }

                    pstack[n] = mem[TEMP_HEAD].b32.s1;
                    align_state = align_state - unbalance;

                    for (m = 0; m <= n; m++)
                        flush_list(pstack[m]);

                    goto exit;
                }
            }

            if (cur_tok < RIGHT_BRACE_LIMIT) {
                if (cur_tok < LEFT_BRACE_LIMIT) { /*417:*/
                    unbalance = 1;

                    while (true) {
                        q = avail;
                        if (q == TEX_NULL) {
                            q = get_avail();
                        } else {
                            avail = mem[q].b32.s1;
                            mem[q].b32.s1 = TEX_NULL;
                        }

                        mem[p].b32.s1 = q;
                        mem[q].b32.s0 = cur_tok;
                        p = q;

                        get_token();

                        if (cur_tok == par_token) {
                            if (long_state != LONG_CALL) { /*414:*/
                                if (long_state == CALL) {
                                    runaway();
                                    error_here_with_diagnostic("Paragraph ended before ");
                                    sprint_cs(warning_index);
                                    print_cstr(" was complete");
                                    capture_to_diagnostic(NULL);
                                    help_ptr = 3;
                                    help_line[2] = "I suspect you've forgotten a `}', causing me to apply this";
                                    help_line[1] = "control sequence to too much text. How can we recover?";
                                    help_line[0] = "My plan is to forget the whole thing and hope for the best.";
                                    back_error();
                                }

                                pstack[n] = mem[TEMP_HEAD].b32.s1;
                                align_state = align_state - unbalance;

                                for (m = 0; m <= n; m++)
                                    flush_list(pstack[m]);

                                goto exit;
                            }
                        }

                        if (cur_tok < RIGHT_BRACE_LIMIT) {
                            if (cur_tok < LEFT_BRACE_LIMIT) {
                                unbalance++;
                            } else {
                                unbalance--;
                                if (unbalance == 0)
                                    goto done1;
                            }
                        }

                    }

                done1:
                    rbrace_ptr = p;

                    q = get_avail();
                    mem[p].b32.s1 = q;
                    mem[q].b32.s0 = cur_tok;
                    p = q;
                } else { /*413:*/
                    back_input();

                    error_here_with_diagnostic("Argument of ");
                    sprint_cs(warning_index);
                    print_cstr(" has an extra }");
                    capture_to_diagnostic(NULL);

                    help_ptr = 6;
                    help_line[5] = "I've run across a `}' that doesn't seem to match anything.";
                    help_line[4] = "For example, `\\def\\a#1{...}' and `\\a}' would produce";
                    help_line[3] = "this error. If you simply proceed now, the `\\par' that";
                    help_line[2] = "I've just inserted will cause me to report a runaway";
                    help_line[1] = "argument that might be the root of the problem. But if";
                    help_line[0] = "your `}' was spurious, just type `2' and it will go away.";
                    align_state++;
                    long_state = CALL;
                    cur_tok = par_token;
                    ins_error();
                    goto continue_;
                }
            } else { /*411:*/
                if (cur_tok == SPACE_TOKEN) {
                    if (mem[r].b32.s0 <= END_MATCH_TOKEN) {
                        if (mem[r].b32.s0 >= MATCH_TOKEN)
                            goto continue_;
                    }
                }

                q = get_avail();
                mem[p].b32.s1 = q;
                mem[q].b32.s0 = cur_tok;
                p = q;
            }

            m++;

            if (mem[r].b32.s0 > END_MATCH_TOKEN)
                goto continue_;
            if (mem[r].b32.s0 < MATCH_TOKEN)
                goto continue_;

        found:
            if (s != TEX_NULL) { /*418:*/
                if (m == 1 && mem[p].b32.s0 < RIGHT_BRACE_LIMIT && p != TEMP_HEAD) {
                    mem[rbrace_ptr].b32.s1 = TEX_NULL;
                    mem[p].b32.s1 = avail;
                    avail = p;
                    p = mem[TEMP_HEAD].b32.s1;
                    pstack[n] = mem[p].b32.s1;
                    mem[p].b32.s1 = avail;
                    avail = p;
                } else {
                    pstack[n] = mem[TEMP_HEAD].b32.s1;
                }

                n++;

                if (INTPAR(tracing_macros) > 0) {
                    begin_diagnostic();
                    diagnostic_begin_capture_warning_here();
                    print_nl(match_chr);
                    print_int(n);
                    print_cstr("<-");
                    show_token_list(pstack[n - 1], TEX_NULL, 1000);
                    capture_to_diagnostic(NULL);
                    end_diagnostic(false);
                }
            }
        } while (mem[r].b32.s0 != END_MATCH_TOKEN);
    }

    while (cur_input.state == TOKEN_LIST && cur_input.loc == TEX_NULL && cur_input.index != V_TEMPLATE)
        end_token_list();

    begin_token_list(ref_count, MACRO);
    cur_input.name = warning_index;
    cur_input.loc = mem[r].b32.s1;

    if (n > 0) {
        if (param_ptr + n > max_param_stack) {
            max_param_stack = param_ptr + n;
            if (max_param_stack > param_size)
                overflow("parameter stack size", param_size);
        }

        for (m = 0; m <= n - 1; m++)
            param_stack[param_ptr + m] = pstack[m];

        param_ptr += n;
    }

exit:
    scanner_status = save_scanner_status;
    warning_index = save_warning_index;
}


void
insert_relax(void)
{
    cur_tok = CS_TOKEN_FLAG + cur_cs;
    back_input();
    cur_tok = CS_TOKEN_FLAG + FROZEN_RELAX;
    back_input();
    cur_input.index = INSERTED;
}


void new_index(uint16_t i, int32_t q)
{
    small_number k;
    cur_ptr = get_node(INDEX_NODE_SIZE);
    mem[cur_ptr].b16.s1 = i;
    mem[cur_ptr].b16.s0 = 0;
    mem[cur_ptr].b32.s1 = q;
    {
        register int32_t for_end;
        k = 1;
        for_end = (INDEX_NODE_SIZE - 1);
        if (k <= for_end)
            do
                mem[cur_ptr + k] = sa_null;
            while (k++ < for_end);
    }
}

void find_sa_element(small_number t, int32_t n, bool w)
{
    int32_t q;
    small_number i;
    cur_ptr = sa_root[t];
    {
        if (cur_ptr == TEX_NULL) {

            if (w)
                goto not_found;
            else
                return;
        }
    }
    q = cur_ptr;
    i = n / 0x40000;
    if (odd(i))
        cur_ptr = mem[q + (i / 2) + 1].b32.s1;
    else
        cur_ptr = mem[q + (i / 2) + 1].b32.s0;
    {
        if (cur_ptr == TEX_NULL) {

            if (w)
                goto lab46;
            else
                return;
        }
    }
    q = cur_ptr;
    i = (n / 4096) % 64;
    if (odd(i))
        cur_ptr = mem[q + (i / 2) + 1].b32.s1;
    else
        cur_ptr = mem[q + (i / 2) + 1].b32.s0;
    {
        if (cur_ptr == TEX_NULL) {

            if (w)
                goto lab47;
            else
                return;
        }
    }
    q = cur_ptr;
    i = (n / 64) % 64;
    if (odd(i))
        cur_ptr = mem[q + (i / 2) + 1].b32.s1;
    else
        cur_ptr = mem[q + (i / 2) + 1].b32.s0;
    {
        if (cur_ptr == TEX_NULL) {

            if (w)
                goto lab48;
            else
                return;
        }
    }
    q = cur_ptr;
    i = n % 64;
    if (odd(i))
        cur_ptr = mem[q + (i / 2) + 1].b32.s1;
    else
        cur_ptr = mem[q + (i / 2) + 1].b32.s0;
    if ((cur_ptr == TEX_NULL) && w)
        goto lab49;
    return;
 not_found:
    new_index(t, TEX_NULL);
    sa_root[t] = cur_ptr;
    q = cur_ptr;
    i = n / 0x40000;
 lab46:                        /*not_found1 */ new_index(i, q);
    {
        if (odd(i))
            mem[q + (i / 2) + 1].b32.s1 = cur_ptr;
        else
            mem[q + (i / 2) + 1].b32.s0 = cur_ptr;
        mem[q].b16.s0++;
    }
    q = cur_ptr;
    i = (n / 4096) % 64;
 lab47:                        /*not_found2 */ new_index(i, q);
    {
        if (odd(i))
            mem[q + (i / 2) + 1].b32.s1 = cur_ptr;
        else
            mem[q + (i / 2) + 1].b32.s0 = cur_ptr;
        mem[q].b16.s0++;
    }
    q = cur_ptr;
    i = (n / 64) % 64;
 lab48:                        /*not_found3 */ new_index(i, q);
    {
        if (odd(i))
            mem[q + (i / 2) + 1].b32.s1 = cur_ptr;
        else
            mem[q + (i / 2) + 1].b32.s0 = cur_ptr;
        mem[q].b16.s0++;
    }
    q = cur_ptr;
    i = n % 64;
 lab49:/*not_found4 *//*1608: */ if (t == MARK_VAL) {
        cur_ptr = get_node(MARK_CLASS_NODE_SIZE);
        mem[cur_ptr + 1] = sa_null;
        mem[cur_ptr + 2] = sa_null;
        mem[cur_ptr + 3] = sa_null;
    } else {

        if (t <= DIMEN_VAL) {
            cur_ptr = get_node(WORD_NODE_SIZE);
            mem[cur_ptr + 2].b32.s1 = 0;
            mem[cur_ptr + 1].b32.s1 = n;
        } else {

            cur_ptr = get_node(POINTER_NODE_SIZE);
            if (t <= MU_VAL) {
                mem[cur_ptr + 1].b32.s1 = 0;
                GLUE_SPEC_ref_count(0)++;
            } else
                mem[cur_ptr + 1].b32.s1 = TEX_NULL;
        }
        mem[cur_ptr + 1].b32.s0 = TEX_NULL;
    }
    mem[cur_ptr].b16.s1 = 64 * t + i;
    mem[cur_ptr].b16.s0 = 1 /*level_one *//*:1608 */ ;
    mem[cur_ptr].b32.s1 = q;
    {
        if (odd(i))
            mem[q + (i / 2) + 1].b32.s1 = cur_ptr;
        else
            mem[q + (i / 2) + 1].b32.s0 = cur_ptr;
        mem[q].b16.s0++;
    }
}


void
expand(void)
{
    int32_t t;
    bool b;
    int32_t p, q, r;
    int32_t j;
    int32_t cv_backup;
    small_number cvl_backup, radix_backup, co_backup;
    int32_t backup_backup;
    small_number save_scanner_status;

    expand_depth_count++;
    if (expand_depth_count >= expand_depth)
        overflow("expansion depth", expand_depth);

    cv_backup = cur_val;
    cvl_backup = cur_val_level;
    radix_backup = radix;
    co_backup = cur_order;
    backup_backup = mem[BACKUP_HEAD].b32.s1;

reswitch:
    if (cur_cmd < CALL) { /*384:*/
        if (INTPAR(tracing_commands) > 1)
            show_cur_cmd_chr();

        switch (cur_cmd) {
        case TOP_BOT_MARK:
            t = cur_chr % 5;

            if (cur_chr >= 5)
                scan_register_num();
            else
                cur_val = 0;

            if (cur_val == 0) {
                cur_ptr = cur_mark[t];
            } else { /*1612:*/
                find_sa_element(MARK_VAL, cur_val, false);
                if (cur_ptr != TEX_NULL) {
                    if (odd(t))
                        cur_ptr = mem[cur_ptr + (t / 2) + 1].b32.s1;
                    else
                        cur_ptr = mem[cur_ptr + (t / 2) + 1].b32.s0;
                }
            }

            if (cur_ptr != TEX_NULL)
                begin_token_list(cur_ptr, MARK_TEXT);
            break;

        case EXPAND_AFTER: /*385:*/
            if (cur_chr == 0) {
                get_token();
                t = cur_tok;
                get_token();
                if (cur_cmd > MAX_COMMAND)
                    expand();
                else
                    back_input();
                cur_tok = t;
                back_input();
            } else { /*1553: "\unless" implementation */
                get_token();

                if (cur_cmd == IF_TEST && cur_chr != IF_CASE_CODE) {
                    cur_chr = cur_chr + UNLESS_CODE;
                    goto reswitch;
                }

                error_here_with_diagnostic("You can't use `");
                print_esc_cstr("unless");
                print_cstr("' before `");
                print_cmd_chr(cur_cmd, cur_chr);
                print_char('\'');
                capture_to_diagnostic(NULL);
                help_ptr = 1;
                help_line[0] = "Continue, and I'll forget that it ever happened.";
                back_error();
            }
            break;

        case NO_EXPAND: /*386:*/
            if (cur_chr == 0) {
                save_scanner_status = scanner_status;
                scanner_status = NORMAL;
                get_token();
                scanner_status = save_scanner_status;
                t = cur_tok;
                back_input();
                if (t >= CS_TOKEN_FLAG) {
                    p = get_avail();
                    mem[p].b32.s0 = CS_TOKEN_FLAG + FROZEN_DONT_EXPAND;
                    mem[p].b32.s1 = cur_input.loc;
                    cur_input.start = p;
                    cur_input.loc = p;
                }
            } else { /*387: \primitive implementation */
                save_scanner_status = scanner_status;
                scanner_status = NORMAL;
                get_token();
                scanner_status = save_scanner_status;

                if (cur_cs < HASH_BASE)
                    cur_cs = prim_lookup(cur_cs - SINGLE_BASE);
                else
                    cur_cs = prim_lookup(hash[cur_cs].s1);

                if (cur_cs != UNDEFINED_PRIMITIVE) {
                    t = eqtb[PRIM_EQTB_BASE + cur_cs].b16.s1;
                    if (t > MAX_COMMAND) {
                        cur_cmd = t;
                        cur_chr = eqtb[PRIM_EQTB_BASE + cur_cs].b32.s1;
                        cur_tok = (cur_cmd * MAX_CHAR_VAL) + cur_chr;
                        cur_cs = 0;
                        goto reswitch;
                    } else {
                        back_input();
                        p = get_avail();
                        mem[p].b32.s0 = CS_TOKEN_FLAG + FROZEN_PRIMITIVE;
                        mem[p].b32.s1 = cur_input.loc;
                        cur_input.loc = p;
                        cur_input.start = p;
                    }
                }
            }
            break;

        case CS_NAME:
            r = get_avail();
            p = r;
            b = is_in_csname;
            is_in_csname = true;

            do {
                get_x_token();
                if (cur_cs == 0) {
                    q = get_avail();
                    mem[p].b32.s1 = q;
                    mem[q].b32.s0 = cur_tok;
                    p = q;
                }
            } while (cur_cs == 0);

            if (cur_cmd != END_CS_NAME) { /*391:*/
                error_here_with_diagnostic("Missing ");
                print_esc_cstr("endcsname");
                print_cstr(" inserted");
                capture_to_diagnostic(NULL);

                help_ptr = 2;
                help_line[1] = "The control sequence marked <to be read again> should";
                help_line[0] = "not appear between \\csname and \\endcsname.";
                back_error();
            }

            is_in_csname = b;
            j = first;
            p = mem[r].b32.s1;

            while (p != TEX_NULL) {
                if (j >= max_buf_stack) {
                    max_buf_stack = j + 1;
                    if (max_buf_stack == buf_size)
                        overflow("buffer size", buf_size);
                }
                buffer[j] = mem[p].b32.s0 % MAX_CHAR_VAL;
                j++;
                p = LLIST_link(p);
            }

            if (j > first + 1 || buffer[first] > 65535L) {
                no_new_control_sequence = false;
                cur_cs = id_lookup(first, j - first);
                no_new_control_sequence = true;
            } else if (j == first) {
                cur_cs = NULL_CS;
            } else {
                cur_cs = SINGLE_BASE + buffer[first]; /*:392*/
            }

            flush_list(r);

            if (eqtb[cur_cs].b16.s1 == UNDEFINED_CS)
                eq_define(cur_cs, RELAX, TOO_BIG_USV);

            cur_tok = cur_cs + CS_TOKEN_FLAG;
            back_input();
            break;

        case CONVERT:
            conv_toks();
            break;

        case THE:
            ins_the_toks();
            break;

        case IF_TEST:
            conditional();
            break;

        case FI_OR_ELSE:
            if (INTPAR(tracing_ifs) > 0) {
                if (INTPAR(tracing_commands) <= 1)
                    show_cur_cmd_chr();
            }

            if (cur_chr > if_limit) {
                if (if_limit == IF_CODE) {
                    insert_relax();
                } else {
                    error_here_with_diagnostic("Extra ");
                    print_cmd_chr(FI_OR_ELSE, cur_chr);
                    capture_to_diagnostic(NULL);

                    help_ptr = 1;
                    help_line[0] = "I'm ignoring this; it doesn't match any \\if.";
                    error();
                }
            } else {
                while (cur_chr != FI_CODE)
                    pass_text();

                if (if_stack[in_open] == cond_ptr)
                    if_warning();
                p = cond_ptr;
                if_line = mem[p + 1].b32.s1;
                cur_if = mem[p].b16.s0;
                if_limit = mem[p].b16.s1;
                cond_ptr = mem[p].b32.s1;
                free_node(p, IF_NODE_SIZE);
            }
            break;

        case INPUT:
            if (cur_chr == 1) /* \endinput */
                force_eof = true; /*1537:*/
            else if (cur_chr == 2) /* \scantokens */
                pseudo_start();
            else if (name_in_progress)
                insert_relax();
            else /* \input */
                start_input(NULL);
            break;

        default:
            error_here_with_diagnostic("Undefined control sequence");
            capture_to_diagnostic(NULL);

            help_ptr = 5;
            help_line[4] = "The control sequence at the end of the top line";
            help_line[3] = "of your error message was never \\def'ed. If you have";
            help_line[2] = "misspelled it (e.g., `\\hobx'), type `I' and the correct";
            help_line[1] = "spelling (e.g., `I\\hbox'). Otherwise just continue,";
            help_line[0] = "and I'll forget about whatever was undefined.";
            error();
            break;
        }
    } else if (cur_cmd < END_TEMPLATE) {
        macro_call();
    } else { /*393:*/
        cur_tok = CS_TOKEN_FLAG + FROZEN_ENDV;
        back_input();
    }

    cur_val = cv_backup;
    cur_val_level = cvl_backup;
    radix = radix_backup;
    cur_order = co_backup;
    mem[BACKUP_HEAD].b32.s1 = backup_backup;
    expand_depth_count--;
}


void get_x_token(void)
{
 restart:
    get_next();

    if (cur_cmd <= MAX_COMMAND)
        goto done;
    if (cur_cmd >= CALL) {

        if (cur_cmd < END_TEMPLATE)
            macro_call();
        else {

            cur_cs = FROZEN_ENDV;
            cur_cmd = ENDV;
            goto done;
        }
    } else
        expand();
    goto restart;
done:
    if (cur_cs == 0)
        cur_tok = (cur_cmd * MAX_CHAR_VAL) + cur_chr;
    else
        cur_tok = CS_TOKEN_FLAG + cur_cs;
}

void x_token(void)
{
    while (cur_cmd > MAX_COMMAND) {

        expand();
        get_next();
    }
    if (cur_cs == 0)
        cur_tok = (cur_cmd * MAX_CHAR_VAL) + cur_chr;
    else
        cur_tok = CS_TOKEN_FLAG + cur_cs;
}


void
scan_left_brace(void)
{
    do {
        get_x_token();
    } while (cur_cmd == SPACER || cur_cmd == RELAX);

    if (cur_cmd != LEFT_BRACE) {
        error_here_with_diagnostic("Missing { inserted");
        capture_to_diagnostic(NULL);

        help_ptr = 4;
        help_line[3] = "A left brace was mandatory here, so I've put one in.";
        help_line[2] = "You might want to delete and/or insert some corrections";
        help_line[1] = "so that I will find a matching right brace soon.";
        help_line[0] = "(If you're confused by all this, try typing `I}' now.)";
        back_error();
        cur_tok = (LEFT_BRACE_TOKEN + '{' );
        cur_cmd = LEFT_BRACE;
        cur_chr = '{' ;
        align_state++;
    }
}


void
scan_optional_equals(void)
{
    do {
        get_x_token();
    } while (cur_cmd == SPACER);

    if (cur_tok != OTHER_TOKEN + 61 /*"="*/)
        back_input();
}


bool scan_keyword(const char* s)
{
    int32_t p = BACKUP_HEAD;
    int32_t q;
    int32_t save_cur_cs;

    mem[p].b32.s1 = TEX_NULL;

    if (strlen(s) == 1) {
        char c = s[0];
        save_cur_cs = cur_cs;

        while (true) {
            get_x_token();
            if ((cur_cs == 0) && ((cur_chr == c) || (cur_chr == c - 32))) {
                {
                    q = get_avail();
                    mem[p].b32.s1 = q;
                    mem[q].b32.s0 = cur_tok;
                    p = q;
                }
                flush_list(mem[BACKUP_HEAD].b32.s1);
                return true;
            } else if ((cur_cmd != SPACER) || (p != BACKUP_HEAD)) {
                back_input();
                if (p != BACKUP_HEAD)
                    begin_token_list(mem[BACKUP_HEAD].b32.s1, BACKED_UP);
                cur_cs = save_cur_cs;
                return false;
            }
        }
    }

    size_t slen = strlen(s);
    size_t i = 0;
    while (i < slen) {

        get_x_token();
        if ((cur_cs == 0) && ((cur_chr == s[i]) || (cur_chr == s[i] - 32))) {
            {
                q = get_avail();
                mem[p].b32.s1 = q;
                mem[q].b32.s0 = cur_tok;
                p = q;
            }
            i++;
        } else if ((cur_cmd != SPACER) || (p != BACKUP_HEAD)) {
            back_input();
            if (p != BACKUP_HEAD)
                begin_token_list(mem[BACKUP_HEAD].b32.s1, BACKED_UP);
            return false;
        }
    }
    flush_list(mem[BACKUP_HEAD].b32.s1);
    return true;
}

void mu_error(void)
{
    error_here_with_diagnostic("Incompatible glue units");
    capture_to_diagnostic(NULL);

    help_ptr = 1;
    help_line[0] = "I'm going to assume that 1mu=1pt when they're mixed.";
    error();
}

void scan_glyph_number(internal_font_number f)
{
    if (scan_keyword("/")) {
        scan_and_pack_name();
        {
            cur_val = map_glyph_to_index(f);
            cur_val_level = INT_VAL;
        }
    } else if (scan_keyword("u")) {
        scan_char_num();
        {
            cur_val = map_char_to_glyph(f, cur_val);
            cur_val_level = INT_VAL;
        }
    } else
        scan_int();
}

void scan_char_class(void)
{
    scan_int();
    if ((cur_val < 0) || (cur_val > CHAR_CLASS_LIMIT)) {
        ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Bad character class");
        ttstub_diag_printf(errmsg, " (%d)", cur_val);
        capture_to_diagnostic(NULL);

        {
            help_ptr = 2;
            help_line[1] = "A character class must be between 0 and 4096.";
            help_line[0] = "I changed this one to zero.";
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_char_class_not_ignored(void)
{
    scan_int();
    if ((cur_val < 0) || (cur_val > CHAR_CLASS_LIMIT)) {
        ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Bad character class");
        ttstub_diag_printf(errmsg, " (%d)", cur_val);
        capture_to_diagnostic(NULL);

        {
            help_ptr = 2;
            help_line[1] = "A class for inter-character transitions must be between 0 and 4095.";
            help_line[0] = "I changed this one to zero.";
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_eight_bit_int(void)
{
    scan_int();
    if ((cur_val < 0) || (cur_val > 255)) {
        ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Bad register code");
        ttstub_diag_printf(errmsg, " (%d)", cur_val);
        capture_to_diagnostic(NULL);

        {
            help_ptr = 2;
            help_line[1] = "A register code or char class must be between 0 and 255.";
            help_line[0] = "I changed this one to zero.";
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_usv_num(void)
{
    scan_int();
    if ((cur_val < 0) || (cur_val > BIGGEST_USV)) {
        ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Bad character code");
        ttstub_diag_printf(errmsg, " (%d)", cur_val);
        capture_to_diagnostic(NULL);

        {
            help_ptr = 2;
            help_line[1] = "A Unicode scalar value must be between 0 and \"10FFFF.";
            help_line[0] = "I changed this one to zero.";
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_char_num(void)
{
    scan_int();
    if ((cur_val < 0) || (cur_val > BIGGEST_CHAR)) {
        ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Bad character code");
        ttstub_diag_printf(errmsg, " (%d)", cur_val);
        capture_to_diagnostic(NULL);

        {
            help_ptr = 2;
            help_line[1] = "A character number must be between 0 and 65535.";
            help_line[0] = "I changed this one to zero.";
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_xetex_math_char_int(void)
{
    scan_int();
    if (math_char(cur_val) == ACTIVE_MATH_CHAR) {
        if (cur_val != ACTIVE_MATH_CHAR) {
            ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Bad active XeTeX math code");
            ttstub_diag_printf(errmsg, " (%d)", cur_val);
            capture_to_diagnostic(NULL);

            {
                help_ptr = 2;
                help_line[1] = "Since I ignore class and family for active math chars,";
                help_line[0] = "I changed this one to \"1FFFFF.";
            }
            int_error(cur_val);
            cur_val = ACTIVE_MATH_CHAR;
        }
    } else if (math_char(cur_val) > BIGGEST_USV) {
        ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Bad XeTeX math character code");
        ttstub_diag_printf(errmsg, " (%d)", cur_val);
        capture_to_diagnostic(NULL);

        {
            help_ptr = 2;
            help_line[1] = "Since I expected a character number between 0 and \"10FFFF,";
            help_line[0] = "I changed this one to zero.";
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_math(int32_t p)
{
    int32_t c;

restart: /*422:*/
    do {
        get_x_token();
    } while (cur_cmd == SPACER || cur_cmd == RELAX);
reswitch:
    switch (cur_cmd) {
    case 11:
    case 12:
    case 68:
        {
            c = MATH_CODE(cur_chr);
            if (math_char(c) == ACTIVE_MATH_CHAR) {
                {
                    cur_cs = cur_chr + 1;
                    cur_cmd = eqtb[cur_cs].b16.s1;
                    cur_chr = eqtb[cur_cs].b32.s1;
                    x_token();
                    back_input();
                }
                goto restart;
            }
        }
        break;
    case 16:
        {
            scan_char_num();
            cur_chr = cur_val;
            cur_cmd = CHAR_GIVEN;
            goto reswitch;
        }
        break;
    case 17:
        if (cur_chr == 2) {
            scan_math_class_int();
            c = set_class(cur_val);
            scan_math_fam_int();
            c = c + set_family(cur_val);
            scan_usv_num();
            c = c + cur_val;
        } else if (cur_chr == 1) {
            scan_xetex_math_char_int();
            c = cur_val;
        } else {

            scan_fifteen_bit_int();
            c = set_class(cur_val / 4096) + set_family((cur_val % 4096) / 256) + (cur_val % 256);
        }
        break;
    case 69:
        {
            c = set_class(cur_chr / 4096) + set_family((cur_chr % 4096) / 256) + (cur_chr % 256);
        }
        break;
    case 70:
        c = cur_chr;
        break;
    case 15:
        {
            if (cur_chr == 1) {
                scan_math_class_int();
                c = set_class(cur_val);
                scan_math_fam_int();
                c = c + set_family(cur_val);
                scan_usv_num();
                c = c + cur_val;
            } else {

                scan_delimiter_int();
                c = cur_val / 4096;
                c = set_class(c / 4096) + set_family((c % 4096) / 256) + (c % 256);
            }
        }
        break;
    default:
        {
            back_input();
            scan_left_brace();
            save_stack[save_ptr + 0].b32.s1 = p;
            save_ptr++;
            push_math(MATH_GROUP);
            return;
        }
        break;
    }
    mem[p].b32.s1 = MATH_CHAR;
    mem[p].b16.s0 = c % 65536L;
    if ((math_class(c) == 7)
        && ((INTPAR(cur_fam) >= 0)
            && (INTPAR(cur_fam) < NUMBER_MATH_FAMILIES)))
        mem[p].b16.s1 = INTPAR(cur_fam);
    else
        mem[p].b16.s1 = (math_fam(c));
    mem[p].b16.s1 = mem[p].b16.s1 + (math_char(c) / 65536L) * 256;
}

void set_math_char(int32_t c)
{
    int32_t p;
    UnicodeScalar ch;

    if (math_char(c) == ACTIVE_MATH_CHAR) {        /*1187: */
        cur_cs = cur_chr + 1;
        cur_cmd = eqtb[cur_cs].b16.s1;
        cur_chr = eqtb[cur_cs].b32.s1;
        x_token();
        back_input();
    } else {

        p = new_noad();
        mem[p + 1].b32.s1 = MATH_CHAR;
        ch = math_char(c);
        mem[p + 1].b16.s0 = ch % 65536L;
        mem[p + 1].b16.s1 = math_fam(c);
        if (math_class(c) == 7) {
            if (((INTPAR(cur_fam) >= 0)
                 && (INTPAR(cur_fam) < NUMBER_MATH_FAMILIES)))
                mem[p + 1].b16.s1 = INTPAR(cur_fam);
            mem[p].b16.s1 = ORD_NOAD;
        } else
            mem[p].b16.s1 = ORD_NOAD + math_class(c);
        mem[p + 1].b16.s1 = mem[p + 1].b16.s1 + (ch / 65536L) * 256;
        mem[cur_list.tail].b32.s1 = p;
        cur_list.tail = p;
    }
}

void scan_math_class_int(void)
{
    scan_int();
    if ((cur_val < 0) || (cur_val > 7)) {
        ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Bad math class");
        ttstub_diag_printf(errmsg, " (%d)", cur_val);
        capture_to_diagnostic(NULL);

        {
            help_ptr = 2;
            help_line[1] = "Since I expected to read a number between 0 and 7,";
            help_line[0] = "I changed this one to zero.";
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_math_fam_int(void)
{
    scan_int();
    if ((cur_val < 0) || (cur_val > (NUMBER_MATH_FAMILIES - 1))) {
        ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Bad math family");
        ttstub_diag_printf(errmsg, " (%d)", cur_val);
        capture_to_diagnostic(NULL);

        {
            help_ptr = 2;
            help_line[1] = "Since I expected to read a number between 0 and 255,";
            help_line[0] = "I changed this one to zero.";
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_four_bit_int(void)
{
    scan_int();
    if ((cur_val < 0) || (cur_val > 15)) {
        ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Bad number");
        ttstub_diag_printf(errmsg, " (%d)", cur_val);
        capture_to_diagnostic(NULL);

        {
            help_ptr = 2;
            help_line[1] = "Since I expected to read a number between 0 and 15,"; /* ... "between 0 and 15" */
            help_line[0] = "I changed this one to zero.";
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_fifteen_bit_int(void)
{
    scan_int();
    if ((cur_val < 0) || (cur_val > 32767)) {
        ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Bad mathchar");
        ttstub_diag_printf(errmsg, " (%d)", cur_val);
        capture_to_diagnostic(NULL);

        {
            help_ptr = 2;
            help_line[1] = "A mathchar number must be between 0 and 32767.";
            help_line[0] = "I changed this one to zero.";
        }
        int_error(cur_val);
        cur_val = 0;
    }
}


void
scan_delimiter_int(void)
{
    scan_int();

    if (cur_val < 0 || cur_val > 0x7FFFFFF) {
        ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Bad delimiter code");
        ttstub_diag_printf(errmsg, " (%d)", cur_val);
        capture_to_diagnostic(NULL);

        help_ptr = 2;
        help_line[1] = "A numeric delimiter code must be between 0 and 2^{27}-1.";
        help_line[0] = "I changed this one to zero.";
        int_error(cur_val);
        cur_val = 0;
    }
}


void scan_register_num(void)
{
    scan_int();
    if ((cur_val < 0) || (cur_val > max_reg_num)) {
        ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Bad register code");
        ttstub_diag_printf(errmsg, " (%d)", cur_val);
        capture_to_diagnostic(NULL);

        {
            help_ptr = 2;
            help_line[1] = max_reg_help_line;
            help_line[0] = "I changed this one to zero.";
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void scan_four_bit_int_or_18(void)
{
    scan_int();
    if ((cur_val < 0) || ((cur_val > 15) && (cur_val != 18))) {
        ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Bad number");
        ttstub_diag_printf(errmsg, " (%d)", cur_val);
        capture_to_diagnostic(NULL);

        {
            help_ptr = 2;
            help_line[1] = "Since I expected to read a number between 0 and 15,"; /* ... "between 0 and 15" */
            help_line[0] = "I changed this one to zero.";
        }
        int_error(cur_val);
        cur_val = 0;
    }
}

void get_x_or_protected(void)
{
    while (true) {

        get_token();
        if (cur_cmd <= MAX_COMMAND)
            return;
        if ((cur_cmd >= CALL) && (cur_cmd < END_TEMPLATE)) {

            if (mem[mem[cur_chr].b32.s1].b32.s0 == PROTECTED_TOKEN)
                return;
        }
        expand();
    }
}


int32_t
effective_char(bool err_p, internal_font_number f, uint16_t c)
{
    if (!xtx_ligature_present && font_mapping[f] != NULL)
        c = apply_tfm_font_mapping(font_mapping[f], c);

    xtx_ligature_present = false;
    return c;
}


void scan_font_ident(void)
{
    internal_font_number f;
    int32_t m;

    do {
        get_x_token();
    } while (cur_cmd == SPACER);

    if (cur_cmd == DEF_FONT)
        f = eqtb[CUR_FONT_LOC].b32.s1;
    else if (cur_cmd == SET_FONT)
        f = cur_chr;
    else if (cur_cmd == DEF_FAMILY) {
        m = cur_chr;
        scan_math_fam_int();
        f = eqtb[m + cur_val].b32.s1;
    } else {
        error_here_with_diagnostic("Missing font identifier");
        capture_to_diagnostic(NULL);
        {
            help_ptr = 2;
            help_line[1] = "I was looking for a control sequence whose";
            help_line[0] = "current meaning has been defined by \\font.";
        }
        back_error();
        f = FONT_BASE;
    }
    cur_val = f;
}

void find_font_dimen(bool writing)
{
    internal_font_number f;
    int32_t n;
    scan_int();
    n = cur_val;
    scan_font_ident();
    f = cur_val;
    if (n <= 0)
        cur_val = fmem_ptr;
    else {

        if (writing && (n <= SPACE_SHRINK_CODE) && (n >= SPACE_CODE) && (font_glue[f] != TEX_NULL)) {
            delete_glue_ref(font_glue[f]);
            font_glue[f] = TEX_NULL;
        }
        if (n > font_params[f]) {

            if (f < font_ptr)
                cur_val = fmem_ptr;
            else {              /*599: */

                do {
                    if (fmem_ptr == font_mem_size)
                        overflow("font memory", font_mem_size);
                    font_info[fmem_ptr].b32.s1 = 0;
                    fmem_ptr++;
                    font_params[f]++;
                } while (!(n == font_params[f]));
                cur_val = fmem_ptr - 1;
            }
        } else
            cur_val = n + param_base[f];
    }
    if (cur_val == fmem_ptr) {
        error_here_with_diagnostic("Font ");
        print_esc(hash[FONT_ID_BASE + f].s1);
        print_cstr(" has only ");
        print_int(font_params[f]);
        print_cstr(" fontdimen parameters");
        capture_to_diagnostic(NULL);
        {
            help_ptr = 2;
            help_line[1] = "To increase the number of font parameters, you must";
            help_line[0] = "use \\fontdimen immediately after the \\font is loaded.";
        }
        error();
    }
}


void
scan_something_internal(small_number level, bool negative)
{
    int32_t m;
    int32_t n, k, kk;
    int32_t q, r;
    int32_t tx;
    b16x4 i;
    int32_t p;

restart:
    m = cur_chr;

    switch (cur_cmd) {
    case DEF_CODE:
        scan_usv_num();
        if (m == MATH_CODE_BASE) {
            cur_val1 = MATH_CODE(cur_val);
            if (math_char(cur_val1) == ACTIVE_MATH_CHAR) {
                cur_val1 = 0x8000;
            } else if (math_class(cur_val1) > 7 || math_fam(cur_val1) > 15 || math_char(cur_val1) > 255) {
                ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Extended mathchar used as mathchar");
                ttstub_diag_printf(errmsg, " (%d)", cur_val1);
                capture_to_diagnostic(NULL);

                help_ptr = 2;
                help_line[1] = "A mathchar number must be between 0 and \"7FFF.";
                help_line[0] = "I changed this one to zero.";
                int_error(cur_val1);
                cur_val1 = 0;
            }

            cur_val1 = math_class(cur_val1) * 0x1000 + math_fam(cur_val1) * 0x100 + math_char(cur_val1);
            cur_val = cur_val1;
            cur_val_level = INT_VAL;
        } else if (m == DEL_CODE_BASE) {
            cur_val1 = DEL_CODE(cur_val);
            if (cur_val1 >= 0x40000000) {
                error_here_with_diagnostic("Extended delcode used as delcode");
                capture_to_diagnostic(NULL);

                help_ptr = 2;
                help_line[1] = "I can only go up to 2147483647='17777777777=\"7FFFFFFF,";
                help_line[0] = "I changed this one to zero.";
                error();
                cur_val = 0;
                cur_val_level = INT_VAL;
            } else {
                cur_val = cur_val1;
                cur_val_level = INT_VAL;
            }
        } else if (m < SF_CODE_BASE) {
            cur_val = eqtb[m + cur_val].b32.s1;
            cur_val_level = INT_VAL;
        } else if (m < MATH_CODE_BASE) {
            cur_val = eqtb[m + cur_val].b32.s1 % 65536L;
            cur_val_level = INT_VAL;
        } else {
            cur_val = eqtb[m + cur_val].b32.s1;
            cur_val_level = INT_VAL;
        }
        break;

    case XETEX_DEF_CODE:
        scan_usv_num();

        if (m == SF_CODE_BASE) {
            cur_val = SF_CODE(cur_val) / 65536L;
            cur_val_level = INT_VAL;
        } else if (m == MATH_CODE_BASE) {
            cur_val = MATH_CODE(cur_val);
            cur_val_level = INT_VAL;
        } else if (m == MATH_CODE_BASE + 1) {
            error_here_with_diagnostic("Can't use \\Umathcode as a number (try \\Umathcodenum)");
            capture_to_diagnostic(NULL);

            help_ptr = 2;
            help_line[1] = "\\Umathcode is for setting a mathcode from separate values;";
            help_line[0] = "use \\Umathcodenum to access them as single values.";
            error();
            cur_val = 0;
            cur_val_level = INT_VAL;
        } else if (m == DEL_CODE_BASE) {
            cur_val = DEL_CODE(cur_val);
            cur_val_level = INT_VAL;
        } else {
            error_here_with_diagnostic("Can't use \\Udelcode as a number (try \\Udelcodenum)");
            capture_to_diagnostic(NULL);

            help_ptr = 2;
            help_line[1] = "\\Udelcode is for setting a delcode from separate values;";
            help_line[0] = "use \\Udelcodenum to access them as single values.";
            error();
            cur_val = 0;
            cur_val_level = INT_VAL;
        }
        break;

    case TOKS_REGISTER:
    case ASSIGN_TOKS:
    case DEF_FAMILY:
    case SET_FONT:
    case DEF_FONT:
        if (level != TOK_VAL) {
            error_here_with_diagnostic("Missing number, treated as zero");
            capture_to_diagnostic(NULL);

            help_ptr = 3;
            help_line[2] = "A number should have been here; I inserted `0'.";
            help_line[1] = "(If you can't figure out why I needed to see a number,";
            help_line[0] = "look up `weird error' in the index to The TeXbook.)";
            back_error();
            cur_val = 0;
            cur_val_level = DIMEN_VAL;
        } else if (cur_cmd <= ASSIGN_TOKS) {
            if (cur_cmd < ASSIGN_TOKS) {
                if (m == 0) {
                    scan_register_num();
                    if (cur_val < 256) {
                        cur_val = TOKS_REG(cur_val);
                    } else {
                        find_sa_element(TOK_VAL, cur_val, false);
                        if (cur_ptr == TEX_NULL)
                            cur_val = TEX_NULL;
                        else
                            cur_val = mem[cur_ptr + 1].b32.s1;
                    }
                } else {
                    cur_val = mem[m + 1].b32.s1;
                }
            } else if (cur_chr == LOCAL_BASE + LOCAL__xetex_inter_char) {
                scan_char_class_not_ignored();
                cur_ptr = cur_val;
                scan_char_class_not_ignored();
                find_sa_element(INTER_CHAR_VAL, cur_ptr * CHAR_CLASS_LIMIT + cur_val, false);
                if (cur_ptr == TEX_NULL)
                    cur_val = TEX_NULL;
                else
                    cur_val = mem[cur_ptr + 1].b32.s1;
            } else {
                cur_val = eqtb[m].b32.s1;
            }
            cur_val_level = TOK_VAL;
        } else {
            back_input();
            scan_font_ident();
            cur_val = FONT_ID_BASE + cur_val;
            cur_val_level = IDENT_VAL;
        }
        break;

    case ASSIGN_INT:
        cur_val = eqtb[m].b32.s1;
        cur_val_level = INT_VAL;
        break;

    case ASSIGN_DIMEN:
        cur_val = eqtb[m].b32.s1;
        cur_val_level = DIMEN_VAL;
        break;

    case ASSIGN_GLUE:
        cur_val = eqtb[m].b32.s1;
        cur_val_level = GLUE_VAL;
        break;

    case ASSIGN_MU_GLUE:
        cur_val = eqtb[m].b32.s1;
        cur_val_level = MU_VAL;
        break;

    case SET_AUX:
        if (abs(cur_list.mode) != m) {
            error_here_with_diagnostic("Improper ");
            print_cmd_chr(SET_AUX, m);
            capture_to_diagnostic(NULL);

            help_ptr = 4;
            help_line[3] = "You can refer to \\spacefactor only in horizontal mode;";
            help_line[2] = "you can refer to \\prevdepth only in vertical mode; and";
            help_line[1] = "neither of these is meaningful inside \\write. So";
            help_line[0] = "I'm forgetting what you said and using zero instead.";
            error();

            if (level != TOK_VAL) {
                cur_val = 0;
                cur_val_level = DIMEN_VAL;
            } else {
                cur_val = 0;
                cur_val_level = INT_VAL;
            }
        } else if (m == VMODE) {
            cur_val = cur_list.aux.b32.s1;
            cur_val_level = DIMEN_VAL;
        } else {
            cur_val = cur_list.aux.b32.s0;
            cur_val_level = INT_VAL;
        }
        break;

    case SET_PREV_GRAF:
        if (cur_list.mode == 0) {
            cur_val = 0;
            cur_val_level = INT_VAL;
        } else {
            nest[nest_ptr] = cur_list;
            p = nest_ptr;
            while (abs(nest[p].mode) != VMODE)
                p--;

            cur_val = nest[p].prev_graf;
            cur_val_level = INT_VAL;
        }
        break;

    case SET_PAGE_INT:
        if (m == 0)
            cur_val = dead_cycles;
        else if (m == 2)
            cur_val = interaction;
        else
            cur_val = insert_penalties;
        cur_val_level = INT_VAL;
        break;

    case SET_PAGE_DIMEN:
        if (page_contents == EMPTY && !output_active) {
            if (m == 0)
                cur_val = MAX_HALFWORD;
            else
                cur_val = 0;
        } else {
            cur_val = page_so_far[m];
        }

        cur_val_level = DIMEN_VAL;
        break;

    case SET_SHAPE:
        if (m > LOCAL_BASE + LOCAL__par_shape) { /*1654:*/
            scan_int();
            if (eqtb[m].b32.s1 == TEX_NULL || cur_val < 0) {
                cur_val = 0;
            } else {
                if (cur_val > mem[eqtb[m].b32.s1 + 1].b32.s1)
                    cur_val = mem[eqtb[m].b32.s1 + 1].b32.s1;
                cur_val = mem[eqtb[m].b32.s1 + cur_val + 1].b32.s1;
            }
        } else if (LOCAL(par_shape) == TEX_NULL) {
            cur_val = 0;
        } else {
            cur_val = mem[LOCAL(par_shape)].b32.s0;
        }

        cur_val_level = INT_VAL;
        break;

    case SET_BOX_DIMEN:
        scan_register_num();

        if (cur_val < 256) {
            q = BOX_REG(cur_val);
        } else {
            find_sa_element(4, cur_val, false);
            if (cur_ptr == TEX_NULL)
                q = TEX_NULL;
            else
                q = mem[cur_ptr + 1].b32.s1;
        }

        if (q == TEX_NULL)
            cur_val = 0;
        else
            cur_val = mem[q + m].b32.s1;
        cur_val_level = DIMEN_VAL;
        break;

    case CHAR_GIVEN:
    case MATH_GIVEN:
    case XETEX_MATH_GIVEN:
        cur_val = cur_chr;
        cur_val_level = INT_VAL;
        break;

    case ASSIGN_FONT_DIMEN:
        find_font_dimen(false);
        font_info[fmem_ptr].b32.s1 = 0;
        cur_val = font_info[cur_val].b32.s1;
        cur_val_level = DIMEN_VAL;
        break;

    case ASSIGN_FONT_INT:
        scan_font_ident();
        if (m == 0) {
            cur_val = hyphen_char[cur_val];
            cur_val_level = INT_VAL;
        } else if (m == 1) {
            cur_val = skew_char[cur_val];
            cur_val_level = INT_VAL;
        } else {
            n = cur_val;

            if (font_area[n] == AAT_FONT_FLAG || font_area[n] == OTGR_FONT_FLAG)
                scan_glyph_number(n);
            else
                scan_char_num();

            k = cur_val;
            switch (m) {
            case LP_CODE_BASE:
                cur_val = get_cp_code(n, k, LEFT_SIDE);
                cur_val_level = INT_VAL;
                break;
            case RP_CODE_BASE:
                cur_val = get_cp_code(n, k, RIGHT_SIDE);
                cur_val_level = INT_VAL;
                break;
            }
        }
        break;

    case REGISTER:
        if (m < 0 || m > 19) { /* 19 = "lo_mem_stat_max" */
            cur_val_level = (mem[m].b16.s1 / 64);
            if (cur_val_level < GLUE_VAL)
                cur_val = mem[m + 2].b32.s1;
            else
                cur_val = mem[m + 1].b32.s1;
        } else {
            scan_register_num();
            cur_val_level = m;
            if (cur_val > 255) {
                find_sa_element(cur_val_level, cur_val, false);
                if (cur_ptr == TEX_NULL)
                    cur_val = 0;
                else if (cur_val_level < GLUE_VAL)
                    cur_val = mem[cur_ptr + 2].b32.s1;
                else
                    cur_val = mem[cur_ptr + 1].b32.s1;
            } else {
                switch (cur_val_level) {
                case INT_VAL:
                    cur_val = COUNT_REG(cur_val);
                    break;
                case DIMEN_VAL:
                    cur_val = SCALED_REG(cur_val);
                    break;
                case GLUE_VAL:
                    cur_val = SKIP_REG(cur_val);
                    break;
                case MU_VAL:
                    cur_val = MU_SKIP_REG(cur_val);
                    break;
                }
            }
        }
        break;

    case LAST_ITEM:
        if (m >= INPUT_LINE_NO_CODE) {
            if (m >= ETEX_GLUE) { /*1568:*/
                if (m < ETEX_MU) {
                    switch (m) { /*1595:*/
                    case MU_TO_GLUE_CODE:
                        scan_mu_glue();
                        break;
                    }
                    cur_val_level = GLUE_VAL;
                } else if (m < ETEX_EXPR) {
                    switch (m) { /*1596:*/
                    case GLUE_TO_MU_CODE:
                        scan_normal_glue();
                        break;
                    }
                    cur_val_level = MU_VAL;
                } else {
                    cur_val_level = m - ETEX_EXPR;
                    scan_expr();
                }

                while (cur_val_level > level) {
                    if (cur_val_level == GLUE_VAL) {
                        m = cur_val;
                        cur_val = mem[m + 1].b32.s1;
                        delete_glue_ref(m);
                    } else if (cur_val_level == MU_VAL) {
                        mu_error();
                    }
                    cur_val_level--;
                }

                if (negative) {
                    if (cur_val_level >= GLUE_VAL) {
                        m = cur_val;
                        cur_val = new_spec(m);
                        delete_glue_ref(m);
                        mem[cur_val + 1].b32.s1 = -(int32_t) mem[cur_val + 1].b32.s1;
                        mem[cur_val + 2].b32.s1 = -(int32_t) mem[cur_val + 2].b32.s1;
                        mem[cur_val + 3].b32.s1 = -(int32_t) mem[cur_val + 3].b32.s1;
                    } else {
                        cur_val = -(int32_t) cur_val;
                    }
                }
                return;
            }

            if (m >= XETEX_DIM) {
                switch (m) { /*1435:*/
                case XETEX_GLYPH_BOUNDS_CODE:
                    if (font_area[eqtb[CUR_FONT_LOC].b32.s1] == AAT_FONT_FLAG ||
                        font_area[eqtb[CUR_FONT_LOC].b32.s1] == OTGR_FONT_FLAG) {
                        scan_int();
                        n = cur_val;
                        if (n < 1 || n > 4) {
                            error_here_with_diagnostic("\\\\XeTeXglyphbounds requires an edge index from 1 to 4;");
                            print_nl_cstr("I don't know anything about edge ");
                            print_int(n);
                            capture_to_diagnostic(NULL);

                            error();
                            cur_val = 0;
                        } else {
                            scan_int();
                            cur_val = get_glyph_bounds(eqtb[CUR_FONT_LOC].b32.s1, n, cur_val);
                        }
                    } else {
                        not_native_font_error(LAST_ITEM, m, eqtb[CUR_FONT_LOC].b32.s1);
                        cur_val = 0;
                    }
                    break;

                case FONT_CHAR_WD_CODE:
                case FONT_CHAR_HT_CODE:
                case FONT_CHAR_DP_CODE:
                case FONT_CHAR_IC_CODE:
                    scan_font_ident();
                    q = cur_val;
                    scan_usv_num();
                    if (font_area[q] == AAT_FONT_FLAG || font_area[q] == OTGR_FONT_FLAG) {
                        switch (m) {
                        case FONT_CHAR_WD_CODE:
                            cur_val = getnativecharwd(q, cur_val);
                            break;
                        case FONT_CHAR_HT_CODE:
                            cur_val = getnativecharht(q, cur_val);
                            break;
                        case FONT_CHAR_DP_CODE:
                            cur_val = getnativechardp(q, cur_val);
                            break;
                        case FONT_CHAR_IC_CODE:
                            cur_val = getnativecharic(q, cur_val);
                            break;
                        }
                    } else {
                        if (font_bc[q] <= cur_val && font_ec[q] >= cur_val) {
                            i = FONT_CHARACTER_INFO(q, effective_char(true, q, cur_val));

                            switch (m) {
                            case FONT_CHAR_WD_CODE:
                                cur_val = FONT_CHARINFO_WIDTH(q, i);
                                break;
                            case FONT_CHAR_HT_CODE:
                                cur_val = FONT_CHARINFO_HEIGHT(q, i);
                                break;
                            case FONT_CHAR_DP_CODE:
                                cur_val = FONT_CHARINFO_DEPTH(q, i);
                                break;
                            case FONT_CHAR_IC_CODE:
                                cur_val = FONT_CHARINFO_ITALCORR(q, i);
                                break;
                            }
                        } else {
                            cur_val = 0;
                        }
                    }
                    break;

                case PAR_SHAPE_LENGTH_CODE:
                case PAR_SHAPE_INDENT_CODE:
                case PAR_SHAPE_DIMEN_CODE:
                    q = cur_chr - PAR_SHAPE_LENGTH_CODE;
                    scan_int();
                    if (LOCAL(par_shape) == TEX_NULL || cur_val <= 0) {
                        cur_val = 0;
                    } else {
                        if (q == 2) {
                            q = cur_val % 2;
                            cur_val = (cur_val + q) / 2;
                        }
                        if (cur_val > mem[LOCAL(par_shape)].b32.s0)
                            cur_val = mem[LOCAL(par_shape)].b32.s0;
                        cur_val = mem[LOCAL(par_shape) + 2 * cur_val - q].b32.s1;
                    }
                    cur_val_level = DIMEN_VAL;
                    break;

                case GLUE_STRETCH_CODE:
                case GLUE_SHRINK_CODE:
                    scan_normal_glue();
                    q = cur_val;
                    if (m == GLUE_STRETCH_CODE)
                        cur_val = mem[q + 2].b32.s1;
                    else
                        cur_val = mem[q + 3].b32.s1;
                    delete_glue_ref(q);
                    break;
                }
                cur_val_level = DIMEN_VAL;
            } else { /* if(m >= XETEX_DIM) */
                switch (m) {
                case INPUT_LINE_NO_CODE:
                    cur_val = line;
                    break;

                case BADNESS_CODE:
                    cur_val = last_badness;
                    break;

                case ELAPSED_TIME_CODE:
                    cur_val = get_microinterval();
                    break;

                case RANDOM_SEED_CODE:
                    cur_val = random_seed;
                    break;

                case PDF_SHELL_ESCAPE_CODE:
                    cur_val = 0; /* shellenabledp */
                    break;

                case ETEX_VERSION_CODE:
                    cur_val = ETEX_VERSION;
                    break;

                case XETEX_VERSION_CODE:
                    cur_val = XETEX_VERSION;
                    break;

                case XETEX_COUNT_GLYPHS_CODE:
                    scan_font_ident();
                    n = cur_val;
                    if (font_area[n] == AAT_FONT_FLAG)
                        cur_val = aat_font_get(m - XETEX_INT, font_layout_engine[n]);
                    else if (font_area[n] == OTGR_FONT_FLAG)
                        cur_val = ot_font_get(m - XETEX_INT, font_layout_engine[n]);
                    else
                        cur_val = 0;
                    break;

                case XETEX_COUNT_FEATURES_CODE:
                    scan_font_ident();
                    n = cur_val;
                    if (font_area[n] == AAT_FONT_FLAG)
                        cur_val = aat_font_get(m - XETEX_INT, font_layout_engine[n]);
                    else if (font_area[n] == OTGR_FONT_FLAG && usingGraphite(font_layout_engine[n]))
                        cur_val = ot_font_get(m - XETEX_INT, font_layout_engine[n]);
                    else
                        cur_val = 0;
                    break;

                case XETEX_VARIATION_CODE:
                case XETEX_VARIATION_MIN_CODE:
                case XETEX_VARIATION_MAX_CODE:
                case XETEX_VARIATION_DEFAULT_CODE:
                case XETEX_COUNT_VARIATIONS_CODE:
                    scan_font_ident();
                    n = cur_val;
                    cur_val = 0;
                    break;

                case XETEX_FEATURE_CODE_CODE:
                case XETEX_IS_EXCLUSIVE_FEATURE_CODE:
                case XETEX_COUNT_SELECTORS_CODE:
                    scan_font_ident();
                    n = cur_val;
                    if (font_area[n] == AAT_FONT_FLAG) {
                        scan_int();
                        k = cur_val;
                        cur_val = aat_font_get_1(m - XETEX_INT, font_layout_engine[n], k);
                    } else if (font_area[n] == OTGR_FONT_FLAG && usingGraphite(font_layout_engine[n])) {
                        scan_int();
                        k = cur_val;
                        cur_val = ot_font_get_1(m - XETEX_INT, font_layout_engine[n], k);
                    } else {
                        not_aat_gr_font_error(LAST_ITEM, m, n);
                        cur_val = -1;
                    }
                    break;

                case XETEX_SELECTOR_CODE_CODE:
                case XETEX_IS_DEFAULT_SELECTOR_CODE:
                    scan_font_ident();
                    n = cur_val;
                    if (font_area[n] == AAT_FONT_FLAG) {
                        scan_int();
                        k = cur_val;
                        scan_int();
                        cur_val = aat_font_get_2(m - XETEX_INT, font_layout_engine[n], k, cur_val);
                    } else if (font_area[n] == OTGR_FONT_FLAG && usingGraphite(font_layout_engine[n])) {
                        scan_int();
                        k = cur_val;
                        scan_int();
                        cur_val = ot_font_get_2(m - XETEX_INT, font_layout_engine[n], k, cur_val);
                    } else {
                        not_aat_gr_font_error(LAST_ITEM, m, n);
                        cur_val = -1;
                    }
                    break;

                case XETEX_FIND_VARIATION_BY_NAME_CODE:
                    scan_font_ident();
                    n = cur_val;
                    if (font_area[n] == AAT_FONT_FLAG) {
                        scan_and_pack_name();
                        cur_val = aat_font_get_named(m - XETEX_INT, font_layout_engine[n]);
                    } else {
                        not_aat_font_error(LAST_ITEM, m, n);
                        cur_val = -1;
                    }
                    break;

                case XETEX_FIND_FEATURE_BY_NAME_CODE:
                    scan_font_ident();
                    n = cur_val;
                    if (font_area[n] == AAT_FONT_FLAG) {
                        scan_and_pack_name();
                        cur_val = aat_font_get_named(m - XETEX_INT, font_layout_engine[n]);
                    } else if (font_area[n] == OTGR_FONT_FLAG && usingGraphite(font_layout_engine[n])) {
                        scan_and_pack_name();
                        cur_val = gr_font_get_named(m - XETEX_INT, font_layout_engine[n]);
                    } else {
                        not_aat_gr_font_error(LAST_ITEM, m, n);
                        cur_val = -1;
                    }
                    break;

                case XETEX_FIND_SELECTOR_BY_NAME_CODE:
                    scan_font_ident();
                    n = cur_val;
                    if (font_area[n] == AAT_FONT_FLAG) {
                        scan_int();
                        k = cur_val;
                        scan_and_pack_name();
                        cur_val = aat_font_get_named_1(m - XETEX_INT, font_layout_engine[n], k);
                    } else if (font_area[n] == OTGR_FONT_FLAG && usingGraphite(font_layout_engine[n])) {
                        scan_int();
                        k = cur_val;
                        scan_and_pack_name();
                        cur_val = gr_font_get_named_1(m - XETEX_INT, font_layout_engine[n], k);
                    } else {
                        not_aat_gr_font_error(LAST_ITEM, m, n);
                        cur_val = -1;
                    }
                    break;

                case XETEX_OT_COUNT_SCRIPTS_CODE:
                    scan_font_ident();
                    n = cur_val;
                    if (font_area[n] == OTGR_FONT_FLAG && usingOpenType(font_layout_engine[n])) {
                        cur_val = ot_font_get(m - XETEX_INT, font_layout_engine[n]);
                    } else {
                        cur_val = 0;
                    }
                    break;

                case XETEX_OT_COUNT_LANGUAGES_CODE:
                case XETEX_OT_SCRIPT_CODE:
                    scan_font_ident();
                    n = cur_val;
                    if (font_area[n] == OTGR_FONT_FLAG && usingOpenType(font_layout_engine[n])) {
                        scan_int();
                        cur_val = ot_font_get_1(m - XETEX_INT, font_layout_engine[n], cur_val);
                    } else {
                        not_ot_font_error(LAST_ITEM, m, n);
                        cur_val = -1;
                    }
                    break;

                case XETEX_OT_COUNT_FEATURES_CODE:
                case XETEX_OT_LANGUAGE_CODE:
                    scan_font_ident();
                    n = cur_val;
                    if (font_area[n] == OTGR_FONT_FLAG && usingOpenType(font_layout_engine[n])) {
                        scan_int();
                        k = cur_val;
                        scan_int();
                        cur_val = ot_font_get_2(m - XETEX_INT, font_layout_engine[n], k, cur_val);
                    } else {
                        not_ot_font_error(LAST_ITEM, m, n);
                        cur_val = -1;
                    }
                    break;

                case XETEX_OT_FEATURE_CODE:
                    scan_font_ident();
                    n = cur_val;
                    if (font_area[n] == OTGR_FONT_FLAG && usingOpenType(font_layout_engine[n])) {
                        scan_int();
                        k = cur_val;
                        scan_int();
                        kk = cur_val;
                        scan_int();
                        cur_val = ot_font_get_3(m - XETEX_INT, font_layout_engine[n], k, kk, cur_val);
                    } else {
                        not_ot_font_error(LAST_ITEM, m, n);
                        cur_val = -1;
                    }
                    break;

                case XETEX_MAP_CHAR_TO_GLYPH_CODE:
                    if (font_area[eqtb[CUR_FONT_LOC].b32.s1] == AAT_FONT_FLAG ||
                        font_area[eqtb[CUR_FONT_LOC].b32.s1] == OTGR_FONT_FLAG) {
                        scan_int();
                        n = cur_val;
                        cur_val = map_char_to_glyph(eqtb[CUR_FONT_LOC].b32.s1, n);
                    } else {
                        not_native_font_error(LAST_ITEM, m, eqtb[CUR_FONT_LOC].b32.s1);
                        cur_val = 0;
                    }
                    break;

                case XETEX_GLYPH_INDEX_CODE:
                    if (font_area[eqtb[CUR_FONT_LOC].b32.s1] == AAT_FONT_FLAG ||
                        font_area[eqtb[CUR_FONT_LOC].b32.s1] == OTGR_FONT_FLAG) {
                        scan_and_pack_name();
                        cur_val = map_glyph_to_index(eqtb[CUR_FONT_LOC].b32.s1);
                    } else {
                        not_native_font_error(LAST_ITEM, m, eqtb[CUR_FONT_LOC].b32.s1);
                        cur_val = 0;
                    }
                    break;

                case XETEX_FONT_TYPE_CODE:
                    scan_font_ident();
                    n = cur_val;
                    if (font_area[n] == AAT_FONT_FLAG)
                        cur_val = 1;
                    else if (font_area[n] == OTGR_FONT_FLAG && usingOpenType(font_layout_engine[n]))
                        cur_val = 2;
                    else if (font_area[n] == OTGR_FONT_FLAG && usingGraphite(font_layout_engine[n]))
                        cur_val = 3;
                    else
                        cur_val = 0;
                    break;

                case XETEX_FIRST_CHAR_CODE:
                case XETEX_LAST_CHAR_CODE:
                    scan_font_ident();
                    n = cur_val;
                    if (font_area[n] == AAT_FONT_FLAG || font_area[n] == OTGR_FONT_FLAG) {
                        cur_val = get_font_char_range(n, m == XETEX_FIRST_CHAR_CODE);
                    } else {
                        if (m == XETEX_FIRST_CHAR_CODE)
                            cur_val = font_bc[n];
                        else
                            cur_val = font_ec[n];
                    }
                    break;

                case PDF_LAST_X_POS_CODE:
                    cur_val = pdf_last_x_pos;
                    break;

                case PDF_LAST_Y_POS_CODE:
                    cur_val = pdf_last_y_pos;
                    break;

                case XETEX_PDF_PAGE_COUNT_CODE:
                    scan_and_pack_name();
                    cur_val = count_pdf_file_pages();
                    break;

                case CURRENT_GROUP_LEVEL_CODE:
                    cur_val = cur_level - 1;
                    break;

                case CURRENT_GROUP_TYPE_CODE:
                    cur_val = cur_group;
                    break;

                case CURRENT_IF_LEVEL_CODE:
                    q = cond_ptr;
                    cur_val = 0;
                    while (q != TEX_NULL) {
                        cur_val++;
                        q = LLIST_link(q);
                    }
                    break;

                case CURRENT_IF_TYPE_CODE:
                    if (cond_ptr == TEX_NULL)
                        cur_val = 0;
                    else if (cur_if < UNLESS_CODE)
                        cur_val = cur_if + 1;
                    else
                        cur_val = -(int32_t) (cur_if - 31);
                    break;

                case CURRENT_IF_BRANCH_CODE:
                    if (if_limit == OR_CODE || if_limit == ELSE_CODE)
                        cur_val = 1;
                    else if (if_limit == FI_CODE)
                        cur_val = -1;
                    else
                        cur_val = 0;
                    break;

                case GLUE_STRETCH_ORDER_CODE:
                case GLUE_SHRINK_ORDER_CODE:
                    scan_normal_glue();
                    q = cur_val;
                    if (m == GLUE_STRETCH_ORDER_CODE)
                        cur_val = mem[q].b16.s1;
                    else
                        cur_val = mem[q].b16.s0;
                    delete_glue_ref(q);
                    break;
                }

                cur_val_level = INT_VAL;
            }
        } else {
            cur_val = 0;
            tx = cur_list.tail;

            if (tx < hi_mem_min) {
                if (NODE_type(tx) == MATH_NODE && mem[tx].b16.s0 == END_M_CODE) {
                    r = cur_list.head;
                    do {
                        q = r;
                        r = mem[q].b32.s1;
                    } while (r != tx);
                    tx = q;
                }
            }

            if (cur_chr == LAST_NODE_TYPE_CODE) {
                cur_val_level = INT_VAL;
                if (tx == cur_list.head || cur_list.mode == 0)
                    cur_val = -1;
            } else {
                cur_val_level = cur_chr;
            }

            if (tx < hi_mem_min && cur_list.mode != 0)
                switch (cur_chr) {
                case INT_VAL:
                    if (NODE_type(tx) == PENALTY_NODE)
                        cur_val = mem[tx + 1].b32.s1;
                    break;
                case DIMEN_VAL:
                    if (NODE_type(tx) == KERN_NODE)
                        cur_val = mem[tx + 1].b32.s1;
                    break;
                case GLUE_VAL:
                    if (NODE_type(tx) == GLUE_NODE) {
                        cur_val = mem[tx + 1].b32.s0;
                        if (mem[tx].b16.s0 == MU_GLUE)
                            cur_val_level = MU_VAL;
                    }
                    break;
                case LAST_NODE_TYPE_CODE:
                    if (NODE_type(tx) <= UNSET_NODE)
                        cur_val = mem[tx].b16.s1 + 1;
                    else
                        cur_val = (UNSET_NODE + 2);
                    break;
            } else if (cur_list.mode == VMODE && tx == cur_list.head)
                switch (cur_chr) {
                case INT_VAL:
                    cur_val = last_penalty;
                    break;
                case DIMEN_VAL:
                    cur_val = last_kern;
                    break;
                case GLUE_VAL:
                    if (last_glue != MAX_HALFWORD)
                        cur_val = last_glue;
                    break;
                case LAST_NODE_TYPE_CODE:
                    cur_val = last_node_type;
                    break;
                }
        }
        break;

    case IGNORE_SPACES:
        if (cur_chr == 1) { /*406: */
            get_token();

            if (cur_cs < HASH_BASE) {
                cur_cs = prim_lookup(cur_cs - SINGLE_BASE);
            } else {
                cur_cs = prim_lookup(hash[cur_cs].s1);
            }

            if (cur_cs != UNDEFINED_PRIMITIVE) {
                cur_cmd = eqtb[PRIM_EQTB_BASE + cur_cs].b16.s1;
                cur_chr = eqtb[PRIM_EQTB_BASE + cur_cs].b32.s1;
                cur_cs = PRIM_EQTB_BASE + cur_cs;
                cur_tok = CS_TOKEN_FLAG + cur_cs;
            } else {
                cur_cmd = RELAX;
                cur_chr = 0;
                cur_tok = CS_TOKEN_FLAG + FROZEN_RELAX;
                cur_cs = FROZEN_RELAX;
            }
            goto restart;
        }
        break;

    default:
        error_here_with_diagnostic("You can't use `");
        print_cmd_chr(cur_cmd, cur_chr);
        print_cstr("' after ");
        print_esc_cstr("the");
        capture_to_diagnostic(NULL);

        help_ptr = 1;
        help_line[0] = "I'm forgetting what you said and using zero instead.";
        error();
        cur_val = 0;
        if (level != TOK_VAL) {
            cur_val_level = DIMEN_VAL;
        } else {
            cur_val_level = INT_VAL;
        }
        break;
    }

    while (cur_val_level > level) { /*447:*/
        if (cur_val_level == GLUE_VAL)
            cur_val = mem[cur_val + 1].b32.s1;
        else if (cur_val_level == MU_VAL)
            mu_error();
        cur_val_level--;
    }

    if (negative) {
        if (cur_val_level >= GLUE_VAL) {
            cur_val = new_spec(cur_val);
            mem[cur_val + 1].b32.s1 = -(int32_t) mem[cur_val + 1].b32.s1;
            mem[cur_val + 2].b32.s1 = -(int32_t) mem[cur_val + 2].b32.s1;
            mem[cur_val + 3].b32.s1 = -(int32_t) mem[cur_val + 3].b32.s1;
        } else {
            cur_val = -(int32_t) cur_val;
        }
    } else if (cur_val_level >= GLUE_VAL && cur_val_level <= MU_VAL) {
        GLUE_SPEC_ref_count(cur_val)++;
    }
}


void
scan_int(void)
{
    bool negative;
    int32_t m;
    small_number d;
    bool vacuous;
    bool OK_so_far;

    radix = 0;
    OK_so_far = true;
    negative = false;

    do { /*424:*/
        do {
            get_x_token();
        } while (cur_cmd == SPACER);

        if (cur_tok == OTHER_TOKEN + '-' ) {
            negative = !negative;
            cur_tok = OTHER_TOKEN + '+';
        }
    } while (cur_tok == OTHER_TOKEN + '+');

restart:
    if (cur_tok == ALPHA_TOKEN) { /*460:*/
        get_token();

        if (cur_tok < CS_TOKEN_FLAG) {
            cur_val = cur_chr;
            if (cur_cmd <= RIGHT_BRACE) {
                if (cur_cmd == RIGHT_BRACE)
                    align_state++;
                else
                    align_state--;
            }
        } else if (cur_tok < CS_TOKEN_FLAG + SINGLE_BASE) {
            cur_val = cur_tok - (CS_TOKEN_FLAG + ACTIVE_BASE);
        } else {
            cur_val = cur_tok - (CS_TOKEN_FLAG + SINGLE_BASE);
        }

        if (cur_val > BIGGEST_USV) {
            error_here_with_diagnostic("Improper alphabetic constant");
            capture_to_diagnostic(NULL);

            help_ptr = 2;
            help_line[1] = "A one-character control sequence belongs after a ` mark.";
            help_line[0] = "So I'm essentially inserting \\0 here.";
            cur_val = '0' ;
            back_error();
        } else { /*461:*/
            get_x_token();
            if (cur_cmd != SPACER)
                back_input();
        }
    } else if (cur_tok == CS_TOKEN_FLAG + FROZEN_PRIMITIVE) { /*406:*/
        get_token();

        if (cur_cs < HASH_BASE) {
            cur_cs = prim_lookup(cur_cs - SINGLE_BASE);
        } else {
            cur_cs = prim_lookup(hash[cur_cs].s1);
        }

        if (cur_cs != UNDEFINED_PRIMITIVE) {
            cur_cmd = eqtb[PRIM_EQTB_BASE + cur_cs].b16.s1;
            cur_chr = eqtb[PRIM_EQTB_BASE + cur_cs].b32.s1;
            cur_cs = PRIM_EQTB_BASE + cur_cs;
            cur_tok = CS_TOKEN_FLAG + cur_cs;
        } else {
            cur_cmd = RELAX;
            cur_chr = 0;
            cur_tok = CS_TOKEN_FLAG + FROZEN_RELAX;
            cur_cs = FROZEN_RELAX;
        }
        goto restart;
    } else if (cur_cmd >= MIN_INTERNAL && cur_cmd <= MAX_INTERNAL) {
        scan_something_internal(INT_VAL, false);
    } else { /*462:*/
        radix = 10;
        m = 0xCCCCCCC;

        if (cur_tok == OCTAL_TOKEN) {
            radix = 8;
            m = 0x10000000;
            get_x_token();
        } else if (cur_tok == HEX_TOKEN) {
            radix = 16;
            m = 0x8000000;
            get_x_token();
        }

        vacuous = true;
        cur_val = 0;

        while (true) {
            if (cur_tok < ZERO_TOKEN + radix && cur_tok >= ZERO_TOKEN && cur_tok <= ZERO_TOKEN + 9) {
                d = cur_tok - ZERO_TOKEN;
            } else if (radix == 16) {
                if (cur_tok <= A_TOKEN + 5 && cur_tok >= A_TOKEN)
                    d = cur_tok - A_TOKEN + 10;
                else if (cur_tok <= OTHER_A_TOKEN + 5 && cur_tok >= OTHER_A_TOKEN)
                    d = cur_tok - OTHER_A_TOKEN + 10;
                else
                    break;
            } else {
                break;
            }

            vacuous = false;

            if (cur_val >= m && (cur_val > m || d > 7 || radix != 10)) {
                if (OK_so_far) {
                    error_here_with_diagnostic("Number too big");
                    capture_to_diagnostic(NULL);

                    help_ptr = 2;
                    help_line[1] = "I can only go up to 2147483647='17777777777=\"7FFFFFFF,";
                    help_line[0] = "so I'm using that number instead of yours.";
                    error();
                    cur_val = TEX_INFINITY;
                    OK_so_far = false;
                }
            } else {
                cur_val = cur_val * radix + d;
            }

            get_x_token();
        } /*:463*/

        if (vacuous) { /*464:*/
            error_here_with_diagnostic("Missing number, treated as zero");
            capture_to_diagnostic(NULL);

            help_ptr = 3;
            help_line[2] = "A number should have been here; I inserted `0'.";
            help_line[1] = "(If you can't figure out why I needed to see a number,";
            help_line[0] = "look up `weird error' in the index to The TeXbook.)";
            back_error();
        } else if (cur_cmd != SPACER) {
            back_input();
        }
    }

    if (negative)
        cur_val = -(int32_t) cur_val;
}


static scaled_t
round_decimals(small_number k)
{
    int32_t a = 0;

    while (k > 0) {
        k--;
        a = (a + dig[k] * 0x20000) / 10;
    }

    return (a + 1) / 2;
}


void
xetex_scan_dimen(bool mu, bool inf, bool shortcut, bool requires_units)
{
    bool negative;
    int32_t f;
    int32_t num, denom;
    small_number k, kk;
    int32_t p, q;
    scaled_t v;
    int32_t save_cur_val;

    f = 0;
    arith_error = false;
    cur_order = NORMAL;
    negative = false;

    if (!shortcut) {
        negative = false;

        do {
            do {
                get_x_token();
            } while (cur_cmd == SPACER);

            if (cur_tok == OTHER_TOKEN + '-' ) {
                negative = !negative;
                cur_tok = OTHER_TOKEN + '+';
            }
        } while (cur_tok == OTHER_TOKEN + '+');

        if (cur_cmd >= MIN_INTERNAL && cur_cmd <= MAX_INTERNAL) { /*468:*/
            if (mu) {
                scan_something_internal(MU_VAL, false);
                if (cur_val_level >= GLUE_VAL) {
                    v = mem[cur_val + 1].b32.s1;
                    delete_glue_ref(cur_val);
                    cur_val = v;
                }

                if (cur_val_level == MU_VAL)
                    goto attach_sign;
                if (cur_val_level != INT_VAL)
                    mu_error();
            } else {
                scan_something_internal(DIMEN_VAL, false);
                if (cur_val_level == DIMEN_VAL)
                    goto attach_sign;
            }
        } else {
            back_input();

            if (cur_tok == CONTINENTAL_POINT_TOKEN)
                cur_tok = POINT_TOKEN;

            if (cur_tok != POINT_TOKEN) {
                scan_int();
            } else {
                radix = 10;
                cur_val = 0;
            }

            if (cur_tok == CONTINENTAL_POINT_TOKEN)
                cur_tok = POINT_TOKEN;

            if (radix == 10 && cur_tok == POINT_TOKEN) { /*471:*/
                k = 0;
                p = TEX_NULL;
                get_token();

                while (true) {
                    get_x_token();
                    if (cur_tok > ZERO_TOKEN + 9 || cur_tok < ZERO_TOKEN)
                        goto done1;

                    if (k < 17) {
                        q = get_avail();
                        mem[q].b32.s1 = p;
                        mem[q].b32.s0 = cur_tok - ZERO_TOKEN;
                        p = q;
                        k++;
                    }
                }

            done1:
                for (kk = k; kk >= 1; kk--) {
                    dig[kk - 1] = mem[p].b32.s0;
                    q = p;
                    p = LLIST_link(p);
                    mem[q].b32.s1 = avail;
                    avail = q;
                }

                f = round_decimals(k);
                if (cur_cmd != SPACER)
                    back_input();
            }
        }
    }

    if (cur_val < 0) {
        negative = !negative;
        cur_val = -(int32_t) cur_val;
    }

    if (requires_units) {
        if (inf) { /*473:*/
            if (scan_keyword("fil")) {
                cur_order = FIL;

                while (scan_keyword("l")) {
                    if (cur_order == FILLL) {
                        error_here_with_diagnostic("Illegal unit of measure (replaced with filll)");
                        capture_to_diagnostic(NULL);

                        help_ptr = 1;
                        // "ddon't" looks like wordplay from Knuth inspired by "filll"
                        help_line[0] = "I dddon't go any higher than filll.";
                        error();
                    } else {
                        cur_order++;
                    }
                }

                goto attach_fraction;
            }
        }

        save_cur_val = cur_val;

        do {
            get_x_token();
        } while (cur_cmd == SPACER);

        if (cur_cmd < MIN_INTERNAL || cur_cmd > MAX_INTERNAL) {
            back_input();
        } else {
            if (mu) {
                scan_something_internal(MU_VAL, false);
                if (cur_val_level >= GLUE_VAL) {
                    v = mem[cur_val + 1].b32.s1;
                    delete_glue_ref(cur_val);
                    cur_val = v;
                }
                if (cur_val_level != MU_VAL)
                    mu_error();
            } else {
                scan_something_internal(DIMEN_VAL, false);
            }

            v = cur_val;
            goto found;
        }

        if (mu)
            goto not_found;

        if (scan_keyword("em"))
            v = font_info[QUAD_CODE + param_base[eqtb[CUR_FONT_LOC].b32.s1]].b32.s1;
        else if (scan_keyword("ex"))
            v = font_info[X_HEIGHT_CODE + param_base[eqtb[CUR_FONT_LOC].b32.s1]].b32.s1;
        else
            goto not_found;

        get_x_token();
        if (cur_cmd != SPACER)
            back_input();

    found:
        cur_val = mult_and_add(save_cur_val, v, xn_over_d(v, f, 65536L), MAX_HALFWORD);
        goto attach_sign;

    not_found:
        if (mu) { /*475:*/
            if (scan_keyword("mu")) {
                goto attach_fraction;
            } else {
                error_here_with_diagnostic("Illegal unit of measure (mu inserted)");
                capture_to_diagnostic(NULL);

                help_ptr = 4;
                help_line[3] = "The unit of measurement in math glue must be mu.";
                help_line[2] = "To recover gracefully from this error, it's best to";
                help_line[1] = "delete the erroneous units; e.g., type `2' to delete";
                help_line[0] = "two letters. (See Chapter 27 of The TeXbook.)";
                error();
                goto attach_fraction;
            }
        }

        if (scan_keyword("true")) { /*476:*/
            prepare_mag();
            if (INTPAR(mag) != 1000) {
                cur_val = xn_over_d(cur_val, 1000, INTPAR(mag));
                f = (1000 * f + 65536L * tex_remainder) / INTPAR(mag);
                cur_val = cur_val + (f / 65536L);
                f = f % 65536L;
            }
        }

        if (scan_keyword("pt"))
            goto attach_fraction;

        if (scan_keyword("in")) {
            num = 7227; /* magic ratio consant */
            denom = 100;
        } else if (scan_keyword("pc")) {
            num = 12;
            denom = 1;
        } else if (scan_keyword("cm")) {
            num = 7227; /* magic ratio consant */
            denom = 254; /* magic ratio consant */
        } else if (scan_keyword("mm")) {
            num = 7227; /* magic ratio consant */
            denom = 2540; /* magic ratio consant */
        } else if (scan_keyword("bp")) {
            num = 7227; /* magic ratio consant */
            denom = 7200; /* magic ratio consant */
        } else if (scan_keyword("dd")) {
            num = 1238; /* magic ratio consant */
            denom = 1157; /* magic ratio consant */
        } else if (scan_keyword("cc")) {
            num = 14856; /* magic ratio consant */
            denom = 1157; /* magic ratio consant */
        } else if (scan_keyword("sp")) {
            goto done;
        } else { /*478:*/
            error_here_with_diagnostic("Illegal unit of measure (pt inserted)");
            capture_to_diagnostic(NULL);

            help_ptr = 6;
            help_line[5] = "Dimensions can be in units of em, ex, in, pt, pc,";
            help_line[4] = "cm, mm, dd, cc, bp, or sp; but yours is a new one!";
            help_line[3] = "I'll assume that you meant to say pt, for printer's points.";
            help_line[2] = "To recover gracefully from this error, it's best to";
            help_line[1] = "delete the erroneous units; e.g., type `2' to delete";
            help_line[0] = "two letters. (See Chapter 27 of The TeXbook.)";
            error();
            goto done2;
        }

        cur_val = xn_over_d(cur_val, num, denom);
        f = (num * f + 65536L * tex_remainder) / denom;
        cur_val = cur_val + (f / 65536L);
        f = f % 65536L;

    done2:
        ;
    attach_fraction:

        if (cur_val >= 16384)
            arith_error = true;
        else
            cur_val = cur_val * 65536L + f;

    done:
        get_x_token();
        if (cur_cmd != SPACER)
            back_input();
    } else { /* if(requires_units) */
        if (cur_val >= 16384)
            arith_error = true;
        else
            cur_val = cur_val * 65536L + f;
    }

attach_sign:
    if (arith_error || abs(cur_val) >= 0x40000000) { /*479:*/
        error_here_with_diagnostic("Dimension too large");
        capture_to_diagnostic(NULL);

        help_ptr = 2;
        help_line[1] = "I can't work with sizes bigger than about 19 feet.";
        help_line[0] = "Continue and I'll use the largest value I can.";
        error();
        cur_val = MAX_HALFWORD;
        arith_error = false;
    }

    if (negative)
        cur_val = -(int32_t) cur_val;
}


void scan_dimen(bool mu, bool inf, bool shortcut)
{
    xetex_scan_dimen(mu, inf, shortcut, true);
}

void scan_decimal(void)
{
    xetex_scan_dimen(false, false, false, false);
}


void
scan_glue(small_number level)
{
    bool negative;
    int32_t q;
    bool mu;

    mu = (level == MU_VAL);
    negative = false;

    do {
        do {
            get_x_token();
        } while (cur_cmd == SPACER);

        if (cur_tok == OTHER_TOKEN + 45 /*"-"*/) {
            negative = !negative;
            cur_tok = OTHER_TOKEN + 43 /*"+"*/;
        }
    } while (cur_tok == OTHER_TOKEN + 43 /*"+"*/);

    if (cur_cmd >= MIN_INTERNAL && cur_cmd <= MAX_INTERNAL) {
        scan_something_internal(level, negative);
        if (cur_val_level >= GLUE_VAL) {
            if (cur_val_level != level)
                mu_error();
            return;
        }

        if (cur_val_level == INT_VAL)
            scan_dimen(mu, false, true);
        else if (level == MU_VAL)
            mu_error();
    } else {
        back_input();
        scan_dimen(mu, false, false);
        if (negative)
            cur_val = -(int32_t) cur_val;
    }

    q = new_spec(0);
    mem[q + 1].b32.s1 = cur_val;

    if (scan_keyword("plus")) {
        scan_dimen(mu, true, false);
        mem[q + 2].b32.s1 = cur_val;
        mem[q].b16.s1 = cur_order;
    }

    if (scan_keyword("minus")) {
        scan_dimen(mu, true, false);
        mem[q + 3].b32.s1 = cur_val;
        mem[q].b16.s0 = cur_order;
    }

    cur_val = q; /*:481*/
}


int32_t add_or_sub(int32_t x, int32_t y, int32_t max_answer, bool negative)
{
    int32_t a;
    if (negative)
        y = -(int32_t) y;
    if (x >= 0) {

        if (y <= max_answer - x)
            a = x + y;
        else {

            arith_error = true;
            a = 0;
        }
    } else if (y >= -(int32_t) max_answer - x)
        a = x + y;
    else {

        arith_error = true;
        a = 0;
    }
    return a;
}

int32_t quotient(int32_t n, int32_t d)
{
    bool negative;
    int32_t a;
    if (d == 0) {
        arith_error = true;
        a = 0;
    } else {

        if (d > 0)
            negative = false;
        else {

            d = -(int32_t) d;
            negative = true;
        }
        if (n < 0) {
            n = -(int32_t) n;
            negative = !negative;
        }
        a = n / d;
        n = n - a * d;
        d = n - d;
        if (d + n >= 0)
            a++;
        if (negative)
            a = -(int32_t) a;
    }
    return a;
}

int32_t fract(int32_t x, int32_t n, int32_t d, int32_t max_answer)
{
    bool negative;
    int32_t a;
    int32_t f;
    int32_t h;
    int32_t r;
    int32_t t;
    if (d == 0)
        goto too_big;
    a = 0;
    if (d > 0)
        negative = false;
    else {

        d = -(int32_t) d;
        negative = true;
    }
    if (x < 0) {
        x = -(int32_t) x;
        negative = !negative;
    } else if (x == 0)
        goto done;
    if (n < 0) {
        n = -(int32_t) n;
        negative = !negative;
    }
    t = n / d;
    if (t > max_answer / x)
        goto too_big;
    a = t * x;
    n = n - t * d;
    if (n == 0)
        goto found;
    t = x / d;
    if (t > (max_answer - a) / n)
        goto too_big;
    a = a + t * n;
    x = x - t * d;
    if (x == 0)
        goto found;
    if (x < n) {
        t = x;
        x = n;
        n = t;
    }
    f = 0;
    r = (d / 2) - d;
    h = -(int32_t) r;
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
            goto found1;
        if (x < h)
            x = x + x;
        else {

            t = x - d;
            x = t + x;
            f = f + n;
            if (x < n) {
                if (x == 0)
                    goto found1;
                t = x;
                x = n;
                n = t;
            }
        }
    }
found1:
    if (f > (max_answer - a))
        goto too_big;
    a = a + f;
 found:
    if (negative)
        a = -(int32_t) a;
    goto done;
too_big:
    {
        arith_error = true;
        a = 0;
    }
 done:
    return a;
}

void scan_expr(void)
{
    bool a, b;
    small_number l;
    small_number r;
    small_number s;
    small_number o;
    int32_t e;
    int32_t t;
    int32_t f;
    int32_t n;
    int32_t p;
    int32_t q;
    l = cur_val_level;
    a = arith_error;
    b = false;
    p = TEX_NULL;

restart:
    r = EXPR_NONE;
    e = 0;
    s = EXPR_NONE;
    t = 0;
    n = 0;
continue_:
    if (s == EXPR_NONE)
        o = l;
    else
        o = INT_VAL;
    do {
        get_x_token();
    } while (cur_cmd == SPACER);
    if (cur_tok == (OTHER_TOKEN + 40)) {    /*1576: */
        q = get_node(EXPR_NODE_SIZE);
        mem[q].b32.s1 = p;
        mem[q].b16.s1 = l;
        mem[q].b16.s0 = 4 * s + r;
        mem[q + 1].b32.s1 = e;
        mem[q + 2].b32.s1 = t;
        mem[q + 3].b32.s1 = n;
        p = q;
        l = o;
        goto restart;
    }
    back_input();
    if (o == INT_VAL)
        scan_int();
    else if (o == DIMEN_VAL)
        scan_dimen(false, false, false);
    else if (o == GLUE_VAL)
        scan_normal_glue();
    else
        scan_mu_glue();
    f = /*:1573 */ cur_val;
found: /*1572:*//*424:*/
    do {
        get_x_token();
    } while (cur_cmd == SPACER);
    if (cur_tok == (OTHER_TOKEN + 43))
        o = EXPR_ADD;
    else if (cur_tok == (OTHER_TOKEN + 45))
        o = EXPR_SUB;
    else if (cur_tok == (OTHER_TOKEN + 42))
        o = EXPR_MULT;
    else if (cur_tok == (OTHER_TOKEN + 47))
        o = EXPR_DIV;
    else {

        o = EXPR_NONE;
        if (p == TEX_NULL) {
            if (cur_cmd != RELAX)
                back_input();
        } else if (cur_tok != (OTHER_TOKEN + 41)) {
            error_here_with_diagnostic("Missing ) inserted for expression");
            capture_to_diagnostic(NULL);

            {
                help_ptr = 1;
                help_line[0] = "I was expecting to see `+', `-', `*', `/', or `)'. Didn't.";
            }
            back_error();
        }
    }
    arith_error = b;
    if ((l == INT_VAL) || (s > EXPR_SUB)) {
        if ((f > TEX_INFINITY) || (f < -TEX_INFINITY)) {
            arith_error = true;
            f = 0;
        }
    } else if (l == DIMEN_VAL) {
        if (abs(f) > MAX_HALFWORD) {
            arith_error = true;
            f = 0;
        }
    } else {

        if ((abs(mem[f + 1].b32.s1) > MAX_HALFWORD) || (abs(mem[f + 2].b32.s1) > MAX_HALFWORD)
            || (abs(mem[f + 3].b32.s1) > MAX_HALFWORD)) {
            arith_error = true;
            delete_glue_ref(f);
            f = new_spec(0);
        }
    }
    switch (s) {                /*1579: */
    case 0:
        if ((l >= GLUE_VAL) && (o != EXPR_NONE)) {
            t = new_spec(f);
            delete_glue_ref(f);
            if (mem[t + 2].b32.s1 == 0)
                mem[t].b16.s1 = NORMAL;
            if (mem[t + 3].b32.s1 == 0)
                mem[t].b16.s0 = NORMAL;
        } else
            t = f;
        break;
    case 3:
        if (o == EXPR_DIV) {
            n = f;
            o = EXPR_SCALE;
        } else if (l == INT_VAL)
            t = mult_and_add(t, f, 0, TEX_INFINITY);
        else if (l == DIMEN_VAL)
            t = mult_and_add(t, f, 0, MAX_HALFWORD);
        else {

            mem[t + 1].b32.s1 = mult_and_add(mem[t + 1].b32.s1, f, 0, MAX_HALFWORD);
            mem[t + 2].b32.s1 = mult_and_add(mem[t + 2].b32.s1, f, 0, MAX_HALFWORD);
            mem[t + 3].b32.s1 = mult_and_add(mem[t + 3].b32.s1, f, 0, MAX_HALFWORD);
        }
        break;
    case 4:
        if (l < GLUE_VAL)
            t = quotient(t, f);
        else {

            mem[t + 1].b32.s1 = quotient(mem[t + 1].b32.s1, f);
            mem[t + 2].b32.s1 = quotient(mem[t + 2].b32.s1, f);
            mem[t + 3].b32.s1 = quotient(mem[t + 3].b32.s1, f);
        }
        break;
    case 5:
        if (l == INT_VAL)
            t = fract(t, n, f, TEX_INFINITY);
        else if (l == DIMEN_VAL)
            t = fract(t, n, f, MAX_HALFWORD);
        else {

            mem[t + 1].b32.s1 = fract(mem[t + 1].b32.s1, n, f, MAX_HALFWORD);
            mem[t + 2].b32.s1 = fract(mem[t + 2].b32.s1, n, f, MAX_HALFWORD);
            mem[t + 3].b32.s1 = fract(mem[t + 3].b32.s1, n, f, MAX_HALFWORD);
        }
        break;
    }
    if (o > EXPR_SUB)
        s = o;
    else {                      /*1580: */

        s = EXPR_NONE;
        if (r == EXPR_NONE)
            e = t;
        else if (l == INT_VAL)
            e = add_or_sub(e, t, TEX_INFINITY, r == EXPR_SUB);
        else if (l == DIMEN_VAL)
            e = add_or_sub(e, t, MAX_HALFWORD, r == EXPR_SUB);
        else {                  /*1582: */

            mem[e + 1].b32.s1 = add_or_sub(mem[e + 1].b32.s1, mem[t + 1].b32.s1, MAX_HALFWORD, r == EXPR_SUB);
            if (mem[e].b16.s1 == mem[t].b16.s1)
                mem[e + 2].b32.s1 = add_or_sub(mem[e + 2].b32.s1, mem[t + 2].b32.s1, MAX_HALFWORD, r == EXPR_SUB);
            else if ((mem[e].b16.s1 < mem[t].b16.s1) && (mem[t + 2].b32.s1 != 0)) {
                mem[e + 2].b32.s1 = mem[t + 2].b32.s1;
                mem[e].b16.s1 = mem[t].b16.s1;
            }
            if (mem[e].b16.s0 == mem[t].b16.s0)
                mem[e + 3].b32.s1 = add_or_sub(mem[e + 3].b32.s1, mem[t + 3].b32.s1, MAX_HALFWORD, r == EXPR_SUB);
            else if ((mem[e].b16.s0 < mem[t].b16.s0) && (mem[t + 3].b32.s1 != 0)) {
                mem[e + 3].b32.s1 = mem[t + 3].b32.s1;
                mem[e].b16.s0 = mem[t].b16.s0;
            }
            delete_glue_ref(t);
            if (mem[e + 2].b32.s1 == 0)
                mem[e].b16.s1 = NORMAL;
            if (mem[e + 3].b32.s1 == 0)
                mem[e].b16.s0 = NORMAL;
        }
        r = o;
    }
    b = arith_error;
    if (o != EXPR_NONE)
        goto continue_;
    if (p != TEX_NULL) {     /*1577: */
        f = e;
        q = p;
        e = mem[q + 1].b32.s1;
        t = mem[q + 2].b32.s1;
        n = mem[q + 3].b32.s1;
        s = mem[q].b16.s0 / 4;
        r = mem[q].b16.s0 % 4;
        l = mem[q].b16.s1;
        p = mem[q].b32.s1;
        free_node(q, EXPR_NODE_SIZE);
        goto found;
    }
    if (b) {
        error_here_with_diagnostic("Arithmetic overflow");
        capture_to_diagnostic(NULL);

        {
            help_ptr = 2;
            help_line[1] = "I can't evaluate this expression,";
            help_line[0] = "since the result is out of range.";
        }
        error();
        if (l >= GLUE_VAL) {
            delete_glue_ref(e);
            e = 0;
            GLUE_SPEC_ref_count(e)++;
        } else
            e = 0;
    }
    arith_error = a;
    cur_val = e;
    cur_val_level = l;
}

void scan_normal_glue(void)
{
    scan_glue(GLUE_VAL);
}

void scan_mu_glue(void)
{
    scan_glue(MU_VAL);
}

int32_t scan_rule_spec(void)
{
    int32_t q;
    q = new_rule();
    if (cur_cmd == VRULE)
        mem[q + 1].b32.s1 = DEFAULT_RULE;
    else {

        mem[q + 3].b32.s1 = DEFAULT_RULE;
        mem[q + 2].b32.s1 = 0;
    }
reswitch:
    if (scan_keyword("width")) {
        scan_dimen(false, false, false);
        mem[q + 1].b32.s1 = cur_val;
        goto reswitch;
    }
    if (scan_keyword("height")) {
        scan_dimen(false, false, false);
        mem[q + 3].b32.s1 = cur_val;
        goto reswitch;
    }
    if (scan_keyword("depth")) {
        scan_dimen(false, false, false);
        mem[q + 2].b32.s1 = cur_val;
        goto reswitch;
    }
    return q;
}

void scan_general_text(void)
{
    unsigned char /*absorbing */ s;
    int32_t w;
    int32_t d;
    int32_t p;
    int32_t q;
    int32_t unbalance;
    s = scanner_status;
    w = warning_index;
    d = def_ref;
    scanner_status = ABSORBING;
    warning_index = cur_cs;
    def_ref = get_avail();
    mem[def_ref].b32.s0 = TEX_NULL;
    p = def_ref;
    scan_left_brace();
    unbalance = 1;
    while (true) {

        get_token();
        if (cur_tok < RIGHT_BRACE_LIMIT) {

            if (cur_cmd < RIGHT_BRACE)
                unbalance++;
            else {

                unbalance--;
                if (unbalance == 0)
                    goto found;
            }
        }
        {
            q = get_avail();
            mem[p].b32.s1 = q;
            mem[q].b32.s0 = cur_tok;
            p = q;
        }
    }
 found:
    q = mem[def_ref].b32.s1;
    {
        mem[def_ref].b32.s1 = avail;
        avail = def_ref;
    }
    if (q == TEX_NULL)
        cur_val = TEMP_HEAD;
    else
        cur_val = p;
    mem[TEMP_HEAD].b32.s1 = q;
    scanner_status = s;
    warning_index = w;
    def_ref = d;
}

void pseudo_start(void)
{
    unsigned char /*max_selector */ old_setting;
    str_number s;
    pool_pointer l, m;
    int32_t p, q, r;
    b16x4 w;
    int32_t nl, sz;

    scan_general_text();
    old_setting = selector;
    selector = SELECTOR_NEW_STRING ;
    token_show(TEMP_HEAD);
    selector = old_setting;
    flush_list(mem[TEMP_HEAD].b32.s1);
    {
        if (pool_ptr + 1 > pool_size)
            overflow("pool size", pool_size - init_pool_ptr);
    }
    s = make_string();
    str_pool[pool_ptr] = ' ' ;
    l = str_start[(s) - 65536L];
    nl = INTPAR(new_line_char);
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
        mem[q].b32.s1 = r;
        q = r;
        mem[q].b32.s0 = sz;
        while (sz > 2) {

            sz--;
            r++;
            w.s3 = str_pool[m];
            w.s2 = str_pool[m + 1];
            w.s1 = str_pool[m + 2];
            w.s0 = str_pool[m + 3];
            mem[r].b16 = w;
            m = m + 4;
        }
        w.s3 = ' ' ;
        w.s2 = ' ' ;
        w.s1 = ' ' ;
        w.s0 = ' ' ;
        if (l > m) {
            w.s3 = str_pool[m];
            if (l > m + 1) {
                w.s2 = str_pool[m + 1];
                if (l > m + 2) {
                    w.s1 = str_pool[m + 2];
                    if (l > m + 3)
                        w.s0 = str_pool[m + 3];
                }
            }
        }
        mem[r + 1].b16 = w;
        if (str_pool[l] == nl)
            l++;
    }
    mem[p].b32.s0 = mem[p].b32.s1;
    mem[p].b32.s1 = pseudo_files;
    pseudo_files = /*:1542 */ p;
    {
        str_ptr--;
        pool_ptr = str_start[str_ptr - TOO_BIG_CHAR];
    }
    begin_file_reading();
    line = 0;
    cur_input.limit = cur_input.start;
    cur_input.loc = cur_input.limit + 1;
    if (INTPAR(tracing_scan_tokens) > 0) {
        if (term_offset > max_print_line - 3)
            print_ln();
        else if ((term_offset > 0) || (file_offset > 0))
            print_char(' ');
        cur_input.name = 19;
        print_cstr("( ");
        open_parens++;
        ttstub_output_flush (rust_stdout);
    } else {

        cur_input.name = 18;
        cur_input.synctex_tag = 0;
    }
}

int32_t str_toks_cat(pool_pointer b, small_number cat)
{
    int32_t p;
    int32_t q;
    int32_t t;
    pool_pointer k;
    {
        if (pool_ptr + 1 > pool_size)
            overflow("pool size", pool_size - init_pool_ptr);
    }
    p = TEMP_HEAD;
    mem[p].b32.s1 = TEX_NULL;
    k = b;
    while (k < pool_ptr) {

        t = str_pool[k];
        if ((t == ' ' ) && (cat == 0))
            t = SPACE_TOKEN;
        else {

            if ((t >= 0xD800) && (t < 0xDC00) && (k + 1 < pool_ptr) && (str_pool[k + 1] >= 0xDC00)
                && (str_pool[k + 1] < 0xE000)) {
                k++;
                t = 65536L + (t - 0xD800) * 1024 + (str_pool[k] - 0xDC00);
            }
            if (cat == 0)
                t = OTHER_TOKEN + t;
            else if (cat == ACTIVE_CHAR)
                t = CS_TOKEN_FLAG + 1 + t;
            else
                t = MAX_CHAR_VAL * cat + t;
        }
        {
            {
                q = avail;
                if (q == TEX_NULL)
                    q = get_avail();
                else {

                    avail = mem[q].b32.s1;
                    mem[q].b32.s1 = TEX_NULL;
                }
            }
            mem[p].b32.s1 = q;
            mem[q].b32.s0 = t;
            p = q;
        }
        k++;
    }
    pool_ptr = b;
    return p;
}

int32_t str_toks(pool_pointer b)
{
    return str_toks_cat(b, 0);
}

int32_t the_toks(void)
{
    unsigned char /*max_selector */ old_setting;
    int32_t p, q, r;
    pool_pointer b;
    small_number c;
    if (odd(cur_chr)) {
        c = cur_chr;
        scan_general_text();
        if (c == 1)
            return cur_val;
        else {

            old_setting = selector;
            selector = SELECTOR_NEW_STRING ;
            b = pool_ptr;
            p = get_avail();
            mem[p].b32.s1 = mem[TEMP_HEAD].b32.s1;
            token_show(p);
            flush_list(p);
            selector = old_setting;
            return str_toks(b);
        }
    }
    get_x_token();
    scan_something_internal(TOK_VAL, false);
    if (cur_val_level >= IDENT_VAL) {   /*485: */
        p = TEMP_HEAD;
        mem[p].b32.s1 = TEX_NULL;
        if (cur_val_level == IDENT_VAL) {
            q = get_avail();
            mem[p].b32.s1 = q;
            mem[q].b32.s0 = CS_TOKEN_FLAG + cur_val;
            p = q;
        } else if (cur_val != TEX_NULL) {
            r = mem[cur_val].b32.s1;
            while (r != TEX_NULL) {

                {
                    {
                        q = avail;
                        if (q == TEX_NULL)
                            q = get_avail();
                        else {

                            avail = mem[q].b32.s1;
                            mem[q].b32.s1 = TEX_NULL;
                        }
                    }
                    mem[p].b32.s1 = q;
                    mem[q].b32.s0 = mem[r].b32.s0;
                    p = q;
                }
                r = LLIST_link(r);
            }
        }
        return p;
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
                print_cstr("pt");
            }
            break;
        case 2:
            {
                print_spec(cur_val, "pt");
                delete_glue_ref(cur_val);
            }
            break;
        case 3:
            {
                print_spec(cur_val, "mu");
                delete_glue_ref(cur_val);
            }
            break;
        }
        selector = old_setting;
        return str_toks(b);
    }
}

void ins_the_toks(void)
{
    mem[GARBAGE].b32.s1 = the_toks();
    begin_token_list(mem[TEMP_HEAD].b32.s1, INSERTED);
}


void
conv_toks(void)
{
    unsigned char old_setting;
    int32_t save_warning_index, save_def_ref;
    bool boolvar;
    str_number s;
    str_number u;
    small_number c;
    small_number save_scanner_status;
    pool_pointer b;
    int32_t fnt = 0, arg1 = 0, arg2 = 0;
    str_number font_name_str;
    small_number i;
    UTF16_code quote_char;
    small_number cat;
    UnicodeScalar saved_chr;
    int32_t p = TEX_NULL, q, j;

    cat = 0;
    c = cur_chr;

    switch (c) {
    case NUMBER_CODE:
    case ROMAN_NUMERAL_CODE:
        scan_int();
        break;

    case STRING_CODE:
    case MEANING_CODE:
        save_scanner_status = scanner_status;
        scanner_status = NORMAL;
        get_token();
        scanner_status = save_scanner_status;
        break;

    case FONT_NAME_CODE:
        scan_font_ident();
        break;

    case ETEX_REVISION_CODE:
        break;

    case EXPANDED_CODE:
        save_scanner_status = scanner_status;
        save_warning_index = warning_index;
        save_def_ref = def_ref;
        if (str_start[str_ptr - TOO_BIG_CHAR] < pool_ptr)
            u = make_string();
        else
            u = 0;
        scan_pdf_ext_toks();
        warning_index = save_warning_index;
        scanner_status = save_scanner_status;
        begin_token_list(mem[def_ref].b32.s1, INSERTED);
        def_ref = save_def_ref;
        if (u != 0)
            str_ptr--;
        return;

    case LEFT_MARGIN_KERN_CODE:
    case RIGHT_MARGIN_KERN_CODE:
        scan_register_num();

        if (cur_val < 256) {
            p = BOX_REG(cur_val);
        } else {
            find_sa_element(4, cur_val, false);
            if (cur_ptr == TEX_NULL)
                p = TEX_NULL;
            else
                p = mem[cur_ptr + 1].b32.s1;
        }

        if (p == TEX_NULL || NODE_type(p) != HLIST_NODE)
            pdf_error("marginkern", "a non-empty hbox expected");
        break;

    case PDF_CREATION_DATE_CODE:
        b = pool_ptr;
        getcreationdate();
        mem[GARBAGE].b32.s1 = str_toks(b);
        begin_token_list(mem[TEMP_HEAD].b32.s1, INSERTED);
        break;

    case PDF_FILE_MOD_DATE_CODE:
        save_scanner_status = scanner_status;
        save_warning_index = warning_index;
        save_def_ref = def_ref;
        if (str_start[str_ptr - TOO_BIG_CHAR] < pool_ptr)
            u = make_string();
        else
            u = 0;
        scan_pdf_ext_toks();

        if (selector == SELECTOR_NEW_STRING)
            pdf_error("tokens", "tokens_to_string() called while selector = new_string");

        old_setting = selector;
        selector = SELECTOR_NEW_STRING;
        show_token_list(mem[def_ref].b32.s1, TEX_NULL, pool_size - pool_ptr);
        selector = old_setting;
        s = make_string();
        delete_token_ref(def_ref);
        def_ref = save_def_ref;
        warning_index = save_warning_index;
        scanner_status = save_scanner_status;
        b = pool_ptr;
        getfilemoddate(s);  /* <= the difference-maker */
        mem[GARBAGE].b32.s1 = str_toks(b);

        if (s == str_ptr - 1) {
            str_ptr--;
            pool_ptr = str_start[str_ptr - TOO_BIG_CHAR];
        }

        begin_token_list(mem[TEMP_HEAD].b32.s1, INSERTED);
        if (u != 0)
            str_ptr--;
        return;

    case PDF_FILE_SIZE_CODE:
        save_scanner_status = scanner_status;
        save_warning_index = warning_index;
        save_def_ref = def_ref;
        if (str_start[str_ptr - TOO_BIG_CHAR] < pool_ptr)
            u = make_string();
        else
            u = 0;
        scan_pdf_ext_toks();

        if (selector == SELECTOR_NEW_STRING)
            pdf_error("tokens", "tokens_to_string() called while selector = new_string");

        old_setting = selector;
        selector = SELECTOR_NEW_STRING;
        show_token_list(mem[def_ref].b32.s1, TEX_NULL, pool_size - pool_ptr);
        selector = old_setting;
        s = make_string();
        delete_token_ref(def_ref);
        def_ref = save_def_ref;
        warning_index = save_warning_index;
        scanner_status = save_scanner_status;
        b = pool_ptr;
        getfilesize(s);  /* <= the difference-maker */
        mem[GARBAGE].b32.s1 = str_toks(b);

        if (s == str_ptr - 1) {
            str_ptr--;
            pool_ptr = str_start[str_ptr - TOO_BIG_CHAR];
        }

        begin_token_list(mem[TEMP_HEAD].b32.s1, INSERTED);
        if (u != 0)
            str_ptr--;
        return;

    case PDF_MDFIVE_SUM_CODE:
        save_scanner_status = scanner_status;
        save_warning_index = warning_index;
        save_def_ref = def_ref;

        if (str_start[str_ptr - TOO_BIG_CHAR] < pool_ptr)
            u = make_string();
        else
            u = 0;

        boolvar = scan_keyword("file");
        scan_pdf_ext_toks();

        if (selector == SELECTOR_NEW_STRING)
            pdf_error("tokens", "tokens_to_string() called while selector = new_string");

        old_setting = selector;
        selector = SELECTOR_NEW_STRING;
        show_token_list(mem[def_ref].b32.s1, TEX_NULL, pool_size - pool_ptr);
        selector = old_setting;
        s = make_string();
        delete_token_ref(def_ref);
        def_ref = save_def_ref;
        warning_index = save_warning_index;
        scanner_status = save_scanner_status;
        b = pool_ptr;
        getmd5sum(s, boolvar); /* <== the difference-maker */
        mem[GARBAGE].b32.s1 = str_toks(b);

        if (s == str_ptr - 1) {
            str_ptr--;
            pool_ptr = str_start[str_ptr - TOO_BIG_CHAR];
        }

        begin_token_list(mem[TEMP_HEAD].b32.s1, INSERTED);
        if (u != 0)
            str_ptr--;
        return;
        break;

    case PDF_FILE_DUMP_CODE:
        save_scanner_status = scanner_status;
        save_warning_index = warning_index;
        save_def_ref = def_ref;

        if (str_start[str_ptr - TOO_BIG_CHAR] < pool_ptr)
            u = make_string();
        else
            u = 0;

        cur_val = 0;

        if (scan_keyword("offset")) {
            scan_int();

            if (cur_val < 0) {
                error_here_with_diagnostic("Bad file offset");
                capture_to_diagnostic(NULL);
                help_ptr = 2;
                help_line[1] = "A file offset must be between 0 and 2^_31_-1,";
                help_line[0] = "I changed this one to zero.";
                int_error(cur_val);
                cur_val = 0;
            }
        }

        i = cur_val;
        cur_val = 0;

        if (scan_keyword("length")) {
            scan_int();

            if (cur_val < 0) {
                error_here_with_diagnostic("Bad dump length");
                capture_to_diagnostic(NULL);
                help_ptr = 2;
                help_line[1] = "A dump length must be between 0 and 2^_31_-1,";
                help_line[0] = "I changed this one to zero.";
                int_error(cur_val);
                cur_val = 0;
            }
        }

        j = cur_val;

        scan_pdf_ext_toks();

        if (selector == SELECTOR_NEW_STRING)
            pdf_error("tokens", "tokens_to_string() called while selector = new_string");

        old_setting = selector;
        selector = SELECTOR_NEW_STRING;
        show_token_list(mem[def_ref].b32.s1, TEX_NULL, pool_size - pool_ptr);
        selector = old_setting;
        s = make_string();
        delete_token_ref(def_ref);
        def_ref = save_def_ref;
        warning_index = save_warning_index;
        scanner_status = save_scanner_status;
        b = pool_ptr;
        getfiledump(s, i, j); /* <=== non-boilerplate */
        mem[GARBAGE].b32.s1 = str_toks(b);

        if (s == str_ptr - 1) {
            str_ptr--;
            pool_ptr = str_start[str_ptr - TOO_BIG_CHAR];
        }

        begin_token_list(mem[TEMP_HEAD].b32.s1, INSERTED);
        if (u != 0)
            str_ptr--;
        return;

    case PDF_STRCMP_CODE:
        save_scanner_status = scanner_status;
        save_warning_index = warning_index;
        save_def_ref = def_ref;
        if (str_start[str_ptr - TOO_BIG_CHAR] < pool_ptr)
            u = make_string();
        else
            u = 0;
        compare_strings();
        def_ref = save_def_ref;
        warning_index = save_warning_index;
        scanner_status = save_scanner_status;
        if (u != 0)
            str_ptr--;
        break;

    case XETEX_UCHAR_CODE:
        scan_usv_num();
        break;

    case XETEX_UCHARCAT_CODE:
        scan_usv_num();
        saved_chr = cur_val;
        scan_int();

        if (cur_val < LEFT_BRACE || cur_val > OTHER_CHAR || cur_val == OUT_PARAM || cur_val == IGNORE) {
            error_here_with_diagnostic("Invalid code (");
            print_int(cur_val);
            print_cstr("), should be in the ranges 1..4, 6..8, 10..12");
            capture_to_diagnostic(NULL);

            help_ptr = 1;
            help_line[0] = "I'm going to use 12 instead of that illegal code value.";
            error();
            cat = 12;
        } else {
            cat = cur_val;
        }

        cur_val = saved_chr;
        break;

    case XETEX_REVISION_CODE:
        break;

    case XETEX_VARIATION_NAME_CODE:
        scan_font_ident();
        fnt = cur_val;
        if (font_area[fnt] == AAT_FONT_FLAG) {
            scan_int();
            arg1 = cur_val;
            arg2 = 0;
        } else {
            not_aat_font_error(CONVERT, c, fnt);
        }
        break;

    case XETEX_FEATURE_NAME_CODE:
        scan_font_ident();
        fnt = cur_val;
        if (font_area[fnt] == AAT_FONT_FLAG ||
            (font_area[fnt] == OTGR_FONT_FLAG && usingGraphite(font_layout_engine[fnt]))) {
            scan_int();
            arg1 = cur_val;
            arg2 = 0;
        } else {
            not_aat_gr_font_error(CONVERT, c, fnt);
        }
        break;

    case XETEX_SELECTOR_NAME_CODE:
        scan_font_ident();
        fnt = cur_val;
        if (font_area[fnt] == AAT_FONT_FLAG
            || (font_area[fnt] == OTGR_FONT_FLAG && usingGraphite(font_layout_engine[fnt]))) {
            scan_int();
            arg1 = cur_val;
            scan_int();
            arg2 = cur_val;
        } else {
            not_aat_gr_font_error(CONVERT, c, fnt);
        }
        break;

    case XETEX_GLYPH_NAME_CODE:
        scan_font_ident();
        fnt = cur_val;
        if (font_area[fnt] == AAT_FONT_FLAG || font_area[fnt] == OTGR_FONT_FLAG) {
            scan_int();
            arg1 = cur_val;
        } else {
            not_native_font_error(CONVERT, c, fnt);
        }
        break;

    case JOB_NAME_CODE:
        if (job_name == 0)
            open_log_file();
        break;

    case UNIFORM_DEVIATE_CODE:
        scan_int();
        break;

    case NORMAL_DEVIATE_CODE:
        break;
    }

    old_setting = selector;
    selector = SELECTOR_NEW_STRING;
    b = pool_ptr;

    switch (c) {
    case NUMBER_CODE:
        print_int(cur_val);
        break;

    case ROMAN_NUMERAL_CODE:
        print_roman_int(cur_val);
        break;

    case STRING_CODE:
        if (cur_cs != 0)
            sprint_cs(cur_cs);
        else
            print_char(cur_chr);
        break;

    case MEANING_CODE:
        print_meaning();
        break;

    case FONT_NAME_CODE:
        font_name_str = font_name[cur_val];

        if (font_area[cur_val] == AAT_FONT_FLAG || font_area[cur_val] == OTGR_FONT_FLAG) {
            quote_char = '"' ;

            for (i = 0; i <= length(font_name_str) - 1; i++)
                if (str_pool[str_start[(font_name_str) - 65536L] + i] == '"' )
                    quote_char = '\'' ;

            print_char(quote_char);
            print(font_name_str);
            print_char(quote_char);
        } else {
            print(font_name_str);
        }

        if (font_size[cur_val] != font_dsize[cur_val]) {
            print_cstr(" at ");
            print_scaled(font_size[cur_val]);
            print_cstr("pt");
        }
        break;

    case ETEX_REVISION_CODE:
        print_cstr(".6");
        break;

    case LEFT_MARGIN_KERN_CODE:
        p = mem[p + 5].b32.s1;
        while (p != TEX_NULL &&
               ((p < hi_mem_min
                 && (NODE_type(p) == INS_NODE ||
                     NODE_type(p) == MARK_NODE ||
                     NODE_type(p) == ADJUST_NODE ||
                     NODE_type(p) == PENALTY_NODE ||
                     (NODE_type(p) == DISC_NODE &&
                      mem[p + 1].b32.s0 == TEX_NULL &&
                      mem[p + 1].b32.s1 == TEX_NULL &&
                      mem[p].b16.s0 == 0) ||
                     (NODE_type(p) == MATH_NODE &&
                      mem[p + 1].b32.s1 == 0) ||
                     (NODE_type(p) == KERN_NODE &&
                      (mem[p + 1].b32.s1 == 0 || mem[p].b16.s0 == NORMAL)) ||
                     (NODE_type(p) == GLUE_NODE &&
                      mem[p + 1].b32.s0 == 0) ||
                     (NODE_type(p) == HLIST_NODE &&
                      mem[p + 1].b32.s1 == 0 &&
                      mem[p + 3].b32.s1 == 0 &&
                      mem[p + 2].b32.s1 == 0 &&
                      mem[p + 5].b32.s1 == TEX_NULL)
                     )) ||
                (p < hi_mem_min && NODE_type(p) == GLUE_NODE && mem[p].b16.s0 == (GLUE_PAR__left_skip + 1))))
            p = LLIST_link(p);

        if (p != TEX_NULL && p < hi_mem_min && NODE_type(p) == MARGIN_KERN_NODE && mem[p].b16.s0 == 0)
            print_scaled(mem[p + 1].b32.s1);
        else
            print('0');
        print_cstr("pt");
        break;

    case RIGHT_MARGIN_KERN_CODE:
        q = mem[p + 5].b32.s1;
        p = prev_rightmost(q, TEX_NULL);
        while (p != TEX_NULL &&
               ((p < hi_mem_min &&
                 (NODE_type(p) == INS_NODE ||
                  NODE_type(p) == MARK_NODE ||
                  NODE_type(p) == ADJUST_NODE ||
                  NODE_type(p) == PENALTY_NODE ||
                  (NODE_type(p) == DISC_NODE &&
                   mem[p + 1].b32.s0 == TEX_NULL &&
                   mem[p + 1].b32.s1 == TEX_NULL &&
                   mem[p].b16.s0 == 0) ||
                  (NODE_type(p) == MATH_NODE &&
                   mem[p + 1].b32.s1 == 0) ||
                  (NODE_type(p) == KERN_NODE &&
                   (mem[p + 1].b32.s1 == 0 || mem[p].b16.s0 == NORMAL)) ||
                  (NODE_type(p) == GLUE_NODE &&
                   mem[p + 1].b32.s0 == 0) ||
                  (NODE_type(p) == HLIST_NODE &&
                   mem[p + 1].b32.s1 == 0 &&
                   mem[p + 3].b32.s1 == 0 &&
                   mem[p + 2].b32.s1 == 0 &&
                   mem[p + 5].b32.s1 == TEX_NULL)
                  )) ||
                (p < hi_mem_min && NODE_type(p) == GLUE_NODE && mem[p].b16.s0 == (GLUE_PAR__right_skip + 1))))
            p = prev_rightmost(q, p);

        if (p != TEX_NULL && p < hi_mem_min && NODE_type(p) == MARGIN_KERN_NODE && mem[p].b16.s0 == 1)
            print_scaled(mem[p + 1].b32.s1);
        else
            print('0');
        print_cstr("pt");
        break;

    case PDF_STRCMP_CODE:
        print_int(cur_val);
        break;

    case UNIFORM_DEVIATE_CODE:
        print_int(unif_rand(cur_val));
        break;

    case NORMAL_DEVIATE_CODE:
        print_int(norm_rand());
        break;

    case XETEX_UCHAR_CODE:
    case XETEX_UCHARCAT_CODE:
        print_char(cur_val);
        break;

    case XETEX_REVISION_CODE:
        print_cstr(".999992");
        break;

    case XETEX_VARIATION_NAME_CODE:
        if (font_area[fnt] == AAT_FONT_FLAG)
            aat_print_font_name(c, font_layout_engine[fnt], arg1, arg2);
        break;

    case XETEX_FEATURE_NAME_CODE:
    case XETEX_SELECTOR_NAME_CODE:
        if (font_area[fnt] == AAT_FONT_FLAG)
            aat_print_font_name(c, font_layout_engine[fnt], arg1, arg2);
        else if (font_area[fnt] == OTGR_FONT_FLAG && usingGraphite(font_layout_engine[fnt]))
            gr_print_font_name(c, font_layout_engine[fnt], arg1, arg2);
        break;

    case XETEX_GLYPH_NAME_CODE:
        if (font_area[fnt] == AAT_FONT_FLAG || font_area[fnt] == OTGR_FONT_FLAG)
            print_glyph_name(fnt, arg1);
        break;

    case JOB_NAME_CODE:
        print_file_name(job_name, 0, 0);
        break;
    }

    selector = old_setting;
    mem[GARBAGE].b32.s1 = str_toks_cat(b, cat);
    begin_token_list(mem[TEMP_HEAD].b32.s1, INSERTED);
}


int32_t scan_toks(bool macro_def, bool xpand)
{
    int32_t t;
    int32_t s;
    int32_t p;
    int32_t q;
    int32_t unbalance;
    int32_t hash_brace;
    if (macro_def)
        scanner_status = DEFINING;
    else
        scanner_status = ABSORBING;
    warning_index = cur_cs;
    def_ref = get_avail();
    mem[def_ref].b32.s0 = TEX_NULL;
    p = def_ref;
    hash_brace = 0;
    t = ZERO_TOKEN;
    if (macro_def) {            /*493: */
        while (true) {

            get_token();
            if (cur_tok < RIGHT_BRACE_LIMIT)
                goto done1;
            if (cur_cmd == MAC_PARAM) { /*495: */
                s = MATCH_TOKEN + cur_chr;
                get_token();
                if (cur_cmd == LEFT_BRACE) {
                    hash_brace = cur_tok;
                    {
                        q = get_avail();
                        mem[p].b32.s1 = q;
                        mem[q].b32.s0 = cur_tok;
                        p = q;
                    }
                    {
                        q = get_avail();
                        mem[p].b32.s1 = q;
                        mem[q].b32.s0 = END_MATCH_TOKEN;
                        p = q;
                    }
                    goto done;
                }
                if (t == (ZERO_TOKEN + 9)) {
                    error_here_with_diagnostic("You already have nine parameters");
                    capture_to_diagnostic(NULL);
                    {
                        help_ptr = 1;
                        help_line[0] = "I'm going to ignore the # sign you just used.";
                    }
                    error();
                } else {

                    t++;
                    if (cur_tok != t) {
                        error_here_with_diagnostic("Parameters must be numbered consecutively");
                        {
                            help_ptr = 2;
                            help_line[1] = "I've inserted the digit you should have used after the #.";
                            help_line[0] = "Type `1' to delete what you did use.";
                        }
                        back_error();
                    }
                    cur_tok = s;
                }
            }
            {
                q = get_avail();
                mem[p].b32.s1 = q;
                mem[q].b32.s0 = cur_tok;
                p = q;
            }
        }
    done1:
        {
            q = get_avail();
            mem[p].b32.s1 = q;
            mem[q].b32.s0 = END_MATCH_TOKEN;
            p = q;
        }
        if (cur_cmd == RIGHT_BRACE) {   /*494: */
            error_here_with_diagnostic("Missing { inserted");
            capture_to_diagnostic(NULL);

            align_state++;
            help_ptr = 2;
            help_line[1] = "Where was the left brace? You said something like `\\def\\a}',";
            help_line[0] = "which I'm going to interpret as `\\def\\a{}'.";
            error();
            goto found;
        }
    done:
        ;
    } else
        scan_left_brace();
    unbalance = 1;
    while (true) {

        if (xpand) {            /*497: */
            while (true) {

                get_next();
                if (cur_cmd >= CALL) {

                    if (mem[mem[cur_chr].b32.s1].b32.s0 == PROTECTED_TOKEN) {
                        cur_cmd = RELAX;
                        cur_chr = NO_EXPAND_FLAG;
                    }
                }
                if (cur_cmd <= MAX_COMMAND)
                    goto done2;
                if (cur_cmd != THE)
                    expand();
                else {

                    q = the_toks();
                    if (mem[TEMP_HEAD].b32.s1 != TEX_NULL) {
                        mem[p].b32.s1 = mem[TEMP_HEAD].b32.s1;
                        p = q;
                    }
                }
            }
        done2:
            x_token();
        } else
            get_token();
        if (cur_tok < RIGHT_BRACE_LIMIT) {

            if (cur_cmd < RIGHT_BRACE)
                unbalance++;
            else {

                unbalance--;
                if (unbalance == 0)
                    goto found;
            }
        } else if (cur_cmd == MAC_PARAM) {

            if (macro_def) {    /*498: */
                s = cur_tok;
                if (xpand)
                    get_x_token();
                else
                    get_token();
                if (cur_cmd != MAC_PARAM) {

                    if ((cur_tok <= ZERO_TOKEN) || (cur_tok > t)) {
                        error_here_with_diagnostic("Illegal parameter number in definition of ");
                        sprint_cs(warning_index);
                        capture_to_diagnostic(NULL);
                        {
                            help_ptr = 3;
                            help_line[2] = "You meant to type ## instead of #, right?";
                            help_line[1] = "Or maybe a } was forgotten somewhere earlier, and things";
                            help_line[0] = "are all screwed up? I'm going to assume that you meant ##.";
                        }
                        back_error();
                        cur_tok = s;
                    } else
                        cur_tok = (OUT_PARAM_TOKEN - 48) + cur_chr;
                }
            }
        }
        {
            q = get_avail();
            mem[p].b32.s1 = q;
            mem[q].b32.s0 = cur_tok;
            p = q;
        }
    }
found:
    scanner_status = NORMAL;
    if (hash_brace != 0) {
        q = get_avail();
        mem[p].b32.s1 = q;
        mem[q].b32.s0 = hash_brace;
        p = q;
    }
    return p;
}


void
read_toks(int32_t n, int32_t r, int32_t j)
{
    int32_t p;
    int32_t q;
    int32_t s;
    small_number m;

    scanner_status = DEFINING;
    warning_index = r;
    def_ref = get_avail();
    mem[def_ref].b32.s0 = TEX_NULL;
    p = def_ref;

    q = get_avail();
    mem[p].b32.s1 = q;
    mem[q].b32.s0 = END_MATCH_TOKEN;
    p = q;

    if (n < 0 || n > 15)
        m = 16;
    else
        m = n;

    s = align_state;
    align_state = 1000000L;

    do { /*502:*/
        begin_file_reading();
        cur_input.name = m + 1;

        if (read_open[m] == CLOSED) { /*503:*/
            _tt_abort ("terminal input forbidden");
        } else if (read_open[m] == JUST_OPEN) { /*504:*/
            if (input_line(read_file[m])) {
                read_open[m] = NORMAL;
            } else {
                u_close(read_file[m]);
                read_open[m] = CLOSED;
            }
        } else { /*505:*/
            if (!input_line(read_file[m])) {
                u_close(read_file[m]);
                read_open[m] = CLOSED;
                if (align_state != 1000000L) {
                    runaway();
                    error_here_with_diagnostic("File ended within ");
                    print_esc_cstr("read");
                    capture_to_diagnostic(NULL);
                    help_ptr = 1;
                    help_line[0] = "This \\read has unbalanced braces.";
                    align_state = 1000000L;
                    error();
                }
            }
        }

        cur_input.limit = last;

        if (INTPAR(end_line_char) < 0 || INTPAR(end_line_char) > 255)
            cur_input.limit--;
        else
            buffer[cur_input.limit] = INTPAR(end_line_char);

        first = cur_input.limit + 1;
        cur_input.loc = cur_input.start;
        cur_input.state = NEW_LINE;

        if (j == 1) {
            while (cur_input.loc <= cur_input.limit) {
                cur_chr = buffer[cur_input.loc];
                cur_input.loc++;
                if (cur_chr == ' ' )
                    cur_tok = SPACE_TOKEN;
                else
                    cur_tok = cur_chr + OTHER_TOKEN;

                q = get_avail();
                mem[p].b32.s1 = q;
                mem[q].b32.s0 = cur_tok;
                p = q;
            }
            goto done;
        }

        while (true) {
            get_token();
            if (cur_tok == 0)
                goto done;

            if (align_state < 1000000L) {
                do {
                    get_token();
                } while (cur_tok != 0);
                align_state = 1000000L;
                goto done;
            }

            q = get_avail();
            mem[p].b32.s1 = q;
            mem[q].b32.s0 = cur_tok;
            p = q;
        }

    done:
        end_file_reading();
    } while (align_state != 1000000L);

    cur_val = def_ref;
    scanner_status = NORMAL;
    align_state = s;
}


void pass_text(void)
{
    int32_t l;
    small_number save_scanner_status;

    save_scanner_status = scanner_status;
    scanner_status = SKIPPING;
    l = 0;
    skip_line = line;

    while (true) {

        get_next();
        if (cur_cmd == FI_OR_ELSE) {
            if (l == 0)
                goto done;
            if (cur_chr == FI_CODE)
                l--;
        } else if (cur_cmd == IF_TEST)
            l++;
    }
done:
    scanner_status = save_scanner_status;
    if (INTPAR(tracing_ifs) > 0)
        show_cur_cmd_chr();
}

void change_if_limit(small_number l, int32_t p)
{
    int32_t q;
    if (p == cond_ptr)
        if_limit = l;
    else {

        q = cond_ptr;
        while (true) {

            if (q == TEX_NULL)
                confusion("if");
            if (mem[q].b32.s1 == p) {
                mem[q].b16.s1 = l;
                return;
            }
            q = LLIST_link(q);
        }
    }
}


void
conditional(void)
{
    bool b = false;
    bool e;
    unsigned char /*">" */ r;
    int32_t m, n;
    int32_t p, q;
    small_number save_scanner_status;
    int32_t save_cond_ptr;
    small_number this_if;
    bool is_unless;

    if (INTPAR(tracing_ifs) > 0) {
        if (INTPAR(tracing_commands) <= 1)
            show_cur_cmd_chr();
    }

    p = get_node(IF_NODE_SIZE);
    mem[p].b32.s1 = cond_ptr;
    mem[p].b16.s1 = if_limit;
    mem[p].b16.s0 = cur_if;
    mem[p + 1].b32.s1 = if_line;
    cond_ptr = p;
    cur_if = cur_chr;
    if_limit = IF_CODE;
    if_line = line;

    save_cond_ptr = cond_ptr;
    is_unless = (cur_chr >= UNLESS_CODE);
    this_if = cur_chr % UNLESS_CODE;

    switch (this_if) {
    case IF_CHAR_CODE:
    case IF_CAT_CODE:
        get_x_token();

        if (cur_cmd == RELAX) {
            if (cur_chr == NO_EXPAND_FLAG) {
                cur_cmd = ACTIVE_CHAR;
                cur_chr = cur_tok - (CS_TOKEN_FLAG + ACTIVE_BASE);
            }
        }

        if (cur_cmd > ACTIVE_CHAR || cur_chr > BIGGEST_USV) {
            m = RELAX;
            n = TOO_BIG_USV;
        } else {
            m = cur_cmd;
            n = cur_chr;
        }

        get_x_token();

        if (cur_cmd == RELAX) {
            if (cur_chr == NO_EXPAND_FLAG) {
                cur_cmd = ACTIVE_CHAR;
                cur_chr = cur_tok - (CS_TOKEN_FLAG + ACTIVE_BASE);
            }
        }

        if (cur_cmd > ACTIVE_CHAR || cur_chr > BIGGEST_USV) {
            cur_cmd = RELAX;
            cur_chr = TOO_BIG_USV;
        }

        if (this_if == IF_CHAR_CODE)
            b = (n == cur_chr);
        else
            b = (m == cur_cmd);
        break;

    case IF_INT_CODE:
    case IF_DIM_CODE:
        if (this_if == IF_INT_CODE)
            scan_int();
        else
            scan_dimen(false, false, false);

        n = cur_val;

        do {
            get_x_token();
        } while (cur_cmd == SPACER);

        if (cur_tok >= OTHER_TOKEN + 60 && cur_tok <= OTHER_TOKEN + 62) {
            r = cur_tok - OTHER_TOKEN;
        } else {
            error_here_with_diagnostic("Missing = inserted for ");
            print_cmd_chr(IF_TEST, this_if);
            capture_to_diagnostic(NULL);

            help_ptr = 1;
            help_line[0] = "I was expecting to see `<', `=', or `>'. Didn't.";
            back_error();
            r = '=';
        }

        if (this_if == IF_INT_CODE)
            scan_int();
        else
            scan_dimen(false, false, false);

        switch (r) {
        case 60: /*"<"*/
            b = (n < cur_val);
            break;
        case 61: /*"="*/
            b = (n == cur_val);
            break;
        case 62: /*">"*/
            b = (n > cur_val);
            break;
        }
        break;

    case IF_ODD_CODE:
        scan_int();
        b = odd(cur_val);
        break;

    case IF_VMODE_CODE:
        b = (abs(cur_list.mode) == VMODE);
        break;

    case IF_HMODE_CODE:
        b = (abs(cur_list.mode) == HMODE);
        break;

    case IF_MMODE_CODE:
        b = (abs(cur_list.mode) == MMODE);
        break;

    case IF_INNER_CODE:
        b = (cur_list.mode < 0);
        break;

    case IF_VOID_CODE:
    case IF_HBOX_CODE:
    case IF_VBOX_CODE:
        scan_register_num();
        if (cur_val < 256) {
            p = BOX_REG(cur_val);
        } else {
            find_sa_element(4, cur_val, false);
            if (cur_ptr == TEX_NULL)
                p = TEX_NULL;
            else
                p = mem[cur_ptr + 1].b32.s1;
        }

        if (this_if == IF_VOID_CODE)
            b = (p == TEX_NULL);
        else if (p == TEX_NULL)
            b = false;
        else if (this_if == IF_HBOX_CODE)
            b = (NODE_type(p) == HLIST_NODE);
        else
            b = (NODE_type(p) == VLIST_NODE);
        break;

    case IFX_CODE:
        save_scanner_status = scanner_status;
        scanner_status = NORMAL;
        get_next();
        n = cur_cs;
        p = cur_cmd;
        q = cur_chr;
        get_next();

        if (cur_cmd != p) {
            b = false;
        } else if (cur_cmd < CALL) {
            b = (cur_chr == q);
        } else { /*527:*/
            p = mem[cur_chr].b32.s1;
            q = mem[eqtb[n].b32.s1].b32.s1;
            if (p == q) {
                b = true;
            } else {
                while (p != TEX_NULL && q != TEX_NULL) {
                    if (mem[p].b32.s0 != mem[q].b32.s0) {
                        p = TEX_NULL;
                    } else {
                        p = LLIST_link(p);
                        q = LLIST_link(q);
                    }
                }

                b = (p == TEX_NULL && q == TEX_NULL);
            }
        }

        scanner_status = save_scanner_status;
        break;

    case IF_EOF_CODE:
        scan_four_bit_int_or_18();
        if (cur_val == 18)
            b = 1; /* !shellenabledp */
        else
            b = (read_open[cur_val] == CLOSED);
        break;

    case IF_TRUE_CODE:
        b = true;
        break;

    case IF_FALSE_CODE:
        b = false;
        break;

    case IF_DEF_CODE:
        save_scanner_status = scanner_status;
        scanner_status = NORMAL;
        get_next();
        b = (cur_cmd != UNDEFINED_CS);
        scanner_status = save_scanner_status;
        break;

    case IF_CS_CODE:
        n = get_avail();
        p = n;
        e = is_in_csname;
        is_in_csname = true;

        do {
            get_x_token();
            if (cur_cs == 0) {
                q = get_avail();
                mem[p].b32.s1 = q;
                mem[q].b32.s0 = cur_tok;
                p = q;
            }
        } while (cur_cs == 0);

        if (cur_cmd != END_CS_NAME) { /*391:*/
            error_here_with_diagnostic("Missing ");
            print_esc_cstr("endcsname");
            print_cstr(" inserted");
            capture_to_diagnostic(NULL);

            help_ptr = 2;
            help_line[1] = "The control sequence marked <to be read again> should";
            help_line[0] = "not appear between \\csname and \\endcsname.";
            back_error();
        }

        m = first;
        p = mem[n].b32.s1;

        while (p != TEX_NULL) {
            if (m >= max_buf_stack) {
                max_buf_stack = m + 1;
                if (max_buf_stack == buf_size)
                    overflow("buffer size", buf_size);
            }

            buffer[m] = mem[p].b32.s0 % MAX_CHAR_VAL;
            m++;
            p = LLIST_link(p);
        }

        if (m == first)
            cur_cs = NULL_CS;
        else if (m == first + 1)
            cur_cs = SINGLE_BASE + buffer[first];
        else
            cur_cs = id_lookup(first, m - first); /*:1556*/

        flush_list(n);
        b = (eqtb[cur_cs].b16.s1 != UNDEFINED_CS);
        is_in_csname = e;
        break;

    case IF_IN_CSNAME_CODE:
        b = is_in_csname;
        break;

    case IF_FONT_CHAR_CODE:
        scan_font_ident();
        n = cur_val;
        scan_usv_num();

        if (font_area[n] == AAT_FONT_FLAG || font_area[n] == OTGR_FONT_FLAG) {
            b = (map_char_to_glyph(n, cur_val) > 0);
        } else {
            if (font_bc[n] <= cur_val && font_ec[n] >= cur_val)
                b = (FONT_CHARACTER_INFO(n, effective_char(true, n, cur_val)).s3 > 0);
            else
                b = false;
        }
        break;

    case IF_CASE_CODE:
        scan_int();
        n = cur_val;

        if (INTPAR(tracing_commands) > 1) {
            begin_diagnostic();
            diagnostic_begin_capture_warning_here();
            print_cstr("{case ");
            print_int(n);
            print_char('}');
            capture_to_diagnostic(NULL);
            end_diagnostic(false);
        }

        while (n != 0) {
            pass_text();

            if (cond_ptr == save_cond_ptr) {
                if (cur_chr == OR_CODE)
                    n--;
                else
                    goto common_ending;
            } else if (cur_chr == FI_CODE) { /*515:*/
                if (if_stack[in_open] == cond_ptr)
                    if_warning();
                p = cond_ptr;
                if_line = mem[p + 1].b32.s1;
                cur_if = mem[p].b16.s0;
                if_limit = mem[p].b16.s1;
                cond_ptr = mem[p].b32.s1;
                free_node(p, IF_NODE_SIZE);
            }
        }
        change_if_limit(OR_CODE, save_cond_ptr);
        return;
        break;

    case IF_PRIMITIVE_CODE:
        save_scanner_status = scanner_status;
        scanner_status = NORMAL;
        get_next();
        scanner_status = save_scanner_status;
        if (cur_cs < HASH_BASE)
            m = prim_lookup(cur_cs - SINGLE_BASE);
        else
            m = prim_lookup(hash[cur_cs].s1);
        b = (cur_cmd != UNDEFINED_CS
             && m != UNDEFINED_PRIMITIVE
             && cur_cmd == eqtb[PRIM_EQTB_BASE + m].b16.s1
             && cur_chr == eqtb[PRIM_EQTB_BASE + m].b32.s1);
        break;
    }

    if (is_unless)
        b = !b;

    if (INTPAR(tracing_commands) > 1) { /*521:*/
        begin_diagnostic();
        diagnostic_begin_capture_warning_here();
        if (b)
            print_cstr("{true}");
        else
            print_cstr("{false}");
        capture_to_diagnostic(NULL);
        end_diagnostic(false);
    }

    if (b) {
        change_if_limit(ELSE_CODE, save_cond_ptr);
        return;
    }

    while (true) {
        pass_text();

        if (cond_ptr == save_cond_ptr) {
            if (cur_chr != OR_CODE)
                goto common_ending;

            error_here_with_diagnostic("Extra ");
            print_esc_cstr("or");
            capture_to_diagnostic(NULL);

            help_ptr = 1;
            help_line[0] = "I'm ignoring this; it doesn't match any \\if.";
            error();
        } else if (cur_chr == FI_CODE) { /*515:*/
            if (if_stack[in_open] == cond_ptr)
                if_warning();
            p = cond_ptr;
            if_line = mem[p + 1].b32.s1;
            cur_if = mem[p].b16.s0;
            if_limit = mem[p].b16.s1;
            cond_ptr = mem[p].b32.s1;
            free_node(p, IF_NODE_SIZE);
        }
    }

common_ending:
    if (cur_chr == FI_CODE) { /*515:*/
        if (if_stack[in_open] == cond_ptr)
            if_warning();
        p = cond_ptr;
        if_line = mem[p + 1].b32.s1;
        cur_if = mem[p].b16.s0;
        if_limit = mem[p].b16.s1;
        cond_ptr = mem[p].b32.s1;
        free_node(p, IF_NODE_SIZE);
    } else {
        if_limit = FI_CODE;
    }
}


void
begin_name(void)
{
    area_delimiter = 0;
    ext_delimiter = 0;
    quoted_filename = false;
    file_name_quote_char = 0;
}


bool
more_name(UTF16_code c)
{
    if (stop_at_space && file_name_quote_char == 0 && c == ' ' )
        return false;

    if (stop_at_space && file_name_quote_char != 0 && c == file_name_quote_char) {
        file_name_quote_char = 0;
        return true;
    }

    if (stop_at_space && file_name_quote_char == 0 && (c == '"'  || c == '\'' )) {
        file_name_quote_char = c;
        quoted_filename = true;
        return true;
    }

    if (pool_ptr + 1 > pool_size)
        overflow("pool size", pool_size - init_pool_ptr);

    str_pool[pool_ptr++] = c;

    if (IS_DIR_SEP(c)) {
        area_delimiter = cur_length();
        ext_delimiter = 0;
    } else if (c == '.' ) {
        ext_delimiter = cur_length();
    }

    return true;
}


void
end_name(void)
{
    str_number temp_str;
    pool_pointer j;

    if (str_ptr + 3 > max_strings)
        overflow("number of strings", max_strings - init_str_ptr);

    /* area_delimiter is the length from the start of the filename to the
     * directory seperator "/", which we use to construct the stringpool
     * string `cur_area`. If there was already a string in the stringpool for
     * the area, reuse it. */

    if (area_delimiter == 0) {
        cur_area = EMPTY_STRING;
    } else {
        cur_area = str_ptr;
        str_start[(str_ptr + 1) - 65536L] = str_start[str_ptr - TOO_BIG_CHAR] + area_delimiter;
        str_ptr++;
        temp_str = search_string(cur_area);

        if (temp_str > 0) {
            cur_area = temp_str;
            str_ptr--;

            for (j = str_start[(str_ptr + 1) - 65536L]; j <= pool_ptr - 1; j++)
                str_pool[j - area_delimiter] = str_pool[j];

            pool_ptr = pool_ptr - area_delimiter;
        }
    }

    /* ext_delimiter is the length from the start of the filename to the
     * extension '.' delimiter, which we use to construct the stringpool
     * strings `cur_ext` and `cur_name`. */

    if (ext_delimiter == 0) {
        cur_ext = EMPTY_STRING;
        cur_name = slow_make_string();
    } else {
        cur_name = str_ptr;
        str_start[(str_ptr + 1) - 65536L] = str_start[str_ptr - TOO_BIG_CHAR] + ext_delimiter - area_delimiter - 1;
        str_ptr++;

        cur_ext = make_string();
        str_ptr--;
        temp_str = search_string(cur_name);

        if (temp_str > 0) {
            cur_name = temp_str;
            str_ptr--;

            for (j = str_start[(str_ptr + 1) - 65536L]; j <= pool_ptr - 1; j++)
                str_pool[j - ext_delimiter + area_delimiter + 1] = str_pool[j];

            pool_ptr = pool_ptr - ext_delimiter + area_delimiter + 1;
        }

        cur_ext = slow_make_string();
    }
}


void
pack_file_name(str_number n, str_number a, str_number e)
{
    // Note that we populate the buffer in an order different than how the
    // arguments are passed to this function!
    char* work_buffer = xmalloc_array(UTF8_code, (length(a) + length(n) + length(e)) * 3 + 1);
    work_buffer[0] = '\0';

    char* a_utf8 = gettexstring(a);
    strcat(work_buffer, a_utf8);
    free(a_utf8);

    char* n_utf8 = gettexstring(n);
    strcat(work_buffer, n_utf8);
    free(n_utf8);

    char* e_utf8 = gettexstring(e);
    strcat(work_buffer, e_utf8);
    free(e_utf8);

    name_length = strlen(work_buffer);

    free(name_of_file);
    name_of_file = xmalloc_array(char, name_length + 1);
    strcpy(name_of_file, work_buffer);
    free(work_buffer);
}


str_number
make_name_string(void)
{
    int32_t k;
    pool_pointer save_area_delimiter, save_ext_delimiter;
    bool save_name_in_progress, save_stop_at_space;

    if (pool_ptr + name_length > pool_size || str_ptr == max_strings || cur_length() > 0)
        return '?';

    make_utf16_name();

    for (k = 0; k < name_length16; k++)
        str_pool[pool_ptr++] = name_of_file16[k];


    str_number Result = make_string();

    save_area_delimiter = area_delimiter;
    save_ext_delimiter = ext_delimiter;
    save_name_in_progress = name_in_progress;
    save_stop_at_space = stop_at_space;
    name_in_progress = true;
    begin_name();
    stop_at_space = false;
    k = 0;

    while (k < name_length16 && more_name(name_of_file16[k]))
        k++;

    stop_at_space = save_stop_at_space;
    end_name();
    name_in_progress = save_name_in_progress;
    area_delimiter = save_area_delimiter;
    ext_delimiter = save_ext_delimiter;

    return Result;
}


static void
scan_file_name_braced(void)
{
    small_number save_scanner_status;
    int32_t save_def_ref, save_cur_cs;
    str_number s;
    int32_t i;
    bool save_stop_at_space;

    save_scanner_status = scanner_status;
    save_def_ref = def_ref;
    save_cur_cs = cur_cs;
    cur_cs = warning_index;
    scan_toks(false, true);

    old_setting = selector;
    selector = SELECTOR_NEW_STRING;
    show_token_list(mem[def_ref].b32.s1, TEX_NULL, pool_size - pool_ptr);
    selector = old_setting;
    s = make_string();
    delete_token_ref(def_ref);
    def_ref = save_def_ref;
    cur_cs = save_cur_cs;
    scanner_status = save_scanner_status;
    save_stop_at_space = stop_at_space;

    begin_name();

    for (i = str_start[s - TOO_BIG_CHAR]; i < str_start[s + 1 - TOO_BIG_CHAR]; i++)
        more_name(str_pool[i]);

    stop_at_space = save_stop_at_space;
}


void
scan_file_name(void)
{
    int32_t save_warning_index;
    save_warning_index = warning_index;
    warning_index = cur_cs;

    do {
        get_x_token();
    } while (cur_cmd == SPACER || cur_cmd == RELAX);

    back_input();

    if (cur_cmd == LEFT_BRACE) {
        scan_file_name_braced();
    } else {
        name_in_progress = true;
        begin_name();

        do {
            get_x_token();
        } while(cur_cmd == SPACER);

        while (true) {
            if (cur_cmd > OTHER_CHAR || cur_chr > BIGGEST_CHAR) {
                back_input();
                break;
            }

            if (!more_name(cur_chr))
                break;

            get_x_token();
        }
    }

    end_name();
    name_in_progress = false;
    warning_index = save_warning_index;
}


void pack_job_name(const char* s)
{
    cur_area = EMPTY_STRING;
    cur_ext = maketexstring(s);
    cur_name = job_name;
    pack_file_name(cur_name, cur_area, cur_ext);
}


void
open_log_file(void)
{
    unsigned char old_setting;
    int32_t k;
    int32_t l;

    old_setting = selector;
    if (job_name == 0)
        job_name = maketexstring("texput");

    pack_job_name(".log");

    log_file = ttstub_output_open (name_of_file, 0);
    if (log_file == NULL)
        _tt_abort ("cannot open log file output \"%s\"", name_of_file);

    texmf_log_name = make_name_string();
    selector = SELECTOR_LOG_ONLY;
    log_opened = true;

    input_stack[input_ptr] = cur_input;

    /* Here we catch the log file up with anything that has already been
     * printed. The eqtb reference is end_line_char. */

    print_nl_cstr("**");
    l = input_stack[0].limit;
    if (buffer[l] == INTPAR(end_line_char))
        l--;

    for (k = 1; k <= l; k++)
        print(buffer[k]);

    print_ln();
    selector = old_setting + 2;
}


void
start_input(const char *primary_input_name)
{
    ttbc_file_format format = TTBC_FILE_FORMAT_TEX;
    str_number temp_str;

    if (primary_input_name != NULL) {
        /* If this is the case, we're opening the primary input file, and the
         * name that we should use to refer to it has been handed directly to
         * us. We emulate the hacks used below to fill in cur_name, etc., from
         * a UTF-8 C string. It looks like the `cur_{name,area,ext}` strings
         * are hardly used so it'd be nice to get rid of them someday. */

        format = TTBC_FILE_FORMAT_TECTONIC_PRIMARY;

        name_in_progress = true;
        begin_name();
        stop_at_space = false;



        const unsigned char *cp = (const unsigned char *) primary_input_name;

        if (pool_ptr + strlen(primary_input_name) * 2 >= pool_size)
            _tt_abort ("string pool overflow [%i bytes]", (int) pool_size);

        UInt32 rval;
        while ((rval = *(cp++)) != 0) {
            UInt16 extraBytes = bytesFromUTF8[rval];
            switch (extraBytes) { /* note: code falls through cases! */
            case 5: rval <<= 6; if (*cp) rval += *(cp++);
            case 4: rval <<= 6; if (*cp) rval += *(cp++);
            case 3: rval <<= 6; if (*cp) rval += *(cp++);
            case 2: rval <<= 6; if (*cp) rval += *(cp++);
            case 1: rval <<= 6; if (*cp) rval += *(cp++);
            case 0: ;
            };
            rval -= offsetsFromUTF8[extraBytes];
            if (rval > 0xffff) {
                rval -= 0x10000;
                str_pool[pool_ptr++] = 0xd800 + rval / 0x0400;
                str_pool[pool_ptr++] = 0xdc00 + rval % 0x0400;
            } else {
                str_pool[pool_ptr++] = rval;
            }

            if (IS_DIR_SEP(rval)) {
                area_delimiter = cur_length();
                ext_delimiter = 0;
            } else if (rval == '.' ) {
                ext_delimiter = cur_length();
            }
        }

        stop_at_space = true;
        end_name();
        name_in_progress = false;
    } else {
        /* Scan in the file name from the current token stream. The file name to
         * input is saved as the stringpool strings `cur_{name,area,ext}` and the
         * UTF-8 string `name_of_file`. */
        scan_file_name();
    }

    pack_file_name(cur_name, cur_area, cur_ext);

    /* Open up the new file to be read. The name of the file to be read comes
     * from `name_of_file`. */

    begin_file_reading();

    if (!u_open_in(&input_file[cur_input.index], format, "rb",
                  INTPAR(xetex_default_input_mode), INTPAR(xetex_default_input_encoding)))
        _tt_abort ("failed to open input file \"%s\"", name_of_file);

    /* Now re-encode `name_of_file` into the UTF-16 variable `name_of_file16`,
     * and use that to recompute `cur_{name,area,ext}`. */

    make_utf16_name();
    name_in_progress = true;
    begin_name();
    stop_at_space = false;
    int k = 0;
    while (k < name_length16 && more_name(name_of_file16[k]))
        k++;
    stop_at_space = true;
    end_name();
    name_in_progress = false;

    /* Now generate a stringpool string corresponding to the full path of the
     * input file. This calls make_utf16_name() again and reruns through the
     * {begin,more,end}_name() trifecta to re-re-compute
     * `cur_{name,area,ext}`. */

    cur_input.name = make_name_string();
    source_filename_stack[in_open] = cur_input.name;

    /* *This* variant is a TeX string made out of `name_of_input_file`. */

    full_source_filename_stack[in_open] = maketexstring(name_of_input_file);
    if (cur_input.name == str_ptr - 1) {
        temp_str = search_string(cur_input.name);
        if (temp_str > 0) {
            cur_input.name = temp_str;
            str_ptr--;
            pool_ptr = str_start[str_ptr - TOO_BIG_CHAR];
        }
    }

    /* Finally we start really doing stuff with the newly-opened file. */

    if (job_name == 0) {
        job_name = cur_name;
        open_log_file();
    }

    if (term_offset + length(full_source_filename_stack[in_open]) > max_print_line - 2)
        print_ln();
    else if (term_offset > 0 || file_offset > 0)
        print_char(' ');
    print_char('(');
    open_parens++;
    print(full_source_filename_stack[in_open]);
    ttstub_output_flush(rust_stdout);

    cur_input.state = NEW_LINE;

    synctex_start_input();

    line = 1;
    input_line(input_file[cur_input.index]);
    cur_input.limit = last;

    if (INTPAR(end_line_char) < 0 || INTPAR(end_line_char) > 255)
        cur_input.limit--;
    else
        buffer[cur_input.limit] = INTPAR(end_line_char);

    first = cur_input.limit + 1;
    cur_input.loc = cur_input.start;
}


b16x4
effective_char_info(internal_font_number f, uint16_t c)
{
    if (!xtx_ligature_present && font_mapping[f] != NULL)
        c = apply_tfm_font_mapping(font_mapping[f], c);

    xtx_ligature_present = false;
    return FONT_CHARACTER_INFO(f, c);
}


void char_warning(internal_font_number f, int32_t c)
{
    int32_t old_setting;

    if (INTPAR(tracing_lost_chars) > 0) {
        old_setting = INTPAR(tracing_online);
        if (INTPAR(tracing_lost_chars) > 1)
            INTPAR(tracing_online) = 1;

        begin_diagnostic();
        diagnostic_begin_capture_warning_here();
        print_nl_cstr("Missing character: There is no ");
        if (c < 65536L)
            print(c);
        else
            print_char(c);
        print_cstr(" in font ");
        print(font_name[f]);
        print_char('!');
        capture_to_diagnostic(NULL);
        end_diagnostic(false);

        INTPAR(tracing_online) = old_setting;
    }

    {
        char *fn = gettexstring(font_name[f]);
        char *chr = NULL;
        selector_t prev_selector = selector;
        int s;

        selector = SELECTOR_NEW_STRING;
        if (c < 0x10000)
            print(c);
        else
            print_char(c);
        selector = prev_selector;
        s = make_string();
        chr = gettexstring(s);
        str_ptr--; /* this is the "flush_string" macro which discards the most recent string */
        pool_ptr = str_start[str_ptr - 0x10000];

        ttstub_issue_warning("could not represent character \"%s\" (0x%" PRIx32 ") in font \"%s\"", chr, c, fn);

        free(fn);
        free(chr);

        if (!gave_char_warning_help) {
            ttstub_issue_warning("  you may need to load the `fontspec` package and use (e.g.) \\setmainfont to");
            ttstub_issue_warning("  choose a different font that covers the unrepresentable character(s)");
            gave_char_warning_help = true;
        }
    }
}


int32_t
new_native_word_node(internal_font_number f, int32_t n)
{
    int32_t l;
    int32_t q;

    l = NATIVE_NODE_SIZE + (n * sizeof(UTF16_code) + sizeof(memory_word) - 1) / sizeof(memory_word);
    q = get_node(l);
    NODE_type(q) = WHATSIT_NODE;

    if (INTPAR(xetex_generate_actual_text) > 0)
        NODE_subtype(q) = NATIVE_WORD_NODE_AT;
    else
        NODE_subtype(q) = NATIVE_WORD_NODE;

    NATIVE_NODE_size(q) = l;
    NATIVE_NODE_font(q) = f;
    NATIVE_NODE_length(q) = n;
    NATIVE_NODE_glyph_count(q) = 0;
    NATIVE_NODE_glyph_info_ptr(q) = NULL;
    return q;
}


int32_t
new_native_character(internal_font_number f, UnicodeScalar c)
{
    int32_t p;
    int32_t i, len;

    if (font_mapping[f] != NULL) {
        if (c > 65535L) {
            if (pool_ptr + 2 > pool_size)
                overflow("pool size", pool_size - init_pool_ptr);

            str_pool[pool_ptr] = (c - 65536L) / 1024 + 0xD800;
            pool_ptr++;
            str_pool[pool_ptr] = (c - 65536L) % 1024 + 0xDC00;
            pool_ptr++;
        } else {
            if (pool_ptr + 1 > pool_size)
                overflow("pool size", pool_size - init_pool_ptr);

            str_pool[pool_ptr] = c;
            pool_ptr++;
        }

        len = apply_mapping(
            font_mapping[f],
            &str_pool[str_start[str_ptr - TOO_BIG_CHAR]],
            cur_length()
        );
        pool_ptr = str_start[str_ptr - TOO_BIG_CHAR];

        i = 0;

        while (i < len) {
            if (mapped_text[i] >= 0xD800 && mapped_text[i] < 0xDC00) {
                c = (mapped_text[i] - 0xD800) * 1024 + mapped_text[i + 1] + 9216;
                if (map_char_to_glyph(f, c) == 0)
                    char_warning(f, c);

                i += 2;
            } else {
                if (map_char_to_glyph(f, mapped_text[i]) == 0)
                    char_warning(f, mapped_text[i]);

                i += 1;
            }
        }

        p = new_native_word_node(f, len);

        for (i = 0; i <= len - 1; i++)
            NATIVE_NODE_text(p)[i] = mapped_text[i];
    } else {
        if (INTPAR(tracing_lost_chars) > 0)
            if (map_char_to_glyph(f, c) == 0)
                char_warning(f, c);

        p = get_node(NATIVE_NODE_SIZE + 1);
        NODE_type(p) = WHATSIT_NODE;
        NODE_subtype(p) = NATIVE_WORD_NODE;
        NATIVE_NODE_size(p) = NATIVE_NODE_SIZE + 1;
        NATIVE_NODE_glyph_count(p) = 0;
        NATIVE_NODE_glyph_info_ptr(p) = NULL;
        NATIVE_NODE_font(p) = f;

        if (c > 65535L) {
            NATIVE_NODE_length(p) = 2;
            NATIVE_NODE_text(p)[0] = (c - 65536L) / 1024 + 0xD800;
            NATIVE_NODE_text(p)[1] = (c - 65536L) % 1024 + 0xDC00;
        } else {
            NATIVE_NODE_length(p) = 1;
            NATIVE_NODE_text(p)[0] = c;
        }
    }

    set_native_metrics(p, INTPAR(xetex_use_glyph_metrics) > 0);
    return p;
}


void font_feature_warning(const void *featureNameP, int32_t featLen, const void *settingNameP, int32_t setLen)
{
    begin_diagnostic();
    diagnostic_begin_capture_warning_here();
    print_nl_cstr("Unknown ");
    if (setLen > 0) {
        print_cstr("selector `");
        print_utf8_str(settingNameP, setLen);
        print_cstr("' for ");
    }
    print_cstr("feature `");
    print_utf8_str(featureNameP, featLen);
    print_cstr("' in font `");
    for (int32_t i = 0; name_of_file[i] != 0; i++)
        print_raw_char(name_of_file[i], true);
    print_cstr("'.");
    capture_to_diagnostic(NULL);
    end_diagnostic(false);
}

void font_mapping_warning(const void *mappingNameP, int32_t mappingNameLen, int32_t warningType)
{
    begin_diagnostic();
    diagnostic_begin_capture_warning_here();
    if (warningType == 0)
        print_nl_cstr("Loaded mapping `");
    else
        print_nl_cstr("Font mapping `");
    print_utf8_str(mappingNameP, mappingNameLen);
    print_cstr("' for font `");

    for (int32_t i = 0; name_of_file[i] != 0; i++)
        print_raw_char(name_of_file[i], true);

    switch (warningType) {
    case 1:
        print_cstr("' not found.");
        break;
    case 2:
        {
            print_cstr("' not usable;");
            print_nl_cstr("bad mapping file or incorrect mapping type.");
        }
        break;
    default:
        print_cstr("'.");
        break;
    }
    capture_to_diagnostic(NULL);
    end_diagnostic(false);
}

void graphite_warning(void)
{
    begin_diagnostic();
    diagnostic_begin_capture_warning_here();
    print_nl_cstr("Font `");

    for (int32_t i = 0; name_of_file[i] != 0; i++)
        print_raw_char(name_of_file[i], true);

    print_cstr("' does not support Graphite. Trying OpenType layout instead.");
    capture_to_diagnostic(NULL);
    end_diagnostic(false);
}


internal_font_number
load_native_font(int32_t u, str_number nom, str_number aire, scaled_t s)
{
    int32_t k, num_font_dimens;
    void *font_engine; /* "really a CFDictionaryRef or XeTeXLayoutEngine" */
    scaled_t actual_size;
    int32_t p;
    scaled_t ascent, descent, font_slant, x_ht, cap_ht;
    internal_font_number f;
    str_number full_name;

    font_engine = find_native_font(name_of_file, s);
    if (!font_engine)
        return FONT_BASE;

    if (s >= 0)
        actual_size = s;
    else if (s != -1000)
        actual_size = xn_over_d(loaded_font_design_size, -(int32_t) s, 1000);
    else
        actual_size = loaded_font_design_size;

    if (pool_ptr + name_length > pool_size)
        overflow("pool size", pool_size - init_pool_ptr);

    for (k = 0; k < name_length; k++)
        str_pool[pool_ptr++] = name_of_file[k];

    full_name = make_string();

    for (f = FONT_BASE + 1; f <= font_ptr; f++) {
        if (font_area[f] == native_font_type_flag && str_eq_str(font_name[f], full_name) && font_size[f] == actual_size) {
            release_font_engine(font_engine, native_font_type_flag);

            str_ptr--;
            pool_ptr = str_start[str_ptr - TOO_BIG_CHAR];

            return f;
        }
    }

    if (native_font_type_flag == OTGR_FONT_FLAG && isOpenTypeMathFont(font_engine))
        num_font_dimens = 65; /* = first_math_fontdimen (=10) + lastMathConstant (= radicalDegreeBottomRaisePercent = 55) */
    else
        num_font_dimens = 8;

    if (font_ptr == font_max || fmem_ptr + num_font_dimens > font_mem_size) {
        error_here_with_diagnostic("Font ");
        sprint_cs(u);
        print_char('=');
        if (file_name_quote_char != 0)
            print_char(file_name_quote_char);
        print_file_name(nom, aire, cur_ext);
        if (file_name_quote_char != 0)
            print_char(file_name_quote_char);
        if (s >= 0) {
            print_cstr(" at ");
            print_scaled(s);
            print_cstr("pt");
        } else if (s != -1000) {
            print_cstr(" scaled ");
            print_int(-(int32_t) s);
        }
        print_cstr(" not loaded: Not enough room left");
        capture_to_diagnostic(NULL);

        help_ptr = 4;
        help_line[3] = "I'm afraid I won't be able to make use of this font,";
        help_line[2] = "because my memory for character-size data is too small.";
        help_line[1] = "If you're really stuck, ask a wizard to enlarge me.";
        help_line[0] = "Or maybe try `I\\font<same font id>=<name of loaded font>'.";

        error();
        return FONT_BASE;
    }

    font_ptr++;
    font_area[font_ptr] = native_font_type_flag;
    font_name[font_ptr] = full_name;
    font_check[font_ptr].s3 = 0;
    font_check[font_ptr].s2 = 0;
    font_check[font_ptr].s1 = 0;
    font_check[font_ptr].s0 = 0;
    font_glue[font_ptr] = TEX_NULL;
    font_dsize[font_ptr] = loaded_font_design_size;
    font_size[font_ptr] = actual_size;

    if (native_font_type_flag == AAT_FONT_FLAG)
        aat_get_font_metrics(font_engine, &ascent, &descent, &x_ht, &cap_ht, &font_slant);
    else
        ot_get_font_metrics(font_engine, &ascent, &descent, &x_ht, &cap_ht, &font_slant);

    height_base[font_ptr] = ascent;
    depth_base[font_ptr] = -(int32_t) descent;
    font_params[font_ptr] = num_font_dimens;
    font_bc[font_ptr] = 0;
    font_ec[font_ptr] = 65535L;
    font_used[font_ptr] = false;
    hyphen_char[font_ptr] = INTPAR(default_hyphen_char);
    skew_char[font_ptr] = INTPAR(default_skew_char);
    param_base[font_ptr] = fmem_ptr - 1;
    font_layout_engine[font_ptr] = font_engine;
    font_mapping[font_ptr] = 0;
    font_letter_space[font_ptr] = loaded_font_letter_space;

    /* "measure the width of the space character and set up font parameters" */
    p = new_native_character(font_ptr, ' ' );
    s = BOX_width(p) + loaded_font_letter_space;
    /* Free up the memory */
    if(NATIVE_NODE_glyph_info_ptr(p)) {
       NATIVE_NODE_glyph_info_ptr(p) = mfree(NATIVE_NODE_glyph_info_ptr(p));
    }
    free_node(p, NATIVE_NODE_size(p));

    font_info[fmem_ptr++].b32.s1 = font_slant;
    font_info[fmem_ptr++].b32.s1 = s;
    font_info[fmem_ptr++].b32.s1 = s / 2; /* space_stretch */
    font_info[fmem_ptr++].b32.s1 = s / 3; /* space_shrink */
    font_info[fmem_ptr++].b32.s1 = x_ht;
    font_info[fmem_ptr++].b32.s1 = font_size[font_ptr]; /* quad */
    font_info[fmem_ptr++].b32.s1 = s / 3; /* extra_space */
    font_info[fmem_ptr++].b32.s1 = cap_ht;

    if (num_font_dimens == 65) {
        font_info[fmem_ptr++].b32.s1 = num_font_dimens;

        for (k = 0; k <= 55; k++) /* 55 = lastMathConstant */
            font_info[fmem_ptr++].b32.s1 = get_ot_math_constant(font_ptr, k);
    }

    font_mapping[font_ptr] = loaded_font_mapping;
    font_flags[font_ptr] = loaded_font_flags;
    return font_ptr;
}


void do_locale_linebreaks(int32_t s, int32_t len)
{
    int32_t offs, prevOffs, i;
    bool use_penalty, use_skip;

    if ((INTPAR(xetex_linebreak_locale) == 0) || (len == 1)) {
        mem[cur_list.tail].b32.s1 = new_native_word_node(main_f, len);
        cur_list.tail = LLIST_link(cur_list.tail);
        {
            register int32_t for_end;
            i = 0;
            for_end = len - 1;
            if (i <= for_end)
                do
                    NATIVE_NODE_text(cur_list.tail)[i] = native_text[s + i];
                while (i++ < for_end);
        }
        set_native_metrics(cur_list.tail, (INTPAR(xetex_use_glyph_metrics) > 0));
    } else {

        use_skip = GLUEPAR(xetex_linebreak_skip) != 0;
        use_penalty = INTPAR(xetex_linebreak_penalty) != 0 || !use_skip;
        linebreak_start(main_f, INTPAR(xetex_linebreak_locale), native_text + s, len);
        offs = 0;
        do {
            prevOffs = offs;
            offs = linebreak_next();
            if (offs > 0) {
                if (prevOffs != 0) {
                    if (use_penalty) {
                        mem[cur_list.tail].b32.s1 = new_penalty(INTPAR(xetex_linebreak_penalty));
                        cur_list.tail = LLIST_link(cur_list.tail);
                    }
                    if (use_skip) {
                        mem[cur_list.tail].b32.s1 = new_param_glue(GLUE_PAR__xetex_linebreak_skip);
                        cur_list.tail = LLIST_link(cur_list.tail);
                    }
                }
                mem[cur_list.tail].b32.s1 = new_native_word_node(main_f, offs - prevOffs);
                cur_list.tail = LLIST_link(cur_list.tail);
                {
                    register int32_t for_end;
                    i = prevOffs;
                    for_end = offs - 1;
                    if (i <= for_end)
                        do
                            NATIVE_NODE_text(cur_list.tail)[i - prevOffs] = native_text[s + i];
                        while (i++ < for_end);
                }
                set_native_metrics(cur_list.tail, (INTPAR(xetex_use_glyph_metrics) > 0));
            }
        } while (!(offs < 0));
    }
}

void bad_utf8_warning(void)
{
    begin_diagnostic();
    diagnostic_begin_capture_warning_here();
    print_nl_cstr("Invalid UTF-8 byte or sequence");
    if (cur_input.name == 0)
        print_cstr(" in terminal input");
    else {

        print_cstr(" at line ");
        print_int(line);
    }
    print_cstr(" replaced by U+FFFD.");
    capture_to_diagnostic(NULL);
    end_diagnostic(false);
}

int32_t get_input_normalization_state(void)
{

    if (eqtb == NULL)
        return 0;
    else
        return INTPAR(xetex_input_normalization);
}

int32_t get_tracing_fonts_state(void)
{

    return INTPAR(xetex_tracing_fonts);
}

internal_font_number
read_font_info(int32_t u, str_number nom, str_number aire, scaled_t s)
{
    font_index k;
    bool name_too_long;
    bool file_opened;
    int32_t lf, lh, bc, ec, nw, nh, nd, ni, nl, nk, ne, np;
    internal_font_number f;
    internal_font_number g;
    int a, b, c, d;
    b16x4 qw;
    scaled_t sw;
    int32_t bch_label;
    short bchar;
    scaled_t z;
    int32_t alpha;
    unsigned char beta;
    rust_input_handle_t tfm_file;

    g = FONT_BASE;

    file_opened = false;
    pack_file_name(nom, aire, cur_ext);

    if (INTPAR(xetex_tracing_fonts) > 0) {
        begin_diagnostic();
        diagnostic_begin_capture_warning_here();
        print_nl_cstr("Requested font \"");
        print_c_string(name_of_file);
        print('"');
        if (s < 0) {
            print_cstr(" scaled ");
            print_int(-(int32_t) s);
        } else {
            print_cstr(" at ");
            print_scaled(s);
            print_cstr("pt");
        }
        capture_to_diagnostic(NULL);
        end_diagnostic(false);
    }

    if (quoted_filename) {
        g = load_native_font(u, nom, aire, s);
        if (g != FONT_BASE)
            goto done;
    }

    name_too_long = (length(nom) > 255 || length(aire) > 255);
    if (name_too_long)
        goto bad_tfm;

    pack_file_name(nom, aire, EMPTY_STRING);
    check_for_tfm_font_mapping();

    tfm_file = tt_xetex_open_input (TTBC_FILE_FORMAT_TFM);
    if (tfm_file == NULL) {
        if (!quoted_filename) {
            g = load_native_font(u, nom, aire, s);
            if (g != FONT_BASE)
                goto done;
        }
        goto bad_tfm;
    }

    file_opened = true; /*:582*/

    /* We are a bit cavalier about EOF-checking since we can't very
     * conveniently implement feof() in the Rust layer, and it only ever is
     * used in this one place. */

#define READFIFTEEN(x) do { \
        x = ttstub_input_getc (tfm_file); \
        if (x > 127 || x == EOF) \
            goto bad_tfm; \
        x *= 256; \
        x += ttstub_input_getc (tfm_file);\
    } while (0)

    READFIFTEEN(lf);
    READFIFTEEN(lh);
    READFIFTEEN(bc);
    READFIFTEEN(ec);

    if (bc > ec + 1 || ec > 255)
        goto bad_tfm;

    if (bc > 255) {
        bc = 1;
        ec = 0;
    }

    READFIFTEEN(nw);
    READFIFTEEN(nh);
    READFIFTEEN(nd);
    READFIFTEEN(ni);
    READFIFTEEN(nl);
    READFIFTEEN(nk);
    READFIFTEEN(ne);
    READFIFTEEN(np);

    if (lf != 6 + lh + (ec - bc + 1) + nw + nh + nd + ni + nl + nk + ne + np)
        goto bad_tfm;
    if (nw == 0 || nh == 0 || nd == 0 || ni == 0)
        goto bad_tfm;

    lf = lf - 6 - lh;
    if (np < 7)
        lf = lf + 7 - np;

    if (font_ptr == font_max || fmem_ptr + lf > font_mem_size)
        _tt_abort("not enough memory to load another font");

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

    if (lh < 2)
        goto bad_tfm;

    qw.s3 = a = ttstub_input_getc (tfm_file);
    qw.s2 = b = ttstub_input_getc (tfm_file);
    qw.s1 = c = ttstub_input_getc (tfm_file);
    qw.s0 = d = ttstub_input_getc (tfm_file);
    if (a == EOF || b == EOF || c == EOF || d == EOF)
        goto bad_tfm;
    font_check[f] = qw;

    READFIFTEEN(z);
    z = z * 256 + ttstub_input_getc (tfm_file);
    z = (z * 16) + (ttstub_input_getc (tfm_file) / 16);
    if (z < 65536L)
        goto bad_tfm;

    while (lh > 2) {
        ttstub_input_getc (tfm_file);
        ttstub_input_getc (tfm_file);
        ttstub_input_getc (tfm_file);
        ttstub_input_getc (tfm_file);
        lh--;
    }

    font_dsize[f] = z;
    if (s != -1000) {
        if (s >= 0)
            z = s;
        else
            z = xn_over_d(z, -(int32_t) s, 1000);
    }

    font_size[f] = z;

    for (k = fmem_ptr; k <= width_base[f] - 1; k++) {
        qw.s3 = a = ttstub_input_getc (tfm_file);
        qw.s2 = b = ttstub_input_getc (tfm_file);
        qw.s1 = c = ttstub_input_getc (tfm_file);
        qw.s0 = d = ttstub_input_getc (tfm_file);
        if (a == EOF || b == EOF || c == EOF || d == EOF)
            goto bad_tfm;
        font_info[k].b16 = qw;

        if (a >= nw || b / 16 >= nh || b % 16 >= nd || c / 4 >= ni)
            goto bad_tfm;

        switch (c % 4) {
        case 1:
            if (d >= nl)
                goto bad_tfm;
            break;
        case 3:
            if (d >= ne)
                goto bad_tfm;
            break;
        case 2:
            if (d < bc || d > ec)
                goto bad_tfm;

            while (d < k + bc - fmem_ptr) {
                qw = FONT_CHARACTER_INFO(f, d);
                if ((qw.s1 % 4) != LIST_TAG)
                    goto not_found;
                d = qw.s0;
            }

            if (d == k + bc - fmem_ptr)
                goto bad_tfm;

        not_found:
            break;
        }
    }

    alpha = 16;
    while (z >= 0x800000) {
        z = z / 2;
        alpha = alpha + alpha;
    }
    beta = 256 / alpha;
    alpha = alpha * z;

    for (k = width_base[f]; k <= lig_kern_base[f] - 1; k++) {
        a = ttstub_input_getc (tfm_file);
        b = ttstub_input_getc (tfm_file);
        c = ttstub_input_getc (tfm_file);
        d = ttstub_input_getc (tfm_file);
        if (a == EOF || b == EOF || c == EOF || d == EOF)
            goto bad_tfm;
        sw = (((((d * z) / 256) + c * z) / 256) + b * z) / beta;

        if (a == 0)
            font_info[k].b32.s1 = sw;
        else if (a == 255)
            font_info[k].b32.s1 = sw - alpha;
        else
            goto bad_tfm;
    }

    if (font_info[width_base[f]].b32.s1 != 0)
        goto bad_tfm;
    if (font_info[height_base[f]].b32.s1 != 0)
        goto bad_tfm;
    if (font_info[depth_base[f]].b32.s1 != 0)
        goto bad_tfm;
    if (font_info[italic_base[f]].b32.s1 != 0)
        goto bad_tfm;

    bch_label = 32767;
    bchar = 256;

    if (nl > 0) {
        for (k = lig_kern_base[f]; k <= kern_base[f] + 256 * 128 - 1; k++) {
            qw.s3 = a = ttstub_input_getc (tfm_file);
            qw.s2 = b = ttstub_input_getc (tfm_file);
            qw.s1 = c = ttstub_input_getc (tfm_file);
            qw.s0 = d = ttstub_input_getc (tfm_file);
            if (a == EOF || b == EOF || c == EOF || d == EOF)
                goto bad_tfm;
            font_info[k].b16 = qw;

            if (a > 128) {
                if (256 * c + d >= nl)
                    goto bad_tfm;

                if (a == 255 && k == lig_kern_base[f])
                    bchar = b;
            } else {
                if (b != bchar) {
                    if ((b < bc) || (b > ec))
                        goto bad_tfm;

                    qw = FONT_CHARACTER_INFO(f, b);
                    if (!(qw.s3 > 0))
                        goto bad_tfm;
                }

                if (c < 128) {
                    if ((d < bc) || (d > ec))
                        goto bad_tfm;
                    qw = FONT_CHARACTER_INFO(f, d);
                    if (!(qw.s3 > 0))
                        goto bad_tfm;
                } else if (256 * (c - 128) + d >= nk)
                    goto bad_tfm;

                if (a < 128 && k - lig_kern_base[f] + a + 1 >= nl)
                    goto bad_tfm;
            }
        }

        if (a == 255)
            bch_label = 256 * c + d;
    }

    for (k = kern_base[f] + 256 * 128; k <= exten_base[f] - 1; k++) {
        a = ttstub_input_getc (tfm_file);
        b = ttstub_input_getc (tfm_file);
        c = ttstub_input_getc (tfm_file);
        d = ttstub_input_getc (tfm_file);
        if (a == EOF || b == EOF || c == EOF || d == EOF)
            goto bad_tfm;
        sw = (((((d * z) / 256) + c * z) / 256) + b * z) / beta;

        if (a == 0)
            font_info[k].b32.s1 = sw;
        else if (a == 255)
            font_info[k].b32.s1 = sw - alpha;
        else
            goto bad_tfm;
    }

    for (k = exten_base[f]; k <= param_base[f] - 1; k++) {
        qw.s3 = a = ttstub_input_getc (tfm_file);
        qw.s2 = b = ttstub_input_getc (tfm_file);
        qw.s1 = c = ttstub_input_getc (tfm_file);
        qw.s0 = d = ttstub_input_getc (tfm_file);
        if (a == EOF || b == EOF || c == EOF || d == EOF)
            goto bad_tfm;
        font_info[k].b16 = qw;

        if (a != 0) {
            if ((a < bc) || (a > ec))
                goto bad_tfm;
            qw = FONT_CHARACTER_INFO(f, a);
            if (!(qw.s3 > 0))
                goto bad_tfm;
        }

        if (b != 0) {
            if ((b < bc) || (b > ec))
                goto bad_tfm;
            qw = FONT_CHARACTER_INFO(f, b);
            if (!(qw.s3 > 0))
                goto bad_tfm;
        }

        if (c != 0) {
            if ((c < bc) || (c > ec))
                goto bad_tfm;
            qw = FONT_CHARACTER_INFO(f, c);
            if (!(qw.s3 > 0))
                goto bad_tfm;
        }

        if ((d < bc) || (d > ec))
            goto bad_tfm;
        qw = FONT_CHARACTER_INFO(f, d);
        if (!(qw.s3 > 0))
            goto bad_tfm;
    }

    for (k = 1; k <= np; k++) {
        if (k == 1) {
            sw = ttstub_input_getc (tfm_file);
            if (sw == EOF)
                goto bad_tfm;
            if (sw > 127)
                sw = sw - 256;

            sw = sw * 256 + ttstub_input_getc (tfm_file);
            sw = sw * 256 + ttstub_input_getc (tfm_file);
            font_info[param_base[f]].b32.s1 = (sw * 16) + (ttstub_input_getc (tfm_file) / 16);
        } else {
            a = ttstub_input_getc (tfm_file);
            b = ttstub_input_getc (tfm_file);
            c = ttstub_input_getc (tfm_file);
            d = ttstub_input_getc (tfm_file);
            if (a == EOF || b == EOF || c == EOF || d == EOF)
                goto bad_tfm;
            sw = (((((d * z) / 256) + c * z) / 256) + b * z) / beta;

            if (a == 0)
                font_info[param_base[f] + k - 1].b32.s1 = sw;
            else if (a == 255)
                font_info[param_base[f] + k - 1].b32.s1 = sw - alpha;
            else
                goto bad_tfm;
        }
    }

    for (k = np + 1; k <= 7; k++)
        font_info[param_base[f] + k - 1].b32.s1 = 0;

    if (np >= 7)
        font_params[f] = np;
    else
        font_params[f] = 7;

    hyphen_char[f] = INTPAR(default_hyphen_char);
    skew_char[f] = INTPAR(default_skew_char);
    if (bch_label < nl)
        bchar_label[f] = bch_label + lig_kern_base[f];
    else
        bchar_label[f] = NON_ADDRESS;
    font_bchar[f] = bchar;
    font_false_bchar[f] = bchar;

    if (bchar <= ec) {
        if (bchar >= bc) {
            qw = FONT_CHARACTER_INFO(f, bchar);
            if ((qw.s3 > 0))
                font_false_bchar[f] = TOO_BIG_CHAR;
        }
    }

    font_name[f] = nom;
    font_area[f] = aire;
    font_bc[f] = bc;
    font_ec[f] = ec;
    font_glue[f] = TEX_NULL;
    param_base[f]--;
    fmem_ptr = fmem_ptr + lf;
    font_ptr = f;
    g = f;
    font_mapping[f] = load_tfm_font_mapping();
    goto done;

bad_tfm:
    if (INTPAR(suppress_fontnotfound_error) == 0) {
        /* NOTE: must preserve this path to keep passing the TRIP tests */

        error_here_with_diagnostic("Font ");
        sprint_cs(u);
        print_char('=');
        if (file_name_quote_char != 0)
            print_char(file_name_quote_char);
        print_file_name(nom, aire, cur_ext);
        if (file_name_quote_char != 0)
            print_char(file_name_quote_char);
        if (s >= 0) {
            print_cstr(" at ");
            print_scaled(s);
            print_cstr("pt");
        } else if (s != -1000) {
            print_cstr(" scaled ");
            print_int(-(int32_t) s);
        }

        if (file_opened)
            print_cstr(" not loadable: Bad metric (TFM) file");
        else if (name_too_long)
            print_cstr(" not loadable: Metric (TFM) file name too long");
        else
            print_cstr(" not loadable: Metric (TFM) file or installed font not found");
        capture_to_diagnostic(NULL);

        help_ptr = 5;
        help_line[4] = "I wasn't able to read the size data for this font,";
        help_line[3] = "so I will ignore the font specification.";
        help_line[2] = "[Wizards can fix TFM files using TFtoPL/PLtoTF.]";
        help_line[1] = "You might try inserting a different font spec;";
        help_line[0] = "e.g., type `I\\font<same font id>=<substitute font name>'.";

        error();
    }

done:
    if (file_opened)
        ttstub_input_close (tfm_file);

    if (INTPAR(xetex_tracing_fonts) > 0) {
        if (g == FONT_BASE || file_opened) {
            begin_diagnostic();
            diagnostic_begin_capture_warning_here();

            print_nl_cstr(" -> ");
            if (g == FONT_BASE)
                print_c_string("font not found, using \"nullfont\"");
            else
                print_c_string(name_of_file);

            capture_to_diagnostic(NULL);
            end_diagnostic(false);
        }
    }

    return g;
}

int32_t new_character(internal_font_number f, UTF16_code c)
{
    int32_t p;
    uint16_t ec;
    if (((font_area[f] == AAT_FONT_FLAG) || (font_area[f] == OTGR_FONT_FLAG))) {
        return new_native_character(f, c);
    }
    ec = effective_char(false, f, c);
    if (font_bc[f] <= ec) {

        if (font_ec[f] >= ec) {

            if ((FONT_CHARACTER_INFO(f, ec).s3 > 0)) {
                p = get_avail();
                mem[p].b16.s1 = f;
                mem[p].b16.s0 = c;
                return p;
            }
        }
    }
    char_warning(f, c);
    return TEX_NULL;
}


void scan_spec(group_code c, bool three_codes)
{
    int32_t s;
    unsigned char /*additional */ spec_code;
    if (three_codes)
        s = save_stack[save_ptr + 0].b32.s1;
    if (scan_keyword("to"))
        spec_code = EXACTLY;
    else if (scan_keyword("spread"))
        spec_code = ADDITIONAL;
    else {

        spec_code = ADDITIONAL;
        cur_val = 0;
        goto found;
    }
    scan_dimen(false, false, false);
 found:
    if (three_codes) {
        save_stack[save_ptr + 0].b32.s1 = s;
        save_ptr++;
    }
    save_stack[save_ptr + 0].b32.s1 = spec_code;
    save_stack[save_ptr + 1].b32.s1 = cur_val;
    save_ptr = save_ptr + 2;
    new_save_level(c);
    scan_left_brace();
}

scaled_t char_pw(int32_t p, small_number side)
{
    internal_font_number f;
    int32_t c;
    if (side == 0)
        last_leftmost_char = TEX_NULL;
    else
        last_rightmost_char = TEX_NULL;
    if (p == TEX_NULL)
        return 0;
    if (((p != TEX_NULL && (!(is_char_node(p))) && (NODE_type(p) == WHATSIT_NODE)
          && (NODE_subtype(p) == NATIVE_WORD_NODE || NODE_subtype(p) == NATIVE_WORD_NODE_AT)))) {
        if (NATIVE_NODE_glyph_info_ptr(p) != NULL) {
            f = NATIVE_NODE_font(p);
            return round_xn_over_d(font_info[QUAD_CODE + param_base[f]].b32.s1, get_native_word_cp(p, side), 1000);
        } else {
            return 0;
        }
    }
    if ((((p) != TEX_NULL && (!(is_char_node(p))) && (NODE_type(p) == WHATSIT_NODE)
          && (mem[p].b16.s0 == GLYPH_NODE)))) {
        f = mem[p + 4].b16.s2;
        return round_xn_over_d(font_info[QUAD_CODE + param_base[f]].b32.s1, get_cp_code(f, mem[p + 4].b16.s1, side),
                               1000);
    }
    if (!(is_char_node(p))) {
        if (NODE_type(p) == LIGATURE_NODE)
            p = p + 1;
        else
            return 0;
    }
    f = CHAR_NODE_font(p);
    c = get_cp_code(f, mem[p].b16.s0, side);
    switch (side) {
    case 0:
        last_leftmost_char = p;
        break;
    case 1:
        last_rightmost_char = p;
        break;
    }
    if (c == 0)
        return 0;
    return round_xn_over_d(font_info[QUAD_CODE + param_base[f]].b32.s1, c, 1000);
}

int32_t new_margin_kern(scaled_t w, int32_t p, small_number side)
{
    int32_t k;
    k = get_node(MARGIN_KERN_NODE_SIZE);
    NODE_type(k) = MARGIN_KERN_NODE;
    mem[k].b16.s0 = side;
    mem[k + 1].b32.s1 = w;
    return k;
}

int32_t hpack(int32_t p, scaled_t w, small_number m)
{
    int32_t r;
    int32_t q;
    scaled_t h, d, x;
    scaled_t s;
    int32_t g;
    glue_ord o;
    internal_font_number f;
    b16x4 i;
    int32_t pp, ppp = TEX_NULL;
    int32_t total_chars, k;

    last_badness = 0;
    r = get_node(BOX_NODE_SIZE);
    NODE_type(r) = HLIST_NODE;
    mem[r].b16.s0 = 0;
    mem[r + 4].b32.s1 = 0;
    q = r + 5;
    mem[q].b32.s1 = p;
    h = 0;
    d = 0;
    x = 0;
    total_stretch[NORMAL] = 0;
    total_shrink[NORMAL] = 0;
    total_stretch[FIL] = 0;
    total_shrink[FIL] = 0;
    total_stretch[FILL] = 0;
    total_shrink[FILL] = 0;
    total_stretch[FILLL] = 0;
    total_shrink[FILLL] = 0 /*:673 */ ;
    if (INTPAR(texxet) > 0) {    /*1497: */
        temp_ptr = get_avail();
        mem[temp_ptr].b32.s0 = BEFORE;
        mem[temp_ptr].b32.s1 = LR_ptr;
        LR_ptr = temp_ptr;
    }
    while (p != TEX_NULL) {  /*674: */

    reswitch:
        while ((is_char_node(p))) {
                                                        /*677: */

            f = CHAR_NODE_font(p);
            i = FONT_CHARACTER_INFO(f, effective_char(true, f, CHAR_NODE_character(p)));
            x = x + FONT_CHARINFO_WIDTH(f, i);
            s = FONT_CHARINFO_HEIGHT(f, i);
            if (s > h)
                h = s;
            s = FONT_CHARINFO_DEPTH(f, i);
            if (s > d)
                d = s;
            p = LLIST_link(p);
        }
        if (p != TEX_NULL) {
            switch (mem[p].b16.s1) {
            case 0:
            case 1:
            case 2:
            case 13:
                {
                    x = x + mem[p + 1].b32.s1;
                    if (NODE_type(p) >= RULE_NODE)
                        s = 0;
                    else
                        s = mem[p + 4].b32.s1;
                    if (mem[p + 3].b32.s1 - s > h)
                        h = mem[p + 3].b32.s1 - s;
                    if (mem[p + 2].b32.s1 + s > d)
                        d = mem[p + 2].b32.s1 + s;
                }
                break;
            case 3:
            case 4:
            case 5:
                if ((adjust_tail != TEX_NULL) || (pre_adjust_tail != TEX_NULL)) { /*680: */
                    while (mem[q].b32.s1 != p)
                        q = LLIST_link(q);
                    if (NODE_type(p) == ADJUST_NODE) {
                        if (mem[p].b16.s0 != 0) {
                            if (pre_adjust_tail == TEX_NULL)
                                confusion("pre vadjust");
                            mem[pre_adjust_tail].b32.s1 = mem[p + 1].b32.s1;
                            while (mem[pre_adjust_tail].b32.s1 != TEX_NULL)
                                pre_adjust_tail = LLIST_link(pre_adjust_tail);
                        } else {

                            if (adjust_tail == TEX_NULL)
                                confusion("pre vadjust");
                            mem[adjust_tail].b32.s1 = mem[p + 1].b32.s1;
                            while (mem[adjust_tail].b32.s1 != TEX_NULL)
                                adjust_tail = LLIST_link(adjust_tail);
                        }
                        p = LLIST_link(p);
                        free_node(mem[q].b32.s1, SMALL_NODE_SIZE);
                    } else {

                        mem[adjust_tail].b32.s1 = p;
                        adjust_tail = p;
                        p = LLIST_link(p);
                    }
                    mem[q].b32.s1 = p;
                    p = q;
                }
                break;
            case 8:
                {
                    switch (mem[p].b16.s0) {
                    case 40:
                    case 41:
                        {
                            if ((q != r + 5) && (NODE_type(q) == DISC_NODE))
                                k = mem[q].b16.s0;
                            else
                                k = 0;
                            while ((mem[q].b32.s1 != p)) {

                                k--;
                                q = LLIST_link(q);
                                if (NODE_type(q) == DISC_NODE)
                                    k = mem[q].b16.s0;
                            }
                            pp = mem[p].b32.s1;
                        restart:
                            if ((k <= 0) && (pp != TEX_NULL) && (!(is_char_node(pp)))) {
                                if ((NODE_type(pp) == WHATSIT_NODE)
                                    && ((mem[pp].b16.s0 == NATIVE_WORD_NODE)
                                        || (mem[pp].b16.s0 == NATIVE_WORD_NODE_AT))
                                    && (mem[pp + 4].b16.s2 == mem[p + 4].b16.s2)) {
                                    pp = LLIST_link(pp);
                                    goto restart;
                                } else if (NODE_type(pp) == DISC_NODE) {
                                    ppp = mem[pp].b32.s1;
                                    if ((((ppp) != TEX_NULL && (!(is_char_node(ppp)))
                                          && (NODE_type(ppp) == WHATSIT_NODE)
                                          && ((mem[ppp].b16.s0 == NATIVE_WORD_NODE)
                                              || (mem[ppp].b16.s0 == NATIVE_WORD_NODE_AT))))
                                        && (mem[ppp + 4].b16.s2 == mem[p + 4].b16.s2)) {
                                        pp = mem[ppp].b32.s1;
                                        goto restart;
                                    }
                                }
                            }
                            if ((pp != mem[p].b32.s1)) {
                                total_chars = 0;
                                p = mem[q].b32.s1;
                                while ((p != pp)) {

                                    if (NODE_type(p) == WHATSIT_NODE)
                                        total_chars = total_chars + mem[p + 4].b16.s1;
                                    ppp = p;
                                    p = LLIST_link(p);
                                }
                                p = mem[q].b32.s1;
                                pp = new_native_word_node(mem[p + 4].b16.s2, total_chars);
                                mem[pp].b16.s0 = mem[p].b16.s0;
                                mem[q].b32.s1 = pp;
                                mem[pp].b32.s1 = mem[ppp].b32.s1;
                                mem[ppp].b32.s1 = TEX_NULL;
                                total_chars = 0;
                                ppp = p;
                                do {
                                    if (NODE_type(ppp) == WHATSIT_NODE) {
                                        register int32_t for_end;
                                        k = 0;
                                        for_end = mem[ppp + 4].b16.s1 - 1;
                                        if (k <= for_end)
                                            do {
                                                NATIVE_NODE_text(pp)[total_chars] = NATIVE_NODE_text(ppp)[k];
                                                total_chars++;
                                            }
                                            while (k++ < for_end);
                                    }
                                    ppp = LLIST_link(ppp);
                                } while (!((ppp == TEX_NULL)));
                                flush_node_list(p);
                                p = mem[q].b32.s1;
                                set_native_metrics(p, (INTPAR(xetex_use_glyph_metrics) > 0));
                            }
                            if (mem[p + 3].b32.s1 > h)
                                h = mem[p + 3].b32.s1;
                            if (mem[p + 2].b32.s1 > d)
                                d = mem[p + 2].b32.s1;
                            x = x + mem[p + 1].b32.s1;
                        }
                        break;
                    case 42:
                    case 43:
                    case 44:
                        {
                            if (mem[p + 3].b32.s1 > h)
                                h = mem[p + 3].b32.s1;
                            if (mem[p + 2].b32.s1 > d)
                                d = mem[p + 2].b32.s1;
                            x = x + mem[p + 1].b32.s1;
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
                    g = mem[p + 1].b32.s0;
                    x = x + mem[g + 1].b32.s1;
                    o = mem[g].b16.s1;
                    total_stretch[o] = total_stretch[o] + mem[g + 2].b32.s1;
                    o = mem[g].b16.s0;
                    total_shrink[o] = total_shrink[o] + mem[g + 3].b32.s1;
                    if (mem[p].b16.s0 >= A_LEADERS) {
                        g = mem[p + 1].b32.s1;
                        if (mem[g + 3].b32.s1 > h)
                            h = mem[g + 3].b32.s1;
                        if (mem[g + 2].b32.s1 > d)
                            d = mem[g + 2].b32.s1;
                    }
                }
                break;
            case 11:
                x = x + mem[p + 1].b32.s1;
                break;
            case 40:
                x = x + mem[p + 1].b32.s1;
                break;
            case 9:
                {
                    x = x + mem[p + 1].b32.s1;
                    if (INTPAR(texxet) > 0) {    /*1498: */

                        if (odd(mem[p].b16.s0)) {

                            if (mem[LR_ptr].b32.s0 == (L_CODE * (mem[p].b16.s0 / L_CODE) + 3)) {
                                temp_ptr = LR_ptr;
                                LR_ptr = mem[temp_ptr].b32.s1;
                                {
                                    mem[temp_ptr].b32.s1 = avail;
                                    avail = temp_ptr;
                                }
                            } else {

                                LR_problems++;
                                NODE_type(p) = KERN_NODE;
                                NODE_subtype(p) = EXPLICIT;
                            }
                        } else {

                            temp_ptr = get_avail();
                            mem[temp_ptr].b32.s0 = (L_CODE * (mem[p].b16.s0 / L_CODE) + 3);
                            mem[temp_ptr].b32.s1 = LR_ptr;
                            LR_ptr = temp_ptr;
                        }
                    }
                }
                break;
            case 6:
                mem[GARBAGE] = mem[p + 1];
                mem[GARBAGE].b32.s1 = mem[p].b32.s1;
                p = GARBAGE;
                xtx_ligature_present = true;
                goto reswitch;
            default:
                break;
            }
            p = LLIST_link(p);
        }
    }
    if (adjust_tail != TEX_NULL)
        mem[adjust_tail].b32.s1 = TEX_NULL;
    if (pre_adjust_tail != TEX_NULL)
        mem[pre_adjust_tail].b32.s1 = TEX_NULL;
    mem[r + 3].b32.s1 = h;
    mem[r + 2].b32.s1 = d;
    if (m == ADDITIONAL)
        w = x + w;
    mem[r + 1].b32.s1 = w;
    x = w - x;
    if (x == 0) {
        mem[r + 5].b16.s1 = NORMAL;
        mem[r + 5].b16.s0 = NORMAL;
        BOX_glue_set(r) = 0.0;
        goto exit;
    } else if (x > 0) {         /*683: */
        if (total_stretch[FILLL] != 0)
            o = FILLL;
        else if (total_stretch[FILL] != 0)
            o = FILL;
        else if (total_stretch[FIL] != 0)
            o = FIL;
        else
            o = 0 /*normal *//*:684 */ ;
        mem[r + 5].b16.s0 = o;
        mem[r + 5].b16.s1 = STRETCHING;
        if (total_stretch[o] != 0)
            BOX_glue_set(r) = x / ((double)total_stretch[o]);
        else {
            mem[r + 5].b16.s1 = NORMAL;
            BOX_glue_set(r) = 0.0;
        }

        if (o == NORMAL) {

            if (mem[r + 5].b32.s1 != TEX_NULL) {    /*685: */
                last_badness = badness(x, total_stretch[NORMAL]);
                if (last_badness > INTPAR(hbadness)) {
                    print_ln();

                    diagnostic_begin_capture_warning_here();

                    if (last_badness > 100)
                        print_nl_cstr("Underfull");
                    else
                        print_nl_cstr("Loose");
                    print_cstr(" \\hbox (badness ");
                    print_int(last_badness);
                    goto common_ending;
                }
            }
        }
        goto exit;
    } else {                    /*689: */

        if (total_shrink[FILLL] != 0)
            o = FILLL;
        else if (total_shrink[FILL] != 0)
            o = FILL;
        else if (total_shrink[FIL] != 0)
            o = FIL;
        else
            o = 0 /*normal *//*:690 */ ;
        mem[r + 5].b16.s0 = o;
        mem[r + 5].b16.s1 = SHRINKING;
        if (total_shrink[o] != 0)
            BOX_glue_set(r) = (-(int32_t) x) / ((double)total_shrink[o]);
        else {
            mem[r + 5].b16.s1 = NORMAL;
            BOX_glue_set(r) = 0.0;
        }
        if ((total_shrink[o] < -(int32_t) x) && (o == NORMAL) && (mem[r + 5].b32.s1 != TEX_NULL)) {
            last_badness = 1000000L;
            BOX_glue_set(r) = 1.0;
            if ((-(int32_t) x - total_shrink[NORMAL] > DIMENPAR(hfuzz))
                || (INTPAR(hbadness) < 100)) {
                if ((DIMENPAR(overfull_rule) > 0)
                    && (-(int32_t) x - total_shrink[NORMAL] > DIMENPAR(hfuzz))) {
                    while (mem[q].b32.s1 != TEX_NULL)
                        q = LLIST_link(q);
                    mem[q].b32.s1 = new_rule();
                    mem[mem[q].b32.s1 + 1].b32.s1 = DIMENPAR(overfull_rule);
                }
                print_ln();

                diagnostic_begin_capture_warning_here();
                print_nl_cstr("Overfull \\hbox (");
                print_scaled(-(int32_t) x - total_shrink[NORMAL]);
                print_cstr("pt too wide");
                goto common_ending;
            }
        } else if (o == NORMAL) {

            if (mem[r + 5].b32.s1 != TEX_NULL) {    /*692: */
                last_badness = badness(-(int32_t) x, total_shrink[NORMAL]);
                if (last_badness > INTPAR(hbadness)) {
                    print_ln();

                    diagnostic_begin_capture_warning_here();
                    print_nl_cstr("Tight \\hbox (badness ");
                    print_int(last_badness);
                    goto common_ending;
                }
            }
        }
        goto exit;
    }

common_ending:
    if (output_active)
        print_cstr(") has occurred while \\output is active");
    else {

        if (pack_begin_line != 0) {
            if (pack_begin_line > 0)
                print_cstr(") in paragraph at lines ");
            else
                print_cstr(") in alignment at lines ");
            print_int(abs(pack_begin_line));
            print_cstr("--");
        } else
            print_cstr(") detected at line ");
        print_int(line);
    }

    capture_to_diagnostic(NULL);

    print_ln();
    font_in_short_display = FONT_BASE;
    short_display(mem[r + 5].b32.s1);
    print_ln();
    begin_diagnostic();
    show_box(r);
    end_diagnostic(true);

exit:
    if (INTPAR(texxet) > 0) {
        /*1499: */
        if (mem[LR_ptr].b32.s0 != BEFORE) {
            while (mem[q].b32.s1 != TEX_NULL)
                q = LLIST_link(q);
            do {
                temp_ptr = q;
                q = new_math(0, mem[LR_ptr].b32.s0);
                mem[temp_ptr].b32.s1 = q;
                LR_problems = LR_problems + 10000;
                {
                    temp_ptr = LR_ptr;
                    LR_ptr = mem[temp_ptr].b32.s1;
                    {
                        mem[temp_ptr].b32.s1 = avail;
                        avail = temp_ptr;
                    }
                }
            } while (!(mem[LR_ptr].b32.s0 == BEFORE));
        }
        if (LR_problems > 0) {
            {
                print_ln();

                diagnostic_begin_capture_warning_here();
                print_nl_cstr("\\endL or \\endR problem (");
                print_int(LR_problems / 10000);
                print_cstr(" missing, ");
                print_int(LR_problems % 10000);
                print_cstr(" extra");
                LR_problems = 0;
            }
            goto common_ending;
        }
        {
            temp_ptr = LR_ptr;
            LR_ptr = mem[temp_ptr].b32.s1;
            {
                mem[temp_ptr].b32.s1 = avail;
                avail = temp_ptr;
            }
        }
        if (LR_ptr != TEX_NULL)
            confusion("LR1");
    }
    return r;
}

int32_t vpackage(int32_t p, scaled_t h, small_number m, scaled_t l)
{
    int32_t r;
    scaled_t w, d, x;
    scaled_t s;
    int32_t g;
    glue_ord o;

    last_badness = 0;
    r = get_node(BOX_NODE_SIZE);
    NODE_type(r) = VLIST_NODE;
    if ((INTPAR(xetex_upwards) > 0))
        mem[r].b16.s0 = 1;
    else
        mem[r].b16.s0 = 0;
    mem[r + 4].b32.s1 = 0;
    mem[r + 5].b32.s1 = p;
    w = 0;
    d = 0;
    x = 0;
    total_stretch[NORMAL] = 0;
    total_shrink[NORMAL] = 0;
    total_stretch[FIL] = 0;
    total_shrink[FIL] = 0;
    total_stretch[FILL] = 0;
    total_shrink[FILL] = 0;
    total_stretch[FILLL] = 0;
    total_shrink[FILLL] = 0 /*:673 */ ;
    while (p != TEX_NULL) {  /*694: */

        if ((is_char_node(p)))
            confusion("vpack");
        else
            switch (mem[p].b16.s1) {
            case 0:
            case 1:
            case 2:
            case 13:
                {
                    x = x + d + mem[p + 3].b32.s1;
                    d = mem[p + 2].b32.s1;
                    if (NODE_type(p) >= RULE_NODE)
                        s = 0;
                    else
                        s = mem[p + 4].b32.s1;
                    if (mem[p + 1].b32.s1 + s > w)
                        w = mem[p + 1].b32.s1 + s;
                }
                break;
            case 8:
                {
                    if ((mem[p].b16.s0 == PIC_NODE) || (mem[p].b16.s0 == PDF_NODE)) {
                        x = x + d + mem[p + 3].b32.s1;
                        d = mem[p + 2].b32.s1;
                        if (mem[p + 1].b32.s1 > w)
                            w = mem[p + 1].b32.s1;
                    }
                }
                break;
            case 10:
                {
                    x = x + d;
                    d = 0;
                    g = mem[p + 1].b32.s0;
                    x = x + mem[g + 1].b32.s1;
                    o = mem[g].b16.s1;
                    total_stretch[o] = total_stretch[o] + mem[g + 2].b32.s1;
                    o = mem[g].b16.s0;
                    total_shrink[o] = total_shrink[o] + mem[g + 3].b32.s1;
                    if (mem[p].b16.s0 >= A_LEADERS) {
                        g = mem[p + 1].b32.s1;
                        if (mem[g + 1].b32.s1 > w)
                            w = mem[g + 1].b32.s1;
                    }
                }
                break;
            case 11:
                {
                    x = x + d + mem[p + 1].b32.s1;
                    d = 0;
                }
                break;
            default:
                ;
                break;
            }
        p = LLIST_link(p);
    }
    mem[r + 1].b32.s1 = w;
    if (d > l) {
        x = x + d - l;
        mem[r + 2].b32.s1 = l;
    } else
        mem[r + 2].b32.s1 = d;
    if (m == ADDITIONAL)
        h = x + h;
    mem[r + 3].b32.s1 = h;
    x = h - x;
    if (x == 0) {
        mem[r + 5].b16.s1 = NORMAL;
        mem[r + 5].b16.s0 = NORMAL;
        BOX_glue_set(r) = 0.0;
        goto exit;
    } else if (x > 0) {         /*698: */
        if (total_stretch[FILLL] != 0)
            o = FILLL;
        else if (total_stretch[FILL] != 0)
            o = FILL;
        else if (total_stretch[FIL] != 0)
            o = FIL;
        else
            o = 0 /*normal *//*:684 */ ;
        mem[r + 5].b16.s0 = o;
        mem[r + 5].b16.s1 = STRETCHING;
        if (total_stretch[o] != 0)
            BOX_glue_set(r) = x / ((double)total_stretch[o]);
        else {
            mem[r + 5].b16.s1 = NORMAL;
            BOX_glue_set(r) = 0.0;
        }
        if (o == NORMAL) {

            if (mem[r + 5].b32.s1 != TEX_NULL) {    /*699: */
                last_badness = badness(x, total_stretch[NORMAL]);
                if (last_badness > INTPAR(vbadness)) {
                    print_ln();

                    diagnostic_begin_capture_warning_here();
                    if (last_badness > 100)
                        print_nl_cstr("Underfull");
                    else
                        print_nl_cstr("Loose");
                    print_cstr(" \\vbox (badness ");
                    print_int(last_badness);
                    goto common_ending;
                }
            }
        }
        goto exit;
    } else {                    /*701: */

        if (total_shrink[FILLL] != 0)
            o = FILLL;
        else if (total_shrink[FILL] != 0)
            o = FILL;
        else if (total_shrink[FIL] != 0)
            o = FIL;
        else
            o = 0 /*normal *//*:690 */ ;
        mem[r + 5].b16.s0 = o;
        mem[r + 5].b16.s1 = SHRINKING;
        if (total_shrink[o] != 0)
            BOX_glue_set(r) = (-(int32_t) x) / ((double)total_shrink[o]);
        else {
            mem[r + 5].b16.s1 = NORMAL;
            BOX_glue_set(r) = 0.0;
        }
        if ((total_shrink[o] < -(int32_t) x) && (o == NORMAL) && (mem[r + 5].b32.s1 != TEX_NULL)) {
            last_badness = 1000000L;
            BOX_glue_set(r) = 1.0;
            if ((-(int32_t) x - total_shrink[NORMAL] > DIMENPAR(vfuzz))
                || (INTPAR(vbadness) < 100)) {
                print_ln();

                diagnostic_begin_capture_warning_here();
                print_nl_cstr("Overfull \\vbox (");
                print_scaled(-(int32_t) x - total_shrink[NORMAL]);
                print_cstr("pt too high");
                goto common_ending;
            }
        } else if (o == NORMAL) {

            if (mem[r + 5].b32.s1 != TEX_NULL) {    /*703: */
                last_badness = badness(-(int32_t) x, total_shrink[NORMAL]);
                if (last_badness > INTPAR(vbadness)) {
                    print_ln();

                    diagnostic_begin_capture_warning_here();
                    print_nl_cstr("Tight \\vbox (badness ");
                    print_int(last_badness);
                    goto common_ending;
                }
            }
        }
        goto exit;
    }

common_ending:
    if (output_active)
        print_cstr(") has occurred while \\output is active");
    else {

        if (pack_begin_line != 0) {
            print_cstr(") in alignment at lines ");
            print_int(abs(pack_begin_line));
            print_cstr("--");
        } else
            print_cstr(") detected at line ");
        print_int(line);
        print_ln();
    }

    capture_to_diagnostic(NULL);

    begin_diagnostic();
    show_box(r);
    end_diagnostic(true);

exit:
    return r;
}

void append_to_vlist(int32_t b)
{
    scaled_t d;
    int32_t p;
    bool upwards;

    upwards = (INTPAR(xetex_upwards) > 0);
    if (cur_list.aux.b32.s1 > IGNORE_DEPTH) {
        if (upwards)
            d = mem[GLUEPAR(baseline_skip) + 1].b32.s1 - cur_list.aux.b32.s1 - mem[b + 2].b32.s1;
        else
            d = mem[GLUEPAR(baseline_skip) + 1].b32.s1 - cur_list.aux.b32.s1 - mem[b + 3].b32.s1;
        if (d < DIMENPAR(line_skip_limit))
            p = new_param_glue(GLUE_PAR__line_skip);
        else {

            p = new_skip_param(GLUE_PAR__baseline_skip);
            mem[temp_ptr + 1].b32.s1 = d;
        }
        mem[cur_list.tail].b32.s1 = p;
        cur_list.tail = p;
    }
    mem[cur_list.tail].b32.s1 = b;
    cur_list.tail = b;
    if (upwards)
        cur_list.aux.b32.s1 = mem[b + 3].b32.s1;
    else
        cur_list.aux.b32.s1 = mem[b + 2].b32.s1;
}

int32_t new_noad(void)
{
    int32_t p;
    p = get_node(NOAD_SIZE);
    mem[p].b16.s1 = ORD_NOAD;
    mem[p].b16.s0 = NORMAL;
    mem[p + 1].b32 = empty;
    mem[p + 3].b32 = empty;
    mem[p + 2].b32 = empty;
    return p;
}

int32_t new_style(small_number s)
{
    int32_t p;
    p = get_node(STYLE_NODE_SIZE);
    NODE_type(p) = STYLE_NODE;
    mem[p].b16.s0 = s;
    mem[p + 1].b32.s1 = 0;
    mem[p + 2].b32.s1 = 0;
    return p;
}

int32_t new_choice(void)
{
    int32_t p;
    p = get_node(STYLE_NODE_SIZE);
    NODE_type(p) = CHOICE_NODE;
    mem[p].b16.s0 = 0;
    mem[p + 1].b32.s0 = TEX_NULL;
    mem[p + 1].b32.s1 = TEX_NULL;
    mem[p + 2].b32.s0 = TEX_NULL;
    mem[p + 2].b32.s1 = TEX_NULL;
    return p;
}

void show_info(void)
{

    show_node_list(mem[temp_ptr].b32.s0);
}

void push_alignment(void)
{
    int32_t p;
    p = get_node(ALIGN_STACK_NODE_SIZE);
    mem[p].b32.s1 = align_ptr;
    mem[p].b32.s0 = cur_align;
    mem[p + 1].b32.s0 = mem[ALIGN_HEAD].b32.s1;
    mem[p + 1].b32.s1 = cur_span;
    mem[p + 2].b32.s1 = cur_loop;
    mem[p + 3].b32.s1 = align_state;
    mem[p + 4].b32.s0 = cur_head;
    mem[p + 4].b32.s1 = cur_tail;
    mem[p + 5].b32.s0 = cur_pre_head;
    mem[p + 5].b32.s1 = cur_pre_tail;
    align_ptr = p;
    cur_head = get_avail();
    cur_pre_head = get_avail();
}

void pop_alignment(void)
{
    int32_t p;
    {
        mem[cur_head].b32.s1 = avail;
        avail = cur_head;
    }
    {
        mem[cur_pre_head].b32.s1 = avail;
        avail = cur_pre_head;
    }
    p = align_ptr;
    cur_tail = mem[p + 4].b32.s1;
    cur_head = mem[p + 4].b32.s0;
    cur_pre_tail = mem[p + 5].b32.s1;
    cur_pre_head = mem[p + 5].b32.s0;
    align_state = mem[p + 3].b32.s1;
    cur_loop = mem[p + 2].b32.s1;
    cur_span = mem[p + 1].b32.s1;
    mem[ALIGN_HEAD].b32.s1 = mem[p + 1].b32.s0;
    cur_align = mem[p].b32.s0;
    align_ptr = mem[p].b32.s1;
    free_node(p, ALIGN_STACK_NODE_SIZE);
}

void get_preamble_token(void)
{

restart:
    get_token();

    while ((cur_chr == SPAN_CODE) && (cur_cmd == TAB_MARK)) {

        get_token();
        if (cur_cmd > MAX_COMMAND) {
            expand();
            get_token();
        }
    }
    if (cur_cmd == ENDV)
        fatal_error("(interwoven alignment preambles are not allowed)");
    if ((cur_cmd == ASSIGN_GLUE) && (cur_chr == (GLUE_BASE + 11))) {
        scan_optional_equals();
        scan_glue(GLUE_VAL);
        if (INTPAR(global_defs) > 0)
            geq_define((GLUE_BASE + 11), GLUE_REF, cur_val);
        else
            eq_define((GLUE_BASE + 11), GLUE_REF, cur_val);
        goto restart;
    }
}


void
init_align(void)
{
    int32_t save_cs_ptr;
    int32_t p;

    save_cs_ptr = cur_cs;
    push_alignment();
    align_state = -1000000L;

    if (cur_list.mode == MMODE && (cur_list.tail != cur_list.head || cur_list.aux.b32.s1 != TEX_NULL)) {
        error_here_with_diagnostic("Improper ");
        print_esc_cstr("halign");
        print_cstr(" inside $$'s");
        capture_to_diagnostic(NULL);
        help_ptr = 3;
        help_line[2] = "Displays can use special alignments (like \\eqalignno)";
        help_line[1] = "only if nothing but the alignment itself is between $$'s.";
        help_line[0] = "So I've deleted the formulas that preceded this alignment.";
        error();
        flush_math();
    }

    push_nest();

    if (cur_list.mode == MMODE) {
        cur_list.mode = -1;
        cur_list.aux.b32.s1 = nest[nest_ptr - 2].aux.b32.s1;
    } else if (cur_list.mode > 0) {
        cur_list.mode = -(int32_t) cur_list.mode; /*:804*/
    }

    scan_spec(ALIGN_GROUP, false);
    mem[ALIGN_HEAD].b32.s1 = TEX_NULL;
    cur_align = ALIGN_HEAD;
    cur_loop = TEX_NULL;
    scanner_status = ALIGNING;
    warning_index = save_cs_ptr;
    align_state = -1000000L;

    while (true) {
        mem[cur_align].b32.s1 = new_param_glue(GLUE_PAR__tab_skip);
        cur_align = LLIST_link(cur_align); /*:807*/
        if (cur_cmd == CAR_RET)
            goto done;

        p = HOLD_HEAD;
        mem[p].b32.s1 = TEX_NULL;

        while (true) {
            get_preamble_token();
            if (cur_cmd == MAC_PARAM)
                goto done1;

            if (cur_cmd <= CAR_RET && cur_cmd >= TAB_MARK && align_state == -1000000L) {
                if (p == HOLD_HEAD && cur_loop == TEX_NULL && cur_cmd == TAB_MARK) {
                    cur_loop = cur_align;
                } else {
                    error_here_with_diagnostic("Missing # inserted in alignment preamble");
                    capture_to_diagnostic(NULL);
                    help_ptr = 3;
                    help_line[2] = "There should be exactly one # between &'s, when an";
                    help_line[1] = "\\halign or \\valign is being set up. In this case you had";
                    help_line[0] = "none, so I've put one in; maybe that will work.";
                    back_error();
                    goto done1;
                }
            } else if (cur_cmd != SPACER || p != HOLD_HEAD) {
                mem[p].b32.s1 = get_avail();
                p = LLIST_link(p);
                mem[p].b32.s0 = cur_tok;
            }
        }

    done1:
        mem[cur_align].b32.s1 = new_null_box();
        cur_align = LLIST_link(cur_align);
        mem[cur_align].b32.s0 = END_SPAN;
        mem[cur_align + 1].b32.s1 = NULL_FLAG;
        mem[cur_align + 3].b32.s1 = mem[HOLD_HEAD].b32.s1;
        p = HOLD_HEAD;
        mem[p].b32.s1 = TEX_NULL;

        while (true) {
        continue_:
            get_preamble_token();
            if (cur_cmd <= CAR_RET && cur_cmd >= TAB_MARK && align_state == -1000000L)
                goto done2;

            if (cur_cmd == MAC_PARAM) {
                error_here_with_diagnostic("Only one # is allowed per tab");
                capture_to_diagnostic(NULL);

                help_ptr = 3;
                help_line[2] = "There should be exactly one # between &'s, when an";
                help_line[1] = "\\halign or \\valign is being set up. In this case you had";
                help_line[0] = "more than one, so I'm ignoring all but the first.";
                error();
                goto continue_;
            }

            mem[p].b32.s1 = get_avail();
            p = LLIST_link(p);
            mem[p].b32.s0 = cur_tok;
        }

    done2:
        mem[p].b32.s1 = get_avail();
        p = LLIST_link(p);
        mem[p].b32.s0 = CS_TOKEN_FLAG + FROZEN_END_TEMPLATE; /*:813*/
        mem[cur_align + 2].b32.s1 = mem[HOLD_HEAD].b32.s1; /*:808 */
    }

done:
    scanner_status = NORMAL; /*:806 */
    new_save_level(ALIGN_GROUP);

    if (LOCAL(every_cr) != TEX_NULL)
        begin_token_list(LOCAL(every_cr), EVERY_CR_TEXT);

    align_peek();
}


void init_span(int32_t p)
{
    push_nest();
    if (cur_list.mode == -104)
        cur_list.aux.b32.s0 = 1000;
    else {

        cur_list.aux.b32.s1 = IGNORE_DEPTH;
        normal_paragraph();
    }
    cur_span = p;
}

void init_row(void)
{
    push_nest();
    cur_list.mode = (-105) - cur_list.mode;
    if (cur_list.mode == -104)
        cur_list.aux.b32.s0 = 0;
    else
        cur_list.aux.b32.s1 = 0;
    {
        mem[cur_list.tail].b32.s1 = new_glue(mem[mem[ALIGN_HEAD].b32.s1 + 1].b32.s0);
        cur_list.tail = LLIST_link(cur_list.tail);
    }
    mem[cur_list.tail].b16.s0 = (GLUE_PAR__tab_skip + 1);
    cur_align = mem[mem[ALIGN_HEAD].b32.s1].b32.s1;
    cur_tail = cur_head;
    cur_pre_tail = cur_pre_head;
    init_span(cur_align);
}

void init_col(void)
{
    mem[cur_align + 5].b32.s0 = cur_cmd;
    if (cur_cmd == OMIT)
        align_state = 0;
    else {

        back_input();
        begin_token_list(mem[cur_align + 3].b32.s1, U_TEMPLATE);
    }
}

bool fin_col(void)
{
    int32_t p;
    int32_t q, r;
    int32_t s;
    int32_t u;
    scaled_t w;
    glue_ord o;
    int32_t n;
    if (cur_align == TEX_NULL)
        confusion("endv");
    q = mem[cur_align].b32.s1;
    if (q == TEX_NULL)
        confusion("endv");
    if (align_state < 500000L)
        fatal_error("(interwoven alignment preambles are not allowed)");
    p = mem[q].b32.s1;
    if ((p == TEX_NULL) && (mem[cur_align + 5].b32.s0 < CR_CODE)) {

        if (cur_loop != TEX_NULL) {  /*822: */
            mem[q].b32.s1 = new_null_box();
            p = mem[q].b32.s1;
            mem[p].b32.s0 = END_SPAN;
            mem[p + 1].b32.s1 = NULL_FLAG;
            cur_loop = LLIST_link(cur_loop);
            q = HOLD_HEAD;
            r = mem[cur_loop + 3].b32.s1;
            while (r != TEX_NULL) {

                mem[q].b32.s1 = get_avail();
                q = LLIST_link(q);
                mem[q].b32.s0 = mem[r].b32.s0;
                r = LLIST_link(r);
            }
            mem[q].b32.s1 = TEX_NULL;
            mem[p + 3].b32.s1 = mem[HOLD_HEAD].b32.s1;
            q = HOLD_HEAD;
            r = mem[cur_loop + 2].b32.s1;
            while (r != TEX_NULL) {

                mem[q].b32.s1 = get_avail();
                q = LLIST_link(q);
                mem[q].b32.s0 = mem[r].b32.s0;
                r = LLIST_link(r);
            }
            mem[q].b32.s1 = TEX_NULL;
            mem[p + 2].b32.s1 = mem[HOLD_HEAD].b32.s1 /*:823 */ ;
            cur_loop = LLIST_link(cur_loop);
            mem[p].b32.s1 = new_glue(mem[cur_loop + 1].b32.s0);
        } else {
            error_here_with_diagnostic("Extra alignment tab has been changed to ");
            print_esc_cstr("cr");
            capture_to_diagnostic(NULL);

            {
                help_ptr = 3;
                help_line[2] = "You have given more \\span or & marks than there were";
                help_line[1] = "in the preamble to the \\halign or \\valign now in progress.";
                help_line[0] = "So I'll assume that you meant to type \\cr instead.";
            }
            mem[cur_align + 5].b32.s0 = CR_CODE;
            error();
        }
    }
    if (mem[cur_align + 5].b32.s0 != SPAN_CODE) {
        unsave();
        new_save_level(ALIGN_GROUP);
        {
            if (cur_list.mode == -104) {
                adjust_tail = cur_tail;
                pre_adjust_tail = cur_pre_tail;
                u = hpack(mem[cur_list.head].b32.s1, 0, ADDITIONAL);
                w = mem[u + 1].b32.s1;
                cur_tail = adjust_tail;
                adjust_tail = TEX_NULL;
                cur_pre_tail = pre_adjust_tail;
                pre_adjust_tail = TEX_NULL;
            } else {

                u = vpackage(mem[cur_list.head].b32.s1, 0, ADDITIONAL, 0);
                w = mem[u + 3].b32.s1;
            }
            n = 0;
            if (cur_span != cur_align) {        /*827: */
                q = cur_span;
                do {
                    n++;
                    q = mem[mem[q].b32.s1].b32.s1;
                } while (!(q == cur_align));
                if (n > UINT16_MAX)
                    confusion("too many spans");
                q = cur_span;
                while (mem[mem[q].b32.s0].b32.s1 < n)
                    q = mem[q].b32.s0;
                if (mem[mem[q].b32.s0].b32.s1 > n) {
                    s = get_node(SPAN_NODE_SIZE);
                    mem[s].b32.s0 = mem[q].b32.s0;
                    mem[s].b32.s1 = n;
                    mem[q].b32.s0 = s;
                    mem[s + 1].b32.s1 = w;
                } else if (mem[mem[q].b32.s0 + 1].b32.s1 < w)
                    mem[mem[q].b32.s0 + 1].b32.s1 = w;
            } else if (w > mem[cur_align + 1].b32.s1)
                mem[cur_align + 1].b32.s1 = w;
            NODE_type(u) = UNSET_NODE;
            mem[u].b16.s0 = n;
            if (total_stretch[FILLL] != 0)
                o = FILLL;
            else if (total_stretch[FILL] != 0)
                o = FILL;
            else if (total_stretch[FIL] != 0)
                o = FIL;
            else
                o = 0 /*normal *//*:684 */ ;
            mem[u + 5].b16.s0 = o;
            mem[u + 6].b32.s1 = total_stretch[o];
            if (total_shrink[FILLL] != 0)
                o = FILLL;
            else if (total_shrink[FILL] != 0)
                o = FILL;
            else if (total_shrink[FIL] != 0)
                o = FIL;
            else
                o = 0 /*normal *//*:690 */ ;
            mem[u + 5].b16.s1 = o;
            mem[u + 4].b32.s1 = total_shrink[o];
            pop_nest();
            mem[cur_list.tail].b32.s1 = u;
            cur_list.tail = u;
        }
        {
            mem[cur_list.tail].b32.s1 = new_glue(mem[mem[cur_align].b32.s1 + 1].b32.s0);
            cur_list.tail = LLIST_link(cur_list.tail);
        }
        mem[cur_list.tail].b16.s0 = 12 /*tab_skip_code 1 *//*:824 */ ;
        if (mem[cur_align + 5].b32.s0 >= CR_CODE) {
            return true;
        }
        init_span(p);
    }
    align_state = 1000000L;
    do {
        get_x_or_protected();
    } while (!(cur_cmd != SPACER));
    cur_align = p;
    init_col();
    return false;
}

void fin_row(void)
{
    int32_t p;

    if (cur_list.mode == -104) {
        p = hpack(mem[cur_list.head].b32.s1, 0, ADDITIONAL);
        pop_nest();
        if (cur_pre_head != cur_pre_tail) {
            mem[cur_list.tail].b32.s1 = mem[cur_pre_head].b32.s1;
            cur_list.tail = cur_pre_tail;
        }
        append_to_vlist(p);
        if (cur_head != cur_tail) {
            mem[cur_list.tail].b32.s1 = mem[cur_head].b32.s1;
            cur_list.tail = cur_tail;
        }
    } else {

        p = vpackage(mem[cur_list.head].b32.s1, 0, ADDITIONAL, MAX_HALFWORD);
        pop_nest();
        mem[cur_list.tail].b32.s1 = p;
        cur_list.tail = p;
        cur_list.aux.b32.s0 = 1000;
    }
    NODE_type(p) = UNSET_NODE;
    mem[p + 6].b32.s1 = 0;
    if (LOCAL(every_cr) != TEX_NULL)
        begin_token_list(LOCAL(every_cr), EVERY_CR_TEXT);
    align_peek();
}

void fin_align(void)
{
    int32_t p, q, r, s, u, v;
    scaled_t t, w;
    scaled_t o;
    int32_t n;
    scaled_t rule_save;
    memory_word aux_save;

    if (cur_group != ALIGN_GROUP)
        confusion("align1");
    unsave();
    if (cur_group != ALIGN_GROUP)
        confusion("align0");
    unsave();
    if (nest[nest_ptr - 1].mode == MMODE)
        o = DIMENPAR(display_indent);
    else
        o = 0;
    q = mem[mem[ALIGN_HEAD].b32.s1].b32.s1;
    do {
        flush_list(mem[q + 3].b32.s1);
        flush_list(mem[q + 2].b32.s1);
        p = mem[mem[q].b32.s1].b32.s1;
        if (mem[q + 1].b32.s1 == NULL_FLAG) {  /*831: */
            mem[q + 1].b32.s1 = 0;
            r = mem[q].b32.s1;
            s = mem[r + 1].b32.s0;
            if (s != 0) {
                GLUE_SPEC_ref_count(0)++;
                delete_glue_ref(s);
                mem[r + 1].b32.s0 = 0;
            }
        }
        if (mem[q].b32.s0 != END_SPAN) {    /*832: */
            t = mem[q + 1].b32.s1 + mem[mem[mem[q].b32.s1 + 1].b32.s0 + 1].b32.s1;
            r = mem[q].b32.s0;
            s = END_SPAN;
            mem[s].b32.s0 = p;
            n = 1;
            do {
                mem[r + 1].b32.s1 = mem[r + 1].b32.s1 - t;
                u = mem[r].b32.s0;
                while (mem[r].b32.s1 > n) {

                    s = mem[s].b32.s0;
                    n = mem[mem[s].b32.s0].b32.s1 + 1;
                }
                if (mem[r].b32.s1 < n) {
                    mem[r].b32.s0 = mem[s].b32.s0;
                    mem[s].b32.s0 = r;
                    mem[r].b32.s1--;
                    s = r;
                } else {

                    if (mem[r + 1].b32.s1 > mem[mem[s].b32.s0 + 1].b32.s1)
                        mem[mem[s].b32.s0 + 1].b32.s1 = mem[r + 1].b32.s1;
                    free_node(r, SPAN_NODE_SIZE);
                }
                r = u;
            } while (!(r == END_SPAN));
        }
        NODE_type(q) = UNSET_NODE;
        mem[q].b16.s0 = 0;
        mem[q + 3].b32.s1 = 0;
        mem[q + 2].b32.s1 = 0;
        mem[q + 5].b16.s0 = NORMAL;
        mem[q + 5].b16.s1 = NORMAL;
        mem[q + 6].b32.s1 = 0;
        mem[q + 4].b32.s1 = 0;
        q = p;
    } while (!(q == TEX_NULL /*:830 */ ));
    save_ptr = save_ptr - 2;
    pack_begin_line = -(int32_t) cur_list.mode_line;
    if (cur_list.mode == -1) {
        rule_save = DIMENPAR(overfull_rule);
        DIMENPAR(overfull_rule) = 0;
        p = hpack(mem[ALIGN_HEAD].b32.s1, save_stack[save_ptr + 1].b32.s1, save_stack[save_ptr + 0].b32.s1);
        DIMENPAR(overfull_rule) = rule_save;
    } else {

        q = mem[mem[ALIGN_HEAD].b32.s1].b32.s1;
        do {
            mem[q + 3].b32.s1 = mem[q + 1].b32.s1;
            mem[q + 1].b32.s1 = 0;
            q = mem[mem[q].b32.s1].b32.s1;
        } while (!(q == TEX_NULL));
        p = vpackage(mem[ALIGN_HEAD].b32.s1, save_stack[save_ptr + 1].b32.s1, save_stack[save_ptr + 0].b32.s1,
                     MAX_HALFWORD);
        q = mem[mem[ALIGN_HEAD].b32.s1].b32.s1;
        do {
            mem[q + 1].b32.s1 = mem[q + 3].b32.s1;
            mem[q + 3].b32.s1 = 0;
            q = mem[mem[q].b32.s1].b32.s1;
        } while (!(q == TEX_NULL));
    }
    pack_begin_line = 0 /*:833 */ ;
    q = mem[cur_list.head].b32.s1;
    s = cur_list.head;
    while (q != TEX_NULL) {

        if (!(is_char_node(q))) {

            if (NODE_type(q) == UNSET_NODE) {  /*836: */
                if (cur_list.mode == -1) {
                    NODE_type(q) = HLIST_NODE;
                    mem[q + 1].b32.s1 = mem[p + 1].b32.s1;
                    if (nest[nest_ptr - 1].mode == MMODE)
                        mem[q].b16.s0 = DLIST;
                } else {

                    NODE_type(q) = VLIST_NODE;
                    mem[q + 3].b32.s1 = mem[p + 3].b32.s1;
                }
                mem[q + 5].b16.s0 = mem[p + 5].b16.s0;
                mem[q + 5].b16.s1 = mem[p + 5].b16.s1;
                BOX_glue_set(q) = BOX_glue_set(p);
                mem[q + 4].b32.s1 = o;
                r = mem[mem[q + 5].b32.s1].b32.s1;
                s = mem[mem[p + 5].b32.s1].b32.s1;
                do {
                    /*837: */ n = mem[r].b16.s0;
                    t = mem[s + 1].b32.s1;
                    w = t;
                    u = HOLD_HEAD;
                    mem[r].b16.s0 = 0;
                    while (n > 0) {

                        n--;
                        s = LLIST_link(s);
                        v = mem[s + 1].b32.s0;
                        mem[u].b32.s1 = new_glue(v);
                        u = LLIST_link(u);
                        mem[u].b16.s0 = (GLUE_PAR__tab_skip + 1);
                        t = t + mem[v + 1].b32.s1;
                        if (mem[p + 5].b16.s1 == STRETCHING) {
                            if (mem[v].b16.s1 == mem[p + 5].b16.s0)
                                t = t + tex_round(BOX_glue_set(p) * mem[v + 2].b32.s1);
                        } else if (mem[p + 5].b16.s1 == SHRINKING) {
                            if (mem[v].b16.s0 == mem[p + 5].b16.s0)
                                t = t - tex_round(BOX_glue_set(p) * mem[v + 3].b32.s1);
                        }
                        s = LLIST_link(s);
                        mem[u].b32.s1 = new_null_box();
                        u = LLIST_link(u);
                        t = t + mem[s + 1].b32.s1;
                        if (cur_list.mode == -1)
                            mem[u + 1].b32.s1 = mem[s + 1].b32.s1;
                        else {

                            NODE_type(u) = VLIST_NODE;
                            mem[u + 3].b32.s1 = mem[s + 1].b32.s1;
                        }
                    }
                    if (cur_list.mode == -1) {    /*839: */
                        mem[r + 3].b32.s1 = mem[q + 3].b32.s1;
                        mem[r + 2].b32.s1 = mem[q + 2].b32.s1;
                        if (t == mem[r + 1].b32.s1) {
                            mem[r + 5].b16.s1 = NORMAL;
                            mem[r + 5].b16.s0 = NORMAL;
                            BOX_glue_set(r) = 0.0;
                        } else if (t > mem[r + 1].b32.s1) {
                            mem[r + 5].b16.s1 = STRETCHING;
                            if (mem[r + 6].b32.s1 == 0)
                                BOX_glue_set(r) = 0.0;
                            else
                                BOX_glue_set(r) = (t - mem[r + 1].b32.s1) / ((double)mem[r + 6].b32.s1);
                        } else {

                            mem[r + 5].b16.s0 = mem[r + 5].b16.s1;
                            mem[r + 5].b16.s1 = SHRINKING;
                            if (mem[r + 4].b32.s1 == 0)
                                BOX_glue_set(r) = 0.0;
                            else if ((mem[r + 5].b16.s0 == NORMAL) && (mem[r + 1].b32.s1 - t > mem[r + 4].b32.s1))
                                BOX_glue_set(r) = 1.0;
                            else
                                BOX_glue_set(r) = (mem[r + 1].b32.s1 - t) / ((double)mem[r + 4].b32.s1);
                        }
                        mem[r + 1].b32.s1 = w;
                        NODE_type(r) = HLIST_NODE;
                    } else {    /*840: */

                        mem[r + 1].b32.s1 = mem[q + 1].b32.s1;
                        if (t == mem[r + 3].b32.s1) {
                            mem[r + 5].b16.s1 = NORMAL;
                            mem[r + 5].b16.s0 = NORMAL;
                            BOX_glue_set(r) = 0.0;
                        } else if (t > mem[r + 3].b32.s1) {
                            mem[r + 5].b16.s1 = STRETCHING;
                            if (mem[r + 6].b32.s1 == 0)
                                BOX_glue_set(r) = 0.0;
                            else
                                BOX_glue_set(r) = (t - mem[r + 3].b32.s1) / ((double)mem[r + 6].b32.s1);
                        } else {

                            mem[r + 5].b16.s0 = mem[r + 5].b16.s1;
                            mem[r + 5].b16.s1 = SHRINKING;
                            if (mem[r + 4].b32.s1 == 0)
                                BOX_glue_set(r) = 0.0;
                            else if ((mem[r + 5].b16.s0 == NORMAL) && (mem[r + 3].b32.s1 - t > mem[r + 4].b32.s1))
                                BOX_glue_set(r) = 1.0;
                            else
                                BOX_glue_set(r) = (mem[r + 3].b32.s1 - t) / ((double)mem[r + 4].b32.s1);
                        }
                        mem[r + 3].b32.s1 = w;
                        NODE_type(r) = VLIST_NODE;
                    }
                    mem[r + 4].b32.s1 = 0;
                    if (u != HOLD_HEAD) {
                        mem[u].b32.s1 = mem[r].b32.s1;
                        mem[r].b32.s1 = mem[HOLD_HEAD].b32.s1;
                        r = u;
                    }
                    r = mem[mem[r].b32.s1].b32.s1;
                    s = mem[mem[s].b32.s1].b32.s1;
                } while (!(r == TEX_NULL));
            } else if (NODE_type(q) == RULE_NODE) {     /*835: */
                if (mem[q + 1].b32.s1 == NULL_FLAG)
                    mem[q + 1].b32.s1 = mem[p + 1].b32.s1;
                if (mem[q + 3].b32.s1 == NULL_FLAG)
                    mem[q + 3].b32.s1 = mem[p + 3].b32.s1;
                if (mem[q + 2].b32.s1 == NULL_FLAG)
                    mem[q + 2].b32.s1 = mem[p + 2].b32.s1;
                if (o != 0) {
                    r = mem[q].b32.s1;
                    mem[q].b32.s1 = TEX_NULL;
                    q = hpack(q, 0, ADDITIONAL);
                    mem[q + 4].b32.s1 = o;
                    mem[q].b32.s1 = r;
                    mem[s].b32.s1 = q;
                }
            }
        }
        s = q;
        q = LLIST_link(q);
    }
    flush_node_list(p);
    pop_alignment();
    aux_save = cur_list.aux;
    p = mem[cur_list.head].b32.s1;
    q = cur_list.tail;
    pop_nest();
    if (cur_list.mode == MMODE) {       /*1241: */
        do_assignments();
        if (cur_cmd != MATH_SHIFT) {    /*1242: */
            error_here_with_diagnostic("Missing $$ inserted");
            capture_to_diagnostic(NULL);
            {
                help_ptr = 2;
                help_line[1] = "Displays can use special alignments (like \\eqalignno)";
                help_line[0] = "only if nothing but the alignment itself is between $$'s.";
            }
            back_error();
        } else {                /*1232: */

            get_x_token();
            if (cur_cmd != MATH_SHIFT) {
                error_here_with_diagnostic("Display math should end with $$");
                capture_to_diagnostic(NULL);
                {
                    help_ptr = 2;
                    help_line[1] = "The `$' that I just saw supposedly matches a previous `$$'.";
                    help_line[0] = "So I shall assume that you typed `$$' both times.";
                }
                back_error();
            }
        }
        flush_node_list(cur_list.eTeX_aux);
        pop_nest();
        {
            mem[cur_list.tail].b32.s1 = new_penalty(INTPAR(pre_display_penalty));
            cur_list.tail = LLIST_link(cur_list.tail);
        }
        {
            mem[cur_list.tail].b32.s1 = new_param_glue(GLUE_PAR__above_display_skip);
            cur_list.tail = LLIST_link(cur_list.tail);
        }
        mem[cur_list.tail].b32.s1 = p;
        if (p != TEX_NULL)
            cur_list.tail = q;
        {
            mem[cur_list.tail].b32.s1 = new_penalty(INTPAR(post_display_penalty));
            cur_list.tail = LLIST_link(cur_list.tail);
        }
        {
            mem[cur_list.tail].b32.s1 = new_param_glue(GLUE_PAR__below_display_skip);
            cur_list.tail = LLIST_link(cur_list.tail);
        }
        cur_list.aux.b32.s1 = aux_save.b32.s1;
        resume_after_display();
    } else {

        cur_list.aux = aux_save;
        mem[cur_list.tail].b32.s1 = p;
        if (p != TEX_NULL)
            cur_list.tail = q;
        if (cur_list.mode == VMODE)
            build_page();
    }
}

void align_peek(void)
{
restart:
    align_state = 1000000L;

    do {
        get_x_or_protected();
    } while (!(cur_cmd != SPACER));
    if (cur_cmd == NO_ALIGN) {
        scan_left_brace();
        new_save_level(NO_ALIGN_GROUP);
        if (cur_list.mode == -1)
            normal_paragraph();
    } else if (cur_cmd == RIGHT_BRACE)
        fin_align();
    else if ((cur_cmd == CAR_RET) && (cur_chr == CR_CR_CODE))
        goto restart;
    else {

        init_row();
        init_col();
    }
}

int32_t max_hyphenatable_length(void)
{

    if (INTPAR(xetex_hyphenatable_length) > HYPHENATABLE_LENGTH_LIMIT)
        return HYPHENATABLE_LENGTH_LIMIT;
    return INTPAR(xetex_hyphenatable_length);
}

bool eTeX_enabled(bool b, uint16_t j, int32_t k)
{
    if (!b) {
        error_here_with_diagnostic("Improper ");
        print_cmd_chr(j, k);
        capture_to_diagnostic(NULL);

        {
            help_ptr = 1;
            help_line[0] = "Sorry, this optional e-TeX feature has been disabled.";
        }
        error();
    }
    return b;
}


void
show_save_groups(void)
{
    int32_t p;
    short /*mmode */ m;
    save_pointer v;
    uint16_t l;
    group_code c;
    signed char a;
    int32_t i;
    uint16_t j;
    const char * s = NULL;

    p = nest_ptr;
    nest[p] = cur_list;
    v = save_ptr;
    l = cur_level;
    c = cur_group;
    save_ptr = cur_boundary;
    cur_level--;
    a = 1;

    print_nl_cstr("");
    print_ln();

    while (true) {
        print_nl_cstr("### ");
        print_group(true);

        if (cur_group == BOTTOM_LEVEL)
            goto done;

        do {
            m = nest[p].mode;
            if (p > 0)
                p--;
            else
                m = VMODE;
        } while (m == HMODE);

        print_cstr(" (");

        switch (cur_group) {
        case SIMPLE_GROUP:
            p++;
            goto found2;
            break;

        case HBOX_GROUP:
        case ADJUSTED_HBOX_GROUP:
            s = "hbox";
            break;

        case VBOX_GROUP:
            s = "vbox";
            break;

        case VTOP_GROUP:
            s = "vtop";
            break;

        case ALIGN_GROUP:
            if (a == 0) {
                if (m == -VMODE)
                    s = "halign";
                else
                    s = "valign";
                a = 1;
                goto found1;
            } else {
                if (a == 1)
                    print_cstr("align entry");
                else
                    print_esc_cstr("cr");

                if (p >= a)
                    p = p - a;
                a = 0;
                goto found;
            }
            break;

        case NO_ALIGN_GROUP:
            p++;
            a = -1;
            print_esc_cstr("noalign");
            goto found2;
            break;

        case OUTPUT_GROUP:
            print_esc_cstr("output");
            goto found;
            break;

        case MATH_GROUP:
            goto found2;
            break;

        case DISC_GROUP:
        case MATH_CHOICE_GROUP:
            if (cur_group == DISC_GROUP)
                print_esc_cstr("discretionary");
            else
                print_esc_cstr("mathchoice");

            for (i = 1; i <= 3; i++) {
                if (i <= save_stack[save_ptr - 2].b32.s1)
                    print_cstr("{}");
            }
            goto found2;
            break;

        case INSERT_GROUP:
            if (save_stack[save_ptr - 2].b32.s1 == 255) {
                print_esc_cstr("vadjust");
            } else {
                print_esc_cstr("insert");
                print_int(save_stack[save_ptr - 2].b32.s1);
            }
            goto found2;
            break;

        case VCENTER_GROUP:
            s = "vcenter";
            goto found1;
            break;

        case SEMI_SIMPLE_GROUP:
            p++;
            print_esc_cstr("begingroup");
            goto found;
            break;

        case MATH_SHIFT_GROUP:
            if (m == MMODE) {
                print_char('$');
            } else if (nest[p].mode == MMODE) {
                print_cmd_chr(EQ_NO, save_stack[save_ptr - 2].b32.s1);
                goto found;
            }

            print_char('$');
            goto found;
            break;

        case MATH_LEFT_GROUP:
            if (mem[nest[p + 1].eTeX_aux].b16.s1 == LEFT_NOAD)
                print_esc_cstr("left");
            else
                print_esc_cstr("middle");
            goto found;
            break;
        }

        i = save_stack[save_ptr - 4].b32.s1;

        if (i != 0) {
            if (i < BOX_FLAG) {
                if (abs(nest[p].mode) == VMODE)
                    j = HMOVE;
                else
                    j = VMOVE;

                if (i > 0)
                    print_cmd_chr(j, 0);
                else
                    print_cmd_chr(j, 1);

                print_scaled(abs(i));
                print_cstr("pt");
            } else if (i < SHIP_OUT_FLAG) {
                if (i >= GLOBAL_BOX_FLAG) {
                    print_esc_cstr("global");
                    i = i - (GLOBAL_BOX_FLAG - BOX_FLAG);
                }

                print_esc_cstr("setbox");
                print_int(i - BOX_FLAG);
                print_char('=');
            } else {
                print_cmd_chr(LEADER_SHIP, i - (LEADER_FLAG - A_LEADERS));
            }
        }

    found1:
        print_esc_cstr(s);
        if (save_stack[save_ptr - 2].b32.s1 != 0) {
            print_char(' ');
            if (save_stack[save_ptr - 3].b32.s1 == EXACTLY)
                print_cstr("to");
            else
                print_cstr("spread");
            print_scaled(save_stack[save_ptr - 2].b32.s1);
            print_cstr("pt");
        }

    found2:
        print_char('{');

    found:
        print_char(')');
        cur_level--;
        cur_group = save_stack[save_ptr].b16.s0;
        save_ptr = save_stack[save_ptr].b32.s1;
    }

done:
    save_ptr = v;
    cur_level = l;
    cur_group = c;
}


int32_t vert_break(int32_t p, scaled_t h, scaled_t d)
{
    int32_t prev_p;
    int32_t q, r;
    int32_t pi;
    int32_t b;
    int32_t least_cost;
    int32_t best_place = TEX_NULL;
    scaled_t prev_dp;
    small_number t;
    prev_p = p;
    least_cost = MAX_HALFWORD;
    active_width[1] = 0;
    active_width[2] = 0;
    active_width[3] = 0;
    active_width[4] = 0;
    active_width[5] = 0;
    active_width[6] = 0;
    prev_dp = 0;
    while (true) {

        if (p == TEX_NULL)
            pi = EJECT_PENALTY;
        else /*1008: */
            switch (mem[p].b16.s1) {
            case 0:
            case 1:
            case 2:
                active_width[1] = active_width[1] + prev_dp + mem[p + 3].b32.s1;
                prev_dp = mem[p + 2].b32.s1;
                goto not_found;
            case 8:
                if ((mem[p].b16.s0 == PIC_NODE) || (mem[p].b16.s0 == PDF_NODE)) {
                    active_width[1] = active_width[1] + prev_dp + mem[p + 3].b32.s1;
                    prev_dp = mem[p + 2].b32.s1;
                }
                goto not_found;
            case 10:
                if ((is_non_discardable_node(prev_p))) {
                    pi = 0;
                    break;
                } else {
                    goto lab90;
                }
            case 11:
                if (mem[p].b32.s1 == TEX_NULL) {
                    t = PENALTY_NODE;
                } else {
                    t = mem[mem[p].b32.s1].b16.s1;
                }
                if (t == GLUE_NODE)  {
                    pi = 0;
                    break;
                } else {
                    goto lab90;
                }
            case 12:
                pi = mem[p + 1].b32.s1;
                break;
            case 4:
            case 3:
                goto not_found;
            default:
                confusion("vertbreak");
                break;
            }
        if (pi < INF_PENALTY) {
            if (active_width[1] < h) {

                if ((active_width[3] != 0) || (active_width[4] != 0) || (active_width[5] != 0))
                    b = 0;
                else
                    b = badness(h - active_width[1], active_width[2]);
            } else if (active_width[1] - h > active_width[6])
                b = MAX_HALFWORD;
            else
                b = badness(active_width[1] - h, active_width[6]) /*:1010 */ ;
            if (b < MAX_HALFWORD) {

                if (pi <= EJECT_PENALTY)
                    b = pi;
                else if (b < INF_BAD)
                    b = b + pi;
                else
                    b = 100000L;
            }
            if (b <= least_cost) {
                best_place = p;
                least_cost = b;
                best_height_plus_depth = active_width[1] + prev_dp;
            }
            if ((b == MAX_HALFWORD) || (pi <= EJECT_PENALTY))
                goto done;
        }
        if ((NODE_type(p) < GLUE_NODE) || (NODE_type(p) > KERN_NODE))
            goto not_found;
 lab90:/*update_heights *//*1011: */ if (NODE_type(p) == KERN_NODE)
            q = p;
        else {

            q = mem[p + 1].b32.s0;
            active_width[2 + mem[q].b16.s1] = active_width[2 + mem[q].b16.s1] + mem[q + 2].b32.s1;
            active_width[6] = active_width[6] + mem[q + 3].b32.s1;
            if ((mem[q].b16.s0 != NORMAL) && (mem[q + 3].b32.s1 != 0)) {
                error_here_with_diagnostic("Infinite glue shrinkage found in box being split");
                capture_to_diagnostic(NULL);
                {
                    help_ptr = 4;
                    help_line[3] = "The box you are \\vsplitting contains some infinitely";
                    help_line[2] = "shrinkable glue, e.g., `\\vss' or `\\vskip 0pt minus 1fil'.";
                    help_line[1] = "Such glue doesn't belong there; but you can safely proceed,";
                    help_line[0] = "since the offensive shrinkability has been made finite.";
                }
                error();
                r = new_spec(q);
                GLUE_SPEC_shrink_order(r) = NORMAL;
                delete_glue_ref(q);
                mem[p + 1].b32.s0 = r;
                q = r;
            }
        }
        active_width[1] = active_width[1] + prev_dp + mem[q + 1].b32.s1;
        prev_dp = 0 /*:1011 */ ;
    not_found:
        if (prev_dp > d) {
            active_width[1] = active_width[1] + prev_dp - d;
            prev_dp = d;
        }
        prev_p = p;
        p = mem[prev_p].b32.s1;
    }
done:
    return best_place;
}

int32_t vsplit(int32_t n, scaled_t h)
{
    int32_t v;
    int32_t p;
    int32_t q;

    cur_val = n;
    if (cur_val < 256)
        v = BOX_REG(cur_val);
    else {

        find_sa_element(4, cur_val, false);
        if (cur_ptr == TEX_NULL)
            v = TEX_NULL;
        else
            v = mem[cur_ptr + 1].b32.s1;
    }
    flush_node_list(disc_ptr[VSPLIT_CODE]);
    disc_ptr[VSPLIT_CODE] = TEX_NULL;
    if (sa_root[MARK_VAL] != TEX_NULL) {

        if (do_marks(0, 0, sa_root[MARK_VAL]))
            sa_root[MARK_VAL] = TEX_NULL;
    }
    if (cur_mark[SPLIT_FIRST_MARK_CODE] != TEX_NULL) {
        delete_token_ref(cur_mark[SPLIT_FIRST_MARK_CODE]);
        cur_mark[SPLIT_FIRST_MARK_CODE] = TEX_NULL;
        delete_token_ref(cur_mark[SPLIT_BOT_MARK_CODE]);
        cur_mark[SPLIT_BOT_MARK_CODE] = TEX_NULL;
    }
    if (v == TEX_NULL) {
        return TEX_NULL;
    }
    if (NODE_type(v) != VLIST_NODE) {
        error_here_with_diagnostic("");
        print_esc_cstr("vsplit");
        print_cstr(" needs a ");
        print_esc_cstr("vbox");
        capture_to_diagnostic(NULL);

        {
            help_ptr = 2;
            help_line[1] = "The box you are trying to split is an \\hbox.";
            help_line[0] = "I can't split such a box, so I'll leave it alone.";
        }
        error();
        return TEX_NULL;
    }
    q = vert_break(mem[v + 5].b32.s1, h, DIMENPAR(split_max_depth));
    p = mem[v + 5].b32.s1;
    if (p == q)
        mem[v + 5].b32.s1 = TEX_NULL;
    else
        while (true) {

            if (NODE_type(p) == MARK_NODE) {

                if (mem[p + 1].b32.s0 != 0) {  /*1615: */
                    find_sa_element(MARK_VAL, mem[p + 1].b32.s0, true);
                    if (mem[cur_ptr + 2].b32.s1 == TEX_NULL) {
                        mem[cur_ptr + 2].b32.s1 = mem[p + 1].b32.s1;
                        mem[mem[p + 1].b32.s1].b32.s0++;
                    } else
                        delete_token_ref(mem[cur_ptr + 3].b32.s0);
                    mem[cur_ptr + 3].b32.s0 = mem[p + 1].b32.s1;
                    mem[mem[p + 1].b32.s1].b32.s0++;
                } else if (cur_mark[SPLIT_FIRST_MARK_CODE] == TEX_NULL) {
                    cur_mark[SPLIT_FIRST_MARK_CODE] = mem[p + 1].b32.s1;
                    cur_mark[SPLIT_BOT_MARK_CODE] = cur_mark[SPLIT_FIRST_MARK_CODE];
                    mem[cur_mark[SPLIT_FIRST_MARK_CODE]].b32.s0 =
                        mem[cur_mark[SPLIT_FIRST_MARK_CODE]].b32.s0 + 2;
                } else {

                    delete_token_ref(cur_mark[SPLIT_BOT_MARK_CODE]);
                    cur_mark[SPLIT_BOT_MARK_CODE] = mem[p + 1].b32.s1;
                    mem[cur_mark[SPLIT_BOT_MARK_CODE]].b32.s0++;
                }
            }
            if (mem[p].b32.s1 == q) {
                mem[p].b32.s1 = TEX_NULL;
                goto done;
            }
            p = LLIST_link(p);
        } /*:1014*/
done:
    q = prune_page_top(q, INTPAR(saving_vdiscards) > 0);
    p = mem[v + 5].b32.s1;
    free_node(v, BOX_NODE_SIZE);
    if (q != TEX_NULL)
        q = vpackage(q, 0, ADDITIONAL, MAX_HALFWORD);
    if (cur_val < 256)
        BOX_REG(cur_val) = q;
    else {

        find_sa_element(4, cur_val, false);
        if (cur_ptr != TEX_NULL) {
            mem[cur_ptr + 1].b32.s1 = q;
            mem[cur_ptr + 1].b32.s0++;
            delete_sa_ref(cur_ptr);
        }
    }
    return vpackage(p, h, EXACTLY, DIMENPAR(split_max_depth));
}

void print_totals(void)
{
    print_scaled(page_so_far[1]);
    if (page_so_far[2] != 0) {
        print_cstr(" plus ");
        print_scaled(page_so_far[2]);
        print_cstr("");
    }
    if (page_so_far[3] != 0) {
        print_cstr(" plus ");
        print_scaled(page_so_far[3]);
        print_cstr("fil");
    }
    if (page_so_far[4] != 0) {
        print_cstr(" plus ");
        print_scaled(page_so_far[4]);
        print_cstr("fill");
    }
    if (page_so_far[5] != 0) {
        print_cstr(" plus ");
        print_scaled(page_so_far[5]);
        print_cstr("filll");
    }
    if (page_so_far[6] != 0) {
        print_cstr(" minus ");
        print_scaled(page_so_far[6]);
    }
}

void box_error(eight_bits n)
{
    error();

    begin_diagnostic();
    diagnostic_begin_capture_warning_here();

    print_nl_cstr("The following box has been deleted:");
    show_box(BOX_REG(n));

    capture_to_diagnostic(NULL);
    end_diagnostic(true);

    flush_node_list(BOX_REG(n));
    BOX_REG(n) = TEX_NULL;
}


void app_space(void)
{
    int32_t q;

    if ((cur_list.aux.b32.s0 >= 2000) && (GLUEPAR(xspace_skip) != 0))
        q = new_param_glue(GLUE_PAR__xspace_skip);
    else {

        if (GLUEPAR(space_skip) != 0)
            main_p = GLUEPAR(space_skip);
        else {                  /*1077: */

            main_p = font_glue[eqtb[CUR_FONT_LOC].b32.s1];
            if (main_p == TEX_NULL) {
                main_p = new_spec(0);
                main_k = param_base[eqtb[CUR_FONT_LOC].b32.s1] + 2;
                mem[main_p + 1].b32.s1 = font_info[main_k].b32.s1;
                mem[main_p + 2].b32.s1 = font_info[main_k + 1].b32.s1;
                mem[main_p + 3].b32.s1 = font_info[main_k + 2].b32.s1;
                font_glue[eqtb[CUR_FONT_LOC].b32.s1] = main_p;
            }
        }
        main_p = new_spec(main_p);
        if (cur_list.aux.b32.s0 >= 2000)
            mem[main_p + 1].b32.s1 =
                mem[main_p + 1].b32.s1 + font_info[EXTRA_SPACE_CODE +
                                                 param_base[eqtb[CUR_FONT_LOC].b32.s1]].b32.s1;
        mem[main_p + 2].b32.s1 = xn_over_d(mem[main_p + 2].b32.s1, cur_list.aux.b32.s0, 1000);
        mem[main_p + 3].b32.s1 = xn_over_d(mem[main_p + 3].b32.s1, 1000, cur_list.aux.b32.s0) /*:1079 */ ;
        q = new_glue(main_p);
        mem[main_p].b32.s1 = TEX_NULL;
    }
    mem[cur_list.tail].b32.s1 = q;
    cur_list.tail = q;
}

void
insert_dollar_sign(void)
{
    back_input();
    cur_tok = (MATH_SHIFT_TOKEN + 36 /*'$'*/);

    error_here_with_diagnostic("Missing $ inserted");
    capture_to_diagnostic(NULL);

    help_ptr = 2;
    help_line[1] = "I've inserted a begin-math/end-math symbol since I think";
    help_line[0] = "you left one out. Proceed, with fingers crossed.";
    ins_error();
}

void you_cant(void)
{
    error_here_with_diagnostic("You can't use `");
    print_cmd_chr(cur_cmd, cur_chr);
    print_in_mode(cur_list.mode);
    capture_to_diagnostic(NULL);
}

void report_illegal_case(void)
{
    you_cant();
    {
        help_ptr = 4;
        help_line[3] = "Sorry, but I'm not programmed to handle this case;";
        help_line[2] = "I'll just pretend that you didn't ask for it.";
        help_line[1] = "If you're in the wrong mode, you might be able to";
        help_line[0] = "return to the right one by typing `I}' or `I$' or `I\\par'.";
    }
    error();
}

bool privileged(void)
{
    if (cur_list.mode > 0) {
        return true;
    } else {
        report_illegal_case();
        return false;
    }
}

bool its_all_over(void)
{

    if (privileged()) {
        if ((PAGE_HEAD == page_tail) && (cur_list.head == cur_list.tail) && (dead_cycles == 0)) {
            return true;
        }
        back_input();
        {
            mem[cur_list.tail].b32.s1 = new_null_box();
            cur_list.tail = LLIST_link(cur_list.tail);
        }
        mem[cur_list.tail + 1].b32.s1 = DIMENPAR(hsize);
        {
            mem[cur_list.tail].b32.s1 = new_glue(8);
            cur_list.tail = LLIST_link(cur_list.tail);
        }
        {
            mem[cur_list.tail].b32.s1 = new_penalty(NULL_FLAG);
            cur_list.tail = LLIST_link(cur_list.tail);
        }
        build_page();
    }
    return false;
}

void append_glue(void)
{
    small_number s;
    s = cur_chr;
    switch (s) {
    case 0:
        cur_val = 4;
        break;
    case 1:
        cur_val = 8;
        break;
    case 2:
        cur_val = 12;
        break;
    case 3:
        cur_val = 16;
        break;
    case 4:
        scan_glue(GLUE_VAL);
        break;
    case 5:
        scan_glue(MU_VAL);
        break;
    }
    {
        mem[cur_list.tail].b32.s1 = new_glue(cur_val);
        cur_list.tail = LLIST_link(cur_list.tail);
    }
    if (s >= SKIP_CODE) {
        mem[cur_val].b32.s1--;
        if (s > SKIP_CODE)
            mem[cur_list.tail].b16.s0 = MU_GLUE;
    }
}

void append_kern(void)
{
    uint16_t s;
    s = cur_chr;
    scan_dimen(s == MU_GLUE, false, false);
    {
        mem[cur_list.tail].b32.s1 = new_kern(cur_val);
        cur_list.tail = LLIST_link(cur_list.tail);
    }
    mem[cur_list.tail].b16.s0 = s;
}


void
off_save(void)
{
    int32_t p;

    if (cur_group == BOTTOM_LEVEL) { /*1101:*/
        error_here_with_diagnostic("Extra ");
        print_cmd_chr(cur_cmd, cur_chr);
        capture_to_diagnostic(NULL);

        help_ptr = 1;
        help_line[0] = "Things are pretty mixed up, but I think the worst is over.";
        error();
    } else {
        back_input();
        p = get_avail();
        mem[TEMP_HEAD].b32.s1 = p;

        error_here_with_diagnostic("Missing ");

        switch (cur_group) {
        case SEMI_SIMPLE_GROUP:
            mem[p].b32.s0 = CS_TOKEN_FLAG + FROZEN_END_GROUP;
            print_esc_cstr("endgroup");
            break;
        case MATH_SHIFT_GROUP:
            mem[p].b32.s0 = MATH_SHIFT_TOKEN + '$' ;
            print_char('$');
            break;
        case MATH_LEFT_GROUP:
            mem[p].b32.s0 = CS_TOKEN_FLAG + FROZEN_RIGHT;
            mem[p].b32.s1 = get_avail();
            p = LLIST_link(p);
            mem[p].b32.s0 = OTHER_TOKEN + '.' ;
            print_esc_cstr("right.");
            break;
        default:
            mem[p].b32.s0 = (RIGHT_BRACE_TOKEN + '}' );
            print_char('}');
            break;
        }

        print_cstr(" inserted");
        begin_token_list(mem[TEMP_HEAD].b32.s1, INSERTED);
        capture_to_diagnostic(NULL);

        help_ptr = 5;
        help_line[4] = "I've inserted something that you may have forgotten.";
        help_line[3] = "(See the <inserted text> above.)";
        help_line[2] = "With luck, this will get me unwedged. But if you";
        help_line[1] = "really didn't forget anything, try typing `2' now; then";
        help_line[0] = "my insertion and my current dilemma will both disappear.";
        error();
    }
}


void
extra_right_brace(void)
{
    error_here_with_diagnostic("Extra }, or forgotten ");

    switch (cur_group) {
    case SEMI_SIMPLE_GROUP:
        print_esc_cstr("endgroup");
        break;
    case MATH_SHIFT_GROUP:
        print_char('$');
        break;
    case MATH_LEFT_GROUP:
        print_esc_cstr("right");
        break;
    }

    capture_to_diagnostic(NULL);

    help_ptr = 5;
    help_line[4] = "I've deleted a group-closing symbol because it seems to be";
    help_line[3] = "spurious, as in `$x}$'. But perhaps the } is legitimate and";
    help_line[2] = "you forgot something else, as in `\\hbox{$x}'. In such cases";
    help_line[1] = "the way to recover is to insert both the forgotten and the";
    help_line[0] = "deleted material, e.g., by typing `I$}'.";
    error();
    align_state++;
}


void normal_paragraph(void)
{

    if (INTPAR(looseness) != 0)
        eq_word_define(INT_BASE + INT_PAR__looseness, 0);
    if (DIMENPAR(hang_indent) != 0)
        eq_word_define(DIMEN_BASE + DIMEN_PAR__hang_indent, 0);
    if (INTPAR(hang_after) != 1)
        eq_word_define(INT_BASE + INT_PAR__hang_after, 1);
    if (LOCAL(par_shape) != TEX_NULL)
        eq_define(LOCAL_BASE + LOCAL__par_shape, SHAPE_REF, TEX_NULL);
    if (eqtb[INTER_LINE_PENALTIES_LOC].b32.s1 != TEX_NULL)
        eq_define(INTER_LINE_PENALTIES_LOC, SHAPE_REF, TEX_NULL);
}


/*1110: "The box_end procedure does the right thing with cur_box, if
 * box_context represents the context as explained [as follows]." The
 * box_context is one of (1) a signed shift amount; (2) BOX_FLAG+N, signifying
 * a `\setbox<N>`; (3) GLOBAL_BOX_FLAG+N, signifying `\global\setbox<N>`; (4)
 * SHIP_OUT_FLAG, signifying `\shipout`; or (5) LEADER_FLAG+k, signifying (in
 * order) `\leaders`, `\cleaders`, or `\xleaders`. */
void
box_end(int32_t box_context)
{
    int32_t p;
    small_number a;

    if (box_context < BOX_FLAG) { /*1111:*/
        if (cur_box != TEX_NULL) {
            mem[cur_box + 4].b32.s1 = box_context;

            if (abs(cur_list.mode) == VMODE) {
                if (pre_adjust_tail != TEX_NULL) {
                    if (PRE_ADJUST_HEAD != pre_adjust_tail) {
                        mem[cur_list.tail].b32.s1 = mem[PRE_ADJUST_HEAD].b32.s1;
                        cur_list.tail = pre_adjust_tail;
                    }
                    pre_adjust_tail = TEX_NULL;
                }

                append_to_vlist(cur_box);

                if (adjust_tail != TEX_NULL) {
                    if (ADJUST_HEAD != adjust_tail) {
                        mem[cur_list.tail].b32.s1 = mem[ADJUST_HEAD].b32.s1;
                        cur_list.tail = adjust_tail;
                    }
                    adjust_tail = TEX_NULL;
                }

                if (cur_list.mode > 0)
                    build_page();
            } else {
                if (abs(cur_list.mode) == HMODE) {
                    cur_list.aux.b32.s0 = 1000;
                } else {
                    p = new_noad();
                    mem[p + 1].b32.s1 = SUB_BOX;
                    mem[p + 1].b32.s0 = cur_box;
                    cur_box = p;
                }

                mem[cur_list.tail].b32.s1 = cur_box;
                cur_list.tail = cur_box;
            }
        }
    } else if (box_context < SHIP_OUT_FLAG) { /*1112:*/
        if (box_context < GLOBAL_BOX_FLAG) {
            cur_val = box_context - BOX_FLAG;
            a = 0;
        } else {
            cur_val = box_context - GLOBAL_BOX_FLAG;
            a = 4;
        }

        if (cur_val < 256) {
            if (a >= 4)
                geq_define(BOX_BASE + cur_val, BOX_REF, cur_box);
            else
                eq_define(BOX_BASE + cur_val, BOX_REF, cur_box);
        } else {
            find_sa_element(4, cur_val, true);
            if (a >= 4)
                gsa_def(cur_ptr, cur_box);
            else
                sa_def(cur_ptr, cur_box);
        }
    } else if (cur_box != TEX_NULL) {
        if (box_context > SHIP_OUT_FLAG) { /*1113:*/
            do {
                get_x_token();
            } while (cur_cmd == SPACER || cur_cmd == RELAX);

            if ((cur_cmd == HSKIP && abs(cur_list.mode) != VMODE) || (cur_cmd == VSKIP && abs(cur_list.mode) == VMODE)) {
                append_glue();
                mem[cur_list.tail].b16.s0 = box_context - (LEADER_FLAG - A_LEADERS);
                mem[cur_list.tail + 1].b32.s1 = cur_box;
            } else {
                error_here_with_diagnostic("Leaders not followed by proper glue");
                capture_to_diagnostic(NULL);

                help_ptr = 3;
                help_line[2] = "You should say `\\leaders <box or rule><hskip or vskip>'.";
                help_line[1] = "I found the <box or rule>, but there's no suitable";
                help_line[0] = "<hskip or vskip>, so I'm ignoring these leaders.";
                back_error();
                flush_node_list(cur_box);
            }
        } else {
            ship_out(cur_box);
        }
    }
}


void
begin_box(int32_t box_context)
{
    int32_t p, q;
    int32_t r;
    bool fm;
    int32_t tx;
    uint16_t m;
    int32_t k;
    int32_t n;

    switch (cur_chr) {
    case BOX_CODE:
        scan_register_num();

        if (cur_val < 256) {
            cur_box = BOX_REG(cur_val);
        } else {
            find_sa_element(4, cur_val, false);
            if (cur_ptr == TEX_NULL)
                cur_box = TEX_NULL;
            else
                cur_box = mem[cur_ptr + 1].b32.s1;
        }

        if (cur_val < 256) {
            BOX_REG(cur_val) = TEX_NULL;
        } else {
            find_sa_element(4, cur_val, false);
            if (cur_ptr != TEX_NULL) {
                mem[cur_ptr + 1].b32.s1 = TEX_NULL;
                mem[cur_ptr + 1].b32.s0++;
                delete_sa_ref(cur_ptr);
            }
        }
        break;

    case COPY_CODE:
        scan_register_num();

        if (cur_val < 256) {
            q = BOX_REG(cur_val);
        } else {
            find_sa_element(4, cur_val, false);
            if (cur_ptr == TEX_NULL)
                q = TEX_NULL;
            else
                q = mem[cur_ptr + 1].b32.s1;
        }

        cur_box = copy_node_list(q);
        break;

    case LAST_BOX_CODE:
        cur_box = TEX_NULL;

        if (abs(cur_list.mode) == MMODE) {
            you_cant();
            help_ptr = 1;
            help_line[0] = "Sorry; this \\lastbox will be void.";
            error();
        } else if (cur_list.mode == VMODE && cur_list.head == cur_list.tail) {
            you_cant();
            help_ptr = 2;
            help_line[1] = "Sorry...I usually can't take things from the current page.";
            help_line[0] = "This \\lastbox will therefore be void.";
            error();
        } else {
            tx = cur_list.tail;

            if (tx < hi_mem_min) {
                if (NODE_type(tx) == MATH_NODE && mem[tx].b16.s0 == END_M_CODE) {
                    r = cur_list.head;
                    do {
                        q = r;
                        r = mem[q].b32.s1;
                    } while (r != tx);
                    tx = q;
                }
            }

            if (tx < hi_mem_min) {
                if (NODE_type(tx) == HLIST_NODE || NODE_type(tx) == VLIST_NODE) { /*1116:*/
                    q = cur_list.head;
                    p = TEX_NULL;

                    do {
                        r = p;
                        p = q;
                        fm = false;

                        if (q < hi_mem_min) {
                            if (NODE_type(q) == DISC_NODE) {
                                for (m = 1; m <= mem[q].b16.s0; m++)
                                    p = LLIST_link(p);

                                if (p == tx)
                                    goto done;
                            } else if (NODE_type(q) == MATH_NODE && mem[q].b16.s0 == BEGIN_M_CODE) {
                                fm = true;
                            }
                        }

                        q = mem[p].b32.s1;
                    } while (q != tx);

                    q = mem[tx].b32.s1;
                    mem[p].b32.s1 = q;
                    mem[tx].b32.s1 = TEX_NULL;

                    if (q == TEX_NULL) {
                        if (fm)
                            confusion("tail1");
                        else
                            cur_list.tail = p;
                    } else if (fm) {
                        cur_list.tail = r;
                        mem[r].b32.s1 = TEX_NULL;
                        flush_node_list(p);
                    }

                    cur_box = tx;
                    mem[cur_box + 4].b32.s1 = 0;
                }
            }
        done:
            ;
        }
        break;

    case VSPLIT_CODE:
        scan_register_num();
        n = cur_val;

        if (!scan_keyword("to")) {
            error_here_with_diagnostic("Missing `to' inserted");
            capture_to_diagnostic(NULL);

            help_ptr = 2;
            help_line[1] = "I'm working on `\\vsplit<box number> to <dimen>';";
            help_line[0] = "will look for the <dimen> next.";
            error();
        }

        scan_dimen(false, false, false);
        cur_box = vsplit(n, cur_val);
        break;

    default:
        k = cur_chr - 4;
        save_stack[save_ptr + 0].b32.s1 = box_context;
        if (k == HMODE) {
            if (box_context < BOX_FLAG && abs(cur_list.mode) == VMODE)
                scan_spec(ADJUSTED_HBOX_GROUP, true);
            else
                scan_spec(HBOX_GROUP, true);
        } else {
            if (k == VMODE)
                scan_spec(VBOX_GROUP, true);
            else {
                scan_spec(VTOP_GROUP, true);
                k = VMODE;
            }
            normal_paragraph();
        }

        push_nest();
        cur_list.mode = -(int32_t) k;

        if (k == VMODE) {
            cur_list.aux.b32.s1 = IGNORE_DEPTH;
            if (LOCAL(every_vbox) != TEX_NULL)
                begin_token_list(LOCAL(every_vbox), EVERY_VBOX_TEXT);
        } else {
            cur_list.aux.b32.s0 = 1000;
            if (LOCAL(every_hbox) != TEX_NULL)
                begin_token_list(LOCAL(every_hbox), EVERY_HBOX_TEXT);
        }

        return;
    }

    box_end(box_context);
}


void
scan_box(int32_t box_context)
{
    do {
        get_x_token();
    } while (cur_cmd == SPACER || cur_cmd == RELAX);

    if (cur_cmd == MAKE_BOX) {
        begin_box(box_context);
    } else if (box_context >= LEADER_FLAG && (cur_cmd == HRULE || cur_cmd == VRULE)) {
        cur_box = scan_rule_spec();
        box_end(box_context);
    } else {
        error_here_with_diagnostic("A <box> was supposed to be here");
        capture_to_diagnostic(NULL);

        help_ptr = 3;
        help_line[2] = "I was expecting to see \\hbox or \\vbox or \\copy or \\box or";
        help_line[1] = "something like that. So you might find something missing in";
        help_line[0] = "your output. But keep trying; you can fix this later.";
        back_error();
    }
}


void package(small_number c)
{
    scaled_t h;
    int32_t p;
    scaled_t d;
    int32_t u, v;

    d = DIMENPAR(box_max_depth);
    u = INTPAR(xetex_upwards);
    unsave();
    save_ptr = save_ptr - 3;
    v = INTPAR(xetex_upwards);
    INTPAR(xetex_upwards) = u;
    if (cur_list.mode == -104)
        cur_box = hpack(mem[cur_list.head].b32.s1, save_stack[save_ptr + 2].b32.s1, save_stack[save_ptr + 1].b32.s1);
    else {

        cur_box =
            vpackage(mem[cur_list.head].b32.s1, save_stack[save_ptr + 2].b32.s1, save_stack[save_ptr + 1].b32.s1, d);
        if (c == VTOP_CODE) {   /*1122: */
            h = 0;
            p = mem[cur_box + 5].b32.s1;
            if (p != TEX_NULL) {

                if (NODE_type(p) <= RULE_NODE)
                    h = mem[p + 3].b32.s1;
            }
            mem[cur_box + 2].b32.s1 = mem[cur_box + 2].b32.s1 - h + mem[cur_box + 3].b32.s1;
            mem[cur_box + 3].b32.s1 = h;
        }
    }
    INTPAR(xetex_upwards) = v;
    pop_nest();
    box_end(save_stack[save_ptr + 0].b32.s1);
}

small_number norm_min(int32_t h)
{
    if (h <= 0)
        return 1;
    else if (h >= 63)
        return 63;
    else
        return h;
}

void new_graf(bool indented)
{
    cur_list.prev_graf = 0;

    if ((cur_list.mode == VMODE) || (cur_list.head != cur_list.tail)) {
        mem[cur_list.tail].b32.s1 = new_param_glue(GLUE_PAR__par_skip);
        cur_list.tail = LLIST_link(cur_list.tail);
    }

    push_nest();
    cur_list.mode = HMODE;
    cur_list.aux.b32.s0 = 1000;
    if (INTPAR(language) <= 0)
        cur_lang = 0;
    else if (INTPAR(language) > BIGGEST_LANG)
        cur_lang = 0;
    else
        cur_lang = INTPAR(language);
    cur_list.aux.b32.s1 = cur_lang;
    cur_list.prev_graf =
        (norm_min(INTPAR(left_hyphen_min)) * 64 +
         norm_min(INTPAR(right_hyphen_min))) * 65536L + cur_lang;
    if (indented) {
        cur_list.tail = new_null_box();
        mem[cur_list.head].b32.s1 = cur_list.tail;
        mem[cur_list.tail + 1].b32.s1 = eqtb[DIMEN_BASE].b32.s1;
        if ((insert_src_special_every_par))
            insert_src_special();
    }
    if (LOCAL(every_par) != TEX_NULL)
        begin_token_list(LOCAL(every_par), EVERY_PAR_TEXT);
    if (nest_ptr == 1)
        build_page();
}

void indent_in_hmode(void)
{
    int32_t p, q;

    if (cur_chr > 0) {
        p = new_null_box();
        mem[p + 1].b32.s1 = eqtb[DIMEN_BASE].b32.s1;
        if (abs(cur_list.mode) == HMODE)
            cur_list.aux.b32.s0 = 1000;
        else {

            q = new_noad();
            mem[q + 1].b32.s1 = SUB_BOX;
            mem[q + 1].b32.s0 = p;
            p = q;
        }
        {
            mem[cur_list.tail].b32.s1 = p;
            cur_list.tail = LLIST_link(cur_list.tail);
        }
    }
}

void head_for_vmode(void)
{
    if (cur_list.mode < 0) {

        if (cur_cmd != HRULE)
            off_save();
        else {
            error_here_with_diagnostic("You can't use `");
            print_esc_cstr("hrule");
            print_cstr("' here except with leaders");
            capture_to_diagnostic(NULL);

            {
                help_ptr = 2;
                help_line[1] = "To put a horizontal rule in an hbox or an alignment,";
                help_line[0] = "you should use \\leaders or \\hrulefill (see The TeXbook).";
            }
            error();
        }
    } else {

        back_input();
        cur_tok = par_token;
        back_input();
        cur_input.index = INSERTED;
    }
}

void end_graf(void)
{
    if (cur_list.mode == HMODE) {
        if (cur_list.head == cur_list.tail)
            pop_nest();
        else
            line_break(false);
        if (cur_list.eTeX_aux != TEX_NULL) {
            flush_list(cur_list.eTeX_aux);
            cur_list.eTeX_aux = TEX_NULL;
        }
        normal_paragraph();
        error_count = 0;
    }
}

void begin_insert_or_adjust(void)
{
    if (cur_cmd == VADJUST)
        cur_val = 255;
    else {

        scan_eight_bit_int();
        if (cur_val == 255) {
            error_here_with_diagnostic("You can't ");
            print_esc_cstr("insert");
            print_int(255);
            capture_to_diagnostic(NULL);

            {
                help_ptr = 1;
                help_line[0] = "I'm changing to \\insert0; box 255 is special.";
            }
            error();
            cur_val = 0;
        }
    }
    save_stack[save_ptr + 0].b32.s1 = cur_val;
    if ((cur_cmd == VADJUST) && scan_keyword("pre"))
        save_stack[save_ptr + 1].b32.s1 = 1;
    else
        save_stack[save_ptr + 1].b32.s1 = 0;
    save_ptr = save_ptr + 2;
    new_save_level(INSERT_GROUP);
    scan_left_brace();
    normal_paragraph();
    push_nest();
    cur_list.mode = -1;
    cur_list.aux.b32.s1 = IGNORE_DEPTH;
}

void make_mark(void)
{
    int32_t p;
    int32_t c;
    if (cur_chr == 0)
        c = 0;
    else {

        scan_register_num();
        c = cur_val;
    }
    p = scan_toks(false, true);
    p = get_node(SMALL_NODE_SIZE);
    mem[p + 1].b32.s0 = c;
    NODE_type(p) = MARK_NODE;
    mem[p].b16.s0 = 0;
    mem[p + 1].b32.s1 = def_ref;
    mem[cur_list.tail].b32.s1 = p;
    cur_list.tail = p;
}

void append_penalty(void)
{
    scan_int();
    {
        mem[cur_list.tail].b32.s1 = new_penalty(cur_val);
        cur_list.tail = LLIST_link(cur_list.tail);
    }
    if (cur_list.mode == VMODE)
        build_page();
}

void delete_last(void)
{
    int32_t p, q;
    int32_t r;
    bool fm;
    int32_t tx;
    uint16_t m;
    if ((cur_list.mode == VMODE) && (cur_list.tail == cur_list.head)) {       /*1141: */
        if ((cur_chr != GLUE_NODE) || (last_glue != MAX_HALFWORD)) {
            you_cant();
            {
                help_ptr = 2;
                help_line[1] = "Sorry...I usually can't take things from the current page.";
                help_line[0] = "Try `I\\vskip-\\lastskip' instead.";
            }
            if (cur_chr == KERN_NODE)
                help_line[0] = "Try `I\\kern-\\lastkern' instead.";
            else if (cur_chr != GLUE_NODE)
                help_line[0] = "Perhaps you can make the output routine do it.";
            error();
        }
    } else {

        tx = cur_list.tail;
        if (!(is_char_node(tx))) {

            if ((NODE_type(tx) == MATH_NODE) && (mem[tx].b16.s0 == END_M_CODE)) {
                r = cur_list.head;
                do {
                    q = r;
                    r = mem[q].b32.s1;
                } while (!(r == tx));
                tx = q;
            }
        }
        if (!(is_char_node(tx))) {

            if (mem[tx].b16.s1 == cur_chr) {
                q = cur_list.head;
                p = TEX_NULL;
                do {
                    r = p;
                    p = q;
                    fm = false;
                    if (!(is_char_node(q))) {

                        if (NODE_type(q) == DISC_NODE) {
                            {
                                register int32_t for_end;
                                m = 1;
                                for_end = mem[q].b16.s0;
                                if (m <= for_end)
                                    do
                                        p = LLIST_link(p);
                                    while (m++ < for_end);
                            }
                            if (p == tx)
                                return;
                        } else if ((NODE_type(q) == MATH_NODE) && (mem[q].b16.s0 == BEGIN_M_CODE))
                            fm = true;
                    }
                    q = mem[p].b32.s1;
                } while (!(q == tx));
                q = mem[tx].b32.s1;
                mem[p].b32.s1 = q;
                mem[tx].b32.s1 = TEX_NULL;
                if (q == TEX_NULL) {

                    if (fm)
                        confusion("tail1");
                    else
                        cur_list.tail = p;
                } else if (fm) {
                    cur_list.tail = r;
                    mem[r].b32.s1 = TEX_NULL;
                    flush_node_list(p);
                }
                flush_node_list(tx);
            }
        }
    }
}

void unpackage(void)
{
    int32_t p;
    int32_t r;
    unsigned char /*copy_code */ c;

    if (cur_chr > COPY_CODE) {  /*1651: */
        mem[cur_list.tail].b32.s1 = disc_ptr[cur_chr];
        disc_ptr[cur_chr] = TEX_NULL;
        goto done;
    }
    c = cur_chr;
    scan_register_num();
    if (cur_val < 256)
        p = BOX_REG(cur_val);
    else {

        find_sa_element(4, cur_val, false);
        if (cur_ptr == TEX_NULL)
            p = TEX_NULL;
        else
            p = mem[cur_ptr + 1].b32.s1;
    }
    if (p == TEX_NULL)
        return;
    if ((abs(cur_list.mode) == MMODE)
        || ((abs(cur_list.mode) == VMODE) && (NODE_type(p) != VLIST_NODE))
        || ((abs(cur_list.mode) == HMODE) && (NODE_type(p) != HLIST_NODE))) {
        error_here_with_diagnostic("Incompatible list can't be unboxed");
        capture_to_diagnostic(NULL);
        {
            help_ptr = 3;
            help_line[2] = "Sorry, Pandora. (You sneaky devil.)";
            help_line[1] = "I refuse to unbox an \\hbox in vertical mode or vice versa.";
            help_line[0] = "And I can't open any boxes in math mode.";
        }
        error();
        return;
    }
    if (c == COPY_CODE)
        mem[cur_list.tail].b32.s1 = copy_node_list(mem[p + 5].b32.s1);
    else {

        mem[cur_list.tail].b32.s1 = mem[p + 5].b32.s1;
        if (cur_val < 256)
            BOX_REG(cur_val) = TEX_NULL;
        else {

            find_sa_element(4, cur_val, false);
            if (cur_ptr != TEX_NULL) {
                mem[cur_ptr + 1].b32.s1 = TEX_NULL;
                mem[cur_ptr + 1].b32.s0++;
                delete_sa_ref(cur_ptr);
            }
        }
        free_node(p, BOX_NODE_SIZE);
    }
done:
    while (mem[cur_list.tail].b32.s1 != TEX_NULL) {

        r = mem[cur_list.tail].b32.s1;
        if (!(is_char_node(r)) && (NODE_type(r) == MARGIN_KERN_NODE)) {
            mem[cur_list.tail].b32.s1 = mem[r].b32.s1;
            free_node(r, MARGIN_KERN_NODE_SIZE);
        }
        cur_list.tail = LLIST_link(cur_list.tail);
    }
}

void append_italic_correction(void)
{
    int32_t p;
    internal_font_number f;
    if (cur_list.tail != cur_list.head) {
        if ((is_char_node(cur_list.tail)))
            p = cur_list.tail;
        else if (NODE_type(cur_list.tail) == LIGATURE_NODE)
            p = cur_list.tail + 1;
        else if (NODE_type(cur_list.tail) == WHATSIT_NODE) {
            if ((mem[cur_list.tail].b16.s0 == NATIVE_WORD_NODE)
                || (mem[cur_list.tail].b16.s0 == NATIVE_WORD_NODE_AT)) {
                {
                    mem[cur_list.tail].b32.s1 = new_kern(get_native_italic_correction(cur_list.tail));
                    cur_list.tail = LLIST_link(cur_list.tail);
                }
                NODE_subtype(cur_list.tail) = EXPLICIT;
            } else if (mem[cur_list.tail].b16.s0 == GLYPH_NODE) {
                {
                    mem[cur_list.tail].b32.s1 =
                        new_kern(get_native_glyph_italic_correction(cur_list.tail));
                    cur_list.tail = LLIST_link(cur_list.tail);
                }
                NODE_subtype(cur_list.tail) = EXPLICIT;
            }
            return;
        } else
            return;
        f = CHAR_NODE_font(p);
        {
            mem[cur_list.tail].b32.s1 =
                new_kern(FONT_CHARINFO_ITALCORR(f, FONT_CHARACTER_INFO(f, effective_char(true, f, CHAR_NODE_character(p)))));
            cur_list.tail = LLIST_link(cur_list.tail);
        }
        NODE_subtype(cur_list.tail) = EXPLICIT;
    }
}

void append_discretionary(void)
{
    int32_t c;

    mem[cur_list.tail].b32.s1 = new_disc();
    cur_list.tail = LLIST_link(cur_list.tail);

    if (cur_chr == 1) {
        c = hyphen_char[eqtb[CUR_FONT_LOC].b32.s1];
        if (c >= 0) {

            if (c <= BIGGEST_CHAR)
                mem[cur_list.tail + 1].b32.s0 = new_character(eqtb[CUR_FONT_LOC].b32.s1, c);
        }
    } else {

        save_ptr++;
        save_stack[save_ptr - 1].b32.s1 = 0;
        new_save_level(DISC_GROUP);
        scan_left_brace();
        push_nest();
        cur_list.mode = -104;
        cur_list.aux.b32.s0 = 1000;
    }
}

void build_discretionary(void)
{
    int32_t p, q;
    int32_t n;
    unsave();
    q = cur_list.head;
    p = mem[q].b32.s1;
    n = 0;
    while (p != TEX_NULL) {

        if (!(is_char_node(p))) {

            if (NODE_type(p) > RULE_NODE) {

                if (NODE_type(p) != KERN_NODE) {

                    if (NODE_type(p) != LIGATURE_NODE) {

                        if ((NODE_type(p) != WHATSIT_NODE)
                            || ((mem[p].b16.s0 != NATIVE_WORD_NODE)
                                && (mem[p].b16.s0 != NATIVE_WORD_NODE_AT)
                                && (mem[p].b16.s0 != GLYPH_NODE))) {
                            error_here_with_diagnostic("Improper discretionary list");
                            capture_to_diagnostic(NULL);
                            {
                                help_ptr = 1;
                                help_line[0] = "Discretionary lists must contain only boxes and kerns.";
                            }
                            error();

                            begin_diagnostic();
                            diagnostic_begin_capture_warning_here();

                            print_nl_cstr("The following discretionary sublist has been deleted:");
                            show_box(p);

                            capture_to_diagnostic(NULL);
                            end_diagnostic(true);
                            flush_node_list(p);
                            mem[q].b32.s1 = TEX_NULL;
                            goto done;
                        }
                    }
                }
            }
        }
        q = p;
        p = mem[q].b32.s1;
        n++;
    } /*:1156 */
done:
    p = mem[cur_list.head].b32.s1;
    pop_nest();
    switch (save_stack[save_ptr - 1].b32.s1) {
    case 0:
        mem[cur_list.tail + 1].b32.s0 = p;
        break;
    case 1:
        mem[cur_list.tail + 1].b32.s1 = p;
        break;
    case 2:
        {
            if ((n > 0) && (abs(cur_list.mode) == MMODE)) {
                error_here_with_diagnostic("Illegal math ");
                print_esc_cstr("discretionary");
                capture_to_diagnostic(NULL);

                {
                    help_ptr = 2;
                    help_line[1] = "Sorry: The third part of a discretionary break must be";
                    help_line[0] = "empty, in math formulas. I had to delete your third part.";
                }
                flush_node_list(p);
                n = 0;
                error();
            } else
                mem[cur_list.tail].b32.s1 = p;
            if (n <= UINT16_MAX)
                mem[cur_list.tail].b16.s0 = n;
            else {
                error_here_with_diagnostic("Discretionary list is too long");
                capture_to_diagnostic(NULL);

                {
                    help_ptr = 2;
                    help_line[1] = "Wow---I never thought anybody would tweak me here.";
                    help_line[0] = "You can't seriously need such a huge discretionary list?";
                }
                error();
            }
            if (n > 0)
                cur_list.tail = q;
            save_ptr--;
            return;
        }
        break;
    }
    save_stack[save_ptr - 1].b32.s1++;
    new_save_level(DISC_GROUP);
    scan_left_brace();
    push_nest();
    cur_list.mode = -104;
    cur_list.aux.b32.s0 = 1000;
}

void make_accent(void)
{
    double s, t;
    int32_t p, q, r;
    internal_font_number f;
    scaled_t a, h, x, w, delta, lsb, rsb;
    b16x4 i;

    scan_char_num();
    f = eqtb[CUR_FONT_LOC].b32.s1;
    p = new_character(f, cur_val);

    if (p != TEX_NULL) {
        x = font_info[X_HEIGHT_CODE + param_base[f]].b32.s1;
        s = font_info[SLANT_CODE + param_base[f]].b32.s1 / ((double)65536.0);
        if (((font_area[f] == AAT_FONT_FLAG) || (font_area[f] == OTGR_FONT_FLAG))) {
            a = mem[p + 1].b32.s1;
            if (a == 0)
                get_native_char_sidebearings(f, cur_val, &lsb, &rsb);
        } else
            a = FONT_CHARACTER_WIDTH(f,
                                     effective_char(true, f, CHAR_NODE_character(p)));
        do_assignments();
        q = TEX_NULL;
        f = eqtb[CUR_FONT_LOC].b32.s1;
        if ((cur_cmd == LETTER) || (cur_cmd == OTHER_CHAR) || (cur_cmd == CHAR_GIVEN)) {
            q = new_character(f, cur_chr);
            cur_val = cur_chr;
        } else if (cur_cmd == CHAR_NUM) {
            scan_char_num();
            q = new_character(f, cur_val);
        } else
            back_input();
        if (q != TEX_NULL) { /*1160: */
            t = font_info[SLANT_CODE + param_base[f]].b32.s1 / ((double)65536.0);
            if (((font_area[f] == AAT_FONT_FLAG) || (font_area[f] == OTGR_FONT_FLAG))) {
                w = mem[q + 1].b32.s1;
                get_native_char_height_depth(f, cur_val, &h, &delta);
            } else {

                i = FONT_CHARACTER_INFO(f, effective_char(true, f, CHAR_NODE_character(q)));
                w = FONT_CHARINFO_WIDTH(f, i);
                h = FONT_CHARINFO_HEIGHT(f, i);
            }
            if (h != x) {
                p = hpack(p, 0, ADDITIONAL);
                mem[p + 4].b32.s1 = x - h;
            }
            if (((font_area[f] == AAT_FONT_FLAG) || (font_area[f] == OTGR_FONT_FLAG))
                && (a == 0))
                delta = tex_round((w - lsb + rsb) / ((double)2.0) + h * t - x * s);
            else
                delta = tex_round((w - a) / ((double)2.0) + h * t - x * s);
            r = new_kern(delta);
            NODE_subtype(r) = ACC_KERN;
            mem[cur_list.tail].b32.s1 = r;
            mem[r].b32.s1 = p;
            cur_list.tail = new_kern(-(int32_t) a - delta);
            NODE_subtype(cur_list.tail) = ACC_KERN;
            mem[p].b32.s1 = cur_list.tail;
            p = q;
        }
        mem[cur_list.tail].b32.s1 = p;
        cur_list.tail = p;
        cur_list.aux.b32.s0 = 1000;
    }
}

void align_error(void)
{
    if (abs(align_state) > 2) {      /*1163: */
        error_here_with_diagnostic("Misplaced ");
        print_cmd_chr(cur_cmd, cur_chr);
        capture_to_diagnostic(NULL);

        if (cur_tok == (TAB_TOKEN + 38)) {
            {
                help_ptr = 6;
                help_line[5] = "I can't figure out why you would want to use a tab mark";
                help_line[4] = "here. If you just want an ampersand, the remedy is";
                help_line[3] = "simple: Just type `I\\&' now. But if some right brace";
                help_line[2] = "up above has ended a previous alignment prematurely,";
                help_line[1] = "you're probably due for more error messages, and you";
                help_line[0] = "might try typing `S' now just to see what is salvageable.";
            }
        } else {

            {
                help_ptr = 5;
                help_line[4] = "I can't figure out why you would want to use a tab mark";
                help_line[3] = "or \\cr or \\span just now. If something like a right brace";
                help_line[2] = "up above has ended a previous alignment prematurely,";
                help_line[1] = "you're probably due for more error messages, and you";
                help_line[0] = "might try typing `S' now just to see what is salvageable.";
            }
        }
        error();
    } else {

        back_input();
        if (align_state < 0) {
            error_here_with_diagnostic("Missing { inserted");
            capture_to_diagnostic(NULL);
            align_state++;
            cur_tok = (LEFT_BRACE_TOKEN + 123);
        } else {
            error_here_with_diagnostic("Missing } inserted");
            capture_to_diagnostic(NULL);
            align_state--;
            cur_tok = (RIGHT_BRACE_TOKEN + 125);
        }
        {
            help_ptr = 3;
            help_line[2] = "I've put in what seems to be necessary to fix";
            help_line[1] = "the current column of the current alignment.";
            help_line[0] = "Try to go on, since this might almost work.";
        }
        ins_error();
    }
}

void no_align_error(void)
{
    error_here_with_diagnostic("Misplaced ");
    print_esc_cstr("noalign");
    capture_to_diagnostic(NULL);

    {
        help_ptr = 2;
        help_line[1] = "I expect to see \\noalign only after the \\cr of";
        help_line[0] = "an alignment. Proceed, and I'll ignore this case.";
    }
    error();
}

void omit_error(void)
{
    error_here_with_diagnostic("Misplaced ");
    print_esc_cstr("omit");
    capture_to_diagnostic(NULL);

    {
        help_ptr = 2;
        help_line[1] = "I expect to see \\omit only after tab marks or the \\cr of";
        help_line[0] = "an alignment. Proceed, and I'll ignore this case.";
    }
    error();
}

void do_endv(void)
{
    base_ptr = input_ptr;
    input_stack[base_ptr] = cur_input;
    while ((input_stack[base_ptr].index != V_TEMPLATE) && (input_stack[base_ptr].loc == TEX_NULL)
           && (input_stack[base_ptr].state == TOKEN_LIST))
        base_ptr--;
    if ((input_stack[base_ptr].index != V_TEMPLATE) || (input_stack[base_ptr].loc != TEX_NULL)
        || (input_stack[base_ptr].state != TOKEN_LIST))
        fatal_error("(interwoven alignment preambles are not allowed)");
    if (cur_group == ALIGN_GROUP) {
        end_graf();
        if (fin_col())
            fin_row();
    } else
        off_save();
}

void cs_error(void)
{
    error_here_with_diagnostic("Extra ");
    print_esc_cstr("endcsname");
    capture_to_diagnostic(NULL);
    {
        help_ptr = 1;
        help_line[0] = "I'm ignoring this, since I wasn't doing a \\csname.";
    }
    error();
}

void push_math(group_code c)
{
    push_nest();
    cur_list.mode = -207;
    cur_list.aux.b32.s1 = TEX_NULL;
    new_save_level(c);
}


void
just_copy(int32_t p, int32_t h, int32_t t)
{
    int32_t r;
    unsigned char words;

    while (p != TEX_NULL) {
        words = 1;

        if (is_char_node(p))
            r = get_avail();
        else
            switch (NODE_type(p)) {
            case HLIST_NODE:
            case VLIST_NODE:
                r = get_node(BOX_NODE_SIZE);
                SYNCTEX_tag(r, BOX_NODE_SIZE) = SYNCTEX_tag(p, BOX_NODE_SIZE);
                SYNCTEX_line(r, BOX_NODE_SIZE) = SYNCTEX_line(p, BOX_NODE_SIZE);
                mem[r + 6] = mem[p + 6];
                mem[r + 5] = mem[p + 5];
                words = 5;
                BOX_list_ptr(r) = TEX_NULL;
                break;

            case RULE_NODE:
                r = get_node(RULE_NODE_SIZE);
                words = RULE_NODE_SIZE;
                break;

            case LIGATURE_NODE:
                r = get_avail();
                mem[r] = mem[p + 1];
                goto found;
                break;

            case KERN_NODE:
            case MATH_NODE:
                words = MEDIUM_NODE_SIZE;
                r = get_node(words);
                break;

            case GLUE_NODE:
                r = get_node(MEDIUM_NODE_SIZE);
                GLUE_SPEC_ref_count(GLUE_NODE_glue_ptr(p))++;
                SYNCTEX_tag(r, MEDIUM_NODE_SIZE) = SYNCTEX_tag(p, MEDIUM_NODE_SIZE);
                SYNCTEX_line(r, MEDIUM_NODE_SIZE) = SYNCTEX_line(p, MEDIUM_NODE_SIZE);
                GLUE_NODE_glue_ptr(r) = GLUE_NODE_glue_ptr(p);
                GLUE_NODE_leader_ptr(r) = TEX_NULL;
                break;

            case WHATSIT_NODE:
                switch (NODE_subtype(p)) {
                case OPEN_NODE:
                    r = get_node(OPEN_NODE_SIZE);
                    words = OPEN_NODE_SIZE;
                    break;

                case WRITE_NODE:
                case SPECIAL_NODE:
                    r = get_node(WRITE_NODE_SIZE);
                    TOKEN_LIST_ref_count(WRITE_NODE_tokens(p))++;
                    words = WRITE_NODE_SIZE;
                    break;

                case CLOSE_NODE:
                case LANGUAGE_NODE:
                    r = get_node(SMALL_NODE_SIZE);
                    words = SMALL_NODE_SIZE;
                    break;

                case NATIVE_WORD_NODE:
                case NATIVE_WORD_NODE_AT:
                    words = NATIVE_NODE_size(p);
                    r = get_node(words);

                    while (words > 0) {
                        words--;
                        mem[r + words] = mem[p + words];
                    }

                    NATIVE_NODE_glyph_info_ptr(r) = NULL;
                    NATIVE_NODE_glyph_count(r) = 0;
                    copy_native_glyph_info(p, r);
                    break;

                case GLYPH_NODE:
                    r = get_node(GLYPH_NODE_SIZE);
                    words = GLYPH_NODE_SIZE;
                    break;

                case PIC_NODE:
                case PDF_NODE:
                    words = PIC_NODE_total_size(p);
                    r = get_node(words);
                    break;

                case PDF_SAVE_POS_NODE:
                    r = get_node(SMALL_NODE_SIZE);
                    break;

                default:
                    confusion("ext2");
                    break;
                }

                break;
            default:
                goto not_found;
                break;
            }

        while (words > 0) {
            words--;
            mem[r + words] = mem[p + words];
        }

    found:
        LLIST_link(h) = r;
        h = r;

    not_found:
        p = LLIST_link(p);
    }

    LLIST_link(h) = t;
}

void just_reverse(int32_t p)
{
    int32_t l;
    int32_t t;
    int32_t q;
    int32_t m, n;
    m = MIN_HALFWORD;
    n = MIN_HALFWORD;
    if (mem[TEMP_HEAD].b32.s1 == TEX_NULL) {
        just_copy(mem[p].b32.s1, TEMP_HEAD, TEX_NULL);
        q = mem[TEMP_HEAD].b32.s1;
    } else {

        q = mem[p].b32.s1;
        mem[p].b32.s1 = TEX_NULL;
        flush_node_list(mem[TEMP_HEAD].b32.s1);
    }
    t = new_edge(cur_dir, 0);
    l = t;
    cur_dir = 1 - cur_dir;
    while (q != TEX_NULL)
        if ((is_char_node(q)))
            do {
                p = q;
                q = mem[p].b32.s1;
                mem[p].b32.s1 = l;
                l = p;
            } while (!(!(is_char_node(q))));
        else {

            p = q;
            q = mem[p].b32.s1;
            if (NODE_type(p) == MATH_NODE) {    /*1527: */

                if (odd(mem[p].b16.s0)) {

                    if (mem[LR_ptr].b32.s0 != (L_CODE * (mem[p].b16.s0 / L_CODE) + 3)) {
                        NODE_type(p) = KERN_NODE;
                        LR_problems++;
                    } else {

                        {
                            temp_ptr = LR_ptr;
                            LR_ptr = mem[temp_ptr].b32.s1;
                            {
                                mem[temp_ptr].b32.s1 = avail;
                                avail = temp_ptr;
                            }
                        }
                        if (n > MIN_HALFWORD) {
                            n--;
                            mem[p].b16.s0--;
                        } else {

                            if (m > MIN_HALFWORD)
                                m--;
                            else {

                                mem[t + 1].b32.s1 = mem[p + 1].b32.s1;
                                mem[t].b32.s1 = q;
                                free_node(p, MEDIUM_NODE_SIZE);
                                goto done;
                            }
                            NODE_type(p) = KERN_NODE;
                        }
                    }
                } else {

                    {
                        temp_ptr = get_avail();
                        mem[temp_ptr].b32.s0 = (L_CODE * (mem[p].b16.s0 / L_CODE) + 3);
                        mem[temp_ptr].b32.s1 = LR_ptr;
                        LR_ptr = temp_ptr;
                    }
                    if ((n > MIN_HALFWORD) || ((mem[p].b16.s0 / R_CODE) != cur_dir)) {
                        n++;
                        mem[p].b16.s0++;
                    } else {

                        NODE_type(p) = KERN_NODE;
                        m++;
                    }
                }
            }
            mem[p].b32.s1 = l;
            l = p;
        }
    goto done;
    mem[t + 1].b32.s1 = mem[p + 1].b32.s1;
    mem[t].b32.s1 = q;
    free_node(p, SMALL_NODE_SIZE);
done:
    mem[TEMP_HEAD].b32.s1 = l;
}


void
get_r_token(void)
{
restart:
    do {
        get_token();
    } while (cur_tok == SPACE_TOKEN);

    if (cur_cs == 0 || cur_cs > eqtb_top || (cur_cs > FROZEN_CONTROL_SEQUENCE && cur_cs <= EQTB_SIZE)) {
        error_here_with_diagnostic("Missing control sequence inserted");
        capture_to_diagnostic(NULL);

        help_ptr = 5;
        help_line[4] = "Please don't say `\\def cs{...}', say `\\def\\cs{...}'.";
        help_line[3] = "I've inserted an inaccessible control sequence so that your";
        help_line[2] = "definition will be completed without mixing me up too badly.";
        help_line[1] = "You can recover graciously from this error, if you're";
        help_line[0] = "careful; see exercise 27.2 in The TeXbook.";

        if (cur_cs == 0)
            back_input();

        cur_tok = CS_TOKEN_FLAG + FROZEN_PROTECTION;
        ins_error();
        goto restart;
    }
}


void trap_zero_glue(void)
{

    if ((mem[cur_val + 1].b32.s1 == 0) && (mem[cur_val + 2].b32.s1 == 0) && (mem[cur_val + 3].b32.s1 == 0)) {
        GLUE_SPEC_ref_count(0)++;
        delete_glue_ref(cur_val);
        cur_val = 0;
    }
}


void
do_register_command(small_number a)
{
    int32_t l = TEX_NULL, q, r, s = TEX_NULL;
    unsigned char /*mu_val */ p;
    bool e;
    int32_t w = 0;

    q = cur_cmd;
    e = false;

    if (q != REGISTER) {
        get_x_token();

        if (cur_cmd >= ASSIGN_INT && cur_cmd <= ASSIGN_MU_GLUE) {
            l = cur_chr;
            p = cur_cmd - ASSIGN_INT;
            goto found;
        }

        if (cur_cmd != REGISTER) {
            error_here_with_diagnostic("You can't use `");
            print_cmd_chr(cur_cmd, cur_chr);
            print_cstr("' after ");
            print_cmd_chr(q, 0);
            capture_to_diagnostic(NULL);

            help_ptr = 1;
            help_line[0] = "I'm forgetting what you said and not changing anything.";
            error();
            return;
        }
    }

    if (cur_chr < 0 || cur_chr > 19 /*lo_mem_stat_max*/) {
        l = cur_chr;
        p = (mem[l].b16.s1 / 64);
        e = true;
    } else {
        p = cur_chr;
        scan_register_num();
        if (cur_val > 255) {
            find_sa_element(p, cur_val, true);
            l = cur_ptr;
            e = true;
        } else {
            switch (p) {
            case INT_VAL:
                l = cur_val + COUNT_BASE;
                break;
            case DIMEN_VAL:
                l = cur_val + SCALED_BASE;
                break;
            case GLUE_VAL:
                l = cur_val + SKIP_BASE;
                break;
            case MU_VAL:
                l = cur_val + MU_SKIP_BASE;
                break;
            }
        }
    }

found:
    if (p < GLUE_VAL) {
        if (e)
            w = mem[l + 2].b32.s1;
        else
            w = eqtb[l].b32.s1;
    } else if (e) {
        s = mem[l + 1].b32.s1;
    } else {
        s = eqtb[l].b32.s1; /*:1272*/
    }

    if (q == REGISTER)
        scan_optional_equals();
    else
        scan_keyword("by");

    arith_error = false;

    if (q < MULTIPLY) { /*1273:*/
        if (p < GLUE_VAL) {
            if (p == INT_VAL)
                scan_int();
            else
                scan_dimen(false, false, false);

            if (q == ADVANCE)
                cur_val = cur_val + w;
        } else {
            scan_glue(p);

            if (q == ADVANCE) { /*1274:*/
                q = new_spec(cur_val);
                r = s;
                delete_glue_ref(cur_val);
                mem[q + 1].b32.s1 = mem[q + 1].b32.s1 + mem[r + 1].b32.s1;

                if (mem[q + 2].b32.s1 == 0)
                    mem[q].b16.s1 = NORMAL;

                if (mem[q].b16.s1 == mem[r].b16.s1) {
                    mem[q + 2].b32.s1 = mem[q + 2].b32.s1 + mem[r + 2].b32.s1;
                } else if (mem[q].b16.s1 < mem[r].b16.s1 && mem[r + 2].b32.s1 != 0) {
                    mem[q + 2].b32.s1 = mem[r + 2].b32.s1;
                    mem[q].b16.s1 = mem[r].b16.s1;
                }

                if (mem[q + 3].b32.s1 == 0)
                    mem[q].b16.s0 = NORMAL;

                if (mem[q].b16.s0 == mem[r].b16.s0) {
                    mem[q + 3].b32.s1 = mem[q + 3].b32.s1 + mem[r + 3].b32.s1;
                } else if (mem[q].b16.s0 < mem[r].b16.s0 && mem[r + 3].b32.s1 != 0) {
                    mem[q + 3].b32.s1 = mem[r + 3].b32.s1;
                    mem[q].b16.s0 = mem[r].b16.s0;
                }

                cur_val = q;
            }
        }
    } else { /*1275:*/
        scan_int();

        if (p < GLUE_VAL) {
            if (q == MULTIPLY) {
                if (p == INT_VAL)
                    cur_val = mult_and_add(w, cur_val, 0, TEX_INFINITY);
                else
                    cur_val = mult_and_add(w, cur_val, 0, MAX_HALFWORD);
            } else {
                cur_val = x_over_n(w, cur_val);
            }
        } else {
            r = new_spec(s);

            if (q == MULTIPLY) {
                mem[r + 1].b32.s1 = mult_and_add(mem[s + 1].b32.s1, cur_val, 0, MAX_HALFWORD);
                mem[r + 2].b32.s1 = mult_and_add(mem[s + 2].b32.s1, cur_val, 0, MAX_HALFWORD);
                mem[r + 3].b32.s1 = mult_and_add(mem[s + 3].b32.s1, cur_val, 0, MAX_HALFWORD);
            } else {
                mem[r + 1].b32.s1 = x_over_n(mem[s + 1].b32.s1, cur_val);
                mem[r + 2].b32.s1 = x_over_n(mem[s + 2].b32.s1, cur_val);
                mem[r + 3].b32.s1 = x_over_n(mem[s + 3].b32.s1, cur_val);
            }

            cur_val = r;
        }
    }

    if (arith_error) {
        error_here_with_diagnostic("Arithmetic overflow");
        capture_to_diagnostic(NULL);

        help_ptr = 2;
        help_line[1] = "I can't carry out that multiplication or division,";
        help_line[0] = "since the result is out of range.";
        if (p >= GLUE_VAL)
            delete_glue_ref(cur_val);
        error();
        return;
    }

    if (p < GLUE_VAL) {
        if (e) {
            if (a >= 4)
                gsa_w_def(l, cur_val);
            else
                sa_w_def(l, cur_val);
        } else if (a >= 4) {
            geq_word_define(l, cur_val);
        } else {
            eq_word_define(l, cur_val);
        }
    } else {
        trap_zero_glue();

        if (e) {
            if (a >= 4)
                gsa_def(l, cur_val);
            else
                sa_def(l, cur_val);
        } else if (a >= 4) {
            geq_define(l, GLUE_REF, cur_val);
        } else {
            eq_define(l, GLUE_REF, cur_val);
        }
    }
}


void alter_aux(void)
{
    int32_t c;
    if (cur_chr != abs(cur_list.mode))
        report_illegal_case();
    else {

        c = cur_chr;
        scan_optional_equals();
        if (c == VMODE) {
            scan_dimen(false, false, false);
            cur_list.aux.b32.s1 = cur_val;
        } else {

            scan_int();
            if ((cur_val <= 0) || (cur_val > 32767)) {
                ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Bad space factor");
                ttstub_diag_printf(errmsg, " (%d)", cur_val);
                capture_to_diagnostic(NULL);

                {
                    help_ptr = 1;
                    help_line[0] = "I allow only values in the range 1..32767 here.";
                }
                int_error(cur_val);
            } else
                cur_list.aux.b32.s0 = cur_val;
        }
    }
}

void alter_prev_graf(void)
{
    int32_t p;
    nest[nest_ptr] = cur_list;
    p = nest_ptr;
    while (abs(nest[p].mode) != VMODE)
        p--;
    scan_optional_equals();
    scan_int();
    if (cur_val < 0) {
        ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Bad ");
        ttstub_diag_printf(errmsg, " (%d)", cur_val);
        print_esc_cstr("prevgraf");
        capture_to_diagnostic(NULL);
        {
            help_ptr = 1;
            help_line[0] = "I allow only nonnegative values here.";
        }
        int_error(cur_val);
    } else {

        nest[p].prev_graf = cur_val;
        cur_list = nest[nest_ptr];
    }
}

void alter_page_so_far(void)
{
    unsigned char c;
    c = cur_chr;
    scan_optional_equals();
    scan_dimen(false, false, false);
    page_so_far[c] = cur_val;
}

void alter_integer(void)
{
    small_number c;
    c = cur_chr;
    scan_optional_equals();
    scan_int();
    if (c == 0)
        dead_cycles = /*1483: */ cur_val;
    else if (c == 2) {
        if ((cur_val < BATCH_MODE) || (cur_val > ERROR_STOP_MODE)) {
            ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Bad interaction mode");
            ttstub_diag_printf(errmsg, " (%d)", cur_val);
            capture_to_diagnostic(NULL);
            {
                help_ptr = 2;
                help_line[1] = "Modes are 0=batch, 1=nonstop, 2=scroll, and";
                help_line[0] = "3=errorstop. Proceed, and I'll ignore this case.";
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
    small_number c;
    int32_t b;

    c = cur_chr;
    scan_register_num();
    if (cur_val < 256)
        b = BOX_REG(cur_val);
    else {

        find_sa_element(4, cur_val, false);
        if (cur_ptr == TEX_NULL)
            b = TEX_NULL;
        else
            b = mem[cur_ptr + 1].b32.s1;
    }
    scan_optional_equals();
    scan_dimen(false, false, false);
    if (b != TEX_NULL)
        mem[b + c].b32.s1 = cur_val;
}

void new_font(small_number a)
{
    int32_t u;
    scaled_t s;
    internal_font_number f;
    str_number t;
    unsigned char /*max_selector */ old_setting;

    if (job_name == 0)
        open_log_file();

    get_r_token();
    u = cur_cs;
    if (u >= HASH_BASE)
        t = hash[u].s1;
    else if (u >= SINGLE_BASE) {

        if (u == NULL_CS)
            t = maketexstring("FONT");
        else
            t = u - SINGLE_BASE;
    } else {

        old_setting = selector;
        selector = SELECTOR_NEW_STRING ;
        print_cstr("FONT");
        print(u - 1);
        selector = old_setting;
        {
            if (pool_ptr + 1 > pool_size)
                overflow("pool size", pool_size - init_pool_ptr);
        }
        t = make_string();
    }
    if ((a >= 4))
        geq_define(u, SET_FONT, FONT_BASE);
    else
        eq_define(u, SET_FONT, FONT_BASE);
    scan_optional_equals();
    scan_file_name();
    name_in_progress = true;
    if (scan_keyword("at")) {      /*1294: */
        scan_dimen(false, false, false);
        s = cur_val;
        if ((s <= 0) || (s >= 0x8000000)) {
            error_here_with_diagnostic("Improper `at' size (");
            print_scaled(s);
            print_cstr("pt), replaced by 10pt");
            capture_to_diagnostic(NULL);
            {
                help_ptr = 2;
                help_line[1] = "I can only handle fonts at positive sizes that are";
                help_line[0] = "less than 2048pt, so I've changed what you said to 10pt.";
            }
            error();
            s = 10 * 65536L;
        }
    } else if (scan_keyword("scaled")) {
        scan_int();
        s = -(int32_t) cur_val;
        if ((cur_val <= 0) || (cur_val > 32768L)) {
            ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Illegal magnification has been changed to 1000");
            ttstub_diag_printf(errmsg, " (%d)", cur_val);
            capture_to_diagnostic(NULL);

            {
                help_ptr = 1;
                help_line[0] = "The magnification ratio must be between 1 and 32768.";
            }
            int_error(cur_val);
            s = -1000;
        }
    } else
        s = -1000;
    name_in_progress = false /*:1293 */ ;
    {
        register int32_t for_end;
        f = (FONT_BASE + 1);
        for_end = font_ptr;
        if (f <= for_end)
            do {
                if (
                        str_eq_str(font_name[f], cur_name) &&
                        (
                            (
                                (length(cur_area) == 0) &&
                                ((font_area[f] == AAT_FONT_FLAG) || (font_area[f] == OTGR_FONT_FLAG))
                            ) || str_eq_str(font_area[f], cur_area)
                        )
                    ) {
                    if (s > 0) {
                        if (s == font_size[f])
                            goto common_ending;
                    } else if (font_size[f] == xn_over_d(font_dsize[f], -(int32_t) s, 1000))
                        goto common_ending;
                }
                append_str(cur_area);
                append_str(cur_name);
                append_str(cur_ext);
                if (str_eq_str(font_name[f], make_string())) {
                    {
                        str_ptr--;
                        pool_ptr = str_start[str_ptr - TOO_BIG_CHAR];
                    }
                    if (((font_area[f] == AAT_FONT_FLAG) || (font_area[f] == OTGR_FONT_FLAG))) {
                        if (s > 0) {
                            if (s == font_size[f])
                                goto common_ending;
                        } else if (font_size[f] == xn_over_d(font_dsize[f], -(int32_t) s, 1000))
                            goto common_ending;
                    }
                } else {

                    str_ptr--;
                    pool_ptr = str_start[str_ptr - TOO_BIG_CHAR];
                }
            }
            while (f++ < for_end);
    }
    f = read_font_info(u, cur_name, cur_area, s);

common_ending:
    if ((a >= 4))
        geq_define(u, SET_FONT, f);
    else
        eq_define(u, SET_FONT, f);
    eqtb[FONT_ID_BASE + f] = eqtb[u];
    hash[FONT_ID_BASE + f].s1 = t;
}

void new_interaction(void)
{
    print_ln();
    interaction = cur_chr;
    if (interaction == BATCH_MODE)
        selector = SELECTOR_NO_PRINT;
    else
        selector = SELECTOR_TERM_ONLY/*:79 */ ;
    if (log_opened)
        selector = selector + 2;
}

void issue_message(void)
{
    unsigned char /*max_selector */ old_setting;
    unsigned char c;
    str_number s;

    c = cur_chr;
    mem[GARBAGE].b32.s1 = scan_toks(false, true);
    old_setting = selector;
    selector = SELECTOR_NEW_STRING ;
    token_show(def_ref);
    selector = old_setting;
    flush_list(def_ref);
    {
        if (pool_ptr + 1 > pool_size)
            overflow("pool size", pool_size - init_pool_ptr);
    }
    s = make_string();
    if (c == 0) {               /*1315: */
        if (term_offset + length(s) > max_print_line - 2)
            print_ln();
        else if ((term_offset > 0) || (file_offset > 0))
            print_char(' ');
        print(s);
        ttstub_output_flush (rust_stdout);
    } else {                    /*1318: */
        error_here_with_diagnostic("");
        print(s);
        capture_to_diagnostic(NULL);

        if (LOCAL(err_help) != TEX_NULL)
            use_err_help = true;
        else if (long_help_seen) {
            help_ptr = 1;
            help_line[0] = "(That was another \\errmessage.)";
        } else {

            if (interaction < ERROR_STOP_MODE)
                long_help_seen = true;
            {
                help_ptr = 4;
                help_line[3] = "This error message was generated by an \\errmessage";
                help_line[2] = "command, so I can't give any explicit help.";
                help_line[1] = "Pretend that you're Hercule Poirot: Examine all clues,";
                help_line[0] = "and deduce the truth by order and method.";
            }
        }
        error();
        use_err_help = false;
    }
    {
        str_ptr--;
        pool_ptr = str_start[str_ptr - TOO_BIG_CHAR];
    }
}


void
shift_case(void)
{
    int32_t b;
    int32_t p;
    int32_t t;
    int32_t c;

    b = cur_chr;
    p = scan_toks(false, false);
    p = mem[def_ref].b32.s1;

    while (p != TEX_NULL) {
        t = mem[p].b32.s0;
        if (t < CS_TOKEN_FLAG + SINGLE_BASE) {
            c = t % MAX_CHAR_VAL;
            if (eqtb[b + c].b32.s1 != 0)
                mem[p].b32.s0 = t - c + eqtb[b + c].b32.s1;
        }
        p = LLIST_link(p);
    }

    begin_token_list(mem[def_ref].b32.s1, BACKED_UP);
    mem[def_ref].b32.s1 = avail;
    avail = def_ref;
}


void show_whatever(void)
{
    int32_t p;
    small_number t;
    unsigned char /*or_code */ m;
    int32_t l;
    int32_t n;

    // In all cases we eventually call error().
    diagnostic_begin_capture_warning_here();

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
                p = BOX_REG(cur_val);
            else {

                find_sa_element(4, cur_val, false);
                if (cur_ptr == TEX_NULL)
                    p = TEX_NULL;
                else
                    p = mem[cur_ptr + 1].b32.s1;
            }
            begin_diagnostic();
            print_nl_cstr("> \\box");
            print_int(cur_val);
            print_char('=');
            if (p == TEX_NULL)
                print_cstr("void");
            else
                show_box(p);
        }
        break;
    case 0:
        {
            get_token();
            print_nl_cstr("> ");
            if (cur_cs != 0) {
                sprint_cs(cur_cs);
                print_char('=');
            }
            print_meaning();
            goto common_ending;
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
            print_nl_cstr("");
            print_ln();
            if (cond_ptr == TEX_NULL) {
                print_nl_cstr("### ");
                print_cstr("no active conditionals");
            } else {

                p = cond_ptr;
                n = 0;
                do {
                    n++;
                    p = LLIST_link(p);
                } while (!(p == TEX_NULL));
                p = cond_ptr;
                t = cur_if;
                l = if_line;
                m = if_limit;
                do {
                    print_nl_cstr("### level ");
                    print_int(n);
                    print_cstr(": ");
                    print_cmd_chr(IF_TEST, t);
                    if (m == FI_CODE)
                        print_esc_cstr("else");
                    if (l != 0) {
                        print_cstr(" entered on line ");
                        print_int(l);
                    }
                    n--;
                    t = mem[p].b16.s0;
                    l = mem[p + 1].b32.s1;
                    m = mem[p].b16.s1;
                    p = LLIST_link(p);
                } while (!(p == TEX_NULL));
            }
        }
        break;
    default:
        {
            p = the_toks();
            print_nl_cstr("> ");
            token_show(TEMP_HEAD);
            flush_list(mem[TEMP_HEAD].b32.s1);
            goto common_ending;
        }
        break;
    }
    capture_to_diagnostic(NULL);
    end_diagnostic(true);
    {
        if (file_line_error_style_p)
            print_file_line();
        else
            print_nl_cstr("! ");
        print_cstr("OK");
    }
    if (selector == SELECTOR_TERM_AND_LOG) {

        if (INTPAR(tracing_online) <= 0) {
            selector = SELECTOR_TERM_ONLY;
            print_cstr(" (see the transcript file)");
            selector = SELECTOR_TERM_AND_LOG;
        }
    }

common_ending:
    capture_to_diagnostic(NULL); // calling with null twice is fine
    if (interaction < ERROR_STOP_MODE) {
        help_ptr = 0;
        error_count--;
    } else if (INTPAR(tracing_online) > 0) {
        {
            help_ptr = 3;
            help_line[2] = "This isn't an error message; I'm just \\showing something.";
            help_line[1] = "Type `I\\show...' to show more (e.g., \\show\\cs,";
            help_line[0] = "\\showthe\\count10, \\showbox255, \\showlists).";
        }
    } else {

        {
            help_ptr = 5;
            help_line[4] = "This isn't an error message; I'm just \\showing something.";
            help_line[3] = "Type `I\\show...' to show more (e.g., \\show\\cs,";
            help_line[2] = "\\showthe\\count10, \\showbox255, \\showlists).";
            help_line[1] = "And type `I\\tracingonline=1\\show...' to show boxes and";
            help_line[0] = "lists on your terminal as well as in the transcript file.";
        }
    }
    error();
}

void new_write_whatsit(small_number w)
{
    new_whatsit(cur_chr, w);
    if (w != WRITE_NODE_SIZE)
        scan_four_bit_int();
    else {

        scan_int();
        if (cur_val < 0)
            cur_val = 17;
        else if ((cur_val > 15) && (cur_val != 18))
            cur_val = 16;
    }
    mem[cur_list.tail + 1].b32.s0 = cur_val;
}

void scan_and_pack_name(void)
{
    scan_file_name();
    pack_file_name(cur_name, cur_area, cur_ext);
}

void do_extension(void)
{
    int32_t i, j, k;
    int32_t p;

    switch (cur_chr) {
    case OPEN_NODE:
        {
            new_write_whatsit(OPEN_NODE_SIZE);
            scan_optional_equals();
            scan_file_name();
            mem[cur_list.tail + 1].b32.s1 = cur_name;
            mem[cur_list.tail + 2].b32.s0 = cur_area;
            mem[cur_list.tail + 2].b32.s1 = cur_ext;
        }
        break;

    case WRITE_NODE:
        {
            k = cur_cs;
            new_write_whatsit(WRITE_NODE_SIZE);
            cur_cs = k;
            p = scan_toks(false, false);
            mem[cur_list.tail + 1].b32.s1 = def_ref;
        }
        break;

    case CLOSE_NODE:
        {
            new_write_whatsit(WRITE_NODE_SIZE);
            mem[cur_list.tail + 1].b32.s1 = TEX_NULL;
        }
        break;

    case SPECIAL_NODE:
        {
            new_whatsit(SPECIAL_NODE, WRITE_NODE_SIZE);
            mem[cur_list.tail + 1].b32.s0 = TEX_NULL;
            p = scan_toks(false, true);
            mem[cur_list.tail + 1].b32.s1 = def_ref;
        }
        break;

    case IMMEDIATE_CODE:
        {
            get_x_token();
            if ((cur_cmd == EXTENSION) && (cur_chr <= CLOSE_NODE)) {
                p = cur_list.tail;
                do_extension();
                out_what(cur_list.tail);
                flush_node_list(cur_list.tail);
                cur_list.tail = p;
                mem[p].b32.s1 = TEX_NULL;
            } else
                back_input();
        }
        break;

    case SET_LANGUAGE_CODE:
        if (abs(cur_list.mode) != HMODE)
            report_illegal_case();
        else {
            new_whatsit(LANGUAGE_NODE, SMALL_NODE_SIZE);
            scan_int();
            if (cur_val <= 0)
                cur_list.aux.b32.s1 = 0;
            else if (cur_val > 255)
                cur_list.aux.b32.s1 = 0;
            else
                cur_list.aux.b32.s1 = cur_val;
            mem[cur_list.tail + 1].b32.s1 = cur_list.aux.b32.s1;
            mem[cur_list.tail + 1].b16.s1 = norm_min(INTPAR(left_hyphen_min));
            mem[cur_list.tail + 1].b16.s0 = norm_min(INTPAR(right_hyphen_min));
        }
        break;

    case PDF_SAVE_POS_NODE:
        new_whatsit(PDF_SAVE_POS_NODE, SMALL_NODE_SIZE);
        break;

    case RESET_TIMER_CODE:
        get_seconds_and_micros(&epochseconds, &microseconds);
        break;

    case SET_RANDOM_SEED_CODE:
        scan_int();
        if (cur_val < 0)
            cur_val = -cur_val;
        random_seed = cur_val;
        init_randoms(random_seed);
        break;

    case PIC_FILE_CODE:
        if (abs(cur_list.mode) == MMODE)
            report_illegal_case();
        else
            load_picture(false);
        break;

    case PDF_FILE_CODE:
        if (abs(cur_list.mode) == MMODE)
            report_illegal_case();
        else
            load_picture(true);
        break;

    case GLYPH_CODE:
        {
            if (abs(cur_list.mode) == VMODE) {
                back_input();
                new_graf(true);
            } else if (abs(cur_list.mode) == MMODE)
                report_illegal_case();
            else {

                if (((font_area[eqtb[CUR_FONT_LOC].b32.s1] == AAT_FONT_FLAG)
                     || (font_area[eqtb[CUR_FONT_LOC].b32.s1] == OTGR_FONT_FLAG))) {
                    new_whatsit(GLYPH_NODE, GLYPH_NODE_SIZE);
                    scan_int();
                    if ((cur_val < 0) || (cur_val > 65535L)) {
                        ttbc_diagnostic_t *errmsg = error_here_with_diagnostic("Bad glyph number");
                        ttstub_diag_printf(errmsg, " (%d)", cur_val);
                        capture_to_diagnostic(NULL);

                        {
                            help_ptr = 2;
                            help_line[1] = "A glyph number must be between 0 and 65535.";
                            help_line[0] = "I changed this one to zero.";
                        }
                        int_error(cur_val);
                        cur_val = 0;
                    }
                    mem[cur_list.tail + 4].b16.s2 = eqtb[CUR_FONT_LOC].b32.s1;
                    mem[cur_list.tail + 4].b16.s1 = cur_val;
                    set_native_glyph_metrics(cur_list.tail, (INTPAR(xetex_use_glyph_metrics) > 0));
                } else
                    not_native_font_error(EXTENSION, GLYPH_CODE,
                                          eqtb[CUR_FONT_LOC].b32.s1);
            }
        }
        break;

    case XETEX_INPUT_ENCODING_EXTENSION_CODE:
        {
            scan_and_pack_name();
            i = get_encoding_mode_and_info(&j);
            if (i == XETEX_INPUT_MODE_AUTO) {
                error_here_with_diagnostic("Encoding mode `auto' is not valid for \\XeTeXinputencoding");
                capture_to_diagnostic(NULL);
                {
                    help_ptr = 2;
                    help_line[1] = "You can't use `auto' encoding here, only for \\XeTeXdefaultencoding.";
                    help_line[0] = "I'll ignore this and leave the current encoding unchanged.";
                }
                error();
            } else
                set_input_file_encoding(input_file[in_open], i, j);
        }
        break;

    case XETEX_DEFAULT_ENCODING_EXTENSION_CODE:
        {
            scan_and_pack_name();
            i = get_encoding_mode_and_info(&j);
            INTPAR(xetex_default_input_mode) = i;
            INTPAR(xetex_default_input_encoding) = j;
        }
        break;

    case XETEX_LINEBREAK_LOCALE_EXTENSION_CODE:
        {
            scan_file_name();
            if (length(cur_name) == 0)
                INTPAR(xetex_linebreak_locale) = 0;
            else
                INTPAR(xetex_linebreak_locale) = cur_name;
        }
        break;

    default:
        confusion("ext1");
        break;
    }
}

void fix_language(void)
{
    UTF16_code l;

    if (INTPAR(language) <= 0)
        l = 0;
    else if (INTPAR(language) > 255)
        l = 0;
    else
        l = INTPAR(language);
    if (l != cur_list.aux.b32.s1) {
        new_whatsit(LANGUAGE_NODE, SMALL_NODE_SIZE);
        mem[cur_list.tail + 1].b32.s1 = l;
        cur_list.aux.b32.s1 = l;
        mem[cur_list.tail + 1].b16.s1 = norm_min(INTPAR(left_hyphen_min));
        mem[cur_list.tail + 1].b16.s0 = norm_min(INTPAR(right_hyphen_min));
    }
}


void
insert_src_special(void)
{
    int32_t toklist, p, q;

    if (source_filename_stack[in_open] > 0 && is_new_source(source_filename_stack[in_open], line)) {
        toklist = get_avail();
        p = toklist;
        mem[p].b32.s0 = CS_TOKEN_FLAG + FROZEN_SPECIAL;
        mem[p].b32.s1 = get_avail();
        p = LLIST_link(p);
        mem[p].b32.s0 = (LEFT_BRACE_TOKEN + '{' );
        q = str_toks(make_src_special(source_filename_stack[in_open], line));
        mem[p].b32.s1 = mem[TEMP_HEAD].b32.s1;
        p = q;
        mem[p].b32.s1 = get_avail();
        p = LLIST_link(p);
        mem[p].b32.s0 = (RIGHT_BRACE_TOKEN + '}' );
        begin_token_list(toklist, INSERTED);
        remember_source_info(source_filename_stack[in_open], line);
    }
}


void append_src_special(void)
{
    if ((source_filename_stack[in_open] > 0 && is_new_source(source_filename_stack[in_open], line))) {
        new_whatsit(SPECIAL_NODE, WRITE_NODE_SIZE);
        mem[cur_list.tail + 1].b32.s0 = 0;
        def_ref = get_avail();
        mem[def_ref].b32.s0 = TEX_NULL;
        str_toks(make_src_special(source_filename_stack[in_open], line));
        mem[def_ref].b32.s1 = mem[TEMP_HEAD].b32.s1;
        mem[cur_list.tail + 1].b32.s1 = def_ref;
        remember_source_info(source_filename_stack[in_open], line);
    }
}


void
handle_right_brace(void)
{
    int32_t p, q;
    scaled_t d;
    int32_t f;

    switch (cur_group) {
    case SIMPLE_GROUP:
        unsave();
        break;

    case BOTTOM_LEVEL:
        error_here_with_diagnostic("Too many }'s");
        capture_to_diagnostic(NULL);
        help_ptr = 2;
        help_line[1] = "You've closed more groups than you opened.";
        help_line[0] = "Such booboos are generally harmless, so keep going.";
        error();
        break;

    case SEMI_SIMPLE_GROUP:
    case MATH_SHIFT_GROUP:
    case MATH_LEFT_GROUP:
        extra_right_brace();
        break;

    case HBOX_GROUP:
        package(0);
        break;

    case ADJUSTED_HBOX_GROUP:
        adjust_tail = ADJUST_HEAD;
        pre_adjust_tail = PRE_ADJUST_HEAD;
        package(0);
        break;

    case VBOX_GROUP:
        end_graf();
        package(0);
        break;

    case VTOP_GROUP:
        end_graf();
        package(VTOP_CODE);
        break;

    case INSERT_GROUP:
        end_graf();
        q = GLUEPAR(split_top_skip);
        GLUE_SPEC_ref_count(q)++;
        d = DIMENPAR(split_max_depth);
        f = INTPAR(floating_penalty);
        unsave();
        save_ptr = save_ptr - 2;
        p = vpackage(mem[cur_list.head].b32.s1, 0, ADDITIONAL, MAX_HALFWORD);
        pop_nest();

        if (save_stack[save_ptr + 0].b32.s1 < 255) {
            mem[cur_list.tail].b32.s1 = get_node(INS_NODE_SIZE);
            cur_list.tail = LLIST_link(cur_list.tail);
            NODE_type(cur_list.tail) = INS_NODE;
            mem[cur_list.tail].b16.s0 = save_stack[save_ptr + 0].b32.s1;
            mem[cur_list.tail + 3].b32.s1 = mem[p + 3].b32.s1 + mem[p + 2].b32.s1;
            mem[cur_list.tail + 4].b32.s0 = mem[p + 5].b32.s1;
            mem[cur_list.tail + 4].b32.s1 = q;
            mem[cur_list.tail + 2].b32.s1 = d;
            mem[cur_list.tail + 1].b32.s1 = f;
        } else {
            mem[cur_list.tail].b32.s1 = get_node(SMALL_NODE_SIZE);
            cur_list.tail = LLIST_link(cur_list.tail);
            NODE_type(cur_list.tail) = ADJUST_NODE;
            mem[cur_list.tail].b16.s0 = save_stack[save_ptr + 1].b32.s1;
            mem[cur_list.tail + 1].b32.s1 = mem[p + 5].b32.s1;
            delete_glue_ref(q);
        }

        free_node(p, BOX_NODE_SIZE);
        if (nest_ptr == 0)
            build_page();
        break;

    case OUTPUT_GROUP: /*1062:*/
        if (cur_input.loc != TEX_NULL || (cur_input.index != OUTPUT_TEXT && cur_input.index != BACKED_UP)) {
            error_here_with_diagnostic("Unbalanced output routine");
            capture_to_diagnostic(NULL);
            help_ptr = 2;
            help_line[1] = "Your sneaky output routine has problematic {'s and/or }'s.";
            help_line[0] = "I can't handle that very well; good luck.";
            error();

            do {
                get_token();
            } while (cur_input.loc != TEX_NULL);
        }

        end_token_list();
        end_graf();
        unsave();
        output_active = false;
        insert_penalties = 0;

        if (BOX_REG(255) != TEX_NULL) {
            error_here_with_diagnostic("Output routine didn't use all of ");
            print_esc_cstr("box");
            print_int(255);
            capture_to_diagnostic(NULL);

            help_ptr = 3;
            help_line[2] = "Your \\output commands should empty \\box255,";
            help_line[1] = "e.g., by saying `\\shipout\\box255'.";
            help_line[0] = "Proceed; I'll discard its present contents.";
            box_error(255);
        }

        if (cur_list.tail != cur_list.head) {
            mem[page_tail].b32.s1 = mem[cur_list.head].b32.s1;
            page_tail = cur_list.tail;
        }

        if (mem[PAGE_HEAD].b32.s1 != TEX_NULL) {
            if (mem[CONTRIB_HEAD].b32.s1 == TEX_NULL)
                nest[0].tail = page_tail;
            mem[page_tail].b32.s1 = mem[CONTRIB_HEAD].b32.s1;
            mem[CONTRIB_HEAD].b32.s1 = mem[PAGE_HEAD].b32.s1;
            mem[PAGE_HEAD].b32.s1 = TEX_NULL;
            page_tail = PAGE_HEAD;
        }

        flush_node_list(disc_ptr[LAST_BOX_CODE]);
        disc_ptr[LAST_BOX_CODE] = TEX_NULL;
        pop_nest();
        build_page();
        break;

    case DISC_GROUP:
        build_discretionary();
        break;

    case ALIGN_GROUP:
        back_input();
        cur_tok = CS_TOKEN_FLAG + FROZEN_CR;

        error_here_with_diagnostic("Missing ");
        print_esc_cstr("cr");
        print_cstr(" inserted");
        capture_to_diagnostic(NULL);

        help_ptr = 1;
        help_line[0] = "I'm guessing that you meant to end an alignment here.";
        ins_error();
        break;

    case NO_ALIGN_GROUP:
        end_graf();
        unsave();
        align_peek();
        break;

    case VCENTER_GROUP:
        end_graf();
        unsave();
        save_ptr = save_ptr - 2;
        p = vpackage(mem[cur_list.head].b32.s1,
                     save_stack[save_ptr + 1].b32.s1,
                     save_stack[save_ptr + 0].b32.s1,
                     MAX_HALFWORD);
        pop_nest();
        mem[cur_list.tail].b32.s1 = new_noad();
        cur_list.tail = LLIST_link(cur_list.tail);
        mem[cur_list.tail].b16.s1 = VCENTER_NOAD;
        mem[cur_list.tail + 1].b32.s1 = SUB_BOX;
        mem[cur_list.tail + 1].b32.s0 = p;
        break;

    case MATH_CHOICE_GROUP:
        build_choices();
        break;

    case MATH_GROUP:
        unsave();
        save_ptr--;
        mem[save_stack[save_ptr + 0].b32.s1].b32.s1 = SUB_MLIST;
        p = fin_mlist(TEX_NULL);
        mem[save_stack[save_ptr + 0].b32.s1].b32.s0 = p;

        if (p != TEX_NULL) {
            if (mem[p].b32.s1 == TEX_NULL) {
                if (mem[p].b16.s1 == ORD_NOAD) {
                    if (mem[p + 3].b32.s1 == EMPTY) {
                        if (mem[p + 2].b32.s1 == EMPTY) {
                            mem[save_stack[save_ptr + 0].b32.s1].b32 = mem[p + 1].b32;
                            free_node(p, NOAD_SIZE);
                        }
                    }
                } else if (mem[p].b16.s1 == ACCENT_NOAD) {
                    if (save_stack[save_ptr + 0].b32.s1 == cur_list.tail + 1) {
                        if (mem[cur_list.tail].b16.s1 == ORD_NOAD) { /*1222:*/
                            q = cur_list.head;
                            while (mem[q].b32.s1 != cur_list.tail)
                                q = LLIST_link(q);
                            mem[q].b32.s1 = p;
                            free_node(cur_list.tail, NOAD_SIZE);
                            cur_list.tail = p;
                        }
                    }
                }
            }
        }
        break;

    default:
        confusion("rightbrace");
        break;
    }
}


void main_control(void)
{
    int32_t t;

    if (LOCAL(every_job) != TEX_NULL)
        begin_token_list(LOCAL(every_job), EVERY_JOB_TEXT);

big_switch: /* big_switch */
    get_x_token();

reswitch:
    /*1066: */

    if (INTPAR(tracing_commands) > 0)
        show_cur_cmd_chr();
    switch (abs(cur_list.mode) + cur_cmd) {
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
            if ((cur_cmd == LETTER) || (cur_cmd == OTHER_CHAR) || (cur_cmd == CHAR_GIVEN)
                || (cur_cmd == CHAR_NUM))
                cancel_boundary = true;
            goto reswitch;
        }
        break;
    default:
        {
            if (abs(cur_list.mode) == HMODE) {

                if ((INTPAR(xetex_inter_char_tokens) > 0) && (space_class != CHAR_CLASS_LIMIT)
                    && (prev_class != ((CHAR_CLASS_LIMIT - 1)))) {
                    prev_class = ((CHAR_CLASS_LIMIT - 1));
                    find_sa_element(INTER_CHAR_VAL,
                                    space_class * CHAR_CLASS_LIMIT + ((CHAR_CLASS_LIMIT - 1)),
                                    false);
                    if (cur_ptr != TEX_NULL && ETEX_SA_ptr(cur_ptr) != TEX_NULL) {
                        if (cur_cs == 0) {
                            if (cur_cmd == CHAR_NUM)
                                cur_cmd = OTHER_CHAR;
                            cur_tok = (cur_cmd * MAX_CHAR_VAL) + cur_chr;
                        } else
                            cur_tok = CS_TOKEN_FLAG + cur_cs;
                        back_input();
                        begin_token_list(mem[cur_ptr + 1].b32.s1, INTER_CHAR_TEXT);
                        goto big_switch;
                    }
                }
            }
            switch (abs(cur_list.mode) + cur_cmd) {
            case 114:
                if (cur_list.aux.b32.s0 == 1000)
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
                        } while (cur_cmd == SPACER);
                        goto reswitch;
                    } else {

                        t = scanner_status;
                        scanner_status = NORMAL;
                        get_next();
                        scanner_status = t;
                        if (cur_cs < HASH_BASE)
                            cur_cs = prim_lookup(cur_cs - SINGLE_BASE);
                        else
                            cur_cs = prim_lookup(hash[cur_cs].s1);
                        if (cur_cs != UNDEFINED_PRIMITIVE) {
                            cur_cmd = eqtb[PRIM_EQTB_BASE + cur_cs].b16.s1;
                            cur_chr = eqtb[PRIM_EQTB_BASE + cur_cs].b32.s1;
                            cur_tok = CS_TOKEN_FLAG + PRIM_EQTB_BASE + cur_cs;
                            goto reswitch;
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
                        mem[cur_list.tail].b32.s1 = scan_rule_spec();
                        cur_list.tail = LLIST_link(cur_list.tail);
                    }
                    if (abs(cur_list.mode) == VMODE)
                        cur_list.aux.b32.s1 = IGNORE_DEPTH;
                    else if (abs(cur_list.mode) == HMODE)
                        cur_list.aux.b32.s0 = 1000;
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
                new_save_level(SIMPLE_GROUP);
                break;
            case 62:
            case 165:
            case 268:
                new_save_level(SEMI_SIMPLE_GROUP);
                break;
            case 63:
            case 166:
            case 269:
                if (cur_group == SEMI_SIMPLE_GROUP)
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
                        scan_box(-(int32_t) cur_val);
                }
                break;
            case 32:
            case 135:
            case 238:
                scan_box(LEADER_FLAG - A_LEADERS + cur_chr);
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
                    if (cur_list.mode > 0)
                        build_page();
                }
                break;
            case 117:
                {
                    if (align_state < 0)
                        off_save();
                    end_graf();
                    if (cur_list.mode == VMODE)
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
                    mem[cur_list.tail].b32.s1 = new_kern(0);
                    cur_list.tail = LLIST_link(cur_list.tail);
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
                    if (eTeX_enabled(INTPAR(texxet) > 0, cur_cmd, cur_chr)) {
                        mem[cur_list.tail].b32.s1 = new_math(0, cur_chr);
                        cur_list.tail = LLIST_link(cur_list.tail);
                    }
                } else /*:1490 */
                    init_align();
                break;
            case 239:
                if (privileged()) {

                    if (cur_group == MATH_SHIFT_GROUP)
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

                    if (cur_group == MATH_SHIFT_GROUP)
                        start_eq_no();
                    else
                        off_save();
                }
                break;
            case 208:
                {
                    {
                        mem[cur_list.tail].b32.s1 = new_noad();
                        cur_list.tail = LLIST_link(cur_list.tail);
                    }
                    back_input();
                    scan_math(cur_list.tail + 1);
                }
                break;
            case 218:
            case 219:
            case 275:
                set_math_char(MATH_CODE(cur_chr));
                break;
            case 223:
                {
                    scan_char_num();
                    cur_chr = cur_val;
                    set_math_char(MATH_CODE(cur_chr));
                }
                break;
            case 224:
                if (cur_chr == 2) {
                    scan_math_class_int();
                    t = set_class(cur_val);
                    scan_math_fam_int();
                    t = t + set_family(cur_val);
                    scan_usv_num();
                    t = t + cur_val;
                    set_math_char(t);
                } else if (cur_chr == 1) {
                    scan_xetex_math_char_int();
                    set_math_char(cur_val);
                } else {

                    scan_fifteen_bit_int();
                    set_math_char(set_class(cur_val / 4096) + set_family((cur_val % 4096) / 256) +
                                  (cur_val % 256));
                }
                break;
            case 276:
                {
                    set_math_char(set_class(cur_chr / 4096) + set_family((cur_chr % 4096) / 256) +
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
                        t = set_class(cur_val);
                        scan_math_fam_int();
                        t = t + set_family(cur_val);
                        scan_usv_num();
                        t = t + cur_val;
                        set_math_char(t);
                    } else {

                        scan_delimiter_int();
                        cur_val = cur_val / 4096;
                        set_math_char(set_class(cur_val / 4096) + set_family((cur_val % 4096) / 256) +
                                      (cur_val % 256));
                    }
                }
                break;
            case 257:
                {
                    {
                        mem[cur_list.tail].b32.s1 = new_noad();
                        cur_list.tail = LLIST_link(cur_list.tail);
                    }
                    mem[cur_list.tail].b16.s1 = cur_chr;
                    scan_math(cur_list.tail + 1);
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
                    scan_spec(VCENTER_GROUP, false);
                    normal_paragraph();
                    push_nest();
                    cur_list.mode = -1;
                    cur_list.aux.b32.s1 = IGNORE_DEPTH;
                    if ((insert_src_special_every_vbox))
                        insert_src_special();
                    if (LOCAL(every_vbox) != TEX_NULL)
                        begin_token_list(LOCAL(every_vbox), EVERY_VBOX_TEXT);
                }
                break;
            case 260:
                {
                    mem[cur_list.tail].b32.s1 = new_style(cur_chr);
                    cur_list.tail = LLIST_link(cur_list.tail);
                }
                break;
            case 262:
                {
                    {
                        mem[cur_list.tail].b32.s1 = new_glue(0);
                        cur_list.tail = LLIST_link(cur_list.tail);
                    }
                    mem[cur_list.tail].b16.s0 = COND_MATH_GLUE;
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
                if (cur_group == MATH_SHIFT_GROUP)
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
    goto big_switch;
 lab70:                        /*main_loop *//*1069: */ if (((cur_list.head == cur_list.tail) && (cur_list.mode > 0))) {
        if ((insert_src_special_auto))
            append_src_special();
    }
    prev_class = ((CHAR_CLASS_LIMIT - 1));
    if (((font_area[eqtb[CUR_FONT_LOC].b32.s1] == AAT_FONT_FLAG)
         || (font_area[eqtb[CUR_FONT_LOC].b32.s1] == OTGR_FONT_FLAG))) {
        if (cur_list.mode > 0) {

            if (INTPAR(language) != cur_list.aux.b32.s1)
                fix_language();
        }
        main_h = 0;
        main_f = eqtb[CUR_FONT_LOC].b32.s1;
        native_len = 0;
 lab71:/*collect_native */ main_s = SF_CODE(cur_chr) % 65536L;
        if (main_s == 1000)
            cur_list.aux.b32.s0 = 1000;
        else if (main_s < 1000) {
            if (main_s > 0)
                cur_list.aux.b32.s0 = main_s;
        } else if (cur_list.aux.b32.s0 < 1000)
            cur_list.aux.b32.s0 = 1000;
        else
            cur_list.aux.b32.s0 = main_s;
        cur_ptr = TEX_NULL;
        space_class = SF_CODE(cur_chr) / 65536L;
        if ((INTPAR(xetex_inter_char_tokens) > 0) && space_class != CHAR_CLASS_LIMIT) {
            if (prev_class == ((CHAR_CLASS_LIMIT - 1))) {
                if ((cur_input.state != TOKEN_LIST) || (cur_input.index != BACKED_UP_CHAR)) {
                    find_sa_element(INTER_CHAR_VAL,
                                    ((CHAR_CLASS_LIMIT - 1)) * CHAR_CLASS_LIMIT + space_class,
                                    false);
                    if (cur_ptr != TEX_NULL && ETEX_SA_ptr(cur_ptr) != TEX_NULL) {
                        if (cur_cmd != LETTER)
                            cur_cmd = OTHER_CHAR;
                        cur_tok = (cur_cmd * MAX_CHAR_VAL) + cur_chr;
                        back_input();
                        cur_input.index = BACKED_UP_CHAR;
                        begin_token_list(mem[cur_ptr + 1].b32.s1, INTER_CHAR_TEXT);
                        goto big_switch;
                    }
                }
            } else {

                find_sa_element(INTER_CHAR_VAL, prev_class * CHAR_CLASS_LIMIT + space_class, false);
                if (cur_ptr != TEX_NULL && ETEX_SA_ptr(cur_ptr) != TEX_NULL) {
                    if (cur_cmd != LETTER)
                        cur_cmd = OTHER_CHAR;
                    cur_tok = (cur_cmd * MAX_CHAR_VAL) + cur_chr;
                    back_input();
                    cur_input.index = BACKED_UP_CHAR;
                    begin_token_list(mem[cur_ptr + 1].b32.s1, INTER_CHAR_TEXT);
                    prev_class = ((CHAR_CLASS_LIMIT - 1));
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
                native_text[native_len] = (cur_chr - 65536L) / 1024 + 0xD800;
                native_len++;
            }
            {
                native_text[native_len] = (cur_chr - 65536L) % 1024 + 0xDC00;
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
        is_hyph = (cur_chr == hyphen_char[main_f]) || ((INTPAR(xetex_dash_break) > 0)
                                                       && ((cur_chr == 8212) || (cur_chr == 8211)));
        if ((main_h == 0) && is_hyph)
            main_h = native_len;
        get_next();
        if ((cur_cmd == LETTER) || (cur_cmd == OTHER_CHAR) || (cur_cmd == CHAR_GIVEN))
            goto lab71;
        x_token();
        if ((cur_cmd == LETTER) || (cur_cmd == OTHER_CHAR) || (cur_cmd == CHAR_GIVEN))
            goto lab71;
        if (cur_cmd == CHAR_NUM) {
            scan_usv_num();
            cur_chr = cur_val;
            goto lab71;
        }
        if ((INTPAR(xetex_inter_char_tokens) > 0) && (space_class != CHAR_CLASS_LIMIT)
            && (prev_class != ((CHAR_CLASS_LIMIT - 1)))) {
            prev_class = ((CHAR_CLASS_LIMIT - 1));
            find_sa_element(INTER_CHAR_VAL,
                            space_class * CHAR_CLASS_LIMIT + ((CHAR_CLASS_LIMIT - 1)), false);
            if (cur_ptr != TEX_NULL && ETEX_SA_ptr(cur_ptr) != TEX_NULL) {
                if (cur_cs == 0) {
                    if (cur_cmd == CHAR_NUM)
                        cur_cmd = OTHER_CHAR;
                    cur_tok = (cur_cmd * MAX_CHAR_VAL) + cur_chr;
                } else
                    cur_tok = CS_TOKEN_FLAG + cur_cs;
                back_input();
                begin_token_list(mem[cur_ptr + 1].b32.s1, INTER_CHAR_TEXT);
                goto lab72;
            }
        }
 lab72:                        /*collected */ if ((font_mapping[main_f] != NULL)) {
            main_k = apply_mapping(font_mapping[main_f], native_text, native_len);
            native_len = 0;
            while (native_text_size <= native_len + main_k) {

                native_text_size = native_text_size + 128;
                native_text = xrealloc(native_text, native_text_size * sizeof(UTF16_code));
            }
            main_h = 0;
            {
                register int32_t for_end;
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
                                || ((INTPAR(xetex_dash_break) > 0)
                                    && ((mapped_text[main_p] == 8212) || (mapped_text[main_p] == 8211)))))
                            main_h = native_len;
                    }
                    while (main_p++ < for_end);
            }
        }
        if (INTPAR(tracing_lost_chars) > 0) {
            temp_ptr = 0;
            while ((temp_ptr < native_len)) {

                main_k = native_text[temp_ptr];
                temp_ptr++;
                if ((main_k >= 0xD800) && (main_k < 0xDC00)) {
                    main_k = 65536L + (main_k - 0xD800) * 1024;
                    main_k = main_k + native_text[temp_ptr] - 0xDC00;
                    temp_ptr++;
                }
                if (map_char_to_glyph(main_f, main_k) == 0)
                    char_warning(main_f, main_k);
            }
        }
        main_k = native_len;
        main_pp = cur_list.tail;
        if (cur_list.mode == HMODE) {
            main_ppp = cur_list.head;

            while (main_ppp != main_pp && mem[main_ppp].b32.s1 != main_pp) {
                if (!is_char_node(main_ppp) && NODE_type(main_ppp) == DISC_NODE) {
                    temp_ptr = main_ppp;
                    {
                        register int32_t for_end;
                        main_p = 1;
                        for_end = mem[temp_ptr].b16.s0;
                        if (main_p <= for_end)
                            do
                                main_ppp = LLIST_link(main_ppp);
                            while (main_p++ < for_end);
                    }
                }
                if (main_ppp != main_pp)
                    main_ppp = LLIST_link(main_ppp);
            }
            temp_ptr = 0;
            do {
                if (main_h == 0)
                    main_h = main_k;
                if ((((main_pp) != TEX_NULL && (!(is_char_node(main_pp)))
                      && (NODE_type(main_pp) == WHATSIT_NODE)
                      && ((mem[main_pp].b16.s0 == NATIVE_WORD_NODE)
                          || (mem[main_pp].b16.s0 == NATIVE_WORD_NODE_AT))))
                    && (mem[main_pp + 4].b16.s2 == main_f) && (main_ppp != main_pp) && (!(is_char_node(main_ppp)))
                    && (NODE_type(main_ppp) != DISC_NODE)) {
                    main_k = main_h + mem[main_pp + 4].b16.s1;
                    while (native_text_size <= native_len + main_k) {

                        native_text_size = native_text_size + 128;
                        native_text = xrealloc(native_text, native_text_size * sizeof(UTF16_code));
                    }
                    save_native_len = native_len;
                    {
                        register int32_t for_end;
                        main_p = 0;
                        for_end = mem[main_pp + 4].b16.s1 - 1;
                        if (main_p <= for_end)
                            do {
                                native_text[native_len] = NATIVE_NODE_text(main_pp)[main_p];
                                native_len++;
                            }
                            while (main_p++ < for_end);
                    }
                    {
                        register int32_t for_end;
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
                           && ((!(INTPAR(xetex_dash_break) > 0))
                               || ((native_text[temp_ptr + main_h] != 8212)
                                   && (native_text[temp_ptr + main_h] != 8211))))
                        main_h++;
                    if ((main_h < main_k))
                        main_h++;
                    mem[main_ppp].b32.s1 = mem[main_pp].b32.s1;
                    mem[main_pp].b32.s1 = TEX_NULL;
                    flush_node_list(main_pp);
                    main_pp = cur_list.tail;
                    while ((mem[main_ppp].b32.s1 != main_pp))
                        main_ppp = LLIST_link(main_ppp);
                } else {

                    do_locale_linebreaks(temp_ptr, main_h);
                    temp_ptr = temp_ptr + main_h;
                    main_k = main_k - main_h;
                    main_h = 0;
                    while ((main_h < main_k) && (native_text[temp_ptr + main_h] != hyphen_char[main_f])
                           && ((!(INTPAR(xetex_dash_break) > 0))
                               || ((native_text[temp_ptr + main_h] != 8212)
                                   && (native_text[temp_ptr + main_h] != 8211))))
                        main_h++;
                    if ((main_h < main_k))
                        main_h++;
                }
                if ((main_k > 0) || is_hyph) {
                    {
                        mem[cur_list.tail].b32.s1 = new_disc();
                        cur_list.tail = LLIST_link(cur_list.tail);
                    }
                    main_pp = cur_list.tail;
                }
            } while (!(main_k == 0));
        } else {

            main_ppp = cur_list.head;
            while (main_ppp != main_pp && mem[main_ppp].b32.s1 != main_pp) {
                if (!is_char_node(main_ppp) && NODE_type(main_ppp) == DISC_NODE) {
                    temp_ptr = main_ppp;
                    {
                        register int32_t for_end;
                        main_p = 1;
                        for_end = mem[temp_ptr].b16.s0;
                        if (main_p <= for_end)
                            do
                                main_ppp = LLIST_link(main_ppp);
                            while (main_p++ < for_end);
                    }
                }
                if (main_ppp != main_pp)
                    main_ppp = LLIST_link(main_ppp);
            }
            if ((((main_pp) != TEX_NULL && (!(is_char_node(main_pp))) && (NODE_type(main_pp) == WHATSIT_NODE)
                  && ((mem[main_pp].b16.s0 == NATIVE_WORD_NODE)
                      || (mem[main_pp].b16.s0 == NATIVE_WORD_NODE_AT)))) && (mem[main_pp + 4].b16.s2 == main_f)
                && (main_ppp != main_pp) && (!(is_char_node(main_ppp))) && (NODE_type(main_ppp) != DISC_NODE)) {
                mem[main_pp].b32.s1 = new_native_word_node(main_f, main_k + mem[main_pp + 4].b16.s1);
                cur_list.tail = mem[main_pp].b32.s1;
                {
                    register int32_t for_end;
                    main_p = 0;
                    for_end = mem[main_pp + 4].b16.s1 - 1;
                    if (main_p <= for_end)
                        do
                            NATIVE_NODE_text(cur_list.tail)[main_p] = NATIVE_NODE_text(main_pp)[main_p];
                        while (main_p++ < for_end);
                }
                {
                    register int32_t for_end;
                    main_p = 0;
                    for_end = main_k - 1;
                    if (main_p <= for_end)
                        do
                            NATIVE_NODE_text(cur_list.tail)[main_p + mem[main_pp + 4].b16.s1] = native_text[main_p];
                        while (main_p++ < for_end);
                }
                set_native_metrics(cur_list.tail, (INTPAR(xetex_use_glyph_metrics) > 0));
                main_p = cur_list.head;
                if (main_p != main_pp)
                    while (mem[main_p].b32.s1 != main_pp)
                        main_p = LLIST_link(main_p);
                mem[main_p].b32.s1 = mem[main_pp].b32.s1;
                mem[main_pp].b32.s1 = TEX_NULL;
                flush_node_list(main_pp);
            } else {

                mem[main_pp].b32.s1 = new_native_word_node(main_f, main_k);
                cur_list.tail = mem[main_pp].b32.s1;
                {
                    register int32_t for_end;
                    main_p = 0;
                    for_end = main_k - 1;
                    if (main_p <= for_end)
                        do
                            NATIVE_NODE_text(cur_list.tail)[main_p] =  native_text[main_p];
                        while (main_p++ < for_end);
                }
                set_native_metrics(cur_list.tail, (INTPAR(xetex_use_glyph_metrics) > 0));
            }
        }
        if (INTPAR(xetex_interword_space_shaping) > 0) {
            main_p = cur_list.head;
            main_pp = TEX_NULL;
            while (main_p != cur_list.tail) {

                if ((((main_p) != TEX_NULL && (!(is_char_node(main_p)))
                      && (NODE_type(main_p) == WHATSIT_NODE)
                      && ((mem[main_p].b16.s0 == NATIVE_WORD_NODE)
                          || (mem[main_p].b16.s0 == NATIVE_WORD_NODE_AT)))))
                    main_pp = main_p;
                main_p = LLIST_link(main_p);
            }
            if ((main_pp != TEX_NULL)) {
                if (mem[main_pp + 4].b16.s2 == main_f) {
                    main_p = mem[main_pp].b32.s1;
                    while (!(is_char_node(main_p))
                           && ((NODE_type(main_p) == PENALTY_NODE) || (NODE_type(main_p) == INS_NODE)
                               || (NODE_type(main_p) == MARK_NODE) || (NODE_type(main_p) == ADJUST_NODE)
                               || ((NODE_type(main_p) == WHATSIT_NODE) && (mem[main_p].b16.s0 <= 4))))
                        main_p = LLIST_link(main_p);
                    if (!(is_char_node(main_p)) && (NODE_type(main_p) == GLUE_NODE)) {
                        main_ppp = mem[main_p].b32.s1;
                        while (!(is_char_node(main_ppp))
                               && ((NODE_type(main_ppp) == PENALTY_NODE)
                                   || (NODE_type(main_ppp) == INS_NODE)
                                   || (NODE_type(main_ppp) == MARK_NODE)
                                   || (NODE_type(main_ppp) == ADJUST_NODE)
                                   || ((NODE_type(main_ppp) == WHATSIT_NODE) && (mem[main_ppp].b16.s0 <= 4))))
                            main_ppp = LLIST_link(main_ppp);
                        if (main_ppp == cur_list.tail) {
                            temp_ptr =
                                new_native_word_node(main_f,
                                                     mem[main_pp + 4].b16.s1 + 1 + mem[cur_list.tail +
                                                                                        4].b16.s1);
                            main_k = 0;
                            {
                                register int32_t for_end;
                                t = 0;
                                for_end = mem[main_pp + 4].b16.s1 - 1;
                                if (t <= for_end)
                                    do {
                                        NATIVE_NODE_text(temp_ptr)[main_k] = NATIVE_NODE_text(main_pp)[t];
                                        main_k++;
                                    }
                                    while (t++ < for_end);
                            }
                            NATIVE_NODE_text(temp_ptr)[main_k] = ' ';
                            main_k++;
                            {
                                register int32_t for_end;
                                t = 0;
                                for_end = mem[cur_list.tail + 4].b16.s1 - 1;
                                if (t <= for_end)
                                    do {
                                        NATIVE_NODE_text(temp_ptr)[main_k] = NATIVE_NODE_text(cur_list.tail)[t];
                                        main_k++;
                                    }
                                    while (t++ < for_end);
                            }
                            set_native_metrics(temp_ptr, (INTPAR(xetex_use_glyph_metrics) > 0));
                            t = mem[temp_ptr + 1].b32.s1 - mem[main_pp + 1].b32.s1 - mem[cur_list.tail + 1].b32.s1;
                            free_node(temp_ptr, mem[temp_ptr + 4].b16.s3);
                            if (t != mem[font_glue[main_f] + 1].b32.s1) {
                                temp_ptr = new_kern(t - mem[font_glue[main_f] + 1].b32.s1);
                                NODE_subtype(temp_ptr) = SPACE_ADJUSTMENT;
                                mem[temp_ptr].b32.s1 = mem[main_p].b32.s1;
                                mem[main_p].b32.s1 = temp_ptr;
                            }
                        }
                    }
                }
            }
        }
        if (cur_ptr != TEX_NULL)
            goto big_switch;
        else
            goto reswitch;
    }
    main_s = SF_CODE(cur_chr) % 65536L;
    if (main_s == 1000)
        cur_list.aux.b32.s0 = 1000;
    else if (main_s < 1000) {
        if (main_s > 0)
            cur_list.aux.b32.s0 = main_s;
    } else if (cur_list.aux.b32.s0 < 1000)
        cur_list.aux.b32.s0 = 1000;
    else
        cur_list.aux.b32.s0 = main_s;
    cur_ptr = TEX_NULL;
    space_class = SF_CODE(cur_chr) / 65536L;
    if ((INTPAR(xetex_inter_char_tokens) > 0) && space_class != CHAR_CLASS_LIMIT) {
        if (prev_class == ((CHAR_CLASS_LIMIT - 1))) {
            if ((cur_input.state != TOKEN_LIST) || (cur_input.index != BACKED_UP_CHAR)) {
                find_sa_element(INTER_CHAR_VAL,
                                ((CHAR_CLASS_LIMIT - 1)) * CHAR_CLASS_LIMIT + space_class, false);
                if (cur_ptr != TEX_NULL && ETEX_SA_ptr(cur_ptr) != TEX_NULL) {
                    if (cur_cmd != LETTER)
                        cur_cmd = OTHER_CHAR;
                    cur_tok = (cur_cmd * MAX_CHAR_VAL) + cur_chr;
                    back_input();
                    cur_input.index = BACKED_UP_CHAR;
                    begin_token_list(mem[cur_ptr + 1].b32.s1, INTER_CHAR_TEXT);
                    goto big_switch;
                }
            }
        } else {

            find_sa_element(INTER_CHAR_VAL, prev_class * CHAR_CLASS_LIMIT + space_class, false);
            if (cur_ptr != TEX_NULL && ETEX_SA_ptr(cur_ptr) != TEX_NULL) {
                if (cur_cmd != LETTER)
                    cur_cmd = OTHER_CHAR;
                cur_tok = (cur_cmd * MAX_CHAR_VAL) + cur_chr;
                back_input();
                cur_input.index = BACKED_UP_CHAR;
                begin_token_list(mem[cur_ptr + 1].b32.s1, INTER_CHAR_TEXT);
                prev_class = ((CHAR_CLASS_LIMIT - 1));
                goto big_switch;
            }
        }
        prev_class = space_class;
    }
    main_f = eqtb[CUR_FONT_LOC].b32.s1;
    bchar = font_bchar[main_f];
    false_bchar = font_false_bchar[main_f];
    if (cur_list.mode > 0) {

        if (INTPAR(language) != cur_list.aux.b32.s1)
            fix_language();
    }
    {
        lig_stack = avail;
        if (lig_stack == TEX_NULL)
            lig_stack = get_avail();
        else {

            avail = mem[lig_stack].b32.s1;
            mem[lig_stack].b32.s1 = TEX_NULL;
        }
    }
    mem[lig_stack].b16.s1 = main_f;
    cur_l = cur_chr;
    mem[lig_stack].b16.s0 = cur_l;
    cur_q = cur_list.tail;
    if (cancel_boundary) {
        cancel_boundary = false;
        main_k = NON_ADDRESS;
    } else
        main_k = bchar_label[main_f];
    if (main_k == NON_ADDRESS)
        goto lab92;
    cur_r = cur_l;
    cur_l = TOO_BIG_CHAR;
    goto lab111;
 lab80:/*main_loop_wrapup *//*1070: */ if (cur_l < TOO_BIG_CHAR) {
        if (mem[cur_q].b32.s1 > TEX_NULL) {

            if (mem[cur_list.tail].b16.s0 == hyphen_char[main_f])
                ins_disc = true;
        }
        if (ligature_present) {
            main_p = new_ligature(main_f, cur_l, mem[cur_q].b32.s1);
            if (lft_hit) {
                mem[main_p].b16.s0 = 2;
                lft_hit = false;
            }
            if (rt_hit) {

                if (lig_stack == TEX_NULL) {
                    mem[main_p].b16.s0++;
                    rt_hit = false;
                }
            }
            mem[cur_q].b32.s1 = main_p;
            cur_list.tail = main_p;
            ligature_present = false;
        }
        if (ins_disc) {
            ins_disc = false;
            if (cur_list.mode > 0) {
                mem[cur_list.tail].b32.s1 = new_disc();
                cur_list.tail = LLIST_link(cur_list.tail);
            }
        }
    }
 lab90:                        /*main_loop_move *//*1071: */ if (lig_stack == TEX_NULL)
        goto reswitch;
    cur_q = cur_list.tail;
    cur_l = mem[lig_stack].b16.s0;
 lab91:                        /*main_loop_move 1 */ if (!(is_char_node(lig_stack)))
        goto lab95;
 lab92:                        /*main_loop_move 2 */ if ((effective_char(false, main_f, cur_chr) > font_ec[main_f])
                              || (effective_char(false, main_f, cur_chr) < font_bc[main_f])) {
        char_warning(main_f, cur_chr);
        {
            mem[lig_stack].b32.s1 = avail;
            avail = lig_stack;
        }
        goto big_switch;
    }
    main_i = effective_char_info(main_f, cur_l);
    if (!(main_i.s3 > 0)) {
        char_warning(main_f, cur_chr);
        {
            mem[lig_stack].b32.s1 = avail;
            avail = lig_stack;
        }
        goto big_switch;
    }
    mem[cur_list.tail].b32.s1 = lig_stack;
    cur_list.tail = /*:1071 */ lig_stack;
 lab100:                       /*main_loop_lookahead *//*1073: */ get_next();
    if (cur_cmd == LETTER)
        goto lab101;
    if (cur_cmd == OTHER_CHAR)
        goto lab101;
    if (cur_cmd == CHAR_GIVEN)
        goto lab101;
    x_token();
    if (cur_cmd == LETTER)
        goto lab101;
    if (cur_cmd == OTHER_CHAR)
        goto lab101;
    if (cur_cmd == CHAR_GIVEN)
        goto lab101;
    if (cur_cmd == CHAR_NUM) {
        scan_char_num();
        cur_chr = cur_val;
        goto lab101;
    }
    if (cur_cmd == NO_BOUNDARY)
        bchar = TOO_BIG_CHAR;
    cur_r = bchar;
    lig_stack = TEX_NULL;
    goto lab110;
 lab101:/*main_loop_lookahead 1 */ main_s = SF_CODE(cur_chr) % 65536L;
    if (main_s == 1000)
        cur_list.aux.b32.s0 = 1000;
    else if (main_s < 1000) {
        if (main_s > 0)
            cur_list.aux.b32.s0 = main_s;
    } else if (cur_list.aux.b32.s0 < 1000)
        cur_list.aux.b32.s0 = 1000;
    else
        cur_list.aux.b32.s0 = main_s;
    cur_ptr = TEX_NULL;
    space_class = SF_CODE(cur_chr) / 65536L;
    if ((INTPAR(xetex_inter_char_tokens) > 0) && space_class != CHAR_CLASS_LIMIT) {
        if (prev_class == ((CHAR_CLASS_LIMIT - 1))) {
            if ((cur_input.state != TOKEN_LIST) || (cur_input.index != BACKED_UP_CHAR)) {
                find_sa_element(INTER_CHAR_VAL,
                                ((CHAR_CLASS_LIMIT - 1)) * CHAR_CLASS_LIMIT + space_class, false);
                if (cur_ptr != TEX_NULL && ETEX_SA_ptr(cur_ptr) != TEX_NULL) {
                    if (cur_cmd != LETTER)
                        cur_cmd = OTHER_CHAR;
                    cur_tok = (cur_cmd * MAX_CHAR_VAL) + cur_chr;
                    back_input();
                    cur_input.index = BACKED_UP_CHAR;
                    begin_token_list(mem[cur_ptr + 1].b32.s1, INTER_CHAR_TEXT);
                    goto big_switch;
                }
            }
        } else {

            find_sa_element(INTER_CHAR_VAL, prev_class * CHAR_CLASS_LIMIT + space_class, false);
            if (cur_ptr != TEX_NULL && ETEX_SA_ptr(cur_ptr) != TEX_NULL) {
                if (cur_cmd != LETTER)
                    cur_cmd = OTHER_CHAR;
                cur_tok = (cur_cmd * MAX_CHAR_VAL) + cur_chr;
                back_input();
                cur_input.index = BACKED_UP_CHAR;
                begin_token_list(mem[cur_ptr + 1].b32.s1, INTER_CHAR_TEXT);
                prev_class = ((CHAR_CLASS_LIMIT - 1));
                goto big_switch;
            }
        }
        prev_class = space_class;
    }
    {
        lig_stack = avail;
        if (lig_stack == TEX_NULL)
            lig_stack = get_avail();
        else {

            avail = mem[lig_stack].b32.s1;
            mem[lig_stack].b32.s1 = TEX_NULL;
        }
    }
    mem[lig_stack].b16.s1 = main_f;
    cur_r = cur_chr;
    mem[lig_stack].b16.s0 = cur_r;
    if (cur_r == false_bchar)
        cur_r = TOO_BIG_CHAR/*:1073 */ ;
 lab110:/*main_lig_loop *//*1074: */ if (((main_i.s1) % 4) != LIG_TAG)
        goto lab80;
    if (cur_r == TOO_BIG_CHAR)
        goto lab80;
    main_k = lig_kern_base[main_f] + main_i.s0;
    main_j = font_info[main_k].b16;
    if (main_j.s3 <= 128)
        goto lab112;
    main_k = lig_kern_base[main_f] + 256 * main_j.s1 + main_j.s0 + 32768L - 256 * (128);
 lab111:                       /*main_lig_loop 1 */ main_j = font_info[main_k].b16;
 lab112:                       /*main_lig_loop 2 */ if (main_j.s2 == cur_r) {

        if (main_j.s3 <= 128) { /*1075: */
            if (main_j.s1 >= 128) {
                if (cur_l < TOO_BIG_CHAR) {
                    if (mem[cur_q].b32.s1 > TEX_NULL) {

                        if (mem[cur_list.tail].b16.s0 == hyphen_char[main_f])
                            ins_disc = true;
                    }
                    if (ligature_present) {
                        main_p = new_ligature(main_f, cur_l, mem[cur_q].b32.s1);
                        if (lft_hit) {
                            mem[main_p].b16.s0 = 2;
                            lft_hit = false;
                        }
                        if (rt_hit) {

                            if (lig_stack == TEX_NULL) {
                                mem[main_p].b16.s0++;
                                rt_hit = false;
                            }
                        }
                        mem[cur_q].b32.s1 = main_p;
                        cur_list.tail = main_p;
                        ligature_present = false;
                    }
                    if (ins_disc) {
                        ins_disc = false;
                        if (cur_list.mode > 0) {
                            mem[cur_list.tail].b32.s1 = new_disc();
                            cur_list.tail = LLIST_link(cur_list.tail);
                        }
                    }
                }
                {
                    mem[cur_list.tail].b32.s1 =
                        new_kern(font_info[kern_base[main_f] + 256 * main_j.s1 + main_j.s0].b32.s1);
                    cur_list.tail = LLIST_link(cur_list.tail);
                }
                goto lab90;
            }
            if (cur_l == TOO_BIG_CHAR)
                lft_hit = true;
            else if (lig_stack == TEX_NULL)
                rt_hit = true;
            switch (main_j.s1) {
            case 1:
            case 5:
                {
                    cur_l = main_j.s0;
                    main_i = FONT_CHARACTER_INFO(main_f, effective_char(true, main_f, cur_l));
                    ligature_present = true;
                }
                break;
            case 2:
            case 6:
                {
                    cur_r = main_j.s0;
                    if (lig_stack == TEX_NULL) {
                        lig_stack = new_lig_item(cur_r);
                        bchar = TOO_BIG_CHAR;
                    } else if ((is_char_node(lig_stack))) {
                        main_p = lig_stack;
                        lig_stack = new_lig_item(cur_r);
                        mem[lig_stack + 1].b32.s1 = main_p;
                    } else
                        mem[lig_stack].b16.s0 = cur_r;
                }
                break;
            case 3:
                {
                    cur_r = main_j.s0;
                    main_p = lig_stack;
                    lig_stack = new_lig_item(cur_r);
                    mem[lig_stack].b32.s1 = main_p;
                }
                break;
            case 7:
            case 11:
                {
                    if (cur_l < TOO_BIG_CHAR) {
                        if (mem[cur_q].b32.s1 > TEX_NULL) {

                            if (mem[cur_list.tail].b16.s0 == hyphen_char[main_f])
                                ins_disc = true;
                        }
                        if (ligature_present) {
                            main_p = new_ligature(main_f, cur_l, mem[cur_q].b32.s1);
                            if (lft_hit) {
                                mem[main_p].b16.s0 = 2;
                                lft_hit = false;
                            }
                            if (false) {

                                if (lig_stack == TEX_NULL) {
                                    mem[main_p].b16.s0++;
                                    rt_hit = false;
                                }
                            }
                            mem[cur_q].b32.s1 = main_p;
                            cur_list.tail = main_p;
                            ligature_present = false;
                        }
                        if (ins_disc) {
                            ins_disc = false;
                            if (cur_list.mode > 0) {
                                mem[cur_list.tail].b32.s1 = new_disc();
                                cur_list.tail = LLIST_link(cur_list.tail);
                            }
                        }
                    }
                    cur_q = cur_list.tail;
                    cur_l = main_j.s0;
                    main_i = FONT_CHARACTER_INFO(main_f, effective_char(true, main_f, cur_l));
                    ligature_present = true;
                }
                break;
            default:
                {
                    cur_l = main_j.s0;
                    ligature_present = true;
                    if (lig_stack == TEX_NULL)
                        goto lab80;
                    else
                        goto lab91;
                }
                break;
            }
            if (main_j.s1 > 4) {

                if (main_j.s1 != 7)
                    goto lab80;
            }
            if (cur_l < TOO_BIG_CHAR)
                goto lab110;
            main_k = bchar_label[main_f];
            goto lab111;
        }
    }
    if (main_j.s3 == 0)
        main_k++;
    else {

        if (main_j.s3 >= 128)
            goto lab80;
        main_k = main_k + main_j.s3 + 1;
    }
    goto lab111;
 lab95:                        /*main_loop_move_lig *//*1072: */ main_p = mem[lig_stack + 1].b32.s1;
    if (main_p > TEX_NULL) {
        mem[cur_list.tail].b32.s1 = main_p;
        cur_list.tail = LLIST_link(cur_list.tail);
    }
    temp_ptr = lig_stack;
    lig_stack = mem[temp_ptr].b32.s1;
    free_node(temp_ptr, SMALL_NODE_SIZE);
    main_i = FONT_CHARACTER_INFO(main_f, effective_char(true, main_f, cur_l));
    ligature_present = true;
    if (lig_stack == TEX_NULL) {

        if (main_p > TEX_NULL)
            goto lab100;
        else
            cur_r = bchar;
    } else
        cur_r = mem[lig_stack].b16.s0;
    goto lab110;
 lab120:/*append_normal_space */ if ((INTPAR(xetex_inter_char_tokens) > 0)
                                 && (space_class != CHAR_CLASS_LIMIT)
                                 && (prev_class != ((CHAR_CLASS_LIMIT - 1)))) {
        prev_class = ((CHAR_CLASS_LIMIT - 1));
        find_sa_element(INTER_CHAR_VAL,
                        space_class * CHAR_CLASS_LIMIT + ((CHAR_CLASS_LIMIT - 1)), false);
        if (cur_ptr != TEX_NULL && ETEX_SA_ptr(cur_ptr) != TEX_NULL) {
            if (cur_cs == 0) {
                if (cur_cmd == CHAR_NUM)
                    cur_cmd = OTHER_CHAR;
                cur_tok = (cur_cmd * MAX_CHAR_VAL) + cur_chr;
            } else
                cur_tok = CS_TOKEN_FLAG + cur_cs;
            back_input();
            begin_token_list(mem[cur_ptr + 1].b32.s1, INTER_CHAR_TEXT);
            goto big_switch;
        }
    }
    if (GLUEPAR(space_skip) == 0) {
        {
            main_p = font_glue[eqtb[CUR_FONT_LOC].b32.s1];
            if (main_p == TEX_NULL) {
                main_p = new_spec(0);
                main_k = param_base[eqtb[CUR_FONT_LOC].b32.s1] + 2;
                mem[main_p + 1].b32.s1 = font_info[main_k].b32.s1;
                mem[main_p + 2].b32.s1 = font_info[main_k + 1].b32.s1;
                mem[main_p + 3].b32.s1 = font_info[main_k + 2].b32.s1;
                font_glue[eqtb[CUR_FONT_LOC].b32.s1] = main_p;
            }
        }
        temp_ptr = new_glue(main_p);
    } else
        temp_ptr = new_param_glue(GLUE_PAR__space_skip);
    mem[cur_list.tail].b32.s1 = temp_ptr;
    cur_list.tail = temp_ptr;
    goto big_switch;
}

void give_err_help(void)
{
    token_show(LOCAL(err_help));
}


void
close_files_and_terminate(void)
{
    int32_t k;

    terminate_font_manager();

    for (k = 0; k <= 15; k++)
        if (write_open[k])
            ttstub_output_close (write_file[k]);

    finalize_dvi_file();
    synctex_terminate(log_opened);

    if (log_opened) {
        ttstub_output_putc (log_file, '\n');
        ttstub_output_close (log_file);
        selector = selector - 2;
        if (selector == SELECTOR_TERM_ONLY) {
            print_nl_cstr("Transcript written on ");
            print(texmf_log_name);
            print_char('.');
        }
    }

    print_ln();
}


void flush_str(str_number s)
{
    if (s == str_ptr - 1) {
        str_ptr--;
        pool_ptr = str_start[str_ptr - TOO_BIG_CHAR];
    }
}

str_number tokens_to_string(int32_t p)
{
    if (selector == SELECTOR_NEW_STRING )
        pdf_error("tokens", "tokens_to_string() called while selector = new_string");
    old_setting = selector;
    selector = SELECTOR_NEW_STRING ;
    show_token_list(mem[p].b32.s1, TEX_NULL, pool_size - pool_ptr);
    selector = old_setting;
    return make_string();
}

void scan_pdf_ext_toks(void)
{
    scan_toks(false, true);
}

void compare_strings(void)
{
    str_number s1, s2;
    pool_pointer i1, i2, j1, j2;
    int32_t save_cur_cs;

    save_cur_cs = cur_cs;
    {
        scan_toks(false, true);
    }
    s1 = tokens_to_string(def_ref);
    delete_token_ref(def_ref);
    cur_cs = save_cur_cs;
    {
        scan_toks(false, true);
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
            goto done;
        }
        if (str_pool[i1] > str_pool[i2]) {
            cur_val = 1;
            goto done;
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
done:
    flush_str(s2);
    flush_str(s1);
    cur_val_level = INT_VAL;
}


int32_t
prune_page_top(int32_t p, bool s)
{
    int32_t prev_p;
    int32_t q, r = TEX_NULL;

    prev_p = TEMP_HEAD;
    mem[TEMP_HEAD].b32.s1 = p;

    while (p != TEX_NULL) {
        switch (mem[p].b16.s1) {
        case HLIST_NODE:
        case VLIST_NODE:
        case RULE_NODE:
            q = new_skip_param(GLUE_PAR__split_top_skip);
            mem[prev_p].b32.s1 = q;
            mem[q].b32.s1 = p;
            if (mem[temp_ptr + 1].b32.s1 > mem[p + 3].b32.s1)
                mem[temp_ptr + 1].b32.s1 = mem[temp_ptr + 1].b32.s1 - mem[p + 3].b32.s1;
            else
                mem[temp_ptr + 1].b32.s1 = 0;
            p = TEX_NULL;
            break;
        case WHATSIT_NODE:
        case MARK_NODE:
        case INS_NODE:
            prev_p = p;
            p = mem[prev_p].b32.s1;
            break;
        case GLUE_NODE:
        case KERN_NODE:
        case PENALTY_NODE:
            q = p;
            p = mem[q].b32.s1;
            mem[q].b32.s1 = TEX_NULL;
            mem[prev_p].b32.s1 = p;
            if (s) {
                if (disc_ptr[VSPLIT_CODE] == TEX_NULL)
                    disc_ptr[VSPLIT_CODE] = q;
                else
                    mem[r].b32.s1 = q;
                r = q;
            } else {
                flush_node_list(q);
            }
            break;
        default:
            confusion("pruning");
            break;
        }
    }

    return mem[TEMP_HEAD].b32.s1;
}


bool
do_marks(small_number a, small_number l, int32_t q)
{
    small_number i;

    if (l < 4) {
        for (i = 0; i <= 15; i++) {
            if (odd(i))
                cur_ptr = mem[q + (i / 2) + 1].b32.s1;
            else
                cur_ptr = mem[q + (i / 2) + 1].b32.s0;

            if (cur_ptr != TEX_NULL) {
                if (do_marks(a, l + 1, cur_ptr)) {
                    if (odd(i))
                        mem[q + (i / 2) + 1].b32.s1 = TEX_NULL;
                    else
                        mem[q + (i / 2) + 1].b32.s0 = TEX_NULL;
                    mem[q].b16.s0--;
                }
            }
        }

        if (mem[q].b16.s0 == 0) {
            free_node(q, INDEX_NODE_SIZE);
            q = TEX_NULL;
        }
    } else {
        switch (a) { /*1614: */
        case VSPLIT_INIT:
            if (mem[q + 2].b32.s1 != TEX_NULL) {
                delete_token_ref(mem[q + 2].b32.s1);
                mem[q + 2].b32.s1 = TEX_NULL;
                delete_token_ref(mem[q + 3].b32.s0);
                mem[q + 3].b32.s0 = TEX_NULL;
            }
            break;

        case FIRE_UP_INIT:
            if (mem[q + 2].b32.s0 != TEX_NULL) {
                if (mem[q + 1].b32.s0 != TEX_NULL)
                    delete_token_ref(mem[q + 1].b32.s0);
                delete_token_ref(mem[q + 1].b32.s1);
                mem[q + 1].b32.s1 = TEX_NULL;
                if (mem[mem[q + 2].b32.s0].b32.s1 == TEX_NULL) {
                    delete_token_ref(mem[q + 2].b32.s0);
                    mem[q + 2].b32.s0 = TEX_NULL;
                } else
                    mem[mem[q + 2].b32.s0].b32.s0++;
                mem[q + 1].b32.s0 = mem[q + 2].b32.s0;
            }
            break;

        case FIRE_UP_DONE:
            if ((mem[q + 1].b32.s0 != TEX_NULL) && (mem[q + 1].b32.s1 == TEX_NULL)) {
                mem[q + 1].b32.s1 = mem[q + 1].b32.s0;
                mem[mem[q + 1].b32.s0].b32.s0++;
            }
            break;

        case DESTROY_MARKS:
            for (i = TOP_MARK_CODE; i <= SPLIT_BOT_MARK_CODE; i++) {
                if (odd(i))
                    cur_ptr = mem[q + (i / 2) + 1].b32.s1;
                else
                    cur_ptr = mem[q + (i / 2) + 1].b32.s0;

                if (cur_ptr != TEX_NULL) {
                    delete_token_ref(cur_ptr);
                    if (odd(i))
                        mem[q + (i / 2) + 1].b32.s1 = TEX_NULL;
                    else
                        mem[q + (i / 2) + 1].b32.s0 = TEX_NULL;
                }
            }
            break;
        }

        if (mem[q + 2].b32.s0 == TEX_NULL) {
            if (mem[q + 3].b32.s0 == TEX_NULL) {
                free_node(q, MARK_CLASS_NODE_SIZE);
                q = TEX_NULL;
            }
        }
    }

    return (q == TEX_NULL);
}


void
do_assignments(void)
{
    while (true) {
        do {
            get_x_token();
        } while (cur_cmd == SPACER || cur_cmd == RELAX);

        if (cur_cmd <= MAX_NON_PREFIXED_COMMAND)
            return;

        set_box_allowed = false;
        prefixed_command();
        set_box_allowed = true;
    }
}


void
new_whatsit(small_number s, small_number w)
{
    int32_t p;

    p = get_node(w);
    NODE_type(p) = WHATSIT_NODE;
    mem[p].b16.s0 = s;
    mem[cur_list.tail].b32.s1 = p;
    cur_list.tail = p;
}

/* ini.c: WEB initialization code translated to C
   Copyright 2016-2018 The Tectonic Project
   Licensed under the MIT License.
*/

#include "core.h"
#include "xetex.h"
#include "synctex.h"
#include <xdvipdfmx/pdfobj.h> /* pdf_files_{init,close} */
#include <xetex_bindings.h> /* FORMAT_SERIAL */

/* All the following variables are declared in xetex.h */
bool shell_escape_enabled = false;
memory_word *eqtb;
int32_t bad;
char *name_of_file;
UTF16_code *name_of_file16;
int32_t name_length;
int32_t name_length16;
UnicodeScalar *buffer;
int32_t first;
int32_t last;
int32_t max_buf_stack;
bool in_initex_mode;
int32_t error_line;
int32_t half_error_line;
int32_t max_print_line;
int32_t max_strings;
int32_t strings_free;
int32_t string_vacancies;
int32_t pool_size;
int32_t pool_free;
int32_t font_mem_size;
int32_t font_max;
int32_t hyph_size;
int32_t trie_size;
int32_t buf_size;
int32_t stack_size;
int32_t max_in_open;
int32_t param_size;
int32_t nest_size;
int32_t save_size;
int32_t expand_depth;
int file_line_error_style_p;
int halt_on_error_p;
bool quoted_filename;
bool insert_src_special_auto;
bool insert_src_special_every_par;
bool insert_src_special_every_math;
bool insert_src_special_every_vbox;
packed_UTF16_code *str_pool;
pool_pointer *str_start;
pool_pointer pool_ptr;
str_number str_ptr;
pool_pointer init_pool_ptr;
str_number init_str_ptr;
rust_output_handle_t rust_stdout;
rust_output_handle_t log_file;
selector_t selector;
unsigned char dig[23];
int32_t tally;
int32_t term_offset;
int32_t file_offset;
UTF16_code trick_buf[256];
int32_t trick_count;
int32_t first_count;
bool doing_special;
UTF16_code *native_text;
int32_t native_text_size;
int32_t native_len;
int32_t save_native_len;
unsigned char interaction;
bool deletions_allowed;
bool set_box_allowed;
tt_history_t history;
signed char error_count;
const char* help_line[6];
unsigned char help_ptr;
bool use_err_help;
bool arith_error;
scaled_t tex_remainder;
int32_t randoms[55];
unsigned char j_random;
scaled_t random_seed;
int32_t two_to_the[31];
int32_t spec_log[29];
int32_t temp_ptr;
memory_word *mem;
int32_t lo_mem_max;
int32_t hi_mem_min;
int32_t var_used, dyn_used;
int32_t avail;
int32_t mem_end;
int32_t rover;
int32_t last_leftmost_char;
int32_t last_rightmost_char;
int32_t hlist_stack[513];
short hlist_stack_level;
int32_t first_p;
int32_t global_prev_p;
int32_t font_in_short_display;
int32_t depth_threshold;
int32_t breadth_max;
list_state_record *nest;
int32_t nest_ptr;
int32_t max_nest_stack;
list_state_record cur_list;
short shown_mode;
unsigned char old_setting;
b32x2 *hash;
int32_t hash_used;
int32_t hash_extra;
int32_t hash_top;
int32_t eqtb_top;
int32_t hash_high;
bool no_new_control_sequence;
int32_t cs_count;
b32x2 prim[501];
int32_t prim_used;
memory_word *save_stack;
int32_t save_ptr;
int32_t max_save_stack;
uint16_t cur_level;
group_code cur_group;
int32_t cur_boundary;
int32_t mag_set;
eight_bits cur_cmd;
int32_t cur_chr;
int32_t cur_cs;
int32_t cur_tok;
input_state_t *input_stack;
int32_t input_ptr;
int32_t max_in_stack;
input_state_t cur_input;
int32_t in_open;
int32_t open_parens;
UFILE **input_file;
int32_t line;
int32_t *line_stack;
str_number *source_filename_stack;
str_number *full_source_filename_stack;
unsigned char scanner_status;
int32_t warning_index;
int32_t def_ref;
int32_t *param_stack;
int32_t param_ptr;
int32_t max_param_stack;
int32_t align_state;
int32_t base_ptr;
int32_t par_loc;
int32_t par_token;
bool force_eof;
int32_t expand_depth_count;
bool is_in_csname;
int32_t cur_mark[5];
unsigned char long_state;
int32_t pstack[9];
int32_t cur_val;
int32_t cur_val1;
unsigned char cur_val_level;
small_number radix;
glue_ord cur_order;
UFILE *read_file[16];
unsigned char read_open[17];
int32_t cond_ptr;
unsigned char if_limit;
small_number cur_if;
int32_t if_line;
int32_t skip_line;
str_number cur_name;
str_number cur_area;
str_number cur_ext;
pool_pointer area_delimiter;
pool_pointer ext_delimiter;
UTF16_code file_name_quote_char;
int32_t format_default_length;
char *TEX_format_default;
bool name_in_progress;
str_number job_name;
bool log_opened;
const char* output_file_extension;
str_number texmf_log_name;
memory_word *font_info;
font_index fmem_ptr;
internal_font_number font_ptr;
b16x4 *font_check;
scaled_t *font_size;
scaled_t *font_dsize;
font_index *font_params;
str_number *font_name;
str_number *font_area;
UTF16_code *font_bc;
UTF16_code *font_ec;
int32_t *font_glue;
bool *font_used;
int32_t *hyphen_char;
int32_t *skew_char;
font_index *bchar_label;
nine_bits *font_bchar;
nine_bits *font_false_bchar;
void **font_layout_engine;
void **font_mapping;
char *font_flags;
scaled_t *font_letter_space;
void *loaded_font_mapping;
char loaded_font_flags;
scaled_t loaded_font_letter_space;
UTF16_code *mapped_text;
char *xdv_buffer;
int32_t *char_base;
int32_t *width_base;
int32_t *height_base;
int32_t *depth_base;
int32_t *italic_base;
int32_t *lig_kern_base;
int32_t *kern_base;
int32_t *exten_base;
int32_t *param_base;
b16x4 null_character;
int32_t total_pages;
scaled_t max_v;
scaled_t max_h;
int32_t max_push;
int32_t last_bop;
int32_t dead_cycles;
bool doing_leaders;
scaled_t rule_ht, rule_dp, rule_wd;
scaled_t cur_h, cur_v;
int32_t epochseconds;
int32_t microseconds;
scaled_t total_stretch[4], total_shrink[4];
int32_t last_badness;
int32_t adjust_tail;
int32_t pre_adjust_tail;
int32_t pack_begin_line;
b32x2 empty;
internal_font_number cur_f;
int32_t cur_c;
b16x4 cur_i;
int32_t cur_align;
int32_t cur_span;
int32_t cur_loop;
int32_t align_ptr;
int32_t cur_head, cur_tail;
int32_t cur_pre_head, cur_pre_tail;
int32_t just_box;
scaled_t active_width[7];
int32_t hc[4099];
internal_font_number hf;
int32_t hu[4097];
unsigned char cur_lang;
int32_t max_hyph_char;
unsigned char hyf[4097];
int32_t init_list;
bool init_lig;
bool init_lft;
small_number hyphen_passed;
int32_t cur_l, cur_r;
int32_t cur_q;
int32_t lig_stack;
bool ligature_present;
bool lft_hit, rt_hit;
trie_pointer *trie_trl;
trie_pointer *trie_tro;
uint16_t *trie_trc;
small_number hyf_distance[TRIE_OP_SIZE + 1];
small_number hyf_num[TRIE_OP_SIZE + 1];
trie_opcode hyf_next[TRIE_OP_SIZE + 1];
int32_t op_start[256];
str_number *hyph_word;
int32_t *hyph_list;
hyph_pointer *hyph_link;
int32_t hyph_count;
int32_t hyph_next;
trie_opcode trie_used[256];
unsigned char trie_op_lang[TRIE_OP_SIZE + 1];
trie_opcode trie_op_val[TRIE_OP_SIZE + 1];
int32_t trie_op_ptr;
trie_opcode max_op_used;
packed_UTF16_code *trie_c;
trie_opcode *trie_o;
trie_pointer *trie_l;
trie_pointer *trie_r;
trie_pointer trie_ptr;
trie_pointer *trie_hash;
bool *trie_taken;
trie_pointer trie_min[65536];
trie_pointer trie_max;
bool trie_not_ready;
scaled_t best_height_plus_depth;
internal_font_number main_f;
b16x4 main_i;
b16x4 main_j;
font_index main_k;
int32_t main_p;
int32_t main_pp, main_ppp;
int32_t main_h;
bool is_hyph;
int32_t space_class;
int32_t prev_class;
int32_t main_s;
int32_t bchar;
int32_t false_bchar;
bool cancel_boundary;
bool ins_disc;
int32_t cur_box;
int32_t after_token;
bool long_help_seen;
str_number format_ident;
rust_output_handle_t write_file[16];
bool write_open[18];
int32_t write_loc;
scaled_t cur_page_width;
scaled_t cur_page_height;
scaled_t cur_h_offset;
scaled_t cur_v_offset;
int32_t pdf_last_x_pos;
int32_t pdf_last_y_pos;
bool *eof_seen;
int32_t LR_ptr;
int32_t LR_problems;
small_number cur_dir;
int32_t pseudo_files;
save_pointer *grp_stack;
int32_t *if_stack;
int32_t max_reg_num;
const char* max_reg_help_line;
int32_t sa_root[8];
int32_t cur_ptr;
memory_word sa_null;
int32_t sa_chain;
uint16_t sa_level;
trie_pointer hyph_start;
trie_pointer hyph_index;
int32_t disc_ptr[4];
pool_pointer edit_name_start;
bool stop_at_space;
int32_t native_font_type_flag;
bool xtx_ligature_present;
scaled_t delta;
int synctex_enabled;
bool used_tectonic_coda_tokens;
bool semantic_pagination_enabled;
bool gave_char_warning_help;

/* These ought to live in pagebuilder.c but are shared a lot: */
int32_t page_tail;
unsigned char page_contents;
scaled_t page_so_far[8];
int32_t last_glue;
int32_t last_penalty;
scaled_t last_kern;
int32_t last_node_type;
int32_t insert_penalties;
bool output_active;

uint16_t _xeq_level_array[EQTB_SIZE - INT_BASE + 1];

#define NEG_TRIE_OP_SIZE -35111L
#define MAX_TRIE_OP 65535L

static int32_t _trie_op_hash_array[TRIE_OP_SIZE - NEG_TRIE_OP_SIZE + 1];
#define TRIE_OP_HASH(i) _trie_op_hash_array[(i) - NEG_TRIE_OP_SIZE]

static b32x2 *yhash;

#define FORMAT_HEADER_MAGIC 0x54544E43 /* "TTNC" in ASCII */
#define FORMAT_FOOTER_MAGIC 0x0000029A

/* Read and write dump files.  As distributed, these files are
   architecture dependent; specifically, BigEndian and LittleEndian
   architectures produce different files.  These routines always output
   BigEndian files.  This still does not guarantee them to be
   architecture-independent, because it is possible to make a format
   that dumps a glue ratio, i.e., a floating-point number.  Fortunately,
   none of the standard formats do that.  */

#if !defined (WORDS_BIGENDIAN)

/* This macro is always invoked as a statement.  It assumes a variable
   `temp'.  */

#define SWAP(x, y) temp = (x); (x) = (y); (y) = temp


/* Make the NITEMS items pointed at by P, each of size SIZE, be the
   opposite-endianness of whatever they are now.  */

static void
swap_items (char *p, size_t nitems, size_t size)
{
    char temp;

    switch (size) {
    case 16:
        while (nitems--) {
            SWAP (p[0], p[15]);
            SWAP (p[1], p[14]);
            SWAP (p[2], p[13]);
            SWAP (p[3], p[12]);
            SWAP (p[4], p[11]);
            SWAP (p[5], p[10]);
            SWAP (p[6], p[9]);
            SWAP (p[7], p[8]);
            p += size;
        }
        break;
    case 8:
        while (nitems--) {
            SWAP (p[0], p[7]);
            SWAP (p[1], p[6]);
            SWAP (p[2], p[5]);
            SWAP (p[3], p[4]);
            p += size;
        }
        break;
    case 4:
        while (nitems--) {
            SWAP (p[0], p[3]);
            SWAP (p[1], p[2]);
            p += size;
        }
        break;
    case 2:
        while (nitems--) {
            SWAP (p[0], p[1]);
            p += size;
        }
        break;
    case 1:
        break; /* Nothing to do. */
    default:
        _tt_abort("can't swap a %"PRIuZ"-byte item for (un)dumping", size);
    }
}
#else /* not WORDS_BIGENDIAN */
#define swap_items(a,b,c) do {} while(0)
#endif


/* Here we write NITEMS items, each item being ITEM_SIZE bytes long.
   The pointer to the stuff to write is P, and we write to the file
   OUT_FILE.  */

static void
do_dump (char *p, size_t item_size, size_t nitems, rust_output_handle_t out_file)
{
    swap_items (p, nitems, item_size);

    ssize_t r = ttstub_output_write (out_file, p, item_size * nitems);
    if (r < 0 || (size_t) r != item_size * nitems)
        _tt_abort ("could not write %"PRIuZ" %"PRIuZ"-byte item(s) to %s",
                   nitems, item_size, name_of_file);

    /* Have to restore the old contents of memory, since some of it might
       get used again.  */
    swap_items (p, nitems, item_size);
}


/* Here is the dual of the writing routine.  */

static void
do_undump (char *p, size_t item_size, size_t nitems, rust_input_handle_t in_file)
{
    ssize_t r = ttstub_input_read (in_file, p, item_size * nitems);
    if (r < 0 || (size_t) r != item_size * nitems)
        _tt_abort("could not undump %"PRIuZ" %"PRIuZ"-byte item(s) from %s",
                  nitems, item_size, name_of_file);

    swap_items (p, nitems, item_size);
}


#define dump_things(base, len) \
    do_dump ((char *) &(base), sizeof (base), (size_t) (len), fmt_out)
#define undump_things(base, len) \
    do_undump ((char *) &(base), sizeof (base), (size_t) (len), fmt_in)

/* Like do_undump, but check each value against LOW and HIGH.  The
   slowdown isn't significant, and this improves the chances of
   detecting incompatible format files.  In fact, Knuth himself noted
   this problem with Web2c some years ago, so it seems worth fixing.  We
   can't make this a subroutine because then we lose the type of BASE.  */
#define undump_checked_things(low, high, base, len)                     \
    do {                                                                \
        int i;                                                     \
        undump_things (base, len);                                      \
        for (i = 0; i < (len); i++) {                                   \
            if ((&(base))[i] < (low) || (&(base))[i] > (high)) {        \
                _tt_abort ("item %u (=%" PRIdPTR ") of .fmt array at %" PRIxPTR \
                           " <%" PRIdPTR " or >%" PRIdPTR,              \
                           i, (uintptr_t) (&(base))[i], (uintptr_t) &(base), \
                           (uintptr_t) low, (uintptr_t) high);          \
            }                                                           \
        }                                                               \
    } while (0)

/* Like undump_checked_things, but only check the upper value. We use
   this when the base type is unsigned, and thus all the values will be
   greater than zero by definition.  */
#define undump_upper_check_things(high, base, len)                      \
    do {                                                                \
        int i;                                                     \
        undump_things (base, len);                                      \
        for (i = 0; i < (len); i++) {                                   \
            if ((&(base))[i] > (high)) {                                \
                _tt_abort ("Item %u (=%" PRIdPTR ") of .fmt array at %" PRIxPTR \
                           " >%" PRIdPTR,                               \
                           i, (uintptr_t) (&(base))[i], (uintptr_t) &(base), \
                           (uintptr_t) high);                           \
            }                                                           \
        }                                                               \
    } while (0)


/* Since dump_things is a macro with a sizeof(), these all work: */
#define dump_b64(x) dump_things(x, 1)
#define dump_b32(x) dump_things(x, 1)
#define undump_b64(x) undump_things(x, 1)
#define undump_b32(x) undump_things(x, 1)

/* `dump_int' is called with constant integers, so we put them into a
   variable first.  */
#define dump_int(x)            \
    do {                       \
        int32_t x_val = (x);   \
        dump_things(x_val, 1); \
    } while (0)

#define undump_int(x) undump_things(x, 1)


#define hash_offset 514
#define sup_max_strings 2097151L /* magic constant, origin unclear */
#define sup_font_mem_size 147483647L /* magic constant, origin unclear */
#define sup_pool_size 40000000L
#define sup_string_vacancies (sup_pool_size - 23000)
#define sup_hash_extra sup_max_strings
#define inf_expand_depth 10
#define sup_expand_depth 10000000L


/*:134*//*135: */

static void
sort_avail(void)
{
    int32_t p, q, r;
    int32_t old_rover;

    p = get_node(0x40000000);
    p = mem[rover + 1].b32.s1;
    mem[rover + 1].b32.s1 = MAX_HALFWORD;
    old_rover = rover;

    /*136: */

    while (p != old_rover) {
        if (p < rover) {
            q = p;
            p = mem[q + 1].b32.s1;
            mem[q + 1].b32.s1 = rover;
            rover = q;
        } else {
            q = rover;
            while (mem[q + 1].b32.s1 < p)
                q = mem[q + 1].b32.s1;
            r = mem[p + 1].b32.s1;
            mem[p + 1].b32.s1 = mem[q + 1].b32.s1;
            mem[q + 1].b32.s1 = p;
            p = r;
        }
    }

    p = rover;

    while (mem[p + 1].b32.s1 != MAX_HALFWORD) {
        mem[mem[p + 1].b32.s1 + 1].b32.s0 = p;
        p = mem[p + 1].b32.s1;
    }

    mem[p + 1].b32.s1 = rover;
    mem[rover + 1].b32.s0 = p;
}

/*:271*//*276: */

static void
primitive(const char* ident, uint16_t c, int32_t o)
{
    int32_t prim_val;
    int len = strlen(ident);
    if (len > 1) {
        str_number s = maketexstring(ident);

        if (first + len > buf_size + 1)
            overflow("buffer size", buf_size);

        for (int i = 0; i < len; i++)
            buffer[first + i] = ident[i];

        cur_val = id_lookup(first, len);
        str_ptr--;
        pool_ptr = str_start[str_ptr - TOO_BIG_CHAR];
        hash[cur_val].s1 = s;
        prim_val = prim_lookup(s);
    } else {
        cur_val = ident[0] + SINGLE_BASE;
        prim_val = prim_lookup(ident[0]);
    }

    eqtb[cur_val].b16.s0 = LEVEL_ONE;
    eqtb[cur_val].b16.s1 = c;
    eqtb[cur_val].b32.s1 = o;
    eqtb[PRIM_EQTB_BASE + prim_val].b16.s0 = LEVEL_ONE;
    eqtb[PRIM_EQTB_BASE + prim_val].b16.s1 = c;
    eqtb[PRIM_EQTB_BASE + prim_val].b32.s1 = o;
}

/*:925*//*977: */

trie_opcode new_trie_op(small_number d, small_number n, trie_opcode v)
{
    int32_t h;
    trie_opcode u;
    int32_t l;
    h = abs(n + 313 * d + 361 * v + 1009 * cur_lang) % (TRIE_OP_SIZE - NEG_TRIE_OP_SIZE) + NEG_TRIE_OP_SIZE;
    while (true) {

        l = TRIE_OP_HASH(h);
        if (l == 0) {
            if (trie_op_ptr == TRIE_OP_SIZE)
                overflow("pattern memory ops", TRIE_OP_SIZE);
            u = trie_used[cur_lang];
            if (u == MAX_TRIE_OP)
                overflow("pattern memory ops per language",
                         MAX_TRIE_OP - MIN_TRIE_OP);
            trie_op_ptr++;
            u++;
            trie_used[cur_lang] = u;
            if (u > max_op_used)
                max_op_used = u;
            hyf_distance[trie_op_ptr] = d;
            hyf_num[trie_op_ptr] = n;
            hyf_next[trie_op_ptr] = v;
            trie_op_lang[trie_op_ptr] = cur_lang;
            TRIE_OP_HASH(h) = trie_op_ptr;
            trie_op_val[trie_op_ptr] = u;
            return u;
        }
        if ((hyf_distance[l] == d) && (hyf_num[l] == n) && (hyf_next[l] == v) && (trie_op_lang[l] == cur_lang)) {
            return trie_op_val[l];
        }
        if (h > -(int32_t) TRIE_OP_SIZE)
            h--;
        else
            h = TRIE_OP_SIZE;
    }
}

trie_pointer trie_node(trie_pointer p)
{
    trie_pointer h;
    trie_pointer q;
    h = abs(trie_c[p] + 1009 * trie_o[p] + 2718 * trie_l[p] + 3142 * trie_r[p]) % trie_size;
    while (true) {

        q = trie_hash[h];
        if (q == 0) {
            trie_hash[h] = p;
            return p;
        }
        if ((trie_c[q] == trie_c[p]) && (trie_o[q] == trie_o[p]) && (trie_l[q] == trie_l[p])
            && (trie_r[q] == trie_r[p])) {
            return q;
        }
        if (h > 0)
            h--;
        else
            h = trie_size;
    }
}

trie_pointer compress_trie(trie_pointer p)
{
    if (p == 0)
        return 0;
    else {

        trie_l[p] = compress_trie(trie_l[p]);
        trie_r[p] = compress_trie(trie_r[p]);
        return trie_node(p);
    }
}

void first_fit(trie_pointer p)
{
    trie_pointer h;
    trie_pointer z;
    trie_pointer q;
    UTF16_code c;
    trie_pointer l, r;
    int32_t /*too_big_char */ ll;
    c = trie_c[p];
    z = trie_min[c];
    while (true) {

        h = z - c;
        if (trie_max < h + max_hyph_char) {
            if (trie_size <= h + max_hyph_char)
                overflow("pattern memory", trie_size);
            do {
                trie_max++;
                trie_taken[trie_max] = false;
                trie_trl[trie_max] = trie_max + 1;
                trie_tro[trie_max] = trie_max - 1;
            } while (!(trie_max == h + max_hyph_char));
        }
        if (trie_taken[h])
            goto not_found;
        q = trie_r[p];
        while (q > 0) {

            if (trie_trl[h + trie_c[q]] == 0)
                goto not_found;
            q = trie_r[q];
        }
        goto found;
    not_found:                        /*not_found */ z = trie_trl[z];
    }
found:                        /*found *//*991: */ trie_taken[h] = true;
    trie_hash[p] = h;
    q = p;
    do {
        z = h + trie_c[q];
        l = trie_tro[z];
        r = trie_trl[z];
        trie_tro[r] = l;
        trie_trl[l] = r;
        trie_trl[z] = 0;
        if (l < max_hyph_char) {
            if (z < max_hyph_char)
                ll = z;
            else
                ll = max_hyph_char;
            do {
                trie_min[l] = r;
                l++;
            } while (!(l == ll));
        }
        q = trie_r[q];
    } while (!(q == 0 /*:991 */ ));
}

void trie_pack(trie_pointer p)
{
    trie_pointer q;
    do {
        q = trie_l[p];
        if ((q > 0) && (trie_hash[q] == 0)) {
            first_fit(q);
            trie_pack(q);
        }
        p = trie_r[p];
    } while (!(p == 0));
}

void trie_fix(trie_pointer p)
{
    trie_pointer q;
    UTF16_code c;
    trie_pointer z;
    z = trie_hash[p];
    do {
        q = trie_l[p];
        c = trie_c[p];
        trie_trl[z + c] = trie_hash[q];
        trie_trc[z + c] = c;
        trie_tro[z + c] = trie_o[p];
        if (q > 0)
            trie_fix(q);
        p = trie_r[p];
    } while (!(p == 0));
}

static void
new_patterns(void)
{
    short /*hyphenatable_length_limit 1 */ k, l;
    bool digit_sensed;
    trie_opcode v;
    trie_pointer p, q;
    bool first_child;
    UTF16_code c;

    if (trie_not_ready) {
        if (INTPAR(language) <= 0)
            cur_lang = 0;
        else if (INTPAR(language) > BIGGEST_LANG)
            cur_lang = 0;
        else
            cur_lang = INTPAR(language);

        scan_left_brace();
        k = 0;
        hyf[0] = 0;
        digit_sensed = false;

        while (true) {
            get_x_token();

            switch (cur_cmd) {
            case LETTER:
            case OTHER_CHAR:
                if (digit_sensed || cur_chr < '0'  || cur_chr > '9' ) {
                    if (cur_chr == '.' ) {
                        cur_chr = 0;
                    } else {
                        cur_chr = LC_CODE(cur_chr);

                        if (cur_chr == 0) {
                            error_here_with_diagnostic("Nonletter");
                            capture_to_diagnostic(NULL);
                            help_ptr = 1;
                            help_line[0] = "(See Appendix H.)";
                            error();
                        }
                    }

                    if (cur_chr > max_hyph_char)
                        max_hyph_char = cur_chr;

                    if (k < max_hyphenatable_length()) {
                        k++;
                        hc[k] = cur_chr;
                        hyf[k] = 0;
                        digit_sensed = false;
                    }
                } else if (k < max_hyphenatable_length()) {
                    hyf[k] = cur_chr - 48;
                    digit_sensed = true;
                }
                break;

            case SPACER:
            case RIGHT_BRACE:
                if (k > 0) { /*998:*/
                    if (hc[1] == 0)
                        hyf[0] = 0;
                    if (hc[k] == 0)
                        hyf[k] = 0;

                    l = k;
                    v = MIN_TRIE_OP;

                    while (true) {
                        if (hyf[l] != 0)
                            v = new_trie_op(k - l, hyf[l], v);
                        if (l > 0)
                            l--;
                        else
                            break;
                    }

                    q = 0;
                    hc[0] = cur_lang;

                    while (l <= k) {
                        c = hc[l];
                        l++;
                        p = trie_l[q];
                        first_child = true;

                        while (p > 0 && c > trie_c[p]) {
                            q = p;
                            p = trie_r[q];
                            first_child = false;
                        }

                        if (p == 0 || c < trie_c[p]) { /*999:*/
                            if (trie_ptr == trie_size)
                                overflow("pattern memory", trie_size);
                            trie_ptr++;
                            trie_r[trie_ptr] = p;
                            p = trie_ptr;
                            trie_l[p] = 0;
                            if (first_child)
                                trie_l[q] = p;
                            else
                                trie_r[q] = p;
                            trie_c[p] = c;
                            trie_o[p] = MIN_TRIE_OP;
                        }

                        q = p;
                    }

                    if (trie_o[q] != MIN_TRIE_OP) {
                        error_here_with_diagnostic("Duplicate pattern");
                        capture_to_diagnostic(NULL);
                        help_ptr = 1;
                        help_line[0] = "(See Appendix H.)";
                        error();
                    }

                    trie_o[q] = v;
                }

                if (cur_cmd == RIGHT_BRACE)
                    goto done;

                k = 0;
                hyf[0] = 0;
                digit_sensed = false;
                break;

            default:
                error_here_with_diagnostic("Bad ");
                print_esc_cstr("patterns");
                capture_to_diagnostic(NULL);
                help_ptr = 1;
                help_line[0] = "(See Appendix H.)";
                error();
                break;
            }
        }

    done: /*:996*/
        if (INTPAR(saving_hyphs) > 0) { /*1643:*/
            c = cur_lang;
            first_child = false;
            p = 0;

            do {
                q = p;
                p = trie_r[q];
            } while (!(p == 0 || c <= trie_c[p]));

            if (p == 0 || c < trie_c[p]) { /*999:*/
                if (trie_ptr == trie_size)
                    overflow("pattern memory", trie_size);
                trie_ptr++;
                trie_r[trie_ptr] = p;
                p = trie_ptr;
                trie_l[p] = 0;
                if (first_child)
                    trie_l[q] = p;
                else
                    trie_r[q] = p;
                trie_c[p] = c;
                trie_o[p] = MIN_TRIE_OP;
            }

            q = p;
            p = trie_l[q];
            first_child = true;

            for (c = 0; c <= 255; c++) {
                if (LC_CODE(c) > 0 || (c == 255 && first_child)) {
                    if (p == 0) { /*999:*/
                        if (trie_ptr == trie_size)
                            overflow("pattern memory", trie_size);
                        trie_ptr++;
                        trie_r[trie_ptr] = p;
                        p = trie_ptr;
                        trie_l[p] = 0;
                        if (first_child)
                            trie_l[q] = p;
                        else
                            trie_r[q] = p;
                        trie_c[p] = c;
                        trie_o[p] = MIN_TRIE_OP;
                    } else {
                        trie_c[p] = c;
                    }

                    trie_o[p] = LC_CODE(c);
                    q = p;
                    p = trie_r[q];
                    first_child = false;
                }
            }

            if (first_child)
                trie_l[q] = 0;
            else
                trie_r[q] = 0; /*:1644*/
        }
    } else {
        error_here_with_diagnostic("Too late for ");
        print_esc_cstr("patterns");
        capture_to_diagnostic(NULL);
        help_ptr = 1;
        help_line[0] = "All patterns must be given before typesetting begins.";
        error();

        mem[GARBAGE].b32.s1 = scan_toks(false, false);
        flush_list(def_ref);
    }
}

void init_trie(void)
{
    trie_pointer p;
    int32_t j, k, t;
    trie_pointer r, s;
    max_hyph_char++;
    op_start[0] = -(int32_t) MIN_TRIE_OP;
    {
        register int32_t for_end;
        j = 1;
        for_end = BIGGEST_LANG;
        if (j <= for_end)
            do
                op_start[j] = op_start[j - 1] + trie_used[j - 1];
            while (j++ < for_end);
    }
    {
        register int32_t for_end;
        j = 1;
        for_end = trie_op_ptr;
        if (j <= for_end)
            do
                TRIE_OP_HASH(j) = op_start[trie_op_lang[j]] + trie_op_val[j];
            while (j++ < for_end);
    }
    {
        register int32_t for_end;
        j = 1;
        for_end = trie_op_ptr;
        if (j <= for_end)
            do
                while (TRIE_OP_HASH(j) > j) {

                    k = TRIE_OP_HASH(j);
                    t = hyf_distance[k];
                    hyf_distance[k] = hyf_distance[j];
                    hyf_distance[j] = t;
                    t = hyf_num[k];
                    hyf_num[k] = hyf_num[j];
                    hyf_num[j] = t;
                    t = hyf_next[k];
                    hyf_next[k] = hyf_next[j];
                    hyf_next[j] = t;
                    TRIE_OP_HASH(j) = TRIE_OP_HASH(k);
                    TRIE_OP_HASH(k) = k;
                }
            while (j++ < for_end);
    }
    {
        register int32_t for_end;
        p = 0;
        for_end = trie_size;
        if (p <= for_end)
            do
                trie_hash[p] = 0;
            while (p++ < for_end);
    }
    trie_r[0] = compress_trie(trie_r[0]);
    trie_l[0] = compress_trie(trie_l[0]);
    {
        register int32_t for_end;
        p = 0;
        for_end = trie_ptr;
        if (p <= for_end)
            do
                trie_hash[p] = 0;
            while (p++ < for_end);
    }
    {
        register int32_t for_end;
        p = 0;
        for_end = BIGGEST_CHAR;
        if (p <= for_end)
            do
                trie_min[p] = p + 1;
            while (p++ < for_end);
    }
    trie_trl[0] = 1;
    trie_max = 0 /*:987 */ ;
    if (trie_l[0] != 0) {
        first_fit(trie_l[0]);
        trie_pack(trie_l[0]);
    }
    if (trie_r[0] != 0) {       /*1645: */
        if (trie_l[0] == 0) {
            register int32_t for_end;
            p = 0;
            for_end = 255;
            if (p <= for_end)
                do
                    trie_min[p] = p + 2;
                while (p++ < for_end);
        }
        first_fit(trie_r[0]);
        trie_pack(trie_r[0]);
        hyph_start = trie_hash[trie_r[0]];
    }
    if (trie_max == 0) {
        {
            register int32_t for_end;
            r = 0;
            for_end = max_hyph_char;
            if (r <= for_end)
                do {
                    trie_trl[r] = 0;
                    trie_tro[r] = MIN_TRIE_OP;
                    trie_trc[r] = 0;
                }
                while (r++ < for_end);
        }
        trie_max = max_hyph_char;
    } else {

        if (trie_r[0] > 0)
            trie_fix(trie_r[0]);
        if (trie_l[0] > 0)
            trie_fix(trie_l[0]);
        r = 0;
        do {
            s = trie_trl[r];
            {
                trie_trl[r] = 0;
                trie_tro[r] = MIN_TRIE_OP;
                trie_trc[r] = 0;
            }
            r = s;
        } while (!(r > trie_max));
    }
    trie_trc[0] = '?' ;
    trie_not_ready = false;
}

/*:1001*/

static void
new_hyph_exceptions(void)
{
    short /*hyphenatable_length_limit 1 */ n;
    short /*hyphenatable_length_limit 1 */ j;
    hyph_pointer h;
    str_number k;
    int32_t p;
    int32_t q;
    str_number s;
    pool_pointer u, v;

    scan_left_brace();

    if (INTPAR(language) <= 0)
        cur_lang = 0;
    else if (INTPAR(language) > BIGGEST_LANG)
        cur_lang = 0;
    else
        cur_lang = INTPAR(language);

    if (trie_not_ready) {
        hyph_index = 0;
        goto not_found1;
    }

    if (trie_trc[hyph_start + cur_lang] != cur_lang)
        hyph_index = 0;
    else
        hyph_index = trie_trl[hyph_start + cur_lang];

not_found1: /*970:*/
    n = 0;
    p = TEX_NULL;

    while (true) {
        get_x_token();

    reswitch:
        switch (cur_cmd) {
        case LETTER:
        case OTHER_CHAR:
        case CHAR_GIVEN:
            if (cur_chr == '-' ) { /*973:*/
                if (n < max_hyphenatable_length()) {
                    q = get_avail();
                    mem[q].b32.s1 = p;
                    mem[q].b32.s0 = n;
                    p = q;
                }
            } else {
                if (hyph_index == 0 || cur_chr > 255)
                    hc[0] = LC_CODE(cur_chr);
                else if (trie_trc[hyph_index + cur_chr] != cur_chr)
                    hc[0] = 0;
                else
                    hc[0] = trie_tro[hyph_index + cur_chr];

                if (hc[0] == 0) {
                    error_here_with_diagnostic("Not a letter");
                    capture_to_diagnostic(NULL);
                    help_ptr = 2;
                    help_line[1] = "Letters in \\hyphenation words must have \\lccode>0.";
                    help_line[0] = "Proceed; I'll ignore the character I just read.";
                    error();
                } else if (n < max_hyphenatable_length()) {
                    n++;

                    if (hc[0] < 65536L) {
                        hc[n] = hc[0];
                    } else {
                        hc[n] = (hc[0] - 65536L) / 1024 + 55296L;
                        n++;
                        hc[n] = hc[0] % 1024 + 56320L;
                    }
                }
            }
            break;

        case CHAR_NUM:
            scan_char_num();
            cur_chr = cur_val;
            cur_cmd = CHAR_GIVEN;
            goto reswitch;
            break;

        case SPACER:
        case RIGHT_BRACE:
            if (n > 1) { /*974:*/
                n++;
                hc[n] = cur_lang;
                if (pool_ptr + n > pool_size)
                    overflow("pool size", pool_size - init_pool_ptr);
                h = 0;

                for (j = 1; j <= n; j++) {
                    h = (h + h + hc[j]) % HYPH_PRIME;
                    str_pool[pool_ptr] = hc[j];
                    pool_ptr++;
                }

                s = make_string();

                if (hyph_next <= HYPH_PRIME) {
                    while (hyph_next > 0 && hyph_word[hyph_next - 1] > 0)
                        hyph_next--;
                }

                if (hyph_count == hyph_size || hyph_next == 0)
                    overflow("exception dictionary", hyph_size);

                hyph_count++;

                while (hyph_word[h] != 0) {
                    k = hyph_word[h];
                    if (length(k) != length(s))
                        goto not_found;

                    u = str_start[(k) - 65536L];
                    v = str_start[(s) - 65536L];

                    do {
                        if (str_pool[u] != str_pool[v])
                            goto not_found;
                        u++;
                        v++;
                    } while (u != str_start[(k + 1) - 65536L]);

                    str_ptr--;
                    pool_ptr = str_start[str_ptr - TOO_BIG_CHAR];
                    s = hyph_word[h];
                    hyph_count--;
                    goto found;

                not_found: /*:976*/
                    if (hyph_link[h] == 0) {
                        hyph_link[h] = hyph_next;
                        if (hyph_next >= hyph_size)
                            hyph_next = HYPH_PRIME;
                        if (hyph_next > HYPH_PRIME)
                            hyph_next++;
                    }
                    h = hyph_link[h] - 1;
                }

            found:
                hyph_word[h] = s;
                hyph_list[h] = p; /*:975*/
            }

            if (cur_cmd == RIGHT_BRACE)
                return;

            n = 0;
            p = TEX_NULL;
            break;

        default:
            error_here_with_diagnostic("Improper ");
            print_esc_cstr("hyphenation");
            print_cstr(" will be flushed");
            capture_to_diagnostic(NULL);
            help_ptr = 2;
            help_line[1] = "Hyphenation exceptions must contain only letters";
            help_line[0] = "and hyphens. But continue; I'll forgive and forget.";
            error();
            break;
        }
    }
}


void
prefixed_command(void)
{
    small_number a;
    internal_font_number f;
    int32_t j;
    font_index k;
    int32_t p, q;
    int32_t n;
    bool e;

    a = 0;

    while (cur_cmd == PREFIX) {
        if (!odd(a / cur_chr))
            a = a + cur_chr;

        do {
            get_x_token();
        } while (cur_cmd == SPACER || cur_cmd == RELAX);

        if (cur_cmd <= MAX_NON_PREFIXED_COMMAND) { /*1247:*/
            error_here_with_diagnostic("You can't use a prefix with `");
            print_cmd_chr(cur_cmd, cur_chr);
            print_char('\'');
            capture_to_diagnostic(NULL);
            help_ptr = 1;
            help_line[0] = "I'll pretend you didn't say \\long or \\outer or \\global or \\protected.";
            back_error();
            return;
        }

        if (INTPAR(tracing_commands) > 2) {
            show_cur_cmd_chr();
        }
    }

    if (a >= 8) {
        j = PROTECTED_TOKEN;
        a = a - 8;
    } else {
        j = 0;
    }

    if (cur_cmd != DEF && (a % 4 != 0 || j != 0)) {
        error_here_with_diagnostic("You can't use `");
        print_esc_cstr("long");
        print_cstr("' or `");
        print_esc_cstr("outer");
        print_cstr("' or `");
        print_esc_cstr("protected");
        print_cstr("' with `");
        print_cmd_chr(cur_cmd, cur_chr);
        print_char('\'');
        capture_to_diagnostic(NULL);
        help_ptr = 1;
        help_line[0] = "I'll pretend you didn't say \\long or \\outer or \\protected here.";
        error();
    }

    if (INTPAR(global_defs) != 0) {
        if (INTPAR(global_defs) < 0) {
            if (a >= 4)
                a = a - 4;
        } else {
            if (a < 4)
                a = a + 4;
        }
    }

    switch (cur_cmd) { /*1252:*/
    case SET_FONT:
        if (a >= 4)
            geq_define(CUR_FONT_LOC, DATA, cur_chr);
        else
            eq_define(CUR_FONT_LOC, DATA, cur_chr);
        break;

    case DEF:
        if (odd(cur_chr) && a < 4 && INTPAR(global_defs) >= 0)
            a = a + 4;

        e = (cur_chr >= 2);
        get_r_token();
        p = cur_cs;
        q = scan_toks(true, e);

        if (j != 0) {
            q = get_avail();
            mem[q].b32.s0 = j;
            mem[q].b32.s1 = mem[def_ref].b32.s1;
            mem[def_ref].b32.s1 = q;
        }

        if (a >= 4)
            geq_define(p, CALL + (a % 4), def_ref);
        else
            eq_define(p, CALL + (a % 4), def_ref);
        break;

    case LET:
        n = cur_chr;
        get_r_token();
        p = cur_cs;

        if (n == NORMAL) {
            do {
                get_token();
            } while (cur_cmd == SPACER);

            if (cur_tok == (OTHER_TOKEN + '=' )) {
                get_token();
                if (cur_cmd == SPACER)
                    get_token();
            }
        } else {
            get_token();
            q = cur_tok;
            get_token();
            back_input();
            cur_tok = q;
            back_input();
        }

        if (cur_cmd >= CALL) {
            mem[cur_chr].b32.s0++;
        } else if (cur_cmd == REGISTER || cur_cmd == TOKS_REGISTER) {
            if (cur_chr < 0 || cur_chr > 19) /* 19 = lo_mem_stat_max, I think */
                mem[cur_chr + 1].b32.s0++;
        }

        if (a >= 4)
            geq_define(p, cur_cmd, cur_chr);
        else
            eq_define(p, cur_cmd, cur_chr);
        break;

    case SHORTHAND_DEF:
        if (cur_chr == CHAR_SUB_DEF_CODE) {
            scan_char_num();
            p = CHAR_SUB_CODE_BASE + cur_val;
            scan_optional_equals();
            scan_char_num();
            n = cur_val;
            scan_char_num();
            if (INTPAR(tracing_char_sub_def) > 0) {
                begin_diagnostic();
                print_nl_cstr("New character substitution: ");
                print(p - CHAR_SUB_CODE_BASE);
                print_cstr(" = ");
                print(n);
                print_char(' ');
                print(cur_val);
                end_diagnostic(false);
            }

            n = n * 256 + cur_val;

            if (a >= 4)
                geq_define(p, DATA, n);
            else
                eq_define(p, DATA, n);

            if ((p - CHAR_SUB_CODE_BASE) < INTPAR(char_sub_def_min)) {
                if (a >= 4)
                    geq_word_define(INT_BASE + INT_PAR__char_sub_def_min, p - CHAR_SUB_CODE_BASE);
                else
                    eq_word_define(INT_BASE + INT_PAR__char_sub_def_min, p - CHAR_SUB_CODE_BASE);
            }

            if ((p - CHAR_SUB_CODE_BASE) > INTPAR(char_sub_def_max)) {
                if (a >= 4)
                    geq_word_define(INT_BASE + INT_PAR__char_sub_def_max, p - CHAR_SUB_CODE_BASE);
                else
                    eq_word_define(INT_BASE + INT_PAR__char_sub_def_max, p - CHAR_SUB_CODE_BASE);
            }
        } else {
            n = cur_chr;
            get_r_token();
            p = cur_cs;

            if (a >= 4)
                geq_define(p, RELAX, TOO_BIG_USV);
            else
                eq_define(p, RELAX, TOO_BIG_USV);

            scan_optional_equals();

            switch (n) {
            case CHAR_DEF_CODE:
                scan_usv_num();
                if (a >= 4)
                    geq_define(p, CHAR_GIVEN, cur_val);
                else
                    eq_define(p, CHAR_GIVEN, cur_val);
                break;

            case MATH_CHAR_DEF_CODE:
                scan_fifteen_bit_int();
                if (a >= 4)
                    geq_define(p, MATH_GIVEN, cur_val);
                else
                    eq_define(p, MATH_GIVEN, cur_val);
                break;

            case XETEX_MATH_CHAR_NUM_DEF_CODE:
                scan_xetex_math_char_int();
                if (a >= 4)
                    geq_define(p, XETEX_MATH_GIVEN, cur_val);
                else
                    eq_define(p, XETEX_MATH_GIVEN, cur_val);
                break;

            case XETEX_MATH_CHAR_DEF_CODE:
                scan_math_class_int();
                n = set_class(cur_val);
                scan_math_fam_int();
                n = n + set_family(cur_val);
                scan_usv_num();
                n = n + cur_val;
                if (a >= 4)
                    geq_define(p, XETEX_MATH_GIVEN, n);
                else
                    eq_define(p, XETEX_MATH_GIVEN, n);
                break;

            default:
                scan_register_num();

                if (cur_val > 255) {
                    j = n - 2;

                    if (j > MU_VAL)
                        j = TOK_VAL;

                    find_sa_element(j, cur_val, true);
                    mem[cur_ptr + 1].b32.s0++;

                    if (j == TOK_VAL)
                        j = TOKS_REGISTER;
                    else
                        j = REGISTER;

                    if (a >= 4)
                        geq_define(p, j, cur_ptr);
                    else
                        eq_define(p, j, cur_ptr);
                } else {
                    switch (n) {
                    case COUNT_DEF_CODE:
                        if (a >= 4)
                            geq_define(p, ASSIGN_INT, COUNT_BASE + cur_val);
                        else
                            eq_define(p, ASSIGN_INT, COUNT_BASE + cur_val);
                        break;
                    case DIMEN_DEF_CODE:
                        if (a >= 4)
                            geq_define(p, ASSIGN_DIMEN, SCALED_BASE + cur_val);
                        else
                            eq_define(p, ASSIGN_DIMEN, SCALED_BASE + cur_val);
                        break;
                    case SKIP_DEF_CODE:
                        if (a >= 4)
                            geq_define(p, ASSIGN_GLUE, SKIP_BASE + cur_val);
                        else
                            eq_define(p, ASSIGN_GLUE, SKIP_BASE + cur_val);
                        break;
                    case MU_SKIP_DEF_CODE:
                        if (a >= 4)
                            geq_define(p, ASSIGN_MU_GLUE, MU_SKIP_BASE + cur_val);
                        else
                            eq_define(p, ASSIGN_MU_GLUE, MU_SKIP_BASE + cur_val);
                        break;
                    case TOKS_DEF_CODE:
                        if (a >= 4)
                            geq_define(p, ASSIGN_TOKS, TOKS_BASE + cur_val);
                        else
                            eq_define(p, ASSIGN_TOKS, TOKS_BASE + cur_val);
                        break;
                    }
                }
                break;
            }
        }
        break;

    case READ_TO_CS:
        j = cur_chr;
        scan_int();
        n = cur_val;
        if (!scan_keyword("to")) {
            error_here_with_diagnostic("Missing `to' inserted");
            capture_to_diagnostic(NULL);
            help_ptr = 2;
            help_line[1] = "You should have said `\\read<number> to \\cs'.";
            help_line[0] = "I'm going to look for the \\cs now.";
            error();
        }

        get_r_token();
        p = cur_cs;
        read_toks(n, p, j);

        if (a >= 4)
            geq_define(p, CALL, cur_val);
        else
            eq_define(p, CALL, cur_val);
        break;

    case TOKS_REGISTER:
    case ASSIGN_TOKS:
        q = cur_cs;
        e = false;

        if (cur_cmd == TOKS_REGISTER) {
            if (cur_chr == 0) {
                scan_register_num();
                if (cur_val > 255) {
                    find_sa_element(TOK_VAL, cur_val, true);
                    cur_chr = cur_ptr;
                    e = true;
                } else {
                    cur_chr = TOKS_BASE + cur_val;
                }
            } else {
                e = true;
            }
        } else if (cur_chr == LOCAL_BASE + LOCAL__xetex_inter_char) {
            scan_char_class_not_ignored();
            cur_ptr = cur_val;
            scan_char_class_not_ignored();
            find_sa_element(INTER_CHAR_VAL, cur_ptr * CHAR_CLASS_LIMIT + cur_val, true);
            cur_chr = cur_ptr;
            e = true;
        }

        p = cur_chr;
        scan_optional_equals();

        do {
            get_x_token();
        } while (cur_cmd == SPACER || cur_cmd == RELAX);

        if (cur_cmd != LEFT_BRACE) { /*1262:*/
            if (cur_cmd == TOKS_REGISTER || cur_cmd == ASSIGN_TOKS) {
                if (cur_cmd == TOKS_REGISTER) {
                    if (cur_chr == 0) {
                        scan_register_num();
                        if (cur_val < 256) {
                            q = TOKS_REG(cur_val);
                        } else {
                            find_sa_element(TOK_VAL, cur_val, false);
                            if (cur_ptr == TEX_NULL)
                                q = TEX_NULL;
                            else
                                q = mem[cur_ptr + 1].b32.s1;
                        }
                    } else {
                        q = mem[cur_chr + 1].b32.s1;
                    }
                } else if (cur_chr == LOCAL_BASE + LOCAL__xetex_inter_char) {
                    scan_char_class_not_ignored();
                    cur_ptr = cur_val;
                    scan_char_class_not_ignored();
                    find_sa_element(INTER_CHAR_VAL, cur_ptr * CHAR_CLASS_LIMIT + cur_val, false);
                    if (cur_ptr == TEX_NULL)
                        q = TEX_NULL;
                    else
                        q = mem[cur_ptr + 1].b32.s1;
                } else {
                    q = eqtb[cur_chr].b32.s1;
                }

                if (q == TEX_NULL) {
                    if (e) {
                        if (a >= 4)
                            gsa_def(p, TEX_NULL);
                        else
                            sa_def(p, TEX_NULL);
                    } else if (a >= 4) {
                        geq_define(p, UNDEFINED_CS, TEX_NULL);
                    } else {
                        eq_define(p, UNDEFINED_CS, TEX_NULL);
                    }
                } else {
                    mem[q].b32.s0++;
                    if (e) {
                        if (a >= 4)
                            gsa_def(p, q);
                        else
                            sa_def(p, q);
                    } else if (a >= 4) {
                        geq_define(p, CALL, q);
                    } else {
                        eq_define(p, CALL, q);
                    }
                }

                goto done;
            }
        }

        back_input();
        cur_cs = q;
        q = scan_toks(false, false);

        if (mem[def_ref].b32.s1 == TEX_NULL) {
            if (e) {
                if (a >= 4)
                    gsa_def(p, TEX_NULL);
                else
                    sa_def(p, TEX_NULL);
            } else if (a >= 4) {
                geq_define(p, UNDEFINED_CS, TEX_NULL);
            } else {
                eq_define(p, UNDEFINED_CS, TEX_NULL);
            }

            mem[def_ref].b32.s1 = avail;
            avail = def_ref;
        } else {
            if (p == LOCAL_BASE + LOCAL__output_routine && !e) {
                mem[q].b32.s1 = get_avail();
                q = LLIST_link(q);
                mem[q].b32.s0 = (RIGHT_BRACE_TOKEN + 125);
                q = get_avail();
                mem[q].b32.s0 = (LEFT_BRACE_TOKEN + 123);
                mem[q].b32.s1 = mem[def_ref].b32.s1;
                mem[def_ref].b32.s1 = q;
            }

            if (e) {
                if (a >= 4)
                    gsa_def(p, def_ref);
                else
                    sa_def(p, def_ref);
            } else if (a >= 4) {
                geq_define(p, CALL, def_ref);
            } else {
                eq_define(p, CALL, def_ref);
            }
        }

        break;

    case ASSIGN_INT:
        p = cur_chr;
        scan_optional_equals();
        scan_int();
        if (a >= 4)
            geq_word_define(p, cur_val);
        else
            eq_word_define(p, cur_val);
        break;

    case ASSIGN_DIMEN:
        p = cur_chr;
        scan_optional_equals();
        scan_dimen(false, false, false);
        if (a >= 4)
            geq_word_define(p, cur_val);
        else
            eq_word_define(p, cur_val);
        break;

    case ASSIGN_GLUE:
    case ASSIGN_MU_GLUE:
        p = cur_chr;
        n = cur_cmd;
        scan_optional_equals();
        if (n == ASSIGN_MU_GLUE)
            scan_glue(MU_VAL);
        else
            scan_glue(GLUE_VAL);
        trap_zero_glue();
        if (a >= 4)
            geq_define(p, GLUE_REF, cur_val);
        else
            eq_define(p, GLUE_REF, cur_val);
        break;

    case XETEX_DEF_CODE:
        if (cur_chr == SF_CODE_BASE) {
            p = cur_chr;
            scan_usv_num();
            p = p + cur_val;
            n = SF_CODE(cur_val) % 65536L;
            scan_optional_equals();
            scan_char_class();
            if (a >= 4)
                geq_define(p, DATA, cur_val * 65536L + n);
            else
                eq_define(p, DATA, cur_val * 65536L + n);
        } else if (cur_chr == MATH_CODE_BASE) {
            p = cur_chr;
            scan_usv_num();
            p = p + cur_val;
            scan_optional_equals();
            scan_xetex_math_char_int();
            if (a >= 4)
                geq_define(p, DATA, cur_val);
            else
                eq_define(p, DATA, cur_val);
        } else if (cur_chr == MATH_CODE_BASE + 1) {
            p = cur_chr - 1;
            scan_usv_num();
            p = p + cur_val;
            scan_optional_equals();
            scan_math_class_int();
            n = set_class(cur_val);
            scan_math_fam_int();
            n = n + set_family(cur_val);
            scan_usv_num();
            n = n + cur_val;
            if (a >= 4)
                geq_define(p, DATA, n);
            else
                eq_define(p, DATA, n);
        } else if (cur_chr == DEL_CODE_BASE) {
            p = cur_chr;
            scan_usv_num();
            p = p + cur_val;
            scan_optional_equals();
            scan_int();
            if (a >= 4)
                geq_word_define(p, cur_val);
            else
                eq_word_define(p, cur_val);
        } else {
            p = cur_chr - 1;
            scan_usv_num();
            p = p + cur_val;
            scan_optional_equals();
            n = 0x40000000; /* "extended delimiter code flag" */
            scan_math_fam_int();
            n = n + cur_val * 0x200000; /* "extended delimiter code family */
            scan_usv_num();
            n = n + cur_val;
            if (a >= 4)
                geq_word_define(p, n);
            else
                eq_word_define(p, n);
        }
        break;

    case DEF_CODE:
        if (cur_chr == CAT_CODE_BASE)
            n = MAX_CHAR_CODE;
        else if (cur_chr == MATH_CODE_BASE)
            n = 0x8000;
        else if (cur_chr == SF_CODE_BASE)
            n = 0x7FFF;
        else if (cur_chr == DEL_CODE_BASE)
            n = 0xFFFFFF;
        else
            n = BIGGEST_USV; /*:1268 */

        p = cur_chr;
        scan_usv_num();
        p = p + cur_val;
        scan_optional_equals();
        scan_int();

        if ((cur_val < 0 && p < DEL_CODE_BASE) || cur_val > n) {
            error_here_with_diagnostic("Invalid code (");
            print_int(cur_val);
            if (p < DEL_CODE_BASE)
                print_cstr("), should be in the range 0..");
            else
                print_cstr("), should be at most ");
            print_int(n);
            capture_to_diagnostic(NULL);
            help_ptr = 1;
            help_line[0] = "I'm going to use 0 instead of that illegal code value.";
            error();
            cur_val = 0;
        }

        if (p < MATH_CODE_BASE) {
            if (p >= SF_CODE_BASE) {
                n = eqtb[p].b32.s1 / 65536L;
                if (a >= 4)
                    geq_define(p, DATA, n * 65536L + cur_val);
                else
                    eq_define(p, DATA, n * 65536L + cur_val);
            } else if (a >= 4) {
                geq_define(p, DATA, cur_val);
            } else {
                eq_define(p, DATA, cur_val);
            }
        } else if (p < DEL_CODE_BASE) {
            if (cur_val == 32768L)
                cur_val = ACTIVE_MATH_CHAR;
            else
                cur_val = set_class(cur_val / 4096) + set_family((cur_val % 4096) / 256) + (cur_val % 256);

            if (a >= 4)
                geq_define(p, DATA, cur_val);
            else
                eq_define(p, DATA, cur_val);
        } else if (a >= 4) {
            geq_word_define(p, cur_val);
        } else {
            eq_word_define(p, cur_val);
        }
        break;

    case DEF_FAMILY:
        p = cur_chr;
        scan_math_fam_int();
        p = p + cur_val;
        scan_optional_equals();
        scan_font_ident();
        if (a >= 4)
            geq_define(p, DATA, cur_val);
        else
            eq_define(p, DATA, cur_val);
        break;

    case REGISTER:
    case ADVANCE:
    case MULTIPLY:
    case DIVIDE:
        do_register_command(a);
        break;

    case SET_BOX:
        scan_register_num();
        if (a >= 4)
            n = GLOBAL_BOX_FLAG + cur_val;
        else
            n = BOX_FLAG + cur_val;

        scan_optional_equals();

        if (set_box_allowed) {
            scan_box(n);
        } else {
            error_here_with_diagnostic("Improper ");
            print_esc_cstr("setbox");
            capture_to_diagnostic(NULL);
            help_ptr = 2;
            help_line[1] = "Sorry, \\setbox is not allowed after \\halign in a display,";
            help_line[0] = "or between \\accent and an accented character.";
            error();
        }
        break;

    case SET_AUX:
        alter_aux();
        break;

    case SET_PREV_GRAF:
        alter_prev_graf();
        break;

    case SET_PAGE_DIMEN:
        alter_page_so_far();
        break;

    case SET_PAGE_INT:
        alter_integer();
        break;

    case SET_BOX_DIMEN:
        alter_box_dimen();
        break;

    case SET_SHAPE:
        q = cur_chr;
        scan_optional_equals();
        scan_int();
        n = cur_val;

        if (n <= 0) {
            p = TEX_NULL;
        } else if (q > LOCAL_BASE + LOCAL__par_shape) {
            n = (cur_val / 2) + 1;
            p = get_node(2 * n + 1);
            mem[p].b32.s0 = n;
            n = cur_val;
            mem[p + 1].b32.s1 = n;

            for (j = p + 2; j <= p + n + 1; j++) {
                scan_int();
                mem[j].b32.s1 = cur_val;
            }

            if (!odd(n))
                mem[p + n + 2].b32.s1 = 0;
        } else {
            p = get_node(2 * n + 1);
            mem[p].b32.s0 = n;

            for (j = 1; j <= n; j++) {
                scan_dimen(false, false, false);
                mem[p + 2 * j - 1].b32.s1 = cur_val;
                scan_dimen(false, false, false);
                mem[p + 2 * j].b32.s1 = cur_val;
            }
        }

        if (a >= 4)
            geq_define(q, SHAPE_REF, p);
        else
            eq_define(q, SHAPE_REF, p);
        break;

    case HYPH_DATA:
        if (cur_chr == 1) {
            if (in_initex_mode) {
                new_patterns();
                goto done;
            }

            error_here_with_diagnostic("Patterns can be loaded only by INITEX");
            capture_to_diagnostic(NULL);
            help_ptr = 0;
            error();

            do {
                get_token();
            } while (cur_cmd != RIGHT_BRACE);

            return;
        } else {
            new_hyph_exceptions();
            goto done;
        }
        break;

    case ASSIGN_FONT_DIMEN:
        find_font_dimen(true);
        k = cur_val;
        scan_optional_equals();
        scan_dimen(false, false, false);
        font_info[k].b32.s1 = cur_val;
        break;

    case ASSIGN_FONT_INT:
        n = cur_chr;
        scan_font_ident();
        f = cur_val;

        if (n < 2) {
            scan_optional_equals();
            scan_int();
            if (n == 0)
                hyphen_char[f] = cur_val;
            else
                skew_char[f] = cur_val;
        } else {
            if (font_area[f] == AAT_FONT_FLAG || font_area[f] == OTGR_FONT_FLAG)
                scan_glyph_number(f);
            else
                scan_char_num();

            p = cur_val;
            scan_optional_equals();
            scan_int();
            switch (n) {
            case LP_CODE_BASE:
                set_cp_code(f, p, LEFT_SIDE, cur_val);
                break;
            case RP_CODE_BASE:
                set_cp_code(f, p, RIGHT_SIDE, cur_val);
                break;
            }
        }
        break;

    case DEF_FONT:
        new_font(a);
        break;

    case SET_INTERACTION:
        new_interaction();
        break;

    default:
        confusion("prefix");
        break;
    }

done: /*1304:*/
    if (after_token != 0) {
        cur_tok = after_token;
        back_input();
        after_token = 0;
    }
}
/*:1328*/


/*1337:*/
static void
store_fmt_file(void)
{
    int32_t j, k, l;
    int32_t p, q;
    int32_t x;
    rust_output_handle_t fmt_out;

    if (save_ptr != 0) {
        error_here_with_diagnostic("You can't dump inside a group");
        capture_to_diagnostic(NULL);
        help_ptr = 1;
        help_line[0] = "`{...\\dump}' is a no-no.";

        if (interaction == ERROR_STOP_MODE)
            interaction = SCROLL_MODE;
        if (log_opened)
            error();

        history = HISTORY_FATAL_ERROR;
        close_files_and_terminate();
        ttstub_output_flush (rust_stdout);
        _tt_abort("\\dump inside a group");
    }

    selector = SELECTOR_NEW_STRING;
    print_cstr(" (preloaded format=");
    print(job_name);
    print_char(' ');
    print_int(INTPAR(year));
    print_char('.');
    print_int(INTPAR(month));
    print_char('.');
    print_int(INTPAR(day));
    print_char(')');

    if (interaction == BATCH_MODE)
        selector = SELECTOR_LOG_ONLY;
    else
        selector = SELECTOR_TERM_AND_LOG;

    if (pool_ptr + 1 > pool_size)
        overflow("pool size", pool_size - init_pool_ptr);

    format_ident = make_string();
    pack_job_name(".fmt");

    fmt_out = ttstub_output_open (name_of_file, 0);
    if (fmt_out == NULL)
        _tt_abort ("cannot open format output file \"%s\"", name_of_file);

    print_nl_cstr("Beginning to dump on file ");
    print(make_name_string());

    str_ptr--;
    pool_ptr = str_start[str_ptr - TOO_BIG_CHAR];

    print_nl_cstr("");
    print(format_ident);

    /* Header */

    dump_int(FORMAT_HEADER_MAGIC);
    dump_int(FORMAT_SERIAL);
    dump_int(hash_high);

    while (pseudo_files != TEX_NULL)
        pseudo_close(); /* TODO: can we move this farther up in this function? */

    dump_int(MEM_TOP);
    dump_int(EQTB_SIZE);
    dump_int(HASH_PRIME);
    dump_int(HYPH_PRIME);

    /* string pool */

    dump_int(pool_ptr);
    dump_int(str_ptr);
    dump_things(str_start[0], str_ptr - TOO_BIG_CHAR + 1);
    dump_things(str_pool[0], pool_ptr);

    print_ln();
    print_int(str_ptr);
    print_cstr(" strings of total length ");
    print_int(pool_ptr);

    /* "memory locations" */

    sort_avail();
    var_used = 0;
    dump_int(lo_mem_max);
    dump_int(rover);

    for (k = INT_VAL; k <= INTER_CHAR_VAL; k++)
        dump_int(sa_root[k]);

    p = 0;
    q = rover;
    x = 0;
    do {
        dump_things(mem[p], q + 2 - p);
        x = x + q + 2 - p;
        var_used = var_used + q - p;
        p = q + mem[q].b32.s0;
        q = mem[q + 1].b32.s1;
    } while (q != rover);

    var_used = var_used + lo_mem_max - p;
    dyn_used = mem_end + 1 - hi_mem_min;
    dump_things(mem[p], lo_mem_max + 1 - p);

    x = x + lo_mem_max + 1 - p;
    dump_int(hi_mem_min);
    dump_int(avail);
    dump_things(mem[hi_mem_min], mem_end + 1 - hi_mem_min);

    x = x + mem_end + 1 - hi_mem_min;
    p = avail;
    while (p != TEX_NULL) {
        dyn_used--;
        p = LLIST_link(p);
    }

    dump_int(var_used);
    dump_int(dyn_used);

    print_ln();
    print_int(x);
    print_cstr(" memory locations dumped; current usage is ");
    print_int(var_used);
    print_char('&');
    print_int(dyn_used);

    /* equivalents table / primitive */

    k = ACTIVE_BASE;

    do {
        j = k;

        while (j < INT_BASE - 1) {
            if (eqtb[j].b32.s1 == eqtb[j + 1].b32.s1 &&
                eqtb[j].b16.s1 == eqtb[j + 1].b16.s1 &&
                eqtb[j].b16.s0 == eqtb[j + 1].b16.s0)
                goto found1;
            j++;
        }

        l = INT_BASE;
        goto done1;

    found1:
        j++;
        l = j;

        while (j < INT_BASE - 1) {
            if (eqtb[j].b32.s1 != eqtb[j + 1].b32.s1 ||
                eqtb[j].b16.s1 != eqtb[j + 1].b16.s1 ||
                eqtb[j].b16.s0 != eqtb[j + 1].b16.s0)
                goto done1;
            j++;
        }
    done1:

        dump_int(l - k);
        dump_things(eqtb[k], l - k);
        k = j + 1;
        dump_int(k - l);
    } while (k != INT_BASE); /*:1350*/

    do {
        j = k;

        while (j < EQTB_SIZE) {
            if (eqtb[j].b32.s1 == eqtb[j + 1].b32.s1)
                goto found2;
            j++;
        }

        l = EQTB_SIZE + 1;
        goto done2;

    found2:
        j++;
        l = j;

        while (j < EQTB_SIZE) {
            if (eqtb[j].b32.s1 != eqtb[j + 1].b32.s1)
                goto done2;
            j++;
        }

    done2:
        dump_int(l - k);
        dump_things(eqtb[k], l - k);
        k = j + 1;
        dump_int(k - l);
    } while (k <= EQTB_SIZE);

    if (hash_high > 0)
        dump_things(eqtb[EQTB_SIZE + 1], hash_high);

    dump_int(par_loc);
    dump_int(write_loc);

    for (p = 0; p <= PRIM_SIZE; p++)
        dump_b32(prim[p]);

    /* control sequences */

    dump_int(hash_used);
    cs_count = (FROZEN_CONTROL_SEQUENCE - 1) - hash_used + hash_high;

    for (p = HASH_BASE; p <= hash_used; p++) {
        if (hash[p].s1 != 0) {
            dump_int(p);
            dump_b32(hash[p]);
            cs_count++;
        }
    }

    dump_things(hash[hash_used + 1], (UNDEFINED_CONTROL_SEQUENCE - 1) - hash_used);
    if (hash_high > 0)
        dump_things(hash[EQTB_SIZE + 1], hash_high);

    dump_int(cs_count);

    print_ln();
    print_int(cs_count);
    print_cstr(" multiletter control sequences");

    /* fonts */

    dump_int(fmem_ptr);
    dump_things(font_info[0], fmem_ptr);
    dump_int(font_ptr);
    dump_things(font_check[FONT_BASE], font_ptr + 1);
    dump_things(font_size[FONT_BASE], font_ptr + 1);
    dump_things(font_dsize[FONT_BASE], font_ptr + 1);
    dump_things(font_params[FONT_BASE], font_ptr + 1);
    dump_things(hyphen_char[FONT_BASE], font_ptr + 1);
    dump_things(skew_char[FONT_BASE], font_ptr + 1);
    dump_things(font_name[FONT_BASE], font_ptr + 1);
    dump_things(font_area[FONT_BASE], font_ptr + 1);
    dump_things(font_bc[FONT_BASE], font_ptr + 1);
    dump_things(font_ec[FONT_BASE], font_ptr + 1);
    dump_things(char_base[FONT_BASE], font_ptr + 1);
    dump_things(width_base[FONT_BASE], font_ptr + 1);
    dump_things(height_base[FONT_BASE], font_ptr + 1);
    dump_things(depth_base[FONT_BASE], font_ptr + 1);
    dump_things(italic_base[FONT_BASE], font_ptr + 1);
    dump_things(lig_kern_base[FONT_BASE], font_ptr + 1);
    dump_things(kern_base[FONT_BASE], font_ptr + 1);
    dump_things(exten_base[FONT_BASE], font_ptr + 1);
    dump_things(param_base[FONT_BASE], font_ptr + 1);
    dump_things(font_glue[FONT_BASE], font_ptr + 1);
    dump_things(bchar_label[FONT_BASE], font_ptr + 1);
    dump_things(font_bchar[FONT_BASE], font_ptr + 1);
    dump_things(font_false_bchar[FONT_BASE], font_ptr + 1);

    for (k = FONT_BASE; k <= font_ptr; k++) {
        print_nl_cstr("\\font");
        print_esc(hash[FONT_ID_BASE + k].s1);
        print_char('=');

        if (font_area[k] == AAT_FONT_FLAG || font_area[k] == OTGR_FONT_FLAG || font_mapping[k] != NULL) {
            print_file_name(font_name[k], EMPTY_STRING, EMPTY_STRING);

            error_here_with_diagnostic("Can't \\dump a format with native fonts or font-mappings");
            capture_to_diagnostic(NULL);

            help_ptr = 3;
            help_line[2] = "You really, really don't want to do this.";
            help_line[1] = "It won't work, and only confuses me.";
            help_line[0] = "(Load them at runtime, not as part of the format file.)";
            error();
        } else {
            print_file_name(font_name[k], font_area[k], EMPTY_STRING);
        }

        if (font_size[k] != font_dsize[k]) {
            print_cstr(" at ");
            print_scaled(font_size[k]);
            print_cstr("pt");
        }
    }

    print_ln();
    print_int(fmem_ptr - 7);
    print_cstr(" words of font info for ");
    print_int(font_ptr - 0);
    if (font_ptr != FONT_BASE + 1)
        print_cstr(" preloaded fonts");
    else
        print_cstr(" preloaded font");

    /* hyphenation info */

    dump_int(hyph_count);
    if (hyph_next <= HYPH_PRIME)
        hyph_next = hyph_size;
    dump_int(hyph_next);

    for (k = 0; k <= hyph_size; k++) {
        if (hyph_word[k] != 0) {
            dump_int(k + 65536L * hyph_link[k]);
            dump_int(hyph_word[k]);
            dump_int(hyph_list[k]);
        }
    }

    print_ln();
    print_int(hyph_count);
    if (hyph_count != 1)
        print_cstr(" hyphenation exceptions");
    else
        print_cstr(" hyphenation exception");

    if (trie_not_ready)
        init_trie();

    dump_int(trie_max);
    dump_int(hyph_start);
    dump_things(trie_trl[0], trie_max + 1);
    dump_things(trie_tro[0], trie_max + 1);
    dump_things(trie_trc[0], trie_max + 1);
    dump_int(max_hyph_char);
    dump_int(trie_op_ptr);
    dump_things(hyf_distance[1], trie_op_ptr);
    dump_things(hyf_num[1], trie_op_ptr);
    dump_things(hyf_next[1], trie_op_ptr);

    print_nl_cstr("Hyphenation trie of length ");
    print_int(trie_max);
    print_cstr(" has ");
    print_int(trie_op_ptr);
    if (trie_op_ptr != 1)
        print_cstr(" ops");
    else
        print_cstr(" op");
    print_cstr(" out of ");
    print_int(TRIE_OP_SIZE);

    for (k = BIGGEST_LANG; k >= 0; k--) {
        if (trie_used[k] > 0) {
            print_nl_cstr("  ");
            print_int(trie_used[k]);
            print_cstr(" for language ");
            print_int(k);
            dump_int(k);
            dump_int(trie_used[k]);
        }
    }

    /* footer */

    dump_int(FORMAT_FOOTER_MAGIC);

    INTPAR(tracing_stats) = 0; /*:1361*/
    ttstub_output_close(fmt_out);
}


static void
pack_buffered_name(small_number n, int32_t a, int32_t b)
{
    free(name_of_file);
    name_of_file = xmalloc_array(UTF8_code, format_default_length + 1);

    strcpy(name_of_file, TEX_format_default);
    name_length = strlen(name_of_file);
}


static bool
load_fmt_file(void)
{
    int32_t j, k;
    int32_t p, q;
    int32_t x;
    rust_input_handle_t fmt_in;

    j = cur_input.loc;

    /* This is where a first line starting with "&" used to
     * trigger code that would change the format file. */

    pack_buffered_name(format_default_length - 4, 1, 0);

    fmt_in = ttstub_input_open(name_of_file, TTBC_FILE_FORMAT_FORMAT, 0);
    if (fmt_in == NULL)
        _tt_abort("cannot open the format file \"%s\"", name_of_file);

    cur_input.loc = j;

    if (in_initex_mode) {
        free(font_info);
        free(str_pool);
        free(str_start);
        free(yhash);
        free(eqtb);
        free(mem);
        mem = NULL;
    }

    /* start reading the header */

    undump_int(x);
    if (x != FORMAT_HEADER_MAGIC)
        goto bad_fmt;

    undump_int(x);
    if (x != FORMAT_SERIAL)
        _tt_abort("format file \"%s\" is of the wrong version: expected %d, found %d",
                  name_of_file, FORMAT_SERIAL, x);

    /* hash table parameters */

    undump_int(hash_high);
    if (hash_high < 0 || hash_high > sup_hash_extra)
        goto bad_fmt;
    if (hash_extra < hash_high)
        hash_extra = hash_high;

    eqtb_top = EQTB_SIZE + hash_extra;
    if (hash_extra == 0)
        hash_top = UNDEFINED_CONTROL_SEQUENCE;
    else
        hash_top = eqtb_top;

    yhash = xmalloc_array(b32x2, 1 + hash_top - hash_offset);
    hash = yhash - hash_offset;
    hash[HASH_BASE].s0 = 0;
    hash[HASH_BASE].s1 = 0;

    for (x = HASH_BASE + 1; x <= hash_top; x++)
        hash[x] = hash[HASH_BASE];

    eqtb = xmalloc_array(memory_word, eqtb_top + 1);
    eqtb[UNDEFINED_CONTROL_SEQUENCE].b16.s1 = UNDEFINED_CS;
    eqtb[UNDEFINED_CONTROL_SEQUENCE].b32.s1 = TEX_NULL;
    eqtb[UNDEFINED_CONTROL_SEQUENCE].b16.s0 = LEVEL_ZERO;

    for (x = EQTB_SIZE + 1; x <= eqtb_top; x++)
        eqtb[x] = eqtb[UNDEFINED_CONTROL_SEQUENCE];

    max_reg_num = 32767;
    max_reg_help_line = "A register number must be between 0 and 32767.";

    /* "memory locations" */

    undump_int(x);
    if (x != MEM_TOP)
        goto bad_fmt;

    cur_list.head = CONTRIB_HEAD;
    cur_list.tail = CONTRIB_HEAD;
    page_tail = PAGE_HEAD;
    mem = xmalloc_array(memory_word, MEM_TOP + 1);

    undump_int(x);
    if (x != EQTB_SIZE)
        goto bad_fmt;

    undump_int(x);
    if (x != HASH_PRIME)
        goto bad_fmt;

    undump_int(x);
    if (x != HYPH_PRIME)
        goto bad_fmt;

    /* string pool */

    undump_int(x);
    if (x < 0)
        goto bad_fmt;
    if (x > sup_pool_size - pool_free)
        _tt_abort ("must increase string_pool_size");
    pool_ptr = x;

    if (pool_size < pool_ptr + pool_free)
        pool_size = pool_ptr + pool_free;

    undump_int(x);
    if (x < 0)
        goto bad_fmt;
    if (x > sup_max_strings - strings_free)
        _tt_abort ("must increase sup_strings");
    str_ptr = x;

    if (max_strings < str_ptr + strings_free)
        max_strings = str_ptr + strings_free;

    str_start = xmalloc_array(pool_pointer, max_strings);
    undump_checked_things(0, pool_ptr, str_start[0], str_ptr - TOO_BIG_CHAR + 1);
    str_pool = xmalloc_array(packed_UTF16_code, pool_size);

    undump_things(str_pool[0], pool_ptr);

    init_str_ptr = str_ptr;
    init_pool_ptr = pool_ptr; /*:1345 */

    /* "By sorting the list of available spaces in the variable-size portion
     * of |mem|, we are usually able to get by without having to dump very
     * much of the dynamic memory." */

    undump_int(x);
    if (x < 1019 || x > MEM_TOP - HI_MEM_STAT_USAGE)
        goto bad_fmt;
    else
        lo_mem_max = x;

    undump_int(x);
    if (x < 20 || x > lo_mem_max)
        goto bad_fmt;
    else
        rover = x;

    for (k = INT_VAL; k <= INTER_CHAR_VAL; k++) {
        undump_int(x);
        if (x < MIN_HALFWORD || x > lo_mem_max)
            goto bad_fmt;
        else
            sa_root[k] = x;
    }

    p = 0;
    q = rover;

    do {
        undump_things(mem[p], q + 2 - p);
        p = q + mem[q].b32.s0;
        if (p > lo_mem_max || (q >= mem[q + 1].b32.s1 && mem[q + 1].b32.s1 != rover))
            goto bad_fmt;
        q = mem[q + 1].b32.s1;
    } while (q != rover);

    undump_things(mem[p], lo_mem_max + 1 - p);

    undump_int(x);
    if (x < lo_mem_max + 1 || x > PRE_ADJUST_HEAD)
        goto bad_fmt;
    else
        hi_mem_min = x;

    undump_int(x);
    if (x < MIN_HALFWORD || x > MEM_TOP)
        goto bad_fmt;
    else
        avail = x;

    mem_end = MEM_TOP;

    undump_things(mem[hi_mem_min], mem_end + 1 - hi_mem_min);
    undump_int(var_used);
    undump_int(dyn_used);

    /* equivalents table / primitives
     *
     * "The table of equivalents usually contains repeated information, so we
     * dump it in compressed form: The sequence of $n + 2$ values
     * $(n, x_1, \ldots, x_n, m)$ in the format file represents $n + m$ consecutive
     * entries of |eqtb|, with |m| extra copies of $x_n$, namely
     * $(x_1, \ldots, x_n, x_n, \ldots, x_n)$"
     */

    k = ACTIVE_BASE;

    do {
        undump_int(x);
        if (x < 1 || k + x > EQTB_SIZE + 1)
            goto bad_fmt;

        undump_things(eqtb[k], x);
        k = k + x;

        undump_int(x);
        if (x < 0 || k + x > EQTB_SIZE + 1)
            goto bad_fmt;

        for (j = k; j <= k + x - 1; j++)
            eqtb[j] = eqtb[k - 1];

        k = k + x;
    } while (k <= EQTB_SIZE);

    if (hash_high > 0)
        undump_things(eqtb[EQTB_SIZE + 1], hash_high);

    undump_int(x);
    if (x < HASH_BASE || x > hash_top)
        goto bad_fmt;
    else
        par_loc = x;

    par_token = CS_TOKEN_FLAG + par_loc;

    undump_int(x);
    if (x < HASH_BASE || x > hash_top)
        goto bad_fmt;
    else
        write_loc = x;

    /* control sequence names
     *
     * "A different scheme is used to compress the hash table, since its lower
     * region is usually sparse. When |text(p) != 0| for |p <= hash_used|, we
     * output two words, |p| and |hash[p]|. The hash table is, of course,
     * densely packed for |p >= hash_used|, so the remaining entries are
     * output in a block."
     */

    for (p = 0; p <= PRIM_SIZE; p++)
        undump_b32(prim[p]);

    undump_int(x);
    if (x < HASH_BASE || x > FROZEN_CONTROL_SEQUENCE)
        goto bad_fmt;
    else
        hash_used = x;

    p = HASH_BASE - 1;

    do {
        undump_int(x);
        if (x < p + 1 || x > hash_used)
            goto bad_fmt;
        else
            p = x;
        undump_b32(hash[p]);
    } while (p != hash_used);

    undump_things(hash[hash_used + 1], (UNDEFINED_CONTROL_SEQUENCE - 1) - hash_used);

    if (hash_high > 0)
        undump_things(hash[EQTB_SIZE + 1], hash_high);

    undump_int(cs_count);

    /* font info */

    undump_int(x);
    if (x < 7)
        goto bad_fmt;
    if (x > sup_font_mem_size)
        _tt_abort ("must increase font_mem_size");

    fmem_ptr = x;
    if (fmem_ptr > font_mem_size)
        font_mem_size = fmem_ptr;

    font_info = xmalloc_array(memory_word, font_mem_size);
    undump_things(font_info[0], fmem_ptr);

    undump_int(x);
    if (x < FONT_BASE)
        goto bad_fmt;
    if (x > FONT_BASE + MAX_FONT_MAX)
        _tt_abort ("must increase font_max");

    font_ptr = x;

    font_mapping = xmalloc_array(void *, font_max);
    font_layout_engine = xcalloc_array(void *, font_max);
    font_flags = xmalloc_array(char, font_max);
    font_letter_space = xmalloc_array(scaled_t, font_max);
    font_check = xmalloc_array(b16x4, font_max);
    font_size = xmalloc_array(scaled_t, font_max);
    font_dsize = xmalloc_array(scaled_t, font_max);
    font_params = xmalloc_array(font_index, font_max);
    font_name = xmalloc_array(str_number, font_max);
    font_area = xmalloc_array(str_number, font_max);
    font_bc = xmalloc_array(UTF16_code, font_max);
    font_ec = xmalloc_array(UTF16_code, font_max);
    font_glue = xmalloc_array(int32_t, font_max);
    hyphen_char = xmalloc_array(int32_t, font_max);
    skew_char = xmalloc_array(int32_t, font_max);
    bchar_label = xmalloc_array(font_index, font_max);
    font_bchar = xmalloc_array(nine_bits, font_max);
    font_false_bchar = xmalloc_array(nine_bits, font_max);
    char_base = xmalloc_array(int32_t, font_max);
    width_base = xmalloc_array(int32_t, font_max);
    height_base = xmalloc_array(int32_t, font_max);
    depth_base = xmalloc_array(int32_t, font_max);
    italic_base = xmalloc_array(int32_t, font_max);
    lig_kern_base = xmalloc_array(int32_t, font_max);
    kern_base = xmalloc_array(int32_t, font_max);
    exten_base = xmalloc_array(int32_t, font_max);
    param_base = xmalloc_array(int32_t, font_max);

    for (k = FONT_BASE; k <= font_ptr; k++)
        font_mapping[k] = 0;

    undump_things(font_check[FONT_BASE], font_ptr + 1);
    undump_things(font_size[FONT_BASE], font_ptr + 1);
    undump_things(font_dsize[FONT_BASE], font_ptr + 1);
    undump_checked_things(MIN_HALFWORD, MAX_HALFWORD, font_params[FONT_BASE], font_ptr + 1);
    undump_things(hyphen_char[FONT_BASE], font_ptr + 1);
    undump_things(skew_char[FONT_BASE], font_ptr + 1);
    undump_upper_check_things(str_ptr, font_name[FONT_BASE], font_ptr + 1);
    undump_upper_check_things(str_ptr, font_area[FONT_BASE], font_ptr + 1);
    undump_things(font_bc[FONT_BASE], font_ptr + 1);
    undump_things(font_ec[FONT_BASE], font_ptr + 1);
    undump_things(char_base[FONT_BASE], font_ptr + 1);
    undump_things(width_base[FONT_BASE], font_ptr + 1);
    undump_things(height_base[FONT_BASE], font_ptr + 1);
    undump_things(depth_base[FONT_BASE], font_ptr + 1);
    undump_things(italic_base[FONT_BASE], font_ptr + 1);
    undump_things(lig_kern_base[FONT_BASE], font_ptr + 1);
    undump_things(kern_base[FONT_BASE], font_ptr + 1);
    undump_things(exten_base[FONT_BASE], font_ptr + 1);
    undump_things(param_base[FONT_BASE], font_ptr + 1);
    undump_checked_things(MIN_HALFWORD, lo_mem_max, font_glue[FONT_BASE], font_ptr + 1);
    undump_checked_things(0, fmem_ptr - 1, bchar_label[FONT_BASE], font_ptr + 1);
    undump_checked_things(0, TOO_BIG_CHAR, font_bchar[FONT_BASE], font_ptr + 1);
    undump_checked_things(0, TOO_BIG_CHAR, font_false_bchar[FONT_BASE], font_ptr + 1);

    /* hyphenations */

    undump_int(x);
    if (x < 0)
        goto bad_fmt;
    if (x > hyph_size)
        _tt_abort ("must increase hyph_size");
    hyph_count = x;

    undump_int(x);
    if (x < HYPH_PRIME)
        goto bad_fmt;
    if (x > hyph_size)
        _tt_abort ("must increase hyph_size");
    hyph_next = x;

    j = 0;

    for (k = 1; k <= hyph_count; k++) {
        undump_int(j);
        if (j < 0)
            goto bad_fmt;
        if (j > 65535L) {
            hyph_next = j / 65536L;
            j = j - hyph_next * 65536L;
        } else {
            hyph_next = 0;
        }

        if (j >= hyph_size || hyph_next > hyph_size)
            goto bad_fmt;

        hyph_link[j] = hyph_next;

        undump_int(x);
        if (x < 0 || x > str_ptr)
            goto bad_fmt;
        else
            hyph_word[j] = x;

        undump_int(x);
        if (x < MIN_HALFWORD || x > MAX_HALFWORD)
            goto bad_fmt;
        else
            hyph_list[j] = x;
    }

    j++;
    if (j < HYPH_PRIME)
        j = HYPH_PRIME;

    hyph_next = j;
    if (hyph_next >= hyph_size)
        hyph_next = HYPH_PRIME;
    else if (hyph_next >= HYPH_PRIME)
        hyph_next++;

    undump_int(x);
    if (x < 0)
        goto bad_fmt;
    if (x > trie_size)
        _tt_abort ("must increase trie_size");

    j = x;
    trie_max = j;

    undump_int(x);
    if (x < 0 || x > j)
        goto bad_fmt;
    else
        hyph_start = x;

    if (!trie_trl)
        trie_trl = xmalloc_array(trie_pointer, j + 1);
    undump_things(trie_trl[0], j + 1);

    if (!trie_tro)
        trie_tro = xmalloc_array(trie_pointer, j + 1);
    undump_things(trie_tro[0], j + 1);

    if (!trie_trc)
        trie_trc = xmalloc_array(uint16_t, j + 1);
    undump_things(trie_trc[0], j + 1);

    undump_int(max_hyph_char);

    undump_int(x);
    if (x < 0)
        goto bad_fmt;
    if (x > TRIE_OP_SIZE)
        _tt_abort ("must increase TRIE_OP_SIZE");

    j = x;
    trie_op_ptr = j;

    undump_things(hyf_distance[1], j);
    undump_things(hyf_num[1], j);
    undump_upper_check_things(MAX_TRIE_OP, hyf_next[1], j);

    for (k = 0; k <= BIGGEST_LANG; k++)
        trie_used[k] = 0;

    k = BIGGEST_LANG + 1;

    while (j > 0) {
        undump_int(x);
        if (x < 0 || x > k - 1)
            goto bad_fmt;
        else
            k = x;

        undump_int(x);
        if (x < 1 || x > j)
            goto bad_fmt;

        trie_used[k] = x;
        j = j - x;
        op_start[k] = j;
    }

    trie_not_ready = false;

    /* trailer */

    undump_int(x);
    if (x != FORMAT_FOOTER_MAGIC)
        goto bad_fmt;

    ttstub_input_close (fmt_in);
    return true;

bad_fmt:
    _tt_abort ("fatal format file error");
}


static void
final_cleanup(void)
{
    small_number c;

    c = cur_chr;
    if (job_name == 0)
        open_log_file();
    while (input_ptr > 0)
        if (cur_input.state == TOKEN_LIST)
            end_token_list();
        else
            end_file_reading();
    while (open_parens > 0) {

        print_cstr(" )");
        open_parens--;
    }
    if (cur_level > LEVEL_ONE) {
        print_nl('(' );
        print_esc_cstr("end occurred ");
        print_cstr("inside a group at level ");
        print_int(cur_level - 1);
        print_char(')');
        show_save_groups();
    }
    while (cond_ptr != TEX_NULL) {

        print_nl('(' );
        print_esc_cstr("end occurred ");
        print_cstr("when ");
        print_cmd_chr(IF_TEST, cur_if);
        if (if_line != 0) {
            print_cstr(" on line ");
            print_int(if_line);
        }
        print_cstr(" was incomplete)");
        if_line = mem[cond_ptr + 1].b32.s1;
        cur_if = mem[cond_ptr].b16.s0;
        temp_ptr = cond_ptr;
        cond_ptr = LLIST_link(cond_ptr);
        free_node(temp_ptr, IF_NODE_SIZE);
    }

    if (history != HISTORY_SPOTLESS) {
        if ((history == HISTORY_WARNING_ISSUED || (interaction < ERROR_STOP_MODE))) {

            if (selector == SELECTOR_TERM_AND_LOG) {
                selector = SELECTOR_TERM_ONLY;
                print_nl_cstr("(see the transcript file for additional information)");
                selector = SELECTOR_TERM_AND_LOG;
            }
        }
    }
    if (c == 1) {
        if (in_initex_mode) {
            {
                register int32_t for_end;
                c = TOP_MARK_CODE;
                for_end = SPLIT_BOT_MARK_CODE;
                if (c <= for_end)
                    do
                        if (cur_mark[c] != TEX_NULL)
                            delete_token_ref(cur_mark[c]);
                    while (c++ < for_end) ;
            }
            if (sa_root[MARK_VAL] != TEX_NULL) {

                if (do_marks(3, 0, sa_root[MARK_VAL]))
                    sa_root[MARK_VAL] = TEX_NULL;
            }
            {
                register int32_t for_end;
                c = LAST_BOX_CODE;
                for_end = VSPLIT_CODE;
                if (c <= for_end)
                    do
                        flush_node_list(disc_ptr[c]);
                    while (c++ < for_end);
            }
            if (last_glue != MAX_HALFWORD)
                delete_glue_ref(last_glue);
            store_fmt_file();
            return;
        }
        print_nl_cstr("(\\dump is performed only by INITEX)");
        return;
    }
}


/* Engine initialization */

static UFILE stdin_ufile;

static void
init_io(void)
{
    /* This is largely vestigial at this point */
    stdin_ufile.handle = NULL;
    stdin_ufile.savedChar = -1;
    stdin_ufile.skipNextLF = 0;
    stdin_ufile.encodingMode = UTF8;
    stdin_ufile.conversionData = 0;
    input_file[0] = &stdin_ufile;

    buffer[first] = 0;
    last = first;
    cur_input.loc = first;
    cur_input.limit = last;
    first = last + 1;
}


static void
initialize_more_variables(void)
{
    int32_t k;
    hyph_pointer z;

    doing_special = false;
    native_text_size = 128;
    native_text = xmalloc(native_text_size * sizeof(UTF16_code));

    interaction = ERROR_STOP_MODE;

    deletions_allowed = true;
    set_box_allowed = true;
    error_count = 0;
    help_ptr = 0;
    use_err_help = false;

    two_to_the[0] = 1;
    for (k = 1; k <= 30; k++)
        two_to_the[k] = 2 * two_to_the[k - 1];

    spec_log[1] = 93032640L;
    spec_log[2] = 38612034L;
    spec_log[3] = 17922280L;
    spec_log[4] = 8662214L;
    spec_log[5] = 4261238L;
    spec_log[6] = 2113709L;
    spec_log[7] = 1052693L;
    spec_log[8] = 525315L;
    spec_log[9] = 262400L;
    spec_log[10] = 131136L;
    spec_log[11] = 65552L;
    spec_log[12] = 32772L;
    spec_log[13] = 16385;
    for (k = 14; k <= 27; k++)
        spec_log[k] = two_to_the[27 - k];
    spec_log[28] = 1;

    nest_ptr = 0;
    max_nest_stack = 0;
    cur_list.mode = VMODE;
    cur_list.head = CONTRIB_HEAD;
    cur_list.tail = CONTRIB_HEAD;
    cur_list.eTeX_aux = TEX_NULL;
    cur_list.aux.b32.s1 = IGNORE_DEPTH;
    cur_list.mode_line = 0;
    cur_list.prev_graf = 0;
    shown_mode = 0;
    page_contents = EMPTY;
    page_tail = PAGE_HEAD;
    last_glue = MAX_HALFWORD;
    last_penalty = 0;
    last_kern = 0;
    last_node_type = -1;
    page_so_far[7] = 0;

    for (k = INT_BASE; k <= EQTB_SIZE; k++)
        XEQ_LEVEL(k) = LEVEL_ONE;

    no_new_control_sequence = true;
    prim[0].s0 = 0;
    prim[0].s1 = 0;

    for (k = 1; k <= PRIM_SIZE; k++)
        prim[k] = prim[0];

    save_ptr = 0;
    cur_level = LEVEL_ONE;
    cur_group = BOTTOM_LEVEL;
    cur_boundary = 0;
    max_save_stack = 0;
    mag_set = 0;
    expand_depth_count = 0;
    is_in_csname = false;
    cur_mark[TOP_MARK_CODE] = TEX_NULL;
    cur_mark[FIRST_MARK_CODE] = TEX_NULL;
    cur_mark[BOT_MARK_CODE] = TEX_NULL;
    cur_mark[SPLIT_FIRST_MARK_CODE] = TEX_NULL;
    cur_mark[SPLIT_BOT_MARK_CODE] = TEX_NULL;
    cur_val = 0;
    cur_val_level = INT_VAL;
    radix = 0;
    cur_order = NORMAL;

    for (k = 0; k <= 16; k++)
        read_open[k] = CLOSED;

    cond_ptr = TEX_NULL;
    if_limit = NORMAL;
    cur_if = 0;
    if_line = 0;
    null_character.s3 = 0;
    null_character.s2 = 0;
    null_character.s1 = 0;
    null_character.s0 = 0;
    total_pages = 0;
    max_v = 0;
    max_h = 0;
    max_push = 0;
    last_bop = -1;
    doing_leaders = false;
    dead_cycles = 0;
    adjust_tail = TEX_NULL;
    last_badness = 0;
    pre_adjust_tail = TEX_NULL;
    pack_begin_line = 0;
    empty.s1 = EMPTY;
    empty.s0 = TEX_NULL;
    align_ptr = TEX_NULL;
    cur_align = TEX_NULL;
    cur_span = TEX_NULL;
    cur_loop = TEX_NULL;
    cur_head = TEX_NULL;
    cur_tail = TEX_NULL;
    cur_pre_head = TEX_NULL;
    cur_pre_tail = TEX_NULL;
    cur_f = 0;
    max_hyph_char = TOO_BIG_LANG;

    for (z = 0; z <= hyph_size; z++) {
        hyph_word[z] = 0;
        hyph_list[z] = TEX_NULL;
        hyph_link[z] = 0;
    }

    hyph_count = 0;
    hyph_next = (HYPH_PRIME + 1);
    if (hyph_next > hyph_size)
        hyph_next = HYPH_PRIME;

    output_active = false;
    insert_penalties = 0;
    ligature_present = false;
    cancel_boundary = false;
    lft_hit = false;
    rt_hit = false;
    ins_disc = false;
    after_token = 0;
    long_help_seen = false;
    format_ident = 0;

    for (k = 0; k <= 17; k++)
        write_open[k] = false;

    LR_ptr = TEX_NULL;
    LR_problems = 0;
    cur_dir = LEFT_TO_RIGHT;
    pseudo_files = TEX_NULL;
    sa_root[MARK_VAL] = TEX_NULL;
    sa_null.b32.s0 = TEX_NULL;
    sa_null.b32.s1 = TEX_NULL;
    sa_chain = TEX_NULL;
    sa_level = LEVEL_ZERO;
    disc_ptr[LAST_BOX_CODE] = TEX_NULL;
    disc_ptr[VSPLIT_CODE] = TEX_NULL;
    edit_name_start = 0;
    stop_at_space = true;
}

static void
initialize_more_initex_variables(void)
{
    int32_t i, k;

    for (k = 1; k <= 19; k++)
        mem[k].b32.s1 = 0;

    for (k = 0; k <= 19; k += 4) {
        mem[k].b32.s1 = TEX_NULL + 1;
        mem[k].b16.s1 = NORMAL;
        mem[k].b16.s0 = NORMAL;
    }

    mem[6].b32.s1 = 65536L;
    mem[4].b16.s1 = FIL;
    mem[10].b32.s1 = 65536L;
    mem[8].b16.s1 = FILL;
    mem[14].b32.s1 = 65536L;
    mem[12].b16.s1 = FIL;
    mem[15].b32.s1 = 65536L;
    mem[12].b16.s0 = FIL;
    mem[18].b32.s1 = -65536L;
    mem[16].b16.s1 = FIL;
    rover = 20;
    mem[rover].b32.s1 = MAX_HALFWORD;
    mem[rover].b32.s0 = 1000;
    mem[rover + 1].b32.s0 = rover;
    mem[rover + 1].b32.s1 = rover;
    lo_mem_max = rover + 1000;
    mem[lo_mem_max].b32.s1 = TEX_NULL;
    mem[lo_mem_max].b32.s0 = TEX_NULL;

    for (k = PRE_ADJUST_HEAD; k <= MEM_TOP; k++)
        mem[k] = mem[lo_mem_max];

    mem[OMIT_TEMPLATE].b32.s0 = CS_TOKEN_FLAG + FROZEN_END_TEMPLATE;
    mem[END_SPAN].b32.s1 = UINT16_MAX + 1;
    mem[END_SPAN].b32.s0 = TEX_NULL;
    mem[ACTIVE_LIST].b16.s1 = HYPHENATED;
    mem[ACTIVE_LIST+1].b32.s0 = MAX_HALFWORD;
    mem[ACTIVE_LIST].b16.s0 = 0;
    mem[PAGE_INS_HEAD].b16.s0 = 255;
    mem[PAGE_INS_HEAD].b16.s1 = SPLIT_UP;
    mem[PAGE_INS_HEAD].b32.s1 = PAGE_INS_HEAD;
    NODE_type(PAGE_HEAD) = GLUE_NODE;
    mem[PAGE_HEAD].b16.s0 = NORMAL;
    avail = TEX_NULL;
    mem_end = MEM_TOP;
    hi_mem_min = PRE_ADJUST_HEAD;
    var_used = 20;
    dyn_used = HI_MEM_STAT_USAGE;
    eqtb[UNDEFINED_CONTROL_SEQUENCE].b16.s1 = UNDEFINED_CS;
    eqtb[UNDEFINED_CONTROL_SEQUENCE].b32.s1 = TEX_NULL;
    eqtb[UNDEFINED_CONTROL_SEQUENCE].b16.s0 = LEVEL_ZERO;

    for (k = ACTIVE_BASE; k <= eqtb_top; k++)
        eqtb[k] = eqtb[UNDEFINED_CONTROL_SEQUENCE];

    eqtb[GLUE_BASE].b32.s1 = 0;
    eqtb[GLUE_BASE].b16.s0 = LEVEL_ONE;
    eqtb[GLUE_BASE].b16.s1 = GLUE_REF;

    for (k = GLUE_BASE + 1; k <= LOCAL_BASE - 1; k++)
        eqtb[k] = eqtb[GLUE_BASE];

    mem[0].b32.s1 += 531;
    LOCAL(par_shape) = TEX_NULL;
    eqtb[LOCAL_BASE + LOCAL__par_shape].b16.s1 = SHAPE_REF;
    eqtb[LOCAL_BASE + LOCAL__par_shape].b16.s0 = LEVEL_ONE;

    for (k = ETEX_PEN_BASE; k <= ETEX_PENS - 1; k++)
        eqtb[k] = eqtb[LOCAL_BASE + LOCAL__par_shape];

    for (k = LOCAL_BASE + LOCAL__output_routine; k <= TOKS_BASE + NUMBER_REGS - 1; k++)
        eqtb[k] = eqtb[UNDEFINED_CONTROL_SEQUENCE];

    eqtb[BOX_BASE].b32.s1 = TEX_NULL;
    eqtb[BOX_BASE].b16.s1 = BOX_REF;
    eqtb[BOX_BASE].b16.s0 = LEVEL_ONE;

    for (k = BOX_BASE + 1; k <= BOX_BASE + NUMBER_REGS - 1; k++)
        eqtb[k] = eqtb[BOX_BASE];

    eqtb[CUR_FONT_LOC].b32.s1 = FONT_BASE;
    eqtb[CUR_FONT_LOC].b16.s1 = DATA;
    eqtb[CUR_FONT_LOC].b16.s0 = LEVEL_ONE;

    for (k = MATH_FONT_BASE; k <= MATH_FONT_BASE + NUMBER_MATH_FONTS - 1; k++)
        eqtb[k] = eqtb[CUR_FONT_LOC];

    eqtb[CAT_CODE_BASE].b32.s1 = 0;
    eqtb[CAT_CODE_BASE].b16.s1 = DATA;
    eqtb[CAT_CODE_BASE].b16.s0 = LEVEL_ONE;

    for (k = CAT_CODE_BASE + 1; k <= INT_BASE - 1; k++)
        eqtb[k] = eqtb[CAT_CODE_BASE];

    for (k = 0; k <= NUMBER_USVS - 1; k++) {
        CAT_CODE(k) = OTHER_CHAR;
        MATH_CODE(k) = k;
        SF_CODE(k) = 1000;
    }

    CAT_CODE(13) = CAR_RET;
    CAT_CODE(32) = SPACER;
    CAT_CODE(92) = ESCAPE;
    CAT_CODE(37) = COMMENT;
    CAT_CODE(127) = INVALID_CHAR;
    eqtb[CAT_CODE_BASE].b32.s1 = IGNORE;

    for (k = '0'; k <= '9'; k++)
        MATH_CODE(k) = k + set_class(VAR_FAM_CLASS);

    for (k = 'A'; k <= 'Z'; k++) {
        CAT_CODE(k) = LETTER;
        CAT_CODE(k + 32) = LETTER;
        MATH_CODE(k) = k + set_family(1) + set_class(VAR_FAM_CLASS);
        MATH_CODE(k + 32) = k + 32 + set_family(1) + set_class(VAR_FAM_CLASS);
        LC_CODE(k) = k + 32;
        LC_CODE(k + 32) = k + 32;
        UC_CODE(k) = k;
        UC_CODE(k + 32) = k;
        SF_CODE(k) = 999;
    }

    for (k = INT_BASE; k <= DEL_CODE_BASE - 1; k++)
        eqtb[k].b32.s1 = 0;

    INTPAR(char_sub_def_min) = 256;
    INTPAR(char_sub_def_max) = -1;
    INTPAR(mag) = 1000;
    INTPAR(tolerance) = 10000;
    INTPAR(hang_after) = 1;
    INTPAR(max_dead_cycles) = 25;
    INTPAR(escape_char) = '\\' ;
    INTPAR(end_line_char) = CARRIAGE_RETURN;

    for (k = 0; k <= NUMBER_USVS - 1; k++)
        DEL_CODE(k) = -1;

    DEL_CODE(46) = 0;

    for (k = DIMEN_BASE; k <= EQTB_SIZE; k++)
        eqtb[k].b32.s1 = 0;

    prim_used = PRIM_SIZE;
    hash_used = FROZEN_CONTROL_SEQUENCE;
    hash_high = 0;
    cs_count = 0;
    eqtb[FROZEN_DONT_EXPAND].b16.s1 = DONT_EXPAND;
    hash[FROZEN_DONT_EXPAND].s1 = maketexstring("notexpanded:");
    eqtb[FROZEN_PRIMITIVE].b16.s1 = IGNORE_SPACES;
    eqtb[FROZEN_PRIMITIVE].b32.s1 = 1;
    eqtb[FROZEN_PRIMITIVE].b16.s0 = LEVEL_ONE;
    hash[FROZEN_PRIMITIVE].s1 = maketexstring("primitive");

    for (k = -(int32_t) TRIE_OP_SIZE; k <= TRIE_OP_SIZE; k++)
        TRIE_OP_HASH(k) = 0;

    for (k = 0; k <= BIGGEST_LANG; k++)
        trie_used[k] = MIN_TRIE_OP;

    max_op_used = MIN_TRIE_OP;
    trie_op_ptr = 0;
    trie_not_ready = true;
    hash[FROZEN_PROTECTION].s1 = maketexstring("inaccessible");

    format_ident = maketexstring(" (INITEX)");

    hash[END_WRITE].s1 = maketexstring("endwrite");
    eqtb[END_WRITE].b16.s0 = LEVEL_ONE;
    eqtb[END_WRITE].b16.s1 = OUTER_CALL;
    eqtb[END_WRITE].b32.s1 = TEX_NULL;

    max_reg_num = 32767;
    max_reg_help_line = "A register number must be between 0 and 32767.";

    for (i = INT_VAL; i <= INTER_CHAR_VAL; i++)
        sa_root[i] = TEX_NULL;

    INTPAR(xetex_hyphenatable_length) = 63;
}


/*:1370*//*1371: */
static void
initialize_primitives(void)
{

    no_new_control_sequence = false;
    first = 0;

    primitive("lineskip", ASSIGN_GLUE, GLUE_BASE + GLUE_PAR__line_skip);
    primitive("baselineskip", ASSIGN_GLUE, GLUE_BASE + GLUE_PAR__baseline_skip);
    primitive("parskip", ASSIGN_GLUE, GLUE_BASE + GLUE_PAR__par_skip);
    primitive("abovedisplayskip", ASSIGN_GLUE, GLUE_BASE + GLUE_PAR__above_display_skip);
    primitive("belowdisplayskip", ASSIGN_GLUE, GLUE_BASE + GLUE_PAR__below_display_skip);
    primitive("abovedisplayshortskip", ASSIGN_GLUE, GLUE_BASE + GLUE_PAR__above_display_short_skip);
    primitive("belowdisplayshortskip", ASSIGN_GLUE, GLUE_BASE + GLUE_PAR__below_display_short_skip);
    primitive("leftskip", ASSIGN_GLUE, GLUE_BASE + GLUE_PAR__left_skip);
    primitive("rightskip", ASSIGN_GLUE, GLUE_BASE + GLUE_PAR__right_skip);
    primitive("topskip", ASSIGN_GLUE, GLUE_BASE + GLUE_PAR__top_skip);
    primitive("splittopskip", ASSIGN_GLUE, GLUE_BASE + GLUE_PAR__split_top_skip);
    primitive("tabskip", ASSIGN_GLUE, GLUE_BASE + GLUE_PAR__tab_skip);
    primitive("spaceskip", ASSIGN_GLUE, GLUE_BASE + GLUE_PAR__space_skip);
    primitive("xspaceskip", ASSIGN_GLUE, GLUE_BASE + GLUE_PAR__xspace_skip);
    primitive("parfillskip", ASSIGN_GLUE, GLUE_BASE + GLUE_PAR__par_fill_skip);
    primitive("XeTeXlinebreakskip", ASSIGN_GLUE, GLUE_BASE + GLUE_PAR__xetex_linebreak_skip);

    primitive("thinmuskip", ASSIGN_MU_GLUE, GLUE_BASE + GLUE_PAR__thin_mu_skip);
    primitive("medmuskip", ASSIGN_MU_GLUE, GLUE_BASE + GLUE_PAR__med_mu_skip);
    primitive("thickmuskip", ASSIGN_MU_GLUE, GLUE_BASE + GLUE_PAR__thick_mu_skip);

    primitive("output", ASSIGN_TOKS, LOCAL_BASE + LOCAL__output_routine);
    primitive("everypar", ASSIGN_TOKS, LOCAL_BASE + LOCAL__every_par);
    primitive("everymath", ASSIGN_TOKS, LOCAL_BASE + LOCAL__every_math);
    primitive("everydisplay", ASSIGN_TOKS, LOCAL_BASE + LOCAL__every_display);
    primitive("everyhbox", ASSIGN_TOKS, LOCAL_BASE + LOCAL__every_hbox);
    primitive("everyvbox", ASSIGN_TOKS, LOCAL_BASE + LOCAL__every_vbox);
    primitive("everyjob", ASSIGN_TOKS, LOCAL_BASE + LOCAL__every_job);
    primitive("everycr", ASSIGN_TOKS, LOCAL_BASE + LOCAL__every_cr);
    primitive("errhelp", ASSIGN_TOKS, LOCAL_BASE + LOCAL__err_help);
    primitive("everyeof", ASSIGN_TOKS, LOCAL_BASE + LOCAL__every_eof);
    primitive("XeTeXinterchartoks", ASSIGN_TOKS, LOCAL_BASE + LOCAL__xetex_inter_char);
    primitive("TectonicCodaTokens", ASSIGN_TOKS, LOCAL_BASE + LOCAL__TectonicCodaTokens);

    primitive("pretolerance", ASSIGN_INT, INT_BASE + INT_PAR__pretolerance);
    primitive("tolerance", ASSIGN_INT, INT_BASE + INT_PAR__tolerance);
    primitive("linepenalty", ASSIGN_INT, INT_BASE + INT_PAR__line_penalty);
    primitive("hyphenpenalty", ASSIGN_INT, INT_BASE + INT_PAR__hyphen_penalty);
    primitive("exhyphenpenalty", ASSIGN_INT, INT_BASE + INT_PAR__ex_hyphen_penalty);
    primitive("clubpenalty", ASSIGN_INT, INT_BASE + INT_PAR__club_penalty);
    primitive("widowpenalty", ASSIGN_INT, INT_BASE + INT_PAR__widow_penalty);
    primitive("displaywidowpenalty", ASSIGN_INT, INT_BASE + INT_PAR__display_widow_penalty);
    primitive("brokenpenalty", ASSIGN_INT, INT_BASE + INT_PAR__broken_penalty);
    primitive("binoppenalty", ASSIGN_INT, INT_BASE + INT_PAR__bin_op_penalty);
    primitive("relpenalty", ASSIGN_INT, INT_BASE + INT_PAR__rel_penalty);
    primitive("predisplaypenalty", ASSIGN_INT, INT_BASE + INT_PAR__pre_display_penalty);
    primitive("postdisplaypenalty", ASSIGN_INT, INT_BASE + INT_PAR__post_display_penalty);
    primitive("interlinepenalty", ASSIGN_INT, INT_BASE + INT_PAR__inter_line_penalty);
    primitive("doublehyphendemerits", ASSIGN_INT, INT_BASE + INT_PAR__double_hyphen_demerits);
    primitive("finalhyphendemerits", ASSIGN_INT, INT_BASE + INT_PAR__final_hyphen_demerits);
    primitive("adjdemerits", ASSIGN_INT, INT_BASE + INT_PAR__adj_demerits);
    primitive("mag", ASSIGN_INT, INT_BASE + INT_PAR__mag);
    primitive("delimiterfactor", ASSIGN_INT, INT_BASE + INT_PAR__delimiter_factor);
    primitive("looseness", ASSIGN_INT, INT_BASE + INT_PAR__looseness);
    primitive("time", ASSIGN_INT, INT_BASE + INT_PAR__time);
    primitive("day", ASSIGN_INT, INT_BASE + INT_PAR__day);
    primitive("month", ASSIGN_INT, INT_BASE + INT_PAR__month);
    primitive("year", ASSIGN_INT, INT_BASE + INT_PAR__year);
    primitive("showboxbreadth", ASSIGN_INT, INT_BASE + INT_PAR__show_box_breadth);
    primitive("showboxdepth", ASSIGN_INT, INT_BASE + INT_PAR__show_box_depth);
    primitive("hbadness", ASSIGN_INT, INT_BASE + INT_PAR__hbadness);
    primitive("vbadness", ASSIGN_INT, INT_BASE + INT_PAR__vbadness);
    primitive("pausing", ASSIGN_INT, INT_BASE + INT_PAR__pausing);
    primitive("tracingonline", ASSIGN_INT, INT_BASE + INT_PAR__tracing_online);
    primitive("tracingmacros", ASSIGN_INT, INT_BASE + INT_PAR__tracing_macros);
    primitive("tracingstats", ASSIGN_INT, INT_BASE + INT_PAR__tracing_stats);
    primitive("tracingparagraphs", ASSIGN_INT, INT_BASE + INT_PAR__tracing_paragraphs);
    primitive("tracingpages", ASSIGN_INT, INT_BASE + INT_PAR__tracing_pages);
    primitive("tracingoutput", ASSIGN_INT, INT_BASE + INT_PAR__tracing_output);
    primitive("tracinglostchars", ASSIGN_INT, INT_BASE + INT_PAR__tracing_lost_chars);
    primitive("tracingcommands", ASSIGN_INT, INT_BASE + INT_PAR__tracing_commands);
    primitive("tracingrestores", ASSIGN_INT, INT_BASE + INT_PAR__tracing_restores);
    primitive("uchyph", ASSIGN_INT, INT_BASE + INT_PAR__uc_hyph);
    primitive("outputpenalty", ASSIGN_INT, INT_BASE + INT_PAR__output_penalty);
    primitive("maxdeadcycles", ASSIGN_INT, INT_BASE + INT_PAR__max_dead_cycles);
    primitive("hangafter", ASSIGN_INT, INT_BASE + INT_PAR__hang_after);
    primitive("floatingpenalty", ASSIGN_INT, INT_BASE + INT_PAR__floating_penalty);
    primitive("globaldefs", ASSIGN_INT, INT_BASE + INT_PAR__global_defs);
    primitive("fam", ASSIGN_INT, INT_BASE + INT_PAR__cur_fam);
    primitive("escapechar", ASSIGN_INT, INT_BASE + INT_PAR__escape_char);
    primitive("defaulthyphenchar", ASSIGN_INT, INT_BASE + INT_PAR__default_hyphen_char);
    primitive("defaultskewchar", ASSIGN_INT, INT_BASE + INT_PAR__default_skew_char);
    primitive("endlinechar", ASSIGN_INT, INT_BASE + INT_PAR__end_line_char);
    primitive("newlinechar", ASSIGN_INT, INT_BASE + INT_PAR__new_line_char);
    primitive("language", ASSIGN_INT, INT_BASE + INT_PAR__language);
    primitive("lefthyphenmin", ASSIGN_INT, INT_BASE + INT_PAR__left_hyphen_min);
    primitive("righthyphenmin", ASSIGN_INT, INT_BASE + INT_PAR__right_hyphen_min);
    primitive("holdinginserts", ASSIGN_INT, INT_BASE + INT_PAR__holding_inserts);
    primitive("errorcontextlines", ASSIGN_INT, INT_BASE + INT_PAR__error_context_lines);

    primitive("XeTeXlinebreakpenalty", ASSIGN_INT, INT_BASE + 69);
    primitive("XeTeXprotrudechars", ASSIGN_INT, INT_BASE + 70);

    primitive("parindent", ASSIGN_DIMEN, DIMEN_BASE + 0);
    primitive("mathsurround", ASSIGN_DIMEN, DIMEN_BASE + 1);
    primitive("lineskiplimit", ASSIGN_DIMEN, DIMEN_BASE + 2);
    primitive("hsize", ASSIGN_DIMEN, DIMEN_BASE + 3);
    primitive("vsize", ASSIGN_DIMEN, DIMEN_BASE + 4);
    primitive("maxdepth", ASSIGN_DIMEN, DIMEN_BASE + 5);
    primitive("splitmaxdepth", ASSIGN_DIMEN, DIMEN_BASE + 6);
    primitive("boxmaxdepth", ASSIGN_DIMEN, DIMEN_BASE + 7);
    primitive("hfuzz", ASSIGN_DIMEN, DIMEN_BASE + 8);
    primitive("vfuzz", ASSIGN_DIMEN, DIMEN_BASE + 9);
    primitive("delimitershortfall", ASSIGN_DIMEN, DIMEN_BASE + 10);
    primitive("nulldelimiterspace", ASSIGN_DIMEN, DIMEN_BASE + 11);
    primitive("scriptspace", ASSIGN_DIMEN, DIMEN_BASE + 12);
    primitive("predisplaysize", ASSIGN_DIMEN, DIMEN_BASE + 13);
    primitive("displaywidth", ASSIGN_DIMEN, DIMEN_BASE + 14);
    primitive("displayindent", ASSIGN_DIMEN, DIMEN_BASE + 15);
    primitive("overfullrule", ASSIGN_DIMEN, DIMEN_BASE + 16);
    primitive("hangindent", ASSIGN_DIMEN, DIMEN_BASE + 17);
    primitive("hoffset", ASSIGN_DIMEN, DIMEN_BASE + 18);
    primitive("voffset", ASSIGN_DIMEN, DIMEN_BASE + 19);
    primitive("emergencystretch", ASSIGN_DIMEN, DIMEN_BASE + 20);
    primitive("pdfpagewidth", ASSIGN_DIMEN, DIMEN_BASE + 21);
    primitive("pdfpageheight", ASSIGN_DIMEN, DIMEN_BASE + 22);

    primitive(" ", EX_SPACE, 0);
    primitive("/", ITAL_CORR, 0);
    primitive("accent", ACCENT, 0);
    primitive("advance", ADVANCE, 0);
    primitive("afterassignment", AFTER_ASSIGNMENT, 0);
    primitive("aftergroup", AFTER_GROUP, 0);
    primitive("begingroup", BEGIN_GROUP, 0);
    primitive("char", CHAR_NUM, 0);
    primitive("csname", CS_NAME, 0);
    primitive("delimiter", DELIM_NUM, 0);
    primitive("XeTeXdelimiter", DELIM_NUM, 1);
    primitive("Udelimiter", DELIM_NUM, 1);
    primitive("divide", DIVIDE, 0);
    primitive("endcsname", END_CS_NAME, 0);
    primitive("endgroup", END_GROUP, 0);
    hash[FROZEN_END_GROUP].s1 = maketexstring("endgroup");
    eqtb[FROZEN_END_GROUP] = eqtb[cur_val];
    primitive("expandafter", EXPAND_AFTER, 0);
    primitive("font", DEF_FONT, 0);
    primitive("fontdimen", ASSIGN_FONT_DIMEN, 0);
    primitive("halign", HALIGN, 0);
    primitive("hrule", HRULE, 0);
    primitive("ignorespaces", IGNORE_SPACES, 0);
    primitive("insert", INSERT, 0);
    primitive("mark", MARK, 0);
    primitive("mathaccent", MATH_ACCENT, 0);
    primitive("XeTeXmathaccent", MATH_ACCENT, 1);
    primitive("Umathaccent", MATH_ACCENT, 1);
    primitive("mathchar", MATH_CHAR_NUM, 0);
    primitive("XeTeXmathcharnum", MATH_CHAR_NUM, 1);
    primitive("Umathcharnum", MATH_CHAR_NUM, 1);
    primitive("XeTeXmathchar", MATH_CHAR_NUM, 2);
    primitive("Umathchar", MATH_CHAR_NUM, 2);
    primitive("mathchoice", MATH_CHOICE, 0);
    primitive("multiply", MULTIPLY, 0);
    primitive("noalign", NO_ALIGN, 0);
    primitive("noboundary", NO_BOUNDARY, 0);
    primitive("noexpand", NO_EXPAND, 0);
    primitive("primitive", NO_EXPAND, 1);
    primitive("nonscript", NON_SCRIPT, 0);
    primitive("omit", OMIT, 0);
    primitive("parshape", SET_SHAPE, LOCAL_BASE + LOCAL__par_shape);
    primitive("penalty", BREAK_PENALTY, 0);
    primitive("prevgraf", SET_PREV_GRAF, 0);
    primitive("radical", RADICAL, 0);
    primitive("XeTeXradical", RADICAL, 1);
    primitive("Uradical", RADICAL, 1);
    primitive("read", READ_TO_CS, 0);
    primitive("relax", RELAX, TOO_BIG_USV);
    hash[FROZEN_RELAX].s1 = maketexstring("relax");
    eqtb[FROZEN_RELAX] = eqtb[cur_val];
    primitive("setbox", SET_BOX, 0);
    primitive("the", THE, 0);
    primitive("toks", TOKS_REGISTER, 0);
    primitive("vadjust", VADJUST, 0);
    primitive("valign", VALIGN, 0);
    primitive("vcenter", VCENTER, 0);
    primitive("vrule", VRULE, 0);
    primitive("par", PAR_END, TOO_BIG_USV);
    par_loc = cur_val;
    par_token = CS_TOKEN_FLAG + par_loc;

    primitive("input", INPUT, 0);
    primitive("endinput", INPUT, 1);

    primitive("topmark", TOP_BOT_MARK, TOP_MARK_CODE);
    primitive("firstmark", TOP_BOT_MARK, FIRST_MARK_CODE);
    primitive("botmark", TOP_BOT_MARK, BOT_MARK_CODE);
    primitive("splitfirstmark", TOP_BOT_MARK, SPLIT_FIRST_MARK_CODE);
    primitive("splitbotmark", TOP_BOT_MARK, SPLIT_BOT_MARK_CODE);

    primitive("count", REGISTER, 0);
    primitive("dimen", REGISTER, 1);
    primitive("skip", REGISTER, 2);
    primitive("muskip", REGISTER, 3);

    primitive("spacefactor", SET_AUX, HMODE);
    primitive("prevdepth", SET_AUX, VMODE);

    primitive("deadcycles", SET_PAGE_INT, 0);
    primitive("insertpenalties", SET_PAGE_INT, 1);

    primitive("wd", SET_BOX_DIMEN, WIDTH_OFFSET);
    primitive("ht", SET_BOX_DIMEN, HEIGHT_OFFSET);
    primitive("dp", SET_BOX_DIMEN, DEPTH_OFFSET);

    primitive("lastpenalty", LAST_ITEM, INT_VAL);
    primitive("lastkern", LAST_ITEM, DIMEN_VAL);
    primitive("lastskip", LAST_ITEM, GLUE_VAL);
    primitive("inputlineno", LAST_ITEM, INPUT_LINE_NO_CODE);
    primitive("badness", LAST_ITEM, BADNESS_CODE);
    primitive("pdflastxpos", LAST_ITEM, PDF_LAST_X_POS_CODE);
    primitive("pdflastypos", LAST_ITEM, PDF_LAST_Y_POS_CODE);
    primitive("elapsedtime", LAST_ITEM, ELAPSED_TIME_CODE);
    primitive("shellescape", LAST_ITEM, PDF_SHELL_ESCAPE_CODE);
    primitive("randomseed", LAST_ITEM, RANDOM_SEED_CODE);

    primitive("number", CONVERT, NUMBER_CODE);
    primitive("romannumeral", CONVERT, ROMAN_NUMERAL_CODE);
    primitive("string", CONVERT, STRING_CODE);
    primitive("meaning", CONVERT, MEANING_CODE);
    primitive("fontname", CONVERT, FONT_NAME_CODE);
    primitive("expanded", CONVERT, EXPANDED_CODE);
    primitive("leftmarginkern", CONVERT, LEFT_MARGIN_KERN_CODE);
    primitive("rightmarginkern", CONVERT, RIGHT_MARGIN_KERN_CODE);
    primitive("creationdate", CONVERT, PDF_CREATION_DATE_CODE);
    primitive("filemoddate", CONVERT, PDF_FILE_MOD_DATE_CODE);
    primitive("filesize", CONVERT, PDF_FILE_SIZE_CODE);
    primitive("mdfivesum", CONVERT, PDF_MDFIVE_SUM_CODE);
    primitive("filedump", CONVERT, PDF_FILE_DUMP_CODE);
    primitive("strcmp", CONVERT, PDF_STRCMP_CODE);
    primitive("uniformdeviate", CONVERT, UNIFORM_DEVIATE_CODE);
    primitive("normaldeviate", CONVERT, NORMAL_DEVIATE_CODE);
    primitive("jobname", CONVERT, JOB_NAME_CODE);
    primitive("Uchar", CONVERT, XETEX_UCHAR_CODE);
    primitive("Ucharcat", CONVERT, XETEX_UCHARCAT_CODE);

    primitive("if", IF_TEST, IF_CHAR_CODE);
    primitive("ifcat", IF_TEST, IF_CAT_CODE);
    primitive("ifnum", IF_TEST, IF_INT_CODE);
    primitive("ifdim", IF_TEST, IF_DIM_CODE);
    primitive("ifodd", IF_TEST, IF_ODD_CODE);
    primitive("ifvmode", IF_TEST, IF_VMODE_CODE);
    primitive("ifhmode", IF_TEST, IF_HMODE_CODE);
    primitive("ifmmode", IF_TEST, IF_MMODE_CODE);
    primitive("ifinner", IF_TEST, IF_INNER_CODE);
    primitive("ifvoid", IF_TEST, IF_VOID_CODE);
    primitive("ifhbox", IF_TEST, IF_HBOX_CODE);
    primitive("ifvbox", IF_TEST, IF_VBOX_CODE);
    primitive("ifx", IF_TEST, IFX_CODE);
    primitive("ifeof", IF_TEST, IF_EOF_CODE);
    primitive("iftrue", IF_TEST, IF_TRUE_CODE);
    primitive("iffalse", IF_TEST, IF_FALSE_CODE);
    primitive("ifcase", IF_TEST, IF_CASE_CODE);
    primitive("ifprimitive", IF_TEST, IF_PRIMITIVE_CODE);

    primitive("fi", FI_OR_ELSE, FI_CODE);
    hash[FROZEN_FI].s1 = maketexstring("fi");
    eqtb[FROZEN_FI] = eqtb[cur_val];
    primitive("or", FI_OR_ELSE, OR_CODE);
    primitive("else", FI_OR_ELSE, ELSE_CODE);

    primitive("nullfont", SET_FONT, FONT_BASE);
    hash[FROZEN_NULL_FONT].s1 = maketexstring("nullfont");
    eqtb[FROZEN_NULL_FONT] = eqtb[cur_val];

    primitive("span", TAB_MARK, SPAN_CODE);
    primitive("cr", CAR_RET, CR_CODE);
    hash[FROZEN_CR].s1 = maketexstring("cr");
    eqtb[FROZEN_CR] = eqtb[cur_val];
    primitive("crcr", CAR_RET, CR_CR_CODE);

    hash[FROZEN_END_TEMPLATE].s1 = maketexstring("endtemplate");
    hash[FROZEN_ENDV].s1 = maketexstring("endtemplate");
    eqtb[FROZEN_ENDV].b16.s1 = ENDV;
    eqtb[FROZEN_ENDV].b32.s1 = NULL_LIST;
    eqtb[FROZEN_ENDV].b16.s0 = LEVEL_ONE;
    eqtb[FROZEN_END_TEMPLATE] = eqtb[FROZEN_ENDV];
    eqtb[FROZEN_END_TEMPLATE].b16.s1 = END_TEMPLATE;

    primitive("pagegoal", SET_PAGE_DIMEN, 0);
    primitive("pagetotal", SET_PAGE_DIMEN, 1);
    primitive("pagestretch", SET_PAGE_DIMEN, 2);
    primitive("pagefilstretch", SET_PAGE_DIMEN, 3);
    primitive("pagefillstretch", SET_PAGE_DIMEN, 4);
    primitive("pagefilllstretch", SET_PAGE_DIMEN, 5);
    primitive("pageshrink", SET_PAGE_DIMEN, 6);
    primitive("pagedepth", SET_PAGE_DIMEN, 7);

    primitive("end", STOP, 0);
    primitive("dump", STOP, 1);

    primitive("hskip", HSKIP, SKIP_CODE);
    primitive("hfil", HSKIP, FIL_CODE);
    primitive("hfill", HSKIP, FILL_CODE);
    primitive("hss", HSKIP, SS_CODE);
    primitive("hfilneg", HSKIP, FIL_NEG_CODE);
    primitive("vskip", VSKIP, SKIP_CODE);
    primitive("vfil", VSKIP, FIL_CODE);
    primitive("vfill", VSKIP, FILL_CODE);
    primitive("vss", VSKIP, SS_CODE);
    primitive("vfilneg", VSKIP, FIL_NEG_CODE);
    primitive("mskip", MSKIP, MSKIP_CODE);

    primitive("kern", KERN, EXPLICIT);
    primitive("mkern", MKERN, MU_GLUE);
    primitive("moveleft", HMOVE, 1);
    primitive("moveright", HMOVE, 0);
    primitive("raise", VMOVE, 1);
    primitive("lower", VMOVE, 0);

    primitive("box", MAKE_BOX, BOX_CODE);
    primitive("copy", MAKE_BOX, COPY_CODE);
    primitive("lastbox", MAKE_BOX, LAST_BOX_CODE);
    primitive("vsplit", MAKE_BOX, VSPLIT_CODE);
    primitive("vtop", MAKE_BOX, VTOP_CODE);
    primitive("vbox", MAKE_BOX, VTOP_CODE + 1);
    primitive("hbox", MAKE_BOX, VTOP_CODE + 104);

    primitive("shipout", LEADER_SHIP, A_LEADERS - 1);
    primitive("leaders", LEADER_SHIP, A_LEADERS);
    primitive("cleaders", LEADER_SHIP, C_LEADERS);
    primitive("xleaders", LEADER_SHIP, X_LEADERS);

    primitive("indent", START_PAR, 1);
    primitive("noindent", START_PAR, 0);
    primitive("unpenalty", REMOVE_ITEM, PENALTY_NODE);
    primitive("unkern", REMOVE_ITEM, KERN_NODE);
    primitive("unskip", REMOVE_ITEM, GLUE_NODE);
    primitive("unhbox", UN_HBOX, BOX_CODE);
    primitive("unhcopy", UN_HBOX, COPY_CODE);
    primitive("unvbox", UN_VBOX, BOX_CODE);
    primitive("unvcopy", UN_VBOX, COPY_CODE);

    primitive("-", DISCRETIONARY, 1);
    primitive("discretionary", DISCRETIONARY, 0);

    primitive("eqno", EQ_NO, 0);
    primitive("leqno", EQ_NO, 1);

    primitive("mathord", MATH_COMP, ORD_NOAD);
    primitive("mathop", MATH_COMP, OP_NOAD);
    primitive("mathbin", MATH_COMP, BIN_NOAD);
    primitive("mathrel", MATH_COMP, REL_NOAD);
    primitive("mathopen", MATH_COMP, OPEN_NOAD);
    primitive("mathclose", MATH_COMP, CLOSE_NOAD);
    primitive("mathpunct", MATH_COMP, PUNCT_NOAD);
    primitive("mathinner", MATH_COMP, INNER_NOAD);
    primitive("underline", MATH_COMP, UNDER_NOAD);
    primitive("overline", MATH_COMP, OVER_NOAD);

    primitive("displaylimits", LIMIT_SWITCH, NORMAL);
    primitive("limits", LIMIT_SWITCH, LIMITS);
    primitive("nolimits", LIMIT_SWITCH, NO_LIMITS);

    primitive("displaystyle", MATH_STYLE, DISPLAY_STYLE);
    primitive("textstyle", MATH_STYLE, TEXT_STYLE);
    primitive("scriptstyle", MATH_STYLE, SCRIPT_STYLE);
    primitive("scriptscriptstyle", MATH_STYLE, SCRIPT_SCRIPT_STYLE);

    primitive("above", ABOVE, ABOVE_CODE);
    primitive("over", ABOVE, OVER_CODE);
    primitive("atop", ABOVE, ATOP_CODE);
    primitive("abovewithdelims", ABOVE, DELIMITED_CODE + 0);
    primitive("overwithdelims", ABOVE, DELIMITED_CODE + 1);
    primitive("atopwithdelims", ABOVE, DELIMITED_CODE + 2);

    primitive("left", LEFT_RIGHT, LEFT_NOAD);
    primitive("right", LEFT_RIGHT, RIGHT_NOAD);
    hash[FROZEN_RIGHT].s1 = maketexstring("right");
    eqtb[FROZEN_RIGHT] = eqtb[cur_val];

    primitive("long", PREFIX, 1);
    primitive("outer", PREFIX, 2);
    primitive("global", PREFIX, 4);
    primitive("def", DEF, 0);
    primitive("gdef", DEF, 1);
    primitive("edef", DEF, 2);
    primitive("xdef", DEF, 3);
    primitive("let", LET, NORMAL);
    primitive("futurelet", LET, NORMAL + 1);

    primitive("chardef", SHORTHAND_DEF, CHAR_DEF_CODE);
    primitive("mathchardef", SHORTHAND_DEF, MATH_CHAR_DEF_CODE);
    primitive("XeTeXmathcharnumdef", SHORTHAND_DEF, XETEX_MATH_CHAR_NUM_DEF_CODE);
    primitive("Umathcharnumdef", SHORTHAND_DEF, XETEX_MATH_CHAR_NUM_DEF_CODE);
    primitive("XeTeXmathchardef", SHORTHAND_DEF, XETEX_MATH_CHAR_DEF_CODE);
    primitive("Umathchardef", SHORTHAND_DEF, XETEX_MATH_CHAR_DEF_CODE);
    primitive("countdef", SHORTHAND_DEF, COUNT_DEF_CODE);
    primitive("dimendef", SHORTHAND_DEF, DIMEN_DEF_CODE);
    primitive("skipdef", SHORTHAND_DEF, SKIP_DEF_CODE);
    primitive("muskipdef", SHORTHAND_DEF, MU_SKIP_DEF_CODE);
    primitive("toksdef", SHORTHAND_DEF, TOKS_DEF_CODE);

    primitive("catcode", DEF_CODE, CAT_CODE_BASE);
    primitive("mathcode", DEF_CODE, MATH_CODE_BASE);
    primitive("XeTeXmathcodenum", XETEX_DEF_CODE, MATH_CODE_BASE);
    primitive("Umathcodenum", XETEX_DEF_CODE, MATH_CODE_BASE);
    primitive("XeTeXmathcode", XETEX_DEF_CODE, MATH_CODE_BASE + 1);
    primitive("Umathcode", XETEX_DEF_CODE, MATH_CODE_BASE + 1);
    primitive("lccode", DEF_CODE, LC_CODE_BASE);
    primitive("uccode", DEF_CODE, UC_CODE_BASE);
    primitive("sfcode", DEF_CODE, SF_CODE_BASE);
    primitive("XeTeXcharclass", XETEX_DEF_CODE, SF_CODE_BASE);
    primitive("delcode", DEF_CODE, DEL_CODE_BASE);
    primitive("XeTeXdelcodenum", XETEX_DEF_CODE, DEL_CODE_BASE);
    primitive("Udelcodenum", XETEX_DEF_CODE, DEL_CODE_BASE);
    primitive("XeTeXdelcode", XETEX_DEF_CODE, DEL_CODE_BASE + 1);
    primitive("Udelcode", XETEX_DEF_CODE, DEL_CODE_BASE + 1);

    primitive("textfont", DEF_FAMILY, MATH_FONT_BASE + TEXT_SIZE);
    primitive("scriptfont", DEF_FAMILY, MATH_FONT_BASE + SCRIPT_SIZE);
    primitive("scriptscriptfont", DEF_FAMILY, MATH_FONT_BASE + SCRIPT_SCRIPT_SIZE);

    primitive("hyphenation", HYPH_DATA, 0);
    primitive("patterns", HYPH_DATA, 1);

    primitive("hyphenchar", ASSIGN_FONT_INT, 0);
    primitive("skewchar", ASSIGN_FONT_INT, 1);
    primitive("lpcode", ASSIGN_FONT_INT, 2);
    primitive("rpcode", ASSIGN_FONT_INT, 3);

    primitive("batchmode", SET_INTERACTION, BATCH_MODE);
    primitive("nonstopmode", SET_INTERACTION, NONSTOP_MODE);
    primitive("scrollmode", SET_INTERACTION, SCROLL_MODE);
    primitive("errorstopmode", SET_INTERACTION, ERROR_STOP_MODE);

    primitive("openin", IN_STREAM, 1);
    primitive("closein", IN_STREAM, 0);
    primitive("message", MESSAGE, 0);
    primitive("errmessage", MESSAGE, 1);
    primitive("lowercase", CASE_SHIFT, LC_CODE_BASE);
    primitive("uppercase", CASE_SHIFT, UC_CODE_BASE);

    primitive("show", XRAY, SHOW_CODE);
    primitive("showbox", XRAY, SHOW_BOX_CODE);
    primitive("showthe", XRAY, SHOW_THE_CODE);
    primitive("showlists", XRAY, SHOW_LISTS);

    primitive("openout", EXTENSION, OPEN_NODE);
    primitive("write", EXTENSION, WRITE_NODE);
    write_loc = cur_val;
    primitive("closeout", EXTENSION, CLOSE_NODE);
    primitive("special", EXTENSION, SPECIAL_NODE);
    hash[FROZEN_SPECIAL].s1 = maketexstring("special");
    eqtb[FROZEN_SPECIAL] = eqtb[cur_val];
    primitive("immediate", EXTENSION, IMMEDIATE_CODE);
    primitive("setlanguage", EXTENSION, SET_LANGUAGE_CODE);
    primitive("resettimer", EXTENSION, RESET_TIMER_CODE);
    primitive("setrandomseed", EXTENSION, SET_RANDOM_SEED_CODE);

    primitive("synctex", ASSIGN_INT, INT_BASE + INT_PAR__synctex);

    no_new_control_sequence = true;
}


static void
get_strings_started(void)
{
    pool_ptr = 0;
    str_ptr = 0;
    str_start[0] = 0;
    str_ptr = TOO_BIG_CHAR;

    if (load_pool_strings(pool_size - string_vacancies) == 0)
        _tt_abort ("must increase pool_size");
}
/*:1001*/

void
tt_cleanup(void) {
    /*
        Cleanup of all intermediate buffers.
        Conceptually, final_cleanup() and close_files_and_terminate() also
        belong here, but that requires a more thorough refactor as presently
        it would result in a segfault.
    */

    pdf_files_close();
    free(TEX_format_default);
    free(font_used);
    deinitialize_shipout_variables();

    destroy_font_manager();

    for (int font_k = 0; font_k < font_max; font_k++) {
        if (font_layout_engine[font_k] != NULL) {
            release_font_engine(font_layout_engine[font_k], font_area[font_k]);
            font_layout_engine[font_k] = NULL;
        }
    }

    // Free the big allocated arrays
    free(buffer);
    free(nest);
    free(save_stack);
    free(input_stack);
    free(input_file);
    free(line_stack);
    free(eof_seen);
    free(grp_stack);
    free(if_stack);
    free(source_filename_stack);
    free(full_source_filename_stack);
    free(param_stack);
    free(hyph_word);
    free(hyph_list);
    free(hyph_link);

    // initialize_more_variables @ 3277
    free(native_text);

    // Free arrays allocated in load_fmt_file
    free(yhash);
    free(eqtb);
    free(mem);
    free(str_start);
    free(str_pool);
    free(font_info);

    free(font_mapping);
    free(font_layout_engine);
    free(font_flags);
    free(font_letter_space);
    free(font_check);
    free(font_size);
    free(font_dsize);
    free(font_params);
    free(font_name);
    free(font_area);
    free(font_bc);
    free(font_ec);
    free(font_glue);
    free(hyphen_char);
    free(skew_char);
    free(bchar_label);
    free(font_bchar);
    free(font_false_bchar);
    free(char_base);
    free(width_base);
    free(height_base);
    free(depth_base);
    free(italic_base);
    free(lig_kern_base);
    free(kern_base);
    free(exten_base);
    free(param_base);

    trie_trl = mfree(trie_trl);
    trie_tro = mfree(trie_tro);
    trie_trc = mfree(trie_trc);
}

tt_history_t
xetex_main(const char *dump_name, const char *input_file_name, time_t build_date)
{
    int32_t font_k;

    /* Miscellaneous initializations that were mostly originally done in the
     * main() driver routines. */

    /* Get our stdout handle */

    rust_stdout = ttstub_output_open_stdout ();

    size_t len = strlen (dump_name);
    TEX_format_default = xmalloc (len + 1);
    strcpy (TEX_format_default, dump_name);
    format_default_length = len;

    /* Not sure why these get custom initializations. */

    if (file_line_error_style_p < 0)
        file_line_error_style_p = 0;

    /* These various parameters were configurable in web2c TeX. We don't
     * bother to allow that. */

    pool_size = 6250000L;
    string_vacancies = 90000L;
    pool_free = 47500L;
    max_strings = 565536L;
    strings_free = 100;
    font_mem_size = 8000000L;
    font_max = 9000;
    trie_size = 1000000L;
    hyph_size = 8191;
    buf_size = 200000L;
    nest_size = 500;
    max_in_open = 15;
    param_size = 10000;
    save_size = 80000L;
    stack_size = 5000;
    error_line = 79;
    half_error_line = 50;
    max_print_line = 79;
    hash_extra = 600000L;
    expand_depth = 10000;

    /* Allocate many of our big arrays. */

    buffer = xmalloc_array(UnicodeScalar, buf_size);
    nest = xmalloc_array(list_state_record, nest_size);
    save_stack = xmalloc_array(memory_word, save_size);
    input_stack = xmalloc_array(input_state_t, stack_size);
    input_file = xmalloc_array(UFILE *, max_in_open);
    line_stack = xmalloc_array(int32_t, max_in_open);
    eof_seen = xmalloc_array(bool, max_in_open);
    grp_stack = xmalloc_array(save_pointer, max_in_open);
    if_stack = xmalloc_array(int32_t, max_in_open);
    source_filename_stack = xmalloc_array(str_number, max_in_open);
    full_source_filename_stack = xmalloc_array(str_number, max_in_open);
    param_stack = xmalloc_array(int32_t, param_size);
    hyph_word = xmalloc_array(str_number, hyph_size);
    hyph_list = xmalloc_array(int32_t, hyph_size);
    hyph_link = xmalloc_array(hyph_pointer, hyph_size);

    /* First bit of initex handling: more allocations. */

    if (in_initex_mode) {
        mem = xmalloc_array(memory_word, MEM_TOP + 1);
        eqtb_top = EQTB_SIZE + hash_extra;

        if (hash_extra == 0)
            hash_top = UNDEFINED_CONTROL_SEQUENCE;
        else
            hash_top = eqtb_top;

        yhash = xmalloc_array(b32x2, 1 + hash_top - hash_offset);
        hash = yhash - hash_offset;
        hash[HASH_BASE].s0 = 0;
        hash[HASH_BASE].s1 = 0;

        for (hash_used = HASH_BASE + 1; hash_used <= hash_top; hash_used++)
            hash[hash_used] = hash[HASH_BASE];

        eqtb = xcalloc_array(memory_word, eqtb_top);
        str_start = xmalloc_array(pool_pointer, max_strings);
        str_pool = xmalloc_array(packed_UTF16_code, pool_size);
        font_info = xmalloc_array(memory_word, font_mem_size);
    }

    /* Sanity-check various invariants. */

    history = HISTORY_FATAL_ERROR;
    bad = 0;

    if (half_error_line < 30 || half_error_line > error_line - 15)
        bad = 1;
    if (max_print_line < 60)
        bad = 2;
    if (1100 > MEM_TOP)
        bad = 4;
    if (HASH_PRIME > HASH_SIZE)
        bad = 5;
    if (max_in_open >= 128)
        bad = 6;
    if (MEM_TOP < 267)
        bad = 7;
    if (MIN_HALFWORD > 0)
        bad = 12;
    if (MAX_FONT_MAX < MIN_HALFWORD || MAX_FONT_MAX > MAX_HALFWORD)
        bad = 15;
    if (font_max > FONT_BASE + 9000)
        bad = 16;
    if (save_size > MAX_HALFWORD || max_strings > MAX_HALFWORD)
        bad = 17;
    if (buf_size > MAX_HALFWORD)
        bad = 18;
    if (CS_TOKEN_FLAG + EQTB_SIZE + hash_extra > MAX_HALFWORD)
        bad = 21;
    if (hash_offset < 0 || hash_offset > HASH_BASE)
        bad = 42;
    if (format_default_length > INT32_MAX)
        bad = 31;
    if (2 * MAX_HALFWORD < MEM_TOP)
        bad = 41;

    if (bad > 0)
        _tt_abort ("failed internal consistency check #%d", bad);

    /* OK, ready to keep on initializing. */

    initialize_more_variables();

    if (in_initex_mode) {
        get_strings_started();
        initialize_more_initex_variables();
        initialize_primitives();
        init_str_ptr = str_ptr;
        init_pool_ptr = pool_ptr;
    }

    /*55:*/
    initialize_math_variables();
    initialize_pagebuilder_variables();
    initialize_shipout_variables();

    get_seconds_and_micros(&epochseconds, &microseconds);
    init_start_time(build_date);

    selector = SELECTOR_TERM_ONLY;
    tally = 0;
    term_offset = 0;
    file_offset = 0;
    job_name = 0;
    name_in_progress = false;
    log_opened = false;

    if (semantic_pagination_enabled)
        output_file_extension = ".spx";
    else
        output_file_extension = ".xdv";

    input_ptr = 0;
    max_in_stack = 0;
    source_filename_stack[0] = 0;
    full_source_filename_stack[0] = 0;
    in_open = 0;
    open_parens = 0;
    max_buf_stack = 0;
    grp_stack[0] = 0;
    if_stack[0] = TEX_NULL;
    param_ptr = 0;
    max_param_stack = 0;
    used_tectonic_coda_tokens = false;
    gave_char_warning_help = false;

    memset(buffer, 0, buf_size * sizeof(buffer[0]));
    first = 0;

    scanner_status = NORMAL;
    warning_index = TEX_NULL;
    first = 1;
    cur_input.state = NEW_LINE;
    cur_input.start = 1;
    cur_input.index = 0;
    line = 0;
    cur_input.name = 0;
    force_eof = false;
    align_state = 1000000L;

    init_io();

    if (in_initex_mode) {
        no_new_control_sequence = false;

        primitive("XeTeXpicfile", EXTENSION, PIC_FILE_CODE);
        primitive("XeTeXpdffile", EXTENSION, PDF_FILE_CODE);
        primitive("XeTeXglyph", EXTENSION, GLYPH_CODE);
        primitive("XeTeXlinebreaklocale", EXTENSION, XETEX_LINEBREAK_LOCALE_EXTENSION_CODE);
        primitive("pdfsavepos", EXTENSION, PDF_SAVE_POS_NODE);

        primitive("lastnodetype", LAST_ITEM, LAST_NODE_TYPE_CODE);
        primitive("eTeXversion", LAST_ITEM, ETEX_VERSION_CODE);

        primitive("eTeXrevision", CONVERT, ETEX_REVISION_CODE);

        primitive("XeTeXversion", LAST_ITEM, XETEX_VERSION_CODE);

        primitive("XeTeXrevision", CONVERT, XETEX_REVISION_CODE);

        primitive("XeTeXcountglyphs", LAST_ITEM, XETEX_COUNT_GLYPHS_CODE);
        primitive("XeTeXcountvariations", LAST_ITEM, XETEX_COUNT_VARIATIONS_CODE);
        primitive("XeTeXvariation", LAST_ITEM, XETEX_VARIATION_CODE);
        primitive("XeTeXfindvariationbyname", LAST_ITEM, XETEX_FIND_VARIATION_BY_NAME_CODE);
        primitive("XeTeXvariationmin", LAST_ITEM, XETEX_VARIATION_MIN_CODE);
        primitive("XeTeXvariationmax", LAST_ITEM, XETEX_VARIATION_MAX_CODE);
        primitive("XeTeXvariationdefault", LAST_ITEM, XETEX_VARIATION_DEFAULT_CODE);
        primitive("XeTeXcountfeatures", LAST_ITEM, XETEX_COUNT_FEATURES_CODE);
        primitive("XeTeXfeaturecode", LAST_ITEM, XETEX_FEATURE_CODE_CODE);
        primitive("XeTeXfindfeaturebyname", LAST_ITEM, XETEX_FIND_FEATURE_BY_NAME_CODE);
        primitive("XeTeXisexclusivefeature", LAST_ITEM, XETEX_IS_EXCLUSIVE_FEATURE_CODE);
        primitive("XeTeXcountselectors", LAST_ITEM, XETEX_COUNT_SELECTORS_CODE);
        primitive("XeTeXselectorcode", LAST_ITEM, XETEX_SELECTOR_CODE_CODE);
        primitive("XeTeXfindselectorbyname", LAST_ITEM, XETEX_FIND_SELECTOR_BY_NAME_CODE);
        primitive("XeTeXisdefaultselector", LAST_ITEM, XETEX_IS_DEFAULT_SELECTOR_CODE);

        primitive("XeTeXvariationname", CONVERT, XETEX_VARIATION_NAME_CODE);
        primitive("XeTeXfeaturename", CONVERT, XeTeX_feature_name);
        primitive("XeTeXselectorname", CONVERT, XeTeX_selector_name);

        primitive("XeTeXOTcountscripts", LAST_ITEM, XETEX_OT_COUNT_SCRIPTS_CODE);
        primitive("XeTeXOTcountlanguages", LAST_ITEM, XETEX_OT_COUNT_LANGUAGES_CODE);
        primitive("XeTeXOTcountfeatures", LAST_ITEM, XETEX_OT_COUNT_FEATURES_CODE);
        primitive("XeTeXOTscripttag", LAST_ITEM, XETEX_OT_SCRIPT_CODE);
        primitive("XeTeXOTlanguagetag", LAST_ITEM, XETEX_OT_LANGUAGE_CODE);
        primitive("XeTeXOTfeaturetag", LAST_ITEM, XETEX_OT_FEATURE_CODE);
        primitive("XeTeXcharglyph", LAST_ITEM, XETEX_MAP_CHAR_TO_GLYPH_CODE);
        primitive("XeTeXglyphindex", LAST_ITEM, XETEX_GLYPH_INDEX_CODE);
        primitive("XeTeXglyphbounds", LAST_ITEM, XETEX_GLYPH_BOUNDS_CODE);

        primitive("XeTeXglyphname", CONVERT, XETEX_GLYPH_NAME_CODE);

        primitive("XeTeXfonttype", LAST_ITEM, XETEX_FONT_TYPE_CODE);
        primitive("XeTeXfirstfontchar", LAST_ITEM, XETEX_FIRST_CHAR_CODE);
        primitive("XeTeXlastfontchar", LAST_ITEM, XETEX_LAST_CHAR_CODE);
        primitive("XeTeXpdfpagecount", LAST_ITEM, XETEX_PDF_PAGE_COUNT_CODE);
        /* everyeof moved to be with other assign_toks */

        primitive("tracingassigns", ASSIGN_INT, INT_BASE + INT_PAR__tracing_assigns);
        primitive("tracinggroups", ASSIGN_INT, INT_BASE + INT_PAR__tracing_groups);
        primitive("tracingifs", ASSIGN_INT, INT_BASE + INT_PAR__tracing_ifs);
        primitive("tracingscantokens", ASSIGN_INT, INT_BASE + INT_PAR__tracing_scan_tokens);
        primitive("tracingnesting", ASSIGN_INT, INT_BASE + INT_PAR__tracing_nesting);
        primitive("predisplaydirection", ASSIGN_INT, INT_BASE + INT_PAR__pre_display_correction);
        primitive("lastlinefit", ASSIGN_INT, INT_BASE + INT_PAR__last_line_fit);
        primitive("savingvdiscards", ASSIGN_INT, INT_BASE + INT_PAR__saving_vdiscards);
        primitive("savinghyphcodes", ASSIGN_INT, INT_BASE + INT_PAR__saving_hyphs);

        primitive("currentgrouplevel", LAST_ITEM, CURRENT_GROUP_LEVEL_CODE);
        primitive("currentgrouptype", LAST_ITEM, CURRENT_GROUP_TYPE_CODE);
        primitive("currentiflevel", LAST_ITEM, CURRENT_IF_LEVEL_CODE);
        primitive("currentiftype", LAST_ITEM, CURRENT_IF_TYPE_CODE);
        primitive("currentifbranch", LAST_ITEM, CURRENT_IF_BRANCH_CODE);
        primitive("fontcharwd", LAST_ITEM, FONT_CHAR_WD_CODE);
        primitive("fontcharht", LAST_ITEM, FONT_CHAR_HT_CODE);
        primitive("fontchardp", LAST_ITEM, FONT_CHAR_DP_CODE);
        primitive("fontcharic", LAST_ITEM, FONT_CHAR_IC_CODE);
        primitive("parshapelength", LAST_ITEM, PAR_SHAPE_LENGTH_CODE);
        primitive("parshapeindent", LAST_ITEM, PAR_SHAPE_INDENT_CODE);
        primitive("parshapedimen", LAST_ITEM, PAR_SHAPE_DIMEN_CODE);

        primitive("showgroups", XRAY, SHOW_GROUPS);
        primitive("showtokens", XRAY, SHOW_TOKENS);

        primitive("unexpanded", THE, 1);
        primitive("detokenize", THE, SHOW_TOKENS);

        primitive("showifs", XRAY, SHOW_IFS);

        primitive("interactionmode", SET_PAGE_INT, 2);

        primitive("middle", LEFT_RIGHT, 1);

        primitive("suppressfontnotfounderror", ASSIGN_INT, INT_BASE + INT_PAR__suppress_fontnotfound_error);

        primitive("TeXXeTstate", ASSIGN_INT, INT_BASE + INT_PAR__texxet);
        primitive("XeTeXupwardsmode", ASSIGN_INT, INT_BASE + INT_PAR__xetex_upwards);
        primitive("XeTeXuseglyphmetrics", ASSIGN_INT, INT_BASE + INT_PAR__xetex_use_glyph_metrics);
        primitive("XeTeXinterchartokenstate", ASSIGN_INT, INT_BASE + INT_PAR__xetex_inter_char_tokens);
        primitive("XeTeXdashbreakstate", ASSIGN_INT, INT_BASE + INT_PAR__xetex_dash_break);
        primitive("XeTeXinputnormalization", ASSIGN_INT, INT_BASE + INT_PAR__xetex_input_normalization);
        primitive("XeTeXtracingfonts", ASSIGN_INT, INT_BASE + INT_PAR__xetex_tracing_fonts);
        primitive("XeTeXinterwordspaceshaping", ASSIGN_INT, INT_BASE + INT_PAR__xetex_interword_space_shaping);
        primitive("XeTeXgenerateactualtext", ASSIGN_INT, INT_BASE + INT_PAR__xetex_generate_actual_text);
        primitive("XeTeXhyphenatablelength", ASSIGN_INT, INT_BASE + INT_PAR__xetex_hyphenatable_length);
        primitive("pdfoutput", ASSIGN_INT, INT_BASE + INT_PAR__pdfoutput);

        primitive("XeTeXinputencoding", EXTENSION, XETEX_INPUT_ENCODING_EXTENSION_CODE);
        primitive("XeTeXdefaultencoding", EXTENSION, XETEX_DEFAULT_ENCODING_EXTENSION_CODE);

        primitive("beginL", VALIGN, BEGIN_L_CODE);
        primitive("endL", VALIGN, END_L_CODE);
        primitive("beginR", VALIGN, BEGIN_R_CODE);
        primitive("endR", VALIGN, END_R_CODE);

        primitive("scantokens", INPUT, 2);
        primitive("readline", READ_TO_CS, 1);
        primitive("unless", EXPAND_AFTER, 1);

        primitive("ifdefined", IF_TEST, IF_DEF_CODE);
        primitive("ifcsname", IF_TEST, IF_CS_CODE);
        primitive("iffontchar", IF_TEST, IF_FONT_CHAR_CODE);
        primitive("ifincsname", IF_TEST, IF_IN_CSNAME_CODE);

        primitive("protected", PREFIX, 8);

        primitive("numexpr", LAST_ITEM, ETEX_EXPR + 0);
        primitive("dimexpr", LAST_ITEM, ETEX_EXPR + 1);
        primitive("glueexpr", LAST_ITEM, ETEX_EXPR + 2);
        primitive("muexpr", LAST_ITEM, ETEX_EXPR + 3);
        primitive("gluestretchorder", LAST_ITEM, GLUE_STRETCH_ORDER_CODE);
        primitive("glueshrinkorder", LAST_ITEM, GLUE_SHRINK_ORDER_CODE);
        primitive("gluestretch", LAST_ITEM, GLUE_STRETCH_CODE);
        primitive("glueshrink", LAST_ITEM, GLUE_SHRINK_CODE);
        primitive("mutoglue", LAST_ITEM, MU_TO_GLUE_CODE);
        primitive("gluetomu", LAST_ITEM, GLUE_TO_MU_CODE);

        primitive("marks", MARK, 5);
        primitive("topmarks", TOP_BOT_MARK, TOP_MARK_CODE + 5);
        primitive("firstmarks", TOP_BOT_MARK, FIRST_MARK_CODE + 5);
        primitive("botmarks", TOP_BOT_MARK, BOT_MARK_CODE + 5);
        primitive("splitfirstmarks", TOP_BOT_MARK, SPLIT_FIRST_MARK_CODE + 5);
        primitive("splitbotmarks", TOP_BOT_MARK, SPLIT_BOT_MARK_CODE + 5);

        primitive("pagediscards", UN_VBOX, LAST_BOX_CODE);
        primitive("splitdiscards", UN_VBOX, VSPLIT_CODE);

        primitive("interlinepenalties", SET_SHAPE, INTER_LINE_PENALTIES_LOC);
        primitive("clubpenalties", SET_SHAPE, CLUB_PENALTIES_LOC);
        primitive("widowpenalties", SET_SHAPE, WIDOW_PENALTIES_LOC);
        primitive("displaywidowpenalties", SET_SHAPE, DISPLAY_WIDOW_PENALTIES_LOC);

        max_reg_num = 32767;
        max_reg_help_line = "A register number must be between 0 and 32767.";
    }

    no_new_control_sequence = true;

    if (!in_initex_mode) {
        if (!load_fmt_file())
            return history;
    }

    if (INTPAR(end_line_char) < 0 || INTPAR(end_line_char) > BIGGEST_CHAR)
        cur_input.limit--;
    else
        buffer[cur_input.limit] = INTPAR(end_line_char);

    if (in_initex_mode) {
        /* TeX initializes with the real date and time, but for format file
         * reproducibility we do this: */
        INTPAR(time) = 0;
        INTPAR(day) = 0;
        INTPAR(month) = 0;
        INTPAR(year) = 0;
    } else {
        get_date_and_time(build_date,
                          &(INTPAR(time)),
                          &(INTPAR(day)),
                          &(INTPAR(month)),
                          &(INTPAR(year)));
    }

    if (trie_not_ready) {
        trie_trl = xmalloc_array(trie_pointer, trie_size);
        trie_tro = xmalloc_array(trie_pointer, trie_size);
        trie_trc = xmalloc_array(uint16_t, trie_size);
        trie_c = xmalloc_array(packed_UTF16_code, trie_size);
        trie_o = xmalloc_array(trie_opcode, trie_size);
        trie_l = xmalloc_array(trie_pointer, trie_size);
        trie_r = xmalloc_array(trie_pointer, trie_size);
        trie_hash = xmalloc_array(trie_pointer, trie_size);
        trie_taken = xmalloc_array(bool, trie_size);
        trie_l[0] = 0;
        trie_c[0] = 0;
        trie_ptr = 0;
        trie_r[0] = 0;
        hyph_start = 0;
        font_mapping = xcalloc_array(void *, font_max);
        font_layout_engine = xcalloc_array(void *, font_max);
        font_flags = xcalloc_array(char, font_max);
        font_letter_space = xcalloc_array(scaled_t, font_max);
        font_check = xcalloc_array(b16x4, font_max);
        font_size = xcalloc_array(scaled_t, font_max);
        font_dsize = xcalloc_array(scaled_t, font_max);
        font_params = xcalloc_array(font_index, font_max);
        font_name = xcalloc_array(str_number, font_max);
        font_area = xcalloc_array(str_number, font_max);
        font_bc = xcalloc_array(UTF16_code, font_max);
        font_ec = xcalloc_array(UTF16_code, font_max);
        font_glue = xcalloc_array(int32_t, font_max);
        hyphen_char = xcalloc_array(int32_t, font_max);
        skew_char = xcalloc_array(int32_t, font_max);
        bchar_label = xcalloc_array(font_index, font_max);
        font_bchar = xcalloc_array(nine_bits, font_max);
        font_false_bchar = xcalloc_array(nine_bits, font_max);
        char_base = xcalloc_array(int32_t, font_max);
        width_base = xcalloc_array(int32_t, font_max);
        height_base = xcalloc_array(int32_t, font_max);
        depth_base = xcalloc_array(int32_t, font_max);
        italic_base = xcalloc_array(int32_t, font_max);
        lig_kern_base = xcalloc_array(int32_t, font_max);
        kern_base = xcalloc_array(int32_t, font_max);
        exten_base = xcalloc_array(int32_t, font_max);
        param_base = xcalloc_array(int32_t, font_max);
        font_ptr = FONT_BASE;
        fmem_ptr = 7;
        font_name[FONT_BASE] = maketexstring("nullfont");
        font_area[FONT_BASE] = EMPTY_STRING;
        hyphen_char[FONT_BASE] = '-';
        skew_char[FONT_BASE] = -1;
        bchar_label[FONT_BASE] = NON_ADDRESS;
        font_bchar[FONT_BASE] = TOO_BIG_CHAR;
        font_false_bchar[FONT_BASE] = TOO_BIG_CHAR;
        font_bc[FONT_BASE] = 1;
        font_ec[FONT_BASE] = 0;
        font_size[FONT_BASE] = 0;
        font_dsize[FONT_BASE] = 0;
        char_base[FONT_BASE] = 0;
        width_base[FONT_BASE] = 0;
        height_base[FONT_BASE] = 0;
        depth_base[FONT_BASE] = 0;
        italic_base[FONT_BASE] = 0;
        lig_kern_base[FONT_BASE] = 0;
        kern_base[FONT_BASE] = 0;
        exten_base[FONT_BASE] = 0;
        font_glue[FONT_BASE] = TEX_NULL;
        font_params[FONT_BASE] = 7;
        font_mapping[FONT_BASE] = 0;
        param_base[FONT_BASE] = -1;

        for (font_k = 0; font_k <= 6; font_k++)
            font_info[font_k].b32.s1 = 0;
    }

    font_used = xmalloc_array(bool, font_max);
    for (font_k = 0; font_k <= font_max; font_k++)
        font_used[font_k] = false;

    random_seed = (microseconds * 1000) + (epochseconds % 1000000L);
    init_randoms(random_seed);

    if (interaction == BATCH_MODE)
        selector = SELECTOR_NO_PRINT;
    else
        selector = SELECTOR_TERM_ONLY; /*:79*/

    if (semantic_pagination_enabled)
        INTPAR(xetex_generate_actual_text) = 1;

    pdf_files_init();
    synctex_init_command();
    start_input(input_file_name);
    history = HISTORY_SPOTLESS;
    main_control();
    final_cleanup();
    close_files_and_terminate();

    tt_cleanup();

    return history;
}

tt_history_t
tt_engine_xetex_main(ttbc_state_t *api, const char *dump_name, const char *input_file_name, time_t build_date)
{
    History rv;

    if (setjmp(*ttbc_global_engine_enter(api))) {
        ttbc_global_engine_exit();
        return HISTORY_FATAL_ERROR;
    }

    rv = xetex_main(dump_name, input_file_name, build_date);
    ttbc_global_engine_exit();
    return rv;
}