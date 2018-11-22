/* ============================================================================ */
/*	tetris.h : Tetris Header File		*/
/* ============================================================================ */

//#include <avr/pgmspace.h>
//#include <math.h>
//#include "OK-TFT_font.h"			// 8x16 English(ASCII), 16x16 Korean font

/* ---------------------------------------------------------------------------- */



#if 0
unsigned char key_flag = 0;

unsigned char Key_input(void)			/* input key KEY1 - KEY4 */
{
    unsigned char key;

    key = PINF & 0xF0;				// any key pressed ?
    if(key == 0xF0) {				// if no key, check key off
        if(key_flag == 0) {
            return key;
        } else {
            Timer0_Delay(20); 
            key_flag = 0;
            return key;
        }
    } else {						// if key input, check continuous key
        if(key_flag != 0) {			// if continuous key, treat as no key input
            return 0xF0;
        } else{				    	// if new key, beep and delay for debounce
            //Beep();
            key_flag = 1;
            return key;
        }
    }
}

unsigned char Key_input_silent(void)		/* input key KEY1 - KEY4 without sound */
{
    unsigned char key;

    key = PINF & 0xF0;				// any key pressed ?
    if(key == 0xF0) {				// if no key, check key off
        if(key_flag == 0) {
            return key; 
        } else {
            Delay_ms(20);
            key_flag = 0;
            return key;
        }
    } else {						// if key input, check continuous key
        if(key_flag != 0) {				// if continuous key, treat as no key input
            return 0xF0;
        } else {					// if new key, delay for debounce
            Delay_ms(20);
            key_flag = 1;
            return key;
        }
    }
}
#endif

/* ---------------------------------------------------------------------------- */
/*		OK-TFT28 보드 기본 함수						*/
/* ---------------------------------------------------------------------------- */

//    -RESET = PD7     of ATmega2561-16AU
//       -CS = PD6
//        RS = PD5
//       -WR = PD4
//  DB15~DB8 = PC7~PC0
//   DB7~DB0 = PA7~PA0
#if 0
#define WHITE		0xFFFF			/* 16 bit 8 color data */
#define BLACK		0x0000
#define RED		    0xF800
#define GREEN		0x07E0
#define BLUE		0x001F
#define YELLOW		0xFFE0			// Yellow = Red + Green
#define CYAN		0x07FF			// Cyan = Green + Blue
#define MAGENTA		0xF81F			// Magenta = Red + Blue
#define BROWN		0xA145
#define KHAKI		0xF731
#define OLIVE		0x8400
#define ORANGE		0xFD20
#define PINK		0xFB56
#define PURPLE		0x8010
#define SILVER		0xC618
#define VIOLET		0xEC1D
#define TRANSPARENT	0x0821
#endif

//unsigned char ScreenMode = 'P';			    // screen mode(P=portrait, L=landscape)
//unsigned char LineLimit = 30;			    // character length of line (30 or 40)
//unsigned char KoreanBuffer[32] = {0};		// 32 byte Korean font buffer
//unsigned char xcharacter, ycharacter;		// xcharacter(0-29), ycharacter(0-39) for portrait
// xcharacter(0-39), ycharacter(0-29) for landscape
//unsigned int  foreground, background;		// foreground and background color
//unsigned char cursor_flag = 0;			    // 0 = cursor off, 1 = cursor on
//unsigned char xcursor, ycursor;			    // cursor position
//unsigned int  cursor;				        // cursor color
//unsigned char outline_flag = 0;			    // 0 = outline off, 1 = outline on
//unsigned int  outline;				        // outline color

#if 0
void TFT_command(unsigned char IR)			/* write IR to TFT-LCD */
{
    PORTC = 0x00;					// write high byte
    PORTA = IR;					    // write high + low byte
    PORTD = 0x90;					// RS = -CS = 0
    PORTD = 0x80;					// -WR = 0
    PORTD = 0x90;					// -WR = 1
    PORTD = 0xF0;					// RS = -CS = 1
}

void TFT_data(unsigned int data)				/* write data to TFT-LCD */
{
    PORTC = data >> 8;				// write high byte
    PORTA = data & 0x00FF;			// write high + low byte
    PORTD = 0xB0;					// RS = 1, -CS = 0
    PORTD = 0xA0;					// -WR = 0
    PORTD = 0xB0;					// -WR = 1
    PORTD = 0xF0;					// RS = -CS = 1
}

