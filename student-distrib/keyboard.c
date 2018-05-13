/* Keyboard Driver */

/* Files Imported */
#include "keyboard.h"
#include "system_calls.h"

/* Inspired by OSDever */

/* Global variables initialization to remove warnings */
volatile int shift_flag = 0;
volatile int alt_flag = 0;
volatile int caps_flag = 0;
volatile int ctrl_flag = 0;
//volatile int first = 1;

int count = 0;      // for moving cursor to the right
int minus = 0;
int max_h_index = -1;

char scancode_regular[MAX_SIZE] = 
    {
        0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
      '9', '0', '-', '=', '\b', /* Backspace */
      '\t',         /* Tab */
      'q', 'w', 'e', 'r',   /* 19 */
      't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
        0,          /* 29   - Control */
      'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
     '\'', '`',   0,        /* Left shift */
     '\\', 'z', 'x', 'c', 'v', 'b', 'n',            /* 49 */
      'm', ',', '.', '/',   0,              /* Right shift */
      '*',
        0,  /* Alt */
      ' ',  /* Space bar */
        0,  /* Caps lock */
        0,  /* 59 - F1 key ... > */
        0,   0,   0,   0,   0,   0,   0,   0,
        0,  /* < ... F10 */
        0,  /* 69 - Num lock*/
        0,  /* Scroll Lock */
        0,  /* Home key */
        0,  /* Up Arrow */
        0,  /* Page Up */
      '-',
        0,  /* Left Arrow */
        0,
        0,  /* Right Arrow */
      '+',
        0,  /* 79 - End key*/
        0,  /* Down Arrow */
        0,  /* Page Down */
        0,  /* Insert Key */
        0,  /* Delete Key */
        0,   0,   0,
        0,  /* F11 Key */
        0,  /* F12 Key */
        0,  /* All other keys are undefined */
    };

 char scancode_shift[MAX_SIZE] =   
    {
        0,  27, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
      '(', ')', '_', '+', '\b', /* Backspace */
      '\t',         /* Tab */
      'Q', 'W', 'E', 'R',   /* 19 */
      'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', /* Enter key */
        0,          /* 29   - Control */
      'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', /* 39 */
     '\"', '~',   0,        /* Left shift */
     '|', 'Z', 'X', 'C', 'V', 'B', 'N',         /* 49 */
      'M', '<', '>', '?',   0,              /* Right shift */
      '*',
        0,  /* Alt */
      ' ',  /* Space bar */
        0,  /* Caps lock */
        0,  /* 59 - F1 key ... > */
        0,   0,   0,   0,   0,   0,   0,   0,
        0,  /* < ... F10 */
        0,  /* 69 - Num lock*/
        0,  /* Scroll Lock */
        0,  /* Home key */
        0,  /* Up Arrow */
        0,  /* Page Up */
      '-',
        0,  /* Left Arrow */
        0,
        0,  /* Right Arrow */
      '+', 
        0,  /* 79 - End key*/
        0,  /* Down Arrow */
        0,  /* Page Down */
        0,  /* Insert Key */
        0,  /* Delete Key */
        0,   0,   0,
        0,  /* F11 Key */
        0,  /* F12 Key */
        0,  /* All other keys are undefined */
    };

char scancode_caps[MAX_SIZE] =
    {
        0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
      '9', '0', '-', '=', '\b', /* Backspace */
      '\t',         /* Tab */
      'Q', 'W', 'E', 'R',   /* 19 */
      'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', /* Enter key */
        0,          /* 29   - Control */
      'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', /* 39 */
     '\'', '`',   0,        /* Left shift */
     '\\', 'Z', 'X', 'C', 'V', 'B', 'N',         /* 49 */
      'M', ',', '.', '/',   0,              /* Right shift */
      '*',
        0,  /* Alt */
      ' ',  /* Space bar */
        0,  /* Caps lock */
        0,  /* 59 - F1 key ... > */
        0,   0,   0,   0,   0,   0,   0,   0,
        0,  /* < ... F10 */
        0,  /* 69 - Num lock*/
        0,  /* Scroll Lock */
        0,  /* Home key */
        0,  /* Up Arrow */
        0,  /* Page Up */
      '-',
        0,  /* Left Arrow */
        0,
        0,  /* Right Arrow */
      '+',
        0,  /* 79 - End key*/
        0,  /* Down Arrow */
        0,  /* Page Down */
        0,  /* Insert Key */
        0,  /* Delete Key */
        0,   0,   0,
        0,  /* F11 Key */
        0,  /* F12 Key */
        0,  /* All other keys are undefined */
    };

