begin_unit
begin_include
include|#
directive|include
file|"git-compat-util.h"
end_include
begin_include
include|#
directive|include
file|"line-range.h"
end_include
begin_include
include|#
directive|include
file|"xdiff-interface.h"
end_include
begin_include
include|#
directive|include
file|"strbuf.h"
end_include
begin_include
include|#
directive|include
file|"userdiff.h"
end_include
begin_comment
comment|/*  * Parse one item in the -L option  *  * 'begin' is applicable only to relative range anchors. Absolute anchors  * ignore this value.  *  * When parsing "-L A,B", parse_loc() is called once for A and once for B.  *  * When parsing A, 'begin' must be a negative number, the absolute value of  * which is the line at which relative start-of-range anchors should be  * based. Beginning of file is represented by -1.  *  * When parsing B, 'begin' must be the positive line number immediately  * following the line computed for 'A'.  */
end_comment
begin_function
DECL|function|parse_loc
specifier|static
specifier|const
name|char
modifier|*
name|parse_loc
parameter_list|(
specifier|const
name|char
modifier|*
name|spec
parameter_list|,
name|nth_line_fn_t
name|nth_line
parameter_list|,
name|void
modifier|*
name|data
parameter_list|,
name|long
name|lines
parameter_list|,
name|long
name|begin
parameter_list|,
name|long
modifier|*
name|ret
parameter_list|)
block|{
name|char
modifier|*
name|term
decl_stmt|;
specifier|const
name|char
modifier|*
name|line
decl_stmt|;
name|long
name|num
decl_stmt|;
name|int
name|reg_error
decl_stmt|;
name|regex_t
name|regexp
decl_stmt|;
name|regmatch_t
name|match
index|[
literal|1
index|]
decl_stmt|;
comment|/* Allow "-L<something>,+20" to mean starting at<something> 	 * for 20 lines, or "-L<something>,-5" for 5 lines ending at 	 *<something>. 	 */
if|if
condition|(
literal|1
operator|<=
name|begin
operator|&&
operator|(
name|spec
index|[
literal|0
index|]
operator|==
literal|'+'
operator|||
name|spec
index|[
literal|0
index|]
operator|==
literal|'-'
operator|)
condition|)
block|{
name|num
operator|=
name|strtol
argument_list|(
name|spec
operator|+
literal|1
argument_list|,
operator|&
name|term
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
name|term
operator|!=
name|spec
operator|+
literal|1
condition|)
block|{
if|if
condition|(
operator|!
name|ret
condition|)
return|return
name|term
return|;
if|if
condition|(
name|num
operator|==
literal|0
condition|)
name|die
argument_list|(
literal|"-L invalid empty range"
argument_list|)
expr_stmt|;
if|if
condition|(
name|spec
index|[
literal|0
index|]
operator|==
literal|'-'
condition|)
name|num
operator|=
literal|0
operator|-
name|num
expr_stmt|;
if|if
condition|(
literal|0
operator|<
name|num
condition|)
operator|*
name|ret
operator|=
name|begin
operator|+
name|num
operator|-
literal|2
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|num
condition|)
operator|*
name|ret
operator|=
name|begin
expr_stmt|;
else|else
operator|*
name|ret
operator|=
name|begin
operator|+
name|num
expr_stmt|;
return|return
name|term
return|;
block|}
return|return
name|spec
return|;
block|}
name|num
operator|=
name|strtol
argument_list|(
name|spec
argument_list|,
operator|&
name|term
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
name|term
operator|!=
name|spec
condition|)
block|{
if|if
condition|(
name|ret
condition|)
operator|*
name|ret
operator|=
name|num
expr_stmt|;
return|return
name|term
return|;
block|}
if|if
condition|(
name|begin
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|spec
index|[
literal|0
index|]
operator|!=
literal|'^'
condition|)
name|begin
operator|=
operator|-
name|begin
expr_stmt|;
else|else
block|{
name|begin
operator|=
literal|1
expr_stmt|;
name|spec
operator|++
expr_stmt|;
block|}
block|}
if|if
condition|(
name|spec
index|[
literal|0
index|]
operator|!=
literal|'/'
condition|)
return|return
name|spec
return|;
comment|/* it could be a regexp of form /.../ */
for|for
control|(
name|term
operator|=
operator|(
name|char
operator|*
operator|)
name|spec
operator|+
literal|1
init|;
operator|*
name|term
operator|&&
operator|*
name|term
operator|!=
literal|'/'
condition|;
name|term
operator|++
control|)
block|{
if|if
condition|(
operator|*
name|term
operator|==
literal|'\\'
condition|)
name|term
operator|++
expr_stmt|;
block|}
if|if
condition|(
operator|*
name|term
operator|!=
literal|'/'
condition|)
return|return
name|spec
return|;
comment|/* in the scan-only case we are not interested in the regex */
if|if
condition|(
operator|!
name|ret
condition|)
return|return
name|term
operator|+
literal|1
return|;
comment|/* try [spec+1 .. term-1] as regexp */
operator|*
name|term
operator|=
literal|0
expr_stmt|;
name|begin
operator|--
expr_stmt|;
comment|/* input is in human terms */
name|line
operator|=
name|nth_line
argument_list|(
name|data
argument_list|,
name|begin
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|reg_error
operator|=
name|regcomp
argument_list|(
operator|&
name|regexp
argument_list|,
name|spec
operator|+
literal|1
argument_list|,
name|REG_NEWLINE
argument_list|)
operator|)
operator|&&
operator|!
operator|(
name|reg_error
operator|=
name|regexec
argument_list|(
operator|&
name|regexp
argument_list|,
name|line
argument_list|,
literal|1
argument_list|,
name|match
argument_list|,
literal|0
argument_list|)
operator|)
condition|)
block|{
specifier|const
name|char
modifier|*
name|cp
init|=
name|line
operator|+
name|match
index|[
literal|0
index|]
operator|.
name|rm_so
decl_stmt|;
specifier|const
name|char
modifier|*
name|nline
decl_stmt|;
while|while
condition|(
name|begin
operator|++
operator|<
name|lines
condition|)
block|{
name|nline
operator|=
name|nth_line
argument_list|(
name|data
argument_list|,
name|begin
argument_list|)
expr_stmt|;
if|if
condition|(
name|line
operator|<=
name|cp
operator|&&
name|cp
operator|<
name|nline
condition|)
break|break;
name|line
operator|=
name|nline
expr_stmt|;
block|}
operator|*
name|ret
operator|=
name|begin
expr_stmt|;
name|regfree
argument_list|(
operator|&
name|regexp
argument_list|)
expr_stmt|;
operator|*
name|term
operator|++
operator|=
literal|'/'
expr_stmt|;
return|return
name|term
return|;
block|}
else|else
block|{
name|char
name|errbuf
index|[
literal|1024
index|]
decl_stmt|;
name|regerror
argument_list|(
name|reg_error
argument_list|,
operator|&
name|regexp
argument_list|,
name|errbuf
argument_list|,
literal|1024
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"-L parameter '%s' starting at line %ld: %s"
argument_list|,
name|spec
operator|+
literal|1
argument_list|,
name|begin
operator|+
literal|1
argument_list|,
name|errbuf
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|match_funcname
specifier|static
name|int
name|match_funcname
parameter_list|(
name|xdemitconf_t
modifier|*
name|xecfg
parameter_list|,
specifier|const
name|char
modifier|*
name|bol
parameter_list|,
specifier|const
name|char
modifier|*
name|eol
parameter_list|)
block|{
if|if
condition|(
name|xecfg
condition|)
block|{
name|char
name|buf
index|[
literal|1
index|]
decl_stmt|;
return|return
name|xecfg
operator|->
name|find_func
argument_list|(
name|bol
argument_list|,
name|eol
operator|-
name|bol
argument_list|,
name|buf
argument_list|,
literal|1
argument_list|,
name|xecfg
operator|->
name|find_func_priv
argument_list|)
operator|>=
literal|0
return|;
block|}
if|if
condition|(
name|bol
operator|==
name|eol
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|isalpha
argument_list|(
operator|*
name|bol
argument_list|)
operator|||
operator|*
name|bol
operator|==
literal|'_'
operator|||
operator|*
name|bol
operator|==
literal|'$'
condition|)
return|return
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|find_funcname_matching_regexp
specifier|static
specifier|const
name|char
modifier|*
name|find_funcname_matching_regexp
parameter_list|(
name|xdemitconf_t
modifier|*
name|xecfg
parameter_list|,
specifier|const
name|char
modifier|*
name|start
parameter_list|,
name|regex_t
modifier|*
name|regexp
parameter_list|)
block|{
name|int
name|reg_error
decl_stmt|;
name|regmatch_t
name|match
index|[
literal|1
index|]
decl_stmt|;
while|while
condition|(
literal|1
condition|)
block|{
specifier|const
name|char
modifier|*
name|bol
decl_stmt|,
modifier|*
name|eol
decl_stmt|;
name|reg_error
operator|=
name|regexec
argument_list|(
name|regexp
argument_list|,
name|start
argument_list|,
literal|1
argument_list|,
name|match
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|reg_error
operator|==
name|REG_NOMATCH
condition|)
return|return
name|NULL
return|;
elseif|else
if|if
condition|(
name|reg_error
condition|)
block|{
name|char
name|errbuf
index|[
literal|1024
index|]
decl_stmt|;
name|regerror
argument_list|(
name|reg_error
argument_list|,
name|regexp
argument_list|,
name|errbuf
argument_list|,
literal|1024
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"-L parameter: regexec() failed: %s"
argument_list|,
name|errbuf
argument_list|)
expr_stmt|;
block|}
comment|/* determine extent of line matched */
name|bol
operator|=
name|start
operator|+
name|match
index|[
literal|0
index|]
operator|.
name|rm_so
expr_stmt|;
name|eol
operator|=
name|start
operator|+
name|match
index|[
literal|0
index|]
operator|.
name|rm_eo
expr_stmt|;
while|while
condition|(
name|bol
operator|>
name|start
operator|&&
operator|*
name|bol
operator|!=
literal|'\n'
condition|)
name|bol
operator|--
expr_stmt|;
if|if
condition|(
operator|*
name|bol
operator|==
literal|'\n'
condition|)
name|bol
operator|++
expr_stmt|;
while|while
condition|(
operator|*
name|eol
operator|&&
operator|*
name|eol
operator|!=
literal|'\n'
condition|)
name|eol
operator|++
expr_stmt|;
if|if
condition|(
operator|*
name|eol
operator|==
literal|'\n'
condition|)
name|eol
operator|++
expr_stmt|;
comment|/* is it a funcname line? */
if|if
condition|(
name|match_funcname
argument_list|(
name|xecfg
argument_list|,
operator|(
name|char
operator|*
operator|)
name|bol
argument_list|,
operator|(
name|char
operator|*
operator|)
name|eol
argument_list|)
condition|)
return|return
name|bol
return|;
name|start
operator|=
name|eol
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|parse_range_funcname
specifier|static
specifier|const
name|char
modifier|*
name|parse_range_funcname
parameter_list|(
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|nth_line_fn_t
name|nth_line_cb
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|,
name|long
name|lines
parameter_list|,
name|long
modifier|*
name|begin
parameter_list|,
name|long
modifier|*
name|end
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|char
modifier|*
name|pattern
decl_stmt|;
specifier|const
name|char
modifier|*
name|term
decl_stmt|;
name|struct
name|userdiff_driver
modifier|*
name|drv
decl_stmt|;
name|xdemitconf_t
modifier|*
name|xecfg
init|=
name|NULL
decl_stmt|;
specifier|const
name|char
modifier|*
name|start
decl_stmt|;
specifier|const
name|char
modifier|*
name|p
decl_stmt|;
name|int
name|reg_error
decl_stmt|;
name|regex_t
name|regexp
decl_stmt|;
name|assert
argument_list|(
operator|*
name|arg
operator|==
literal|':'
argument_list|)
expr_stmt|;
name|term
operator|=
name|arg
operator|+
literal|1
expr_stmt|;
while|while
condition|(
operator|*
name|term
operator|&&
operator|*
name|term
operator|!=
literal|':'
condition|)
block|{
if|if
condition|(
operator|*
name|term
operator|==
literal|'\\'
operator|&&
operator|*
operator|(
name|term
operator|+
literal|1
operator|)
condition|)
name|term
operator|++
expr_stmt|;
name|term
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|term
operator|==
name|arg
operator|+
literal|1
condition|)
return|return
name|NULL
return|;
if|if
condition|(
operator|!
name|begin
condition|)
comment|/* skip_range_arg case */
return|return
name|term
return|;
name|pattern
operator|=
name|xstrndup
argument_list|(
name|arg
operator|+
literal|1
argument_list|,
name|term
operator|-
operator|(
name|arg
operator|+
literal|1
operator|)
argument_list|)
expr_stmt|;
name|start
operator|=
name|nth_line_cb
argument_list|(
name|cb_data
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|drv
operator|=
name|userdiff_find_by_path
argument_list|(
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|drv
operator|&&
name|drv
operator|->
name|funcname
operator|.
name|pattern
condition|)
block|{
specifier|const
name|struct
name|userdiff_funcname
modifier|*
name|pe
init|=
operator|&
name|drv
operator|->
name|funcname
decl_stmt|;
name|xecfg
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|xecfg
argument_list|)
argument_list|)
expr_stmt|;
name|xdiff_set_find_func
argument_list|(
name|xecfg
argument_list|,
name|pe
operator|->
name|pattern
argument_list|,
name|pe
operator|->
name|cflags
argument_list|)
expr_stmt|;
block|}
name|reg_error
operator|=
name|regcomp
argument_list|(
operator|&
name|regexp
argument_list|,
name|pattern
argument_list|,
name|REG_NEWLINE
argument_list|)
expr_stmt|;
if|if
condition|(
name|reg_error
condition|)
block|{
name|char
name|errbuf
index|[
literal|1024
index|]
decl_stmt|;
name|regerror
argument_list|(
name|reg_error
argument_list|,
operator|&
name|regexp
argument_list|,
name|errbuf
argument_list|,
literal|1024
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"-L parameter '%s': %s"
argument_list|,
name|pattern
argument_list|,
name|errbuf
argument_list|)
expr_stmt|;
block|}
name|p
operator|=
name|find_funcname_matching_regexp
argument_list|(
name|xecfg
argument_list|,
operator|(
name|char
operator|*
operator|)
name|start
argument_list|,
operator|&
name|regexp
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|p
condition|)
name|die
argument_list|(
literal|"-L parameter '%s': no match"
argument_list|,
name|pattern
argument_list|)
expr_stmt|;
operator|*
name|begin
operator|=
literal|0
expr_stmt|;
while|while
condition|(
name|p
operator|>
name|nth_line_cb
argument_list|(
name|cb_data
argument_list|,
operator|*
name|begin
argument_list|)
condition|)
operator|(
operator|*
name|begin
operator|)
operator|++
expr_stmt|;
if|if
condition|(
operator|*
name|begin
operator|>=
name|lines
condition|)
name|die
argument_list|(
literal|"-L parameter '%s' matches at EOF"
argument_list|,
name|pattern
argument_list|)
expr_stmt|;
operator|*
name|end
operator|=
operator|*
name|begin
operator|+
literal|1
expr_stmt|;
while|while
condition|(
operator|*
name|end
operator|<
name|lines
condition|)
block|{
specifier|const
name|char
modifier|*
name|bol
init|=
name|nth_line_cb
argument_list|(
name|cb_data
argument_list|,
operator|*
name|end
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|eol
init|=
name|nth_line_cb
argument_list|(
name|cb_data
argument_list|,
operator|*
name|end
operator|+
literal|1
argument_list|)
decl_stmt|;
if|if
condition|(
name|match_funcname
argument_list|(
name|xecfg
argument_list|,
name|bol
argument_list|,
name|eol
argument_list|)
condition|)
break|break;
operator|(
operator|*
name|end
operator|)
operator|++
expr_stmt|;
block|}
name|regfree
argument_list|(
operator|&
name|regexp
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|xecfg
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|pattern
argument_list|)
expr_stmt|;
comment|/* compensate for 1-based numbering */
operator|(
operator|*
name|begin
operator|)
operator|++
expr_stmt|;
return|return
name|term
return|;
block|}
end_function
begin_function
DECL|function|parse_range_arg
name|int
name|parse_range_arg
parameter_list|(
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|nth_line_fn_t
name|nth_line_cb
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|,
name|long
name|lines
parameter_list|,
name|long
name|anchor
parameter_list|,
name|long
modifier|*
name|begin
parameter_list|,
name|long
modifier|*
name|end
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
operator|*
name|begin
operator|=
operator|*
name|end
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|anchor
operator|<
literal|1
condition|)
name|anchor
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|anchor
operator|>
name|lines
condition|)
name|anchor
operator|=
name|lines
operator|+
literal|1
expr_stmt|;
if|if
condition|(
operator|*
name|arg
operator|==
literal|':'
condition|)
block|{
name|arg
operator|=
name|parse_range_funcname
argument_list|(
name|arg
argument_list|,
name|nth_line_cb
argument_list|,
name|cb_data
argument_list|,
name|lines
argument_list|,
name|begin
argument_list|,
name|end
argument_list|,
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|arg
operator|||
operator|*
name|arg
condition|)
return|return
operator|-
literal|1
return|;
return|return
literal|0
return|;
block|}
name|arg
operator|=
name|parse_loc
argument_list|(
name|arg
argument_list|,
name|nth_line_cb
argument_list|,
name|cb_data
argument_list|,
name|lines
argument_list|,
operator|-
name|anchor
argument_list|,
name|begin
argument_list|)
expr_stmt|;
if|if
condition|(
operator|*
name|arg
operator|==
literal|','
condition|)
name|arg
operator|=
name|parse_loc
argument_list|(
name|arg
operator|+
literal|1
argument_list|,
name|nth_line_cb
argument_list|,
name|cb_data
argument_list|,
name|lines
argument_list|,
operator|*
name|begin
operator|+
literal|1
argument_list|,
name|end
argument_list|)
expr_stmt|;
if|if
condition|(
operator|*
name|arg
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
operator|*
name|begin
operator|&&
operator|*
name|end
operator|&&
operator|*
name|end
operator|<
operator|*
name|begin
condition|)
block|{
name|long
name|tmp
decl_stmt|;
name|tmp
operator|=
operator|*
name|end
expr_stmt|;
operator|*
name|end
operator|=
operator|*
name|begin
expr_stmt|;
operator|*
name|begin
operator|=
name|tmp
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|skip_range_arg
specifier|const
name|char
modifier|*
name|skip_range_arg
parameter_list|(
specifier|const
name|char
modifier|*
name|arg
parameter_list|)
block|{
if|if
condition|(
operator|*
name|arg
operator|==
literal|':'
condition|)
return|return
name|parse_range_funcname
argument_list|(
name|arg
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|)
return|;
name|arg
operator|=
name|parse_loc
argument_list|(
name|arg
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|,
operator|-
literal|1
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|*
name|arg
operator|==
literal|','
condition|)
name|arg
operator|=
name|parse_loc
argument_list|(
name|arg
operator|+
literal|1
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|,
literal|0
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
return|return
name|arg
return|;
block|}
end_function
end_unit