void Initialize_TFT_LCD(void)			/* initialize TFT-LCD with HX8347 */
{
    PORTD = 0x70;					// reset
    Delay_ms(1);
    PORTD = 0xF0;
    Delay_ms(120);

    TFT_command(0x02); TFT_data(0x0000);		// window setting
    TFT_command(0x03); TFT_data(0x0000);		// x = 0~239
    TFT_command(0x04); TFT_data(0x0000);
    TFT_command(0x05); TFT_data(0x00EF);
    TFT_command(0x06); TFT_data(0x0000);		// y = 0~319
    TFT_command(0x07); TFT_data(0x0000);
    TFT_command(0x08); TFT_data(0x0001);
    TFT_command(0x09); TFT_data(0x003F);

    TFT_command(0x01); TFT_data(0x0006);		// display setting
    TFT_command(0x16); TFT_data(0x0008);		// MV=0, MX=0, MY=0, BGR=1
    TFT_command(0x18); TFT_data(0x0000);		// SM=0
    TFT_command(0x70); TFT_data(0x0065);		// GS=1, SS=1, 16 bit/pixel
    TFT_command(0x23); TFT_data(0x0095);
    TFT_command(0x24); TFT_data(0x0095);
    TFT_command(0x25); TFT_data(0x00FF);
    TFT_command(0x27); TFT_data(0x0002);
    TFT_command(0x28); TFT_data(0x0002);
    TFT_command(0x29); TFT_data(0x0002);
    TFT_command(0x2A); TFT_data(0x0002);
    TFT_command(0x2C); TFT_data(0x0002);
    TFT_command(0x2D); TFT_data(0x0002);
    TFT_command(0x3A); TFT_data(0x0001);
    TFT_command(0x3B); TFT_data(0x0001);
    TFT_command(0x3C); TFT_data(0x00F0);
    TFT_command(0x3D); TFT_data(0x0000);
    Delay_ms(20);
    TFT_command(0x35); TFT_data(0x0038);
    TFT_command(0x36); TFT_data(0x0078);
    TFT_command(0x3E); TFT_data(0x0038);
    TFT_command(0x40); TFT_data(0x000F);
    TFT_command(0x41); TFT_data(0x00F0); 

    TFT_command(0x19); TFT_data(0x0049);		// power setting
    TFT_command(0x93); TFT_data(0x000F);
    Delay_ms(10);
    TFT_command(0x20); TFT_data(0x0040);
    TFT_command(0x1D); TFT_data(0x0007);
    Delay_ms(10); 
    TFT_command(0x1E); TFT_data(0x0000);
    TFT_command(0x1F); TFT_data(0x0004);
    TFT_command(0x44); TFT_data(0x0040);		// VCom control for 3.3V
    TFT_command(0x45); TFT_data(0x0012);
    Delay_ms(10); 
    TFT_command(0x1C); TFT_data(0x0004);
    Delay_ms(20);
    TFT_command(0x43); TFT_data(0x0080);  
    Delay_ms(5);
    TFT_command(0x1B); TFT_data(0x0018);    
    Delay_ms(40); 
    TFT_command(0x1B); TFT_data(0x0010);      
    Delay_ms(40);

    TFT_command(0x46); TFT_data(0x00A4);		// gamma setting 
    TFT_command(0x47); TFT_data(0x0053);
    TFT_command(0x48); TFT_data(0x0010);
    TFT_command(0x49); TFT_data(0x005F);
    TFT_command(0x4A); TFT_data(0x0004);
    TFT_command(0x4B); TFT_data(0x003F);
    TFT_command(0x4C); TFT_data(0x0002);
    TFT_command(0x4D); TFT_data(0x00F6);
    TFT_command(0x4E); TFT_data(0x0012);
    TFT_command(0x4F); TFT_data(0x004C);
    TFT_command(0x50); TFT_data(0x0046);
    TFT_command(0x51); TFT_data(0x0044);

    TFT_command(0x90); TFT_data(0x007F);		// display on setting
    TFT_command(0x26); TFT_data(0x0004);
    Delay_ms(40);
    TFT_command(0x26); TFT_data(0x0024);
    TFT_command(0x26); TFT_data(0x002C);
    Delay_ms(40); 
    TFT_command(0x26); TFT_data(0x003C);

    TFT_command(0x57); TFT_data(0x0002);		// internal VDDD setting
    TFT_command(0x55); TFT_data(0x0000);
    TFT_command(0xFE); TFT_data(0x005A);
    TFT_command(0x57); TFT_data(0x0000);

    TFT_clear_screen();				// clear screen
}
#endif

/* ---------------------------------------------------------------------------- */
/*		OK-TFT28 보드 출력제어 함수					*/
/* ---------------------------------------------------------------------------- */
#if 0
void TFT_clear_screen(void)			/* TFT-LCD clear screen with black color */
{
    TFT_color_screen(Black);
}

void TFT_color_screen(unsigned int color)		/* TFT-LCD full screen color */
{
    unsigned int i, j;

    TFT_GRAM_address(0,0);

    for(i=0; i<320; i++) {
        for(j=0; j<240; j++) {
            TFT_data(color);
        }
    }
}