char scancode_shift_and_caps[MAX_SIZE] =
    {
        0,  27, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
      '(', ')', '_', '+', '\b', /* Backspace */
      '\t',         /* Tab */
      'q', 'w', 'e', 'r',   /* 19 */
      't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n', /* Enter key */
        0,          /* 29   - Control */
      'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', /* 39 */
     '\"', '~',   0,        /* Left shift */
     '|', 'z', 'x', 'c', 'v', 'b', 'n',            /* 49 */
      'm', '<', '>', '?',   0,              /* Right shift */
      '*',
        0,  /* Alt */
      ' ',  /* Space bar */
        0,  /* Caps lock */
        0,  /* 59 - F1 key ... > */
        0,   0,   0,   0,   0,   0,   0,   0,
        0,  /* < ... F10 */
        0,  /* 69 - Num lock*/
        0,  /* Scroll Lock */
        0,  /* Home key */
        0,  /* Up Arrow */
        0,  /* Page Up */
      '-',
        0,  /* Left Arrow */
        0,
        0,  /* Right Arrow */
      '+',
        0,  /* 79 - End key*/
        0,  /* Down Arrow */
        0,  /* Page Down */
        0,  /* Insert Key */
        0,  /* Delete Key */
        0,   0,   0,
        0,  /* F11 Key */
        0,  /* F12 Key */
        0,  /* All other keys are undefined */
    };


/* void kb_init()
 *
 * enables the irq to KD_IRQ
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void kb_init()
{
    enable_irq(KB_IRQ);
    term_open(NULL);
    reset();
}


/* void kb_handler()
 *
 * gets the keypress value and maps it to the pertinent scancode from the table above
 * then sends the end of interrupt signal
 * Inputs: None
 * Outputs: None
 * Side Effects: None
 */
void kb_handler()
{
    disable_irq(0);
    cli();
    int pressed = inb(KB_ADDR);

    //Prints to screen if scancode isn't 0 and not a special character
    if(pressed != 0)
    {
        char todo = fill_keyboard_buffer(pressed);

        switch(todo)
        {
            case UNKNOWN:
                break;

            default:
                if (ctrl_flag || (ctrl_flag && (t_terms[curr_running].b_index == MAX_SIZE)))
                {
                    if(todo == 'l' || todo == 'L')
                    {
                        clear();
                        t_terms[curr_running].b_index = 0;
                        break;
                    }
                    if(todo == 'c' || todo == 'C')
                    {
                        send_eoi(KB_IRQ);
                        sti();
                        halt(0);
                        return;
                    }
                }
                if(t_terms[curr_running].b_index < MAX_SIZE)
                {
                    t_terms[curr_running].kb_buffer[t_terms[curr_running].b_index] = todo;

                    term_write(0, &t_terms[curr_running].kb_buffer[t_terms[curr_running].b_index], 1);
                    
                    t_terms[curr_running].b_index++;
                }
                break;
        }
    }

    send_eoi(KB_IRQ);
    sti();
    enable_irq(0);
}

/* void fill_keyboard_buffer(int keycode)
 *
 * fills the keyboar buffer with normal ascii
 * Inputs: keycode = the scan code
 * Outputs: none
 * Side effects: sets special key flags
 */
char fill_keyboard_buffer(int keycode)
{
    //special key flag setting
    switch(keycode)
    {
        case ALT_DOWN:
            alt_flag = 1;
            return 0;

        case ALT_RELEASE:
            alt_flag = 0;
            return 0;

        case CAPS_KEYCODE:
            if (caps_flag)
                caps_flag = 0;
            else
                caps_flag = 1;
            return 0;

        case RCTRL_DOWN:
        case LCTRL_DOWN:
            ctrl_flag = 1;
            return 0;
            
        case CTRL_RELEASE:
            ctrl_flag = 0;
            return 0;

        case RSHIFT_DOWN:
        case LSHIFT_DOWN:
            shift_flag = 1;
            return 0;

        case RSHIFT_RELEASE:
        case LSHIFT_RELEASE:
            shift_flag = 0;
            return 0;

        case ENTER:
            putc('\n');

            /* for history */
            t_terms[curr_running].read_index = t_terms[curr_running].b_index;
            t_terms[curr_running].b_index = 0;
            t_terms[curr_running].read_flag = 1;

            minus = count = 0;  // for moving cursor to the right

            if(max_h_index < HIST_NUM-1)
                max_h_index++;
            return 0;

        case BACKSPACE:
            if(t_terms[curr_running].b_index < 0)
                t_terms[curr_running].b_index = 0;
            if(t_terms[curr_running].b_index > 0)
            {
                putc('\b');
                t_terms[curr_running].b_index--;
                minus++;
            }
            return 0;

        case UP:
            return 0;

        case DOWN:
            return 0;

        case LEFT:
            if(t_terms[curr_running].b_index <= 0)
                return 0;
            if(t_terms[curr_running].b_index > count)
                count = t_terms[curr_running].b_index;

            t_terms[curr_running].b_index--;
            screen_x--;
            update_cursor(screen_x, screen_y);

            return 0;

        case RIGHT:
            if((count + minus + 7) % 80 <= screen_x)
                return 0;
            
            if(minus > 0)
                minus--;

            t_terms[curr_running].b_index++;
            t_terms[curr_running].read_index++;
            screen_x++;
            update_cursor(screen_x, screen_y);
            
            return 0;

        case TAB:
            return 0;

        default:
            if (alt_flag)
            {
                if(keycode == Fn1)
                    change_screen(0);
                else if (keycode == Fn2)
                    change_screen(1);      
                else if (keycode == Fn3)
                    change_screen(2);
         
                return 0;
            }
            if (keycode > 0 && keycode < MAX_SIZE)
            {
                if (shift_flag && !caps_flag)
                    return scancode_shift[keycode];

                else if (!shift_flag && caps_flag)
                    return scancode_caps[keycode];

                else if (shift_flag && caps_flag)
                    return scancode_shift_and_caps[keycode];

                else
                    return scancode_regular[keycode];
            }
            return 0;
    }
}

