begin_unit
begin_comment
comment|/*  * Sane locale-independent, ASCII ctype.  *  * No surprises, and works with signed and unsigned chars.  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_enum
enum|enum
block|{
DECL|enumerator|S
name|S
init|=
name|GIT_SPACE
block|,
DECL|enumerator|A
name|A
init|=
name|GIT_ALPHA
block|,
DECL|enumerator|D
name|D
init|=
name|GIT_DIGIT
block|,
DECL|enumerator|G
name|G
init|=
name|GIT_GLOB_SPECIAL
block|,
comment|/* *, ?, [, \\ */
DECL|enumerator|R
name|R
init|=
name|GIT_REGEX_SPECIAL
block|,
comment|/* $, (, ), +, ., ^, {, | */
DECL|enumerator|P
name|P
init|=
name|GIT_PATHSPEC_MAGIC
comment|/* other non-alnum, except for ] and } */
block|}
enum|;
end_enum
begin_decl_stmt
DECL|variable|sane_ctype
name|unsigned
name|char
name|sane_ctype
index|[
literal|256
index|]
init|=
block|{
literal|0
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|,
name|S
block|,
name|S
block|,
literal|0
block|,
literal|0
block|,
name|S
block|,
literal|0
block|,
literal|0
block|,
comment|/*   0.. 15 */
literal|0
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|,
literal|0
block|,
comment|/*  16.. 31 */
name|S
block|,
name|P
block|,
name|P
block|,
name|P
block|,
name|R
block|,
name|P
block|,
name|P
block|,
name|P
block|,
name|R
block|,
name|R
block|,
name|G
block|,
name|R
block|,
name|P
block|,
name|P
block|,
name|R
block|,
name|P
block|,
comment|/*  32.. 47 */
name|D
block|,
name|D
block|,
name|D
block|,
name|D
block|,
name|D
block|,
name|D
block|,
name|D
block|,
name|D
block|,
name|D
block|,
name|D
block|,
name|P
block|,
name|P
block|,
name|P
block|,
name|P
block|,
name|P
block|,
name|G
block|,
comment|/*  48.. 63 */
name|P
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
comment|/*  64.. 79 */
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|G
block|,
name|G
block|,
literal|0
block|,
name|R
block|,
name|P
block|,
comment|/*  80.. 95 */
name|P
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
comment|/*  96..111 */
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|A
block|,
name|R
block|,
name|R
block|,
literal|0
block|,
name|P
block|,
literal|0
block|,
comment|/* 112..127 */
comment|/* Nothing in the 128.. range */
block|}
decl_stmt|;
end_decl_stmt
begin_comment
comment|/* For case-insensitive kwset */
end_comment
begin_decl_stmt
DECL|variable|tolower_trans_tbl
specifier|const
name|char
name|tolower_trans_tbl
index|[
literal|256
index|]
init|=
block|{
literal|0x00
block|,
literal|0x01
block|,
literal|0x02
block|,
literal|0x03
block|,
literal|0x04
block|,
literal|0x05
block|,
literal|0x06
block|,
literal|0x07
block|,
literal|0x08
block|,
literal|0x09
block|,
literal|0x0a
block|,
literal|0x0b
block|,
literal|0x0c
block|,
literal|0x0d
block|,
literal|0x0e
block|,
literal|0x0f
block|,
literal|0x10
block|,
literal|0x11
block|,
literal|0x12
block|,
literal|0x13
block|,
literal|0x14
block|,
literal|0x15
block|,
literal|0x16
block|,
literal|0x17
block|,
literal|0x18
block|,
literal|0x19
block|,
literal|0x1a
block|,
literal|0x1b
block|,
literal|0x1c
block|,
literal|0x1d
block|,
literal|0x1e
block|,
literal|0x1f
block|,
literal|' '
block|,
literal|'!'
block|,
literal|'"'
block|,
literal|'#'
block|,
literal|'$'
block|,
literal|'%'
block|,
literal|'&'
block|,
literal|0x27
block|,
literal|'('
block|,
literal|')'
block|,
literal|'*'
block|,
literal|'+'
block|,
literal|','
block|,
literal|'-'
block|,
literal|'.'
block|,
literal|'/'
block|,
literal|'0'
block|,
literal|'1'
block|,
literal|'2'
block|,
literal|'3'
block|,
literal|'4'
block|,
literal|'5'
block|,
literal|'6'
block|,
literal|'7'
block|,
literal|'8'
block|,
literal|'9'
block|,
literal|':'
block|,
literal|';'
block|,
literal|'<'
block|,
literal|'='
block|,
literal|'>'
block|,
literal|'?'
block|,
literal|'@'
block|,
literal|'a'
block|,
literal|'b'
block|,
literal|'c'
block|,
literal|'d'
block|,
literal|'e'
block|,
literal|'f'
block|,
literal|'g'
block|,
literal|'h'
block|,
literal|'i'
block|,
literal|'j'
block|,
literal|'k'
block|,
literal|'l'
block|,
literal|'m'
block|,
literal|'n'
block|,
literal|'o'
block|,
literal|'p'
block|,
literal|'q'
block|,
literal|'r'
block|,
literal|'s'
block|,
literal|'t'
block|,
literal|'u'
block|,
literal|'v'
block|,
literal|'w'
block|,
literal|'x'
block|,
literal|'y'
block|,
literal|'z'
block|,
literal|'['
block|,
literal|0x5c
block|,
literal|']'
block|,
literal|'^'
block|,
literal|'_'
block|,
literal|'`'
block|,
literal|'a'
block|,
literal|'b'
block|,
literal|'c'
block|,
literal|'d'
block|,
literal|'e'
block|,
literal|'f'
block|,
literal|'g'
block|,
literal|'h'
block|,
literal|'i'
block|,
literal|'j'
block|,
literal|'k'
block|,
literal|'l'
block|,
literal|'m'
block|,
literal|'n'
block|,
literal|'o'
block|,
literal|'p'
block|,
literal|'q'
block|,
literal|'r'
block|,
literal|'s'
block|,
literal|'t'
block|,
literal|'u'
block|,
literal|'v'
block|,
literal|'w'
block|,
literal|'x'
block|,
literal|'y'
block|,
literal|'z'
block|,
literal|'{'
block|,
literal|'|'
block|,
literal|'}'
block|,
literal|'~'
block|,
literal|0x7f
block|,
literal|0x80
block|,
literal|0x81
block|,
literal|0x82
block|,
literal|0x83
block|,
literal|0x84
block|,
literal|0x85
block|,
literal|0x86
block|,
literal|0x87
block|,
literal|0x88
block|,
literal|0x89
block|,
literal|0x8a
block|,
literal|0x8b
block|,
literal|0x8c
block|,
literal|0x8d
block|,
literal|0x8e
block|,
literal|0x8f
block|,
literal|0x90
block|,
literal|0x91
block|,
literal|0x92
block|,
literal|0x93
block|,
literal|0x94
block|,
literal|0x95
block|,
literal|0x96
block|,
literal|0x97
block|,
literal|0x98
block|,
literal|0x99
block|,
literal|0x9a
block|,
literal|0x9b
block|,
literal|0x9c
block|,
literal|0x9d
block|,
literal|0x9e
block|,
literal|0x9f
block|,
literal|0xa0
block|,
literal|0xa1
block|,
literal|0xa2
block|,
literal|0xa3
block|,
literal|0xa4
block|,
literal|0xa5
block|,
literal|0xa6
block|,
literal|0xa7
block|,
literal|0xa8
block|,
literal|0xa9
block|,
literal|0xaa
block|,
literal|0xab
block|,
literal|0xac
block|,
literal|0xad
block|,
literal|0xae
block|,
literal|0xaf
block|,
literal|0xb0
block|,
literal|0xb1
block|,
literal|0xb2
block|,
literal|0xb3
block|,
literal|0xb4
block|,
literal|0xb5
block|,
literal|0xb6
block|,
literal|0xb7
block|,
literal|0xb8
block|,
literal|0xb9
block|,
literal|0xba
block|,
literal|0xbb
block|,
literal|0xbc
block|,
literal|0xbd
block|,
literal|0xbe
block|,
literal|0xbf
block|,
literal|0xc0
block|,
literal|0xc1
block|,
literal|0xc2
block|,
literal|0xc3
block|,
literal|0xc4
block|,
literal|0xc5
block|,
literal|0xc6
block|,
literal|0xc7
block|,
literal|0xc8
block|,
literal|0xc9
block|,
literal|0xca
block|,
literal|0xcb
block|,
literal|0xcc
block|,
literal|0xcd
block|,
literal|0xce
block|,
literal|0xcf
block|,
literal|0xd0
block|,
literal|0xd1
block|,
literal|0xd2
block|,
literal|0xd3
block|,
literal|0xd4
block|,
literal|0xd5
block|,
literal|0xd6
block|,
literal|0xd7
block|,
literal|0xd8
block|,
literal|0xd9
block|,
literal|0xda
block|,
literal|0xdb
block|,
literal|0xdc
block|,
literal|0xdd
block|,
literal|0xde
block|,
literal|0xdf
block|,
literal|0xe0
block|,
literal|0xe1
block|,
literal|0xe2
block|,
literal|0xe3
block|,
literal|0xe4
block|,
literal|0xe5
block|,
literal|0xe6
block|,
literal|0xe7
block|,
literal|0xe8
block|,
literal|0xe9
block|,
literal|0xea
block|,
literal|0xeb
block|,
literal|0xec
block|,
literal|0xed
block|,
literal|0xee
block|,
literal|0xef
block|,
literal|0xf0
block|,
literal|0xf1
block|,
literal|0xf2
block|,
literal|0xf3
block|,
literal|0xf4
block|,
literal|0xf5
block|,
literal|0xf6
block|,
literal|0xf7
block|,
literal|0xf8
block|,
literal|0xf9
block|,
literal|0xfa
block|,
literal|0xfb
block|,
literal|0xfc
block|,
literal|0xfd
block|,
literal|0xfe
block|,
literal|0xff
block|, }
decl_stmt|;
end_decl_stmt
end_unit