void TFT_GRAM_address(unsigned int xPos, unsigned int yPos)	/* set GRAM address of TFT-LCD */
{
    if((xPos > 239) || (yPos > 319)) return;

    TFT_command(0x02); TFT_data(0x0000);		// xPos = 0~239
    TFT_command(0x03); TFT_data(xPos);
    TFT_command(0x06); TFT_data(yPos >> 8);	    // yPos = 0~319
    TFT_command(0x07); TFT_data(yPos & 0x00FF);
    TFT_command(0x22);
}

void TFT_screen_mode(unsigned char mode)			/* set screen direction mode */
{
    if((mode == 'P') || (mode == 'p')) {    // portrait mode(30x40 character unit)
        ScreenMode = 'P';
        LineLimit = 30;
    } else {						        // landscape mode(40x30 character unit)
        ScreenMode = 'L';
        LineLimit = 40;
    }
}

void TFT_xy(unsigned char xChar, unsigned char yChar)		/* set character position (x,y) */
{
    xcharacter = xChar;
    ycharacter = yChar;
}

void TFT_color(unsigned int colorfore, unsigned int colorback)	/* set foreground and background color */
{
    foreground = colorfore;
    background = colorback;
}

void TFT_pixel(unsigned int xPos, unsigned int yPos, unsigned int color)	/* write a pixel */
{
    if((xPos > 239) || (yPos > 319)) return;

    TFT_command(0x02); TFT_data(0x0000);		// xPos = 0~239
    TFT_command(0x03); TFT_data(xPos);
    TFT_command(0x06); TFT_data(yPos >> 8);	// yPos = 0~319
    TFT_command(0x07); TFT_data(yPos & 0x00FF);
    TFT_command(0x22);

    if(color != Transparent) {			// transparent mode ?
        TFT_data(color);
    }
}
#endif

/* ---------------------------------------------------------------------------- */
/*		OK-TFT28 보드 문자 출력 함수					*/
/* ---------------------------------------------------------------------------- */

#if 0
void TFT_string(unsigned char xChar, unsigned char yChar, unsigned int colorfore, unsigned int colorback, char *str)	/* write TFT-LCD string */
{
    unsigned char ch1;
    unsigned int ch2;

    xcharacter = xChar;
    ycharacter = yChar;

    foreground = colorfore;
    background = colorback;

    while (*str) {
        ch1 = *str;
        str++;

        if(ch1 < 0x80) {				// English ASCII character
            TFT_English(ch1);
        } else {					    // Korean
            ch2 = (ch1 << 8) + (*str);
            str++;
            ch2 = KS_code_conversion(ch2);	// convert KSSM(완성형) to KS(조합형)
            TFT_Korean(ch2);
        }
    }
}

void TFT_English(unsigned char code)			/* write a English ASCII character */
{
    unsigned char data, x, y;
    unsigned int pixel[8][16];
    unsigned int dot0, dot1, dot2, dot3, dot4;

    for(x = 0; x < 8; x++) {			// read English ASCII font
        data = pgm_read_byte(&E_font[code][x]);

        for(y = 0; y < 8; y++) {
            if(data & 0x01) pixel[x][y] = foreground;
            else            pixel[x][y] = background;

            data = data >> 1;
        }
    }

    for(x = 0; x < 8; x++) {
        data = pgm_read_byte(&E_font[code][x+8]);
        for(y = 0; y < 8; y++) {
            if(data & 0x01) pixel[x][y+8] = foreground;
            else            pixel[x][y+8] = background;

            data = data >> 1;
        }
    }

    if(outline_flag == 1) {				// display outline
        for(x = 0; x < 8; x++) {
            dot0 = pgm_read_byte(&E_font[code][x]) + pgm_read_byte(&E_font[code][x+8])*256;
            dot1 = dot0 >> 1;								 // up side
            dot2 = dot0;									 // down side
            dot3 = pgm_read_byte(&E_font[code][x+1]) + pgm_read_byte(&E_font[code][x+9])*256;// left side
            dot4 = pgm_read_byte(&E_font[code][x-1]) + pgm_read_byte(&E_font[code][x+7])*256;// right side

            for(y = 0; y < 15; y++) {
                if(!(dot0 & 0x0001)) {
                    if(dot1 & 0x0001) pixel[x][y] = outline;
                    if(dot2 & 0x0001) pixel[x][y] = outline;
                    if((dot3 & 0x0001) && (x < 7 )) pixel[x][y] = outline;
                    if((dot4 & 0x0001) && (x > 0 )) pixel[x][y] = outline;
                }

                dot1 >>= 1;
                dot2 = dot0;
                dot0 >>= 1;
                dot3 >>= 1;
                dot4 >>= 1;
            }
        }
    }

    if((cursor_flag == 1) && (xcharacter == xcursor) && (ycharacter == ycursor)) {
        for(x = 0; x < 8; x++) {			// display cursor
            pixel[x][14] = cursor;
            pixel[x][15] = cursor;
        }
    }

    if(ScreenMode == 'P') {
        for(y = 0; y < 16; y++) {			// write in portrait mode
            for(x = 0; x < 8; x++) {
                TFT_pixel(xcharacter*8 + x, ycharacter*8 + y, pixel[x][y]);
            }
        }
    } else {
        for(y = 0; y < 16; y++) {			// write in landscape mode
            for(x = 0; x < 8; x++) {
                TFT_pixel((29-ycharacter)*8 + 7 - y, xcharacter*8 + x, pixel[x][y]);
            }
        }
    }

    xcharacter += 1;
    if(xcharacter >= LineLimit) {			// end of line ?
        xcharacter = 0;
        ycharacter += 2;
    }
}