/* void putc2(uint8_t c);
 * Inputs: uint_8* c = character to print
 *       : term_num = the terminal to print into
 * Return Value: void
 * Function: Prints a charcter into video memory of the corresponding terminal */
void putc2(uint8_t c, int term_num) 
{
    if(c == '\n' || c == '\r') 
    {
        t_terms[term_num].y_position++;
        t_terms[term_num].x_position = 0;

        if(t_terms[term_num].y_position == NUM_ROWS)
            scroll_up2(term_num);
    } 
    else if(c == '\b')
    {
        if(t_terms[term_num].x_position > 0)
            t_terms[term_num].x_position--;
        else
        {
            if(screen_y > 0)
            {
                t_terms[term_num].y_position--;
                t_terms[term_num].x_position = NUM_COLS-1;
            }
        }

        *(uint8_t *)(t_terms[term_num].vidmem_cpy + ((NUM_COLS * t_terms[term_num].y_position + t_terms[term_num].x_position) << 1)) = ' ';
        *(uint8_t *)(t_terms[term_num].vidmem_cpy + ((NUM_COLS * t_terms[term_num].y_position + t_terms[term_num].x_position) << 1) + 1) = t_terms[term_num].change_attrib;

    }
    else if(c == '\t')
    {
            if(t_terms[term_num].x_position < (NUM_COLS-4))
                t_terms[term_num].x_position = t_terms[term_num].x_position + 4;
            else
            {
                t_terms[term_num].y_position++;
                t_terms[term_num].x_position = 4;

                if(t_terms[term_num].y_position == NUM_ROWS)
                    scroll_up2(term_num);
            }
    }

    else 
    {
        *(uint8_t *)(t_terms[term_num].vidmem_cpy + ((NUM_COLS * t_terms[term_num].y_position + t_terms[term_num].x_position) << 1)) = c;
        *(uint8_t *)(t_terms[term_num].vidmem_cpy + ((NUM_COLS * t_terms[term_num].y_position + t_terms[term_num].x_position) << 1) + 1) = t_terms[term_num].change_attrib;

        if(screen_x == NUM_COLS-1)
        {
            t_terms[term_num].x_position = 0;
            t_terms[term_num].y_position++;

            if(t_terms[term_num].y_position == NUM_ROWS)
                scroll_up2(term_num);
        }
        else
        {
            t_terms[term_num].x_position++;
        }
    }
}

/* void scroll_up2(int term_num);
 * Inputs: term_num - the terminal to scroll up on
 * Return Value: void
 * Function: Scrolls up on a terminal */
void scroll_up2(int term_num)
{
    int i, j;

    for(i = 0; i < NUM_ROWS - 1 ; ++i)
    {
        for(j = 0; j < NUM_COLS; ++j)
        {
            *(uint8_t *)(t_terms[term_num].vidmem_cpy + ((NUM_COLS * i + j) << 1)) =  *(uint8_t *)(t_terms[term_num].vidmem_cpy + ((NUM_COLS * (i + 1) + j) << 1));
            *(uint8_t *)(t_terms[term_num].vidmem_cpy + ((NUM_COLS * i + j) << 1) + 1) = t_terms[term_num].change_attrib;
        }
    }

    i = NUM_ROWS - 1;
    for(j = 0; j < NUM_COLS; ++j)
    {
        *(uint8_t *)(t_terms[term_num].vidmem_cpy + ((NUM_COLS * i + j) << 1)) = ' ';
        *(uint8_t *)(t_terms[term_num].vidmem_cpy + ((NUM_COLS * i + j) << 1) + 1) = t_terms[term_num].change_attrib;
    }
    t_terms[term_num].x_position = 0;
    t_terms[term_num].y_position = NUM_ROWS - 1;
}


