begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|GREP_H
end_ifndef
begin_define
DECL|macro|GREP_H
define|#
directive|define
name|GREP_H
end_define
begin_include
include|#
directive|include
file|"color.h"
end_include
begin_enum
DECL|enum|grep_pat_token
enum|enum
name|grep_pat_token
block|{
DECL|enumerator|GREP_PATTERN
name|GREP_PATTERN
block|,
DECL|enumerator|GREP_PATTERN_HEAD
name|GREP_PATTERN_HEAD
block|,
DECL|enumerator|GREP_PATTERN_BODY
name|GREP_PATTERN_BODY
block|,
DECL|enumerator|GREP_AND
name|GREP_AND
block|,
DECL|enumerator|GREP_OPEN_PAREN
name|GREP_OPEN_PAREN
block|,
DECL|enumerator|GREP_CLOSE_PAREN
name|GREP_CLOSE_PAREN
block|,
DECL|enumerator|GREP_NOT
name|GREP_NOT
block|,
DECL|enumerator|GREP_OR
name|GREP_OR
block|}
enum|;
end_enum
begin_enum
DECL|enum|grep_context
enum|enum
name|grep_context
block|{
DECL|enumerator|GREP_CONTEXT_HEAD
name|GREP_CONTEXT_HEAD
block|,
DECL|enumerator|GREP_CONTEXT_BODY
name|GREP_CONTEXT_BODY
block|}
enum|;
end_enum
begin_enum
DECL|enum|grep_header_field
enum|enum
name|grep_header_field
block|{
DECL|enumerator|GREP_HEADER_AUTHOR
name|GREP_HEADER_AUTHOR
init|=
literal|0
block|,
DECL|enumerator|GREP_HEADER_COMMITTER
name|GREP_HEADER_COMMITTER
block|}
enum|;
end_enum
begin_define
DECL|macro|GREP_HEADER_FIELD_MAX
define|#
directive|define
name|GREP_HEADER_FIELD_MAX
value|(GREP_HEADER_COMMITTER + 1)
end_define
begin_struct
DECL|struct|grep_pat
struct|struct
name|grep_pat
block|{
DECL|member|next
name|struct
name|grep_pat
modifier|*
name|next
decl_stmt|;
DECL|member|origin
specifier|const
name|char
modifier|*
name|origin
decl_stmt|;
DECL|member|no
name|int
name|no
decl_stmt|;
DECL|member|token
name|enum
name|grep_pat_token
name|token
decl_stmt|;
DECL|member|pattern
specifier|const
name|char
modifier|*
name|pattern
decl_stmt|;
DECL|member|patternlen
name|size_t
name|patternlen
decl_stmt|;
DECL|member|field
name|enum
name|grep_header_field
name|field
decl_stmt|;
DECL|member|regexp
name|regex_t
name|regexp
decl_stmt|;
DECL|member|fixed
name|unsigned
name|fixed
range|:
literal|1
decl_stmt|;
DECL|member|ignore_case
name|unsigned
name|ignore_case
range|:
literal|1
decl_stmt|;
DECL|member|word_regexp
name|unsigned
name|word_regexp
range|:
literal|1
decl_stmt|;
block|}
struct|;
end_struct
begin_enum
DECL|enum|grep_expr_node
enum|enum
name|grep_expr_node
block|{
DECL|enumerator|GREP_NODE_ATOM
name|GREP_NODE_ATOM
block|,
DECL|enumerator|GREP_NODE_NOT
name|GREP_NODE_NOT
block|,
DECL|enumerator|GREP_NODE_AND
name|GREP_NODE_AND
block|,
DECL|enumerator|GREP_NODE_TRUE
name|GREP_NODE_TRUE
block|,
DECL|enumerator|GREP_NODE_OR
name|GREP_NODE_OR
block|}
enum|;
end_enum
begin_struct
DECL|struct|grep_expr
struct|struct
name|grep_expr
block|{
DECL|member|node
name|enum
name|grep_expr_node
name|node
decl_stmt|;
DECL|member|hit
name|unsigned
name|hit
decl_stmt|;
union|union
block|{
DECL|member|atom
name|struct
name|grep_pat
modifier|*
name|atom
decl_stmt|;
DECL|member|unary
name|struct
name|grep_expr
modifier|*
name|unary
decl_stmt|;
struct|struct
block|{
DECL|member|left
name|struct
name|grep_expr
modifier|*
name|left
decl_stmt|;
DECL|member|right
name|struct
name|grep_expr
modifier|*
name|right
decl_stmt|;
block|}
DECL|member|binary
name|binary
struct|;
block|}
DECL|member|u
name|u
union|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|grep_opt
struct|struct
name|grep_opt
block|{
DECL|member|pattern_list
name|struct
name|grep_pat
modifier|*
name|pattern_list
decl_stmt|;
DECL|member|pattern_tail
name|struct
name|grep_pat
modifier|*
modifier|*
name|pattern_tail
decl_stmt|;
DECL|member|header_list
name|struct
name|grep_pat
modifier|*
name|header_list
decl_stmt|;
DECL|member|header_tail
name|struct
name|grep_pat
modifier|*
modifier|*
name|header_tail
decl_stmt|;
DECL|member|pattern_expression
name|struct
name|grep_expr
modifier|*
name|pattern_expression
decl_stmt|;
DECL|member|prefix
specifier|const
name|char
modifier|*
name|prefix
decl_stmt|;
DECL|member|prefix_length
name|int
name|prefix_length
decl_stmt|;
DECL|member|regexp
name|regex_t
name|regexp
decl_stmt|;
DECL|member|linenum
name|int
name|linenum
decl_stmt|;
DECL|member|invert
name|int
name|invert
decl_stmt|;
DECL|member|ignore_case
name|int
name|ignore_case
decl_stmt|;
DECL|member|status_only
name|int
name|status_only
decl_stmt|;
DECL|member|name_only
name|int
name|name_only
decl_stmt|;
DECL|member|unmatch_name_only
name|int
name|unmatch_name_only
decl_stmt|;
DECL|member|count
name|int
name|count
decl_stmt|;
DECL|member|word_regexp
name|int
name|word_regexp
decl_stmt|;
DECL|member|fixed
name|int
name|fixed
decl_stmt|;
DECL|member|all_match
name|int
name|all_match
decl_stmt|;
DECL|macro|GREP_BINARY_DEFAULT
define|#
directive|define
name|GREP_BINARY_DEFAULT
value|0
DECL|macro|GREP_BINARY_NOMATCH
define|#
directive|define
name|GREP_BINARY_NOMATCH
value|1
DECL|macro|GREP_BINARY_TEXT
define|#
directive|define
name|GREP_BINARY_TEXT
value|2
DECL|member|binary
name|int
name|binary
decl_stmt|;
DECL|member|extended
name|int
name|extended
decl_stmt|;
DECL|member|relative
name|int
name|relative
decl_stmt|;
DECL|member|pathname
name|int
name|pathname
decl_stmt|;
DECL|member|null_following_name
name|int
name|null_following_name
decl_stmt|;
DECL|member|color
name|int
name|color
decl_stmt|;
DECL|member|max_depth
name|int
name|max_depth
decl_stmt|;
DECL|member|funcname
name|int
name|funcname
decl_stmt|;
DECL|member|color_context
name|char
name|color_context
index|[
name|COLOR_MAXLEN
index|]
decl_stmt|;
DECL|member|color_filename
name|char
name|color_filename
index|[
name|COLOR_MAXLEN
index|]
decl_stmt|;
DECL|member|color_function
name|char
name|color_function
index|[
name|COLOR_MAXLEN
index|]
decl_stmt|;
DECL|member|color_lineno
name|char
name|color_lineno
index|[
name|COLOR_MAXLEN
index|]
decl_stmt|;
DECL|member|color_match
name|char
name|color_match
index|[
name|COLOR_MAXLEN
index|]
decl_stmt|;
DECL|member|color_selected
name|char
name|color_selected
index|[
name|COLOR_MAXLEN
index|]
decl_stmt|;
DECL|member|color_sep
name|char
name|color_sep
index|[
name|COLOR_MAXLEN
index|]
decl_stmt|;
DECL|member|regflags
name|int
name|regflags
decl_stmt|;
DECL|member|pre_context
name|unsigned
name|pre_context
decl_stmt|;
DECL|member|post_context
name|unsigned
name|post_context
decl_stmt|;
DECL|member|last_shown
name|unsigned
name|last_shown
decl_stmt|;
DECL|member|show_hunk_mark
name|int
name|show_hunk_mark
decl_stmt|;
DECL|member|priv
name|void
modifier|*
name|priv
decl_stmt|;
DECL|member|output
name|void
function_decl|(
modifier|*
name|output
function_decl|)
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|,
specifier|const
name|void
modifier|*
name|data
parameter_list|,
name|size_t
name|size
parameter_list|)
function_decl|;
DECL|member|output_priv
name|void
modifier|*
name|output_priv
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
specifier|extern
name|void
name|append_grep_pat
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|pat
parameter_list|,
name|size_t
name|patlen
parameter_list|,
specifier|const
name|char
modifier|*
name|origin
parameter_list|,
name|int
name|no
parameter_list|,
name|enum
name|grep_pat_token
name|t
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|append_grep_pattern
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|pat
parameter_list|,
specifier|const
name|char
modifier|*
name|origin
parameter_list|,
name|int
name|no
parameter_list|,
name|enum
name|grep_pat_token
name|t
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|append_header_grep_pattern
parameter_list|(
name|struct
name|grep_opt
modifier|*
parameter_list|,
name|enum
name|grep_header_field
parameter_list|,
specifier|const
name|char
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|compile_grep_patterns
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|free_grep_patterns
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|grep_buffer
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|char
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|struct
name|grep_opt
modifier|*
name|grep_opt_dup
parameter_list|(
specifier|const
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|grep_threads_ok
parameter_list|(
specifier|const
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
end_unit
