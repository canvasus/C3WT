/////////////////////////////////////////////////////
//
//  256-byte Pulse Wave (64-byte pulse)
//
//  The MIT License (MIT)
//  
//  Copyright (c) 2013 Robert W. Gallup (www.robertgallup.com)
//  
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the Software), to deal
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

const PROGMEM byte pulse64[] = {

	255,	     // 0
	255,	     // 1
	255,	     // 2
	255,	     // 3
	255,	     // 4
	255,	     // 5
	255,	     // 6
	255,	     // 7
	255,	     // 8
	255,	     // 9
	255,	     // 10
	255,	     // 11
	255,	     // 12
	255,	     // 13
	255,	     // 14
	255,	     // 15
	255,	     // 16
	255,	     // 17
	255,	     // 18
	255,	     // 19
	255,	     // 20
	255,	     // 21
	255,	     // 22
	255,	     // 23
	255,	     // 24
	255,	     // 25
	255,	     // 26
	255,	     // 27
	255,	     // 28
	255,	     // 29
	255,	     // 30
	255,	     // 31
	255,	     // 32
	255,	     // 33
	255,	     // 34
	255,	     // 35
	255,	     // 36
	255,	     // 37
	255,	     // 38
	255,	     // 39
	255,	     // 40
	255,	     // 41
	255,	     // 42
	255,	     // 43
	255,	     // 44
	255,	     // 45
	255,	     // 46
	255,	     // 47
	255,	     // 48
	255,	     // 49
	255,	     // 50
	255,	     // 51
	255,	     // 52
	255,	     // 53
	255,	     // 54
	255,	     // 55
	255,	     // 56
	255,	     // 57
	255,	     // 58
	255,	     // 59
	255,	     // 60
	255,	     // 61
	255,	     // 62
	255,	     // 63
	0,	     // 64
	0,	     // 65
	0,	     // 66
	0,	     // 67
	0,	     // 68
	0,	     // 69
	0,	     // 70
	0,	     // 71
	0,	     // 72
	0,	     // 73
	0,	     // 74
	0,	     // 75
	0,	     // 76
	0,	     // 77
	0,	     // 78
	0,	     // 79
	0,	     // 80
	0,	     // 81
	0,	     // 82
	0,	     // 83
	0,	     // 84
	0,	     // 85
	0,	     // 86
	0,	     // 87
	0,	     // 88
	0,	     // 89
	0,	     // 90
	0,	     // 91
	0,	     // 92
	0,	     // 93
	0,	     // 94
	0,	     // 95
	0,	     // 96
	0,	     // 97
	0,	     // 98
	0,	     // 99
	0,	     // 100
	0,	     // 101
	0,	     // 102
	0,	     // 103
	0,	     // 104
	0,	     // 105
	0,	     // 106
	0,	     // 107
	0,	     // 108
	0,	     // 109
	0,	     // 110
	0,	     // 111
	0,	     // 112
	0,	     // 113
	0,	     // 114
	0,	     // 115
	0,	     // 116
	0,	     // 117
	0,	     // 118
	0,	     // 119
	0,	     // 120
	0,	     // 121
	0,	     // 122
	0,	     // 123
	0,	     // 124
	0,	     // 125
	0,	     // 126
	0,	     // 127
	0,	     // 128
	0,	     // 129
	0,	     // 130
	0,	     // 131
	0,	     // 132
	0,	     // 133
	0,	     // 134
	0,	     // 135
	0,	     // 136
	0,	     // 137
	0,	     // 138
	0,	     // 139
	0,	     // 140
	0,	     // 141
	0,	     // 142
	0,	     // 143
	0,	     // 144
	0,	     // 145
	0,	     // 146
	0,	     // 147
	0,	     // 148
	0,	     // 149
	0,	     // 150
	0,	     // 151
	0,	     // 152
	0,	     // 153
	0,	     // 154
	0,	     // 155
	0,	     // 156
	0,	     // 157
	0,	     // 158
	0,	     // 159
	0,	     // 160
	0,	     // 161
	0,	     // 162
	0,	     // 163
	0,	     // 164
	0,	     // 165
	0,	     // 166
	0,	     // 167
	0,	     // 168
	0,	     // 169
	0,	     // 170
	0,	     // 171
	0,	     // 172
	0,	     // 173
	0,	     // 174
	0,	     // 175
	0,	     // 176
	0,	     // 177
	0,	     // 178
	0,	     // 179
	0,	     // 180
	0,	     // 181
	0,	     // 182
	0,	     // 183
	0,	     // 184
	0,	     // 185
	0,	     // 186
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
	0,	     // 197
	0,	     // 198
	0,	     // 199
	0,	     // 200
	0,	     // 201
	0,	     // 202
	0,	     // 203
	0,	     // 204
	0,	     // 205
	0,	     // 206
	0,	     // 207
	0,	     // 208
	0,	     // 209
	0,	     // 210
	0,	     // 211
	0,	     // 212
	0,	     // 213
	0,	     // 214
	0,	     // 215
	0,	     // 216
	0,	     // 217
	0,	     // 218
	0,	     // 219
	0,	     // 220
	0,	     // 221
	0,	     // 222
	0,	     // 223
	0,	     // 224
	0,	     // 225
	0,	     // 226
	0,	     // 227
	0,	     // 228
	0,	     // 229
	0,	     // 230
	0,	     // 231
	0,	     // 232
	0,	     // 233
	0,	     // 234
	0,	     // 235
	0,	     // 236
	0,	     // 237
	0,	     // 238
	0,	     // 239
	0,	     // 240
	0,	     // 241
	0,	     // 242
	0,	     // 243
	0,	     // 244
	0,	     // 245
	0,	     // 246
	0,	     // 247
	0,	     // 248
	0,	     // 249
	0,	     // 250
	0,	     // 251
	0,	     // 252
	0,	     // 253
	0,	     // 254
	0	     // 255

};
