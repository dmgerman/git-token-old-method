begin_unit
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"strbuf.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_include
include|#
directive|include
file|"string-list.h"
end_include
begin_include
include|#
directive|include
file|"column.h"
end_include
begin_decl_stmt
DECL|variable|builtin_column_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|builtin_column_usage
index|[]
init|=
block|{
name|N_
argument_list|(
literal|"git column [options]"
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|colopts
specifier|static
name|unsigned
name|int
name|colopts
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|column_config
specifier|static
name|int
name|column_config
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
block|{
return|return
name|git_column_config
argument_list|(
name|var
argument_list|,
name|value
argument_list|,
name|cb
argument_list|,
operator|&
name|colopts
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|cmd_column
name|int
name|cmd_column
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
name|struct
name|string_list
name|list
init|=
name|STRING_LIST_INIT_DUP
decl_stmt|;
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|column_options
name|copts
decl_stmt|;
specifier|const
name|char
modifier|*
name|command
init|=
name|NULL
decl_stmt|,
modifier|*
name|real_command
init|=
name|NULL
decl_stmt|;
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"command"
argument_list|,
operator|&
name|real_command
argument_list|,
name|N_
argument_list|(
literal|"name"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"lookup config vars"
argument_list|)
argument_list|)
block|,
name|OPT_COLUMN
argument_list|(
literal|0
argument_list|,
literal|"mode"
argument_list|,
operator|&
name|colopts
argument_list|,
name|N_
argument_list|(
literal|"layout to use"
argument_list|)
argument_list|)
block|,
name|OPT_INTEGER
argument_list|(
literal|0
argument_list|,
literal|"raw-mode"
argument_list|,
operator|&
name|colopts
argument_list|,
name|N_
argument_list|(
literal|"layout to use"
argument_list|)
argument_list|)
block|,
name|OPT_INTEGER
argument_list|(
literal|0
argument_list|,
literal|"width"
argument_list|,
operator|&
name|copts
operator|.
name|width
argument_list|,
name|N_
argument_list|(
literal|"Maximum width"
argument_list|)
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"indent"
argument_list|,
operator|&
name|copts
operator|.
name|indent
argument_list|,
name|N_
argument_list|(
literal|"string"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"Padding space on left border"
argument_list|)
argument_list|)
block|,
name|OPT_INTEGER
argument_list|(
literal|0
argument_list|,
literal|"nl"
argument_list|,
operator|&
name|copts
operator|.
name|nl
argument_list|,
name|N_
argument_list|(
literal|"Padding space on right border"
argument_list|)
argument_list|)
block|,
name|OPT_INTEGER
argument_list|(
literal|0
argument_list|,
literal|"padding"
argument_list|,
operator|&
name|copts
operator|.
name|padding
argument_list|,
name|N_
argument_list|(
literal|"Padding space between columns"
argument_list|)
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
comment|/* This one is special and must be the first one */
if|if
condition|(
name|argc
operator|>
literal|1
operator|&&
operator|!
name|prefixcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"--command="
argument_list|)
condition|)
block|{
name|command
operator|=
name|argv
index|[
literal|1
index|]
operator|+
literal|10
expr_stmt|;
name|git_config
argument_list|(
name|column_config
argument_list|,
operator|(
name|void
operator|*
operator|)
name|command
argument_list|)
expr_stmt|;
block|}
else|else
name|git_config
argument_list|(
name|column_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|copts
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|copts
argument_list|)
argument_list|)
expr_stmt|;
name|copts
operator|.
name|width
operator|=
name|term_columns
argument_list|()
expr_stmt|;
name|copts
operator|.
name|padding
operator|=
literal|1
expr_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
literal|""
argument_list|,
name|options
argument_list|,
name|builtin_column_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
condition|)
name|usage_with_options
argument_list|(
name|builtin_column_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
if|if
condition|(
name|real_command
operator|||
name|command
condition|)
block|{
if|if
condition|(
operator|!
name|real_command
operator|||
operator|!
name|command
operator|||
name|strcmp
argument_list|(
name|real_command
argument_list|,
name|command
argument_list|)
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"--command must be the first argument"
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|finalize_colopts
argument_list|(
operator|&
name|colopts
argument_list|,
operator|-
literal|1
argument_list|)
expr_stmt|;
while|while
condition|(
operator|!
name|strbuf_getline
argument_list|(
operator|&
name|sb
argument_list|,
name|stdin
argument_list|,
literal|'\n'
argument_list|)
condition|)
name|string_list_append
argument_list|(
operator|&
name|list
argument_list|,
name|sb
operator|.
name|buf
argument_list|)
expr_stmt|;
name|print_columns
argument_list|(
operator|&
name|list
argument_list|,
name|colopts
argument_list|,
operator|&
name|copts
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
