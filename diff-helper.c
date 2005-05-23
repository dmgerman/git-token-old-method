begin_unit
begin_comment
comment|/*  * Copyright (C) 2005 Junio C Hamano  */
end_comment
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
file|"diff.h"
end_include
begin_decl_stmt
DECL|variable|detect_rename
specifier|static
name|int
name|detect_rename
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|diff_score_opt
specifier|static
name|int
name|diff_score_opt
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|pickaxe
specifier|static
specifier|const
name|char
modifier|*
name|pickaxe
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|diff_output_style
specifier|static
name|int
name|diff_output_style
init|=
name|DIFF_FORMAT_PATCH
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|line_termination
specifier|static
name|int
name|line_termination
init|=
literal|'\n'
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|inter_name_termination
specifier|static
name|int
name|inter_name_termination
init|=
literal|'\t'
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|parse_diff_raw
specifier|static
name|int
name|parse_diff_raw
parameter_list|(
name|char
modifier|*
name|buf1
parameter_list|,
name|char
modifier|*
name|buf2
parameter_list|,
name|char
modifier|*
name|buf3
parameter_list|)
block|{
name|char
name|old_path
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|unsigned
name|char
name|old_sha1
index|[
literal|20
index|]
decl_stmt|,
name|new_sha1
index|[
literal|20
index|]
decl_stmt|;
name|char
modifier|*
name|ep
decl_stmt|;
name|char
modifier|*
name|cp
init|=
name|buf1
decl_stmt|;
name|int
name|ch
decl_stmt|,
name|old_mode
decl_stmt|,
name|new_mode
decl_stmt|;
name|old_mode
operator|=
name|new_mode
operator|=
literal|0
expr_stmt|;
while|while
condition|(
operator|(
name|ch
operator|=
operator|*
name|cp
operator|)
operator|&&
operator|(
literal|'0'
operator|<=
name|ch
operator|&&
name|ch
operator|<=
literal|'7'
operator|)
condition|)
block|{
name|old_mode
operator|=
operator|(
name|old_mode
operator|<<
literal|3
operator|)
operator||
operator|(
name|ch
operator|-
literal|'0'
operator|)
expr_stmt|;
name|cp
operator|++
expr_stmt|;
block|}
if|if
condition|(
operator|*
name|cp
operator|++
operator|!=
literal|' '
condition|)
return|return
operator|-
literal|1
return|;
while|while
condition|(
operator|(
name|ch
operator|=
operator|*
name|cp
operator|)
operator|&&
operator|(
literal|'0'
operator|<=
name|ch
operator|&&
name|ch
operator|<=
literal|'7'
operator|)
condition|)
block|{
name|new_mode
operator|=
operator|(
name|new_mode
operator|<<
literal|3
operator|)
operator||
operator|(
name|ch
operator|-
literal|'0'
operator|)
expr_stmt|;
name|cp
operator|++
expr_stmt|;
block|}
if|if
condition|(
operator|*
name|cp
operator|++
operator|!=
literal|' '
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|cp
argument_list|,
name|old_sha1
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|cp
operator|+=
literal|40
expr_stmt|;
if|if
condition|(
operator|*
name|cp
operator|++
operator|!=
literal|' '
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|cp
argument_list|,
name|new_sha1
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|cp
operator|+=
literal|40
expr_stmt|;
if|if
condition|(
operator|*
name|cp
operator|++
operator|!=
name|inter_name_termination
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|buf2
condition|)
name|cp
operator|=
name|buf2
expr_stmt|;
name|ep
operator|=
name|strchr
argument_list|(
name|cp
argument_list|,
name|inter_name_termination
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ep
condition|)
return|return
operator|-
literal|1
return|;
operator|*
name|ep
operator|++
operator|=
literal|0
expr_stmt|;
name|strcpy
argument_list|(
name|old_path
argument_list|,
name|cp
argument_list|)
expr_stmt|;
name|diff_guif
argument_list|(
name|old_mode
argument_list|,
name|new_mode
argument_list|,
name|old_sha1
argument_list|,
name|new_sha1
argument_list|,
name|old_path
argument_list|,
name|buf3
condition|?
name|buf3
else|:
name|ep
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|diff_helper_usage
specifier|static
specifier|const
name|char
modifier|*
name|diff_helper_usage
init|=
literal|"git-diff-helper [-z] [-R] [-M] [-C] [-S<string>] paths..."
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|ac
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|av
parameter_list|)
block|{
name|struct
name|strbuf
name|sb1
decl_stmt|,
name|sb2
decl_stmt|,
name|sb3
decl_stmt|;
name|int
name|reverse_diff
init|=
literal|0
decl_stmt|;
name|strbuf_init
argument_list|(
operator|&
name|sb1
argument_list|)
expr_stmt|;
name|strbuf_init
argument_list|(
operator|&
name|sb2
argument_list|)
expr_stmt|;
name|strbuf_init
argument_list|(
operator|&
name|sb3
argument_list|)
expr_stmt|;
while|while
condition|(
literal|1
operator|<
name|ac
operator|&&
name|av
index|[
literal|1
index|]
index|[
literal|0
index|]
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
name|av
index|[
literal|1
index|]
index|[
literal|1
index|]
operator|==
literal|'R'
condition|)
name|reverse_diff
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|av
index|[
literal|1
index|]
index|[
literal|1
index|]
operator|==
literal|'z'
condition|)
name|line_termination
operator|=
name|inter_name_termination
operator|=
literal|0
expr_stmt|;
elseif|else
if|if
condition|(
name|av
index|[
literal|1
index|]
index|[
literal|1
index|]
operator|==
literal|'p'
condition|)
comment|/* hidden from the help */
name|diff_output_style
operator|=
name|DIFF_FORMAT_HUMAN
expr_stmt|;
elseif|else
if|if
condition|(
name|av
index|[
literal|1
index|]
index|[
literal|1
index|]
operator|==
literal|'P'
condition|)
comment|/* hidden from the help */
name|diff_output_style
operator|=
name|DIFF_FORMAT_MACHINE
expr_stmt|;
elseif|else
if|if
condition|(
name|av
index|[
literal|1
index|]
index|[
literal|1
index|]
operator|==
literal|'M'
condition|)
block|{
name|detect_rename
operator|=
name|DIFF_DETECT_RENAME
expr_stmt|;
name|diff_score_opt
operator|=
name|diff_scoreopt_parse
argument_list|(
name|av
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|av
index|[
literal|1
index|]
index|[
literal|1
index|]
operator|==
literal|'C'
condition|)
block|{
name|detect_rename
operator|=
name|DIFF_DETECT_COPY
expr_stmt|;
name|diff_score_opt
operator|=
name|diff_scoreopt_parse
argument_list|(
name|av
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|av
index|[
literal|1
index|]
index|[
literal|1
index|]
operator|==
literal|'S'
condition|)
block|{
name|pickaxe
operator|=
name|av
index|[
literal|1
index|]
operator|+
literal|2
expr_stmt|;
block|}
else|else
name|usage
argument_list|(
name|diff_helper_usage
argument_list|)
expr_stmt|;
name|ac
operator|--
expr_stmt|;
name|av
operator|++
expr_stmt|;
block|}
comment|/* the remaining parameters are paths patterns */
name|diff_setup
argument_list|(
name|reverse_diff
argument_list|)
expr_stmt|;
while|while
condition|(
literal|1
condition|)
block|{
name|int
name|status
decl_stmt|;
name|read_line
argument_list|(
operator|&
name|sb1
argument_list|,
name|stdin
argument_list|,
name|line_termination
argument_list|)
expr_stmt|;
if|if
condition|(
name|sb1
operator|.
name|eof
condition|)
break|break;
switch|switch
condition|(
name|sb1
operator|.
name|buf
index|[
literal|0
index|]
condition|)
block|{
case|case
literal|'U'
case|:
name|diff_unmerge
argument_list|(
name|sb1
operator|.
name|buf
operator|+
literal|2
argument_list|)
expr_stmt|;
continue|continue;
case|case
literal|':'
case|:
break|break;
default|default:
goto|goto
name|unrecognized
goto|;
block|}
if|if
condition|(
operator|!
name|line_termination
condition|)
block|{
name|read_line
argument_list|(
operator|&
name|sb2
argument_list|,
name|stdin
argument_list|,
name|line_termination
argument_list|)
expr_stmt|;
if|if
condition|(
name|sb2
operator|.
name|eof
condition|)
break|break;
name|read_line
argument_list|(
operator|&
name|sb3
argument_list|,
name|stdin
argument_list|,
name|line_termination
argument_list|)
expr_stmt|;
if|if
condition|(
name|sb3
operator|.
name|eof
condition|)
break|break;
name|status
operator|=
name|parse_diff_raw
argument_list|(
name|sb1
operator|.
name|buf
operator|+
literal|1
argument_list|,
name|sb2
operator|.
name|buf
argument_list|,
name|sb3
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
else|else
name|status
operator|=
name|parse_diff_raw
argument_list|(
name|sb1
operator|.
name|buf
operator|+
literal|1
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|status
condition|)
block|{
name|unrecognized
label|:
name|diff_flush
argument_list|(
name|diff_output_style
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|sb1
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
block|}
if|if
condition|(
name|detect_rename
condition|)
name|diffcore_rename
argument_list|(
name|detect_rename
argument_list|,
name|diff_score_opt
argument_list|)
expr_stmt|;
name|diffcore_prune
argument_list|()
expr_stmt|;
if|if
condition|(
name|pickaxe
condition|)
name|diffcore_pickaxe
argument_list|(
name|pickaxe
argument_list|)
expr_stmt|;
if|if
condition|(
name|ac
condition|)
name|diffcore_pathspec
argument_list|(
name|av
operator|+
literal|1
argument_list|)
expr_stmt|;
name|diff_flush
argument_list|(
name|diff_output_style
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
