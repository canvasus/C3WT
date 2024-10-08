/////////////////////////////////////////////////////
//
// Positive 256-byte Sine Wave
//
//  The MIT License (MIT)
//  
//  Copyright (c) 2013 Robert W. Gallup (www.robertgallup.com)
//  
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//  
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
// 
#include <Arduino.h>

const PROGMEM byte sine256[] ={

	128,	     // 0
	131,	     // 1
	134,	     // 2
	137,	     // 3
	140,	     // 4
	143,	     // 5
	146,	     // 6
	149,	     // 7
	153,	     // 8
	156,	     // 9
	159,	     // 10
	162,	     // 11
	165,	     // 12
	168,	     // 13
	171,	     // 14
	174,	     // 15
	177,	     // 16
	180,	     // 17
	182,	     // 18
	185,	     // 19
	188,	     // 20
	191,	     // 21
	194,	     // 22
	196,	     // 23
	199,	     // 24
	201,	     // 25
	204,	     // 26
	207,	     // 27
	209,	     // 28
	211,	     // 29
	214,	     // 30
	216,	     // 31
	218,	     // 32
	220,	     // 33
	223,	     // 34
	225,	     // 35
	227,	     // 36
	229,	     // 37
	231,	     // 38
	232,	     // 39
	234,	     // 40
	236,	     // 41
	238,	     // 42
	239,	     // 43
	241,	     // 44
	242,	     // 45
	243,	     // 46
	245,	     // 47
	246,	     // 48
	247,	     // 49
	248,	     // 50
	249,	     // 51
	250,	     // 52
	251,	     // 53
	252,	     // 54
	253,	     // 55
	253,	     // 56
	254,	     // 57
	254,	     // 58
	255,	     // 59
	255,	     // 60
	255,	     // 61
	255,	     // 62
	255,	     // 63
	255,	     // 64
	255,	     // 65
	255,	     // 66
	255,	     // 67
	255,	     // 68
	254,	     // 69
	254,	     // 70
	253,	     // 71
	253,	     // 72
	252,	     // 73
	251,	     // 74
	251,	     // 75
	250,	     // 76
	249,	     // 77
	248,	     // 78
	247,	     // 79
	245,	     // 80
	244,	     // 81
	243,	     // 82
	241,	     // 83
	240,	     // 84
	238,	     // 85
	237,	     // 86
	235,	     // 87
	233,	     // 88
	232,	     // 89
	230,	     // 90
	228,	     // 91
	226,	     // 92
	224,	     // 93
	222,	     // 94
	219,	     // 95
	217,	     // 96
	215,	     // 97
	213,	     // 98
	210,	     // 99
	208,	     // 100
	205,	     // 101
	203,	     // 102
	200,	     // 103
	198,	     // 104
	195,	     // 105
	192,	     // 106
	189,	     // 107
	187,	     // 108
	184,	     // 109
	181,	     // 110
	178,	     // 111
	175,	     // 112
	172,	     // 113
	169,	     // 114
	166,	     // 115
	163,	     // 116
	160,	     // 117
	157,	     // 118
	154,	     // 119
	151,	     // 120
	148,	     // 121
	145,	     // 122
	142,	     // 123
	139,	     // 124
	135,	     // 125
	132,	     // 126
	129,	     // 127
	126,	     // 128
	123,	     // 129
	120,	     // 130
	116,	     // 131
	113,	     // 132
	110,	     // 133
	107,	     // 134
	104,	     // 135
	101,	     // 136
	98,	     // 137
	95,	     // 138
	92,	     // 139
	89,	     // 140
	86,	     // 141
	83,	     // 142
	80,	     // 143
	77,	     // 144
	74,	     // 145
	71,	     // 146
	68,	     // 147
	66,	     // 148
	63,	     // 149
	60,	     // 150
	57,	     // 151
	55,	     // 152
	52,	     // 153
	50,	     // 154
	47,	     // 155
	45,	     // 156
	42,	     // 157
	40,	     // 158
	38,	     // 159
	36,	     // 160
	33,	     // 161
	31,	     // 162
	29,	     // 163
	27,	     // 164
	25,	     // 165
	23,	     // 166
	22,	     // 167
	20,	     // 168
	18,	     // 169
	17,	     // 170
	15,	     // 171
	14,	     // 172
	12,	     // 173
	11,	     // 174
	10,	     // 175
	8,	     // 176
	7,	     // 177
	6,	     // 178
	5,	     // 179
	4,	     // 180
	4,	     // 181
	3,	     // 182
	2,	     // 183
	2,	     // 184
	1,	     // 185
	1,	     // 186
	0,	     // 187
	0,	     // 188
	0,	     // 189
	0,	     // 190
	0,	     // 191
	0,	     // 192
	0,	     // 193
	0,	     // 194
	0,	     // 195
	0,	     // 196
	1,	     // 197
	1,	     // 198
	2,	     // 199
	2,	     // 200
	3,	     // 201
	4,	     // 202
	5,	     // 203
	6,	     // 204
	7,	     // 205
	8,	     // 206
	9,	     // 207
	10,	     // 208
	12,	     // 209
	13,	     // 210
	14,	     // 211
	16,	     // 212
	17,	     // 213
	19,	     // 214
	21,	     // 215
	23,	     // 216
	24,	     // 217
	26,	     // 218
	28,	     // 219
	30,	     // 220
	32,	     // 221
	35,	     // 222
	37,	     // 223
	39,	     // 224
	41,	     // 225
	44,	     // 226
	46,	     // 227
	48,	     // 228
	51,	     // 229
	54,	     // 230
	56,	     // 231
	59,	     // 232
	61,	     // 233
	64,	     // 234
	67,	     // 235
	70,	     // 236
	73,	     // 237
	75,	     // 238
	78,	     // 239
	81,	     // 240
	84,	     // 241
	87,	     // 242
	90,	     // 243
	93,	     // 244
	96,	     // 245
	99,	     // 246
	102,	     // 247
	106,	     // 248
	109,	     // 249
	112,	     // 250
	115,	     // 251
	118,	     // 252
	121,	     // 253
	124,	     // 254
	127,	     // 255

};
