/***************************************************************************
                          gs_wgsil.cpp  -  description
                             -------------------
    begin                : Sat. April 27, 2001
    copyright            : (C) 2001 Terry Biggs
    email                : tbiggs@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <gs_wgsil.h>

GS_WGSIL::GS_WGSIL()
{
}


char GS_WGSIL::ProcessText(char *text, int maxlen, const SWKey *key)
{
	char *to, *from, token[2048];
	int tokpos = 0;
	bool intoken 	= false;
	bool hasFootnotePreTag = false;
	bool isRightJustified = false;
	bool isCentered = false;
	int len;
	int test; 
	
	len = strlen(text) + 1;
	if (len < maxlen) {
		memmove(&text[maxlen - len], text, len);
		from = &text[maxlen - len];
	} else
		from = text;
	for (to = text; *from; from++) {
		test = *from;
		switch(test){
			case 34: *to++ = 19;
				break;
			case 35: *to++ = 4;
				break;
			case 36: *to++ = 118;
				break;
			case 37: *to++ = 32;
				break;
			/*case 38: *to++ = 019;
				break;*/
			case 39: *to++ = 104;
				break;
			/*case 40: *to++ = 019;
				break;
			case 41: *to++ = 019;
				break;
			case 42: *to++ = 019;
				break;
			case 43: *to++ = 019;
				break;
			case 44: *to++ = 019;
				break;
			case 45: *to++ = 019;
				break;
			case 46: *to++ = 019;
				break;*/
			case 47: *to++ = 39;
				break;
			/*case 48: *to++ = 048;
				break;
			case 49: *to++ = 019;
				break;
			case 50: *to++ = 019;
				break;
			case 51: *to++ = 019;
				break;
			case 52: *to++ = 019;
				break;
			case 53: *to++ = 019;
				break;
			case 54: *to++ = 019;
				break;
			case 55: *to++ = 019;
				break;
			case 56: *to++ = 019;
				break;
			case 57: *to++ = 019;
				break;*/
			case 58: *to++ = 59;
				break;
			case 59: *to++ = 63;
				break;
			/*case 60: *to++ = 019;
				break;
			case 61: *to++ = 019;
				break;
			case 62: *to++ = 019;
				break;
			case 63: *to++ = 063;  
				break;*/
			case 64: *to++ = 34;
				break;
			/*case 65: *to++ = 019;
				break;
			case 66: *to++ = 019;
				break;
			case 67: *to++ = 019;
				break;
			case 68: *to++ = 019;
				break;
			case 69: *to++ = 019;
				break;
			case 70: *to++ = 019;
				break;
			case 71: *to++ = 019;
				break;*/
			case 72: *to++ = 74;
				break;
			/*case 73: *to++ = 019;
				break;*/
			case 74: *to++ = 251;
				break;
			/*case 75: *to++ = 019;
				break;
			case 76: *to++ = 019;
				break;
			case 77: *to++ = 019;
				break;
			case 78: *to++ = 019;
				break;
			case 79: *to++ = 019;
				break;
			case 80: *to++ = 019;
				break;
			case 81: *to++ = 019;
				break;
			case 82: *to++ = 019;
				break;
			case 83: *to++ = 019;
				break;
			case 84: *to++ = 019;
				break;
			case 85: *to++ = 019;
				break;*/
			case 86: *to++ = 219;
				break;
			/*case 87: *to++ = 086;
				break;
			case 88: *to++ = 087;
				break;
			case 89: *to++ = 088;
				break;
			case 90: *to++ = 089;
				break;
			case 91: *to++ = 090;
				break;*/
			case 92: *to++ = 96;
				break;
			/*case 93: *to++ = 093;
				break;
			case 94: *to++ = 094;
				break;*/
			case 95: *to++ = 126;
				break;
			case 96: *to++ = 104;
				break;
			/*case 97: *to++ = 96;
				break;
			case 98: *to++ = 097;
				break;
			case 99: *to++ = 098;
				break;
			case 100: *to++ = 099;
				break;
			case 101: *to++ = 100;
				break;
			case 102: *to++ = 101;
				break;
			case 103: *to++ = 102;
				break;*/
			case 104: *to++ = 106;
				break;
			/*case 105: *to++ = 105;
				break;*/
			case 106: *to++ = 118;
				break;
			/*case 107: *to++ = 106;
				break;
			case 108: *to++ = 107;
				break;
			case 109: *to++ = 116;
				break;
			case 110: *to++ = 109;
				break;
			case 111: *to++ = 110;
				break;
			case 112: *to++ = 111;
				break;
			case 113: *to++ = 112;
				break;
			case 114: *to++ = 113;
				break;
			case 115: *to++ = 114;
				break;
			case 116: cout << "we got here";
				break; 
			case 117: *to++ = 118;
				break;*/
			case 118: *to++ = 139;
				break;
			/*case 119: *to++ = 019;
				break;
			case 120: *to++ = 019;
				break;
			case 121: *to++ = 019;
				break;
			case 122: *to++ = 019;
				break;
			case 123: *to++ = 019;
				break;
			case 124: *to++ = 019;
				break;
			case 125: *to++ = 019;
				break;
			case 126: *to++ = 019;
				break;*/
			case 127: *to++ = 124;
				break;
			case 128: *to++ = 032;
				break;
			case 129: *to++ = 032;
				break;
			case 130: *to++ = 032;
				break;
			case 131: *to++ = 179;
				break;
			case 132: *to++ = 184;
				break;
			case 133: *to++ = 176;
				break;
			case 134: *to++ = 180;
				break;
			case 135: *to++ = 185;
				break;
			case 136: *to++ = 177;
				break;
			case 137: *to++ = 190;
				break;
			case 138: *to++ = 184;
				*to++ = 96;
				break;
			case 139: *to++ = 178;
				break;
			case 140: *to++ = 181;
				break;
			case 141: *to++ = 186;
				break;
			case 142: *to++ = 187;
				break;
			case 143: *to++ = 188;
				break;
			case 144: *to++ = 189;
				break;
			case 145: *to++ = 171;
				break;
			case 146: *to++ = 175;
				break;
			case 147: *to++ = 169;				
				break;
			case 148: *to++ = 173;
				break;
			case 149: *to++ = 170;
				break;
			case 150: *to++ = 174;
				break;
			case 151: *to++ = 238;
				break;
			case 152: *to++ = 155;
				break;
			case 153: *to++ = 157;
				break;
			case 154: *to++ = 153;
				break;
			case 155: *to++ = 156;
				break;
			case 156: *to++ = 158;
				break;
			case 157: *to++ = 154;
				break;
			case 158: *to++ = 159;
				break;
			case 159: *to++ = 157;
				*to++ = 96;
				break;
			case 160: *to++ = 239;
				break;
			case 161: *to++ = 131;
				break;
			case 162: *to++ = 135;
				break;
			case 163: *to++ = 128;
				break;
			case 164: *to++ = 144;
				break;
			case 165: *to++ = 136;
				break;
			case 166: *to++ = 141;
				break;
			case 167: *to++ = 145;
				break;
			case 168: *to++ = 137;
				break;
			case 169: *to++ = 130;
				break;
			case 170: *to++ = 134;
				break;
			case 171: *to++ = 138;
				break;
			case 172: *to++ = 143;
				break;
			case 173: *to++ = 147;
				break;
			case 174: *to++ = 140;
				break;
			case 175: *to++ = 144;
				break;
			case 176: *to++ = 148;
				break;
			case 177: *to++ = 141;
				break;
			case 178: *to++ = 145;
				break;
			case 179: *to++ = 149;
				break;
			case 180: *to++ = 142;
				break;
			case 181: *to++ = 146;
				break;
			case 182: *to++ = 152;
				break;
			case 183: *to++ = 191;
				break;
			case 184: *to++ = 207;
				break;
			case 185: *to++ = 211;
				break;
			case 186: *to++ = 215;
				break;
			case 187: *to++ = 208;
				break;
			case 188: *to++ = 212;
				break;
			case 189: *to++ = 216;
				break;
			case 190: *to++ = 209;
				break;
			case 191: *to++ = 213;
				break;
			case 192: *to++ = 217;
				break;
			case 193: *to++ = 210;
				break;
			case 194: *to++ = 226;
				break;
			case 195: *to++ = 218;
				break;
			case 196: *to++ = 95;
				break;
			case 197: *to++ = 223;
				break;
			case 198: *to++ = 227;
				break;
			case 199: *to++ = 220;
				break;
			case 200: *to++ = 224;
				break;
			case 201: *to++ = 228;
				break;
			case 202: *to++ = 221;
				break;
			case 203: *to++ = 225;
				break;
			case 204: *to++ = 229;
				break;
			case 205: *to++ = 222;
				break;
			case 206: *to++ = 226;
				break;
			case 207: *to++ = 230;
				break;
			case 208: *to++ = 233;
				break;
			case 209: *to++ = 236;
				break;
			case 210: *to++ = 231;
				break;
			case 211: *to++ = 234;
				break;
			case 212: *to++ = 237;
				break;
			case 213: *to++ = 232;
				break;
			case 214: *to++ = 235;
				break;
			case 215: *to++ = 206;
				break;
			case 216: *to++ = 195;
				break;
			case 217: *to++ = 199;
				break;
			case 218: *to++ = 192;
				break;
			case219: *to++ = 219;
				break;
			case 220: *to++ = 200;
				break;
			case 221: *to++ = 193;
				break;
			case 222: *to++ = 197;
				break;
			case 223: *to++ = 201;
				break;
			case 224: *to++ = 194;
				break;
			case 225: *to++ = 198;
				break;
			case 226: *to++ = 202;
				break;
			case 227: *to++ = 203;
				break;
			case 228: *to++ = 204;
				break;
			case 229: *to++ = 205;
				break;
			case 230: *to++ = 243;
				break;
			case 231: *to++ = 247;
				break;
			case 232: *to++ = 240;
				break;
			case 233: *to++ = 244;
				break;
			case 234: *to++ = 248;
				break;
			case 235: *to++ = 241;
				break;
			case 236: *to++ = 245;
				break;
			case 237: *to++ = 249;
				break;
			case 238: *to++ = 242;
				break;
			case 239: *to++ = 246;
				break;
			case 240: *to++ = 250;
				break;
			case 241: *to++ = 255;
				break;
			case 242: *to++ = 165;
				break;
			case 243: *to++ = 252;
				break;
			case 244: *to++ = 161;
				break;
			case 245: *to++ = 166;
				break;
			case 246: *to++ = 253;
				break;
			case 247: *to++ = 162;
				break;
			case 248: *to++ = 167;
				break;
			case 249: *to++ = 254;
				break;
			case 250: *to++ = 163;
				break;
			case 251: *to++ = 168;
				break;
			case 252: *to++ = 101;
				*to++ = 94;
				break;
			case 253: *to++ = 111;
				*to++ = 94;
				break;		
			default: *to++ = *from;
				break;
		}
	}
	*to = 0;
	return 0;
}