unsigned int KS_code_conversion(unsigned int KSSM)	/* convert KSSM(완성형) to KS(조합형) */
{
    unsigned char HB, LB;
    unsigned int index, KS;

    HB = KSSM >> 8;
    LB = KSSM & 0x00FF;

    if(KSSM >= 0xB0A1 && KSSM <= 0xC8FE) {
        index = (HB - 0xB0)*94 + LB - 0xA1;
        KS  =  pgm_read_byte(&KS_Table[index][0]) * 256;
        KS |=  pgm_read_byte(&KS_Table[index][1]);

        return KS;
    } else {
        return -1;
    }
}

void TFT_Korean(unsigned int code)			/* write a Korean character */
{
    unsigned char cho_5bit, joong_5bit, jong_5bit;
    unsigned char cho_bul, joong_bul, jong_bul = 0, i, jong_flag;
    unsigned int ch;

    cho_5bit   = pgm_read_byte(&table_cho[(code >> 10) & 0x001F]);  // get 5bit(14-10) of chosung
    joong_5bit = pgm_read_byte(&table_joong[(code >> 5) & 0x001F]); // get 5bit(09-05) of joongsung
    jong_5bit  = pgm_read_byte(&table_jong[code & 0x001F]);	  // get 5bit(04-00) of jongsung

    if(jong_5bit == 0) {				// if jongsung not exist
        jong_flag = 0;
        cho_bul = pgm_read_byte(&bul_cho1[joong_5bit]);
        if((cho_5bit == 1) || (cho_5bit == 16)) {
            joong_bul = 0;
        } else {
            joong_bul = 1;
        }
    } else {						// if jongsung exist
        jong_flag = 1;
        cho_bul = pgm_read_byte(&bul_cho2[joong_5bit]);
        if((cho_5bit == 1) || (cho_5bit == 16)) {
            joong_bul = 2;
        } else {
            joong_bul = 3;
        }

        jong_bul = pgm_read_byte(&bul_jong[joong_5bit]);
    }

    ch = cho_bul*20 + cho_5bit;			// get chosung font 
    for(i=0; i<32; i++) {
        KoreanBuffer[i] = pgm_read_byte(&K_font[ch][i]);
    }
    ch = 8*20 + joong_bul*22 + joong_5bit;	// OR joongsung font
    for(i=0; i<32; i++) {
        KoreanBuffer[i] |= pgm_read_byte(&K_font[ch][i]);
    }
    if(jong_flag) {					// OR jongsung font
        ch = 8*20 + 4*22 + jong_bul*28 + jong_5bit;
        for(i=0; i<32; i++) {
            KoreanBuffer[i] |= pgm_read_byte(&K_font[ch][i]);
        }
    }

    unsigned char data, x, y;
    unsigned int pixel[16][16];
    unsigned int dot0, dot1, dot2, dot3, dot4;

    for(x=0; x<16; x++) {			// read Korean font
        data = KoreanBuffer[x];
        for(y=0; y<8; y++) {
            if(data & 0x01) pixel[x][y] = foreground;
            else            pixel[x][y] = background;

            data = data >> 1;
        }
    }

    for(x=0; x<16; x++) {
        data = KoreanBuffer[x+16];
        for(y=0; y<8; y++) {
            if(data & 0x01) pixel[x][y+8] = foreground;
            else            pixel[x][y+8] = background;

            data = data >> 1;
        }
    }

    if (outline_flag == 1) {				// display outline
        for(x=0; x<16; x++) {
            dot0 = KoreanBuffer[x] + KoreanBuffer[x+16]*256;
            dot1 = dot0 >> 1;				   // up side
            dot2 = dot0;					   // down side
            dot3 = KoreanBuffer[x+1] + KoreanBuffer[x+17]*256; // left side
            dot4 = KoreanBuffer[x-1] + KoreanBuffer[x+15]*256; // right side

            for(y=0; y<16; y++) {
                if(!(dot0 & 0x0001)) {
                    if(dot1 & 0x0001) pixel[x][y] = outline;
                    if(dot2 & 0x0001) pixel[x][y] = outline;
                    if((dot3 & 0x0001) && (x < 15 )) pixel[x][y] = outline;
                    if((dot4 & 0x0001) && (x > 0 ))  pixel[x][y] = outline;
                }

                dot1 >>= 1;
                dot2 = dot0;
                dot0 >>= 1;
                dot3 >>= 1;
                dot4 >>= 1;
            }
        }
    }

    if ((cursor_flag == 1) && (xcharacter == xcursor) &&(ycharacter == ycursor)) {
        for (x=0; x<16; x++) {			// display cursor
            pixel[x][14] = cursor;
            pixel[x][15] = cursor;
        }
    }

    if (xcharacter >= (LineLimit-1)) {		// end of line ?
        xcharacter = 0;
        ycharacter += 2;
    }

    if(ScreenMode == 'P') {				// write in portrait mode
        for(y = 0; y < 16; y++) {
            for(x = 0; x < 16; x++) {
                TFT_pixel(xcharacter*8 + x, ycharacter*8 + y, pixel[x][y]);
            }
        }
    } else {						// write in landscape mode
        for(y = 0; y < 16; y++) {
            for(x = 0; x < 16; x++) {
                TFT_pixel((29-ycharacter)*8 + 7 - y, xcharacter*8 + x, pixel[x][y]);
            }
        }
    }

    xcharacter += 2;
    if(xcharacter >= LineLimit) {			// end of line ?
        xcharacter = 0;
        ycharacter += 2;
    }
}

