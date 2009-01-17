begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|COLOR_H
end_ifndef
begin_define
DECL|macro|COLOR_H
define|#
directive|define
name|COLOR_H
end_define
begin_comment
comment|/* "\033[1;38;5;2xx;48;5;2xxm\0" is 23 bytes */
end_comment
begin_define
DECL|macro|COLOR_MAXLEN
define|#
directive|define
name|COLOR_MAXLEN
value|24
end_define
begin_comment
comment|/*  * This variable stores the value of color.ui  */
end_comment
begin_decl_stmt
specifier|extern
name|int
name|git_use_color_default
decl_stmt|;
end_decl_stmt
begin_comment
comment|/*  * Use this instead of git_default_config if you need the value of color.ui.  */
end_comment
begin_function_decl
name|int
name|git_color_default_config
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|,
name|void
modifier|*
name|cb
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|git_config_colorbool
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|,
name|int
name|stdout_is_tty
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|color_parse
parameter_list|(
specifier|const
name|char
modifier|*
name|value
parameter_list|,
specifier|const
name|char
modifier|*
name|var
parameter_list|,
name|char
modifier|*
name|dst
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|color_parse_mem
parameter_list|(
specifier|const
name|char
modifier|*
name|value
parameter_list|,
name|int
name|len
parameter_list|,
specifier|const
name|char
modifier|*
name|var
parameter_list|,
name|char
modifier|*
name|dst
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|color_fprintf
parameter_list|(
name|FILE
modifier|*
name|fp
parameter_list|,
specifier|const
name|char
modifier|*
name|color
parameter_list|,
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|color_fprintf_ln
parameter_list|(
name|FILE
modifier|*
name|fp
parameter_list|,
specifier|const
name|char
modifier|*
name|color
parameter_list|,
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* COLOR_H */
end_comment
end_unit