void TFT_cursor(unsigned int cursor_color)		/* set cursor and color */
{
    if(cursor_color == Transparent) {		// disable cursor
        cursor_flag = 0;
    } else {						// enable cursor
        cursor_flag = 1;
        cursor = cursor_color;
    }
}

void TFT_outline(unsigned int outline_color)		/* set outline and color */
{
    if(outline_color == Transparent) {		// disable outline
        outline_flag = 0;
    } else {						// enable outline
        outline_flag = 1;
        outline = outline_color;
    }
}
#endif

/* ---------------------------------------------------------------------------- */
/*		OK-TFT28 보드 수치 데이터 출력 함수				*/
/* ---------------------------------------------------------------------------- */
#if 0
void TFT_unsigned_decimal(unsigned long number, unsigned char zerofill, unsigned char digit) /* display unsigned decimal number */
{
    unsigned char zero_flag, character;
    unsigned long div;

    if((digit == 0) || (digit > 9)) return;

    div = 1;
    while(--digit) div *= 10;

    zero_flag = zerofill;
    while(div > 0) {				// display number
        character = number / div;
        if((character == 0) && (zero_flag == 0) && (div != 1)) {
            TFT_English(character + ' ');
        } else {
            zero_flag = 1;
            TFT_English(character + '0');
        }
        number %= div;
        div /= 10;
    }
}

void TFT_signed_decimal(long number, unsigned char zerofill, unsigned char digit)	/* display signed decimal number */
{
    unsigned char zero_flag, character;
    unsigned long div;

    if((digit == 0) || (digit > 9)) return;

    if(number >= 0) {				// display sign
        TFT_English('+');
    } else {
        TFT_English('-');
        number = -number;
    }

    div = 1;
    while(--digit) div *= 10;

    zero_flag = zerofill;
    while(div > 0) {				// display number
        character = number / div;
        if((character == 0) && (zero_flag == 0) && (div != 1)) {
            TFT_English(character + ' ');
        } else {
            zero_flag = 1;
            TFT_English(character + '0');
        }
        number %= div;
        div /= 10;
    }
}

void TFT_hexadecimal(unsigned long number, unsigned char digit)	/* display hexadecimal number */
{
    unsigned char i, character;

    if((digit == 0) || (digit > 8)) return;

    for(i=digit; i>0; i--) {
        character = (number >> 4*(i-1)) & 0x0F;
        if(character < 10) TFT_English(character + '0');
        else               TFT_English(character - 10 + 'A');
    }
}

void TFT_0x_hexadecimal(unsigned long number, unsigned char digit)	/* display hexadecimal number with 0x */
{
    unsigned char i, character;

    if((digit == 0) || (digit > 8)) return;

    TFT_English('0');
    TFT_English('x');

    for(i=digit; i>0; i--) {
        character = (number >> 4*(i-1)) & 0x0F;
        if(character < 10) TFT_English(character + '0');
        else               TFT_English(character - 10 + 'A');
    }
}

void TFT_unsigned_float(float number, unsigned char integral, unsigned char fractional) /* display unsigned floating-point number */
{
    unsigned char zero_flag, digit, character;	// integral = digits of integral part
    unsigned long div, integer;			// fractional = digits of fractional part

    digit = integral + fractional;
    if((integral == 0) || (fractional == 0) || (digit > 9)) return;

    div = 1;
    while(--digit) div *= 10;

    while(fractional--) number *= 10.;
    integer = (unsigned long)(number + 0.5);

    zero_flag = 0;
    digit = 1;
    while(div > 0) {				// display number
        character = integer / div;
        if((character == 0) && (zero_flag == 0) && (digit != integral)) {
            TFT_English(character + ' ');
        } else {
            zero_flag = 1;
            TFT_English(character + '0');
        }
        integer %= div;
        div /= 10;

        if(digit == integral) {
            TFT_English('.');
        }
        digit++;
    }
}

void TFT_signed_float(float number, unsigned char integral, unsigned char fractional) /* display signed floating-point number */
{
    unsigned char zero_flag, digit, character;
    unsigned long div, integer;

    digit = integral + fractional;
    if((integral == 0) || (fractional == 0) || (digit > 9)) return;

    if(number >= 0) {				// display sign 
        TFT_English('+');
    } else {
        TFT_English('-');
        number = -number;
    }

    div = 1;
    while(--digit) div *= 10;

    while(fractional--) number *= 10.;
    integer = (unsigned long)(number + 0.5);

    zero_flag = 0;
    digit = 1;
    while(div > 0) {				// display number
        character = integer / div;
        if((character == 0) && (zero_flag == 0) && (digit != integral)) {
            TFT_English(character + ' ');
        } else {
            zero_flag = 1;
            TFT_English(character + '0');
        }
        integer %= div;
        div /= 10;

        if(digit == integral) {
            TFT_English('.');
        }
        digit++;
    }
}
#endif

/* ---------------------------------------------------------------------------- */
/*		그래픽 함수							*/
/* ---------------------------------------------------------------------------- */

#if 0
void Line(int x1,int y1, int x2,int y2, unsigned int color)	/* draw a straight line */
{
    int x, y;

    if(y1 != y2) {					// if y1 != y2, y is variable
        if(y1 < y2) {				//              x is function
            for(y = y1; y <= y2; y++) {
                x = x1 + (long)(y - y1)*(long)(x2 - x1)/(y2 - y1);
                TFT_pixel(x, y, color);
            }
        } else {
            for(y = y1; y >= y2; y--) {
                x = x1 + (long)(y - y1)*(long)(x2 - x1)/(y2 - y1);
                TFT_pixel(x, y, color);
            }
        }
    } else if(x1 != x2) {				// if x1 != x2, x is variable
        if(x1 < x2) {				//              y is function
            for(x = x1; x <= x2; x++) {
                y = y1 + (long)(x - x1)*(long)(y2 - y1)/(x2 - x1);
                TFT_pixel(x, y, color);
            }
        } else {
            for(x = x1; x >= x2; x--) {
                y = y1 + (long)(x - x1)*(long)(y2 - y1)/(x2 - x1);
                TFT_pixel(x, y, color);
            }
        }
    } else {						// if x1 == x2 and y1 == y2, it is a dot
        TFT_pixel(x1, y1, color);
    }
}

void Rectangle(int x1,int y1, int x2,int y2, unsigned int color) /* draw a rectangle */
{
    Line(x1,y1, x1,y2, color);			// horizontal line
    Line(x2,y1, x2,y2, color);
    Line(x1,y1, x2,y1, color);			// vertical line
    Line(x1,y2, x2,y2, color);
}

void Block(int x1,int y1, int x2,int y2, unsigned int color, unsigned int fill) /* draw a rectangle with filled color */
{
    int i;

    Line(x1,y1, x1,y2, color);			// horizontal line
    Line(x2,y1, x2,y2, color);
    Line(x1,y1, x2,y1, color);			// vertical line
    Line(x1,y2, x2,y2, color);

    if((y1 < y2) && (x1 != x2)) {			// fill block
        for(i = y1+1; i <= y2-1; i++) {
            Line(x1+1,i, x2-1,i, fill);
        }
    } else if((y1 > y2) && (x1 != x2)) {
        for(i = y2+1; i <= y1-1; i++) {
            Line(x1+1,i, x2-1,i, fill);
        }
    }
}

void Circle(int x1,int y1, int r, unsigned int color)	/* draw a circle */
{
    int x, y;
    float s;

    for(y = y1 - r*3/4; y <= y1 + r*3/4; y++) {	// draw with y variable
        s = sqrt((long)r*(long)r - (long)(y-y1)*(long)(y-y1)) + 0.5;
        x = x1 + (int)s;
        TFT_pixel(x, y, color);
        x = x1 - (int)s;
        TFT_pixel(x, y, color);
    }

    for(x = x1 - r*3/4; x <= x1 + r*3/4; x++) {	// draw with x variable
        s = sqrt((long)r*(long)r - (long)(x-x1)*(long)(x-x1)) + 0.5;
        y = y1 + (int)s;
        TFT_pixel(x, y, color);
        y = y1 - (int)s;
        TFT_pixel(x, y, color);
    }
}

void Sine(int peak, unsigned char mode, unsigned int color)	/* draw a sine curve */
{
    int x, y;

    if(mode == 0) {
        for(y = 0; y <= 319; y++) {
            x = 120 - (int)(sin((float)y * 1.6875 * M_PI / 180.) * peak + 0.5);
            TFT_pixel(x, y, color);
        }
    } else {
        for(y = 0; y <= 319; y++) {
            x = 120 + (int)(sin((float)y * 1.6875 * M_PI / 180.) * peak + 0.5);
            TFT_pixel(x, y, color);
        }
    }
}
#endif

/* ---------------------------------------------------------------------------- */
/*		OK-TFT28 보드 ADS7846 터치 스크린 컨트롤러 입력 함수		*/
/* ---------------------------------------------------------------------------- */

#if 0
#define CS_SPI		PB0			    // ADS7846 chip select signal
#define IRQ_SPI		0b00100000		// PE5(INT5) = -IRQ_SPI from ADS7846 -PENIRQ

#define ADS7846_CMD_X	0b11010000	// 12-bit X position measurement command
#define ADS7846_CMD_Y	0b10010000	// 12-bit Y position measurement command

#define x_touch_min	250			    // minimum value of touch screen x-axis
#define x_touch_max	3850			// maximum value of touch screen x-axis
#define y_touch_min	220			    // minimum value of touch screen y-axis
#define y_touch_max	3850			// maximum value of touch screen y-axis

unsigned int x_12bit, y_12bit;		// x_12bit(0-4095), y_12bit(0-4095)
unsigned int x_touch, y_touch;		// x_touch(0-239), y_touch(0-319)

void SPI_Mode0_1MHz(void)			/* SPI mode 0, 1MHz */
{
    SPCR = 0x51;					// 16MHz/16 = 1MHz
    SPSR = 0x00;
}

unsigned char SPI_write(unsigned char data)		/* send a byte to SPI and receive */
{
    SPDR = data;
    while(!(SPSR & 0x80));

    return SPDR;
}

void Initialize_ADS7846(void)			/* initialize ADS7846 */
{
    sbi(DDRB, CS_SPI);				// -CS_SPI is output
    SPI_Mode0_1MHz();				// initialize SPI

    Read_ADS7846(ADS7846_CMD_X);			// dummy input to enable -PENIRG signal output
}

unsigned int Read_ADS7846(unsigned char command)		/* read X, Y value from ADS7846 */
{
    unsigned int axis;

    cbi(PORTB, CS_SPI);				// -CS_SPI = 0

    SPI_write(command);
    axis = SPI_write(0x00);			// read high 7 bit
    axis <<= 8;
    axis += SPI_write(0x00);			// read low 5 bit
    axis >>= 3;

    sbi(PORTB, CS_SPI);				// -CS_SPI = 1

    return axis;
}

void Touch_input_ADS7846(void)			/* touch input X, Y average value from ADS7846 */
{
    unsigned char i;

    x_12bit = 0;					// initial value
    y_12bit = 0;

    for(i = 0; i < 16; i++) {			// read X, Y value by 16 times if -PENIRQ enable
        if((PINE & IRQ_SPI) == 0x00) {		// -IRQ_SPI = 0 ?
            x_12bit += Read_ADS7846(ADS7846_CMD_X);	// if yes, measure X position
        } else {					// if not, return with 0
            x_12bit = 0;
            y_12bit = 0;
            break;
        }
        Delay_us(10);

        if((PINE & IRQ_SPI) == 0x00) {		// -IRQ_SPI = 0 ?
            y_12bit += Read_ADS7846(ADS7846_CMD_Y);	// if yes, measure Y position
        } else {					// if not, return with 0
            x_12bit = 0;
            y_12bit = 0;
            break;
        }
        Delay_us(10);
    }

    x_12bit >>= 4;				// calculate average for 16 times
    y_12bit >>= 4;

    if(x_12bit <= x_touch_min)			// convert to pixel x-axis value
        x_touch = 0;
    else if(x_12bit >= x_touch_max)
        x_touch = 239;
    else
        x_touch = (unsigned int)((float)(x_12bit - x_touch_min) * 239./(float)(x_touch_max - x_touch_min));

    if(y_12bit <= y_touch_min)			// convert to pixel y-axis value
        y_touch = 0;
    else if(y_12bit >= y_touch_max)
        y_touch = 319;
    else
        y_touch = (unsigned int)((float)(y_12bit - y_touch_min) * 319./(float)(y_touch_max - y_touch_min));
}

/* ---------------------------------------------------------------------------- */
/*		OK-TFT28 보드 TSC2003 터치 스크린 컨트롤러 입력 함수		*/
/* ---------------------------------------------------------------------------- */

#define IRQ_I2C		0b01000000		    // PE6(INT6) = -IRQ_I2C from TSC2003 -PENIRQ

#define TSC2003_CMD_X	0b11000000		// 12-bit X position measurement command
#define TSC2003_CMD_Y	0b11010000		// 12-bit Y position measurement command

void Initialize_TSC2003(void)			/* initialize TSC2003 */
{
    TWBR = 12;					// 400kHz(TWBR=12, TWPS=0)
    TWSR = 0x00;

    Read_TSC2003(TSC2003_CMD_X);			// dummy input to enable -PENIRG signal output
}

unsigned int Read_TSC2003(unsigned char command)		/* read X, Y value from TSC2003 */
{
    unsigned int axis;

    TWCR = 0xA4;					// START condition
    while(((TWCR & 0x80) == 0x00) || ((TWSR & 0xF8) != 0x08)); // START complete ?
    TWDR = 0x90;					// SLA+W
    TWCR = 0x84;
    while(((TWCR & 0x80) == 0x00) || ((TWSR & 0xF8) != 0x18)); // SLA+W complete ?
    TWDR = command;				// command
    TWCR = 0x84;
    while(((TWCR & 0x80) == 0x00) || ((TWSR & 0xF8) != 0x28)); // address complete ?
    TWCR = 0x94;   		                // STOP condition

    Delay_us(10);

    TWCR = 0xA4;					// START condition
    while(((TWCR & 0x80) == 0x00) || ((TWSR & 0xF8) != 0x08)); // START complete ?
    TWDR = 0x91;					// SLA+R
    TWCR = 0x84;
    while(((TWCR & 0x80) == 0x00) || ((TWSR & 0xF8) != 0x40)); // SLA+R complete ?
    TWCR = 0xC4;					// read data with acknowledge
    while(((TWCR & 0x80) == 0x00) || ((TWSR & 0xF8) != 0x50)); // data complete ?
    axis = TWDR;					// read high 8 bit
    axis <<= 8;
    TWCR = 0x84;					// read data with no acknowledge
    while(((TWCR & 0x80) == 0x00) || ((TWSR & 0xF8) != 0x58)); // data complete ?
    axis += TWDR;					// read low 4 bit
    TWCR = 0x94;   		                // STOP condition
    axis >>= 4;

    return axis;
}

void Touch_input_TSC2003(void)			/* touch input X, Y average value from TSC2003 */
{
    unsigned char i;

    x_12bit = 0;					// initial value
    y_12bit = 0;

    for(i = 0; i < 16; i++) {			// read X, Y value by 16 times if -PENIRQ enable
        if((PINE & IRQ_I2C) == 0x00) {		// -IRQ_I2C = 0 ?
            x_12bit += Read_TSC2003(TSC2003_CMD_X);	// if yes, measure X position
        } else {					// if not, return with 0
            x_12bit = 0;
            y_12bit = 0;
            break;
        }
        Delay_us(10);

        if((PINE & IRQ_I2C) == 0x00) {		// -IRQ_I2C = 0 ?
            y_12bit += Read_TSC2003(TSC2003_CMD_Y);	// if yes, measure Y position
        } else {					// if not, return with 0
            x_12bit = 0;
            y_12bit = 0;
            break;
        }
        Delay_us(10);
    }

    x_12bit >>= 4;				// calculate average for 16 times
    y_12bit >>= 4;

    if(x_12bit <= x_touch_min)			// convert to pixel x-axis value
        x_touch = 0;
    else if(x_12bit >= x_touch_max)
        x_touch = 239;
    else
        x_touch = (unsigned int)((float)(x_12bit - x_touch_min) * 239./(float)(x_touch_max - x_touch_min));

    if(y_12bit <= y_touch_min)			// convert to pixel y-axis value
        y_touch = 0;
    else if(y_12bit >= y_touch_max)
        y_touch = 319;
    else
        y_touch = (unsigned int)((float)(y_12bit - y_touch_min) * 319./(float)(y_touch_max - y_touch_min));
}
#endif
